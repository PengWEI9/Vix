/* -*- mode: c; tabs: 4; -*- */
/*=============================================================================
**
**    Vix Technology                   Licensed software
**    (C) 2013                         All rights reserved
**
**=============================================================================
**
**  Project/Product : MBU
**  Filename        : test_BR_LLSC_7_4.c
**  Author(s)       : David Purdie
**  Description     :
**      Implements BR_LLSC_7_4 business rule unit-tests.
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
            if (pType == PRODUCT_TYPE_EPASS)
            {
                pProduct->InstanceCount = 30 /*DAYS*/;
            }
        } else
        {
            CsErrx("test_BR_LLSC_7_4 : setProduct::MYKI_CS_TAProductGet( %d ) failed", pIndex);
        }
    } else
    {
        CsErrx("test_BR_LLSC_7_4 : setProduct::myki_br_GetCardProduct( %d ) failed", pIndex);
    }
}

/*----------------------------------------------------------------------------
** FUNCTION           : dumpZones
**
** DESCRIPTION        : Dump out a part of the calculated zone map
**
**
** INPUTS             : pMYKI_TAControl     - Pointer to data
**
** RETURNS            :
**
----------------------------------------------------------------------------*/
static void dumpZones(MYKI_BR_ContextData_t *pData)
{
    int zone;
    for ( zone = pData->InternalData.ZoneMap.zoneLow; zone <= pData->InternalData.ZoneMap.zoneHigh; zone++ )
    {
        CsVerbose("test_BR_LLSC_7_4 - Zone: %3d, Mask: %x, DirIndex: %d\n",
                zone,
                pData->InternalData.ZoneMap.zoneList[ zone ].priority,
                pData->InternalData.ZoneMap.zoneList[ zone ].dirIndex );
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
**                      inUse           - Expected Product in use. if > 0
**
** RETURNS            : TRUE        - Test was OK
**                      FALSE       - Test Failed
**
----------------------------------------------------------------------------*/

static int runTest( MYKI_BR_ContextData_t *pData, MYKI_TAControl_t *pMYKI_TAControl, int rc, int bc, int inUse)
{
    //ShowCardImage( "test1_before.txt" );
    int rv = BR_LLSC_7_4(pData);

    /*
    **  Always reset for next test
    */
    MYKI_CD_setStartDateTime(0);

    if ( rv != rc )
    {
        CsVerbose("test_BR_LLSC_7_4 - Unexpected exit code. Got:%d, Expected:%d\n", rv, rc );
        dumpZones(pData);
        return FALSE;
    }

    if ( pData->ReturnedData.bypassCode != bc)
    {
        CsVerbose("test_BR_LLSC_7_4 - Unexpected ByPassCode. Got:%d, Expected:%d\n", pData->ReturnedData.bypassCode, bc);
        dumpZones(pData);
        return FALSE;
    }

    /*
    **  The active product should have changed
    */
    if ( inUse > 0 && pMYKI_TAControl )
    {
        if ( pMYKI_TAControl->ProductInUse != inUse )
        {
            CsVerbose("test_BR_LLSC_7_4 - Product In Use incorrect. Got:%d, Expected:%d", pMYKI_TAControl->ProductInUse, inUse);
            return FALSE;
        }
    }

    return TRUE;
}


/*==========================================================================*
**
**  test_BR_LLSC_7_4_001
**
**  Description     :
**      Expired Tariff Full Product Coverage
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

int test_BR_LLSC_7_4_001(MYKI_BR_ContextData_t *pData)
{

    /*
    ** Test with bad data pointer
    */
    CsVerbose("+BR_LLSC_7_4 - Test with NULL data pointer");
    if (BR_LLSC_7_4(NULL) != RULE_RESULT_ERROR)
    {
        return FALSE;
    }

    /*
    **  Test with product not in use
    */
    CardImage1(pData);

    /*
    **  Run the Test and examine the results
    */
    return runTest(pData, NULL, RULE_RESULT_BYPASSED, 1, 0 );
}

/*==========================================================================*
**
**  test_BR_LLSC_7_4_002
**
**  Description     :
**      Expired Tariff Full Product Coverage
**      Unit-test Execute conditions.
**              Provisional Capping Option not Option3
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

int test_BR_LLSC_7_4_002(MYKI_BR_ContextData_t *pData)
{
    MYKI_TAControl_t       *pMYKI_TAControl         = NULL;

    if ( MYKI_CS_TAControlGet( &pMYKI_TAControl ) < 0 || pMYKI_TAControl == NULL )
    {
        return FALSE;
    }

    /*
    **  Test with product in use
    **  Provisional Capping Option set to Option3
    */
    CardImage1(pData);
    pMYKI_TAControl->ProductInUse = 1;
    pData->Tariff.provisionalCappingOption = 3;
    
    /*
    **  Run the Test and examine the results
    */
    return runTest(pData, NULL, RULE_RESULT_BYPASSED, 2, 0 );
}

