//=============================================================================
//
//  API for individual business rules in the Myki Business Rules package
//
//  Normally an application would not use this API - instead, it should use
//  the main API in myki_br.h. This API is included more for testing.
//
//=============================================================================

#ifndef MYKI_BR_RULES_H_
#define MYKI_BR_RULES_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "myki_br_context_data.h"

//
//  Possible return values from a business rule function
//

typedef enum
{
    RULE_RESULT_EXECUTED,               // Rule was executed (pre-conditions were all met)
    RULE_RESULT_BYPASSED,               // Rule was not executed (one or more pre-conditions were *not* met)
    RULE_RESULT_ERROR                   // All BR errors are treated the same in terms of sequencing - ie, abort all sequences and return SEQ_RESULT_ERROR.

}   RuleResult_e;

RuleResult_e BR_VIX_0_1( MYKI_BR_ContextData_t *pData );
RuleResult_e BR_VIX_0_2( MYKI_BR_ContextData_t *pData );
RuleResult_e BR_VIX_0_3( MYKI_BR_ContextData_t *pData );
RuleResult_e BR_VIX_0_4( MYKI_BR_ContextData_t *pData );
RuleResult_e BR_VIX_0_5( MYKI_BR_ContextData_t *pData );
//RuleResult_e BR_VIX_0_6( MYKI_BR_ContextData_t *pData );

RuleResult_e BR_LLSC_0_A( MYKI_BR_ContextData_t *pData );
RuleResult_e BR_LLSC_0_B( MYKI_BR_ContextData_t *pData );

RuleResult_e BR_LLSC_0_1a( MYKI_BR_ContextData_t *pData );
RuleResult_e BR_LLSC_0_1b( MYKI_BR_ContextData_t *pData );

RuleResult_e BR_LLSC_1_1( MYKI_BR_ContextData_t *pData );
RuleResult_e BR_LLSC_1_2( MYKI_BR_ContextData_t *pData );
RuleResult_e BR_LLSC_1_3( MYKI_BR_ContextData_t *pData );
RuleResult_e BR_LLSC_1_4( MYKI_BR_ContextData_t *pData );
RuleResult_e BR_LLSC_1_5( MYKI_BR_ContextData_t *pData );
RuleResult_e BR_LLSC_1_6( MYKI_BR_ContextData_t *pData );
RuleResult_e BR_LLSC_1_7( MYKI_BR_ContextData_t *pData );
RuleResult_e BR_LLSC_1_8( MYKI_BR_ContextData_t *pData );
RuleResult_e BR_LLSC_1_9( MYKI_BR_ContextData_t *pData );
RuleResult_e BR_LLSC_1_10( MYKI_BR_ContextData_t *pData );

RuleResult_e BR_LLSC_2_1( MYKI_BR_ContextData_t *pData );
RuleResult_e BR_LLSC_2_2( MYKI_BR_ContextData_t *pData );
RuleResult_e BR_LLSC_2_3( MYKI_BR_ContextData_t *pData );
RuleResult_e BR_LLSC_2_4( MYKI_BR_ContextData_t *pData );
RuleResult_e BR_LLSC_2_5( MYKI_BR_ContextData_t *pData );
RuleResult_e BR_LLSC_2_6( MYKI_BR_ContextData_t *pData );
RuleResult_e BR_LLSC_2_7( MYKI_BR_ContextData_t *pData );
RuleResult_e BR_LLSC_2_8( MYKI_BR_ContextData_t *pData );
RuleResult_e BR_LLSC_2_9( MYKI_BR_ContextData_t *pData );
RuleResult_e BR_LLSC_2_10( MYKI_BR_ContextData_t *pData );
RuleResult_e BR_LLSC_2_11( MYKI_BR_ContextData_t *pData );
RuleResult_e BR_LLSC_2_12( MYKI_BR_ContextData_t *pData );
RuleResult_e BR_LLSC_2_13( MYKI_BR_ContextData_t *pData );
RuleResult_e BR_LLSC_2_14( MYKI_BR_ContextData_t *pData );

