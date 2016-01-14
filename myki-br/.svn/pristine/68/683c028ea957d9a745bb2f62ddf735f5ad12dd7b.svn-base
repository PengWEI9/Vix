//=============================================================================
//
//  Test BR_LLSC_0_A
//
//=============================================================================

#include <myki_cardservices.h>

#include <myki_br.h>        // For testing sequences
#include <myki_br_rules.h>  // For testing individual rules
#include <myki_br_context_data.h>

//#include <BusinessRulesEngine.h>

#include "test_common.h"

//=============================================================================
//
//  test_BR_LLSC_1_1_001 pData->DynamicData.numberOfCardsPresented = 1
//
//=============================================================================

int     test_BR_LLSC_1_1_001( MYKI_BR_ContextData_t *pData )
{
    pData->DynamicData.numberOfCardsPresented = 1;

    if ( BR_LLSC_1_1( pData ) != RULE_RESULT_BYPASSED )
        return FALSE;

    return TRUE;
}

//=============================================================================
//
//  test_BR_LLSC_1_1_002 All preconditions met, reject multiple cards
//
//=============================================================================

int     test_BR_LLSC_1_1_002( MYKI_BR_ContextData_t *pData )
{
    pData->DynamicData.numberOfCardsPresented = 2;

    if ( BR_LLSC_1_1( pData ) != RULE_RESULT_EXECUTED )
        return FALSE;

    return TRUE;
}