/*==========================================================================*
**
**  test_BR_LLSC_7_4_003
**
**  Description     :
**      Expired Tariff Full Product Coverage
**      Unit-test Execute conditions.
**              Invalid Tariff Scan On Date
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

int test_BR_LLSC_7_4_003(MYKI_BR_ContextData_t *pData)
{
    MYKI_TAControl_t       *pMYKI_TAControl         = NULL;

    if ( MYKI_CS_TAControlGet( &pMYKI_TAControl ) < 0 || pMYKI_TAControl == NULL )
    {
        return FALSE;
    }

    /*
    **  Test with product in use
    **  Provisional Capping Option NOT set to Option3
    **  ProductInUse is a provisional product
    **  Force bad Tariff Date
    */
    CardImage1(pData);

    pData->Tariff.provisionalCappingOption = 2;
    pMYKI_TAControl->ProductInUse = 1;

    /*
    **  Setup the provisional product
    */
    setProduct(pMYKI_TAControl, 1, PRODUCT_TYPE_WEEKLY, 5, 10);     // Product 1. Weekly. zoneLow: 5 , zoneHigh 10
    if ( pProduct )
    {
        pProduct->ControlBitmap |=  TAPP_TPRODUCT_CONTROL_PROVISIONAL_BITMAP;
    }

    /*
    **  Force good Tariff Date
    */
    MYKI_CD_setStartDateTime(pProduct->LastUsage.DateTime - 100);

    /*
    **  Run the Test and examine the results
    */
    return runTest(pData, NULL, RULE_RESULT_BYPASSED, 3, 0 );
}


/*==========================================================================*
**
**  test_BR_LLSC_7_4_004
**
**  Description     :
**      Expired Tariff Full Product Coverage
**      Unit-test Execute conditions.
**              ProductInUse is not a provisional product
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

int test_BR_LLSC_7_4_004(MYKI_BR_ContextData_t *pData)
{
    MYKI_TAControl_t       *pMYKI_TAControl         = NULL;

    if ( MYKI_CS_TAControlGet( &pMYKI_TAControl ) < 0 || pMYKI_TAControl == NULL )
    {
        return FALSE;
    }

    /*
    **  Test with product in use
    **  Provisional Capping Option NOT set to Option3
    **  ProductInUse is not a provisional product
    **  Invalid Tariff
    */
    CardImage1(pData);
    pMYKI_TAControl->ProductInUse = 1;
    pData->Tariff.provisionalCappingOption = 2;
    MYKI_CD_setStartDateTime(pProduct->LastUsage.DateTime + 100);

    /*
    **  Run the Test and examine the results
    */
    return runTest(pData, NULL, RULE_RESULT_BYPASSED, 4, 0 );
}

