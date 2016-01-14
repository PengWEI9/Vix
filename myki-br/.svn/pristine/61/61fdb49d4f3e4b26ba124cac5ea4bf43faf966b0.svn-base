//=============================================================================
//
//  Tests 1nn - Exit Only Sequence
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
//  test101 Exit Only - No card errors
//
//=============================================================================

int     test101( MYKI_BR_ContextData_t *pData )
{
    BusinessRulesResult_e   brResult;

    //  Device

    pData->StaticData.isEntry                   = FALSE;
    pData->StaticData.isExit                    = TRUE;

    pData->DynamicData.numberOfCardsPresented   = 1;
    pData->DynamicData.isOperator               = FALSE;
    pData->DynamicData.isTransit                = TRUE;

    pData->DynamicData.currentDateTime          = CsTime( NULL ).tm_secs;
    pData->DynamicData.entryPointId             = 10550;

    pData->Tariff.TPurseMinBalance              = 100;
    pData->Tariff.TPurseMaximumBalance          = 10000;

    //  Card

    CardImage6( pData );

    //
    //  Run sequence
    //

    ShowCardImage( "test101_0_Start" );

    brResult = MYKI_BR_Execute( pData );

    ShowCardImage( "test101_1_End" );

    if ( brResult != MYKI_BR_RESULT_OPEN_GATE )
        return FALSE;

    return TRUE;
}

//=============================================================================
//
//  test102 Exit Only - Various non-fatal card errors
//
//=============================================================================

int     test102( MYKI_BR_ContextData_t *pData )
{
    BusinessRulesResult_e   brResult;

    //  Device

    pData->StaticData.isEntry                   = FALSE;
    pData->StaticData.isExit                    = TRUE;

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

    ShowCardImage( "test102_0_Start" );

    brResult = MYKI_BR_Execute( pData );

    ShowCardImage( "test102_1_End" );

    if ( brResult != MYKI_BR_RESULT_OPEN_GATE )
        return FALSE;

    return TRUE;
}

//=============================================================================
//
//  test103 Exit Only - Multiple cards presented
//
//=============================================================================

int     test103( MYKI_BR_ContextData_t *pData )
{
    BusinessRulesResult_e   brResult;

    //  Device

    pData->StaticData.isEntry                   = FALSE;
    pData->StaticData.isExit                    = TRUE;

    pData->DynamicData.numberOfCardsPresented   = 2;

    //  The sequences are silly - we need to know whether there's multiple cards before we decide the "card type"
    //  but NTS0177 has multiple card detection as part of the LLSC sequences.

    pData->DynamicData.isOperator               = FALSE;
    pData->DynamicData.isTransit                = TRUE;

    pData->DynamicData.currentDateTime          = CsTime( NULL ).tm_secs;
    pData->DynamicData.entryPointId             = 10500;

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

//=============================================================================
//
//  test104 Exit Only - Not scanned on (no purse balance)
//
//=============================================================================

int     test104( MYKI_BR_ContextData_t *pData )
{
    BusinessRulesResult_e   brResult;

    //  Device

    pData->StaticData.isEntry                   = FALSE;
    pData->StaticData.isExit                    = TRUE;

    pData->DynamicData.numberOfCardsPresented   = 1;
    pData->DynamicData.isOperator               = FALSE;
    pData->DynamicData.isTransit                = TRUE;

    pData->DynamicData.currentDateTime          = CsTime( NULL ).tm_secs;
    pData->DynamicData.entryPointId             = 10500;

    pData->Tariff.TPurseMinBalance              = 100;
    pData->Tariff.TPurseMaximumBalance          = 10000;

    //  Card

    CardImage4( pData );

    //
    //  Run sequence
    //

    ShowCardImage( "test104_0_Start" );

    brResult = MYKI_BR_Execute( pData );

    ShowCardImage( "test104_1_End" );

    if ( brResult == MYKI_BR_RESULT_OPEN_GATE )
        return FALSE;

    return TRUE;
}

//=============================================================================
//
//  test105 Exit Only - Not scanned on (purse balance available)
//
//=============================================================================

int     test105( MYKI_BR_ContextData_t *pData )
{
    BusinessRulesResult_e   brResult;

    //  Device

    pData->StaticData.isEntry                   = FALSE;
    pData->StaticData.isExit                    = TRUE;

    pData->DynamicData.currentDateTime          = CsTime( NULL ).tm_secs;

    pData->DynamicData.numberOfCardsPresented   = 1;
    pData->DynamicData.isOperator               = FALSE;
    pData->DynamicData.isTransit                = TRUE;

    pData->DynamicData.currentDateTime          = CsTime( NULL ).tm_secs;
    pData->DynamicData.entryPointId             = 10500;

    pData->Tariff.stationExitFee                = 100;

    pData->Tariff.TPurseMinBalance              = 100;
    pData->Tariff.TPurseMaximumBalance          = 10000;

    //  Card

    CardImage5( pData );

    //
    //  Run sequence
    //

    ShowCardImage( "test105_0_Start" );

    brResult = MYKI_BR_Execute( pData );

    ShowCardImage( "test105_1_End" );

    if ( brResult != MYKI_BR_RESULT_OPEN_GATE )
        return FALSE; //test fail

    return TRUE;
}

