/**************************************************************************
*   ID :  BR_LLSC_4_8
*    Missing Rail Scan-Off Zone 0/1
*    1.  TAppControl.ProductInUse
*    2.  TAppTProduct.EndDateTime
*    3.  TAppTProduct.LastUsageZone
*    4.  TAppTProduct.LastUsageLineId
*    5.  TAppTProduct.LastUsageProviderId
*    6.  Static.TransportMode
*    7.  Dynamic.CurrentZone
*    8.  Dynamic.CurrentDateTime
*    9.  Dynamic.LineId
*    10.  Dynamic.RouteChangeover
*    11.  Tariff.ProvisionalCappingOption
*    12.  Dynamic.FareRouteID
*
*    1.  The product in use field1 is not 0 (a product is in a scanned-on state).
*    2.  The RouteChangeover field10 indicates no route changeover condition.
*    3.  The mode of transport as determined by the scan-on service provider5 of the product in use1 is equal to rail
*    4.  The mode of transport of the device6 is not equal to rail
*    5.  The zone of origin3 and/or the current zone7 is physical zone 0 or 1.
*    6.  The provisional capping option is not equal to `Option 3`
*
*      Description
*    1.  Set the FareRouteID12 to none - no route will be considered for pricing
*    2.  Perform a Scan-Off.
*      Post-Conditions
*    1.  A scan-off is performed.
*      Devices
*    Fare payment devices
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
//  Moved pre-condition 4 to position 1 because otherwise routeChangeover can
//  be used before it's set (BR_LLSC_4_11 bypasses because of RAIL check).
//
//=============================================================================

RuleResult_e BR_LLSC_4_8( MYKI_BR_ContextData_t *pData )
{
    MYKI_TAControl_t    *pMYKI_TAControl;
    MYKI_Directory_t    *pDirectory;
    MYKI_TAProduct_t    *pProduct;

    CsDbg( BRLL_RULE, "BR_LLSC_4_8 : Start (Missing Rail Scan-Off Zone 0/1)" );

    if ( ! pData )
    {
        CsErrx( "BR_LLSC_4_8 : Invalid argument(s)" );
        return RULE_RESULT_ERROR;
    }

    //  4.  The mode of transport of the device6 is not equal to rail

    if ( pData->InternalData.TransportMode == TRANSPORT_MODE_RAIL )
    {
        CsDbg( BRLL_RULE, "BR_LLSC_4_8 : Bypass - Device Transport Mode == RAIL" );
        return RULE_RESULT_BYPASSED;
    }

    //  1. The product in use field1 is not 0 (a product is in a scanned-on state).

    if ( MYKI_CS_TAControlGet( &pMYKI_TAControl ) < 0 )
    {
        CsErrx( "BR_LLSC_4_8 : MYKI_CS_TAControlGet() failed" );
        return RULE_RESULT_ERROR;
    }

    if ( pMYKI_TAControl->ProductInUse == 0 )
    {
        CsDbg( BRLL_RULE, "BR_LLSC_4_8 : Bypass - No products in use" );
        return RULE_RESULT_BYPASSED;
    }

    //  2.  The RouteChangeover field10 indicates no route changeover condition.

    if ( pData->DynamicData.routeChangeover != ROUTE_CHANGEOVER_NONE )
    {
        CsDbg( BRLL_RULE, "BR_LLSC_4_8 : Bypass - Route Changeover condition is not NONE" );
        return RULE_RESULT_BYPASSED;
    }

    //  Get a pointer to the product in use

    if ( myki_br_GetCardProduct( pMYKI_TAControl->ProductInUse, &pDirectory, &pProduct ) < 0 )
    {
        CsErrx( "BR_LLSC_4_8 : myki_br_GetCardProduct( %d ) failed", pMYKI_TAControl->ProductInUse );
        return RULE_RESULT_ERROR;
    }

    //  3.  The mode of transport as determined by the scan-on service provider5 of the product in use1 is equal to rail

    if ( myki_br_cd_GetTransportModeForProvider( pProduct->LastUsage.ProviderId ) != TRANSPORT_MODE_RAIL )
    {
        CsDbg( BRLL_RULE, "BR_LLSC_4_8 : Bypass - Scan-on Transport Mode != RAIL" );
        return RULE_RESULT_BYPASSED;
    }

    //  5.  The zone of origin3 and/or the current zone7 is physical zone 0 or 1.

    if ( !(myki_br_isPhysicalZoneOneOrZero(pProduct->LastUsage.Zone) || myki_br_isPhysicalZoneOneOrZero(pData->DynamicData.currentZone)))
    {
        CsDbg( BRLL_RULE, "BR_LLSC_4_8 : Bypass - Neither Origin zone (%d) nor current zone (%d) is 0 or 1", pProduct->LastUsage.Zone, pData->DynamicData.currentZone );
        return RULE_RESULT_BYPASSED;
    }

    //  6.  The provisional capping option is not equal to `Option 3`

    if ( pData->Tariff.provisionalCappingOption == PROVISIONAL_CAPPING_OPTION_3 )
    {
        CsDbg( BRLL_RULE, "BR_LLSC_4_8 : Bypass - Provisional Capping Option = 3" );
        return RULE_RESULT_BYPASSED;
    }

    pData->DynamicData.fareRouteIdIsValid = FALSE;      // 1. Set the FareRouteID7 to none - no route will be considered for pricing

    CsDbg( BRLL_RULE, "BR_LLSC_4_8 : Executed" );
    return RULE_RESULT_EXECUTED;
}

