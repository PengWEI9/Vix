/* -*- mode: c; tabs: 4; -*- */
/*=============================================================================
**
**    Vix Technology                   Licensed software
**    (C) 2013                         All rights reserved
**
**=============================================================================
**
**  Project/Product : MBU
**  Filename        : test_BR_LLSC_1_3.c
**  Author(s)       : An Tran
**
**  Description     :
**      Implements BR_LLSC_1_3 business rule unit-tests.
**
**  Function(s)     :
**      Template                [Public]    template function
**
**  Information     :
**   Compiler(s)    : ANSI C
**   Target(s)      : Independent
**
**  History         :
**   Vers.  Date        Aut.  Type     Description
**   -----  ----------  ----  -------  ----------------------------------------
**    1.00  16.10.13    ANT   Create
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

//============================================================================
//
//
//
//============================================================================

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
**  test_BR_LLSC_1_3_001
**
**  Description     :
**      Unit-test BYPASSED conditions.
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

int test_BR_LLSC_1_3_001( MYKI_BR_ContextData_t *pData )
{
    CardImage_t     CardImage;

    if ( pData == NULL )
    {
        CsErrx( "test_BR_LLSC_1_3_001() Invalid argument (pData == NULL)" );
        return FALSE;
    }

    //
    //  Test 1 - No product in use
    //  Result - Bypass (pre-condition 1 not met)
    //

    if ( CreateCardImage_Empty( &CardImage ) < 0 )
    {
        return FALSE;
    }

    if ( BR_LLSC_1_3( pData ) != RULE_RESULT_BYPASSED )
    {
        return FALSE;
    }

    //
    //  Test 2 - Product in use is not daily or weekly
    //  Result - Bypass (pre-condition 2 not met)
    //

    if ( CreateCardImage_Empty( &CardImage ) < 0 )
    {
        return FALSE;
    }

    CreateProduct( &CardImage, 2, PRODUCT_ID_NHOUR, 1, 1, pData->DynamicData.currentDateTime );

    CardImage.pMYKI_TAControl->ProductInUse = 2;

    if ( BR_LLSC_1_3( pData ) != RULE_RESULT_BYPASSED )
    {
        return FALSE;
    }

    //
    //  Test 3 - No other active unexpired product matching the product in use forms a contiguous zone range
    //  Result - Bypass (pre-condition 3 not met)
    //

    if ( CreateCardImage_Empty( &CardImage ) < 0 )
    {
        return FALSE;
    }

    CreateProduct( &CardImage, 1, PRODUCT_ID_NHOUR, 1, 1, pData->DynamicData.currentDateTime        );  // Type does not match
    CreateProduct( &CardImage, 2, PRODUCT_ID_DAILY, 1, 1, pData->DynamicData.currentDateTime        );  // Product in use
    CreateProduct( &CardImage, 3, PRODUCT_ID_DAILY, 1, 1, pData->DynamicData.currentDateTime - 1    );  // Expired
    CreateProduct( &CardImage, 4, PRODUCT_ID_DAILY, 3, 3, pData->DynamicData.currentDateTime        );  // Not contiguous

    CardImage.pMYKI_TAControl->ProductInUse = 2;

    if ( BR_LLSC_1_3( pData ) != RULE_RESULT_BYPASSED )
    {
        return FALSE;
    }

    //
    //  All tests passed
    //

    return TRUE;
}

/*==========================================================================*
**
**  test_BR_LLSC_1_3_002
**
**  Description     :
**      Unit-test EXECUTED conditions, merge DAILY.
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

int test_BR_LLSC_1_3_002( MYKI_BR_ContextData_t *pData )
{
    CardImage_t     CardImage;

    if ( pData == NULL )
    {
        CsErrx( "test_BR_LLSC_1_3_002() Invalid argument (pData == NULL)" );
        return FALSE;
    }

    //
    //  Test 1 - Single mergeable daily product, entirely within product-in-use zone range
    //  Result - Executed, mergeable product invalidated, product-in-use unchanged.
    //

    if ( CreateCardImage_Empty( &CardImage ) < 0 )
    {
        CsErrx( "test_BR_LLSC_1_3_002() Failed to create card image" );
        return FALSE;
    }

    CreateProduct( &CardImage, 2, PRODUCT_ID_DAILY, 1, 3, pData->DynamicData.currentDateTime );  // Product in use
    CreateProduct( &CardImage, 3, PRODUCT_ID_DAILY, 2, 2, pData->DynamicData.currentDateTime );  // Mergeable product

    CardImage.pMYKI_TAControl->ProductInUse = 2;

    if ( BR_LLSC_1_3( pData ) != RULE_RESULT_EXECUTED )
    {
        CsErrx( "test_BR_LLSC_1_3_002() Rule did not returned EXECUTED" );
        return FALSE;   // Rule returned wrong value
    }

    if ( CardImage.pMYKI_TAControl->Directory[ 3 ].Status != TAPP_CONTROL_DIRECTORY_STATUS_UNUSED )
    {
        CsErrx( "test_BR_LLSC_1_3_002() Mergeable product was not invalidated" );
        return FALSE;   // Mergeable product was not invalidated
    }

    if ( CardImage.pMYKI_TAControl->Directory[ 2 ].Status != TAPP_CONTROL_DIRECTORY_STATUS_ACTIVATED )
    {
        CsErrx( "test_BR_LLSC_1_3_002() Product in use was invalidated" );
        return FALSE;   // Product-in-use was invalidated
    }

    if ( CardImage.pMYKI_TAProduct[ 2 - 1 ]->ZoneLow != 1 )
    {
        CsErrx( "test_BR_LLSC_1_3_002() Product in use low zone changed" );
        return FALSE;   // Product-in-use low zone changed
    }

    if ( CardImage.pMYKI_TAProduct[ 2 - 1 ]->ZoneHigh != 3 )
    {
        CsErrx( "test_BR_LLSC_1_3_002() Product in use high zone changed" );
        return FALSE;   // Product-in-use high zone changed
    }

    //
    //  All tests passed
    //

    return TRUE;
}

/*==========================================================================*
**
**  test_BR_LLSC_1_3_003
**
**  Description     :
**      Unit-test EXECUTED conditions, merge WEEKLY.
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

int test_BR_LLSC_1_3_003( MYKI_BR_ContextData_t *pData )
{
    CardImage_t     CardImage;

    if ( pData == NULL )
    {
        CsErrx( "test_BR_LLSC_1_3_003() Invalid argument (pData == NULL)" );
        return FALSE;
    }

    //
    //  Test 1 - Single mergeable weekly product, entirely within product-in-use zone range
    //  Result - Executed, mergeable product invalidated, product-in-use unchanged.
    //

    if ( CreateCardImage_Empty( &CardImage ) < 0 )
    {
        CsErrx( "test_BR_LLSC_1_3_003() Failed to create card image" );
        return FALSE;
    }

    CreateProduct( &CardImage, 2, PRODUCT_ID_WEEKLY, 1, 3, pData->DynamicData.currentDateTime );  // Product in use
    CreateProduct( &CardImage, 3, PRODUCT_ID_WEEKLY, 2, 2, pData->DynamicData.currentDateTime );  // Mergeable product

    CardImage.pMYKI_TAControl->ProductInUse = 2;

    if ( BR_LLSC_1_3( pData ) != RULE_RESULT_EXECUTED )
    {
        CsErrx( "test_BR_LLSC_1_3_003() Rule did not returned EXECUTED" );
        return FALSE;   // Rule returned wrong value
    }

    if ( CardImage.pMYKI_TAControl->Directory[ 3 ].Status != TAPP_CONTROL_DIRECTORY_STATUS_UNUSED )
    {
        CsErrx( "test_BR_LLSC_1_3_003() Mergeable product was not invalidated" );
        return FALSE;   // Mergeable product was not invalidated
    }

    if ( CardImage.pMYKI_TAControl->Directory[ 2 ].Status != TAPP_CONTROL_DIRECTORY_STATUS_ACTIVATED )
    {
        CsErrx( "test_BR_LLSC_1_3_003() Product in use was invalidated" );
        return FALSE;   // Product-in-use was invalidated
    }

    if ( CardImage.pMYKI_TAProduct[ 2 - 1 ]->ZoneLow != 1 )
    {
        CsErrx( "test_BR_LLSC_1_3_003() Product in use low zone changed" );
        return FALSE;   // Product-in-use low zone changed
    }

    if ( CardImage.pMYKI_TAProduct[ 2 - 1 ]->ZoneHigh != 3 )
    {
        CsErrx( "test_BR_LLSC_1_3_003() Product in use high zone changed" );
        return FALSE;   // Product-in-use high zone changed
    }

    //
    //  All tests passed
    //

    return TRUE;
}

/*==========================================================================*
**
**  test_BR_LLSC_1_3_004
**
**  Description     :
**      Unit-test EXECUTED conditions, merge DAILY.
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

int test_BR_LLSC_1_3_004( MYKI_BR_ContextData_t *pData )
{
    CardImage_t     CardImage;

    if ( pData == NULL )
    {
        CsErrx( "test_BR_LLSC_1_3_004() Invalid argument (pData == NULL)" );
        return FALSE;
    }

    //
    //  Test 1 - Multiple mergeable daily products, some contiguous, some not
    //  Result - Executed, mergeable products invalidated, product-in-use updated
    //

    if ( CreateCardImage_Empty( &CardImage ) < 0 )
    {
        CsErrx( "test_BR_LLSC_1_3_004() Failed to create card image" );
        return FALSE;
    }

    CreateProduct( &CardImage, 1, PRODUCT_ID_DAILY, 4, 4, pData->DynamicData.currentDateTime );  // Mergeable product (contiguous with 3)
    CreateProduct( &CardImage, 2, PRODUCT_ID_DAILY, 2, 2, pData->DynamicData.currentDateTime );  // Product in use
    CreateProduct( &CardImage, 3, PRODUCT_ID_DAILY, 1, 3, pData->DynamicData.currentDateTime );  // Mergeable product (contiguous with 2)
    CreateProduct( &CardImage, 4, PRODUCT_ID_DAILY, 6, 6, pData->DynamicData.currentDateTime );  // Mergeable product (not contiguous)
    CreateProduct( &CardImage, 5, PRODUCT_ID_DAILY, 3, 4, pData->DynamicData.currentDateTime );  // Mergeable product (contiguous with 2, 3 and 4)

    CardImage.pMYKI_TAControl->ProductInUse = 2;

    if ( BR_LLSC_1_3( pData ) != RULE_RESULT_EXECUTED )
    {
        CsErrx( "test_BR_LLSC_1_3_004() Rule did not returned EXECUTED" );
        return FALSE;   // Rule returned wrong value
    }

    if ( CardImage.pMYKI_TAControl->Directory[ 1 ].Status != TAPP_CONTROL_DIRECTORY_STATUS_UNUSED )
    {
        CsErrx( "test_BR_LLSC_1_3_004() Mergeable product was not invalidated" );
        return FALSE;   // Mergeable product was not invalidated
    }

    if ( CardImage.pMYKI_TAControl->Directory[ 2 ].Status != TAPP_CONTROL_DIRECTORY_STATUS_ACTIVATED )
    {
        CsErrx( "test_BR_LLSC_1_3_004() Product in use was invalidated" );
        return FALSE;   // Product-in-use was invalidated
    }

    if ( CardImage.pMYKI_TAControl->Directory[ 3 ].Status != TAPP_CONTROL_DIRECTORY_STATUS_UNUSED )
    {
        CsErrx( "test_BR_LLSC_1_3_004() Mergeable product was not invalidated" );
        return FALSE;   // Mergeable product was not invalidated
    }

    if ( CardImage.pMYKI_TAControl->Directory[ 4 ].Status != TAPP_CONTROL_DIRECTORY_STATUS_ACTIVATED )
    {
        CsErrx( "test_BR_LLSC_1_3_004() Non-mergeable product was invalidated" );
        return FALSE;   // Non-mergeable product was invalidated
    }

    if ( CardImage.pMYKI_TAControl->Directory[ 5 ].Status != TAPP_CONTROL_DIRECTORY_STATUS_UNUSED )
    {
        CsErrx( "test_BR_LLSC_1_3_004() Mergeable product was not invalidated" );
        return FALSE;   // Mergeable product was not invalidated
    }

    if ( CardImage.pMYKI_TAProduct[ 2 - 1 ]->ZoneLow != 1 )
    {
        CsErrx( "test_BR_LLSC_1_3_004() Product in use low zone not correct" );
        return FALSE;   // Product-in-use low zone not updated correctly
    }

    if ( CardImage.pMYKI_TAProduct[ 2 - 1 ]->ZoneHigh != 4 )
    {
        CsErrx( "test_BR_LLSC_1_3_004() Product in use high zone not correct" );
        return FALSE;   // Product-in-use high zone not updated correctly
    }

    if ( CardImage.pMYKI_TAProduct[ 2 - 1 ]->PurchaseValue != 1108 )        // Zone 1 - 4 Daily fare
    {
        CsErrx( "test_BR_LLSC_1_3_004() Product in use fare not correct" );
        return FALSE;   // Product-in-use high zone changed
    }

    //
    //  All tests passed
    //

    return TRUE;
}

/*==========================================================================*
**
**  test_BR_LLSC_1_3_005
**
**  Description     :
**      Unit-test EXECUTED conditions, merge DAILY.
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

int test_BR_LLSC_1_3_005( MYKI_BR_ContextData_t *pData )
{
    CardImage_t     CardImage;

    if ( pData == NULL )
    {
        CsErrx( "test_BR_LLSC_1_3_005() Invalid argument (pData == NULL)" );
        return FALSE;
    }

    //
    //  Test 1 - Multiple mergeable daily products, some contiguous, some not
    //  Result - Executed, mergeable products invalidated, product-in-use updated
    //

    if ( CreateCardImage_Empty( &CardImage ) < 0 )
    {
        CsErrx( "test_BR_LLSC_1_3_005() Failed to create card image" );
        return FALSE;
    }

    CreateProduct( &CardImage, 1, PRODUCT_ID_WEEKLY, 4, 4, pData->DynamicData.currentDateTime );  // Mergeable product (contiguous with 3)
    CreateProduct( &CardImage, 2, PRODUCT_ID_WEEKLY, 2, 2, pData->DynamicData.currentDateTime );  // Product in use
    CreateProduct( &CardImage, 3, PRODUCT_ID_WEEKLY, 1, 3, pData->DynamicData.currentDateTime );  // Mergeable product (contiguous with 2)
    CreateProduct( &CardImage, 4, PRODUCT_ID_WEEKLY, 6, 6, pData->DynamicData.currentDateTime );  // Mergeable product (not contiguous)
    CreateProduct( &CardImage, 5, PRODUCT_ID_WEEKLY, 3, 4, pData->DynamicData.currentDateTime );  // Mergeable product (contiguous with 2, 3 and 4)

    CardImage.pMYKI_TAControl->ProductInUse = 2;

    if ( BR_LLSC_1_3( pData ) != RULE_RESULT_EXECUTED )
    {
        CsErrx( "test_BR_LLSC_1_3_005() Rule did not returned EXECUTED" );
        return FALSE;   // Rule returned wrong value
    }

    if ( CardImage.pMYKI_TAControl->Directory[ 1 ].Status != TAPP_CONTROL_DIRECTORY_STATUS_UNUSED )
    {
        CsErrx( "test_BR_LLSC_1_3_005() Mergeable product was not invalidated" );
        return FALSE;   // Mergeable product was not invalidated
    }

    if ( CardImage.pMYKI_TAControl->Directory[ 2 ].Status != TAPP_CONTROL_DIRECTORY_STATUS_ACTIVATED )
    {
        CsErrx( "test_BR_LLSC_1_3_005() Product in use was invalidated" );
        return FALSE;   // Product-in-use was invalidated
    }

    if ( CardImage.pMYKI_TAControl->Directory[ 3 ].Status != TAPP_CONTROL_DIRECTORY_STATUS_UNUSED )
    {
        CsErrx( "test_BR_LLSC_1_3_005() Mergeable product was not invalidated" );
        return FALSE;   // Mergeable product was not invalidated
    }

    if ( CardImage.pMYKI_TAControl->Directory[ 4 ].Status != TAPP_CONTROL_DIRECTORY_STATUS_ACTIVATED )
    {
        CsErrx( "test_BR_LLSC_1_3_005() Non-mergeable product was invalidated" );
        return FALSE;   // Non-mergeable product was invalidated
    }

    if ( CardImage.pMYKI_TAControl->Directory[ 5 ].Status != TAPP_CONTROL_DIRECTORY_STATUS_UNUSED )
    {
        CsErrx( "test_BR_LLSC_1_3_005() Mergeable product was not invalidated" );
        return FALSE;   // Mergeable product was not invalidated
    }

    if ( CardImage.pMYKI_TAProduct[ 2 - 1 ]->ZoneLow != 1 )
    {
        CsErrx( "test_BR_LLSC_1_3_005() Product in use low zone not correct" );
        return FALSE;   // Product-in-use low zone not updated correctly
    }

    if ( CardImage.pMYKI_TAProduct[ 2 - 1 ]->ZoneHigh != 4 )
    {
        CsErrx( "test_BR_LLSC_1_3_005() Product in use high zone not correct" );
        return FALSE;   // Product-in-use high zone not updated correctly
    }

    if ( CardImage.pMYKI_TAProduct[ 2 - 1 ]->PurchaseValue != 5540 )        // Zone 1 - 4 Weekly fare
    {
        CsErrx( "test_BR_LLSC_1_3_005() Product in use fare not correct" );
        return FALSE;   // Product-in-use high zone changed
    }

    //
    //  All tests passed
    //

    return TRUE;
}


