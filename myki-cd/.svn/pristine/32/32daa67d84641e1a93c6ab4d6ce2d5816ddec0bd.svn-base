/* -*- mode: c; tabs: 4; -*- */
/*=============================================================================
**
**    Vix Technology                   Licensed software
**    (C) 2014                         All rights reserved
**
**=============================================================================
**
**  Project/Product : MBU
**  Filename        : myki_cd.h
**  Author(s)       : James Ho
**
**  Description     :
**
**  Member(s)       :
**      MYKI_CD_openCDDatabase      [public]    Opens Tariff database
**      MYKI_CD_closeCDDatabase     [public]    Closes Tariff database
**      MYKI_CD_openDeltaActionlistDatabase
**                                  [public]    Opens DELTA Actionlist database
**      MYKI_CD_openFullActionlistDatabase
**                                  [public]    Opens FULL Actionlist database
**      MYKI_CD_closeDeltaActionlistDatabase
**                                  [public]    Closes DELTA Actionlist database
**      MYKI_CD_closeFullActionlistDatabase
**                                  [public]    Closes FULL Actionlist database
**      MYKI_CD_getProducts         [public]    Retrieves product details
**
**  Information     :
**   Compiler(s)    : C++
**   Target(s)      : Independent
**
**  Subversion      :
**      $Id: $
**      $HeadURL: $
**
**  History         :
**   Vers.  Date        Aut.  Type     Description
**   -----  ----------  ----  -------  ----------------------------------------
**    1.00  10.10.12    JH    Create
**    1.01  03.03.15    ANT   Add      APIs to close databases
**
**===========================================================================*/

#ifndef MYKI_CD_H_
#define MYKI_CD_H_

#include <corebasetypes.h>
#ifdef __cplusplus
#include <json/json.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

#define defaultActionlistDBFile  "actionlist.db"
#define defaultActionlistDeltaDBFile "actionlist_delta.db"


#define LEN_Short_Description_t 50
#define LEN_Long_Description_t 255
#define LEN_Gps_t 20
#define LEN_Mode_t 10
#define LEN_Code_t 5
#define LEN_15_t 15
#define LEN_20_t 20
#define LEN_50_t 50
#define LEN_Value_t 255
#define LEN_6_t 6


typedef struct
{
    U16_t priority;
    char tableType[LEN_15_t]; 
    
    char fieldKey1[LEN_15_t];
    char fieldKey2[LEN_15_t];
    char fieldKey3[LEN_15_t];
    char fieldKey4[LEN_15_t];
    char fieldKey5[LEN_15_t];
    char fieldKey6[LEN_15_t];
    char fieldKey7[LEN_15_t];
    char fieldKey8[LEN_15_t];
    U8_t effect1;
    char textToDisplay[LEN_50_t];
}  MYKI_CD_HMIMessaging_t;


typedef struct
{
    U16_t id;
    U8_t zone;
    U8_t inner_zone;
    U8_t outer_zone;
    char short_desc[LEN_Short_Description_t];
    char long_desc[LEN_Long_Description_t];
    // gps points should be float. Pass as char for now as it's not being used.
    char gps_lat[LEN_Gps_t];
    char gps_long[LEN_Gps_t];
    char gps_lat_tolerance[LEN_Gps_t];
    char gps_long_tolerance[LEN_Gps_t];
}  MYKI_CD_Locations_t;

typedef struct
{
    U16_t id;
    U16_t location;
    U8_t provisional_zone_low;
    U8_t provisional_zone_high;
} MYKI_CD_Stations_t;

typedef struct
{
    U16_t station;
} MYKI_CD_LinesByStation_t;

typedef struct
{
    U16_t line;
} MYKI_CD_StationsByLine_t;

typedef struct
{
    U16_t arraySize;
    U16_t* arrayOfU16;
} MYKI_CD_U16Array_t;

typedef struct
{
    U8_t zone_low;
    U8_t zone_high;
    U16_t days;
    U32_t value;
} MYKI_CD_FaresEpass_t;

