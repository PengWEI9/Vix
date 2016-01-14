/* -*- mode: c; tabs: 4; -*- */
/*=============================================================================
**
**    Vix Technology                   Licensed software
**    (C) 2013                         All rights reserved
**
**=============================================================================
**
**  Project/Product : MBU
**  Filename        : test_BR_LLSC_9_1.c
**  Author(s)       : David Purdie
**  Description     :
**      Implements BR_LLSC_9_1 business rule unit-tests.
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
            CsErrx("test_BR_LLSC_9_1 : setProduct::MYKI_CS_TAProductGet( %d ) failed", pIndex);
        }
    } else
    {
        CsErrx("test_BR_LLSC_9_1 : setProduct::myki_br_GetCardProduct( %d ) failed", pIndex);
    }
}

/*----------------------------------------------------------------------------
** FUNCTION           : testExistsProduct
**
** DESCRIPTION        : Test and see if a product exists
**
**
** INPUTS             : pMYKI_TAControl     - Pointer to data
**                      pIndex              - product index
**
** RETURNS            : -1                  - Internal error
**                       0                  - Product does not exist
**                       1                  - Product exists
**
----------------------------------------------------------------------------*/
static int testExistsProduct(MYKI_TAControl_t *pMYKI_TAControl, int pIndex)
{
    if (pMYKI_TAControl->Directory[pIndex].Status != TAPP_CONTROL_DIRECTORY_STATUS_UNUSED)
    {
        return 1;
    }
    return 0;
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
**                      tp              - > 0 Product to test for existence
**                                        < 0 Product to text for deletion
**
** RETURNS            : TRUE        - Test was OK
**                      FALSE       - Test Failed
**
----------------------------------------------------------------------------*/

static int runTest( MYKI_BR_ContextData_t *pData, MYKI_TAControl_t *pMYKI_TAControl, int rc, int tp)
{
    //ShowCardImage( "test1_before.txt" );
    int rv = BR_LLSC_9_1(pData);

    if ( rv != rc )
    {
        CsVerbose("test_BR_LLSC_9_1 - Unexpected exit code. Got:%d, Expected:%d\n", rv, rc );
        return FALSE;
    }

    /*
    **  Test product exsits or deleted
    */
    if ( tp != 0 && pMYKI_TAControl )
    {
        if ( tp > 0 )
        {
            if (testExistsProduct(pMYKI_TAControl, tp) != 1 )
            {
                CsVerbose("BR_LLSC_9_1 - Product deleted. Was not expected");
                return FALSE;
            }
        }
        else
        {
            if (testExistsProduct(pMYKI_TAControl, -tp) != 0)
            {

                CsVerbose("BR_LLSC_9_1 - Product Not deleted as expected");
                return FALSE;
            }
        }
    }

    return TRUE;
}


/*==========================================================================*
**
**  test_BR_LLSC_9_1_001
**
**  Description     :
**      Unit-test Execute conditions. Test with one covered daily product
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

int test_BR_LLSC_9_1_001(MYKI_BR_ContextData_t *pData)
{
    MYKI_TACapping_t       *pMYKI_TACapping         = NULL;
    MYKI_TAControl_t       *pMYKI_TAControl         = NULL;

    if (pData == NULL)
    {
        return FALSE;
    }   /* end-of-if */

    if (MYKI_CS_TACappingGet(&pMYKI_TACapping) < 0 || pMYKI_TACapping == NULL)
    {
        return FALSE;
    }   /* end-of-if */

    if (MYKI_CS_TAControlGet(&pMYKI_TAControl) < 0 || pMYKI_TAControl == NULL)
    {
        return FALSE;
    }   /* end-of-if */

    /*
    ** Test with bad data pointer
    */
    CsVerbose("+BR_LLSC_9_1 - Test with NULL data pointer");
    if (BR_LLSC_9_1(NULL) != RULE_RESULT_ERROR)
    {
        return FALSE;
    }


    /*
    ** Setup various products on the card 
    ** Basic assumtions 
    **      - the card can hold 5 products
    **      - product index 0 is special and not a part of this test
    **  
    ** Set up prodcuts as: 
    **      
    */
    CardImage1(pData);
    setProduct(pMYKI_TAControl, 1, PRODUCT_TYPE_WEEKLY, 1, 10);     // Product 1. Weekly. zoneLow: 1 , zoneHigh 10
    setProduct(pMYKI_TAControl, 2, PRODUCT_TYPE_WEEKLY, 5, 15);     // Product 2. Weekly. zoneLow: 5 , zoneHigh 15
    setProduct(pMYKI_TAControl, 3, PRODUCT_TYPE_WEEKLY, 15, 25);    // Product 3. Weekly. zoneLow: 15, zoneHigh 25
    setProduct(pMYKI_TAControl, 4, PRODUCT_TYPE_DAILY, 7, 13);      // Product 4. Daily.  zoneLow: 7,  zoneHigh 13  - Covered by Product 2
    setProduct(pMYKI_TAControl, 5, PRODUCT_TYPE_DAILY, 6, 16);      // Product 5. Daily.  zoneLow: 6,  zoneHigh 16

    /*
    **  Run the test and examine results
    */
    return runTest(pData, pMYKI_TAControl, RULE_RESULT_EXECUTED, -4 );
}

