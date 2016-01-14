/* -*- mode: c; tabs: 4; -*- */
/*=============================================================================
**
**    Vix Technology                   Licensed software
**    (C) 2013                         All rights reserved
**
**=============================================================================
**
**  Project/Product : MBU
**  Filename        : BR_LLSC_2_1.c
**  Author(s)       : ???
**
**  ID              : BR_LLSC_2_1 - NTS0177 v7.2
**
**  Name            : Process Hotlist Block Transit Application
**
**  Data Fields     :
**
**      1.  Actionlist.Type 
**      2.  Actionlist.ActionSeqNo 
**      3.  Actionlist.BlockingReason 
**      4.  TAppControl.ActionSeqNo 
**      5.  TAppControl.BlockingReason 
**      6.  Dynamic.LoadLog 
**      7.  Dynamic.CurrentDateTime 
**      8.  Dynamic.EntryPointID 
**      9.  Dynamic.LineID 
**      10. Dynamic.StopID 
**          Dynamic.Zone
**
**  Pre-Conditions  :
**
**      1.  An actionlist entry exists for the card UID. 
**      2.  The actionlist type is equal to TAppUpdate/Block. 
**      3.  The actionlist action sequence number is between 1 and 15. 
**      4.  If the transit application action sequence number is equal to 15: 
**          a.  The actionlist action sequence number is equal to 1. 
**      5.  Else: 
**          a.  The actionlist action sequence number is equal to the transit
**              application action sequence number + 1
**
**  Description     :
**
**      1.  Perform a TAppUpdate/Block transaction: 
**          a.  Set the transit application blocking reason to the blocking
**              reason specified in the actionlist. 
**      2.  Modify usage log
**          a.  Definition: 
**              i.      TxUsageType = (10) 
**              ii.     Determine uselog ControlBitmap though look up of the
**                      TxUsageType 
**              iii.    Set TxDateTime as current Date time
**              iv.     TxSeqNo = NextTxSeqNo 
**              v.      ServiceProvider = as returned from the TAppUpdate/Block 
**              vi.     Location.EntryPoint = EntryPointID
**              vii.    Location.Route = LineID
**              viii.   Location.Stop = Stopid
**          b.  Usage
**              [i].    Zone = Dynamic.Zone
**
**  Post-Conditions :
**
**      1.  The transit application is blocked.
**
**  Member(s)       :
**      BR_LLSC_2_1             [public]    business rule
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
**    1.02  29.10.14    ANT   Modify   Changed myki_br_GetActionlist() signature.
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
**  BR_LLSC_2_1
**
**  Description     :
**      Implements business rule BR_LLSC_2_1.
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

RuleResult_e BR_LLSC_2_1( MYKI_BR_ContextData_t *pData )
{
    MYKI_TAControl_t    *pMYKI_TAControl;
    CT_CardInfo_t       *pCardInfo;
    int                  errcode;

    CsDbg( BRLL_RULE, "BR_LLSC_2_1 : Start (Process Hotlist Block Transit Application)" );

    if ( ! pData )
    {
        CsErrx( "BR_LLSC_2_1 : Invalid argument(s)" );
        return RULE_RESULT_ERROR;
    }

    if ( MYKI_CS_TAControlGet( &pMYKI_TAControl ) < 0 )
    {
        CsErrx( "BR_LLSC_2_1 : MYKI_CS_TAControlGet() failed" );
        return RULE_RESULT_ERROR;
    }

    if ( MYKI_CS_GetCardInfo( &pCardInfo ) < 0 )
    {
        CsErrx( "BR_LLSC_2_1 : MYKI_CS_GetCardInfo() failed" );
        return RULE_RESULT_ERROR;
    }
    
    /*  PRE-CONDITIONS */
    {
        U8_t    searchActionSequenceNumber  = ( pMYKI_TAControl->ActionSeqNo == 15 ? 1 : ( pMYKI_TAControl->ActionSeqNo + 1 ) );

        // populate the ActionList structure
        if ( pData->ActionList.type == ACTION_TAPP_UPDATE_BLOCK )
        {
            if ( pData->ActionList.actionlist.tAppUpdate.actionSequenceNo == searchActionSequenceNumber )
            {
                CsDbg( BRLL_RULE, "BR_LLSC_2_1 : found desired actionlist in cache" );
            }
            else
            {
                pData->ActionList.type  = ACTION_NULL;
            }
        }

        if ( pData->ActionList.type != ACTION_TAPP_UPDATE_BLOCK )
        {
            if ( ( errcode = myki_br_GetActionlist( pCardInfo->SerialNumber, ACTION_TAPP_UPDATE_BLOCK, searchActionSequenceNumber, 0 /*DONTCARE*/, pData ) ) < 0 )
            {
                CsDbg( BRLL_RULE, "BR_LLSC_2_1 : myki_br_GetActionlist returns error %d", errcode);
                return RULE_RESULT_ERROR;
            }
        }

        /*  1.  An actionlist entry exists for the card UID.
            2.  The actionlist type is equal to TAppUpdate/Block. */
        if ( pData->ActionList.type != ACTION_TAPP_UPDATE_BLOCK )
        {
            CsDbg( BRLL_RULE, "BR_LLSC_2_1 : Bypassed - ActionList.type is not ACTION_TAPP_UPDATE_BLOCK." );
            pData->ReturnedData.bypassCode  = BYPASS_CODE( 2, 1, 2, 0 );
            return RULE_RESULT_BYPASSED;
        }

        /*  3.  The actionlist action sequence number is between 1 and 15. */
        if ( pData->ActionList.actionlist.tAppUpdate.actionSequenceNo < 1 || pData->ActionList.actionlist.tAppUpdate.actionSequenceNo > 15 )
        {
            CsDbg( BRLL_RULE, "BR_LLSC_2_1 : Bypassed - ActionList.actionSeqNo (%d) is out of range.", pData->ActionList.actionlist.tAppUpdate.actionSequenceNo );
            pData->ReturnedData.bypassCode  = BYPASS_CODE( 2, 1, 3, 0 );
            return RULE_RESULT_BYPASSED;
        }

        CsDbg( BRLL_RULE, "BR_LLSC_2_1 : pMYKI_TAControl->ActionSeqNo = %d, pData->ActionList.actionSeqNo = %d", pMYKI_TAControl->ActionSeqNo, pData->ActionList.actionlist.tAppUpdate.actionSequenceNo );

        /*  4.  If the transit application action sequence number is equal to 15:
                a.  The actionlist action sequence number is equal to 1.
            5.  Else:
                a.  The actionlist action sequence number is equal to the transit
                    application action sequence number + 1. */
        if ( pMYKI_TAControl->ActionSeqNo == 15 )
        {
            if ( pData->ActionList.actionlist.tAppUpdate.actionSequenceNo != 1 )
            {
                CsDbg( BRLL_RULE, "BR_LLSC_2_1 : Bypassed - pMYKI_TAControl->ActionSeqNo = 15, ActionList.actionSeqNo != 1");
                pData->ReturnedData.bypassCode  = BYPASS_CODE( 2, 1, 4, 0 );
                return RULE_RESULT_BYPASSED;
            }
        }
        else
        if ( pData->ActionList.actionlist.tAppUpdate.actionSequenceNo != ( pMYKI_TAControl->ActionSeqNo + 1 ) )
        {
            CsDbg( BRLL_RULE, "BR_LLSC_2_1 : Bypassed - pMYKI_TAControl->ActionSeqNo + 1 != ActionList.actionSeqNo");
            pData->ReturnedData.bypassCode  = BYPASS_CODE( 2, 1, 5, 0 );
            return RULE_RESULT_BYPASSED;
        }
    }

    /*  PROCESSING */
    {
        /*  1.	Perform a TAppUpdate/Block transaction:
                a.	Set the transit application blocking reason to the blocking reason
                    specified in the actionlist. */
        if ( myki_br_ldt_Actionlist( pData ) < 0 )
        {
            CsErrx( "BR_LLSC_2_1 : myki_br_ldt_Actionlist() failed" );
            return RULE_RESULT_ERROR;
        }

        /*  NOTE:   TAppControl.ActionSeqNo is updated by LDT */

        /*  2.  Modify usage log
                a.  Definition: */
        {
            /*      i.  TxUsageType = (10) */
            pData->InternalData.UsageLogData.transactionType    = MYKI_BR_TRANSACTION_TYPE_APPLICATION_BLOCK;
            /*      ii. Determine uselog ControlBitmap though look up of the TxUsageType (Done by framework)
                    iii.Set TxDateTime as current Date time (Done by framework)
                    iv. TxSeqNo = NextTxSeqNo (?)
                    v.  ServiceProvider =  as returned from the TAppUpdate/Block (Done by framework)
                    vi. Location.EntryPoint = EntryPointID (Done by framework)
                    vii.Location.Route = LineID (Done by framework)
                    viii.Location.Stop = Stopid (Done by framework) */
        }

        /*      b.  Usage */
        {
            /*      Zone = Dynamic.Zone */
            pData->InternalData.UsageLogData.isZoneSet          = TRUE;
            pData->InternalData.UsageLogData.zone               = pData->DynamicData.currentZone;
        }

        /*  NOTE:   TAppUsageLog entry is added by application framework. */
        pData->InternalData.IsUsageLogUpdated                   = TRUE;
        CsDbg( BRLL_RULE, "BR_LLSC_2_1 : Updated TAppUsageLog" );
    }

    /*  POST-CONDITIONS */
    {
        /*  1.  The transit application is blocked. */
        pData->ReturnedData.rejectReason        = MYKI_BR_REJECT_REASON_APPLICATION_BLOCKED;
        pData->ReturnedData.blockingReason      = (int)pData->ActionList.actionlist.tAppUpdate.blockingReason;
        pData->ReturnedData.actionApplied       = TRUE;
    }

    CsDbg( BRLL_RULE, "BR_LLSC_2_1 : Executed" );

    return RULE_RESULT_EXECUTED;
}

