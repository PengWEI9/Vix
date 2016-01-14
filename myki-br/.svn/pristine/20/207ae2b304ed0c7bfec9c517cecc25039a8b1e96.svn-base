//=============================================================================
//
//  Test BR_LLSC_0_A
//
//=============================================================================

#include <cs.h>
#include <myki_cardservices.h>

#include <myki_br.h>        // For testing sequences
#include <myki_br_rules.h>  // For testing individual rules
#include <myki_br_context_data.h>
#include <myki_br_card_constants.h>

#include "test_common.h"

//=============================================================================
//
//  test_BR_LLSC_0_A_001 All preconditions met (ie, Test Mode Bit is set)
//
//=============================================================================

int     test_BR_LLSC_0_A_001( MYKI_BR_ContextData_t *pData )
{
    MYKI_CAIssuer_t             *pMYKI_CAIssuer;

    if ( MYKI_CS_CAIssuerGet( &pMYKI_CAIssuer ) < 0 )
    {
        CsErrx( "test_BR_LLSC_0_A_001 : MYKI_CS_CAIssuerGet() failed" );
        return FALSE;
    }

    pMYKI_CAIssuer->OperationMode = OPERATION_MODE_TEST;      // Bit 0 set, all other bits clear

    if ( BR_LLSC_0_A( pData ) != RULE_RESULT_EXECUTED )
        return FALSE;

    return TRUE;
}

//=============================================================================
//
//  test_BR_LLSC_0_A_002 Not Test Mode
//
//=============================================================================

int     test_BR_LLSC_0_A_002( MYKI_BR_ContextData_t *pData )
{
    MYKI_CAIssuer_t             *pMYKI_CAIssuer;

    if ( MYKI_CS_CAIssuerGet( &pMYKI_CAIssuer ) < 0 )
    {
        CsErrx( "test_BR_LLSC_0_A_001 : MYKI_CS_CAIssuerGet() failed" );
        return FALSE;
    }

    pMYKI_CAIssuer->OperationMode = 0;                        // All bits clear

    if ( BR_LLSC_0_A( pData ) != RULE_RESULT_BYPASSED )
        return FALSE;

    return TRUE;
}

