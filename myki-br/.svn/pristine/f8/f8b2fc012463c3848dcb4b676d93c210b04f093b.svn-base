/* -*- mode: c; tabs: 4; -*- */
/*=============================================================================
**
**    Vix Technology                   Licensed software
**    (C) 2013                         All rights reserved
**
**=============================================================================
**
**  Project/Product : MBU
**  Filename        : test_BR_LLSC_4_17.c
**  Author(s)       : David Purdie
**  Description     :
**      Implements BR_LLSC_4_17 business rule unit-tests.
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
MYKI_Directory_t *pDirectory;       // As set by setProduct for tweaking
MYKI_TAProduct_t *pProduct;         // As set by setProduct for tweaking

/*----------------------------------------------------------------------------
** FUNCTION           : setProduct
**
** DESCRIPTION        : Set specific parameters with a product
**
**
** INPUTS             : pMYKI_TAControl     - Pointer to data
**                      pIndex              - product index
**                      pType               - product type
**                      zoneLow             - low zone
**                      hizone              - high zone
**
** RETURNS            : Nothing
**
----------------------------------------------------------------------------*/

static void setProduct(MYKI_TAControl_t *pMYKI_TAControl, int pIndex, int pType, int zoneLow, int zoneHigh)
{
    pDirectory = NULL;
    pProduct = NULL;

    //pMYKI_TAControl->Directory[pIndex].Status = TAPP_CONTROL_DIRECTORY_STATUS_ACTIVATED;
    pMYKI_TAControl->Directory[pIndex].Status = TAPP_CONTROL_DIRECTORY_STATUS_ISSUED;

    if (myki_br_GetCardProduct(pIndex, &pDirectory, NULL) >= 0)
    {
        pDirectory->ProductId = myki_br_cd_GetProductId(pType);

        if (MYKI_CS_TAProductGet( (U8_t)( pIndex - 1 ), &pProduct) >= 0)
        {
            pProduct->ZoneLow = zoneLow;
            pProduct->ZoneHigh = zoneHigh;
        } else
        {
            CsErrx("test_BR_LLSC_4_17 : setProduct::MYKI_CS_TAProductGet( %d ) failed", pIndex);
        }
    } else
    {
        CsErrx("test_BR_LLSC_4_17 : setProduct::myki_br_GetCardProduct( %d ) failed", pIndex);
    }
}

/*----------------------------------------------------------------------------
** FUNCTION           : runTest
**
** DESCRIPTION        : Run the test and examine the results
**
**
** INPUTS             : pData           - Context Data
**                      pMYKI_TAControl - Pointer to data
**                      rc              - Expected Result Code
**                      bc              - Expected bypass Code
**
** RETURNS            : TRUE        - Test was OK
**                      FALSE       - Test Failed
**
----------------------------------------------------------------------------*/

static int runTest( MYKI_BR_ContextData_t *pData, MYKI_TAControl_t *pMYKI_TAControl, int rc, int bc)
{
    //ShowCardImage( "test1_before.txt" );
    int rv = BR_LLSC_4_17(pData);

    if ( rv != rc )
    {
        CsVerbose("test_BR_LLSC_4_17 - Unexpected exit code. Got:%d, Expected:%d\n", rv, rc );
        return FALSE;
    }

    if ( pData->ReturnedData.bypassCode != bc)
    {
        CsVerbose("test_BR_LLSC_4_17 - Unexpected ByPassCode. Got:%d, Expected:%d\n", pData->ReturnedData.bypassCode, bc);
        return FALSE;
    }
    return TRUE;
}


/*==========================================================================*
**
**  test_BR_LLSC_4_17_001
**
**  Description     :
**      Wrong entry at gate, missing Rail Scan-Off- Penalty
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

int test_BR_LLSC_4_17_001(MYKI_BR_ContextData_t *pData)
{

    /*
    ** Test with bad data pointer
    */
    CsVerbose("BR_LLSC_4_17 - Test with NULL data pointer");
    if (BR_LLSC_4_17(NULL) != RULE_RESULT_ERROR)
    {
        return FALSE;
    }

    /*
    **  Test with
    **      No product not in use
    */
    CardImage1(pData);

    /*
    **  Run the Test and examine the results
    */
    return runTest(pData, NULL, RULE_RESULT_BYPASSED, 1 );
}

