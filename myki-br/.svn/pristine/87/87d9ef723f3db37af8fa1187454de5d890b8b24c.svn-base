/* -*- mode: c; tabs: 4; -*- */
/*=============================================================================
**
**    Vix Technology                   Licensed software
**    (C) 2013                         All rights reserved
**
**=============================================================================
**
**  Project/Product : MBU
**  Filename        : test_BR_LLSC_6_18.c
**  Author(s)       : David Purdie
**  Description     :
**      Implements BR_LLSC_6_18 business rule unit-tests.
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

//  Nortmally this should be given to us through a header file, but in this case the function
//  exists only for the purpose of unit testing with the proper CD functions stubbed out. This
//  function has no purpose in the normal Myki CD API, so it shouldn't be placed into myki_cd.h,
//  but at the same time it is required by the Myki CD Stub functions which use the Myki CD API,
//  but with this one extra function needed because they're stubs.

extern void MYKI_CD_setEarlyBirdDay(int day);

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
typedef struct
{
    MYKI_TAControl_t *pMYKI_TAControl;
    struct tm        *pTm;
    MYKI_TAProduct_t *pProductInUse;
    MYKI_Directory_t *pDirectoryInUse;

} testIf_t;

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
** FUNCTION           : setupTest
**
** DESCRIPTION        : Setup the required conditions for a succesful transaction
**                      Retain points to facilitate the creation of all bypass
**                      conditions
**
**
** INPUTS             : pData           - Context Data
**                      tIf             - Reference to Test Interface Data
**                                        created by this function
**
** RETURNS            : FALSE           - Setup error
**
----------------------------------------------------------------------------*/
static int setupTest( MYKI_BR_ContextData_t *pData, testIf_t *tIf)
{
    /*
    **  Init Test Interface Data Items
    */
    tIf->pMYKI_TAControl = NULL;
    tIf->pTm = NULL;
    tIf->pProductInUse = NULL;
    tIf->pDirectoryInUse = NULL;

    if ( MYKI_CS_TAControlGet( &tIf->pMYKI_TAControl ) < 0 || tIf->pMYKI_TAControl == NULL )
    {
        return -1;
    }

    /*
    **  Create all the conditions required for a successful execution
    **  of the Business Rule
    **  A tst with:
    **      A Product is in use
    **      Is a provisional product
    **      Scan on is rail
    **      Is a rail device
    **      Scan on Station is an early bird station
    **      Current location is an early bird station
    **      Scan on time window good
    **      Is an early bird day
    **      Before Early Bird cutoff
    **      After Start of Business Day
    */
    CardImage1(pData);

    pData->InternalData.TransportMode = TRANSPORT_MODE_RAIL;

    tIf->pMYKI_TAControl->ProductInUse = 2;

    /*
    **  Setup a product
    **      Issued
    **      PRODUCT_TYPE_WEEKLY
    **      Zones 5:10
    */
    tIf->pMYKI_TAControl->Directory[tIf->pMYKI_TAControl->ProductInUse].Status = TAPP_CONTROL_DIRECTORY_STATUS_ACTIVATED;

    if (myki_br_GetCardProduct(tIf->pMYKI_TAControl->ProductInUse, &tIf->pDirectoryInUse, NULL) >= 0)
    {
        tIf->pDirectoryInUse->ProductId = myki_br_cd_GetProductId(PRODUCT_TYPE_WEEKLY);

        if (MYKI_CS_TAProductGet( (U8_t)( tIf->pMYKI_TAControl->ProductInUse - 1 ), &tIf->pProductInUse) >= 0)
        {
            tIf->pProductInUse->ZoneLow = 5;
            tIf->pProductInUse->ZoneHigh = 10;
        } else
        {
            CsErrx("test_BR_LLSC_6_18 : setProduct::MYKI_CS_TAProductGet( %d ) failed", tIf->pMYKI_TAControl->ProductInUse);
        }
    }
    else
    {
        CsErrx("test_BR_LLSC_6_18 : setProduct::myki_br_GetCardProduct( %d ) failed", tIf->pMYKI_TAControl->ProductInUse);
    }

    tIf->pProductInUse->ControlBitmap |=  TAPP_TPRODUCT_CONTROL_PROVISIONAL_BITMAP;

    pData->DynamicData.entryPointId = 98;
    tIf->pProductInUse->LastUsage.Location.EntryPointId = 99;
    MYKI_CD_setEarlyBirdStation( (U16_t)tIf->pProductInUse->LastUsage.Location.EntryPointId, (U16_t)pData->DynamicData.entryPointId );

    tIf->pTm = localtime( (time_t*)&pData->DynamicData.currentDateTime );
    MYKI_CD_setEarlyBirdDay(tIf->pTm->tm_wday);

    MYKI_CD_setEarlyBirdCutOffTime((tIf->pTm->tm_hour * 60) + tIf->pTm->tm_min + 1);

    tIf->pProductInUse->LastUsage.DateTime = pData->DynamicData.currentDateTime - 100;

    return 1;
}

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
    int rv = BR_LLSC_6_18(pData);

    if ( rv != rc )
    {
        CsVerbose("test_BR_LLSC_6_18 - Unexpected exit code. Got:%d, Expected:%d\n", rv, rc );
        return FALSE;
    }

    if ( pData->ReturnedData.bypassCode != bc)
    {
        CsVerbose("test_BR_LLSC_6_18 - Unexpected ByPassCode. Got:%d, Expected:%d\n", pData->ReturnedData.bypassCode, bc);
        return FALSE;
    }
    return TRUE;
}

