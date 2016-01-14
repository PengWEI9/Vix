/**************************************************************************
*   ID :  BR_LLSC_6_14
*
*    Asymmetric Pricing - Full coverage
*
*    1.  TAppControl.ProductInUse
*    2.  TAppTProduct.ControlBitmap
*    3.  TAppTProduct.ZoneLow
*    4.  TAppTProduct.ZoneHigh
*    5.  Dynamic.CurrentTripZoneLow
*    6.  Dynamic.CurrentTripZoneHigh
*    7.  Dynamic.CurrentTrip.IsThinZone
*    8.  TAppControl.Directory.Status
*    9.  TAppTProduct.PurchaseValue
*    10.  Dynamic.CombinedProductValue
*    11.  Dynamic.ProposedMergezone.Low
*    12.  Dynamic.ProposedMergezone.High
*    13.  Dynamic.ProposedMergeFare
*    14.  Dynamic.ProposedMergeDirection
*    15.  Tariff.ApplicableFare
*    16.  Dynamic.CappingContribution
*    17.  Dynamic.CappingEpassZoneLow
*    18.  Dynamic.CappingEpassZoneHigh
*    19.  Dynamic.CurrentTripDirection
*    20.  Dynamic.FareRouteID
*    21.  Dynamic.CurrentDateTime
*    22.  TAppControl.PassengerCode
*
*   Pre-Conditions
*    1.  Determine if we have full product coverage. Full product coverage is where a product already exists
*        under each zone of the current trip zone range:
*           a.  For each zone that is covered by the current trip zone range (zone low(5)  to zone high(6)  ) check
*               that:
*               i.  An existing activated product covers the zone.
*
*
*   Description
*    1.  Produce a zonal map for the combined zone range(11) ,(12) (nb: A zonal map is structure to determine
*        which products cover which zones. For each zone only 1 products of type: e-Pass, Weekly, Daily,
*        single trip, or n-Hour - where the border status is not set, can exist; For n-Hour products with different
*        border status more than 1 can cover the same zone )
*    2.  For each active product that is of type e-Pass and that is within the combined zone range(11) ,12
*           a.  Determine the zones that the products cover - recording these on a zonal map
*    3.  For every product that is type Weekly
*           a.  Determine the zones that the products cover - update the zonal map.
*    4.  For every product that is type Daily
*           a.  Determine the zones that the product cover - update the zonal map
*    5.  For every product that is type n-Hour or single trip determine the zones that the product covers and
*        update the zonal map Border
*    6.  For each contiguous block of zones that resolves it self as type e-Pass, weekly or daily,
*          a.  determine the fare as:
*             i.   The zone range determined as determined by the zonal map
*             ii.  Passenger type(22)
*             iii. Determined fare route(20)
*             iv.  The current date time(21)
*          b.  Accumulate this value in CombinedProductValue(10)
*    7.  For each contiguous block of products that contains 1 or more n-Hour or Single Trip products;
*        accumulate the product's purchase value into the CombinedProductvalue(10)
*    8.  If the combined product value(10)  is greater than the combined zone fare(13)  then we need to create a
*        refund:
*          a.  Decrement applicable fare(15)  by the result of combined product value(10)  minus the combined zone
*              range fare(13) .
*          b.  Upgrade the oldest product n-Hour product
*          i.  If the oldest product is a city saver then perform a ProductSale/Upgrade
*             (1)  Alter the product type to n-Hour
*             (2)  Set product value purchase value(9)  to combined zone range fare(10)
*             (3)  Extend the product zone range to include the combined zone range(11) ,12
*                 (a)  If the low zone(3)  of the n-Hour product is greater than low zone of the combined
*                      zone(11)  range set the low zone(3)  of the n-Hour product to the low zone of the
*                      combined zone range(11) .
*                 (b)  If the high zone(4)  of the n-Hour product is less than the high zone of the combined
*                      zone(12)  range set the high zone(4)  of the n-Hour product to the high zone of the
*                      combined zone range(12) .
*             (4)  As required adjust for city saver zone:
*                 (a)   If the product low zone(5)  less 1 is a City Saver zone then decrement the product low
*                       zone(7)  by 1
*                 (b)  If the product High zone(6)  is a city saver then increment the product high zone(8)  by 1
*          ii.  Else upgrade the oldest n-Hour: perform a ProductUpdate/Extend
*                (1)  Set product value purchase value(9)  to combined zone range fare(10)
*                (2)  Extend the product zone range to include the combined zone range(11) ,12
*                   (a)  If the low zone(3)  of the n-Hour product is greater than or equal to the low zone of the
*                        combined zone(11)  range set the low zone of the n-Hour product to the low zone of
*                        the combined zone range(11) .
*                   (b)  If the high zone(4)  of the n-Hour product is less than or equal to the high zone of the
*                        combined zone(12)  range set the high zone(4)  of the n-Hour product to the high zone of
*                        the combined zone range(12) .
*                   (c)  As required adjust for city saver zone:
*                      1.   If the product low zone(5)  less 1 is a City Saver zone then decrement the
*                           product low zone(7)  by 1
*                      2.  If the product High zone(6)  is a city saver then increment the product high
*                          zone(8)  by 1
*                (3)  Perform a ProductUpdate/None
*                     (a)  Set the trip direction of the product to be combined trip direction(14) .
*                     (b)  Clear the border status bit
*          iii.  Set the n-Hour product as the product in use(1)  using a TAppUpdate/SetProductInUse
*          iv.  For each n-Hour product (excluding the product in use(1) ) perform ProductUpdate/invalidate
*               transaction
*          v.  Decrement the capping amount(16)  to the combined product value(10)  minus combined zone
*              range fare(13) .
*          vi.  Determine the e-pass capping contribution using the zonal map. For each contiguous block
*               of zones from the zonal map, where the product is of type e-Pass
*          (1)  Determine the fare:
*             (a)  The low zone of the contiguous block
*             (b)  The high zone of the contiguous block
*             (c)  The passenger type(22)
*             (d)  Determined fare type
*             (e)  The current date time(21)
*          (2)  Increase the capping contribution(16)  by the e-Pass contribution amount
*          (3)  Set the e-Pass capping low  zone(17)  range to the e-Pass low zone(3)
*          (4)   Set the e-Pass capping high zone(18)  range to the e-Pass high zone(4)
*    9.  Else
*           a.  Perform ProductUpdate/invalidate transaction on the product in use(1)  if the provisional bit on
*               product control bitmap(2)  is set.
*           b.  For each activated valid product  on the card check to see if a single product covers the current
*               trip zone range(5) ,6
*                 i.  If such a product does exist, set this product as the product in use(1)  using a
*                     TAppUpdate/SetProductInUse transaction
*                 ii.  Else set the product in use(1)  to undefined using a TAppUpdate/SetProductInUse
*           c.  Re-determine the trip direction for the combined trip zone range
*              i.  Reset the proposed merge trip direction(14)  to unknown
*              ii.  For each n-hour product on the zonal map between the current trip low zone(5)  and current trip
*                   high zone(6)
*                 (1)  If the proposed merge trip direction(14)  is Unknown, set the proposed merge trip
*                      direction(14)  to the product trip direction
*                 (2)  Else if the proposed merge trip direction(14)  is not equal to the product control bitmap trip
*                      direction(2) , and the product trip direction(2)  is not equal to Unknown, set the proposed
*                      merge trip direction(14)  to Disabled
*              iii.  If the proposed merge trip direction(14)  is Unknown, and the Current Trip direction(19)  is not
*                    Unknown; set the proposed merge trip direction(14)  to the product trip direction.
*              iv.  Else if the proposed merge trip direction(14)  is not equal to the current trip direction(19)  and the
*                   current trip direction(19)  is not equal to Unknown, set the proposed merge trip direction(14)  to
*                   Disabled
*              v.  For each n-hour product (including the product in use(1)  that is covered either partially or fully
*                  by the current trip zone range perform a ProductUpdate/none transaction where current trip
*                  direction is not equal to the proposed merge trip direction(14)
*                     (1)  Set the trip direction to the proposed merge trip direction(14) .
*
*
*   Notes
*    Doesn't cater for border zones
*
*    With full product coverage we need still check
*    that no more merging can take place.
*    Determine the total value you have paid so far.
*
*    This method means that hour and single trips
*    will float to the top
*
*    Fare for the current proposed merge zone
*    range.
*
 ***********************************************************************/