/*==========================================================================*
**
**  test_BR_LLSC_4_17_002
**
**  Description     :
**      Wrong entry at gate, missing Rail Scan-Off- Penalty
**      Unit-test Execute conditions.
**              Route Change over
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

int test_BR_LLSC_4_17_002(MYKI_BR_ContextData_t *pData)
{
    MYKI_TAControl_t       *pMYKI_TAControl         = NULL;

    if ( MYKI_CS_TAControlGet( &pMYKI_TAControl ) < 0 || pMYKI_TAControl == NULL )
    {
        return FALSE;
    }

    /*
    **  Test with
    **      Product in use
    **      Route Change Over
    */
    CardImage1(pData);
    pMYKI_TAControl->ProductInUse = 1;
    pData->DynamicData.routeChangeover = ROUTE_CHANGEOVER_LEGITIMATE_WITHOUT_ROUTE_CHANGEOVER;

    /*
    **  Run the Test and examine the results
    */
    return runTest(pData, NULL, RULE_RESULT_BYPASSED, 2);
}

/*==========================================================================*
**
**  test_BR_LLSC_4_17_003
**
**  Description     :
**      Wrong entry at gate, missing Rail Scan-Off- Penalty
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

int test_BR_LLSC_4_17_003(MYKI_BR_ContextData_t *pData)
{
    MYKI_TAControl_t       *pMYKI_TAControl         = NULL;

    if ( MYKI_CS_TAControlGet( &pMYKI_TAControl ) < 0 || pMYKI_TAControl == NULL )
    {
        return FALSE;
    }

    /*
    **  Test with
    **      Product in use
    **      No Route Change Over
    **      Scan on not rail
    */
    CardImage1(pData);
    pData->DynamicData.routeChangeover = ROUTE_CHANGEOVER_NONE;
    pMYKI_TAControl->ProductInUse = 1;

    setProduct(pMYKI_TAControl, 1, PRODUCT_TYPE_WEEKLY, 5, 10);
    if ( pProduct )
    {
        pProduct->LastUsage.ProviderId = 1;
    }
    
    /*
    **  Run the Test and examine the results
    */
    return runTest(pData, NULL, RULE_RESULT_BYPASSED, 3);
}


/*==========================================================================*
**
**  test_BR_LLSC_4_17_004
**
**  Description     :
**      Wrong entry at gate, missing Rail Scan-Off- Penalty
**      Unit-test Execute conditions.
**              I am not rail
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

int test_BR_LLSC_4_17_004(MYKI_BR_ContextData_t *pData)
{
    MYKI_TAControl_t       *pMYKI_TAControl         = NULL;

    if ( MYKI_CS_TAControlGet( &pMYKI_TAControl ) < 0 || pMYKI_TAControl == NULL )
    {
        return FALSE;
    }

    /*
    **  Test with
    **      Product in use
    **      No Route Change Over
    **      Scan on is rail
    **      I am not rail
    */
    CardImage1(pData);
    pData->DynamicData.routeChangeover = ROUTE_CHANGEOVER_NONE;

    pMYKI_TAControl->ProductInUse = 1;
    setProduct(pMYKI_TAControl, 1, PRODUCT_TYPE_WEEKLY, 5, 10);
    if ( pProduct )
    {
        pProduct->LastUsage.ProviderId = 0;
    }

    pData->InternalData.TransportMode = TRANSPORT_MODE_BUS;

    /*
    **  Run the Test and examine the results
    */
    return runTest(pData, NULL, RULE_RESULT_BYPASSED, 4);
}

/*==========================================================================*
**
**  test_BR_LLSC_4_17_005
**
**  Description     :
**      Wrong entry at gate, missing Rail Scan-Off- Penalty
**      Unit-test Execute conditions.
**              Same entry point
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

int test_BR_LLSC_4_17_005(MYKI_BR_ContextData_t *pData)
{
    MYKI_TAControl_t       *pMYKI_TAControl         = NULL;

    if ( MYKI_CS_TAControlGet( &pMYKI_TAControl ) < 0 || pMYKI_TAControl == NULL )
    {
        return FALSE;
    }

    /*
    **  Test with
    **      Product in use
    **      No Route Change Over
    **      Scan on is rail
    **      I am rail
    **      Same Exit Point
    */
    CardImage1(pData);
    pData->DynamicData.routeChangeover = ROUTE_CHANGEOVER_NONE;
    pData->InternalData.TransportMode = TRANSPORT_MODE_RAIL;

    pMYKI_TAControl->ProductInUse = 1;
    setProduct(pMYKI_TAControl, 1, PRODUCT_TYPE_WEEKLY, 5, 10);
    if ( pProduct )
    {
        pProduct->LastUsage.ProviderId = 0;
        pProduct->LastUsage.Location.EntryPointId = pData->DynamicData.entryPointId;
    }

    /*
    **  Run the Test and examine the results
    */
    return runTest(pData, NULL, RULE_RESULT_BYPASSED, 5);
}