typedef struct
{
    U16_t id;
    char short_desc[LEN_Short_Description_t];
    char long_desc[LEN_Long_Description_t];
    char mode[LEN_Mode_t];
} MYKI_CD_ServiceProviders_t;

typedef struct
{
    U8_t zone_low;
    U8_t zone_high;
    U32_t sv_full;
    U32_t sv_conc;
    U32_t epass_full;
    U32_t epass_conc;
} MYKI_CD_FaresProvisional_t;

typedef struct
{
    U8_t zone_low;
    U8_t zone_high;
    U32_t single_trip;
    U32_t nhour;
    U32_t daily;
    U32_t weekly;
    U32_t weekend;
} MYKI_CD_FaresStoredValue_t;

//struct MYKI_CD_ProductAttribute_t
//{
//  std::string name;
//  std::string value;
//};
//
//struct MYKI_CD_ProductDefaultAttribute_t
//{
//  std::string name;
//  std::string value;
//};

typedef struct
{
    U8_t issuer_id;
    U8_t id;
    char type[LEN_20_t];
    char subtype[LEN_20_t];
    char short_desc[LEN_Short_Description_t];
    char long_desc[LEN_Long_Description_t];
//  std::vector<MYKI_CD_ProductDefaultAttribute_t> productDefaultAttribute;
//  std::vector<MYKI_CD_ProductAttribute_t> productAttribute;
} MYKI_CD_Product_t;

typedef struct
{
    U8_t id;
    char code[LEN_Code_t];
    char desc[LEN_Short_Description_t];
    U8_t percent;
    char isdda[LEN_6_t];
    U16_t cardexpirydays;
    U16_t tappexpirydays;
    U16_t passengercodedays;
} MYKI_CD_PassengerType_t;

typedef struct
{
    U8_t id;
    char short_desc[LEN_Short_Description_t];
    char long_desc[LEN_Long_Description_t];
} MYKI_CD_ProductIssuer_t;

//typedef struct
//{
//  std::string zone;
//  std::string mode;
//  std::string value;
//  std::string device_type;
//  std::string location;
//  std::string service_provider;
//  std::string passenger_type;
//  std::string blocking_reason;
//  std::string payment_method;
//} MYKI_CD_DeviceParameterAttribute_t;

typedef enum
{
	MYKI_CD_MODE_UNKNOWN = 0,
	MYKI_CD_MODE_ALL = 1,
	MYKI_CD_MODE_BUS = 2,   
	MYKI_CD_MODE_RAIL = 3,  
	MYKI_CD_MODE_TRAM = 4,  
} MYKI_CD_Mode_t;


typedef enum
{
	MYKI_CD_DEVICE_TYPE_UNKNOWN = 0,
	MYKI_CD_DEVICE_TYPE_TOTF    = 1,    // Ticket Office Machine (Fixed)
	MYKI_CD_DEVICE_TYPE_TOTM    = 2,    // Ticket Office Machine (Mobile)
	MYKI_CD_DEVICE_TYPE_SMT     = 3,    // Smartcard Management Terminal
	MYKI_CD_DEVICE_TYPE_CVMS    = 4,    // Card Vending Machine (Stationary)
	MYKI_CD_DEVICE_TYPE_CVMM    = 5,    // Card Vending Machine (Mobile_  
	MYKI_CD_DEVICE_TYPE_HHD     = 6,    // Hand-Held Device  
	MYKI_CD_DEVICE_TYPE_RD      = 7,    // Retail Device
	MYKI_CD_DEVICE_TYPE_WEB     = 8,    // Web Site  
	MYKI_CD_DEVICE_TYPE_CALL    = 9,    // Call Centre  
	MYKI_CD_DEVICE_TYPE_FPDM    = 10,   // Fare Payment Device (Mobile)
    MYKI_CD_DEVICE_TYPE_FPDG    = 11,   // Fare Payment Device (Gate)         
	MYKI_CD_DEVICE_TYPE_FPDS    = 12,   // Fare Payment Device (Stationary)
    MYKI_CD_DEVICE_TYPE_SEM     = 13,   // Standalone Enquiry Machine
	MYKI_CD_DEVICE_TYPE_BDC     = 14,   // Bus Driver Console
    MYKI_CD_DEVICE_TYPE_TDC     = 15,   // Tram Driver Console
	MYKI_CD_DEVICE_TYPE_TDC2    = 16,   // Tram Driver Secondary Console 
} MYKI_CD_DeviceType_t;


