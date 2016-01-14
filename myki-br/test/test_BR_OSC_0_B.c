//=============================================================================
//
//  Test BR_OSC_0_B
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
//  test_BR_OSC_0_B_001 All preconditions met
//
//=============================================================================

int     test_BR_OSC_0_B_001( MYKI_BR_ContextData_t *pData )
{
    MYKI_CAControl_t            *pMYKI_CAControl;
    MYKI_OAControl_t            *pMYKI_OAControl;

    if ( MYKI_CS_CAControlGet( &pMYKI_CAControl ) < 0 )
    {
        CsErrx( "BR_OSC_0_1 : MYKI_CS_CAControlGet() failed" );
        return RULE_RESULT_ERROR;
    }

    if ( MYKI_CS_OAControlGet( &pMYKI_OAControl ) < 0 )
    {
        CsErrx( "BR_OSC_0_1 : MYKI_CS_OAControlGet() failed" );
        return RULE_RESULT_ERROR;
    }

    pMYKI_CAControl->Checksum   = 0;
    pMYKI_CAControl->ExpiryDate = pData->DynamicData.currentBusinessDate;
    pMYKI_CAControl->Status     = CARD_CONTROL_STATUS_ACTIVATED;
    pMYKI_OAControl->Checksum   = 0;
    pMYKI_OAControl->ExpiryDate = pData->DynamicData.currentBusinessDate;
    pMYKI_OAControl->Status     = TAPP_CONTROL_STATUS_ACTIVATED;
    
    if ( BR_OSC_0_B( pData ) != RULE_RESULT_EXECUTED )
        return FALSE;

    return TRUE;
}


//=============================================================================
//
//  test_BR_OSC_0_B_002 pMYKI_CAControl->ExpiryDate < pData->DynamicData.currentBusinessDate
//
//=============================================================================

int     test_BR_OSC_0_B_002( MYKI_BR_ContextData_t *pData )
{
    MYKI_CAControl_t            *pMYKI_CAControl;
    MYKI_OAControl_t            *pMYKI_OAControl;

    if ( MYKI_CS_CAControlGet( &pMYKI_CAControl ) < 0 )
    {
        CsErrx( "BR_OSC_0_1 : MYKI_CS_CAControlGet() failed" );
        return RULE_RESULT_ERROR;
    }

    if ( MYKI_CS_OAControlGet( &pMYKI_OAControl ) < 0 )
    {
        CsErrx( "BR_OSC_0_1 : MYKI_CS_OAControlGet() failed" );
        return RULE_RESULT_ERROR;
    }

    pMYKI_CAControl->Checksum   = 0;
    pMYKI_CAControl->ExpiryDate = pData->DynamicData.currentBusinessDate - 1;
    pMYKI_CAControl->Status     = CARD_CONTROL_STATUS_ACTIVATED;
    pMYKI_OAControl->Checksum   = 0;
    pMYKI_OAControl->ExpiryDate = pData->DynamicData.currentBusinessDate;
    pMYKI_OAControl->Status     = TAPP_CONTROL_STATUS_ACTIVATED;

    if ( BR_OSC_0_B( pData ) != RULE_RESULT_BYPASSED )
        return FALSE;

    return TRUE;
}

//=============================================================================
//
//  test_BR_OSC_0_B_003 pMYKI_CAControl->Status 1= CARD_CONTROL_STATUS_ACTIVATED
//
//=============================================================================