/*==========================================================================*
**
**  test_BR_LLSC_7_4_005
**
**  Description     :
**      Expired Tariff Full Product Coverage
**      Unit-test Execute conditions.
**              Zero product purchase value
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

int test_BR_LLSC_7_4_005(MYKI_BR_ContextData_t *pData)
{
    MYKI_TAControl_t       *pMYKI_TAControl         = NULL;
    MYKI_TAProduct_t       *pMYKI_TAProduct         = NULL;

    if ( MYKI_CS_TAControlGet( &pMYKI_TAControl ) < 0 || pMYKI_TAControl == NULL )
    {
        return FALSE;
    }

    /*
    **  Test with product in use
    **  Provisional Capping Option NOT set to Option3
    **  ProductInUse is a provisional product
    **  Invalid Tariff
    **  Zero product purchase value
    */
    CardImage1(pData);
    pMYKI_TAControl->ProductInUse = 1;
    pData->Tariff.provisionalCappingOption = 2;
    MYKI_CD_setStartDateTime(pProduct->LastUsage.DateTime + 100);

    if ( MYKI_CS_TAProductGet( (U8_t)( pMYKI_TAControl->ProductInUse - 1 ), &pMYKI_TAProduct ) < 0 || pMYKI_TAProduct == NULL )
    {
        return FALSE;
    }
    pMYKI_TAProduct->ControlBitmap |=  TAPP_TPRODUCT_CONTROL_PROVISIONAL_BITMAP;
    pMYKI_TAProduct->PurchaseValue = 0;
    
    /*
    **  Run the Test and examine the results
    */
    return runTest(pData, NULL, RULE_RESULT_BYPASSED, 5, 0 );
}

/*==========================================================================*
**
**  test_BR_LLSC_7_4_006
**
**  Description     :
**      Expired Tariff Full Product Coverage
**      Unit-test Execute conditions.
**              No Coverage
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

int test_BR_LLSC_7_4_006(MYKI_BR_ContextData_t *pData)
{
    MYKI_TAControl_t       *pMYKI_TAControl         = NULL;

    if ( MYKI_CS_TAControlGet( &pMYKI_TAControl ) < 0 || pMYKI_TAControl == NULL )
    {
        return FALSE;
    }

    /*
    **  Test with product in use
    **  Provisional Capping Option NOT set to Option3
    **  ProductInUse is a provisional product
    **  Invalid Tariff
    **  Non-Zero product purchase value
    **  No Coverage
    */
    CardImage1(pData);

    pData->Tariff.provisionalCappingOption = 2;
    pMYKI_TAControl->ProductInUse = 1;
    MYKI_CD_setStartDateTime(pProduct->LastUsage.DateTime + 100);

    /*
    **  Setup the provisional product
    */
    setProduct(pMYKI_TAControl, 1, PRODUCT_TYPE_WEEKLY, 5, 10);     // Product 1. Weekly. zoneLow: 5 , zoneHigh 10
    if ( pProduct )
    {
        pProduct->ControlBitmap |=  TAPP_TPRODUCT_CONTROL_PROVISIONAL_BITMAP;
        pProduct->PurchaseValue = 100;
    }

    /*
    **  Setup other products - no coverage
    */
    setProduct(pMYKI_TAControl, 2, PRODUCT_TYPE_WEEKLY, 1, 2);
    setProduct(pMYKI_TAControl, 3, PRODUCT_TYPE_WEEKLY, 1, 2);
    setProduct(pMYKI_TAControl, 4, PRODUCT_TYPE_WEEKLY, 1, 2);
    setProduct(pMYKI_TAControl, 5, PRODUCT_TYPE_WEEKLY, 1, 2);

    /*
    **  Run the Test and examine the results
    */
    return runTest(pData, pMYKI_TAControl, RULE_RESULT_BYPASSED, 6, 1 );
}

