//=============================================================================
//
//  Sequence consists of Step(s)
//  Step consists of Sequence or BusinessRule
//
//  Sequence
//      Defined by Sequence ID
//      Returns SEQ_RESULT_xxx
//  BusinessRule
//      Defined by Rule ID
//      Rule function returns RULE_RESULT_xxx
//      This gets remapped in MYKI_BR_RunSequence() to SEQ_RESULT_xxx
//
//=============================================================================

#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include <cs.h>

#include "myki_br.h"
#include "myki_br_rules.h"
#include "BR_Common.h"

#include "version_defs.h"
//
//  Maximum number of rules that can be processed in a sequence before it's assumed that the sequence has an infinite loop.
//  Should be greater than the maximum number of rules that might legitimately be processed in any sequence.
//

#define MAX_STEPS_BEFORE_LOOP_ASSUMED   100

//=============================================================================
//
//  Used during sanity checking to maintain a list of rules in a sequence.
//
//=============================================================================

static  struct
{
    MYKI_BR_RuleFunc_t      *func;
    char                    *name;

}   foundRuleList[ 500 ];

static int                  currentServiceProviderId        = -1;
static int                  currentEntryPointId             = -1;
static int                  currentStopId                   = -1;
static U8_t                 currentStopProvisionalZoneLow   = 0;
static U8_t                 currentStopProvisionalZoneHigh  = 0;
static U8_t                 currentStopCurrentZone          = 0;
static U8_t                 currentStopCurrentInnerZone     = 0;
static U8_t                 currentStopCurrentOuterZone     = 0;
static int                  currentStopLineId               = -1;

//=============================================================================
//
//  Sequence definitions
//
//=============================================================================

//  Macros to simplify step result action definitions
//  Each macro defines (appropriately according to the macro type) the
//  ActionType, Sequence, Rule, ReturnResult and Desc fields of the ResultAction_t structure

//                              ActionType           , Sequence  , Rule  , ReturnResult , Desc
#define GOTO_SEQUENCE( s )      ACTION_TYPE_GOTO_SEQ , &s        , 0     , 0            , #s
#define GOTO_RULE( r )          ACTION_TYPE_GOTO_RULE, 0         , r     , 0            , #r
#define RETURN_VALUE( v )       ACTION_TYPE_RETURN   , 0         , 0     , v            , #v

//  Wierd difference in the way this macro is handled between VC6 and gcc compilers.
#ifdef WIN32
#define BR_RESULT_PAIR( e, b )                                      { { SEQ_RESULT_EXECUTED, e }, { SEQ_RESULT_BYPASSED, b } }
#else
#define BR_RESULT_PAIR( e1, e2, e3, e4, e5, b1, b2, b3, b4, b5 )    { { SEQ_RESULT_EXECUTED, e1, e2, e3, e4, e5 }, { SEQ_RESULT_BYPASSED, b1, b2, b3, b4, b5 } }
#endif

//  Macros to simplify definition of steps as sequence steps or rule steps

#define STEP_SEQUENCE( id )     STEP_TYPE_SEQ, &id, NULL, #id
#define STEP_RULE( id )         STEP_TYPE_RULE, NULL, id, #id

#define EXECUTE_RULE( r, e, b ) { STEP_RULE( r ), BR_RESULT_PAIR( e, b ) }

//  TODO: Add a RESULT() macro so we can display the result enum as text during MYKI_BR_PrintSequence()

static Sequence_t  SEQ_LLSC_0A =
{
    "SEQ_LLSC_0A - Card Transaction Initialization",
    {
        EXECUTE_RULE( BR_LLSC_0_A,  GOTO_RULE( BR_LLSC_0_B ),               GOTO_RULE( BR_LLSC_0_B ) ),             // Step 1 - mapped to new BR_LLSC_0_A not in document
        EXECUTE_RULE( BR_LLSC_0_B,  GOTO_RULE( BR_LLSC_0_1a ),              RETURN_VALUE( SEQ_RESULT_REJECT ) ),    // Steps 2-9 - mapped to new BR_LLSC_0_B not in document
        EXECUTE_RULE( BR_LLSC_0_1a, GOTO_RULE( BR_LLSC_0_1b ),              RETURN_VALUE( SEQ_RESULT_CONTINUE ) ),  // Step 10
        EXECUTE_RULE( BR_LLSC_0_1b, RETURN_VALUE( SEQ_RESULT_CONTINUE ),    RETURN_VALUE( SEQ_RESULT_REJECT ) ),
        { STEP_TYPE_DONE }
    }
};

static Sequence_t  SEQ_LLSC_1 =
{
    "SEQ_LLSC_1 - Card Detection",
    {
        EXECUTE_RULE( BR_LLSC_1_1, RETURN_VALUE( SEQ_RESULT_MULTIPLE_CARDS ), GOTO_SEQUENCE( SEQ_LLSC_0A ) ),
        {
            STEP_SEQUENCE( SEQ_LLSC_0A ),
            {
                { SEQ_RESULT_CONTINUE   , RETURN_VALUE( SEQ_RESULT_CARD_INITIALISED ) },
                { SEQ_RESULT_REJECT     , RETURN_VALUE( SEQ_RESULT_REJECT ) },
            }
        },
        { STEP_TYPE_DONE }
    }
};

static Sequence_t  SEQ_LLSC_2 =
{
    "SEQ_LLSC_2 - Process Actionlists",
    {
        EXECUTE_RULE( BR_LLSC_2_11, GOTO_RULE( BR_LLSC_2_12 )                                        , GOTO_RULE( BR_LLSC_2_12 ) ),
        EXECUTE_RULE( BR_LLSC_2_12, RETURN_VALUE( SEQ_RESULT_INVALID_TRANSIT_APPLICATION_STATUS )    , GOTO_RULE( BR_LLSC_2_1 ) ),
        EXECUTE_RULE( BR_LLSC_2_1 , RETURN_VALUE( SEQ_RESULT_INVALID_TRANSIT_APPLICATION_STATUS )    , GOTO_RULE( BR_LLSC_2_14 ) ),
        EXECUTE_RULE( BR_LLSC_2_14, RETURN_VALUE( SEQ_RESULT_INVALID_TRANSIT_APPLICATION_STATUS )    , GOTO_RULE( BR_LLSC_2_2 ) ),
        EXECUTE_RULE( BR_LLSC_2_2 , GOTO_RULE( BR_LLSC_2_3 )                                         , GOTO_RULE( BR_LLSC_2_3 ) ),
        EXECUTE_RULE( BR_LLSC_2_3 , GOTO_RULE( BR_LLSC_2_8 )                                         , GOTO_RULE( BR_LLSC_2_8 ) ),
        EXECUTE_RULE( BR_LLSC_2_8 , GOTO_RULE( BR_LLSC_2_9 )                                         , GOTO_RULE( BR_LLSC_2_9 ) ),
        EXECUTE_RULE( BR_LLSC_2_9 , GOTO_RULE( BR_LLSC_2_4 )                                         , GOTO_RULE( BR_LLSC_2_4 ) ),
        EXECUTE_RULE( BR_LLSC_2_4 , GOTO_RULE( BR_LLSC_2_5 )                                         , GOTO_RULE( BR_LLSC_2_5 ) ),
        EXECUTE_RULE( BR_LLSC_2_5 , GOTO_RULE( BR_LLSC_2_13 )                                        , GOTO_RULE( BR_LLSC_2_13 ) ),
        EXECUTE_RULE( BR_LLSC_2_13, GOTO_RULE( BR_LLSC_2_6 )                                         , GOTO_RULE( BR_LLSC_2_6 ) ),
        EXECUTE_RULE( BR_LLSC_2_6 , GOTO_RULE( BR_LLSC_2_7 )                                         , GOTO_RULE( BR_LLSC_2_7 ) ),
        EXECUTE_RULE( BR_LLSC_2_7 , GOTO_RULE( BR_LLSC_2_10 )                                        , GOTO_RULE( BR_LLSC_2_10 ) ),
        EXECUTE_RULE( BR_LLSC_2_10, RETURN_VALUE( SEQ_RESULT_INVALID_TRANSIT_APPLICATION_STATUS )    , RETURN_VALUE( SEQ_RESULT_ACTIONLISTS_PROCESSED ) ),
        { STEP_TYPE_DONE }
    }
};

static Sequence_t  SEQ_LLSC_3 =
{
    "SEQ_LLSC_3 - Process Autoload",
    {
        EXECUTE_RULE( BR_LLSC_3_1, GOTO_RULE( BR_LLSC_3_2 )                         , GOTO_RULE( BR_LLSC_3_2 ) ),
        EXECUTE_RULE( BR_LLSC_3_2, RETURN_VALUE( SEQ_RESULT_AUTOLOAD_PROCESSED )    , RETURN_VALUE( SEQ_RESULT_AUTOLOAD_PROCESSED ) ),
        { STEP_TYPE_DONE }
    }
};

static Sequence_t  SEQ_LLSC_4 =
{
    "SEQ_LLSC_4 - Check for Change of Mind",
    {
        EXECUTE_RULE( BR_LLSC_4_4, RETURN_VALUE( SEQ_RESULT_CHANGE_OF_MIND )       , GOTO_RULE( BR_LLSC_4_5 ) ),
        EXECUTE_RULE( BR_LLSC_4_5, RETURN_VALUE( SEQ_RESULT_SCAN_OFF_REQUIRED )    , GOTO_RULE( BR_LLSC_4_6 ) ),
        EXECUTE_RULE( BR_LLSC_4_6, RETURN_VALUE( SEQ_RESULT_FORCED_SCAN_OFF )      , RETURN_VALUE( SEQ_RESULT_NO_CHANGE_OF_MIND ) ),
        { STEP_TYPE_DONE }
    }
};

static Sequence_t  SEQ_LLSC_5 =
{
    "SEQ_LLSC_5 - Check for Missing Scan-Offs",
    {
        EXECUTE_RULE( BR_LLSC_4_11, GOTO_RULE( BR_LLSC_6_22 )                        , GOTO_RULE( BR_LLSC_6_22 ) ),
        EXECUTE_RULE( BR_LLSC_6_22, RETURN_VALUE( SEQ_RESULT_UNABLE_TO_SCAN_OFF )    , GOTO_RULE( BR_LLSC_4_8 ) ),
        EXECUTE_RULE( BR_LLSC_4_8 , RETURN_VALUE( SEQ_RESULT_SCAN_OFF_REQUIRED )     , GOTO_RULE( BR_LLSC_4_15 ) ),
        EXECUTE_RULE( BR_LLSC_4_15, RETURN_VALUE( SEQ_RESULT_FORCED_SCAN_OFF )       , GOTO_RULE( BR_LLSC_4_9 ) ),
        EXECUTE_RULE( BR_LLSC_4_9 , RETURN_VALUE( SEQ_RESULT_FORCED_SCAN_OFF )       , GOTO_RULE( BR_LLSC_4_10 ) ),
        EXECUTE_RULE( BR_LLSC_4_10, RETURN_VALUE( SEQ_RESULT_FORCED_SCAN_OFF )       , GOTO_RULE( BR_LLSC_4_14 ) ),
        EXECUTE_RULE( BR_LLSC_4_14, RETURN_VALUE( SEQ_RESULT_FORCED_SCAN_OFF )       , RETURN_VALUE( SEQ_RESULT_NO_MISSING_SCAN_OFF ) ),
        { STEP_TYPE_DONE }
    }
};

static Sequence_t  SEQ_LLSC_6 =
{
    "SEQ_LLSC_6 - Check for Missing Scan-Ons",
    {
        EXECUTE_RULE( BR_LLSC_5_1, RETURN_VALUE( SEQ_RESULT_TPURSE_BALANCE_BELOW_MINIMUM ), GOTO_RULE( BR_LLSC_5_2 ) ),
        EXECUTE_RULE( BR_LLSC_5_2, RETURN_VALUE( SEQ_RESULT_TPURSE_BALANCE_BELOW_MINIMUM ), GOTO_RULE( BR_LLSC_4_1 ) ),
        EXECUTE_RULE( BR_LLSC_4_1, RETURN_VALUE( SEQ_RESULT_STATION_EXIT_FEE_CHARGED )    , GOTO_RULE( BR_LLSC_4_2 ) ),
        EXECUTE_RULE( BR_LLSC_4_2, RETURN_VALUE( SEQ_RESULT_MISSING_SCAN_ON )             , RETURN_VALUE( SEQ_RESULT_SCAN_ON_PRESENT ) ),
        { STEP_TYPE_DONE }
    }
};

static Sequence_t  SEQ_LLSC_7 =
{
    "SEQ_LLSC_7 - Scan-On",
    {
        EXECUTE_RULE( BR_LLSC_1_4 , GOTO_RULE( BR_LLSC_1_10 )                                  , GOTO_RULE( BR_LLSC_1_10 ) ),
        EXECUTE_RULE( BR_LLSC_1_10, GOTO_RULE( BR_LLSC_5_1 )                                   , GOTO_RULE( BR_LLSC_5_1 ) ),
        EXECUTE_RULE( BR_LLSC_5_1 , RETURN_VALUE( SEQ_RESULT_TPURSE_BALANCE_BELOW_MINIMUM )    , GOTO_RULE( BR_LLSC_5_2 ) ),
        EXECUTE_RULE( BR_LLSC_5_2 , RETURN_VALUE( SEQ_RESULT_TPURSE_BALANCE_BELOW_MINIMUM )    , GOTO_RULE( BR_LLSC_5_9 ) ),
        EXECUTE_RULE( BR_LLSC_5_9 , GOTO_RULE( BR_LLSC_5_8 )                                   , GOTO_RULE( BR_LLSC_5_4 ) ),
        EXECUTE_RULE( BR_LLSC_5_4 , GOTO_RULE( BR_LLSC_5_8 )                                   , GOTO_RULE( BR_LLSC_5_5 ) ),
        EXECUTE_RULE( BR_LLSC_5_5 , GOTO_RULE( BR_LLSC_5_8 )                                   , GOTO_RULE( BR_LLSC_5_6 ) ),
        EXECUTE_RULE( BR_LLSC_5_6 , GOTO_RULE( BR_LLSC_5_8 )                                   , RETURN_VALUE( SEQ_RESULT_ERROR ) ),
        EXECUTE_RULE( BR_LLSC_5_8 , RETURN_VALUE( SEQ_RESULT_PRODUCT_SCANNED_ON )              , RETURN_VALUE( SEQ_RESULT_PRODUCT_SCANNED_ON ) ),
        { STEP_TYPE_DONE }
    }
};
/*
static Sequence_t  SEQ_LLSC_8 =
{
    "SEQ_LLSC_8 - Rescan-On",
    {
        EXECUTE_RULE( BR_LLSC_5_1, RETURN_VALUE( SEQ_RESULT_TPURSE_BALANCE_BELOW_MINIMUM )    , GOTO_RULE( BR_LLSC_5_2 ) ),
        EXECUTE_RULE( BR_LLSC_5_2, RETURN_VALUE( SEQ_RESULT_TPURSE_BALANCE_BELOW_MINIMUM )    , GOTO_RULE( BR_LLSC_5_7 ) ),
        EXECUTE_RULE( BR_LLSC_5_7, GOTO_RULE( BR_LLSC_5_8 )                                   , RETURN_VALUE( SEQ_RESULT_NO_SCANNED_ON_PRODUCT ) ),
        EXECUTE_RULE( BR_LLSC_5_8, RETURN_VALUE( SEQ_RESULT_RESCAN_ON_COMPLETE )              , RETURN_VALUE( SEQ_RESULT_RESCAN_ON_COMPLETE ) ),
        { STEP_TYPE_DONE }
    }
};
*/
static Sequence_t  SEQ_LLSC_9 =
{
    "SEQ_LLSC_9 - Scan Off",
    {
        EXECUTE_RULE( BR_LLSC_6_18, RETURN_VALUE( SEQ_RESULT_SCAN_OFF_PERFORMED )               , GOTO_RULE( BR_LLSC_6_2 ) ),
        EXECUTE_RULE( BR_LLSC_6_2 , RETURN_VALUE( SEQ_RESULT_SCAN_OFF_PERFORMED )               , GOTO_RULE( BR_LLSC_6_3 ) ),
        EXECUTE_RULE( BR_LLSC_6_3 , RETURN_VALUE( SEQ_RESULT_SCAN_OFF_PERFORMED )               , GOTO_RULE( BR_LLSC_6_10 ) ),
        EXECUTE_RULE( BR_LLSC_6_10, GOTO_RULE( BR_LLSC_6_9 )                                    , GOTO_RULE( BR_LLSC_6_9 ) ),
        EXECUTE_RULE( BR_LLSC_6_9 , RETURN_VALUE( SEQ_RESULT_SCAN_OFF_PERFORMED )               , GOTO_RULE( BR_LLSC_6_7 ) ),
        EXECUTE_RULE( BR_LLSC_6_7 , GOTO_RULE( BR_LLSC_6_11 )                                   , GOTO_RULE( BR_LLSC_6_11 ) ),
        EXECUTE_RULE( BR_LLSC_6_11, GOTO_RULE( BR_LLSC_6_12 )                                   , GOTO_RULE( BR_LLSC_6_12 ) ),
        EXECUTE_RULE( BR_LLSC_6_12, GOTO_RULE( BR_LLSC_6_14 )                                   , GOTO_RULE( BR_LLSC_6_13 ) ),
        EXECUTE_RULE( BR_LLSC_6_13, RETURN_VALUE( SEQ_RESULT_SCAN_OFF_WITH_PARTIAL_COVERAGE )   , GOTO_RULE( BR_LLSC_6_14 ) ),
        EXECUTE_RULE( BR_LLSC_6_14, RETURN_VALUE( SEQ_RESULT_SCAN_OFF_WITH_PARTIAL_COVERAGE )   , GOTO_RULE( BR_LLSC_6_15 ) ),
        EXECUTE_RULE( BR_LLSC_6_15, RETURN_VALUE( SEQ_RESULT_SCAN_OFF_WITH_PARTIAL_COVERAGE )   , RETURN_VALUE( SEQ_RESULT_ERROR ) ),
        { STEP_TYPE_DONE }
    }
};
//duplicate of above since you cant copy static structs
static Sequence_t  SEQ_LLSC_9b =
{
    "SEQ_LLSC_9 - Scan Off",
    {
        EXECUTE_RULE( BR_LLSC_6_18, RETURN_VALUE( SEQ_RESULT_SCAN_OFF_PERFORMED )               , GOTO_RULE( BR_LLSC_6_2 ) ),
        EXECUTE_RULE( BR_LLSC_6_2 , RETURN_VALUE( SEQ_RESULT_SCAN_OFF_PERFORMED )               , GOTO_RULE( BR_LLSC_6_3 ) ),
        EXECUTE_RULE( BR_LLSC_6_3 , RETURN_VALUE( SEQ_RESULT_SCAN_OFF_PERFORMED )               , GOTO_RULE( BR_LLSC_6_10 ) ),
        EXECUTE_RULE( BR_LLSC_6_10, GOTO_RULE( BR_LLSC_6_9 )                                    , GOTO_RULE( BR_LLSC_6_9 ) ),
        EXECUTE_RULE( BR_LLSC_6_9 , RETURN_VALUE( SEQ_RESULT_SCAN_OFF_PERFORMED )               , GOTO_RULE( BR_LLSC_6_7 ) ),
        EXECUTE_RULE( BR_LLSC_6_7 , GOTO_RULE( BR_LLSC_6_11 )                                   , GOTO_RULE( BR_LLSC_6_11 ) ),
        EXECUTE_RULE( BR_LLSC_6_11, GOTO_RULE( BR_LLSC_6_12 )                                   , GOTO_RULE( BR_LLSC_6_12 ) ),
        EXECUTE_RULE( BR_LLSC_6_12, GOTO_RULE( BR_LLSC_6_14 )                                   , GOTO_RULE( BR_LLSC_6_13 ) ),
        EXECUTE_RULE( BR_LLSC_6_13, RETURN_VALUE( SEQ_RESULT_SCAN_OFF_WITH_PARTIAL_COVERAGE )   , GOTO_RULE( BR_LLSC_6_14 ) ),
        EXECUTE_RULE( BR_LLSC_6_14, RETURN_VALUE( SEQ_RESULT_SCAN_OFF_WITH_PARTIAL_COVERAGE )   , GOTO_RULE( BR_LLSC_6_15 ) ),
        EXECUTE_RULE( BR_LLSC_6_15, RETURN_VALUE( SEQ_RESULT_SCAN_OFF_WITH_PARTIAL_COVERAGE )   , RETURN_VALUE( SEQ_RESULT_ERROR ) ),
        { STEP_TYPE_DONE }
    }
};

