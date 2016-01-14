//=============================================================================
//
//  Common functionality used by other modules within this package.
//
//  This module is near the bottom of the dependency tree within this package.
//  It's only current dependency is on MYKI_BR_context_data.h.
//
//=============================================================================
#ifndef BR_COMMON_H_
#define BR_COMMON_H_

#ifdef __cplusplus
    extern "C" {
#endif

//=============================================================================
//
//  Includes
//
//=============================================================================

//#include <cs.h>                           // Only for CSDEBUGx, which we're not using anyway
#include <myki_cardservices.h>              // Myki_CS  - Card Services

#include <myki_cdd_enums.h>                 // Myki_LDT - Logical Device Transactions. Named constants for contents of card fields.
#include "myki_br_card_constants.h"         // Equivalent of myki_cdd_enums.h - Deprecated. KWS: TODO: Change all occurrences of these constants to the myki_cdd_enums.h equivalents.

#include "myki_br_context_data.h"

//=============================================================================
//
//  Macros / Constants
//
//=============================================================================

#ifndef FALSE
#define FALSE   0
#endif
#ifndef TRUE
#define TRUE    (!FALSE)
#endif

#ifndef DIMOF
#define DIMOF( a )      ( sizeof( a ) / sizeof( (a)[0] ) )
#endif

#ifndef BIT
#define BIT( b )        ( 1 << (b) )
#endif

#ifndef TF
#define TF( val )   ( (val) ? "TRUE" : "FALSE" )
#endif

#define MINUTES_TO_SECONDS( m )                     ( (m) * 60 )
#define HOURS_TO_SECONDS( h )                       ( (h) * 60 * 60 )
#define DAYS_TO_SECONDS( d )                        ( (d) * 24 * 60 * 60 )
#define HOURS_MINUTES_TO_SECONDS( h, m )            ( HOURS_TO_SECONDS( h ) + MINUTES_TO_SECONDS( m ) )
#define DAYS_HOURS_MINUTES_TO_SECONDS( d, h, m )    ( DAYS_TO_SECONDS( d ) + HOURS_TO_SECONDS( h ) + MINUTES_TO_SECONDS( m ) )

//
//  Convenience macro to adjust a low/high zone pair for CitySaver zones,
//  because this is done in a lot of places with different variables for the zone pairs.
//  The do/while wrapper is so we can use the macro anywhere we can use a normal function call.
//  Note that the high zone will never be extended because the current system only supports a single
//  CitySaver zone in zone 1 and there are no longer any zone 1 - 1 (ie CitySaver) products.
//

#define ADJUST_FOR_CITYSAVER( zoneLow, zoneHigh )               \
    do                                                          \
    {                                                           \
        if ( MYKI_CD_isCitySaverZone( (U8_t)( zoneLow - 1 ) ) ) \
            zoneLow--;                                          \
        if ( MYKI_CD_isCitySaverZone( (U8_t)( zoneHigh + 1 ) ) )\
            zoneHigh++;                                         \
    }                                                           \
    while (0)

//  BR Logging Levels
//  Note: CSDEBUGx is defined in <cslog.h> but cannot be included in plain C file becuase of c++ overloads

#define BRLL_SESSION     3          // CSDEBUG3 - Open / close per card session
#define BRLL_RULE        4          // CSDEBUG4 - Entry / exit per rule
#define BRLL_CHOICE      5          // CSDEBUG5 - Decision point within rule
#define BRLL_FUNC        6          // CSDEBUG6 - Subfunction within rule
#define BRLL_FUNC_CHOICE 7          // CSDEBUG7 - Decision point within subfunction
#define BRLL_FIELD       8          // CSDEBUG8 - Field level logging - function arguments, structure dumps, etc

    /** Day of week (value of tm.tm_wday) */
typedef enum
{
    DayOfWeek_Sunday = 0,
    DayOfWeek_Monday,
    DayOfWeek_Tuesday,
    DayOfWeek_Wednesday,
    DayOfWeek_Thursday,
    DayOfWeek_Friday,
    DayOfWeek_Saturday,
    DayOfWeek_DaysOfWeek

}   DayOfWeek_e;

typedef enum {
    TxType_EntryExit = 7,
    TxType_ExitOnly = 18,
    TxType_EntryOnly = 19,
    TxType_OnBoard = 20,
    TxType_Unknown = 0xff
} TxType_e;

typedef enum
{
    PRODUCT_CARD_TYPE_UNKNOWN,
    PRODUCT_CARD_TYPE_DSC,
    PRODUCT_CARD_TYPE_LLSC

}   ProductCardType_e;

typedef enum
{
    ACTION_NULL,
    ACTION_TAPP_UPDATE_BLOCK,
    ACTION_TAPP_UPDATE_UNBLOCK,
    ACTION_TAPP_UPDATE_ACTIVATE,
    ACTION_TAPP_UPDATE_NONE,
    ACTION_PRODUCT_UPDATE_BLOCK,
    ACTION_PRODUCT_UPDATE_UNBLOCK,
    ACTION_PRODUCT_UPDATE_NONE,
    ACTION_TPURSE_UPDATE_BLOCK,
    ACTION_TPURSE_UPDATE_UNBLOCK,
    ACTION_TPURSE_UPDATE_NONE,
    ACTION_TPURSE_LOAD_DEBIT,
    ACTION_TPURSE_LOAD_NONE,
    ACTION_PRODUCT_SALE_NONE,
    ACTION_OAPP_UPDATE_BLOCK,
    ACTION_OAPP_UPDATE_UNBLOCK

}   ActionlistType_e;

typedef enum
{
    MYKI_BR_FARETYPE_SINGLE_PEAK,
    MYKI_BR_FARETYPE_PROVISIONAL

}   myki_br_FareType_t;

/*----------------------------------------------------------------------------
** STRUCTURE          : ProductIterator
**
** DESCRIPTION        : A structure used to iterate over the products
**                      It also holds useful information about the product
**                      such that all the information required is in the
**                      structure and can be passed around.
**
**                      See: myki_br_InitProductIterator
**                           myki_br_ProductIterate
**
----------------------------------------------------------------------------*/

typedef struct
{
    MYKI_TAControl_t   *pMYKI_TAControl;            // Ref to MYKI_TAIssuer_t record in internal data store
    ProductType_e       pType;                      // Required Product Type
    int                 error;                      // Error encountered

    // Maintaied as we traverse the products
    ProductType_e       currentProduct;             // Current product Type
    MYKI_TAProduct_t   *pProduct;
    int                 index;                      // Dir Index
    MYKI_Directory_t   *pDirectory;

} ProductIterator;

//=============================================================================
//
//  Function prototypes
//
//=============================================================================

//                  General utility functions

    /**
     *      Checks scan on location is current location OR on the same route.
     *      @param  pData           BR context data.
     *      @param  pMYKI_TAProduct product to test against.
     *      @param  bCheckSameStop  TRUE if must be same stop; FALSE on same route only.
     */
int                 myki_br_CurrentLocationIsScanOnLocationOrRoute(
                            MYKI_BR_ContextData_t  *pData,
                            MYKI_TAProduct_t       *pMYKI_TAProduct,
                            int                     bCheckSameStop );
int                 myki_br_CurrentLocationIsScanOnLocation( MYKI_BR_ContextData_t *pData, MYKI_TAProduct_t *pMYKI_TAProduct );
MYKI_BR_LogTransactionType_t    myki_br_getTransactionType( MYKI_BR_ContextData_t *pData );
int                 myki_br_GetCardProduct( int dirIndex, MYKI_Directory_t **pDirectory, MYKI_TAProduct_t **pProduct );
TripDirection_t     myki_br_getProductTripDirection( MYKI_TAProduct_t *pProduct );
int                 myki_br_setProductTripDirection( MYKI_TAProduct_t *pProduct, TripDirection_t direction );
int                 myki_br_setProductProvisionalStatus( MYKI_TAProduct_t *pProduct, int status );
int                 myki_br_setProductBorderStatus( MYKI_TAProduct_t *pProduct, int status );
int                 myki_br_setProductOffPeakStatus( MYKI_TAProduct_t *pProduct, int status );
int                 myki_br_GetOldestNHourDirContained( MYKI_TAControl_t *pMYKI_TAControl, int zoneLow, int zoneHigh );
int                 myki_br_GetOldestNHourDirOverlap( MYKI_TAControl_t *pMYKI_TAControl, int zoneLow, int zoneHigh );
int                 myki_br_GetOldestNHourDir( MYKI_TAControl_t *pMYKI_TAControl );
U8_t                myki_br_GetDirWithSerial( MYKI_TAControl_t    *pMYKI_TAControl, U32_t productSerialNo);
int                 myki_br_CreateZoneMap( MYKI_BR_ContextData_t *pData, int zoneRangeLow, int zoneRangeHigh );
int                 myki_br_CalculateCombinedProductValue( MYKI_BR_ContextData_t *pData, int passengerCode, Currency_t *pPeak, Currency_t *pOffPeak );
int                 myki_br_IsFullCoverage( MYKI_BR_ContextData_t *pData, int zoneLow, int zoneHigh );

    /**
     *  Determines ePass product duration.
     *  @param  pDirectory product directory entry.
     *  @param  pProduct product object.
     *  @return ePass product duration if successful; TIME_NOT_SET otherwise.
     */
int                 myki_br_GetEpassDuration(
                            MYKI_Directory_t       *pDirectory,
                            MYKI_TAProduct_t       *pProduct );

int                 myki_br_GetProductDuration( MYKI_BR_ContextData_t *pData, ProductType_e productType );
char                *myki_br_GetProductTypeName( ProductType_e productType );
int                 myki_br_SetBitmapDirection( U8_t *pBitmap, TripDirection_t direction );
int                 myki_br_SetBitmapBorderStatus( U8_t *pBitmap, BorderStatus_t borderStatus );

void                myki_br_InitProductIterator(MYKI_TAControl_t *pMYKI_TAControl, ProductIterator *pIter, int pType);
int                 myki_br_ProductIterate(ProductIterator *pIter);
Time_t              myki_br_getCommonDate(MYKI_BR_ContextData_t *pData);
int                 myki_br_isPhysicalZoneOneOrZero(U8_t lzone);

int myki_br_buildDifferencePriceRequest
(
    MYKI_BR_ContextData_t                *pData,
    MYKI_CD_DifferentialPricingRequest_t *MYKI_CD_DifferentialPricingRequest,
    int                                   nZoneLow,
    int                                   nZoneHigh,
    int                                   nPassengerCode,
    TripDirection_t                       nTripDirection,
    Time_t                                nScanOnDateTime,
    Time_t                                nScanOffDateTime
);


    /**
     *      Determines Stored Value fares. If successful, the following data
     *      shall be updated,
     *
     *      Tariff.DailyCapValue
     *      Tariff.WeeklyCapValue
     *      Tariff.EntitlementZoneLow
     *      Tariff.EntitlementZoneHigh
     *      Tariff.EntitlementProduct
     *      Tariff.EntitlementValue
     *
     *      @param  pApplicableFare returned applicable fare.
     *      @param  pData BR context data.
     *      @param  nZoneLow zone low.
     *      @param  nZoneHigh zone high.
     *      @param  nPassengerCode passenger type code.
     *      @param  nTripDirection trip direction.
     *      @param  bFareRouteIdIsValid not used.
     *      @param  nFareRouteId not used.
     *      @return 0 if successful; -1 otherwise.
     */
int                 myki_br_getFareStoredValueEx(
                            Currency_t*                 pApplicableFare,
                            MYKI_BR_ContextData_t*      pData,
                            int                         nZoneLow,
                            int                         nZoneHigh,
                            int                         nPassengerCode,
                            TripDirection_t             nTripDirection,
                            int                         bFareRouteIdIsValid,
                            int                         nFareRouteId,
                            Time_t                      nScanOnDateTime,
                            Time_t                      nScanOffDateTime );
#define             myki_br_getFareStoredValue( pData, nZoneLow, nZoneHigh, nPassengerCode, nTripDirection, bFareRouteIdIsValid, nFareRouteId, nScanOnDateTime, nScanOffDateTime )\
                            myki_br_getFareStoredValueEx( NULL, pData, nZoneLow, nZoneHigh, nPassengerCode, nTripDirection, bFareRouteIdIsValid, nFareRouteId, nScanOnDateTime, nScanOffDateTime )

int                 myki_br_ClearProposedMergeTripDirection( MYKI_BR_ContextData_t *pData );
int                 myki_br_UpdateProposedMergeTripDirection( MYKI_BR_ContextData_t *pData, TripDirection_t newDirection );

int                 myki_br_GetCardSerialNumberString  ( unsigned char* cardUIDUnformatted, char* pSerialNumberBuffer, int length );

    /**
     *      Searches for actionlist record of specified search criteria.
     *
     *      @param  cardUIDUnformatted          card UID to search.
     *      @param  searchType                  actionlist type to search.
     *      @param  searchActionSequenceNumber  desired action sequence number to search.
     *      @param  searchProductSerialNumber   desired product serial number to search, 0=don't care.
     *      @return -1 if failed retrieving actionlist; 0 if desired actionlist not found; 1 if desired actionlist found.
     */
int                 myki_br_GetActionlist(
                            unsigned char              *cardUIDUnformatted,
                            ActionlistType_e            searchType,
                            U8_t                        searchActionSequenceNumber,
                            U16_t                       searchProductSerialNumber,
                            MYKI_BR_ContextData_t      *pData );

    /**
     *      Rounds up the specified time to the next Tariff.nHourRoundingPortion.
     *
     *      @param  pData BR context data.
     *      @param  pTime time.
     */
void                myki_br_RoundUpEndTime( MYKI_BR_ContextData_t *pData, Time_t *pTime );

    /**
     *      Determines end of business date/time (UTC) of given date/time (UTC).
     *
     *      @param  pData BR context data.
     *      @param  dateTime date/time.
     *      @param  endOfWeek TRUE if returning end of bussiness week;
     *              FALSE if returning end of business day.
     *      @return end of business day/week date/time of given date/time.
     */
Time_t              myki_br_EndOfBusinessDateTime( MYKI_BR_ContextData_t* pData, Time_t dateTime, int endOfWeek );

    /**
     *      Determines the UTC Start of Business Date/Time given UTC.
     *
     *      @param  pData BR context data.
     *      @param  dateTime date/time.
     *      @return start of business day date/time of given date/time.
     */
Time_t              myki_br_StartOfBusinessDateTime(MYKI_BR_ContextData_t*  pData, Time_t dateTime );

    /**
     *      Converts UTC to MYKI date, ie. days since "Reference Date" which is 01/01/2006.
     *
     *      @param  pData BR context data.
     *      @param  dateTime date/time.
     *      @return Converted MYKI date.
     */
DateC19_t           myki_br_ToMykiDate( MYKI_BR_ContextData_t* pData, Time_t dateTime );

    /**
     *      Determines if specified passenger type code is a DDA passenger type.
     *
     *      @param  PassengerCode passenger type code.
     *      @return <0 if failed retrieving passenger type record;
     *              0 if passenger type is not DDA;
     *              >0 if passenger type is DDA.
     */
int                 myki_br_IsDDA( U8_t PassengerCode );

//
//  Return the discounted fare given a full fare and a percentage discount.
//

Currency_t          CalculateDiscountedFare( Currency_t fullFare, int percentDiscount );

//
//  Extend an existing product - also allows various bits in the product control bitmap to be changed
//

int                 myki_br_ExtendProduct
(
    MYKI_BR_ContextData_t   *pData,
    MYKI_Directory_t        *pDirectory,
    MYKI_TAProduct_t        *pProduct,
    int                     zoneLow,
    int                     zoneHigh,
    Currency_t              purchaseValue,
    int                     additionalHours,
    U8_t                    bitmap,
    int                     alwaysCreateProductUpdateNone
);

int                 myki_br_ZonesTravelled( int zoneLow, int zoneHigh );
int                 myki_br_GetAdditionalMinutes( MYKI_BR_ContextData_t *pData, int zoneLow, int zoneHigh );

int                 myki_br_AddLocationToTransportLocations( MYKI_BR_ContextData_t *pData, MYKI_CD_Locations_t *pLocation );
int                 myki_br_AddStationToTransportLocations( MYKI_BR_ContextData_t *pData, int stationId );

    /**
     *      Adds location at Route/Stop to transport locations.
     *
     *      @param  pData BR context data.
     *      @param  routeId route ID.
     *      @param  stopId stop ID.
     *      @param  count number of times to add the location
     *      @return 0 if successful;
     *              <0 if failed
     */
int                 myki_br_AddStopToTransportLocationsEx( MYKI_BR_ContextData_t *pData, int routeId, int stopId, int count );
int                 myki_br_AddStopToTransportLocations( MYKI_BR_ContextData_t *pData, int routeId, int stopId );

    /**
     *      Adds locations at Route/Stop range to transport locations.
     *
     *      @param  pData BR context data.
     *      @param  routeId route ID.
     *      @param  fromStopId from stop ID (inclusive), 0 if first stop.
     *      @param  toStopId to stop ID (inclusive), 0 if last stop.
     *      @return 0 if successful;
     *              <0 if failed
     */
int                 myki_br_AddStopRangeToTransportLocations( MYKI_BR_ContextData_t *pData, int routeId, int fromStopId, int toStopId );

int                 myki_br_SetTripDirectionFromZones( MYKI_BR_ContextData_t *pData, int originZone, int DestinationZone );
int                 myki_br_CommonLines( MYKI_BR_ContextData_t *pData, int stationId1, int stationId2 );
int                 myki_br_ZonesOverlapOrAdjacent( int zone1Low, int zone1High, int zone2Low, int zone2High );
int                 myki_br_ZonesOverlap( int zone1Low, int zone1High, int zone2Low, int zone2High );
int                 myki_br_CalculateEPassCappingContribution( MYKI_BR_ContextData_t *pData, int passengerCode );

typedef enum
{
    CHECK_PROVISION_PRODUCTZONE_COVERAGE_ERROR                     = -1,   // General Error indication. All errors will be <0
    CHECK_PROVISION_PRODUCTZONE_COVERAGE_PARTIAL                   = 1,    // Partial Coverage
    CHECK_PROVISION_PRODUCTZONE_COVERAGE_FULL                      = 2,    // Full Coverage
    CHECK_PROVISION_PRODUCTZONE_COVERAGE_FULL_WITH_INACTIVE_EPASS  = 3,    // Full Coverage, but only with inactive epass (and there is no active ePass)
} myki_br_CheckProvisionProductZoneCoverage_t;

myki_br_CheckProvisionProductZoneCoverage_t myki_br_CheckProvisionProductZoneCoverage( MYKI_BR_ContextData_t *pData, MYKI_TAControl_t *pMYKI_TAControl, MYKI_TAProduct_t *pProductInUse, int *pPass);

Time_t  myki_br_NHourEndDateTimeEx( MYKI_BR_ContextData_t *pData, Time_t startDateTime, int duration, int periodExtension );
int     myki_br_HasActiveEpass( MYKI_BR_ContextData_t *pData, MYKI_TAControl_t *pMYKI_TAControl );

    /**
     *      Retrieves scan-on location details.
     *      @param  pLocation returned location details.
     *      @param  pProductInUse product in use object.
     *      @return 0 if success; -1 otherwise.
     */
int                 myki_br_GetScanOnLocation( MYKI_CD_Locations_t *pLocation, const MYKI_TAProduct_t *pProductInUse );

    /**
     *      Creates card image snapshot.
     *      @param  pData BR context data.
     *      @return 0 if success; failed otherwise.
     */
int                 myki_br_CreateCardSnapshot( MYKI_BR_ContextData_t *pData );

//=============================================================================
//
//                  LDT Wrappers
//
//=============================================================================

//                  Product Sale


//int                 myki_br_ldt_ProductSaleAL_deprecated( MYKI_BR_ContextData_t *pData, ProductSale_t *pRequest );
int                 myki_br_ldt_ProductSaleEx( MYKI_BR_ContextData_t *pData, ProductSale_t *pRequest );
int                 myki_br_ldt_ProductSale( MYKI_BR_ContextData_t *pData, int productId, int zoneLow, int zoneHigh, int purchaseValue, int isProvisional );
int                 myki_br_ldt_ProductSale_UpgradeEx( MYKI_BR_ContextData_t *pData, MYKI_Directory_t *pDirectory, int productId, int zoneLow, int zoneHigh, int purchaseValue, Time_t expiryDateTime, int clearProvisional );
int                 myki_br_ldt_ProductSale_Upgrade( MYKI_BR_ContextData_t *pData, MYKI_Directory_t *pDirectory, int productId, int zoneLow, int zoneHigh, int purchaseValue, Time_t expiryDateTime );
int                 myki_br_ldt_ProductSale_Upgrade_SameProduct( MYKI_BR_ContextData_t *pData, MYKI_Directory_t *pDirectory, int zoneLow, int zoneHigh, int purchaseValue, Time_t expiryDateTime );
int                 myki_br_ldt_ProductSale_Reverse( MYKI_BR_ContextData_t *pData, MYKI_Directory_t *pDirectory );

//                  Product Update

int                 myki_br_ldt_ProductUpdate( MYKI_BR_ContextData_t *pData, MYKI_Directory_t *pDirectory, MYKI_TAProduct_t *pProductOrg, MYKI_TAProduct_t *pProductNew );
int                 myki_br_ldt_ProductUpdate_IfChanged( MYKI_BR_ContextData_t *pData, MYKI_Directory_t *pDirectory, MYKI_TAProduct_t *pProductOrg, MYKI_TAProduct_t *pProductNew );
int                 myki_br_ldt_ProductUpdate_Extend( MYKI_BR_ContextData_t *pData, MYKI_Directory_t *pDirectory, MYKI_TAProduct_t *pProductOld, MYKI_TAProduct_t *pProductNew, int isClearBorderStatus, int showEntryPointId, int includeExtraFields );
int                 myki_br_ldt_ProductUpdate_ClearBorderStatus( MYKI_BR_ContextData_t *pData, MYKI_Directory_t *pDirectory );
int                 myki_br_ldt_ProductUpdate_ClearPremiumSurcharge( MYKI_BR_ContextData_t *pData, MYKI_Directory_t *pDirectory );
int                 myki_br_ldt_ProductUpdate_ClearAutoloadStatus( MYKI_BR_ContextData_t *pData, MYKI_Directory_t *pDirectory );
int                 myki_br_ldt_ProductUpdate_SetTripDirection( MYKI_BR_ContextData_t *pData, MYKI_Directory_t *pDirectory, TripDirection_t direction );
int                 myki_br_ldt_ProductUpdate_SetOffPeak( MYKI_BR_ContextData_t *pData, MYKI_Directory_t *pDirectory );
int                 myki_br_ldt_ProductUpdate_ActivateEx( MYKI_BR_ContextData_t *pData, MYKI_Directory_t *pDirectory, Time_t startDateTime, Time_t endDateTime );
int                 myki_br_ldt_ProductUpdate_ActivateFrom( MYKI_BR_ContextData_t *pData, MYKI_Directory_t *pDirectory, Time_t startDateTime );
int                 myki_br_ldt_ProductUpdate_Activate( MYKI_BR_ContextData_t *pData, MYKI_Directory_t *pDirectory );
int                 myki_br_ldt_ProductUpdate_Invalidate( MYKI_BR_ContextData_t *pData, MYKI_Directory_t *pDirectory );
int                 myki_br_ldt_ProductUpdate_Block_deprecated( MYKI_BR_ContextData_t *pData, MYKI_Directory_t *pDirectory );
int                 myki_br_ldt_ProductUpdate_ExtendExpiryDateTime( MYKI_BR_ContextData_t *pData, MYKI_Directory_t *pDirectory, Time_t expiryDateTime, int showEntryPointId );

//                  Product Usage

int                 myki_br_ldt_ProductUsage_ScanOff( MYKI_BR_ContextData_t *pData, MYKI_Directory_t *pDirectory );
int                 myki_br_ldt_ProductUsage_ScanOnEPass( MYKI_BR_ContextData_t *pData, MYKI_Directory_t *pDirectory );
int                 myki_br_ldt_ProductUsage_ScanOnStoredValue( MYKI_BR_ContextData_t *pData, MYKI_Directory_t *pDirectory );
int                 myki_br_ldt_ProductUsage_Reverse( MYKI_BR_ContextData_t *pData, MYKI_Directory_t *pDirectory );

//                  Purse Load

int                 myki_br_ldt_PurseLoad_Autoload( MYKI_BR_ContextData_t *pData, int amount );
int                 myki_br_ldt_PurseLoad_None( MYKI_BR_ContextData_t *pData, int amount );
int                 myki_br_ldt_PurseLoad_Reverse( MYKI_BR_ContextData_t *pData, int amount );
int                 myki_br_ldt_PurseLoad_Debit_deprecated( MYKI_BR_ContextData_t *pData, Currency_t amount );

//                  Purse Update

int                 myki_br_ldt_PurseUpdate_None( MYKI_BR_ContextData_t *pData, int amount, int isAutoload, int autoThreshold );
int                 myki_br_ldt_PurseUpdate_Block_deprecated( MYKI_BR_ContextData_t *pData );

//                  Purse Usage

int                 myki_br_ldt_PurseUsage_Penalty( MYKI_BR_ContextData_t *pData, int amount );     // Positive amount to deduct
int                 myki_br_ldt_PurseUsage_Transit( MYKI_BR_ContextData_t *pData, int amount );     // Positive amount to deduct
int                 myki_br_ldt_PurseUsage_Reverse( MYKI_BR_ContextData_t *pData, int amount );     // Positive amount to refund

//                  Capping

int                 myki_br_ldt_PerformCappingUpdate( MYKI_BR_ContextData_t *pData, MYKI_TACapping_t* cappingData, int isDailyUpdate, int isWeeklyUpdate );
int                 myki_br_ldt_PerformCappingReset( MYKI_BR_ContextData_t *pData, int isDailyReset, int isWeeklyReset );
int                 myki_br_ldt_PerformCappingUpdateDailyZones( MYKI_BR_ContextData_t *pData, U8_t zoneValue);


//                  App Update

int                 myki_br_ldt_AppUpdate_deprecated( MYKI_BR_ContextData_t *pData, MYKI_Directory_t *pDirectory );
int                 myki_br_ldt_AppUpdate_Activate_deprecated( MYKI_BR_ContextData_t *pData, MYKI_Directory_t *pDirectory );
int                 myki_br_ldt_AppUpdate_Block( MYKI_BR_ContextData_t *pData, U8_t blockingReason );
int                 myki_br_ldt_AppUpdate_Unblock_deprecated( MYKI_BR_ContextData_t *pData, MYKI_Directory_t *pDirectory );
int                 myki_br_ldt_AppUpdate_SetProductInUse( MYKI_BR_ContextData_t *pData, MYKI_Directory_t *pDirectory );

//                  Usage Log / Load Log

int                 myki_br_ldt_AddUsageLogEntry ( MYKI_BR_ContextData_t *pData );
int                 myki_br_ldt_AddLoadLogEntry( MYKI_BR_ContextData_t *pData );

//                  Operator App Update

int                 myki_br_ldt_OAppUpdate_Block( MYKI_BR_ContextData_t *pData );
int                 myki_br_ldt_OAppUpdate_Unblock( MYKI_BR_ContextData_t *pData );

int                 myki_br_ldt_OAppUpdate_AOCardProcessing( MYKI_BR_ContextData_t *pData );

//                  ShiftData Update
U8_t                myki_br_ldt_GetShiftRecordTypeIndex( U8_t recordType, const MYKI_OAShiftDataRecordList_t *pMYKI_OAShiftDataRecordList , U8_t recordListSize );
int                 myki_br_ldt_OAppShiftUpdate( MYKI_BR_ContextData_t *pData, U8_t recordIndex, MYKI_OAShiftDataControl_t *pMYKI_OAShiftDataControl );
int                 myki_br_ldt_OAppShiftControlUpdate( MYKI_BR_ContextData_t *pData, U8_t activeRecordCount );
int                 myki_br_ldt_AddShiftDataLogEntry( MYKI_BR_ContextData_t *pData );

//					Actionlist Update
int                 myki_br_ldt_Actionlist( MYKI_BR_ContextData_t  *pData);

//                  Failure Response

int                 myki_br_ldt_FailureResponse( MYKI_BR_ContextData_t *pData, MYKI_UD_Type_t eType, int eSubType, MYKI_UD_FailureReason_t eFailureReason );
#define             myki_br_ldt_ProductUsage_ScanOn_FailureResponse( pData, eFailureReason )\
                        myki_br_ldt_FailureResponse( (pData), MYKI_UD_TYPE_PRODUCT_USAGE, (int)MYKI_UD_PRODUCT_USAGE_SUB_TYPE_SCAN_ON, (eFailureReason) )
#define             myki_br_ldt_ProductUpdate_None_FailureResponse( pData, eFailureReason )\
                        myki_br_ldt_FailureResponse( (pData), MYKI_UD_TYPE_PRODUCT_UPDATE, (int)MYKI_UD_PRODUCT_UPDATE_SUB_TYPE_NONE, (eFailureReason) )
#define             myki_br_ldt_ProductUpdate_Block_FailureResponse( pData, eFailureReason )\
                        myki_br_ldt_FailureResponse( (pData), MYKI_UD_TYPE_PRODUCT_UPDATE, (int)MYKI_UD_PRODUCT_UPDATE_SUB_TYPE_BLOCK, (eFailureReason) )
#define             myki_br_ldt_ProductSale_None_FailureResponse( pData, eFailureReason )\
                        myki_br_ldt_FailureResponse( (pData), MYKI_UD_TYPE_PRODUCT_SALE, (int)MYKI_UD_PRODUCT_SALE_SUB_TYPE_NONE, (eFailureReason) )
#define             myki_br_ldt_PurseUpdate_None_FailureResponse( pData, eFailureReason )\
                        myki_br_ldt_FailureResponse( (pData), MYKI_UD_TYPE_TPURSE_UPDATE, (int)MYKI_UD_TPURSE_UPDATE_SUB_TYPE_NONE, (eFailureReason) )
#define             myki_br_ldt_PurseLoad_None_FailureResponse( pData, eFailureReason )\
                        myki_br_ldt_FailureResponse( (pData), MYKI_UD_TYPE_TPURSE_LOAD, (int)MYKI_UD_TPURSE_LOAD_SUB_TYPE_NONE, (eFailureReason) )
#define             myki_br_ldt_PurseLoad_Debit_FailureResponse( pData, eFailureReason )\
                        myki_br_ldt_FailureResponse( (pData), MYKI_UD_TYPE_TPURSE_LOAD, (int)MYKI_UD_TPURSE_LOAD_SUB_TYPE_DEBIT, (eFailureReason) )

//=============================================================================
//
//                  CD Wrappers
//
//=============================================================================

ProductType_e       myki_br_cd_GetProductType( int productId );

    /**
     *  Determines if specified product (ID) is a "FixedePass".
     *  @param  productId product ID
     *  @return TRUE if product is fixed ePass; FALSE otherwise.
     */
int                 myki_br_cd_IsFixedePass( int productId );

int                 myki_br_cd_GetProductId  ( ProductType_e productType );
ProductCardType_e   myki_br_cd_GetProductCardType( int productId );

    /**
     *  Returns text representation of specified transport mode.
     *  @param  transportMode transport mode.
     *  @return text representation of transport mode.
     */
const char*         myki_br_GetTransportModeString( TransportMode_e transportMode );

TransportMode_e     myki_br_cd_GetTransportModeForProvider( int providerId );
int                 myki_br_cd_GetActionList( CT_CardInfo_t *pCardInfo, MYKI_BR_ContextData_t *pData);
int                 myki_br_cd_GetZoneByLocation( int locationId );
int                 myki_br_cd_GetStationDetails( int stationId, MYKI_CD_Stations_t *pStation, MYKI_CD_Locations_t *pLocation );

//=============================================================================

//=============================================================================
//
//                  Staff Roles
//
//=============================================================================


    /**
     *  Checks to see if AO card is presented. AO card has a type of 13 14 or 15 with profile = 1
     *  @param  type	: card type in the card
     *  @param  profile : profile encoded in the caed
     *  @return int		: TRUE if it is an AO, FALSE if it is not an AO
     */
int					myki_br_isAO(MYKI_BR_ContextData_t *pData, int type, int profile);

    /**
     *  @brief  Determines operator role.
     *  Determines if presented operator card is of specified role.
     *  @param  pData business rule context data.
     *  @param  eRole operator role to check for.
     *  @return >0  if operator card is of specified role,\n
     *          0   if operator card is not of specified role, and\n
     *          <0  if failed reading operator card.
     */
int                 myki_br_isOfRole( MYKI_BR_ContextData_t *pData, OperatorType_e eRole );

    /**
     *  @brief  Converts BCD16 (16-bit Binary Coded Decimal) to integer.
     *  @param  value BCD16 value.
     *  @return Converted integer value.
     */
int                 myki_br_Bcd16ToInt( int value );

    /**
     *  @brief  Converts integer to BCD16.
     *  @param  value integer value.
     *  @return Converted BCD16 value.
     */
int                 myki_br_IntToBcd16( int value );

    /**
     *  @brief  Gets shift id from MYKI_OAShiftDataControl_t object.
     *  @param  p MYKI_OAShiftDataControl_t object.
     *  @return Shift id.
     */
#define             myki_br_GetShiftId( p )\
                        ( (p) == NULL ? 0 : ( (int)( (p)->ShiftSeqNo & 0xffff ) << 16 ) | ( (int)(p)->ShiftId & 0xffff ) )

    /**
     *  @brief  Sets shift id given MYKI_OAShiftDataControl_t object.
     *  @param  p MYKI_OAShiftDataControl_t object.
     *  @param  v shift id.
     */
#define             myki_br_SetShiftId( p, v )\
                        if ( (p) != NULL )\
                        {\
                            (p)->ShiftSeqNo = ( ( (v) >> 16 ) & 0xffff );\
                            (p)->ShiftId    = (   (v)         & 0xffff );\
                        }

#ifdef __cplusplus
}
#endif

#endif  // BR_COMMON_H_
