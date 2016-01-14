//=============================================================================
//
//  Unit test harness for Myki Business Rules
//
//  Tests are named as follows :
//      test_BR_XXX_nnn     Single Business Rule tests (XXX = rule name, nnn = test number)
//      test0nn             Entry-Only tests (nn = test number)
//      test1nn             Exit-Only tests (nn = test number)
//      test2nn             Operator Card tests (nn = test number)
//
//  All test functions should return :
//      TRUE    The test passed
//      FALSE   The test failed
//
//=============================================================================

//-----------------------------------------------------------------------------
//  Includes
//-----------------------------------------------------------------------------

#include <stdio.h>
#include <time.h>
#include <memory.h>

#include <cs.h>
#include <myki_cardservices.h>

#include <myki_br.h>        // For testing sequences
#include <myki_br_rules.h>  // For testing individual rules
#include <myki_br_context_data.h>

#include "test_common.h"

//-----------------------------------------------------------------------------
//  External variables - these should be avoided.
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//  Constants / Macros
//-----------------------------------------------------------------------------

    /*
    **      NOTE:   To run particular set of unit-tests, set MASTER_SWITCH to
    **              FALSE and set particular UT_BR_LLSC_XXX to TRUE.
    **
    **              PLEASE! restore to default setting (ie. run all unit-tests)
    **              prior checking in.
    */
#define MASTER_SWITCH               TRUE

#define UT_BR_LLSC_0_A_BYPASS       MASTER_SWITCH
#define UT_BR_LLSC_0_A_EXECUTE      MASTER_SWITCH
#define UT_BR_LLSC_0_B_BYPASS       MASTER_SWITCH
#define UT_BR_LLSC_0_B_EXECUTE      MASTER_SWITCH
#define UT_BR_LLSC_0_1_BYPASS       MASTER_SWITCH
#define UT_BR_LLSC_0_1_EXECUTE      MASTER_SWITCH

#define UT_BR_LLSC_1_1_BYPASS       MASTER_SWITCH
#define UT_BR_LLSC_1_1_EXECUTE      MASTER_SWITCH
#define UT_BR_LLSC_1_2_BYPASS       MASTER_SWITCH
#define UT_BR_LLSC_1_2_EXECUTE      MASTER_SWITCH
#define UT_BR_LLSC_1_3_BYPASS       MASTER_SWITCH
#define UT_BR_LLSC_1_3_EXECUTE      MASTER_SWITCH
#define UT_BR_LLSC_1_5_BYPASS       MASTER_SWITCH
#define UT_BR_LLSC_1_5_EXECUTE      MASTER_SWITCH
#define UT_BR_LLSC_1_6_BYPASS       MASTER_SWITCH
#define UT_BR_LLSC_1_6_EXECUTE      MASTER_SWITCH
#define UT_BR_LLSC_1_7_BYPASS       MASTER_SWITCH
#define UT_BR_LLSC_1_7_EXECUTE      MASTER_SWITCH
#define UT_BR_LLSC_1_8_BYPASS       MASTER_SWITCH
#define UT_BR_LLSC_1_8_EXECUTE      MASTER_SWITCH
#define UT_BR_LLSC_1_9_BYPASS       MASTER_SWITCH
#define UT_BR_LLSC_1_9_EXECUTE      MASTER_SWITCH
#define UT_BR_LLSC_1_10_BYPASS      MASTER_SWITCH
#define UT_BR_LLSC_1_10_EXECUTE     MASTER_SWITCH

#define UT_BR_LLSC_2_2_BYPASS       MASTER_SWITCH
#define UT_BR_LLSC_2_2_EXECUTE      MASTER_SWITCH
#define UT_BR_LLSC_2_5_BYPASS       MASTER_SWITCH
#define UT_BR_LLSC_2_5_EXECUTE      MASTER_SWITCH
#define UT_BR_LLSC_2_6_BYPASS       MASTER_SWITCH
#define UT_BR_LLSC_2_6_EXECUTE      MASTER_SWITCH
#define UT_BR_LLSC_2_7_BYPASS       MASTER_SWITCH
#define UT_BR_LLSC_2_7_EXECUTE      MASTER_SWITCH
#define UT_BR_LLSC_2_11_BYPASS      MASTER_SWITCH
#define UT_BR_LLSC_2_11_EXECUTE     MASTER_SWITCH
#define UT_BR_LLSC_2_13_BYPASS      MASTER_SWITCH
#define UT_BR_LLSC_2_13_EXECUTE     MASTER_SWITCH
#define UT_BR_LLSC_2_14_BYPASS      MASTER_SWITCH
#define UT_BR_LLSC_2_14_EXECUTE     MASTER_SWITCH

#define UT_BR_LLSC_4_1_BYPASS       MASTER_SWITCH
#define UT_BR_LLSC_4_1_EXECUTE      MASTER_SWITCH
#define UT_BR_LLSC_4_11_BYPASS      MASTER_SWITCH
#define UT_BR_LLSC_4_11_EXECUTE     MASTER_SWITCH
#define UT_BR_LLSC_4_14_BYPASS      MASTER_SWITCH
#define UT_BR_LLSC_4_14_EXECUTE     MASTER_SWITCH
#define UT_BR_LLSC_4_15_BYPASS      MASTER_SWITCH
#define UT_BR_LLSC_4_15_EXECUTE     MASTER_SWITCH
#define UT_BR_LLSC_4_17_BYPASS      MASTER_SWITCH
#define UT_BR_LLSC_4_17_EXECUTE     MASTER_SWITCH
#define UT_BR_LLSC_4_19_BYPASS      MASTER_SWITCH
#define UT_BR_LLSC_4_19_EXECUTE     MASTER_SWITCH

#define UT_BR_LLSC_5_2_BYPASS       MASTER_SWITCH
#define UT_BR_LLSC_5_2_EXECUTE      MASTER_SWITCH
#define UT_BR_LLSC_5_3_BYPASS       MASTER_SWITCH
#define UT_BR_LLSC_5_3_EXECUTE      MASTER_SWITCH
#define UT_BR_LLSC_5_9_BYPASS       MASTER_SWITCH
#define UT_BR_LLSC_5_9_EXECUTE      MASTER_SWITCH

#define UT_BR_LLSC_6_1_BYPASS       MASTER_SWITCH
#define UT_BR_LLSC_6_1_EXECUTE      MASTER_SWITCH
#define UT_BR_LLSC_6_3_BYPASS       MASTER_SWITCH
#define UT_BR_LLSC_6_3_EXECUTE      MASTER_SWITCH
#define UT_BR_LLSC_6_10_BYPASS      MASTER_SWITCH
#define UT_BR_LLSC_6_10_EXECUTE     MASTER_SWITCH
#define UT_BR_LLSC_6_18_BYPASS      MASTER_SWITCH
#define UT_BR_LLSC_6_18_EXECUTE     MASTER_SWITCH
#define UT_BR_LLSC_6_19_BYPASS      MASTER_SWITCH
#define UT_BR_LLSC_6_19_EXECUTE     MASTER_SWITCH
#define UT_BR_LLSC_6_20_BYPASS      MASTER_SWITCH
#define UT_BR_LLSC_6_20_EXECUTE     MASTER_SWITCH
#define UT_BR_LLSC_6_21_BYPASS      MASTER_SWITCH
#define UT_BR_LLSC_6_21_EXECUTE     MASTER_SWITCH
#define UT_BR_LLSC_6_22_BYPASS      MASTER_SWITCH
#define UT_BR_LLSC_6_22_EXECUTE     MASTER_SWITCH
#define UT_BR_LLSC_6_23_BYPASS      MASTER_SWITCH
#define UT_BR_LLSC_6_23_EXECUTE     MASTER_SWITCH

#define UT_BR_LLSC_7_4_BYPASS       MASTER_SWITCH
#define UT_BR_LLSC_7_4_EXECUTE      MASTER_SWITCH
#define UT_BR_LLSC_7_5_BYPASS       MASTER_SWITCH
#define UT_BR_LLSC_7_5_EXECUTE      MASTER_SWITCH

#define UT_BR_LLSC_9_1_BYPASS       MASTER_SWITCH
#define UT_BR_LLSC_9_1_EXECUTE      MASTER_SWITCH

#define UT_BR_LLSC_10_1_BYPASS      MASTER_SWITCH
#define UT_BR_LLSC_10_1_EXECUTE     MASTER_SWITCH
#define UT_BR_LLSC_10_3_BYPASS      MASTER_SWITCH
#define UT_BR_LLSC_10_3_EXECUTE     MASTER_SWITCH
#define UT_BR_LLSC_10_4_BYPASS      MASTER_SWITCH
#define UT_BR_LLSC_10_4_EXECUTE     MASTER_SWITCH
#define UT_BR_LLSC_10_6_BYPASS      MASTER_SWITCH
#define UT_BR_LLSC_10_6_EXECUTE     MASTER_SWITCH

#define UT_BR_LLSC_99_1_BYPASS      MASTER_SWITCH
#define UT_BR_LLSC_99_1_EXECUTE     MASTER_SWITCH

#define UT_BR_OSC_0_B_BYPASS        MASTER_SWITCH
#define UT_BR_OSC_0_B_EXECUTE       MASTER_SWITCH

#define UT_BR_OSC_1_4_EXECUTE       MASTER_SWITCH
#define UT_BR_OSC_1_4_BYPASS        MASTER_SWITCH

#define UT_BR_VIX_OSC_2_1_BYPASS    MASTER_SWITCH
#define UT_BR_VIX_OSC_2_1_EXECUTE   MASTER_SWITCH
#define UT_BR_VIX_OSC_2_3_BYPASS    MASTER_SWITCH
#define UT_BR_VIX_OSC_2_3_EXECUTE   MASTER_SWITCH
#define UT_BR_VIX_OSC_2_5_BYPASS    MASTER_SWITCH
#define UT_BR_VIX_OSC_2_5_EXECUTE   MASTER_SWITCH

#define UT_BR_VIX_OSC_3_1_BYPASS    MASTER_SWITCH
#define UT_BR_VIX_OSC_3_1_EXECUTE   MASTER_SWITCH
#define UT_BR_VIX_OSC_3_2_BYPASS    MASTER_SWITCH
#define UT_BR_VIX_OSC_3_2_EXECUTE   MASTER_SWITCH
#define UT_BR_VIX_OSC_3_3_BYPASS    MASTER_SWITCH
#define UT_BR_VIX_OSC_3_3_EXECUTE   MASTER_SWITCH
#define UT_BR_VIX_OSC_3_4_BYPASS    MASTER_SWITCH
#define UT_BR_VIX_OSC_3_4_EXECUTE   MASTER_SWITCH
#define UT_BR_VIX_OSC_3_5_BYPASS    MASTER_SWITCH
#define UT_BR_VIX_OSC_3_5_EXECUTE   MASTER_SWITCH