static Sequence_t  SEQ_LLSC_10 =
{
    "SEQ_LLSC_10 - Forced Scan Off",
    {
        EXECUTE_RULE( BR_LLSC_7_3 , RETURN_VALUE( SEQ_RESULT_FORCED_SCAN_OFF_PENALTY )           , GOTO_RULE( BR_LLSC_7_1 ) ),
        EXECUTE_RULE( BR_LLSC_7_1 , RETURN_VALUE( SEQ_RESULT_FORCED_SCAN_OFF_PENALTY )           , GOTO_RULE( BR_LLSC_7_4 ) ),
        EXECUTE_RULE( BR_LLSC_7_4 , RETURN_VALUE( SEQ_RESULT_SCAN_OFF_WITH_PARTIAL_COVERAGE )    , GOTO_RULE( BR_LLSC_7_5 ) ),
        EXECUTE_RULE( BR_LLSC_7_5 , GOTO_RULE( BR_LLSC_4_19 )                                    , GOTO_RULE( BR_LLSC_7_2 ) ),
        EXECUTE_RULE( BR_LLSC_7_2 , GOTO_RULE( BR_LLSC_4_19 )                                    , GOTO_RULE( BR_LLSC_4_19 ) ),
        EXECUTE_RULE( BR_LLSC_4_19, GOTO_RULE( BR_LLSC_6_7 )                                     , GOTO_RULE( BR_LLSC_6_7 ) ),
        EXECUTE_RULE( BR_LLSC_6_7 , GOTO_RULE( BR_LLSC_6_11 )                                    , GOTO_RULE( BR_LLSC_6_11 ) ),
        EXECUTE_RULE( BR_LLSC_6_11, GOTO_RULE( BR_LLSC_6_14 )                                    , GOTO_RULE( BR_LLSC_6_14 ) ),
        EXECUTE_RULE( BR_LLSC_6_14, RETURN_VALUE( SEQ_RESULT_SCAN_OFF_WITH_PARTIAL_COVERAGE )    , GOTO_RULE( BR_LLSC_6_15 ) ),
        EXECUTE_RULE( BR_LLSC_6_15, RETURN_VALUE( SEQ_RESULT_SCAN_OFF_WITH_PARTIAL_COVERAGE )    , RETURN_VALUE( SEQ_RESULT_ERROR ) ),
        { STEP_TYPE_DONE }
    }
};

static Sequence_t  SEQ_LLSC_11 =
{
    "SEQ_LLSC_11 - Process Capping",
    {
        EXECUTE_RULE( BR_LLSC_1_2 , GOTO_RULE( BR_LLSC_1_6 )                        , RETURN_VALUE( SEQ_RESULT_CAPPING_PROCESSED) ),
        EXECUTE_RULE( BR_LLSC_1_6 , GOTO_RULE( BR_LLSC_1_8 )                        , GOTO_RULE( BR_LLSC_1_5) ),
        EXECUTE_RULE( BR_LLSC_1_5 , GOTO_RULE( BR_LLSC_1_8 )                        , GOTO_RULE( BR_LLSC_1_8) ),
        EXECUTE_RULE( BR_LLSC_1_8 , GOTO_RULE( BR_LLSC_1_10 )                       , GOTO_RULE( BR_LLSC_1_7) ),
        EXECUTE_RULE( BR_LLSC_1_7 , GOTO_RULE( BR_LLSC_1_10 )                       , GOTO_RULE( BR_LLSC_1_9) ),
        EXECUTE_RULE( BR_LLSC_1_9 , GOTO_RULE( BR_LLSC_1_10 )                       , GOTO_RULE( BR_LLSC_1_10) ),
        EXECUTE_RULE( BR_LLSC_1_10, RETURN_VALUE( SEQ_RESULT_CAPPING_PROCESSED )    , RETURN_VALUE( SEQ_RESULT_CAPPING_PROCESSED) ),
        { STEP_TYPE_DONE }
    }
};
static Sequence_t  SEQ_LLSC_11b =
{
    "SEQ_LLSC_11 - Process Capping",
    {
        EXECUTE_RULE( BR_LLSC_1_2 , GOTO_RULE( BR_LLSC_1_6 )                        , RETURN_VALUE( SEQ_RESULT_CAPPING_PROCESSED) ),
        EXECUTE_RULE( BR_LLSC_1_6 , GOTO_RULE( BR_LLSC_1_8 )                        , GOTO_RULE( BR_LLSC_1_5) ),
        EXECUTE_RULE( BR_LLSC_1_5 , GOTO_RULE( BR_LLSC_1_8 )                        , GOTO_RULE( BR_LLSC_1_8) ),
        EXECUTE_RULE( BR_LLSC_1_8 , GOTO_RULE( BR_LLSC_1_10 )                       , GOTO_RULE( BR_LLSC_1_7) ),
        EXECUTE_RULE( BR_LLSC_1_7 , GOTO_RULE( BR_LLSC_1_10 )                       , GOTO_RULE( BR_LLSC_1_9) ),
        EXECUTE_RULE( BR_LLSC_1_9 , GOTO_RULE( BR_LLSC_1_10 )                       , GOTO_RULE( BR_LLSC_1_10) ),
        EXECUTE_RULE( BR_LLSC_1_10, RETURN_VALUE( SEQ_RESULT_CAPPING_PROCESSED )    , RETURN_VALUE( SEQ_RESULT_CAPPING_PROCESSED) ),
        { STEP_TYPE_DONE }
    }
};

static Sequence_t  SEQ_LLSC_12 =
{
    "SEQ_LLSC_12 - Merge Adjacent Products",
    {
        EXECUTE_RULE( BR_LLSC_1_3, RETURN_VALUE( SEQ_RESULT_MERGE_PRODUCTS ), RETURN_VALUE( SEQ_RESULT_MERGE_PRODUCTS ) ),
        { STEP_TYPE_DONE }
    }
};
static Sequence_t  SEQ_LLSC_12b =
{
    "SEQ_LLSC_12 - Merge Adjacent Products",
    {
        EXECUTE_RULE( BR_LLSC_1_3, RETURN_VALUE( SEQ_RESULT_MERGE_PRODUCTS ), RETURN_VALUE( SEQ_RESULT_MERGE_PRODUCTS ) ),
        { STEP_TYPE_DONE }
    }
};

static Sequence_t  SEQ_LLSC_14 =
{
    "SEQ_LLSC_14 - Finalize Scan-Off",
    {
        EXECUTE_RULE( BR_LLSC_9_1, GOTO_RULE( BR_LLSC_6_8 )                         , GOTO_RULE( BR_LLSC_6_8 ) ),
        EXECUTE_RULE( BR_LLSC_6_8, GOTO_RULE( BR_LLSC_1_4 )                         , RETURN_VALUE( SEQ_RESULT_ERROR ) ),
        EXECUTE_RULE( BR_LLSC_1_4, RETURN_VALUE( SEQ_RESULT_SCAN_OFF_FINALIZED )    , RETURN_VALUE( SEQ_RESULT_SCAN_OFF_FINALIZED ) ),
        { STEP_TYPE_DONE }
    }
};
static Sequence_t  SEQ_LLSC_14b =
{
    "SEQ_LLSC_14 - Finalize Scan-Off",
    {
        EXECUTE_RULE( BR_LLSC_9_1, GOTO_RULE( BR_LLSC_6_8 )                         , GOTO_RULE( BR_LLSC_6_8 ) ),
        EXECUTE_RULE( BR_LLSC_6_8, GOTO_RULE( BR_LLSC_1_4 )                         , RETURN_VALUE( SEQ_RESULT_ERROR ) ),
        EXECUTE_RULE( BR_LLSC_1_4, RETURN_VALUE( SEQ_RESULT_SCAN_OFF_FINALIZED )    , RETURN_VALUE( SEQ_RESULT_SCAN_OFF_FINALIZED ) ),
        { STEP_TYPE_DONE }
    }
};

static Sequence_t  SEQ_LLSC_15 =
{
    "SEQ_LLSC_15 - Check for Duplicate Scan-On",
    {
        EXECUTE_RULE( BR_LLSC_5_3, RETURN_VALUE( SEQ_RESULT_DUPLICATE_SCAN_ON ),RETURN_VALUE( SEQ_RESULT_NO_DUPLICATE_SCAN_ON ) ),
        { STEP_TYPE_DONE }
    }
};

static Sequence_t  SEQ_LLSC_16 =
{
    "SEQ_LLSC_16 - Check for Blocking Period",
    {
        EXECUTE_RULE( BR_LLSC_4_3 , RETURN_VALUE( SEQ_RESULT_BLOCKING_PERIOD_APPLIES )  , GOTO_RULE( BR_LLSC_4_13 ) ),
        EXECUTE_RULE( BR_LLSC_4_13, RETURN_VALUE( SEQ_RESULT_BLOCKING_PERIOD_APPLIES )  , GOTO_RULE( BR_LLSC_10_1 ) ),
        EXECUTE_RULE( BR_LLSC_10_1, RETURN_VALUE( SEQ_RESULT_TRAVEL_ACCESS_PASS_IN_USE ), RETURN_VALUE( SEQ_RESULT_BLOCKING_PERIOD_EXPIRED ) ),
        { STEP_TYPE_DONE }
    }
};

static Sequence_t  SEQ_LLSC_19 =
{
    "SEQ_LLSC_19 - Determine Zone Range",
    {
        EXECUTE_RULE( BR_LLSC_6_21, GOTO_RULE( BR_LLSC_6_20 )                        , GOTO_RULE( BR_LLSC_6_23 ) ),
        EXECUTE_RULE( BR_LLSC_6_23, GOTO_RULE( BR_LLSC_6_20 )                        , GOTO_RULE( BR_LLSC_6_24 ) ),
        EXECUTE_RULE( BR_LLSC_6_24, GOTO_RULE( BR_LLSC_6_20 )                        , GOTO_RULE( BR_LLSC_6_19 ) ),
        EXECUTE_RULE( BR_LLSC_6_19, GOTO_RULE( BR_LLSC_6_20 )                        , GOTO_RULE( BR_LLSC_6_1 ) ),
        EXECUTE_RULE( BR_LLSC_6_1 , GOTO_RULE( BR_LLSC_6_20 )                        , GOTO_RULE( BR_LLSC_6_16 ) ),
        EXECUTE_RULE( BR_LLSC_6_16, GOTO_RULE( BR_LLSC_6_20 )                        , GOTO_RULE( BR_LLSC_6_17 ) ),
        EXECUTE_RULE( BR_LLSC_6_17, GOTO_RULE( BR_LLSC_6_20 )                        , RETURN_VALUE( SEQ_RESULT_NO_SCANNED_ON_PRODUCTS ) ),
        EXECUTE_RULE( BR_LLSC_6_20, RETURN_VALUE( SEQ_RESULT_ZONE_RANGE_DETERMINED ) , RETURN_VALUE( SEQ_RESULT_ERROR ) ),
        { STEP_TYPE_DONE }
    }
};
static Sequence_t  SEQ_LLSC_19b =
{
    "SEQ_LLSC_19 - Determine Zone Range",
    {
        EXECUTE_RULE( BR_LLSC_6_21, GOTO_RULE( BR_LLSC_6_20 )                        , GOTO_RULE( BR_LLSC_6_23 ) ),
        EXECUTE_RULE( BR_LLSC_6_23, GOTO_RULE( BR_LLSC_6_20 )                        , GOTO_RULE( BR_LLSC_6_24 ) ),
        EXECUTE_RULE( BR_LLSC_6_24, GOTO_RULE( BR_LLSC_6_20 )                        , GOTO_RULE( BR_LLSC_6_19 ) ),
        EXECUTE_RULE( BR_LLSC_6_19, GOTO_RULE( BR_LLSC_6_20 )                        , GOTO_RULE( BR_LLSC_6_1 ) ),
        EXECUTE_RULE( BR_LLSC_6_1 , GOTO_RULE( BR_LLSC_6_20 )                        , GOTO_RULE( BR_LLSC_6_16 ) ),
        EXECUTE_RULE( BR_LLSC_6_16, GOTO_RULE( BR_LLSC_6_20 )                        , GOTO_RULE( BR_LLSC_6_17 ) ),
        EXECUTE_RULE( BR_LLSC_6_17, GOTO_RULE( BR_LLSC_6_20 )                        , RETURN_VALUE( SEQ_RESULT_NO_SCANNED_ON_PRODUCTS ) ),
        EXECUTE_RULE( BR_LLSC_6_20, RETURN_VALUE( SEQ_RESULT_ZONE_RANGE_DETERMINED ) , RETURN_VALUE( SEQ_RESULT_ERROR ) ),
        { STEP_TYPE_DONE }
    }
};

static Sequence_t  SEQ_LLSC_20 =
{
    "SEQ_LLSC_20 - Entry only flow control validation",
    {
        EXECUTE_RULE( BR_LLSC_4_16, RETURN_VALUE( SEQ_RESULT_SCAN_OFF_REQUIRED ), GOTO_RULE( BR_LLSC_4_17 ) ),
        EXECUTE_RULE( BR_LLSC_4_17, RETURN_VALUE( SEQ_RESULT_FORCED_SCAN_OFF )  , GOTO_RULE( BR_LLSC_4_18 ) ),
        EXECUTE_RULE( BR_LLSC_4_18, RETURN_VALUE( SEQ_RESULT_FORCED_SCAN_OFF )  , GOTO_RULE( BR_LLSC_4_7 ) ),
        EXECUTE_RULE( BR_LLSC_4_7 , RETURN_VALUE( SEQ_RESULT_FORCED_SCAN_OFF )  , RETURN_VALUE( SEQ_RESULT_NO_MISSING_SCAN_OFF ) ),
        { STEP_TYPE_DONE }
    }
};

