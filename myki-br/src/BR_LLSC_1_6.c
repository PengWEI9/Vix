/* -*- mode: c; tabs: 4; -*- */
/*=============================================================================
**
**    Vix Technology                   Licensed software
**    (C) 2013                         All rights reserved
**
**=============================================================================
**
**  Project/Product : MBU
**  Filename        : BR_LLSC_1_6.c
**  Author(s)       : An Tran
**
**  ID              : BR_LLSC_1_6
**
**  Name            : Validate Daily Cap
**
**  Data Fields     :
**
**      1.  TAppControl.ProductInUse
**      2.  TAppControl.Directory.ProductId
**      3.  TAppControl.PassengerCode
**      4.  TAppTProduct.ZoneLow
**      5.  TAppTProduct.ZoneHigh
**      6.  TAppTProduct.EndDateTime
**      7.  TAppTProduct.PurchaseValue
**      8.  TAppCapping.Daily.Value
**      9.  TAppCapping.Daily.ZoneLow
**      10. TAppCapping.Daily.ZoneHigh
**      11. TAppCapping.Weekly.Value
**      12. Dynamic.CurrentDateTime
**      13. Tariff.EndOfBusinessDay
**      14. Tariff.DailyCapValue
**      15. Tariff.EntitlementZoneLow
**      16. Tariff.EntitlementZoneHigh
**      17. Tariff.EntitlementProduct
**      18. Tariff.BaseFare
**      19. Tariff.ApplicableFare
**      20. Tariff.CitySaverZoneFlag
**      21. TAppTProduct.ControlBitmap
**      22. Dynamic.ApplicableProductInUse
**      23. Dynamic.CurrentTripZoneLow
**      24. Dynamic.CurrentTripZoneHigh
**      25. Tarrif.EntitlementValue
**      26. Dynamic.IsForcedScanOff
**      27. Dynamic.ForcedScanOffDate
**      28. Dynamic.FareRouteID
**      29. Dynamic.Commondate
**
**  Pre-Conditions  :
**
**      1.  If this is a forced scan off sequence (ie IsForcedScanOff is true)
**          then for this business rule the date for calculations to be used is
**          the Forced Scan Off Date else use the  common date if set else the
**          current date time this will be referred to as the common date
**      2.  Determine the daily capping fares total and the applicable fare for the:
**          a.  Daily capping zone range (low to high daily capping zones).
**          b.  Passenger type
**          c.  The determined fare route
**          d.  The common date
**      3.  If either of the following are true :
**          a.  The daily capping fares total is greater than or equal to the daily
**              cap and no product of type Daily exits where the product:
**              i.  low zone is less than or equal to the daily capping low zone
**              ii. high zone is greater than or equal to daily capping high zone
**          b.  if  an entitlement product exists and daily capping fares total is
**              greater than or equal to the determined entitlement product value
**              and no product of the entitlement product type exists where the
**              entitlement product:
**              i.  low zone is less than or equal to the daily capping zone low
**              ii. high zone is greater than or equal to the daily capping zone high
**
**  Description     :
**
**      1.  If ProductInUse is not 0 and if the ProductInUse is either an n-Hour
**          where the product control bitmap on the product indicates that the
**          border active bit is not set or the product if of type Single trip and
**          the product expiry is less than or equal to the of business day for the
**          common date
**      2.
**          a.
**              i.  Perform a ProductSale/Upgrade for the ProductInUse set Expiry
**                  to the end of the Business Day for the common date Entitlement
**                  Product exists set the Product ID to the Entitlement Product
**                  ID else to the product ID to Daily
**              ii. If Entitlement zone low is applicable set the zone Low to the
**                  Entitlement zone Low else to the DailyCapping zone Low.
**              iii.If Entitlement zone high is applicable set the zone High to the
**                  Entitlement zone High else to the DailyCapping zone High.
**              iv. Adjust the zone Range to include City Saver zones if applicable.
**              v.  If Entitlement product exists set the PurchaseValue to the
**                  entitlement value else to the Daily fare.
**          b.  Perform a ProductUpdate/None on the product in use
**              i.  Set BorderStaus bit of the product control bitmap to False.
**              ii. Set the off-peak bit of the product control bitmap to false
**              iii.Set the trip direction on the product control bit map to status
**                  to Unknown
**      3.  else;
**          a.   Perform a ProductSale/None transaction
**              i.  Set Expiry to the end of the Business Day for the common date.
**                  If Entitlement Product exist set the ProductId to the
**                  Entitlement ProductId else to Daily
**              ii. If Entitlement zone low is applicable set the zone Low to the
**                  Entitlement zone Low else to the DailyCapping zone Low.
**              iii.If Entitlement zone high is applicable set the zone High to
**                  the Entitlement zone High else to the DailyCapping zone High.
**              iv. Adjust the zone Range to include City Saver zones if applicable.
**              v.  If Entitlement product exists set the PurchaseValue to the
**                  determined fare else to the Daily fare.
**          b.  Perform a ProductUpdate/Activate transaction
**              i.  Set the serial number to that of the newly created product.
**          c.  Perform a TAppUpdate/SetProductInUse to set this product as the product in use.
**      4.  If an Entitlement product exists and if the Daily Value is greater
**          than or equal to the determined fare OR if the Daily Value is greater
**          than or equal to the Daily Fare.
**          a.  Perform a capping update / none transaction
**              i.  Reduce the daily capping fares total by the difference
**              ii. Reduce the weekly capping fares total by the difference,
**          b.  Reduce the applicable fare by the difference.
**
**  Post-Conditions :
**
**      1.  The product in use is upgraded to a Daily product (or a daily
**          entitlement product), with the applicable capping zone range or a
**          new Daily product is created.
**
**  Member(s)       :
**      BR_LLSC_1_6             [public]    business rule
**
**  Information     :
**   Compiler(s)    : C
**   Target(s)      : Independent
**
**  Subversion      :
**      $Id: $
**      $HeadURL: $
**
**  History         :
**   Vers.  Date        Aut.  Type     Description
**   -----  ----------  ----  -------  ----------------------------------------
**    1.00  18.09.13    ANT   Create
**    1.01  23.10.13    ANT   Modify   myki_br_ldt_ProductUpdate_ActivateEx()
**                                     signature changed
**    1.02  11.11.13    ANT   Modify   myki_br_ldt_ProductSaleEx signature
**                                     changed
**    1.03  19.11.13    ANT   Modify   Creates local copy of TAppCapping
**    1.04  31.01.14    ANT   Modify   Replaced Tariff.applicableFare with
**                                     ReturnedData.applicableFare
**    1.05  11.02.14    ANT   Modify   Clears TAppProduct.ControlBitmap.Provisional
**                                     bit when performing ProductUpdate/None
**    1.06  28.02.14    ANT   Modify   Added configurable ACS compatible mode
**    1.07  25.07.14    ANT   Modify   Removed ACS compatible mode for MBU-601.
**    1.08  28.08.14    ANT   Modify   Tariff.ApplicableFare can be less than
**                                     zero. NOTE: Negative Tariff.ApplicableFare
**                                     indicates refund to be performed
**    1.09  18.09.14    ANT   Modify   MBU-1208: Added checking entitlement
**                                     product value against daily capping
**                                     prior performing product sale
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
**  BR_LLSC_1_6
**
**  Description     :
**      Implements business rule BR_LLSC_1_6.
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
**      1.  The maximum value for the daily capping limit on a card is
**          0xFFFFFFF, as while it's an unsigned 32-bit value on the card,
**          it's a signed 32-bit value everywhere else, including in BR.
**
**==========================================================================*/