#define UT_BR_VIX_LLSC_20_1_BYPASS  MASTER_SWITCH
#define UT_BR_VIX_LLSC_20_1_EXECUTE MASTER_SWITCH
#define UT_BR_VIX_LLSC_20_2_BYPASS  MASTER_SWITCH
#define UT_BR_VIX_LLSC_20_2_EXECUTE MASTER_SWITCH
#define UT_BR_VIX_LLSC_21_1_BYPASS  MASTER_SWITCH
#define UT_BR_VIX_LLSC_21_1_EXECUTE MASTER_SWITCH
#define UT_BR_VIX_LLSC_21_2_BYPASS  MASTER_SWITCH
#define UT_BR_VIX_LLSC_21_2_EXECUTE MASTER_SWITCH

#define UT_SEQ_0XX                  MASTER_SWITCH
#define UT_SEQ_1XX                  MASTER_SWITCH
#define UT_SEQ_2XX                  MASTER_SWITCH
#define UT_SEQ_OSC_Pin              MASTER_SWITCH
#define UT_SEQ_AddValue             MASTER_SWITCH
#define UT_SEQ_AddValueReversal     MASTER_SWITCH

//-----------------------------------------------------------------------------
//  File Local Function Prototypes
//-----------------------------------------------------------------------------

static  int     test_Sanity( MYKI_BR_ContextData_t *pData );

//-----------------------------------------------------------------------------
//  File Local Variables
//-----------------------------------------------------------------------------

static  MYKI_BR_ContextData_t   ContextData;

//
//  List of tests to run
//