static Sequence_t  SEQ_LLSC_21 =
{
    "SEQ_LLSC_21 - Assessment of provisional trip extension",
    {
        EXECUTE_RULE( BR_LLSC_4_19, GOTO_RULE( BR_LLSC_4_20 )                      ,   GOTO_RULE( BR_LLSC_4_20 ) ),
        EXECUTE_RULE( BR_LLSC_4_20, GOTO_RULE( BR_LLSC_4_7 )                      ,  GOTO_RULE( BR_LLSC_4_7 ) ),
        EXECUTE_RULE( BR_LLSC_4_7 , RETURN_VALUE( SEQ_RESULT_FORCED_SCAN_OFF )    ,   RETURN_VALUE( SEQ_RESULT_SCAN_OFF_REQUIRED ) ),
        { STEP_TYPE_DONE }
    }
};
static Sequence_t  SEQ_LLSC_21b =
{
    "SEQ_LLSC_21 - Assessment of provisional trip extension",
    {
        EXECUTE_RULE( BR_LLSC_4_19, GOTO_RULE( BR_LLSC_4_7 )                      ,   GOTO_RULE( BR_LLSC_4_7 ) ),
        EXECUTE_RULE( BR_LLSC_4_7 , RETURN_VALUE( SEQ_RESULT_FORCED_SCAN_OFF )    ,   RETURN_VALUE( SEQ_RESULT_SCAN_OFF_REQUIRED ) ),
        { STEP_TYPE_DONE }
    }
};

/*
static Sequence_t  SEQ_LLSC_22 =
{
    "SEQ_LLSC_22 - Assess provisional zone range",
    {
        { STEP_TYPE_DONE }
    }
};
*/
static Sequence_t  SEQ_LLSC_23 =
{
    "SEQ_LLSC_23 - DDA Entry Only",
    {
        EXECUTE_RULE( BR_LLSC_10_4, GOTO_RULE( BR_LLSC_10_6 )                                  , GOTO_RULE( BR_LLSC_10_6 ) ),
        EXECUTE_RULE( BR_LLSC_10_6, GOTO_RULE( BR_LLSC_5_1 )                                   , GOTO_RULE( BR_LLSC_5_1 ) ),
        EXECUTE_RULE( BR_LLSC_5_1 , RETURN_VALUE( SEQ_RESULT_TPURSE_BALANCE_BELOW_MINIMUM )    , GOTO_RULE( BR_LLSC_5_2 ) ),
        EXECUTE_RULE( BR_LLSC_5_2 , RETURN_VALUE( SEQ_RESULT_TPURSE_BALANCE_BELOW_MINIMUM )    , GOTO_RULE( BR_LLSC_10_3 ) ),
        EXECUTE_RULE( BR_LLSC_10_3, GOTO_RULE( BR_LLSC_5_8 )                                   , GOTO_RULE( BR_LLSC_5_8 ) ),
        EXECUTE_RULE( BR_LLSC_5_8 , GOTO_RULE( BR_LLSC_1_10 )                                  , GOTO_RULE( BR_LLSC_1_10 ) ),
        EXECUTE_RULE( BR_LLSC_1_10, GOTO_RULE( BR_LLSC_1_4 )                                   , GOTO_RULE( BR_LLSC_1_4 ) ),
        EXECUTE_RULE( BR_LLSC_1_4 , RETURN_VALUE( SEQ_RESULT_SCAN_ON_TRAVEL_ACCESS_PASS )      , RETURN_VALUE( SEQ_RESULT_SCAN_ON_TRAVEL_ACCESS_PASS ) ),
        { STEP_TYPE_DONE }
    }
};

static Sequence_t  SEQ_LLSC_24 =
{
    "SEQ_LLSC_24 - DDA Exit Only",
    {
        EXECUTE_RULE( BR_LLSC_10_4, GOTO_RULE( BR_LLSC_5_1 )                                   , GOTO_RULE( BR_LLSC_5_1 ) ),
        EXECUTE_RULE( BR_LLSC_5_1 , RETURN_VALUE( SEQ_RESULT_TPURSE_BALANCE_BELOW_MINIMUM )    , GOTO_RULE( BR_LLSC_5_2 ) ),
        EXECUTE_RULE( BR_LLSC_5_2 , RETURN_VALUE( SEQ_RESULT_TPURSE_BALANCE_BELOW_MINIMUM )    , GOTO_RULE( BR_LLSC_10_3 ) ),
        EXECUTE_RULE( BR_LLSC_10_3, GOTO_RULE( BR_LLSC_10_6 )                                  , GOTO_RULE( BR_LLSC_10_6 ) ),
        EXECUTE_RULE( BR_LLSC_10_6, GOTO_RULE( BR_LLSC_1_10 )                                  , RETURN_VALUE( SEQ_RESULT_ERROR ) ),
        EXECUTE_RULE( BR_LLSC_1_10, GOTO_RULE( BR_LLSC_1_4 )                                   , GOTO_RULE( BR_LLSC_1_4 ) ),
        EXECUTE_RULE( BR_LLSC_1_4 , RETURN_VALUE( SEQ_RESULT_SCAN_OFF_TRAVEL_ACCESS_PASS )     , RETURN_VALUE( SEQ_RESULT_SCAN_OFF_TRAVEL_ACCESS_PASS ) ),
        { STEP_TYPE_DONE }
    }
};

static RuleResult_e BR_LLSC_1_4a( MYKI_BR_ContextData_t *pData ) { return BR_LLSC_1_4( pData ); }
static RuleResult_e BR_LLSC_1_4b( MYKI_BR_ContextData_t *pData ) { return BR_LLSC_1_4( pData ); }

static Sequence_t  SEQ_LLSC_25 =
{
    "SEQ_LLSC_25 - DDA Entry / Exit",
    {
        EXECUTE_RULE( BR_LLSC_10_4, GOTO_RULE( BR_LLSC_5_1 )                                   , GOTO_RULE( BR_LLSC_10_6 ) ),
        EXECUTE_RULE( BR_LLSC_10_6, GOTO_RULE( BR_LLSC_1_4a )                                  , GOTO_RULE( BR_LLSC_5_1 ) ),
        EXECUTE_RULE( BR_LLSC_1_4a, RETURN_VALUE( SEQ_RESULT_SCAN_OFF_TRAVEL_ACCESS_PASS )     , RETURN_VALUE( SEQ_RESULT_SCAN_OFF_TRAVEL_ACCESS_PASS ) ),
        EXECUTE_RULE( BR_LLSC_5_1,  RETURN_VALUE( SEQ_RESULT_TPURSE_BALANCE_BELOW_MINIMUM )    , GOTO_RULE( BR_LLSC_5_2 ) ),
        EXECUTE_RULE( BR_LLSC_5_2,  RETURN_VALUE( SEQ_RESULT_TPURSE_BALANCE_BELOW_MINIMUM )    , GOTO_RULE( BR_LLSC_10_3 ) ),
        EXECUTE_RULE( BR_LLSC_10_3, GOTO_RULE( BR_LLSC_5_8 )                                   , GOTO_RULE( BR_LLSC_5_8 ) ),
        EXECUTE_RULE( BR_LLSC_5_8,  GOTO_RULE( BR_LLSC_1_10 )                                  , GOTO_RULE( BR_LLSC_1_10 ) ),
        EXECUTE_RULE( BR_LLSC_1_10, GOTO_RULE( BR_LLSC_1_4b )                                  , GOTO_RULE( BR_LLSC_1_4b ) ),
        EXECUTE_RULE( BR_LLSC_1_4b, RETURN_VALUE( SEQ_RESULT_SCAN_ON_TRAVEL_ACCESS_PASS )      , RETURN_VALUE( SEQ_RESULT_SCAN_ON_TRAVEL_ACCESS_PASS ) ),
        { STEP_TYPE_DONE }
    }
};
/*

static Sequence_t  SEQ_LLSC_26 =
{
    "SEQ_LLSC_26 - DDA Onboard Validation",
    {
        { STEP_TYPE_DONE }
    }
};

static Sequence_t  SEQ_LLSC_27 =
{
    "SEQ_LLSC_27 - DDA Validation",
    {
        { STEP_TYPE_DONE }
    }
};
*/
Sequence_t  SEQ_LLSC_EntryOnly =
{
    "SEQ_LLSC_EntryOnly - LLSC Entry-Only Processing",
    {
        {   // Step 1
            STEP_SEQUENCE( SEQ_LLSC_1 ),
            {
                { SEQ_RESULT_MULTIPLE_CARDS                     , RETURN_VALUE( SEQ_RESULT_REJECT           ) },    // Reject
                { SEQ_RESULT_REJECT                             , RETURN_VALUE( SEQ_RESULT_REJECT           ) },    // Reject (not covered by NTS0177)
                { SEQ_RESULT_CARD_INITIALISED                   , GOTO_SEQUENCE( SEQ_LLSC_2                 ) }     // Step 2
            }
        },
        {   // Step 2
            STEP_SEQUENCE( SEQ_LLSC_2 ),
            {
                { SEQ_RESULT_INVALID_TRANSIT_APPLICATION_STATUS , RETURN_VALUE( SEQ_RESULT_REJECT           ) },    // Reject
                { SEQ_RESULT_ACTIONLISTS_PROCESSED              , GOTO_SEQUENCE( SEQ_LLSC_3                 ) }     // Step 3
            }
        },
        {   // Step 3
            STEP_SEQUENCE( SEQ_LLSC_3 ),
            {
                { SEQ_RESULT_AUTOLOAD_PROCESSED                 , GOTO_SEQUENCE( SEQ_LLSC_16                ) }     // Step 4
            }
        },
        {   // Step 4
            STEP_SEQUENCE( SEQ_LLSC_16 ),
            {
                { SEQ_RESULT_BLOCKING_PERIOD_APPLIES            , RETURN_VALUE( SEQ_RESULT_REJECT           ) },    // Reject
                { SEQ_RESULT_BLOCKING_PERIOD_EXPIRED            , GOTO_SEQUENCE( SEQ_LLSC_5                 ) },    // Step 5
                { SEQ_RESULT_TRAVEL_ACCESS_PASS_IN_USE          , GOTO_SEQUENCE( SEQ_LLSC_23                ) }     // Step 16
            }
        },
        {   // Step 5
            STEP_SEQUENCE( SEQ_LLSC_5 ),
            {
                { SEQ_RESULT_SCAN_OFF_REQUIRED                  , GOTO_SEQUENCE( SEQ_LLSC_19                ) },    // Step 13
                { SEQ_RESULT_FORCED_SCAN_OFF                    , GOTO_SEQUENCE( SEQ_LLSC_10                ) },    // Step 8
                { SEQ_RESULT_NO_MISSING_SCAN_OFF                , GOTO_SEQUENCE( SEQ_LLSC_20                ) },    // Step 14
                { SEQ_RESULT_UNABLE_TO_SCAN_OFF                 , GOTO_SEQUENCE( SEQ_LLSC_14                ) }     // Step 11
            }
        },
        {   // Step 6
            STEP_SEQUENCE( SEQ_LLSC_7 ),
            {
                { SEQ_RESULT_TPURSE_BALANCE_BELOW_MINIMUM       , RETURN_VALUE( SEQ_RESULT_REJECT           ) },    // Reject
                { SEQ_RESULT_PRODUCT_SCANNED_ON                 , RETURN_VALUE( SEQ_RESULT_SCAN_ON          ) }     // Success (done)
            }
        },
        {   // Step 7
            STEP_SEQUENCE( SEQ_LLSC_9 ),
            {
                { SEQ_RESULT_SCAN_OFF_PERFORMED                 , GOTO_SEQUENCE(  SEQ_LLSC_14               ) },    // Step 11
                { SEQ_RESULT_SCAN_OFF_WITH_PARTIAL_COVERAGE     , GOTO_SEQUENCE(  SEQ_LLSC_11               ) }     // Step 9
            }
        },
        {   // Step 8
            STEP_SEQUENCE( SEQ_LLSC_10 ),
            {
                { SEQ_RESULT_FORCED_SCAN_OFF_PENALTY            , GOTO_SEQUENCE( SEQ_LLSC_14                ) },    // Step 11
                { SEQ_RESULT_SCAN_OFF_WITH_PARTIAL_COVERAGE     , GOTO_SEQUENCE( SEQ_LLSC_11                ) }     // Step 9
            }
        },
        {   // Step 9
            STEP_SEQUENCE( SEQ_LLSC_11 ),
            {
                { SEQ_RESULT_CAPPING_PROCESSED                  , GOTO_SEQUENCE( SEQ_LLSC_12                ) }     // Step 10
            }
        },
        {   // Step 10
            STEP_SEQUENCE( SEQ_LLSC_12 ),
            {
                { SEQ_RESULT_MERGE_PRODUCTS                     , GOTO_SEQUENCE( SEQ_LLSC_14                ) }     // Step 11
            }
        },
        {   // Step 11
            STEP_SEQUENCE( SEQ_LLSC_14 ),
            {
                { SEQ_RESULT_SCAN_OFF_FINALIZED                 , GOTO_SEQUENCE( SEQ_LLSC_7                 ) }     // Step 6
            }
        },
        {   // Step 12
            STEP_SEQUENCE( SEQ_LLSC_15 ),
            {
                { SEQ_RESULT_DUPLICATE_SCAN_ON                  , RETURN_VALUE( SEQ_RESULT_REJECT           ) },    // Reject
                { SEQ_RESULT_NO_DUPLICATE_SCAN_ON               , GOTO_SEQUENCE( SEQ_LLSC_7                 ) }     // Step 6
            }
        },
        {   // Step 13
            STEP_SEQUENCE( SEQ_LLSC_19 ),
            {
                { SEQ_RESULT_ZONE_RANGE_DETERMINED              , GOTO_SEQUENCE( SEQ_LLSC_21                ) },    // Step 15
                { SEQ_RESULT_NO_SCANNED_ON_PRODUCTS             , RETURN_VALUE( SEQ_RESULT_REJECT           ) }     // Reject
            }
        },
        {   // Step 14
            STEP_SEQUENCE( SEQ_LLSC_20 ),
            {
                { SEQ_RESULT_SCAN_OFF_REQUIRED                  , GOTO_SEQUENCE( SEQ_LLSC_19                ) },    // Step 13
                { SEQ_RESULT_FORCED_SCAN_OFF                    , GOTO_SEQUENCE( SEQ_LLSC_10                ) },    // Step 8
                { SEQ_RESULT_NO_MISSING_SCAN_OFF                , GOTO_SEQUENCE( SEQ_LLSC_15                ) }     // Step 12
            }
        },
        {   // Step 15
            STEP_SEQUENCE( SEQ_LLSC_21 ),
            {
                { SEQ_RESULT_SCAN_OFF_REQUIRED                  , GOTO_SEQUENCE( SEQ_LLSC_9                 ) },    // Step 7
                { SEQ_RESULT_FORCED_SCAN_OFF                    , GOTO_SEQUENCE( SEQ_LLSC_10                ) }     // Step 8
            }
        },
        {   // Step 16
            STEP_SEQUENCE( SEQ_LLSC_23 ),
            {
                { SEQ_RESULT_TPURSE_BALANCE_BELOW_MINIMUM       , RETURN_VALUE( SEQ_RESULT_REJECT           ) },    // Reject
                { SEQ_RESULT_SCAN_ON_TRAVEL_ACCESS_PASS         , RETURN_VALUE( SEQ_RESULT_SCAN_ON          ) }     // Success (done)
            }
        },
        { STEP_TYPE_DONE }
    }
};

