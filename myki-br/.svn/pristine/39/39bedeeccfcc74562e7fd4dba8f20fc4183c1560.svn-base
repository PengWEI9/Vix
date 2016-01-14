/* -*- mode: c; tabs: 4; -*- */
/*=============================================================================
**
**    Vix Technology                   Licensed software
**    (C) 2014                         All rights reserved
**
**=============================================================================
**
**  Project/Product : MBU
**  Filename        : BR_LLSC_6_24.c
**  Author(s)       : David Purdie
**
**  ID              :  BR_LLSC_6_24 - NTS0177 v7.3
**
**  Name            : Missing scan off zone 0/1 and current location unavailable
**
**  Data Fields     :
**
**      1.  TAppControl.ProductInUse
**      2.  Dynamic.CurrentTripZoneLow
**      3.  Dynamic.CurrentTripZoneHigh
**      4.  TAppTProduct.LastUsageRouteID
**      5.  TAppTproduct.LastUsageEntryPoint
**      6.  Dynamic.LineId
**      7.  Dynamic.CurrentDateTime
**      8.  Dynamic.CurrentTripIsThinZone
**      9.  Dynamic.EntryPointID
**      10. Static.TransportMode
**      11. Dynamic.FareRouteID
**      12. Dynamic.TransportLocations
**      13. Dynamic.CurrentTripDirection
**      14. TAppTproduct.LastUseageServiceProvider
**      15. Dynamic.IsEarlyBirdTrip
**      16. Dynamic.LocationDataUnavailable
**      17. Dynamic.IsForcedScanOff
**
**  Pre-Conditions  :
**
**      1.  The product in use field(1) is not 0 (a product is in a scanned-on state).
**      2.  The data for current route/stop is currently not available (ie Dynamic.LocationDataUnavailable(16) is True)
**      3.  The current Mode of transport(10) is not RAIL
**      4.  The service provide of the provisional product is a rail service provider
**      5.  The location of the station of scan on was is in zone 0/1
**      6.  This is not a force scan off sequence(17)
**      7.  The provisional product has not expired
**
**  Description     :
**
**      1.  Set IsEarlyBirdTrip(15) to false
**      2.  Set the FareRouteID(11) to none - no route will be considered for pricing
**      3.  Determine the scan on location via the entry point; then append the station location to the list of transport
**          locations(12) twice. (As we will only charge for the zone of the station)
**      4.  Set the Current Trip Direction(13) to Unknown.
** 
**      In this scenario we will only charge for the zone that the original station entry is in.
** 
**  Post-Conditions :
**
**      1.  The locations for use in zone range determination have been appended to the list of locations
**
**  Devices         :
**
**      Fare payment devices
**
**  Member(s)       :
**      BR_LLSC_6_24            [public]    business rule
**
**  Information     :
**   Compiler(s)    : C
**   Target(s)      : Independent
**
**  Subversion      :
**      $Id: $
**      $HeadURL: $
**
**  History         :
**   Vers.  Date        Aut.  Type     Description
**   -----  ----------  ----  -------  ----------------------------------------
**    1.00  ??.07.14    DP    Create
**    1.01  27.08.14    ANT   Modify   Rectified business rule
**
**===========================================================================*/

#include <cs.h>                 // For debug logging
#include <myki_cardservices.h>  // Myki Card Services
#include <LDT.h>                // Logical Device Transaction

#include "myki_br_rules.h"      // API for all BR_xxx modules
#include "BR_Common.h"          // Common utilities, macros, etc

//=============================================================================
//
//  Notes :
//  -   Pre-conditions may be re-arranged for greater efficiency.
//  -   Bypass codes used in testing reflect the original pre-condition number.
//
//=============================================================================

#define RULE_NAME       "BR_LLSC_6_24"
#define RULE_PREFIX     RULE_NAME " : "

