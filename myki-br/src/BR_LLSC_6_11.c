/**************************************************************************
*   ID :  BR_LLSC_6_11
*
*    Prepare for Asymmetric Pricing
*
*    1.  TAppControl.ProductInUse
*    2.  TAppTProduct.ControlBitmap
*    3.  TAppTProduct.ZoneLow
*    4.  TAppTProduct.ZoneHigh
*    5.  Dynamic.CurrentTripZoneLow
*    6.  Dynamic.CurrentTripZoneHigh
*    7.  Tariff.CitySaverZoneFlag
*    8.  TAppTProudct.EndDateTime
*    9.  Dynamic.ProposedMergeZoneLow
*    10.  Dynamic.ProposedMergeZoneHigh
*    11.  Dynamic.ProposedMergeFare
*    12.  Dynamic.ProposedMerge.Direction
*    13.  TAppControl.Directory.Status
*    14.  Dynamic.CurrentTripDirection
*    15.  Dynamic.CappingEpassZoneLow
*    16.  Dynamic.CappingEpassZoneHigh
*    17.  Dynamic.FareRouteID
*    18.  Dynamic.OffpeakDiscountRate
*    19.  Dynamic.CominedZoneRangeOffPeakFare
*    20.  TAppTProduct.StartDateTime
*    21.  Dynamic.CurrentDateTime
*    22.  Dynamic.Zone
*    23.  UsageLog
*    24. Common date
*    25. Border
*
*   Pre-Conditions
*    None
*
*   Description
*    1.  Set the:
*        a.  Proposed merge zone low(9)  to 255 , proposed merge zone high(10)   to 0
*        b.  Proposed merge fare(11)  to zero
*        c.  Proposed merge trip direction(12)  to unknown
*        d.  Capping e-Pass Zone Low(15)   to 255
*        e.  Capping e-Pass zone high(16)   to 0
*    2.  Determine:
*        a.  For each product on the card
*            i.  That is not:
*                (1)  A Product with Border status bit of the product control bitmap(2)  set true (1); and
*                (2)  A Product that is type e-Pass that has product status of inactive(13)
*                (3)  The Product In Use(1)
*                (4)  A product status(13)  is not activated
*            ii.  Determine the combined zone range(9) ,10 covered by all existing products
*                (1)  If ProposedMergeZoneLow(9)  is greater than the low zone of the product; then set the
*                     Proposed Merge Zone Low(9)  to low zone of the Product.
*                (2)  If ProposedMergeZoneHigh(10)  is less than the zone high of the product; then set the
*                     ProposedMergeZoneHigh(10)  to high zone of the Product.
*            iii.  If the product is of type n-Hour; resolve the trip direction as follows:
*                (1)  If the proposed merge trip direction(12)  is Unknown, set the proposed merge trip
*                     direction(12)  to the product trip direction
*                (2)  Else if the proposed merge trip direction(12)  is not equal to the product control bitmap trip
*                     direction(2) , and the produ ct trip direction(2)  is not equal to Unknown, set the proposed
*                     merge trip direction(12)  to Disabled
*    3.  Update the trip direction to include the current trip direction(14)
*        a.  If the proposed merge trip direction(12)  is Unknown, and the current trip direction(14)  is not Unknown;
*            then set the proposed merge trip direction(12)  to the product trip direction(14) .
*        b.  Else if the proposed merge trip direction(12)  is not equal current trip direction(14)   and the current trip
*            direction(14)  is not equal to Unknown then set the proposed merge trip direction(12)  to Disabled
*    4.  If there is no active e-Pass; then for each zone in the CurrentTrip from zone low(5)  to zone high(6)  check
*        that we have existing product coverage from any product with a product status(13)  of active on the card
*        excluding  the provisional product (ie the provisional bit of product control bit map(2)  is not set)
*            a.  If there are no valid products on the card covering this zone and
*            b.  a product of type e-Pass with the product status(13)  of inactive and the start date of the e-pass(20)  is
*                less than or equal  the start time of the product in use exists where the product would provide
*                coverage for this zone then:
*                i.   Activate the Inactive e-Pass by performing an ProductUpdate/Activate transaction
*                ii.  Update the low zone of the combined zone(9)  range if the e-Pass zone Low(3)  is less than low
*                     zone of the combined zone range(9)
*                iii. Update the high zone of the combined zone range(10)  if the e-Pass zone High(4)  is greater than
*                     the high zone of the combined zone range(10)
*                iv.  Modify usage log(23)
*                    (a)  Definition:
*                    (b)  Set Usage - Product validation status Activated, bit 0 = true
*
*    5.  If the low zone of the current trip(5)  is less than the low zone of the combined zone range(9)  and the
*        current trip border status of product control bit map(2)  is not set (ie the current trip is not a border trip)
*        then set the low zone of the combined zone range(9)  to the low zone of the current trip(5)
*
*    6.  If the high zone of the current trip(6)  is greater than the high zone of the combined zone range(10)  and
*        the current trip is not a border trip then set the high zone of the combined zone range(10)  to the high
*        zone of the current trip(6)
*
*    7. If the tariff has valid tariff data for the current date.
*       a. then
*           i. If the proposed merge zone low(9)  and proposed merge zone high(10)  is not equal to the seed values (ie
*              255,0 respectively ) then set combined zone range fare(11)  to the fare determined for the:
*               i.   combined zone range(9) ,(10)
*               ii.  passenger type
*               iii. date of travel
*               iv.  fare route id(17)
*               v.   current trip direction
*       b. Else set the combinded zone range fare(11) to the provisional product purchase value.
*
*    8.  Set the combined zone range off peak fare(19)  to the combined zone range fare(11)  multiplied by 1 less
*        the off peak discount rate(18) .
*
*    9. Set the common date(24) to the start date of the product in use.
*
*    Devices
*    Fare payment devices.
*
 ***********************************************************************/

