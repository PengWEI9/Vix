/* -*- mode: c; tabs: 4; -*- */
/*=============================================================================
**
**    Vix Technology                   Licensed software
**    (C) 2013                         All rights reserved
**
**=============================================================================
**
**  Project/Product : MBU
**  Filename        : BR_LLSC_1_3.c
**  Author(s)       : An Tran
**
**  ID              : BR_LLSC_1_3 - KA0004 v7.0
**
**  Name            : Merge Adjacent Stored Value Products
**
**  Data Fields     :
**
**      1.  TAppControl.ProductInUse
**      2.  TAppControl.Directory.ProductID
**      3.  TAppControl.Directory.Status
**      4.  TAppTProduct.EndDateTime
**      5.  Dynamic.CurrentDateTime
**      6.  TAppTProduct.ControlBitmap
**
**  Pre-Conditions  :
**
**      1.  The product in use field is not 0.
**      2.  The Product ID of the product in use is daily or weekly
**      3.  One or more than one stored value products exist on the smartcard:
**          a.  With the same Product ID as the product in use.
**          b.  That allows travel in zone(s) adjacent or overlapping with each
**              other and/or the product in use. (low zone of the product is
**              less than or equal to high zone of product in use plus 1 AND
**              greater than or equal to low zone of the product in use) OR
**              (if high zone of the product is greater than or equal to low
**              zone of product in use minus 1 AND less than or equal to high
**              zone of the product in use).
**          c.  With a product expiry greater than or equal to the current
**              date/time.
**
**  Description     :
**
**      1.  For each applicable product that is:
**          a.  not the product of use; and
**          b.  of the same product type as the product in use.
**          c.  That allows travel in zone(s) adjacent or overlapping with
**              each other and/or the product in use.
**              i.  if low zone of the product is less than or equal to high
**                  zone of product in use plus 1 AND greater than or equal
**                  to low zone of the product in use OR if high zone of the
**                  product is greater than or equal to low zone of product in
**                  use minus 1 AND less than or equal to high zone of the
**                  product in use.
**              ii. With a product expiry greater than or equal to the
**                  current date/time.
**          d.  Perform a ProductUpdate/Extend transaction for the product in use
**              i.  Extend the zone range to include the zone range of the product.
**                  (1) If zone low of the product is less than the product in
**                      use low zone; then set the product in use low zone
**                      to the low of zone of the product.
**                  (2) If zone high of the product is greater than the product
**                      in use high zone; then set the product in use high zone
**                      to the high zone of the product.
**              ii. Determine the fare for the:
**                  (1) product in use zone range
**                  (2) The current date time
**                  (3) A route ID of none
**                  (4) The type of passenger
**              iii.Set the purchase value for the product in use to the fare
**          e.  Perform a Product Update/Invalidate transaction for the product
**
**  Post-Conditions :
**
**      1.  One or more Daily or Weekly stored value products valid for
**          adjacent zones or Over lapping zones are merged into a single
**          Daily or Weekly for the combined zone range of the product.
**      2.  One or more stored value products deprecated by the merge are
**          invalidated.
**
**  Member(s)       :
**      BR_LLSC_1_5             [public]    business rule
**
**  Information     :
**   Compiler(s)    : C
**   Target(s)      : Independent
**
**  History         :
**   Vers.  Date        Aut.  Type     Description
**   -----  ----------  ----  -------  ----------------------------------------
**    1.00  14.10.13    ANT   Create
**
**===========================================================================*/

/*
 *      Includes
 *      --------
 */

#include <cs.h>                         /* For debug logging */
#include <myki_cardservices.h>          /* Myki Card Services */
#include <LDT.h>                        /* Logical Device Transaction */

#include "myki_br_rules.h"              /* API for all BR_xxx modules */
#include "BR_Common.h"                  /* Common utilities, macros, etc */

