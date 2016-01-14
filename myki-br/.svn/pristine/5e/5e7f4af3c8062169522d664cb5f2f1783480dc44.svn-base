/* -*- mode: c; tabs: 4; -*- */
/*=============================================================================
**
**    Vix Technology                   Licensed software
**    (C) 2014                         All rights reserved
**
**=============================================================================
**
**  Project/Product : MBU
**  Filename        : test_BR_LLSC_6_22.c
**  Author(s)       : Morgan Dell
**  Description     :
**      Implements BR_LLSC_6_22 business rule unit-tests.
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

#define DirIndex_ProductInUse           1

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
    int rv = BR_LLSC_6_22(pData);

    if ( rv != rc )
    {
        CsVerbose("test_BR_LLSC_6_22 - Unexpected exit code. Got:%d, Expected:%d\n", rv, rc );
        return FALSE;
    }

    if ( (pData != NULL) && (pData->ReturnedData.bypassCode != bc) )
    {
        CsVerbose("test_BR_LLSC_6_22 - Unexpected ByPassCode. Got:%d, Expected:%d\n", pData->ReturnedData.bypassCode, bc);
        return FALSE;
    }
    return TRUE;
}

/*----------------------------------------------------------------------------
** FUNCTION           : ResetCardAndContext
**
** DESCRIPTION        : Reset the card and context data to a known state
**
**
** INPUTS             :
**      pCardImage      [I/O]   Card Image Data
**      pData           [I/O]   BR context data
**
** RETURNS            : Nothing
**
----------------------------------------------------------------------------*/

static  void    ResetCardAndContext( MYKI_BR_ContextData_t *pData, CardImage_t *pCardImage )
{
    pData->InternalData.TransportMode           = TRANSPORT_MODE_BUS;
    pData->DynamicData.locationDataUnavailable  = TRUE;
    pData->DynamicData.isForcedScanOff          = FALSE;
    pData->ReturnedData.bypassCode              = 0;

    //  Create product

    pCardImage->pMYKI_TAProduct[ DirIndex_ProductInUse - 1 ]->ZoneLow                           = 1;
    pCardImage->pMYKI_TAProduct[ DirIndex_ProductInUse - 1 ]->ZoneHigh                          = 2;
    pCardImage->pMYKI_TAProduct[ DirIndex_ProductInUse - 1 ]->EndDateTime                       = pData->DynamicData.currentDateTime;
    pCardImage->pMYKI_TAProduct[ DirIndex_ProductInUse - 1 ]->LastUsage.Location.EntryPointId   = pData->DynamicData.entryPointId;
    pCardImage->pMYKI_TAProduct[ DirIndex_ProductInUse - 1 ]->LastUsage.ProviderId              = pData->DynamicData.originatingInformation.serviceProviderId;
    pCardImage->pMYKI_TAControl->ProductInUse                                                   = DirIndex_ProductInUse;
    pCardImage->pMYKI_TAControl->ProvisionalFare                                                = 0;

    //  Create directory entry for product

    pCardImage->pMYKI_TAControl->Directory[ DirIndex_ProductInUse ].Status                      = TAPP_CONTROL_DIRECTORY_STATUS_ACTIVATED;
    pCardImage->pMYKI_TAControl->Directory[ DirIndex_ProductInUse ].ProductId                   = PRODUCT_ID_NHOUR;
    pCardImage->pMYKI_TAControl->Directory[ DirIndex_ProductInUse ].SerialNo                    = pCardImage->pMYKI_TAControl->NextProductSerialNo++;
}


/*==========================================================================*
**
**  test_BR_LLSC_6_22_001
**
**  Description     :
**      Location Data Unavailable at both scan on & scan off
**      Unit-test pre-conditions.
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

int test_BR_LLSC_6_22_001(MYKI_BR_ContextData_t *pData)
{
    /*
     * Test with bad data pointer
     */
    CsVerbose("BR_LLSC_6_22 - Test with NULL data pointer");
    return runTest(NULL, RULE_RESULT_ERROR, 0);
}

/*==========================================================================*
**
**  test_BR_LLSC_6_22_002
**
**  Description     :
**      Location Data Unavailable at both scan on & scan off
**      Unit-test pre-conditions.
**              No products in use
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

int test_BR_LLSC_6_22_002(MYKI_BR_ContextData_t *pData)
{
    /*
     * Test with no product in use
     */
    CardImage_t CardImage;
    if (CreateCardImage_Empty(&CardImage) < 0)
    {
        return FALSE;
    }
    ResetCardAndContext(pData, &CardImage);

    CardImage.pMYKI_TAControl->ProductInUse = 0;

    return runTest(pData, RULE_RESULT_BYPASSED, BYPASS_CODE(6, 22, 1, 0));
}

/*==========================================================================*
**
**  test_BR_LLSC_6_22_003
**
**  Description     :
**      Location Data Unavailable at both scan on & scan off
**      Unit-test pre-conditions.
**              Rail transport mode
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

int test_BR_LLSC_6_22_003(MYKI_BR_ContextData_t *pData)
{
    /*
     * Test with rail transport mode
     */
    CardImage_t CardImage;
    if (CreateCardImage_Empty(&CardImage) < 0)
    {
        return FALSE;
    }
    ResetCardAndContext(pData, &CardImage);

    pData->InternalData.TransportMode = TRANSPORT_MODE_RAIL;

    return runTest(pData, RULE_RESULT_BYPASSED, BYPASS_CODE(6, 22, 2, 0));
}