/*==========================================================================*
**
**  test_BR_LLSC_6_18_000
**
**  Description     :
**      Scan off Early Bird
**      Unit-test Execute conditions.
**              All is Good
**
**              00:00               EndOfBusinessDay
**              |                   |
**              v                   v
**      ...-----+-------------------+-------------------...
**                                      ^           ^
**                                      |           +- CurrentDateTime
**                                      +------------- LastUsateDateTime
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

int test_BR_LLSC_6_18_000(MYKI_BR_ContextData_t *pData)
{
    testIf_t    tIf;
    Time_t      now                     = pData->DynamicData.currentDateTime;
    Time_t      startOfBusinessDateTime = myki_br_StartOfBusinessDateTime( pData, now );
    pData->DynamicData.currentDateTime  = startOfBusinessDateTime + 150 /*SECS*/;

    /*
    **  Setup test conditions
    */
    if (setupTest( pData, &tIf) < 0)
    {
        return FALSE;
    }

    /*
    **  Run the Test and examine the results
    */
    return runTest(pData, RULE_RESULT_EXECUTED, 11);
}

/*==========================================================================*
**
**  test_BR_LLSC_6_18_001
**
**  Description     :
**      Scan off Early Bird
**      Unit-test Execute conditions.
**              Test with NULL pData
**              Test with product not in use
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

int test_BR_LLSC_6_18_001(MYKI_BR_ContextData_t *pData)
{
    testIf_t    tIf;
    Time_t      now                     = pData->DynamicData.currentDateTime;
    Time_t      startOfBusinessDateTime = myki_br_StartOfBusinessDateTime( pData, now );
    pData->DynamicData.currentDateTime  = startOfBusinessDateTime + 200 /*SECS*/;

    /*
    **  Setup test conditions
    */
    if (setupTest( pData, &tIf) < 0)
    {
        return FALSE;
    }

    /*
    ** Test with bad data pointer
    */
    CsVerbose("BR_LLSC_6_18 - Test with NULL data pointer");
    if (BR_LLSC_6_18(NULL) != RULE_RESULT_ERROR)
    {
        return FALSE;
    }

    /*
    **  Test with
    **      No Product is in use
    */
    tIf.pMYKI_TAControl->ProductInUse = 0;

    /*
    **  Run the Test and examine the results
    */
    return runTest(pData, RULE_RESULT_BYPASSED, 1 );
}

