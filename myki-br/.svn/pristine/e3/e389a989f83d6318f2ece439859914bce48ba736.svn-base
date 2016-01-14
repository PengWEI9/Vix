//=============================================================================
//
//  Stubbed replacements for myki_cardservices.
//
//  Notes :
//  -   Had to remove 'const' modifier on RouteStops_n and pMYKI_CD_RouteStops
//      as the MYKI_CD_getRouteStopsStructure() prototype does not have it.
//      Perhaps it should...
//
//=============================================================================

//-----------------------------------------------------------------------------
//  Includes
//-----------------------------------------------------------------------------

#include <stdio.h>
#include <string.h>

#include <myki_cd.h>            // Myki_CD  - Configuration Data
#include <myki_actionlist.h>    // Actionlist

#include <cs.h>                 // Debug Support
#include "BR_Common.h"          // Debug Support

//-----------------------------------------------------------------------------
//  File-Local variables
//-----------------------------------------------------------------------------

static  const   MYKI_CD_Locations_t             Locations[ ] =
{
    {   10500,  3,      3,      3,      {0},    {0},    {0},    {0},    {0},    {0}     },
    {   10501,  3,      3,      3,      {0},    {0},    {0},    {0},    {0},    {0}     },
    {   10502,  3,      3,      3,      {0},    {0},    {0},    {0},    {0},    {0}     },
    {   10503,  3,      3,      3,      {0},    {0},    {0},    {0},    {0},    {0}     },
    {   10504,  3,      3,      3,      {0},    {0},    {0},    {0},    {0},    {0}     },
    {   10505,  3,      3,      3,      {0},    {0},    {0},    {0},    {0},    {0}     },
    {   10550,  2,      2,      2,      {0},    {0},    {0},    {0},    {0},    {0}     },
    {   10551,  3,      3,      3,      {0},    {0},    {0},    {0},    {0},    {0}     },
    {   10552,  2,      2,      2,      {0},    {0},    {0},    {0},    {0},    {0}     },
    {   10553,  2,      2,      3,      {0},    {0},    {0},    {0},    {0},    {0}     },
    {   19948,  3,      3,      3,      {0},    {0},    {0},    {0},    {0},    {0}     },
    {   19949,  3,      3,      3,      {0},    {0},    {0},    {0},    {0},    {0}     },
};

static  MYKI_CD_RouteStop_t   RouteStops_1[ ] =
{
    {   1,      10552,      1,      2,      0       },
    {   2,      10553,      1,      2,      0       },
    {   3,      10500,      1,      2,      0       },
    {   4,      10501,      1,      2,      0       },
    {   5,      10502,      1,      2,      0       },
};
static  MYKI_CD_RouteStop_t   RouteStops_2[ ] =
{
    {   1,      10550,      2,      2,      0       },
    {   2,      10551,      2,      2,      0       },
    {   3,      10552,      2,      2,      0       },
};

//=============================================================================
//
//  Stub functions to replace MYKI_CD package functions, to allow unit testing
//  of BR without having to set up a SQLite database.
//
//=============================================================================

int MYKI_CD_getServiceProvidersStructure( U16_t id, MYKI_CD_ServiceProviders_t *provider )
{
    if ( ! provider )
    {
        return FALSE;
    }

    provider->id = id;
    sprintf( provider->short_desc, "Provider %d", id );
    sprintf( provider->long_desc , "Provider %d", id );

    switch ( id )
    {
        //  "Standard" values, not tied to any CD
        case 0 :    strcpy( provider->mode, "RAIL" );     return TRUE;
        case 1 :    strcpy( provider->mode, "TRAM" );     return TRUE;
        case 2 :    strcpy( provider->mode, "BUS"  );     return TRUE;

        //  Values taken from current Kamco reference CD
        case 301 :  strcpy( provider->mode, "RAIL" );     return TRUE;      // Kamco
        case 1001 : strcpy( provider->mode, "TRAM" );     return TRUE;      // Yarra Trams
        case 10 :   strcpy( provider->mode, "BUS"  );     return TRUE;      // B M Walkers Bus Service

        default :
            return FALSE;
    }
}

