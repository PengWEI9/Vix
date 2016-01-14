/* -*- mode: c; tabs: 4; -*- */
/*=============================================================================
**
**    Vix Technology                   Licensed software
**    (C) 2013                         All rights reserved
**
**=============================================================================
**
**  Project/Product : MBU
**  Filename        : test_BR_LLSC_6_10.c
**  Author(s)       : David Purdie
**  Description     :
**      Implements BR_LLSC_6_10 business rule unit-tests.
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
**                      productType     - Ensure there is a product of this type
**                      tIf             - Reference to Test Interface Data
**                                        created by this function
**
** RETURNS            : FALSE           - Setup error
**
----------------------------------------------------------------------------*/
static int setupTest( MYKI_BR_ContextData_t *pData, ProductType_e productType,  testIf_t *tIf)
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
    **  A test with:
    **      Product in Use
    */
    CardImage1(pData);

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

    /*
    **  Setup a known product type - in product 1
    */
    {
        MYKI_Directory_t *pDirectory;

        if (myki_br_GetCardProduct(1, &pDirectory, NULL) >= 0)
        {
            pDirectory->ProductId = myki_br_cd_GetProductId(productType);
        }
        else
        {
            CsErrx("test_BR_LLSC_6_18 : setProduct::myki_br_GetCardProduct( %d ) failed", tIf->pMYKI_TAControl->ProductInUse);
        }
    }
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
    int rv = BR_LLSC_6_10(pData);

    if ( rv != rc )
    {
        CsVerbose("test_BR_LLSC_6_10 - Unexpected exit code. Got:%d, Expected:%d\n", rv, rc );
        return FALSE;
    }

    if ( pData && pData->ReturnedData.bypassCode != bc)
    {
        CsVerbose("test_BR_LLSC_6_10 - Unexpected ByPassCode. Got:%d, Expected:%d\n", pData->ReturnedData.bypassCode, bc);
        return FALSE;
    }
    return TRUE;
}

/*==========================================================================*
**
**  test_BR_LLSC_6_10_001
**
**  Description     :
**      Process Single Trip Products
**      Unit-test Execute conditions.
**              Null Data Pointer
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

int test_BR_LLSC_6_10_001(MYKI_BR_ContextData_t *pData)
{
    /*
    **  Run the Test and examine the results
    */
    return runTest(NULL, RULE_RESULT_ERROR, 0);
}

/*==========================================================================*
**
**  test_BR_LLSC_6_10_002
**
**  Description     :
**      Process Single Trip Products
**      Unit-test Execute conditions.
**              Product is in use
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

int test_BR_LLSC_6_10_002(MYKI_BR_ContextData_t *pData)
{
    testIf_t    tIf;

    /*
    **  Setup test conditions
    */
    if (setupTest( pData, PRODUCT_TYPE_WEEKLY, &tIf) < 0)
    {
        return FALSE;
    }
    tIf.pMYKI_TAControl->ProductInUse = 0;

    /*
    **  Run the Test and examine the results
    */
    return runTest(pData, RULE_RESULT_BYPASSED, 1 );
}

/*==========================================================================*
**
**  test_BR_LLSC_6_10_003
**
**  Description     :
**      Process Single Trip Products
**      Unit-test Execute conditions.
**              Not a provisional product
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

int test_BR_LLSC_6_10_003(MYKI_BR_ContextData_t *pData)
{
    testIf_t    tIf;

    /*
    **  Setup test conditions
    */
    if (setupTest( pData, PRODUCT_TYPE_WEEKLY, &tIf) < 0)
    {
        return FALSE;
    }
    tIf.pProductInUse->ControlBitmap &=  ~TAPP_TPRODUCT_CONTROL_PROVISIONAL_BITMAP;

    /*
    **  Run the Test and examine the results
    */
    return runTest(pData, RULE_RESULT_BYPASSED, 2);
}

/*==========================================================================*
**
**  test_BR_LLSC_6_10_004
**
**  Description     :
**      Process Single Trip Products
**      Unit-test Execute conditions.
**              No Single Trip Product
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

int test_BR_LLSC_6_10_004(MYKI_BR_ContextData_t *pData)
{
    testIf_t    tIf;

    /*
    **  Setup test conditions
    */
    if (setupTest( pData, PRODUCT_TYPE_WEEKLY, &tIf) < 0)
    {
        return FALSE;
    }

    /*
    **  Run the Test and examine the results
    */
    return runTest(pData, RULE_RESULT_BYPASSED, 4);
}

/*==========================================================================*
**
**  test_BR_LLSC_6_10_005
**
**  Description     :
**      Process Single Trip Products
**      Unit-test Execute conditions.
**              Single Trip Product
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

int test_BR_LLSC_6_10_005(MYKI_BR_ContextData_t *pData)
{
    testIf_t    tIf;

    /*
    **  Setup test conditions
    */
    if (setupTest( pData, PRODUCT_TYPE_SINGLE, &tIf) < 0)
    {
        return FALSE;
    }

    /*
    **  Run the Test and examine the results
    */
    return runTest(pData, RULE_RESULT_ERROR, 3);
}