/*==========================================================================*
**
**  test_BR_LLSC_9_1_002
**
**  Description     :
**      Unit-test Execute conditions. Test with one covered border nhour product
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

int test_BR_LLSC_9_1_002(MYKI_BR_ContextData_t *pData)
{
    MYKI_TACapping_t       *pMYKI_TACapping         = NULL;
    MYKI_TAControl_t       *pMYKI_TAControl         = NULL;

    if (pData == NULL)
    {
        return FALSE;
    }   /* end-of-if */

    if (MYKI_CS_TACappingGet(&pMYKI_TACapping) < 0 || pMYKI_TACapping == NULL)
    {
        return FALSE;
    }   /* end-of-if */

    if (MYKI_CS_TAControlGet(&pMYKI_TAControl) < 0 || pMYKI_TAControl == NULL)
    {
        return FALSE;
    }   /* end-of-if */

    /*
    **  Test with one covered nhour product
    **  which is a border product
    */
    CardImage1(pData);
    setProduct(pMYKI_TAControl, 1, PRODUCT_TYPE_WEEKLY, 1, 10);     // Product 1. Weekly. zoneLow: 1 , zoneHigh 10
    setProduct(pMYKI_TAControl, 2, PRODUCT_TYPE_WEEKLY, 5, 15);     // Product 2. Weekly. zoneLow: 5 , zoneHigh 15
    setProduct(pMYKI_TAControl, 3, PRODUCT_TYPE_WEEKLY, 15, 25);    // Product 3. Weekly. zoneLow: 15, zoneHigh 25
    setProduct(pMYKI_TAControl, 4, PRODUCT_TYPE_NHOUR, 7, 13);      // Product 4. nHour.  zoneLow: 7,  zoneHigh 13  - Covered by Product 2
    if (pProduct)
    {
        pProduct->ControlBitmap = TAPP_TPRODUCT_CONTROL_BORDER_STATUS_BITMAP;
    }
    setProduct(pMYKI_TAControl, 5, PRODUCT_TYPE_NHOUR, 6, 16);  // Product 5. nHour.  zoneLow: 6,  zoneHigh 16

    /*
    **  Run the test and examine results
    */
    return runTest(pData, pMYKI_TAControl, RULE_RESULT_EXECUTED, 4 );
}