/*==========================================================================*
**
**  test_BR_LLSC_6_22_004
**
**  Description     :
**      Location Data Unavailable at both scan on & scan off
**      Unit-test pre-conditions.
**              Location data unavailable
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

int test_BR_LLSC_6_22_004(MYKI_BR_ContextData_t *pData)
{
    /*
     * Test with location data available
     */
    CardImage_t CardImage;
    if (CreateCardImage_Empty(&CardImage) < 0)
    {
        return FALSE;
    }
    ResetCardAndContext(pData, &CardImage);

    pData->DynamicData.locationDataUnavailable = FALSE;

    return runTest(pData, RULE_RESULT_BYPASSED, BYPASS_CODE(6, 22, 3, 0));
}

/*==========================================================================*
**
**  test_BR_LLSC_6_22_005
**
**  Description     :
**      Location Data Unavailable at both scan on & scan off
**      Unit-test pre-conditions.
**              Entry point mismatch
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

int test_BR_LLSC_6_22_005(MYKI_BR_ContextData_t *pData)
{
    /*
     * Entry point mismatch
     */
    CardImage_t CardImage;
    if (CreateCardImage_Empty(&CardImage) < 0)
    {
        return FALSE;
    }
    ResetCardAndContext(pData, &CardImage);

    CardImage.pMYKI_TAProduct[ DirIndex_ProductInUse - 1 ]->LastUsage.Location.EntryPointId++;

    return runTest(pData, RULE_RESULT_BYPASSED, BYPASS_CODE(6, 22, 4, 1));
}

/*==========================================================================*
**
**  test_BR_LLSC_6_22_006
**
**  Description     :
**      Location Data Unavailable at both scan on & scan off
**      Unit-test pre-conditions.
**              Entry service provider mismatch
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

int test_BR_LLSC_6_22_006(MYKI_BR_ContextData_t *pData)
{
    /*
     * Entry service provider mismatch
     */
    CardImage_t CardImage;
    if (CreateCardImage_Empty(&CardImage) < 0)
    {
        return FALSE;
    }
    ResetCardAndContext(pData, &CardImage);

    CardImage.pMYKI_TAProduct[ DirIndex_ProductInUse - 1 ]->LastUsage.ProviderId++;

    return runTest(pData, RULE_RESULT_BYPASSED, BYPASS_CODE(6, 22, 4, 2));
}

/*==========================================================================*
**
**  test_BR_LLSC_6_22_007
**
**  Description     :
**      Location Data Unavailable at both scan on & scan off
**      Unit-test pre-conditions.
**              Provisional fare not zero
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

int test_BR_LLSC_6_22_007(MYKI_BR_ContextData_t *pData)
{
    /*
     * Provisional fare not zero
     */
    CardImage_t CardImage;
    if (CreateCardImage_Empty(&CardImage) < 0)
    {
        return FALSE;
    }
    ResetCardAndContext(pData, &CardImage);

    CardImage.pMYKI_TAControl->ProvisionalFare = 1;

    return runTest(pData, RULE_RESULT_BYPASSED, BYPASS_CODE(6, 22, 5, 0));
}

/*==========================================================================*
**
**  test_BR_LLSC_6_22_008
**
**  Description     :
**      Location Data Unavailable at both scan on & scan off
**      Unit-test pre-conditions.
**              Is forced scan off
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

int test_BR_LLSC_6_22_008(MYKI_BR_ContextData_t *pData)
{
    /*
     * Is forced scan off
     */
    CardImage_t CardImage;
    if (CreateCardImage_Empty(&CardImage) < 0)
    {
        return FALSE;
    }
    ResetCardAndContext(pData, &CardImage);

    pData->DynamicData.isForcedScanOff = TRUE;

    return runTest(pData, RULE_RESULT_BYPASSED, BYPASS_CODE(6, 22, 6, 0));
}

/*==========================================================================*
**
**  test_BR_LLSC_6_22_009
**
**  Description     :
**      Location Data Unavailable at both scan on & scan off
**      Unit-test pre-conditions.
**              Product expired
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

int test_BR_LLSC_6_22_009(MYKI_BR_ContextData_t *pData)
{
    /*
     * Product expired
     */
    CardImage_t CardImage;
    if (CreateCardImage_Empty(&CardImage) < 0)
    {
        return FALSE;
    }
    ResetCardAndContext(pData, &CardImage);

    CardImage.pMYKI_TAProduct[ 0 ]->EndDateTime = pData->DynamicData.currentDateTime - 1;

    return runTest(pData, RULE_RESULT_BYPASSED, BYPASS_CODE(6, 22, 7, 0));
}

/*==========================================================================*
**
**  test_BR_LLSC_6_22_010
**
**  Description     :
**      Location Data Unavailable at both scan on & scan off
**      Unit-test check execution
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

int test_BR_LLSC_6_22_010(MYKI_BR_ContextData_t *pData)
{
    CardImage_t CardImage;
    if (CreateCardImage_Empty(&CardImage) < 0)
    {
        return FALSE;
    }
    ResetCardAndContext(pData, &CardImage);

    if ( !runTest(pData, RULE_RESULT_EXECUTED, 0) )
    {
        return FALSE;
    }

    if ( pData->DynamicData.isEarlyBirdTrip != FALSE )
    {
        CsVerbose("test_BR_LLSC_6_22_010 - bad isEarlyBirdTrip");
        return FALSE;
    }

    if ( pData->DynamicData.fareRouteIdIsValid != FALSE )
    {
        CsVerbose("test_BR_LLSC_6_22_010 - bad fareRouteIdIsValid");
        return FALSE;
    }

    if ( pData->DynamicData.currentTripDirection != TRIP_DIRECTION_UNKNOWN )
    {
        CsVerbose("test_BR_LLSC_6_22_010 - bad currentTripDirection");
        return FALSE;
    }

    return TRUE;
}