int     test_BR_OSC_0_B_003( MYKI_BR_ContextData_t *pData )
{
    MYKI_CAControl_t            *pMYKI_CAControl;
    MYKI_OAControl_t            *pMYKI_OAControl;

    if ( MYKI_CS_CAControlGet( &pMYKI_CAControl ) < 0 )
    {
        CsErrx( "BR_OSC_O_1 : MYKI_CS_CAControlGet() failed" );
        return RULE_RESULT_ERROR;
    }

    if ( MYKI_CS_OAControlGet( &pMYKI_OAControl ) < 0 )
    {
        CsErrx( "BR_OSC_O_1 : MYKI_CS_OAControlGet() failed" );
        return RULE_RESULT_ERROR;
    }

    pMYKI_CAControl->Checksum   = 0;
    pMYKI_CAControl->ExpiryDate = pData->DynamicData.currentBusinessDate;
    pMYKI_CAControl->Status     = CARD_CONTROL_STATUS_RESERVED;
    pMYKI_OAControl->Checksum   = 0;
    pMYKI_OAControl->ExpiryDate = pData->DynamicData.currentBusinessDate;
    pMYKI_OAControl->Status     = TAPP_CONTROL_STATUS_ACTIVATED;

    if ( BR_OSC_0_B( pData ) != RULE_RESULT_BYPASSED )
        return FALSE;

    return TRUE;
}


//=============================================================================
//
//  test_BR_OSC_0_B_004 pMYKI_OAControl->ExpiryDate < pData->DynamicData.currentBusinessDate
//
//=============================================================================

int     test_BR_OSC_0_B_004( MYKI_BR_ContextData_t *pData )
{
    MYKI_CAControl_t            *pMYKI_CAControl;
    MYKI_OAControl_t            *pMYKI_OAControl;

    if ( MYKI_CS_CAControlGet( &pMYKI_CAControl ) < 0 )
    {
        CsErrx( "BR_OSC_O_1 : MYKI_CS_CAControlGet() failed" );
        return RULE_RESULT_ERROR;
    }

    if ( MYKI_CS_OAControlGet( &pMYKI_OAControl ) < 0 )
    {
        CsErrx( "BR_OSC_O_1 : MYKI_CS_OAControlGet() failed" );
        return RULE_RESULT_ERROR;
    }

    pMYKI_CAControl->Checksum   = 0;
    pMYKI_CAControl->ExpiryDate = pData->DynamicData.currentBusinessDate;
    pMYKI_CAControl->Status     = CARD_CONTROL_STATUS_ACTIVATED;
    pMYKI_OAControl->Checksum   = 0;
    pMYKI_OAControl->ExpiryDate = pData->DynamicData.currentBusinessDate - 1;
    pMYKI_OAControl->Status     = TAPP_CONTROL_STATUS_ACTIVATED;

    if ( BR_OSC_0_B( pData ) != RULE_RESULT_BYPASSED )
        return FALSE;

    return TRUE;
}

//=============================================================================
//
//  test_BR_OSC_0_B_005 pMYKI_OAControl->Status != TAPP_CONTROL_STATUS_ACTIVATED etc
//
//=============================================================================

int     test_BR_OSC_0_B_005( MYKI_BR_ContextData_t *pData )
{
    MYKI_CAControl_t            *pMYKI_CAControl;
    MYKI_OAControl_t            *pMYKI_OAControl;

    if ( MYKI_CS_CAControlGet( &pMYKI_CAControl ) < 0 )
    {
        CsErrx( "BR_OSC_O_1 : MYKI_CS_CAControlGet() failed" );
        return RULE_RESULT_ERROR;
    }

    if ( MYKI_CS_OAControlGet( &pMYKI_OAControl ) < 0 )
    {
        CsErrx( "BR_OSC_O_1 : MYKI_CS_OAControlGet() failed" );
        return RULE_RESULT_ERROR;
    }

    pMYKI_CAControl->Checksum   = 0;
    pMYKI_CAControl->ExpiryDate = pData->DynamicData.currentBusinessDate;
    pMYKI_CAControl->Status     = CARD_CONTROL_STATUS_ACTIVATED;
    pMYKI_OAControl->Checksum   = 0;
    pMYKI_OAControl->ExpiryDate = pData->DynamicData.currentBusinessDate;
    pMYKI_OAControl->Status     = TAPP_CONTROL_STATUS_RESERVED;

    if ( BR_OSC_0_B( pData ) != RULE_RESULT_BYPASSED )
        return FALSE;

    return TRUE;
}