/*==========================================================================*
**
**  test_BR_LLSC_6_18_002
**
**  Description     :
**      Scan off Early Bird
**      Unit-test Execute conditions.
**              Is Not a provisional product
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

int test_BR_LLSC_6_18_002(MYKI_BR_ContextData_t *pData)
{
    testIf_t    tIf;
    Time_t      now                     = pData->DynamicData.currentDateTime;
    Time_t      startOfBusinessDateTime = myki_br_StartOfBusinessDateTime( pData, now );
    pData->DynamicData.currentDateTime  = startOfBusinessDateTime + 200 /*SECS*/;

    /*
    **  Setup test conditions
    */
    if (setupTest( pData, &tIf) < 0)
    {
        return FALSE;
    }

    /*
    **  Test with
    **      Is NOT a provisional product
    */
    tIf.pProductInUse->ControlBitmap &=  ~TAPP_TPRODUCT_CONTROL_PROVISIONAL_BITMAP;

    /*
    **  Run the Test and examine the results
    */
    return runTest(pData, RULE_RESULT_BYPASSED, 2);
}

/*==========================================================================*
**
**  test_BR_LLSC_6_18_003
**
**  Description     :
**      Scan off Early Bird
**      Unit-test Execute conditions.
**              Scan on not rail
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

int test_BR_LLSC_6_18_003(MYKI_BR_ContextData_t *pData)
{
    testIf_t    tIf;
    Time_t      now                     = pData->DynamicData.currentDateTime;
    Time_t      startOfBusinessDateTime = myki_br_StartOfBusinessDateTime( pData, now );
    pData->DynamicData.currentDateTime  = startOfBusinessDateTime + 200 /*SECS*/;

    /*
    **  Setup test conditions
    */
    if (setupTest( pData, &tIf) < 0)
    {
        return FALSE;
    }

    /*
    **  Test with
    **      Scan on is NOT rail
    */
    tIf.pProductInUse->LastUsage.ProviderId++;

    /*
    **  Run the Test and examine the results
    */
    return runTest(pData, RULE_RESULT_BYPASSED, 3);
}


/*==========================================================================*
**
**  test_BR_LLSC_6_18_004
**
**  Description     :
**      Scan off Early Bird
**      Unit-test Execute conditions.
**              Not a rail device
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

int test_BR_LLSC_6_18_004(MYKI_BR_ContextData_t *pData)
{
    testIf_t    tIf;
    Time_t      now                     = pData->DynamicData.currentDateTime;
    Time_t      startOfBusinessDateTime = myki_br_StartOfBusinessDateTime( pData, now );
    pData->DynamicData.currentDateTime  = startOfBusinessDateTime + 200 /*SECS*/;

    /*
    **  Setup test conditions
    */
    if (setupTest( pData, &tIf) < 0)
    {
        return FALSE;
    }

    /*
    **  Test with
    **      Is NOT a rail device
    */
    pData->InternalData.TransportMode = TRANSPORT_MODE_BUS;

    /*
    **  Run the Test and examine the results
    */
    return runTest(pData, RULE_RESULT_BYPASSED, 4);
}

/*==========================================================================*
**
**  test_BR_LLSC_6_18_005
**
**  Description     :
**      Scan off Early Bird
**      Unit-test Execute conditions.
**              Scan on Station not an early bird station
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

int test_BR_LLSC_6_18_005(MYKI_BR_ContextData_t *pData)
{
    testIf_t    tIf;
    Time_t      now                     = pData->DynamicData.currentDateTime;
    Time_t      startOfBusinessDateTime = myki_br_StartOfBusinessDateTime( pData, now );
    pData->DynamicData.currentDateTime  = startOfBusinessDateTime + 200 /*SECS*/;

    /*
    **  Setup test conditions
    */
    if (setupTest( pData, &tIf) < 0)
    {
        return FALSE;
    }

    /*
    **  Test with
    **      Scan on Station is NOT an early bird station
    */
    MYKI_CD_setEarlyBirdStation(-1, (U16_t)pData->DynamicData.entryPointId );

    /*
    **  Run the Test and examine the results
    */
    return runTest(pData, RULE_RESULT_BYPASSED, 5);
}

