/**************************************************************************
*   ID :  BR_LLSC_6_16
*
*    Resolve trip locations on the same station line
*
*    1.  TAppControl.ProductInUse
*    2.  Dynamic.CurrentTripZoneLow
*    3.  Dynamic.CurrentTripZoneHigh
*    4.  TAppTProduct.LastUsageRouteID
*    5.  TAppTproduct.LastUsageEntryPoint
*    6.  Dynamic.LineId
*    7.  Dynamic.CurrentDateTime
*    8.  Dynamic.CurrentTripIsThinZone
*    9.  Dynamic.EntryPointID
*    10.  Static.TransportMode
*    11.  Dynamic.FareRouteID
*    12.  Dynamic.TransportLocations
*    13.  Dynamic.CurrentTripDirection
*    14.  TAppTproduct.LastUseageServiceProvider
*    15.  Dynamic.IsEarlyBirdTrip
*
*    Pre-Conditions
*    1.  The product in use field(1) is not 0 (a product is in a scanned-on state).
*    2.  The Mode of transport of the service provider(14) for the product in use(1) is Rail
*    3.  The Mode of transport of the current device(10) is Rail
*    4.  Either of the following are true:
*       a.  The scan on station line(6) is contained within the list of station lines that pass though the current
*           location
*       b.  The any of the current location station lines is contained in the scan on station list of station lines.
*
*    Description
*    1.  Set IsEarlyBirdTrip(15) to false
*    2.  Set the FareRouteID(11) to none - no route will be considered for pricing
*    3.  Determine the origin location and destination location by using the scan on Station ID and current
*        station ID respectively ;  insert the origin and destination location into the list of transport locations(12)
*        for zone range determination
*    4.  Determine the trip direction, set the Current Trip Direction(13) to Unknown.
*        a.  if the OriginZone is greater than the Destination zone then:
*           i.  Set the current trip direction(13) to Inbound
*        b.  else if the OriginZone is less than DestinationZone then:
*           i.  set the current trip direction(13) to Outbound
*
 ***********************************************************************/

#include <cs.h>                 // For debug logging
#include <myki_cardservices.h>  // Myki Card Services
#include <LDT.h>                // Logical Device Transaction

#include <myki_cd.h>

#include "myki_br_rules.h"      // API for all BR_xxx modules
#include "BR_Common.h"          // Common utilities, macros, etc

//=============================================================================
//
//
//
//=============================================================================

