/* -*- mode: c; tabs: 4; -*- */
/*=============================================================================
**
**    Vix Technology                   Licensed software
**    (C) 2014                         All rights reserved
**
**=============================================================================
**
**  Project/Product : MBU
**  Filename        : BR_LLSC_6_1.c
**  Author(s)       : ???
**
**  ID              : BR_LLSC_6_1 - NTS0177 v7.3
**
**  Name            : Resolve locations Trip Zone Range
**
**  Data Fields     :
**
**      1.  TAppControl.ProductInUse
**      2.  TAppTProduct.LastUsageStopId
**      3.  Dynamic.StopId
**      4.  Dynamic.CurrentTripZoneLow
**      5.  Dynamic.CurrentTripZoneHigh
**      6.  TAppTProduct.LastUsageLineId
**      7.  Dynamic.LineId
**      8.  Dynamic.CurrentDateTime
**      9.  TAppControl.PassengerCode
**      10. Tariff.HeadlessRouteId
**      11. Dynamic.CurrentTripIsThinZone
**      12. Dynamic.CurrentTripDirection
**      13. Dynamic.RouteChangeoverStatus
**      14. Dynamic.TraverseRouteID
**      15. Dynamic.TraverseStopID
**      16. Dynamic DestinationStopID
**      17. Dynamic.OriginRouteID
**      18. Dynamic.OriginStopID
**      19. Dynamic.TraverseDestinationStopID
**      20. Dynamic.DestinationRouteID
**      21. Dynamic.IsEarlyBirdTrip
**      22. Static.ModeofTransport
**      23. Dynamic.FareRouteID
**      24. Dynamic.TransportLocations
**
**  Pre-Conditions  :
**
**      1.  The product in use field(1) is not 0 (a product is in a scanned-on
**          state).
**      2.  The current mode of transport(22) of the current device is not
**          equal to Rail
**
**  Description     :
**
**      1.  Set IsEarlyBirdTrip to false
**      2.  Determine the Origin Zone and Destination Zone based on the 
**          originRouteId/OriginStopId and DestinationRouteId/DestinationStopId.
**      3.  Set the Current Trip Direction12 to Unknown.
**      4.  if the OriginZone is greater than the Destination zone then:
**          a.  Set the current trip direction12 to Inbound 
**      5.  else if the OriginZone is less than DestinationZone then:
**          a.  set the current trip direction12 to Outbound
**      6.  If both the origin route id or current route id are not equal to the
**          headless route id then
**          a.  If the origin route id6 is equal to the  route id of the device
**              i.  If the scan on stop id2 is equal to the stop id of the
**                  device then
**                  (1) Add then the current location3 to the list of the
**                      locations24 twice.
**                  (2) Set the Fare Route to use23 to current route
**              ii. Else if the scan on stop id2 is less than the stop id of
**                  the device then 
**                  (1) Add all locations from the scan on stop to the scan off
**                      stop location to the list of transport locations24
**                  (2) Set the Fare Route to use23 to current route
**              iii.Else condider the route as a circular route and thus
**                  (1) Add all locations from the scan on stop location to
**                      end of the route to the list of transport locations24
**                  (2) Add all locations from the first stop on the route to
**                      the devices current stop id to the list of transport
**                      locations24
**                  (3) Set the Fare Route to use23 to current route
**          b.  Else the routes must be different 
**              (1) Add all locations from the scan on route and stop to end
**                  of the route to the list of transport locations24
**              (2) Add all locations from the scan off route stop 1 to the
**                  current stop to the list of transport locations24
**              (3) If both scan on route and scan off route has Fare Route
**                  defined then set the fare route to use as the scan off
**                  route else set the fare route to none.
**      7.  Else we must consider headless route impact
**          a.  Add  the scan-on location and the current location to the list
**              of transport locations24 for determining the zone range 
**          b.  Set the Fare Route to use23 to none - no route will be used
**              in pricing.
**
**  Post-Conditions :
**
**      1.  The list of locations for zone range determinations has
**          been populated.
**
**  Devices         :
**
**      Fare payment devices
**
**  Member(s)       :
**      BR_LLSC_6_1             [public]    business rule
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
**    1.00  ??.??.??    ???   Create
**    1.01  25.06.14    ANT   Add      Implemented the business rule
**    1.02  26.08.14    ANT   Add      ACS compatible mode. Note, it seems
**                                     ACS only adds the current stop to
**                                     list if scan off on different route!?
**    1.03  01.10.14    ANT   Modify   MTU-112: Determine origin zone from
**                                     last usage route and stop ids
**
**===========================================================================*/