/*==========================================================================*
**
**  test_BR_LLSC_9_1_003
**
**  Description     :
**      Unit-test Execute conditions.
**      Test with one covered non-border nhour product. Expire test
**      the expiry date of the n-hour is NOT less than or equal to the expiry of
**      the weekly product
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

int test_BR_LLSC_9_1_003(MYKI_BR_ContextData_t *pData)
{
    MYKI_TACapping_t       *pMYKI_TACapping         = NULL;
    MYKI_TAControl_t       *pMYKI_TAControl         = NULL;
    Time_t                  savedTime;

    if (pData == NULL)
    {
        return FALSE;
    }   /* end-of-if */

    if (MYKI_CS_TACappingGet(&pMYKI_TACapping) < 0 || pMYKI_TACapping == NULL)
    {
        return FALSE;
    }   /* end-of-if */

    if (MYKI_CS_TAControlGet(&pMYKI_TAControl) < 0 || pMYKI_TAControl == NULL)
    {
        return FALSE;
    }   /* end-of-if */
    /*
    **  Test with one covered nhour product
    **  which is not a border product, but the expiry date of the n-hour is NOT less than or equal to the expiry of
    **  the weekly product
    **  
    */
    CardImage1(pData);
    setProduct(pMYKI_TAControl, 1, PRODUCT_TYPE_WEEKLY, 1, 10);     // Product 1. Weekly. zoneLow: 1 , zoneHigh 10
    setProduct(pMYKI_TAControl, 2, PRODUCT_TYPE_WEEKLY, 5, 15);     // Product 2. Weekly. zoneLow: 5 , zoneHigh 15
    if (pProduct)
    {
        savedTime = pProduct->EndDateTime;
    }
    setProduct(pMYKI_TAControl, 3, PRODUCT_TYPE_WEEKLY, 15, 25);    // Product 3. Weekly. zoneLow: 15, zoneHigh 25
    setProduct(pMYKI_TAControl, 4, PRODUCT_TYPE_NHOUR, 7, 13);      // Product 4. nHour.  zoneLow: 7,  zoneHigh 13  - Covered by Product 2
    if (pProduct)
    {
        pProduct->ControlBitmap = TAPP_TPRODUCT_CONTROL_PREMIUM_BITMAP;
        pProduct->EndDateTime = savedTime + 10;
    }
    setProduct(pMYKI_TAControl, 5, PRODUCT_TYPE_NHOUR, 6, 16);  // Product 5. nHour.  zoneLow: 6,  zoneHigh 16

    /*
    **  Run the test and examine results
    */
    return runTest(pData, pMYKI_TAControl, RULE_RESULT_EXECUTED, 4 );
}

/*==========================================================================*
**
**  test_BR_LLSC_9_1_004
**
**  Description     :
**      Unit-test Execute conditions.
**      Test with one covered non-border nhour product
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

int test_BR_LLSC_9_1_004(MYKI_BR_ContextData_t *pData)
{
    MYKI_TACapping_t       *pMYKI_TACapping         = NULL;
    MYKI_TAControl_t       *pMYKI_TAControl         = NULL;

    if (pData == NULL)
    {
        return FALSE;
    }   /* end-of-if */

    if (MYKI_CS_TACappingGet(&pMYKI_TACapping) < 0 || pMYKI_TACapping == NULL)
    {
        return FALSE;
    }   /* end-of-if */

    if (MYKI_CS_TAControlGet(&pMYKI_TAControl) < 0 || pMYKI_TAControl == NULL)
    {
        return FALSE;
    }   /* end-of-if */
    /*
    **  Test with one covered nhour product
    **  which is not a border product
    */
    CardImage1(pData);
    setProduct(pMYKI_TAControl, 1, PRODUCT_TYPE_WEEKLY, 1, 10);     // Product 1. Weekly. zoneLow: 1 , zoneHigh 10
    setProduct(pMYKI_TAControl, 2, PRODUCT_TYPE_WEEKLY, 5, 15);     // Product 2. Weekly. zoneLow: 5 , zoneHigh 15
    setProduct(pMYKI_TAControl, 3, PRODUCT_TYPE_WEEKLY, 15, 25);    // Product 3. Weekly. zoneLow: 15, zoneHigh 25
    setProduct(pMYKI_TAControl, 4, PRODUCT_TYPE_NHOUR, 7, 13);      // Product 4. nHour.  zoneLow: 7,  zoneHigh 13  - Covered by Product 2
    if (pProduct)
    {
        pProduct->ControlBitmap = TAPP_TPRODUCT_CONTROL_PREMIUM_BITMAP;
    }
    setProduct(pMYKI_TAControl, 5, PRODUCT_TYPE_NHOUR, 6, 16);  // Product 5. nHour.  zoneLow: 6,  zoneHigh 16


    /*
    **  Run the test and examine results
    */
    return runTest(pData, pMYKI_TAControl, RULE_RESULT_EXECUTED, -4 );
}

