/* -*- mode: c; tabs: 4; -*- */
/*=============================================================================
**
**    Vix Technology                   Licensed software
**    (C) 2014                         All rights reserved
**
**=============================================================================
**
**  Project/Product : MBU
**  Filename        : test_BR_LLSC_6_23.c
**  Author(s)       : Morgan Dell
**  Description     :
**      Implements BR_LLSC_6_23 business rule unit-tests.
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

#define DirIndex_ProductInUse               1
#define DirIndex_ActiveEpass                2
#define DirIndex_InactiveEpass              3

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
    int rv = BR_LLSC_6_23(pData);

    if ( rv != rc )
    {
        CsVerbose("test_BR_LLSC_6_23 - Unexpected exit code. Got:%d, Expected:%d\n", rv, rc );
        return FALSE;
    }

    if ( (pData != NULL) && (pData->ReturnedData.bypassCode != bc) )
    {
        CsVerbose("test_BR_LLSC_6_23 - Unexpected ByPassCode. Got:%d, Expected:%d\n", pData->ReturnedData.bypassCode, bc);
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
    pData->InternalData.TransportMode               = TRANSPORT_MODE_BUS;
    pData->DynamicData.locationDataUnavailable      = FALSE;
    pData->DynamicData.isForcedScanOff              = FALSE;
    pData->DynamicData.lineId                       = 1;
    pData->DynamicData.stopId                       = 5;
    pData->DynamicData.currentInnerZone             = 2;
    pData->DynamicData.currentOuterZone             = 3;
    pData->ReturnedData.bypassCode                  = 0;

    //  Create products

    pCardImage->pMYKI_TAProduct[ DirIndex_ProductInUse  - 1 ]->ZoneLow                          = 1;
    pCardImage->pMYKI_TAProduct[ DirIndex_ProductInUse  - 1 ]->ZoneHigh                         = 1;
    pCardImage->pMYKI_TAProduct[ DirIndex_ProductInUse  - 1 ]->EndDateTime                      = pData->DynamicData.currentDateTime;
    pCardImage->pMYKI_TAProduct[ DirIndex_ProductInUse  - 1 ]->LastUsage.Location.EntryPointId  = pData->DynamicData.entryPointId;
    pCardImage->pMYKI_TAProduct[ DirIndex_ProductInUse  - 1 ]->LastUsage.ProviderId             = pData->StaticData.serviceProviderId;

    pCardImage->pMYKI_TAProduct[ DirIndex_ActiveEpass   - 1 ]->ZoneLow                          = pData->DynamicData.currentInnerZone;
    pCardImage->pMYKI_TAProduct[ DirIndex_ActiveEpass   - 1 ]->ZoneHigh                         = pData->DynamicData.currentOuterZone;

    pCardImage->pMYKI_TAProduct[ DirIndex_InactiveEpass - 1 ]->ZoneLow                          = pData->DynamicData.currentInnerZone;
    pCardImage->pMYKI_TAProduct[ DirIndex_InactiveEpass - 1 ]->ZoneHigh                         = pData->DynamicData.currentOuterZone;

    pCardImage->pMYKI_TAControl->ProductInUse                                                   = DirIndex_ProductInUse;
    pCardImage->pMYKI_TAControl->ProvisionalFare                                                = 0;

    //  Create directory entry for products

    pCardImage->pMYKI_TAControl->Directory[ DirIndex_ProductInUse  ].Status                     = TAPP_CONTROL_DIRECTORY_STATUS_ACTIVATED;
    pCardImage->pMYKI_TAControl->Directory[ DirIndex_ProductInUse  ].ProductId                  = PRODUCT_ID_NHOUR;
    pCardImage->pMYKI_TAControl->Directory[ DirIndex_ProductInUse  ].SerialNo                   = pCardImage->pMYKI_TAControl->NextProductSerialNo++;

    pCardImage->pMYKI_TAControl->Directory[ DirIndex_ActiveEpass   ].Status                     = TAPP_CONTROL_DIRECTORY_STATUS_ACTIVATED;
    pCardImage->pMYKI_TAControl->Directory[ DirIndex_ActiveEpass   ].ProductId                  = PRODUCT_ID_EPASS;
    pCardImage->pMYKI_TAControl->Directory[ DirIndex_ActiveEpass   ].SerialNo                   = pCardImage->pMYKI_TAControl->NextProductSerialNo++;

    pCardImage->pMYKI_TAControl->Directory[ DirIndex_InactiveEpass ].Status                     = TAPP_CONTROL_DIRECTORY_STATUS_ISSUED;
    pCardImage->pMYKI_TAControl->Directory[ DirIndex_InactiveEpass ].ProductId                  = PRODUCT_ID_EPASS;
    pCardImage->pMYKI_TAControl->Directory[ DirIndex_InactiveEpass ].SerialNo                   = pCardImage->pMYKI_TAControl->NextProductSerialNo++;
}


/*==========================================================================*
**
**  test_BR_LLSC_6_23_001
**
**  Description     :
**      Location Data was Unavailable at scan on
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

int test_BR_LLSC_6_23_001(MYKI_BR_ContextData_t *pData)
{
    /*
     * Test with bad data pointer
     */
    CsVerbose("BR_LLSC_6_23 - Test with NULL data pointer");
    return runTest(NULL, RULE_RESULT_ERROR, 0);
}