/*==========================================================================*
**
**  test_BR_LLSC_7_4_007
**
**  Description     :
**      Expired Tariff Full Product Coverage
**      Unit-test Execute conditions.
**              Active ePass
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

int test_BR_LLSC_7_4_007(MYKI_BR_ContextData_t *pData)
{
    MYKI_TAControl_t       *pMYKI_TAControl         = NULL;

    if ( MYKI_CS_TAControlGet( &pMYKI_TAControl ) < 0 || pMYKI_TAControl == NULL )
    {
        return FALSE;
    }

    /*
    **  Test with product in use
    **  Provisional Capping Option NOT set to Option3
    **  Invalid Tariff
    **  ProductInUse is a provisional product
    **  Non-Zero product purchase value
    **      Active ePass
    */
    CardImage1(pData);

    pData->Tariff.provisionalCappingOption = 2;
    pMYKI_TAControl->ProductInUse = 1;
    MYKI_CD_setStartDateTime(pProduct->LastUsage.DateTime + 100);

    /*
    **  Setup the provisional product
    */
    setProduct(pMYKI_TAControl, 1, PRODUCT_TYPE_WEEKLY, 5, 10);     // Product 1. Weekly. zoneLow: 5 , zoneHigh 10
    if ( pProduct )
    {
        pProduct->ControlBitmap |=  TAPP_TPRODUCT_CONTROL_PROVISIONAL_BITMAP;
        pProduct->PurchaseValue = 100;
    }

    /*
    **  Activated ePass
    */
    setProduct(pMYKI_TAControl, 2, PRODUCT_TYPE_EPASS, 4, 11);
    pMYKI_TAControl->Directory[2].Status = TAPP_CONTROL_DIRECTORY_STATUS_ACTIVATED;

    /*
    **  Setup other products - no coverage
    */
    setProduct(pMYKI_TAControl, 3, PRODUCT_TYPE_DAILY, 1, 2);
    setProduct(pMYKI_TAControl, 4, PRODUCT_TYPE_DAILY, 1, 2);
    setProduct(pMYKI_TAControl, 5, PRODUCT_TYPE_DAILY, 1, 2);

    /*
    **  Run the Test and examine the results
    */
    return runTest(pData, pMYKI_TAControl, RULE_RESULT_EXECUTED, 7, 0 );
}

/*==========================================================================*
**
**  test_BR_LLSC_7_4_008
**
**  Description     :
**      Expired Tariff Full Product Coverage
**      Unit-test Execute conditions.
**              InActive ePass - Future Start
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

int test_BR_LLSC_7_4_008(MYKI_BR_ContextData_t *pData)
{
    MYKI_TAControl_t       *pMYKI_TAControl         = NULL;

    if ( MYKI_CS_TAControlGet( &pMYKI_TAControl ) < 0 || pMYKI_TAControl == NULL )
    {
        return FALSE;
    }

    /*
    **  Test with product in use
    **  Provisional Capping Option NOT set to Option3
    **  Invalid Tariff
    **  ProductInUse is a provisional product
    **  Non-Zero product purchase value
    **  InActive ePass, Start Date in the future
    */
    CardImage1(pData);

    pData->Tariff.provisionalCappingOption = 2;
    pMYKI_TAControl->ProductInUse = 1;
    MYKI_CD_setStartDateTime(pProduct->LastUsage.DateTime + 100);

    /*
    **  Setup the provisional product
    */
    setProduct(pMYKI_TAControl, 1, PRODUCT_TYPE_WEEKLY, 5, 10);     // Product 1. Weekly. zoneLow: 5 , zoneHigh 10
    if ( pProduct )
    {
        pProduct->ControlBitmap |=  TAPP_TPRODUCT_CONTROL_PROVISIONAL_BITMAP;
        pProduct->PurchaseValue = 100;
        pProduct->StartDateTime = 1000000;
    }

    /*
    **  InActivated ePass
    */
    setProduct(pMYKI_TAControl, 2, PRODUCT_TYPE_EPASS, 1, 2);
    if ( pProduct )
    {
        pProduct->StartDateTime = 1000001;
    }

    /*
    **  Setup other products - no coverage
    */
    setProduct(pMYKI_TAControl, 3, PRODUCT_TYPE_DAILY, 1, 2);
    setProduct(pMYKI_TAControl, 4, PRODUCT_TYPE_DAILY, 1, 2);
    setProduct(pMYKI_TAControl, 5, PRODUCT_TYPE_DAILY, 1, 2);

    /*
    **  Run the Test and examine the results
    */
    return runTest(pData, pMYKI_TAControl, RULE_RESULT_BYPASSED, 6, 1 );
}

