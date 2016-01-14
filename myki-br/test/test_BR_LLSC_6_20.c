/* -*- mode: c; tabs: 4; -*- */
/*=============================================================================
**
**    Vix Technology                   Licensed software
**    (C) 2013                         All rights reserved
**
**=============================================================================
**
**  Project/Product : MBU
**  Filename        : test_BR_LLSC_6_20.c
**  Author(s)       : David Purdie
**  Description     :
**      Implements BR_LLSC_6_20 business rule unit-tests.
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
    //ShowCardImage( "test1_before.txt" );
    int rv = BR_LLSC_6_20(pData);

    if ( rv != rc )
    {
        CsVerbose("test_BR_LLSC_6_20 - Unexpected exit code. Got:%d, Expected:%d\n", rv, rc );
        return FALSE;
    }

    if ( pData->ReturnedData.bypassCode != bc)
    {
        CsVerbose("test_BR_LLSC_6_20 - Unexpected ByPassCode. Got:%d, Expected:%d\n", pData->ReturnedData.bypassCode, bc);
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
**  test_BR_LLSC_6_20_001
**
**  Description     :
**      Resolve trip Zone range
**      Unit-test Execute conditions.
**              Test with NULL pData
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

int test_BR_LLSC_6_20_001(MYKI_BR_ContextData_t *pData)
{

    /*
    ** Test with bad data pointer
    */
    CsVerbose("BR_LLSC_6_20 - Test with NULL data pointer");
    if (BR_LLSC_6_20(NULL) != RULE_RESULT_ERROR)
    {
        return FALSE;
    }
    return TRUE;
}

/*==========================================================================*
**
**  test_BR_LLSC_6_20_002
**
**  Description     :
**      Resolve trip Zone range
**      Unit-test Execute conditions.
**              Insufficient transport locations
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

int test_BR_LLSC_6_20_002(MYKI_BR_ContextData_t *pData)
{
    /*
    **  Run the Test and examine the results
    */
    return runTest(pData, RULE_RESULT_BYPASSED, 1);
}

/*==========================================================================*
**
**  test_BR_LLSC_6_20_003
**
**  Description     :
**      Resolve trip Zone range
**      Unit-test Execute conditions.
**              Execute - All Locations in same Thin Zone
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

int test_BR_LLSC_6_20_003(MYKI_BR_ContextData_t *pData)
{
    CardImage_t CardImage;
    int rv = TRUE;

    /*
    **  Test with
    */
    if (CreateCardImage_Empty(&CardImage) < 0)
    {
        return FALSE;
    }
    CreateProduct( &CardImage, 1, PRODUCT_ID_NHOUR, 1, 1, pData->DynamicData.currentDateTime );
    CardImage.pMYKI_TAControl->ProductInUse = 1;

    /*
    ** Setup Transport locations
    **  All in Same Thin Zone
    */
    pData->DynamicData.transportLocationsCount = 2;

    pData->DynamicData.transportLocations[0].inner_zone = 3;
    pData->DynamicData.transportLocations[0].zone = 4;
    pData->DynamicData.transportLocations[0].outer_zone = 5;

    pData->DynamicData.transportLocations[1].inner_zone = 4;
    pData->DynamicData.transportLocations[1].zone = 5;
    pData->DynamicData.transportLocations[1].outer_zone = 6;

    /*
    **  Run the Test and examine the results
    */
    if (!runTest(pData, RULE_RESULT_EXECUTED, 303))
    {
        return FALSE;
    }

    /*
    ** Examine conditions
    */
    if (!pData->DynamicData.currentTripIsThinZone)
    {
        CsVerbose("test_BR_LLSC_6_20 - currentTripIsThinZone NOT set as expected" );
        rv = FALSE;
    }

    if (pData->DynamicData.currentTripZoneLow != 5)
    {
        CsVerbose("test_BR_LLSC_6_20 - currentTripZoneLow NOT as expected. Was:%d",pData->DynamicData.currentTripZoneLow );
        rv = FALSE;
    }

    if (pData->DynamicData.currentTripZoneHigh != 5)
    {
        CsVerbose("test_BR_LLSC_6_20 - currentTripZoneHigh NOT as expected. Was:%d",pData->DynamicData.currentTripZoneHigh );
        rv = FALSE;
    }

    if (pData->DynamicData.transportLocationsCount)
    {
        CsVerbose("test_BR_LLSC_6_20 - transportLocationsCount NOT zero as expected" );
        rv = FALSE;
    }

    return rv;
}