/*==========================================================================*
**
**  test_BR_LLSC_6_23_002
**
**  Description     :
**      Location Data was Unavailable at scan on
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

int test_BR_LLSC_6_23_002(MYKI_BR_ContextData_t *pData)
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

    return runTest(pData, RULE_RESULT_BYPASSED, BYPASS_CODE(6, 23, 1, 0));
}

/*==========================================================================*
**
**  test_BR_LLSC_6_23_003
**
**  Description     :
**      Location Data was Unavailable at scan on
**      Unit-test pre-conditions.
**              Location data available
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

int test_BR_LLSC_6_23_003(MYKI_BR_ContextData_t *pData)
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

    pData->DynamicData.locationDataUnavailable = TRUE;

    return runTest(pData, RULE_RESULT_BYPASSED, BYPASS_CODE(6, 23, 2, 0));
}

/*==========================================================================*
**
**  test_BR_LLSC_6_23_004
**
**  Description     :
**      Location Data was Unavailable at scan on
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

int test_BR_LLSC_6_23_004(MYKI_BR_ContextData_t *pData)
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

    return runTest(pData, RULE_RESULT_BYPASSED, BYPASS_CODE(6, 23, 3, 1));
}

/*==========================================================================*
**
**  test_BR_LLSC_6_23_005
**
**  Description     :
**      Location Data was Unavailable at scan on
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

int test_BR_LLSC_6_23_005(MYKI_BR_ContextData_t *pData)
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

    return runTest(pData, RULE_RESULT_BYPASSED, BYPASS_CODE(6, 23, 3, 2));
}

/*==========================================================================*
**
**  test_BR_LLSC_6_23_006
**
**  Description     :
**      Location Data was Unavailable at scan on
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

int test_BR_LLSC_6_23_006(MYKI_BR_ContextData_t *pData)
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

    return runTest(pData, RULE_RESULT_BYPASSED, BYPASS_CODE(6, 23, 4, 0));
}

/*==========================================================================*
**
**  test_BR_LLSC_6_23_007
**
**  Description     :
**      Location Data was Unavailable at scan on
**      Unit-test pre-conditions.
**              Incorrect e-passes
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

int test_BR_LLSC_6_23_007(MYKI_BR_ContextData_t *pData)
{
    /*
     * Incorrect e-passes
     */
    CardImage_t CardImage;
    if (CreateCardImage_Empty(&CardImage) < 0)
    {
        return FALSE;
    }
    ResetCardAndContext(pData, &CardImage);

    CardImage.pMYKI_TAControl->Directory[ DirIndex_ActiveEpass   ].Status   = TAPP_CONTROL_DIRECTORY_STATUS_UNUSED;

    /*
     *              +------------------ Current.InnerZone
     *              |       +---------- Current.OuterZone
     *              v       v
     *      1-------2-------3-------4
     *      ^               ^
     *      |               +---------- Epass.ZoneHigh
     *      +-------------------------- Epass.ZoneLow
     */
    CardImage.pMYKI_TAProduct[ DirIndex_InactiveEpass - 1 ]->ZoneLow        = pData->DynamicData.currentInnerZone - 1;
    CardImage.pMYKI_TAProduct[ DirIndex_InactiveEpass - 1 ]->ZoneHigh       = pData->DynamicData.currentOuterZone;
    if (runTest(pData, RULE_RESULT_BYPASSED, BYPASS_CODE(6, 23, 4, 1)) == FALSE)
        return FALSE;

    /*
     *              +------------------ Current.InnerZone
     *              |       +---------- Current.OuterZone
     *              v       v
     *      1-------2-------3-------4
     *              ^               ^
     *              |               +-- Epass.ZoneHigh
     *              +------------------ Epass.ZoneLow
     */
    CardImage.pMYKI_TAProduct[ DirIndex_InactiveEpass - 1 ]->ZoneLow        = pData->DynamicData.currentInnerZone;
    CardImage.pMYKI_TAProduct[ DirIndex_InactiveEpass - 1 ]->ZoneHigh       = pData->DynamicData.currentOuterZone + 1;
    if (runTest(pData, RULE_RESULT_BYPASSED, BYPASS_CODE(6, 23, 4, 1)) == FALSE)
        return FALSE;

    /*
     *              +------------------ Current.InnerZone
     *              |       +---------- Current.OuterZone
     *              v       v
     *      1-------2-------3-------4
     *      ^                       ^
     *      |                       +-- Epass.ZoneHigh
     *      +-------------------------- Epass.ZoneLow
     */
    CardImage.pMYKI_TAProduct[ DirIndex_InactiveEpass - 1 ]->ZoneLow        = pData->DynamicData.currentInnerZone - 1;
    CardImage.pMYKI_TAProduct[ DirIndex_InactiveEpass - 1 ]->ZoneHigh       = pData->DynamicData.currentOuterZone + 1;
    if (runTest(pData, RULE_RESULT_BYPASSED, BYPASS_CODE(6, 23, 4, 1)) == FALSE)
        return FALSE;

    return TRUE;
}

