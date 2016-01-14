/* -*- mode: c; tabs: 4; -*- */
/*=============================================================================
**
**    Vix Technology                   Licensed software
**    (C) 2013                         All rights reserved
**
**=============================================================================
**
**  Project/Product : MBU
**  Filename        : BR_LLSC_2_14.c
**  Author(s)       : ???
**
**  ID              : BR_LLSC_2_14 - NTS0177 v7.2
**
**  Name            : Process Hotlist By Sam Id
**
**  Data Fields     :
**
**      1.  Sam Hotlist Data.SamId 
**      2.  Sam Hotlist Data effective date 
**      3.  TAppTPurseControl.LastChangeSamId 
**      4.  TAppTPurseControl.LastChangeDate 
**      5.  Usagelog 
**      6.  Dynamic.CurrentDateTime
**
**  Pre-Conditions  :
**
**      1.  An actionlist entry exists where LastChangeSamId is equal to the
**          hot listed Sam ID and LastChangeDate is greater than or equal to
**          hotlisting effective date. 
**
**  Description     :
**
**      1.  Perform TPurseUsage/Transit with theamount of 0. (This is done
**          to force the reset last used SAM ESN id) 
**      2.  Perform a TAppUpdate/ Block transaction settingreason code to
**          2 – Hot listed SAM 
**      3.  Modify usage log
**          a.  Definition: 
**              i.  If LoadTxType is not set then set Block Application (10)
**                  Else set LoadTxType to Multiple Actionlists (21) 
**              ii. Determine uselog ControlBitmap though look up of the
**                  TxUsageType 
**              iii.ServiceProvider = Static.ServiceProvider 
**              iv. Location = Current location 
**              v.  Set TxDateTime as current Date time
**          b.  Value: 
**              i.  No change 
**          c.  Product: 
**              i.  No change
**
**  Post-Conditions :
**
**      1.  The transit application is blocked based upon SAM esn ID
**
**  Member(s)       :
**      BR_LLSC_2_14            [public]    business rule
**
**  Information     :
**   Compiler(s)    : C
**   Target(s)      : Independent
**
**  Subversion      :
**      $Id: $
**      $HeadURL: $
**
**  History         :
**   Vers.  Date        Aut.  Type     Description
**   -----  ----------  ----  -------  ----------------------------------------
**    1.00  ??.??.??    ???   Create
**    1.01  04.12.13    ANT   Modify   No code change. Updated comments.
**    1.02  23.12.14    ANT   Modify   Rectified TAppUpdate/Block.
**    1.03  12.11.15    ANT   Modify   NGBU-838: Added returning blocking reason.
**
**===========================================================================*/

#include <cs.h>                 // For debug logging
#include <myki_cardservices.h>  // Myki Card Services
#include <LDT.h>                // Logical Device Transaction
#include <myki_actionlist.h>    // Actionlist

#include "myki_br_rules.h"      // API for all BR_xxx modules
#include "BR_Common.h"          // Common utilities, macros, etc

/*==========================================================================*
**
**  BR_LLSC_2_14
**
**  Description     :
**      Implements business rule BR_LLSC_2_14.
**
**  Parameters      :
**      pData               [I/O]   BR context data
**
**  Returns         :
**      RULE_RESULT_EXECUTED
**      RULE_RESULT_BYPASSED
**      RULE_RESULT_ERROR
**
**  Notes           :
**
**==========================================================================*/