typedef struct
{
    char name[LEN_Short_Description_t];
    char default_value[LEN_Value_t];
    char value[LEN_Value_t];
} MYKI_CD_DeviceParameter_t;


typedef enum
{
    MYKI_CD_DIFFERENTIAL_PRICING_RULE_TYPE_STANDARD = 1,
    MYKI_CD_DIFFERENTIAL_PRICING_RULE_TYPE_OFFPEAK = 2,
    MYKI_CD_DIFFERENTIAL_PRICING_RULE_TYPE_ANY = 3
} MYKI_CD_DifferentialPricingRuleType_e;

typedef enum
{
    MYKI_CD_TRIP_DIRECTION_INBOUND = 1,
    MYKI_CD_TRIP_DIRECTION_OUTBOUND = 2,
    MYKI_CD_TRIP_DIRECTION_UNKNOWN = 3
} MYKI_CD_TripDirection_e;

typedef struct
{
    char code[LEN_15_t];
    char short_desc[LEN_Short_Description_t];
    char long_desc[LEN_Long_Description_t];
    unsigned int locationCount;
    U16_t* locations;
} MYKI_CD_MasterRoute_t;

typedef struct
{
   U32_t service_provider_id;
   U32_t route_id;
} MYKI_CD_Route_t;

typedef struct
{
    U16_t route_id;
    U16_t master_route;
    char code[LEN_Short_Description_t];
    char short_desc[LEN_Short_Description_t];
    char long_desc[LEN_Long_Description_t];
} MYKI_CD_RouteInfo_t;

typedef struct
{
    U8_t section;
    char short_desc[LEN_Short_Description_t];
    char long_desc[LEN_Long_Description_t];
} MYKI_CD_RouteSection_t;

typedef struct
{
	U8_t stopId;
	U16_t locationId;
	U8_t provisionalZoneLow;
	U8_t provisionalZoneHigh;
	U8_t gpsBearing;
} MYKI_CD_RouteStop_t;

/*
**  MYKI_CD_DifferentialPricingRequest_t is used for both
**  DifferentialPrices and DifferentialPricesProvisional although not all
**  the fields are used for both.
*/
typedef struct
{
    MYKI_CD_DifferentialPricingRuleType_e rule_type;    /* Type of Rule */
    char passenger_type_null;                           /* Set to 1 if passenger_type is null, 0 otherwise */
    U8_t passenger_type;
    int scanon_minutes_since_midnight;                  /* Use -1 for null */
    int scanoff_minutes_since_midnight;                 /* Use -1 for null */
    U8_t day_of_week;                                   /* Monday = 1, Sunday = 7 */
    U8_t day_of_month;                                  /* Range 1..31 */
    U8_t month;                                         /* January = 1 */
    U8_t zone_low;
    U8_t zone_high;
    MYKI_CD_TripDirection_e trip_direction;
    unsigned int route_count;
    const MYKI_CD_Route_t* routes;
} MYKI_CD_DifferentialPricingRequest_t;

typedef enum
{
    MYKI_CD_DISCOUNT_TYPE_STATIC = 1,
    MYKI_CD_DISCOUNT_TYPE_PERCENT = 2
} MYKI_CD_DiscountType_e;

typedef struct
{
    U32_t id;                                           /* For logging purpose? */
    char short_desc[LEN_Short_Description_t];           /* For logging purpose? */
    MYKI_CD_DiscountType_e discount_type;
    U32_t base_discount;
    U32_t applied_discount;
    U8_t entitlement_issuer;
    U8_t entitlement_product;
    U8_t entitlement_zone_low;
    U8_t entitlement_zone_high; 
} MYKI_CD_DifferentialPricing_t;