RuleResult_e BR_LLSC_6_16( MYKI_BR_ContextData_t *pData )
{
    MYKI_TAControl_t    *pMYKI_TAControl;
    MYKI_Directory_t    *pDirectory = NULL;
    MYKI_TAProduct_t    *pProduct   = NULL;

    MYKI_CD_Stations_t          origStation;
    MYKI_CD_Stations_t          destStation;
    MYKI_CD_Locations_t         origLocation;
    MYKI_CD_Locations_t         destLocation;
    int                         commonLines;

    CsDbg( BRLL_RULE, "BR_LLSC_6_16 : Start (Resolve trip locations on the same station line)" );

    if ( ! pData )
    {
        CsErrx( "BR_LLSC_6_16 : Invalid argument(s)" );
        return RULE_RESULT_ERROR;
    }

    // Pre-Conditions
    //
    // 1.   The product in use field(1) is not 0 (a product is in a scanned-on state).

    if ( MYKI_CS_TAControlGet( &pMYKI_TAControl ) )
    {
        CsErrx( "BR_LLSC_6_16 : MYKI_CS_TAControlGet() failed" );
        return RULE_RESULT_ERROR;
    }

    if ( pMYKI_TAControl->ProductInUse == 0 )
    {
        CsDbg( BRLL_RULE, "BR_LLSC_6_16 : Bypass - ProductInUse is zero");
        return RULE_RESULT_BYPASSED;
    }

    if ( myki_br_GetCardProduct( pMYKI_TAControl->ProductInUse, &pDirectory, &pProduct ) < 0 )
    {
        CsErrx( "BR_LLSC_6_16 : Failed to get product %d", pMYKI_TAControl->ProductInUse );
        return RULE_RESULT_ERROR;
    }

    // 2.   The Mode of transport of the service provider(14) for the product in use(1) is Rail
    if ( myki_br_cd_GetTransportModeForProvider( pProduct->LastUsage.ProviderId ) != TRANSPORT_MODE_RAIL )
    {
        CsDbg( BRLL_RULE, "BR_LLSC_6_16 : Bypass - Last service provider transport mode is not RAIL" );
        return RULE_RESULT_BYPASSED;
    }

    // 3.   The Mode of transport of the current device10 is Rail
    if ( pData->InternalData.TransportMode != TRANSPORT_MODE_RAIL )
    {
        CsDbg( BRLL_RULE, "BR_LLSC_6_16 : Bypass - Device Transport Mode is not RAIL" );
        return RULE_RESULT_BYPASSED;
    }

    // 4.   Either of the following are true:
    // a.   The scan on station line(6) is contained within the list of station lines that pass though the current location
    // b.   The any of the current location station lines is contained in the scan on station list of station lines.
    //
    //  KWS:    Interpreted this to mean :
    //          There is at least one LineId which is present in both the scan on station list of LineIds and the current station list of LineIds.
    //          This is because if 4a is met, then 4b must also be met (4a is a special case of 4b), so only 4a needs to be checked.
    //          However, since we don't have the "scan-on station line" stored on the card (only the scan-on station), we have no idea which
    //          line was actually used, so we have to check only 4b instead. We do this by calling the generic function myki_br_CommonLines()
    //          which returns TRUE if there is at least one line common to both stations.

    if ( ( commonLines = myki_br_CommonLines( pData, pProduct->LastUsage.Location.EntryPointId, pData->DynamicData.entryPointId ) ) < 0 )
    {
        CsErrx( "BR_LLSC_6_16 : myki_br_CommonLines() failed" );
        return RULE_RESULT_ERROR;
    }
    else if ( commonLines == 0 )
    {
        CsDbg( BRLL_RULE, "BR_LLSC_6_16 : Bypass - No lines common to both scan on station and current station" );
        return RULE_RESULT_BYPASSED;
    }

    //
    // Description
    //

    // 1.   Set IsEarlyBirdTrip(15) to false

    pData->DynamicData.isEarlyBirdTrip = FALSE;

    // 2.   Set the FareRouteID(11) to none - no route will be considered for pricing

    pData->DynamicData.fareRouteIdIsValid = FALSE;

    // 3.   Determine the origin location and destination location by using the scan on Station ID and current station ID respectively
    //      This is ok because we already know both are rail or this rule would have bypassed above.

    if ( myki_br_cd_GetStationDetails( pProduct->LastUsage.Location.EntryPointId, &origStation, &origLocation ) < 0 )
    {
        CsErrx( "BR_LLSC_6_16 : Failed to get station details" );
        return RULE_RESULT_ERROR;
    }

    if ( myki_br_cd_GetStationDetails( pData->DynamicData.entryPointId, &destStation, &destLocation ) < 0 )
    {
        CsErrx( "BR_LLSC_6_16 : Failed to get station details" );
        return RULE_RESULT_ERROR;
    }

    // insert the origin and destination location into the list of transport locations(12) for zone range determination

    if ( myki_br_AddLocationToTransportLocations( pData, &origLocation ) < 0 )
    {
        CsErrx( "BR_LLSC_6_16 : Unable to add transport location" );
        return RULE_RESULT_ERROR;
    }

    if ( myki_br_AddLocationToTransportLocations( pData, &destLocation ) < 0 )
    {
        CsErrx( "BR_LLSC_6_16 : Unable to add transport location" );
        return RULE_RESULT_ERROR;
    }

    // 4.   Determine the trip direction, set the Current Trip Direction(13) to Unknown.
    // a.   if the OriginZone is greater than the Destination zone then:
    //       i.   Set the current trip direction(13) to Inbound
    // b.   else if the OriginZone is less than DestinationZone then:
    //      i.   set the current trip direction(13) to Outbound

    myki_br_SetTripDirectionFromZones( pData, origLocation.zone, destLocation.zone );

    CsDbg( BRLL_RULE, "BR_LLSC_6_16 : Executed" );
    return RULE_RESULT_EXECUTED;
}