/*==========================================================================*
**
**  test_BR_LLSC_4_17_006
**
**  Description     :
**      Wrong entry at gate, missing Rail Scan-Off- Penalty
**      Unit-test Execute conditions.
**              Product Expired
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

int test_BR_LLSC_4_17_006(MYKI_BR_ContextData_t *pData)
{
    MYKI_TAControl_t       *pMYKI_TAControl         = NULL;

    if ( MYKI_CS_TAControlGet( &pMYKI_TAControl ) < 0 || pMYKI_TAControl == NULL )
    {
        return FALSE;
    }

    /*
    **  Test with
    **      Product in use
    **      No Route Change Over
    **      Scan on is rail
    **      I am rail
    **      Different Exit Point
    **      Product Expired
    */
    CardImage1(pData);
    pData->DynamicData.routeChangeover = ROUTE_CHANGEOVER_NONE;
    pData->InternalData.TransportMode = TRANSPORT_MODE_RAIL;

    pMYKI_TAControl->ProductInUse = 1;
    setProduct(pMYKI_TAControl, 1, PRODUCT_TYPE_WEEKLY, 5, 10);
    if ( pProduct )
    {
        pProduct->LastUsage.ProviderId = 0;
        pProduct->LastUsage.Location.EntryPointId = pData->DynamicData.entryPointId + 1;
        pProduct->EndDateTime = pData->DynamicData.currentDateTime - 100;
    }

    /*
    **  Run the Test and examine the results
    */
    return runTest(pData, pMYKI_TAControl, RULE_RESULT_BYPASSED, 6);
}

/*==========================================================================*
**
**  test_BR_LLSC_4_17_007
**
**  Description     :
**      Wrong entry at gate, missing Rail Scan-Off- Penalty
**      Unit-test Execute conditions.
**          Provisional Capping Option is NOT Option3
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

int test_BR_LLSC_4_17_007(MYKI_BR_ContextData_t *pData)
{
    MYKI_TAControl_t       *pMYKI_TAControl         = NULL;

    if ( MYKI_CS_TAControlGet( &pMYKI_TAControl ) < 0 || pMYKI_TAControl == NULL )
    {
        return FALSE;
    }

    /*
    **  Test with
    **      Product in use
    **      No Route Change Over
    **      Scan on is rail
    **      I am rail
    **      Different Exit Point
    **      Product not Expired
    **      Provisional Capping Option is NOT Option3
    */
    CardImage1(pData);
    pData->DynamicData.routeChangeover = ROUTE_CHANGEOVER_NONE;
    pData->InternalData.TransportMode = TRANSPORT_MODE_RAIL;
    pData->Tariff.provisionalCappingOption = 2;

    pMYKI_TAControl->ProductInUse = 1;
    setProduct(pMYKI_TAControl, 1, PRODUCT_TYPE_WEEKLY, 5, 10);
    if ( pProduct )
    {
        pProduct->LastUsage.ProviderId = 0;
        pProduct->LastUsage.Location.EntryPointId = pData->DynamicData.entryPointId + 1;
        pProduct->EndDateTime = pData->DynamicData.currentDateTime + 100;
    }

    /*
    **  Run the Test and examine the results
    */
    return runTest(pData, pMYKI_TAControl, RULE_RESULT_BYPASSED, 7);
}

/*==========================================================================*
**
**  test_BR_LLSC_4_17_008
**
**  Description     :
**      Wrong entry at gate, missing Rail Scan-Off- Penalty
**      Unit-test Execute conditions.
**              All is Good
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

int test_BR_LLSC_4_17_008(MYKI_BR_ContextData_t *pData)
{
    MYKI_TAControl_t       *pMYKI_TAControl         = NULL;

    if ( MYKI_CS_TAControlGet( &pMYKI_TAControl ) < 0 || pMYKI_TAControl == NULL )
    {
        return FALSE;
    }

    /*
    **  Test with
    **      Product in use
    **      No Route Change Over
    **      Scan on is rail
    **      I am rail
    **      Different Exit Point
    **      Product not Expired
    **      Provisional Capping Option is NOT Option3
    */
    CardImage1(pData);
    pData->DynamicData.routeChangeover = ROUTE_CHANGEOVER_NONE;
    pData->InternalData.TransportMode = TRANSPORT_MODE_RAIL;
    pData->Tariff.provisionalCappingOption = 3;

    pMYKI_TAControl->ProductInUse = 1;
    setProduct(pMYKI_TAControl, 1, PRODUCT_TYPE_WEEKLY, 5, 10);
    if ( pProduct )
    {
        pProduct->LastUsage.ProviderId = 0;
        pProduct->LastUsage.Location.EntryPointId = pData->DynamicData.entryPointId + 1;
        pProduct->EndDateTime = pData->DynamicData.currentDateTime + 100;
    }

    /*
    **  Run the Test and examine the results
    */
    return runTest(pData, pMYKI_TAControl, RULE_RESULT_EXECUTED, 8);
}