typedef struct
{
    U32_t id;                                               /* For logging purpose? */
    char short_desc[LEN_Short_Description_t];               /* For logging purpose? */
    MYKI_CD_DiscountType_e discount_type;
    U32_t discount;
} MYKI_CD_DifferentialPricingProvisional_t;

/*
 *      Function Prototypes
 *      -------------------
 */

    /**
     *      Opens Tariff database.
     *      @param  pFilename   Tariff database file name.
     *      @return true if successful; false otherwise.
     */
int MYKI_CD_openCDDatabase( const char *pFilename );

    /**
     *      Closes Tariff database.
     */
void MYKI_CD_closeCDDatabase( void );

    /**
     *      Opens DELTA Actionlist database.
     *      @param  filename    DELTA Actionlist database file name.
     *      @return true if successful; false otherwise.
     */
int MYKI_CD_openDeltaActionlistDatabase( const char *filename );

    /**
     *      Opens FULL Actionlist database.
     *      @param  filename    DELTA Actionlist database file name.
     *      @return true if successful; false otherwise.
     */
int MYKI_CD_openFullActionlistDatabase( const char *filename );

    /**
     *      Closes DELTA Actionlist database.
     */
void MYKI_CD_closeDeltaActionlistDatabase( void );

    /**
     *      Closes FULL Actionlist database.
     */
void MYKI_CD_closeFullActionlistDatabase( void );

// Set the device type and device mode used for
// some device parameter queries
int MYKI_CD_setDeviceParameters(
	const char* deviceType, // set to null for unknown
	MYKI_CD_Mode_t mode,
	U8_t zone, // set to 0 for unknown
	U16_t locationId, // set to 0 for unknown
	U8_t passengerType, // set to 0 for unknown
	U16_t serviceProvider, // set to 0 for unknown
	U8_t blockingReason, // set to 0 for unknown
	U8_t paymentMethod // set to 0 for unknown
	);

// Use the CD set current as at the supplied time
int MYKI_CD_useCurrentVersion(Time_t currentTime);

// The start date
// returns -1 on failure
Time_t MYKI_CD_getStartDateTime();
// returns -1 on failure
DateC20_t MYKI_CD_getStartDate();

// Return the version info for the in-use CD.
int MYKI_CD_getMajorVersion();
int MYKI_CD_getMinorVersion();

int MYKI_CD_validateTariff();

// Return the 3rd party archive the in-use CD is derived from
const char* MYKI_CD_getSource();

// Revised API
int MYKI_CD_getProductAutoEnabled(U8_t * booleanReturn); // Boolean return value
int MYKI_CD_getTPurseAutoEnabled(U8_t *booleanReturn);

// Device Parameter Type
int MYKI_CD_getDeviceParameterStructure(const char *deviceParameterName, MYKI_CD_DeviceParameter_t *);
int MYKI_CD_getPaymentMethodDeviceParameterStructure(const char* deviceType, U16_t serviceProvider, const char *paymentMethod, MYKI_CD_DeviceParameter_t *);
int MYKI_CD_getServiceProviderDeviceParameterStructure(U16_t serviceProvider, MYKI_CD_DeviceParameter_t *);
int MYKI_CD_getServiceProviderAndModeDeviceParameterStructure(U16_t serviceProviderId, MYKI_CD_Mode_t mode, const char *deviceParameterName,  MYKI_CD_DeviceParameter_t *deviceParameter);

