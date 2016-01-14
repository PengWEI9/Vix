//=============================================================================
//
//  Tests 0nn - Entry Only Sequence
//
//=============================================================================

#include <cs.h>
#include <myki_cardservices.h>

#include <myki_br.h>        // For testing sequences
#include <myki_br_rules.h>  // For testing individual rules
#include <myki_br_context_data.h>

#include "test_common.h"

//=============================================================================
//
//  test001 Entry Only - No card errors
//
//=============================================================================

int     test001( MYKI_BR_ContextData_t *pData )
{
    BusinessRulesResult_e   brResult;

    //  Device

    pData->StaticData.isEntry                   = TRUE;
    pData->StaticData.isExit                    = FALSE;

    pData->DynamicData.numberOfCardsPresented   = 1;
    pData->DynamicData.isOperator               = FALSE;
    pData->DynamicData.isTransit                = TRUE;

    pData->DynamicData.currentDateTime          = CsTime( NULL ).tm_secs;
    pData->DynamicData.entryPointId             = 10500;

    pData->Tariff.TPurseMinBalance              = 100;
    pData->Tariff.TPurseMaximumBalance          = 10000;

    //  Card

    CardImage7( pData );

    //
    //  Run sequence
    //

    ShowCardImage( "test001_0_Start" );

    brResult = MYKI_BR_Execute( pData );

    ShowCardImage( "test001_1_End" );

    if ( brResult != MYKI_BR_RESULT_OPEN_GATE )
        return FALSE;

    return TRUE;
}

//=============================================================================
//
//  test002 Entry Only - Various non-fatal card errors
//
//=============================================================================

int     test002( MYKI_BR_ContextData_t *pData )
{
    BusinessRulesResult_e   brResult;

    //  Device

    pData->StaticData.isEntry                   = TRUE;
    pData->StaticData.isExit                    = FALSE;
    pData->StaticData.deviceSamId               = 1234;

    pData->DynamicData.numberOfCardsPresented   = 1;
    pData->DynamicData.isOperator               = FALSE;
    pData->DynamicData.isTransit                = TRUE;

    pData->DynamicData.currentDateTime          = CsTime( NULL ).tm_secs;
    pData->DynamicData.entryPointId             = 10500;

    pData->Tariff.TPurseMinBalance              = 100;
    pData->Tariff.TPurseMaximumBalance          = 10000;

    //  Card

    CardImage2( pData );

    //
    //  Run sequence
    //

    ShowCardImage( "test002_0_Start" );

    brResult = MYKI_BR_Execute( pData );

    ShowCardImage( "test002_1_End" );

    if ( brResult != MYKI_BR_RESULT_OPEN_GATE )
        return FALSE;

    return TRUE;
}

//=============================================================================
//
//  test003 Entry Only - Multiple cards presented
//
//=============================================================================

int     test003( MYKI_BR_ContextData_t *pData )
{
    BusinessRulesResult_e   brResult;

    //  Device

    pData->StaticData.isEntry                   = TRUE;
    pData->StaticData.isExit                    = FALSE;

    pData->DynamicData.numberOfCardsPresented   = 2;

    //  The sequences are silly - we need to know whether there's multiple cards before we decide the "card type"
    //  but NTS0177 has multiple card detection as part of the LLSC sequences.

    pData->DynamicData.isOperator               = FALSE;
    pData->DynamicData.isTransit                = TRUE;

    pData->DynamicData.currentDateTime          = CsTime( NULL ).tm_secs;

    pData->Tariff.TPurseMinBalance              = 100;
    pData->Tariff.TPurseMaximumBalance          = 10000;

    //
    //  Run sequence
    //

    brResult = MYKI_BR_Execute( pData );

    if ( brResult != MYKI_BR_RESULT_REJECT )
        return FALSE;

    return TRUE;
}

