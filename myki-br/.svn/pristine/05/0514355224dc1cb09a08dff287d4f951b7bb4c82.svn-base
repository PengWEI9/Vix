/* -*- mode: c; tabs: 4; -*- */
/*=============================================================================
**
**    Vix Technology                   Licensed software
**    (C) 2013                         All rights reserved
**
**=============================================================================
**
**  Project/Product : MBU
**  Filename        : BR_LLSC_2_13.c
**  Author(s)       : ???
**
**  ID              : BR_LLSC_2_13 - NTS0177 v7.2
**
**  Name            : Process Actionlist T-Purse Debit
**
**  Data Fields     :
**
**      1.  Tariff.TPurseMaximumBalance
**      2.  Actionlist.Type
**      3.  Actionlist.ActionSeqNo
**      4.  Actionlist.Value
**      5.  TAppControl.Directory.Status
**      6.  TAppTPurseControl.ActionSeqNo
**      7.  TAppTPurseBalance.Balance
**      8.  Dynamic.CurrentDateTime
**      9.  ActionList.ServiceProviderID
**      10. Dynamic.EntryPointID
**      11. Dynamic.LineID
**      12. Dynamic.StopID
**      13. TAppTPurseControl.NextTxSeqNo
**      14. Dynamic.LoadLog
**      15. Dynamic.LoadLogTxValue
**
**  Pre-Conditions  :
**
**      1.  An actionlist entry exists for the card UID.
**      2.  The actionlist type is equal to TPurseLoad/Debit.
**      3.  The actionlist action sequence number is between 1 and 15.
**      4.  If the t-Purse action sequence number is equal to 15:
**          a.  The actionlist action sequence number is equal to 1.
**      5.  Else:
**          a.  The actionlist action sequence number is equal to the
**              T-Purse action sequence number + 1.
**      6.  The t-Purse status is not equal to Blocked. If the T-Purse
**          is blocked, generate a FailureResponse of type TPurseBlocked.
**
**  Description     :
**
**      1.  Perform a TPurseLoad/Debit transaction.
**      2.  Modify load log
**          a.  Definition:
**              i.  TAppLoadLog.ControlBitmap: Value
**              ii. If LoadTxType is not set  or is set to disable
**                  autoload (25) then
**                  (1) Set to LoadTxType to TPurse Reverse ( 33)
**                  (2) Else set to Multiple Actionlist (21)
**              iii.Determine LoadLog ControlBitmap th[r]ough look up of the
**                  TxLoadType
**              iv. Set TxDateTime as current Date time
**              v.  TxSeqNo = NextTxSeqNo
**              vi. ServiceProvider =  as returned from the TPurseLoad.None
**              vii.Set location object to the current device location
**                  (i.e Entry Point, Route, Stop ID)
**          b.  Value:
**              i.  Dyanamic.LoadLogTxValue = Dynamic.LoadLogTxValue Less
**                  Action List value
**              ii. If Dynamic.logLogTxValue less than zero
**                  (1) If LoadTxType is not Type T-Purse Reverse (33) then
**                      (a) Set LoadLog.TxValue to zero
**                      (b) Else LoadLog.TxValue = Absolute Value of
**                          (Dynamic.LoadLogTxValue)
**                  (2) Else Set LoadLog.TxValue  to Dynamic.LoadLogTxValue
**              iii.NewTPurseBalance =  t-Purse balance
**              iv. PaymentMethod = Ad-hoc autoload (6)
**
**  Post-Conditions :
**
**      1.  The T-Purse balance is decreased by the amount specified in the actionlist.
**
**  Member(s)       :
**      BR_LLSC_2_13            [public]    business rule
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
**    1.01  15.11.13    ANT   Modify   Removed updating
**                                     TAppPurseControl.ActionSeqNo as done
**                                     by LDT
**    1.02  03.12.13    ANT   Add      Generating FailureResponse messages
**    1.03  04.12.13    ANT   Modify   Rectified TAppLoadLog generation
**    1.04  29.10.14    ANT   Modify   Changed myki_br_GetActionlist() signature.
**    1.05  23.04.15    ANT   Modify   MBU-1352: Set ReturnedData.topupAmount
**
**===========================================================================*/

