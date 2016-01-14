/* -*- mode: c; tabs: 4; -*- */
/*=============================================================================
**
**    Vix Technology                   Licensed software
**    (C) 2013                         All rights reserved
**
**=============================================================================
**
**  Project/Product : MBU
**  Filename        : test_BR_LLSC_6_21.c
**  Author(s)       : David Purdie
**  Description     :
**      Implements BR_LLSC_6_21 business rule unit-tests.
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
    int rv = BR_LLSC_6_21( pData );

    if ( rv != rc )
    {
        CsVerbose("test_BR_LLSC_6_21 - Unexpected exit code. Got:%d, Expected:%d\n", rv, rc );
        return FALSE;
    }

    if ( pData && pData->ReturnedData.bypassCode != bc )
    {
        CsVerbose("test_BR_LLSC_6_21 - Unexpected ByPassCode. Got:%d, Expected:%d\n", pData->ReturnedData.bypassCode, bc);
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

/*----------------------------------------------------------------------------
** FUNCTION           : SetupStandardEnvironment_6_21
**
** DESCRIPTION        : Set up the standard context and card image for this test
**
**
** INPUTS             : pCardImage              - Card Image Data
**
** RETURNS            : Nothing
**
----------------------------------------------------------------------------*/

static  void    SetupStandardEnvironment_6_21( MYKI_BR_ContextData_t *pData, CardImage_t *pCardImage )
{
    pData->InternalData.TransportMode           = TRANSPORT_MODE_BUS;
    pData->DynamicData.lineId                   = 1;
    pData->DynamicData.stopId                   = 1;
    pData->DynamicData.locationDataUnavailable  = TRUE;
    pData->DynamicData.entryPointId             = 123;
    pData->StaticData.serviceProviderId         = ProviderId_BUS;

    if ( CreateCardImage_Empty( pCardImage ) < 0 )  // Sets product in use to (0)
    {
        return;
    }

    CreateProduct( pCardImage, 1, PRODUCT_ID_NHOUR, 1, 1, pData->DynamicData.currentDateTime + 60 );    // Unexpired product of any type

    pCardImage->pMYKI_TAProduct[ 0 ]->LastUsage.DateTime = pData->DynamicData.currentDateTime - 60;       // Don't care but make it sensible
    pCardImage->pMYKI_TAProduct[ 0 ]->LastUsage.Location.EntryPointId = 123;              // Vehicle Id
    pCardImage->pMYKI_TAProduct[ 0 ]->LastUsage.Location.RouteId      = 1;                // Route
    pCardImage->pMYKI_TAProduct[ 0 ]->LastUsage.Location.StopId       = 1;                // Stop
    pCardImage->pMYKI_TAProduct[ 0 ]->LastUsage.ProviderId            = ProviderId_BUS;   // Bus provider
    pCardImage->pMYKI_TAProduct[ 0 ]->LastUsage.Zone                  = 0;                // Don't care

    pCardImage->pMYKI_TAControl->ProductInUse = 1;
    pCardImage->pMYKI_TAControl->ProvisionalFare = 1;
}