int MYKI_CD_getServiceProviderTransportMode(U16_t id, char* mode, int bufferLength)
{
    switch ( id )
    {
        //  "Standard" values, not tied to any CD
        case 0 :    strncpy( mode, "RAIL", bufferLength );     return TRUE;
        case 1 :    strncpy( mode, "TRAM", bufferLength );     return TRUE;
        case 2 :    strncpy( mode, "BUS", bufferLength  );     return TRUE;

        //  Values taken from current Kamco reference CD
        case 301 :  strncpy( mode, "RAIL", bufferLength );     return TRUE;      // Kamco
        case 1001 : strncpy( mode, "TRAM", bufferLength );     return TRUE;      // Yarra Trams
        case 10 :   strncpy( mode, "BUS", bufferLength  );     return TRUE;      // B M Walkers Bus Service

        default :
            return FALSE;
    }

    return FALSE;
}

int MYKI_CD_getProductType(U8_t id, char* type, int bufferLength)
{
   switch ( id )
    {
        case 2 :    strncpy( type, "ePass"      ,bufferLength);    return TRUE;
        case 3 :    strncpy( type, "SingleTrip" ,bufferLength);    return TRUE;
        case 4 :    strncpy( type, "nHour"      ,bufferLength);    return TRUE;
        case 5 :    strncpy( type, "Daily"      ,bufferLength);    return TRUE;
        case 6 :    strncpy( type, "Weekly"     ,bufferLength);    return TRUE;

        default :
            return FALSE;
    }

    return FALSE;
}

int MYKI_CD_getProductCardType(U8_t id, char* type, int bufferLength)
{
    strncpy( type, "LLSC", bufferLength );
    return TRUE;
}

int MYKI_CD_getProductStructure( U8_t id, MYKI_CD_Product_t *product )
{
    if ( ! product )
    {
        return -1;
    }

    product->id = id;
    product->issuer_id = 66;

    switch ( id )
    {
        case 2 :    strcpy( product->type, "LLSC" ); strcpy( product->subtype, "ePass"      ); strcpy( product->short_desc, "myki pass"  ); strcpy( product->long_desc, "myki pass"                   ); return TRUE;
        case 3 :    strcpy( product->type, "LLSC" ); strcpy( product->subtype, "SingleTrip" ); strcpy( product->short_desc, "City Saver" ); strcpy( product->long_desc, "City Saver"                  ); return TRUE;
        case 4 :    strcpy( product->type, "LLSC" ); strcpy( product->subtype, "nHour"      ); strcpy( product->short_desc, "2 hour"     ); strcpy( product->long_desc, "2 Hour Stored Value Product" ); return TRUE;
        case 5 :    strcpy( product->type, "LLSC" ); strcpy( product->subtype, "Daily"      ); strcpy( product->short_desc, "Daily"      ); strcpy( product->long_desc, "Daily Stored Value Product"  ); return TRUE;
        case 6 :    strcpy( product->type, "LLSC" ); strcpy( product->subtype, "Weekly"     ); strcpy( product->short_desc, "Weekly"     ); strcpy( product->long_desc, "Weekly Stored Value Product" ); return TRUE;

        default :
            return FALSE;
    }
}

int MYKI_CD_getStationsStructure( U16_t id, MYKI_CD_Stations_t *station )
{
    station->id                     = id;

    switch ( id )
    {
        case 10500 :
            station->location               = 19948;
            station->provisional_zone_low   = 2;
            station->provisional_zone_high  = 3;
            return TRUE;

        case 10550 :
            station->location               = 19949;
            station->provisional_zone_low   = 2;
            station->provisional_zone_high  = 3;
            return TRUE;
    }

    return FALSE;
}

int MYKI_CD_getFaresProvisionalStructure( U8_t zoneLow, U8_t zoneHigh, MYKI_CD_FaresProvisional_t *faresProvisional )
{
    faresProvisional->zone_low      = zoneLow;
    faresProvisional->zone_high     = zoneHigh;
    faresProvisional->sv_full       = 100;
    faresProvisional->sv_conc       = 50;
    faresProvisional->epass_full    = 100;
    faresProvisional->epass_conc    = 50;

    return TRUE;
}

