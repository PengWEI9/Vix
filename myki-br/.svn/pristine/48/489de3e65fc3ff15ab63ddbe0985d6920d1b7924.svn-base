/**************************************************************************
*   ID :  BR_LLSC_4_18
*    Wrong entry at gate, Missing Rail Scan-Off - Not Zone 0/1
*    1.  TAppControl.ProductInUse
*    2.  TAppTProduct.LastUsageZone
*    3.  TAppTProduct.LastUsageProviderId
*    4.  Static.TransportMode
*    5.  Dynamic.CurrentZone
*    6.  TAppTProduct.EndDateTime
*    7.  Dynamic.FareRouteID
*    1.  The product in use field1 is not 0 (a product is in a scanned-on state).
*    2.  The RouteChangeover field10 indicates no route changeover condition.
*    3.  The mode of transport as determined by the scan-on service provider5 of the product in use1 is equal to Rail
*    4.  The mode of transport of the device6 is equal to Rail
*    5.  The entry point of the product in use1 is not equal to the entry point13 of the current device14. (ie its not the same station)
*    6.  Both the zone of origin3 and the current zone7 are not physical zone 0 or 1.
*    7.  The product expiry2 of the product in use1 is greater than or equal to the current date/time8 (the product has not expired).
*    8.  The provisional capping option is not equal to `Option 3`
*      Description
*    1.  Perform a Forced Scan-Off.
*      Post-Conditions
*    1.  A forced scan-off is performed.
*      Devices
*    Entry only side of gate
*
*
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

RuleResult_e BR_LLSC_4_18( MYKI_BR_ContextData_t *pData )
{
    MYKI_TAControl_t    *pMYKI_TAControl;
    MYKI_Directory_t    *pDirectory;
    MYKI_TAProduct_t    *pProduct;

    CsDbg( BRLL_RULE, "BR_LLSC_4_18 : Start (Wrong entry at gate, Missing Rail Scan- Not Zone 0/1)" );

    if ( ! pData )
    {
        CsErrx( "BR_LLSC_4_18 : Invalid argument(s)" );
        return RULE_RESULT_ERROR;
    }

    if ( MYKI_CS_TAControlGet( &pMYKI_TAControl ) )
    {
        CsErrx( "BR_LLSC_4_18 : MYKI_CS_TAControlGet() failed" );
        return RULE_RESULT_ERROR;
    }

    //  1.  The product in use field1 is not 0 (a product is in a scanned-on state).
    if ( pMYKI_TAControl->ProductInUse == 0 )
    {
        CsDbg( BRLL_RULE, "BR_LLSC_4_18 : Bypass - No product in use" );
        return RULE_RESULT_BYPASSED;
    }

    if ( pData->DynamicData.routeChangeover != ROUTE_CHANGEOVER_NONE )
    {
        CsDbg( BRLL_RULE, "BR_LLSC_4_18 : Bypass - Route Changeover condition (%d) is not NONE", pData->DynamicData.routeChangeover );
        return RULE_RESULT_BYPASSED;
    }

    if ( myki_br_GetCardProduct( pMYKI_TAControl->ProductInUse, &pDirectory, &pProduct ) < 0 )
    {
        CsErrx( "BR_LLSC_4_18 : myki_br_GetCardProduct( %d ) failed", pMYKI_TAControl->ProductInUse );
        return RULE_RESULT_ERROR;
    }

    if ( myki_br_cd_GetTransportModeForProvider( pProduct->LastUsage.ProviderId ) != TRANSPORT_MODE_RAIL )
    {
        CsDbg( BRLL_CHOICE, "BR_LLSC_4_18 : Bypass - Scan on transport mode (%d) is not RAIL", myki_br_cd_GetTransportModeForProvider( pData->StaticData.serviceProviderId ) );
        return RULE_RESULT_BYPASSED;
    }

    if ( pData->InternalData.TransportMode != TRANSPORT_MODE_RAIL )
    {
        CsDbg( BRLL_CHOICE, "BR_LLSC_4_18 : Bypass - Device transport mode (%d) is not RAIL", myki_br_cd_GetTransportModeForProvider( pData->StaticData.serviceProviderId ) );
        return RULE_RESULT_BYPASSED;
    }

    if ( myki_br_CurrentLocationIsScanOnLocation( pData, pProduct ) )    // BR says check "entry point ID" only, but as the device is known to be RAIL at this point, this check is the same thing
    {
        CsDbg( BRLL_RULE, "BR_LLSC_4_18 : Bypass - Scan-on location is Current location" );
        return RULE_RESULT_BYPASSED;
    }

    //  6.  Both the zone of origin(3) and the current zone(7) are not physical zone 0 or 1
    if ( myki_br_isPhysicalZoneOneOrZero(pProduct->LastUsage.Zone) || myki_br_isPhysicalZoneOneOrZero(pData->DynamicData.currentZone) )
    {
        CsDbg( BRLL_RULE, "BR_LLSC_4_18 : Bypass - Either scan on zone (%d) or current zone (%d) is 0 or 1", pProduct->LastUsage.Zone, pData->DynamicData.currentZone );
        return RULE_RESULT_BYPASSED;
    }

    if ( pProduct->EndDateTime < pData->DynamicData.currentDateTime )
    {
        CsDbg( BRLL_RULE, "BR_LLSC_4_18 : Bypass - Product expired" );
        return RULE_RESULT_BYPASSED;
    }

    if ( pData->Tariff.provisionalCappingOption == PROVISIONAL_CAPPING_OPTION_3 )
    {
        CsDbg( BRLL_RULE, "BR_LLSC_4_18 : Bypass - Provisional Capping option == 3" );
        return RULE_RESULT_BYPASSED;
    }

    CsDbg( BRLL_RULE, "BR_LLSC_4_18 : Executed" );
    return RULE_RESULT_EXECUTED;
}