/*==========================================================================*
**
**  test_BR_LLSC_6_21_001
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

int test_BR_LLSC_6_21_001(MYKI_BR_ContextData_t *pData)
{
    return runTest( NULL, RULE_RESULT_ERROR, 0 );
}

/*==========================================================================*
**
**  test_BR_LLSC_6_21_002
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

int test_BR_LLSC_6_21_002(MYKI_BR_ContextData_t *pData)
{
    CardImage_t CardImage;

    SetupStandardEnvironment_6_21( pData, &CardImage );

    pData->InternalData.TransportMode           = TRANSPORT_MODE_RAIL;

    return runTest( pData, RULE_RESULT_BYPASSED, 2 );
}

/*==========================================================================*
**
**  test_BR_LLSC_6_21_003
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

int test_BR_LLSC_6_21_003(MYKI_BR_ContextData_t *pData)
{
    CardImage_t CardImage;

    SetupStandardEnvironment_6_21( pData, &CardImage );

    CardImage.pMYKI_TAControl->ProductInUse = 0;

    return runTest( pData, RULE_RESULT_BYPASSED, 1 );
}

/*==========================================================================*
**
**  test_BR_LLSC_6_21_004
**
**  Description     :
**      Test bypass - Location data is available
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

int test_BR_LLSC_6_21_004(MYKI_BR_ContextData_t *pData)
{
    CardImage_t CardImage;

    SetupStandardEnvironment_6_21( pData, &CardImage );

    pData->DynamicData.locationDataUnavailable  = FALSE;

    return runTest( pData, RULE_RESULT_BYPASSED, 3 );
}

/*==========================================================================*
**
**  test_BR_LLSC_6_21_005
**
**  Description     :
**      Test bypass - Location data is available
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

int test_BR_LLSC_6_21_005(MYKI_BR_ContextData_t *pData)
{
    CardImage_t CardImage;

    SetupStandardEnvironment_6_21( pData, &CardImage );

    CardImage.pMYKI_TAProduct[ 0 ]->EndDateTime        = pData->DynamicData.currentDateTime - 60;     // Expired

    return runTest( pData, RULE_RESULT_BYPASSED, 7 );
}

/*==========================================================================*
**
**  test_BR_LLSC_6_21_005
**
**  Description     :
**      Test bypass - Location data is available
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

int test_BR_LLSC_6_21_006(MYKI_BR_ContextData_t *pData)
{
    CardImage_t CardImage;

    SetupStandardEnvironment_6_21( pData, &CardImage );

    CardImage.pMYKI_TAControl->ProvisionalFare = 0;

    return runTest( pData, RULE_RESULT_BYPASSED, 5 );
}

/*==========================================================================*
**
**  test_BR_LLSC_6_21_007
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

int test_BR_LLSC_6_21_007(MYKI_BR_ContextData_t *pData)
{
    CardImage_t CardImage;

    SetupStandardEnvironment_6_21( pData, &CardImage );

    CardImage.pMYKI_TAProduct[ 0 ]->LastUsage.Location.EntryPointId = 124;              // Vehicle Id (mismatched)

    return runTest( pData, RULE_RESULT_BYPASSED, 4 );
}

/*==========================================================================*
**
**  test_BR_LLSC_6_21_008
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

int test_BR_LLSC_6_21_008(MYKI_BR_ContextData_t *pData)
{
    CardImage_t CardImage;

    SetupStandardEnvironment_6_21( pData, &CardImage );

    CardImage.pMYKI_TAProduct[ 0 ]->LastUsage.ProviderId            = ProviderId_TRAM;  // Tram provider (mismatched)

    return runTest( pData, RULE_RESULT_BYPASSED, 4 );
}

/*==========================================================================*
**
**  test_BR_LLSC_6_21_009
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

int test_BR_LLSC_6_21_009(MYKI_BR_ContextData_t *pData)
{
    CardImage_t CardImage;
    int         rv = TRUE;

    SetupStandardEnvironment_6_21( pData, &CardImage );

    if ( ! runTest( pData, RULE_RESULT_EXECUTED, 0 ) )
    {
        return FALSE;
    }

    if ( pData->DynamicData.fareRouteIdIsValid != FALSE )
    {
        CsVerbose("test_BR_LLSC_6_21 - fareRouteIdIsValid NOT false as expected" );
        rv = FALSE;
    }

    if ( pData->DynamicData.isEarlyBirdTrip != FALSE )
    {
        CsVerbose("test_BR_LLSC_6_21 - isEarlyBirdTrip NOT false as expected" );
        rv = FALSE;
    }

    if ( pData->DynamicData.transportLocationsCount != 2 )
    {
        CsVerbose("test_BR_LLSC_6_21 - transportLocationsCount NOT (2) as expected" );
        rv = FALSE;
    }

    if ( pData->DynamicData.currentTripDirection != TRIP_DIRECTION_UNKNOWN )
    {
        CsVerbose("test_BR_LLSC_6_21 - currentTripDirection NOT unknown as expected" );
        rv = FALSE;
    }

    return rv;
}