/*==========================================================================*
**
**  test_BR_LLSC_6_23_008
**
**  Description     :
**      Location Data was Unavailable at scan on
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

int test_BR_LLSC_6_23_008(MYKI_BR_ContextData_t *pData)
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

    return runTest(pData, RULE_RESULT_BYPASSED, BYPASS_CODE(6, 23, 5, 0));
}

/*==========================================================================*
**
**  test_BR_LLSC_6_23_009
**
**  Description     :
**      Location Data was Unavailable at scan on
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

int test_BR_LLSC_6_23_009(MYKI_BR_ContextData_t *pData)
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

    return runTest(pData, RULE_RESULT_BYPASSED, BYPASS_CODE(6, 23, 6, 0));
}

/*==========================================================================*
**
**  test_BR_LLSC_6_23_010
**
**  Description     :
**      Location Data was Unavailable at scan on
**      Unit-test check execution - no ePass
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

int test_BR_LLSC_6_23_010(MYKI_BR_ContextData_t *pData)
{
    CardImage_t CardImage;
    if (CreateCardImage_Empty(&CardImage) < 0)
    {
        return FALSE;
    }
    ResetCardAndContext(pData, &CardImage);

    CardImage.pMYKI_TAControl->Directory[ DirIndex_ActiveEpass   ].Status = TAPP_CONTROL_DIRECTORY_STATUS_UNUSED;
    CardImage.pMYKI_TAControl->Directory[ DirIndex_InactiveEpass ].Status = TAPP_CONTROL_DIRECTORY_STATUS_UNUSED;
    if ( !runTest(pData, RULE_RESULT_EXECUTED, 0) )
    {
        return FALSE;
    }

    if ( pData->DynamicData.isEarlyBirdTrip != FALSE )
    {
        CsVerbose("test_BR_LLSC_6_23_010 - bad isEarlyBirdTrip");
        return FALSE;
    }

    if ( pData->DynamicData.fareRouteIdIsValid != FALSE )
    {
        CsVerbose("test_BR_LLSC_6_23_010 - bad fareRouteIdIsValid");
        return FALSE;
    }

    if ( pData->DynamicData.currentTripDirection != TRIP_DIRECTION_UNKNOWN )
    {
        CsVerbose("test_BR_LLSC_6_23_010 - bad currentTripDirection");
        return FALSE;
    }

    return TRUE;
}

/*==========================================================================*
**
**  test_BR_LLSC_6_23_011
**
**  Description     :
**      Location Data was Unavailable at scan on
**      Unit-test check execution - active ePass exists
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

int test_BR_LLSC_6_23_011(MYKI_BR_ContextData_t *pData)
{
    CardImage_t CardImage;
    if (CreateCardImage_Empty(&CardImage) < 0)
    {
        return FALSE;
    }
    ResetCardAndContext(pData, &CardImage);

    CardImage.pMYKI_TAControl->Directory[ DirIndex_InactiveEpass ].Status = TAPP_CONTROL_DIRECTORY_STATUS_UNUSED;
    if ( !runTest(pData, RULE_RESULT_EXECUTED, 0) )
    {
        return FALSE;
    }

    if ( pData->DynamicData.isEarlyBirdTrip != FALSE )
    {
        CsVerbose("test_BR_LLSC_6_23_011 - bad isEarlyBirdTrip");
        return FALSE;
    }

    if ( pData->DynamicData.fareRouteIdIsValid != FALSE )
    {
        CsVerbose("test_BR_LLSC_6_23_011 - bad fareRouteIdIsValid");
        return FALSE;
    }

    if ( pData->DynamicData.currentTripDirection != TRIP_DIRECTION_UNKNOWN )
    {
        CsVerbose("test_BR_LLSC_6_23_011 - bad currentTripDirection");
        return FALSE;
    }

    return TRUE;
}

/*==========================================================================*
**
**  test_BR_LLSC_6_23_012
**
**  Description     :
**      Location Data was Unavailable at scan on
**      Unit-test check execution - in-active ePass with correct zone range
**      exists
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

int test_BR_LLSC_6_23_012(MYKI_BR_ContextData_t *pData)
{
    CardImage_t CardImage;
    if (CreateCardImage_Empty(&CardImage) < 0)
    {
        return FALSE;
    }
    ResetCardAndContext(pData, &CardImage);

    /*
     *              +------------------ Current.InnerZone
     *              |       +---------- Current.OuterZone
     *              v       v
     *      1-------2-------3-------4
     *              ^       ^        
     *              |       +---------- Epass.ZoneHigh
     *              +------------------ Epass.ZoneLow
     */
    CardImage.pMYKI_TAControl->Directory[ DirIndex_ActiveEpass   ].Status = TAPP_CONTROL_DIRECTORY_STATUS_UNUSED;
    CardImage.pMYKI_TAProduct[ DirIndex_InactiveEpass - 1 ]->ZoneLow        = pData->DynamicData.currentInnerZone;
    CardImage.pMYKI_TAProduct[ DirIndex_InactiveEpass - 1 ]->ZoneHigh       = pData->DynamicData.currentOuterZone;
    if ( !runTest(pData, RULE_RESULT_EXECUTED, 0) )
    {
        return FALSE;
    }

    if ( pData->DynamicData.isEarlyBirdTrip != FALSE )
    {
        CsVerbose("test_BR_LLSC_6_23_012 - bad isEarlyBirdTrip");
        return FALSE;
    }

    if ( pData->DynamicData.fareRouteIdIsValid != FALSE )
    {
        CsVerbose("test_BR_LLSC_6_23_012 - bad fareRouteIdIsValid");
        return FALSE;
    }

    if ( pData->DynamicData.currentTripDirection != TRIP_DIRECTION_UNKNOWN )
    {
        CsVerbose("test_BR_LLSC_6_23_012 - bad currentTripDirection");
        return FALSE;
    }

    return TRUE;
}

