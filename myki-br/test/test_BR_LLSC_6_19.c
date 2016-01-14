/* -*- mode: c; tabs: 4; -*- */
/*=============================================================================
**
**    Vix Technology                   Licensed software
**    (C) 2013                         All rights reserved
**
**=============================================================================
**
**  Project/Product : MBU
**  Filename        : test_BR_LLSC_6_19.c
**  Author(s)       : David Purdie
**  Description     :
**      Implements BR_LLSC_6_19 business rule unit-tests.
**
**  Function(s)     :
**      Template                [Public]    template function
**
**  Information     :
**   Compiler(s)    : ANSI C
**   Target(s)      : Independent
**
**===========================================================================*/

/*
 *      Options
 *      -------
 */

/*
 *      Includes
 *      --------
 */

#include <cs.h>
#include <myki_cardservices.h>
#include <myki_cdd_enums.h>
#include <myki_br.h>
#include <myki_br_rules.h>
#include <myki_br_context_data.h>

#include "test_common.h"
#include "BR_Common.h"

/*
 *      External References
 *      -------------------
 */

/*
 *      Local Constants and Macros
 *      --------------------------
 */

/*
 *      Local Data Types
 *      ----------------
 */

/*
 *      Local Function Prototypes
 *      -------------------------
 */

/*
 *      Local Variables
 *      ---------------
 */

/*
 *      Global Variables
 *      ----------------
 */

/*----------------------------------------------------------------------------
** FUNCTION           : runTest
**
** DESCRIPTION        : Run the test and examine the results
**
**
** INPUTS             : pData           - Context Data
**                      rc              - Expected Result Code
**                      bc              - Expected bypass Code
**
** RETURNS            : TRUE        - Test was OK
**                      FALSE       - Test Failed
**
----------------------------------------------------------------------------*/

static int runTest( MYKI_BR_ContextData_t *pData, int rc, int bc)
{
    int rv = BR_LLSC_6_19( pData );

    if ( rv != rc )
    {
        CsVerbose("test_BR_LLSC_6_19 - Unexpected exit code. Got:%d, Expected:%d\n", rv, rc );
        return FALSE;
    }

    if ( pData && pData->ReturnedData.bypassCode != bc )
    {
        CsVerbose("test_BR_LLSC_6_19 - Unexpected ByPassCode. Got:%d, Expected:%d\n", pData->ReturnedData.bypassCode, bc);
        return FALSE;
    }

    return TRUE;
}

/*----------------------------------------------------------------------------
** FUNCTION           : CreateProduct
**
** DESCRIPTION        : Create a product with specified values
**
**
** INPUTS             : pCardImage              - Card Image Data
**                      dirIndex                - Product index to create
**                      productId               - Product to Create
**                      zoneLow
**                      zoneHigh
**                      expiry
**
** RETURNS            : Nothing
**
----------------------------------------------------------------------------*/

static  void    CreateProduct( CardImage_t *pCardImage, int dirIndex, int productId, int zoneLow, int zoneHigh, Time_t expiry )
{
    //  Just in case the caller gets it wrong - index should be between 1 and 5 inclusive

    if ( dirIndex < 1 || dirIndex >= DIMOF( pCardImage->pMYKI_TAControl->Directory ) )
    {
        CsErrx( "CreateProduct() Invalid directory index %d", dirIndex );
        return;
    }

    //  Create product

    pCardImage->pMYKI_TAProduct[ dirIndex - 1 ]->ZoneLow                = zoneLow;
    pCardImage->pMYKI_TAProduct[ dirIndex - 1 ]->ZoneHigh               = zoneHigh;
    pCardImage->pMYKI_TAProduct[ dirIndex - 1 ]->EndDateTime            = expiry;

    //  Create directory entry for product

    pCardImage->pMYKI_TAControl->Directory[ dirIndex ].Status           = TAPP_CONTROL_DIRECTORY_STATUS_ACTIVATED;
    pCardImage->pMYKI_TAControl->Directory[ dirIndex ].ProductId        = productId;
    pCardImage->pMYKI_TAControl->Directory[ dirIndex ].SerialNo         = pCardImage->pMYKI_TAControl->NextProductSerialNo++;
}


/*==========================================================================*
**
**  test_BR_LLSC_6_19_001
**
**  Description     :
**      Test error - NULL pData
**
**  Parameters      :
**      pData           [I/O]   BR context data
**
**  Returns         :
**      TRUE                    test passed
**      FALSE                   test failed
**
**  Notes           :
**
**
**==========================================================================*/

int test_BR_LLSC_6_19_001(MYKI_BR_ContextData_t *pData)
{
    return runTest( NULL, RULE_RESULT_ERROR, 0 );
}

/*==========================================================================*
**
**  test_BR_LLSC_6_19_002
**
**  Description     :
**      Test bypass - Device Transport Mode is RAIL
**
**  Parameters      :
**      pData           [I/O]   BR context data
**
**  Returns         :
**      TRUE                    test passed
**      FALSE                   test failed
**
**  Notes           :
**
**
**==========================================================================*/

int test_BR_LLSC_6_19_002(MYKI_BR_ContextData_t *pData)
{
    pData->InternalData.TransportMode = TRANSPORT_MODE_RAIL;

    return runTest( pData, RULE_RESULT_BYPASSED, 1 );
}

/*==========================================================================*
**
**  test_BR_LLSC_6_19_003
**
**  Description     :
**      Test bypass - No product in use
**      (transport mode is not RAIL)
**
**  Parameters      :
**      pData           [I/O]   BR context data
**
**  Returns         :
**      TRUE                    test passed
**      FALSE                   test failed
**
**  Notes           :
**
**
**==========================================================================*/