int MYKI_CD_getPassengerTypeStructure( U8_t id, MYKI_CD_PassengerType_t *passenger )
{
    if ( passenger != NULL )
    {
        memset( passenger, 0, sizeof( MYKI_CD_PassengerType_t ) );
        switch ( id )
        {
        case 57:
            passenger->id                   = 57;
            passenger->percent              = 100;
            passenger->cardexpirydays       = 1461;
            passenger->tappexpirydays       = 1461;
            passenger->passengercodedays    = 1461;
            strncpy( passenger->code, "APT", sizeof( passenger->code ) );
            strncpy( passenger->desc, "Access Travel Pass", sizeof( passenger->desc ) );
            strncpy( passenger->isdda, "true", sizeof( passenger->isdda ) );
            break;

        default:
            passenger->id                   = id;
            passenger->percent              = 0;
            passenger->cardexpirydays       = 0;
            passenger->tappexpirydays       = 0;
            passenger->passengercodedays    = 0;
            sprintf( passenger->code, "%d", id );
            sprintf( passenger->desc, "%d", id );
            strncpy( passenger->isdda, "false", sizeof( passenger->isdda ) );
            break;
        }   /* end-of-switch */
        return TRUE;
    }   /* end-of-if */
    return FALSE;
}

static int citySaverZoneId = -1;
void MYKI_CD_setCitySaverZone(U8_t zoneId)
{
    citySaverZoneId = zoneId;

}
int MYKI_CD_isCitySaverZone(U8_t zoneId)
{
    if ( citySaverZoneId > 0 && zoneId == citySaverZoneId )
    {
        return TRUE;
    }
    return FALSE;
}

static U16_t earlyBirdStationId1 = 0;
static U16_t earlyBirdStationId2 = 0;
int MYKI_CD_isEarlyBirdStation(U16_t stationId)
{
    return (earlyBirdStationId1 == stationId || earlyBirdStationId2 == stationId);
}

void MYKI_CD_setEarlyBirdStation(U16_t stationId1, U16_t stationId2)
{
    earlyBirdStationId1 = stationId1;
    earlyBirdStationId2 = stationId2;
}

static int earlyBirdDay = -1;
int MYKI_CD_isEarlyBirdDay(int day)
{
    return (day == earlyBirdDay );
}
void MYKI_CD_setEarlyBirdDay(int day)
{
    earlyBirdDay = day;
}

int MYKI_CD_getLinesByStationStructure( U16_t stationId, MYKI_CD_LinesByStation_t *linesByStation, MYKI_CD_U16Array_t *lineList )
{
    static  U16_t   station1[] = { 17 };    // Line 17 goes through all stations

    lineList->arraySize  = sizeof( station1 ) / sizeof( station1[ 0 ] );
    lineList->arrayOfU16 = station1;

    return TRUE;
}

int MYKI_CD_getStationsByLineStructure( U16_t lineId, MYKI_CD_StationsByLine_t *stationsByLine, MYKI_CD_U16Array_t *stationList )
{
    static  U16_t   line1[] = { 60020 };    // Melbourne Central - goes through all lines

    stationList->arraySize  = sizeof( line1 ) / sizeof( line1[ 0 ] );
    stationList->arrayOfU16 = line1;

    return TRUE;
}

int MYKI_CD_getLocationsStructure( U16_t id, MYKI_CD_Locations_t *pLocation )
{
    int i   = 0;

    if ( pLocation != NULL )
    {
        while ( i < ( sizeof( Locations ) / sizeof( Locations[ 0 ] ) ) )
        {
            if ( Locations[ i ].id == id )
            {
                memcpy( pLocation, &Locations[ i ], sizeof( MYKI_CD_Locations_t ) );
            return TRUE;
    }
            i++;
        }
    }
    return FALSE;
}