Sequence_t  SEQ_LLSC_ExitOnly =
{
    "SEQ_LLSC_ExitOnly - LLSC Exit-Only Processing",
    {
        {   // Step 1
            STEP_SEQUENCE( SEQ_LLSC_1 ),
            {
                { SEQ_RESULT_MULTIPLE_CARDS                     , RETURN_VALUE( SEQ_RESULT_REJECT           ) },    // Reject
                { SEQ_RESULT_REJECT                             , RETURN_VALUE( SEQ_RESULT_REJECT           ) },    // Reject (not covered by NTS0177)
                { SEQ_RESULT_CARD_INITIALISED                   , GOTO_SEQUENCE( SEQ_LLSC_2                 ) }     // Step 2
            }
        },
        {   // Step 2
            STEP_SEQUENCE( SEQ_LLSC_2 ),
            {
                { SEQ_RESULT_INVALID_TRANSIT_APPLICATION_STATUS , RETURN_VALUE( SEQ_RESULT_REJECT           ) },    // Reject
                { SEQ_RESULT_ACTIONLISTS_PROCESSED              , GOTO_SEQUENCE( SEQ_LLSC_3                 ) }     // Step 3
            }
        },
        {   // Step 3
            STEP_SEQUENCE( SEQ_LLSC_3 ),
            {
                { SEQ_RESULT_AUTOLOAD_PROCESSED                 , GOTO_SEQUENCE( SEQ_LLSC_16                ) }     // Step 4
            }
        },
        {   // Step 4
            STEP_SEQUENCE( SEQ_LLSC_16 ),
            {
                { SEQ_RESULT_BLOCKING_PERIOD_APPLIES            , RETURN_VALUE( SEQ_RESULT_REJECT           ) },    // Reject
                { SEQ_RESULT_BLOCKING_PERIOD_EXPIRED            , GOTO_SEQUENCE( SEQ_LLSC_4                 ) },    // Step 5
                { SEQ_RESULT_TRAVEL_ACCESS_PASS_IN_USE          , GOTO_SEQUENCE( SEQ_LLSC_24                ) }     // Step 15
            }
        },
        {   // Step 5
            STEP_SEQUENCE( SEQ_LLSC_4 ),
            {
                { SEQ_RESULT_CHANGE_OF_MIND                     , GOTO_SEQUENCE( SEQ_LLSC_14                ) },    // Step 12b
                { SEQ_RESULT_SCAN_OFF_REQUIRED                  , GOTO_SEQUENCE( SEQ_LLSC_19                ) },    // Step 13
                { SEQ_RESULT_FORCED_SCAN_OFF                    , RETURN_VALUE( SEQ_RESULT_REJECT           ) },    // Reject
                { SEQ_RESULT_NO_CHANGE_OF_MIND                  , GOTO_SEQUENCE( SEQ_LLSC_5                 ) }     // Step 6
            }
        },
        {   // Step 6
            STEP_SEQUENCE( SEQ_LLSC_5 ),
            {
                { SEQ_RESULT_SCAN_OFF_REQUIRED                  , GOTO_SEQUENCE( SEQ_LLSC_19                ) },    // Step 13
                { SEQ_RESULT_FORCED_SCAN_OFF                    , RETURN_VALUE( SEQ_RESULT_REJECT           ) },    // Reject
                { SEQ_RESULT_NO_MISSING_SCAN_OFF                , GOTO_SEQUENCE( SEQ_LLSC_6                 ) },    // Step 7
                { SEQ_RESULT_UNABLE_TO_SCAN_OFF                 , GOTO_SEQUENCE( SEQ_LLSC_14                ) }     // Step 12b
            }
        },
        {   // Step 7
            STEP_SEQUENCE( SEQ_LLSC_6 ),
            {
                { SEQ_RESULT_TPURSE_BALANCE_BELOW_MINIMUM       , RETURN_VALUE( SEQ_RESULT_REJECT           ) },    // Reject
                { SEQ_RESULT_STATION_EXIT_FEE_CHARGED           , RETURN_VALUE( SEQ_RESULT_SCAN_OFF         ) },    // Success (done)
                { SEQ_RESULT_MISSING_SCAN_ON                    , RETURN_VALUE( SEQ_RESULT_REJECT           ) },    // Reject
                { SEQ_RESULT_SCAN_ON_PRESENT                    , GOTO_SEQUENCE( SEQ_LLSC_19                ) }     // Step 13
            }
        },
        {   // Step 8
            STEP_SEQUENCE( SEQ_LLSC_9 ),
            {
                { SEQ_RESULT_SCAN_OFF_PERFORMED                 , GOTO_SEQUENCE(  SEQ_LLSC_14               ) },    // Step 12b
                { SEQ_RESULT_SCAN_OFF_WITH_PARTIAL_COVERAGE     , GOTO_SEQUENCE(  SEQ_LLSC_11               ) }     // Step 10b
            }
        },
        {   // Step 10b
            STEP_SEQUENCE( SEQ_LLSC_11 ),
            {
                { SEQ_RESULT_CAPPING_PROCESSED                  , GOTO_SEQUENCE( SEQ_LLSC_12                ) }     // Step 11b
            }
        },
        {   // Step 11b
            STEP_SEQUENCE( SEQ_LLSC_12 ),
            {
                { SEQ_RESULT_MERGE_PRODUCTS                     , GOTO_SEQUENCE( SEQ_LLSC_14                ) }     // Step 12b
            }
        },
        {   // Step 12b
            STEP_SEQUENCE( SEQ_LLSC_14 ),
            {
                { SEQ_RESULT_SCAN_OFF_FINALIZED                 , RETURN_VALUE( SEQ_RESULT_SCAN_OFF          ) }     // Success (done)
            }
        },
        {   // Step 13
            STEP_SEQUENCE( SEQ_LLSC_19 ),
            {
                { SEQ_RESULT_ZONE_RANGE_DETERMINED              , GOTO_SEQUENCE( SEQ_LLSC_21                ) },    // Step 15
                { SEQ_RESULT_NO_SCANNED_ON_PRODUCTS             , RETURN_VALUE( SEQ_RESULT_REJECT           ) }     // Reject
            }
        },
        {   // Step 14
            STEP_SEQUENCE( SEQ_LLSC_21 ),
            {
                { SEQ_RESULT_SCAN_OFF_REQUIRED                  , GOTO_SEQUENCE( SEQ_LLSC_9                 ) },    // Step 8
                { SEQ_RESULT_FORCED_SCAN_OFF                    , RETURN_VALUE( SEQ_RESULT_REJECT           ) }     // Reject
            }
        },
        {   // Step 15
            STEP_SEQUENCE( SEQ_LLSC_24 ),
            {
                { SEQ_RESULT_SCAN_OFF_TRAVEL_ACCESS_PASS        , RETURN_VALUE( SEQ_RESULT_SCAN_OFF           ) },    // Success (done)
                { SEQ_RESULT_TPURSE_BALANCE_BELOW_MINIMUM       , RETURN_VALUE( SEQ_RESULT_REJECT             ) }     // Reject
            }
        },
        { STEP_TYPE_DONE }
    }
};

Sequence_t  SEQ_LLSC_EntryExit =
{
    "SEQ_LLSC_EntryExit - LLSC Entry/Exit Processing",
    {
        {   // Step 1
            STEP_SEQUENCE( SEQ_LLSC_1 ),
            {
                { SEQ_RESULT_MULTIPLE_CARDS                     , RETURN_VALUE( SEQ_RESULT_REJECT           ) },    // Reject
                { SEQ_RESULT_REJECT                             , RETURN_VALUE( SEQ_RESULT_REJECT           ) },    // Reject (not covered by NTS0177)
                { SEQ_RESULT_CARD_INITIALISED                   , GOTO_SEQUENCE( SEQ_LLSC_2                 ) }     // Step 2
            }
        },
        {   // Step 2
            STEP_SEQUENCE( SEQ_LLSC_2 ),
            {
                { SEQ_RESULT_INVALID_TRANSIT_APPLICATION_STATUS , RETURN_VALUE( SEQ_RESULT_REJECT           ) },    // Reject
                { SEQ_RESULT_ACTIONLISTS_PROCESSED              , GOTO_SEQUENCE( SEQ_LLSC_3                 ) }     // Step 3
            }
        },
        {   // Step 3
            STEP_SEQUENCE( SEQ_LLSC_3 ),
            {
                { SEQ_RESULT_AUTOLOAD_PROCESSED                 , GOTO_SEQUENCE( SEQ_LLSC_16                ) }     // Step 4
            }
        },
        {   // Step 4
            STEP_SEQUENCE( SEQ_LLSC_16 ),
            {
                { SEQ_RESULT_BLOCKING_PERIOD_APPLIES            , RETURN_VALUE( SEQ_RESULT_REJECT           ) },    // Reject
                { SEQ_RESULT_BLOCKING_PERIOD_EXPIRED            , GOTO_SEQUENCE( SEQ_LLSC_4                 ) },    // Step 4a
                { SEQ_RESULT_TRAVEL_ACCESS_PASS_IN_USE          , GOTO_SEQUENCE( SEQ_LLSC_25                ) }     // Step 14
            }
        },
        {   // Step 4a
            STEP_SEQUENCE( SEQ_LLSC_4 ),
            {
                { SEQ_RESULT_CHANGE_OF_MIND                     , GOTO_SEQUENCE( SEQ_LLSC_14b               ) },    // Step 11b
                { SEQ_RESULT_SCAN_OFF_REQUIRED                  , GOTO_SEQUENCE( SEQ_LLSC_19b               ) },    // Step 12b
                { SEQ_RESULT_FORCED_SCAN_OFF                    , GOTO_SEQUENCE( SEQ_LLSC_10                ) },    // Step 8
                { SEQ_RESULT_NO_CHANGE_OF_MIND                  , GOTO_SEQUENCE( SEQ_LLSC_5                 ) }     // Step 5
            }
        },
        {   // Step 5
            STEP_SEQUENCE( SEQ_LLSC_5 ),
            {
                { SEQ_RESULT_SCAN_OFF_REQUIRED                  , GOTO_SEQUENCE( SEQ_LLSC_19                ) },    // Step 12a
                { SEQ_RESULT_FORCED_SCAN_OFF                    , GOTO_SEQUENCE( SEQ_LLSC_10                ) },    // Step 8
                { SEQ_RESULT_NO_MISSING_SCAN_OFF                , GOTO_SEQUENCE( SEQ_LLSC_19b               ) },    // Step 12b
                { SEQ_RESULT_UNABLE_TO_SCAN_OFF                 , GOTO_SEQUENCE( SEQ_LLSC_14b               ) }     // Step 11b
            }
        },
        {   // Step 6
            STEP_SEQUENCE( SEQ_LLSC_7 ),
            {
                { SEQ_RESULT_TPURSE_BALANCE_BELOW_MINIMUM       , RETURN_VALUE( SEQ_RESULT_REJECT           ) },    // Reject
                { SEQ_RESULT_PRODUCT_SCANNED_ON                 , RETURN_VALUE( SEQ_RESULT_SCAN_ON          ) }     // Success (done)
            }
        },
        {   // Step 7a
            STEP_SEQUENCE( SEQ_LLSC_9 ),
            {
                { SEQ_RESULT_SCAN_OFF_PERFORMED                 , GOTO_SEQUENCE(  SEQ_LLSC_14               ) },    // Step 11a
                { SEQ_RESULT_SCAN_OFF_WITH_PARTIAL_COVERAGE     , GOTO_SEQUENCE(  SEQ_LLSC_11               ) }     // Step 9a
            }
        },
        {   // Step 7b
            STEP_SEQUENCE( SEQ_LLSC_9b ),
            {
                { SEQ_RESULT_SCAN_OFF_PERFORMED                 , GOTO_SEQUENCE(  SEQ_LLSC_14b              ) },    // Step 11b
                { SEQ_RESULT_SCAN_OFF_WITH_PARTIAL_COVERAGE     , GOTO_SEQUENCE(  SEQ_LLSC_11b              ) }     // Step 9b
            }
        },
        {   // Step 8
            STEP_SEQUENCE( SEQ_LLSC_10 ),
            {
                { SEQ_RESULT_SCAN_OFF_WITH_PARTIAL_COVERAGE     , GOTO_SEQUENCE( SEQ_LLSC_11                ) },   // Step 9a
                { SEQ_RESULT_FORCED_SCAN_OFF_PENALTY            , GOTO_SEQUENCE( SEQ_LLSC_14                ) }    // Step 11a
            }
        },
        {   // Step 9a
            STEP_SEQUENCE( SEQ_LLSC_11 ),
            {
                { SEQ_RESULT_CAPPING_PROCESSED                  , GOTO_SEQUENCE( SEQ_LLSC_12                ) }     // Step 10a
            }
        },
        {   // Step 10a
            STEP_SEQUENCE( SEQ_LLSC_12 ),
            {
                { SEQ_RESULT_MERGE_PRODUCTS                     , GOTO_SEQUENCE( SEQ_LLSC_14                ) }     // Step 11a
            }
        },
        {   // Step 11a
            STEP_SEQUENCE( SEQ_LLSC_14 ),
            {
                { SEQ_RESULT_SCAN_OFF_FINALIZED                 , GOTO_SEQUENCE(  SEQ_LLSC_7                ) }     // Step 6
            }
        },
        {   // Step 9b
            STEP_SEQUENCE( SEQ_LLSC_11b ),
            {
                { SEQ_RESULT_CAPPING_PROCESSED                  , GOTO_SEQUENCE( SEQ_LLSC_12b               ) }     // Step 10b
            }
        },
        {   // Step 10b
            STEP_SEQUENCE( SEQ_LLSC_12b ),
            {
                { SEQ_RESULT_MERGE_PRODUCTS                     , GOTO_SEQUENCE( SEQ_LLSC_14b               ) }     // Step 11b
            }
        },
        {   // Step 11b
            STEP_SEQUENCE( SEQ_LLSC_14b ),
            {
                { SEQ_RESULT_SCAN_OFF_FINALIZED                 ,  RETURN_VALUE( SEQ_RESULT_SCAN_OFF        ) }     // Success (done)
            }
        },
        {   // Step 12a
            STEP_SEQUENCE( SEQ_LLSC_19 ),
            {
                { SEQ_RESULT_ZONE_RANGE_DETERMINED              , GOTO_SEQUENCE( SEQ_LLSC_21                ) },    // Step 13a
                { SEQ_RESULT_NO_SCANNED_ON_PRODUCTS             , RETURN_VALUE( SEQ_RESULT_REJECT           ) }     // Reject
            }
        },
        {   // Step 12b
            STEP_SEQUENCE( SEQ_LLSC_19b ),
            {
                { SEQ_RESULT_ZONE_RANGE_DETERMINED              , GOTO_SEQUENCE( SEQ_LLSC_21b               ) },    // Step 13b
                { SEQ_RESULT_NO_SCANNED_ON_PRODUCTS             , GOTO_SEQUENCE( SEQ_LLSC_7                 ) }     // Step6
            }
        },
        {   // Step 13a
            STEP_SEQUENCE( SEQ_LLSC_21 ),
            {
                { SEQ_RESULT_SCAN_OFF_REQUIRED                  , GOTO_SEQUENCE( SEQ_LLSC_9                 ) },    // Step 7a
                { SEQ_RESULT_FORCED_SCAN_OFF                    , GOTO_SEQUENCE( SEQ_LLSC_10                ) }     // Step 8
            }
        },
        {   // Step 13b
            STEP_SEQUENCE( SEQ_LLSC_21b ),
            {
                { SEQ_RESULT_SCAN_OFF_REQUIRED                  , GOTO_SEQUENCE( SEQ_LLSC_9b                ) },    // Step 7b
                { SEQ_RESULT_FORCED_SCAN_OFF                    , GOTO_SEQUENCE( SEQ_LLSC_10                ) }     // Step 8
            }
        },
        {   // Step 14
            STEP_SEQUENCE( SEQ_LLSC_25 ),
            {
                { SEQ_RESULT_TPURSE_BALANCE_BELOW_MINIMUM       , RETURN_VALUE( SEQ_RESULT_REJECT           ) },    // Reject
                { SEQ_RESULT_SCAN_ON_TRAVEL_ACCESS_PASS         , RETURN_VALUE( SEQ_RESULT_SCAN_ON          ) },    // Success (done)
                { SEQ_RESULT_SCAN_OFF_TRAVEL_ACCESS_PASS        , RETURN_VALUE( SEQ_RESULT_SCAN_OFF         ) }     // Success (done)
            }
        },
        { STEP_TYPE_DONE }
    }
};

    /*      Pre-commit check and reject card presentation.
     *
     *      SEQ_RESULT_REJECT                   Reject transaction
     *      SEQ_RESULT_PRECOMMIT_CHECK_FAILED   Failed pre-commit check
     */
Sequence_t  SEQ_PrecommitCheckAndReject =
{
    "SEQ_PrecommitCheckAndReject - Pre-commit check and reject card",
    {
        EXECUTE_RULE( BR_LLSC_99_1,         RETURN_VALUE( SEQ_RESULT_REJECT ),      RETURN_VALUE( SEQ_RESULT_PRECOMMIT_CHECK_FAILED ) ),
        { STEP_TYPE_DONE }
    }
};

    /*      Pre-commit check and scan-on.
     *
     *      SEQ_RESULT_SCAN_ON                  scan-on transaction
     *      SEQ_RESULT_PRECOMMIT_CHECK_FAILED   Failed pre-commit check
     */
Sequence_t  SEQ_PrecommitCheckAndScanOn =
{
    "SEQ_PrecommitCheckAndReject - Pre-commit check and scan-on",
    {
        EXECUTE_RULE( BR_LLSC_99_1,         RETURN_VALUE( SEQ_RESULT_SCAN_ON ),     RETURN_VALUE( SEQ_RESULT_PRECOMMIT_CHECK_FAILED ) ),
        { STEP_TYPE_DONE }
    }
};

    /*      Pre-commit check and scan-off.
     *
     *      SEQ_RESULT_SCAN_OFF                 scan-off transaction
     *      SEQ_RESULT_PRECOMMIT_CHECK_FAILED   Failed pre-commit check
     */
Sequence_t  SEQ_PrecommitCheckAndScanOff =
{
    "SEQ_PrecommitCheckAndScanOff - Pre-commit check and scan-off",
    {
        EXECUTE_RULE( BR_LLSC_99_1,         RETURN_VALUE( SEQ_RESULT_SCAN_OFF ),    RETURN_VALUE( SEQ_RESULT_PRECOMMIT_CHECK_FAILED ) ),
        { STEP_TYPE_DONE }
    }
};

//  Not in NTS0177 - Chooses whether to run the LLSC Entry-Only or Exit-Only sequences based on the device mode