static  struct
{
    int     (*func)( MYKI_BR_ContextData_t *pData );
    char    *desc;
} testList[] =
{
    //  Sanity check sequences

    { test_Sanity, "test_Sanity : Sanity check all sequences" },

    //  Individual business rules

    #if     UT_BR_LLSC_0_A_EXECUTE
    { test_BR_LLSC_0_A_001, "test_BR_LLSC_0_A_001 : Execute" },
    #endif
    #if     UT_BR_LLSC_0_A_BYPASS
    { test_BR_LLSC_0_A_002, "test_BR_LLSC_0_A_002 : Bypass - Not Test Mode" },
    #endif

    #if     UT_BR_LLSC_0_B_EXECUTE
    { test_BR_LLSC_0_B_001, "test_BR_LLSC_0_B_001 : Execute" },
    #endif
    #if     UT_BR_LLSC_0_B_BYPASS
//  { test_BR_LLSC_0_B_002, "test_BR_LLSC_0_B_002 : Bypass - MYKI_CAControl.Checksum != 0" },
    { test_BR_LLSC_0_B_003, "test_BR_LLSC_0_B_003 : Bypass - MYKI_CAControl.ExpiryDate < pData->DynamicData.currentBusinessDate" },
    { test_BR_LLSC_0_B_004, "test_BR_LLSC_0_B_004 : Bypass - MYKI_CAControl.Status 1= CARD_STATUS_ACTIVATED" },
//  { test_BR_LLSC_0_B_005, "test_BR_LLSC_0_B_005 : Bypass - MYKI_TAControl.Checksum != 0" },
    { test_BR_LLSC_0_B_006, "test_BR_LLSC_0_B_006 : Bypass - MYKI_TAControl.ExpiryDate < pData->DynamicData.currentBusinessDate" },
    { test_BR_LLSC_0_B_007, "test_BR_LLSC_0_B_007 : Bypass - MYKI_TAControl.Status != APP_STATUS_ACTIVATED etc" },
    #endif

    #if     UT_BR_LLSC_0_1_BYPASS
    { test_BR_LLSC_0_1_001, "test_BR_LLSC_0_1_001 : Bypass - pData->DynamicData.isTransit = FALSE" },
    { test_BR_LLSC_0_1_010, "test_BR_LLSC_0_1_010 : Bypass - TPurse balance < Minimum" },
    { test_BR_LLSC_0_1_011, "test_BR_LLSC_0_1_011 : Bypass - TPurse balance > Maximum" },
    #endif
    #if     UT_BR_LLSC_0_1_EXECUTE
    { test_BR_LLSC_0_1_002, "test_BR_LLSC_0_1_002 : Execute - activated product with EndDateTime < StartDateTime" },
    { test_BR_LLSC_0_1_003, "test_BR_LLSC_0_1_003 : Execute - DAILY/SINGLE TRIP products with EndDateTime > (StartDateTime + 24 Hours)" },
    { test_BR_LLSC_0_1_004, "test_BR_LLSC_0_1_004 : Execute - WEEKLY product with EndDateTime > (StartDateTime + 8 Days)" },
    { test_BR_LLSC_0_1_005, "test_BR_LLSC_0_1_005 : Execute - EPASS product with IntanceCount > 1825 OR EndDateTime > (StartDateTime + InstanceCount + 1 Day)" },
    { test_BR_LLSC_0_1_006, "test_BR_LLSC_0_1_006 : Execute - N-HOUR product with EndDateTime > (StartDateTime + 1 Day)" },
    { test_BR_LLSC_0_1_007, "test_BR_LLSC_0_1_007 : Execute - Issued product of type other than EPASS" },
    { test_BR_LLSC_0_1_008, "test_BR_LLSC_0_1_008 : Execute - Issued EPASS with InstanceCount > 1825 OR EndDateTime < StartDateTime" },
    { test_BR_LLSC_0_1_009, "test_BR_LLSC_0_1_009 : Execute - Invalid provisional product" },
    { test_BR_LLSC_0_1_012, "test_BR_LLSC_0_1_012 : Execute - TPurse balance != LastChangeNewBalance" },
    { test_BR_LLSC_0_1_013, "test_BR_LLSC_0_1_013 : Execute - NextTxSeqNo != LastChangeTxSeqNo" },
    { test_BR_LLSC_0_1_014, "test_BR_LLSC_0_1_014 : Execute - ProductInUse references unused product" },
    #endif

    #if     UT_BR_LLSC_1_1_BYPASS
    { test_BR_LLSC_1_1_001, "test_BR_LLSC_1_1_001 : Bypass - pData->DynamicData.numberOfCardsPresented = 1" },
    #endif
    #if     UT_BR_LLSC_1_1_EXECUTE
    { test_BR_LLSC_1_1_002, "test_BR_LLSC_1_1_002 : Execute, reject multiple cards" },
    #endif

    #if     UT_BR_LLSC_1_2_BYPASS
    { test_BR_LLSC_1_2_001, "test_BR_LLSC_1_2_001 : Bypass - pData->DynamicData.isEarlyBirdTrip = TRUE" },
    #endif
    #if     UT_BR_LLSC_1_2_EXECUTE
    { test_BR_LLSC_1_2_002, "test_BR_LLSC_1_2_002 : Execute" },
    #endif

    #if     UT_BR_LLSC_2_2_BYPASS
    { test_BR_LLSC_2_2_001, "test_BR_LLSC_2_2_001 : Bypass - pre-conditions not met" },
    #endif
    #if     UT_BR_LLSC_2_2_EXECUTE
    { test_BR_LLSC_2_2_002, "test_BR_LLSC_2_2_002 : Execute, T-Purse blocked" },
    #endif

    #if     UT_BR_LLSC_2_5_BYPASS
    { test_BR_LLSC_2_5_001, "test_BR_LLSC_2_5_001 : Bypass - pre-conditions not met" },
    #endif
    #if     UT_BR_LLSC_2_5_EXECUTE
    { test_BR_LLSC_2_5_002, "test_BR_LLSC_2_5_002 : Execute, disable auto-load" },
    { test_BR_LLSC_2_5_003, "test_BR_LLSC_2_5_003 : Execute, enable auto-load" },
    #endif

    #if     UT_BR_LLSC_2_6_BYPASS
    { test_BR_LLSC_2_6_001a, "test_BR_LLSC_2_6_001a : Bypass - Not TPurseLoad/None actionlist" },
    { test_BR_LLSC_2_6_001b, "test_BR_LLSC_2_6_001b : Bypass - ActionSeqNo < 1" },
    { test_BR_LLSC_2_6_001c, "test_BR_LLSC_2_6_001c : Bypass - ActionSeqNo > 15" },
    { test_BR_LLSC_2_6_001d, "test_BR_LLSC_2_6_001d : Bypass - ActionSeqNo mismatched (wrap-around)" },
    { test_BR_LLSC_2_6_001e, "test_BR_LLSC_2_6_001e : Bypass - ActionSeqNo mismatched" },
    { test_BR_LLSC_2_6_001f, "test_BR_LLSC_2_6_001f : Bypass - TPurse blocked" },
    { test_BR_LLSC_2_6_001g, "test_BR_LLSC_2_6_001g : Bypass - Exceed maximum TPurse balance" },
    #endif

    #if     UT_BR_LLSC_2_6_EXECUTE
    { test_BR_LLSC_2_6_002, "test_BR_LLSC_2_6_002 : Execute, Dynamic.TxType = 0" },
    { test_BR_LLSC_2_6_003, "test_BR_LLSC_2_6_003 : Execute, Dynamic.TxType != 0" },
    { test_BR_LLSC_2_6_004, "test_BR_LLSC_2_6_004 : Execute, Dynamic.TxType != 0, Dynamic.TxValue < 0" },
    #endif

    #if     UT_BR_LLSC_2_7_BYPASS
    { test_BR_LLSC_2_7_001, "test_BR_LLSC_2_7_001 : Bypass - pre-conditions not met" },
    #endif
    #if     UT_BR_LLSC_2_7_EXECUTE
    { test_BR_LLSC_2_7_002, "test_BR_LLSC_2_7_002 : Execute, Dynamic.TxType = 0, Dynamic.TxValue = 0" },
    { test_BR_LLSC_2_7_003, "test_BR_LLSC_2_7_003 : Execute, Dynamic.TxType = 1, Dynamic.TxValue = 45" },
    { test_BR_LLSC_2_7_004, "test_BR_LLSC_2_7_004 : Execute, Yearly ePass" },
    #endif

    #if     UT_BR_LLSC_2_11_BYPASS
    { test_BR_LLSC_2_11_001a, "test_BR_LLSC_2_11_001a : Bypass - Not TAppUpdate/Unblock actionlist" },
    { test_BR_LLSC_2_11_001b, "test_BR_LLSC_2_11_001b : Bypass - ActionSeqNo < 1" },
    { test_BR_LLSC_2_11_001c, "test_BR_LLSC_2_11_001c : Bypass - ActionSeqNo > 15" },
    { test_BR_LLSC_2_11_001d, "test_BR_LLSC_2_11_001d : Bypass - ActionSeqNo mismatched (wrap-around)" },
    { test_BR_LLSC_2_11_001e, "test_BR_LLSC_2_11_001e : Bypass - ActionSeqNo mismatched" },
    #endif
    #if     UT_BR_LLSC_2_11_EXECUTE
    { test_BR_LLSC_2_11_002, "test_BR_LLSC_2_11_002 : Execute, Success" },
    #endif

    #if     UT_BR_LLSC_2_13_BYPASS
    { test_BR_LLSC_2_13_001a, "test_BR_LLSC_2_13_001a : Bypass - Not TPurseLoad/Debit actionlist" },
    { test_BR_LLSC_2_13_001b, "test_BR_LLSC_2_13_001b : Bypass - ActionSeqNo < 1" },
    { test_BR_LLSC_2_13_001c, "test_BR_LLSC_2_13_001c : Bypass - ActionSeqNo > 15" },
    { test_BR_LLSC_2_13_001d, "test_BR_LLSC_2_13_001d : Bypass - ActionSeqNo mismatched (wrap-around)" },
    { test_BR_LLSC_2_13_001e, "test_BR_LLSC_2_13_001e : Bypass - ActionSeqNo mismatched" },
    { test_BR_LLSC_2_13_001f, "test_BR_LLSC_2_13_001f : Bypass - TPurse blocked" },
    #endif
    #if     UT_BR_LLSC_2_13_EXECUTE
    { test_BR_LLSC_2_13_002, "test_BR_LLSC_2_13_002 : Execute, Dynamic.TxType = 0, Dynamic.TxValue = 0" },
    { test_BR_LLSC_2_13_003, "test_BR_LLSC_2_13_003 : Execute, Dynamic.TxType = 25, Dynamic.TxValue < Actionlist.Value" },
    { test_BR_LLSC_2_13_004, "test_BR_LLSC_2_13_004 : Execute, Dynamic.TxType != 0 or 25, Dynamic.TxValue < Actionlist.Value" },
    { test_BR_LLSC_2_13_005, "test_BR_LLSC_2_13_005 : Execute, Dynamic.TxType != 0 or 25, Dynamic.TxValue > Actionlist.Value" },
    #endif

    #if     UT_BR_LLSC_2_14_BYPASS
    { test_BR_LLSC_2_14_001, "test_BR_LLSC_2_14_001 : Bypass - Null Data" },
    { test_BR_LLSC_2_14_002, "test_BR_LLSC_2_14_002 : Bypass - No Matching HostList entry" },
    #endif
    #if     UT_BR_LLSC_2_14_EXECUTE
    { test_BR_LLSC_2_14_003, "test_BR_LLSC_2_14_003 : Execute" },
    #endif

    #if     UT_BR_LLSC_4_1_BYPASS
    { test_BR_LLSC_4_1_001, "test_BR_LLSC_4_1_001 : Bypass - Null Pointer" },
    { test_BR_LLSC_4_1_002, "test_BR_LLSC_4_1_002 : Bypass - Product is in use" },
    { test_BR_LLSC_4_1_003, "test_BR_LLSC_4_1_003 : Bypass - No Station Exit Fee" },
    #endif
    #if     UT_BR_LLSC_4_1_EXECUTE
    { test_BR_LLSC_4_1_004, "test_BR_LLSC_4_1_004 : Execute" },
    #endif

    #if     UT_BR_LLSC_4_11_BYPASS
    { test_BR_LLSC_4_11_001, "test_BR_LLSC_4_11 : Bypass" },
    #endif
    #if     UT_BR_LLSC_4_11_EXECUTE
    { test_BR_LLSC_4_11_002, "test_BR_LLSC_4_11 : Execute" },
    #endif

    #if     UT_BR_LLSC_4_14_BYPASS
    { test_BR_LLSC_4_14_001, "test_BR_LLSC_4_14 : Bypass" },
    #endif
    #if     UT_BR_LLSC_4_14_EXECUTE
    { test_BR_LLSC_4_14_002, "test_BR_LLSC_4_14 : Execute" },
    #endif

    #if     UT_BR_LLSC_4_15_BYPASS
    { test_BR_LLSC_4_15_001, "test_BR_LLSC_4_15 : Bypass" },
    #endif
    #if     UT_BR_LLSC_4_15_EXECUTE
    { test_BR_LLSC_4_15_002, "test_BR_LLSC_4_15 : Execute" },
    #endif

    #if     UT_BR_LLSC_4_17_BYPASS
    { test_BR_LLSC_4_17_001, "test_BR_LLSC_4_17_001 : Bypass - No Product in use" },
    { test_BR_LLSC_4_17_002, "test_BR_LLSC_4_17_002 : Bypass - Route Change over" },
    { test_BR_LLSC_4_17_003, "test_BR_LLSC_4_17_003 : Bypass - Scan on not rail" },
    { test_BR_LLSC_4_17_004, "test_BR_LLSC_4_17_004 : Bypass - I am not rail" },
    { test_BR_LLSC_4_17_005, "test_BR_LLSC_4_17_005 : Bypass - Same entry point" },
    { test_BR_LLSC_4_17_006, "test_BR_LLSC_4_17_006 : Bypass - Product Expired" },
    { test_BR_LLSC_4_17_007, "test_BR_LLSC_4_17_007 : Bypass - Provisional Capping Option is NOT Option3" },
    #endif
    #if     UT_BR_LLSC_4_17_EXECUTE
    { test_BR_LLSC_4_17_008, "test_BR_LLSC_4_17_008 : Execute" },
    #endif

    #if     UT_BR_LLSC_4_19_BYPASS
    { test_BR_LLSC_4_19_001, "test_BR_LLSC_4_19_001 : Bypass - Determine trip extension" },
    #endif
    #if     UT_BR_LLSC_4_19_EXECUTE
    { test_BR_LLSC_4_19_002a, "test_BR_LLSC_4_19_002a : Execute - Trip extension" },
    { test_BR_LLSC_4_19_002b, "test_BR_LLSC_4_19_002b : Execute - No trip extension" },
    { test_BR_LLSC_4_19_002c, "test_BR_LLSC_4_19_002c : Execute - Trip extension - exceeded maximum nHourMaximumDuration" },
    { test_BR_LLSC_4_19_002d, "test_BR_LLSC_4_19_002d : Execute - Trip extension - before StartOfBusinessDay" },
    { test_BR_LLSC_4_19_002e, "test_BR_LLSC_4_19_002e : Execute - Trip extension - after nHourEveningCutOff" },
    #endif

    #if     UT_BR_LLSC_5_2_BYPASS
    { test_BR_LLSC_5_2_001, "test_BR_LLSC_5_2_001 : Bypass - pre-conditions not met" },
    #endif
    #if     UT_BR_LLSC_5_2_EXECUTE
    { test_BR_LLSC_5_2_002, "test_BR_LLSC_5_2_002 : Execute - Insufficient funds" },
    { test_BR_LLSC_5_2_003, "test_BR_LLSC_5_2_003 : Execute - Usable active ePass but insufficient funds for surcharge" },
    { test_BR_LLSC_5_2_004, "test_BR_LLSC_5_2_004 : Execute - Usable inactive ePass but unusable active ePass exists" },
    #endif

    #if     UT_BR_LLSC_5_3_BYPASS
    { test_BR_LLSC_5_3_001a, "test_BR_LLSC_5_3_001a : Bypass - No provisional product" },
    { test_BR_LLSC_5_3_001b, "test_BR_LLSC_5_3_001b : Bypass - Different transport mode" },
    { test_BR_LLSC_5_3_001c, "test_BR_LLSC_5_3_001c : Bypass - TRAM/BUS Different EntryPointId" },
    { test_BR_LLSC_5_3_001d, "test_BR_LLSC_5_3_001d : Bypass - Different LineId/RouteId" },
    { test_BR_LLSC_5_3_001e, "test_BR_LLSC_5_3_001d : Bypass - RAIL Different EntryPointId" },
    #endif
    #if     UT_BR_LLSC_5_3_EXECUTE
    { test_BR_LLSC_5_3_002a, "test_BR_LLSC_5_3_002a : Execute - TRAM/BUS same stop" },
    { test_BR_LLSC_5_3_002b, "test_BR_LLSC_5_3_002b : Execute - TRAM/BUS different stop" },
    { test_BR_LLSC_5_3_002c, "test_BR_LLSC_5_3_002c : Execute - RAIL" },
    #endif

    #if     UT_BR_LLSC_5_9_BYPASS
    { test_BR_LLSC_5_9_001, "test_BR_LLSC_5_9_001 : Bypass" },
    #endif
    #if     UT_BR_LLSC_5_9_EXECUTE
    { test_BR_LLSC_5_9_002, "test_BR_LLSC_5_9_002 : Execute" },
    { test_BR_LLSC_5_9_003, "test_BR_LLSC_5_9_003 : Execute - Zone low is city zone" },
    { test_BR_LLSC_5_9_004, "test_BR_LLSC_5_9_004 : Execute - Zone high is city zone" },
    #endif

    #if     UT_BR_LLSC_6_1_BYPASS
    { test_BR_LLSC_6_1_001, "test_BR_LLSC_6_1_001 : Bypass - No Product in use" },
    #endif
    #if     UT_BR_LLSC_6_1_EXECUTE
    { test_BR_LLSC_6_1_002, "test_BR_LLSC_6_1_002 : Execute - OrgZone=DstZone, OrgRoute=DstRoute, OrgStop=DstStop" },
    { test_BR_LLSC_6_1_003, "test_BR_LLSC_6_1_003 : Execute - OrgZone<DstZone, OrgRoute=DstRoute, OrgStop=DstStop" },
    { test_BR_LLSC_6_1_004, "test_BR_LLSC_6_1_004 : Execute - OrgZone>DstZone, OrgRoute=DstRoute, OrgStop>DstStop" },
    { test_BR_LLSC_6_1_005, "test_BR_LLSC_6_1_005 : Execute - OrgZone=DstZone, OrgRoute!=DstRoute" },
    { test_BR_LLSC_6_1_006, "test_BR_LLSC_6_1_006 : Execute - OrgZone=DstZone, OrgRoute=HeadlessRoute" },
    #endif

    #if     UT_BR_LLSC_6_3_BYPASS
    { test_BR_LLSC_6_3_001, "test_BR_LLSC_6_3_001 : Bypass - No Product in use" },
    { test_BR_LLSC_6_3_002, "test_BR_LLSC_6_3_002 : Bypass - Not an ePass" },
    { test_BR_LLSC_6_3_003, "test_BR_LLSC_6_3_003 : Bypass - Allow out of zone travel" },
    { test_BR_LLSC_6_3_004, "test_BR_LLSC_6_3_004 : Bypass - Is within zone" },
    #endif
    #if     UT_BR_LLSC_6_3_EXECUTE
    { test_BR_LLSC_6_3_005, "test_BR_LLSC_6_3_005 : Execute - Reject Card" },
    #endif

    #if     UT_BR_LLSC_6_10_BYPASS
    { test_BR_LLSC_6_10_001, "test_BR_LLSC_6_10_001 : Bypass - Null Pointer" },
    { test_BR_LLSC_6_10_002, "test_BR_LLSC_6_10_002 : Bypass - No Product in use" },
    { test_BR_LLSC_6_10_003, "test_BR_LLSC_6_10_003 : Bypass - Not a provisional product" },
    { test_BR_LLSC_6_10_004, "test_BR_LLSC_6_10_004 : Bypass - No Single Trip Product" },
    #endif
    #if     UT_BR_LLSC_6_10_EXECUTE
    { test_BR_LLSC_6_10_005, "test_BR_LLSC_6_10_005 : Error  - Single Trip Product" },
    #endif

    #if     UT_BR_LLSC_6_18_EXECUTE
    { test_BR_LLSC_6_18_000, "test_BR_LLSC_6_18_000 : Execute" },
    #endif
    #if     UT_BR_LLSC_6_18_BYPASS
    { test_BR_LLSC_6_18_001, "test_BR_LLSC_6_18_001 : Bypass - No Product in use" },
    { test_BR_LLSC_6_18_002, "test_BR_LLSC_6_18_002 : Bypass - Is Not a provisional product" },
    { test_BR_LLSC_6_18_003, "test_BR_LLSC_6_18_003 : Bypass - Scan on not rail" },
    { test_BR_LLSC_6_18_004, "test_BR_LLSC_6_18_004 : Bypass - Not a rail device" },
    { test_BR_LLSC_6_18_005, "test_BR_LLSC_6_18_005 : Bypass - Scan on Station not an early bird station" },
    { test_BR_LLSC_6_18_006, "test_BR_LLSC_6_18_006 : Bypass - Current location not an early bird station" },
    { test_BR_LLSC_6_18_007, "test_BR_LLSC_6_18_007 : Bypass - Scan on time too old" },
    { test_BR_LLSC_6_18_008, "test_BR_LLSC_6_18_008 : Bypass - Not an early bird day" },
    { test_BR_LLSC_6_18_009, "test_BR_LLSC_6_18_009 : Bypass - After Early Bird cutoff" },
    { test_BR_LLSC_6_18_010, "test_BR_LLSC_6_18_010 : Bypass - Before Start of Business Day (CurrentDateTime>EndOfBusinessDay)" },
    { test_BR_LLSC_6_18_011, "test_BR_LLSC_6_18_011 : Bypass - Before Start of Business Day (CurrentDateTime<EndOfBusinessDay)" },
    { test_BR_LLSC_6_18_012, "test_BR_LLSC_6_18_012 : Bypass - Early Bird cut off not configured" },
    #endif

    #if     UT_BR_LLSC_6_19_BYPASS
    { test_BR_LLSC_6_19_001, "test_BR_LLSC_6_19_001 : Bypass - Bad Context" },
    { test_BR_LLSC_6_19_002, "test_BR_LLSC_6_19_002 : Bypass - Device Transport Mode is RAIL" },
    { test_BR_LLSC_6_19_003, "test_BR_LLSC_6_19_003 : Bypass - No product in use" },
    { test_BR_LLSC_6_19_004, "test_BR_LLSC_6_19_004 : Bypass - Last service provider is not RAIL" },
    #endif
    #if     UT_BR_LLSC_6_19_EXECUTE
    { test_BR_LLSC_6_19_005, "test_BR_LLSC_6_19_005 : Execute - Plain" },
    #endif

    #if     UT_BR_LLSC_6_20_BYPASS
    { test_BR_LLSC_6_20_001, "test_BR_LLSC_6_20_001 : Bypass - Bad Context" },
    { test_BR_LLSC_6_20_002, "test_BR_LLSC_6_20_002 : Bypass - Insufficient transport locations" },
    #endif
    #if     UT_BR_LLSC_6_20_EXECUTE
    { test_BR_LLSC_6_20_003, "test_BR_LLSC_6_20_003 : Execute - All Locations in same Thin Zone" },
    { test_BR_LLSC_6_20_004, "test_BR_LLSC_6_20_004 : Execute - All locations of the traversal are border stops along the same border(1)" },
    { test_BR_LLSC_6_20_005, "test_BR_LLSC_6_20_005 : Execute - All locations of the traversal are border stops along the same border(2)" },
    { test_BR_LLSC_6_20_006, "test_BR_LLSC_6_20_006 : Execute - All locations of the traversal are border stops along the same border(3)" },
    { test_BR_LLSC_6_20_007, "test_BR_LLSC_6_20_007 : Execute - Forced ScanOff (flag already set)" },
    { test_BR_LLSC_6_20_008, "test_BR_LLSC_6_20_008 : Execute - Forced ScanOff (scan-on rail, scan-off not rail)" },
    { test_BR_LLSC_6_20_009, "test_BR_LLSC_6_20_009 : Execute - Off Peak" },
    { test_BR_LLSC_6_20_010, "test_BR_LLSC_6_20_010 : Execute - Plain" },
    #endif

    #if     UT_BR_LLSC_6_21_BYPASS
    { test_BR_LLSC_6_21_001, "test_BR_LLSC_6_21_001 : Bypass - Bad Context" },
    { test_BR_LLSC_6_21_002, "test_BR_LLSC_6_21_002 : Bypass - Device Transport Mode is RAIL" },
    { test_BR_LLSC_6_21_003, "test_BR_LLSC_6_21_003 : Bypass - No product in use" },
    { test_BR_LLSC_6_21_004, "test_BR_LLSC_6_21_004 : Bypass - Location data is available" },
    { test_BR_LLSC_6_21_005, "test_BR_LLSC_6_21_005 : Bypass - Product in use expired" },
    { test_BR_LLSC_6_21_006, "test_BR_LLSC_6_21_006 : Bypass - Provisional fare is zero" },
    { test_BR_LLSC_6_21_007, "test_BR_LLSC_6_21_007 : Bypass - Mismatched Entry Point" },
    { test_BR_LLSC_6_21_008, "test_BR_LLSC_6_21_008 : Bypass - Mismatched Provider" },
    #endif
    #if     UT_BR_LLSC_6_21_EXECUTE
    { test_BR_LLSC_6_21_009, "test_BR_LLSC_6_21_009 : Execute - Location data unavailable at scan-off" },
    #endif

    #if     UT_BR_LLSC_6_22_BYPASS
    { test_BR_LLSC_6_22_001, "test_BR_LLSC_6_22_001 : Bypass - Bad Context" },
    { test_BR_LLSC_6_22_003, "test_BR_LLSC_6_22_002 : Bypass - No product in use" },
    { test_BR_LLSC_6_22_002, "test_BR_LLSC_6_22_003 : Bypass - Device Transport Mode is RAIL" },
    { test_BR_LLSC_6_22_004, "test_BR_LLSC_6_22_004 : Bypass - Location data is available" },
    { test_BR_LLSC_6_22_005, "test_BR_LLSC_6_22_005 : Bypass - Mismatched Entry Point" },
    { test_BR_LLSC_6_22_006, "test_BR_LLSC_6_22_006 : Bypass - Mismatched Provider" },
    { test_BR_LLSC_6_22_007, "test_BR_LLSC_6_22_007 : Bypass - Provisional fare is zero" },
    { test_BR_LLSC_6_22_008, "test_BR_LLSC_6_22_008 : Bypass - Forced scan off" },
    { test_BR_LLSC_6_22_009, "test_BR_LLSC_6_22_009 : Bypass - Product expired" },
    #endif
    #if     UT_BR_LLSC_6_22_EXECUTE
    { test_BR_LLSC_6_22_010, "test_BR_LLSC_6_22_010 : Execute" },
    #endif

    #if     UT_BR_LLSC_6_23_BYPASS
    { test_BR_LLSC_6_23_001, "test_BR_LLSC_6_23_001 : Bypass - Bad Context" },
    { test_BR_LLSC_6_23_003, "test_BR_LLSC_6_23_002 : Bypass - No product in use" },
    { test_BR_LLSC_6_23_002, "test_BR_LLSC_6_23_003 : Bypass - Location data is unavailable" },
    { test_BR_LLSC_6_23_004, "test_BR_LLSC_6_23_004 : Bypass - Mismatched Entry Point" },
    { test_BR_LLSC_6_23_005, "test_BR_LLSC_6_23_005 : Bypass - Mismatched Provider" },
    { test_BR_LLSC_6_23_006, "test_BR_LLSC_6_23_006 : Bypass - Provisional fare is zero" },
    { test_BR_LLSC_6_23_007, "test_BR_LLSC_6_23_007 : Bypass - Correct e-passes" },
    { test_BR_LLSC_6_23_008, "test_BR_LLSC_6_23_008 : Bypass - Forced scan off" },
    { test_BR_LLSC_6_23_009, "test_BR_LLSC_6_23_009 : Bypass - Device Transport Mode is RAIL" },
    #endif
    #if     UT_BR_LLSC_6_23_EXECUTE
    { test_BR_LLSC_6_23_010, "test_BR_LLSC_6_23_010 : Execute - no ePass" },
    { test_BR_LLSC_6_23_011, "test_BR_LLSC_6_23_011 : Execute - active ePass exists" },
    { test_BR_LLSC_6_23_012, "test_BR_LLSC_6_23_012 : Execute - inactive ePass covers current zone range" },
    { test_BR_LLSC_6_23_013, "test_BR_LLSC_6_23_013 : Execute - inactive ePass covers current inner zone" },
    { test_BR_LLSC_6_23_014, "test_BR_LLSC_6_23_014 : Execute - inactive ePass covers current outer zone" },
    #endif

    #if     UT_BR_LLSC_7_4_BYPASS
    { test_BR_LLSC_7_4_001, "test_BR_LLSC_7_4_001 : Bypass - No Product in use" },
    { test_BR_LLSC_7_4_002, "test_BR_LLSC_7_4_002 : Bypass - Provisional Capping Option is Option3" },
    { test_BR_LLSC_7_4_003, "test_BR_LLSC_7_4_003 : Bypass - Valid Tariff Scan On Date" },
    { test_BR_LLSC_7_4_004, "test_BR_LLSC_7_4_004 : Bypass - ProductInUse is not a provisional product" },
    { test_BR_LLSC_7_4_005, "test_BR_LLSC_7_4_005 : Bypass - Zero product purchase value" },
    { test_BR_LLSC_7_4_006, "test_BR_LLSC_7_4_006 : Bypass - No Coverage" },
    { test_BR_LLSC_7_4_007, "test_BR_LLSC_7_4_007 : Bypass - Active ePass" },
    { test_BR_LLSC_7_4_008, "test_BR_LLSC_7_4_008 : Bypass - InActive ePass with Future Start" },
    #endif
    #if     UT_BR_LLSC_7_4_EXECUTE
    { test_BR_LLSC_7_4_009, "test_BR_LLSC_7_4_009 : Execute - InActive ePass with Past Start" },
    { test_BR_LLSC_7_4_010, "test_BR_LLSC_7_4_010 : Execute - Covered by two active products" },
    { test_BR_LLSC_7_4_011, "test_BR_LLSC_7_4_011 : Execute - Covered by one active products" },
    { test_BR_LLSC_7_4_012, "test_BR_LLSC_7_4_012 : Execute - Covered by one active products and one inactive Epass" },
    #endif
    #if     UT_BR_LLSC_7_4_BYPASS
    { test_BR_LLSC_7_4_013, "test_BR_LLSC_7_4_013 : Bypass - Covered by one active products and one inactive Epass with active ePass" },
    #endif

    #if     UT_BR_LLSC_7_5_BYPASS
    { test_BR_LLSC_7_5_001, "test_BR_LLSC_7_5_001 : Bypass - No Product in use" },
    { test_BR_LLSC_7_5_002, "test_BR_LLSC_7_5_002 : Bypass - Provisional Capping Option is Option3" },
//    { test_BR_LLSC_7_5_003, "test_BR_LLSC_7_5_003 : Bypass - Valid Tariff Scan On Date" },
    { test_BR_LLSC_7_5_004, "test_BR_LLSC_7_5_004 : Bypass - ProductInUse is not a provisional product" },
    { test_BR_LLSC_7_5_005, "test_BR_LLSC_7_5_005 : Bypass - Zero product purchase value" },
    #endif
    #if     UT_BR_LLSC_7_5_EXECUTE
    { test_BR_LLSC_7_5_006, "test_BR_LLSC_7_5_006 : Execute - No Coverage" },
    #endif
    #if     UT_BR_LLSC_7_5_BYPASS
    { test_BR_LLSC_7_5_007, "test_BR_LLSC_7_5_007 : Bypass - Active ePass" },
    #endif
    #if     UT_BR_LLSC_7_5_EXECUTE
    { test_BR_LLSC_7_5_008, "test_BR_LLSC_7_5_008 : Execute - InActive ePass with Future Start" },
    #endif
    #if     UT_BR_LLSC_7_5_BYPASS
    { test_BR_LLSC_7_5_009, "test_BR_LLSC_7_5_009 : Bypass - InActive ePass with Past Start" },
    { test_BR_LLSC_7_5_010, "test_BR_LLSC_7_5_010 : Bypass - Covered by two active products" },
    { test_BR_LLSC_7_5_011, "test_BR_LLSC_7_5_011 : Bypass - Covered by one active products" },
    { test_BR_LLSC_7_5_012, "test_BR_LLSC_7_5_012 : Bypass - Covered by one active products and one inactive Epass" },
    #endif
    #if     UT_BR_LLSC_7_5_EXECUTE
    { test_BR_LLSC_7_5_013, "test_BR_LLSC_7_5_013 : Execute - Covered by one active products and one inactive Epass with active ePass" },
    { test_BR_LLSC_7_5_014, "test_BR_LLSC_7_5_014 : Execute - Partially covered and provisional product is a border product" },
    #endif

    #if     UT_BR_LLSC_9_1_EXECUTE
    { test_BR_LLSC_9_1_001, "test_BR_LLSC_9_1_001 : Execute - One covered daily product" },
    { test_BR_LLSC_9_1_002, "test_BR_LLSC_9_1_002 : Execute - One covered border nhour product" },
    { test_BR_LLSC_9_1_003, "test_BR_LLSC_9_1_003 : Execute - One covered non-border nhour product. Expire" },
    { test_BR_LLSC_9_1_004, "test_BR_LLSC_9_1_004 : Execute - One covered non-border nhour product" },
    { test_BR_LLSC_9_1_005, "test_BR_LLSC_9_1_005 : Execute - Single Product covered by Daily Product" },
    { test_BR_LLSC_9_1_006, "test_BR_LLSC_9_1_006 : Execute - Single Product covered by Weekly Product" },
    { test_BR_LLSC_9_1_007, "test_BR_LLSC_9_1_007 : Execute - Single Product covered by an NOT activated ePass Product" },
    { test_BR_LLSC_9_1_008, "test_BR_LLSC_9_1_008 : Execute - Single Product covered by an activated ePass Product" },
    { test_BR_LLSC_9_1_009, "test_BR_LLSC_9_1_009 : Execute - Covered Product replaces In Use Product" },
    #endif

    #if     UT_BR_LLSC_1_3_BYPASS
    { test_BR_LLSC_1_3_001, "test_BR_LLSC_1_3_001 : Bypass - Pre-conditions not met" },
    #endif
    #if     UT_BR_LLSC_1_3_EXECUTE
    { test_BR_LLSC_1_3_002, "test_BR_LLSC_1_3_002 : Execute - Single mergeable daily product, entirely within product-in-use zone range" },
    { test_BR_LLSC_1_3_003, "test_BR_LLSC_1_3_003 : Execute - Single mergeable weekly product, entirely within product-in-use zone range" },
    { test_BR_LLSC_1_3_004, "test_BR_LLSC_1_3_004 : Execute - Multiple mergeable daily products, some contiguous, some not" },
    { test_BR_LLSC_1_3_005, "test_BR_LLSC_1_3_005 : Execute - Multiple mergeable weekly products, some contiguous, some not" },
    #endif

    #if     UT_BR_LLSC_1_5_BYPASS
    { test_BR_LLSC_1_5_001, "test_BR_LLSC_1_5_001 : Bypass - pre-conditions not met" },
    #endif
    #if     UT_BR_LLSC_1_5_EXECUTE
    { test_BR_LLSC_1_5_002, "test_BR_LLSC_1_5_002 : Execute, ProductInUse N-HOURS upgraded to DAILY" },
    { test_BR_LLSC_1_5_003, "test_BR_LLSC_1_5_003 : Execute, ProductInUse SINGLE TRIP upgraded to DAILY" },
    { test_BR_LLSC_1_5_004, "test_BR_LLSC_1_5_004 : Execute, No ProductInUse, DAILY product sold" },
    { test_BR_LLSC_1_5_005, "test_BR_LLSC_1_5_005 : Execute, ProductInUse N-HOURS not same zone(s), DAILY product sold" },
    { test_BR_LLSC_1_5_006, "test_BR_LLSC_1_5_006 : Execute, ProductInUse N-HOURS is crossed border, DAILY product sold" },
    { test_BR_LLSC_1_5_007, "test_BR_LLSC_1_5_007 : Execute, ProductInUse N-HOURS is valid, DAILY product sold" },
    #endif

    #if     UT_BR_LLSC_1_6_BYPASS
    { test_BR_LLSC_1_6_001, "test_BR_LLSC_1_6_001 : Bypass - pre-conditions not met" },
    #endif
    #if     UT_BR_LLSC_1_6_EXECUTE
    { test_BR_LLSC_1_6_002, "test_BR_LLSC_1_6_002 : Execute, ProductInUse N-HOURS upgraded to DAILY" },
    { test_BR_LLSC_1_6_003, "test_BR_LLSC_1_6_003 : Execute, ProductInUse SINGLE TRIP upgraded to DAILY" },
    { test_BR_LLSC_1_6_004, "test_BR_LLSC_1_6_004 : Execute, ProductInUse N-HOURS upgraded to ENTITLEMENT" },
    { test_BR_LLSC_1_6_005, "test_BR_LLSC_1_6_005 : Execute, ProductInUse SINGLE TRIP upgraded to ENTITLEMENT" },
    { test_BR_LLSC_1_6_006, "test_BR_LLSC_1_6_006 : Execute, No ProductInUse, DAILY product sold" },
    { test_BR_LLSC_1_6_007, "test_BR_LLSC_1_6_007 : Execute, ProductInUse N-HOURS is crossed border, DAILY product sold" },
    { test_BR_LLSC_1_6_008, "test_BR_LLSC_1_6_008 : Execute, ProductInUse N-HOURS is valid, DAILY product sold" },
    { test_BR_LLSC_1_6_009, "test_BR_LLSC_1_6_009 : Execute, No ProductInUse, ENTITLEMENET product sold" },
    #endif

    #if     UT_BR_LLSC_1_7_BYPASS
    { test_BR_LLSC_1_7_001, "test_BR_LLSC_1_7_001 : Bypass - pre-conditions not met" },
    #endif
    #if     UT_BR_LLSC_1_7_EXECUTE
    { test_BR_LLSC_1_7_002, "test_BR_LLSC_1_7_002 : Execute, ProductInUse N-HOURS upgraded to WEEKLY" },
    { test_BR_LLSC_1_7_003, "test_BR_LLSC_1_7_003 : Execute, ProductInUse DAILY upgraded to WEEKLY" },
    { test_BR_LLSC_1_7_004, "test_BR_LLSC_1_7_004 : Execute, ProductInUse SINGLE TRIP upgraded to WEEKLY" },
    { test_BR_LLSC_1_7_005, "test_BR_LLSC_1_7_005 : Execute, No ProductInUse, WEEKLY product sold" },
    { test_BR_LLSC_1_7_006, "test_BR_LLSC_1_7_006 : Execute, ProductInUse N-HOURS not same zone(s), WEEKLY product sold" },
    { test_BR_LLSC_1_7_007, "test_BR_LLSC_1_7_007 : Execute, ProductInUse SINGLE TRIP not within zones, WEEKLY product sold" },
    { test_BR_LLSC_1_7_008, "test_BR_LLSC_1_7_008 : Execute, ProductInUse DAILY is valid, WEEKLY product sold" },
    #endif

    #if     UT_BR_LLSC_1_8_BYPASS
    { test_BR_LLSC_1_8_001, "test_BR_LLSC_1_8_001 : Bypass - pre-conditions not met" },
    #endif
    #if     UT_BR_LLSC_1_8_EXECUTE
    { test_BR_LLSC_1_8_002, "test_BR_LLSC_1_8_002 : Execute, ProductInUse N-HOURS upgraded to WEEKLY" },
    { test_BR_LLSC_1_8_003, "test_BR_LLSC_1_8_003 : Execute, ProductInUse DAILY upgraded to WEEKLY" },
    { test_BR_LLSC_1_8_004, "test_BR_LLSC_1_8_004 : Execute, ProductInUse SINGLE TRIP upgraded to WEEKLY" },
    { test_BR_LLSC_1_8_005, "test_BR_LLSC_1_8_005 : Execute, No ProductInUse, WEEKLY product sold" },
    { test_BR_LLSC_1_8_006, "test_BR_LLSC_1_8_006 : Execute, ProductInUse N-HOURS not same zone(s), WEEKLY product sold" },
    { test_BR_LLSC_1_8_007, "test_BR_LLSC_1_8_007 : Execute, ProductInUse SINGLE TRIP not within zones, WEEKLY product sold" },
    { test_BR_LLSC_1_8_008, "test_BR_LLSC_1_8_008 : Execute, ProductInUse DAILY is valid, WEEKLY product sold" },
    #endif

    #if     UT_BR_LLSC_1_9_BYPASS
    { test_BR_LLSC_1_9_001, "test_BR_LLSC_1_9_001 : Bypass - pre-conditions not met" },
    #endif
    #if     UT_BR_LLSC_1_9_EXECUTE
    { test_BR_LLSC_1_9_002, "test_BR_LLSC_1_9_002 : Execute, ProductInUse N-HOURS upgraded to WEEKLY" },
    { test_BR_LLSC_1_9_003, "test_BR_LLSC_1_9_003 : Execute, ProductInUse DAILY upgraded to WEEKLY" },
    { test_BR_LLSC_1_9_004, "test_BR_LLSC_1_9_004 : Execute, ProductInUse SINGLE TRIP upgraded to WEEKLY" },
    { test_BR_LLSC_1_9_005, "test_BR_LLSC_1_9_005 : Execute, No ProductInUse, WEEKLY product sold" },
    { test_BR_LLSC_1_9_006, "test_BR_LLSC_1_9_006 : Execute, ProductInUse N-HOURS not same zone(s), WEEKLY product sold" },
    { test_BR_LLSC_1_9_007, "test_BR_LLSC_1_9_007 : Execute, ProductInUse SINGLE TRIP not within zones, WEEKLY product sold" },
    { test_BR_LLSC_1_9_008, "test_BR_LLSC_1_9_008 : Execute, ProductInUse DAILY is valid, WEEKLY product sold" },
    #endif

    #if     UT_BR_LLSC_1_10_BYPASS
    { test_BR_LLSC_1_10_001,"test_BR_LLSC_1_10_001: Bypass - MIKI_TACapping.Daily/Weekly not expired" },
    #endif
    #if     UT_BR_LLSC_1_10_EXECUTE
    { test_BR_LLSC_1_10_002,"test_BR_LLSC_1_10_002: Execute, reset MIKI_TACapping.Daily" },
    { test_BR_LLSC_1_10_003,"test_BR_LLSC_1_10_003: Execute, reset MIKI_TACapping.Weekly" },
    { test_BR_LLSC_1_10_004,"test_BR_LLSC_1_10_004: Execute, reset MIKI_TACapping.Daily and MIKI_TACapping.Weekly" },
    { test_BR_LLSC_1_10_005,"test_BR_LLSC_1_10_005: Execute, reset MIKI_TACapping.Weekly.Expiry to next Monday" },
    #endif

    #if     UT_BR_LLSC_10_1_BYPASS
    { test_BR_LLSC_10_1_001, "test_BR_LLSC_10_1_001 : Bypass - Passenger type is not DDA" },
    #endif
    #if     UT_BR_LLSC_10_1_EXECUTE
    { test_BR_LLSC_10_1_002, "test_BR_LLSC_10_1_002 : Execute, Passenger type is DDA" },
    #endif

    #if     UT_BR_LLSC_10_3_BYPASS
    { test_BR_LLSC_10_3_001, "test_BR_LLSC_10_1_001 : Bypass - pre-conditions not met" },
    #endif
    #if     UT_BR_LLSC_10_3_EXECUTE
    { test_BR_LLSC_10_3_002, "test_BR_LLSC_10_3_002 : Execute, neither entry nor exit, N-HOURS sold" },
    { test_BR_LLSC_10_3_003, "test_BR_LLSC_10_3_003 : Execute, Entry only, N-HOURS sold" },
    { test_BR_LLSC_10_3_004, "test_BR_LLSC_10_3_004 : Execute, Exit only, N-HOURS sold" },
    { test_BR_LLSC_10_3_005, "test_BR_LLSC_10_3_005 : Execute, Entry and Exit, N-HOURS sold" },
    #endif

    #if     UT_BR_LLSC_10_4_BYPASS
    { test_BR_LLSC_10_4_001, "test_BR_LLSC_10_4_001 : Bypass - pre-conditions not met" },
    #endif
    #if     UT_BR_LLSC_10_4_EXECUTE
    { test_BR_LLSC_10_4_002, "test_BR_LLSC_10_4_002 : Execute, force scan off product in use" },
    { test_BR_LLSC_10_4_003, "test_BR_LLSC_10_4_003 : Execute, force scan off product in use" },
    { test_BR_LLSC_10_4_004, "test_BR_LLSC_10_4_004 : Execute, activate and force scan off epass" },
    { test_BR_LLSC_10_4_005, "test_BR_LLSC_10_4_005 : Execute, force scan off active epass" },
    #endif

    #if     UT_BR_LLSC_10_6_BYPASS
    { test_BR_LLSC_10_6_001, "test_BR_LLSC_10_6_001 : Bypass - pre-conditions not met" },
    #endif
    #if     UT_BR_LLSC_10_6_EXECUTE
    { test_BR_LLSC_10_6_002, "test_BR_LLSC_10_6_002 : Execute, scan off product in use" },
    { test_BR_LLSC_10_6_003, "test_BR_LLSC_10_6_003 : Execute, activate and scan off epass" },
    { test_BR_LLSC_10_6_004, "test_BR_LLSC_10_6_004 : Execute, scan off active epass" },
    #endif

    #if     UT_BR_LLSC_99_1_BYPASS
    { test_BR_LLSC_99_1_001a, "test_BR_LLSC_99_1_001a : Bypass - EndDateTime < StartDateTime" },
    { test_BR_LLSC_99_1_001b, "test_BR_LLSC_99_1_001b : Bypass - EndDateTime > StartDateTime + 24hrs" },
    { test_BR_LLSC_99_1_001c, "test_BR_LLSC_99_1_001c : Bypass - EndDateTime > StartDateTime + 8days" },
    { test_BR_LLSC_99_1_001d, "test_BR_LLSC_99_1_001d : Bypass - EndDateTime - StartDateTime > InstanceCount" },
    { test_BR_LLSC_99_1_001e, "test_BR_LLSC_99_1_001e : Bypass - EndDateTime - StartDateTime < InstanceCount" },
    { test_BR_LLSC_99_1_001f, "test_BR_LLSC_99_1_001f : Bypass - Issued product not EPass" },
    { test_BR_LLSC_99_1_001g, "test_BR_LLSC_99_1_001g : Bypass - Provisional product not NHour" },
    { test_BR_LLSC_99_1_001h, "test_BR_LLSC_99_1_001h : Bypass - Provisional product not in use" },
    { test_BR_LLSC_99_1_001i, "test_BR_LLSC_99_1_001i : Bypass - Daily.Expiry > CurrentBusinessDay + 1" },
    { test_BR_LLSC_99_1_001j, "test_BR_LLSC_99_1_001j : Bypass - Weekly.Expiry > CurrentBusinessDay + 8" },
    { test_BR_LLSC_99_1_001k, "test_BR_LLSC_99_1_001k : Bypass - TPurse balance < Minimum" },
    { test_BR_LLSC_99_1_001l, "test_BR_LLSC_99_1_001l : Bypass - TPurse balance > Maximum" },
    { test_BR_LLSC_99_1_001m, "test_BR_LLSC_99_1_001m : Bypass - Balance != LastChangeNewBalance" },
    { test_BR_LLSC_99_1_001n, "test_BR_LLSC_99_1_001n : Bypass - NextTxSeqNo != LastChangeTxSeqNo + 1" },
    #endif
    #if     UT_BR_LLSC_99_1_EXECUTE
    { test_BR_LLSC_99_1_002, "test_BR_LLSC_99_1_002 : Execute, pre-commit check passed" },
    #endif

    #if     UT_BR_OSC_0_B_EXECUTE
    { test_BR_OSC_0_B_001, "test_BR_OSC_0_B_001 : Execute" },
    #endif
    #if     UT_BR_OSC_0_B_BYPASS
    { test_BR_OSC_0_B_002, "test_BR_OSC_0_B_002 : Bypass - pMYKI_CAControl->ExpiryDate < pData->DynamicData.currentBusinessDate" },
    { test_BR_OSC_0_B_003, "test_BR_OSC_0_B_003 : Bypass - pMYKI_CAControl->Status 1= CARD_CONTROL_STATUS_ACTIVATED" },
    { test_BR_OSC_0_B_004, "test_BR_OSC_0_B_004 : Bypass - pMYKI_TAControl->ExpiryDate < pData->DynamicData.currentBusinessDate" }, 
    { test_BR_OSC_0_B_005, "test_BR_OSC_0_B_005 : Bypass - pMYKI_TAControl->Status != TAPP_CONTROL_STATUS_ACTIVATED" },
    #endif

    #if     UT_BR_OSC_1_4_EXECUTE
    { test_BR_OSC_1_4_001, "test_BR_OSC_1_4_001 : Executed" },
    #endif
    #if     UT_BR_OSC_1_4_BYPASS
    { test_BR_OSC_1_4_002a, "test_BR_OSC_1_4_002a : Bypass - not authorised officer type" },
    { test_BR_OSC_1_4_002b, "test_BR_OSC_1_4_002b : Bypass - not authorised officer profile" },
    { test_BR_OSC_1_4_002c, "test_BR_OSC_1_4_002c : Bypass - not device service provider" },
    #endif

    #if     UT_BR_VIX_OSC_2_1_BYPASS
    { test_BR_VIX_OSC_2_1_001a, "test_BR_VIX_OSC_2_1_001a : Bypass - invalid service provider" },
    { test_BR_VIX_OSC_2_1_001b, "test_BR_VIX_OSC_2_1_001b : Bypass - exceeded PIN retries" },
    { test_BR_VIX_OSC_2_1_001c, "test_BR_VIX_OSC_2_1_001c : Bypass - PIN not entered" },
    { test_BR_VIX_OSC_2_1_001d, "test_BR_VIX_OSC_2_1_001d : Bypass - OApplication not activated" },
    #endif
    #if     UT_BR_VIX_OSC_2_1_EXECUTE
    { test_BR_VIX_OSC_2_1_002a, "test_BR_VIX_OSC_2_1_002 : Executed, invalid PIN" },
    { test_BR_VIX_OSC_2_1_002b, "test_BR_VIX_OSC_2_1_002 : Executed, exceeded PIN retries" },
    { test_BR_VIX_OSC_2_1_003, "test_BR_VIX_OSC_2_1_003 : Executed, valid PIN" },
    #endif

    #if     UT_BR_VIX_OSC_2_3_BYPASS
    { test_BR_VIX_OSC_2_3_001a, "test_BR_VIX_OSC_2_3_001a : Bypass - invalid service provider" },
    { test_BR_VIX_OSC_2_3_001b, "test_BR_VIX_OSC_2_3_001a : Bypass - OApplication blocked" },
    { test_BR_VIX_OSC_2_3_001c, "test_BR_VIX_OSC_2_3_001a : Bypass - OApplication not activated" },
    #endif
    #if     UT_BR_VIX_OSC_2_3_EXECUTE
    { test_BR_VIX_OSC_2_3_002, "test_BR_VIX_OSC_2_3_002 : Executed, no new PIN" },
    { test_BR_VIX_OSC_2_3_003, "test_BR_VIX_OSC_2_3_003 : Executed, unblock OApplication and new PIN entered" },
    #endif

    #if     UT_BR_VIX_OSC_2_5_BYPASS
    { test_BR_VIX_OSC_2_5_001a, "test_BR_VIX_OSC_2_5_001a : Bypass - not Driver type" },
    { test_BR_VIX_OSC_2_5_001b, "test_BR_VIX_OSC_2_5_001b : Bypass - not Driver profile" },
    #endif
    #if     UT_BR_VIX_OSC_2_5_EXECUTE
    { test_BR_VIX_OSC_2_5_002a, "test_BR_VIX_OSC_2_5_002a : Executed, is Driver role" },
    #endif

    #if     UT_BR_VIX_OSC_3_1_BYPASS
    { test_BR_VIX_OSC_3_1_001a, "test_BR_VIX_OSC_3_1_001a : Bypass - shift data buffer not provided" },
    { test_BR_VIX_OSC_3_1_001b, "test_BR_VIX_OSC_3_1_001b : Bypass - shift status not 'Suspended' nor 'Closed'" },
    #endif
    #if     UT_BR_VIX_OSC_3_1_EXECUTE
    { test_BR_VIX_OSC_3_1_002a, "test_BR_VIX_OSC_3_1_002a : Executed, zero active shift records" },
    { test_BR_VIX_OSC_3_1_002b, "test_BR_VIX_OSC_3_1_002b : Executed, non-zero active shift records" },
    #endif

    #if     UT_BR_VIX_OSC_3_2_BYPASS
    { test_BR_VIX_OSC_3_2_001a, "test_BR_VIX_OSC_3_2_001a : Bypass - shift data buffer not provided" },
    { test_BR_VIX_OSC_3_2_001b, "test_BR_VIX_OSC_3_2_001b : Bypass - shift start/end time within limit" },
    { test_BR_VIX_OSC_3_2_001c, "test_BR_VIX_OSC_3_2_001c : Bypass - shift start time within limit and no shift end time" },
    #endif
    #if     UT_BR_VIX_OSC_3_2_EXECUTE
    { test_BR_VIX_OSC_3_2_002a, "test_BR_VIX_OSC_3_2_002a : Executed, unused shift data" },
    { test_BR_VIX_OSC_3_2_002b, "test_BR_VIX_OSC_3_2_002b : Executed, shift start time exceeded limit" },
    { test_BR_VIX_OSC_3_2_002c, "test_BR_VIX_OSC_3_2_002c : Executed, shift end time exceeded limit" },
    #endif

    #if     UT_BR_VIX_OSC_3_3_BYPASS
    { test_BR_VIX_OSC_3_3_001a, "test_BR_VIX_OSC_3_3_001a : Bypass - shift data buffer not provided" },
    { test_BR_VIX_OSC_3_3_001b, "test_BR_VIX_OSC_3_3_001b : Bypass - shift portion not brought forward" },
    #endif
    #if     UT_BR_VIX_OSC_3_3_EXECUTE
    { test_BR_VIX_OSC_3_3_002a, "test_BR_VIX_OSC_3_3_002a : Executed, updated shift data" },
    { test_BR_VIX_OSC_3_3_002b, "test_BR_VIX_OSC_3_3_002b : Executed, added new shift data" },
    { test_BR_VIX_OSC_3_3_002c, "test_BR_VIX_OSC_3_3_002c : Executed, update shift data with random indexing" },
    #endif

    #if     UT_BR_VIX_OSC_3_4_BYPASS
    { test_BR_VIX_OSC_3_4_001a, "test_BR_VIX_OSC_3_4_001a : Bypass - shift data buffer not provided" },
    { test_BR_VIX_OSC_3_4_001b, "test_BR_VIX_OSC_3_4_001b : Bypass - shift portion not out-of-balance" },
    #endif
    #if     UT_BR_VIX_OSC_3_4_EXECUTE
    { test_BR_VIX_OSC_3_4_002a, "test_BR_VIX_OSC_3_4_002a : Executed, shift remains out-of-balance" },
    #endif

    #if     UT_BR_VIX_OSC_3_5_BYPASS
    { test_BR_VIX_OSC_3_5_001a, "test_BR_VIX_OSC_3_5_001a : Bypass - shift data buffer not provided" },
    { test_BR_VIX_OSC_3_5_001b, "test_BR_VIX_OSC_3_5_001b : Bypass - shift not activated" },
    #endif
    #if     UT_BR_VIX_OSC_3_5_EXECUTE
    { test_BR_VIX_OSC_3_5_002a, "test_BR_VIX_OSC_3_5_002a : Executed, shift out-of-balance" },
    { test_BR_VIX_OSC_3_5_002b, "test_BR_VIX_OSC_3_5_002b : Executed, shift already out-of-balance" },
    #endif

    #if     UT_BR_VIX_LLSC_20_1_BYPASS
    { test_BR_VIX_LLSC_20_1_001a, "test_BR_VIX_LLSC_20_1_001a : Bypass - add value service not enabled" },
    { test_BR_VIX_LLSC_20_1_001b, "test_BR_VIX_LLSC_20_1_001b : Bypass - zero top-up amount" },
    { test_BR_VIX_LLSC_20_1_001c, "test_BR_VIX_LLSC_20_1_001c : Bypass - top-up amount < minimum amount" },
    { test_BR_VIX_LLSC_20_1_001d, "test_BR_VIX_LLSC_20_1_001d : Bypass - top-up amount > maximum amount" },
    { test_BR_VIX_LLSC_20_1_001e, "test_BR_VIX_LLSC_20_1_001e : Bypass - exceed tpurse maximum balance" },
    #endif
    #if     UT_BR_VIX_LLSC_20_1_EXECUTE
    { test_BR_VIX_LLSC_20_1_002a, "test_BR_VIX_LLSC_20_1_002a : Executed, tpurse value added" },
    #endif

    #if     UT_BR_VIX_LLSC_20_2_BYPASS
    { test_BR_VIX_LLSC_20_2_001a, "test_BR_VIX_LLSC_20_2_001a : Bypass - no load log record" },
    { test_BR_VIX_LLSC_20_2_001b, "test_BR_VIX_LLSC_20_2_001b : Bypass - invalid reverse sequence number" },
    { test_BR_VIX_LLSC_20_2_001c, "test_BR_VIX_LLSC_20_2_001c : Bypass - invalid reverse amount" },
    { test_BR_VIX_LLSC_20_2_001d, "test_BR_VIX_LLSC_20_2_001d : Bypass - card used after add value transaction" },
    #endif
    #if     UT_BR_VIX_LLSC_20_2_EXECUTE
    { test_BR_VIX_LLSC_20_2_002a, "test_BR_VIX_LLSC_20_2_002a : Executed, tpurse value reversed" },
    #endif

    #if     UT_BR_VIX_LLSC_21_1_BYPASS
    { test_BR_VIX_LLSC_21_1_001a, "test_BR_VIX_LLSC_21_1_001a : Bypass - debit amount less than 0" },
    { test_BR_VIX_LLSC_21_1_001b, "test_BR_VIX_LLSC_21_1_001b : Bypass - debit amount greater than TPurse balance" },
    #endif
    #if     UT_BR_VIX_LLSC_21_1_EXECUTE
    { test_BR_VIX_LLSC_21_1_002a, "test_BR_VIX_LLSC_21_1_002a : Executed, tpurse value debitted" },
    #endif
    
    #if     UT_BR_VIX_LLSC_21_2_BYPASS
    { test_BR_VIX_LLSC_21_2_001a, "test_BR_VIX_LLSC_21_2_001a : Bypass - invalid reversal amount" },
    { test_BR_VIX_LLSC_21_2_001b, "test_BR_VIX_LLSC_21_2_001b : Bypass - would exceed maximum TPurse balance" },
    #endif
    #if     UT_BR_VIX_LLSC_21_2_EXECUTE
    { test_BR_VIX_LLSC_21_2_002a, "test_BR_VIX_LLSC_21_2_002a : Executed, tpurse value refunded" },
    #endif

    //  Sequences
    #if     UT_SEQ_0XX
    { test001, "test001 : Entry Only - No card errors" },
    { test002, "test002 : Entry Only - Various non-fatal card errors" },
    { test003, "test003 : Entry Only - Multiple cards presented" },
    #endif

    #if     UT_SEQ_1XX
//  { test101, "test101 : Exit Only - No card errors" },
//  { test102, "test102 : Exit Only - Various non-fatal card errors" },
    { test103, "test103 : Exit Only - Multiple cards presented" },
    { test104, "test104 : Exit Only - Not Scanned on (No purse balance)" },
    { test105, "test105 : Exit Only - Purse Balance Available" },
    #endif

    #if     UT_SEQ_2XX
//    { test201, "test201 : Perform a trip in Zone 1 using FPDx" },
//    { test202, "test202 : Perform a trip in Zone 1 using FPDx ( insufficent balace for the trip)" },
    { test203, "test203 : Touch on with a card which has -ve balance on it" },
    { test204, "test204 : Touch off with a card which is not previously touched on" },
    { test205, "test205 : Touch on with a card which has already been touched on at the same gate." },
    { test206, "test206 : Touch on  and touch off with in change of mind period at the same station" },
    { test207, "test207 : Touch on touch off in zone 1 with a card which has Zone 1 epass" },
    { test208, "test208 : Forced touch off and touch on with a card which was previously touched on." },
    { test209, "test209 : Touch on touch off with autoload enabled on the card" },
    { test210, "test210 : Touch on with a top up actionlist of $ 10.00" },
    { test211, "test211 : Touch on touch off with card which is blocked" },
    { test212, "test212 : Touch on with a top up actionlist of Epass" },
    #endif

    #if     UT_SEQ_OSC_Pin
    { test_SEQ_OSC_ValidatePin_001, "test_SEQ_OSC_ValidatePin_001 : Valid PIN entered" },
    { test_SEQ_OSC_ValidatePin_002, "test_SEQ_OSC_ValidatePin_002 : Invalid PIN entered" },
    { test_SEQ_OSC_UpdatePin_001, "test_SEQ_OSC_UpdatePin_001 : Update PIN" },
    #endif

    #if     UT_SEQ_AddValue
    { test_SEQ_AddValue_001, "test_SEQ_AddValue_001 : TPurse add value" },
    #endif

    #if     UT_SEQ_AddValueReversal
    { test_SEQ_AddValueReversal_001, "test_SEQ_AddValueReversal_001 : TPurse add value reversal" },
    #endif
};

