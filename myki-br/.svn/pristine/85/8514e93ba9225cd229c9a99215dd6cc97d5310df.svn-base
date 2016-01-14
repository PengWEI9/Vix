/**************************************************************************
*   ID :  BR_LLSC_4_2
*
* Missing Scan-On
*    1.  TAppControl.ProductInUse
*    2.  Dynamic.StopId
*    3.  Tariff.StationExitFee
*    4.  TAppTProduct.PurchaseValue
*    5.  TAppTProduct.NextTxSeqNo
*    6.  TAppTPurse.CurrentBalance
*    7.  UseageLog
*
* Pre-Conditions
*    1.  The product in use field(1) is 0 (no products are scanned-on).
*    2.  The station exit fee(3) for the current station(2) is zero.
*
* Description
*    1.  Reject the smartcard.
*    2.  Modify usage log
*    a.  Definition:
*    i.  TxType = Invalid provisional (27)
*    ii.  ProviderID as retruned from the device
*    iii.  TxDateTime = Current Date time
*    iv.  Location = The current location
*
* Post-Conditions
*    1.  The smartcard is rejected.
*
* Devices
*    Exit gates
*
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

RuleResult_e BR_LLSC_4_2( MYKI_BR_ContextData_t *pData )
{
    MYKI_TAControl_t *pMYKI_TAControl;

    CsDbg( BRLL_RULE, "BR_LLSC_4_2 : Start (Missing Scan-On)" );

    if ( ! pData )
    {
        CsErrx( "BR_LLSC_4_2 : Invalid argument(s)" );
        return RULE_RESULT_ERROR;
    }

    if ( MYKI_CS_TAControlGet( &pMYKI_TAControl ) < 0 )
    {
        CsErrx( "BR_LLSC_4_2 : MYKI_CS_TAControlGet() failed" );
        return RULE_RESULT_ERROR;
    }

    //  1.  The product in use field(1) is 0 (no products are scanned-on).

    if ( pMYKI_TAControl->ProductInUse != 0 )
    {
        CsDbg( BRLL_RULE, "BR_LLSC_4_2 : Bypass - Product in use" );
        return RULE_RESULT_BYPASSED;
    }

    //  2.  The station exit fee(3) for the current station(2) is zero.

    if ( pData->Tariff.stationExitFee != 0 )
    {
        CsDbg( BRLL_RULE, "BR_LLSC_4_2 : Bypass - Station Exit Fee (%d) != 0", pData->Tariff.stationExitFee );
        return RULE_RESULT_BYPASSED;
    }

    // Modify Usage Log
    pData->InternalData.IsUsageLogUpdated                           = TRUE;
    pData->InternalData.UsageLogData.transactionType                = MYKI_BR_TRANSACTION_TYPE_WRONG_SIDE_OF_GATE;

    pData->ReturnedData.rejectReason = MYKI_BR_REJECT_REASON_MISSING_TOUCH_ON;
    CsDbg( BRLL_RULE, "BR_LLSC_4_2 : Executed" );
    return RULE_RESULT_EXECUTED;
}

