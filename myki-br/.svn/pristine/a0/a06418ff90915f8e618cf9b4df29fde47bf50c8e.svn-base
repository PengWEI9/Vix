/* -*- mode: c; tabs: 4; -*- */
/*=============================================================================
**
**    Vix Technology                   Licensed software
**    (C) 2013                         All rights reserved
**
**=============================================================================
**
**  Project/Product : MBU
**  Filename        : BR_LLSC_1_7.c
**  Author(s)       : An Tran
**
**  ID              : BR_LLSC_1_7 - KA0004 v7.0
**
**  Name            : Validate Weekly Cap Current Trip
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
**      8.  TAppCapping.Weekly.Value
**      9.  Dynamic.CurrentDateTime
**      10. Dynamic.CurrentTripZoneLow
**      11. Dynamic.CurrentTripZoneHigh
**      12. Tariff.EndOfBusinessDay
**      13. Tariff.WeekendCapValue
**      14. Tariff.EntitlementZoneLow
**      15. Tariff.EntitlementZoneHigh
**      16. Tariff.EntitlementProduct
**      17. Tariff.CitySaverZoneFlag
**      18. Dynamic.IsForcedScanOff
**      19. Dynamic.ForceScanOffDate
**      20. Dynamic.FareRouteID
**      21. Dynamic.CommonDate
**
**  Pre-Conditions  :
**
**          This is a forced scan off sequence (ie IsForcedScanOff is true)
**          then for this business rule the date for calculations to be used
**          is the Forced Scan Off Date else use the common date if set
**          otherwise use the current date time this will be referred to as
**          the common date
**      1.  The common date was not  a Saturday or Sunday .
**      2.  Determine the weekly capping fares total and the applicable fare for the:
**          a.  Current trip zone  range (low zone to high zone)
**          b.  Passenger type.
**          c.  Determined Fare route
**          d.  The common day of the week/date.
**      3.  The weekly capping fares total is greater than or equal to the
**          weekly cap
**      4.  There is no weekly product that exists on the card that has a
**          zone range greater than or equal to the current weekly capping
**          zone range.
**      5.  This is a forced scan off sequence (ie IsForcedScanOff is true)
**          and if its the same business week as the force scan off event
**          ie Force Scan date is equal to the current business week or this
**          is not a force scan off sequence (ie isForceScanOff = false)
**
**  Description     :
**
**      1.  If the low zone of the current trip minus 1 is marked as City
**          Saver set the product low zone to the low zone of the current
**          trip minus 1, else set the product low zone to the low zone
**          of the current trip.
**      2.  If the high zone of the current trip is marked as City Saver
**          set the product high zone to the high zone of the current trip
**          plus 1, else set the product high zone5 to the high zone of
**          the current trip.
**      3.  If the product in use is not zero (0) and the product type of
**          the product in use is either an n-Hour or Daily and the product
**          in use low zone is equal the low zone of the current trip and
**          the if the product in use high zone is equal to the high zone
**          of the current trip. OR If the product in use1 is of type Single
**          Trip and if the Product in use zone low is between the current
**          trip zone low (when adjusted for city saver zone where applicable)
**          and  current trip zone high (when adjusted for city saver zone
**          where applicable) and the product in use expiry is less than
**          or equal to the end of business day for the common date
**      4.  Then:
**          a.  If the product is either n-Hour or Single Trip perform a
**              Perform ProductSale/Upgrade transaction:
**              i.  Set the product ID to Weekly
**              ii. Set the product expiry to the end of business day of
**                  the following common date Monday.
**              iii.Set the product purchase value to the value of the
**                  applicable weekly cap.
**          b.  ProductUpdate/None on the product in use
**              i.  Set BorderStaus bit of the product control bitmap to False.
**              ii. Set the trip direction on the product control bit map to
**                  status to 0 (unknown)
**              iii.Set the off-peak bit of the product control bitmap to false
**      5.  else
**          a.  Perform a ProductSale/None transaction to create the new product
**              i.  Set the zone range the current trip zone range.
**              ii. Set the Product ID to Weekly
**              iii.Set the product purchase value to the value of the
**                  applicable weekly cap.
**          b.  Perform a ProductSale/Activate to activate the new product.
**              i.  Set the product expiry to the end of business day of the
**                  following common date Monday.
**          c.  Perform a TAppUpdate/SetProductInUse to set this as product
**              as the product in use.
**
**  Post-Conditions :
**
**      1.  The applicable product in use is upgraded to a Weekly product,
**          with the applicable capping zone range.
**
**  Member(s)       :
**      BR_LLSC_1_7             [public]    business rule
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
**    1.00  10.10.13    ANT   Create
**    1.01  23.10.13    ANT   Modify   myki_br_ldt_ProductUpdate_ActivateEx()
**                                     signature changed
**    1.02  11.11.13    ANT   Modify   myki_br_ldt_ProductSaleEx signature
**                                     changed
**    1.03  11.02.14    ANT   Modify   Clears TAppProduct.ControlBitmap.Provisional
**                                     bit when performing ProductUpdate/None
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
**  BR_LLSC_1_7
**
**  Description     :
**      Implements business rule BR_LLSC_1_7.
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

RuleResult_e BR_LLSC_1_7( MYKI_BR_ContextData_t *pData )
{
    MYKI_TAControl_t       *pMYKI_TAControl             = NULL;
    MYKI_TACapping_t       *pMYKI_TACapping             = NULL;
    MYKI_Directory_t       *pMYKI_Directory             = NULL;
    MYKI_TAProduct_t       *pMYKI_TAProduct             = NULL;
    int                     i                           = 0;
    Time_t                  CommonDateTime;
    struct tm              *pTm                         = NULL;
    ProductType_e           ProductType                 = PRODUCT_TYPE_UNKNOWN;
    int                     ProductId                   = 0;
    int                     ZoneLow                     = 0;
    int                     ZoneHigh                    = 0;
    int                     PurchaseValue               = 0;
    Time_t                  EndOfBusinessDayDateTime    = 0;
    Time_t                  EndOfBusinessWeekDateTime   = 0;
    int                     upgradeProductInUse         = FALSE;
    MYKI_TAProduct_t        UpdatedProduct;
    ProductSale_t           request;

    CsDbg( BRLL_RULE, "BR_LLSC_1_7 : Start (Validate Weekly Cap Current Trip)" );

    if ( pData == NULL )
    {
        CsErrx( "BR_LLSC_1_7 : Invalid argument(s)" );
        return RULE_RESULT_ERROR;
    }   /* end-of-if */

    if ( MYKI_CS_TAControlGet( &pMYKI_TAControl ) < 0 )
    {
        CsErrx( "BR_LLSC_1_7 : MYKI_CS_TAControlGet() failed" );
        return RULE_RESULT_ERROR;
    }   /* end-of-if */

    if ( MYKI_CS_TACappingGet( &pMYKI_TACapping ) < 0 )
    {
        CsErrx( "BR_LLSC_1_7 : MYKI_CS_TACappingGet() failed" );
        return RULE_RESULT_ERROR;
    }   /* end-of-if */

    if ( pMYKI_TACapping->Weekly.Value > 0x7FFFFFFF )
    {
        CsErrx( "BR_LLSC_1_7 : Weekly Capping Value (%u) on card too large, must be between 0 and 0x7FFFFFFF",
            pMYKI_TACapping->Weekly.Value );
        return RULE_RESULT_ERROR;
    }   /* end-of-if */

    /* PRE-CONDITIONS */
    {
        //  1.  If this is a forced scan off sequence (ie IsForcedScanOff is true)
        //      then for this business rule the date for calculations to be used is
        //      the Forced Scan Off Date else use the  common date if set else the
        //      current date time this will be referred to as the common date
        //
        CommonDateTime = myki_br_getCommonDate(pData);

        /*  1.  The common date was not a Saturday or Sunday. */
        pTm = localtime( (time_t*)&CommonDateTime );
        if ( pTm == NULL )
        {
            CsErrx( "BR_LLSC_1_7 : localtime(%u) failed", CommonDateTime );
            return RULE_RESULT_ERROR;
        }   /* end-of-if */
        if ( pTm->tm_wday == DayOfWeek_Saturday ||
             pTm->tm_wday == DayOfWeek_Sunday   )
        {
            CsDbg( BRLL_RULE, "BR_LLSC_1_7 : BYPASSED - CommonDate(%u) is weekend", CommonDateTime );
            return RULE_RESULT_BYPASSED;
        }   /* end-of-if */

        /*  2.  Determine the weekly capping fares total and the applicable fare for the:
                a.  Current trip zone  range (low zone to high zone)
                b.  Passenger type.
                c.  Determined Fare route
                d.  The common day of the week/date. */
        if ( myki_br_getFareStoredValue(
                pData,
                pData->DynamicData.currentTripZoneLow, pData->DynamicData.currentTripZoneHigh,
                pMYKI_TAControl->PassengerCode,
                pData->DynamicData.currentTripDirection,
                pData->DynamicData.fareRouteIdIsValid, pData->DynamicData.fareRouteId,
                CommonDateTime,
                pData->DynamicData.currentDateTime ) < 0 )
        {
            CsErrx( "BR_LLSC_1_7 : myki_br_getFareStoredValue() failed" );
            return RULE_RESULT_ERROR;
        }   /* end-of-if */

        /*  3.  The weekly capping fares total is greater than or equal to the weekly cap */
        if ( (Currency_t)pMYKI_TACapping->Weekly.Value < pData->Tariff.weeklyCapValue )
        {
            CsDbg( BRLL_RULE, "BR_LLSC_1_7 : BYPASSED - TACapping.Weekly.Value(%d) less than Tariff.WeeklyCapValue(%d)",
                pMYKI_TACapping->Weekly.Value,
                pData->Tariff.weeklyCapValue );
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
                CsErrx( "BR_LLSC_1_7 : myki_br_GetCardProduct(%d) failed", i );
                return RULE_RESULT_ERROR;
            }   /* end-of-for */

            ProductType = myki_br_cd_GetProductType( pMYKI_Directory->ProductId );

            /*  4.  There is no weekly product that exists on the card that has
                    a zone range greater than or equal to the current weekly capping zone range. */
            if
            (
                ProductType               == PRODUCT_TYPE_WEEKLY                    &&
                pMYKI_TAProduct->ZoneLow  <= pMYKI_TACapping->Weekly.Zone[ 0 ].Low  &&
                pMYKI_TAProduct->ZoneHigh >= pMYKI_TACapping->Weekly.Zone[ 0 ].High
            )
            {
                CsDbg( BRLL_RULE, "BR_LLSC_1_7 : BYPASSED - found weekly product convers capping zone(s)" );
                return RULE_RESULT_BYPASSED;
            }   /* end-of-if */
        }   /* end-of-for */

        /*  5.  This is a forced scan off sequence (ie IsForcedScanOff is true)
                and if it[']s the same business week as the force scan off event
                ie Force Scan date is equal to the current business week
                or this is not a force scan off sequence (ie isForceScanOff = false) */
        EndOfBusinessWeekDateTime   = myki_br_EndOfBusinessDateTime( pData, CommonDateTime, TRUE /*WEEK*/ );
        if ( pData->DynamicData.isForcedScanOff != FALSE )
        {
            Time_t  CurrentBusinessWeekDateTime = \
                myki_br_EndOfBusinessDateTime( pData, pData->DynamicData.currentDateTime, TRUE  /*WEEK*/ );
            if ( EndOfBusinessWeekDateTime != CurrentBusinessWeekDateTime )
            {
                CsDbg( BRLL_RULE, "BR_LLSC_1_7 : BYPASSED - End business week of ForcedScanOffDate(%u) is not current(%u)",
                    EndOfBusinessWeekDateTime, CurrentBusinessWeekDateTime );
                return RULE_RESULT_BYPASSED;
            }   /* end-of-if */
        }   /* end-of-if */
    }

    /* PROCESSING */
    {
        /*  1.  If the low zone of the current trip minus 1 is marked as City Saver
                set the product low zone to the low zone of the current trip minus 1,
                else set the product low zone to the low zone of the current trip. */
        /*  2.  If the high zone of the current trip is marked as City Saver set
                the product high zone to the high zone of the current trip plus 1,
                else set the product high zone to the high zone of the current trip. */
        ZoneLow     = pData->DynamicData.currentTripZoneLow;
        ZoneHigh    = pData->DynamicData.currentTripZoneHigh;
        ADJUST_FOR_CITYSAVER( ZoneLow, ZoneHigh );

        do
        {
            EndOfBusinessDayDateTime    = myki_br_EndOfBusinessDateTime( pData, CommonDateTime, FALSE /*DAY*/  );

            /*  3.  If the product in use is not zero (0) */
            if ( pMYKI_TAControl->ProductInUse > 0 )
            {
                if ( myki_br_GetCardProduct( pMYKI_TAControl->ProductInUse, &pMYKI_Directory, &pMYKI_TAProduct ) < 0 )
                {
                    CsErrx( "BR_LLSC_1_7 : myki_br_GetCardProduct( %d ) failed", pMYKI_TAControl->ProductInUse );
                    return RULE_RESULT_ERROR;
                }   /* end-of-for */

                ProductType = myki_br_cd_GetProductType( pMYKI_Directory->ProductId );

                switch ( ProductType )
                {
                case PRODUCT_TYPE_NHOUR:
                case PRODUCT_TYPE_DAILY:
                    /*  and the product type of the product in use is either an n-Hour or Daily
                        and the product in use low zone is equal the low zone of the current trip
                        and the if the product in use high zone is equal to the high zone of the current trip. */
                    if ( pMYKI_TAProduct->ZoneLow  == pData->DynamicData.currentTripZoneLow  &&
                         pMYKI_TAProduct->ZoneHigh == pData->DynamicData.currentTripZoneHigh )
                    {
                        upgradeProductInUse = TRUE;
                    }   /* end-of-if */
                    break;

                case PRODUCT_TYPE_SINGLE:
                    /*  OR If the product in use is of type Single Trip
                        and if the Product in use zone low is between the current trip zone low
                        (when adjusted for city saver zone where applicable) and current trip zone high
                        (when adjusted for city saver zone where applicable) */
                    if ( pMYKI_TAProduct->ZoneLow >= ZoneLow  &&
                         pMYKI_TAProduct->ZoneLow <= ZoneHigh )
                    {
                        upgradeProductInUse = TRUE;
                    }   /* end-of-if */
                    break;

                default:
                    /* Others!? */
                    break;
                }   /* end-of-switch */

                /*  and the product in use expiry is less than or equal to the end of business day for the common date */
                if ( upgradeProductInUse          != FALSE                    &&
                     pMYKI_TAProduct->EndDateTime <= EndOfBusinessDayDateTime )
                {
                    /*  4.  Then:
                            a.  If the product is either n-Hour[, Daily] or Single Trip perform a Perform ProductSale/Upgrade transaction:
                                i.  Set the product ID to Weekly
                                ii. Set the product expiry to the end of business day of the following common date Monday.
                                iii.Set the product purchase value to the value of the applicable weekly cap. */
                    PurchaseValue   = pData->Tariff.weeklyCapValue;
                    ProductId       = myki_br_cd_GetProductId( PRODUCT_TYPE_WEEKLY );
                    if
                    (
                        myki_br_ldt_ProductSale_UpgradeEx(
                            pData,
                            pMYKI_Directory,
                            ProductId,
                            ZoneLow,
                            ZoneHigh,
                            PurchaseValue,
                            EndOfBusinessWeekDateTime,
                            pData->StaticData.AcsCompatibilityMode ? FALSE : TRUE   // Kamco : DO NOT clear TAppProduct.ControlBitmap.Provisional bit
                        ) < 0
                    )
                    {
                        CsErrx( "BR_LLSC_1_7 : myki_br_ldt_ProductSale_UpgradeEx() failed" );
                        return RULE_RESULT_ERROR;
                    }   /* end-of-if */

                    /*      b.  ProductUpdate/None on the product in use
                                i.  Set BorderStaus bit of the product control bitmap to False.
                                ii. Set the trip direction on the product control bit map to status to 0 (unknown)
                                iii.Set the off-peak bit of the product control bitmap to false */

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
                        CsErrx( "BR_LLSC_1_7 : myki_br_ldt_ProductUpdate() failed" );
                        return RULE_RESULT_ERROR;
                    }   /* end-of-if */

                    break;  /* out-of-do-while */
                }   /* end-of-if */
            }   /* end-of-if */

            /*  5.  else
                    a.  Perform a ProductSale/None transaction to create the new product
                        i.  Set the zone range the current trip zone range.
                        ii. Set the Product ID to Weekly
                        iii.Set the product purchase value to the value of the applicable weekly cap. */
            PurchaseValue   = pData->Tariff.weeklyCapValue;
            ProductId       = myki_br_cd_GetProductId( PRODUCT_TYPE_WEEKLY );

            memset( &request, 0, sizeof( request ) );
            request.productId               = ProductId;
            request.purchaseValue           = PurchaseValue;
            request.zoneLow                 = ZoneLow;
            request.zoneHigh                = ZoneHigh;
            request.isEndDateTimeSet        = TRUE;
            request.endDateTime             = EndOfBusinessWeekDateTime;

            if ( ( i = myki_br_ldt_ProductSaleEx( pData, &request ) ) < 0 )
            {
                CsErrx( "BR_LLSC_1_7 : myki_br_ldt_ProductSaleEx() failed" );
                return RULE_RESULT_ERROR;
            }   /* end-of-if */

            /*      b.  Perform a ProductSale/Activate to activate the new product.
                        i.  Set the product expiry to the end of business day of the following common date Monday. */
            pMYKI_Directory     = &pMYKI_TAControl->Directory[ i ];
            if ( myki_br_ldt_ProductUpdate_ActivateEx( pData, pMYKI_Directory,
                        pData->DynamicData.currentDateTime, EndOfBusinessWeekDateTime ) < 0 )
            {
                CsErrx( "BR_LLSC_1_7 : myki_br_ldt_ProductUpdate_Activate() failed" );
                return RULE_RESULT_ERROR;
            }   /* end-of-if */

            /*      c.  Perform a TAppUpdate/SetProductInUse to set this as product as the product in use. */
            if ( myki_br_ldt_AppUpdate_SetProductInUse( pData, pMYKI_Directory ) < 0 )
            {
                CsErrx( "BR_LLSC_1_7 : myki_br_ldt_AppUpdate_SetProductInUse() failed" );
                return RULE_RESULT_ERROR;
            }   /* end-of-if */
        }
        while ( 0 /*ONCE-ONLY*/ );
    }

    CsDbg( BRLL_RULE, "BR_LLSC_1_7 : Executed" );
    return RULE_RESULT_EXECUTED;
}   /* BR_LLSC_1_7( ) */
