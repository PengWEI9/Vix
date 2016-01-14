/* -*- mode: c; tabs: 4; -*- */
/*=============================================================================
**
**    Vix Technology                   Licensed software
**    (C) 2015                         All rights reserved
**
**=============================================================================
**
**  Project/Product : MBU
**  Filename        : BR_VIX_LLSC_21_1.cpp
**  Author(s)       : Sveta Atseva
**
**  ID              : BR_VIX_LLSC_21_1
**
**  Name            : TPurse Deduct Value
**
**  Data Fields     :
**
**      1.  TAppTPurseBalance.Balance
**      2.  TAppTPurseControl.NextTxSeqNo
**      3.  StaticData.serviceProviderId
**      4.  DynamicData.lineId
**      5.  DynamicData.entryPointId
**      6.  DynamicData.stopId
**      7.  DynamicData.tPurseDebitAmount
**
**  Pre-Conditions  :
**
**      1.  TPurse debit amount (7) is greater than 0.
**      2.  TPurse debit amount (7) is less than or equal to TPurse balance (1).
**      
**
**  Description     :
**
**      1.  Perform a TPurseUsage/Transit tranaction:
**          a.  Decrement TAppTPurseBalance.Balance (1) by the debit amount (7).
**      2.  Create usage log:
**          a.  Definition:
**              i.  If the transaction type is not set then 
**                  1. set the transaction type to TPurse usage manual (9).
**                  2. else set the transaction type to Multiple Actionlists (21).
**              ii. Determine usage log control bit map through look up of the transaction usage type,
**              iii.Set ProviderID to service provider id (3),
**              iv. Set location to current location.
** 
**          b.  Value:
**              i.  Set transaction value to debit amount,
**              ii. Set new TPurse balance to TPurse balance after deduction,
**              iii.Set payment method to TPurse (1).
**
**          c.  Usage:
**              i.  Set zone to current zone,
**              ii. Set product validation status to zero.
** 
**
**  Post-Conditions :
**
**      1.  Debit amount is deducted from TPurse balance.
**
**  Member(s)       :
**      BR_VIX_LLSC_21_1        [public]    business rule
**
**  Information     :
**   Compiler(s)    : C++
**   Target(s)      : Independent
**
**  Subversion      :
**      $Id: BR_VIX_LLSC_21_1.c $
**      $HeadURL: https://auperasvn01.aupera.erggroup.com/svn/DPG_SWBase/myki-br/trunk/src/BR_VIX_LLSC_21_1.c $
**
**  History         :
**   Vers.  Date        Aut.  Type     Description
**   -----  ----------  ----  -------  ----------------------------------------
**    1.00  14.12.15    SAT   Create
**    
**
**===========================================================================*/

/*
 *      Options
 *      -------
 */

/*
 *      Includes
 *      --------
 */

#include <cs.h>
#include <myki_cardservices.h>
#include <LDT.h>
#include "myki_br_rules.h"
#include "BR_Common.h"

/*
 *      Local Constants and Macros
 *      --------------------------
 */

/*
 *      Local Variables
 *      ---------------
 */

/*
 *      Global Variables
 *      ----------------
 */

/*==========================================================================*
**
**  BR_VIX_LLSC_21_1
**
**  Description     :
**      Implements business rule BR_VIX_LLSC_21_1.
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
**
**==========================================================================*/

