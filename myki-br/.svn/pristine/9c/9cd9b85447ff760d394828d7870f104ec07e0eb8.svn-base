/**************************************************************************
*   ID :  BR_LLSC_6_17
*
*    Resolve trip locations on different station line
*
*    1.  TAppControl.ProductInUse
*    2.  Dynamic.CurrentTripZoneLow
*    3.  Dynamic.CurrentTripZoneHigh
*    4.  TAppTProduct.LastUsageRouteID
*    5.  TAppTproduct.LastUsageEntryPoint
*    6.  Dynamic.LineId
*    7.  Dynamic.CurrentDateTime
*    8.  TAppControl.PassengerCode
*    9.  Dynamic.CurrentTripIsThinZone
*    10.  Dynamic.EntryPointID
*    11.  Static.TransportMode
*    12.  Dynamic.FareRouteID
*    13.  Dynamic.TransportLocations
*    14.  Dynamic.IsForceScanOff
*    15.  Dynamic.CurrentTripDirection
*
*    Pre-Conditions
*    1.  The product in use field(1) is not 0 (a product is in a scanned-on state).
*    2.  The mode of Transport for the current location(11) is Rail.
*    3.  The mode of Transport for the scan on location service provider(5) is Rail
*    4.  None of the current location station lines are contained within the scan on station list of station lines
*
*    Description
*    1.  Set IsEarlyBirdTrip(14) to false
*    2.  Set the FareRouteID(12) to none - no route will be considered for pricing
*    3.  Determine the stations lines passing through scan-on stations and scan-off stations (the current
*        device).
*       a.  Get the Scanon Station and Station Line (using the last usage entry point(5))
*       b.  Get the Scanoff Station and Station Line (using the current devices(6))
*    4.  Using the scan on location and the current device location determine the first station (changeover
*        station) at which at both the scan-on station line and scan-off station line pass though.
*    5.  Append the scan on location, current location, and the change over location to the list of locations(13) to
*        determine the zone range
*    6.  Determine the Origin Zone and Destination Zone based on the Scan on StationID and current station
*        location.
*    7.  Set the Current Trip Direction(15) to Unknown.
*    8.  if the OriginZone is greater than the Destination zone then:
*       a.  Set the current trip direction(15) to Inbound
*    9.  else if the OriginZone is less than DestinationZone then:
*       a.  set the current trip direction(15) to Outbound
*
*   Notes
*    If there is no valid intersections then there is a
*    an exception condition - the Tariff is
*    invalid.
*
*   Post-Conditions
*    1.  The Locations for zone range determination have been determined and stored for zone range
*    determination
*
 ***********************************************************************/

//=============================================================================
//
//  Set Early Bird Trip, Fare Route, Trip Direction and Location List
//  for Rail to Rail journey leg involving more than one line.
//
//  If the scan-on location was Rail and the scan-off (ie current) location is also Rail,
//  and there is no single line containing both locations, then
//  -   Populate the list of traversed locations with the scan-on station,
//      the scan-off station, and the calculated changeover station.
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
//  Determine the station at which the passenger is deemed to have switched
//  from the scan-on line to the scan-off line.
//
//  This is determined as the nearest station to the scan-on station (heading towards
//  the CBD) which is on both the scan-on and scan-off lines.
//
//  Implements the following step in KA0004 (BR_LLSC_6_17) :
//  4.  Using the scan on location and the current device location determine the
//      first station (changeover station) at which at both the scan-on station
//      line and scan-off station line pass though
//
//  Notes :
//      -   This function assumes both scan-on and scan-off locations are Rail.
//      -   This function assumes that there is at most one changeover required
//          to get from any station to any other station. It will only search
//          for that single changeover station and will return an error if
//          more than one changeover is needed.
//      -   While not stated in the documentation, Brendan has confirmed that
//          station IDs on a given line are ordered in descending distance
//          from the CBD - ie, a station further from the CBD is guaranteed to
//          have a numerically smaller ID than a station closer to the CBD on
//          the same line.
//      -   The function MYKI_CD_getChangeoverLoc() in the Myki CD package is not
//          being used because it doesn't deliver reproducible results - it returns
//          the first common station it finds, but if there's more than one then
//          the one it returns is effectively randomly picked from the set of matches.
//          Also, determining the changeover station is more a BR function than a CD one.
//
//  Algorithm :
//      For each line passing through the scan-on station
//          For each station on that line from the scan-on station to the CBD (in that order)
//              If there is at least one line common to both that station and the scan-off station
//                  Return that station as the changeover station
//
//  Arguments :
//      scanOnStationId         The location ID at which the scan-on occurred.
//      scanOffStationId        The current location ID.
//
//  Returns :
//      The station ID of the station which is deemed to be the station at
//      which the patron switched from the scan-on line to the current line.
//
//=============================================================================

