/* -*- mode: c; tabs: 4; -*- */
/*=============================================================================
**
**    Vix Technology                   Licensed software
**    (C) 2013                         All rights reserved
**
**=============================================================================
**
**  Project/Product : MBU
**  Filename        : BR_LLSC_1_5.c
**  Author(s)       : An Tran
**
**  ID              : BR_LLSC_1_5 - KA0004 v7.0
**
**  Name            : Validate Daily Cap Current Trip
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
**      9.  Dynamic.CurrentDateTime
**      10. Dynamic.CurrentTripZoneLow
**      11. Dynamic.CurrentTripZoneHigh
**      12. Tariff.EndOfBusinessDay
**      13. Tariff.DailyCapValue
**      14. Tariff.EntitlementZoneLow
**      15. Tariff.EntitlementZoneHigh
**      16. Tariff.EntitlementProduct
**      17. Tariff.CitySaverZoneFlag
**      18. Dynamic.PreviousDailyCapZoneLow
**      19. Dynamic.PreviousDailyCapZoneHigh
**      20. Dynamic.ApplicableFare
**      21. TAppCapping.Weekly.Value
**      22. Tariff.EntitlementValue
**      23. Dynamic.IsForceScanOff
**      24. Dynamic.ForceScanDate
**      25. Dynamic.FareRouteID
**      26. Dynamic.CommonDate
**
**  Pre-Conditions  :
**
**      1.  If this is a forced scan off sequence (ie IsForcedScanOff is true)
**          then for this business rule the date for calculations to be used is
**          the Forced Scan Off Date else use the  common date if set otherwise
**          use the current date time this will be referred to as the common date.
**      2.  The current trip zone range must be fully covered by the capping
**          counters prior to the current trip.
**          a.  If the low zone of the current trip is greater than or equal to
**              PreviousDailyCapZoneLow and less than or equal to the
**              PreviousDailyCapZoneHigh AND If the high zone of the current
**              trip is greater than or equal to PreviousDailyCapZoneLow and less
**              than or equal to the PreviousDailyCapZoneHigh
**      3.  Determine the daily capping fares total and the applicable fare for the:
**          a.  Zone range of the current trip (low to high zones of the current trip).
**          b.  Passenger type.
**          c.  The determined fare route
**          d.  The common date.
**      4.  Either of the following conditions is true:
**          a.  The daily capping fares total is greater than or equal to the daily
**              cap and no product of type Daily exists where the product:
**              i.  low zone is less than or equal to the current trip low zone
**              ii. high zone is greater than or equal to the current trip high zone
**          b.  If an entitlement product exists and daily capping fares total is
**              greater than or equal to the determined entitlement product value and
**              no product the entitlement product type exits where the entitlement
**              product:
**              i.  low zone is less than or equal to the current trip low zone
**              ii. high zone is greater than or equal to the current trip high zone
**          5.  This is a forced scan off sequence (ie IsForcedScanOff is true)
**              and it is the same business day as the force scan off event
**              (ie Force Scan date = current business day) or this is not a force
**              scan off sequence (ie isForceScanOff = false)
**
**  Description     :
**
**      1.  If either of the following 2 scenarios is true.
**          a.  If all of the the following points are true:
**              i.  the ProductInUse is not zero
**              ii. if the product in use is an n-hour
**              iii.the low zone of the current trip is equal to the low zone
**                  of the product
**              iv. the high zone of the current trip is equal to the high
**                  zone of the product
**              v.  the product control bitmap on the product in use indicates
**                  that the border active bit is not set
**              vi. the product expiry of the product in use is less than or
**                  equal to the end of business day for the common date
**          b.  Or, If all of the following are true:
**              i.  The product in use is of type Single Trip
**                  the Product in use1 zone low is between the current trip
**                  zone low (when adjusted for city saver zone where applicable)
**              ii. and current trip zone high (when adjusted for city saver
**                  zone where applicable)
**              iii.Perform ProductSale/Upgrade on the ProductInUse
**          c.  Then do:
**              i.
**                  (1) If an Entitlement product is applicable set the product
**                      ID to the Entitlement product ID else to Daily.
**                  (2) If Entitlement zone Low is applicable set the zone low
**                      to the Entitlement zone Low else to zone Low of the
**                      Current trip.
**                  (3) If Entitlement zone High exists set the zone High to the
**                      Entitlement zone High else to the zone High of the current
**                      trip.
**                  (4) Adjust the zone Range to include City Saver zones if
**                      applicable
**                  (5) If Entitlement product is applicable set the Purchase value
**                      to the entitlement fare else to Daily Fare.
**                  (6) Set the product expiry to the end of Business day of the
**                      common date.
**              ii. Perform a ProductUpdate/None request
**                  (1) Set the BorderStatus bit of the product control bitmap to False
**                  (2) Set the Trip Direction on the product control bitmap to Unknown.
**                  (3) Set the off-peak bit of the product control bitmap to false
**      2.  else
**          a.  Perform ProductSale/None transaction
**              i.  If Entitlement product is applicable set the productId to the
**                  Entitlement productId else to Daily.
**              ii. If Entitlement zone Low is applicable set the zone low to the
**                  Entitlement zone Low else to  zone Low of the Current trip
**              iii.If Entitlement zone High exists set the zone High to the Entitlement
**                  zone High else to the zone High of the current trip.
**              iv. Adjust the zone Range to include City Saver zones if applicable
**              v.  If Entitlement product is applicable set the Purchase value to
**                  the determined fare else to Daily Fare.
**              vi. Set the product expiry to the end of Business day of the common date.
**          b.  Perform ProductUpdate/Activate transaction
**              i.  Set the serial number to the newly created product
**          c.  Perform a TAppUpdate/SetProductInUse setting this Product as the product
**              in use.
**
**  Post-Conditions :
**
**  Member(s)       :
**      BR_LLSC_1_5             [public]    business rule
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
**    1.00  02.10.13    ANT   Create
**    1.01  23.10.13    ANT   Modify   myki_br_ldt_ProductUpdate_ActivateEx()
**                                     signature changed
**    1.02  11.11.13    ANT   Modify   myki_br_ldt_ProductSaleEx signature
**                                     changed
**    1.03  11.02.14    ANT   Modify   Clears TAppProduct.ControlBitmap.Provisional
**                                     bit when performing ProductUpdate/None
**    1.04  18.09.14    ANT   Modify   MBU-1208: Added checking entitlement
**                                     product value against daily capping
**                                     prior performing product sale
**
**===========================================================================*/