RuleResult_e BR_LLSC_3_1( MYKI_BR_ContextData_t *pData );
RuleResult_e BR_LLSC_3_2( MYKI_BR_ContextData_t *pData );

RuleResult_e BR_LLSC_4_1( MYKI_BR_ContextData_t *pData );
RuleResult_e BR_LLSC_4_2( MYKI_BR_ContextData_t *pData );
RuleResult_e BR_LLSC_4_3( MYKI_BR_ContextData_t *pData );
RuleResult_e BR_LLSC_4_4( MYKI_BR_ContextData_t *pData );
RuleResult_e BR_LLSC_4_5( MYKI_BR_ContextData_t *pData );
RuleResult_e BR_LLSC_4_6( MYKI_BR_ContextData_t *pData );
RuleResult_e BR_LLSC_4_7( MYKI_BR_ContextData_t *pData );
RuleResult_e BR_LLSC_4_8( MYKI_BR_ContextData_t *pData );
RuleResult_e BR_LLSC_4_9( MYKI_BR_ContextData_t *pData );
RuleResult_e BR_LLSC_4_10( MYKI_BR_ContextData_t *pData );
RuleResult_e BR_LLSC_4_11( MYKI_BR_ContextData_t *pData );
RuleResult_e BR_LLSC_4_13( MYKI_BR_ContextData_t *pData );
RuleResult_e BR_LLSC_4_14( MYKI_BR_ContextData_t *pData );
RuleResult_e BR_LLSC_4_15( MYKI_BR_ContextData_t *pData );
RuleResult_e BR_LLSC_4_16( MYKI_BR_ContextData_t *pData );
RuleResult_e BR_LLSC_4_17( MYKI_BR_ContextData_t *pData );
RuleResult_e BR_LLSC_4_18( MYKI_BR_ContextData_t *pData );
RuleResult_e BR_LLSC_4_19( MYKI_BR_ContextData_t *pData );
RuleResult_e BR_LLSC_4_20( MYKI_BR_ContextData_t *pData );

RuleResult_e BR_LLSC_5_1( MYKI_BR_ContextData_t *pData );
RuleResult_e BR_LLSC_5_2( MYKI_BR_ContextData_t *pData );
RuleResult_e BR_LLSC_5_3( MYKI_BR_ContextData_t *pData );
RuleResult_e BR_LLSC_5_4( MYKI_BR_ContextData_t *pData );
RuleResult_e BR_LLSC_5_5( MYKI_BR_ContextData_t *pData );
RuleResult_e BR_LLSC_5_6( MYKI_BR_ContextData_t *pData );
RuleResult_e BR_LLSC_5_7( MYKI_BR_ContextData_t *pData );
RuleResult_e BR_LLSC_5_8( MYKI_BR_ContextData_t *pData );
RuleResult_e BR_LLSC_5_9( MYKI_BR_ContextData_t *pData );

RuleResult_e BR_LLSC_6_1( MYKI_BR_ContextData_t *pData );
RuleResult_e BR_LLSC_6_2( MYKI_BR_ContextData_t *pData );
RuleResult_e BR_LLSC_6_3( MYKI_BR_ContextData_t *pData );
RuleResult_e BR_LLSC_6_7( MYKI_BR_ContextData_t *pData );
RuleResult_e BR_LLSC_6_8( MYKI_BR_ContextData_t *pData );
RuleResult_e BR_LLSC_6_9( MYKI_BR_ContextData_t *pData );
RuleResult_e BR_LLSC_6_10( MYKI_BR_ContextData_t *pData );
RuleResult_e BR_LLSC_6_11( MYKI_BR_ContextData_t *pData );
RuleResult_e BR_LLSC_6_12( MYKI_BR_ContextData_t *pData );
RuleResult_e BR_LLSC_6_13( MYKI_BR_ContextData_t *pData );
RuleResult_e BR_LLSC_6_14( MYKI_BR_ContextData_t *pData );
RuleResult_e BR_LLSC_6_15( MYKI_BR_ContextData_t *pData );
RuleResult_e BR_LLSC_6_16( MYKI_BR_ContextData_t *pData );
RuleResult_e BR_LLSC_6_17( MYKI_BR_ContextData_t *pData );
RuleResult_e BR_LLSC_6_18( MYKI_BR_ContextData_t *pData );
RuleResult_e BR_LLSC_6_19( MYKI_BR_ContextData_t *pData );
RuleResult_e BR_LLSC_6_20( MYKI_BR_ContextData_t *pData );
RuleResult_e BR_LLSC_6_21( MYKI_BR_ContextData_t *pData );
RuleResult_e BR_LLSC_6_22( MYKI_BR_ContextData_t *pData );
RuleResult_e BR_LLSC_6_23( MYKI_BR_ContextData_t *pData );
RuleResult_e BR_LLSC_6_24( MYKI_BR_ContextData_t *pData );

