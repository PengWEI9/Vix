/**************************************************************************
*   ID :  BR_LLSC_3_1
*   Autoload T-Purse
* 
*   Data Fields 
*   1. TAppTPurseControl.ControlBitmap
*   2. TAppTPurseBalance.Balance
*   3. TAppTPurseControl.AutoThreshold
*   4. TAppTPurseControl.AutoValue
*   5. Static.TPurseAutoEnabled
*   6. Dynamic.CurrentDateTime
*   7. Static.ServiceProviderID
*   8. Dynamic.EntryPointID
*   9. Dynamic.LineID
*   10. Dynamic.StopID
*   11. TAppTPurseControl.NextTxSeqNo
*   12. Dynamic.LoadLog
*   13. Dynamic.LoadLogTxValue
*   
*   Pre-Conditions 
*   1. The autoload bit in the T-Purse control bitmap(1) is set to 1 and the device T-Purse autoload flag(5) is set to 1.
*   2. The value of the T-Purse balance(2) is less than the value of the T-Purse autoload threshold(3).
*   
*   Description 
*   1. Perform a TPurseLoad/Autoload transaction:
*       a. Increment TAppTPurseBalance.Balance(2) by TAppTPurseControl.AutoValue(4).
*   2. Update load log
*       a. Definition:
*           i. If LoadTxType is not set then
*               (1) LoadTxType to Autoload Load value (30)
*               (2) Else Set LoadTxType To Multiple ActionLists (21)
*           ii. Determine loadlog ControlBitmap though look up of the TxLoadType
*           iii. TxSeqNo = NextTxSeqNo(11) 
*           iv. ProviderID = ServiceProviderID(7) 
*           v. Set location object to the current device location (i.e Entry Point, Route, Stop ID)
*       b. Value:
*           i. Dyanamic.LoadLogTxValue(13) = Dynamic.LoadLogTxValue(13) + Add value amount(4) 
*           ii. If Dynamic.LoadLogTxValue(13) is greater than 0 then set LoadLogTxValue(12) = Dynamic.LoadLogTxValue(13) Else Set LoadLogTxValue(12) = 0
*           iii. NewTPurseBalance = TAppTPurseBalance.Balance(2) 
*           iv. PaymentMethod = Reoccurring Auto load (5)
*   
*   Post-Conditions 
*   1. The T-Purse balance is increased by the autoload value.
*   
*   Devices 
*   All devices
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

RuleResult_e BR_LLSC_3_1( MYKI_BR_ContextData_t *pData )
{
    MYKI_TAControl_t            *pMYKI_TAControl;
    MYKI_TAPurseBalance_t       *pMYKI_TAPurseBalance;
    MYKI_TAPurseControl_t       *pMYKI_TAPurseControl;
    TAppLoadLog_t               *pAppLoadLog            = NULL;


    CsDbg( BRLL_RULE, "BR_LLSC_3_1 : Start (Autoload T-Purse)" );

    if ( ! pData )
    {
        CsErrx( "BR_LLSC_3_1 : Invalid argument(s)" );
        return RULE_RESULT_ERROR;
    }

    if ( MYKI_CS_TAControlGet( &pMYKI_TAControl ) < 0 )
    {
        CsErrx( "BR_LLSC_3_1 : MYKI_CS_TAControlGet() failed" );
        return RULE_RESULT_ERROR;
    }

    if ( MYKI_CS_TAPurseBalanceGet( &pMYKI_TAPurseBalance ) < 0 )
    {
        CsErrx( "BR_LLSC_3_1 : MYKI_CS_TAPurseBalanceGet() failed" );
        return RULE_RESULT_ERROR;
    }

    if ( MYKI_CS_TAPurseControlGet( &pMYKI_TAPurseControl ) < 0 )
    {
        CsErrx( "BR_LLSC_3_1 : MYKI_CS_TAPurseControlGet() failed" );
        return RULE_RESULT_ERROR;
    }

    //  1. The autoload bit in the T-Purse control bitmap(1) is set to 1 
    //     and the device T-Purse autoload flag(5) is set to 1.
    //
    if ( ! (    (pMYKI_TAPurseControl->ControlBitmap & TAPP_TPURSE_CONTROL_AUTOLOAD_BITMAP) 
             && (pData->StaticData.purseAutoEnabled == 1) ) )
    {
        CsDbg( BRLL_RULE, "BR_LLSC_3_1 : Bypassed - purseAutoEnabled or TAPP_TPURSE_CONTROL_AUTOLOAD_BITMAP not set." );
        return RULE_RESULT_BYPASSED;
    }

    //  2. The value of the T-Purse balance(2) is less than the value of the T-Purse autoload threshold3.
    // 
    //  Note:
    //      Autothreshold is a U32, Balance is an S32.
    //      By casting without checking, this will fail if autothreshold > 0xFFFFFFF but that's unlikely in this case

    if ( ! ( pMYKI_TAPurseBalance->Balance < (S32_t)pMYKI_TAPurseControl->AutoThreshold ) )
    {
        CsDbg( BRLL_RULE, "BR_LLSC_3_1 : Bypassed - pMYKI_TAPurseBalance->Balance not less than pMYKI_TAPurseControl->AutoThreshold" );
        return RULE_RESULT_BYPASSED;
    }

    //  1. Perform a TPurseLoad/Autoload transaction:
    //      a. Increment TAppTPurseBalance.Balance(2) by TAppTPurseControl.AutoValue(4).
    //
    //  Note: Increment TAppTPurseBalance.Balance is done within myki_br_ldt_PurseLoad_Autoload()
    //
    if ( myki_br_ldt_PurseLoad_Autoload( pData, pData->ReturnedData.applicableFare ) < 0 )
    {
        CsErrx("BR_LLSC_3_1 : myki_br_ldt_PurseLoad_Autoload() failed");
        return RULE_RESULT_ERROR;
    }

    //  2. Update load log
    //      a. Definition:

    pData->InternalData.IsLoadLogUpdated = TRUE;
    pAppLoadLog = &pData->InternalData.LoadLogData;

    //          i. If LoadTxType is not set then
    //              (1) LoadTxType to Autoload Load value (30)
    //              (2) Else Set LoadTxType To Multiple ActionLists (21)
    if ( pAppLoadLog->transactionType == MYKI_BR_TRANSACTION_TYPE_NONE )
    {
        pAppLoadLog->transactionType        = MYKI_BR_TRANSACTION_TYPE_TPURSE_AUTO_LOAD_VALUE;
    }
    else
    {
        pAppLoadLog->transactionType        = MYKI_BR_TRANSACTION_TYPE_MULTIPLE_ACTION_LIST;
    }

    //          ii. Determine loadlog ControlBitmap though look up of the TxLoadType (Done by framework)
    //          iii. TxSeqNo = NextTxSeqNo(11)
    // 
    pAppLoadLog->transactionSequenceNumber  = pMYKI_TAPurseControl->NextTxSeqNo - 1;

    //  
    //          iv. ProviderID = ServiceProviderID(7) (Done by framework) 
    //          v. Set location object to the current device location (i.e Entry Point, Route, Stop ID) (Done by framework)
    // 
    //      b. Value:
    //          i.   Dyanamic.LoadLogTxValue(13) = Dynamic.LoadLogTxValue(13) + Add value amount(4)
    // 
    pAppLoadLog->isTransactionValueSet      = TRUE;
    pData->DynamicData.loadLogTxValue      += pMYKI_TAPurseControl->AutoValue;

    //          ii. If Dynamic.LoadLogTxValue(13) is greater than 0
    if ( pData->DynamicData.loadLogTxValue > 0 )
    {
        //          then set LoadLogTxValue(12) = Dynamic.LoadLogTxValue(13) 
        pAppLoadLog->transactionValue       = (U32_t)pData->DynamicData.loadLogTxValue;
    }
    else
    {
        //          Else Set LoadLogTxValue(12) = 0
        pAppLoadLog->transactionValue       = (U32_t)0;
    }

    // 
    //          iii. NewTPurseBalance = TAppTPurseBalance.Balance(2) 
    //
    pAppLoadLog->isNewTPurseBalanceSet      = TRUE;
    pAppLoadLog->newTPurseBalance           = pMYKI_TAPurseBalance->Balance;

    //          iv.  PaymentMethod = Reoccurring Auto load (5)
    //
    pAppLoadLog->isPaymentMethodSet         = TRUE;
    pAppLoadLog->paymentMethod              = TAPP_USAGE_LOG_PAYMENT_METHOD_RECURRING_AUTOLOAD;


    CsDbg( BRLL_RULE, "BR_LLSC_3_1 : Executed" );
    return RULE_RESULT_EXECUTED;
}