/*==========================================================================*
**
**  test_BR_LLSC_6_18_006
**
**  Description     :
**      Scan off Early Bird
**      Unit-test Execute conditions.
**              Current location not an early bird station
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

int test_BR_LLSC_6_18_006(MYKI_BR_ContextData_t *pData)
{
    testIf_t    tIf;
    Time_t      now                     = pData->DynamicData.currentDateTime;
    Time_t      startOfBusinessDateTime = myki_br_StartOfBusinessDateTime( pData, now );
    pData->DynamicData.currentDateTime  = startOfBusinessDateTime + 200 /*SECS*/;

    /*
    **  Setup test conditions
    */
    if (setupTest( pData, &tIf) < 0)
    {
        return FALSE;
    }

    /*
    **  Test with
    **      Current location is NOT an early bird station
    */
    MYKI_CD_setEarlyBirdStation(tIf.pProductInUse->LastUsage.Location.EntryPointId, -1);

    /*
    **  Run the Test and examine the results
    */
    return runTest(pData, RULE_RESULT_BYPASSED, 6);
}

/*==========================================================================*
**
**  test_BR_LLSC_6_18_007
**
**  Description     :
**      Scan off Early Bird
**      Unit-test Execute conditions.
**          Scan on time too old
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

int test_BR_LLSC_6_18_007(MYKI_BR_ContextData_t *pData)
{
    testIf_t    tIf;
    Time_t      now                     = pData->DynamicData.currentDateTime;
    Time_t      startOfBusinessDateTime = myki_br_StartOfBusinessDateTime( pData, now );
    pData->DynamicData.currentDateTime  = startOfBusinessDateTime + 200 /*SECS*/;

    /*
    **  Setup test conditions
    */
    if (setupTest( pData, &tIf) < 0)
    {
        return FALSE;
    }

    /*
    **  Test with
    **      Scan on time too old
    */
    tIf.pProductInUse->LastUsage.DateTime = pData->DynamicData.currentDateTime - MINUTES_TO_SECONDS(pData->Tariff.nHourPeriodMinutes + (1 * 60)) - 1;

    /*
    **  Run the Test and examine the results
    */
    return runTest(pData, RULE_RESULT_BYPASSED, 7);
}

/*==========================================================================*
**
**  test_BR_LLSC_6_18_008
**
**  Description     :
**      Scan off Early Bird
**      Unit-test Execute conditions.
**          Not an early bird day
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

int test_BR_LLSC_6_18_008(MYKI_BR_ContextData_t *pData)
{
    testIf_t    tIf;
    Time_t      now                     = pData->DynamicData.currentDateTime;
    Time_t      startOfBusinessDateTime = myki_br_StartOfBusinessDateTime( pData, now );
    pData->DynamicData.currentDateTime  = startOfBusinessDateTime + 200 /*SECS*/;

    /*
    **  Setup test conditions
    */
    if (setupTest( pData, &tIf) < 0)
    {
        return FALSE;
    }

    /*
    **  Test with
    **      NOT an early bird day
    */
    MYKI_CD_setEarlyBirdDay(-1);

    /*
    **  Run the Test and examine the results
    */
    return runTest(pData, RULE_RESULT_BYPASSED, 8);
}