int MYKI_CD_getMinimumBalanceePass(S32_t*);
int MYKI_CD_getMinimumBalanceStoredValue(S32_t*);
int MYKI_CD_getProvisionalFareMode(U16_t *);
int MYKI_CD_getEndOfTransportDay(U16_t*); // returns minutes past midnight
int MYKI_CD_getBlockingPeriodDSC(S32_t*);
int MYKI_CD_getBlockingPeriodLLSC(S32_t*);
int MYKI_CD_getChangeOfMindDSC(S32_t*);
int MYKI_CD_getChangeOfMindLLSC(S32_t*);
int MYKI_CD_getStationExitFee(S32_t*);
int MYKI_CD_getePassOutOfZone(U8_t *booleanReturn);
int MYKI_CD_getnHourPeriodMinutes(U16_t*);
int MYKI_CD_getnHourEveningCutoff(U16_t*);
int MYKI_CD_getnHourExtendPeriodMinutes(U16_t*);
int MYKI_CD_getnHourExtendThreshold(U16_t *);
int MYKI_CD_getnHourMaximumDuration(U16_t *);
int MYKI_CD_getnHourRoundingPortion(U16_t*);
int MYKI_CD_getHeadlessModeRoute(U16_t serviceProvider, U16_t*);
int MYKI_CD_getePassLowWarningThreshold(U16_t*);
int MYKI_CD_getTPurseLowWarningThreshold(U16_t*);
int MYKI_CD_getAddValueEnabled(U8_t * booleanReturn);
int MYKI_CD_getePassMaximumDay(U16_t *);
int MYKI_CD_getePassMinimumDay(U16_t *);
int MYKI_CD_getePassSelectableDays(U16_t *, U16_t * len);
int MYKI_CD_getePassTemporaryDays(U16_t *);
int MYKI_CD_getGSTPercentage(U8_t *);
int MYKI_CD_getLLSCCancellationFee(U32_t *);
int MYKI_CD_getLLSCDeposit(U32_t *);
int MYKI_CD_getLLSCPersonalisationFee(U32_t *);
int MYKI_CD_getLLSCRegistrationFee(U32_t *);
int MYKI_CD_getLLSCReplacementFee(U32_t *);
int MYKI_CD_getLLSCExpiryWindow(U16_t *);
int MYKI_CD_getLLSCSaleFee(U32_t *);
int MYKI_CD_getMaximumAddValue(U32_t *);
int MYKI_CD_getMaximumChange(U32_t *);
int MYKI_CD_getMaximumEFTPOSAmount(U32_t *);
int MYKI_CD_getMaximumTPurseBalance(S32_t*);
int MYKI_CD_getMaximumTripTolerance(U16_t serviceProvider, MYKI_CD_Mode_t mode, U8_t *);
int MYKI_CD_getMinimumAddValue(U32_t *);
int MYKI_CD_getMinimumTPurseAutoloadAmount(U32_t *);
int MYKI_CD_getMinimumTPurseAutoloadThreshold(U32_t *);
int MYKI_CD_getMinimumReceiptAmount(U32_t *);
int MYKI_CD_getMinimumRefundBalance(U32_t *);


int MYKI_CD_getPaperTicketMessage(char *, U16_t len); 
int MYKI_CD_getPaperTicketName(char *, U16_t len);
int MYKI_CD_getRefundCancelEnabled(U8_t * value);
int MYKI_CD_getRefundMessage(char *, U16_t len);
int MYKI_CD_getReversalMessage(char *, U16_t len);
int MYKI_CD_getSalesMessage(char *, U16_t len);
int MYKI_CD_getRecieptInvoiceMessageThreshold(U32_t *);
int MYKI_CD_getLLSCConcessionEnabled(U8_t * booleanReturn);
int MYKI_CD_getDSCConcessionEnabled(U8_t * booleanReturn);
int MYKI_CD_getUnblockEnabled(U8_t * booleanReturn);
int MYKI_CD_getPaymentMethodEnabled(const char* deviceType, U16_t serviceProvider, const char* paymentMethod, U8_t * value); 
int MYKI_CD_getReversalPeriod(U16_t *);

int MYKI_CD_getCompanyABN(char *, U16_t len); 
int MYKI_CD_getCompanyAddress(char *, U16_t  len); 
int MYKI_CD_getCompanyCallCentreName(char *, U16_t len); 
int MYKI_CD_getCompanyName(char *, U16_t len); 
int MYKI_CD_getCompanyNameShort(char *, U16_t len); 
int MYKI_CD_getCompanyPhone(char *, U16_t len); 
int MYKI_CD_getCompanyWebSite(char *, U16_t len); 