RuleResult_e BR_LLSC_2_14( MYKI_BR_ContextData_t *pData )
{
    MYKI_TAPurseBalance_t      *pMYKI_TAPurseBalance    = NULL;
    MYKI_TAPurseControl_t      *pMYKI_TAPurseControl    = NULL;

    CsDbg( BRLL_RULE, "BR_LLSC_2_14 : Start (Process Hotlist by Sam Id)" );

    if ( ! pData )
    {
        CsErrx( "BR_LLSC_2_14 : Invalid argument(s)" );
        return RULE_RESULT_ERROR;
    }

    // Must MYKI_CS_TAPurseBalanceGet before MYKI_CS_TAPurseControlGet 
    if ( MYKI_CS_TAPurseBalanceGet( &pMYKI_TAPurseBalance ) < 0 )
    {
        CsErrx( "BR_LLSC_2_14 : MYKI_CS_TAPurseBalanceGet() failed" );
        return RULE_RESULT_ERROR;
    }

    if ( MYKI_CS_TAPurseControlGet( &pMYKI_TAPurseControl ) < 0 )
    {
        CsErrx( "BR_LLSC_2_14 : MYKI_CS_TAPurseControlGet() failed" );
        return RULE_RESULT_ERROR;
    }

    //----------------------------------------------------------------------------
    //  Pre-Conditions
    //  1.  An actionlist entry exists where LastChangeSamId(3) is equal to the hot
    //     listed Sam ID(1) and LastChangeDate(4) is greater than or equal to
    //     hotlisting effective date(2).
    //
    if ( MYKI_ACTIONLIST_isDeviceHotlisted( pMYKI_TAPurseControl->LastChangeSamId, pMYKI_TAPurseControl->LastChangeDate ) <= 0 )
    {
        CsDbg( BRLL_RULE, "BR_LLSC_2_14 : Bypass - No device hotlist entry found for %08X/%d",
                pMYKI_TAPurseControl->LastChangeSamId, pMYKI_TAPurseControl->LastChangeDate );
        pData->ReturnedData.bypassCode = BYPASS_CODE( 2, 14, 1, 0 );
        return RULE_RESULT_BYPASSED;
    }

    //
    //  Indicate to Test Harness that all Pre-Conditions have been satisfied
    //
    pData->ReturnedData.bypassCode = BYPASS_CODE( 2, 14, 255, 0 );

    //----------------------------------------------------------------------------
    //  Description
    //  1.  Perform TPurseUsage/Transit with the amount of 0.
    //      (This is done to force the reset last used SAM ESN id)
    //
    if ( myki_br_ldt_PurseUsage_Transit( pData, 0 ) < 0 )
    {
        CsErrx( "BR_LLSC_2_14 : myki_br_ldt_PurseUsage_Transit() failed" );
        return RULE_RESULT_ERROR;
    }

    //
    //  2.  Perform a TAppUpdate/ Block  transaction setting reason code to 2 - Hot listed SAM
    //
    if ( myki_br_ldt_AppUpdate_Block( pData, TAPP_CONTROL_BLOCKING_REASON_HOTLISTED_SAM ) < 0 )
    {
        CsErrx( "BR_LLSC_2_14 : myki_br_ldt_AppUpdate_Block() failed" );
        return RULE_RESULT_ERROR;
    }

    //
    //  3.  Modify usage log(5)
    //      a.  Definition:
    {
        //      i.     If LoadTxType is not set
        if ( pData->InternalData.UsageLogData.transactionType == MYKI_BR_TRANSACTION_TYPE_NONE )
        {
            //         then set Block Application (10)
            pData->InternalData.UsageLogData.transactionType    = MYKI_BR_TRANSACTION_TYPE_APPLICATION_BLOCK;
        }
        else
        {
            //         Else set  LoadTxType to Multiple Actionlists (21)
            pData->InternalData.UsageLogData.transactionType    = MYKI_BR_TRANSACTION_TYPE_MULTIPLE_ACTION_LIST;
        }
        //      ii.    Determine uselog ControlBitmap though look up of the TxUsageType (Done by framework)
        //      iii.   ServiceProvider  = Static.ServiceProvider (Done by framework)
        //      iv.    Location = Current location (Done by framework)
        //      v.     Set TxDateTime as current Date time (Done by framework)
    }

    //      b.  Value:
    //         i.  No change
    //      c.  Product:
    //         i.  No change
    //

    /*  NOTE:   TAppUsageLog entry is added by application framework. */
    pData->InternalData.IsUsageLogUpdated           = TRUE;
    CsDbg( BRLL_RULE, "BR_LLSC_2_14 : Updated TAppUsageLog" );

    /*  POST-CONDITIONS */
    {
        /*  1.  The transit application is blocked based upon SAM esn ID */
        pData->ReturnedData.rejectReason        = MYKI_BR_REJECT_REASON_APPLICATION_BLOCKED;
        pData->ReturnedData.blockingReason      = (int)TAPP_CONTROL_BLOCKING_REASON_HOTLISTED_SAM;
        pData->ReturnedData.actionApplied       = TRUE;
    }

    CsDbg( BRLL_RULE, "BR_LLSC_2_14 : Executed" );
    return RULE_RESULT_EXECUTED;
}

