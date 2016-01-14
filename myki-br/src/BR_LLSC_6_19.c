/**************************************************************************
*   ID :  BR_LLSC_6_19
*
*    Resolve Trip Locations Different mode of Travel
*
*    1.  TAppControl.ProductInUse
*    2.  TAppTProduct.LastUsageStopId
*    3.  Dynamic.StopId
*    4.  Dynamic.CurrentTripZoneLow
*    5.  Dynamic.CurrentTripZoneHigh
*    6.  TAppTProduct.LastUsageLineId
*    7.  Dynamic.LineId
*    8.  Dynamic.CurrentDateTime
*    9.  TAppControl.PassengerCode
*    10.  Tariff.HeadlessRouteId
*    11.  Dynamic.CurrentTripIsThinZone
*    12.  Dynamic.CurrentTripDirection
*    13.  Dynamic.RouteChangeoverStatus
*    14.  Dynamic.TraverseRouteID
*    15.  Dynamic.TraverseStopID
*    16.  Dynamic DestinationStopID
*    17.  Dynamic.OriginRouteID
*    18.  Dynamic.OriginStopID
*    19.  Dynamic.TraverseDestinationStopID
*    20.  Dynamic.DestinationRouteID
*    21.  Dynamic.IsEarlyBirdTrip
*    22.  Static.ModeofTransport
*    23.  Dynamic.FareRouteID
*    24.  Dynamic.TransportLocations
*
*
*   Pre-Conditions
*    1.  The product in use field(1) is not 0 (a product is in a scanned-on state).
*    2.  The current mode of transport(22) of the current device is not equal to Rail
*    3.  The mode of transport of the scan on service provider is equal Rail
*
*    Description
*    1.  Set the Fare Route ID(23) to none - no route will be used for pricing
*    2.  Set IsEarlyBirdTrip(21) to false
*    3.  Determine the Origin Zone and Destination Zone based on the StationID and
*        DestinationRouteId/DestinationStopId.
*    4.  Set the Current Trip Direction(12) to Unknown.
*    5.  if the OriginZone is greater than the Destination zone then:
*    a.  Set the current trip direction(12) to Inbound
*    6.  else if the OriginZone is less than DestinationZone then:
*    a.  set the current trip direction(12) to Outbound
*    7.  Using the scan on location and the current location(3), append the locations to list of transport
*        locations.
*
*    Post-Conditions
*    1.  The list of locations used to determine the trip zone range has been populated
*
*    Devices
*    Fare payment devices
*
 ***********************************************************************/

//=============================================================================
//
//  Set Early Bird Trip, Fare Route, Trip Direction and Location List
//  for Rail to Non-Rail journey leg.
//
//  If the scan-on location was Rail and the scan-off (ie current) location is not Rail, then
//  -   Populate the list of traversed locations with the scan-on station and
//      the scan-off route/stop.
//  -   Set the trip direction based on the scan-on and scan-off zones.
//  -   Clear "Early Bird Trip" flag
//  -   Clear Fare Route
//
//=============================================================================

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

