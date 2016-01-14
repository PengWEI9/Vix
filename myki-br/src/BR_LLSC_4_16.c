/**************************************************************************
*   ID :  BR_LLSC_4_16
*    Wrong entry at gate, Missing Rail Scan-Off Zone 0/1
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
*    13.  TAppTProduct.LastUsageEntryPointId
*    14.  Dynamic.EntryPointId
*
*   Pre-Conditions
*    1.  The product in use field(1)  is not 0 (a product is in a scanned-on state).
*    2.  The RouteChangeover field(10)  indicates no route changeover condition.
*    3.  The mode of transport as determined by the scan-on service provider(5)  of the product in use(1)  is equal to Rail
*    4.  The mode of transport of the device(6)  is equal to Rail
*    5.  The entry point of the product in use(1)  is not equal to the entry point(13)  of the current device(14) . (ie its not the same station)
*    6.  The zone of origin(3)  and/or the current zone(7)  is physical zone 0 or 1.
*    7.  The product expiry(2)  of the product in use(1)  is greater than or equal to the current date/time(8)  (the product has not expired).
*    8.  The provisional capping option is not equal to `Option 3`
*
*   Description
*    1.  Set the FareRouteID(12)  to none - no route will be considered for pricing
*    2.  Perform a Scan-Off.
*
*   Post-Conditions
*    1.  A scan-off is performed.
*
*   Devices
*    Entry only side of gate.
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

RuleResult_e BR_LLSC_4_16( MYKI_BR_ContextData_t *pData )
{
    MYKI_TAControl_t    *pMYKI_TAControl;
    MYKI_Directory_t    *pDirectory;
    MYKI_TAProduct_t    *pProduct;

    CsDbg( BRLL_RULE, "BR_LLSC_4_16 : Start (Wrong entry at gate, Missing Rail Scan-Off Zone 0/1)" );

    if ( ! pData )
    {
        CsErrx( "BR_LLSC_4_16 : Invalid argument(s)" );
        return RULE_RESULT_ERROR;
    }

    //  1.  The product in use field(1)  is not 0 (a product is in a scanned-on state).

    if ( MYKI_CS_TAControlGet( &pMYKI_TAControl ) )
    {
        CsErrx( "BR_LLSC_4_16 : MYKI_CS_TAControlGet() failed" );
        return RULE_RESULT_ERROR;
    }

    if ( pMYKI_TAControl->ProductInUse == 0 )
    {
        CsDbg( BRLL_RULE, "BR_LLSC_4_16 : Bypass - No product in use" );
        return RULE_RESULT_BYPASSED;
    }

    //  2.  The RouteChangeover field(10)  indicates no route changeover condition.

    if ( pData->DynamicData.routeChangeover != ROUTE_CHANGEOVER_NONE )
    {
        CsDbg( BRLL_RULE, "BR_LLSC_4_16 : Bypass - Route Changeover condition (%d) is not NONE", pData->DynamicData.routeChangeover );
        return RULE_RESULT_BYPASSED;
    }

    //  3.  The mode of transport as determined by the scan-on service provider(5)  of the product in use(1)  is equal to Rail

    if ( myki_br_GetCardProduct( pMYKI_TAControl->ProductInUse, &pDirectory, &pProduct ) < 0 )
    {
        CsErrx( "BR_LLSC_4_16 : myki_br_GetCardProduct( %d ) failed", pMYKI_TAControl->ProductInUse );
        return RULE_RESULT_ERROR;
    }

    if ( myki_br_cd_GetTransportModeForProvider( pProduct->LastUsage.ProviderId ) != TRANSPORT_MODE_RAIL )
    {
        CsDbg( BRLL_CHOICE, "BR_LLSC_4_16 : Bypass - Scan on transport mode (%d) is not RAIL", myki_br_cd_GetTransportModeForProvider( pProduct->LastUsage.ProviderId ) );
        return RULE_RESULT_BYPASSED;
    }

    //  4.  The mode of transport of the device(6)  is equal to Rail

    if ( pData->InternalData.TransportMode != TRANSPORT_MODE_RAIL )
    {
        CsDbg( BRLL_CHOICE, "BR_LLSC_4_16 : Bypass - Device transport mode (%d) is not RAIL", pData->InternalData.TransportMode );
        return RULE_RESULT_BYPASSED;
    }

    //  5.  The entry point of the product in use(1) is not equal to the entry point(13)  of the current device(14) . (ie its not the same station)
    //      BR says check "entry point ID" only, but as the device is known to be RAIL at this point, this check is the same thing

    if ( myki_br_CurrentLocationIsScanOnLocation( pData, pProduct ) )
    {
        CsDbg( BRLL_RULE, "BR_LLSC_4_16 : Bypass - Scan-on location is Current location" );
        return RULE_RESULT_BYPASSED;
    }

    //  6.  The zone of origin(3) and/or the current zone(7) is physical zone 0 or 1.
    //
    //  Clarification from Brendon: This is a logical 'OR'. Either condition being true will
    //  satisfy the precondition.

    if ( !(myki_br_isPhysicalZoneOneOrZero(pProduct->LastUsage.Zone) || myki_br_isPhysicalZoneOneOrZero(pData->DynamicData.currentZone)))
    {
        CsDbg( BRLL_RULE, "BR_LLSC_4_16 : Bypass - Neither scan on zone (%d) nor current zone (%d) is 0 or 1", pProduct->LastUsage.Zone, pData->DynamicData.currentZone );
        return RULE_RESULT_BYPASSED;
    }

    //  7.  The product expiry(2) of the product in use(1) is greater than or equal to the current date/time(8)  (the product has not expired).

    if ( pProduct->EndDateTime < pData->DynamicData.currentDateTime )
    {
        CsDbg( BRLL_RULE, "BR_LLSC_4_16 : Bypass - Product expired" );
        return RULE_RESULT_BYPASSED;
    }

    //  8.  The provisional capping option is not equal to `Option 3`
    if ( pData->Tariff.provisionalCappingOption == PROVISIONAL_CAPPING_OPTION_3 )
    {
        CsDbg( BRLL_RULE, "BR_LLSC_4_16 : Bypass - Provisional Capping option == 3" );
        return RULE_RESULT_BYPASSED;
    }

    //  Description
    //  1.  Set the FareRouteID(12)  to none - no route will be considered for pricing

    pData->DynamicData.fareRouteIdIsValid = FALSE;

    //  2.  Perform a Scan-Off.
    //
    // Implementation Note:
    //      Although NTS0177 V7.2 says to perform a scan-off, the current thinking is that this is
    //      more a note of what will happen than an action to be performed as a part of this rule.
    //
    //      This rule is just a part of a complete engine. The actual scan-off is done elsewhere.

    CsDbg( BRLL_RULE, "BR_LLSC_4_16 : Executed" );
    return RULE_RESULT_EXECUTED;
}
