/**************************************************************************
*   ID :  BR_LLSC_4_5
*    Same Location with Provisional Product
*    1.  TAppControl.ProductInUse
*    2.  TAppTProduct.ControlBitmap
*    3.  TAppTProduct.EndDateTime
*    4.  TAppTProduct.LastUsageEntryPointId
*    5.  TAppTProduct.LastUsageDateTime
*    6.  Dynamic.EntryPointId
*    7.  Dynamic.CurrentDateTime
*    8.  Tariff.ChangeOfMindPeriod
*    9.  Dynamic.LocationDataUnavailable
*
*    1.  The product in use field1 is not 0 (a product is in a scanned-on state).
*    2.  The provisional bit on the product control bitmap2 of the product in use1 is set to 1.
*    3.  The scan-on date/time5 of the product in use1 plus the change of mind period8 is less than the current date/time7.
*    4.  The product expiry3 is greater than or equal to the current date/time7.
*    5.  If the Mode of transport by the current device is Rail
*    a.  The scan-on location3 of the product in use1 is equal to the current location5.
*    6.  Else the scan-on location3 of the product in use1 is equal to the current location5 and the scan on Route/Stop is equal to the current route/stop of the
*    device.
*    7.  The location data is currently available ie Dynamic.LocationDataUnavailable is false.
*
*
*      Description
*    1.  Perform a Scan-Off.
*      Post-Conditions
*    1.  A scan-off is performed.
*      Devices
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

RuleResult_e BR_LLSC_4_5( MYKI_BR_ContextData_t *pData )
{
    MYKI_TAControl_t    *pMYKI_TAControl;
    MYKI_Directory_t    *pDirectory;
    MYKI_TAProduct_t    *pProduct;

    CsDbg( BRLL_RULE, "BR_LLSC_4_5 : Start (Same Location with Provisional Product)" );

    if ( ! pData )
    {
        CsErrx( "BR_LLSC_4_5 : Invalid argument(s)" );
        return RULE_RESULT_ERROR;
    }

    if ( MYKI_CS_TAControlGet( &pMYKI_TAControl ) < 0 )
    {
        CsErrx( "BR_LLSC_4_5 : MYKI_CS_TAControlGet() failed" );
        return RULE_RESULT_ERROR;
    }

    //  1. The product in use field1 is not 0 (a product is in a scanned-on state).

    if ( pMYKI_TAControl->ProductInUse == 0 )
    {
        CsDbg( BRLL_RULE, "BR_LLSC_4_5 : Bypass - No products in use" );
        return RULE_RESULT_BYPASSED;
    }

    //  Get a pointer to the product in use

    if ( myki_br_GetCardProduct( pMYKI_TAControl->ProductInUse, &pDirectory, &pProduct ) < 0 )
    {
        CsErrx( "BR_LLSC_4_5 : myki_br_GetCardProduct( %d ) failed", pMYKI_TAControl->ProductInUse );
        return RULE_RESULT_ERROR;
    }

    //  2. The provisional bit on the product control bitmap2 of the product in use1 is set to 1.

    if ( ! ( pProduct->ControlBitmap & TAPP_TPRODUCT_CONTROL_PROVISIONAL_BITMAP ) )
    {
        CsDbg( BRLL_RULE, "BR_LLSC_4_5 : Bypass - Product Provisional Bit not set" );
        return RULE_RESULT_BYPASSED;
    }

    //  3. The scan-on date/time5 of the product in use1 plus the change of mind period8 is less than the current date/time7.

    if ( pProduct->LastUsage.DateTime + pData->Tariff.changeOfMindPeriod >= pData->DynamicData.currentDateTime )
    {
        CsDbg( BRLL_RULE, "BR_LLSC_4_5 : Bypass - Scan-on date/time (%d) + Change of mind period (%d) >= Current date/time (%d)", pProduct->LastUsage.DateTime, pData->Tariff.changeOfMindPeriod, pData->DynamicData.currentDateTime );
        return RULE_RESULT_BYPASSED;
    }

    //  4. The product expiry3 is greater than or equal to the current date/time7.

    if ( pProduct->EndDateTime < pData->DynamicData.currentDateTime )
    {
        CsDbg( BRLL_RULE, "BR_LLSC_4_5 : Bypass - Expiry date/time (%d) < Current date/time (%d)", pProduct->EndDateTime, pData->DynamicData.currentDateTime );
        return RULE_RESULT_BYPASSED;
    }

    //  4. If the Mode of transport by the current device is Rail
    //  a. The scan-on location3 of the product in use1 is equal to the current location5.
    //  5. Else the scan-on location3 of the product in use1 is equal to the current location5 and the scan on Route/Stop3 is equal to the current route/stop of the
    //  device.

    if ( ! myki_br_CurrentLocationIsScanOnLocation( pData, pProduct ) )
    {
        CsDbg( BRLL_RULE, "BR_LLSC_4_5 : Bypass - Scan-on location != Current location" );
        return RULE_RESULT_BYPASSED;
    }

    //  7. The location data is currently available ie Dynamic.LocationDataUnavailable is false.

    if ( pData->DynamicData.locationDataUnavailable )
    {
        CsDbg( BRLL_RULE, "BR_LLSC_4_5 : Bypass - Location data is not available" );
        return RULE_RESULT_BYPASSED;
    }

    CsDbg( BRLL_RULE, "BR_LLSC_4_5 : Executed" );
    return RULE_RESULT_EXECUTED;
}

