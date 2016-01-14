/* -*- mode: c; tabs: 4; -*- */
/*=============================================================================
**
**    Vix Technology                   Licensed software
**    (C) 2013                         All rights reserved
**
**=============================================================================
**
**  Project/Product : MBU
**  Filename        : test_BR_LLSC_6_3.c
**  Author(s)       : David Purdie
**  Description     :
**      Implements BR_LLSC_6_3 business rule unit-tests.
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
            CsErrx("test_BR_LLSC_6_3 : setProduct::MYKI_CS_TAProductGet( %d ) failed", pIndex);
        }
    } else
    {
        CsErrx("test_BR_LLSC_6_3 : setProduct::myki_br_GetCardProduct( %d ) failed", pIndex);
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
    int rv = BR_LLSC_6_3(pData);

    if ( rv != rc )
    {
        CsVerbose("test_BR_LLSC_6_3 - Unexpected exit code. Got:%d, Expected:%d\n", rv, rc );
        return FALSE;
    }

    if ( pData->ReturnedData.bypassCode != bc)
    {
        CsVerbose("test_BR_LLSC_6_3 - Unexpected ByPassCode. Got:%d, Expected:%d\n", pData->ReturnedData.bypassCode, bc);
        return FALSE;
    }
    return TRUE;
}


/*==========================================================================*
**
**  test_BR_LLSC_6_3_001
**
**  Description     :
**      Scan-Off e-Pass with Restricted Out-of-Zone
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

int test_BR_LLSC_6_3_001(MYKI_BR_ContextData_t *pData)
{

    /*
    ** Test with bad data pointer
    */
    CsVerbose("BR_LLSC_6_3 - Test with NULL data pointer");
    if (BR_LLSC_6_3(NULL) != RULE_RESULT_ERROR)
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
**  test_BR_LLSC_6_3_002
**
**  Description     :
**      Scan-Off e-Pass with Restricted Out-of-Zone
**      Unit-test Execute conditions.
**              Not an ePass
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

int test_BR_LLSC_6_3_002(MYKI_BR_ContextData_t *pData)
{
    MYKI_TAControl_t       *pMYKI_TAControl         = NULL;

    if ( MYKI_CS_TAControlGet( &pMYKI_TAControl ) < 0 || pMYKI_TAControl == NULL )
    {
        return FALSE;
    }

    /*
    **  Test with
    **      Product in use
    **      Not an ePass
    */
    CardImage1(pData);
    pMYKI_TAControl->ProductInUse = 3;

    setProduct(pMYKI_TAControl, 1, PRODUCT_TYPE_WEEKLY, 5, 10);
    setProduct(pMYKI_TAControl, 2, PRODUCT_TYPE_WEEKLY, 5, 10);
    setProduct(pMYKI_TAControl, 3, PRODUCT_TYPE_WEEKLY, 5, 10);
    setProduct(pMYKI_TAControl, 4, PRODUCT_TYPE_WEEKLY, 5, 10);
    setProduct(pMYKI_TAControl, 5, PRODUCT_TYPE_WEEKLY, 5, 10);

    /*
    **  Run the Test and examine the results
    */
    return runTest(pData, NULL, RULE_RESULT_BYPASSED, 2);
}

/*==========================================================================*
**
**  test_BR_LLSC_6_3_003
**
**  Description     :
**      Scan-Off e-Pass with Restricted Out-of-Zone
**      Unit-test Execute conditions.
**              Allow out of zone travel
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

int test_BR_LLSC_6_3_003(MYKI_BR_ContextData_t *pData)
{
    MYKI_TAControl_t       *pMYKI_TAControl         = NULL;

    if ( MYKI_CS_TAControlGet( &pMYKI_TAControl ) < 0 || pMYKI_TAControl == NULL )
    {
        return FALSE;
    }

    /*
    **  Test with
    **      Product in use
    **      Is an ePAss
    **      Allow out of zone travel
    */
    CardImage1(pData);
    pMYKI_TAControl->ProductInUse = 3;

    setProduct(pMYKI_TAControl, 1, PRODUCT_TYPE_WEEKLY, 5, 10);
    setProduct(pMYKI_TAControl, 2, PRODUCT_TYPE_WEEKLY, 5, 10);
    setProduct(pMYKI_TAControl, 3, PRODUCT_TYPE_EPASS , 5, 10);
    setProduct(pMYKI_TAControl, 4, PRODUCT_TYPE_WEEKLY, 5, 10);
    setProduct(pMYKI_TAControl, 5, PRODUCT_TYPE_WEEKLY, 5, 10);

    pData->Tariff.allowEPassOutOfZone = TRUE;
    
    /*
    **  Run the Test and examine the results
    */
    return runTest(pData, NULL, RULE_RESULT_BYPASSED, 3);
}