int MYKI_CD_getFaresStoredValueStructure( U8_t zoneLow, U8_t zoneHigh, MYKI_CD_FaresStoredValue_t* faresStoredValue )
{
    faresStoredValue->zone_low      = zoneLow;
    faresStoredValue->zone_high     = zoneHigh;
    faresStoredValue->single_trip   = 854;
    faresStoredValue->nhour         = 854;
    faresStoredValue->daily         = 1108;
    faresStoredValue->weekly        = ( faresStoredValue->daily * 5 );
    faresStoredValue->weekend       = ( faresStoredValue->daily * 2 );

    return TRUE;
}

int MYKI_CD_getFaresEpassStructure( U8_t zoneLow, U8_t zoneHigh, U16_t days, MYKI_CD_FaresEpass_t *faresEpassValue )
{
    faresEpassValue->zone_low       = zoneLow;
    faresEpassValue->zone_high      = zoneHigh;
    faresEpassValue->days           = days;
    faresEpassValue->value          = 0;

    return TRUE;
}

int MYKI_CD_getnHourPeriodMinutes(U16_t* value)
{
     *value = (2 * 60);
    return TRUE;
}

int MYKI_CD_getnHourEveningCutoff(U16_t* value)
{
    *value = 1800;
    return TRUE;
}

// Actionlist isCardActionlist
int MYKI_ACTIONLIST_isCardActionlist(const char * cardUID)
{
    return FALSE; // TODO Implement Stub
}

// Card Actionlist Requests
int MYKI_ACTIONLIST_getCardActionlistRequests(const char * cardUID, MYKI_ACTIONLIST_Card_Actionlist_Requests_t * cardActionlistRequests)
{
    return FALSE; // TODO Implement Stub
}

// TAppUpdateRequestNone
int MYKI_ACTIONLIST_getTAppUpdateRequestNone(const char * cardUID, int actionSeqNo, MYKI_ACTIONLIST_TAppUpdateRequest_t * tAppUpdateRequest)
{
    return FALSE; // TODO Implement Stub
}

// TAppUpdateRequestActivate
int MYKI_ACTIONLIST_getTAppUpdateRequestActivate(const char * cardUID, int actionSeqNo, MYKI_ACTIONLIST_TAppUpdateRequest_t * tAppUpdateRequest)
{
    return FALSE; // TODO Implement Stub
}

// TAppUpdateRequestBlock
int MYKI_ACTIONLIST_getTAppUpdateRequestBlock(const char * cardUID, int actionSeqNo, MYKI_ACTIONLIST_TAppUpdateRequest_t * tAppUpdateRequest)
{
    return FALSE; // TODO Implement Stub
}

// TAppUpdateRequestUnblock
int MYKI_ACTIONLIST_getTAppUpdateRequestUnblock(const char * cardUID, int actionSeqNo, MYKI_ACTIONLIST_TAppUpdateRequest_t * tAppUpdateRequest)
{
    return FALSE; // TODO Implement Stub
}

// ProductUpdateRequestNone
int MYKI_ACTIONLIST_getProductUpdateRequestNone(const char * cardUID, int actionSeqNo, MYKI_ACTIONLIST_ProductUpdateRequest_t * productUpdateRequest)
{
    return FALSE; // TODO Implement Stub
}

// ProductUpdateRequestBlock
int MYKI_ACTIONLIST_getProductUpdateRequestBlock(const char * cardUID, int actionSeqNo, MYKI_ACTIONLIST_ProductUpdateRequest_t * productUpdateRequest)
{

    return FALSE; // TODO Implement Stub
}

// ProductUpdateRequestUnblock
int MYKI_ACTIONLIST_getProductUpdateRequestUnblock(const char * cardUID, int actionSeqNo, MYKI_ACTIONLIST_ProductUpdateRequest_t * productUpdateRequest)
{
    return FALSE; // TODO Implement Stub
}

// TPurseLoadRequestNone
int MYKI_ACTIONLIST_getTPurseLoadRequestNone(const char * cardUID, int actionSeqNo, MYKI_ACTIONLIST_TPurseLoadRequest_t * tPurseLoadRequest)
{
    return FALSE; // TODO Implement Stub
}

