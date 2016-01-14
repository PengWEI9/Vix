/* -*- mode: c; tabs: 4; -*- */
/*=============================================================================
**
**    Vix Technology                   Licensed software
**    (C) 2013                         All rights reserved
**
**=============================================================================
**
**  Project/Product : MBU
**  Filename        : BR_LLSC_2_11.c
**  Author(s)       : ???
**
**  ID              : BR_LLSC_2_11 - NTS0177 v7.2
**
**  Name            : Process Hotlist UnBlock Transit Application
**
**  Data Fields     :
**
**      1.  Actionlist.Type
**      2.  ActionList.actionlist.tAppUpdate.actionSequenceNo
**      3.  Actionlist.BlockingReason
**      4.  TAppControl.ActionSeqNo
**      5.  UsageLog
**      6.  Dynamic.CurrentDateTime
**
**  Pre-Conditions  :
**
**      1.  An actionlist entry exists for the card UID.
**      2.  The actionlist type is equal to TAppUpdate/Unblock.
**      3.  The actionlist action sequence number is between 1 and 15.
**      4.  If the transit application action sequence number is equal to 15:
**          a.  The actionlist action sequence number is equal to 1.
**      5.  Else:
**          a.  The actionlist action sequence number is equal to the transit
**              application action sequence number + 1.
**
**  Description     :
**
**      1.  Perform a TAppUpdate/Unblock transaction
**      2.  Modify usage log
**          a.  Definition:
**              i.  UsageLogTxType =  Unblock 12
**              ii. Determine uselog ControlBitmap though look up of the TxUsageType
**              iii.
**              iv. TxSeqNo = as returned from transit unblocking
**              v.  ServiceProvider = ActionList.ServiceProvider
**              vi. Set TxDateTime as current Date time
**
**  Post-Conditions :
**
**      1.  The transit application is unblocked.
**
**  Member(s)       :
**      BR_LLSC_2_11            [public]    business rule
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
**    1.01  13.11.13    ANT   Add      Added generating TAppUsageLog record
**    1.02  04.12.13    ANT   Modify   No code change. Updated comments.
**    1.03  29.10.14    ANT   Modify   Changed myki_br_GetActionlist() signature.
**    1.04  19.05.15    ANT   Modify   MBU-1137: Rectified incorrect setting of
**                                     UsageLog.TransactionType
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
**  BR_LLSC_2_11
**
**  Description     :
**      Implements business rule BR_LLSC_2_11.
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

RuleResult_e BR_LLSC_2_11( MYKI_BR_ContextData_t *pData )
{
    MYKI_TAControl_t            *pMYKI_TAControl;  
    CT_CardInfo_t               *pCardInfo;
    int                          errcode;
    
    CsDbg( BRLL_RULE, "BR_LLSC_2_11 : Start (Process Hotlist Unblock Transit Application)" );

    if ( ! pData )
    {
        CsErrx( "BR_LLSC_2_11 : Invalid argument(s)" );
        return RULE_RESULT_ERROR;
    }

    if ( MYKI_CS_TAControlGet( &pMYKI_TAControl ) < 0 )
    {
        CsErrx( "BR_LLSC_2_11 : MYKI_CS_TAControlGet() failed" );
        return RULE_RESULT_ERROR;
    }

    if ( MYKI_CS_GetCardInfo( &pCardInfo ) < 0 )
    {
        CsErrx( "BR_LLSC_2_11 : MYKI_CS_GetCardInfo() failed" );
        return RULE_RESULT_ERROR;
    }
    
    /*  PRE-CONDITIONS */
    {
        U8_t    searchActionSequenceNumber  = ( pMYKI_TAControl->ActionSeqNo == 15 ? 1 : ( pMYKI_TAControl->ActionSeqNo + 1 ) );

        // populate the ActionList structure
        if ( pData->ActionList.type == ACTION_TAPP_UPDATE_UNBLOCK )
        {
            if ( pData->ActionList.actionlist.tAppUpdate.actionSequenceNo == searchActionSequenceNumber )
            {
                CsDbg( BRLL_RULE, "BR_LLSC_2_11 : found desired actionlist in cache" );
            }
            else
            {
                pData->ActionList.type  = ACTION_NULL;
            }
        }

        if ( pData->ActionList.type != ACTION_TAPP_UPDATE_UNBLOCK )
        {
            if ( ( errcode = myki_br_GetActionlist( pCardInfo->SerialNumber, ACTION_TAPP_UPDATE_UNBLOCK, searchActionSequenceNumber, 0 /*DONTCARE*/, pData ) ) < 0 )
            {
                CsDbg( BRLL_RULE, "BR_LLSC_2_11 : myki_br_GetActionlist returns error %d", errcode);
                return RULE_RESULT_ERROR;
            }
        }

        /*  1.  An actionlist entry exists for the card UID.
            2.  The actionlist type is equal to TAppUpdate/Unblock. */
        if ( pData->ActionList.type != ACTION_TAPP_UPDATE_UNBLOCK )
        {
            CsDbg( BRLL_RULE, "BR_LLSC_2_11 : Bypassed - ActionList.type is not ACTION_TAPP_UPDATE_UNBLOCK." );
            pData->ReturnedData.bypassCode  = BYPASS_CODE( 2, 11, 2, 0 );
            return RULE_RESULT_BYPASSED;
        }

        /*  3.  The actionlist action sequence number is between 1 and 15. */
        if ( pData->ActionList.actionlist.tAppUpdate.actionSequenceNo < 1 || pData->ActionList.actionlist.tAppUpdate.actionSequenceNo > 15 )
        {
            CsDbg( BRLL_RULE, "BR_LLSC_2_11 : Bypassed - ActionList.actionlist.tAppUpdate.actionSequenceNo (%d) is out of range.",
                pData->ActionList.actionlist.tAppUpdate.actionSequenceNo );
            pData->ReturnedData.bypassCode  = BYPASS_CODE( 2, 11, 3, 0 );
            return RULE_RESULT_BYPASSED;
        }

        /*  4.  If the transit application action sequence number is equal to 15:
                a.  The actionlist action sequence number is equal to 1.
            5.  Else:
                a.  The actionlist action sequence number is equal to the transit
                    application action sequence number + 1. */
        CsDbg( BRLL_RULE, "BR_LLSC_2_11 : pMYKI_TAControl->ActionSeqNo = %d, pData->ActionList.actionlist.tAppUpdate.actionSequenceNo = %d",
            pMYKI_TAControl->ActionSeqNo, pData->ActionList.actionlist.tAppUpdate.actionSequenceNo );
        if ( pMYKI_TAControl->ActionSeqNo == 15 )
        {
            if ( pData->ActionList.actionlist.tAppUpdate.actionSequenceNo != 1 )
            {
                CsDbg( BRLL_RULE, "BR_LLSC_2_11 : Bypassed - pMYKI_TAControl->ActionSeqNo = 15, ActionList.actionlist.tAppUpdate.actionSequenceNo != 1");
                pData->ReturnedData.bypassCode  = BYPASS_CODE( 2, 11, 4, 0 );
                return RULE_RESULT_BYPASSED;
            }
        }
        else 
        if ( pData->ActionList.actionlist.tAppUpdate.actionSequenceNo != ( pMYKI_TAControl->ActionSeqNo + 1 ) )
        {
            CsDbg( BRLL_RULE, "BR_LLSC_2_11 : Bypassed - pMYKI_TAControl->ActionSeqNo + 1 != ActionList.actionlist.tAppUpdate.actionSequenceNo" );
            pData->ReturnedData.bypassCode  = BYPASS_CODE( 2, 11, 5, 0 );
            return RULE_RESULT_BYPASSED;
        }
    }

    /*  PROCESSING */
    {
        /*  1.  Perform a TAppUpdate/Unblock transaction */
        if ( myki_br_ldt_Actionlist( pData ) < 0 )
        {
            CsErrx( "BR_LLSC_2_11 : myki_br_ldt_Actionlist() failed" );
            return RULE_RESULT_ERROR;
        }

        /*  NOTE:   TAppControl.ActionSeqNo is updated by LDT */

        /*  2.  Modify usage log
                a.  Definition: */
        {
            /*  i.  UsageLogTxType = Unblock 12 */
            pData->InternalData.UsageLogData.transactionType        = MYKI_BR_TRANSACTION_TYPE_APPLICATION_UNBLOCK;
            /*  ii. Determine uselog ControlBitmap though look up of the TxUsageType (Done by framework)
                iii. */
            /*  iv. TxSeqNo = as returned from transit unblocking (Not in TAppUsageLog?) */
            /*  v.  ServiceProvider = ActionList.ServiceProvider */
            pData->InternalData.UsageLogData.providerId             = pData->ActionList.actionlist.tAppUpdate.serviceProviderId;
            /*  vi. Set TxDateTime as current Date time (Done by framework) */
        }

        /*  NOTE:   TAppUsageLog entry is added by application framework. */
        pData->InternalData.IsUsageLogUpdated                   = TRUE;
        CsDbg( BRLL_RULE, "BR_LLSC_2_11 : Updated TAppUsageLog" );
    }

    /*  POST-CONDITIONS */
    {
        /*  1.  The transit application is unblocked. */
        pData->ReturnedData.actionApplied       = TRUE;
    }

    CsDbg( BRLL_RULE, "BR_LLSC_2_11 : Executed" );
    return RULE_RESULT_EXECUTED;
}
