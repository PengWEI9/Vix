/* -*- mode: c; tabs: 4; -*- */
/*=============================================================================
**
**    Vix Technology                   Licensed software
**    (C) 2013                         All rights reserved
**
**=============================================================================
**
**  Project/Product : MBU
**  Filename        : BR_LLSC_2_4.c
**  Author(s)       : ???
**
**  ID              : BR_LLSC_2_4 - NTS0177 v7.2
**
**  Name            : Process Actionlist Enable/Disable T-Purse Autoload
**
**  Data Fields     :
**
**      1.  Tariff.TPurseMaximumBalance
**      2.  Actionlist.Type
**      3.  Actionlist.ActionSeqNo
**      4.  Actionlist.AutoValue
**      5.  TAppControl.Directory.Status
**      6.  TAppTPurseControl.ActionSeqNo
**      7.  TAppTPurseBalance.Balance
**      8.  Dynamic.AutoLoad
**      9.  Dynamic.CurrentDateTime
**
**  Pre-Conditions  :
**
**      1.  An actionlist entry exists for the card UID.
**      2.  The actionlist type is equal to TPurseUpdate/None (autoload
**          enable/disable).
**      3.  The actionlist action sequence number is between 1 and 15.
**      4.  If the t-Purse action sequence number is equal to 15:
**          a.  The actionlist action sequence number is equal to 1.
**      5.  Else:
**          a.  The actionlist action sequence number is equal to the
**              t-Purse action sequence number + 1.
**      6.  The t-Purse status is not equal to Blocked. If the t-Purse
**          is blocked, generate a FailureResponse of type TPurseBlocked.
**      7.  The t-Purse balance + the autoload value specified in the
**          actionlist is less than or equal to the maximum allowable T-Purse
**          balance. If not, generate a FailureResponse of type ValueLimitReached.
**
**  Description     :
**
**      1.  Perform a TPurseUpdate/None (autoload enable/disable) transaction.
**      2.  Modify load log
**          a.  Definition:
**              i.  If LoadTxType is set; then set  LoadTxType to Multiple
**                  Actionlists (21) else
**                  (1) If the transaction type is enable then set LoadTxType
**                      to t-Purse autoload enable (24) Else set t-Purse
**                      Autoload disable (25)
**              ii. Determine loadlog ControlBitmap though look up of the
**                  TxLoadType
**              iii.Set TxDateTime as current Date time
**              iv. TxSeqNo = NextTxSeqNo
**              v.  ServiceProvider = as returned from the TPurseUpdate/None
**             [vi.]Set location object to the current device location
**                  (i.e Entry Point, Route, Stop ID)
**
**  Post-Conditions :
**
**      1.  t-Purse autoload is enabled/disabled on the smartcard.
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
**    1.01  11.11.13    ANT   Add      Added generating TAppLoadLog record
**    1.02  15.11.13    ANT   Modify   Removed updating
**                                     TAppPurseControl.ActionSeqNo as done
**                                     by LDT
**    1.03  03.12.13    ANT   Add      Generating FailureResponse messages
**    1.04  04.12.13    ANT   Modify   No code change. Updated comments.
**    1.05  29.10.14    ANT   Modify   Changed myki_br_GetActionlist() signature.
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
**  BR_LLSC_2_4
**
**  Description     :
**      Implements business rule BR_LLSC_2_4.
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

RuleResult_e BR_LLSC_2_4( MYKI_BR_ContextData_t *pData )
{
    MYKI_TAControl_t            *pMYKI_TAControl;
    MYKI_TAPurseBalance_t       *pMYKI_TAPurseBalance;
    MYKI_TAPurseControl_t       *pMYKI_TAPurseControl;
    MYKI_Directory_t            *pDirectory;
    CT_CardInfo_t               *pCardInfo;
    int                          errcode;
    TAppLoadLog_t               *pAppLoadLog            = NULL;

    CsDbg( BRLL_RULE, "BR_LLSC_2_4 : Start (Process Actionlist Enable/Disable T-Purse Autoload)" );

    if ( ! pData )
    {
        CsErrx( "BR_LLSC_2_4 : Invalid argument(s)" );
        return RULE_RESULT_ERROR;
    }

    if ( MYKI_CS_TAControlGet( &pMYKI_TAControl ) < 0 )
    {
        CsErrx( "BR_LLSC_2_4 : MYKI_CS_TAControlGet() failed" );
        return RULE_RESULT_ERROR;
    }

    pDirectory = &pMYKI_TAControl->Directory[0];  // Directory Zero contains the purse

    if ( MYKI_CS_GetCardInfo( &pCardInfo ) < 0 )
    {
        CsErrx( "BR_LLSC_2_4 : MYKI_CS_GetCardInfo() failed" );
        return RULE_RESULT_ERROR;
    }

    if ( MYKI_CS_TAPurseBalanceGet( &pMYKI_TAPurseBalance ) < 0 )
    {
        CsErrx( "BR_LLSC_2_4 : MYKI_CS_TAPurseBalanceGet() failed" );
        return RULE_RESULT_ERROR;
    }

    if ( MYKI_CS_TAPurseControlGet( &pMYKI_TAPurseControl ) < 0 )
    {
        CsErrx( "BR_LLSC_2_4 : MYKI_CS_TAPurseControlGet() failed" );
        return RULE_RESULT_ERROR;
    }

    /*  PRE-CONDITIONS */
    {
        U8_t    searchActionSequenceNumber  = ( pMYKI_TAPurseControl->ActionSeqNo == 15 ? 1 : ( pMYKI_TAPurseControl->ActionSeqNo + 1 ) );

        // populate the ActionList structure
        if ( pData->ActionList.type == ACTION_TPURSE_UPDATE_NONE )
        {
            if ( pData->ActionList.actionlist.tPurseUpdate.actionSequenceNo == searchActionSequenceNumber )
            {
                CsDbg( BRLL_RULE, "BR_LLSC_2_4 : found desired actionlist in cache" );
            }
            else
            {
                pData->ActionList.type  = ACTION_NULL;
            }
        }

        if ( pData->ActionList.type != ACTION_TPURSE_UPDATE_NONE )
        {
            if ( ( errcode = myki_br_GetActionlist( pCardInfo->SerialNumber, ACTION_TPURSE_UPDATE_NONE, searchActionSequenceNumber, 0 /*DONTCARE*/, pData ) ) < 0 )
            {
                CsDbg( BRLL_RULE, "BR_LLSC_2_4 : myki_br_GetActionlist returns error %d", errcode);
                return RULE_RESULT_ERROR;
            }
        }

        /*  1.  An actionlist entry exists for the card UID.
            2.  The actionlist type is equal to TPurseUpdate/None (autoload enable/disable). */
        if ( pData->ActionList.type != ACTION_TPURSE_UPDATE_NONE )
        {
            CsDbg( BRLL_RULE, "BR_LLSC_2_4 : Bypassed - ActionList.type is not ACTION_TPURSE_UPDATE_NONE." );
            pData->ReturnedData.bypassCode  = BYPASS_CODE( 2, 4, 2, 0 );
            return RULE_RESULT_BYPASSED;
        }

        /*  3.  The actionlist action sequence number is between 1 and 15. */
        if ( pData->ActionList.actionlist.tPurseUpdate.actionSequenceNo < 1 || pData->ActionList.actionlist.tPurseUpdate.actionSequenceNo > 15 )
        {
            CsDbg( BRLL_RULE, "BR_LLSC_2_4 : Bypassed - ActionList.actionSeqNo (%d) is out of range.",
                pData->ActionList.actionlist.tPurseUpdate.actionSequenceNo );
            pData->ReturnedData.bypassCode  = BYPASS_CODE( 2, 4, 3, 0 );
            return RULE_RESULT_BYPASSED;
        }

        /*  4.  If the t-Purse action sequence number is equal to 15:
                a.  The actionlist action sequence number is equal to 1.
            5.  Else:
                a.  The actionlist action sequence number is equal to the
                    t-Purse action sequence number + 1. */
        CsDbg( BRLL_RULE, "BR_LLSC_2_4 : pMYKI_TAPurseControl->ActionSeqNo = %d, pData->ActionList.actionSeqNo = %d",
            pMYKI_TAPurseControl->ActionSeqNo, pData->ActionList.actionlist.tPurseUpdate.actionSequenceNo );

        if ( pMYKI_TAPurseControl->ActionSeqNo == 15 )
        {
            if ( pData->ActionList.actionlist.tPurseUpdate.actionSequenceNo != 1 )
            {
                CsDbg( BRLL_RULE, "BR_LLSC_2_4 : Bypassed - pMYKI_TAPurseControl->ActionSeqNo = 15, ActionList.actionSeqNo != 1");
                pData->ReturnedData.bypassCode  = BYPASS_CODE( 2, 4, 4, 0 );
                return RULE_RESULT_BYPASSED;
            }
        }
        else
        if ( pData->ActionList.actionlist.tPurseUpdate.actionSequenceNo != ( pMYKI_TAPurseControl->ActionSeqNo + 1 ) )
        {
            CsDbg( BRLL_RULE, "BR_LLSC_2_4 : Bypassed - pMYKI_TAPurseControl->ActionSeqNo + 1 != ActionList.actionSeqNo");
            pData->ReturnedData.bypassCode  = BYPASS_CODE( 2, 4, 5, 0 );
            return RULE_RESULT_BYPASSED;
        }

        /*  6.  The t-Purse status is not equal to Blocked. */
        if ( pDirectory->Status == TAPP_CONTROL_DIRECTORY_STATUS_BLOCKED )
        {
            /*  If the t-Purse is blocked, generate a FailureResponse of type TPurseBlocked. */
            if ( myki_br_ldt_PurseUpdate_None_FailureResponse( pData, MYKI_UD_FAILURE_REASON_TPURSE_BLOCKED ) < 0 )
            {
                CsErrx( "BR_LLSC_2_4 : myki_br_ldt_PurseUpdate_None_FailureResponse() failed" );
                return RULE_RESULT_ERROR;
            }   /* end-of-if */

            CsDbg( BRLL_RULE, "BR_LLSC_2_4 : Bypassed - Product is already blocked.");
            pData->ReturnedData.bypassCode  = BYPASS_CODE( 2, 4, 6, 0 );
            return RULE_RESULT_BYPASSED;
        }

        /*  7.  The t-Purse balance + the autoload value specified in the actionlist
                is less than or equal to the maximum allowable T-Purse balance. */
        /*  NOTE:   Should this pre-condition read "The t-Purse autoload threshold + autoload value ..." !? */
        if ( ( pMYKI_TAPurseBalance->Balance + pData->ActionList.actionlist.tPurseUpdate.autoValue ) > pData->Tariff.TPurseMaximumBalance )
        {
            /*  If not, generate a FailureResponse of type ValueLimitReached. */
            if ( myki_br_ldt_PurseUpdate_None_FailureResponse( pData, MYKI_UD_FAILURE_REASON_VALUE_LIMIT_REACHED ) < 0 )
            {
                CsErrx( "BR_LLSC_2_4 : myki_br_ldt_PurseUpdate_None_FailureResponse() failed" );
                return RULE_RESULT_ERROR;
            }   /* end-of-if */

            CsDbg( BRLL_RULE, "BR_LLSC_2_4 : Bypassed - pMYKI_TAPurseBalance->Balance will exceed maximum after actionlist.");
            pData->ReturnedData.bypassCode  = BYPASS_CODE( 2, 4, 7, 0 );
            return RULE_RESULT_BYPASSED;
        }
    }

    /*  PROCESSING */
    {
        /*  1.  Perform a TPurseUpdate/None (autoload enable/disable) transaction. */
        if ( myki_br_ldt_Actionlist( pData ) < 0 )
        {
            CsErrx( "BR_LLSC_2_4 : myki_br_ldt_Actionlist() failed" );
            return RULE_RESULT_ERROR;
        }

        /*  NOTE:   TAppPurseControl.ActionSeqNo is updated by LDT */

        /*  2.  Modify load log */
        pAppLoadLog                             = &pData->InternalData.LoadLogData;

        /*      a.  Definition: */
        {
            /*      i.  If LoadTxType is set; */
            if ( pAppLoadLog->transactionType != MYKI_BR_TRANSACTION_TYPE_NONE )
            {
                /*      then set LoadTxType  to Multiple Actionlists (21) else */
                pAppLoadLog->transactionType        = MYKI_BR_TRANSACTION_TYPE_MULTIPLE_ACTION_LIST;
            }
            else
            /*          (1) If the transaction type is enable */
            if ( pData->ActionList.actionlist.tPurseUpdate.isAutoload != FALSE )
            {
                /*          then set LoadTxType to t-Purse autoload enable (24) */
                pAppLoadLog->transactionType        = MYKI_BR_TRANSACTION_TYPE_AUTO_LOAD_ENABLE;
            }
            else
            {
                /*          Else set  t-Purse Autoload disable (25) */
                pAppLoadLog->transactionType        = MYKI_BR_TRANSACTION_TYPE_AUTO_LOAD_DISABLE;
            }   /* end-of-if */

            /*      ii. Determine loadlog ControlBitmap though look up of the TxLoadType (Done by framework)
                    iii.Set TxDateTime as current Date time (Done by framework) */
            /*      iv. TxSeqNo = NextTxSeqNo */
/*          NOTE:   Although Section 3.1.4 "Actionlist" of KA0004 v7.0 specifies TAppLoadLog.TxSeqno
                    is set to TAppPurseControl.ActionSeqNo, it should, however, be set to
                    TAppPurseControl.NextTxSeqno */
            /*      myki_br_ldt_PurseUpdate_None increase NextTxSeqNo */
            pAppLoadLog->transactionSequenceNumber  = pMYKI_TAPurseControl->NextTxSeqNo - 1;


			pAppLoadLog->isPaymentMethodSet           = TRUE;
            if ( pData->StaticData.AcsCompatibilityMode )
            {
                /*  NOTE:   KAMCO reader has PaymentMethod set to Cash although
                            transaction amount is zero */
                pAppLoadLog->paymentMethod            = TAPP_USAGE_LOG_PAYMENT_METHOD_CASH;
            }
            else
            {
                pAppLoadLog->paymentMethod            = TAPP_USAGE_LOG_PAYMENT_METHOD_UNDEFINED;
            }

            /*      v.  ServiceProvider = as returned from the TPurseUpdate/None (Done by framework)
                   [vi.]Set location object to the current device location
                        (i.e Entry Point, Route, Stop ID) (Done by framework) */
			pAppLoadLog->providerId              = pData->ActionList.actionlist.tPurseUpdate.serviceProviderId;
        }

        /*  NOTE:   TAppLoadLog entry is added by application framework. */
        pData->InternalData.IsLoadLogUpdated        = TRUE;
        CsDbg( BRLL_RULE, "BR_LLSC_2_4 : Updated TAppLoadLog" );
    }

    /*  POST-CONDITIONS */
    {
        /*  1.  t-Purse autoload is enabled/disabled on the smartcard. */
        pData->ReturnedData.actionApplied       = TRUE;
    }

    CsDbg( BRLL_RULE, "BR_LLSC_2_4 : Executed" );

    return RULE_RESULT_EXECUTED;
}

