/* -*- mode: c; tabs: 4; -*- */
/*=============================================================================
**
**    Vix Technology                   Licensed software
**    (C) 2013                         All rights reserved
**
**=============================================================================
**
**  Project/Product : MBU
**  Filename        : BR_LLSC_1_9.c
**  Author(s)       : An Tran
**
**  ID              : BR_LLSC_1_9 - KA0004 v7.0
**
**  Name            : Validate Weekend Cap
**
**  Data Fields     :
**
**      1.  TAppControl.ProductInUse
**      2.  TAppControl.Directory.ProductId
**      3.  TAppTProduct.ZoneLow
**      4.  TAppTProduct.ZoneHigh
**      5.  TAppTProduct.EndDateTime
**      6.  TAppTProduct.PurchaseValue
**      7.  TAppCapping.Weekly.Value
**      8.  TAppCapping.Weekly.ZoneLow
**      9.  TAppCapping.Weekly.ZoneHigh
**      10. Dynamic.CurrentDateTime
**      11. Tariff.EndOfBusinessDay
**      12. Tariff.WeekendCapValue
**      13. Tariff.BaseFare
**      14. Tariff.ApplicableFare
**      15. Tariff.CitySaverZoneFlag
**      16. Dynamic.CurrentTripZoneLow
**      17. Dynamic.CurrentTripZoneHigh
**      18. Dynamic.IsForceScanOff
**      19. Dynamic.ForceScanOffDate
**      20. Dynamic.FareRouteID
**      21. Dynamic.CommonDate
**
**  Pre-Conditions  :
**
**          If this is a forced scan off sequence (ie IsForcedScanOff is true)
**          then for this business rule the date for calculations to be used
**          is the Forced Scan Off Date else use the common date if set
**          otherwise use the current date time this will be referred to as
**          the common date
**      1.  The common day of the week is Saturday or Sunday.
**      2.  Determine the weekend capping fares total fare for the:
**          a.  Weekly capping zone range (low to high weekly capping zones).
**          b.  Passenger type.
**          c.  Determined Fare route
**          d.  common  day of the week/date
**      3.  The weekly capping fares total is greater than or equal to the
**          Weekend cap for the weekly capping zone range (low to high weekly
**          capping zones).
**      4.  No product of type Weekly exits where the weekly product:
**          i.  low zone is less than or equal to the weekly cap low zone
**          ii. high zone is greater than or equal to the weekly cap high zone
**
**  Description     :
**
**      1.  If product in use field is not 0 and is of type nHour/Daily and
**          the low zone of product in use is equal to low zone of weekly
**          capping zone range AND high zone of product in use is equal to
**          high zone of weekly capping zone range; Or if the product in use
**          is type single trip and further if the Product in use zone low is
**          between the current trip zone low (when adjusted for city saver
**          zone where applicable) and current trip zone high (when adjusted
**          for city saver zone where applicable) and the product in use expiry
**          is less than or eqal to the end of business day for the common date
**      2.  then:
**          a.  Perform a ProductSale/Upgrade transaction for the product in use:
**              i.  Set the product low zone to the low weekly capping zone.
**              ii. Set the product high zone to the high weekly capping zone.
**              iii.Set the product expiry to the end of business day of the
**                  following Monday from the common date
**              iv. Set the product purchase value to the value of the weekend cap.
**              v.  Set the product ID to Weekly.
**          b.  Perform a ProductUpdate/None on the product in use
**              i.  Reset the border status
**              ii. Reset the trip direction
**              iii.Reset the off peak bit
**      3.  Else we have to create a new weekly product.
**          a.  Perform a ProductSale/None transaction with:
**              i.  Set the product low zone to the low weekly capping zone.
**              ii. Set the product high zone to the high weekly capping zone.
**              iii.Set the product purchase value to the value of the weekend cap.
**              iv. Set the product ID to Weekly.
**          b.  Activate the Product using ProductUpdate/Activate transaction.
**              i.  Set the product expiry to the end of business day of the
**                  following Monday from the common date .
**          c.  Perform a TAppUpdate/SetProductInUse to set this product as
**              the product in use
**
**  Post-Conditions :
**
**      1.  The product in use is upgraded to a Weekly product, with the
**          applicable capping zone range.
**
**  Member(s)       :
**      BR_LLSC_1_9             [public]    business rule
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
**    1.00  11.10.13    ANT   Create
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
**  BR_LLSC_1_9
**
**  Description     :
**      Implements business rule BR_LLSC_1_9.
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

RuleResult_e BR_LLSC_1_9( MYKI_BR_ContextData_t *pData )
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

    CsDbg( BRLL_RULE, "BR_LLSC_1_9 : Start (Validate Weekend Cap)" );

    if ( pData == NULL )
    {
        CsErrx( "BR_LLSC_1_9 : Invalid argument(s)" );
        return RULE_RESULT_ERROR;
    }   /* end-of-if */

    if ( MYKI_CS_TAControlGet( &pMYKI_TAControl ) < 0 )
    {
        CsErrx( "BR_LLSC_1_9 : MYKI_CS_TAControlGet() failed" );
        return RULE_RESULT_ERROR;
    }   /* end-of-if */

    if ( MYKI_CS_TACappingGet( &pMYKI_TACapping ) < 0 )
    {
        CsErrx( "BR_LLSC_1_9 : MYKI_CS_TACappingGet() failed" );
        return RULE_RESULT_ERROR;
    }   /* end-of-if */

    if ( pMYKI_TACapping->Weekly.Value > 0x7FFFFFFF )
    {
        CsErrx( "BR_LLSC_1_9 : Weekly Capping Value (%u) on card too large, must be between 0 and 0x7FFFFFFF",
            pMYKI_TACapping->Weekly.Value );
        return RULE_RESULT_ERROR;
    }   /* end-of-if */

    /* PRE-CONDITIONS */
    {
        //  0.  If this is a forced scan off sequence (ie IsForcedScanOff is true)
        //      then for this business rule the date for calculations to be used is
        //      the Forced Scan Off Date else use the  common date if set else the
        //      current date time this will be referred to as the common date
        //
        CommonDateTime = myki_br_getCommonDate(pData);

        /*  1.  The common day of the week is Saturday or Sunday. */
        pTm = localtime( (time_t*)&CommonDateTime );
        if ( pTm == NULL )
        {
            CsErrx( "BR_LLSC_1_9 : localtime(%u) failed", CommonDateTime );
            return RULE_RESULT_ERROR;
        }   /* end-of-if */
        if ( pTm->tm_wday != DayOfWeek_Saturday &&
             pTm->tm_wday != DayOfWeek_Sunday   )
        {
            CsDbg( BRLL_RULE, "BR_LLSC_1_9 : BYPASSED - CommonDate(%u) is weekday", CommonDateTime );
            return RULE_RESULT_BYPASSED;
        }   /* end-of-if */

        /*  2.  Determine the weekend capping fares total fare for the:
                a.  Weekly capping zone range (low to high weekly capping zones).
                b.  Passenger type.
                c.  Determined Fare route
                d.  common  day of the week/date */
        if ( myki_br_getFareStoredValue(
                pData,
                pMYKI_TACapping->Weekly.Zone[ 0 ].Low, pMYKI_TACapping->Weekly.Zone[ 0 ].High,
                pMYKI_TAControl->PassengerCode,
                pData->DynamicData.currentTripDirection,
                pData->DynamicData.fareRouteIdIsValid, pData->DynamicData.fareRouteId,
                CommonDateTime,
                pData->DynamicData.currentDateTime ) < 0 )
        {
            CsErrx( "BR_LLSC_1_9 : myki_br_getFareStoredValue() failed" );
            return RULE_RESULT_ERROR;
        }   /* end-of-if */

        /*  3.  The weekly capping fares total is greater than or equal to
                the Weekend cap for the weekly capping zone range (low to high
                weekly capping zones). */
        if ( (Currency_t)pMYKI_TACapping->Weekly.Value < pData->Tariff.weekendCapValue )
        {
            CsDbg( BRLL_RULE, "BR_LLSC_1_9 : BYPASSED - TACapping.Weekly.Value(%d) less than Tariff.WeekendCapValue(%d)",
                pMYKI_TACapping->Weekly.Value,
                pData->Tariff.weekendCapValue );
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
                CsErrx( "BR_LLSC_1_9 : myki_br_GetCardProduct(%d) failed", i );
                return RULE_RESULT_ERROR;
            }   /* end-of-for */

            ProductType = myki_br_cd_GetProductType( pMYKI_Directory->ProductId );

            /*  4.  No product of type Weekly exits where the weekly product:
                    i.  low zone is less than or equal to the weekly cap low zone
                    ii. high zone is greater than or equal to the weekly cap high zone */
            if
            (
                ProductType               == PRODUCT_TYPE_WEEKLY                    &&
                pMYKI_TAProduct->ZoneLow  <= pMYKI_TACapping->Weekly.Zone[ 0 ].Low  &&
                pMYKI_TAProduct->ZoneHigh >= pMYKI_TACapping->Weekly.Zone[ 0 ].High
            )
            {
                CsDbg( BRLL_RULE, "BR_LLSC_1_9 : BYPASSED - found weekly product convers capping zone(s)" );
                return RULE_RESULT_BYPASSED;
            }   /* end-of-if */
        }   /* end-of-for */
    }

    /* PROCESSING */
    {
        do
        {
            EndOfBusinessDayDateTime    = myki_br_EndOfBusinessDateTime( pData, CommonDateTime, FALSE /*DAY*/  );
            EndOfBusinessWeekDateTime   = myki_br_EndOfBusinessDateTime( pData, CommonDateTime, TRUE  /*WEEK*/ );

            /*  1.  If product in use field is not 0 */
            if ( pMYKI_TAControl->ProductInUse > 0 )
            {
                if ( myki_br_GetCardProduct( pMYKI_TAControl->ProductInUse, &pMYKI_Directory, &pMYKI_TAProduct ) < 0 )
                {
                    CsErrx( "BR_LLSC_1_9 : myki_br_GetCardProduct( %d ) failed", pMYKI_TAControl->ProductInUse );
                    return RULE_RESULT_ERROR;
                }   /* end-of-for */

                ProductType = myki_br_cd_GetProductType( pMYKI_Directory->ProductId );

                switch ( ProductType )
                {
                case PRODUCT_TYPE_NHOUR:
                case PRODUCT_TYPE_DAILY:
                    /*  and is of type nHour/Daily
                        and the low zone of product in use is equal to low zone of weekly capping zone range
                        AND high zone of product in use is equal to high zone of weekly capping zone range; */
                    if ( pMYKI_TAProduct->ZoneLow  == pMYKI_TACapping->Weekly.Zone[ 0 ].Low  &&
                         pMYKI_TAProduct->ZoneHigh == pMYKI_TACapping->Weekly.Zone[ 0 ].High )
                    {
                        upgradeProductInUse = TRUE;
                    }   /* end-of-if */
                    break;

                case PRODUCT_TYPE_SINGLE:
                    /*  Or if the product in use is type single trip
                        and further if the Product in use zone low is between the current trip zone low
                        (when adjusted for city saver zone where applicable) and current trip zone high
                        (when adjusted for city saver zone where applicable) */
                    ZoneLow         = pData->DynamicData.currentTripZoneLow;
                    ZoneHigh        = pData->DynamicData.currentTripZoneHigh;

                    ADJUST_FOR_CITYSAVER( ZoneLow, ZoneHigh );

                    if ( pMYKI_TAProduct->ZoneLow >= ZoneLow && pMYKI_TAProduct->ZoneLow <= ZoneHigh )
                    {
                        upgradeProductInUse = TRUE;
                    }   /* end-of-if */
                    break;

                default:
                    /* Others!? */
                    break;
                }   /* end-of-switch */

                /*  and the product in use expiry is less than or eq(u)al to the end of business day for the common date */
                if ( upgradeProductInUse          != FALSE                    &&
                     pMYKI_TAProduct->EndDateTime <= EndOfBusinessDayDateTime )
                {
                    /*  2.   then:
                            a.  Perform a ProductSale/Upgrade transaction for the product in use:
                                i.  Set the product low zone to the low weekly capping zone.
                                ii. Set the product high zone to the high weekly capping zone.
                                iii.Set the product expiry to the end of business day of the following Monday from the common date
                                iv. Set the product purchase value to the value of the weekend cap.
                                v.  Set the product ID to Weekly. */
                    PurchaseValue   = pData->Tariff.weekendCapValue;
                    ZoneLow         = pMYKI_TACapping->Weekly.Zone[ 0 ].Low;
                    ZoneHigh        = pMYKI_TACapping->Weekly.Zone[ 0 ].High;
                    ProductId       = myki_br_cd_GetProductId( PRODUCT_TYPE_WEEKLY );
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
                            EndOfBusinessWeekDateTime,
                            pData->StaticData.AcsCompatibilityMode ? FALSE : TRUE   // Kamco : DO NOT clear TAppProduct.ControlBitmap.Provisional bit
                        ) < 0
                    )
                    {
                        CsErrx( "BR_LLSC_1_9 : myki_br_ldt_ProductSale_UpgradeEx() failed" );
                        return RULE_RESULT_ERROR;
                    }   /* end-of-if */

                    /*      b.  Perform a ProductUpdate/None on the product in use
                                i.  Reset the border status
                                ii. Reset the trip direction
                                iii.Reset the off peak bit */

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
                        CsErrx( "BR_LLSC_1_9 : myki_br_ldt_ProductUpdate() failed" );
                        return RULE_RESULT_ERROR;
                    }   /* end-of-if */

                    break;  /* out-of-do-while */
                }   /* end-of-if */
            }   /* end-of-if */

            /*  3.  Else we have to create a new weekly product.
                    a.  Perform a ProductSale/None transaction with:
                        i.  Set the product low zone to the low weekly capping zone.
                        ii. Set the product high zone to the high weekly capping zone.
                        iii.Set the product purchase value to the value of the weekend cap.
                        iv. Set the product ID to Weekly. */
            PurchaseValue   = pData->Tariff.weekendCapValue;
            ZoneLow         = pMYKI_TACapping->Weekly.Zone[ 0 ].Low;
            ZoneHigh        = pMYKI_TACapping->Weekly.Zone[ 0 ].High;
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
                CsErrx( "BR_LLSC_1_9 : myki_br_ldt_ProductSaleEx() failed" );
                return RULE_RESULT_ERROR;
            }   /* end-of-if */

            /*      b.  Activate the Product using ProductUpdate/Activate transaction.
                        i.  Set the product expiry to the end of business day of the following Monday from the common date. */
            pMYKI_Directory     = &pMYKI_TAControl->Directory[ i ];
            if ( myki_br_ldt_ProductUpdate_ActivateEx( pData, pMYKI_Directory,
                        pData->DynamicData.currentDateTime, EndOfBusinessWeekDateTime ) < 0 )
            {
                CsErrx( "BR_LLSC_1_9 : myki_br_ldt_ProductUpdate_Activate() failed" );
                return RULE_RESULT_ERROR;
            }   /* end-of-if */

            /*      c.  Perform a TAppUpdate/SetProductInUse to set this product as the product in use */
            if ( myki_br_ldt_AppUpdate_SetProductInUse( pData, pMYKI_Directory ) < 0 )
            {
                CsErrx( "BR_LLSC_1_9 : myki_br_ldt_AppUpdate_SetProductInUse() failed" );
                return RULE_RESULT_ERROR;
            }   /* end-of-if */
        }
        while ( FALSE /*ONCE-ONLY*/ );
    }

    CsDbg( BRLL_RULE, "BR_LLSC_1_9 : Executed" );
    return RULE_RESULT_EXECUTED;
}   /* BR_LLSC_1_9( ) */