/*==========================================================================*
**
**  test_BR_LLSC_9_1_005
**
**  Description     :
**      Unit-test Execute conditions.
**      Single Product covered by Daily Product
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

int test_BR_LLSC_9_1_005(MYKI_BR_ContextData_t *pData)
{
    MYKI_TACapping_t       *pMYKI_TACapping         = NULL;
    MYKI_TAControl_t       *pMYKI_TAControl         = NULL;

    if (pData == NULL)
    {
        return FALSE;
    }   /* end-of-if */

    if (MYKI_CS_TACappingGet(&pMYKI_TACapping) < 0 || pMYKI_TACapping == NULL)
    {
        return FALSE;
    }   /* end-of-if */

    if (MYKI_CS_TAControlGet(&pMYKI_TAControl) < 0 || pMYKI_TAControl == NULL)
    {
        return FALSE;
    }   /* end-of-if */
    /*
    **  Single Covered by Other product
    **  Covered by a daily product
    */
    CardImage1(pData);
    setProduct(pMYKI_TAControl, 1, PRODUCT_TYPE_DAILY, 1, 10);     // Product 1. Weekly. zoneLow: 1 , zoneHigh 10
    setProduct(pMYKI_TAControl, 2, PRODUCT_TYPE_DAILY, 5, 15);     // Product 2. Weekly. zoneLow: 5 , zoneHigh 15
    setProduct(pMYKI_TAControl, 3, PRODUCT_TYPE_DAILY, 15, 25);    // Product 3. Weekly. zoneLow: 15, zoneHigh 25
    setProduct(pMYKI_TAControl, 4, PRODUCT_TYPE_SINGLE, 7, 13);      // Product 4. nHour.  zoneLow: 7,  zoneHigh 13  - Covered by Product 2
    setProduct(pMYKI_TAControl, 5, PRODUCT_TYPE_SINGLE, 6, 16);  // Product 5. nHour.  zoneLow: 6,  zoneHigh 16

    /*
    **  Run the test and examine results
    */
    return runTest(pData, pMYKI_TAControl, RULE_RESULT_EXECUTED, -4 );
}

/*==========================================================================*
**
**  test_BR_LLSC_9_1_006
**
**  Description     :
**      Unit-test Execute conditions.
**      Single Product covered by Weekly Product
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

int test_BR_LLSC_9_1_006(MYKI_BR_ContextData_t *pData)
{
    MYKI_TACapping_t       *pMYKI_TACapping         = NULL;
    MYKI_TAControl_t       *pMYKI_TAControl         = NULL;

    if (pData == NULL)
    {
        return FALSE;
    }   /* end-of-if */

    if (MYKI_CS_TACappingGet(&pMYKI_TACapping) < 0 || pMYKI_TACapping == NULL)
    {
        return FALSE;
    }   /* end-of-if */

    if (MYKI_CS_TAControlGet(&pMYKI_TAControl) < 0 || pMYKI_TAControl == NULL)
    {
        return FALSE;
    }   /* end-of-if */
    /*
    **  Single Covered by Other product
    **  Covered by a Weekly product
    */
    CardImage1(pData);
    setProduct(pMYKI_TAControl, 1, PRODUCT_TYPE_WEEKLY, 1, 10);     // Product 1. Weekly. zoneLow: 1 , zoneHigh 10
    setProduct(pMYKI_TAControl, 2, PRODUCT_TYPE_WEEKLY, 5, 15);     // Product 2. Weekly. zoneLow: 5 , zoneHigh 15
    setProduct(pMYKI_TAControl, 3, PRODUCT_TYPE_WEEKLY, 15, 25);    // Product 3. Weekly. zoneLow: 15, zoneHigh 25
    setProduct(pMYKI_TAControl, 4, PRODUCT_TYPE_SINGLE, 7, 13);      // Product 4. nHour.  zoneLow: 7,  zoneHigh 13  - Covered by Product 2
    setProduct(pMYKI_TAControl, 5, PRODUCT_TYPE_SINGLE, 6, 16);  // Product 5. nHour.  zoneLow: 6,  zoneHigh 16

    /*
    **  Run the test and examine results
    */
    return runTest(pData, pMYKI_TAControl, RULE_RESULT_EXECUTED, -4 );
}