/*==========================================================================*
**
**  test_BR_LLSC_7_4_009
**
**  Description     :
**      Expired Tariff Full Product Coverage
**      Unit-test Execute conditions.
**              InActive ePass - Past Start
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

int test_BR_LLSC_7_4_009(MYKI_BR_ContextData_t *pData)
{
    MYKI_TAControl_t       *pMYKI_TAControl         = NULL;

    if ( MYKI_CS_TAControlGet( &pMYKI_TAControl ) < 0 || pMYKI_TAControl == NULL )
    {
        return FALSE;
    }

    /*
    **  Test with product in use
    **  Provisional Capping Option NOT set to Option3
    **  Invalid Tariff
    **  ProductInUse is a provisional product
    **  Non-Zero product purchase value
    **  InActive ePass, Start Date in the past
    */
    CardImage1(pData);

    pData->Tariff.provisionalCappingOption = 2;
    pMYKI_TAControl->ProductInUse = 1;
    MYKI_CD_setStartDateTime(pProduct->LastUsage.DateTime + 100);

    /*
    **  Setup the provisional product
    */
    setProduct(pMYKI_TAControl, 1, PRODUCT_TYPE_WEEKLY, 5, 10);     // Product 1. Weekly. zoneLow: 5 , zoneHigh 10
    if ( pProduct )
    {
        pProduct->ControlBitmap |=  TAPP_TPRODUCT_CONTROL_PROVISIONAL_BITMAP;
        pProduct->PurchaseValue = 100;
        pProduct->StartDateTime = 1000001;
    }

    /*
    **  InActivated ePass
    */
    setProduct(pMYKI_TAControl, 2, PRODUCT_TYPE_EPASS, 4, 11);
    if ( pProduct )
    {
        pProduct->StartDateTime = 1000000;
    }

    /*
    **  Setup other products - no coverage
    */
    setProduct(pMYKI_TAControl, 3, PRODUCT_TYPE_DAILY, 1, 2);
    setProduct(pMYKI_TAControl, 4, PRODUCT_TYPE_DAILY, 1, 2);
    setProduct(pMYKI_TAControl, 5, PRODUCT_TYPE_DAILY, 1, 2);

    /*
    **  Run the Test and examine the results
    */
    return runTest(pData, pMYKI_TAControl, RULE_RESULT_EXECUTED, 8, 0 );
}

/*==========================================================================*
**
**  test_BR_LLSC_7_4_010
**
**  Description     :
**      Expired Tariff Full Product Coverage
**      Unit-test Execute conditions.
**              Covered by two active products
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

int test_BR_LLSC_7_4_010(MYKI_BR_ContextData_t *pData)
{
    MYKI_TAControl_t       *pMYKI_TAControl         = NULL;

    if ( MYKI_CS_TAControlGet( &pMYKI_TAControl ) < 0 || pMYKI_TAControl == NULL )
    {
        return FALSE;
    }

    /*
    **  Test with product in use
    **  Provisional Capping Option NOT set to Option3
    **  Invalid Tariff
    **  ProductInUse is a provisional product
    **  Non-Zero product purchase value
    **  Covered by two active products
    */
    CardImage1(pData);

    pData->Tariff.provisionalCappingOption = 2;
    pMYKI_TAControl->ProductInUse = 1;
    MYKI_CD_setStartDateTime(pProduct->LastUsage.DateTime + 100);

    /*
    **  Setup the provisional product
    */
    setProduct(pMYKI_TAControl, 1, PRODUCT_TYPE_WEEKLY, 5, 10);     // Product 1. Weekly. zoneLow: 5 , zoneHigh 10
    if ( pProduct )
    {
        pProduct->ControlBitmap |=  TAPP_TPRODUCT_CONTROL_PROVISIONAL_BITMAP;
        pProduct->PurchaseValue = 100;
        pProduct->StartDateTime = 1000001;
    }

    /*
    **  Two Active Product
    */
    setProduct(pMYKI_TAControl, 2, PRODUCT_TYPE_DAILY, 4, 6);
    pMYKI_TAControl->Directory[2].Status = TAPP_CONTROL_DIRECTORY_STATUS_ACTIVATED;

    setProduct(pMYKI_TAControl, 3, PRODUCT_TYPE_DAILY, 6, 11);
    pMYKI_TAControl->Directory[3].Status = TAPP_CONTROL_DIRECTORY_STATUS_ACTIVATED;


    /*
    **  Setup other products - no coverage
    */
    setProduct(pMYKI_TAControl, 4, PRODUCT_TYPE_DAILY, 1, 2);
    setProduct(pMYKI_TAControl, 5, PRODUCT_TYPE_DAILY, 1, 2);

    /*
    **  Run the Test and examine the results
    */
    return runTest(pData, pMYKI_TAControl, RULE_RESULT_EXECUTED, 7, 0 );
}