/*==========================================================================*
**
**  test_BR_LLSC_6_20_004
**
**  Description     :
**      Resolve trip Zone range
**      Unit-test Execute conditions.
**              Execute - All locations of the traversal are border stops along the same border(1)
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

int test_BR_LLSC_6_20_004(MYKI_BR_ContextData_t *pData)
{
    int rv = TRUE;
    CardImage_t CardImage;

    /*
    **  Test with
    */
    if (CreateCardImage_Empty(&CardImage) < 0)
    {
        return FALSE;
    }
    CreateProduct( &CardImage, 1, PRODUCT_ID_NHOUR, 1, 1, pData->DynamicData.currentDateTime );
    CardImage.pMYKI_TAControl->ProductInUse = 1;

    /*
    ** Setup Transport locations
    **  All locations of the traversal are border stops along the same border
    **      Same Low Border
    */
    pData->DynamicData.transportLocationsCount = 2;

    pData->DynamicData.transportLocations[0].inner_zone = 3;
    pData->DynamicData.transportLocations[0].zone = 4;
    pData->DynamicData.transportLocations[0].outer_zone = 4;

    pData->DynamicData.transportLocations[1].inner_zone = 3;
    pData->DynamicData.transportLocations[1].zone = 4;
    pData->DynamicData.transportLocations[1].outer_zone = 4;

    /*
    **  Run the Test and examine the results
    */
    if (!runTest(pData, RULE_RESULT_EXECUTED, 304))
    {
        return FALSE;
    }

    /*
    ** Examine conditions
    */
    if (pData->DynamicData.transportLocationsCount)
    {
        CsVerbose("test_BR_LLSC_6_20 - transportLocationsCount NOT zero as expected" );
        rv = FALSE;
    }

    if (pData->DynamicData.currentTripZoneLow != 3)
    {
        CsVerbose("test_BR_LLSC_6_20 - currentTripZoneLow NOT as expected. Was:%d",pData->DynamicData.currentTripZoneLow );
        rv = FALSE;
    }

    if (pData->DynamicData.currentTripZoneHigh != 3)
    {
        CsVerbose("test_BR_LLSC_6_20 - currentTripZoneHigh NOT as expected. Was:%d",pData->DynamicData.currentTripZoneHigh );
        rv = FALSE;
    }

    return rv;
}

/*==========================================================================*
**
**  test_BR_LLSC_6_20_005
**
**  Description     :
**      Resolve trip Zone range
**      Unit-test Execute conditions.
**              Execute - All locations of the traversal are border stops along the same border(2)
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

int test_BR_LLSC_6_20_005(MYKI_BR_ContextData_t *pData)
{
    int rv = TRUE;
    CardImage_t CardImage;

    /*
    **  Test with
    */
    if (CreateCardImage_Empty(&CardImage) < 0)
    {
        return FALSE;
    }
    CreateProduct( &CardImage, 1, PRODUCT_ID_NHOUR, 1, 1, pData->DynamicData.currentDateTime );
    CardImage.pMYKI_TAControl->ProductInUse = 1;

    /*
    ** Setup Transport locations
    **  All locations of the traversal are border stops along the same border
    **      Same High Border
    */
    pData->DynamicData.transportLocationsCount = 2;

    pData->DynamicData.transportLocations[0].inner_zone = 3;
    pData->DynamicData.transportLocations[0].zone = 3;
    pData->DynamicData.transportLocations[0].outer_zone = 4;

    pData->DynamicData.transportLocations[1].inner_zone = 3;
    pData->DynamicData.transportLocations[1].zone = 3;
    pData->DynamicData.transportLocations[1].outer_zone = 4;

    /*
    **  Run the Test and examine the results
    */
    if (!runTest(pData, RULE_RESULT_EXECUTED, 304))
    {
        return FALSE;
    }

    /*
    ** Examine conditions
    */
    if (pData->DynamicData.transportLocationsCount)
    {
        CsVerbose("test_BR_LLSC_6_20 - transportLocationsCount NOT zero as expected" );
        rv = FALSE;
    }

    if (pData->DynamicData.currentTripZoneLow != 3)
    {
        CsVerbose("test_BR_LLSC_6_20 - currentTripZoneLow NOT as expected. Was:%d",pData->DynamicData.currentTripZoneLow );
        rv = FALSE;
    }

    if (pData->DynamicData.currentTripZoneHigh != 3)
    {
        CsVerbose("test_BR_LLSC_6_20 - currentTripZoneHigh NOT as expected. Was:%d",pData->DynamicData.currentTripZoneHigh );
        rv = FALSE;
    }

    return rv;
}