RuleResult_e BR_LLSC_1_6( MYKI_BR_ContextData_t *pData )
{
    MYKI_TAControl_t       *pMYKI_TAControl         = NULL;
    MYKI_TACapping_t       *pMYKI_TACapping         = NULL;
    MYKI_Directory_t       *pMYKI_Directory         = NULL;
    MYKI_TAProduct_t       *pMYKI_TAProduct         = NULL;
    int                     i                       = 0;
    Time_t                  CommonDateTime;
    ProductType_e           ProductType             = PRODUCT_TYPE_UNKNOWN;
    int                     ProductId               = 0;
    int                     ZoneLow                 = 0;
    int                     ZoneHigh                = 0;
    int                     PurchaseValue           = 0;
    Time_t                  EndOfBusinessDateTime   = 0;
    Currency_t              FareDiff                = 0;
    int                     foundDaily              = FALSE;
    int                     foundEntitlement        = FALSE;
    MYKI_TAProduct_t        UpdatedProduct;
    ProductSale_t           request;

    CsDbg( BRLL_RULE, "BR_LLSC_1_6 : Start (Validate Daily Cap)" );

    if ( pData == NULL )
    {
        CsErrx( "BR_LLSC_1_6 : Invalid argument(s)" );
        return RULE_RESULT_ERROR;
    }

    if ( MYKI_CS_TAControlGet( &pMYKI_TAControl ) < 0 )
    {
        CsErrx( "BR_LLSC_1_6 : MYKI_CS_TAControlGet() failed" );
        return RULE_RESULT_ERROR;
    }

    if ( MYKI_CS_TACappingGet( &pMYKI_TACapping ) < 0 )
    {
        CsErrx( "BR_LLSC_1_6 : MYKI_CS_TACappingGet() failed" );
        return RULE_RESULT_ERROR;
    }

    //  Check that the daily capping value is in the range of an S32 (ie Currency_t)

    if ( pMYKI_TACapping->Daily.Value > 0x7FFFFFFF )
    {
        CsErrx( "BR_LLSC_1_6 : Daily Capping Value (%u) on card too large, must be between 0 and 0x7FFFFFFF", pMYKI_TACapping->Daily.Value );
        return RULE_RESULT_ERROR;
    }

    //---------------------------------
    //  Pre-conditions
    //---------------------------------

    //  1.  If this is a forced scan off sequence (ie IsForcedScanOff is true)
    //      then for this business rule the date for calculations to be used is
    //      the Forced Scan Off Date else use the  common date if set else the
    //      current date time this will be referred to as the common date
    //
    CommonDateTime = myki_br_getCommonDate(pData);

    /*  2.  Determine the daily capping fares total and the applicable fare for the:
            a.  Daily capping zone range (low to high daily capping zones).
            b.  Passenger type
            c.  The determined fare route
            d.  the common date */
    if ( myki_br_getFareStoredValue( pData,
            pMYKI_TACapping->Daily.Zone.Low, pMYKI_TACapping->Daily.Zone.High,
            pMYKI_TAControl->PassengerCode,
            pData->DynamicData.currentTripDirection,
            pData->DynamicData.fareRouteIdIsValid, pData->DynamicData.fareRouteId,
            CommonDateTime,
            pData->DynamicData.currentDateTime ) < 0 )
    {
        CsErrx( "BR_LLSC_1_6 : myki_br_getDailyFareStoredValue() failed" );
        return RULE_RESULT_ERROR;
    }   /* end-of-if */

    /*  3.  If either of the following are true : */

    //  Paraphrase :
    //  - If
    //      - the daily capping value is less than the daily cap, and
    //      - either there is no entitlement product, or the daily capping value is less than the entitlement product value
    //  - Then BYPASS because neither 3.a nor 3.b can be true.

    CsDbg
    (
        BRLL_RULE,
        "BR_LLSC_1_6 : TACapping.Daily.Value = %d, Tariff.DailyCapValue = %d, Tariff.EntitlementProduct = %d, Tariff.EntitlementValue = %d",
        pMYKI_TACapping->Daily.Value,
        pData->Tariff.dailyCapValue,
        pData->Tariff.entitlementProduct,
        pData->Tariff.entitlementValue
    );

    if
    (
        (Currency_t)pMYKI_TACapping->Daily.Value <  pData->Tariff.dailyCapValue &&
        (
            pData->Tariff.entitlementProduct == 0 ||
            (Currency_t)pMYKI_TACapping->Daily.Value <  pData->Tariff.entitlementValue      // We've checked the S32 range above, so just cast it again.
        )
    )
    {
        CsDbg( BRLL_RULE,
            pData->Tariff.entitlementProduct == 0 ?
                "BR_LLSC_1_6 : BYPASSED - TAppCapping.Daily.Value(%d) less than Tariff.DailyCapValue(%d)" :
                "BR_LLSC_1_6 : BYPASSED - TAppCapping.Daily.Value(%d) less than Tariff.DailyCapValue(%d) and Tariff.EntitlementValue(%d)",
            pMYKI_TACapping->Daily.Value,
            pData->Tariff.dailyCapValue,
            pData->Tariff.entitlementValue );
        return RULE_RESULT_BYPASSED;
    }   /* end-of-if */

    for ( i = 1; i < DIMOF( pMYKI_TAControl->Directory ); i++ )
    {
        /* ASSUMED! BR specification does not mention product has to be activated */
        if ( pMYKI_TAControl->Directory[ i ].Status != TAPP_CONTROL_DIRECTORY_STATUS_ACTIVATED )
        {
            continue;   /* for-loop */
        }   /* end-of-if */

        if ( myki_br_GetCardProduct( i, &pMYKI_Directory, &pMYKI_TAProduct ) < 0 )
        {
            CsErrx( "BR_LLSC_1_6 : myki_br_GetCardProduct( %d ) failed", i );
            return RULE_RESULT_ERROR;
        }   /* end-of-for */

        /*  a.  The daily capping fares total is greater than or equal to the daily cap */
        if ( (Currency_t)pMYKI_TACapping->Daily.Value >= pData->Tariff.dailyCapValue )
        {
            ProductType = myki_br_cd_GetProductType( pMYKI_Directory->ProductId );

            /*  and no product of type Daily exits where the product:
                i.  low zone is less than or equal to the daily capping low zone
                ii. high zone is greater than or equal to daily capping high zone */
            if
            (
                ProductType               == PRODUCT_TYPE_DAILY               &&
                pMYKI_TAProduct->ZoneLow  <= pMYKI_TACapping->Daily.Zone.Low  &&
                pMYKI_TAProduct->ZoneHigh >= pMYKI_TACapping->Daily.Zone.High
            )
            {
                foundDaily          = TRUE;
            }   /* end-of-if */
        }   /* end-of-if */

        /*  b.  if an entitlement product exists and daily capping fares total
                is greater than or equal to the determined entitlement product value */
        if ( pData->Tariff.entitlementProduct >  0                              &&
             (Currency_t)pMYKI_TACapping->Daily.Value     >= pData->Tariff.entitlementValue )
        {
            /*  and no product of the entitlement product type exists where the entitlement product:
                i.  low zone is less than or equal to the daily capping zone low
                ii. high zone is greater than or equal to the daily capping zone high */
            if
            (
                pMYKI_Directory->ProductId == pData->Tariff.entitlementProduct &&
                pMYKI_TAProduct->ZoneLow   <= pMYKI_TACapping->Daily.Zone.Low  &&
                pMYKI_TAProduct->ZoneHigh  >= pMYKI_TACapping->Daily.Zone.High
            )
            {
                foundEntitlement    = TRUE;
            }   /* end-of-if */
        }   /* end-of-if */
    }   /* end-of-for */

    if ( foundDaily != FALSE ||
       ( pData->Tariff.entitlementProduct != 0 && foundEntitlement != FALSE ) )
    {
        CsDbg( BRLL_RULE,
            foundDaily != FALSE ?
                "BR_LLSC_1_6 : BYPASSED - found Daily product covers capping zone(s)" :
                "BR_LLSC_1_6 : BYPASSED - found Entitlement product covers capping zone(s)" );
        return RULE_RESULT_BYPASSED;
    }   /* end-of-if */

    //---------------------------------
    //  Processing
    //---------------------------------

    //
    //  Prepare for the over-complicated if statement below, by setting everything we might need for it
    //

    //  Set Expiry to the end of the Business Day for the common date
    EndOfBusinessDateTime   = myki_br_EndOfBusinessDateTime( pData, CommonDateTime, FALSE );

    //  Set the product in use details if there is a product in use
    if ( pMYKI_TAControl->ProductInUse > 0 )
    {
        if ( myki_br_GetCardProduct( pMYKI_TAControl->ProductInUse, &pMYKI_Directory, &pMYKI_TAProduct ) < 0 )
        {
            CsErrx( "BR_LLSC_1_6 : myki_br_GetCardProduct( %d ) failed", pMYKI_TAControl->ProductInUse );
            return RULE_RESULT_ERROR;
        }   /* end-of-for */

        ProductType = myki_br_cd_GetProductType( pMYKI_Directory->ProductId );
    }

    //  1.  If
    //          ProductInUse is not 0, and
    //              the ProductInUse is either an n-Hour where the product control bitmap on the product indicates that the border active bit is not set, or
    //              the product if(s) of type Single trip, and
    //          the product expiry is less than or equal to the of business day for the common date
    if
    (
        ( pMYKI_TAControl->ProductInUse > 0 ) &&
        (
            ( ProductType == PRODUCT_TYPE_NHOUR && ( pMYKI_TAProduct->ControlBitmap & TAPP_TPRODUCT_CONTROL_BORDER_STATUS_BITMAP ) == 0 ) ||
            ( ProductType == PRODUCT_TYPE_SINGLE )
        ) &&
        ( pMYKI_TAProduct->EndDateTime <= EndOfBusinessDateTime )
    )
    {
        {
            CsDbg( BRLL_RULE, "BR_LLSC_1_6 : Upgrade the product in use to a Daily" );

            /*  a.  Perform a ProductSale/Upgrade for the ProductInUse */
            /*          set Expiry to the end of the Business Day for the common date */
            /*      i.  If Entitlement Product exists set the Product ID to the Entitlement
                        Product ID else to the product ID to Daily
                    ii. If Entitlement zone low is applicable set the zone Low to the
                        Entitlement zone Low else to the DailyCapping zone Low.
                    iii.If Entitlement zone high is applicable set the zone High to the
                        Entitlement zone High else to the DailyCapping zone High.
                    iv. Adjust the zone Range to include City Saver zones if applicable.
                    v.  If Entitlement product exists set the PurchaseValue to the
                        entitlement value else to the Daily fare. */
            // BEGIN KIM MOD
            if (
                pData->Tariff.entitlementProduct > 0 &&
                
                /*
                 * NOTE: We should only apply the entitlement product in the case that the
                 *       entitlement cap has been reached.
                 */
                ((Currency_t)pMYKI_TACapping->Daily.Value >=  pData->Tariff.entitlementValue)
                )
            {
                ProductId       = pData->Tariff.entitlementProduct;
                ZoneLow         = pData->Tariff.entitlementZoneLow;
                ZoneHigh        = pData->Tariff.entitlementZoneHigh;
                PurchaseValue   = pData->Tariff.entitlementValue;
            }
            else
            {
                ProductId       = myki_br_cd_GetProductId( PRODUCT_TYPE_DAILY );
                ZoneLow         = pMYKI_TACapping->Daily.Zone.Low;
                ZoneHigh        = pMYKI_TACapping->Daily.Zone.High;
                PurchaseValue   = pData->Tariff.dailyCapValue;
            }   /* end-of-if */
            // END KIM MOD

            ADJUST_FOR_CITYSAVER( ZoneLow, ZoneHigh );

            if
            (
                myki_br_ldt_ProductSale_UpgradeEx
                (
                    pData,
                    pMYKI_Directory,
                    ProductId,
                    ZoneLow,
                    ZoneHigh,
                    PurchaseValue,
                    EndOfBusinessDateTime,
                    pData->StaticData.AcsCompatibilityMode ? FALSE : TRUE   // Kamco : DO NOT clear TAppProduct.ControlBitmap.Provisional bit
                ) < 0
            )
            {
                CsErrx( "BR_LLSC_1_6 : myki_br_ldt_ProductSale_Upgrade() failed" );
                return RULE_RESULT_ERROR;
            }   /* end-of-if */

            /*  b.  Perform a ProductUpdate/None on the product in use
                    i.  Set BorderStaus bit of the product control bitmap to False.
                    ii. Set the off-peak bit of the product control bitmap to false
                    iii.Set the trip direction on the product control bit map to status to(f) Unknown */

            UpdatedProduct = *pMYKI_TAProduct;     //  First, create a copy of the product so we can modify it

            if ( pData->StaticData.AcsCompatibilityMode )
            {
                /*  NOTE:   KAMCO reader clears TAppProduct.ControlBitmap.Provisional bit
                            when performing ProductUpdate/None */
                myki_br_setProductProvisionalStatus( &UpdatedProduct, FALSE );
            }

            myki_br_setProductBorderStatus ( &UpdatedProduct, FALSE );
            myki_br_setProductOffPeakStatus( &UpdatedProduct, FALSE );

            #if     __ACS_RECTIFIED_EXCLUDED__
            if ( pData->StaticData.AcsCompatibilityMode != FALSE )
            {
                // MBU-601  If we set the direction, we fail some of the tests because the ACS software
                //          does not set the direction. Keane have verified that it's an ACS issue -
                //          our implementation is correct. However, for now we need to duplicate the
                //          incorrect ACS behaviour. At some point in the future we may move to the correct
                //          behaviour at which time the if, else, CsDbg, and endif lines should be removed,
                //          leaving only the myki_br_setProductTripDirection line.
                CsDbg( BRLL_RULE, "BR_LLSC_1_6 : MBU-601 - Not setting Trip Direction! This is so we match the incorrect ACS behaviour" );
            }
            else
            #endif  /*  __ACS_RECTIFIED_EXCLUDED__ */
            {
                myki_br_setProductTripDirection( &UpdatedProduct, TRIP_DIRECTION_UNKNOWN );
            }   /* end-of-if */

            if ( myki_br_ldt_ProductUpdate( pData, pMYKI_Directory, pMYKI_TAProduct, &UpdatedProduct ) < 0 )
            {
                CsErrx( "BR_LLSC_1_6 : myki_br_ldt_ProductUpdate() failed" );
                return RULE_RESULT_ERROR;
            }   /* end-of-if */
        }
    }
    else    /*  3.  else; */
    {
        CsDbg( BRLL_RULE, "BR_LLSC_1_6 : Create a new Daily product and make it the product in use" );

        /*  a.  Perform a ProductSale/None transaction
                i.  Set Expiry to the end of the Business Day for the common date. */
        /*          If Entitlement Product exist set the ProductId to the Entitlement ProductId else to Daily
                ii. If Entitlement zone low is applicable set the zone Low to the Entitlement zone Low else to the DailyCapping zone Low.
                iii.If Entitlement zone high is applicable set the zone High to the Entitlement zone High else to the DailyCapping zone High.
                iv. Adjust the zone Range to include City Saver zones if applicable.
                v.  If Entitlement product exists set the PurchaseValue to the determined fare else to the Daily fare. */
        /*  NOTE:   Only sale entitlement product if its product value is less than or equal daily capping value */
        if ( pData->Tariff.entitlementProduct >  0 &&
             pData->Tariff.entitlementValue   <= pData->Tariff.dailyCapValue )
        {
            ProductId       = pData->Tariff.entitlementProduct;
            ZoneLow         = pData->Tariff.entitlementZoneLow;
            ZoneHigh        = pData->Tariff.entitlementZoneHigh;
            PurchaseValue   = pData->Tariff.entitlementValue;
        }
        else
        {
            ProductId       = myki_br_cd_GetProductId( PRODUCT_TYPE_DAILY );
            ZoneLow         = pMYKI_TACapping->Daily.Zone.Low;
            ZoneHigh        = pMYKI_TACapping->Daily.Zone.High;
            PurchaseValue   = pData->Tariff.dailyCapValue;
        }   /* end-of-if */

        ADJUST_FOR_CITYSAVER( ZoneLow, ZoneHigh );

        memset( &request, 0, sizeof( request ) );
        request.productId               = ProductId;
        request.purchaseValue           = PurchaseValue;
        request.zoneLow                 = ZoneLow;
        request.zoneHigh                = ZoneHigh;
        request.isEndDateTimeSet        = TRUE;
        request.endDateTime             = EndOfBusinessDateTime;

        if ( ( i = myki_br_ldt_ProductSaleEx( pData, &request ) ) < 0 )
        {
            CsErrx( "BR_LLSC_1_6 : myki_br_ldt_ProductSaleEx() failed" );
            return RULE_RESULT_ERROR;
        }   /* end-of-if */

        /*  b.  Perform a ProductUpdate/Activate transaction
                i.  Set the serial number to that of the newly created product. */
        pMYKI_Directory     = &pMYKI_TAControl->Directory[ i ];
        if ( myki_br_ldt_ProductUpdate_ActivateEx( pData, pMYKI_Directory,
                    pData->DynamicData.currentDateTime, EndOfBusinessDateTime ) < 0 )
        {
            CsErrx( "BR_LLSC_1_6 : myki_br_ldt_ProductUpdate_Activate() failed" );
            return RULE_RESULT_ERROR;
        }   /* end-of-if */

        /*  c.  Perform a TAppUpdate/SetProductInUse to set this product as the product in use. */
        if ( myki_br_ldt_AppUpdate_SetProductInUse( pData, pMYKI_Directory ) < 0 )
        {
            CsErrx( "BR_LLSC_1_6 : myki_br_ldt_AppUpdate_SetProductInUse() failed" );
            return RULE_RESULT_ERROR;
        }   /* end-of-if */
    }

    /*  4.  If an Entitlement product exists and if the Daily Value is greater than or equal to the determined fare OR */
    if ( pData->Tariff.entitlementProduct >  0 &&
         (Currency_t)pMYKI_TACapping->Daily.Value >= pData->Tariff.entitlementValue )
    {
        FareDiff    = ( pMYKI_TACapping->Daily.Value - pData->Tariff.entitlementValue );
    }
    else
    /*      if the Daily Value is greater than or equal to the Daily Fare. */
    if ( (Currency_t)pMYKI_TACapping->Daily.Value >= pData->Tariff.dailyCapValue )
    {
        FareDiff    = ( pMYKI_TACapping->Daily.Value - pData->Tariff.dailyCapValue );
    }   /* end-of-if */

    if ( FareDiff > 0 )
    {
        MYKI_TACapping_t    MYKI_TACapping_New;

        /*  NOTE:   Making changes to local copy of TAppCapping */
        memcpy( &MYKI_TACapping_New, pMYKI_TACapping, sizeof( MYKI_TACapping_New ) );
        pMYKI_TACapping = &MYKI_TACapping_New;

        /*  a.  Perform a capping update / none transaction
                i.  Reduce the daily capping fares total by the difference
                ii. Reduce the weekly capping fares total by the difference, */
        pMYKI_TACapping->Daily.Value    = \
                ( (Currency_t)pMYKI_TACapping->Daily.Value > FareDiff ) ?
                        ( pMYKI_TACapping->Daily.Value - FareDiff ) :
                        ( 0 );
        pMYKI_TACapping->Weekly.Value   = \
                ( (Currency_t)pMYKI_TACapping->Weekly.Value > FareDiff ) ?
                        ( pMYKI_TACapping->Weekly.Value - FareDiff ) :
                        ( 0 );
        if ( myki_br_ldt_PerformCappingUpdate( pData, pMYKI_TACapping, TRUE, TRUE ) < 0 )
        {
            CsErrx( "BR_LLSC_1_6 : myki_br_ldt_PerformCappingUpdate() failed" );
            return RULE_RESULT_ERROR;
        }   /* end-of-if */

        /*  NOTE:   pData->ReturnedData.applicableFare is Tariff.ApplicableFare. */
        /*  b.  Reduce the applicable fare by the difference. */
        CsDbg( BRLL_FIELD, "BR_LLSC_1_6 : Reducing applicable fare by %d from %d to %d", FareDiff, pData->ReturnedData.applicableFare, pData->ReturnedData.applicableFare - FareDiff );
        pData->ReturnedData.applicableFare -= FareDiff;
    }

    CsDbg( BRLL_RULE, "BR_LLSC_1_6 : Executed" );
    return RULE_RESULT_EXECUTED;
}   /* BR_LLSC_1_6( ) */