Sequence_t  SEQ_LLSC =
{
    "SEQ_LLSC - Process an LLSC card",
    {
        EXECUTE_RULE( BR_VIX_0_1, GOTO_SEQUENCE( SEQ_LLSC_EntryOnly ), GOTO_RULE( BR_VIX_0_2 ) ),           // Is Entry Only
        EXECUTE_RULE( BR_VIX_0_2, GOTO_SEQUENCE( SEQ_LLSC_ExitOnly  ), GOTO_RULE( BR_VIX_0_5  ) ),          // Is Exit Only
        EXECUTE_RULE( BR_VIX_0_5, GOTO_SEQUENCE( SEQ_LLSC_EntryExit ), RETURN_VALUE( SEQ_RESULT_ERROR ) ),  // Is EntryExit (platform)
        //EXECUTE_RULE( BR_VIX_0_?, GOTO_SEQUENCE( SEQ_LLSC_OnBoard ), RETURN_VALUE( SEQ_RESULT_ERROR ) ),  // Is neither (onboard)
        {
            STEP_SEQUENCE( SEQ_LLSC_EntryOnly ),
            {
                { SEQ_RESULT_DEFAULT , RETURN_VALUE( SEQ_RESULT_PASSTHROUGH ) } // Return result from subsequence
            }
        },
        {
            STEP_SEQUENCE( SEQ_LLSC_ExitOnly ),
            {
                { SEQ_RESULT_DEFAULT , RETURN_VALUE( SEQ_RESULT_PASSTHROUGH ) } // Return result from subsequence
            }
        },
        {
            STEP_SEQUENCE( SEQ_LLSC_EntryExit ),
            {
                { SEQ_RESULT_DEFAULT , RETURN_VALUE( SEQ_RESULT_PASSTHROUGH ) } // Return result from subsequence
            }
        },
        { STEP_TYPE_DONE }
    }
};

static Sequence_t  SEQ_OSC_0 =
{
    "SEQ_OSC_0 - Card Transaction Initialisation",
    {
        EXECUTE_RULE( BR_OSC_0_A, GOTO_RULE( BR_OSC_0_B )                , GOTO_RULE( BR_OSC_0_B ) ),
        EXECUTE_RULE( BR_OSC_0_B, RETURN_VALUE( SEQ_RESULT_CONTINUE )    , RETURN_VALUE( SEQ_RESULT_REJECT ) ),
        { STEP_TYPE_DONE }
    }
};

static Sequence_t  SEQ_OSC_1 =
{
    "SEQ_OSC_1 - Actionlist Processing",
    {
        EXECUTE_RULE( BR_OSC_1_1, GOTO_RULE( BR_OSC_1_2 )                , GOTO_RULE( BR_OSC_1_2 ) ),
        EXECUTE_RULE( BR_OSC_1_2, GOTO_RULE( BR_OSC_1_3 )                , GOTO_RULE( BR_OSC_1_3 ) ),
        EXECUTE_RULE( BR_OSC_1_3, RETURN_VALUE( SEQ_RESULT_CONTINUE )    , RETURN_VALUE( SEQ_RESULT_REJECT ) ),
        { STEP_TYPE_DONE }
    }
};

static Sequence_t  SEQ_OSC_2 =
{
    "SEQ_OSC_2 - Authorized Officer Card Processing",
    {
        EXECUTE_RULE( BR_OSC_1_3, GOTO_RULE( BR_OSC_1_4 )                , RETURN_VALUE( SEQ_RESULT_REJECT ) ),
        EXECUTE_RULE( BR_OSC_1_4, RETURN_VALUE( SEQ_RESULT_CONTINUE )    , RETURN_VALUE( SEQ_RESULT_CONTINUE ) ),
        { STEP_TYPE_DONE }
    }
};

//  Appears to be only partially defined in NTS0177

Sequence_t  SEQ_OSC =
{
    "SEQ_OSC - Process an OSC card",
    {
        {   // Step 1
            STEP_SEQUENCE( SEQ_OSC_0 ),
            {
                { SEQ_RESULT_REJECT                             , RETURN_VALUE( SEQ_RESULT_REJECT       ) },    // Reject (not covered by NTS0177)
                { SEQ_RESULT_CONTINUE                           , GOTO_SEQUENCE( SEQ_OSC_1                  ) }     // Step 2
            }
        },
        {   // Step 2
            STEP_SEQUENCE( SEQ_OSC_1 ),
            {
                { SEQ_RESULT_REJECT                             , RETURN_VALUE( SEQ_RESULT_REJECT       ) },    // Reject (Should be Invalid OpAp, but this constant doesn't exist yet)
                { SEQ_RESULT_CONTINUE                           , GOTO_SEQUENCE( SEQ_OSC_2                  ) }     // Step 3
            }
        },
        {   // Step 3
            STEP_SEQUENCE( SEQ_OSC_2 ),
            {
                { SEQ_RESULT_REJECT                             , RETURN_VALUE( SEQ_RESULT_REJECT       ) },    // Reject (AO processing failed)
                { SEQ_RESULT_CONTINUE                           , RETURN_VALUE( SEQ_RESULT_OPERATOR     ) }     // Success (done, not covered by NTS0177)
            }
        },
        { STEP_TYPE_DONE }
    }
};

    /*      Process Driver shift start - VIX specific business rule
     *
     *      SEQ_RESULT_OPERATOR             successfully
     */
static  Sequence_t  SEQ_OSC_ShiftStart =
{
    "SEQ_OSC_ShiftStart",
    {
        EXECUTE_RULE( BR_VIX_OSC_2_5,       GOTO_RULE( BR_VIX_OSC_3_2 ),            RETURN_VALUE( SEQ_RESULT_OPERATOR ) ),
        EXECUTE_RULE( BR_VIX_OSC_3_2,       RETURN_VALUE( SEQ_RESULT_OPERATOR ),    GOTO_RULE( BR_VIX_OSC_3_5 ) ),
        EXECUTE_RULE( BR_VIX_OSC_3_5,       RETURN_VALUE( SEQ_RESULT_OPERATOR ),    GOTO_RULE( BR_VIX_OSC_3_1 ) ),
        EXECUTE_RULE( BR_VIX_OSC_3_1,       RETURN_VALUE( SEQ_RESULT_OPERATOR ),    RETURN_VALUE( SEQ_RESULT_OPERATOR ) ),
        { STEP_TYPE_DONE }
    }
};

    /*      Validate Operator PIN - VIX specific business rule
     *
     *      SEQ_RESULT_REJECT               PIN validation failed
     *      SEQ_RESULT_OPERATOR             PIN validated successfully
     *
     */
Sequence_t  SEQ_OSC_ValidatePin =
{
    "SEQ_OSC_ValidatePin",
    {
        EXECUTE_RULE( BR_VIX_0_4,           GOTO_SEQUENCE( SEQ_OSC ),               RETURN_VALUE( SEQ_RESULT_REJECT ) ),
        {
            STEP_SEQUENCE( SEQ_OSC ),
            {
                { SEQ_RESULT_OPERATOR,                                              GOTO_RULE( BR_VIX_OSC_2_1 ) },
                { SEQ_RESULT_REJECT,                                                RETURN_VALUE( SEQ_RESULT_REJECT ) }
            }
        },
        EXECUTE_RULE( BR_VIX_OSC_2_1,       GOTO_RULE( BR_VIX_OSC_2_2 ),            RETURN_VALUE( SEQ_RESULT_REJECT ) ),
        EXECUTE_RULE( BR_VIX_OSC_2_2,       GOTO_SEQUENCE( SEQ_OSC_ShiftStart ),    RETURN_VALUE( SEQ_RESULT_REJECT ) ),
        {
            STEP_SEQUENCE( SEQ_OSC_ShiftStart ),
            {
                { SEQ_RESULT_DEFAULT,                                               RETURN_VALUE( SEQ_RESULT_PASSTHROUGH ) }
            }
        },
        { STEP_TYPE_DONE }
    }
};

    /*      Update Operator PIN - VIX specific business rule
     *
     *      SEQ_RESULT_REJECT               PIN update failed
     *      SEQ_RESULT_OPERATOR             PIN updated successfully
     */
Sequence_t  SEQ_OSC_UpdatePin =
{
    "SEQ_OSC_UpdatePin",
    {
        EXECUTE_RULE( BR_VIX_0_4,           GOTO_SEQUENCE( SEQ_OSC_0 ),             RETURN_VALUE( SEQ_RESULT_REJECT ) ),
        {
            STEP_SEQUENCE( SEQ_OSC_0 ),
            {
                { SEQ_RESULT_CONTINUE,                                              GOTO_RULE( BR_VIX_OSC_2_3 ) },
                { SEQ_RESULT_REJECT,                                                RETURN_VALUE( SEQ_RESULT_REJECT ) }
            }
        },
        EXECUTE_RULE( BR_VIX_OSC_2_3,       RETURN_VALUE( SEQ_RESULT_OPERATOR ),    RETURN_VALUE( SEQ_RESULT_REJECT ) ),
        { STEP_TYPE_DONE }
    }
};

    /*      Update Operator Shift - VIX specific business rule
     *
     *      SEQ_RESULT_REJECT               shift update failed
     *      SEQ_RESULT_OPERATOR             shift updated successfully
     */
Sequence_t  SEQ_OSC_UpdateShiftData =
{
    "SEQ_OSC_UpdateShiftData",
    {
        EXECUTE_RULE( BR_VIX_0_4,           GOTO_SEQUENCE( SEQ_OSC_0 ),             RETURN_VALUE( SEQ_RESULT_REJECT ) ), 
        {
            STEP_SEQUENCE( SEQ_OSC_0 ), 
            {
                { SEQ_RESULT_CONTINUE,                                              GOTO_RULE( BR_VIX_OSC_3_3 ) },
                { SEQ_RESULT_REJECT,                                                RETURN_VALUE( SEQ_RESULT_REJECT ) } 
            }
        },
        EXECUTE_RULE( BR_VIX_OSC_3_3,       RETURN_VALUE( SEQ_RESULT_OPERATOR ),    GOTO_RULE( BR_VIX_OSC_3_4 ) ), 
        EXECUTE_RULE( BR_VIX_OSC_3_4,       RETURN_VALUE( SEQ_RESULT_OPERATOR ),    RETURN_VALUE( SEQ_RESULT_REJECT ) ), 
        { STEP_TYPE_DONE }
    }
};

//  Not in NTS0177 - Chooses whether to run the LLSC or OSC sequences based on the card type

Sequence_t  SEQ_ProcessCard =
{
    "SEQ_ProcessCard - Process a card presentation",
    {
        EXECUTE_RULE( BR_VIX_0_3,           GOTO_SEQUENCE( SEQ_LLSC ),              GOTO_RULE( BR_VIX_0_4 ) ),                  // Is LLSC Card
        EXECUTE_RULE( BR_VIX_0_4,           GOTO_SEQUENCE( SEQ_OSC ),               RETURN_VALUE( SEQ_RESULT_ERROR ) ),         // Is OSC Card
        {
            STEP_SEQUENCE( SEQ_LLSC ),
            {
                { SEQ_RESULT_REJECT,                                                GOTO_SEQUENCE( SEQ_PrecommitCheckAndReject ) },
                { SEQ_RESULT_SCAN_ON,                                               GOTO_SEQUENCE( SEQ_PrecommitCheckAndScanOn ) },
                { SEQ_RESULT_SCAN_OFF,                                              GOTO_SEQUENCE( SEQ_PrecommitCheckAndScanOff ) }
            }
        },
        {
            STEP_SEQUENCE( SEQ_OSC ),
            {
                { SEQ_RESULT_DEFAULT,                                               RETURN_VALUE( SEQ_RESULT_PASSTHROUGH ) }    // Return result from subsequence
            }
        },
        {
            STEP_SEQUENCE( SEQ_PrecommitCheckAndReject ),
            {
                { SEQ_RESULT_DEFAULT,                                               RETURN_VALUE( SEQ_RESULT_PASSTHROUGH ) }
            }
        },
        {
            STEP_SEQUENCE( SEQ_PrecommitCheckAndScanOn ),
            {
                { SEQ_RESULT_DEFAULT,                                               RETURN_VALUE( SEQ_RESULT_PASSTHROUGH ) }
            }
        },
        {
            STEP_SEQUENCE( SEQ_PrecommitCheckAndScanOff ),
            {
                { SEQ_RESULT_DEFAULT,                                               RETURN_VALUE( SEQ_RESULT_PASSTHROUGH ) }
            }
        },
        { STEP_TYPE_DONE }
    }
};

    /*      Add Value - VIX specific business rule
     *
     *      SEQ_RESULT_REJECT                   add value failed
     *      SEQ_RESULT_SCAN_ON                  add value completed successfully
     *      SEQ_RESULT_PRECOMMIT_CHECK_FAILED   Failed pre-commit check
     */
Sequence_t  SEQ_AddValue =
{
    "SEQ_AddValue - TPurse Add Value",
    {
        EXECUTE_RULE( BR_VIX_0_3,           GOTO_SEQUENCE( SEQ_LLSC_1 ),            RETURN_VALUE( SEQ_RESULT_REJECT ) ),
        {
            STEP_SEQUENCE( SEQ_LLSC_1 ),
            {
                { SEQ_RESULT_MULTIPLE_CARDS,                                        RETURN_VALUE( SEQ_RESULT_REJECT ) },
                { SEQ_RESULT_REJECT,                                                RETURN_VALUE( SEQ_RESULT_REJECT ) },
                { SEQ_RESULT_CARD_INITIALISED,                                      GOTO_SEQUENCE( SEQ_LLSC_2 ) }
            }
        },
        {
            STEP_SEQUENCE( SEQ_LLSC_2 ),
            {
                { SEQ_RESULT_INVALID_TRANSIT_APPLICATION_STATUS,                    RETURN_VALUE( SEQ_RESULT_REJECT ) },
                { SEQ_RESULT_ACTIONLISTS_PROCESSED,                                 GOTO_RULE( BR_VIX_LLSC_20_1 ) }
            }
        },
        EXECUTE_RULE( BR_VIX_LLSC_20_1,     GOTO_RULE( BR_LLSC_99_1 ),              RETURN_VALUE( SEQ_RESULT_REJECT ) ),
        EXECUTE_RULE( BR_LLSC_99_1,         RETURN_VALUE( SEQ_RESULT_SCAN_ON ),     RETURN_VALUE( SEQ_RESULT_PRECOMMIT_CHECK_FAILED ) ),
        { STEP_TYPE_DONE }
    }
};  /*  SEQ_AddValue */

    /*      Add Value Reversal - VIX specific business rule
     *
     *      SEQ_RESULT_REJECT                   add value failed
     *      SEQ_RESULT_SCAN_ON                  add value completed successfully
     *      SEQ_RESULT_PRECOMMIT_CHECK_FAILED   Failed pre-commit check
     */
Sequence_t  SEQ_AddValueReversal =
{
    "SEQ_AddValueReversal - TPurse Add Value Reversal",
    {
        EXECUTE_RULE( BR_VIX_0_3,           GOTO_SEQUENCE( SEQ_LLSC_1 ),            RETURN_VALUE( SEQ_RESULT_REJECT ) ),
        {
            STEP_SEQUENCE( SEQ_LLSC_1 ),
            {
                { SEQ_RESULT_MULTIPLE_CARDS,                                        RETURN_VALUE( SEQ_RESULT_REJECT ) },
                { SEQ_RESULT_REJECT,                                                RETURN_VALUE( SEQ_RESULT_REJECT ) },
                { SEQ_RESULT_CARD_INITIALISED,                                      GOTO_SEQUENCE( SEQ_LLSC_2 ) }
            }
        },
        {
            STEP_SEQUENCE( SEQ_LLSC_2 ),
            {
                { SEQ_RESULT_INVALID_TRANSIT_APPLICATION_STATUS,                    RETURN_VALUE( SEQ_RESULT_REJECT ) },
                { SEQ_RESULT_ACTIONLISTS_PROCESSED,                                 GOTO_RULE( BR_VIX_LLSC_20_2 ) }
            }
        },
        EXECUTE_RULE( BR_VIX_LLSC_20_2,     GOTO_RULE( BR_LLSC_99_1 ),              RETURN_VALUE( SEQ_RESULT_REJECT ) ),
        EXECUTE_RULE( BR_LLSC_99_1,         RETURN_VALUE( SEQ_RESULT_SCAN_ON ),     RETURN_VALUE( SEQ_RESULT_PRECOMMIT_CHECK_FAILED ) ),
        { STEP_TYPE_DONE }
    }
};  /*  SEQ_AddValueReversal */

    /*      Debit TPurse Value - VIX specific business rule
     *
     *      SEQ_RESULT_REJECT                   debit TPurse value failed
     *      SEQ_RESULT_SCAN_ON                  debit TPurse value completed successfully
     *      SEQ_RESULT_PRECOMMIT_CHECK_FAILED   Failed pre-commit check
     */