/*==========================================================================*
**
**  test_BR_LLSC_6_20_006
**
**  Description     :
**      Resolve trip Zone range
**      Unit-test Execute conditions.
**              Execute - All locations of the traversal are border stops along the same border(3)
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

int test_BR_LLSC_6_20_006(MYKI_BR_ContextData_t *pData)
{
    int rv = TRUE;
    CardImage_t CardImage;

    /*
    **  Test with
    */
    if (CreateCardImage_Empty(&CardImage) < 0)
    {
        return FALSE;
    }
    CreateProduct( &CardImage, 1, PRODUCT_ID_NHOUR, 1, 1, pData->DynamicData.currentDateTime );
    CardImage.pMYKI_TAControl->ProductInUse = 1;

    /*
    ** Setup Transport locations
    **  All locations of the traversal are border stops along the same border
    **      Mixed Border
    */
    pData->DynamicData.transportLocationsCount = 2;

    pData->DynamicData.transportLocations[0].inner_zone = 3;
    pData->DynamicData.transportLocations[0].zone = 4;
    pData->DynamicData.transportLocations[0].outer_zone = 4;

    pData->DynamicData.transportLocations[1].inner_zone = 3;
    pData->DynamicData.transportLocations[1].zone = 3;
    pData->DynamicData.transportLocations[1].outer_zone = 4;

    /*
    **  Run the Test and examine the results
    */
    if (!runTest(pData, RULE_RESULT_EXECUTED, 304))
    {
        return FALSE;
    }

    /*
    ** Examine conditions
    */
    if (pData->DynamicData.transportLocationsCount)
    {
        CsVerbose("test_BR_LLSC_6_20 - transportLocationsCount NOT zero as expected" );
        rv = FALSE;
    }

    if (pData->DynamicData.currentTripZoneLow != 3)
    {
        CsVerbose("test_BR_LLSC_6_20 - currentTripZoneLow NOT as expected. Was:%d",pData->DynamicData.currentTripZoneLow );
        rv = FALSE;
    }

    if (pData->DynamicData.currentTripZoneHigh != 3)
    {
        CsVerbose("test_BR_LLSC_6_20 - currentTripZoneHigh NOT as expected. Was:%d",pData->DynamicData.currentTripZoneHigh );
        rv = FALSE;
    }


    return rv;
}

/*==========================================================================*
**
**  test_BR_LLSC_6_20_007
**
**  Description     :
**      Resolve trip Zone range
**      Unit-test Execute conditions.
**              Execute - Force ScanOff (flag already set)
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

int test_BR_LLSC_6_20_007(MYKI_BR_ContextData_t *pData)
{
    int rv = TRUE;
    CardImage_t CardImage;

    /*
    **  Test with
    */
    if (CreateCardImage_Empty(&CardImage) < 0)
    {
        return FALSE;
    }
    CreateProduct( &CardImage, 1, PRODUCT_ID_NHOUR, 1, 1, pData->DynamicData.currentDateTime );
    CardImage.pMYKI_TAControl->ProductInUse = 1;

    /*
    ** Setup Transport locations
    **      Not a Border Trip
    */
    pData->DynamicData.transportLocationsCount = 2;

    pData->DynamicData.transportLocations[0].inner_zone = 3;
    pData->DynamicData.transportLocations[0].zone = 3;
    pData->DynamicData.transportLocations[0].outer_zone = 4;

    pData->DynamicData.transportLocations[1].inner_zone = 4;
    pData->DynamicData.transportLocations[1].zone = 4;
    pData->DynamicData.transportLocations[1].outer_zone = 5;

    /*
    ** Forced Scan Off
    */
     pData->DynamicData.isForcedScanOff = TRUE;

    /*
    **  Run the Test and examine the results
    */
    if (!runTest(pData, RULE_RESULT_EXECUTED, 102))
    {
        return FALSE;
    }

    /*
    ** Examine conditions
    */
    if (pData->DynamicData.transportLocationsCount)
    {
        CsVerbose("test_BR_LLSC_6_20 - transportLocationsCount NOT zero as expected" );
        rv = FALSE;
    }

    if (pData->DynamicData.currentTripZoneLow != 4)
    {
        CsVerbose("test_BR_LLSC_6_20 - currentTripZoneLow NOT as expected. Was:%d",pData->DynamicData.currentTripZoneLow );
        rv = FALSE;
    }

    if (pData->DynamicData.currentTripZoneHigh != 4)
    {
        CsVerbose("test_BR_LLSC_6_20 - currentTripZoneHigh NOT as expected. Was:%d",pData->DynamicData.currentTripZoneHigh );
        rv = FALSE;
    }

    return rv;
}