#include <cs.h>                 // For debug logging
#include <myki_cardservices.h>  // Myki Card Services
#include <LDT.h>                // Logical Device Transaction
#include <myki_cd.h>            // myki_cd (Myki Configuration Data)

#include "myki_br_rules.h"      // API for all BR_xxx modules
#include "BR_Common.h"          // Common utilities, macros, etc


//=============================================================================
//
//  Purpose :
//  ---------
//  If the entire current trip is covered by one or more non-provisional products, then
//  (a) Delete the provisional product if there is one, and
//  (b) If a refund is needed, merge the nHour products to the oldest one and apply the refund, and
//  (c) Determine if a single product can be used and if so, set it to product in use.
//
//  Pseudo-code :
//  -------------
//  -   Create a preferential zone map - this is a map of which active non-provisional product applies to which zone.
//  -   If the entire current trip (low zone to high zone) is not covered by one or more products in the zone map,
//      -   BYPASS
//
//  -   If the product in use is provisional, then invalidate it
//  -   Calculate combined product value - this is the combined peak and off-peak values of all products in the zone
//      map which overlap the current trip.
//  -   If the combined product value is greater than the proposed merge fare,
//      -   Create a refund
//          -   If there are no nHour products in the zone map which overlap the current trip,
//              -   ERROR (shouldn't normally happen, but potentially could if daily/weekly fares get reduced at any point)
//          -   Decrement applicable fare by the difference between the combined product value and the proposed merge fare
//          -   Upgrade the oldest nHour product in the zone map to cover at least the entire proposed merge zone range
//              -   Fare = proposed merge fare
//              -   Zone range = union of proposed merge zone range and product current zone range
//              -   Direction = proposed merge direction
//              -   Border status = clear
//          -   Set the oldest nHour product (the one upgraded above) as the product in use
//          -   Invalidate all other nHour products in the zone map which overlap the current trip
//          -   Set the capping amount to the combined product value less the proposed merge fare
//          -
//  -   Else (the combined product value is NOT greater than the proposed merge fare)
//      -   If there is an active non-provisional product on the card which covers the entire current trip,
//          -   Set it as the product in use (ignore subsequent products if more than one?)
//      -   Else
//          -   Set the product in use to NONE
//      -   Recalculate the proposed merge direction based on all nHour products in the zone map which
//          overlap the current trip
//      -   Update all nHour products in the zone map which overlap the current trip (including the product in use)
//          -   Set the product direction to the proposed merge direction (if it's not already)
//
//=============================================================================

