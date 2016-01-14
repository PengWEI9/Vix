/**************************************************************************
*   ID :  BR_LLSC_6_21
*
*    Location Data Unavailable at scan off
*
*    1.  TAppControl.ProductInUse
*    2.  Dynamic.CurrentTripZoneLow
*    3.  Dynamic.CurrentTripZoneHigh
*    4.  TAppTProduct.LastUsageRouteID
*    5.  TAppTproduct.LastUsageEntryPoint
*    6.  TAppTproduct.EndDateTime
*    7.  Dynamic.CurrentDateTime
*    8.  Dynamic.CurrentTripIsThinZone
*    9.  Dynamic.EntryPointID
*    10.  Static.TransportMode
*    11.  Dynamic.FareRouteID
*    12.  Dynamic.TransportLocations
*    13.  Dynamic.CurrentTripDirection
*    14.  TAppTproduct.LastUseageServiceProvider
*    15.  Dynamic.IsEarlyBirdTrip
*    16.  Dynamic.LocationDataUnavailable
*    17.  Dynamic.IsForcedScanOff
*
*
*   Pre-Conditions
*    1.  The product in use field(1) is not 0 (a product is in a scanned-on state).
*    2.  The current mode of transport(10) is not Rail
*    3.  The location data for current route/stop is currently not available (ie Dynamic.LocationDataUnavailable(16)
*        is true)
*    4.  The entry point(9) and service provider on the product in use is equal to the current service provider &
*        entry point (ie this is the same vehicle)
*    5.  The provisional fare is not equal to $0.00
*    6.  This is not a force scan off(17)
*    7.  The expiry time(6) of the product in use is less than the current date time(7) (ie the product has not expired)
*
*    Description
*    1.  Set IsEarlyBirdTrip(15) to false
*    2.  Set the FareRouteID(11) to none - no route will be considered for pricing
*    3.  Using the provisional product; determine the origin location; append the origin location to the list of
*        transport locations(12) twice. (As we will only charge from Scan on location to Scan on location)
*    4.  Set the Current Trip Direction(13) to Unknown
*
*
*    Notes
*    In this scenario we will charge from touch
*    on location to the touch on location. This
*    will ensure that he is not worse off.
*
*    Post-Conditions
*    1.  The locations for use in zone range determination have been appended to the list of locations
*
*    Devices
*    Fare payment devices
*
 ***********************************************************************/

#include <cs.h>                 // For debug logging
#include <myki_cardservices.h>  // Myki Card Services
#include <LDT.h>                // Logical Device Transaction

#include "myki_br_rules.h"      // API for all BR_xxx modules
#include "BR_Common.h"          // Common utilities, macros, etc

//=============================================================================
//
//  Notes :
//  -   Pre-conditions have been re-arranged for greater efficiency.
//  -   Bypass codes used in testing reflect the original pre-condition number.
//
//=============================================================================

#define RULE_NAME       "BR_LLSC_6_21"
#define RULE_PREFIX     RULE_NAME " : "