Sequence_t  SEQ_DebitTPurseValue =
{
    "SEQ_DebitTPurseValue - Debit TPurse Value",
    {
        EXECUTE_RULE( BR_VIX_0_3,           GOTO_SEQUENCE( SEQ_LLSC_1 ),            RETURN_VALUE( SEQ_RESULT_REJECT ) ),
        {
            STEP_SEQUENCE( SEQ_LLSC_1 ),
            {
                { SEQ_RESULT_MULTIPLE_CARDS,                                        RETURN_VALUE( SEQ_RESULT_REJECT ) },
                { SEQ_RESULT_REJECT,                                                RETURN_VALUE( SEQ_RESULT_REJECT ) },
                { SEQ_RESULT_CARD_INITIALISED,                                      GOTO_SEQUENCE( SEQ_LLSC_2 ) }
            }
        },
        {
            STEP_SEQUENCE( SEQ_LLSC_2 ),
            {
                { SEQ_RESULT_INVALID_TRANSIT_APPLICATION_STATUS,                    RETURN_VALUE( SEQ_RESULT_REJECT ) },
                { SEQ_RESULT_ACTIONLISTS_PROCESSED,                                 GOTO_RULE( BR_VIX_LLSC_21_1 ) }
            }
        },
        EXECUTE_RULE( BR_VIX_LLSC_21_1,     GOTO_RULE( BR_LLSC_99_1 ),              RETURN_VALUE( SEQ_RESULT_REJECT ) ),
        EXECUTE_RULE( BR_LLSC_99_1,         RETURN_VALUE( SEQ_RESULT_SCAN_ON ),     RETURN_VALUE( SEQ_RESULT_PRECOMMIT_CHECK_FAILED ) ),
        { STEP_TYPE_DONE }
    }
};  /*  SEQ_DebitTPurseValue */

    /*      Debit TPurse Value - VIX specific business rule
     *
     *      SEQ_RESULT_REJECT                   debit TPurse value reversal failed
     *      SEQ_RESULT_SCAN_ON                  debit TPurse value reversal completed successfully
     *      SEQ_RESULT_PRECOMMIT_CHECK_FAILED   Failed pre-commit check
     */
Sequence_t  SEQ_DebitTPurseValueReversal =
{
    "SEQ_DebitTPurseValue - Debit TPurse Value Reversal",
    {
        EXECUTE_RULE( BR_VIX_0_3,           GOTO_SEQUENCE( SEQ_LLSC_1 ),            RETURN_VALUE( SEQ_RESULT_REJECT ) ),
        {
            STEP_SEQUENCE( SEQ_LLSC_1 ),
            {
                { SEQ_RESULT_MULTIPLE_CARDS,                                        RETURN_VALUE( SEQ_RESULT_REJECT ) },
                { SEQ_RESULT_REJECT,                                                RETURN_VALUE( SEQ_RESULT_REJECT ) },
                { SEQ_RESULT_CARD_INITIALISED,                                      GOTO_SEQUENCE( SEQ_LLSC_2 ) }
            }
        },
        {
            STEP_SEQUENCE( SEQ_LLSC_2 ),
            {
                { SEQ_RESULT_INVALID_TRANSIT_APPLICATION_STATUS,                    RETURN_VALUE( SEQ_RESULT_REJECT ) },
                { SEQ_RESULT_ACTIONLISTS_PROCESSED,                                 GOTO_RULE( BR_VIX_LLSC_21_2 ) }
            }
        },
        EXECUTE_RULE( BR_VIX_LLSC_21_2,     GOTO_RULE( BR_LLSC_99_1 ),              RETURN_VALUE( SEQ_RESULT_REJECT ) ),
        EXECUTE_RULE( BR_LLSC_99_1,         RETURN_VALUE( SEQ_RESULT_SCAN_ON ),     RETURN_VALUE( SEQ_RESULT_PRECOMMIT_CHECK_FAILED ) ),
        { STEP_TYPE_DONE }
    }
};  /*  SEQ_DebitTPurseValueReversal */

//=============================================================================
//
//  Find a step within a sequence
//
//=============================================================================

static  int     findStep( Sequence_t *pSequence, StepType_e stepType, Sequence_t *pStepSequence, MYKI_BR_RuleFunc_t *pStepRuleFunc )
{
    int     i;

    for ( i = 0; i < DIMOF( pSequence->StepList ) && pSequence->StepList[ i ].Type != STEP_TYPE_DONE; i++ )
    {
//        CsDbg( BRLL_RULE, "Checking step %d (Type = %d, ID = %d, Name = %s)", i, pSequence->StepList[ i ].Type, pSequence->StepList[ i ].Id, pSequence->StepList[ i ].IdDesc );

        if
        (
            ( pSequence->StepList[ i ].Type == stepType ) &&
            (
                ( stepType == STEP_TYPE_SEQ  && pSequence->StepList[ i ].pSequence == pStepSequence ) ||
                ( stepType == STEP_TYPE_RULE && pSequence->StepList[ i ].pRuleFunc == pStepRuleFunc )
            )
        )
        {
//            CsDbg( BRLL_RULE, "Sequence/Rule found at index %d", i );
            return i;
        }
    }

    return -1;
}

//=============================================================================
//
//  The desc[] array must EXACTLY match the values in SequenceResult_e in myki_br.h.
//
//=============================================================================

char    *MYKI_BR_SequenceResultDesc( SequenceResult_e result )
{
    static  char    *desc[] =
    {
        "SEQ_RESULT_DEFAULT",
        "SEQ_RESULT_INTERNAL_ERROR",
        "SEQ_RESULT_EXECUTED",
        "SEQ_RESULT_BYPASSED",
        "SEQ_RESULT_ERROR",
        "SEQ_RESULT_CONTINUE",
        "SEQ_RESULT_REJECT",
        "SEQ_RESULT_SCAN_ON",
        "SEQ_RESULT_SCAN_OFF",
        "SEQ_RESULT_OPERATOR",
        "SEQ_RESULT_CARD_INITIALISED",
        "SEQ_RESULT_MULTIPLE_CARDS",
        "SEQ_RESULT_ACTIONLISTS_PROCESSED",
        "SEQ_RESULT_INVALID_TRANSIT_APPLICATION_STATUS",
        "SEQ_RESULT_AUTOLOAD_PROCESSED",
        "SEQ_RESULT_CHANGE_OF_MIND",
        "SEQ_RESULT_SCAN_OFF_REQUIRED",
        "SEQ_RESULT_FORCED_SCAN_OFF",
        "SEQ_RESULT_NO_CHANGE_OF_MIND",
        "SEQ_RESULT_NO_MISSING_SCAN_OFF",
        "SEQ_RESULT_UNABLE_TO_SCAN_OFF",
        "SEQ_RESULT_TPURSE_BALANCE_BELOW_MINIMUM",
        "SEQ_RESULT_STATION_EXIT_FEE_CHARGED",
        "SEQ_RESULT_MISSING_SCAN_ON",
        "SEQ_RESULT_SCAN_ON_PRESENT",
        "SEQ_RESULT_PRODUCT_SCANNED_ON",
        "SEQ_RESULT_RESCAN_ON_COMPLETE",
        "SEQ_RESULT_NO_SCANNED_ON_PRODUCT",
        "SEQ_RESULT_SCAN_OFF_PERFORMED",
        "SEQ_RESULT_SCAN_OFF_WITH_PARTIAL_COVERAGE",
        "SEQ_RESULT_FORCED_SCAN_OFF_PENALTY",
        "SEQ_RESULT_CAPPING_PROCESSED",
        "SEQ_RESULT_MERGE_PRODUCTS",
        "SEQ_RESULT_SCAN_OFF_FINALIZED",
        "SEQ_RESULT_DUPLICATE_SCAN_ON",
        "SEQ_RESULT_NO_DUPLICATE_SCAN_ON",
        "SEQ_RESULT_BLOCKING_PERIOD_APPLIES",
        "SEQ_RESULT_BLOCKING_PERIOD_EXPIRED",
        "SEQ_RESULT_TRAVEL_ACCESS_PASS_IN_USE",
        "SEQ_RESULT_ZONE_RANGE_DETERMINED",
        "SEQ_RESULT_NO_SCANNED_ON_PRODUCTS",
        "SEQ_RESULT_SCAN_ON_TRAVEL_ACCESS_PASS",
        "SEQ_RESULT_SCAN_OFF_TRAVEL_ACCESS_PASS",
        "SEQ_RESULT_PASSTHROUGH",
        "SEQ_RESULT_MAX"
    };

    if ( result >= 0 && result < DIMOF( desc ) )
        return desc[ result ];

    return "UKNOWN SEQUENCE RESULT";
}

//=============================================================================
//
//  Run a sequence
//
//=============================================================================

SequenceResult_e    MYKI_BR_RunSequence( Sequence_t *pSequence, MYKI_BR_ContextData_t *pSequenceData )
{
    int                 i;
    Step_t              *pStep;
    ResultAction_t      *pAction;
    int                 stepsRun;
    SequenceResult_e    stepResult = SEQ_RESULT_INTERNAL_ERROR;
    int                 currentStep;
    int                 newStep;

//    CsDbg( BRLL_RULE, "Sequence %s", pSequence->IdDesc );

    currentStep = 0;                        // All sequences start with the first step
    for ( stepsRun = 0; stepsRun < MAX_STEPS_BEFORE_LOOP_ASSUMED; stepsRun++ )
    {
        pStep = &pSequence->StepList[ currentStep ];

//        CsDbg( BRLL_RULE, "%s: Step index %d (%s)", pSequence->IdDesc, currentStep, pStep->IdDesc );

        switch ( pStep->Type )
        {
            case STEP_TYPE_SEQ :
                CsDbg( BRLL_RULE, "%s: Running %s", pSequence->IdDesc, pStep->IdDesc );
                stepResult = MYKI_BR_RunSequence( pStep->pSequence, pSequenceData );

                if ( stepResult == SEQ_RESULT_ERROR )   // Special case, not covered by NTS-0177 - abort entire BR process
                {                                       // Handled explicitly to avoid SEQ_RESULT_ERROR being turned into SEQ_RESULT_DEFAULT
                    CsErrx( "%s: %s returned error - aborting sequence", pSequence->IdDesc, pStep->IdDesc );
                    return SEQ_RESULT_ERROR;
                }

                if ( stepResult == SEQ_RESULT_INTERNAL_ERROR )  // Special case, not covered by NTS-0177 - abort entire BR process
                {                                               // Handled explicitly to avoid SEQ_RESULT_INTERNAL_ERROR being turned into SEQ_RESULT_DEFAULT
                    CsErrx( "%s: %s returned internal error - aborting sequence", pSequence->IdDesc, pStep->IdDesc );
                    return SEQ_RESULT_INTERNAL_ERROR;
                }

                CsDbg( BRLL_RULE, "%s: %s returned %s", pSequence->IdDesc, pStep->IdDesc, MYKI_BR_SequenceResultDesc( stepResult ) );
                break;

            case STEP_TYPE_RULE :
                CsDbg( BRLL_RULE, "%s: Running %s", pSequence->IdDesc, pStep->IdDesc );
                switch ( pStep->pRuleFunc( pSequenceData ) )
                {
                    case RULE_RESULT_EXECUTED :
                        stepResult = SEQ_RESULT_EXECUTED;
                        break;
                    case RULE_RESULT_BYPASSED :
                        stepResult = SEQ_RESULT_BYPASSED;
                        break;
                    case RULE_RESULT_ERROR :            // Special case, not covered by NTS-0177 - abort entire BR process
                        CsErrx( "%s: %s returned error - aborting sequence", pSequence->IdDesc, pStep->IdDesc );
                        return SEQ_RESULT_ERROR;
                    default :
                        CsErrx( "%s: %s returned invalid value", pSequence->IdDesc, pStep->IdDesc );
                        return SEQ_RESULT_INTERNAL_ERROR;
                }
                CsDbg( BRLL_RULE, "%s: %s returned %s", pSequence->IdDesc, pStep->IdDesc, MYKI_BR_SequenceResultDesc( stepResult ) );
                break;

            case STEP_TYPE_DONE :
                CsErrx( "%s: STEP_TYPE_DONE: Should never get here", pSequence->IdDesc );   // This is the end-of-list marker - it should never actually be executed
                return SEQ_RESULT_INTERNAL_ERROR;

            default :
                CsErrx( "%s: Invalid Step Type (%d)", pSequence->IdDesc, pStep->Type );
                return SEQ_RESULT_INTERNAL_ERROR;
        }

        newStep = -1;
        for ( i = 0; i < DIMOF( pStep->ResultAction ); i++ )
        {
            pAction = &pStep->ResultAction[ i ];
            if ( ( pAction->ResultValue == SEQ_RESULT_DEFAULT ) || ( pAction->ResultValue == stepResult ) )
            {
//                CsDbg
//                (
//                    BRLL_RULE,
//                    "%s: Found matching action at index %d (result %d, action %s)",
//                    pSequence->IdDesc, i, pAction->ResultValue, pAction->Desc
//                );

                switch ( pAction->ActionType )
                {
                    case ACTION_TYPE_RETURN :
                        if ( pAction->ReturnResult == SEQ_RESULT_PASSTHROUGH )
                        {
                            CsDbg( BRLL_RULE, "%s: Step %d (%s) Returning %s from subsequence", pSequence->IdDesc, currentStep, pStep->IdDesc, MYKI_BR_SequenceResultDesc( stepResult ) );
                            return stepResult;
                        }

                        CsDbg( BRLL_RULE, "%s: Step %d (%s) Returning %s", pSequence->IdDesc, currentStep, pStep->IdDesc, MYKI_BR_SequenceResultDesc( pAction->ReturnResult ) );    // Change last argument to pAction->Desc?
                        return pAction->ReturnResult;

                    case ACTION_TYPE_GOTO_SEQ :
                        newStep = findStep( pSequence, STEP_TYPE_SEQ, pAction->pSequence, NULL );
                        if ( newStep < 0 )
                        {
                            CsErrx( "%s: %s not found", pSequence->IdDesc, pAction->Desc );
                            return SEQ_RESULT_INTERNAL_ERROR;
                        }
                        break;

                    case ACTION_TYPE_GOTO_RULE :
                        newStep = findStep( pSequence, STEP_TYPE_RULE, NULL, pAction->pRuleFunc );
                        if ( newStep < 0 )
                        {
                            CsErrx( "%s: %s not found", pSequence->IdDesc, pAction->Desc );
                            return SEQ_RESULT_INTERNAL_ERROR;
                        }
                        break;

                    default :
                        CsErrx( "%s: Invalid Action Type (%d)", pSequence->IdDesc, pAction->ActionType );
                        return SEQ_RESULT_INTERNAL_ERROR;
                }

                break;          // Found and processed a matching action, exit the action search
            }
        }

        if ( i >= DIMOF( pStep->ResultAction ) )
        {
            CsErrx( "%s: No action found to match result %s, and no default action found", pSequence->IdDesc, MYKI_BR_SequenceResultDesc( stepResult ) );
            return SEQ_RESULT_INTERNAL_ERROR;
        }

        currentStep = newStep;
    }

    CsErrx( "%s: More than %d steps executed, assuming an infinite loop", pSequence->IdDesc, MAX_STEPS_BEFORE_LOOP_ASSUMED );
    return SEQ_RESULT_INTERNAL_ERROR;
}

//=============================================================================
//
//  Print a sequence
//
//=============================================================================

//#define SHOW_ALL

#define INDENT  2

enum
{
    ERR_SEQUENCE_NOT_FOUND,
    ERR_RULE_NOT_FOUND,
    ERR_INVALID_ACTION_TYPE,
    ERR_NO_DEFAULT_ACTION,
    ERR_COUNT                   // Must be last
};

static  char    *ErrorsDesc[] =
{
    "ERR_SEQUENCE_NOT_FOUND     ",
    "ERR_RULE_NOT_FOUND         ",
    "ERR_INVALID_ACTION_TYPE    ",
    "ERR_NO_DEFAULT_ACTION      "
};

static  int     Errors[ ERR_COUNT ];

//=============================================================================
//
//
//
//=============================================================================