#include <cs.h>                 // For debug logging
#include <myki_cardservices.h>  // Myki Card Services
#include <LDT.h>                // Logical Device Transaction

#include "myki_br_rules.h"      // API for all BR_xxx modules
#include "BR_Common.h"          // Common utilities, macros, etc

/*==========================================================================*
**
**  BR_LLSC_2_13
**
**  Description     :
**      Implements business rule BR_LLSC_2_13.
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

RuleResult_e BR_LLSC_2_13( MYKI_BR_ContextData_t *pData )
{
    MYKI_TAControl_t       *pMYKI_TAControl         = NULL;
    MYKI_TAPurseBalance_t  *pMYKI_TAPurseBalance    = NULL;
    MYKI_TAPurseControl_t  *pMYKI_TAPurseControl    = NULL;
    MYKI_Directory_t       *pDirectory              = NULL;
    CT_CardInfo_t          *pCardInfo               = NULL;
    int                     errcode                 = 0;
    TAppLoadLog_t          *pAppLoadLog             = NULL;

    CsDbg( BRLL_RULE, "BR_LLSC_2_13 : Start (Process Actionlist T-Purse Reverse Value)" );

    if ( ! pData )
    {
        CsErrx( "BR_LLSC_2_13 : Invalid argument(s)" );
        return RULE_RESULT_ERROR;
    }

    if ( MYKI_CS_TAControlGet( &pMYKI_TAControl ) < 0 )
    {
        CsErrx( "BR_LLSC_2_13 : MYKI_CS_TAControlGet() failed" );
        return RULE_RESULT_ERROR;
    }

    if ( MYKI_CS_TAPurseBalanceGet( &pMYKI_TAPurseBalance ) < 0 )
    {
        CsErrx( "BR_LLSC_2_13 : MYKI_CS_TAPurseBalanceGet() failed" );
        return RULE_RESULT_ERROR;
    }

    if ( MYKI_CS_TAPurseControlGet( &pMYKI_TAPurseControl ) < 0 )
    {
        CsErrx( "BR_LLSC_2_13 : MYKI_CS_TAPurseControlGet() failed" );
        return RULE_RESULT_ERROR;
    }

    if ( MYKI_CS_GetCardInfo( &pCardInfo ) < 0 )
    {
        CsErrx( "BR_LLSC_2_13 : MYKI_CS_GetCardInfo() failed" );
        return RULE_RESULT_ERROR;
    }

    /* PRE-CONDITIONS */
    {
        U8_t    searchActionSequenceNumber  = ( pMYKI_TAPurseControl->ActionSeqNo == 15 ? 1 : ( pMYKI_TAPurseControl->ActionSeqNo + 1 ) );

        //  populate the ActionList structure
        if ( pData->ActionList.type == ACTION_TPURSE_LOAD_DEBIT )
        {
            if ( pData->ActionList.actionlist.tPurseLoad.actionSequenceNo == searchActionSequenceNumber )
            {
                CsDbg( BRLL_RULE, "BR_LLSC_2_13 : found desired actionlist in cache" );
            }
            else
            {
                pData->ActionList.type  = ACTION_NULL;
            }
        }

        if ( pData->ActionList.type != ACTION_TPURSE_LOAD_DEBIT )
        {
            if ( ( errcode = myki_br_GetActionlist( pCardInfo->SerialNumber, ACTION_TPURSE_LOAD_DEBIT, searchActionSequenceNumber, 0 /*DONTCARE*/, pData ) ) < 0 )
            {
                CsDbg( BRLL_RULE, "BR_LLSC_2_13 : myki_br_GetActionlist returns error %d", errcode);
                return RULE_RESULT_ERROR;
            }
        }

        /*  1.  An actionlist entry exists for the card UID.
            2.  The actionlist type is equal to TPurseLoad/Debit. */
        if ( pData->ActionList.type != ACTION_TPURSE_LOAD_DEBIT )
        {
            CsDbg( BRLL_RULE, "BR_LLSC_2_13 : Bypassed - ActionList.type is not ACTION_TPURSE_LOAD_DEBIT." );
            pData->ReturnedData.bypassCode  = BYPASS_CODE( 2, 13, 2, 0 );
            return RULE_RESULT_BYPASSED;
        }

        /*  3.  The actionlist action sequence number3 is between 1 and 15. */
        if ( pData->ActionList.actionlist.tPurseLoad.actionSequenceNo < 1 || pData->ActionList.actionlist.tPurseLoad.actionSequenceNo > 15 )
        {
            CsDbg( BRLL_RULE, "BR_LLSC_2_13 : Bypassed - ActionList.actionSeqNo (%d) is out of range.",
                pData->ActionList.actionlist.tPurseLoad.actionSequenceNo );
            pData->ReturnedData.bypassCode  = BYPASS_CODE( 2, 13, 3, 0 );
            return RULE_RESULT_BYPASSED;
        }

        CsDbg( BRLL_RULE, "BR_LLSC_2_13 : pMYKI_TAPurseControl->ActionSeqNo = %d, pData->ActionList.actionSeqNo = %d",
            pMYKI_TAPurseControl->ActionSeqNo, pData->ActionList.actionlist.tPurseLoad.actionSequenceNo );

        /*  4.  If the t-Purse action sequence number is equal to 15:
                a.  The actionlist action sequence number is equal to 1.
            5.  Else:
                a.  The actionlist action sequence number is equal to the T-Purse
                    action sequence number + 1. */
        if ( pMYKI_TAPurseControl->ActionSeqNo == 15 )
        {
            if ( pData->ActionList.actionlist.tPurseLoad.actionSequenceNo != 1 )
            {
                CsDbg( BRLL_RULE, "BR_LLSC_2_13 : Bypassed - pMYKI_TAPurseControl->ActionSeqNo = 15, ActionList.actionSeqNo != 1");
                pData->ReturnedData.bypassCode  = BYPASS_CODE( 2, 13, 4, 0 );
                return RULE_RESULT_BYPASSED;
            }
        }
        else
        if ( pData->ActionList.actionlist.tPurseLoad.actionSequenceNo != ( pMYKI_TAPurseControl->ActionSeqNo + 1 ) )
        {
            CsDbg( BRLL_RULE, "BR_LLSC_2_13 : Bypassed - pMYKI_TAPurseControl->ActionSeqNo + 1 != ActionList.actionSeqNo");
            pData->ReturnedData.bypassCode  = BYPASS_CODE( 2, 13, 5, 0 );
            return RULE_RESULT_BYPASSED;
        }

        /*  6.  The t-Purse status is not equal to Blocked. */
        pDirectory  = &pMYKI_TAControl->Directory[ 0 ];

        if ( pDirectory->Status == TAPP_CONTROL_DIRECTORY_STATUS_BLOCKED )
        {
            /*  If the T-Purse is blocked, generate a FailureResponse of type TPurseBlocked. */
            if ( myki_br_ldt_PurseLoad_Debit_FailureResponse( pData, MYKI_UD_FAILURE_REASON_TPURSE_BLOCKED ) < 0 )
            {
                CsErrx( "BR_LLSC_2_13 : myki_br_ldt_PurseLoad_Debit_FailureResponse() failed" );
                return RULE_RESULT_ERROR;
            }   /* end-of-if */

            CsDbg( BRLL_RULE, "BR_LLSC_2_13 : Bypassed - Product status is blocked." );
            pData->ReturnedData.bypassCode  = BYPASS_CODE( 2, 13, 6, 0 );
            return RULE_RESULT_BYPASSED;
        }
    }

    /*  PROCESSING */
    {
        /*  1.  Perform a TPurseLoad/Debit transaction. */
        if ( myki_br_ldt_Actionlist( pData ) < 0 )
        {
            CsErrx( "BR_LLSC_2_13 : myki_br_ldt_Actionlist() failed" );
            return RULE_RESULT_ERROR;
        }   /* end-of-if */

        /*  NOTE:   TAppPurseControl.ActionSeqNo is updated by LDT */

        /*  1.  Modify load log */
        pAppLoadLog                             = &pData->InternalData.LoadLogData;

        /*      a.  Definition: */
        {
            /*      i.  TAppLoadLog.ControlBitmap: Value (Done by framework) */
            /*      ii. If LoadTxType is not set or is set to disable autoload (25) then */
            if ( pAppLoadLog->transactionType == MYKI_BR_TRANSACTION_TYPE_NONE              ||
                 pAppLoadLog->transactionType == MYKI_BR_TRANSACTION_TYPE_AUTO_LOAD_DISABLE )
            {
                /*      (1) Set to LoadTxType to TPurse Reverse (33) */
                pAppLoadLog->transactionType        = MYKI_BR_TRANSACTION_TYPE_TPURSE_AL_LOAD_VALUE;
            }
            else
            {
                /*      (2) Else set to Multiple Actionlist (21) */
                pAppLoadLog->transactionType        = MYKI_BR_TRANSACTION_TYPE_MULTIPLE_ACTION_LIST;
            }   /* end-of-if */

            /*      iii.Determine LoadLog ControlBitmap though look up of the TxLoadType (Done by framework)
                    iv. Set TxDateTime as current Date time (Done by framework) */
            /*      v.  TxSeqNo = NextTxSeqNo */
            /*      myki_br_ldt_PurseLoad_Debit increases NextTxSeqNo */ 
            pAppLoadLog->transactionSequenceNumber  = pMYKI_TAPurseControl->NextTxSeqNo - 1;

            pAppLoadLog->providerId =   pData->ActionList.actionlist.tPurseLoad.serviceProviderId;

            /*      vi. ServiceProvider =  as returned from actionlist 
                    vii.Set location object to the current device location
                        (i.e Entry Point, Route, Stop ID) (Done by framework) */
        }

        /*      b.  Value: */
        {
            /*      i.  Dyanamic.LoadLogTxValue = Dynamic.LoadLogTxValue Less Action List value */
            pData->DynamicData.loadLogTxValue      -= pData->ActionList.actionlist.tPurseLoad.value;

            /*      ii. If Dynamic.logLogTxValue less than zero */
            if ( pData->DynamicData.loadLogTxValue < 0 )
            {
                /*      (1) If LoadTxType is not Type T-Purse Reverse (33) then */
                if ( pAppLoadLog->transactionType != MYKI_BR_TRANSACTION_TYPE_TPURSE_AL_LOAD_VALUE )
                {
                    /*      (a) Set LoadLog.TxValue to zero */
                    pAppLoadLog->transactionValue   = (U32_t)0;
                }
                else
                {
                    /*      (b) Else  LoadLog.TxValue = Absolute Value of (Dynamic.LoadLogTxValue) */
                    pAppLoadLog->transactionValue   = (U32_t)abs( pData->DynamicData.loadLogTxValue );
                }   /* end-of-if */
            }
            else
            {
                /*      (2) Else Set LoadLog.TxValue to Dynamic.LoadLogTxValue */
                pAppLoadLog->transactionValue       = (U32_t)pData->DynamicData.loadLogTxValue;
            }   /* end-of-if */
            pAppLoadLog->isTransactionValueSet      = TRUE;

            /*      iii.NewTPurseBalance = t-Purse balance */
            pAppLoadLog->isNewTPurseBalanceSet      = TRUE;
            pAppLoadLog->newTPurseBalance           = pMYKI_TAPurseBalance->Balance;

            /*      iv. PaymentMethod = Ad-hoc autoload (6) */
            pAppLoadLog->isPaymentMethodSet         = TRUE;
            pAppLoadLog->paymentMethod              = TAPP_USAGE_LOG_PAYMENT_METHOD_ADHOC_AUTOLOAD;
        }

        /*  NOTE:   TAppLoadLog entry is added by application framework. */
        pData->InternalData.IsLoadLogUpdated        = TRUE;
        CsDbg( BRLL_RULE, "BR_LLSC_2_13 : Updated TAppLoadLog" );
    }

    /*  POST-CONDITIONS */
    {
        /*  1.  The T-Purse balance is decreased by the amount specified in the actionlist. */
        pData->ReturnedData.actionApplied       = TRUE;
        pData->ReturnedData.topupAmount        -= pData->ActionList.actionlist.tPurseLoad.value;
    }

    CsDbg( BRLL_RULE, "BR_LLSC_2_13 : Executed" );
    return RULE_RESULT_EXECUTED;
}