static int      GetChangeoverStation( int scanOnStationId, int scanOffStationId )
{
    MYKI_CD_StationsByLine_t    stationsByLine;
    MYKI_CD_U16Array_t          stations;

    MYKI_CD_LinesByStation_t    linesByStationScanOn;
    MYKI_CD_U16Array_t          linesScanOn;
    MYKI_CD_LinesByStation_t    linesByStationScanOff;
    MYKI_CD_U16Array_t          linesScanOff;
    MYKI_CD_LinesByStation_t    linesByStation;
    MYKI_CD_U16Array_t          lines;

    int     line;
    int     lineScanOn;
    int     lineScanOff;
    int     station;

    //  Get all the lines passing through the scan-on station
    if ( ! MYKI_CD_getLinesByStationStructure( (U16_t)scanOnStationId, &linesByStationScanOn, &linesScanOn ) )
    {
        CsErrx( "BR_LLSC_6_17 : Error finding lines for scan-on station" );
        return -1;
    }

    //  Get all the lines passing through the scan-off station
    if ( ! MYKI_CD_getLinesByStationStructure( (U16_t)scanOffStationId, &linesByStationScanOff, &linesScanOff ) )
    {
        CsErrx( "BR_LLSC_6_17 : Error finding lines for scan-off station" );
        return -1;
    }

    //  For each line passing through the scan-on station
    for ( lineScanOn = 0; lineScanOn < linesScanOn.arraySize; lineScanOn++ )
    {
        //  Get all the stations on the line, in ascending order of distance from the CBD
        if ( MYKI_CD_getStationsByLineStructure( linesScanOn.arrayOfU16[ lineScanOn ], &stationsByLine, &stations ) )
        {
            //  For each station on the line (from the end of the line towards the CBD)
            for ( station = stations.arraySize - 1; station >= 0; station-- )
            {
                //  If the candidate station is closer than the scan-on station (ie, ignore the scan-on station and any stations further out)
                if ( stations.arrayOfU16[ station ] > scanOnStationId )
                {
                    //  Get all the lines passing through the candidate station
                    if ( MYKI_CD_getLinesByStationStructure( stations.arrayOfU16[ station ], &linesByStation, &lines ) )
                    {
                        //  For each line passing through the candidate station
                        for ( line = 0; line < lines.arraySize; line++ )
                        {
                            //  For each line passing through the scan-off station
                            for ( lineScanOff = 0; lineScanOff < linesScanOff.arraySize; lineScanOff++ )
                            {
                                //  If the two lines are the same
                                if ( lines.arrayOfU16[ line ] == linesScanOff.arrayOfU16[ lineScanOff ] )
                                {
                                    //  This is the changeover station
                                    return stations.arrayOfU16[ station ];
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    CsErrx( "BR_LLSC_6_17 : Unable to find a changeover station" );
    return -1;
}

//=============================================================================
//
//
//
//=============================================================================

RuleResult_e BR_LLSC_6_17( MYKI_BR_ContextData_t *pData )
{
    MYKI_TAControl_t    *pMYKI_TAControl;
    MYKI_Directory_t    *pDirInUse = NULL;
    MYKI_TAProduct_t    *pPdtInUse = NULL;
    MYKI_CD_Stations_t  station;            // Station details - filled in by myki_br_GetStationDetails() but not used here.
    MYKI_CD_Locations_t location;           // Location details
    int                 origZone;
    int                 destZone;
    int                 commonLines;
    int                 changeoverStationId;

    CsDbg( BRLL_RULE, "BR_LLSC_6_17 : Start (Resolve trip locations on different station line)" );

    if ( ! pData )
    {
        CsErrx( "BR_LLSC_6_17 : Invalid argument(s)" );
        return RULE_RESULT_ERROR;
    }

    if ( MYKI_CS_TAControlGet(&pMYKI_TAControl) )
    {
        CsErrx( "BR_LLSC_6_17 : MYKI_CS_TAControlGet() failed" );
        return RULE_RESULT_ERROR;
    }

    //
    //  Pre-Conditions
    //

    // 1.  The product in use field(1) is not 0 (a product is in a scanned-on state).

    if (pMYKI_TAControl->ProductInUse == 0)
    {
        CsDbg( BRLL_RULE, "BR_LLSC_6_17 : BYPASSED : ProductInUse is zeor");
        return RULE_RESULT_BYPASSED;
    }

    //  Get product and directory structure in use

    if ( myki_br_GetCardProduct( pMYKI_TAControl->ProductInUse, &pDirInUse, &pPdtInUse ) < 0 )
    {
        CsErrx( "BR_LLSC_6_17 : myki_br_GetCardProduct( %d ) failed", pMYKI_TAControl->ProductInUse );
        return RULE_RESULT_ERROR;
    }

    // 2.  The mode of Transport for the current location(11) is Rail.

    if ( pData->InternalData.TransportMode != TRANSPORT_MODE_RAIL )
    {
        CsDbg( BRLL_RULE, "BR_LLSC_6_17 : BYPASSED - Device Transport Mode != RAIL" );
        return RULE_RESULT_BYPASSED;
    }


    // 3.  The mode of Transport for the scan on location service provider(5) is Rail

    if ( myki_br_cd_GetTransportModeForProvider(pPdtInUse->LastUsage.ProviderId) != TRANSPORT_MODE_RAIL )
    {
        CsDbg( BRLL_RULE, "BR_LLSC_6_17 : BYPASSED - Last service provider ID != RAIL" );
        return RULE_RESULT_BYPASSED;
    }

    // 4.   None of the current location station lines are contained within the scan on station list of station lines

    if ( ( commonLines = myki_br_CommonLines( pData, pPdtInUse->LastUsage.Location.EntryPointId, pData->DynamicData.entryPointId ) ) < 0 )
    {
        CsErrx( "BR_LLSC_6_17 : myki_br_CommonLines() failed" );
        return RULE_RESULT_ERROR;
    }
    else if ( commonLines > 0 )
    {
        CsDbg( BRLL_RULE, "BR_LLSC_6_17 : BYPASSED - There is at least one line common to both Scan-On and Current locations" );
        return RULE_RESULT_BYPASSED;
    }

    //
    //  Description
    //

    // 1.   Set IsEarlyBirdTrip(14) to false

    pData->DynamicData.isEarlyBirdTrip = FALSE;

    // 2.   Set the FareRouteID(12) to none - no route will be considered for pricing

    pData->DynamicData.fareRouteIdIsValid = FALSE;

    // 3.   Determine the stations lines passing through scan-on stations and scan-off stations (the current device).
    // a.   Get the Scanon Station and Station Line (using the last usage entry point(5))
    // b.   Get the Scanoff Station and Station Line (using the current devices(6))
    // 4.   Using the scan on location and the current device location determine the first station (changeover station)
    //         at which at both the scan-on station line and scan-off station line pass though.

    if ( ( changeoverStationId = GetChangeoverStation( pPdtInUse->LastUsage.Location.EntryPointId, pData->DynamicData.entryPointId ) ) < 0 )
    {
        CsErrx( "BR_LLSC_6_17 : No changeover location found" );
        return RULE_RESULT_ERROR;
    }

    // 5.   Append the scan on location, current location, and the change over location to the list of locations(13) to determine the zone range
    // 6.   Determine the Origin Zone and Destination Zone based on the Scan on StationID and current station location.

    //      Origin (Scan-on)

    if ( myki_br_cd_GetStationDetails( pPdtInUse->LastUsage.Location.EntryPointId, &station, &location ) < 0 )
    {
        CsErrx( "BR_LLSC_6_17 : Failed to get station details" );
        return RULE_RESULT_ERROR;
    }

    if ( myki_br_AddLocationToTransportLocations( pData, &location ) < 0 )
    {
        CsErrx( "BR_LLSC_6_17 : Unable to add transport location" );
        return RULE_RESULT_ERROR;
    }

    origZone = location.zone;

    //      Destination (Scan-off / Current)

    if ( myki_br_cd_GetStationDetails( pData->DynamicData.entryPointId, &station, &location ) < 0 )
    {
        CsErrx( "BR_LLSC_6_17 : Failed to get station details" );
        return RULE_RESULT_ERROR;
    }

    if ( myki_br_AddLocationToTransportLocations( pData, &location ) < 0 )
    {
        CsErrx( "BR_LLSC_6_17 : Unable to add transport location" );
        return RULE_RESULT_ERROR;
    }

    destZone = location.zone;

    //      Changeover

    if ( myki_br_cd_GetStationDetails( changeoverStationId, &station, &location ) < 0 )
    {
        CsErrx( "BR_LLSC_6_17 : Failed to get station details" );
        return RULE_RESULT_ERROR;
    }

    if ( myki_br_AddLocationToTransportLocations( pData, &location ) < 0 )
    {
        CsErrx( "BR_LLSC_6_17 : Unable to add transport location" );
        return RULE_RESULT_ERROR;
    }

    // 7.       Set the Current Trip Direction(15) to Unknown.
    // 8.       If the OriginZone is greater than the Destination zone then:
    // 8.a.     Set the current trip direction(15) to Inbound
    // 9.       Else if the OriginZone is less than DestinationZone then:
    // 9.a.     Set the current trip direction(15) to Outbound

    myki_br_SetTripDirectionFromZones( pData, origZone, destZone );

    CsDbg( BRLL_RULE, "BR_LLSC_6_17 : EXECUTED" );
    return RULE_RESULT_EXECUTED;
}