/*==========================================================================*
**
**  test_BR_LLSC_6_3_004
**
**  Description     :
**      Scan-Off e-Pass with Restricted Out-of-Zone
**      Unit-test Execute conditions.
**              Is within zone
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

int test_BR_LLSC_6_3_004(MYKI_BR_ContextData_t *pData)
{
    MYKI_TAControl_t       *pMYKI_TAControl         = NULL;

    if ( MYKI_CS_TAControlGet( &pMYKI_TAControl ) < 0 || pMYKI_TAControl == NULL )
    {
        return FALSE;
    }

    /*
    **  Test with
    **      Product in use
    **      Is an ePAss
    **      Disallow out of zone travel
    **      Within Zone
    */
    CardImage1(pData);
    pMYKI_TAControl->ProductInUse = 3;

    setProduct(pMYKI_TAControl, 1, PRODUCT_TYPE_WEEKLY, 5, 10);
    setProduct(pMYKI_TAControl, 2, PRODUCT_TYPE_WEEKLY, 5, 10);
    setProduct(pMYKI_TAControl, 3, PRODUCT_TYPE_EPASS , 5, 10);
    setProduct(pMYKI_TAControl, 4, PRODUCT_TYPE_WEEKLY, 5, 10);
    setProduct(pMYKI_TAControl, 5, PRODUCT_TYPE_WEEKLY, 5, 10);

    pData->Tariff.allowEPassOutOfZone = FALSE;

    pData->DynamicData.currentZone = 6;

    /*
    **  Run the Test and examine the results
    */
    return runTest(pData, NULL, RULE_RESULT_BYPASSED, 4);
}

/*==========================================================================*
**
**  test_BR_LLSC_6_3_005
**
**  Description     :
**      Scan-Off e-Pass with Restricted Out-of-Zone
**      Unit-test Execute conditions.
**              All Preconditions met
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

int test_BR_LLSC_6_3_005(MYKI_BR_ContextData_t *pData)
{
    MYKI_TAControl_t       *pMYKI_TAControl         = NULL;

    if ( MYKI_CS_TAControlGet( &pMYKI_TAControl ) < 0 || pMYKI_TAControl == NULL )
    {
        return FALSE;
    }

    /*
    **  Test with
    **      Product in use
    **      Is an ePAss
    **      Disallow out of zone travel
    **      Outside allowed Zone
    */
    CardImage1(pData);
    pMYKI_TAControl->ProductInUse = 3;

    setProduct(pMYKI_TAControl, 1, PRODUCT_TYPE_WEEKLY, 5, 10);
    setProduct(pMYKI_TAControl, 2, PRODUCT_TYPE_WEEKLY, 5, 10);
    setProduct(pMYKI_TAControl, 3, PRODUCT_TYPE_EPASS , 5, 10);
    setProduct(pMYKI_TAControl, 4, PRODUCT_TYPE_WEEKLY, 5, 10);
    setProduct(pMYKI_TAControl, 5, PRODUCT_TYPE_WEEKLY, 5, 10);

    pData->Tariff.allowEPassOutOfZone = FALSE;

    pData->DynamicData.currentZone = 11;
    
    /*
    **  Run the Test and examine the results
    */
    if ( !runTest(pData, NULL, RULE_RESULT_EXECUTED, 5))
    {
        return FALSE;
    }

    /*
    **  Extra examination
    */
    if ( pData->ReturnedData.rejectReason == MYKI_BR_REJECT_REASON_NOT_APPLICABLE)
    {
        return FALSE;
    }
    return TRUE;
}