// TPurseLoadRequestDebit
int MYKI_ACTIONLIST_getTPurseLoadRequestDebit(const char * cardUID, int actionSeqNo, MYKI_ACTIONLIST_TPurseLoadRequest_t * tPurseLoadRequest)
{
    return FALSE; // TODO Implement Stub
}

// ProductSaleRequestNone
// ProductSaleRequestNone
int MYKI_ACTIONLIST_getProductSaleRequestNone(const char * cardUID, int actionSeqNo, MYKI_ACTIONLIST_ProductSaleRequest_t * productSaleRequests)
{
    return FALSE; // TODO Implement Stub
}

// TPurseUpdateRequestNone
int MYKI_ACTIONLIST_getTPurseUpdateRequestNone(const char * cardUID, int actionSeqNo, MYKI_ACTIONLIST_TPurseUpdateRequest_t * purseUpdateRequest)
{
    return FALSE; // TODO Implement Stub
}

// TPurseUpdateRequestNone
int MYKI_ACTIONLIST_getTPurseUpdateRequestBlock(const char * cardUID, int actionSeqNo, MYKI_ACTIONLIST_TPurseUpdateRequest_t * purseUpdateRequest)
{
    return FALSE; // TODO Implement Stub
}

// TPurseUpdateRequestNone
int MYKI_ACTIONLIST_getTPurseUpdateRequestUnblock(const char * cardUID, int actionSeqNo, MYKI_ACTIONLIST_TPurseUpdateRequest_t * purseUpdateRequest)
{
    return FALSE; // TODO Implement Stub
}

// OAppUpdateRequestBlock
int MYKI_ACTIONLIST_getOAppUpdateRequestBlock(const char * cardUID, int actionSeqNo, MYKI_ACTIONLIST_OAppUpdateRequest_t * oAppUpdateRequest)
{
    return FALSE; // TODO Implement Stub
}

// OAppUpdateRequestUnblock
int MYKI_ACTIONLIST_getOAppUpdateRequestUnblock(const char * cardUID, int actionSeqNo, MYKI_ACTIONLIST_OAppUpdateRequest_t * oAppUpdateRequest)
{
    return FALSE; // TODO Implement Stub
}

static Time_t CD_startDateTime = 0;
void MYKI_CD_setStartDateTime(Time_t dateTime)
{
    CD_startDateTime = dateTime;
}

Time_t MYKI_CD_getStartDateTime(void)
{
    return CD_startDateTime;
}
static int earlyBirdProductId = 0;
void MYKI_CD_setEarlyBirdProductId(int id)
{
    earlyBirdProductId = id;
}

int MYKI_CD_getEarlyBirdProductId(void)
{
    return earlyBirdProductId;
}

static int earlyBirdCutOffTime = 0;
void MYKI_CD_setEarlyBirdCutOffTime(int cutOffTime)
{
    earlyBirdCutOffTime = cutOffTime;
}

int MYKI_CD_getEarlyBirdCutOffTime(void)
{
    return earlyBirdCutOffTime;
}

static U32_t hostListedSamId = 0;
static int   hostListedSamIdSet = FALSE;

int MYKI_ACTIONLIST_isDeviceHotlisted(U32_t samId, DateC19_t requestDate)
{
    if (hostListedSamIdSet && (hostListedSamId == samId))
    {
        return 1;
    }
    return 0;
}

void MYKI_ACTIONLIST_setDeviceHotlisted(U32_t samId)
{
    hostListedSamId = samId;
    hostListedSamIdSet = TRUE;
}

/*----------------------------------------------------------------------------
** FUNCTION           : DebugShowDifferentialPricingRequest
**
** DESCRIPTION        : Debug display of the contents of a MYKI_CD_DifferentialPricingRequest_t
**
**
** INPUTS             : request                     - Ref to request to display
**                      localResult                 - Local result. Use only to control the output
**
** RETURNS            : Nothing
**
----------------------------------------------------------------------------*/