/*==========================================================================*
**
**  test_BR_LLSC_7_4_011
**
**  Description     :
**      Expired Tariff Full Product Coverage
**      Unit-test Execute conditions.
**              Covered by one active products
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

int test_BR_LLSC_7_4_011(MYKI_BR_ContextData_t *pData)
{
    MYKI_TAControl_t       *pMYKI_TAControl         = NULL;

    if ( MYKI_CS_TAControlGet( &pMYKI_TAControl ) < 0 || pMYKI_TAControl == NULL )
    {
        return FALSE;
    }

    /*
    **  Test with product in use
    **  Provisional Capping Option NOT set to Option3
    **  Invalid Tariff
    **  ProductInUse is a provisional product
    **  Non-Zero product purchase value
    **  Covered by one active products
    */
    CardImage1(pData);

    pData->Tariff.provisionalCappingOption = 2;
    pMYKI_TAControl->ProductInUse = 1;
    MYKI_CD_setStartDateTime(pProduct->LastUsage.DateTime + 100);

    /*
    **  Setup the provisional product
    */
    setProduct(pMYKI_TAControl, 1, PRODUCT_TYPE_WEEKLY, 5, 10);     // Product 1. Weekly. zoneLow: 5 , zoneHigh 10
    if ( pProduct )
    {
        pProduct->ControlBitmap |=  TAPP_TPRODUCT_CONTROL_PROVISIONAL_BITMAP;
        pProduct->PurchaseValue = 100;
        pProduct->StartDateTime = 1000001;
    }

    /*
    **  Active Product
    */
    setProduct(pMYKI_TAControl, 2, PRODUCT_TYPE_DAILY, 5, 10);
    pMYKI_TAControl->Directory[2].Status = TAPP_CONTROL_DIRECTORY_STATUS_ACTIVATED;

    /*
    **  Setup other products - no coverage
    */
    setProduct(pMYKI_TAControl, 3, PRODUCT_TYPE_DAILY, 1, 2);
    setProduct(pMYKI_TAControl, 4, PRODUCT_TYPE_DAILY, 1, 2);
    setProduct(pMYKI_TAControl, 5, PRODUCT_TYPE_DAILY, 1, 2);

    /*
    **  Setup the current trip zones
    */
    pData->DynamicData.currentTripZoneLow  = 6;
    pData->DynamicData.currentTripZoneHigh = 9;

    /*
    **  Run the Test and examine the results
    */
    return runTest(pData, pMYKI_TAControl, RULE_RESULT_EXECUTED, 7, 0 );
}

