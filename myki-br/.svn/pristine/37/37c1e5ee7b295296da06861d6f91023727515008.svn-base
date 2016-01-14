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

#include <myki_cdd_enums.h>

#include "test_common.h"

//=============================================================================
//
//  test_BR_LLSC_0_B_001 All preconditions met
//
//=============================================================================

int     test_BR_LLSC_0_B_001( MYKI_BR_ContextData_t *pData )
{
    MYKI_CAControl_t            *pMYKI_CAControl;
    MYKI_TAControl_t            *pMYKI_TAControl;

    if ( MYKI_CS_CAControlGet( &pMYKI_CAControl ) < 0 )
    {
        CsErrx( "BR_LLSC_0_1 : MYKI_CS_CAControlGet() failed" );
        return RULE_RESULT_ERROR;
    }

    if ( MYKI_CS_TAControlGet( &pMYKI_TAControl ) < 0 )
    {
        CsErrx( "BR_LLSC_0_1 : MYKI_CS_TAControlGet() failed" );
        return RULE_RESULT_ERROR;
    }

    pMYKI_CAControl->Checksum   = 0;
    pMYKI_CAControl->ExpiryDate = pData->DynamicData.currentBusinessDate;
    pMYKI_CAControl->Status     = CARD_CONTROL_STATUS_ACTIVATED;
    pMYKI_TAControl->Checksum   = 0;
    pMYKI_TAControl->ExpiryDate = pData->DynamicData.currentBusinessDate;
    pMYKI_TAControl->Status     = TAPP_CONTROL_STATUS_ACTIVATED;

    if ( BR_LLSC_0_B( pData ) != RULE_RESULT_EXECUTED )
        return FALSE;

    return TRUE;
}

//=============================================================================
//
//  test_BR_LLSC_0_B_002 pMYKI_CAControl->Checksum != 0
//
//=============================================================================

int     test_BR_LLSC_0_B_002( MYKI_BR_ContextData_t *pData )
{
    MYKI_CAControl_t            *pMYKI_CAControl;
    MYKI_TAControl_t            *pMYKI_TAControl;

    if ( MYKI_CS_CAControlGet( &pMYKI_CAControl ) < 0 )
    {
        CsErrx( "BR_LLSC_0_1 : MYKI_CS_CAControlGet() failed" );
        return RULE_RESULT_ERROR;
    }

    if ( MYKI_CS_TAControlGet( &pMYKI_TAControl ) < 0 )
    {
        CsErrx( "BR_LLSC_0_1 : MYKI_CS_TAControlGet() failed" );
        return RULE_RESULT_ERROR;
    }

    pMYKI_CAControl->Checksum   = 1;
    pMYKI_CAControl->ExpiryDate = pData->DynamicData.currentBusinessDate;
    pMYKI_CAControl->Status     = CARD_CONTROL_STATUS_ACTIVATED;
    pMYKI_TAControl->Checksum   = 0;
    pMYKI_TAControl->ExpiryDate = pData->DynamicData.currentBusinessDate;
    pMYKI_TAControl->Status     = TAPP_CONTROL_STATUS_ACTIVATED;

    if ( BR_LLSC_0_B( pData ) != RULE_RESULT_BYPASSED )
        return FALSE;

    return TRUE;
}

//=============================================================================
//
//  test_BR_LLSC_0_B_003 pMYKI_CAControl->ExpiryDate < pData->DynamicData.currentBusinessDate
//
//=============================================================================

int     test_BR_LLSC_0_B_003( MYKI_BR_ContextData_t *pData )
{
    MYKI_CAControl_t            *pMYKI_CAControl;
    MYKI_TAControl_t            *pMYKI_TAControl;

    if ( MYKI_CS_CAControlGet( &pMYKI_CAControl ) < 0 )
    {
        CsErrx( "BR_LLSC_0_1 : MYKI_CS_CAControlGet() failed" );
        return RULE_RESULT_ERROR;
    }

    if ( MYKI_CS_TAControlGet( &pMYKI_TAControl ) < 0 )
    {
        CsErrx( "BR_LLSC_0_1 : MYKI_CS_TAControlGet() failed" );
        return RULE_RESULT_ERROR;
    }

    pMYKI_CAControl->Checksum   = 0;
    pMYKI_CAControl->ExpiryDate = pData->DynamicData.currentBusinessDate - 1;
    pMYKI_CAControl->Status     = CARD_CONTROL_STATUS_ACTIVATED;
    pMYKI_TAControl->Checksum   = 0;
    pMYKI_TAControl->ExpiryDate = pData->DynamicData.currentBusinessDate;
    pMYKI_TAControl->Status     = TAPP_CONTROL_STATUS_ACTIVATED;

    if ( BR_LLSC_0_B( pData ) != RULE_RESULT_BYPASSED )
        return FALSE;

    return TRUE;
}

//=============================================================================
//
//  test_BR_LLSC_0_B_004 pMYKI_CAControl->Status 1= CARD_CONTROL_STATUS_ACTIVATED
//
//=============================================================================

