/**************************************************************************
*   ID :  BR_LLSC_4_3
*    Blocking Period
*
* Data-Fields
*    1.  TAppControl.ProductInUse
*    2.  TAppTProduct.ControlBitmap
*    3.  TAppTProduct.LastUsageEntryPointId
*    4.  TAppTProduct.LastUsageDateTime
*    5.  Dynamic.EntryPointId
*    6.  Dynamic.CurrentDateTime
*    7.  Tariff.BlockingPeriod
*
* Pre-Conditions
*    1.  The product in use field(1) is not 0 (a product is in a scanned-on state).
*    2.  The provisional bit on the product control bitmap(2) of the product in use(1) is set to 1.
*    3.  The scan-on date/time(4) of the product in use(1) plus the blocking period(7) is greater than or equal to the current date/time(6).
*    4.  If the Mode of transport by the current device is Rail
*            The scan-on location(3) of the product in use(1) is equal to the current location(5).
*        Else
*            The scan-on location(3) of the product in use(1) is equal to the current location(5)
*            and the scan on Route/Stop is equal to the current route/stop of the device.
*
* Description
*    1.  Reject the smartcard.
*
* Post-Conditions
*    1.  The smartcard is rejected.
*
* Devices
*    Non-directional fare payment devices, exit gates
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

RuleResult_e BR_LLSC_4_3( MYKI_BR_ContextData_t *pData )
{
    MYKI_TAControl_t    *pMYKI_TAControl;
    MYKI_Directory_t    *pDirectory;
    MYKI_TAProduct_t    *pProduct;

    CsDbg( BRLL_RULE, "BR_LLSC_4_3 : Start (Blocking Period scan on)" );

    if ( ! pData )
    {
        CsErrx( "BR_LLSC_4_3 : Invalid argument(s)" );
        return RULE_RESULT_ERROR;
    }

    if ( MYKI_CS_TAControlGet( &pMYKI_TAControl ) < 0 )
    {
        CsErrx( "BR_LLSC_4_3 : MYKI_CS_TAControlGet() failed" );
        return RULE_RESULT_ERROR;
    }

    //  1. The product in use field1 is not 0 (a product is in a scanned-on state).

    if ( pMYKI_TAControl->ProductInUse == 0 )
    {
        CsDbg( BRLL_RULE, "BR_LLSC_4_3 : Bypass - No products in use" );
        return RULE_RESULT_BYPASSED;
    }

    //  Get a pointer to the product in use

    if ( myki_br_GetCardProduct( pMYKI_TAControl->ProductInUse, &pDirectory, &pProduct ) < 0 )
    {
        CsErrx( "BR_LLSC_4_3 : myki_br_GetCardProduct( %d ) failed", pMYKI_TAControl->ProductInUse );
        return RULE_RESULT_ERROR;
    }

    //  2. The provisional bit on the product control bitmap2 of the product in use1 is set to 1.

    CsDbg( BRLL_FIELD, "BR_LLSC_4_3 : Checking Product %d Provisional (0x%02x)", pMYKI_TAControl->ProductInUse, pProduct->ControlBitmap );

    if ( ! ( pProduct->ControlBitmap & TAPP_TPRODUCT_CONTROL_PROVISIONAL_BITMAP ) )
    {
        CsDbg( BRLL_RULE, "BR_LLSC_4_3 : Bypass - Product Provisional Bit not set" );
        return RULE_RESULT_BYPASSED;
    }

    //  3.  The scan-on date/time(4) of the product in use(1) plus the blocking period(7) is greater than or equal to the current date/time(6).

    CsDbg( BRLL_FIELD, "BR_LLSC_4_3 : Checking Last Usage (%d) + BlockingPeriod (%d) < Current (%d)", pProduct->LastUsage.DateTime, pData->Tariff.blockingPeriod, pData->DynamicData.currentDateTime );

    if ( pProduct->LastUsage.DateTime + pData->Tariff.blockingPeriod < pData->DynamicData.currentDateTime )
    {
        CsDbg(BRLL_RULE,"BR_LLSC_4_3 : Bypass - Blocking Period expired");
        return RULE_RESULT_BYPASSED;
    }

    //  4. If the Mode of transport by the current device is Rail
    //  a. The scan-on location3 of the product in use1 is equal to the current location5.
    //  5. Else the scan-on location3 of the product in use1 is equal to the current location5 and the scan on Route/Stop3 is equal to the current route/stop of the
    //  device.

    if ( ! myki_br_CurrentLocationIsScanOnLocation( pData, pProduct ) )
    {
        CsDbg( BRLL_RULE, "BR_LLSC_4_3 : Bypass - Scan-on location != Current location" );
        return RULE_RESULT_BYPASSED;
    }

    pData->ReturnedData.rejectReason = MYKI_BR_REJECT_REASON_ALREADY_TOUCH_ON;
    CsDbg( BRLL_RULE, "BR_LLSC_4_3 : Executed" );
    return RULE_RESULT_EXECUTED;
}