// Differential Prices
int MYKI_CD_getDifferentialPriceStructure(const MYKI_CD_DifferentialPricingRequest_t* input, MYKI_CD_DifferentialPricing_t*);
int MYKI_CD_getDifferentialPriceProvisionalStructure(const MYKI_CD_DifferentialPricingRequest_t* input, MYKI_CD_DifferentialPricingProvisional_t*);

// Fares Epass
int MYKI_CD_getFaresEpassStructure(U8_t zoneLow, U8_t zoneHigh, U16_t days, MYKI_CD_FaresEpass_t*);

// Fares Provisional
int MYKI_CD_getFaresProvisionalStructure(U8_t zoneLow, U8_t zoneHigh, MYKI_CD_FaresProvisional_t*);

// Fares Stored Value
int MYKI_CD_getFaresStoredValueStructure(U8_t zoneLow, U8_t zoneHigh, MYKI_CD_FaresStoredValue_t*);

// HMIMessaging
/*==========================================================================*
**
**  MYKI_CD_getHMIMessaging
**
**  Description     :
**      Populates an array with a priorty ordered list from the database based on a table type 
**      (UsageLog or LoadLog)
**
**  Parameters      :
**      tableType                [I]     Name of tableType to match
**      array                    [I]     Array to populate
**      arrayLength              [I]     Max number of array members to populate
**
**  Returns         :
**      number of rows populated
**
**==========================================================================*/
int MYKI_CD_getHMIMessaging(char* tableType, MYKI_CD_HMIMessaging_t* array, U16_t arrayLength);

// Locations
int MYKI_CD_getLocationsStructure(U16_t id, MYKI_CD_Locations_t*);

// Master Routes
// Caller should not free the MYKI_CD_MasterRoute_t.locations array, it will stay in
// memory for at least 30 calls
int MYKI_CD_getMasterRouteStructure(U16_t masterRouteId, U16_t serviceProviderId, MYKI_CD_MasterRoute_t*);

int MYKI_CD_getRoutesByServiceProvider(U16_t serviceProviderId, MYKI_CD_RouteInfo_t* pRoutes, U16_t arrayLength);

// Passenger Type
int MYKI_CD_getPassengerTypeStructure(U8_t id, MYKI_CD_PassengerType_t*);

// Product Issuer
int MYKI_CD_getProductIssuerStructure(U8_t id, MYKI_CD_ProductIssuer_t*);

// Products
int MYKI_CD_getProductStructure(U8_t id, MYKI_CD_Product_t*);
int MYKI_CD_getProductType(U8_t id, char* type, int bufferLength);
int MYKI_CD_getProductCardType(U8_t id, char* type, int bufferLength);

// Routes
int MYKI_CD_getRouteInfoStructure(U16_t routeId, MYKI_CD_RouteInfo_t*);

// Route Sections
int MYKI_CD_getRouteSectionStructure(U16_t routeId, U8_t stopId, MYKI_CD_RouteSection_t*);

// Route stops
// Retrieves RouteStop records of all stops between fromStopId to toStopId (inclusive).
// @param routeId route ID.
// @param fromStopId from stop ID.
// @param toStopId to stop ID.
// @param pRouteStops returned RouteStop record buffer.
// @return number of RouteStop records; 0 if none found.
// The caller should not allocate or free the MYKI_CD_RouteStop_t items
// in pRouteStops, the array is allocated/freed within a cache. The array
// will be valid for as long as the results of the call stay within the cache,
// which is for at least 100 calls.
int MYKI_CD_getRouteStopsStructure(U16_t routeId, U8_t fromStopId, U8_t toStopId, MYKI_CD_RouteStop_t** pRouteStops);

// Service Providers
int MYKI_CD_getServiceProvidersStructure(U16_t id, MYKI_CD_ServiceProviders_t*);
int MYKI_CD_getServiceProviderTransportMode(U16_t id, char* mode, int bufferLength);