void DebugShowDifferentialPricingRequest(const MYKI_CD_DifferentialPricingRequest_t* request, void *localResult)
{
    if (!request)
    {
        CsErrx( "MYKI_CD_getDifferentialPriceStructure: NULL request" );
        return;
    }

    //  Display Request, but only if we have data to return
    if (localResult != NULL)
    {
        CsDbg( BRLL_RULE, "MYKI_CD_getDifferentialPriceStructure: Request" );
        CsDbg( BRLL_RULE, "    rule_type:%d", request->rule_type );
        CsDbg( BRLL_RULE, "    passenger_type_null:%d", request->passenger_type_null );
        CsDbg( BRLL_RULE, "    passenger_type:%d", request->passenger_type );
        CsDbg( BRLL_RULE, "    scanon_minutes_since_midnight:%d", request->scanon_minutes_since_midnight );
        CsDbg( BRLL_RULE, "    scanoff_minutes_since_midnight:%d", request->scanoff_minutes_since_midnight );
        CsDbg( BRLL_RULE, "    day_of_week:%d", request->day_of_week );
        CsDbg( BRLL_RULE, "    day_of_month:%d", request->day_of_month );
        CsDbg( BRLL_RULE, "    month:%d", request->month );
        CsDbg( BRLL_RULE, "    zone_low:%d", request->zone_low );
        CsDbg( BRLL_RULE, "    zone_high:%d", request->zone_high );
        CsDbg( BRLL_RULE, "    trip_direction:%d", request->trip_direction );
        CsDbg( BRLL_RULE, "    route_count:%d", request->route_count );
        CsDbg( BRLL_RULE, "    routes:%p", request->routes );
    }
}

/*----------------------------------------------------------------------------
** FUNCTION           : MYKI_CD_getDifferentialPriceStructure
**
** DESCRIPTION        : Local test stub to override CD function of the same name
**
**                      At the start of each test MYKI_CD_setDifferentialPriceStructure
**                      will be called to reset the stored data returned to the user
**
** INPUTS             : input               - Displayed but not used
**                      differentialPricing - Ref to structure to fill
**
** RETURNS            : TRUE                - Data is present
**                      FALSE               - No Data is Persent
**
----------------------------------------------------------------------------*/

static MYKI_CD_DifferentialPricing_t *MYKI_CD_DifferentialPricing = NULL;

int MYKI_CD_getDifferentialPriceStructure(
    const MYKI_CD_DifferentialPricingRequest_t* input,
    MYKI_CD_DifferentialPricing_t*              differentialPricing )
{
    DebugShowDifferentialPricingRequest(input, MYKI_CD_DifferentialPricing);

    if (input && MYKI_CD_DifferentialPricing)
    {
        memcpy(differentialPricing,MYKI_CD_DifferentialPricing, sizeof(*differentialPricing));
        return TRUE;
    }

    return FALSE;
}

/*----------------------------------------------------------------------------
** FUNCTION           : MYKI_CD_setDifferentialPriceStructure
**
** DESCRIPTION        : Test Stub support function to configure the data returned
**                      by MYKI_CD_getDifferentialPriceStructure()
**
**
** INPUTS             : differentialPricing - Ref to structure to be returned by
**                                            calls to MYKI_CD_getDifferentialPriceStructure.
**                                            NULL is allowed and will cause MYKI_CD_getDifferentialPriceStructure
**                                            to return FALSE. ie: No Data
**
** RETURNS            : Nothing
**
----------------------------------------------------------------------------*/

void MYKI_CD_setDifferentialPriceStructure(MYKI_CD_DifferentialPricing_t *differentialPricing)
{
    MYKI_CD_DifferentialPricing = differentialPricing;
}

/*----------------------------------------------------------------------------
** FUNCTION           : MYKI_CD_getDifferentialPriceProvisionalStructure
**
** DESCRIPTION        : Local test stub to override CD function of the same name
**
**                      At the start of each test MYKI_CD_setDifferentialPriceStructure
**                      will be called to reset the stored data returned to the user
**
** INPUTS             : input               - Displayed but not used
**                      differentialPricing - Ref to structure to fill
**
** RETURNS            : TRUE                - Data is present
**                      FALSE               - No Data is Persent
**
----------------------------------------------------------------------------*/