RuleResult_e BR_LLSC_6_14( MYKI_BR_ContextData_t *pData )
{
    MYKI_TAControl_t    *pMYKI_TAControl;
  //MYKI_Directory_t    *pDirectory = NULL;
  //MYKI_TAProduct_t    *pProduct   = NULL;
    int                 dir;
    int                 i;

    CsDbg( BRLL_RULE, "BR_LLSC_6_14 : Start (Asymmetric Pricing - Full coverage)" );

    if ( ! pData )
    {
        CsErrx( "BR_LLSC_6_14 : Invalid argument(s)" );
        return RULE_RESULT_ERROR;
    }

    if ( MYKI_CS_TAControlGet( &pMYKI_TAControl ) < 0 )
    {
        CsErrx( "BR_LLSC_6_14 : MYKI_CS_TAControlGet() failed" );
        return RULE_RESULT_ERROR;
    }

    //
    //  Pre-conditions
    //

    //  1. Determine if we have full product coverage

    //  Need to do this here so we can use the created map to check for full product coverage
    //  This covers steps 1 - 5 below
    CsDbg( BRLL_FIELD, "BR_LLSC_6_14 : ProposedMergeZoneLow(%d) ProposedMergeZoneHigh(%d)", pData->InternalData.ProposedMerge.ZoneLow ,pData->InternalData.ProposedMerge.ZoneHigh);

    if ( myki_br_CreateZoneMap( pData, pData->InternalData.ProposedMerge.ZoneLow, pData->InternalData.ProposedMerge.ZoneHigh ) < 0 )
    {
        CsErrx( "BR_LLSC_6_14 : myki_br_CreateZoneMap() failed" );
        return RULE_RESULT_ERROR;
    }

    //  Use the created zone map to see if we have full coverage

    if ( ! myki_br_IsFullCoverage( pData, pData->DynamicData.currentTripZoneLow, pData->DynamicData.currentTripZoneHigh ) )
    {
        CsDbg( BRLL_RULE, "BR_LLSC_6_14 : Bypass - Active products do not fully cover current trip zone range" );
        return RULE_RESULT_BYPASSED;
    }

    //
    //  Execution
    //

    //  Steps 1 - 5 :   Create preferential zone map.
    //                  Already done above

    //  Steps 6 - 7 :   Calculate combined product value.

    if ( myki_br_CalculateCombinedProductValue( pData, pMYKI_TAControl->PassengerCode, &pData->DynamicData.combinedProductValue, &pData->DynamicData.combinedProductValueOffPeak ) < 0 )
    {
        CsErrx( "BR_LLSC_6_14 : myki_br_CalculateCombinedProductValue() failed" );
        return RULE_RESULT_ERROR;
    }

    //  Step 8 :    If the combined product value is greater than the combined zone fare then a merge is possible and we need to reduce the applicable fare (create a refund)

    CsDbg( BRLL_RULE, "BR_LLSC_6_14 : Step 8 : Check CPV(%d) > CZRF(%d)", pData->DynamicData.combinedProductValue ,pData->InternalData.ProposedMerge.Fare);

    if ( pData->DynamicData.combinedProductValue > pData->InternalData.ProposedMerge.Fare )
    {
        U8_t                dir;
        MYKI_Directory_t    *pDirectory = NULL;
        MYKI_TAProduct_t    *pProduct = NULL;
        U8_t                ProductBitmap;
        int                 bAlwaysCreateProductUpdateNone  = FALSE;
        TripDirection_t     ProductTripDirection            = TRIP_DIRECTION_UNKNOWN;

        CsDbg( BRLL_RULE, "BR_LLSC_6_14 : Combined product value (%d) > proposed merge fare (%d) : merge to oldest nHour", pData->DynamicData.combinedProductValue, pData->InternalData.ProposedMerge.Fare );

        //  Step 8.b :  Upgrade the oldest n-Hour product (find the oldest nHour product that overlaps the current trip)
        //              Do this first so nothing gets updated (in step 8.a) if there are no nHour products.

//        if ( ( dir = myki_br_GetOldestNHourDirContained( pMYKI_TAControl, pData->InternalData.ProposedMerge.ZoneLow, pData->InternalData.ProposedMerge.ZoneHigh ) ) <= 0 )
        if ( ( dir = myki_br_GetOldestNHourDirOverlap( pMYKI_TAControl, pData->InternalData.ProposedMerge.ZoneLow, pData->InternalData.ProposedMerge.ZoneHigh ) ) <= 0 )
        {
            CsErrx( "BR_LLSC_6_14 : A refund is indicated, but we have no nHour products to merge, possibly a reduction in fares in CD?" );
            return RULE_RESULT_ERROR;
        }

        //  Step 8.a :  Decrement applicable fare by the result of combined product value minus the combined zone range fare

        CsDbg( BRLL_FIELD, "BR_LLSC_6_14 : Reducing applicable fare by %d from %d to %d", (pData->DynamicData.combinedProductValue - pData->InternalData.ProposedMerge.Fare), pData->ReturnedData.applicableFare, pData->ReturnedData.applicableFare - (pData->DynamicData.combinedProductValue - pData->InternalData.ProposedMerge.Fare) );
        pData->ReturnedData.applicableFare -= (pData->DynamicData.combinedProductValue - pData->InternalData.ProposedMerge.Fare);

        //  Step 8.b :  Upgrade the oldest n-Hour product

        myki_br_GetCardProduct( dir, &pDirectory, &pProduct );

        //  8.b.i           : If the oldest product is a CitySaver (ignore as myki_br_GetOldestNHourDir() will only ever return an nHour product)
        //  Step 8.b.ii     : Else upgrade the oldest n-Hour:
        //  Step 8.b.ii     : - Perform a product update/extend
        //  Step 8.b.ii.2   :   - Extend the product zone range to include the combined zone range
        //  Step 8.b.ii.1   :   - Set product (value) purchase value to the combined zone range fare
        //  Step 8.b.ii.3   : - Perform a product update/none
        //  Step 8.b.ii.3.a :   - Set the trip direction of the product to (be) combined trip direction
        //  Step 8.b.ii.3.b :   - Clear the border status bit

        ProductBitmap           = pProduct->ControlBitmap;
        ProductTripDirection    = myki_br_getProductTripDirection( pProduct );
        if
        (
            pData->StaticData.AcsCompatibilityMode != FALSE &&
            pData->InternalData.ProposedMerge.TripDirection == TRIP_DIRECTION_DISABLED &&
            ProductTripDirection != TRIP_DIRECTION_UNKNOWN &&
            ProductTripDirection != TRIP_DIRECTION_DISABLED
        )
        {
            //  NOTE:   ACS sets TripDir to "Outbound" instead of the correct "Disabled"
            //
            CsDbg( BRLL_RULE, "BR_LLSC_6_14 : Applied ACS Compatibility Mode - Set TripDir to OUTBOUND" );
            myki_br_SetBitmapDirection( &ProductBitmap, TRIP_DIRECTION_OUTBOUND );
            bAlwaysCreateProductUpdateNone  = TRUE;
        }
        else
        {
            myki_br_SetBitmapDirection( &ProductBitmap, pData->InternalData.ProposedMerge.TripDirection );
        }   /*  end-of-if */
        myki_br_SetBitmapBorderStatus( &ProductBitmap, BORDER_STATUS_NONE );

        myki_br_ExtendProduct
        (
            pData,                                              // Context
            pDirectory,                                         // Directory entry
            pProduct,                                           // Product
            pData->InternalData.ProposedMerge.ZoneLow,          // Low Zone
            pData->InternalData.ProposedMerge.ZoneHigh,         // High Zone
            pData->InternalData.ProposedMerge.Fare,             // Purchase Value
            0,                                                  // Additional Hours
            ProductBitmap,                                      // Direction, border status, provisional, off-peak, premium, autoload
            bAlwaysCreateProductUpdateNone                      // Create ProductUpdate/None only if changes
        );

        //  Step 8.b.iii :  Set the nHour product as the product in use (it now covers the entire trip)

        myki_br_ldt_AppUpdate_SetProductInUse( pData, pDirectory );

        //  Step 8.b.iv :   Invalidate all nHour products in the zone map that overlap the current trip, other than the product in use we just extended

        for ( i = 1; i < DIMOF( pMYKI_TAControl->Directory ); i++ ) // T-Purse dir entry does not have a product slot, so there's one less product slots than dir entries
        {
            if ( i != dir /*Extended Product*/ )
            {
                if ( myki_br_GetCardProduct( i, &pDirectory, NULL ) < 0 )
                {
                    CsErrx( "BR_LLSC_6_14 : myki_br_GetCardProduct() failed" );
                    return RULE_RESULT_ERROR;
                }

                if ( pDirectory->Status != TAPP_CONTROL_DIRECTORY_STATUS_UNUSED &&
                     myki_br_cd_GetProductType( pDirectory->ProductId ) == PRODUCT_TYPE_NHOUR )
                {
                    if ( myki_br_GetCardProduct( i, &pDirectory, &pProduct ) < 0 )
                    {
                        CsErrx( "BR_LLSC_6_14 : myki_br_GetCardProduct() failed" );
                        return RULE_RESULT_ERROR;
                    }

                    if ( myki_br_ZonesOverlapOrAdjacent( pProduct->ZoneLow, pProduct->ZoneHigh, pData->DynamicData.currentTripZoneLow, pData->DynamicData.currentTripZoneHigh ) )
                    {
                        CsDbg( BRLL_RULE, "BR_LLSC_6_14 : Invalidate NHOUR product (%d)", i );
                        myki_br_ldt_ProductUpdate_Invalidate( pData, pDirectory );
                    }
                }
            }
        }

        //  Step 8.b.v :    Decrement the capping amount16 to the combined product value10 minus combined zone range fare13

        pData->DynamicData.cappingContribution -= ( pData->DynamicData.combinedProductValue - pData->InternalData.ProposedMerge.Fare );

        //  Step 8.b.vi :   For each contiguous block of zones where the product is e-Pass

        if ( myki_br_CalculateEPassCappingContribution( pData, pMYKI_TAControl->PassengerCode ) < 0 )
        {
            CsErrx( "BR_LLSC_6_14 : Unable to calculate ePass capping contribution" );
            return RULE_RESULT_ERROR;
        }
    }

    // Step 9
    else
    {
        MYKI_Directory_t    *pDirectory = NULL;
        MYKI_TAProduct_t    *pProduct = NULL;

        CsDbg( BRLL_RULE, "BR_LLSC_6_14. Step 8. No Merge possible");
        CsDbg( BRLL_RULE, "BR_LLSC_6_14 : Combined product value (%d) <= proposed merge fare (%d) : try to find a single product to cover trip", pData->DynamicData.combinedProductValue, pData->InternalData.ProposedMerge.Fare );

        // Step 9.a :   Perform ProductUpdate/invalidate transaction on the product in use1 if the provisional bit on product control bitmap2 is set.

        if ( pMYKI_TAControl->ProductInUse > 0 )
        {
            if ( myki_br_GetCardProduct( pMYKI_TAControl->ProductInUse, &pDirectory, &pProduct ) < 0 )
            {
                CsErrx( "BR_LLSC_6_14 : Failed to get product in use" );
                return RULE_RESULT_ERROR;
            }

            if ( pProduct->ControlBitmap & TAPP_TPRODUCT_CONTROL_PROVISIONAL_BITMAP )
            {
                CsDbg( BRLL_RULE, "BR_LLSC_6_14 : Invalidate provisional product in use (%d)", pMYKI_TAControl->ProductInUse );
                myki_br_ldt_ProductUpdate_Invalidate( pData, pDirectory );
            }
        }

        // Step 9.b :   Check if a single active valid product covers the current trip zone range

        for ( dir = 1; dir < DIMOF( pMYKI_TAControl->Directory ); dir++ ) // T-Purse dir entry does not have a product slot, so there's one less product slots than dir entries
        {
            if ( myki_br_GetCardProduct( dir, &pDirectory, NULL ) < 0 )
            {
                CsErrx( "BR_LLSC_6_14 : Unable to get product %d details", dir );
                return RULE_RESULT_ERROR;
            }

            if ( pDirectory->Status == TAPP_CONTROL_DIRECTORY_STATUS_ACTIVATED )
            {
                if ( myki_br_GetCardProduct( dir, &pDirectory, &pProduct ) < 0 )
                {
                    CsErrx( "BR_LLSC_6_14 : Unable to get product %d details", dir );
                    return RULE_RESULT_ERROR;
                }

                if
                (
                    ( pProduct->ZoneLow <= pData->DynamicData.currentTripZoneLow ) &&
                    ( pProduct->ZoneHigh >= pData->DynamicData.currentTripZoneHigh )
                )
                {
                    CsDbg( BRLL_RULE, "Single active product (index %d) covers current trip, make it the product in use", dir );
                    break;      // Break out of the for loop with pDirectory pointing to the valid product, which will satisfy 9.b.i.
                }
            }

            // If this is not our last dir entry, this will be set again at the top of the next iteration
            // If this is out last dir entry, we'll leave the loop with this set to NULL which will satisfy 9.b.ii.
            pDirectory = NULL;
        }

        //  9.b.i  :    If at least one product exists that covers the current trip (pDirectory is not NULL), then set that product to be the product in use.
        //  9.b.ii :    If no products exist that cover the current trip (pDirectory is NULL), then set the product in use to undefined.
        //  Note :      Both these cases are handled within myki_br_ldt_AppUpdate_SetProductInUse() based on the value of pDirectory.

        if ( myki_br_ldt_AppUpdate_SetProductInUse( pData, pDirectory ) < 0 )
        {
            CsErrx( "BR_LLSC_6_14 : myki_br_ldt_AppUpdate_SetProductInUse() failed" );
            return RULE_RESULT_ERROR;
        }

        //  Step 9.c :      Redetermine the direction for the proposed merge zone range
        //  Step 9.c.i :    Reset the proposed merge direction to NONE

        myki_br_ClearProposedMergeTripDirection( pData );

        //  Step 9.c.ii :   For each nHour product in the zone map that overlaps the current trip
        //                  update the proposed merge direction based on the product direction

        for ( i = 0; i < pData->InternalData.ZoneMap.productCount; i++ )
        {
            MYKI_Directory_t    *pDirectory = NULL;
            MYKI_TAProduct_t    *pProduct   = NULL;

            if
            (
                ( pData->InternalData.ZoneMap.productList[ i ].productType == PRODUCT_TYPE_NHOUR  ) &&
                myki_br_ZonesOverlap
                (
                    pData->InternalData.ZoneMap.productList[ i ].zoneLow,
                    pData->InternalData.ZoneMap.productList[ i ].zoneHigh,
                    pData->DynamicData.currentTripZoneLow,
                    pData->DynamicData.currentTripZoneHigh
                )
            )
            {
                if ( myki_br_GetCardProduct( pData->InternalData.ZoneMap.productList[ i ].dirIndex, &pDirectory, &pProduct ) < 0 )
                {
                    CsErrx( "BR_LLSC_6_14 : Failed to get product at directory index %d", pData->InternalData.ZoneMap.productList[ i ].dirIndex );
                    return RULE_RESULT_ERROR;
                }

                myki_br_UpdateProposedMergeTripDirection( pData, myki_br_getProductTripDirection( pProduct ) );
            }
        }

        //  Step 9.c.iii and iv :   Update the proposed merge direction based on the current trip direction

        myki_br_UpdateProposedMergeTripDirection( pData, pData->DynamicData.currentTripDirection );

        // Step 9.c.v : For each nHour product in the zone map which overlaps the current trip,
        //              update the product direction to match the proposed merge direction if needed.

        for ( i = 0; i < pData->InternalData.ZoneMap.productCount; i++ )
        {
            if
            (
                ( pData->InternalData.ZoneMap.productList[ i ].productType == PRODUCT_TYPE_NHOUR  ) &&
                myki_br_ZonesOverlap
                (
                    pData->InternalData.ZoneMap.productList[ i ].zoneLow,
                    pData->InternalData.ZoneMap.productList[ i ].zoneHigh,
                    pData->DynamicData.currentTripZoneLow,
                    pData->DynamicData.currentTripZoneHigh
                )
            )
            {
                if ( myki_br_GetCardProduct( pData->InternalData.ZoneMap.productList[ i ].dirIndex, &pDirectory, &pProduct ) < 0 )
                {
                    CsErrx( "BR_LLSC_6_14 : Failed to get product at directory index %d", pData->InternalData.ZoneMap.productList[ i ].dirIndex );
                    return RULE_RESULT_ERROR;
                }

                //  NOTE:   ACS performed ProductUpdate/None on border product regardless
                //          of product TripDir to be changed or not.
                if
                (
                    (
                        ( pData->StaticData.AcsCompatibilityMode != FALSE ) &&
                        ( ( pProduct->ControlBitmap & TAPP_TPRODUCT_CONTROL_BORDER_STATUS_BITMAP ) != 0 )
                    ) ||
                    ( myki_br_getProductTripDirection( pProduct ) != pData->InternalData.ProposedMerge.TripDirection )
                )
                {
                    TripDirection_t     ProposedMergedTripDirection = pData->InternalData.ProposedMerge.TripDirection;

                    if ( ProposedMergedTripDirection == TRIP_DIRECTION_DISABLED && pData->StaticData.AcsCompatibilityMode != FALSE )
                    {
                        //  NOTE:   ACS sets TripDir to "Outbound" instead of the correct "Disabled".
                        //
                        CsDbg( BRLL_RULE, "BR_LLSC_6_14 : Applied ACS Compatibility Mode - Set TripDir to OUTBOUND" );
                        ProposedMergedTripDirection = TRIP_DIRECTION_OUTBOUND;
                    }

                    if ( myki_br_ldt_ProductUpdate_SetTripDirection( pData, pDirectory, ProposedMergedTripDirection ) < 0 )
                    {
                        CsErrx( "BR_LLSC_6_14 : myki_br_ldt_ProductUpdate_SetTripDirection() failed" );
                        return RULE_RESULT_ERROR;
                    }
                }
            }
        }
    }

    CsDbg( BRLL_RULE, "BR_LLSC_6_14 : Executed" );
    return RULE_RESULT_EXECUTED;
}