/*==========================================================================*
**
**  test_BR_LLSC_9_1_007
**
**  Description     :
**      Unit-test Execute conditions.
**      Single Product covered by an NOT activated ePass Product
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

int test_BR_LLSC_9_1_007(MYKI_BR_ContextData_t *pData)
{
    MYKI_TACapping_t       *pMYKI_TACapping         = NULL;
    MYKI_TAControl_t       *pMYKI_TAControl         = NULL;

    if (pData == NULL)
    {
        return FALSE;
    }   /* end-of-if */

    if (MYKI_CS_TACappingGet(&pMYKI_TACapping) < 0 || pMYKI_TACapping == NULL)
    {
        return FALSE;
    }   /* end-of-if */

    if (MYKI_CS_TAControlGet(&pMYKI_TAControl) < 0 || pMYKI_TAControl == NULL)
    {
        return FALSE;
    }   /* end-of-if */
    /*
    **  Single Covered by Other product
    **  Covered by a an NOT activated ePass
    */
    CardImage1(pData);
    setProduct(pMYKI_TAControl, 1, PRODUCT_TYPE_EPASS, 1, 10);     // Product 1. Weekly. zoneLow: 1 , zoneHigh 10
    setProduct(pMYKI_TAControl, 2, PRODUCT_TYPE_EPASS, 5, 15);     // Product 2. Weekly. zoneLow: 5 , zoneHigh 15
    if (pDirectory)
    {
        pDirectory->Status = TAPP_CONTROL_STATUS_ISSUED;
    }
    setProduct(pMYKI_TAControl, 3, PRODUCT_TYPE_EPASS, 15, 25);  // Product 3. Weekly. zoneLow: 15, zoneHigh 25
    setProduct(pMYKI_TAControl, 4, PRODUCT_TYPE_SINGLE, 7, 13);     // Product 4. nHour.  zoneLow: 7,  zoneHigh 13  - Covered by Product 2
    setProduct(pMYKI_TAControl, 5, PRODUCT_TYPE_SINGLE, 6, 16);  // Product 5. nHour.  zoneLow: 6,  zoneHigh 16

    /*
    **  Run the test and examine results
    */
    return runTest(pData, pMYKI_TAControl, RULE_RESULT_EXECUTED, 4 );
}

