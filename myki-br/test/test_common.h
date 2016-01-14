//=============================================================================
//
//  Functions common to one or more tests.
//
//=============================================================================

#ifndef TEST_COMMON_H
#define TEST_COMMON_H

//-----------------------------------------------------------------------------
//  Includes
//-----------------------------------------------------------------------------

#include <myki_br_context_data.h>
#include "BR_Common.h"

//-----------------------------------------------------------------------------
//  Constants / Macros
//-----------------------------------------------------------------------------

enum
{
    BR_MODEL_OLD,           // Current BR code model
    BR_MODEL_NEW,           // Potential new BR code model (still under development)

    BR_MODEL_MAX
};

//
//  Product IDs from sample CD - chosen to have the corresponding product type
//

#define PRODUCT_ID_EPASS        2   //  <Record issuer_id = "1" id = "2" type = "LLSC" subtype = "ePass" short_desc = "myki pass" long_desc = "myki pass">
#define PRODUCT_ID_SINGLE       3   //  <Record issuer_id = "1" id = "3" type = "LLSC" subtype = "SingleTrip" short_desc = "City Saver" long_desc = "City Saver">
#define PRODUCT_ID_NHOUR        4   //  <Record issuer_id = "1" id = "4" type = "LLSC" subtype = "nHour" short_desc = "2 hour" long_desc = "2 Hour Stored Value Product">
#define PRODUCT_ID_DAILY        5   //  <Record issuer_id = "1" id = "5" type = "LLSC" subtype = "Daily" short_desc = "Daily" long_desc = "Daily Stored Value Product">
#define PRODUCT_ID_WEEKLY       6   //  <Record issuer_id = "1" id = "6" type = "LLSC" subtype = "Weekly" short_desc = "Weekly" long_desc = "Weekly Stored Value Product">
#define PRODUCT_ID_ENTITLEMENT  100

//
//  Provider IDs from sample CD - chosen to have the corresponding mode of transport
//

#define ProviderId_RAIL         0
#define ProviderId_TRAM         1
#define ProviderId_BUS          2

    /**
     *      Starts testing output conditions.
     */
#define UT_Start( )\
            int _Result = TRUE 
    /**
     *      Test condition and "assert" (set test result to false) if not satisfied.
     *      @param  c   test condition
     */