/*==========================================================================*
**
**  test_BR_LLSC_7_4_012
**
**  Description     :
**      Expired Tariff Full Product Coverage
**      Unit-test Execute conditions.
**              Covered by one active products and one inactive Epass
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

int test_BR_LLSC_7_4_012(MYKI_BR_ContextData_t *pData)
{
    MYKI_TAControl_t       *pMYKI_TAControl         = NULL;

    if ( MYKI_CS_TAControlGet( &pMYKI_TAControl ) < 0 || pMYKI_TAControl == NULL )
    {
        return FALSE;
    }

    /*
    **  Test with product in use
    **  Provisional Capping Option NOT set to Option3
    **  Invalid Tariff
    **  ProductInUse is a provisional product
    **  Non-Zero product purchase value
    **  Covered by one active products and one inactive Epass
    */
    CardImage1(pData);

    pData->Tariff.provisionalCappingOption = 2;
    pMYKI_TAControl->ProductInUse = 1;
    MYKI_CD_setStartDateTime(pProduct->LastUsage.DateTime + 100);

    /*
    **  Setup the provisional product
    */
    setProduct(pMYKI_TAControl, 1, PRODUCT_TYPE_WEEKLY, 5, 10);     // Product 1. Weekly. zoneLow: 5 , zoneHigh 10
    if ( pProduct )
    {
        pProduct->ControlBitmap |=  TAPP_TPRODUCT_CONTROL_PROVISIONAL_BITMAP;
        pProduct->PurchaseValue = 100;
        pProduct->StartDateTime = 1000001;
    }


    /*
    **  InActivated ePass
    */
    setProduct(pMYKI_TAControl, 2, PRODUCT_TYPE_EPASS, 4, 6);
    if ( pProduct )
    {
        pProduct->StartDateTime = 1000000;
    }
    
    /*
    **  Active Product
    */
    setProduct(pMYKI_TAControl, 3, PRODUCT_TYPE_DAILY, 7, 10);
    pMYKI_TAControl->Directory[3].Status = TAPP_CONTROL_DIRECTORY_STATUS_ACTIVATED;

    /*
    **  Setup other products - no coverage
    */
    setProduct(pMYKI_TAControl, 4, PRODUCT_TYPE_DAILY, 1, 2);
    setProduct(pMYKI_TAControl, 5, PRODUCT_TYPE_DAILY, 1, 2);

    /*
    **  Run the Test and examine the results
    */
    return runTest(pData, pMYKI_TAControl, RULE_RESULT_EXECUTED, 8, 0 );
}

/*==========================================================================*
**
**  test_BR_LLSC_7_4_013
**
**  Description     :
**      Expired Tariff Full Product Coverage
**      Unit-test Execute conditions.
**              Covered by one active products and one inactive Epass with active ePass
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

int test_BR_LLSC_7_4_013(MYKI_BR_ContextData_t *pData)
{
    MYKI_TAControl_t       *pMYKI_TAControl         = NULL;

    if ( MYKI_CS_TAControlGet( &pMYKI_TAControl ) < 0 || pMYKI_TAControl == NULL )
    {
        return FALSE;
    }

    /*
    **  Test with product in use
    **  Provisional Capping Option NOT set to Option3
    **  Invalid Tariff
    **  ProductInUse is a provisional product
    **  Non-Zero product purchase value
    **  Covered by one active products and one inactive Epass with active ePass
    */
    CardImage1(pData);

    pData->Tariff.provisionalCappingOption = 2;
    pMYKI_TAControl->ProductInUse = 1;
    MYKI_CD_setStartDateTime(pProduct->LastUsage.DateTime + 100);

    /*
    **  Setup the provisional product
    */
    setProduct(pMYKI_TAControl, 1, PRODUCT_TYPE_WEEKLY, 5, 10);     // Product 1. Weekly. zoneLow: 5 , zoneHigh 10
    if ( pProduct )
    {
        pProduct->ControlBitmap |=  TAPP_TPRODUCT_CONTROL_PROVISIONAL_BITMAP;
        pProduct->PurchaseValue = 100;
        pProduct->StartDateTime = 1000001;
    }


    /*
    **  InActivated ePass
    */
    setProduct(pMYKI_TAControl, 2, PRODUCT_TYPE_EPASS, 4, 6);
    if ( pProduct )
    {
        pProduct->StartDateTime = 1000000;
    }
    
    /*
    **  Active Product
    */
    setProduct(pMYKI_TAControl, 3, PRODUCT_TYPE_DAILY, 7, 10);
    pMYKI_TAControl->Directory[3].Status = TAPP_CONTROL_DIRECTORY_STATUS_ACTIVATED;

    /*
    **  Active ePass
    */
    setProduct(pMYKI_TAControl, 4, PRODUCT_TYPE_EPASS, 1, 2);
    pMYKI_TAControl->Directory[4].Status = TAPP_CONTROL_DIRECTORY_STATUS_ACTIVATED;

    /*
    **  Setup other products - no coverage
    */
    setProduct(pMYKI_TAControl, 5, PRODUCT_TYPE_DAILY, 1, 2);

    /*
    **  Run the Test and examine the results
    */
    return runTest(pData, pMYKI_TAControl, RULE_RESULT_BYPASSED, 6, 1 );
}

