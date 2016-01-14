/* -*- mode: c; tabs: 4; -*- */
/*=============================================================================
**
**    Vix Technology                   Licensed software
**    (C) 2013                         All rights reserved
**
**=============================================================================
**
**  Project/Product : MBU
**  Filename        : BR_LLSC_10_3.c
**  Author(s)       : An Tran
**
**  ID              : BR_LLSC_10_3 - KA0004 v7.0
**
**  Name            : Travel Access Pass - Scan On travel Access Pass - No other product
**
**  Data Fields     :
**
**      1.  TAppControl.ProvisionalFare
**      2.  TAppControl.PassengerCode
**      3.  TAppControl.Directory.ProductId
**      4.  TAppTProduct.PurchaseValue
**      5.  TAppTProduct.ControlBitmap
**      6.  TAppTProduct.ZoneLow
**      7.  TAppTProduct.ZoneHigh
**      8.  Static.TransportMode
**      9.  Dynamic.CurrentDateTime
**      10. Dynamic.CurrentZone
**      11. Dynamic.StopId
**      12. Dynamic.ProvisionalZoneLow
**      13. Dynamic.ProvisionalZoneHigh
**      14. Tariff.CitySaverZoneFlag
**      15. Tarrif.ConcessionType.isDDA
**      16. Dynamic.UsageLog
**
**  Pre-Conditions  :
**
**      1.  The product control bit map indicates that there is no provisional
**          products on the card
**      2.  This passenger code is a DDA passenger code.
**
**  Description     :
**
**      1.  Perform a ProductSale/none transaction for the provisional product:
**          a.  Using the current location determine the provisional zone range
**              i.  If the provisional low zone minus 1 is marked as City
**                  Saver set the product low zone to the provisional low
**                  zone minus 1, else set the product low zone to the
**                  provisional low zone.
**              ii. If the provisional high zone is marked as City Saver
**                  set the product high zone to the provisional high zone
**                  plus 1, else set the product high zone to the provisional
**                  high zone.
**          b.  The provisional bit on the product control bitmap to true
**          c.  The product ID to n-Hour
**          d.  Set the provisional fare to 0
**      2.  Perform a ProductUpdate/Activate transaction for the provisional product.
**      3.  Perform a ProductUsage/ScanOnStoredValue transaction for the provisional
**          product.
**      4.  Create usage log
**          a.  Definition:
**              i.  TAppLoadLog.ControlBitmap: Useage
**              ii. If the TXT Type is not set then set the TxType = Type
**                  (Entry Exit = 8, Exit Only = 18; Entry Only = 19,
**                  On board = 20),
**              iii.Determine UsageLog Control Bitmap by lookuping TxUsageType
**              iv. ProviderID = same product ServiceProviderID
**              v.  TxDateTime = Current date time
**              vi. Location.EntryPoint = EntryPointID
**              vii.Location.Route = LineID
**              viii.   Location.Stop = Stopid
**          b.  Value:
**              i.  TxValue = 0
**              ii. NewTPurseBalance = current tpurse balance
**          c.  Usage:
**              i.  Zone = Current zone
**              ii. ProvisionalValidationStatus = +Provisional +ScanOn
**              i.  Product: ProductIssuerId - as used in step 3
**              ii. ProductId - as used in step 3
**              iii.ProductSerialNo - as used in step 3
**
**  Post-Conditions :
**
**      A provisional product has been created & activated
**
**  Member(s)       :
**      BR_LLSC_10_3            [public]    business rule
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
**    1.00  18.10.13    ANT   Create
**    1.01  22.10.13    ANT   Modify   Calling myki_br_IsDDA()
**                                     Calling myki_br_getTransactionType()
**                                     Simplified checking for provisional
**                                     product (ie. is product in use)
**    1.01  30.01.14    ANT   Modify   Set TAppUsageLog.PaymentMethod to
**                                     TAPP_USAGE_LOG_PAYMENT_METHOD_TPURSE to
**                                     mimic KAMCO reader behaviour
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
**  BR_LLSC_10_3
**
**  Description     :
**      Implements business rule BR_LLSC_10_3.
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

RuleResult_e BR_LLSC_10_3( MYKI_BR_ContextData_t *pData )
{
    MYKI_TAControl_t       *pMYKI_TAControl         = NULL;
    MYKI_Directory_t       *pMYKI_Directory         = NULL;
    MYKI_TAProduct_t       *pMYKI_TAProduct         = NULL;
    TAppUsageLog_t         *pTAppUsageLog           = NULL;
    MYKI_TAPurseBalance_t  *pMYKI_TAPurseBalance    = NULL;
    int                     i                       = 0;
    int                     ProductId               = 0;
    int                     ZoneLow                 = 0;
    int                     ZoneHigh                = 0;
    int                     PurchaseValue           = 0;
    int                     IsDDA                   = FALSE;
    int                     IsProvisional           = FALSE;

    CsDbg( BRLL_RULE, "BR_LLSC_10_3 : Start (Travel Access Pass -  Scan On travel Access Pass - No other product)" );

    if ( pData == NULL )
    {
        CsErrx( "BR_LLSC_10_3 : Invalid argument(s)" );
        return RULE_RESULT_ERROR;
    }   /* end-of-if */

    if ( MYKI_CS_TAControlGet( &pMYKI_TAControl ) < 0 )
    {
        CsErrx( "BR_LLSC_10_3 : MYKI_CS_TAControlGet() failed" );
        return RULE_RESULT_ERROR;
    }   /* end-of-if */

    /* PRE-CONDITIONS */
    {
        /*  1.  The product [in use] control bit map indicates that there is no provisional products on the card */
        if ( pMYKI_TAControl->ProductInUse > 0 )
        {
            if ( myki_br_GetCardProduct( pMYKI_TAControl->ProductInUse, &pMYKI_Directory, &pMYKI_TAProduct ) < 0 )
            {
                CsErrx( "BR_LLSC_10_3 : myki_br_GetCardProduct(%d) failed", i );
                return RULE_RESULT_ERROR;
            }   /* end-of-for */

            if ( ( pMYKI_TAProduct->ControlBitmap & TAPP_TPRODUCT_CONTROL_PROVISIONAL_BITMAP ) != 0 )
            {
                CsDbg( BRLL_RULE, "BR_LLSC_10_3 : Bypass - Product in use is provisional product" );
                return RULE_RESULT_BYPASSED;
            }   /* end-of-if */
        }   /* end-of-if */

        /*  2.  This passenger code is a DDA passenger code. */
        IsDDA   = myki_br_IsDDA( pMYKI_TAControl->PassengerCode );
        if ( IsDDA < 0 )
        {
            CsErrx( "BR_LLSC_10_3 : myki_br_IsDDA() failed" );
            return RULE_RESULT_ERROR;
        }   /* end-of-if */

        if ( IsDDA == FALSE )
        {
            CsDbg( BRLL_RULE, "BR_LLSC_10_3 : Bypass - Passenger type is not DDA" );
            pData->Tariff.concessionTypeIsDDA   = FALSE;
            return RULE_RESULT_BYPASSED;
        }   /* end-of-if */
    }

    /* PROCESSING */
    {
        pData->Tariff.concessionTypeIsDDA   = TRUE;

        /*  1.  Perform a ProductSale/none transaction for the provisional product: */
        /*      a.  Using the current location determine the provisional zone range
                    i.  If the provisional low zone minus 1 is marked as City Saver
                        set the product low zone to the provisional low zone minus 1,
                        else set the product low zone8 to the provisional low zone.
                    ii. If the provisional high zone is marked as City Saver set the
                        product high zone to the provisional high zone plus 1,
                        else set the product high zone to the provisional high zone. */
        ZoneLow         = pData->DynamicData.provisionalZoneLow;
        ZoneHigh        = pData->DynamicData.provisionalZoneHigh;
        ADJUST_FOR_CITYSAVER( ZoneLow, ZoneHigh );

        /*      b.  The provisional bit on the product control bitmap to true */
        IsProvisional   = TRUE;

        /*      c.  The product ID to n-Hour */
        ProductId       = myki_br_cd_GetProductId( PRODUCT_TYPE_NHOUR );

        /*      d.  Set the provisional fare to 0 */
        PurchaseValue   = 0;

        if ( ( i = myki_br_ldt_ProductSale( pData,
                    ProductId, ZoneLow, ZoneHigh, PurchaseValue, IsProvisional ) ) < 0 )
        {
            CsErrx( "BR_LLSC_10_3 : myki_br_ldt_ProductSale() failed" );
            return RULE_RESULT_ERROR;
        }   /* end-of-if */

        /*  2.  Perform a ProductUpdate/Activate transaction for the provisional product. */
        pMYKI_Directory = &pMYKI_TAControl->Directory[ i ];
        if ( myki_br_ldt_ProductUpdate_Activate( pData, pMYKI_Directory ) < 0 )
        {
            CsErrx( "BR_LLSC_10_3 : myki_br_ldt_ProductUpdate_Activate() failed" );
            return RULE_RESULT_ERROR;
        }   /* end-of-if */

        /*  3.  Perform a ProductUsage/ScanOnStoredValue transaction for the provisional product. */
        if ( myki_br_ldt_ProductUsage_ScanOnStoredValue( pData, pMYKI_Directory ) < 0 )
        {
            CsErrx( "BR_LLSC_10_3 : myki_br_ldt_ProductUsage_ScanOnStoredValue() failed" );
            return RULE_RESULT_ERROR;
        }   /* end-of-if */

        /*  4.  Create usage log */
        pTAppUsageLog                                   = &pData->InternalData.UsageLogData;

        /*      a.  Definition: */
        {
            /*      i.  TAppLoadLog[TAppUsageLog].ControlBitmap: Useage (Done by framework) */

            /*      ii. If the TXT Type is not set then set the TxType = Type (Entry Exit = 8,
                        Exit Only =18; Entry Only =19, On board= 20), */
            if ( pTAppUsageLog->transactionType == MYKI_BR_TRANSACTION_TYPE_NONE )
            {
                pTAppUsageLog->transactionType          = myki_br_getTransactionType( pData );
            }   /* end-of-if */

            /*      iii.Determine UsageLog Control Bitmap by lookuping TxUsageType (Done by framework) */
            /*      iv. ProviderID =  same product ServiceProviderID (Done by framework) */
            /*      v.  TxDateTime = Current date time (Done by framework) */
            /*      vi. Location.EntryPoint = EntryPointID (Done by framework) */
            /*      vii.Location.Route = LineID (Done by framework) */
            /*      viii.   Location.Stop = Stopid (Done by framework) */
        }

        /*      b.  Value: */
        {
            /*      i.  TxValue = 0 */
            pTAppUsageLog->isTransactionValueSet        = TRUE;
            pTAppUsageLog->transactionValue             = 0;

            /*      ii. NewTPurseBalance = current tpurse balance */
            if ( MYKI_CS_TAPurseBalanceGet( &pMYKI_TAPurseBalance ) < 0 || pMYKI_TAPurseBalance == NULL )
            {
                CsErrx( "BR_LLSC_10_3 : MYKI_CS_TAPurseBalanceGet() failed" );
                return RULE_RESULT_ERROR;
            }   /* end-of-if */
            pTAppUsageLog->isNewTPurseBalanceSet        = TRUE;
            pTAppUsageLog->newTPurseBalance             = pMYKI_TAPurseBalance->Balance;

            /*          Ensure PaymentMethod has sensible value! */
            pTAppUsageLog->isPaymentMethodSet           = TRUE;

            if ( pData->StaticData.AcsCompatibilityMode )
            {
                /*  NOTE:   KAMCO reader has PaymentMethod set to TPurse although
                            transaction amount is zero */
                pTAppUsageLog->paymentMethod            = TAPP_USAGE_LOG_PAYMENT_METHOD_TPURSE;
            }
            else
            {
                pTAppUsageLog->paymentMethod            = TAPP_USAGE_LOG_PAYMENT_METHOD_UNDEFINED;
            }
        }

        /*      c.  Usage: */
        {
            /*      i.  Zone = Current zone */
            pTAppUsageLog->isZoneSet                    = TRUE;
            pTAppUsageLog->zone                         = pData->DynamicData.currentZone;

            /*      ii. ProvisionalValidationStatus = +Provisional +ScanOn */
            pTAppUsageLog->isProductValidationStatusSet = TRUE;
            pTAppUsageLog->productValidationStatus     |= ( TAPP_USAGE_LOG_PROD_VAL_STATUS_PROVISIONAL_BITMAP |
                                                            TAPP_USAGE_LOG_PROD_VAL_STATUS_SCAN_ON_BITMAP     );
        }

        /*     [d.  Product:] */
        {
            /*      i.  Product: ProductIssuerId - as used in step 3 */
            pTAppUsageLog->isProductIssuerIdSet         = TRUE;
            pTAppUsageLog->productIssuerId              = pMYKI_Directory->IssuerId;

            /*      ii. ProductId - as used in step 3 */
            pTAppUsageLog->isProductIdSet               = TRUE;
            pTAppUsageLog->productId                    = pMYKI_Directory->ProductId;

            /*      iii.ProductSerialNo - as used in step 3 */
            pTAppUsageLog->isProductSerialNoSet         = TRUE;
            pTAppUsageLog->productSerialNo              = pMYKI_Directory->SerialNo;
        }

        /*  NOTE:   TAppUsageLog entry is added by application framework. */
        pData->InternalData.IsUsageLogUpdated           = TRUE;
        CsDbg( BRLL_RULE, "BR_LLSC_10_3 : Updated TAppUsageLog" );
    }

    CsDbg( BRLL_RULE, "BR_LLSC_10_3 : Executed" );
    return RULE_RESULT_EXECUTED;
}   /* BR_LLSC_10_3( ) */