/*==========================================================================*
**
**  test_BR_LLSC_6_23_013
**
**  Description     :
**      Location Data was Unavailable at scan on
**      Unit-test check execution - in-active ePass with correct zone range
**      exists
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

int test_BR_LLSC_6_23_013(MYKI_BR_ContextData_t *pData)
{
    CardImage_t CardImage;
    if (CreateCardImage_Empty(&CardImage) < 0)
    {
        return FALSE;
    }
    ResetCardAndContext(pData, &CardImage);

    /*
     *              +------------------ Current.InnerZone
     *              |       +---------- Current.OuterZone
     *              v       v
     *      1-------2-------3-------4
     *              ^
     *              +------------------ Epass.ZoneLow/Epass.ZoneHigh
     */
    CardImage.pMYKI_TAControl->Directory[ DirIndex_ActiveEpass   ].Status = TAPP_CONTROL_DIRECTORY_STATUS_UNUSED;
    CardImage.pMYKI_TAProduct[ DirIndex_InactiveEpass - 1 ]->ZoneLow        = pData->DynamicData.currentInnerZone;
    CardImage.pMYKI_TAProduct[ DirIndex_InactiveEpass - 1 ]->ZoneHigh       = pData->DynamicData.currentInnerZone;
    if ( !runTest(pData, RULE_RESULT_EXECUTED, 0) )
    {
        return FALSE;
    }

    if ( pData->DynamicData.isEarlyBirdTrip != FALSE )
    {
        CsVerbose("test_BR_LLSC_6_23_013 - bad isEarlyBirdTrip");
        return FALSE;
    }

    if ( pData->DynamicData.fareRouteIdIsValid != FALSE )
    {
        CsVerbose("test_BR_LLSC_6_23_013 - bad fareRouteIdIsValid");
        return FALSE;
    }

    if ( pData->DynamicData.currentTripDirection != TRIP_DIRECTION_UNKNOWN )
    {
        CsVerbose("test_BR_LLSC_6_23_013 - bad currentTripDirection");
        return FALSE;
    }

    return TRUE;
}