/*==========================================================================*
**
**  test_BR_LLSC_9_1_008
**
**  Description     :
**      Unit-test Execute conditions.
**      Single Product covered by an activated ePass Product
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

int test_BR_LLSC_9_1_008(MYKI_BR_ContextData_t *pData)
{
    MYKI_TACapping_t       *pMYKI_TACapping         = NULL;
    MYKI_TAControl_t       *pMYKI_TAControl         = NULL;

    if (pData == NULL)
    {
        return FALSE;
    }   /* end-of-if */

    if (MYKI_CS_TACappingGet(&pMYKI_TACapping) < 0 || pMYKI_TACapping == NULL)
    {
        return FALSE;
    }   /* end-of-if */

    if (MYKI_CS_TAControlGet(&pMYKI_TAControl) < 0 || pMYKI_TAControl == NULL)
    {
        return FALSE;
    }   /* end-of-if */
    /*
    **  Single Covered by Other product
    **  Covered by a an activated ePass
    */
    CardImage1(pData);
    setProduct(pMYKI_TAControl, 1, PRODUCT_TYPE_EPASS, 1, 10);     // Product 1. Weekly. zoneLow: 1 , zoneHigh 10
    setProduct(pMYKI_TAControl, 2, PRODUCT_TYPE_EPASS, 5, 15);     // Product 2. Weekly. zoneLow: 5 , zoneHigh 15
    if (pDirectory)
    {
        pDirectory->Status = TAPP_CONTROL_STATUS_ACTIVATED;
    }
    setProduct(pMYKI_TAControl, 3, PRODUCT_TYPE_EPASS, 15, 25);  // Product 3. Weekly. zoneLow: 15, zoneHigh 25
    setProduct(pMYKI_TAControl, 4, PRODUCT_TYPE_SINGLE, 7, 13);     // Product 4. nHour.  zoneLow: 7,  zoneHigh 13  - Covered by Product 2
    setProduct(pMYKI_TAControl, 5, PRODUCT_TYPE_SINGLE, 6, 16);  // Product 5. nHour.  zoneLow: 6,  zoneHigh 16

    /*
    **  Run the test and examine results
    */
    return runTest(pData, pMYKI_TAControl, RULE_RESULT_EXECUTED, -4 );
}

/*==========================================================================*
**
**  test_BR_LLSC_9_1_009
**
**  Description     :
**      Unit-test Execute conditions.
**      Covered Product replaces In Use Product
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

int test_BR_LLSC_9_1_009(MYKI_BR_ContextData_t *pData)
{
    MYKI_TACapping_t       *pMYKI_TACapping         = NULL;
    MYKI_TAControl_t       *pMYKI_TAControl         = NULL;
    int                     rv;

    if (pData == NULL)
    {
        return FALSE;
    }   /* end-of-if */

    if (MYKI_CS_TACappingGet(&pMYKI_TACapping) < 0 || pMYKI_TACapping == NULL)
    {
        return FALSE;
    }   /* end-of-if */

    if (MYKI_CS_TAControlGet(&pMYKI_TAControl) < 0 || pMYKI_TAControl == NULL)
    {
        return FALSE;
    }   /* end-of-if */

    /*
    ** Setup various products on the card 
    ** Basic assumtions 
    **      - the card can hold 5 products
    **      - product index 0 is special and not a part of this test
    **  
    ** Set up prodcuts as: 
    **      
    */
    CardImage1(pData);
    setProduct(pMYKI_TAControl, 1, PRODUCT_TYPE_WEEKLY, 1, 10);     // Product 1. Weekly. zoneLow: 1 , zoneHigh 10
    setProduct(pMYKI_TAControl, 2, PRODUCT_TYPE_WEEKLY, 5, 15);     // Product 2. Weekly. zoneLow: 5 , zoneHigh 15
    setProduct(pMYKI_TAControl, 3, PRODUCT_TYPE_WEEKLY, 15, 25);    // Product 3. Weekly. zoneLow: 15, zoneHigh 25
    setProduct(pMYKI_TAControl, 4, PRODUCT_TYPE_DAILY, 7, 13);      // Product 4. Daily.  zoneLow: 7,  zoneHigh 13  - Covered by Product 2
    setProduct(pMYKI_TAControl, 5, PRODUCT_TYPE_DAILY, 6, 16);      // Product 5. Daily.  zoneLow: 6,  zoneHigh 16

    /*
    ** Setup the product in use to be the one that will be deleted
    */
    pMYKI_TAControl->ProductInUse = 2;

    /*
    **  Run the test and examine results
    */
    rv = runTest(pData, pMYKI_TAControl, RULE_RESULT_EXECUTED, -4 );
    if (!rv)
    {
        return FALSE;
    }

    if (pMYKI_TAControl->ProductInUse != 2)
    {
        CsVerbose("BR_LLSC_9_1 - Product in use not updated correctly");
        return FALSE;
    }

    /*
    ** All is good
    */
    return TRUE;
}