int     PrintStepPaths( Sequence_t *pSequence, Step_t *pStep, char *prefix, int level )
{
    int                 i;
    ResultAction_t      *pAction;
    int                 newStep;

#ifdef SHOW_ALL
    CsDbg( BRLL_CHOICE, "%*s%s", level * INDENT, "", pStep->IdDesc );
#endif

    switch ( pStep->Type )
    {
        case STEP_TYPE_SEQ :
            MYKI_BR_PrintSequencePaths( pStep->pSequence, level + 1 );
            break;
#ifndef SHOW_ALL
        default :
            CsDbg( BRLL_CHOICE, "%*s%s ==> %s", level * INDENT, "", prefix, pStep->IdDesc );
            break;
#endif
    }

    for ( i = 0; i < DIMOF( pStep->ResultAction ); i++ )
    {
        pAction = &pStep->ResultAction[ i ];
        if ( pAction->ResultValue == SEQ_RESULT_DEFAULT )
            break;

#ifdef SHOW_ALL
        CsDbg( BRLL_CHOICE, "%*sAction %d : %s ==> %s", ( level + 1 ) * INDENT, "", i, MYKI_BR_SequenceResultDesc( pAction->ResultValue ), pAction->Desc );
#endif

        switch ( pAction->ActionType )
        {
            case ACTION_TYPE_RETURN :
//                CsDbg( BRLL_CHOICE, "%*sRETURN", ( level + 2 ) * INDENT, "" );
                break;

            case ACTION_TYPE_GOTO_SEQ :
                newStep = findStep( pSequence, STEP_TYPE_SEQ, pAction->pSequence, NULL );
                if ( newStep < 0 )
                {
                    CsErrx( "%s: %s not found", pSequence->IdDesc, pAction->Desc );
                    Errors[ ERR_SEQUENCE_NOT_FOUND ]++;
                    return SEQ_RESULT_INTERNAL_ERROR;
                }
                PrintStepPaths( pSequence, &pSequence->StepList[ newStep ], MYKI_BR_SequenceResultDesc( pAction->ResultValue ), level + 2 );
                break;

            case ACTION_TYPE_GOTO_RULE :
                newStep = findStep( pSequence, STEP_TYPE_RULE, NULL, pAction->pRuleFunc );
                if ( newStep < 0 )
                {
                    CsErrx( "%s: %s not found", pSequence->IdDesc, pAction->Desc );
                    Errors[ ERR_RULE_NOT_FOUND ]++;
                    return SEQ_RESULT_INTERNAL_ERROR;
                }
                PrintStepPaths( pSequence, &pSequence->StepList[ newStep ], MYKI_BR_SequenceResultDesc( pAction->ResultValue ), level + 2 );
                break;

            default :
                CsErrx( "%s: Invalid Action Type (%d)", pSequence->IdDesc, pAction->ActionType );
                Errors[ ERR_INVALID_ACTION_TYPE ]++;
                return SEQ_RESULT_INTERNAL_ERROR;
        }
    }

    if ( i >= DIMOF( pStep->ResultAction ) )
    {
        CsErrx( "%s: No default action found", pSequence->IdDesc );
        Errors[ ERR_NO_DEFAULT_ACTION ]++;
        return SEQ_RESULT_INTERNAL_ERROR;
    }

    return 0;
}

//=============================================================================
//
//
//
//=============================================================================

int     MYKI_BR_PrintSequencePaths( Sequence_t *pSequence, int level )
{
#ifdef SHOW_ALL
    CsDbg( BRLL_SESSION, "%*sSequence : %s", level * INDENT, "", pSequence->IdDesc );
#endif

    PrintStepPaths( pSequence, &pSequence->StepList[ 0 ], "Entry", level + 1 );

    return TRUE;
}

//=============================================================================
//
//
//
//=============================================================================

int     MYKI_BR_PrintSequencePathsTest()
{
    int     i;

    MYKI_BR_PrintSequencePaths( &SEQ_ProcessCard, 0 );
//    MYKI_BR_PrintSequencePaths( &SEQ_OSC, 0 );

    CsDbg( BRLL_RULE, "Errors :" );
    for ( i = 0; i < ERR_COUNT; i++ )
    {
        CsDbg( BRLL_RULE, "  %s = %d", ErrorsDesc[ i ], Errors[ i ] );
    }

    return 0;
}

//=============================================================================
//
//
//
//=============================================================================

int     MYKI_BR_PrintSequence( Sequence_t *pSequence, int level )
{
    int                 i;
    Step_t              *pStep;
    ResultAction_t      *pAction;
    int                 stepsRun;
    int                 currentStep;

    CsDbg( BRLL_SESSION, "%*s%s", level * 2, "", pSequence->IdDesc );

    for ( stepsRun = 0; stepsRun < MAX_STEPS_BEFORE_LOOP_ASSUMED; stepsRun++ )
    {
        currentStep = stepsRun;
        pStep = &pSequence->StepList[ currentStep ];

        if ( pStep->IdDesc )
            CsDbg( BRLL_RULE, "%*s- Step %d (%s)", level * 2, "", currentStep, pStep->IdDesc );

        switch ( pStep->Type )
        {
            case STEP_TYPE_SEQ :
                MYKI_BR_PrintSequence( pStep->pSequence, level + 1 );
                break;

            case STEP_TYPE_RULE :
                //  Search for either a matching function pointer, a blank slot, or the end of the list
                for ( i = 0; i < DIMOF( foundRuleList ) && foundRuleList[ i ].func && foundRuleList[ i ].func != pStep->pRuleFunc; i++ )
                    ;

                //  If we found either a match or a blank slot, then update the slot
                if ( i < DIMOF( foundRuleList ) )
                {
                    foundRuleList[ i ].func = pStep->pRuleFunc;
                    foundRuleList[ i ].name = pStep->IdDesc;
                }
                break;

            case STEP_TYPE_DONE :
                return TRUE;

            default :
                CsErrx( "%s: Invalid Step Type (%d)", pSequence->IdDesc, pStep->Type );
                return FALSE;
        }

        for ( i = 0; i < DIMOF( pStep->ResultAction ); i++ )
        {
            pAction = &pStep->ResultAction[ i ];
            if ( pAction->ResultValue == SEQ_RESULT_DEFAULT )
                break;

            CsDbg( BRLL_CHOICE, "%*s  - Action %d : %s ==> %s", level * 2, "", i, MYKI_BR_SequenceResultDesc( pAction->ResultValue ), pAction->Desc );
        }

        if ( i >= DIMOF( pStep->ResultAction ) )
        {
            CsErrx( "%s: No default action found", pSequence->IdDesc );
            return FALSE;
        }
    }

    return TRUE;
}

//=============================================================================
//
//  Print a list of all the rules called by a sequence
//
//=============================================================================

int     MYKI_BR_PrintSequenceRules( Sequence_t *pSequence )
{
    int                 i;

    memset( foundRuleList, 0, sizeof( foundRuleList ) );

    MYKI_BR_PrintSequence( pSequence, 0 );

    for ( i = 0; i < DIMOF( foundRuleList ) && foundRuleList[ i ].func; i++ )
    {
        CsDbg( BRLL_SESSION, "Rule %s", foundRuleList[ i ].name );
    }

    return TRUE;
}

//=============================================================================
//
//  Initialise the business rules engine
//
//=============================================================================

int     MYKI_BR_Initialise( int initialStationEntryPointId )
{
    #if     __TOBEDELETED__
    MYKI_CD_Stations_t       station;
    MYKI_CD_Locations_t      location;
    MYKI_CD_U16Array_t       lineList;
    MYKI_CD_LinesByStation_t linesByStation;
    #endif

    if ( initialStationEntryPointId < 0 || initialStationEntryPointId > 65535 )
    {
        CsErrx( "MYKI_BR_Initialise : Invalid EntryPointID(%d), must be between 0 and 65535", initialStationEntryPointId );
        return -1;
    }

    /*  NOTE:   BR context data will be initialised in MYKI_BR_InitialiseContext() function */
    #if     __TOBEDELETED__
    if ( MYKI_CD_getStationsStructure( (U16_t)initialStationEntryPointId, &station ) )
    {
        currentStopProvisionalZoneLow       = station.provisional_zone_low;
        currentStopProvisionalZoneHigh      = station.provisional_zone_high;

        if ( MYKI_CD_getLocationsStructure( station.location, &location ) )
        {
            currentStopCurrentZone          = location.zone;
            currentStopCurrentInnerZone     = location.inner_zone;
            currentStopCurrentOuterZone     = location.outer_zone;
        }

        if ( MYKI_CD_getLinesByStationStructure( (U16_t)initialStationEntryPointId, &linesByStation, &lineList ) &&
            lineList.arraySize > 0 )
        {
            currentStopLineId               = lineList.arrayOfU16[0]; // if multiple lines -> pick the first one
        }

        currentEntryPointId                 = initialStationEntryPointId;
    }
    else
    {
        CsWarnx( "MYKI_BR_Initialise : getStationsStructure(%d) failed, could be BUS/TRAM vehicle id", initialStationEntryPointId );
    }
    #endif

    return 0;
}

//=============================================================================
//
//  Finalise the business rules engine
//
//=============================================================================

int     MYKI_BR_Finalise()
{
    return 0;
}


//=============================================================================
//
//  Initialise internal BR context prior to running a single pass through the business rules
//
//=============================================================================

int     MYKI_BR_InitialiseContext( MYKI_BR_ContextData_t *pData )
{
    MYKI_CD_Stations_t              station;
    MYKI_CD_Locations_t             location;
    MYKI_CD_U16Array_t              lineList;
    MYKI_CD_LinesByStation_t        linesByStation;

    //
    //  Show our initial context details in the debug log.
    //  At the moment, it's not practical to log the entire context here, so
    //  we just log the bits we've found useful so far. Feel free to add to this list as needed.
    //

    CsDbg( BRLL_FIELD, "MYKI_BR_InitialiseContext: Service Provider ID  = %d", pData->StaticData.serviceProviderId  );
    CsDbg( BRLL_FIELD, "MYKI_BR_InitialiseContext: Entry Point ID       = %d", pData->DynamicData.entryPointId      );
    CsDbg( BRLL_FIELD, "MYKI_BR_InitialiseContext: Line ID              = %d", pData->DynamicData.lineId            );
    CsDbg( BRLL_FIELD, "MYKI_BR_InitialiseContext: Stop ID              = %d", pData->DynamicData.stopId            );
    CsDbg( BRLL_FIELD, "MYKI_BR_InitialiseContext: Current Date/Time    = %d", pData->DynamicData.currentDateTime   );

    //
    //  Sanity checks on context data so we can safely cast members from 'int' etc to 'U8_t' etc when calling CD, LDT and CS APIs.
    //  Currently this is a single if statement to save time, but it should really be multiple statements so we can provide
    //  meaningful errors.
    //

    if
    (
        ( pData->DynamicData.entryPointId < 0 )         ||
        ( pData->DynamicData.entryPointId > 0xFFFF )
    )
    {
        CsErrx( "Bad context data!" );
        return -1;
    }

    //
    //  Cache some CD results that won't change during a single BR pass, to speed things up
    //

    pData->InternalData.TransportMode = myki_br_cd_GetTransportModeForProvider( pData->StaticData.serviceProviderId );

    CsDbg( BRLL_FIELD, "MYKI_BR_InitialiseContext: Transport Mode       = %s", myki_br_GetTransportModeString( pData->InternalData.TransportMode ) );

    if ( currentServiceProviderId != pData->StaticData.serviceProviderId )
    {
        /*  ONLY during testing! Forced updating BR context data */
        currentServiceProviderId            = pData->StaticData.serviceProviderId;
        currentEntryPointId                 = -1;
        currentStopId                       = -1;
        currentStopLineId                   = -1;
        currentStopProvisionalZoneLow       = 0;
        currentStopProvisionalZoneHigh      = 0;
        currentStopCurrentZone              = 0;
        currentStopCurrentInnerZone         = 0;
        currentStopCurrentOuterZone         = 0;
    }

    if ( pData->InternalData.TransportMode == TRANSPORT_MODE_RAIL )
    {
        // Check if we have done this before
        if ( pData->DynamicData.entryPointId != currentEntryPointId )
        {
            if ( MYKI_CD_getStationsStructure( (U16_t)pData->DynamicData.entryPointId, &station ) )
            {
                pData->DynamicData.stopId               = 0;    // Not applicable to rail, must be 0
                pData->DynamicData.provisionalZoneLow   = station.provisional_zone_low;
                pData->DynamicData.provisionalZoneHigh  = station.provisional_zone_high;

                if ( MYKI_CD_getLocationsStructure( station.location, &location ) )
                {
                    pData->DynamicData.currentZone      = (int)location.zone;
                    pData->DynamicData.currentInnerZone = (int)location.inner_zone;
                    pData->DynamicData.currentOuterZone = (int)location.outer_zone;
                }
                else
                {
                    CsErrx( "InitialiseContext : getStationsStructure(%d) failed", pData->DynamicData.entryPointId );
                    return -1;
                }

                if ( MYKI_CD_getLinesByStationStructure( (U16_t)pData->DynamicData.entryPointId, &linesByStation, &lineList ) &&
                    lineList.arraySize > 0 )
                {
                    pData->DynamicData.lineId           = lineList.arrayOfU16[0]; // if multiple lines -> pick the first one
                }

                currentEntryPointId                     = pData->DynamicData.entryPointId;
                currentStopId                           = pData->DynamicData.stopId;
                currentStopLineId                       = pData->DynamicData.lineId;
                currentStopProvisionalZoneLow           = pData->DynamicData.provisionalZoneLow;
                currentStopProvisionalZoneHigh          = pData->DynamicData.provisionalZoneHigh;
                currentStopCurrentZone                  = (U8_t)pData->DynamicData.currentZone;
                currentStopCurrentInnerZone             = (U8_t)pData->DynamicData.currentInnerZone;
                currentStopCurrentOuterZone             = (U8_t)pData->DynamicData.currentOuterZone;
            }
            else
            {
                CsErrx( "InitialiseContext : getStationsStructure(%d) failed", pData->DynamicData.entryPointId );
                return -1;
            }
        }
        else
        {
            // Return cached data
            pData->DynamicData.stopId               = 0;    // Not applicable to rail, must be 0
            pData->DynamicData.lineId               = currentStopLineId;
            pData->DynamicData.provisionalZoneLow   = currentStopProvisionalZoneLow;
            pData->DynamicData.provisionalZoneHigh  = currentStopProvisionalZoneHigh;
            pData->DynamicData.currentZone          = (int)currentStopCurrentZone;
            pData->DynamicData.currentInnerZone     = (int)currentStopCurrentInnerZone;
            pData->DynamicData.currentOuterZone     = (int)currentStopCurrentOuterZone;
        }
    }
    else if ( pData->InternalData.TransportMode == TRANSPORT_MODE_BUS  || 
              pData->InternalData.TransportMode == TRANSPORT_MODE_TRAM )
    {
        if ( pData->DynamicData.entryPointId != currentEntryPointId )
        {
            currentEntryPointId   = pData->DynamicData.entryPointId;
        }
        if ( pData->DynamicData.lineId != currentStopLineId ||
             pData->DynamicData.stopId != currentStopId     )
        {
            MYKI_CD_RouteStop_t    *pMYKI_CD_RouteStop  = NULL;

            if ( MYKI_CD_getRouteStopsStructure(
                    pData->DynamicData.lineId, pData->DynamicData.stopId, pData->DynamicData.stopId, &pMYKI_CD_RouteStop ) > 0 &&
                 pMYKI_CD_RouteStop != NULL )
            {
                if ( MYKI_CD_getLocationsStructure( pMYKI_CD_RouteStop->locationId, &location ) != FALSE )
                {
                    pData->DynamicData.provisionalZoneLow   = pMYKI_CD_RouteStop->provisionalZoneLow;
                    pData->DynamicData.provisionalZoneHigh  = pMYKI_CD_RouteStop->provisionalZoneHigh;
                    pData->DynamicData.currentZone          = (int)location.zone;
                    pData->DynamicData.currentInnerZone     = (int)location.inner_zone;
                    pData->DynamicData.currentOuterZone     = (int)location.outer_zone;

                    currentStopId                         = pData->DynamicData.stopId;
                    currentStopLineId                     = pData->DynamicData.lineId;
                    currentStopProvisionalZoneLow         = pData->DynamicData.provisionalZoneLow;
                    currentStopProvisionalZoneHigh        = pData->DynamicData.provisionalZoneHigh;
                    currentStopCurrentZone                = (U8_t)pData->DynamicData.currentZone;
                    currentStopCurrentInnerZone           = (U8_t)pData->DynamicData.currentInnerZone;
                    currentStopCurrentOuterZone           = (U8_t)pData->DynamicData.currentOuterZone;
                }
                else
                {
                    CsErrx( "InitialiseContext : MYKI_CD_getLocationsStructure(%d) failed at (%d,%d)",
                        pMYKI_CD_RouteStop->locationId, pData->DynamicData.lineId, pData->DynamicData.stopId );
                    return -1;
                }
            }
            else
            {
                CsErrx( "InitialiseContext : MYKI_CD_getRouteStopsStructure(%d,%d) failed",
                    pData->DynamicData.lineId, pData->DynamicData.stopId );
                return -1;
            }
        }
        else
        {
            pData->DynamicData.stopId               = currentStopId;
            pData->DynamicData.lineId               = currentStopLineId;
            pData->DynamicData.provisionalZoneLow   = currentStopProvisionalZoneLow;
            pData->DynamicData.provisionalZoneHigh  = currentStopProvisionalZoneHigh;
            pData->DynamicData.currentZone          = (int)currentStopCurrentZone;
            pData->DynamicData.currentInnerZone     = (int)currentStopCurrentInnerZone;
            pData->DynamicData.currentOuterZone     = (int)currentStopCurrentOuterZone;
        }
    }
    else
    {
        CsErrx( "InitialiseContext : transport mode is unknown: %d", pData->InternalData.TransportMode );
        return -1;
    }

    memset( &pData->DynamicData.originatingInformation, 0, sizeof( OriginatingInformation_t ) );
    pData->DynamicData.isOriginatingInformationSet      = FALSE;
    pData->DynamicData.isAOCard                         = FALSE;
    
    return 0;
}

//=============================================================================
//
//  Initialise the LDT context prior to running a single pass through the business rules
//
//=============================================================================