#include <cs.h>                 // For debug logging
#include <myki_cardservices.h>  // Myki Card Services
#include <LDT.h>                // Logical Device Transaction

#include "myki_br_rules.h"      // API for all BR_xxx modules
#include "BR_Common.h"          // Common utilities, macros, etc

//=============================================================================
//
//
//
//=============================================================================

RuleResult_e BR_LLSC_6_11( MYKI_BR_ContextData_t *pData )
{
    MYKI_TAControl_t    *pMYKI_TAControl;
    MYKI_Directory_t    *pDirectory = NULL;
    MYKI_TAProduct_t    *pProduct = NULL;
    U8_t                 dir;
    int                 haveActiveEpass = FALSE;

    MYKI_TAProduct_t    *pProductInUse = NULL;

    CsDbg( BRLL_RULE, "BR_LLSC_6_11 : Start (Prepare for Asymmetric Pricing)" );

    if ( ! pData )
    {
        CsErrx( "BR_LLSC_6_11 : Invalid argument(s)" );
        return RULE_RESULT_ERROR;
    }

    if ( MYKI_CS_TAControlGet( &pMYKI_TAControl ) < 0 )
    {
        CsErrx( "BR_LLSC_6_11 : MYKI_CS_TAControlGet() failed" );
        return RULE_RESULT_ERROR;
    }

    if ( pMYKI_TAControl->ProductInUse == 0 )
    {
        CsDbg( BRLL_RULE, "BR_LLSC_6_11 : BYPASSED : ProductInUse is zero");
        return RULE_RESULT_BYPASSED;
    }

    /* Get product and directory structure in use */
    if ( MYKI_CS_TAProductGet( (U8_t)( pMYKI_TAControl->ProductInUse - 1 ), &pProductInUse ) < 0 )
    {
        CsErrx( "BR_LLSC_6_11 : MYKI_CS_TAProductGet() failed" );
        return RULE_RESULT_ERROR;
    }

    //   1.  Set the:
    //       a.  Proposed merge zone low(9)  to 255 , proposed merge zone high(10)   to 0
    //       b.  Proposed merge fare(11)  to zero
    //       c.  Proposed merge trip direction(12)  to unknown
    //       d.  Capping e-Pass Zone Low(15)   to 255
    //       e.  Capping e-Pass zone high(16)   to 0

    pData->InternalData.ProposedMerge.ZoneLow  = ZONE_LOW_NOT_SET;
    pData->InternalData.ProposedMerge.ZoneHigh = ZONE_HIGH_NOT_SET;
    pData->InternalData.ProposedMerge.Fare     = 0;
    myki_br_ClearProposedMergeTripDirection( pData );
    pData->DynamicData.cappingEpassZoneLow     = ZONE_LOW_NOT_SET;
    pData->DynamicData.cappingEpassZoneHigh    = ZONE_HIGH_NOT_SET;

    haveActiveEpass = myki_br_HasActiveEpass( pData, pMYKI_TAControl );

    //   2.  Determine:
    //       a.  For each product on the card
    //           i.  That is not:
    //               (1)  A Product with Border status bit of the product control bitmap(2)  set true (1); and
    //               (2)  A Product that is type e-Pass that has product status of inactive(13)
    //               (3)  The Product In Use(1)
    //               (4)  A product status(13)  is not activated
    //
    //          ie, for each active non-border product that is not the product in use...
    //
    for ( dir = 1; dir < DIMOF( pMYKI_TAControl->Directory ); dir++ ) // T-Purse dir entry does not have a product slot, so there's one less product slots than dir entries
    {
        pDirectory = &pMYKI_TAControl->Directory[ dir ];              // Product slot 0-4 maps to dir entry 1-5

        //  Step 2.a.i.1 : Product border status is set
        //  Step 2.a.i.2 : Product is ePass and not activated
        //  Step 2.a.i.3 : Product is the product in use
        //  Step 2.a.i.4 : Product is not activated

        if ( dir == pMYKI_TAControl->ProductInUse )
        {
            //  Ignore the product in use
            continue;
        }

        if ( pDirectory->Status != TAPP_CONTROL_DIRECTORY_STATUS_ACTIVATED )
        {
            //  Ignore any inactive products
            continue;
        }

        if (MYKI_CS_TAProductGet( (U8_t)( dir - 1 ), &pProduct ) >= 0)
        {
            if ( pProduct->ControlBitmap & TAPP_TPRODUCT_CONTROL_BORDER_STATUS_BITMAP )
            {
                //  Ignore border products
                continue;
            }
        }

        //   2.  Determine ...
        //           ii.  Determine the combined zone range(9) ,10 covered by all existing products
        //               (1)  If ProposedMergeZoneLow(9)  is greater than the low zone of the product; then set the
        //                    Proposed Merge Zone Low(9)  to low zone of the Product.
        //               (2)  If ProposedMergeZoneHigh(10)  is less than the zone high of the product; then set the
        //                    ProposedMergeZoneHigh(10)  to high zone of the Product.
        //
        if ( pData->InternalData.ProposedMerge.ZoneLow > pProduct->ZoneLow )
        {
            pData->InternalData.ProposedMerge.ZoneLow = pProduct->ZoneLow;
        }

        if ( pData->InternalData.ProposedMerge.ZoneHigh < pProduct->ZoneHigh)
        {
            pData->InternalData.ProposedMerge.ZoneHigh = pProduct->ZoneHigh;
        }

        CsDbg( BRLL_RULE, "BR_LLSC_6_11 : Dir: %d. InternalData.ProposedMerge.ZoneLow: %d", dir, pData->InternalData.ProposedMerge.ZoneLow);
        CsDbg( BRLL_RULE, "BR_LLSC_6_11 : Dir: %d. InternalData.ProposedMerge.ZoneHigh:%d", dir, pData->InternalData.ProposedMerge.ZoneHigh);

        //   2.  Determine ...
        //           iii.  If the product is of type n-Hour; resolve the trip direction as follows:
        //               (1)  If the proposed merge trip direction(12)  is Unknown, set the proposed merge trip
        //                    direction(12)  to the product trip direction
        //               (2)  Else if the proposed merge trip direction(12)  is not equal to the product control bitmap trip
        //                    direction(2) , and the produ ct trip direction(2)  is not equal to Unknown, set the proposed
        //                    merge trip direction(12)  to Disabled
        //
        if ( myki_br_cd_GetProductType( pDirectory->ProductId ) == PRODUCT_TYPE_NHOUR )
        {
            myki_br_UpdateProposedMergeTripDirection( pData, myki_br_getProductTripDirection( pProduct ) );
        }
    }

    //   3.  Update the trip direction to include the current trip direction(14)
    //       a.  If the proposed merge trip direction(12)  is Unknown, and the current trip direction(14)  is not Unknown;
    //           then set the proposed merge trip direction(12)  to the product trip direction(14) .
    //       b.  Else if the proposed merge trip direction(12)  is not equal current trip direction(14)   and the current trip
    //           direction(14)  is not equal to Unknown then set the proposed merge trip direction(12)  to Disabled

    myki_br_UpdateProposedMergeTripDirection( pData, pData->DynamicData.currentTripDirection );

    //   4.  If there is no active e-Pass; then for each zone in the CurrentTrip from zone low(5)  to zone high(6)  check
    //       that we have existing product coverage from any product with a product status(13)  of active on the card
    //       excluding the provisional product (ie the provisional bit of product control bit map(2)  is not set)
    //
    if ( ! haveActiveEpass )
    {
        int                 zone;
        int                 ePassActivated = FALSE;

        CsDbg( BRLL_RULE, "BR_LLSC_6_11 : Active ePass NOT found, looking for other active products");

        for (zone = pData->DynamicData.currentTripZoneHigh; zone >= pData->DynamicData.currentTripZoneLow && !ePassActivated; zone--)
        {
            MYKI_TAProduct_t    *pProductActive = NULL;

            CsDbg( BRLL_RULE, "BR_LLSC_6_11 : Checking zone %d", zone );

            for ( dir = 1; dir < DIMOF( pMYKI_TAControl->Directory ); dir++ ) // T-Purse dir entry does not have a product slot, so there's one less product slots than dir entries
            {
                pDirectory = &pMYKI_TAControl->Directory[ dir ];              // Product slot 0-4 maps to dir entry 1-5

                /* exclude inactive product */
                if ( pDirectory->Status != TAPP_CONTROL_DIRECTORY_STATUS_ACTIVATED )
                {
                    continue;
                }

                if ( MYKI_CS_TAProductGet( (U8_t)( dir - 1 ), &pProduct ) >= 0 )
                {
                    /* exclude provisional products */
                    if ( pProduct->ControlBitmap & TAPP_TPRODUCT_CONTROL_PROVISIONAL_BITMAP )
                    {
                        continue;
                    }

                    if ( zone >= pProduct->ZoneLow && zone <= pProduct->ZoneHigh )
                    {
                        // an active product already covers this zone
                        pProductActive = pProduct;
                        break;
                    }
                }
            }

            //           a.  If there are no valid products on the card covering this zone and
            //           b.  a product of type e-Pass with the product status(13)  of inactive and the start date of the e-pass(20)  is
            //               less than or equal  the start time of the product in use exists where the product would provide
            //               coverage for this zone then:
            //
            if ( pProductActive == NULL )
            {
                CsDbg( BRLL_RULE, "BR_LLSC_6_11 : No active products found that cover zone %d", zone );
                MYKI_TAProduct_t    *pProductInActiveEPass = NULL;
//                for ( dir = 1; dir < DIMOF( pMYKI_TAControl->Directory ); dir++ )
                for ( dir = DIMOF( pMYKI_TAControl->Directory ) - 1; dir >= 1; dir-- )
                {
                    pDirectory = &pMYKI_TAControl->Directory[ dir ];

                    // Active the inactive epass that will cover this zone
                    if( ( pDirectory->Status == TAPP_CONTROL_DIRECTORY_STATUS_ISSUED )                  &&
                        ( myki_br_cd_GetProductType( pDirectory->ProductId ) == PRODUCT_TYPE_EPASS )    &&
                        ( MYKI_CS_TAProductGet( (U8_t)( dir - 1 ), &pProductInActiveEPass ) >= 0 )      &&
                        ( pProductInActiveEPass->StartDateTime <= pProductInUse->StartDateTime )        &&
                        ( zone >= pProductInActiveEPass->ZoneLow && zone <= pProductInActiveEPass->ZoneHigh ) )
                    {

                        //   Then ...
                        //   i.   Activate the Inactive e-Pass by performing an ProductUpdate/Activate transaction
                        //   ii.  Update the low zone of the combined zone(9)  range if the e-Pass zone Low(3)  is less than low
                        //        zone of the combined zone range(9)
                        //   iii. Update the high zone of the combined zone range(10)  if the e-Pass zone High(4)  is greater than
                        //        the high zone of the combined zone range(10)
                        //
                        CsDbg( BRLL_RULE, "BR_LLSC_6_11 : Inactive ePass found : Activating");
                        if ( myki_br_ldt_ProductUpdate_Activate( pData, pDirectory ) < 0 )
                        {
                            CsErrx( "BR_LLSC_6_11 : myki_br_ldt_ProductUpdate_Activate() failed" );
                            return RULE_RESULT_ERROR;
                        }

                        if ( pProductInActiveEPass->ZoneLow < pData->InternalData.ProposedMerge.ZoneLow )
                            pData->InternalData.ProposedMerge.ZoneLow = pProductInActiveEPass->ZoneLow;

                        if ( pProductInActiveEPass->ZoneHigh > pData->InternalData.ProposedMerge.ZoneHigh )
                            pData->InternalData.ProposedMerge.ZoneHigh = pProductInActiveEPass->ZoneHigh;

                        //   iv.  Modify usage log(23)
                        //       (a)  Definition:
                        //       (b)  Set Usage - Product validation status Activated, bit 0 = true
                        //
                        pData->InternalData.IsUsageLogUpdated = TRUE;
                        pData->InternalData.UsageLogData.isProductValidationStatusSet = TRUE;
                        pData->InternalData.UsageLogData.productValidationStatus |= TAPP_USAGE_LOG_PROD_VAL_STATUS_ACTIVATE_BITMAP;

                        ePassActivated = TRUE;
                        break;  // Only activate one inactive ePass (the first one) even if there's more than one on the card
                    }
                }
            }   // end activation of the inactive EPASS
        }   // end for zone from currentTripZoneLow to currentTripZoneHigh
    }

    //   5.  If the low zone of the current trip(5)  is less than the low zone of the combined zone range(9)  and the
    //       current trip border status of product control bit map(2)  is not set (ie the current trip is not a border trip)
    //       then set the low zone of the combined zone range(9)  to the low zone of the current trip(5)
    //
    //   6.  If the high zone of the current trip(6)  is greater than the high zone of the combined zone range(10)  and
    //       the current trip is not a border trip then set the high zone of the combined zone range(10)  to the high
    //       zone of the current trip(6)

    if ( pData->DynamicData.currentTripZoneLow < pData->InternalData.ProposedMerge.ZoneLow &&
         ! ( pProductInUse->ControlBitmap & TAPP_TPRODUCT_CONTROL_BORDER_STATUS_BITMAP ) )
    {
        pData->InternalData.ProposedMerge.ZoneLow = pData->DynamicData.currentTripZoneLow;
    }

    if ( pData->DynamicData.currentTripZoneHigh > pData->InternalData.ProposedMerge.ZoneHigh &&
         ! ( pProductInUse->ControlBitmap & TAPP_TPRODUCT_CONTROL_BORDER_STATUS_BITMAP ) )
    {
        pData->InternalData.ProposedMerge.ZoneHigh = pData->DynamicData.currentTripZoneHigh;
    }

    CsDbg( BRLL_RULE, "BR_LLSC_6_11 : 5 and 6. InternalData.ProposedMerge.ZoneLow: %d", pData->InternalData.ProposedMerge.ZoneLow);
    CsDbg( BRLL_RULE, "BR_LLSC_6_11 : 5 and 6. InternalData.ProposedMerge.ZoneHigh:%d", pData->InternalData.ProposedMerge.ZoneHigh);

    //
    //   7. If the tariff has valid tariff data for the current date.
    //
    //   Clarification from Brendon.
    //   The Valid Tariff Data test is based on the StartDate in the header of the CD XML
    //
    //  Implementation Note:
    //      The rule text does not cover the case where Tariff Data is invalid.
    //      I've treated the Valid Tarrif Data test as a part of the 'merge zone not equal to seed values' test
    //
    if (  (MYKI_CD_getStartDateTime() <= pData->DynamicData.currentDateTime) &&
          (pData->InternalData.ProposedMerge.ZoneHigh != ZONE_HIGH_NOT_SET)  &&
          (pData->InternalData.ProposedMerge.ZoneLow  != ZONE_LOW_NOT_SET) )
    {
        //      a. then
        //          i. If the proposed merge zone low(9)  and proposed merge zone high(10)  is not equal to the seed values (ie
        //             255,0 respectively ) then set combined zone range fare(11)  to the fare determined for the:
        //              i.   combined zone range(9) ,(10)
        //              ii.  passenger type
        //              iii. date of travel
        //              iv.  fare route id(17)
        //              v.   current trip direction
        //
        if
        (
            myki_br_getFareStoredValueEx
            (
                &pData->InternalData.ProposedMerge.Fare,
                pData,
                pData->InternalData.ProposedMerge.ZoneLow,
                pData->InternalData.ProposedMerge.ZoneHigh,
                pMYKI_TAControl->PassengerCode,
                pData->InternalData.ProposedMerge.TripDirection,
                pData->DynamicData.fareRouteIdIsValid,
                pData->DynamicData.fareRouteId,
				pProductInUse->StartDateTime,
				pData->DynamicData.currentDateTime
            ) < 0
        )
        {
            CsErrx("BR_LLSC_6_11 : myki_br_getFareStoredValueEx() failed");
            return RULE_RESULT_ERROR;
        }
        CsDbg( BRLL_RULE, "BR_LLSC_6_11 : InternalData.ProposedMerge.Fare: %d", pData->InternalData.ProposedMerge.Fare);
    }
    else
    {
        //
        //      b. Else set the combinded zone range fare(11) to the provisional product purchase value.
        //
        pData->InternalData.ProposedMerge.Fare =  pProductInUse->PurchaseValue;
        CsDbg( BRLL_RULE, "BR_LLSC_6_11 : InternalData.ProposedMerge.Fare: %d", pData->InternalData.ProposedMerge.Fare);
    }

    //
    //   8.  Set the combined zone range off peak fare(19)  to the combined zone range fare(11)  multiplied by 1 less
    //       the off peak discount rate(18) .
    //
    CsDbg( BRLL_RULE, "BR_LLSC_6_11 : DynamicData.offPeakDiscountRate: %d", pData->DynamicData.offPeakDiscountRate);
    pData->InternalData.ProposedMerge.OffPeakFare = CalculateDiscountedFare( pData->InternalData.ProposedMerge.Fare, pData->DynamicData.offPeakDiscountRate );
    CsDbg( BRLL_RULE, "BR_LLSC_6_11 : InternalData.ProposedMerge.OffPeakFare: %d", pData->InternalData.ProposedMerge.OffPeakFare);

    //
    //   9. Set the common date(24) to the start date of the product in use.
    //
    CsDbg( BRLL_RULE, "BR_LLSC_6_11 : Setting CommonDate: %d",  pProductInUse->StartDateTime);
    pData->DynamicData.commonDateTime = pProductInUse->StartDateTime;

    CsDbg( BRLL_RULE, "BR_LLSC_6_11 : EXECUTED" );
    return RULE_RESULT_EXECUTED;
}

