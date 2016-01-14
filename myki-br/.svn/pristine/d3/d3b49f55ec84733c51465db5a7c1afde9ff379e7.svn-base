//=============================================================================
//
//  Test BR_LLSC_1_2
//
//=============================================================================

#include <myki_cardservices.h>

#include <myki_br.h>        // For testing sequences
#include <myki_br_rules.h>  // For testing individual rules
#include <myki_br_context_data.h>

#include "test_common.h"

//=============================================================================
//
//  test_BR_LLSC_1_1_001 pData->DynamicData.isEarlyBirdTrip = TRUE
//
//=============================================================================

int     test_BR_LLSC_1_2_001( MYKI_BR_ContextData_t *pData )
{
    pData->DynamicData.isEarlyBirdTrip = TRUE;

    if ( BR_LLSC_1_2( pData ) != RULE_RESULT_BYPASSED )
        return FALSE;

    return TRUE;
}

//=============================================================================
//
//  test_BR_LLSC_1_1_002 All preconditions met
//
//=============================================================================

int     test_BR_LLSC_1_2_002( MYKI_BR_ContextData_t *pData )
{
    CardImage1( pData );

    pData->DynamicData.isEarlyBirdTrip      = FALSE;
    pData->DynamicData.currentTripZoneLow   = 1;
    pData->DynamicData.currentTripZoneHigh  = 2;
    pData->DynamicData.cappingContribution  = 500;

    if ( BR_LLSC_1_2( pData ) != RULE_RESULT_EXECUTED )
        return FALSE;

    return TRUE;
}