RuleResult_e BR_LLSC_6_19( MYKI_BR_ContextData_t *pData )
{
    MYKI_TAControl_t    *pMYKI_TAControl;
    MYKI_TAProduct_t    *pPdtInUse = NULL;
    MYKI_CD_Stations_t  station;            // Station details - filled in by myki_br_GetStationDetails() but not used here.
    MYKI_CD_Locations_t location;
    int                 origZone;
    int                 destZone;
    MYKI_CD_RouteStop_t* pRouteStops = 0;

    CsDbg( BRLL_RULE, "BR_LLSC_6_19 : Start (Resolve Trip Locations Different mode of Travel)" );

    if ( ! pData )
    {
        CsErrx( "BR_LLSC_6_19 : Invalid argument(s)" );
        return RULE_RESULT_ERROR;
    }

    /*  Data Fields
        1.  TAppControl.ProductInUse
        2.  TAppTProduct.LastUsageStopId
        3.  Dynamic.StopId
        4.  Dynamic.CurrentTripZoneLow
        5.  Dynamic.CurrentTripZoneHigh
        6.  TAppTProduct.LastUsageLineId
        7.  Dynamic.LineId
        8.  Dynamic.CurrentDateTime
        9.  TAppControl.PassengerCode
        10. Tariff.HeadlessRouteId
        11. Dynamic.CurrentTripIsThinZone
        12. Dynamic.CurrentTripDirection
        13. Dynamic.RouteChangeoverStatus
        14. Dynamic.TraverseRouteID
        15. Dynamic.TraverseStopID
        16. Dynamic DestinationStopID
        17. Dynamic.OriginRouteID
        18. Dynamic.OriginStopID
        19. Dynamic.TraverseDestinationStopID
        20. Dynamic.DestinationRouteID
        21. Dynamic.IsEarlyBirdTrip
        22. Static.ModeofTransport
        23. Dynamic.FareRouteID
        24. Dynamic.TransportLocations
    */

    /* Pre-Conditions */

    // 2.  The current mode of transport22 of the current device is not equal to Rail
    //      Put this pre-condition first for efficiency

    if ( pData->InternalData.TransportMode == TRANSPORT_MODE_RAIL )
    {
        CsDbg( BRLL_RULE, "BR_LLSC_6_19 : Bypass - Device Transport Mode is RAIL" );
        pData->ReturnedData.bypassCode = 1;
        return RULE_RESULT_BYPASSED;
    }

    // 1.  The product in use field1 is not 0 (a product is in a scanned-on state).

    if ( MYKI_CS_TAControlGet( &pMYKI_TAControl ) < 0 )
    {
        CsErrx( "BR_LLSC_6_19 : MYKI_CS_TAControlGet() failed" );
        return RULE_RESULT_ERROR;
    }

    if ( pMYKI_TAControl->ProductInUse == 0 )
    {
        CsDbg( BRLL_RULE, "BR_LLSC_6_19 : Bypass : ProductInUse is zero");
        pData->ReturnedData.bypassCode = 2;
        return RULE_RESULT_BYPASSED;
    }

    // 3.   The mode of transport of the scan on service provider is equal Rail

    /* Get product and directory structure in use */
    if ( MYKI_CS_TAProductGet( (U8_t)( pMYKI_TAControl->ProductInUse - 1 ), &pPdtInUse ) < 0 )
    {
        CsErrx( "BR_LLSC_6_19 : MYKI_CS_TAProductGet() failed" );
        return RULE_RESULT_ERROR;
    }

    if ( myki_br_cd_GetTransportModeForProvider(pPdtInUse->LastUsage.ProviderId) != TRANSPORT_MODE_RAIL )
    {
        CsDbg( BRLL_RULE, "BR_LLSC_6_19 : Bypass - Last service provider is not RAIL" );
        pData->ReturnedData.bypassCode = 3;
        return RULE_RESULT_BYPASSED;
    }

    //  All preconditions satisfied - indicate this fact to the automated unit test
    pData->ReturnedData.bypassCode = 0;

    /* Description */

    // 1.   Set the Fare Route ID23 to none - no route will be used for pricing

    pData->DynamicData.fareRouteIdIsValid = FALSE;

    // 2.   Set IsEarlyBirdTrip21 to false

    pData->DynamicData.isEarlyBirdTrip = FALSE;

    // 3.   Determine the Origin Zone and Destination Zone based on the StationID and DestinationRouteId/DestinationStopId.
    // 7.   Using the scan on location and the current location3, append the locations to list of transport locations.

    //      Origin (Scan-on)

    if ( myki_br_cd_GetStationDetails( pPdtInUse->LastUsage.Location.EntryPointId, &station, &location ) < 0 )
    {
        CsErrx( "BR_LLSC_6_19 : Failed to get station details" );
        return RULE_RESULT_ERROR;
    }

    if ( myki_br_AddLocationToTransportLocations( pData, &location ) < 0 )
    {
        CsErrx( "BR_LLSC_6_19 : Unable to add transport location" );
        return RULE_RESULT_ERROR;
    }

    origZone = location.zone;

    //      Current (Scan-off)

    if ( MYKI_CD_getRouteStopsStructure( pData->DynamicData.lineId, pData->DynamicData.stopId, pData->DynamicData.stopId, &pRouteStops ) != 1 )
    {
        CsErrx( "BR_LLSC_6_19 : Stop ID %d is not on route %d", pData->DynamicData.stopId, pData->DynamicData.lineId );
        return RULE_RESULT_ERROR;
    }

    if ( ! MYKI_CD_getLocationsStructure( pRouteStops[ 0 ].locationId, &location ) )
    {
        CsErrx( "BR_LLSC_6_19 : Failed to get location data from CD for location ID %d", pRouteStops[ 0 ].locationId );
        return RULE_RESULT_ERROR;
    }

    if ( myki_br_AddLocationToTransportLocations( pData, &location ) < 0 )
    {
        CsErrx( "BR_LLSC_6_19 : Unable to add transport location" );
        return RULE_RESULT_ERROR;
    }

    destZone = location.zone;

    // 4.   Set the Current Trip Direction12 to Unknown.
    // 5.   if the OriginZone is greater than the Destination zone then:
    // a.   Set the current trip direction12 to Inbound
    // 6.   else if the OriginZone is less than DestinationZone then:
    // a.   set the current trip direction12 to Outbound

    myki_br_SetTripDirectionFromZones( pData, origZone, destZone );

    CsDbg( BRLL_RULE, "BR_LLSC_6_19 : Executed" );
    return RULE_RESULT_EXECUTED;
}