/*==========================================================================*
**
**  BR_LLSC_1_3
**
**  Description     :
**      Implements business rule BR_LLSC_1_3.
**
**  Parameters      :
**      pData               [I/O]   BR context data
**
**  Returns         :
**      RULE_RESULT_EXECUTED
**      RULE_RESULT_BYPASSED
**      RULE_RESULT_ERROR
**
**  Notes           :
**
**  This rule merges (into the product in use) all active, unexpired products
**  of the same type as the product in use which, together with the product
**  in use, form a contiguous block of zones. It only applies if the product
**  in use is a DAILY or WEEKLY product.
**
**  This rule will return EXECUTED if one or more products were invalidated,
**  even if the product in use zone range was NOT extended (ie, all invalidated
**  products were completely covered by the product in use). This is as per the
**  BR document, but may not be correct. Need to verify.
**
**  This rule will only perform a single product update/extend, even if more
**  than one product is invalidated. This does not strictly follow the BR document,
**  but is more likely to be correct. Need to verify.
**
**==========================================================================*/

RuleResult_e BR_LLSC_1_3( MYKI_BR_ContextData_t *pData )
{
    MYKI_TAControl_t       *pMYKI_TAControl                 = NULL;
    MYKI_Directory_t       *pMYKI_Directory                 = NULL;
    MYKI_TAProduct_t       *pMYKI_TAProduct                 = NULL;
    MYKI_Directory_t       *pMYKI_ProductInUse_Directory    = NULL;
    MYKI_TAProduct_t       *pMYKI_ProductInUse_TAProduct    = NULL;
    MYKI_TAProduct_t       MYKI_ProductInUse_TAProduct_New;
    int                     i                               = 0;
    int                     zone;
    ProductType_e           ProductInUse_ProductType        = PRODUCT_TYPE_UNKNOWN;
    int                     ZoneLow                         = 0;
    int                     ZoneHigh                        = 0;
    int                     PurchaseValue                   = 0;
    int                     nMergedProducts                 = 0;
    int                     ZoneMap[ ZONE_MAP_MAX ];
    int                     MergeableProductList[ 5 ];      // List of directory indexes of mergeable products
    int                     MergeableProductCount = 0;      // Number of entries in above list

    CsDbg( BRLL_RULE, "BR_LLSC_1_3 : Start (Merge Adjacent Stored Value Products)" );

    if ( pData == NULL )
    {
        CsErrx( "BR_LLSC_1_3 : Invalid argument(s)" );
        return RULE_RESULT_ERROR;
    }

    if ( MYKI_CS_TAControlGet( &pMYKI_TAControl ) < 0 )
    {
        CsErrx( "BR_LLSC_1_3 : MYKI_CS_TAControlGet() failed" );
        return RULE_RESULT_ERROR;
    }

    //
    //  Pre-conditions
    //

    /*  1.  The product in use field1 is not 0. */
    if ( pMYKI_TAControl->ProductInUse == 0 )
    {
        CsDbg( BRLL_RULE, "BR_LLSC_1_3 : BYPASSED - no product in use" );
        return RULE_RESULT_BYPASSED;
    }   /* end-of-if */

    /*  2.  The Product ID of the product in use1 is daily or weekly  */
    if ( myki_br_GetCardProduct( pMYKI_TAControl->ProductInUse, &pMYKI_ProductInUse_Directory, &pMYKI_ProductInUse_TAProduct ) < 0 )
    {
        CsErrx( "BR_LLSC_1_3 : myki_br_GetCardProduct( %d ) failed", pMYKI_TAControl->ProductInUse );
        return RULE_RESULT_ERROR;
    }

    ProductInUse_ProductType    = myki_br_cd_GetProductType( pMYKI_ProductInUse_Directory->ProductId );

    if ( ProductInUse_ProductType != PRODUCT_TYPE_DAILY && ProductInUse_ProductType != PRODUCT_TYPE_WEEKLY )
    {
        CsDbg( BRLL_RULE, "BR_LLSC_1_3 : BYPASSED - product in use is neither DAILY nor WEEKLY" );
        return RULE_RESULT_BYPASSED;
    }

    //  NOTE:   The following pre-condition is redundant - it's implicitly checked
    //          as part of the main processing, so all we do is return BYPASSED at the
    //          end of processing if no products were processed.
    //  3.  One or more than one stored value products exist on the smartcard:
    //      a.  With the same Product ID as the product in use.
    //      b.  That allows travel in zone(s) adjacent or overlapping with each other
    //          and/or the product in use
    //      c.  With a product expiry greater than or equal to the current date/time.

    //
    //  Processing (does not follow steps in BR document as they are not able to be coded efficiently)
    //

    //  First, create a map of the zones covered by all active unexpired products with the
    //  same *ID* (not type!) as the product in use, but make sure the map gives priority to products which
    //  are NOT the product in use. This is to ensure we invalidate any product we find which is
    //  entirely covered by the product in use.

    memset( ZoneMap, 0, sizeof( ZoneMap ) );
    int extendProductInUseFlag = FALSE;

    for ( i = 1; i < DIMOF( pMYKI_TAControl->Directory ); i++ )
    {
        if ( pMYKI_TAControl->Directory[ i ].Status != TAPP_CONTROL_DIRECTORY_STATUS_ACTIVATED )
        {
            CsDbg( BRLL_RULE, "BR_LLSC_1_3 : Product at directory index %d is not active, ignoring", i );
            continue;
        }

        //  Check ID, not type! We cannot merge two DAILY products if they are not the same product ID
        if ( pMYKI_TAControl->Directory[ i ].ProductId != pMYKI_ProductInUse_Directory->ProductId )
//        if ( myki_br_cd_GetProductType( pMYKI_TAControl->Directory[ i ].ProductId ) != ProductInUse_ProductType )
        {
            CsDbg( BRLL_RULE, "BR_LLSC_1_3 : Product at directory index %d is different ID, ignoring", i );
            continue;
        }

        if ( myki_br_GetCardProduct( i, &pMYKI_Directory, &pMYKI_TAProduct ) < 0 )
        {
            CsErrx( "BR_LLSC_1_3 : myki_br_GetCardProduct( %d ) failed", i );
            return RULE_RESULT_ERROR;
        }

        if ( pMYKI_TAProduct->EndDateTime < pData->DynamicData.currentDateTime )
        {
            CsDbg( BRLL_RULE, "BR_LLSC_1_3 : Product at directory index %d is expired, ignoring", i );
            continue;
        }

        CsDbg( BRLL_RULE, "BR_LLSC_1_3 : Mergeable product at directory index %d, zone %d - %d", i, pMYKI_TAProduct->ZoneLow, pMYKI_TAProduct->ZoneHigh );

        MergeableProductList[ MergeableProductCount++ ] = i;

        //      c.  That allows travel in zone(s) adjacent or overlapping with
        //          each other and/or the product in use.
        //          i.  if low zone of the product is less than or equal to high
        //              zone of product in use plus 1 AND greater than or equal
        //              to low zone of the product in use OR if high zone of the
        //              product is greater than or equal to low zone of product in
        //              use minus 1 AND less than or equal to high zone of the
        //              product in use.

        if ( i != pMYKI_TAControl->ProductInUse )
        {
            if ( ( pMYKI_TAProduct->ZoneLow <= (pMYKI_ProductInUse_TAProduct->ZoneHigh + 1 ) &&
                     pMYKI_TAProduct->ZoneLow >= pMYKI_ProductInUse_TAProduct->ZoneLow ) ||
                 ( pMYKI_TAProduct->ZoneHigh >= (pMYKI_ProductInUse_TAProduct->ZoneLow - 1 ) &&
                     pMYKI_TAProduct->ZoneHigh <= pMYKI_ProductInUse_TAProduct->ZoneHigh )
                )
                {
                    extendProductInUseFlag = TRUE;
                    CsDbg( BRLL_RULE, "BR_LLSC_1_3 : Product in use zone range = %d - %d", pMYKI_ProductInUse_TAProduct->ZoneLow, pMYKI_ProductInUse_TAProduct->ZoneHigh );
                    CsDbg( BRLL_RULE, "BR_LLSC_1_3 : Product[%d] zone range = %d - %d", i, pMYKI_TAProduct->ZoneLow, pMYKI_TAProduct->ZoneHigh );
                    CsDbg( BRLL_RULE, "BR_LLSC_1_3 : Extend the product in use." );
                }
        }

        for ( zone = pMYKI_TAProduct->ZoneLow; zone <= pMYKI_TAProduct->ZoneHigh; zone++ )
            ZoneMap[ zone ] = i;
    }

    //  Now find the lowest and highest covered zones which are contiguous with the zones covered
    //  by the product in use.

    for ( ZoneLow  = pMYKI_ProductInUse_TAProduct->ZoneLow ; ZoneLow  >= 0               && ZoneMap[ ZoneLow  ] > 0; ZoneLow--  )
    {
        //  Do nothing, just searching
    }

    for ( ZoneHigh = pMYKI_ProductInUse_TAProduct->ZoneHigh; ZoneHigh < DIMOF( ZoneMap ) && ZoneMap[ ZoneHigh ] > 0; ZoneHigh++ )
    {
        //  Do nothing, just searching
    }

    ZoneLow++;
    ZoneHigh--;

    CsDbg( BRLL_RULE, "BR_LLSC_1_3 : Combined zone range = %d - %d", ZoneLow, ZoneHigh );

    //  Extend the zone range (and purchase value) of the product in use if needed
    if ( extendProductInUseFlag )
    {
        CsDbg( BRLL_RULE, "BR_LLSC_1_3 : Extending product in use", i );
        if
        (
            myki_br_getFareStoredValue
            (
                pData,
                ZoneLow, ZoneHigh,
                pMYKI_TAControl->PassengerCode,
                MYKI_CD_TRIP_DIRECTION_UNKNOWN,
                FALSE, 0,
                pData->DynamicData.currentDateTime,
                pData->DynamicData.currentDateTime
            ) < 0
        )
        {
            CsErrx( "BR_LLSC_1_3 : myki_br_getDailyFareStoredValue() failed" );
            return RULE_RESULT_ERROR;
        }

        PurchaseValue = ( ProductInUse_ProductType == PRODUCT_TYPE_DAILY ) ? pData->Tariff.dailyCapValue : pData->Tariff.weeklyCapValue;

        MYKI_ProductInUse_TAProduct_New = *pMYKI_ProductInUse_TAProduct;

        MYKI_ProductInUse_TAProduct_New.ZoneLow         = ZoneLow;
        MYKI_ProductInUse_TAProduct_New.ZoneHigh        = ZoneHigh;
        MYKI_ProductInUse_TAProduct_New.PurchaseValue   = PurchaseValue;

        CsDbg( BRLL_RULE, "BR_LLSC_1_3 : Extending product at directory index %d, zone %d - %d, purchase value %d", pMYKI_TAControl->ProductInUse, ZoneLow, ZoneHigh, PurchaseValue );

        if ( myki_br_ldt_ProductUpdate_Extend( pData, pMYKI_ProductInUse_Directory, pMYKI_ProductInUse_TAProduct, &MYKI_ProductInUse_TAProduct_New, FALSE, TRUE, TRUE ) < 0 )
        {
            CsErrx( "BR_LLSC_1_3 : myki_br_ldt_ProductUpdate_Extend() failed" );
            return RULE_RESULT_ERROR;
        }
    }

    //  Invalidate each mergeable product (other than the product in use) in the contiguous zone range.

    for ( i = 0; i < MergeableProductCount; i++ )
    {
        if ( MergeableProductList[ i ] != pMYKI_TAControl->ProductInUse )
        {
            if ( myki_br_GetCardProduct( MergeableProductList[ i ], &pMYKI_Directory, &pMYKI_TAProduct ) < 0 )
            {
                CsErrx( "BR_LLSC_1_3 : myki_br_GetCardProduct( %d ) failed", MergeableProductList[ i ] );
                return RULE_RESULT_ERROR;
            }

            if ( pMYKI_TAProduct->ZoneLow >= ZoneLow && pMYKI_TAProduct->ZoneHigh <= ZoneHigh )
            {
                CsDbg( BRLL_RULE, "BR_LLSC_1_3 : Invalidating product at directory index %d", MergeableProductList[ i ] );

                if ( myki_br_ldt_ProductUpdate_Invalidate( pData, pMYKI_Directory ) < 0 )
                {
                    CsErrx( "BR_LLSC_1_3 : myki_br_ldt_ProductUpdate_Invalidate(%d) failed", MergeableProductList[ i ] );
                    return RULE_RESULT_ERROR;
                }

                nMergedProducts++;
            }
        }
    }

    //  If we did not invalidate any products, return BYPASSED

    if ( nMergedProducts == 0 )
    {
        CsDbg( BRLL_RULE, "BR_LLSC_1_3 : BYPASSED - no product to merge found" );
        return RULE_RESULT_BYPASSED;
    }

    //  If one or more products were invalidated (even if the product in use zone range was NOT extended), return EXECUTED

    CsDbg( BRLL_RULE, "BR_LLSC_1_3 : Executed" );
    return RULE_RESULT_EXECUTED;
}