#include <cs.h>                 // For debug logging
#include <myki_cardservices.h>  // Myki Card Services
#include <LDT.h>                // Logical Device Transaction

#include "myki_br_rules.h"      // API for all BR_xxx modules
#include "BR_Common.h"          // Common utilities, macros, etc

/*==========================================================================*
**
**  BR_LLSC_6_1
**
**  Description     :
**      Implements business rule BR_LLSC_6_1
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
**==========================================================================*/

RuleResult_e BR_LLSC_6_1( MYKI_BR_ContextData_t *pData )
{
    MYKI_TAControl_t       *pMYKI_TAControl     = NULL;
    MYKI_Directory_t       *pDirectoryInUse     = NULL;
    MYKI_TAProduct_t       *pProductInUse       = NULL;
    int                     OriginZone          = -1;
    int                     DestinationZone     = -1;

    CsDbg( BRLL_RULE, "BR_LLSC_6_1 : Start (Resolve locations Trip Zone Range)" );

    if ( pData == NULL )
    {
        CsErrx( "BR_LLSC_6_1 : Invalid argument(s)" );
        return RULE_RESULT_ERROR;
    }

    if ( MYKI_CS_TAControlGet( &pMYKI_TAControl ) < 0 )
    {
        CsErrx( "BR_LLSC_6_1 : MYKI_CS_TAControlGet() failed" );
        return RULE_RESULT_ERROR;
    }

    /*  PRE-CONDITIONS */
    {
        /*  1.  The product in use field(1) is not 0 (a product is in a scanned-on state). */
        if ( pMYKI_TAControl->ProductInUse == 0 )
        {
            CsDbg( BRLL_RULE, "BR_LLSC_6_1 : Bypassed - ProductInUse = 0" );
            pData->ReturnedData.bypassCode  = BYPASS_CODE( 6, 1, 1, 0 );
            return RULE_RESULT_BYPASSED;
        }

        /*  2.  The current mode of transport(22) of the current device is not equal to Rail */
        if ( pData->InternalData.TransportMode == TRANSPORT_MODE_RAIL )
        {
            CsDbg( BRLL_RULE, "BR_LLSC_6_1 : Bypassed - Device Transport Mode is RAIL" );
            pData->ReturnedData.bypassCode  = BYPASS_CODE( 6, 1, 2, 0 );
            return RULE_RESULT_BYPASSED;
        }
    }

    /*  PROCESSING */
    {
        if ( myki_br_GetCardProduct( pMYKI_TAControl->ProductInUse, &pDirectoryInUse, &pProductInUse ) < 0 )
        {
            CsErrx( "BR_LLSC_6_1 : myki_br_GetCardProduct(%d) failed", pMYKI_TAControl->ProductInUse );
            return RULE_RESULT_ERROR;
        }

        /*  1.  Set IsEarlyBirdTrip to false */
        pData->DynamicData.isEarlyBirdTrip  = FALSE;

        /*  2.  Determine the Origin Zone and Destination Zone based on the 
                originRouteId/OriginStopId and DestinationRouteId/DestinationStopId. */
        OriginZone                          = pProductInUse->LastUsage.Zone;
        {
            /*  NOTE:   ACS incorrectly sets LastUsage.Zone if Transport mode is BUS/TRAM.
                        Determine LastUsage.Zone from route id and stop id */
            int                     OriginRouteId   = pProductInUse->LastUsage.Location.RouteId;
            int                     OriginStopId    = pProductInUse->LastUsage.Location.StopId;
            MYKI_CD_RouteStop_t*    pRouteStops     = NULL;
            MYKI_CD_Locations_t     location;

            if ( MYKI_CD_getRouteStopsStructure( OriginRouteId, OriginStopId, OriginStopId, &pRouteStops ) <= 0 )
            {
                CsWarnx( "BR_LLSC_6_1 : MYKI_CD_getRouteStopsStructure(%d,%d) failed", OriginRouteId, OriginStopId );
            }
            else
            if ( MYKI_CD_getLocationsStructure( pRouteStops[ 0 ].locationId, &location ) == FALSE )
            {
                CsWarnx( "BR_LLSC_6_1 : MYKI_CD_getLocationsStructure(%d) failed", pRouteStops[ 0 ].locationId );
            }
            else
            if ( OriginZone != location.zone )
            {
                CsDbg( BRLL_RULE, "BR_LLSC_6_1 : Overwrite LastUsage.Zone(%d) with %d", OriginZone, location.zone );
                OriginZone                  = location.zone;
            }   /*  end-of-if */
        }
        DestinationZone                     = pData->DynamicData.currentZone;
 
        /*  3.  Set the Current Trip Direction12 to Unknown.
            4.  if the OriginZone is greater than the Destination zone then:
                a.  Set the current trip direction12 to Inbound 
            5.  else if the OriginZone is less than DestinationZone then:
                a.  set the current trip direction12 to Outbound */
        myki_br_SetTripDirectionFromZones( pData, OriginZone, DestinationZone );

        /*  6.  If both the origin route id or current route id are not equal to the headless route id then */
        if ( pProductInUse->LastUsage.Location.RouteId != pData->Tariff.headlessRouteId &&
             pData->DynamicData.lineId                 != pData->Tariff.headlessRouteId )
        {
            /*  a.  If the origin route id6 is equal to the route id of the device */
            if ( pProductInUse->LastUsage.Location.RouteId == pData->DynamicData.lineId )
            {
                /*  i.  If the scan on stop id2 is equal to the stop id of the device then */
                if ( pProductInUse->LastUsage.Location.StopId == pData->DynamicData.stopId )
                {
                    /*  (1) Add [then] the current location3 to the list of the locations24 twice. */
                    if ( myki_br_AddStopToTransportLocationsEx( pData, pData->DynamicData.lineId, pData->DynamicData.stopId, 2 ) < 0 )
                    {
                        CsErrx( "BR_LLSC_6_1 : myki_br_AddStopToTransportLocationsEx() failed" );
                        return RULE_RESULT_ERROR;
                    }

                    /*  (2) Set the Fare Route to use23 to current route */
                    pData->DynamicData.fareRouteId          = pData->DynamicData.lineId;
                    pData->DynamicData.fareRouteIdIsValid   = TRUE;
                }
                else
                /*  ii. Else if the scan on stop id2 is less than the stop id of the device then */
                if ( pProductInUse->LastUsage.Location.StopId < pData->DynamicData.stopId )
                {
                    /*  (1) Add all locations from the scan on stop to the scan off stop location to the list of transport locations24 */
                    if ( myki_br_AddStopRangeToTransportLocations( pData, pData->DynamicData.lineId, pProductInUse->LastUsage.Location.StopId, pData->DynamicData.stopId ) < 0 )
                    {
                        CsErrx( "BR_LLSC_6_1 : myki_br_AddStopRangeToTransportLocations() failed" );
                        return RULE_RESULT_ERROR;
                    }

                    /*  (2) Set the Fare Route to use23 to current route */
                    pData->DynamicData.fareRouteId          = pData->DynamicData.lineId;
                    pData->DynamicData.fareRouteIdIsValid   = TRUE;
                }
                /*  iii.Else con[d]sider the route as a circular route and thus */
                else
                {
                    /*  (1) Add all locations from the scan on stop location to end of the route to the list of transport locations24 */
                    if ( myki_br_AddStopRangeToTransportLocations( pData, pData->DynamicData.lineId, pProductInUse->LastUsage.Location.StopId, 255 /*LAST*/ ) < 0 )
                    {
                        CsErrx( "BR_LLSC_6_1 : myki_br_AddStopRangeToTransportLocations() failed" );
                        return RULE_RESULT_ERROR;
                    }

                    /*  (2) Add all locations from the first stop on the route to the devices current stop id to the list of transport locations24 */
                    if ( myki_br_AddStopRangeToTransportLocations( pData, pData->DynamicData.lineId, 1 /*FIRST*/, pData->DynamicData.stopId ) < 0 )
                    {
                        CsErrx( "BR_LLSC_6_1 : myki_br_AddStopRangeToTransportLocations() failed" );
                        return RULE_RESULT_ERROR;
                    }

                    /*  (3) Set the Fare Route to use23 to current route */
                    pData->DynamicData.fareRouteId          = pData->DynamicData.lineId;
                    pData->DynamicData.fareRouteIdIsValid   = TRUE;
                }
            }
            /*  b.  Else the routes must be different */
            else
            {
                /*  (1) Add all locations from the scan on route and stop to end of the route to the list of transport locations24 */
                if ( myki_br_AddStopRangeToTransportLocations( pData, pProductInUse->LastUsage.Location.RouteId, pProductInUse->LastUsage.Location.StopId, 255 /*LAST*/ ) < 0 )
                {
                    CsErrx( "BR_LLSC_6_1 : myki_br_AddStopRangeToTransportLocations() failed" );
                    return RULE_RESULT_ERROR;
                }

                /*  (2) Add all locations from the scan off route stop 1 to the current stop to the list of transport locations24 */
                if ( myki_br_AddStopRangeToTransportLocations( pData,
                        pData->DynamicData.lineId,
                        /*  WARNING! It seems ACS only adds the current stop to the list!? (see Test_Route_Change_V_2300_1.2) */
                        pData->StaticData.AcsCompatibilityMode != FALSE ? pData->DynamicData.stopId : 1 /*FIRST*/,
                        pData->DynamicData.stopId ) < 0 )
                {
                    CsErrx( "BR_LLSC_6_1 : myki_br_AddStopRangeToTransportLocations() failed" );
                    return RULE_RESULT_ERROR;
                }

                /*  (3) If both scan on route and scan off route has Fare Route defined */
                if ( pProductInUse->LastUsage.Location.RouteId != 0 &&
                     pData->DynamicData.lineId                 != 0 )
                {
                    /*  then set the fare route to use as the scan off route */
                    pData->DynamicData.fareRouteId          = pData->DynamicData.lineId;
                    pData->DynamicData.fareRouteIdIsValid   = TRUE;
                }
                else
                {
                    /*  else set the fare route to none. SHOULD NEVER GET HERE!? */
                    pData->DynamicData.fareRouteId          = 0;
                    pData->DynamicData.fareRouteIdIsValid   = FALSE;
                }
            }
        }
        /*  7.  Else we must consider headless route impact */
        else
        {
            /*  a.  Add  the scan-on location and the current location to the list
                    of transport locations24 for determining the zone range */
            if ( myki_br_AddStopToTransportLocations( pData, pProductInUse->LastUsage.Location.RouteId, pProductInUse->LastUsage.Location.StopId ) < 0 ||
                 myki_br_AddStopToTransportLocations( pData, pData->DynamicData.lineId,                 pData->DynamicData.stopId                ) < 0 )
            {
                CsErrx( "BR_LLSC_6_1 : myki_br_AddStopToTransportLocations() failed" );
                return RULE_RESULT_ERROR;
            }

            /*  b.  Set the Fare Route to use23 to none - no route will be used in pricing. */
            pData->DynamicData.fareRouteId          = 0;
            pData->DynamicData.fareRouteIdIsValid   = FALSE;
        }
    }

    CsDbg( BRLL_RULE, "BR_LLSC_6_1 : Executed" );
    return RULE_RESULT_EXECUTED;
}   /*  BR_LLSC_6_1( ) */