/*==========================================================================*
**
**  test_BR_LLSC_6_20_008
**
**  Description     :
**      Resolve trip Zone range
**      Unit-test Execute conditions.
**              Execute - Force ScanOff (scan-on rail, scan-off not rail)
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

int test_BR_LLSC_6_20_008(MYKI_BR_ContextData_t *pData)
{
    int rv = TRUE;
    CardImage_t CardImage;

    pData->InternalData.TransportMode       = TRANSPORT_MODE_BUS;
    pData->DynamicData.lineId               = 1;
    pData->DynamicData.stopId               = 1;

    /*
    **  Test with
    */

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

    /*
    ** Setup Transport locations
    **      Not a Border Trip
    */
    pData->DynamicData.transportLocationsCount = 2;

    pData->DynamicData.transportLocations[0].inner_zone = 3;
    pData->DynamicData.transportLocations[0].zone = 3;
    pData->DynamicData.transportLocations[0].outer_zone = 4;

    pData->DynamicData.transportLocations[1].inner_zone = 4;
    pData->DynamicData.transportLocations[1].zone = 4;
    pData->DynamicData.transportLocations[1].outer_zone = 5;

    /*
    ** Forced Scan Off not set (testing scan-on/off modes of transport instead)
    */
     pData->DynamicData.isForcedScanOff = FALSE;

    /*
    **  Run the Test and examine the results
    */
    if (!runTest(pData, RULE_RESULT_EXECUTED, 102))
    {
        return FALSE;
    }

    /*
    ** Examine conditions
    */
    if (pData->DynamicData.transportLocationsCount)
    {
        CsVerbose("test_BR_LLSC_6_20 - transportLocationsCount NOT zero as expected" );
        rv = FALSE;
    }

    if (pData->DynamicData.currentTripZoneLow != 4)
    {
        CsVerbose("test_BR_LLSC_6_20 - currentTripZoneLow NOT as expected. Was:%d",pData->DynamicData.currentTripZoneLow );
        rv = FALSE;
    }

    if (pData->DynamicData.currentTripZoneHigh != 4)
    {
        CsVerbose("test_BR_LLSC_6_20 - currentTripZoneHigh NOT as expected. Was:%d",pData->DynamicData.currentTripZoneHigh );
        rv = FALSE;
    }

    return rv;
}