void    MYKI_BR_InitialiseLdtContext( MYKI_BR_ContextData_t *pData )
{
    // Transaction Data
    memset( &pData->InternalData.TransactionData, 0, sizeof( pData->InternalData.TransactionData ) );
    pData->InternalData.TransactionData.acquirerId      = pData->StaticData.deviceAquirerId;
    pData->InternalData.TransactionData.deviceId        = pData->StaticData.deviceId;
    pData->InternalData.TransactionData.deviceMinorId   = pData->StaticData.deviceMinorId;
    pData->InternalData.TransactionData.deviceSamId     = pData->StaticData.deviceSamId;
    pData->InternalData.TransactionData.currentDateTime = pData->DynamicData.currentDateTime;

    // Usage Log Data
    memset( &pData->InternalData.UsageLogData, 0, sizeof ( pData->InternalData.UsageLogData ) );
    pData->InternalData.UsageLogData.version                = 1;
    pData->InternalData.UsageLogData.transactionType        = MYKI_BR_TRANSACTION_TYPE_NONE;
    pData->InternalData.UsageLogData.providerId             = pData->StaticData.serviceProviderId;
    pData->InternalData.UsageLogData.transactionDateTime    = pData->DynamicData.currentDateTime;
    pData->InternalData.UsageLogData.entryPointId           = pData->DynamicData.entryPointId;
    pData->InternalData.UsageLogData.routeId                = pData->DynamicData.lineId;
    pData->InternalData.UsageLogData.stopId                 = pData->DynamicData.stopId;
    pData->InternalData.IsUsageLogUpdated                   = FALSE;

    // Load Log Data
    memset( &pData->InternalData.LoadLogData, 0, sizeof( pData->InternalData.LoadLogData ) );
    pData->InternalData.LoadLogData.version                 = 1;
    pData->InternalData.LoadLogData.transactionType         = MYKI_BR_TRANSACTION_TYPE_NONE;
    pData->InternalData.LoadLogData.providerId              = pData->StaticData.serviceProviderId;
    pData->InternalData.LoadLogData.transactionDateTime     = pData->DynamicData.currentDateTime;
    pData->InternalData.LoadLogData.entryPointId            = pData->DynamicData.entryPointId;
    pData->InternalData.LoadLogData.routeId                 = pData->DynamicData.lineId;
    pData->InternalData.LoadLogData.stopId                  = pData->DynamicData.stopId;
    pData->InternalData.IsLoadLogUpdated                    = FALSE;

    // Shift Log Data
    memset( &pData->InternalData.ShiftDataSummaryLog, 0, sizeof ( pData->InternalData.ShiftDataSummaryLog ) );
    pData->InternalData.IsShiftDataLogUpdated               =  FALSE;    
}

//=============================================================================
//
//  Run a single pass through the business rules
//
//=============================================================================

int     MYKI_BR_Execute( MYKI_BR_ContextData_t *pData )
{
    return  MYKI_BR_ExecuteSequence( pData, SEQ_FUNCTION_DEFAULT );
}

int     MYKI_BR_ExecuteSequence( MYKI_BR_ContextData_t *pData, SequenceFunction_e sequenceFunction )
{
    Sequence_t              *pSequence              = NULL;
    SequenceResult_e        result;
    MYKI_TAControl_t        *pMYKI_TAControl        = NULL;
    MYKI_OAControl_t        *pMYKI_OAControl        = NULL;
    MYKI_OAPIN_t            *pMYKI_OAPIN            = NULL;
    MYKI_OARoles_t          *pMYKI_OARoles          = NULL;
    MYKI_TAProduct_t        *pProductInUse          = NULL;
    MYKI_Directory_t        *pDirectoryInUse        = NULL;
    MYKI_TAPurseBalance_t   *pPurseBalance          = NULL;
    int                     i;

    CsDbg
    (
        BRLL_SESSION,
        "MYKI_BR_Execute() Build version %s, ACS Compatiblity Mode = %s",
        VERSION_VERSION,
        TF( pData->StaticData.AcsCompatibilityMode )
    );

    //  Selects sequence to execute

    switch ( sequenceFunction )
    {
    case    SEQ_FUNCTION_DEFAULT:                       pSequence   = &SEQ_ProcessCard;                 break;
    case    SEQ_FUNCTION_VALIDATE_PIN:                  pSequence   = &SEQ_OSC_ValidatePin;             break;
    case    SEQ_FUNCTION_UPDATE_PIN:                    pSequence   = &SEQ_OSC_UpdatePin;               break;
    case    SEQ_FUNCTION_UPDATE_SHIFT_DATA:             pSequence   = &SEQ_OSC_UpdateShiftData;         break;
    case    SEQ_FUNCTION_ADD_VALUE:                     pSequence   = &SEQ_AddValue;                    break;
    case    SEQ_FUNCTION_ADD_VALUE_REVERSAL:            pSequence   = &SEQ_AddValueReversal;            break;
    case    SEQ_FUNCTION_DEBIT_TPURSE_VALUE:            pSequence   = &SEQ_DebitTPurseValue;            break;
    case    SEQ_FUNCTION_DEBIT_TPURSE_VALUE_REVERSAL:   pSequence   = &SEQ_DebitTPurseValueReversal;    break;
    default:
        /*  Others! */
        CsDbg( BRLL_SESSION, "MYKI_BR_Execute : Invalid sequence (%d), returning MYKI_BR_RESULT_ERROR", (int)sequenceFunction );
        return  MYKI_BR_RESULT_ERROR;
    }

    //  Set up any internal variables to default values

    MYKI_BR_InitialiseContext( pData );
    MYKI_BR_InitialiseLdtContext( pData );

    pData->DynamicData.routeChangeover              = ROUTE_CHANGEOVER_NONE ;   // No route changeover condition exists
    pData->DynamicData.isEarlyBirdTrip              = FALSE                 ;   // This passenger trip is not an early bird trip
    pData->DynamicData.transportLocationsCount      = 0                     ;   // No locations recorded on this trip so far
    pData->DynamicData.commonDateTime               = TIME_NOT_SET          ;   // CommonDate not set by rules yet
    pData->DynamicData.loadLogTxValue               = 0                     ;   // TAppLoadLog.TxValue = 0
    pData->DynamicData.additionalMinutesThisTrip    = 0                     ;   // Can't tell from sequences whether this is guaranteed to be set before it's used in BR_LLSC_4_19, so set it here.

    pData->InternalData.cardSnapshotSequenceNumber  = 0                     ;   // Card snapshot sequence number
    pData->InternalData.isValidPin                  = FALSE                 ;   // PIN validation result

    //  Set up any returned variables to default values

    pData->ReturnedData.rejectReason     = MYKI_BR_REJECT_REASON_NOT_APPLICABLE     ; // Extra information about a rejected transaction - eg, card blocked, missing touch-on, insufficent balance, expired, etc
    pData->ReturnedData.acceptReason     = MYKI_BR_ACCEPT_REASON_NOT_APPLICABLE     ; // Extra information about a valid transaction - eg, forced scan-off, etc

    pData->ReturnedData.isTestMode       = FALSE                  ; // Non-Zero if the card is a test card
    pData->ReturnedData.applicableFare   = 0                      ; // Fare charged (+ve) or refunded (-ve)
    pData->ReturnedData.remainingBalance = 0                      ; // Purse balance after the transaction and any applicable topups/reversals
    pData->ReturnedData.expiryDateTime   = 0                      ; // Expiry date of the product if a product was used, or the purse otherwise
    pData->ReturnedData.isProvisional    = 0;
    pData->ReturnedData.productId        = -1                     ; // ID of the product that was used (-1 meaning purse was used)
    pData->ReturnedData.actionApplied    = FALSE                  ; // Non-Zero if an actionlist action was applied
    pData->ReturnedData.topupAmount      = 0                      ; // Non-Zero amount of topup/reversal if a topup (+ve) or topup reversal (-ve) was applied
    pData->ReturnedData.zoneLow          = 0                      ; // Low zone of current (possibly provisional) pass used
    pData->ReturnedData.zoneHigh         = 0                      ; // High zone of current (possibly provisional) pass used
    pData->ReturnedData.passengerId      = 0                      ; // Default
    pData->ReturnedData.bypassCode       = 0                      ; // Initialise

    //  Run the sequence

    result = MYKI_BR_RunSequence( pSequence, pData );

    // Update usage log if required. Only one Usage Log can be created per execution.

    if ( pData->InternalData.IsUsageLogUpdated != FALSE )
    {
        //  Sets up the TAppUsageLog.ControlBitmap
        if ( pData->InternalData.UsageLogData.isTransactionValueSet         != FALSE ||
             pData->InternalData.UsageLogData.isNewTPurseBalanceSet         != FALSE ||
             pData->InternalData.UsageLogData.isPaymentMethodSet            != FALSE )
        {
            //  Value:
            pData->InternalData.UsageLogData.controlBitmap |= TAPP_USAGE_LOG_CONTROL_VALUE;
        }
        else if ( pData->StaticData.AcsCompatibilityMode )
        {
            //  NOTE:   Setting "default" Value data to *mimic* KAMCO reader!
            //
            if ( pData->InternalData.UsageLogData.transactionType           == MYKI_BR_TRANSACTION_TYPE_ENTRY_EXIT  ||
                 pData->InternalData.UsageLogData.transactionType           == MYKI_BR_TRANSACTION_TYPE_EXIT_ONLY   ||
                 pData->InternalData.UsageLogData.transactionType           == MYKI_BR_TRANSACTION_TYPE_ENTRY_ONLY  ||
                 pData->InternalData.UsageLogData.transactionType           == MYKI_BR_TRANSACTION_TYPE_ON_BOARD    )
            {
                pData->InternalData.UsageLogData.controlBitmap     |= TAPP_USAGE_LOG_CONTROL_VALUE;
            }
            //  NOTE:   For others TransactionType, ACS sets the PaymentMethod to "Purse" but
            //          does not set the ControlBitmap.Value bit.
            //
            pData->InternalData.UsageLogData.transactionValue       = 0;
            pData->InternalData.UsageLogData.newTPurseBalance       = 0;
            pData->InternalData.UsageLogData.paymentMethod          = TAPP_USAGE_LOG_PAYMENT_METHOD_TPURSE;
        }

        if ( pData->InternalData.UsageLogData.isZoneSet                     != FALSE ||
             pData->InternalData.UsageLogData.isProductValidationStatusSet  != FALSE )
        {
            //  Usage:
            pData->InternalData.UsageLogData.controlBitmap |= TAPP_USAGE_LOG_CONTROL_USAGE;
        }

        if ( pData->InternalData.UsageLogData.isProductIssuerIdSet          != FALSE ||
             pData->InternalData.UsageLogData.isProductIdSet                != FALSE ||
             pData->InternalData.UsageLogData.isProductSerialNoSet          != FALSE )
        {
            //  Product:
            pData->InternalData.UsageLogData.controlBitmap |= TAPP_USAGE_LOG_CONTROL_PRODUCT;
        }

        if ( myki_br_ldt_AddUsageLogEntry( pData ) < 0 )
        {
            CsErrx( "MYKI_BR_Execute : myki_br_ldt_AddUsageLogEntry() failed" );
        }
    }

    // Updated load log if required. Only one Load Log can be created per execution.

    if ( pData->InternalData.IsLoadLogUpdated != FALSE )
    {
        //  Sets up the TAppLoadLog.ControlBitmap
        if ( pData->InternalData.LoadLogData.isTransactionValueSet          != FALSE ||
             pData->InternalData.LoadLogData.isNewTPurseBalanceSet          != FALSE ||
             pData->InternalData.LoadLogData.isPaymentMethodSet             != FALSE )
        {
            //  Value:
            pData->InternalData.LoadLogData.controlBitmap  |= TAPP_LOAD_LOG_CONTROL_VALUE;
        }

        if ( pData->InternalData.LoadLogData.isProductIssuerIdSet           != FALSE ||
             pData->InternalData.LoadLogData.isProductSerialNoSet           != FALSE ||
             pData->InternalData.LoadLogData.isProductIdSet                 != FALSE )
        {
            //  Product:
            pData->InternalData.LoadLogData.controlBitmap  |= TAPP_LOAD_LOG_CONTROL_PRODUCT;
        }

        if ( myki_br_ldt_AddLoadLogEntry( pData ) < 0 )
        {
            CsErrx( "MYKI_BR_Execute : myki_br_ldt_AddLoadLogEntry() failed" );
        }
    }

    // Updated shift data log if required. 
    if ( pData->InternalData.IsShiftDataLogUpdated != FALSE )
    {

        if ( myki_br_ldt_AddShiftDataLogEntry( pData ) < 0 )
        {
            CsErrx( "MYKI_BR_Execute : myki_br_ldt_AddShiftDataLogEntry() failed" );
        }
    }

    //  Set up the data to be returned (done here rather than in individual BRs for greater control over the final values)

    if ( pData->DynamicData.isTransit &&  MYKI_CS_TAControlGet( &pMYKI_TAControl ) >= 0 )
    {
        if
        (
            ( pMYKI_TAControl->ProductInUse > 0 ) &&
            ( myki_br_GetCardProduct( pMYKI_TAControl->ProductInUse, &pDirectoryInUse, &pProductInUse ) >= 0 )
        )
        {
            pData->ReturnedData.productId       = pDirectoryInUse->ProductId;
            pData->ReturnedData.isProvisional   = (pProductInUse->ControlBitmap & TAPP_TPRODUCT_CONTROL_PROVISIONAL_BITMAP) ? 1 : 0;
            pData->ReturnedData.zoneLow         = pProductInUse->ZoneLow;
            pData->ReturnedData.zoneHigh        = pProductInUse->ZoneHigh;
            pData->ReturnedData.expiryDateTime  = pProductInUse->EndDateTime;
        }

        pData->ReturnedData.passengerId = pMYKI_TAControl->PassengerCode;
    }


    if ( pData->DynamicData.isTransit &&  MYKI_CS_TAPurseBalanceGet( &pPurseBalance ) >= 0 )
    {
        pData->ReturnedData.remainingBalance = pPurseBalance->Balance;
    }

    if ( pData->DynamicData.isOperator && MYKI_CS_OAControlGet( &pMYKI_OAControl ) >= 0 ) // Data required for technician card and operator card
    {
        memcpy(pData->ReturnedData.staffId, pMYKI_OAControl->StaffId, sizeof(pMYKI_OAControl->StaffId));
        pData->ReturnedData.staffExpDate     = pMYKI_OAControl->StaffExpDate;
        pData->ReturnedData.staffProviderId  = pMYKI_OAControl->StaffProviderId;
        pData->ReturnedData.staffBaseId      = pMYKI_OAControl->StaffBaseId;

        if ( MYKI_CS_OAPINGet( &pMYKI_OAPIN ) >= 0 )
        {
            for(i=0;i<8;i++) pData->ReturnedData.staffPin[i]= pMYKI_OAPIN->Pin[i];
        }

        if ( MYKI_CS_OARolesGet( &pMYKI_OARoles ) >= 0 )
        {
            for(i=0;i<pMYKI_OAControl->RolesCount;i++)
            {
                pData->ReturnedData.staffRoles[i].type    = pMYKI_OARoles->Role[i].Type;
                pData->ReturnedData.staffRoles[i].profile = pMYKI_OARoles->Role[i].Profile;
            }
        }
    }

    
    //  Return the result
    pData->ReturnedData.sequenceResult = result;
    switch ( result )
    {
        case SEQ_RESULT_SCAN_ON :
        case SEQ_RESULT_SCAN_OFF :
        case SEQ_RESULT_OPERATOR :
            CsDbg( BRLL_SESSION, "MYKI_BR_Execute() Returning MYKI_BR_RESULT_SUCCESS" );
            return MYKI_BR_RESULT_SUCCESS;

        case SEQ_RESULT_REJECT :
            {
                #define REASON_DECLARATION( a )     #a
                static  const char *sRejectReasons[ ]   =
                {
                    MYKI_BR_REJECT_REASONS
                };  /*  sRejectReasons[ ] */
                #undef  REASON_DECLARATION

                if ( pData->ReturnedData.rejectReason >= MYKI_BR_REJECT_REASON_NOT_APPLICABLE &&
                     pData->ReturnedData.rejectReason <  MYKI_BR_REJECT_REASON_MAX            )
                {
                    CsDbg( BRLL_SESSION, "MYKI_BR_Execute() Returning MYKI_BR_RESULT_REJECT (%s)",
                        sRejectReasons[ pData->ReturnedData.rejectReason ] );
                }
                else
                {
                    CsDbg( BRLL_SESSION, "MYKI_BR_Execute() Returning MYKI_BR_RESULT_REJECT (%d)",
                        pData->ReturnedData.rejectReason );
                }
            }
            return MYKI_BR_RESULT_REJECT;

        case SEQ_RESULT_PRECOMMIT_CHECK_FAILED:
            CsDbg( BRLL_SESSION, "MYKI_BR_Execute() Returning SEQ_RESULT_PRECOMMIT_CHECK_FAILED" );
            myki_br_CreateCardSnapshot( pData );
            return MYKI_BR_RESULT_ERROR;

        default :
            CsDbg( BRLL_SESSION, "MYKI_BR_Execute() Returning MYKI_BR_RESULT_ERROR" );
            return MYKI_BR_RESULT_ERROR;
    }
}