int     test_BR_LLSC_0_B_004( MYKI_BR_ContextData_t *pData )
{
    MYKI_CAControl_t            *pMYKI_CAControl;
    MYKI_TAControl_t            *pMYKI_TAControl;

    if ( MYKI_CS_CAControlGet( &pMYKI_CAControl ) < 0 )
    {
        CsErrx( "BR_LLSC_0_1 : MYKI_CS_CAControlGet() failed" );
        return RULE_RESULT_ERROR;
    }

    if ( MYKI_CS_TAControlGet( &pMYKI_TAControl ) < 0 )
    {
        CsErrx( "BR_LLSC_0_1 : MYKI_CS_TAControlGet() failed" );
        return RULE_RESULT_ERROR;
    }

    pMYKI_CAControl->Checksum   = 0;
    pMYKI_CAControl->ExpiryDate = pData->DynamicData.currentBusinessDate;
    pMYKI_CAControl->Status     = CARD_CONTROL_STATUS_RESERVED;
    pMYKI_TAControl->Checksum   = 0;
    pMYKI_TAControl->ExpiryDate = pData->DynamicData.currentBusinessDate;
    pMYKI_TAControl->Status     = TAPP_CONTROL_STATUS_ACTIVATED;

    if ( BR_LLSC_0_B( pData ) != RULE_RESULT_BYPASSED )
        return FALSE;

    return TRUE;
}

//=============================================================================
//
//  test_BR_LLSC_0_B_005 pMYKI_TAControl->Checksum != 0
//
//=============================================================================

int     test_BR_LLSC_0_B_005( MYKI_BR_ContextData_t *pData )
{
    MYKI_CAControl_t            *pMYKI_CAControl;
    MYKI_TAControl_t            *pMYKI_TAControl;

    if ( MYKI_CS_CAControlGet( &pMYKI_CAControl ) < 0 )
    {
        CsErrx( "BR_LLSC_0_1 : MYKI_CS_CAControlGet() failed" );
        return RULE_RESULT_ERROR;
    }

    if ( MYKI_CS_TAControlGet( &pMYKI_TAControl ) < 0 )
    {
        CsErrx( "BR_LLSC_0_1 : MYKI_CS_TAControlGet() failed" );
        return RULE_RESULT_ERROR;
    }

    pMYKI_CAControl->Checksum   = 0;
    pMYKI_CAControl->ExpiryDate = pData->DynamicData.currentBusinessDate;
    pMYKI_CAControl->Status     = CARD_CONTROL_STATUS_ACTIVATED;
    pMYKI_TAControl->Checksum   = 1;
    pMYKI_TAControl->ExpiryDate = pData->DynamicData.currentBusinessDate;
    pMYKI_TAControl->Status     = TAPP_CONTROL_STATUS_ACTIVATED;

    if ( BR_LLSC_0_B( pData ) != RULE_RESULT_BYPASSED )
        return FALSE;

    return TRUE;
}

//=============================================================================
//
//  test_BR_LLSC_0_B_006 pMYKI_TAControl->ExpiryDate < pData->DynamicData.currentBusinessDate
//
//=============================================================================

int     test_BR_LLSC_0_B_006( MYKI_BR_ContextData_t *pData )
{
    MYKI_CAControl_t            *pMYKI_CAControl;
    MYKI_TAControl_t            *pMYKI_TAControl;

    if ( MYKI_CS_CAControlGet( &pMYKI_CAControl ) < 0 )
    {
        CsErrx( "BR_LLSC_0_1 : MYKI_CS_CAControlGet() failed" );
        return RULE_RESULT_ERROR;
    }

    if ( MYKI_CS_TAControlGet( &pMYKI_TAControl ) < 0 )
    {
        CsErrx( "BR_LLSC_0_1 : MYKI_CS_TAControlGet() failed" );
        return RULE_RESULT_ERROR;
    }

    pMYKI_CAControl->Checksum   = 0;
    pMYKI_CAControl->ExpiryDate = pData->DynamicData.currentBusinessDate;
    pMYKI_CAControl->Status     = CARD_CONTROL_STATUS_ACTIVATED;
    pMYKI_TAControl->Checksum   = 0;
    pMYKI_TAControl->ExpiryDate = pData->DynamicData.currentBusinessDate - 1;
    pMYKI_TAControl->Status     = TAPP_CONTROL_STATUS_ACTIVATED;

    if ( BR_LLSC_0_B( pData ) != RULE_RESULT_BYPASSED )
        return FALSE;

    return TRUE;
}

//=============================================================================
//
//  test_BR_LLSC_0_B_007 pMYKI_TAControl->Status != TAPP_CONTROL_STATUS_ACTIVATED etc
//
//=============================================================================

int     test_BR_LLSC_0_B_007( MYKI_BR_ContextData_t *pData )
{
    MYKI_CAControl_t            *pMYKI_CAControl;
    MYKI_TAControl_t            *pMYKI_TAControl;

    if ( MYKI_CS_CAControlGet( &pMYKI_CAControl ) < 0 )
    {
        CsErrx( "BR_LLSC_0_1 : MYKI_CS_CAControlGet() failed" );
        return RULE_RESULT_ERROR;
    }

    if ( MYKI_CS_TAControlGet( &pMYKI_TAControl ) < 0 )
    {
        CsErrx( "BR_LLSC_0_1 : MYKI_CS_TAControlGet() failed" );
        return RULE_RESULT_ERROR;
    }

    pMYKI_CAControl->Checksum   = 0;
    pMYKI_CAControl->ExpiryDate = pData->DynamicData.currentBusinessDate;
    pMYKI_CAControl->Status     = CARD_CONTROL_STATUS_ACTIVATED;
    pMYKI_TAControl->Checksum   = 0;
    pMYKI_TAControl->ExpiryDate = pData->DynamicData.currentBusinessDate;
    pMYKI_TAControl->Status     = TAPP_CONTROL_STATUS_RESERVED;

    if ( BR_LLSC_0_B( pData ) != RULE_RESULT_BYPASSED )
        return FALSE;

    return TRUE;
}

