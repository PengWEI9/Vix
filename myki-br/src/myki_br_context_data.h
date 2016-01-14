//=============================================================================
//
//  Definition of context data passed into BR from the application and between the
//  various BR rules and sequences.
//
//  This is a secondary header file for this package. The primary header file
//  is myki_br.h. Calling code should not include this file directly. Instead
//  it should include myki_br.h which will in turn include this file.
//
//  Notes :
//  1.  The distinction between the various substructures (eg, static, dynamic)
//      is largely arbitrary. For instance, some dynamic data is actually static
//      on rail devices according to the definitions in NTS0177, and in any case
//      there is no reason for BR to distinguish between the two. All data passed
//      from the application to BR should remain constant during the execution
//      of the sequence.
//  2.  A better categorisation scheme would be to split the data between
//      input, internal, and output data. Input is data passed from the app to
//      the BR, Internal is data used only within the BR, and Output is data
//      passed from the BR back to the app. Some data elements may fall into
//      more than one category (eg, intialised and used internally, then passed
//      back)
//  3.  We'll probably keep the existing substructures for now, to match NTS0177.
//
//=============================================================================
#ifndef MYKI_BR_CONTEXT_DATA_H_
#define MYKI_BR_CONTEXT_DATA_H_

#ifdef __cplusplus
extern "C" {
#endif

//-----------------------------------------------------------------------------
//  Includes
//-----------------------------------------------------------------------------

#include <corebasetypes.h>          // For U8_t, etc
#include <myki_cd.h>                // For MYKI_CD_Locations_t (in myki_cd)
#include <myki_cdd_types.h>         // For MYKI_Location_t (in daf_myki_cardservices)
#include <myki_cdd_enums.h>
#include <LDT.h>                    // For LDTTransactionData_t

//-----------------------------------------------------------------------------
//  Constants / Macros / Enums
//-----------------------------------------------------------------------------

    /// Generates 32-bit BR BYPASSED code.
    /// @param  BR_major    BR major number (eg. 4 in BR_LLSC_4.1).
    /// @param  BR_minor    BR minor number (eg. 1 in BR_LLSC_4.1).
    /// @param  Step_major  Pre-condition step major number (eg. 1 in 1.3).
    /// @param  Step_minor  Pre-condition step minor number (eg. 3 in 1.3).
    /// @return 32-bit BR BYPASSED code.
    ///
#define BYPASS_CODE( BR_major, BR_minor, Step_major, Step_minor )\
            ( ( ( ( BR_major   ) << 24 ) & 0xff000000 ) |\
              ( ( ( BR_minor   ) << 16 ) & 0x00ff0000 ) |\
              ( ( ( Step_major ) <<  8 ) & 0x0000ff00 ) |\
              (   ( Step_minor )         & 0x000000ff ) )

//
//  This should not be exposed as part of the API, but it's here until/unless we can
//  split the data into input/internal/output rather than static/dynamic/etc.
//

#define ZONE_LOW_NOT_SET            255
#define ZONE_HIGH_NOT_SET           0

typedef enum
{
    TRIP_DIRECTION_UNKNOWN,
    TRIP_DIRECTION_INBOUND,
    TRIP_DIRECTION_OUTBOUND,
    TRIP_DIRECTION_DISABLED,

    TRIP_DIRECTION_ERROR            // Special case, used to report errors in functions that return this type

}   TripDirection_t;

typedef enum
{
    BORDER_STATUS_NONE,
    BORDER_STATUS_INBOUND,
    BORDER_STATUS_OUTBOUND,
    BORDER_STATUS_UNKNOWN,

    BORDER_STATUS_ERROR             // Special case, used to report errors in functions that return this type

}   BorderStatus_t;

//
//  This should not be exposed as part of the API, but it's here until/unless we can
//  split the data into input/internal/output rather than static/dynamic/etc.
//
//  Note that each of these constants has a value defined in NTS0177 (Data Dictionary).
//  However these constants appear to *only* be used within a single pass of BR and don't
//  appear to be exposed to the outside world, so the actual values shouldn't matter - it
//  should be possible (and more correct) to define these as enums without explicit values.
//

typedef enum
{
    ROUTE_CHANGEOVER_NONE                                   = 0,
    ROUTE_CHANGEOVER_LEGITIMATE_WITHOUT_ROUTE_CHANGEOVER    = 1,
    ROUTE_CHANGEOVER_LEGITIMATE_WITH_ROUTE_CHANGEOVER       = 2,
    ROUTE_CHANGEOVER_LEGITIMATE_DOUBLE_ROUTE_CHANGEOVER     = 3,
    ROUTE_CHANGEOVER_MISSING_SCAN_OFF                       = 4

}   RouteChangeover_t;

//
//  Used to pass extra information back to the application about the reason for a failed transaction
//

#define MYKI_BR_REJECT_REASONS\
    REASON_DECLARATION(MYKI_BR_REJECT_REASON_NOT_APPLICABLE),           /*  Transaction was not rejected, so reason is N/A */\
    REASON_DECLARATION(MYKI_BR_REJECT_REASON_CARD_BLOCKED),             /*  Card is blocked */\
    REASON_DECLARATION(MYKI_BR_REJECT_REASON_APPLICATION_BLOCKED),      /*  Transit or Operator Application is blocked */\
    REASON_DECLARATION(MYKI_BR_REJECT_REASON_ALREADY_TOUCH_ON),\
    REASON_DECLARATION(MYKI_BR_REJECT_REASON_ALREADY_TOUCH_OFF),\
    REASON_DECLARATION(MYKI_BR_REJECT_REASON_EXPIRED),\
    REASON_DECLARATION(MYKI_BR_REJECT_REASON_INVALID_ZONE),\
    REASON_DECLARATION(MYKI_BR_REJECT_REASON_MISSING_TOUCH_ON),\
    REASON_DECLARATION(MYKI_BR_REJECT_REASON_INSUFFICIENT_FUND),\
    REASON_DECLARATION(MYKI_BR_REJECT_REASON_TOUCH_AGAIN),\
    REASON_DECLARATION(MYKI_BR_REJECT_REASON_MULTIPLE_CARDS),\
    REASON_DECLARATION(MYKI_BR_REJECT_REASON_CARD_NOT_ACTIVATED),\
    REASON_DECLARATION(MYKI_BR_REJECT_REASON_CRYPTO_AUTH_FAIL),\
    REASON_DECLARATION(MYKI_BR_REJECT_REASON_UNREADABLE),\
    REASON_DECLARATION(MYKI_BR_REJECT_REASON_INVALID_SERVICE_PROVIDER),\
    REASON_DECLARATION(MYKI_BR_REJECT_REASON_MAX_PIN_RETRIES_EXCEEDED),\
    REASON_DECLARATION(MYKI_BR_REJECT_REASON_INVALID_PIN),\
    REASON_DECLARATION(MYKI_BR_REJECT_REASON_INVALID_AMOUNT),           /*  Invalid TPurse load amount */\
    REASON_DECLARATION(MYKI_BR_REJECT_REASON_ADDVALUE_DISABLED),        /*  TPurse load disabled */\
    REASON_DECLARATION(MYKI_BR_REJECT_REASON_EXCEED_MAX_BALANCE),       /*  Exceed TPurse maximum balance */\
    REASON_DECLARATION(MYKI_BR_REJECT_REASON_REVERSAL_NOT_ALLOWED),     /*  Reversal not allowed */\
    REASON_DECLARATION(MYKI_BR_REJECT_REASON_PRECOMMIT_CHECK_FAILED),   /*  Pre-commit sanity check failed */\
    REASON_DECLARATION(MYKI_BR_REJECT_REASON_INVALID_BALANCE),          /*  Invalid TPurse balance */\
    REASON_DECLARATION(MYKI_BR_REJECT_REASON_CARD_EXPIRED),             /*  MTU-505 - Card expired */\
    REASON_DECLARATION(MYKI_BR_REJECT_REASON_OPAP_EXPIRED),             /*  MTU-505 - Operator Application expired */\
    REASON_DECLARATION(MYKI_BR_REJECT_REASON_STAFF_EXPIRED),            /*  MTU-505 - ? */\
    REASON_DECLARATION(MYKI_BR_REJECT_REASON_DISABLED_CARD),            /*  MTU-505 - Card disabled */\
    REASON_DECLARATION(MYKI_BR_REJECT_REASON_OPAP_DISABLED),            /*  MTU-505 - Operator Application disabled */\
    REASON_DECLARATION(MYKI_BR_REJECT_REASON_OPAP_INACTIVE),            /*  MTU-505 - Operator Application inactive */\
    REASON_DECLARATION(MYKI_BR_REJECT_REASON_TPURSE_MODIFIED)

#define REASON_DECLARATION( a )     a
typedef enum
{
    MYKI_BR_REJECT_REASONS,
    MYKI_BR_REJECT_REASON_MAX

}   MYKI_BR_RejectReason_t;
#undef  REASON_DECLARATION

//
//  Used to pass extra information back to the application about a successful transaction
//

#define MYKI_BR_ACCEPT_REASONS\
    REASON_DECLARATION(MYKI_BR_ACCEPT_REASON_NOT_APPLICABLE),   /*  Transaction was rejected, so reason is N/A */\
    REASON_DECLARATION(MYKI_BR_ACCEPT_REASON_FORCED_SCANOFF),   /*  Transaction was accepted, forced scan off occurred */\
    REASON_DECLARATION(MYKI_BR_ACCEPT_CHANGE_OF_MIND),\
    REASON_DECLARATION(MYKI_BR_ACCEPT_TPURSE_LOADED),           /*  Transaction was accepted, TPurse value added */\
    REASON_DECLARATION(MYKI_BR_ACCEPT_TPURSE_DEBITED)           /*  Transaction was accepted, TPurse value debited */

#define REASON_DECLARATION( a )     a
typedef enum
{
    MYKI_BR_ACCEPT_REASONS,
    MYKI_BR_ACCEPT_REASON_MAX

}   MYKI_BR_AcceptReason_t;
#undef  REASON_DECLARATION

#define MAX_LOCATIONS_PER_TRIP      250


//
//  Used for load log or usage log
//
typedef enum
{
    // Usage Log
    MYKI_BR_TRANSACTION_TYPE_ENTRY_EXIT                   = 8 , // Purse Usage Automated
    MYKI_BR_TRANSACTION_TYPE_PURSE_USAGE_MANUAL           = 9 ,
    MYKI_BR_TRANSACTION_TYPE_APPLICATION_BLOCK            = 10,
    MYKI_BR_TRANSACTION_TYPE_PRODUCT_BLOCK                = 11,
    MYKI_BR_TRANSACTION_TYPE_APPLICATION_UNBLOCK          = 12,
    MYKI_BR_TRANSACTION_TYPE_PRODUCT_UNBLOCK              = 13,
    MYKI_BR_TRANSACTION_TYPE_CHANGE_OF_MIND               = 14,
    MYKI_BR_TRANSACTION_TYPE_STATION_EXIT_FEE             = 15,
    MYKI_BR_TRANSACTION_TYPE_TRANSIT_TPURSE_USAGE         = 16,
    MYKI_BR_TRANSACTION_TYPE_NON_TRANSIT_TPURSE_USAGE     = 17,
    MYKI_BR_TRANSACTION_TYPE_EXIT_ONLY                    = 18,
    MYKI_BR_TRANSACTION_TYPE_ENTRY_ONLY                   = 19,
    MYKI_BR_TRANSACTION_TYPE_ON_BOARD                     = 20,
    MYKI_BR_TRANSACTION_TYPE_WRONG_SIDE_OF_GATE           = 27,
    // Load Log
    MYKI_BR_TRANSACTION_TYPE_TPURSE_LOAD_VALUE            = 1 ,
    MYKI_BR_TRANSACTION_TYPE_LOAD_PRODUCT                 = 2 ,
    MYKI_BR_TRANSACTION_TYPE_LOAD_PRODUCT_RENEWAL         = 3 ,
    MYKI_BR_TRANSACTION_TYPE_REFUND_VALUE                 = 4 ,
    MYKI_BR_TRANSACTION_TYPE_REFUND_PRODUCT               = 5 ,
    MYKI_BR_TRANSACTION_TYPE_REVERSE_VALUE                = 6 ,
    MYKI_BR_TRANSACTION_TYPE_REVERSE_PRODUCT              = 7 ,
    MYKI_BR_TRANSACTION_TYPE_MULTIPLE_ACTION_LIST         = 21,
    MYKI_BR_TRANSACTION_TYPE_PRODUCT_AUTOLOAD_DISABLE     = 22,   // action list
    MYKI_BR_TRANSACTION_TYPE_PRODUCT_AUTOLOAD_ENABLE      = 23,   // action list
    MYKI_BR_TRANSACTION_TYPE_AUTO_LOAD_ENABLE             = 24,
    MYKI_BR_TRANSACTION_TYPE_AUTO_LOAD_DISABLE            = 25,
    MYKI_BR_TRANSACTION_TYPE_ACTIVATE_TRANSIT_APPLICATION = 26,
    // No 28
    MYKI_BR_TRANSACTION_TYPE_CUSTOMER_CATEGORY_UPDATED    = 29,
    MYKI_BR_TRANSACTION_TYPE_TPURSE_AUTO_LOAD_VALUE       = 30,
    MYKI_BR_TRANSACTION_TYPE_MULTIPLE_LOAD_ACTIONS        = 31,
    MYKI_BR_TRANSACTION_TYPE_ON_BOARD_REVALIDATION        = 32,
    MYKI_BR_TRANSACTION_TYPE_TPURSE_AL_LOAD_VALUE         = 33,   // Actionlist - TPurse Reverse

    MYKI_BR_TRANSACTION_TYPE_NONE                         = 0,
    MYKI_BR_TRANSACTION_TYPE_UNKNOWN                      = 255
} MYKI_BR_LogTransactionType_t;

typedef enum
{
    PRODUCT_TYPE_UNKNOWN,
    PRODUCT_TYPE_SINGLE,    // aka City Saver - no longer used in the system
    PRODUCT_TYPE_DAILY,
    PRODUCT_TYPE_WEEKLY,
    PRODUCT_TYPE_NHOUR,
    PRODUCT_TYPE_EPASS,

    PRODUCT_TYPE_COUNT      // Must be last

}   ProductType_e;

typedef enum
{
    TRANSPORT_MODE_UNKNOWN,
    TRANSPORT_MODE_BUS,
    TRANSPORT_MODE_RAIL,
    TRANSPORT_MODE_TRAM

}   TransportMode_e;

#define     ZONE_MAP_MAX    200                         // Both logical and physical zones are numbered from 1 to 200 (see KA0002 v7.2 section 2.4.9)
#define     ZONE_MAP_MIN    1                           // Both logical and physical zones are numbered from 1 to 200 (see KA0002 v7.2 section 2.4.9)

typedef enum
{
    OperatorType_UNKNOWN,
    OperatorType_DRIVER,
    OperatorType_ATTENDANT,
    OperatorType_TECHNICIAN,
    OperatorType_SUPERVISOR,
    OperatorType_AUTHORISED_OFFICER

}   OperatorType_e;

    /** Method of Payment Codes */
enum
{
    PAYMENT_METHOD_UNDEFINED = (0),                     /**< Undefined */
    PAYMENT_METHOD_TPURSE,                              /**< T-Purse */
    PAYMENT_METHOD_CASH,                                /**< Cash */
    PAYMENT_METHOD_EFT_REFUNDABLE,                      /**< EFT (Refundable) */
    PAYMENT_METHOD_EFT_NONREFUNDABLE,                   /**< EFT (Non-refundable) */
    PAYMENT_METHOD_RECURRING_AUTOLOAD,                  /**< Recurring autoload */
    PAYMENT_METHOD_ADHOC_AUTOLOAD,                      /**< Ad-hoc autoload */
    PAYMENT_METHOD_CHEQUE,                              /**< Cheque */
    PAYMENT_METHOD_VOUCHER,                             /**< Voucher */
    PAYMENT_METHOD_TRANSFER,                            /**< Transfer */

    PAYMENT_METHOD_MAX

};

//-----------------------------------------------------------------------------
//  Typedefs
//-----------------------------------------------------------------------------

//
//  Currency
//      This typedef should be used within all BR (and ideally in the entire
//      Myky front office application) for any variable that holds a currency
//      value, such as purse current, maximum and minimum balancea, fares, etc.
//      Ideally this would be a structure also containing an element indicating
//      the currency units (such as AUD, USD, etc), but for now we'll just assume
//      it's in AUD minor units (ie Cents).
//

typedef S32_t   Currency_t;

//-----------------------------------------------------------------------------
//  Structures
//-----------------------------------------------------------------------------

typedef struct
{
    int                 type;
    int                 profile;
    OperatorType_e      operatorType; 
    int                 permissions;                    // Reserved for future use
}   StaffRole_t;

//
//  Originating Information - used in DynamicData_t
//

typedef struct
{
    int             serviceProviderId;
    MYKI_Location_t location;                   // From card services

}   OriginatingInformation_t;

    /** Driver shift portion status */
typedef enum
{
    SHIFT_PORTION_UNKNOWN,
    SHIFT_PORTION_OUT_OF_BALANCE,                           /**< Shift portion out-of-balance */
    SHIFT_PORTION_RESET,                                    /**< Shift portion reset */
    SHIFT_PORTION_BROUGHT_FORWARD,                          /**< Shift portion brought forward */

}   MYKI_BR_ShiftPortionStatus_e;

    /** Shift data from Driver card */
typedef struct
{
    OAppShiftDataStatus_t           shiftStatus;            /**< Shift status */
    int                             shiftId;                /**< Last closed Driver shift id */
    Time_t                          shiftStartTime;         /**< Driver shift start time */
    Time_t                          shiftEndTime;           /**< Last closed shift end time */

    int                             paperTicketReturns;
    int                             sundryItemReturns;
    int                             cardReturns;

    MYKI_OAShiftDataRecord_t        shiftTotals[ PAYMENT_METHOD_MAX ];
                                                            /**< Driver shift totals */
#define                             shiftTotalsTPurse       shiftTotals[ PAYMENT_METHOD_TPURSE  ]
#define                             shiftTotalsCash         shiftTotals[ PAYMENT_METHOD_CASH    ]

    MYKI_BR_ShiftPortionStatus_e    shiftPortionStatus;     /**< Shift portion status */

}   MYKI_BR_ShiftData_t;
//
//  Dynamic Data
//

typedef struct
{
    //
    //  Unsorted data (not sure yet whether this is input, internal, or returned data)
    //

    Currency_t          cappingContribution;
    U8_t                cappingEpassZoneLow;
    U8_t                cappingEpassZoneHigh;

    U16_t               originRouteId;
    U8_t                originStopId;
//  U8_t                destinationRouteId;         // DUPLICATED - use lineId
//  U8_t                destinationStopId;          // DUPLICATED - use stopId
    U16_t               traverseRouteId;
    U8_t                traverseStopId;
    U8_t                traverseDestinationStopId;

    int                 isForcedScanOff;
    Time_t              forcedScanOffDateTime;
    int                 isOffPeak;
    U16_t               borderProductsToExclude;
    U8_t                autoLoad;
    U8_t                locationDataUnavailable;
    Time_t              additionalMinutesThisTrip;  // Trip extension period in unit of minutes

    OriginatingInformation_t    originatingInformation;
    int                         isOriginatingInformationSet;    // TRUE if originating information is set

    //
    //  Input data (provided by the application)
    //

    int                 numberOfCardsPresented;     // Set to the number of detected cards by the application during card detect
    int                 isTransit;                  // Set to TRUE by the application during card detect / open if the card contains a Transit Application
    int                 isOperator;                 // Set to TRUE by the application during card detect / open if the card contains an Operator Application

    Time_t              commonDateTime;             // Date/time used by some rules under special conditions
    Time_t              currentDateTime;            // The current system date/time on entry into BR (as determined by the application)
    DateC19_t           currentBusinessDate;        // The current business date on entry into BR (as determined by the application)

    int                 entryPointId;               // Rail - Unique station identifier, Bus - Unique vehicle identifier
    int                 lineId;                     // Rail - (0), Bus - Unique line (route) identifier
    int                 stopId;                     // Rail - (0), Bus - Unique stop identifier (Note: NTS0177 says "for rail, same as entryPointId" but this is wrong as it's a U8 on the card)

    int                 currentZone;                // ID of the current zone (KWS: Need more details - how are the zones organised in CD?)
    int                 currentInnerZone;           // ID of the current inner zone
    int                 currentOuterZone;           // ID of the current outer zone
//    int                 Zone;                       // KWS: Probably a pseudonym for currentZone (in NTS0177 body but not data dictionary)

    int                 provisionalZoneLow;         // Low zone to use for provisional fare calculation - Based on a lookup of the stop id
    int                 provisionalZoneHigh;        // High zone to use for provisional fare calculation - Based on a lookup of the stop id

    int                 offPeakDiscountRate;        // This is a value from 0 to 100 which represents the percentage discount to be applied to a peak fare to get an off-peak fare

    Currency_t          tpurseLoadAmount;           // TPurse load amount
    int                 reverseTxSeqNo;             // TPurse load transaction sequence number to be reversed
    
    char                operatorPin[ 8 + 1 ];       // Maximum 8 chars + NULL

    //
    //  Internal data (set/used only within BR) - should all be moved to MYKI_BR_InternalData_t
    //

    RouteChangeover_t   routeChangeover;            // Determined route changeover condition (always NONE for rail devices)
    U8_t                routeChangeoverStatus;      // Probably a pseudonym for routeChangeover (in NTS0177 body but not data dictionary)
//  ????                tripHistory;                // Information about current and previous two passenger trips, used to determine if route changeover condition exists, not used on rail devices

    U8_t                isEarlyBirdTrip;            // Set by BR if this passenger trip qualifies as an "Early Bird Trip"

    U8_t                currentTripZoneLow;         // Used at scan off - actual low zone for current passenger trip    - should be an int, but this causes warnings because of CD API
    U8_t                currentTripZoneHigh;        // used at scan off - actual high zone for current passenger trip   - should be an int, but this causes warnings because of CD API

    TripDirection_t     currentTripDirection;       // Current passenger trip direction - valid only if transportMode is RAIL (TRIP_DIRECTION_xxx)
    int                 currentTripIsThinZone;      // Current passenger trip is entirely within a thin zone

    int                 extendedProducts;           // List of n-Hour products that have been extended in the current sequence. Presumably this means this is internal only. What data does it contain?

    int                 fareRouteId;                // The ID of the route to use for fare calculations (undefined if fareRouteIsValid is FALSE)
    int                 fareRouteIdIsValid;         // Set to TRUE if fareRouteId is a valid route ID, or FALSE if fare calculations should not use any route

    U8_t                previousDailyCapZoneLow;    // Set in BR_LLSC_1_2, used as pre-condition in BR_LLSC_1_5 - holds value before capping update
    U8_t                previousDailyCapZoneHigh;   // Set in BR_LLSC_1_2, used as pre-condition in BR_LLSC_1_5 - holds value before capping update

    MYKI_CD_Locations_t transportLocations[ MAX_LOCATIONS_PER_TRIP ];   // List of locations traversed on the current trip (passenger journey leg)
    int                 transportLocationsCount;                        // Number of populated elements in the above list

    Currency_t          combinedProductValue;
    Currency_t          combinedProductValueOffPeak;

    Currency_t          loadLogTxValue;             // Accumulated signed value of TAppLoadLog.TxValue
    
    Currency_t          tPurseDebitAmount;          // TPurse debit amount for sales paid by TPurse
    
    int                 isAOCard;

    MYKI_BR_ShiftData_t *pShiftData;                 // Shift data buffer or NULL
}   DynamicData_t;

//
//  Static Data (all currently assumed to be "input" data)
//

#define                 MAX_STAFF_ROLES 20

typedef struct
{
    U32_t               deviceId;                   // Uniquely identifies this device within the system - ie Device ESN (8 hex digits)
    U32_t               deviceGroupId;              // Logically groups devices, generally into "gate arrays"
    U32_t               serviceProviderId;          // Service Provider for this device (ie, who "gets paid" for providing this journey leg)
    U32_t               deviceAquirerId;            // Not sure what this is, but it's used by LDT so it needs to be passed in by the App (should be deviceAcquirerId)
    U32_t               deviceMinorId;              // Not sure what this is, but it's used by LDT so it needs to be passed in by the App
    U32_t               deviceSamId;                // Unique identifier of the Myki SAM in the device

    int                 productAutoEnabled;         // Is this device allowed to autoload products? (non-zero means yes)
    int                 purseAutoEnabled;           // Is this device allowed to autoload the purse? (non-zero means yes)

    int                 AcsCompatibilityMode;       // TRUE to replicate any incorrect ACS behaviour, FALSE to run correctly

    //  We currently assume the following four possibilities :
    //      isEntry     isExit
    //      FALSE       FALSE       Bus device (ie, "On-Board" device)
    //      FALSE       TRUE        Exit-Only rail gate device
    //      TRUE        FALSE       Entry-Only rail gate device
    //      TRUE        TRUE        Entry-Exit rail platform validator

    int                 isEntry;                    // For rail devices, this device can be used to allow entry into the paid area
    int                 isExit;                     // For rail devices, this device can be used to allow exit from the paid area
    
    int                 maxPinRetries;              // Maximum PIN retries
    StaffRole_t         staffRoles[MAX_STAFF_ROLES];             // Roles loaded from json file

    int                 numberOfHoursBetweenShifts; // Number of hours between Driver shifts
    int                 maxShiftDownTime;           // Maximum Driver shift down time (hours)

}   StaticData_t;

//
//  Provisional capping options - bare values are used in the BR document - this is an attempt to provide descriptive names for them.
//

enum
{
    PROVISIONAL_CAPPING_OPTION_USE_PROVISIONAL_FARE   = 1,  // Option 1: The provisional fare is added to the daily and weekly capping fare totals.
    PROVISIONAL_CAPPING_OPTION_USE_STANDARD_FARE      = 2,  // Option 2: The standard (actual) fare is added to the daily and weekly capping fare totals
    PROVISIONAL_CAPPING_OPTION_3                      = 3   // Option 3: What is it? Can't find the definition anywhere
};

//
//  Tariff Data
//

typedef struct
{
    // Transaction specific
    Currency_t          singleTripValue;            /**< Single trip stored value */
    Currency_t          nHourValue;                 /**< nHour stored value */
    Currency_t          dailyCapValue;              /**< Daily capping value */
    Currency_t          weeklyCapValue;             /**< Weekly capping value */
    Currency_t          weekendCapValue;            /**< Weekend capping value */
    U8_t                entitlementZoneLow;
    U8_t                entitlementZoneHigh;
    U16_t               entitlementProduct;
    U16_t               entitlementRule;            // ? In appendix ?
//  Currency_t          baseFare;
//  Currency_t          applicableFare;             // Value to be deducted from T-Purse (DUPLICATED! See MYKI_BR_ReturnedData_t)

    Currency_t          entitlementValue;           // Represents the capping value for an entitlement product

    Currency_t          premiumSurcharge;

    U16_t               inactiveEPassValidity;
    U16_t               provisionalCappingOption;   // Represents the globally selected option for managing provisional fares with regard to capping (see PROVISIONAL_CAPPING_OPTION enum)

    //  From Product CD
    Currency_t          stationExitFee;             //
    S32_t               blockingPeriod;             // Number of seconds after a tag during which a second tag will always be rejected (switched on card type)
    S32_t               changeOfMindPeriod;         // Number of seconds after a tag during which a second tag will be treated as a change of mind (switched on card type)
    Currency_t          TPurseMaximumBalance;       //
    U16_t               nHourMaximumDuration;       //
    U16_t               endOfBusinessDay;           // Number of minutes after midnight.
    U16_t               nHourPeriodMinutes;         // n-Hour product period in unit of minutes
    U16_t               nHourRoundingPortion;       // Rounding of n-Hour product period in unit of minutes
    U16_t               nHourExtendThreshold;       // Number of zones per n-Hour product extension period
    U16_t               nHourExtendPeriodMinutes;   // n-Hour product period extension in unit of minutes
    Currency_t          TPurseMinBalanceEPass;      // Minimum purse balance value for an e-Pass usage
    Currency_t          TPurseMinBalance;           // Minimum purse balance value for stored value usage
    U8_t                allowEPassOutOfZone;        //
    U8_t                concessionTypeIsDDA;        // getLLSCConcessionEnabled/getDSCConcessionEnabled
    U8_t                maximumTripTolerance;       // Maximum number of minutes grace that a standard scan off will be applied even though the provisional product is expired. 
    
    //  Sales parameters
    U8_t                addValueEnabled;            // Enabled/disabled add value services
    Currency_t          minimumAddValue;            // Minimum amount that may be loaded onto TPurse in a single transaction
    Currency_t          maximumAddValue;            // Maximum amount that may be loaded onto TPurse in a single transaction

    TripDirection_t     conductorLinesTripDirection;
    U16_t               location;
    U16_t               locationInnerBorder;
    U8_t                locationZone;
    U16_t               locationOuterBorder;
    U16_t               headlessRouteId;            // Represents the service provider-specific ID of the headless mode route. (HeadlessModeRoute? )

    U16_t               maximumProductExpiryTime;   // getLLSCExpiryWindow?

}   Tariff_t;

//
//  Actionlist Data
//

typedef struct
{
    int             type; // ActionlistType_e
// CardUID cardUID; // may not be required

    union
    {
        ProductSale_t productSale;
        ProductUpdate_t productUpdate;
        ProductUsage_t productUsage;
        CardSale_t cardSale;
        CappingUpdate_t cappingUdate;
        TPurseUpdate_t tPurseUpdate;
        TPurseLoad_t tPurseLoad;
        TAppUpdate_t tAppUpdate;
        TPurseUsage_t tPurseUsage;
        OAppUpdate_t OAppUpdate;
        OAppPinUpdate_t OAppPinUpdate;
    } actionlist;
}   Actionlist_t;


//
//  Proposed merged trip details
//

typedef struct
{
    int                 ZoneLow;
    int                 ZoneHigh;
    Currency_t          Fare;
    Currency_t          OffPeakFare;            // DynamicData.CombinedZoneRangeOffPeakFare (renamed for consistency)
    TripDirection_t     TripDirection;

}   MYKI_BR_ProposedMerge_t;

typedef struct
{
    int                 priority;
    int                 dirIndex;
    ProductType_e       productType;
    Currency_t          purchaseValue;
    int                 isOffPeak;

}   ZoneMapZone_t;

typedef struct
{
    int                 zoneLow;
    int                 zoneHigh;
    int                 dirIndex;
    ProductType_e       productType;
    Currency_t          purchaseValue;
    int                 isOffPeak;

}   ZoneMapProduct_t;

typedef struct
{
    int                 zoneLow;
    int                 zoneHigh;
    ZoneMapZone_t       zoneList[ ZONE_MAP_MAX + 1 ];   // For simplicity we include elements for zones less than ZONE_MAP_MIN so we don't have to apply an offset every time we use this array
    ZoneMapProduct_t    productList[ 2 * MYKI_DIRECTORY_MAX_LENGTH  ];
    int                 productCount;

}   ZoneMap_t;

//
//  Internal data (used only within BR)
//

typedef struct
{
    LDTTransactionData_t    TransactionData;            // There must be one copy of this for the duration of a pass through BR to prevent repeated LDT batches
    TAppUsageLog_t          UsageLogData;
    TAppLoadLog_t           LoadLogData;
    MYKI_BR_ProposedMerge_t ProposedMerge;
    ZoneMap_t               ZoneMap;
    int                     IsUsageLogUpdated;
    int                     IsLoadLogUpdated;

    int                     IsShiftDataLogUpdated;
    OAppShiftSummaryLog_t   ShiftDataSummaryLog;

    MYKI_CD_Route_t         route;                     // Setup and used by myki_br_buildDifferencePriceRequest

    //  Updated at the start of each BR pass, remain constant throughout a single pass, used to cache values to speed up processing and reduce error handling code

    TransportMode_e         TransportMode;              // Current transport mode, determined from StaticData.serviceProviderId.

    const char             *pCardSnapshotPathFormat;    // Card image snapshot path format
    int                     cardSnapshotSequenceNumber; // Card image snapshot sequence number

    int                     isValidPin;                 // TRUE if valid operator PIN, FALSE otherwise

}   MYKI_BR_InternalData_t;

//
//  Returned data (returned to the application)
//

typedef struct
{
    MYKI_BR_RejectReason_t  rejectReason;               // Extra information about a rejected transaction - eg, card blocked, missing touch-on, insufficent balance, expired, etc
    MYKI_BR_AcceptReason_t  acceptReason;               // Extra information about a valid transaction - eg, forced scan-off, etc
    int                     sequenceResult;             // Information regarding whether a scan-on, scan-off or an operator card was processed.
    int                     isTestMode;                 // Non-Zero if the card is a test card, Zero otherwise
    Currency_t              applicableFare;             // Fare charged
    Currency_t              remainingBalance;           // Purse balance after the transaction and any applicable topups/reversals
    Time_t                  expiryDateTime;             // Expiry date/time of the product if a product was used, or the purse otherwise
    int                     productId;                  // ID of the product that was used (-1 meaning purse was used)
    int                     isProvisional;              // Non-Zero if product is provisional
    int                     actionApplied;              // Non-Zero if an actionlist action was applied
    Currency_t              topupAmount;                // Non-Zero amount of topup/reversal if a topup (+ve) or topup reversal (-ve) was applied
    int                     txnSeqNo;                   // Load transaction sequence number
    int                     zoneLow;                    // Low zone of current (possibly provisional) pass used
    int                     zoneHigh;                   // High zone of current (possibly provisional) pass used
    int                     passengerId;                // Passenger id or passenger code
    int                     bypassCode;                 // Unit test support only. Can be used to indicate bypass point to test harness
    int                     blockingReason;             // Card/Application blocking reason. Only applicable if card is rejected
                                                        // with MYKI_BR_REJECT_REASON_CARD_BLOCKED or MYKI_BR_REJECT_REASON_APPLICATION_BLOCKED

    // Operator/Technician Card return data
    char                    staffId[9];                 // Maximum 8 chars + NULL
    Time_t                  staffExpDate;
    int                     staffProviderId;
    int                     staffBaseId;
    char                    staffPin[9];                // Maximum 8 chars + NULL
    StaffRole_t             staffRoles[4];              // Roles
    
    DateC19_t               cardExpiryDate;             // Date of expiry of the card when MYKI_BR_REJECT_REASON_EXPIRED is set. May be from card application or transit application

}   MYKI_BR_ReturnedData_t;

//
//  Test data (only applicable in TEST mode)
//

enum
{
    TEST_CONDITION_NONE = (0),

    TEST_CONDITION_BYPASS_BR_LLSC_0_1,                  // Bypass BR_LLSC_0_1 (card integrity check)

};

typedef struct
{
    int                     condition;                  // Test condition(s)

}   MYKI_BR_TestData_t;

//
//  Context Data (all of the above data structures combined)
//

typedef struct
{
    StaticData_t            StaticData;
    DynamicData_t           DynamicData;
    Tariff_t                Tariff;
    Actionlist_t            ActionList;

    //  New structures to allow us to differentiate between input, internal, and returned data.

    MYKI_BR_ReturnedData_t  ReturnedData;
    MYKI_BR_InternalData_t  InternalData;
    MYKI_BR_TestData_t      TestData;

}   MYKI_BR_ContextData_t;

#ifdef __cplusplus
}
#endif

#endif /* MYKI_BR_CONTEXT_DATA_H_ */