//=============================================================================
//
//  Sanity check all sequences
//
//=============================================================================

static  int     test_Sanity( MYKI_BR_ContextData_t *pData )
{
//    MYKI_BR_PrintSequenceRules( &SEQ_ProcessCard );

//    MYKI_BR_PrintSequencePaths( &SEQ_ProcessCard, 0 );
//    MYKI_BR_PrintSequencePaths( &SEQ_OSC, 0 );
    MYKI_BR_PrintSequencePathsTest();

    return TRUE;
}

//=============================================================================
//
//  Initialise the global static structures holding the external data required
//  by the BR and normally supplied by the application.
//
//  Also initialise the global static structures holding the virtual card
//  image.
//
//  Should be called at the start of each test function.
//
//  Note :
//      Using myki_br_ToMykiDate() is a hack, as this function is in
//      BR_Common.c and is not designed to be exposed to the outside world.
//      Hence the compiler warning that this function is undefined because
//      BR_Common.h is not part of the BR API and therefore cannot be included
//      in calling code. It's tempting to move the assignment of currentBusinessDate
//      into the library (eg into InitialiseContext() in myki_br.c), but this should
//      not be done as the method of determining the current business date should
//      be decided by the application, not the library. Thus, the call to
//      myki_br_ToMykiDate() below is simply a convenient way to set the business
//      date to the current date in this test-only code. Real code *should not*
//      do this.
//
//=============================================================================