#define PRE_CONDITION( number, condition )                                      \
    if ( ! (condition) )                                                        \
    {                                                                           \
        CsDbg( BRLL_RULE, RULE_PREFIX "Bypass - (" #condition ") NOT TRUE" );   \
        pData->ReturnedData.bypassCode = number;                                \
        return RULE_RESULT_BYPASSED;                                            \
    }

/*==========================================================================*
**
**  BR_LLSC_6_24
**
**  Description     :
**      Implements business rule BR_LLSC_6_24.
**
**  Parameters      :
**      pData               [I/O]   BR context data
**
**  Returns         :
**      RULE_RESULT_EXECUTED
**      RULE_RESULT_BYPASSED
**      RULE_RESULT_ERROR
**
**  Notes           :
**
**
**==========================================================================*/

RuleResult_e BR_LLSC_6_24( MYKI_BR_ContextData_t *pData )
{
    MYKI_TAControl_t       *pMYKI_TAControl     = NULL;
    MYKI_TAProduct_t       *pProductInUse       = NULL;
    MYKI_Directory_t       *pDirectoryInUse     = NULL;
    MYKI_CD_Stations_t      station;
    MYKI_CD_Locations_t     location;

    CsDbg( BRLL_RULE, RULE_PREFIX "Start (Missing scan off zone 0/1 and current location unavailable)" );

    if ( ! pData )
    {
        CsErrx( RULE_PREFIX "Invalid argument(s)" );
        return RULE_RESULT_ERROR;
    }

    //  1.  The product in use field(1) is not 0 (a product is in a scanned-on state).

    if ( MYKI_CS_TAControlGet( &pMYKI_TAControl ) < 0 )
    {
        CsErrx( RULE_PREFIX "MYKI_CS_TAControlGet() failed" );
        return RULE_RESULT_ERROR;
    }

    PRE_CONDITION( 1, pMYKI_TAControl->ProductInUse != 0 )

    //  2.  The data for current route/stop is currently not available
    //      (ie Dynamic.LocationDataUnavailable(16) is True)

    PRE_CONDITION( 2, pData->DynamicData.locationDataUnavailable )

    //  3.  The current Mode of transport(10) is not RAIL

    PRE_CONDITION(3, pData->InternalData.TransportMode != TRANSPORT_MODE_RAIL )

    //  4.  The service provider of the provisional product is a rail service provider

    if ( myki_br_GetCardProduct( pMYKI_TAControl->ProductInUse, &pDirectoryInUse, &pProductInUse ) < 0 )
    {
        CsErrx( RULE_PREFIX "myki_br_GetCardProduct(%d) failed", pMYKI_TAControl->ProductInUse );
        return RULE_RESULT_ERROR;
    }

    PRE_CONDITION(4, myki_br_cd_GetTransportModeForProvider( pProductInUse->LastUsage.ProviderId ) == TRANSPORT_MODE_RAIL);

    //  5.  The location of the station of scan on was is in zone 0/1

    PRE_CONDITION(5, myki_br_isPhysicalZoneOneOrZero( pProductInUse->LastUsage.Zone ) == TRUE);

    //  6.  This is not a force scan off sequence(17)

    PRE_CONDITION( 6, ! pData->DynamicData.isForcedScanOff );

    //  7.  The provisional product has not expired

    PRE_CONDITION( 7,  pProductInUse->EndDateTime > pData->DynamicData.currentDateTime );

    //  All preconditions satisfied - indicate this fact to the automated unit test

    pData->ReturnedData.bypassCode = 0;

    //=============================================================================
    //
    //  Description
    //
    //=============================================================================

    //  1.  Set IsEarlyBirdTrip(15) to false

    pData->DynamicData.isEarlyBirdTrip = FALSE;

    //  2.  Set the FareRouteID(11) to none - no route will be considered for pricing

    pData->DynamicData.fareRouteIdIsValid = FALSE;

    //  3.  Determine the scan on location via the entry point; then append the station location to the list of transport
    //      locations(12) twice. (As we will only charge for the zone of the station)

    if ( myki_br_cd_GetStationDetails( pProductInUse->LastUsage.Location.EntryPointId, &station, &location ) < 0 )
    {
        CsErrx( RULE_PREFIX "myki_br_cd_GetStationDetails(%d) failed", pProductInUse->LastUsage.Location.EntryPointId );
        return RULE_RESULT_ERROR;
    }

    if ( myki_br_AddLocationToTransportLocations( pData, &location ) < 0 ||
         myki_br_AddLocationToTransportLocations( pData, &location ) < 0 )
    {
        CsErrx( RULE_PREFIX "Unable to add transport location" );
        return RULE_RESULT_ERROR;
    }

    //  4.  Set the Current Trip Direction(13) to Unknown.

    pData->DynamicData.currentTripDirection = TRIP_DIRECTION_UNKNOWN;

    CsDbg( BRLL_RULE, RULE_PREFIX "Executed" );
    return RULE_RESULT_EXECUTED;
}