// Station Lines
// For both functions the caller should not free the items in MYKI_CD_U16Array,
// the array is allocated/freed within a cache. The array will be valid for as
// long as the results of the call stay within the cache. For getLinesByStation
// the result will stay in the cache for at least 250 calls, for getStationsByLine
// the result will stay in the cache for at least 50 calls.
int MYKI_CD_getLinesByStationStructure(U16_t stationId, MYKI_CD_LinesByStation_t *linesByStation, MYKI_CD_U16Array_t*);
int MYKI_CD_getStationsByLineStructure(U16_t lineId, MYKI_CD_StationsByLine_t *stationsByLine, MYKI_CD_U16Array_t*);

// Stations
int MYKI_CD_getStationsStructure(U16_t id, MYKI_CD_Stations_t*);
int MYKI_CD_getStationsByLocationIdStructure(U16_t locationId, MYKI_CD_Stations_t* stations);

    /**
     *      Import Product Configuration XML.
     *      @param  railTariff  TRUE if importing for RAIL transport mode;
     *                          FALSE otherwise.
     *      @return 0 if successful; else failed.
     */
int MYKI_CD_processProductConfigurationXML( int railTariff );

int MYKI_CD_isCitySaverZone(U8_t zoneId);
int MYKI_CD_isEarlyBirdStation(U16_t stationId);
int MYKI_CD_isEarlyBirdDay(int day);
int MYKI_CD_getEarlyBirdProductId(void);
int MYKI_CD_getEarlyBirdCutOffTime(void);

int MYKI_CD_getChangeoverLoc(MYKI_CD_Locations_t * pLocScanOn, MYKI_CD_Locations_t * pLocScanOff, MYKI_CD_Locations_t * pLocChangeover);

#ifdef __cplusplus
}
#endif

#ifdef __cplusplus
    /**
     *  @brief  Retrieves saleable product details given search
     *          criteria.
     *  This (C++) function returns saleable product details that
     *  satisfy the specified search criteria.
     *  @note   Products with product price of zero are excluded
     *          from the saleable product list.
     *  @param  products returned Json:Value object containing
     *          saleable product details. The returned JSON object
     *          has the following format,
     *  @code
     *  {
     *    "products":
     *    [
     *      {
     *        "GSTApplicable":false,
     *        "PLU":"4. myki Child",
     *        "Price":50,
     *        "id":46,
     *        "long_desc":"GTS Timetable",
     *        "short_desc":"GTS Timetable",
     *        "subtype":"None",
     *        "type":"ThirdParty"
     *      },
     *      { ... },
     *      null
     *    ]
     *  }
     *  @endcode
     *  where,
     *  @li     @c id is the product id (1-255).
     *  @li     @c type is the product type, eg. "ThirdParty", "PremiumSurcharge".
     *  @li     @c subtype is the product sub-type, eg. "None".
     *  @li     @c long_desc is the product long description.
     *  @li     @c short_desc is the product short description.
     *  @li     @c Price is the product price in lowest denomination (cents).
     *  @li     @c GSTApplicable is true if GTS is applicable, false otherwise.
     *  @li     @c PLU is (optional) 'Price Lookup Unit' uniquely identifies product.
     *  @param  zone zone to validate against sales zones.
     *  @param  productType product type to search for (eg. "ThirdParty").
     *  @param  time time to validate against or TIME_NOT_SET for
     *          current time.
     *  @param  serviceProviderId service provider id or 0 for last
     *          configured service provider id (see MYKI_CD_setDeviceParameters).
     *  @param  deviceType device type (eg. "BDC") or NULL for last
     *          configured device type (see MYKI_CD_setDeviceParameters).
     *  @return true if successful; false otherwise.
     */
int     MYKI_CD_getProducts(
                Json::Value    &products,
                int             zone,
                const char     *productType,
                Time_t          time = TIME_NOT_SET,
                int             serviceProviderId = 0,
                const char     *deviceType = NULL );
#endif

#endif /* MYKI_CD_H_ */