static  void    initDeviceData( MYKI_BR_ContextData_t *pData )
{
    MYKI_CS_OpenCard( MYKI_CS_OPEN_DEFAULT, NULL );

    memset( pData, 0, sizeof( *pData ) );

    pData->StaticData.serviceProviderId         = ProviderId_RAIL;
    pData->DynamicData.entryPointId             = 10500;
    pData->DynamicData.currentDateTime          = CsTime( NULL ).tm_secs;

    MYKI_BR_InitialiseContext( pData );
    MYKI_BR_InitialiseLdtContext( pData );

    pData->DynamicData.currentBusinessDate      = myki_br_ToMykiDate( pData, pData->DynamicData.currentDateTime ) + 1;
    pData->Tariff.endOfBusinessDay              = ( 3 /*AM*/ * 60 );
    pData->Tariff.nHourPeriodMinutes            = ( 2 /*HOURS*/ * 60 );
    pData->Tariff.nHourRoundingPortion          = ( 1 /*HOUR*/ * 60 );

    pData->InternalData.pCardSnapshotPathFormat = NULL;

    MYKI_CD_setDifferentialPriceStructure(NULL);
    MYKI_CD_setDifferentialPriceProvisionalStructure(NULL);
}

//=============================================================================
//
//  Usage
//
//=============================================================================