/*==========================================================================*
**
**  test_BR_LLSC_6_20_009
**
**  Description     :
**      Resolve trip Zone range
**      Unit-test Execute conditions.
**              Execute - OffPeak
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

int test_BR_LLSC_6_20_009(MYKI_BR_ContextData_t *pData)
{
    int rv = TRUE;
    CardImage_t CardImage;
    MYKI_CD_DifferentialPricing_t differentialPricing;

    /*
    **  Test with
    */
    if (CreateCardImage_Empty(&CardImage) < 0)
    {
        return FALSE;
    }
    CreateProduct( &CardImage, 1, PRODUCT_ID_NHOUR, 1, 1, pData->DynamicData.currentDateTime );
    CardImage.pMYKI_TAControl->ProductInUse = 1;

    /*
    ** Setup Transport locations
    **      Not a Border Trip
    */
    pData->DynamicData.transportLocationsCount = 2;

    pData->DynamicData.transportLocations[0].inner_zone = 3;
    pData->DynamicData.transportLocations[0].zone = 3;
    pData->DynamicData.transportLocations[0].outer_zone = 4;

    pData->DynamicData.transportLocations[1].inner_zone = 4;
    pData->DynamicData.transportLocations[1].zone = 4;
    pData->DynamicData.transportLocations[1].outer_zone = 5;

    /*
    ** Setup a Differential Price to be processed by the Rule
    */
    differentialPricing.discount_type = MYKI_CD_DISCOUNT_TYPE_PERCENT;
    differentialPricing.applied_discount = 33;
    strcpy(differentialPricing.short_desc,"Test 008");

    MYKI_CD_setDifferentialPriceStructure(&differentialPricing);

    /*
    **  Run the Test and examine the results
    */
    if (!runTest(pData, RULE_RESULT_EXECUTED, 202))
    {
        return FALSE;
    }

    /*
    ** Examine conditions
    */
    if (pData->DynamicData.transportLocationsCount)
    {
        CsVerbose("test_BR_LLSC_6_20 - transportLocationsCount NOT zero as expected" );
        rv = FALSE;
    }

    if (pData->DynamicData.currentTripZoneLow != 4)
    {
        CsVerbose("test_BR_LLSC_6_20 - currentTripZoneLow NOT as expected. Was:%d",pData->DynamicData.currentTripZoneLow );
        rv = FALSE;
    }

    if (pData->DynamicData.currentTripZoneHigh != 4)
    {
        CsVerbose("test_BR_LLSC_6_20 - currentTripZoneHigh NOT as expected. Was:%d",pData->DynamicData.currentTripZoneHigh );
        rv = FALSE;
    }

    if (pData->DynamicData.offPeakDiscountRate != 33)
    {
        CsVerbose("test_BR_LLSC_6_20 - offPeakDiscountRate NOT as expected" );
        rv = FALSE;
    }

    if (pData->DynamicData.isOffPeak != TRUE)
    {
        CsVerbose("test_BR_LLSC_6_20 - isOffPeak NOT as expected" );
        rv = FALSE;
    }

    return rv;
}

/*==========================================================================*
**
**  test_BR_LLSC_6_20_010
**
**  Description     :
**      Resolve trip Zone range
**      Unit-test Execute conditions.
**              Execute - Plain
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

int test_BR_LLSC_6_20_010(MYKI_BR_ContextData_t *pData)
{
    int rv = TRUE;
    CardImage_t CardImage;

    /*
    **  Test with
    */
    if (CreateCardImage_Empty(&CardImage) < 0)
    {
        return FALSE;
    }
    CreateProduct( &CardImage, 1, PRODUCT_ID_NHOUR, 1, 1, pData->DynamicData.currentDateTime );
    CardImage.pMYKI_TAControl->ProductInUse = 1;

    /*
    ** Setup Transport locations
    **      Not a Border Trip
    */
    pData->DynamicData.transportLocationsCount = 3;

    pData->DynamicData.transportLocations[0].inner_zone = 3;
    pData->DynamicData.transportLocations[0].zone = 3;
    pData->DynamicData.transportLocations[0].outer_zone = 4;

    pData->DynamicData.transportLocations[1].inner_zone = 4;
    pData->DynamicData.transportLocations[1].zone = 4;
    pData->DynamicData.transportLocations[1].outer_zone = 5;

    pData->DynamicData.transportLocations[2].inner_zone = 5;
    pData->DynamicData.transportLocations[2].zone = 6;
    pData->DynamicData.transportLocations[2].outer_zone = 6;

    /*
    **  Run the Test and examine the results
    */
    if (!runTest(pData, RULE_RESULT_EXECUTED, 302))
    {
        return FALSE;
    }

    /*
    ** Examine conditions
    */
    if (pData->DynamicData.transportLocationsCount)
    {
        CsVerbose("test_BR_LLSC_6_20 - transportLocationsCount NOT zero as expected" );
        rv = FALSE;
    }

    if (pData->DynamicData.currentTripZoneLow != 4)
    {
        CsVerbose("test_BR_LLSC_6_20 - currentTripZoneLow NOT as expected. Was:%d",pData->DynamicData.currentTripZoneLow );
        rv = FALSE;
    }

    if (pData->DynamicData.currentTripZoneHigh != 5)
    {
        CsVerbose("test_BR_LLSC_6_20 - currentTripZoneHigh NOT as expected. Was:%d",pData->DynamicData.currentTripZoneHigh );
        rv = FALSE;
    }

    return rv;
}