#define PRE_CONDITION( number, condition, ... )                                             \
    if ( ! (condition) )                                                                    \
    {                                                                                       \
        CsDbg( BRLL_RULE, RULE_PREFIX "Bypass - (" #condition ") NOT TRUE" __VA_ARGS__ );   \
        pData->ReturnedData.bypassCode = number;                                            \
        return RULE_RESULT_BYPASSED;                                                        \
    }

RuleResult_e BR_LLSC_6_21( MYKI_BR_ContextData_t *pData )
{
    MYKI_TAControl_t    *pMYKI_TAControl;
    MYKI_TAProduct_t    *pProductInUse      = NULL;
    MYKI_Directory_t    *pDirectoryInUse    = NULL;
    MYKI_CD_RouteStop_t *pRouteStops        = 0;
    MYKI_CD_Locations_t location;

    CsDbg( BRLL_RULE, RULE_PREFIX "Start (Location Data Unavailable at scan off)" );

    if ( ! pData )
    {
        CsErrx( RULE_PREFIX "Invalid argument(s)" );
        return RULE_RESULT_ERROR;
    }

    //=============================================================================
    //
    //  Pre-conditions
    //
    //=============================================================================

    // 2.  The current mode of transport22 of the current device is not equal to Rail

    PRE_CONDITION( 2, pData->InternalData.TransportMode != TRANSPORT_MODE_RAIL, ", Transport Mode = %d", pData->InternalData.TransportMode )

    // 3.  The location data for current route/stop is currently not available (ie Dynamic.LocationDataUnavailable(16) is true)

    PRE_CONDITION( 3, pData->DynamicData.locationDataUnavailable )

    // 6.  This is not a force scan off(17)

    PRE_CONDITION( 6, ! pData->DynamicData.isForcedScanOff )

    // 1.  The product in use field1 is not 0 (a product is in a scanned-on state).

    if ( MYKI_CS_TAControlGet( &pMYKI_TAControl ) < 0 )
    {
        CsErrx( RULE_PREFIX "MYKI_CS_TAControlGet() failed" );
        return RULE_RESULT_ERROR;
    }

    PRE_CONDITION( 1, pMYKI_TAControl->ProductInUse != 0, ", pMYKI_TAControl->ProductInUse = %d", pMYKI_TAControl->ProductInUse )

    // 5.  The provisional fare is not equal to $0.00

    PRE_CONDITION( 5, pMYKI_TAControl->ProvisionalFare != 0, ", pMYKI_TAControl->ProvisionalFare = %d", pMYKI_TAControl->ProvisionalFare )

    // 4.  The entry point(9) and service provider on the product in use is equal to the current service provider &
    //     entry point (ie this is the same vehicle)

    if ( myki_br_GetCardProduct( pMYKI_TAControl->ProductInUse, &pDirectoryInUse, &pProductInUse ) < 0 )
    {
        CsErrx( RULE_PREFIX "myki_br_GetCardProduct(%d) failed", pMYKI_TAControl->ProductInUse );
        return RULE_RESULT_ERROR;
    }

    PRE_CONDITION
    (
        4,
        ( pProductInUse->LastUsage.Location.EntryPointId == pData->DynamicData.entryPointId ) && ( pProductInUse->LastUsage.ProviderId == pData->StaticData.serviceProviderId ),
        ", pProductInUse->LastUsage.Location.EntryPointId (%d) != pData->DynamicData.entryPointId (%d) OR pProductInUse->LastUsage.ProviderId (%d) != pData->StaticData.serviceProviderId (%d)",
        pProductInUse->LastUsage.Location.EntryPointId, pData->DynamicData.entryPointId, pProductInUse->LastUsage.ProviderId, pData->StaticData.serviceProviderId
    )

    // 7.  The expiry time(6) of the product in use is less than the current date time(7) (ie the product has not expired)
    //      Note :  Above statement is self-contradicting - expiry less than current implies the product has expired.
    //              Interpreted as "product is not expired and therefore expiry time is *greater than or equal to* current time.

    PRE_CONDITION
    (
        7,
        pProductInUse->EndDateTime >= pData->DynamicData.currentDateTime,
        ", pProductInUse->EndDateTime (%d) < pData->DynamicData.currentDateTime (%d)",
        pProductInUse->EndDateTime, pData->DynamicData.currentDateTime
    )

    //  All preconditions satisfied - indicate this fact to the automated unit test

    pData->ReturnedData.bypassCode = 0;

    //=============================================================================
    //
    //  Description
    //
    //=============================================================================

    // 1.   Set IsEarlyBirdTrip21 to false

    pData->DynamicData.isEarlyBirdTrip = FALSE;

    // 2.   Set the Fare Route ID23 to none - no route will be used for pricing

    pData->DynamicData.fareRouteIdIsValid = FALSE;

    // 3.  Using the provisional product; determine the origin location; append the origin location to the list of
    //     transport locations(12) twice. (As we will only charge from Scan on location to Scan on location)

    if ( MYKI_CD_getRouteStopsStructure( pProductInUse->LastUsage.Location.RouteId, pProductInUse->LastUsage.Location.StopId, pProductInUse->LastUsage.Location.StopId, &pRouteStops ) != 1 )
    {
        CsErrx( RULE_PREFIX "Stop ID %d is not on route %d", pProductInUse->LastUsage.Location.RouteId, pProductInUse->LastUsage.Location.StopId );
        return RULE_RESULT_ERROR;
    }

    if ( ! MYKI_CD_getLocationsStructure( pRouteStops[ 0 ].locationId, &location ) )
    {
        CsErrx( RULE_PREFIX "Failed to get location data from CD for location ID %d", pRouteStops[ 0 ].locationId );
        return RULE_RESULT_ERROR;
    }

    if ( myki_br_AddLocationToTransportLocations( pData, &location ) < 0 )
    {
        CsErrx( RULE_PREFIX "Unable to add transport location" );
        return RULE_RESULT_ERROR;
    }

    if ( myki_br_AddLocationToTransportLocations( pData, &location ) < 0 )
    {
        CsErrx( RULE_PREFIX "Unable to add transport location" );
        return RULE_RESULT_ERROR;
    }

    // 4.  Set the Current Trip Direction(13) to Unknown

    pData->DynamicData.currentTripDirection = TRIP_DIRECTION_UNKNOWN;

    CsDbg( BRLL_RULE, RULE_PREFIX "Executed" );
    return RULE_RESULT_EXECUTED;
}