RuleResult_e BR_LLSC_7_1( MYKI_BR_ContextData_t *pData );
RuleResult_e BR_LLSC_7_2( MYKI_BR_ContextData_t *pData );
RuleResult_e BR_LLSC_7_3( MYKI_BR_ContextData_t *pData );
RuleResult_e BR_LLSC_7_4( MYKI_BR_ContextData_t *pData );
RuleResult_e BR_LLSC_7_5( MYKI_BR_ContextData_t *pData );

RuleResult_e BR_LLSC_9_1( MYKI_BR_ContextData_t *pData );

RuleResult_e BR_LLSC_10_1( MYKI_BR_ContextData_t *pData );
RuleResult_e BR_LLSC_10_2( MYKI_BR_ContextData_t *pData );
RuleResult_e BR_LLSC_10_3( MYKI_BR_ContextData_t *pData );
RuleResult_e BR_LLSC_10_4( MYKI_BR_ContextData_t *pData );
RuleResult_e BR_LLSC_10_5( MYKI_BR_ContextData_t *pData );
RuleResult_e BR_LLSC_10_6( MYKI_BR_ContextData_t *pData );

RuleResult_e BR_LLSC_99_1( MYKI_BR_ContextData_t *pData );

RuleResult_e BR_OSC_0_A( MYKI_BR_ContextData_t *pData );
RuleResult_e BR_OSC_0_B( MYKI_BR_ContextData_t *pData );

RuleResult_e BR_OSC_1_1( MYKI_BR_ContextData_t *pData );
RuleResult_e BR_OSC_1_2( MYKI_BR_ContextData_t *pData );
RuleResult_e BR_OSC_1_3( MYKI_BR_ContextData_t *pData );
RuleResult_e BR_OSC_1_4( MYKI_BR_ContextData_t *pData );

RuleResult_e BR_VIX_OSC_2_1( MYKI_BR_ContextData_t *pData );
RuleResult_e BR_VIX_OSC_2_2( MYKI_BR_ContextData_t *pData );
RuleResult_e BR_VIX_OSC_2_3( MYKI_BR_ContextData_t *pData );
RuleResult_e BR_VIX_OSC_2_5( MYKI_BR_ContextData_t *pData );

RuleResult_e BR_VIX_OSC_3_1( MYKI_BR_ContextData_t *pData );
RuleResult_e BR_VIX_OSC_3_2( MYKI_BR_ContextData_t *pData );
RuleResult_e BR_VIX_OSC_3_3( MYKI_BR_ContextData_t *pData );
RuleResult_e BR_VIX_OSC_3_4( MYKI_BR_ContextData_t *pData );
RuleResult_e BR_VIX_OSC_3_5( MYKI_BR_ContextData_t *pData );

RuleResult_e BR_VIX_LLSC_20_1( MYKI_BR_ContextData_t *pData );
RuleResult_e BR_VIX_LLSC_20_2( MYKI_BR_ContextData_t *pData );
RuleResult_e BR_VIX_LLSC_21_1( MYKI_BR_ContextData_t *pData );
RuleResult_e BR_VIX_LLSC_21_2( MYKI_BR_ContextData_t *pData );

#ifdef __cplusplus
}
#endif

#endif /* MYKI_BR_RULES_H_ */