RuleResult_e    BR_VIX_LLSC_21_1( MYKI_BR_ContextData_t *pData )
{
    MYKI_TAPurseBalance_t      *pMYKI_TAPurseBalance    = NULL;
    MYKI_TAPurseControl_t      *pMYKI_TAPurseControl    = NULL;
    TAppUsageLog_t             *pTAppUsageLog           = NULL;
    int                         nResult                 = 0;

    CsDbg( BRLL_RULE, "BR_VIX_LLSC_21_1 : Start (Deduct T-Purse)" );

    if ( pData == NULL )
    {
        CsErrx( "BR_VIX_LLSC_21_1 : Invalid argument(s)" );
        return  RULE_RESULT_ERROR;
    }

    if ( ( nResult = MYKI_CS_TAPurseBalanceGet( &pMYKI_TAPurseBalance ) ) < 0 )
    {
        CsErrx( "BR_VIX_LLSC_21_1 : MYKI_CS_TAPurseBalanceGet() failed (%d)", nResult );
        return  RULE_RESULT_ERROR;
    }   

    if ( ( nResult = MYKI_CS_TAPurseControlGet( &pMYKI_TAPurseControl ) ) < 0 )
    {
        CsErrx( "BR_VIX_LLSC_20_1 : MYKI_CS_TAPurseControlGet() failed (%d)", nResult );
        return  RULE_RESULT_ERROR;
    }    

    /*  PRE-CONDITIONS */
    {
        /*  1.  TPurse debit amount (7) is greater than 0.                           */
        if ( pData->DynamicData.tPurseDebitAmount <= 0 )
        {
            CsDbg( BRLL_RULE, "BR_VIX_LLSC_21_1 : Bypassed - debit amount less than or equal to 0" );
            pData->ReturnedData.bypassCode      = BYPASS_CODE( 21, 1, 1, 0 );
            pData->ReturnedData.rejectReason    = MYKI_BR_REJECT_REASON_INVALID_AMOUNT;
            return  RULE_RESULT_BYPASSED;
        }

        /*  2.  TPurse debit amount (7) is less than or equal to TPurse balance (1). */
        if ( pMYKI_TAPurseBalance->Balance - pData->DynamicData.tPurseDebitAmount < 0 )
        {
            CsDbg( BRLL_RULE, "BR_VIX_LLSC_21_1 : Bypassed - TPurse debit amount greater than TPurse balance" );
            pData->ReturnedData.bypassCode      = BYPASS_CODE( 21, 1, 2, 0 );
            pData->ReturnedData.rejectReason    = MYKI_BR_REJECT_REASON_INSUFFICIENT_FUND;
            return  RULE_RESULT_BYPASSED;
        }
        
    }

    /*  PROCESSING */
    {
        /*  1.  Perform a TPurseUsage/Transit tranaction:
                a.  Decrement TAppTPurseBalance.Balance (1) by the debit amount (7). */
        if ( myki_br_ldt_PurseUsage_Transit( pData, pData->DynamicData.tPurseDebitAmount ) < 0 )
        {
            CsErrx( "BR_VIX_LLSC_21_1 : myki_br_ldt_PurseUsage_Transit() failed");
            return  RULE_RESULT_ERROR;
        }

        /*  2.  Create usage log: */
        {
            pTAppUsageLog                                   = &pData->InternalData.UsageLogData;
            pData->InternalData.IsUsageLogUpdated           = TRUE;
            pTAppUsageLog->transactionDateTime              = pData->DynamicData.currentDateTime;
            
            /*  a. Definition: */
            {
                
                if ( pTAppUsageLog->transactionType == MYKI_BR_TRANSACTION_TYPE_NONE )
                {
                    /*1. If the transaction type is not set, then set the transaction type to TPurse usage manual (9).*/
                    pTAppUsageLog->transactionType          = MYKI_BR_TRANSACTION_TYPE_PURSE_USAGE_MANUAL;
                }               
                else
                {
                    /*2. set the transaction type to Multiple Actionlists (21)*/
                    pTAppUsageLog->transactionType          = MYKI_BR_TRANSACTION_TYPE_MULTIPLE_ACTION_LIST;
                }
                    
                /*  ii. Determine usage log control bit map through look up of the transaction usage type            */
                //TODO pTAppUsageLog->ControlBitmap = 

                /*  iii. Set ProviderID to service provider id (3)               */
                pTAppUsageLog->providerId = pData->StaticData.serviceProviderId;
                
                /*  iv. Set location to current location                         */
                pTAppUsageLog->stopId = pData->DynamicData.stopId;
            }

            /*  b.  Value: */
            {
                /*  i.  Set transaction value to debit amount                    */
                pTAppUsageLog->isTransactionValueSet        = TRUE;
                pTAppUsageLog->transactionValue             = pData->DynamicData.tPurseDebitAmount;

                /*  ii. Set new TPurse balance to TPurse balance after deduction */
                pTAppUsageLog->isNewTPurseBalanceSet        = TRUE;
                pTAppUsageLog->newTPurseBalance             = (U32_t)pData->DynamicData.loadLogTxValue;
                
                /*  iii.Set payment method to TPurse (1)                         */
                pTAppUsageLog->isPaymentMethodSet           = TRUE;
                pTAppUsageLog->paymentMethod                = PAYMENT_METHOD_TPURSE;
            }
            
            /*  c.  Usage: */
            {
                /*  i.  Set zone to current zone                                 */
                 pTAppUsageLog->isZoneSet                   = TRUE;
                 pTAppUsageLog->zone                        = pData->DynamicData.currentZone;

                /*  ii. Set product validation status to zero                    */
                pTAppUsageLog->isProductValidationStatusSet = TRUE;
                pTAppUsageLog->productValidationStatus     |= TAPP_USAGE_LOG_PROD_VAL_STATUS_ACTIVATE_BITMAP;
            }
        }
    }

    /*  Post-Conditions 
           1. Debit amount is deducted from TPurse balance */
    {
        pData->ReturnedData.remainingBalance                = pMYKI_TAPurseBalance->Balance;
        pData->ReturnedData.acceptReason                    = MYKI_BR_ACCEPT_TPURSE_DEBITED;
        pData->ReturnedData.txnSeqNo                        = pMYKI_TAPurseControl->NextTxSeqNo - 1;
    }

    CsDbg( BRLL_RULE, "BR_VIX_LLSC_21_1 : Executed" );
    return  RULE_RESULT_EXECUTED;
}   /* BR_VIX_LLSC_21_1( ) */