static MYKI_CD_DifferentialPricingProvisional_t *MYKI_CD_DifferentialPricingProvisional = NULL;

int MYKI_CD_getDifferentialPriceProvisionalStructure(
    const MYKI_CD_DifferentialPricingRequest_t* input,
          MYKI_CD_DifferentialPricingProvisional_t *differentialPricing )
{
    DebugShowDifferentialPricingRequest(input, MYKI_CD_DifferentialPricingProvisional);

    if (input && MYKI_CD_DifferentialPricingProvisional)
    {
        memcpy(differentialPricing,MYKI_CD_DifferentialPricingProvisional, sizeof(*differentialPricing));
        return TRUE;
    }

    return FALSE;
}

/*----------------------------------------------------------------------------
** FUNCTION           : MYKI_CD_setDifferentialPriceProvisionalStructure
**
** DESCRIPTION        : Test Stub support function to configure the data returned
**                      by MYKI_CD_getDifferentialPriceProvisionalStructure()
**
**
** INPUTS             : differentialPricing - Ref to structure to be returned by
**                                            calls to MYKI_CD_getDifferentialPriceProvisionalStructure.
**                                            NULL is allowed and will cause MYKI_CD_getDifferentialPriceProvisionalStructure
**                                            to return FALSE. ie: No Data
**
** RETURNS            : Nothing
**
----------------------------------------------------------------------------*/

void MYKI_CD_setDifferentialPriceProvisionalStructure(MYKI_CD_DifferentialPricingProvisional_t *differentialPricing)
{
    MYKI_CD_DifferentialPricingProvisional = differentialPricing;
}

/*----------------------------------------------------------------------------
** FUNCTION           : MYKI_CD_getRouteStopsStructure
**
** DESCRIPTION        : Test Stub support function to configure the data returned
**                      by MYKI_CD_getRouteStopsStructure()
**
**
** INPUTS             : routeId         Numeric ID of the route
**                      fromStopId      Numeric ID of the first stop on the route to retrieve
**                      toStopId        Numeric ID of the last stop on the route to retrieve
**                      pRouteStops     Pointer to a pointer which will be updated to point to the retrieved array of route stops
**
** RETURNS            : The number of elements placed into pRouteStops (ie the
**                      number of stops retrieved), or (-1) on error.
**
**  NOTES
**  -   Algorithm assumes stop IDs are in a continuous ascending sequence - ie, 1, 2, 3, ...
**
----------------------------------------------------------------------------*/

int MYKI_CD_getRouteStopsStructure(
    U16_t                   routeId,
    U8_t                    fromStopId,
    U8_t                    toStopId,
    MYKI_CD_RouteStop_t   **pRouteStops )
{
    MYKI_CD_RouteStop_t    *pMYKI_CD_RouteStops     = NULL;
    int                     nMYKI_CD_RouteStops     = 0;
    int                     fromIndex               = 0;
    int                     toIndex                 = 0;

    if ( pRouteStops == NULL )
        return -1;

    switch ( routeId )
    {
    case 1:
        pMYKI_CD_RouteStops = RouteStops_1;
        nMYKI_CD_RouteStops = DIMOF( RouteStops_1 );
        break;

    case 2:
        pMYKI_CD_RouteStops = RouteStops_2;
        nMYKI_CD_RouteStops = DIMOF( RouteStops_2 );
        break;

    default:
        break;
    }

    while ( fromIndex < nMYKI_CD_RouteStops && pMYKI_CD_RouteStops[ fromIndex ].stopId != fromStopId )
        fromIndex++;

    if ( fromIndex >= nMYKI_CD_RouteStops )
    {
        *pRouteStops = NULL;
        return 0;
    }
    *pRouteStops    = &pMYKI_CD_RouteStops[ fromIndex ];

    toIndex = fromIndex + 1;
    while ( toIndex < nMYKI_CD_RouteStops && pMYKI_CD_RouteStops[ toIndex ].stopId <= toStopId )
        toIndex++;

    return toIndex - fromIndex;
}