int test_BR_LLSC_6_19_003(MYKI_BR_ContextData_t *pData)
{
    CardImage_t CardImage;

    pData->InternalData.TransportMode       = TRANSPORT_MODE_BUS;
    pData->DynamicData.lineId               = 1;
    pData->DynamicData.stopId               = 1;

    if ( CreateCardImage_Empty( &CardImage ) < 0 )  // Sets product in use to (0)
    {
        return FALSE;
    }

    return runTest( pData, RULE_RESULT_BYPASSED, 2 );
}

/*==========================================================================*
**
**  test_BR_LLSC_6_19_004
**
**  Description     :
**      Test bypass - Last service provider is not RAIL
**      (transport mode is not RAIL)
**      (product in use)
**
**  Parameters      :
**      pData           [I/O]   BR context data
**
**  Returns         :
**      TRUE                    test passed
**      FALSE                   test failed
**
**  Notes           :
**
**
**==========================================================================*/

int test_BR_LLSC_6_19_004(MYKI_BR_ContextData_t *pData)
{
    CardImage_t CardImage;

    pData->InternalData.TransportMode       = TRANSPORT_MODE_BUS;
    pData->DynamicData.lineId               = 1;
    pData->DynamicData.stopId               = 1;

    if ( CreateCardImage_Empty( &CardImage ) < 0 )
    {
        return FALSE;
    }

    CreateProduct( &CardImage, 1, PRODUCT_ID_NHOUR, 1, 1, pData->DynamicData.currentDateTime + 60 );    // Don't care but make it sensible

    CardImage.pMYKI_TAProduct[ 0 ]->LastUsage.DateTime = pData->DynamicData.currentDateTime - 60;       // Don't care but make it sensible
    CardImage.pMYKI_TAProduct[ 0 ]->LastUsage.Location.EntryPointId = 0;    // Don't care
    CardImage.pMYKI_TAProduct[ 0 ]->LastUsage.Location.RouteId      = 0;    // Don't care
    CardImage.pMYKI_TAProduct[ 0 ]->LastUsage.Location.StopId       = 0;    // Don't care
    CardImage.pMYKI_TAProduct[ 0 ]->LastUsage.ProviderId            = 2;    // Non-rail
    CardImage.pMYKI_TAProduct[ 0 ]->LastUsage.Zone                  = 0;    // Don't care

    CardImage.pMYKI_TAControl->ProductInUse = 1;

    return runTest( pData, RULE_RESULT_BYPASSED, 3 );
}

/*==========================================================================*
**
**  test_BR_LLSC_6_19_005
**
**  Description     :
**      Test execute - product in use, last provider is rail, device is not rail
**
**  Parameters      :
**      pData           [I/O]   BR context data
**
**  Returns         :
**      TRUE                    test passed
**      FALSE                   test failed
**
**  Notes           :
**
**
**==========================================================================*/

int test_BR_LLSC_6_19_005(MYKI_BR_ContextData_t *pData)
{
    CardImage_t CardImage;
    int         rv = TRUE;

    pData->InternalData.TransportMode       = TRANSPORT_MODE_BUS;
    pData->DynamicData.lineId               = 1;
    pData->DynamicData.stopId               = 1;

    if ( CreateCardImage_Empty( &CardImage ) < 0 )
    {
        return FALSE;
    }

    CreateProduct( &CardImage, 1, PRODUCT_ID_NHOUR, 1, 1, pData->DynamicData.currentDateTime + 60 );    // Don't care but make it sensible

    CardImage.pMYKI_TAProduct[ 0 ]->LastUsage.DateTime = pData->DynamicData.currentDateTime - 60;       // Don't care but make it sensible
    CardImage.pMYKI_TAProduct[ 0 ]->LastUsage.Location.EntryPointId = 10500;    // Station ID
    CardImage.pMYKI_TAProduct[ 0 ]->LastUsage.Location.RouteId      = 0;        // Not set
    CardImage.pMYKI_TAProduct[ 0 ]->LastUsage.Location.StopId       = 0;        // Not set
    CardImage.pMYKI_TAProduct[ 0 ]->LastUsage.ProviderId            = 0;        // Rail provider
    CardImage.pMYKI_TAProduct[ 0 ]->LastUsage.Zone                  = 0;        // Don't care

    CardImage.pMYKI_TAControl->ProductInUse = 1;

    if ( ! runTest( pData, RULE_RESULT_EXECUTED, 0 ) )
    {
        return FALSE;
    }

    if ( pData->DynamicData.fareRouteIdIsValid != FALSE )
    {
        CsVerbose("test_BR_LLSC_6_19 - fareRouteIdIsValid NOT false as expected" );
        rv = FALSE;
    }

    if ( pData->DynamicData.isEarlyBirdTrip != FALSE )
    {
        CsVerbose("test_BR_LLSC_6_19 - isEarlyBirdTrip NOT false as expected" );
        rv = FALSE;
    }

    if ( pData->DynamicData.transportLocationsCount != 2 )
    {
        CsVerbose("test_BR_LLSC_6_19 - transportLocationsCount NOT (2) as expected" );
        rv = FALSE;
    }

    if ( pData->DynamicData.currentTripDirection != TRIP_DIRECTION_INBOUND )
    {
        CsVerbose("test_BR_LLSC_6_19 - currentTripDirection NOT inbound as expected" );
        rv = FALSE;
    }

    return rv;
}