/*==========================================================================*
**
**  test_BR_LLSC_6_23_014
**
**  Description     :
**      Location Data was Unavailable at scan on
**      Unit-test check execution - in-active ePass with correct zone range
**      exists
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

int test_BR_LLSC_6_23_014(MYKI_BR_ContextData_t *pData)
{
    CardImage_t CardImage;
    if (CreateCardImage_Empty(&CardImage) < 0)
    {
        return FALSE;
    }
    ResetCardAndContext(pData, &CardImage);

    /*
     *              +------------------ Current.InnerZone
     *              |       +---------- Current.OuterZone
     *              v       v
     *      1-------2-------3-------4
     *                      ^
     *                      +---------- Epass.ZoneLow/Epass.ZoneHigh
     */
    CardImage.pMYKI_TAControl->Directory[ DirIndex_ActiveEpass   ].Status = TAPP_CONTROL_DIRECTORY_STATUS_UNUSED;
    CardImage.pMYKI_TAProduct[ DirIndex_InactiveEpass - 1 ]->ZoneLow        = pData->DynamicData.currentOuterZone;
    CardImage.pMYKI_TAProduct[ DirIndex_InactiveEpass - 1 ]->ZoneHigh       = pData->DynamicData.currentOuterZone;
    if ( !runTest(pData, RULE_RESULT_EXECUTED, 0) )
    {
        return FALSE;
    }

    if ( pData->DynamicData.isEarlyBirdTrip != FALSE )
    {
        CsVerbose("test_BR_LLSC_6_23_014 - bad isEarlyBirdTrip");
        return FALSE;
    }

    if ( pData->DynamicData.fareRouteIdIsValid != FALSE )
    {
        CsVerbose("test_BR_LLSC_6_23_014 - bad fareRouteIdIsValid");
        return FALSE;
    }

    if ( pData->DynamicData.currentTripDirection != TRIP_DIRECTION_UNKNOWN )
    {
        CsVerbose("test_BR_LLSC_6_23_014 - bad currentTripDirection");
        return FALSE;
    }

    return TRUE;
}