#include <cs.h>                 // For debug logging
#include <myki_cardservices.h>  // Myki Card Services
#include <LDT.h>                // Logical Device Transaction

#include "myki_br_rules.h"      // API for all BR_xxx modules
#include "BR_Common.h"          // Common utilities, macros, etc

/*==========================================================================*
**
**  BR_LLSC_1_5
**
**  Description     :
**      Implements business rule BR_LLSC_1_5.
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
**==========================================================================*/

RuleResult_e BR_LLSC_1_5( MYKI_BR_ContextData_t *pData )
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
    int                     foundDaily              = FALSE;
    int                     foundEntitlement        = FALSE;
    MYKI_TAProduct_t        UpdatedProduct;
    ProductSale_t           request;

    CsDbg( BRLL_RULE, "BR_LLSC_1_5 : Start (Validate Daily Cap Current Trip)" );

    if ( pData == NULL )
    {
        CsErrx( "BR_LLSC_1_5 : Invalid argument(s)" );
        return RULE_RESULT_ERROR;
    }   /* end-of-if */

    if ( MYKI_CS_TAControlGet( &pMYKI_TAControl ) < 0 )
    {
        CsErrx( "BR_LLSC_1_5 : MYKI_CS_TAControlGet() failed" );
        return RULE_RESULT_ERROR;
    }   /* end-of-if */

    if ( MYKI_CS_TACappingGet( &pMYKI_TACapping ) < 0 )
    {
        CsErrx( "BR_LLSC_1_5 : MYKI_CS_TACappingGet() failed" );
        return RULE_RESULT_ERROR;
    }   /* end-of-if */

    //  Check that the daily capping value is in the range of an S32 (ie Currency_t)

    if ( pMYKI_TACapping->Daily.Value > 0x7FFFFFFF )
    {
        CsErrx( "BR_LLSC_1_6 : Daily Capping Value (%u) on card too large, must be between 0 and 0x7FFFFFFF", pMYKI_TACapping->Daily.Value );
        return RULE_RESULT_ERROR;
    }

    /* PRE-CONDITIONS */
    {
        //  1.  If this is a forced scan off sequence (ie IsForcedScanOff is true)
        //      then for this business rule the date for calculations to be used is
        //      the Forced Scan Off Date else use the  common date if set else the
        //      current date time this will be referred to as the common date
        //
        CommonDateTime = myki_br_getCommonDate(pData);

        /*  2.  The current trip zone range must be fully covered by the capping counters
                prior to the current trip. */
        /*      a.  If the low zone of the current trip is greater than or equal to
                    PreviousDailyCapZoneLow and less than or equal to the PreviousDailyCapZoneHigh */
        if ( pData->DynamicData.currentTripZoneLow < pData->DynamicData.previousDailyCapZoneLow  ||
             pData->DynamicData.currentTripZoneLow > pData->DynamicData.previousDailyCapZoneHigh )
        {
            CsDbg( BRLL_RULE, "BR_LLSC_1_5 : BYPASSED - CurrentTripZoneLow(%d) outside PreviousDailyCapZone(%d,%d)",
                    pData->DynamicData.currentTripZoneLow,
                    pData->DynamicData.previousDailyCapZoneLow,
                    pData->DynamicData.previousDailyCapZoneHigh );
            return RULE_RESULT_BYPASSED;
        }   /* end-of-if */
        /*          AND If the high zone of the current trip is greater than or equal to
                    PreviousDailyCapZoneLow and less than or equal to the PreviousDailyCapZoneHigh */
        if ( pData->DynamicData.currentTripZoneHigh < pData->DynamicData.previousDailyCapZoneLow  ||
             pData->DynamicData.currentTripZoneHigh > pData->DynamicData.previousDailyCapZoneHigh )
        {
            CsDbg( BRLL_RULE, "BR_LLSC_1_5 : BYPASSED - CurrentTripZoneHigh(%d) outside PreviousDailyCapZone(%d,%d)",
                    pData->DynamicData.currentTripZoneHigh,
                    pData->DynamicData.previousDailyCapZoneLow,
                    pData->DynamicData.previousDailyCapZoneHigh );
            return RULE_RESULT_BYPASSED;
        }   /* end-of-if */

        /*  3.  Determine the daily capping fares total and the applicable fare for the:
                a.  Zone range of the current trip (low to high zones of the current trip).
                b.  Passenger type.
                c.  The determined fare route
                d.  The common date. */
        if ( myki_br_getFareStoredValue( pData,
                pData->DynamicData.currentTripZoneLow, pData->DynamicData.currentTripZoneHigh,
                pMYKI_TAControl->PassengerCode,
                pData->DynamicData.currentTripDirection,
                pData->DynamicData.fareRouteIdIsValid, pData->DynamicData.fareRouteId,
                CommonDateTime,
                pData->DynamicData.currentDateTime ) < 0 )
        {
            CsErrx( "BR_LLSC_1_5 : myki_br_getDailyFareStoredValue() failed" );
            return RULE_RESULT_ERROR;
        }   /* end-of-if */

        /*  4.  Either of the following conditions is true: */
        if ( (Currency_t)pMYKI_TACapping->Daily.Value     <  pData->Tariff.dailyCapValue    &&
           ( pData->Tariff.entitlementProduct == 0                              ||
             (Currency_t)pMYKI_TACapping->Daily.Value     <  pData->Tariff.entitlementValue ) )
        {
            CsDbg( BRLL_RULE,
                pData->Tariff.entitlementProduct == 0 ?
                    "BR_LLSC_1_5 : BYPASSED - TAppCapping.Daily.Value(%d) less than Tariff.DailyCapValue(%d)" :
                    "BR_LLSC_1_5 : BYPASSED - TAppCapping.Daily.Value(%d) less than Tariff.DailyCapValue(%d) and Tariff.EntitlementValue(%d)",
                pMYKI_TACapping->Daily.Value,
                pData->Tariff.dailyCapValue,
                pData->Tariff.entitlementValue );
            return RULE_RESULT_BYPASSED;
        }   /* end-of-if */

        for ( i = 1; i < DIMOF( pMYKI_TAControl->Directory ); i++ )
        {
            /*  ASSUMED! BR specification does not mention product has to be activated */
            if ( pMYKI_TAControl->Directory[ i ].Status != TAPP_CONTROL_DIRECTORY_STATUS_ACTIVATED )
            {
                continue;   /* for-loop */
            }   /* end-of-if */

            if ( myki_br_GetCardProduct( i, &pMYKI_Directory, &pMYKI_TAProduct ) < 0 )
            {
                CsErrx( "BR_LLSC_1_5 : myki_br_GetCardProduct(%d) failed", i );
                return RULE_RESULT_ERROR;
            }   /* end-of-for */

            /*  a.  The daily capping fares total is greater than or equal to the daily cap */
            if ( (Currency_t)pMYKI_TACapping->Daily.Value >= pData->Tariff.dailyCapValue )
            {
                ProductType = myki_br_cd_GetProductType( pMYKI_Directory->ProductId );

                /*  and no product of type Daily exists where the product:
                    i.  low zone is less than or equal to the current trip low zone
                    ii. high zone is greater than or equal to the current trip high zone */
                if
                (
                    ProductType               == PRODUCT_TYPE_DAILY                     &&
                    pMYKI_TAProduct->ZoneLow  <= pData->DynamicData.currentTripZoneLow  &&
                    pMYKI_TAProduct->ZoneHigh >= pData->DynamicData.currentTripZoneHigh
                )
                {
                    foundDaily          = TRUE;
                }   /* end-of-if */
            }   /* end-of-if */

            /*  b.  If an entitlement product exists and daily capping fares total
                    is greater than or equal to the determined entitlement product value */
            if ( pData->Tariff.entitlementProduct >  0                              &&
                 (Currency_t)pMYKI_TACapping->Daily.Value     >= pData->Tariff.entitlementValue )
            {
                /*  and no product the entitlement product type exits where the entitlement product:
                    i.  low zone is less than or equal to the current trip low zone
                    ii. high zone is greater than or equal to the current trip high zone */
                if
                (
                    pMYKI_Directory->ProductId == pData->Tariff.entitlementProduct      &&
                    pMYKI_TAProduct->ZoneLow  <= pData->DynamicData.currentTripZoneLow  &&
                    pMYKI_TAProduct->ZoneHigh >= pData->DynamicData.currentTripZoneHigh
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
                    "BR_LLSC_1_5 : BYPASSED - found Daily product covers trip zone(s)" :
                    "BR_LLSC_1_5 : BYPASSED - found Entitlement product covers trip zone(s)" );
            return RULE_RESULT_BYPASSED;
        }   /* end-of-if */

        /*  5.  This is a forced scan off sequence (ie IsForcedScanOff is true) and
                it is the same business day as the force scan off event (ie Force Scan date = current business day) or
                this is not a force scan off sequence (ie isForceScanOff = false) */
        if ( pData->DynamicData.isForcedScanOff != FALSE )
        {
            DateC19_t   ForcedScanOffDate   = myki_br_ToMykiDate( pData, pData->DynamicData.forcedScanOffDateTime );

            if ( pData->DynamicData.currentBusinessDate != ForcedScanOffDate )
            {
                CsDbg( BRLL_RULE, "BR_LLSC_1_5 : BYPASSED - ForcedScanOffDate(%d) not same CurrentBusinessDate(%d)",
                        ForcedScanOffDate, pData->DynamicData.currentBusinessDate );
                return RULE_RESULT_BYPASSED;
            }   /* end-of-if */
        }   /* end-of-if */
    }

    /* PROCESSING */
    do
    {
        EndOfBusinessDateTime   = myki_br_EndOfBusinessDateTime( pData, CommonDateTime, FALSE );

        /*  1.  If either of the following 2 scenarios is true. */
        if ( pMYKI_TAControl->ProductInUse > 0 )
        {
            if ( myki_br_GetCardProduct( pMYKI_TAControl->ProductInUse, &pMYKI_Directory, &pMYKI_TAProduct ) < 0 )
            {
                CsErrx( "BR_LLSC_1_5 : myki_br_GetCardProduct( %d ) failed", pMYKI_TAControl->ProductInUse );
                return RULE_RESULT_ERROR;
            }   /* end-of-for */

            ProductType = myki_br_cd_GetProductType( pMYKI_Directory->ProductId );

            if ( ProductType != PRODUCT_TYPE_NHOUR && ProductType != PRODUCT_TYPE_SINGLE )
            {
                break;  /* out-of-do-while */
            }   /* end-of-if */

            /*  a.  If all of the the following points are true:
                    i.  the ProductInUse is not zero
                    ii. if the product in use is an n-hour
                    iii.the low zone of the current trip is equal to the low zone of the product
                    iv. the high zone of the current trip is equal to the high zone of the product
                    v.  the product control bitmap on the product in use indicates that the border active bit is not set
                    vi. the product expiry of the product in use is less than or equal to the end of business day for the common date */
            if ( ProductType == PRODUCT_TYPE_NHOUR )
            {
                if
                (
                    pData->DynamicData.currentTripZoneLow  != pMYKI_TAProduct->ZoneLow  ||
                    pData->DynamicData.currentTripZoneHigh != pMYKI_TAProduct->ZoneHigh ||
                    ( pMYKI_TAProduct->ControlBitmap & TAPP_TPRODUCT_CONTROL_BORDER_STATUS_BITMAP ) != 0 ||
                    pMYKI_TAProduct->EndDateTime > EndOfBusinessDateTime
                )
                {
                    break;  /* out-of-do-while */
                }   /* end-of-if */
            }   /* end-of-if */

            /*  b.  Or, If all of the following are true:
                    i.  The product in use is of type Single Trip
                        the Product in use zone low is between the current trip zone low (when adjusted for city saver zone where applicable)
                    ii. and current trip zone high (when adjusted for city saver zone where applicable) */
            /*      iii.Perform ProductSale/Upgrade on the ProductInUse */

            if ( ProductType == PRODUCT_TYPE_SINGLE )
            {
                ZoneLow     = pData->DynamicData.currentTripZoneLow;
                ZoneHigh    = pData->DynamicData.currentTripZoneHigh;

                ADJUST_FOR_CITYSAVER( ZoneLow, ZoneHigh );

                if ( pMYKI_TAProduct->ZoneLow < ZoneLow  || pMYKI_TAProduct->ZoneLow > ZoneHigh )
                {
                    break;  /* out-of-do-while */
                }
            }

            /*  c.  Then do:
                    i.  [Perform ProductSale/Upgrade on the ProductInUse]
                        (1) If an Entitlement product is applicable set the product ID to the Entitlement product ID else to Daily.
                        (2) If Entitlement zone Low is applicable set the zone low to the Entitlement zone Low else to zone Low of the Current trip.
                        (3) If Entitlement zone High exists set the zone High to the Entitlement zone High else to the zone High of the current trip.
                        (4) Adjust the zone Range to include City Saver zones if applicable
                        (5) If Entitlement product is applicable set the Purchase value to the entitlement fare else to Daily Fare.
                        (6) Set the product expiry to the end of Business day of the common date. */

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
                ZoneLow         = pData->DynamicData.currentTripZoneLow;
                ZoneHigh        = pData->DynamicData.currentTripZoneHigh;
                PurchaseValue   = pData->Tariff.dailyCapValue;
            }

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
                CsErrx( "BR_LLSC_1_5 : myki_br_ldt_ProductSale_Upgrade() failed" );
                return RULE_RESULT_ERROR;
            }

            /*      ii. Perform a ProductUpdate/None request
                        (1) Set the BorderStatus bit of the product control bitmap to False
                        (2) Set the Trip Direction on the product control bitmap to Unknown.
                        (3) Set the off-peak bit of the product control bitmap to false */

            UpdatedProduct = *pMYKI_TAProduct;     //  First, create a copy of the product so we can modify it

            if ( pData->StaticData.AcsCompatibilityMode )
            {
                /*  NOTE:   KAMCO reader clears TAppProduct.ControlBitmap.Provisional bit
                            when performing ProductUpdate/None */
                myki_br_setProductProvisionalStatus( &UpdatedProduct, FALSE );
            }

            myki_br_setProductBorderStatus ( &UpdatedProduct, FALSE );
            myki_br_setProductOffPeakStatus( &UpdatedProduct, FALSE );
            myki_br_setProductTripDirection( &UpdatedProduct, TRIP_DIRECTION_UNKNOWN );

            if ( myki_br_ldt_ProductUpdate( pData, pMYKI_Directory, pMYKI_TAProduct, &UpdatedProduct ) < 0 )
            {
                CsErrx( "BR_LLSC_1_5 : myki_br_ldt_ProductUpdate() failed" );
                return RULE_RESULT_ERROR;
            }

            CsDbg( BRLL_RULE, "BR_LLSC_1_5 : Executed" );
            return RULE_RESULT_EXECUTED;
        }
    }
    while ( FALSE /* ONCE-ONLY */ );

    /*  2.  else
            a.  Perform ProductSale/None transaction
                i.  If Entitlement product is applicable set the productId to the
                    Entitlement productId else to Daily.
                ii. If Entitlement zone Low is applicable set the zone low to the
                    Entitlement zone Low else to zone Low of the Current trip
                iii.If Entitlement zone High exists set the zone High to the
                    Entitlement zone High else to the zone High of the current trip.
                iv. Adjust the zone Range to include City Saver zones if applicable
                v.  If Entitlement product is applicable set the Purchase value to
                    the determined fare else to Daily Fare.
                vi. Set the product expiry to the end of Business day of the common date. */
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
        ZoneLow         = pData->DynamicData.currentTripZoneLow;
        ZoneHigh        = pData->DynamicData.currentTripZoneHigh;
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
        CsErrx( "BR_LLSC_1_5 : myki_br_ldt_ProductSaleEx() failed" );
        return RULE_RESULT_ERROR;
    }   /* end-of-if */

    /*      b.  Perform ProductUpdate/Activate transaction
                i.  Set the serial number to the newly created product */
    pMYKI_Directory     = &pMYKI_TAControl->Directory[ i ];
    if ( myki_br_ldt_ProductUpdate_ActivateEx( pData, pMYKI_Directory,
                pData->DynamicData.currentDateTime, EndOfBusinessDateTime ) < 0 )
    {
        CsErrx( "BR_LLSC_1_5 : myki_br_ldt_ProductUpdate_Activate() failed" );
        return RULE_RESULT_ERROR;
    }   /* end-of-if */

    /*      c.  Perform a TAppUpdate/SetProductInUse setting this Product as the
                product in use. */
    if ( myki_br_ldt_AppUpdate_SetProductInUse( pData, pMYKI_Directory ) < 0 )
    {
        CsErrx( "BR_LLSC_1_5 : myki_br_ldt_AppUpdate_SetProductInUse() failed" );
        return RULE_RESULT_ERROR;
    }   /* end-of-if */

    CsDbg( BRLL_RULE, "BR_LLSC_1_5 : Executed" );
    return RULE_RESULT_EXECUTED;
}   /* BR_LLSC_1_5( ) */