/*==========================================================================*
**
**  test_BR_LLSC_6_18_009
**
**  Description     :
**      Scan off Early Bird
**      Unit-test Execute conditions.
**          After Early Bird cutoff
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

int test_BR_LLSC_6_18_009(MYKI_BR_ContextData_t *pData)
{
    testIf_t    tIf;
    Time_t      now                     = pData->DynamicData.currentDateTime;
    Time_t      startOfBusinessDateTime = myki_br_StartOfBusinessDateTime( pData, now );
    pData->DynamicData.currentDateTime  = startOfBusinessDateTime + 200 /*SECS*/;

    /*
    **  Setup test conditions
    */
    if (setupTest( pData, &tIf) < 0)
    {
        return FALSE;
    }

    /*
    **  Test with
    **      After Early Bird cutoff
    */
    tIf.pTm = localtime( (time_t*)&pData->DynamicData.currentDateTime );

    MYKI_CD_setEarlyBirdCutOffTime((tIf.pTm->tm_hour * 60) + tIf.pTm->tm_min - 1);

    /*
    **  Run the Test and examine the results
    */
    return runTest(pData, RULE_RESULT_BYPASSED, 9);
}

/*==========================================================================*
**
**  test_BR_LLSC_6_18_010
**
**  Description     :
**      Scan off Early Bird
**      Unit-test Execute conditions.
**          Before Start of Business Day
**
**              00:00               EndOfBusinessDay
**              |                   |
**              v                   v
**      ...-----+-------------------+-------------------...
**                          ^           ^
**                          |           +- CurrentDateTime
**                          +------------- LastUsateDateTime
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

int test_BR_LLSC_6_18_010(MYKI_BR_ContextData_t *pData)
{
    testIf_t    tIf;
    Time_t      now                     = pData->DynamicData.currentDateTime;
    Time_t      startOfBusinessDateTime = myki_br_StartOfBusinessDateTime( pData, now );
    pData->DynamicData.currentDateTime  = startOfBusinessDateTime + 60 /*SECS*/;

    /*
    **  Setup test conditions
    */
    if (setupTest( pData, &tIf) < 0)
    {
        return FALSE;
    }

    /*
    **  Run the Test and examine the results
    */
    return runTest(pData, RULE_RESULT_BYPASSED, 10);
}

/*==========================================================================*
**
**  test_BR_LLSC_6_18_011
**
**  Description     :
**      Scan off Early Bird
**      Unit-test Execute conditions.
**          Before Start of Business Day
**
**              00:00               EndOfBusinessDay
**              |                   |
**              v                   v
**      ...-----|-------------------+-------------------...
**                  ^           ^
**                  |           +- CurrentDateTime
**                  +------------- LastUsateDateTime
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

int test_BR_LLSC_6_18_011(MYKI_BR_ContextData_t *pData)
{
    testIf_t    tIf;
    Time_t      now                     = pData->DynamicData.currentDateTime;
    Time_t      startOfBusinessDateTime = myki_br_StartOfBusinessDateTime( pData, now );
    pData->DynamicData.currentDateTime  = startOfBusinessDateTime - 60 /*SECS*/;

    /*
    **  Setup test conditions
    */
    if (setupTest( pData, &tIf) < 0)
    {
        return FALSE;
    }

    /*
    **  Run the Test and examine the results
    */
    return runTest(pData, RULE_RESULT_BYPASSED, 9);
}

/*==========================================================================*
**
**  test_BR_LLSC_6_18_012
**
**  Description     :
**      Scan off Early Bird
**      Unit-test Execute conditions.
**              Early Bird Cut Off Time not configured
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

int test_BR_LLSC_6_18_012(MYKI_BR_ContextData_t *pData)
{
    testIf_t    tIf;

    /*
    **  Setup test conditions
    */
    if (setupTest( pData, &tIf) < 0)
    {
        return FALSE;
    }

    /*
    **  Test with
    **      Early Bird Cut Off Time not configured
    */
    MYKI_CD_setEarlyBirdCutOffTime(0);

    /*
    **  Run the Test and examine the results
    */
    return runTest(pData, RULE_RESULT_BYPASSED, 12);
}

