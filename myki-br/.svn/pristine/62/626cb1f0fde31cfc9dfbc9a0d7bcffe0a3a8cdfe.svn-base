//=============================================================================
//
//  Tests 2nn - Miscellaneous tests
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
//  test201     UAT Test 1
//
//  Perform a trip in Zone 1 using FPDx
//
//=============================================================================

int     test201( MYKI_BR_ContextData_t *pData )
{
    BusinessRulesResult_e   brResult;

    //
    //  Run sequence 1 - entry
    //

    pData->StaticData.isEntry                   = TRUE;
    pData->StaticData.isExit                    = FALSE;

    pData->DynamicData.numberOfCardsPresented   = 1;
    pData->DynamicData.isOperator               = FALSE;
    pData->DynamicData.isTransit                = TRUE;

    pData->DynamicData.currentDateTime          = CsTime( NULL ).tm_secs;
    pData->DynamicData.entryPointId             = 10500;

    CardImage7( pData );

    ShowCardImage( "test201_0_Start" );

    brResult = MYKI_BR_Execute( pData );

    ShowCardImage( "test201_1_AfterEntry" );

    if ( brResult != MYKI_BR_RESULT_OPEN_GATE )
        return FALSE;

    //
    //  Run sequence 2 - exit
    //

    pData->StaticData.isEntry                   = FALSE;
    pData->StaticData.isExit                    = TRUE;

    pData->DynamicData.currentDateTime          += MINUTES_TO_SECONDS( 5 );
    pData->DynamicData.entryPointId             = 10550;

    brResult = MYKI_BR_Execute( pData );

    ShowCardImage( "test201_2_End" );

    if ( brResult != MYKI_BR_RESULT_OPEN_GATE )
        return FALSE;

    return TRUE;
}

//=============================================================================
//
//  test202     UAT Test 2
//
//  Perform a trip in Zone 1 using FPDx ( insufficent balace for the trip)
//
//=============================================================================

int     test202( MYKI_BR_ContextData_t *pData )
{
    BusinessRulesResult_e   brResult;

    //
    //  Run sequence 1 - entry
    //

    pData->StaticData.isEntry                   = TRUE;
    pData->StaticData.isExit                    = FALSE;

    pData->DynamicData.numberOfCardsPresented   = 1;
    pData->DynamicData.isOperator               = FALSE;
    pData->DynamicData.isTransit                = TRUE;

    pData->DynamicData.currentDateTime          = CsTime( NULL ).tm_secs;
    pData->DynamicData.entryPointId             = 10500;

    CardImage8( pData );

    ShowCardImage( "test202_0_Start" );

    brResult = MYKI_BR_Execute( pData );

    ShowCardImage( "test202_1_AfterEntry" );

    if ( brResult != MYKI_BR_RESULT_OPEN_GATE )
        return FALSE;

    //
    //  Run sequence 2 - exit
    //

    pData->StaticData.isEntry                   = FALSE;
    pData->StaticData.isExit                    = TRUE;

    pData->DynamicData.currentDateTime          += MINUTES_TO_SECONDS( 5 );
    pData->DynamicData.entryPointId             = 10550;

    brResult = MYKI_BR_Execute( pData );

    ShowCardImage( "test202_2_End" );

    if ( brResult != MYKI_BR_RESULT_OPEN_GATE )
        return FALSE;

    return TRUE;
}

//=============================================================================
//
//  test203     UAT Test 3
//
//
//
//=============================================================================

int     test203( MYKI_BR_ContextData_t *pData )
{
    BusinessRulesResult_e   brResult;

    //
    //  Run sequence 1 - entry
    //

    pData->StaticData.isEntry                   = TRUE;
    pData->StaticData.isExit                    = FALSE;

    pData->DynamicData.numberOfCardsPresented   = 1;
    pData->DynamicData.isOperator               = FALSE;
    pData->DynamicData.isTransit                = TRUE;

    pData->DynamicData.currentDateTime          = CsTime( NULL ).tm_secs;
    pData->DynamicData.entryPointId             = 10500;

    CardImage9( pData );

    ShowCardImage( "test203_0_Start" );

    brResult = MYKI_BR_Execute( pData );

    ShowCardImage( "test203_1_End" );

    if ( brResult != MYKI_BR_RESULT_REJECT )
        return FALSE;

    return TRUE;
}

//=============================================================================
//
//  test204     UAT Test 4
//
//
//
//=============================================================================

int     test204( MYKI_BR_ContextData_t *pData )
{
    return TRUE;
}

//=============================================================================
//
//  test205     UAT Test 5
//
//
//
//=============================================================================

int     test205( MYKI_BR_ContextData_t *pData )
{
    return TRUE;
}

//=============================================================================
//
//  test206     UAT Test 6
//
//
//
//=============================================================================

int     test206( MYKI_BR_ContextData_t *pData )
{
    return TRUE;
}

//=============================================================================
//
//  test207     UAT Test 7
//
//
//
//=============================================================================

int     test207( MYKI_BR_ContextData_t *pData )
{
    return TRUE;
}

//=============================================================================
//
//  test208     UAT Test 8
//
//
//
//=============================================================================

int     test208( MYKI_BR_ContextData_t *pData )
{
    return TRUE;
}

//=============================================================================
//
//  test209     UAT Test 9
//
//
//
//=============================================================================

int     test209( MYKI_BR_ContextData_t *pData )
{
    return TRUE;
}

//=============================================================================
//
//  test210     UAT Test 10
//
//
//
//=============================================================================

int     test210( MYKI_BR_ContextData_t *pData )
{
    return TRUE;
}

//=============================================================================
//
//  test211     UAT Test 11
//
//
//
//=============================================================================

int     test211( MYKI_BR_ContextData_t *pData )
{
    return TRUE;
}

//=============================================================================
//
//  test212     UAT Test 12
//
//
//
//=============================================================================

int     test212( MYKI_BR_ContextData_t *pData )
{
    return TRUE;
}