static  void    Usage( char *progname )
{
    int     i;

    CsVerbose( "Usage: %s [-b] [-B] [-d<n>] [-n] [-N] [-t<n>]", progname );
    CsVerbose( "    -a     Replicate any incorrect ACS behaviour (default)" );
    CsVerbose( "    -A     Do Not Replicate any incorrect ACS behaviour" );
    CsVerbose( "    -b     Break on error (stop testing on first test failure)" );
    CsVerbose( "    -B     Continue on error (run all tests and count failures)" );
    CsVerbose( "    -d<n>  Set Debug Log Level to <n>" );
    CsVerbose( "    -t<n>  First test to run (sets last test to this test unless overridden)" );
    CsVerbose( "    -T<n>  Last test to run" );

    CsVerbose( "    List of tests" );
    for ( i = 0; i < DIMOF( testList ); i++ )
    {
        CsVerbose( "      %3d %s", i, testList[ i ].desc );
    }
}

//=============================================================================
//
//  Run all tests
//
//=============================================================================

int     main( int argc, char **argv )
{
    char                    *progname;
    unsigned int            flag = 1;                       // CsMsgOptset() "TRUE" value
    int                     i;
    int                     BreakOnError = TRUE;
    int                     DebugLevel   = 9;
    int                     TestsRun     = 0;               // Number of tests run
    int                     TestFailures = 0;               // Number of tests which failed
    int                     FirstTest   = 0;                // First test to run
    int                     LastTest    = -1;               // Last test to run (-1 = all)
    int                     AcsCompatibilityMode = TRUE;    // TRUE - Replicate any incorrect ACS behaviour

    //  Set our basename

    if ((progname = strrchr(argv[0], '/')) == NULL && (progname = strrchr(argv[0], '\\')) == NULL)
        progname = argv[0];
    else
        progname++;

    //  Parse command line

    for ( i = 1; i < argc; i++ )
    {
        if ( argv[ i ][ 0 ] == '-' )
        {
            switch ( argv[ i ][ 1 ] )
            {
                case 'a' :
                    AcsCompatibilityMode = TRUE;
                    break;
                case 'A' :
                    AcsCompatibilityMode = FALSE;
                    break;
                case 'b' :
                    BreakOnError = TRUE;
                    break;
                case 'B' :
                    BreakOnError = FALSE;
                    break;
                case 'd' :
                    DebugLevel = atoi( &argv[ i ][ 2 ] );
                    break;
                case 't' :
                    FirstTest = atoi( &argv[ i ][ 2 ] );
                    LastTest = FirstTest;
                    break;
                case 'T' :
                    LastTest = atoi( &argv[ i ][ 2 ] );
                    break;
                default :
                    Usage( progname );
                    return 1;
            }
        }
    }

    //  Set up CS Logger

//  CsMsgOptset( CSMSGOP_PROGNAME    , progname, (int)strlen( progname ) );
    CsMsgOptset( CSMSGOP_DEBUGENABLED, &flag   , sizeof( flag )          );     // Enable debug output
    CsMsgOptset( CSMSGOP_CONSOLE     , &flag   , sizeof( flag )          );     // Output to console
    CsMsgOptset( CSMSGOP_HIGHRES     , &flag   , sizeof( flag )          );     // Show time down to milliseconds

    flag = DebugLevel;
    CsMsgOptset( CSMSGOP_DEBUG       , &flag   , sizeof( flag )          );     // Set debug level (0 - nnn)

    //  Run the tests

    if ( FirstTest < 0 || FirstTest >= DIMOF( testList ) )
        FirstTest = 0;

    if ( LastTest < 0 || LastTest >= DIMOF( testList ) )
        LastTest = DIMOF( testList ) - 1;

    CsVerbose( "First Test  : %d", FirstTest );
    CsVerbose( "Last Test   : %d", LastTest );
    CsVerbose( "Debug Level : %d", DebugLevel );

    for ( i = FirstTest; i <= LastTest; i++ )
    {
        CsVerbose( "(%d) %s : Started", i, testList[ i ].desc );

        TestsRun++;

        //  Initialise the device context data before calling each test
        initDeviceData( &ContextData );
        ContextData.StaticData.AcsCompatibilityMode = AcsCompatibilityMode;     // Set this here because it's a command line option

        if ( testList[ i ].func( &ContextData ) )   // Call the test
        {
            if ( ContextData.InternalData.IsUsageLogUpdated != 0 )
            {
                myki_br_ldt_AddUsageLogEntry( &ContextData );
            }
            if ( ContextData.InternalData.IsLoadLogUpdated != 0 )
            {
                myki_br_ldt_AddLoadLogEntry( &ContextData );
            }
            CsVerbose( "(%d) %s : Passed", i, testList[ i ].desc );
        }
        else
        {
            CsVerbose( "(%d) %s : Failed", i, testList[ i ].desc );
            TestFailures++;
            if ( BreakOnError )
            {
                CsVerbose( "Aborting remaining tests" );
                break;
            }
        }
    }

    CsVerbose( "Total Tests : %d, Failures : %d", TestsRun, TestFailures );
    return TestFailures;
}