#define UT_Assert( c )\
            if ( _Result != FALSE && !( c ) )\
            {\
                CsErrx( "FAILED: " #c );\
                _Result = FALSE;\
            }

    /**
     *      Returns test result (TRUE=passed or FALSE=failed).
     */
#define UT_Result( )\
            ( _Result )

//-----------------------------------------------------------------------------
//  Typedefs / Structures
//-----------------------------------------------------------------------------

//
//  Single structure to hold pointers to virtual card image components
//

typedef struct
{
    MYKI_CAControl_t            *pMYKI_CAControl;
    MYKI_TAControl_t            *pMYKI_TAControl;
    MYKI_TAProduct_t            *pMYKI_TAProduct[ 5 ];
    MYKI_TAPurseBalance_t       *pMYKI_TAPurseBalance;
    MYKI_TAPurseControl_t       *pMYKI_TAPurseControl;

    int                         NextProductSerial;

}   CardImage_t;

//-----------------------------------------------------------------------------
//  External variables - these should be avoided.
//-----------------------------------------------------------------------------

//  These are here so we don't have to bother with get() functions for them. Should revisit this later...
//
//  Source : test_common.c

extern  StaticData_t                staticData;
extern  DynamicData_t               dynamicData;
extern  Tariff_t                    Tariff;
extern  Actionlist_t                actionList;
extern  MYKI_BR_ContextData_t       data;

//-----------------------------------------------------------------------------
//  Exported Function Prototypes
//-----------------------------------------------------------------------------

int     CreateCardImage_Empty( CardImage_t *pCardImage );

void    CardImage1( MYKI_BR_ContextData_t *pData );
void    CardImage2( MYKI_BR_ContextData_t *pData );
void    CardImage3( MYKI_BR_ContextData_t *pData );
void    CardImage4( MYKI_BR_ContextData_t *pData );
void    CardImage5( MYKI_BR_ContextData_t *pData );
void    CardImage6( MYKI_BR_ContextData_t *pData );
void    CardImage7( MYKI_BR_ContextData_t *pData );
void    CardImage8( MYKI_BR_ContextData_t *pData );
void    CardImage9( MYKI_BR_ContextData_t *pData );

void    ShowCardImage( char *filename );

//============================================================================
//  Test Harness functions provided by stubs
//
void MYKI_CD_setStartDateTime(Time_t dateTime);
void MYKI_CD_setCitySaverZone(U8_t zoneId);
void MYKI_CD_setEarlyBirdCutOffTime(int cutOffTime);
void MYKI_CD_setEarlyBirdProductId(int id);
void MYKI_CD_setEarlyBirdStation(U16_t stationId1, U16_t stationId2);
void MYKI_ACTIONLIST_setDeviceHotlisted(U32_t samId);
void MYKI_CD_setDifferentialPriceStructure(MYKI_CD_DifferentialPricing_t *differentialPricing);
void MYKI_CD_setDifferentialPriceProvisionalStructure(MYKI_CD_DifferentialPricingProvisional_t *differentialPricing);

//
//  Test function prototypes - should ideally be in header files specific to each test module,
//  or in a single file along the lines of "tests.h", but putting them here is simpler and good enough.
//

//  test_BR_LLSC_0_A.c

int     test_BR_LLSC_0_A_001( MYKI_BR_ContextData_t *pData );
int     test_BR_LLSC_0_A_002( MYKI_BR_ContextData_t *pData );

//  test_BR_LLSC_0_B.c

int     test_BR_LLSC_0_B_001( MYKI_BR_ContextData_t *pData );
int     test_BR_LLSC_0_B_002( MYKI_BR_ContextData_t *pData );
int     test_BR_LLSC_0_B_003( MYKI_BR_ContextData_t *pData );
int     test_BR_LLSC_0_B_004( MYKI_BR_ContextData_t *pData );
int     test_BR_LLSC_0_B_005( MYKI_BR_ContextData_t *pData );
int     test_BR_LLSC_0_B_006( MYKI_BR_ContextData_t *pData );
int     test_BR_LLSC_0_B_007( MYKI_BR_ContextData_t *pData );

//  test_BR_LLSC_0_1.c

int     test_BR_LLSC_0_1_001( MYKI_BR_ContextData_t *pData );
int     test_BR_LLSC_0_1_002( MYKI_BR_ContextData_t *pData );
int     test_BR_LLSC_0_1_003( MYKI_BR_ContextData_t *pData );
int     test_BR_LLSC_0_1_004( MYKI_BR_ContextData_t *pData );
int     test_BR_LLSC_0_1_005( MYKI_BR_ContextData_t *pData );
int     test_BR_LLSC_0_1_006( MYKI_BR_ContextData_t *pData );
int     test_BR_LLSC_0_1_007( MYKI_BR_ContextData_t *pData );
int     test_BR_LLSC_0_1_008( MYKI_BR_ContextData_t *pData );
int     test_BR_LLSC_0_1_009( MYKI_BR_ContextData_t *pData );
int     test_BR_LLSC_0_1_010( MYKI_BR_ContextData_t *pData );
int     test_BR_LLSC_0_1_011( MYKI_BR_ContextData_t *pData );
int     test_BR_LLSC_0_1_012( MYKI_BR_ContextData_t *pData );
int     test_BR_LLSC_0_1_013( MYKI_BR_ContextData_t *pData );
int     test_BR_LLSC_0_1_014( MYKI_BR_ContextData_t *pData );

//  test_BR_LLSC_1_1.c

int     test_BR_LLSC_1_1_001( MYKI_BR_ContextData_t *pData );
int     test_BR_LLSC_1_1_002( MYKI_BR_ContextData_t *pData );

//  test_BR_LLSC_1_2.c

int     test_BR_LLSC_1_2_001( MYKI_BR_ContextData_t *pData );
int     test_BR_LLSC_1_2_002( MYKI_BR_ContextData_t *pData );

//  test_BR_LLSC_1_3.c

int     test_BR_LLSC_1_3_001( MYKI_BR_ContextData_t *pData );
int     test_BR_LLSC_1_3_002( MYKI_BR_ContextData_t *pData );
int     test_BR_LLSC_1_3_003( MYKI_BR_ContextData_t *pData );
int     test_BR_LLSC_1_3_004( MYKI_BR_ContextData_t *pData );
int     test_BR_LLSC_1_3_005( MYKI_BR_ContextData_t *pData );

//  test_BR_LLSC_1_5.c

int     test_BR_LLSC_1_5_001( MYKI_BR_ContextData_t *pData );
int     test_BR_LLSC_1_5_002( MYKI_BR_ContextData_t *pData );
int     test_BR_LLSC_1_5_003( MYKI_BR_ContextData_t *pData );
int     test_BR_LLSC_1_5_004( MYKI_BR_ContextData_t *pData );
int     test_BR_LLSC_1_5_005( MYKI_BR_ContextData_t *pData );
int     test_BR_LLSC_1_5_006( MYKI_BR_ContextData_t *pData );
int     test_BR_LLSC_1_5_007( MYKI_BR_ContextData_t *pData );

//  test_BR_LLSC_1_6.c

int     test_BR_LLSC_1_6_001( MYKI_BR_ContextData_t *pData );
int     test_BR_LLSC_1_6_002( MYKI_BR_ContextData_t *pData );
int     test_BR_LLSC_1_6_003( MYKI_BR_ContextData_t *pData );
int     test_BR_LLSC_1_6_004( MYKI_BR_ContextData_t *pData );
int     test_BR_LLSC_1_6_005( MYKI_BR_ContextData_t *pData );
int     test_BR_LLSC_1_6_006( MYKI_BR_ContextData_t *pData );
int     test_BR_LLSC_1_6_007( MYKI_BR_ContextData_t *pData );
int     test_BR_LLSC_1_6_008( MYKI_BR_ContextData_t *pData );
int     test_BR_LLSC_1_6_009( MYKI_BR_ContextData_t *pData );

//  test_BR_LLSC_1_7.c

int     test_BR_LLSC_1_7_001( MYKI_BR_ContextData_t *pData );
int     test_BR_LLSC_1_7_002( MYKI_BR_ContextData_t *pData );
int     test_BR_LLSC_1_7_003( MYKI_BR_ContextData_t *pData );
int     test_BR_LLSC_1_7_004( MYKI_BR_ContextData_t *pData );
int     test_BR_LLSC_1_7_005( MYKI_BR_ContextData_t *pData );
int     test_BR_LLSC_1_7_006( MYKI_BR_ContextData_t *pData );
int     test_BR_LLSC_1_7_007( MYKI_BR_ContextData_t *pData );
int     test_BR_LLSC_1_7_008( MYKI_BR_ContextData_t *pData );

//  test_BR_LLSC_1_8.c

int     test_BR_LLSC_1_8_001( MYKI_BR_ContextData_t *pData );
int     test_BR_LLSC_1_8_002( MYKI_BR_ContextData_t *pData );
int     test_BR_LLSC_1_8_003( MYKI_BR_ContextData_t *pData );
int     test_BR_LLSC_1_8_004( MYKI_BR_ContextData_t *pData );
int     test_BR_LLSC_1_8_005( MYKI_BR_ContextData_t *pData );
int     test_BR_LLSC_1_8_006( MYKI_BR_ContextData_t *pData );
int     test_BR_LLSC_1_8_007( MYKI_BR_ContextData_t *pData );
int     test_BR_LLSC_1_8_008( MYKI_BR_ContextData_t *pData );

//  test_BR_LLSC_1_9.c

int     test_BR_LLSC_1_9_001( MYKI_BR_ContextData_t *pData );
int     test_BR_LLSC_1_9_002( MYKI_BR_ContextData_t *pData );
int     test_BR_LLSC_1_9_003( MYKI_BR_ContextData_t *pData );
int     test_BR_LLSC_1_9_004( MYKI_BR_ContextData_t *pData );
int     test_BR_LLSC_1_9_005( MYKI_BR_ContextData_t *pData );
int     test_BR_LLSC_1_9_006( MYKI_BR_ContextData_t *pData );
int     test_BR_LLSC_1_9_007( MYKI_BR_ContextData_t *pData );
int     test_BR_LLSC_1_9_008( MYKI_BR_ContextData_t *pData );

//  test_BR_LLSC_1_10.c

int     test_BR_LLSC_1_10_001( MYKI_BR_ContextData_t *pData );
int     test_BR_LLSC_1_10_002( MYKI_BR_ContextData_t *pData );
int     test_BR_LLSC_1_10_003( MYKI_BR_ContextData_t *pData );
int     test_BR_LLSC_1_10_004( MYKI_BR_ContextData_t *pData );
int     test_BR_LLSC_1_10_005( MYKI_BR_ContextData_t *pData );

//  test_BR_LLSC_2_2.c
int     test_BR_LLSC_2_2_001( MYKI_BR_ContextData_t *pData );
int     test_BR_LLSC_2_2_002( MYKI_BR_ContextData_t *pData );

//  test_BR_LLSC_2_5.c
int     test_BR_LLSC_2_5_001( MYKI_BR_ContextData_t *pData );
int     test_BR_LLSC_2_5_002( MYKI_BR_ContextData_t *pData );
int     test_BR_LLSC_2_5_003( MYKI_BR_ContextData_t *pData );

//  test_BR_LLSC_2_6.c
int     test_BR_LLSC_2_6_001a( MYKI_BR_ContextData_t *pData );
int     test_BR_LLSC_2_6_001b( MYKI_BR_ContextData_t *pData );
int     test_BR_LLSC_2_6_001c( MYKI_BR_ContextData_t *pData );
int     test_BR_LLSC_2_6_001d( MYKI_BR_ContextData_t *pData );
int     test_BR_LLSC_2_6_001e( MYKI_BR_ContextData_t *pData );
int     test_BR_LLSC_2_6_001f( MYKI_BR_ContextData_t *pData );
int     test_BR_LLSC_2_6_001g( MYKI_BR_ContextData_t *pData );
int     test_BR_LLSC_2_6_002( MYKI_BR_ContextData_t *pData );
int     test_BR_LLSC_2_6_003( MYKI_BR_ContextData_t *pData );
int     test_BR_LLSC_2_6_004( MYKI_BR_ContextData_t *pData );

//  test_BR_LLSC_2_7.c
int     test_BR_LLSC_2_7_001( MYKI_BR_ContextData_t *pData );
int     test_BR_LLSC_2_7_002( MYKI_BR_ContextData_t *pData );
int     test_BR_LLSC_2_7_003( MYKI_BR_ContextData_t *pData );
int     test_BR_LLSC_2_7_004( MYKI_BR_ContextData_t *pData );

//  test_BR_LLSC_2_11.c
int     test_BR_LLSC_2_11_001a( MYKI_BR_ContextData_t *pData );
int     test_BR_LLSC_2_11_001b( MYKI_BR_ContextData_t *pData );
int     test_BR_LLSC_2_11_001c( MYKI_BR_ContextData_t *pData );
int     test_BR_LLSC_2_11_001d( MYKI_BR_ContextData_t *pData );
int     test_BR_LLSC_2_11_001e( MYKI_BR_ContextData_t *pData );
int     test_BR_LLSC_2_11_002( MYKI_BR_ContextData_t *pData );

//  test_BR_LLSC_2_13.c
int     test_BR_LLSC_2_13_001a( MYKI_BR_ContextData_t *pData );
int     test_BR_LLSC_2_13_001b( MYKI_BR_ContextData_t *pData );
int     test_BR_LLSC_2_13_001c( MYKI_BR_ContextData_t *pData );
int     test_BR_LLSC_2_13_001d( MYKI_BR_ContextData_t *pData );
int     test_BR_LLSC_2_13_001e( MYKI_BR_ContextData_t *pData );
int     test_BR_LLSC_2_13_001f( MYKI_BR_ContextData_t *pData );
int     test_BR_LLSC_2_13_002( MYKI_BR_ContextData_t *pData );
int     test_BR_LLSC_2_13_003( MYKI_BR_ContextData_t *pData );
int     test_BR_LLSC_2_13_004( MYKI_BR_ContextData_t *pData );
int     test_BR_LLSC_2_13_005( MYKI_BR_ContextData_t *pData );

//  test_BR_LLSC_2_14.c
int     test_BR_LLSC_2_14_001( MYKI_BR_ContextData_t *pData );
int     test_BR_LLSC_2_14_002( MYKI_BR_ContextData_t *pData );
int     test_BR_LLSC_2_14_003( MYKI_BR_ContextData_t *pData );

// test_BR_LLSC_4_1.c
int     test_BR_LLSC_4_1_001( MYKI_BR_ContextData_t *pData );
int     test_BR_LLSC_4_1_002( MYKI_BR_ContextData_t *pData );
int     test_BR_LLSC_4_1_003( MYKI_BR_ContextData_t *pData );
int     test_BR_LLSC_4_1_004( MYKI_BR_ContextData_t *pData );

//  test_BR_LLSC_4_11.c
int     test_BR_LLSC_4_11_001( MYKI_BR_ContextData_t *pData );
int     test_BR_LLSC_4_11_002( MYKI_BR_ContextData_t *pData );

//  test_BR_LLSC_4_14.c
int     test_BR_LLSC_4_14_001( MYKI_BR_ContextData_t *pData );
int     test_BR_LLSC_4_14_002( MYKI_BR_ContextData_t *pData );

//  test_BR_LLSC_4_15.c
int     test_BR_LLSC_4_15_001( MYKI_BR_ContextData_t *pData );
int     test_BR_LLSC_4_15_002( MYKI_BR_ContextData_t *pData );

//  test_BR_LLSC_4_17.c
int     test_BR_LLSC_4_17_001( MYKI_BR_ContextData_t *pData );
int     test_BR_LLSC_4_17_002( MYKI_BR_ContextData_t *pData );
int     test_BR_LLSC_4_17_003( MYKI_BR_ContextData_t *pData );
int     test_BR_LLSC_4_17_004( MYKI_BR_ContextData_t *pData );
int     test_BR_LLSC_4_17_005( MYKI_BR_ContextData_t *pData );
int     test_BR_LLSC_4_17_006( MYKI_BR_ContextData_t *pData );
int     test_BR_LLSC_4_17_007( MYKI_BR_ContextData_t *pData );
int     test_BR_LLSC_4_17_008( MYKI_BR_ContextData_t *pData );

//  test_BR_LLSC_4_19.c
int     test_BR_LLSC_4_19_001( MYKI_BR_ContextData_t *pData );
int     test_BR_LLSC_4_19_002a( MYKI_BR_ContextData_t *pData );
int     test_BR_LLSC_4_19_002b( MYKI_BR_ContextData_t *pData );
int     test_BR_LLSC_4_19_002c( MYKI_BR_ContextData_t *pData );
int     test_BR_LLSC_4_19_002d( MYKI_BR_ContextData_t *pData );
int     test_BR_LLSC_4_19_002e( MYKI_BR_ContextData_t *pData );

//  test_BR_LLSC_5_2.c
int     test_BR_LLSC_5_2_001( MYKI_BR_ContextData_t *pData );
int     test_BR_LLSC_5_2_002( MYKI_BR_ContextData_t *pData );
int     test_BR_LLSC_5_2_003( MYKI_BR_ContextData_t *pData );
int     test_BR_LLSC_5_2_004( MYKI_BR_ContextData_t *pData );

//  test_BR_LLSC_5_3.c
int     test_BR_LLSC_5_3_001a( MYKI_BR_ContextData_t *pData );
int     test_BR_LLSC_5_3_001b( MYKI_BR_ContextData_t *pData );
int     test_BR_LLSC_5_3_001c( MYKI_BR_ContextData_t *pData );
int     test_BR_LLSC_5_3_001d( MYKI_BR_ContextData_t *pData );
int     test_BR_LLSC_5_3_001e( MYKI_BR_ContextData_t *pData );
int     test_BR_LLSC_5_3_002a( MYKI_BR_ContextData_t *pData );
int     test_BR_LLSC_5_3_002b( MYKI_BR_ContextData_t *pData );
int     test_BR_LLSC_5_3_002c( MYKI_BR_ContextData_t *pData );

//  test_BR_LLSC_5_9.c
int     test_BR_LLSC_5_9_001( MYKI_BR_ContextData_t *pData );
int     test_BR_LLSC_5_9_002( MYKI_BR_ContextData_t *pData );
int     test_BR_LLSC_5_9_003( MYKI_BR_ContextData_t *pData );
int     test_BR_LLSC_5_9_004( MYKI_BR_ContextData_t *pData );

//  test_BR_LLSC_6_1.c
int     test_BR_LLSC_6_1_001( MYKI_BR_ContextData_t *pData );
int     test_BR_LLSC_6_1_002( MYKI_BR_ContextData_t *pData );
int     test_BR_LLSC_6_1_003( MYKI_BR_ContextData_t *pData );
int     test_BR_LLSC_6_1_004( MYKI_BR_ContextData_t *pData );
int     test_BR_LLSC_6_1_005( MYKI_BR_ContextData_t *pData );
int     test_BR_LLSC_6_1_006( MYKI_BR_ContextData_t *pData );

//  test_BR_LLSC_6_3.c
int     test_BR_LLSC_6_3_001( MYKI_BR_ContextData_t *pData );
int     test_BR_LLSC_6_3_002( MYKI_BR_ContextData_t *pData );
int     test_BR_LLSC_6_3_003( MYKI_BR_ContextData_t *pData );
int     test_BR_LLSC_6_3_004( MYKI_BR_ContextData_t *pData );
int     test_BR_LLSC_6_3_005( MYKI_BR_ContextData_t *pData );

//  test_BR_LLSC_6_10.c
int     test_BR_LLSC_6_10_001( MYKI_BR_ContextData_t *pData );
int     test_BR_LLSC_6_10_002( MYKI_BR_ContextData_t *pData );
int     test_BR_LLSC_6_10_003( MYKI_BR_ContextData_t *pData );
int     test_BR_LLSC_6_10_004( MYKI_BR_ContextData_t *pData );
int     test_BR_LLSC_6_10_005( MYKI_BR_ContextData_t *pData );

//  test_BR_LLSC_6_18.c
int     test_BR_LLSC_6_18_000( MYKI_BR_ContextData_t *pData );
int     test_BR_LLSC_6_18_001( MYKI_BR_ContextData_t *pData );
int     test_BR_LLSC_6_18_002( MYKI_BR_ContextData_t *pData );
int     test_BR_LLSC_6_18_003( MYKI_BR_ContextData_t *pData );
int     test_BR_LLSC_6_18_004( MYKI_BR_ContextData_t *pData );
int     test_BR_LLSC_6_18_005( MYKI_BR_ContextData_t *pData );
int     test_BR_LLSC_6_18_006( MYKI_BR_ContextData_t *pData );
int     test_BR_LLSC_6_18_007( MYKI_BR_ContextData_t *pData );
int     test_BR_LLSC_6_18_008( MYKI_BR_ContextData_t *pData );
int     test_BR_LLSC_6_18_009( MYKI_BR_ContextData_t *pData );
int     test_BR_LLSC_6_18_010( MYKI_BR_ContextData_t *pData );
int     test_BR_LLSC_6_18_011( MYKI_BR_ContextData_t *pData );
int     test_BR_LLSC_6_18_012( MYKI_BR_ContextData_t *pData );

//  test_BR_LLSC_6_19.c
int     test_BR_LLSC_6_19_001( MYKI_BR_ContextData_t *pData );
int     test_BR_LLSC_6_19_002( MYKI_BR_ContextData_t *pData );
int     test_BR_LLSC_6_19_003( MYKI_BR_ContextData_t *pData );
int     test_BR_LLSC_6_19_004( MYKI_BR_ContextData_t *pData );
int     test_BR_LLSC_6_19_005( MYKI_BR_ContextData_t *pData );

//  test_BR_LLSC_6_20.c
int     test_BR_LLSC_6_20_001( MYKI_BR_ContextData_t *pData );
int     test_BR_LLSC_6_20_002( MYKI_BR_ContextData_t *pData );
int     test_BR_LLSC_6_20_003( MYKI_BR_ContextData_t *pData );
int     test_BR_LLSC_6_20_004( MYKI_BR_ContextData_t *pData );
int     test_BR_LLSC_6_20_005( MYKI_BR_ContextData_t *pData );
int     test_BR_LLSC_6_20_006( MYKI_BR_ContextData_t *pData );
int     test_BR_LLSC_6_20_007( MYKI_BR_ContextData_t *pData );
int     test_BR_LLSC_6_20_008( MYKI_BR_ContextData_t *pData );
int     test_BR_LLSC_6_20_009( MYKI_BR_ContextData_t *pData );
int     test_BR_LLSC_6_20_010( MYKI_BR_ContextData_t *pData );

//  test_BR_LLSC_6_21.c
int     test_BR_LLSC_6_21_001( MYKI_BR_ContextData_t *pData );
int     test_BR_LLSC_6_21_002( MYKI_BR_ContextData_t *pData );
int     test_BR_LLSC_6_21_003( MYKI_BR_ContextData_t *pData );
int     test_BR_LLSC_6_21_004( MYKI_BR_ContextData_t *pData );
int     test_BR_LLSC_6_21_005( MYKI_BR_ContextData_t *pData );
int     test_BR_LLSC_6_21_006( MYKI_BR_ContextData_t *pData );
int     test_BR_LLSC_6_21_007( MYKI_BR_ContextData_t *pData );
int     test_BR_LLSC_6_21_008( MYKI_BR_ContextData_t *pData );
int     test_BR_LLSC_6_21_009( MYKI_BR_ContextData_t *pData );

//  test_BR_LLSC_6_22.c
int     test_BR_LLSC_6_22_001( MYKI_BR_ContextData_t *pData );
int     test_BR_LLSC_6_22_002( MYKI_BR_ContextData_t *pData );
int     test_BR_LLSC_6_22_003( MYKI_BR_ContextData_t *pData );
int     test_BR_LLSC_6_22_004( MYKI_BR_ContextData_t *pData );
int     test_BR_LLSC_6_22_005( MYKI_BR_ContextData_t *pData );
int     test_BR_LLSC_6_22_006( MYKI_BR_ContextData_t *pData );
int     test_BR_LLSC_6_22_007( MYKI_BR_ContextData_t *pData );
int     test_BR_LLSC_6_22_008( MYKI_BR_ContextData_t *pData );
int     test_BR_LLSC_6_22_009( MYKI_BR_ContextData_t *pData );
int     test_BR_LLSC_6_22_010( MYKI_BR_ContextData_t *pData );

//  test_BR_LLSC_6_23.c
int     test_BR_LLSC_6_23_001( MYKI_BR_ContextData_t *pData );
int     test_BR_LLSC_6_23_002( MYKI_BR_ContextData_t *pData );
int     test_BR_LLSC_6_23_003( MYKI_BR_ContextData_t *pData );
int     test_BR_LLSC_6_23_004( MYKI_BR_ContextData_t *pData );
int     test_BR_LLSC_6_23_005( MYKI_BR_ContextData_t *pData );
int     test_BR_LLSC_6_23_006( MYKI_BR_ContextData_t *pData );
int     test_BR_LLSC_6_23_007( MYKI_BR_ContextData_t *pData );
int     test_BR_LLSC_6_23_008( MYKI_BR_ContextData_t *pData );
int     test_BR_LLSC_6_23_009( MYKI_BR_ContextData_t *pData );
int     test_BR_LLSC_6_23_010( MYKI_BR_ContextData_t *pData );
int     test_BR_LLSC_6_23_011( MYKI_BR_ContextData_t *pData );
int     test_BR_LLSC_6_23_012( MYKI_BR_ContextData_t *pData );
int     test_BR_LLSC_6_23_013( MYKI_BR_ContextData_t *pData );
int     test_BR_LLSC_6_23_014( MYKI_BR_ContextData_t *pData );

//  test_BR_LLSC_7_4.c
int     test_BR_LLSC_7_4_001( MYKI_BR_ContextData_t *pData );
int     test_BR_LLSC_7_4_002( MYKI_BR_ContextData_t *pData );
int     test_BR_LLSC_7_4_003( MYKI_BR_ContextData_t *pData );
int     test_BR_LLSC_7_4_004( MYKI_BR_ContextData_t *pData );
int     test_BR_LLSC_7_4_005( MYKI_BR_ContextData_t *pData );
int     test_BR_LLSC_7_4_006( MYKI_BR_ContextData_t *pData );
int     test_BR_LLSC_7_4_007( MYKI_BR_ContextData_t *pData );
int     test_BR_LLSC_7_4_008( MYKI_BR_ContextData_t *pData );
int     test_BR_LLSC_7_4_009( MYKI_BR_ContextData_t *pData );
int     test_BR_LLSC_7_4_010( MYKI_BR_ContextData_t *pData );
int     test_BR_LLSC_7_4_011( MYKI_BR_ContextData_t *pData );
int     test_BR_LLSC_7_4_012( MYKI_BR_ContextData_t *pData );
int     test_BR_LLSC_7_4_013( MYKI_BR_ContextData_t *pData );

//  test_BR_LLSC_7_5.c
int     test_BR_LLSC_7_5_001( MYKI_BR_ContextData_t *pData );
int     test_BR_LLSC_7_5_002( MYKI_BR_ContextData_t *pData );
int     test_BR_LLSC_7_5_003( MYKI_BR_ContextData_t *pData );
int     test_BR_LLSC_7_5_004( MYKI_BR_ContextData_t *pData );
int     test_BR_LLSC_7_5_005( MYKI_BR_ContextData_t *pData );
int     test_BR_LLSC_7_5_006( MYKI_BR_ContextData_t *pData );
int     test_BR_LLSC_7_5_007( MYKI_BR_ContextData_t *pData );
int     test_BR_LLSC_7_5_008( MYKI_BR_ContextData_t *pData );
int     test_BR_LLSC_7_5_009( MYKI_BR_ContextData_t *pData );
int     test_BR_LLSC_7_5_010( MYKI_BR_ContextData_t *pData );
int     test_BR_LLSC_7_5_011( MYKI_BR_ContextData_t *pData );
int     test_BR_LLSC_7_5_012( MYKI_BR_ContextData_t *pData );
int     test_BR_LLSC_7_5_013( MYKI_BR_ContextData_t *pData );
int     test_BR_LLSC_7_5_014( MYKI_BR_ContextData_t *pData );

//
//  test_BR_LLSC_9_1.c
int     test_BR_LLSC_9_1_001( MYKI_BR_ContextData_t *pData );
int     test_BR_LLSC_9_1_002( MYKI_BR_ContextData_t *pData );
int     test_BR_LLSC_9_1_003( MYKI_BR_ContextData_t *pData );
int     test_BR_LLSC_9_1_004( MYKI_BR_ContextData_t *pData );
int     test_BR_LLSC_9_1_005( MYKI_BR_ContextData_t *pData );
int     test_BR_LLSC_9_1_006( MYKI_BR_ContextData_t *pData );
int     test_BR_LLSC_9_1_007( MYKI_BR_ContextData_t *pData );
int     test_BR_LLSC_9_1_008( MYKI_BR_ContextData_t *pData );
int     test_BR_LLSC_9_1_009( MYKI_BR_ContextData_t *pData );

//  test_BR_LLSC_10_1.c

int     test_BR_LLSC_10_1_001( MYKI_BR_ContextData_t *pData );
int     test_BR_LLSC_10_1_002( MYKI_BR_ContextData_t *pData );

//  test_BR_LLSC_10_3.c

int     test_BR_LLSC_10_3_001( MYKI_BR_ContextData_t *pData );
int     test_BR_LLSC_10_3_002( MYKI_BR_ContextData_t *pData );
int     test_BR_LLSC_10_3_003( MYKI_BR_ContextData_t *pData );
int     test_BR_LLSC_10_3_004( MYKI_BR_ContextData_t *pData );
int     test_BR_LLSC_10_3_005( MYKI_BR_ContextData_t *pData );

//  test_BR_LLSC_10_4.c

int     test_BR_LLSC_10_4_001( MYKI_BR_ContextData_t *pData );
int     test_BR_LLSC_10_4_002( MYKI_BR_ContextData_t *pData );
int     test_BR_LLSC_10_4_003( MYKI_BR_ContextData_t *pData );
int     test_BR_LLSC_10_4_004( MYKI_BR_ContextData_t *pData );
int     test_BR_LLSC_10_4_005( MYKI_BR_ContextData_t *pData );

//  test_BR_LLSC_10_6.c

int     test_BR_LLSC_10_6_001( MYKI_BR_ContextData_t *pData );
int     test_BR_LLSC_10_6_002( MYKI_BR_ContextData_t *pData );
int     test_BR_LLSC_10_6_003( MYKI_BR_ContextData_t *pData );
int     test_BR_LLSC_10_6_004( MYKI_BR_ContextData_t *pData );

//  test_BR_LLSC_99_1.c
int     test_BR_LLSC_99_1_001a( MYKI_BR_ContextData_t *pData );
int     test_BR_LLSC_99_1_001b( MYKI_BR_ContextData_t *pData );
int     test_BR_LLSC_99_1_001c( MYKI_BR_ContextData_t *pData );
int     test_BR_LLSC_99_1_001d( MYKI_BR_ContextData_t *pData );
int     test_BR_LLSC_99_1_001e( MYKI_BR_ContextData_t *pData );
int     test_BR_LLSC_99_1_001f( MYKI_BR_ContextData_t *pData );
int     test_BR_LLSC_99_1_001g( MYKI_BR_ContextData_t *pData );
int     test_BR_LLSC_99_1_001h( MYKI_BR_ContextData_t *pData );
int     test_BR_LLSC_99_1_001i( MYKI_BR_ContextData_t *pData );
int     test_BR_LLSC_99_1_001j( MYKI_BR_ContextData_t *pData );
int     test_BR_LLSC_99_1_001k( MYKI_BR_ContextData_t *pData );
int     test_BR_LLSC_99_1_001l( MYKI_BR_ContextData_t *pData );
int     test_BR_LLSC_99_1_001m( MYKI_BR_ContextData_t *pData );
int     test_BR_LLSC_99_1_001n( MYKI_BR_ContextData_t *pData );
int     test_BR_LLSC_99_1_002( MYKI_BR_ContextData_t *pData );

// test_BR_OSC_0_B.c

int     test_BR_OSC_0_B_001( MYKI_BR_ContextData_t *pData );
int     test_BR_OSC_0_B_002( MYKI_BR_ContextData_t *pData );
int     test_BR_OSC_0_B_003( MYKI_BR_ContextData_t *pData );
int     test_BR_OSC_0_B_004( MYKI_BR_ContextData_t *pData );
int     test_BR_OSC_0_B_005( MYKI_BR_ContextData_t *pData );

int     test_BR_OSC_1_4_001( MYKI_BR_ContextData_t *pData );
int     test_BR_OSC_1_4_002a( MYKI_BR_ContextData_t *pData );
int     test_BR_OSC_1_4_002b( MYKI_BR_ContextData_t *pData );
int     test_BR_OSC_1_4_002c( MYKI_BR_ContextData_t *pData );

//  test_BR_VIX_OSC_2_1.c
int     test_BR_VIX_OSC_2_1_001a( MYKI_BR_ContextData_t *pData );
int     test_BR_VIX_OSC_2_1_001b( MYKI_BR_ContextData_t *pData );
int     test_BR_VIX_OSC_2_1_001c( MYKI_BR_ContextData_t *pData );
int     test_BR_VIX_OSC_2_1_001d( MYKI_BR_ContextData_t *pData );
int     test_BR_VIX_OSC_2_1_002a( MYKI_BR_ContextData_t *pData );
int     test_BR_VIX_OSC_2_1_002b( MYKI_BR_ContextData_t *pData );
int     test_BR_VIX_OSC_2_1_003( MYKI_BR_ContextData_t *pData );

//  test_BR_VIX_OSC_2_3.c
int     test_BR_VIX_OSC_2_3_001a( MYKI_BR_ContextData_t *pData );
int     test_BR_VIX_OSC_2_3_001b( MYKI_BR_ContextData_t *pData );
int     test_BR_VIX_OSC_2_3_001c( MYKI_BR_ContextData_t *pData );
int     test_BR_VIX_OSC_2_3_002( MYKI_BR_ContextData_t *pData );
int     test_BR_VIX_OSC_2_3_003( MYKI_BR_ContextData_t *pData );

//  test_BR_VIX_OSC_2_5.c
int     test_BR_VIX_OSC_2_5_001a( MYKI_BR_ContextData_t *pData );
int     test_BR_VIX_OSC_2_5_001b( MYKI_BR_ContextData_t *pData );
int     test_BR_VIX_OSC_2_5_002a( MYKI_BR_ContextData_t *pData );

//  test_BR_VIX_OSC_3_1.c
int     test_BR_VIX_OSC_3_1_001a( MYKI_BR_ContextData_t *pData );
int     test_BR_VIX_OSC_3_1_001b( MYKI_BR_ContextData_t *pData );
int     test_BR_VIX_OSC_3_1_002a( MYKI_BR_ContextData_t *pData );
int     test_BR_VIX_OSC_3_1_002b( MYKI_BR_ContextData_t *pData );

//  test_BR_VIX_OSC_3_2.c
int     test_BR_VIX_OSC_3_2_001a( MYKI_BR_ContextData_t *pData );
int     test_BR_VIX_OSC_3_2_001b( MYKI_BR_ContextData_t *pData );
int     test_BR_VIX_OSC_3_2_001c( MYKI_BR_ContextData_t *pData );
int     test_BR_VIX_OSC_3_2_002a( MYKI_BR_ContextData_t *pData );
int     test_BR_VIX_OSC_3_2_002b( MYKI_BR_ContextData_t *pData );
int     test_BR_VIX_OSC_3_2_002c( MYKI_BR_ContextData_t *pData );

//  test_BR_VIX_OSC_3_4.c
int     test_BR_VIX_OSC_3_4_001a( MYKI_BR_ContextData_t *pData );
int     test_BR_VIX_OSC_3_4_001b( MYKI_BR_ContextData_t *pData );
int     test_BR_VIX_OSC_3_4_002a( MYKI_BR_ContextData_t *pData );

//  test_BR_VIX_OSC_3_5.c
int     test_BR_VIX_OSC_3_5_001a( MYKI_BR_ContextData_t *pData );
int     test_BR_VIX_OSC_3_5_001b( MYKI_BR_ContextData_t *pData );
int     test_BR_VIX_OSC_3_5_002a( MYKI_BR_ContextData_t *pData );
int     test_BR_VIX_OSC_3_5_002b( MYKI_BR_ContextData_t *pData );

//  test_BR_VIX_OSC_3_3.c
int     test_BR_VIX_OSC_3_3_001a( MYKI_BR_ContextData_t *pData );
int     test_BR_VIX_OSC_3_3_001b( MYKI_BR_ContextData_t *pData );
int     test_BR_VIX_OSC_3_3_002a( MYKI_BR_ContextData_t *pData );
int     test_BR_VIX_OSC_3_3_002b( MYKI_BR_ContextData_t *pData );
int     test_BR_VIX_OSC_3_3_002c( MYKI_BR_ContextData_t *pData );

//  test_BR_VIX_LLSC_20_1.c
int test_BR_VIX_LLSC_20_1_001a( MYKI_BR_ContextData_t *pData );
int test_BR_VIX_LLSC_20_1_001b( MYKI_BR_ContextData_t *pData );
int test_BR_VIX_LLSC_20_1_001c( MYKI_BR_ContextData_t *pData );
int test_BR_VIX_LLSC_20_1_001d( MYKI_BR_ContextData_t *pData );
int test_BR_VIX_LLSC_20_1_001e( MYKI_BR_ContextData_t *pData );
int test_BR_VIX_LLSC_20_1_002a( MYKI_BR_ContextData_t *pData );

//  test_BR_VIX_LLSC_20_2.c
int test_BR_VIX_LLSC_20_2_001a( MYKI_BR_ContextData_t *pData );
int test_BR_VIX_LLSC_20_2_001b( MYKI_BR_ContextData_t *pData );
int test_BR_VIX_LLSC_20_2_001c( MYKI_BR_ContextData_t *pData );
int test_BR_VIX_LLSC_20_2_001d( MYKI_BR_ContextData_t *pData );
int test_BR_VIX_LLSC_20_2_002a( MYKI_BR_ContextData_t *pData );

//  test_BR_VIX_LLSC_21_1.c
int test_BR_VIX_LLSC_21_1_001a( MYKI_BR_ContextData_t *pData );
int test_BR_VIX_LLSC_21_1_001b( MYKI_BR_ContextData_t *pData );
int test_BR_VIX_LLSC_21_1_002a( MYKI_BR_ContextData_t *pData );

//  test_BR_VIX_LLSC_21_2.c
int test_BR_VIX_LLSC_21_2_001a( MYKI_BR_ContextData_t *pData );
int test_BR_VIX_LLSC_21_2_001b( MYKI_BR_ContextData_t *pData );
int test_BR_VIX_LLSC_21_2_002a( MYKI_BR_ContextData_t *pData );
    
//  test0nn.c

int     test001( MYKI_BR_ContextData_t *pData );
int     test002( MYKI_BR_ContextData_t *pData );
int     test003( MYKI_BR_ContextData_t *pData );

//  test1nn.c

int     test101( MYKI_BR_ContextData_t *pData );
int     test102( MYKI_BR_ContextData_t *pData );
int     test103( MYKI_BR_ContextData_t *pData );
int     test104( MYKI_BR_ContextData_t *pData );
int     test105( MYKI_BR_ContextData_t *pData );

//  test2nn.c

int     test201( MYKI_BR_ContextData_t *pData );
int     test202( MYKI_BR_ContextData_t *pData );
int     test203( MYKI_BR_ContextData_t *pData );
int     test204( MYKI_BR_ContextData_t *pData );
int     test205( MYKI_BR_ContextData_t *pData );
int     test206( MYKI_BR_ContextData_t *pData );
int     test207( MYKI_BR_ContextData_t *pData );
int     test208( MYKI_BR_ContextData_t *pData );
int     test209( MYKI_BR_ContextData_t *pData );
int     test210( MYKI_BR_ContextData_t *pData );
int     test211( MYKI_BR_ContextData_t *pData );
int     test212( MYKI_BR_ContextData_t *pData );

//  test_SEQ_OSC_Pin.c

int test_SEQ_OSC_ValidatePin_001( MYKI_BR_ContextData_t *pData );
int test_SEQ_OSC_ValidatePin_002( MYKI_BR_ContextData_t *pData );
int test_SEQ_OSC_UpdatePin_001( MYKI_BR_ContextData_t *pData );

//  test_SEQ_AddValue.c
int test_SEQ_AddValue_001( MYKI_BR_ContextData_t *pData );

//  test_SEQ_AddValueReversal.c
int test_SEQ_AddValueReversal_001( MYKI_BR_ContextData_t *pData );

#endif  // TEST_COMMON_H
