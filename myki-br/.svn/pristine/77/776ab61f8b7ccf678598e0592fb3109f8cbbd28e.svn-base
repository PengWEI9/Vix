/**************************************************************************
*   ID :  BR_LLSC_4_1
*    Missing Scan-On Station Exit Fee
*
* Data Fields
*    1.  TAppControl.ProductInUse
*    2.  Dynamic.StopId
*    3.  Tariff.StationExitFee
*    4.  Static.ServiceProviderId
*    5.  TAppTProduct.PurchaseValue
*    6.  TAppTProduct.NextTxSeqNo
*
* Pre-Conditions
*    1.  The product in use field(1) is 0 (no products are scanned-on).
*    2.  The station exit fee(3) for the current station(2) is greater than zero.
*
* Description
*    1.  Perform a TPurseUsage/Penalty transaction for the value of the station exit fee(3).
*    2.  Modify usage log
*        a.  Definition:
*            i.     TAppLoadLog.ControlBitmap: Value,
*            ii.    TxType = Station Exit Fee (15)
*            iii.   ProviderID =  ServiceProviderID(4)
*        b.  Value:
*            i.     TxValue = TxValue + station Exit fee(3)
*            ii.    NewTPurseBalance = As returned from TPurseUsage/Penalty transaction
*            iii.   PaymentMethod = T-Purse (1)
*
* Post-Conditions
*    1.  A station exit fee is charged.
* Devices
*    Exit gates
*
 ***********************************************************************/

#include <cs.h>                 // For debug logging
#include <myki_cardservices.h>  // Myki Card Services
#include <LDT.h>                // Logical Device Transaction

#include "myki_br_rules.h"      // API for all BR_xxx modules
#include "BR_Common.h"          // Common utilities, macros, etc

//=============================================================================
//
//
//
//=============================================================================

RuleResult_e BR_LLSC_4_1( MYKI_BR_ContextData_t *pData )
{
    MYKI_TAControl_t       *pMYKI_TAControl         = NULL;
    TAppUsageLog_t         *pTAppUsageLog           = NULL;
    MYKI_TAPurseBalance_t  *pMYKI_TAPurseBalance    = NULL;

    CsDbg( BRLL_RULE, "BR_LLSC_4_1 : Start (Missing Scan-On Station Exit Fee)" );

    if ( ! pData )
    {
        CsErrx( "BR_LLSC_4_1 : Invalid argument(s)" );
        return RULE_RESULT_ERROR;
    }

    if ( MYKI_CS_TAControlGet( &pMYKI_TAControl ) < 0 )
    {
        CsErrx( "BR_LLSC_4_1 : MYKI_CS_TAControlGet() failed" );
        return RULE_RESULT_ERROR;
    }

    //  1.  The product in use field(1) is 0 (no products are scanned-on).

    if ( pMYKI_TAControl->ProductInUse != 0 )
    {
        CsDbg( BRLL_RULE, "BR_LLSC_4_1 : Bypass - Product in use" );
        pData->ReturnedData.bypassCode = 1;
        return RULE_RESULT_BYPASSED;
    }

    //  2.  The station exit fee(3) for the current station(2) is greater than zero.

    if ( pData->Tariff.stationExitFee <= 0 )
    {
        CsDbg( BRLL_RULE,"BR_LLSC_4_1 : Bypass - Station Exit Fee (%d) <= 0", pData->Tariff.stationExitFee );
        pData->ReturnedData.bypassCode = 2;
        return RULE_RESULT_BYPASSED;
    }

    //
    //  Indicate to Test Harness that all Pre-Conditions have been satisfied
    //
    pData->ReturnedData.bypassCode = 3;
    
    //----------------------------------------------------------------------------
    //  Description
    //  1.  Perform a TPurseUsage/Penalty transaction for the value of the station exit fee(3).
    if (myki_br_ldt_PurseUsage_Penalty(pData, pData->Tariff.stationExitFee) < 0 )
    {
        CsErrx("BR_LLSC_4_1 : MYKI_LDT_TPurseUsagePenalty() failed");
        return RULE_RESULT_ERROR;
    }

    /*  2.  Modify usage log */
    pTAppUsageLog                                   = &pData->InternalData.UsageLogData;

    /*      a.  Definition: */
    {
        /*      i.  TxType = Station Exit Fee (15) */
        pTAppUsageLog->transactionType              = MYKI_BR_TRANSACTION_TYPE_STATION_EXIT_FEE;

        /*      ii. Determine uselog ControlBitmap though look up of the TxUsageType (Done by framework)
                iii.ProviderID =  ServiceProviderID (Done by framework)
                iv. TXDateTime as Current DateTime (Done by framework) */
    }

    /*      b.  Value: */
    {
        /*      i.  TxValue = TxValue + station Exit fee */
        pTAppUsageLog->isTransactionValueSet        = TRUE;
        pTAppUsageLog->transactionValue            += pData->Tariff.stationExitFee;

        /*      ii. NewTPurseBalance = As returned from TPurseUsage/Penalty transaction */
        if ( MYKI_CS_TAPurseBalanceGet( &pMYKI_TAPurseBalance ) < 0 || pMYKI_TAPurseBalance == NULL )
        {
            CsErrx( "BR_LLSC_4_1 : MYKI_CS_TAPurseBalanceGet() failed" );
            return RULE_RESULT_ERROR;
        }   /* end-of-if */
        pTAppUsageLog->isNewTPurseBalanceSet        = TRUE;
        pTAppUsageLog->newTPurseBalance             = pMYKI_TAPurseBalance->Balance;

        /*      iii.PaymentMethod = T-Purse (1) */
        pTAppUsageLog->isPaymentMethodSet           = TRUE;
        pTAppUsageLog->paymentMethod                = TAPP_USAGE_LOG_PAYMENT_METHOD_TPURSE;
    }

    /*  NOTE:   TAppUsageLog entry is added by application framework. */
    pData->InternalData.IsUsageLogUpdated           = TRUE;
    CsDbg( BRLL_RULE, "BR_LLSC_4_1 : Updated TAppUsageLog" );

    CsDbg( BRLL_RULE, "BR_LLSC_4_1 : Executed" );
    return RULE_RESULT_EXECUTED;
}

