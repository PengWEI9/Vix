/* -*- mode: c; tabs: 4; -*- */
/*=============================================================================
**
**    Vix Technology                   Licensed software
**    (C) 2013                         All rights reserved
**
**=============================================================================
**
**  Project/Product : MBU
**  Filename        : BR_LLSC_10_6.c
**  Author(s)       : An Tran
**
**  ID              : BR_LLSC_10_6 - NTS0177 v7.2
**
**  Name            : Scan Off  Travel Access Pass - with Valid Provisional
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
**      15. Dynamic.UsageLog
**
**  Pre-Conditions  :
**
**      1.  This passenger code is a DDA passenger code.
**      2.  The product control bit map on the product in use indicates that
**          there is a provisional product
**      3.  All of the following are true:
**          a.  The provisional product expiry is greater than the current
**              date time. ie the product has not expired.
**          b.  If the mode of transport on the provisional is not equal to Rail
**              i.  The service provider of scan on is equal to the current
**                  service provider
**              ii. The entry point of the provisional is  equal to the current
**                  entry point
**          c.  The mode of transport of the provisional is Rail and the
**              current service provider mode of transport is Rail.
**
**  Description     :
**
**      1.  Perform a ProductSale/Upgrade on the provisional product
**          a.  Set the provisional bit on the product control bitmap to false
**          b.  Set the product expiry to the current date time - the product
**              will be expired.
**          c.  Using the current Route/Stop or Station ID set the Entry Point,
**              Route, and StopID
**          d.  Set the zone low to the lesser of the current location low zone
**              the last usage entry location zone low
**          e.  Set the zone high to the greater of the current location high
**              zone and the last usage entry location zone high
**      2.  If there is a product of type e-pass and that epass is not active - and
**          further there is no other product of type epass then Activate the
**          Inactive e-Pass by performing an Product Update/Activate transaction
**          on the e-pass product setting the:
**          a.  Start date equal to the scan on date time of the product in
**              use (ie. The provisional product):
**          b.  Calculate the expiry based upon the business day of the start
**              date time of the product in use
**          c.  Modify usage log
**              i.  Definition:
**                  (1) Set Usage - Product validation status Activated = bit 0 = true
**      3.  If there is a product of type e-pass that is activate
**          a.  Using the current location perform a ProductUseage/Scanoff
**              using the epass
**              i.  Set the last usage date time to the  current date time
**              ii. Using the current location to get the Route/Stop/Entry Point
**      4.  Else
**          a.  Using the current location perform a ProductUsage/Scan off
**              i.  Using the current location to get the Route/Stop/Entry Point
**      5.  Create usage log
**          a.  Definition:
**              i.  If the TXT Type is not set then set the TxType = Type
**                  (Entry Exit = 8, Exit Only =18; Entry Only =19, On board= 20),
**              ii. Determine UsageLog Control Bitmap by lookuping TxUsageType
**              iii.ProviderID =  same product ServiceProviderID
**              iv. TxDateTime = Current date time
**              v.  Location.EntryPoint = EntryPointID11
**              vi. Location.Route = LineID
**              vii.Location.Stop = Stopid
**          b.  Value:
**              i.  TxValue = 0
**              ii. NewTPurseBalance = current tpurse balance
**          c.  Usage
**              i.  Zone = Current zone
**              ii. ProvisionalValidationStatus = + Scan off
**          d.  Product:
**              i.  ProductIssuerId - as  used in step 2
**              ii. ProductId - as used in step 2
**              iii.ProductSerialNo - as used in step 2
**
**  Post-Conditions :
**
**      The provisional product has been scanned off
**
**  Member(s)       :
**      BR_LLSC_10_6            [public]    business rule
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
**    1.00  ??.??.13    ANT   Create
**    1.01  30.01.14    ANT   Modify   Set TAppUsageLog.PaymentMethod to
**                                     TAPP_USAGE_LOG_PAYMENT_METHOD_TPURSE to
**                                     mimic KAMCO reader behaviour
**    1.02  03.02.14    ANT   Modify   Rectified settting ProductSale/Upgrade
**                                     zone low/high
**                                     Rectified setting Epass EndDateTime
**    1.03  07.02.14    ANT   Modify   Implemented undocumented feature!
**                                     deriving ePass EndDateTime
**    1.04  01.03.14    ANT   Modify   Rectified selecting first found
**                                     inactive for scan-off
**    1.05  03.03.14    ANT   Modify   Rectified failing to activate ePass
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
**  BR_LLSC_10_6
**
**  Description     :
**      Implements business rule BR_LLSC_10_6.
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

RuleResult_e BR_LLSC_10_6( MYKI_BR_ContextData_t *pData )
{
    MYKI_TAControl_t       *pMYKI_TAControl             = NULL;
    MYKI_Directory_t       *pMYKI_Directory             = NULL;
  //MYKI_TAProduct_t       *pMYKI_TAProduct             = NULL;
    MYKI_Directory_t       *pMYKI_Directory_Provisional = NULL;
    MYKI_TAProduct_t       *pMYKI_TAProduct_Provisional = NULL;
    TAppUsageLog_t         *pTAppUsageLog               = NULL;
    MYKI_TAPurseBalance_t  *pMYKI_TAPurseBalance        = NULL;
    int                     i                           = 0;
    int                     iEPassInactive              = 0;
    int                     iEPassActive                = 0;
    int                     ZoneLow                     = 0;
    int                     ZoneHigh                    = 0;
  //Time_t                  StartDateTime               = TIME_NOT_SET;
  //Time_t                  EndDateTime                 = TIME_NOT_SET;
  //Time_t                  ProductDuration             = TIME_NOT_SET;
    int                     IsDDA                       = FALSE;
    TransportMode_e         TransportModeProvisional    = TRANSPORT_MODE_UNKNOWN;

    CsDbg( BRLL_RULE, "BR_LLSC_10_6 : Start (Scan Off  Travel Access Pass - with Valid Provisional)" );

    if ( pData == NULL )
    {
        CsErrx( "BR_LLSC_10_6 : Invalid argument(s)" );
        return RULE_RESULT_ERROR;
    }   /* end-of-if */

    if ( MYKI_CS_TAControlGet( &pMYKI_TAControl ) < 0 || pMYKI_TAControl == NULL )
    {
        CsErrx( "BR_LLSC_10_4 : MYKI_CS_TAControlGet() failed" );
        return RULE_RESULT_ERROR;
    }   /* end-of-if */

    /* PRE-CONDITIONS */
    {
        /*  1.  This passenger code is a DDA passenger code. */
        IsDDA   = myki_br_IsDDA( pMYKI_TAControl->PassengerCode );
        if ( IsDDA < 0 )
        {
            CsErrx( "BR_LLSC_10_6 : myki_br_IsDDA() failed" );
            return RULE_RESULT_ERROR;
        }   /* end-of-if */

        if ( IsDDA == FALSE )
        {
            CsDbg( BRLL_RULE, "BR_LLSC_10_6 : BYPASSED - passenger type is not DDA" );
            pData->Tariff.concessionTypeIsDDA   = FALSE;
            return RULE_RESULT_BYPASSED;
        }   /* end-of-if */

        /*  2.  The product control bit map on the product in use indicates that there is a provisional product */
        if ( pMYKI_TAControl->ProductInUse == 0 )
        {
            CsDbg( BRLL_RULE, "BR_LLSC_10_6 : BYPASSED - no product in use" );
            return RULE_RESULT_BYPASSED;
        }   /* end-of-if */

        if ( myki_br_GetCardProduct( pMYKI_TAControl->ProductInUse, &pMYKI_Directory_Provisional, &pMYKI_TAProduct_Provisional ) < 0 )
        {
            CsErrx( "BR_LLSC_10_6 : myki_br_GetCardProduct(%d) failed", i );
            return RULE_RESULT_ERROR;
        }   /* end-of-for */

        if ( ( pMYKI_TAProduct_Provisional->ControlBitmap & TAPP_TPRODUCT_CONTROL_PROVISIONAL_BITMAP ) == 0 )
        {
            CsDbg( BRLL_RULE, "BR_LLSC_10_6 : BYPASSED - product in use is not provisional product" );
            return RULE_RESULT_BYPASSED;
        }   /* end-of-if */

        /*  3.  All of the following are true: */
        {
            /*  a.  The provisional product expiry is greater than the current date time.
                    ie the product has not expired. */
            if ( pMYKI_TAProduct_Provisional->EndDateTime <= pData->DynamicData.currentDateTime )
            {
                CsDbg( BRLL_RULE, "BR_LLSC_10_6 : BYPASSED - provisional product expired" );
                return RULE_RESULT_BYPASSED;
            }   /* end-of-if */

            TransportModeProvisional    = myki_br_cd_GetTransportModeForProvider( pMYKI_TAProduct_Provisional->LastUsage.ProviderId );

            if ( TransportModeProvisional != TRANSPORT_MODE_RAIL )
            {
                /*  b.  If the mode of transport on the provisional is not equal to Rail
                        i.  The service provider of scan on is equal to the current service provider */
                if ( pData->StaticData.serviceProviderId != pMYKI_TAProduct_Provisional->LastUsage.ProviderId )
                {
                    CsDbg( BRLL_RULE, "BR_LLSC_10_6 : BYPASSED - scan-on provider (%d) not equal current provider (%d)",
                            pMYKI_TAProduct_Provisional->LastUsage.ProviderId,
                            pData->StaticData.serviceProviderId );
                    return RULE_RESULT_BYPASSED;
                }   /* end-of-if */

                /*      ii. The entry point of the provisional is  equal to the current entry point */
                if ( pData->DynamicData.entryPointId != pMYKI_TAProduct_Provisional->LastUsage.Location.EntryPointId )
                {
                    CsDbg( BRLL_RULE, "BR_LLSC_10_6 : BYPASSED - scan-on entry point (%d) not equal current entry point (%d)",
                            pMYKI_TAProduct_Provisional->LastUsage.Location.EntryPointId,
                            pData->DynamicData.entryPointId );
                    return RULE_RESULT_BYPASSED;
                }   /* end-of-if */
            }
            else
            {
                /*  c.  The mode of transport of the provisional is Rail and
                        the current service provider mode of transport is Rail. */
                if ( pData->InternalData.TransportMode != TRANSPORT_MODE_RAIL )
                {
                    CsDbg( BRLL_RULE, "BR_LLSC_10_6 : BYPASSED - current provider is not RAIL" );
                    return RULE_RESULT_BYPASSED;
                }   /* end-of-if */
            }   /* end-of-if */
        }
    }

    /* PROCESSING */
    {
        pData->Tariff.concessionTypeIsDDA   = TRUE;

        /*  4.  Perform a ProductSale/Upgrade on the provisional product
                a.  Set the provisional bit on the product control bitmap to false
                b.  Set the product expiry to the current date time - the product will be expired.
                c.  Using the current Route/Stop or Station ID set the Entry Point, Route, and StopID
                d.  Set the zone low to the lesser of the current location low zone [and]
                    the last usage entry location zone low
                e.  Set the zone high to the greater of the current location high zone and
                    the last usage entry location zone high */
        ZoneLow     = pMYKI_TAProduct_Provisional->LastUsage.Zone < pData->DynamicData.currentInnerZone ?
                            pMYKI_TAProduct_Provisional->LastUsage.Zone :
                            pData->DynamicData.currentInnerZone;
        ZoneHigh    = pMYKI_TAProduct_Provisional->LastUsage.Zone > pData->DynamicData.currentOuterZone ?
                            pMYKI_TAProduct_Provisional->LastUsage.Zone :
                            pData->DynamicData.currentOuterZone;
        if ( myki_br_ldt_ProductSale_Upgrade_SameProduct(
                pData,
                pMYKI_Directory_Provisional,
                ZoneLow,
                ZoneHigh,
                0,
                pData->DynamicData.currentDateTime ) < 0 )
        {
            CsErrx( "BR_LLSC_10_6 : myki_br_ldt_ProductSale_Upgrade_SameProduct() failed" );
            return RULE_RESULT_ERROR;
        }   /* end-of-if */

        for ( i = 1; i < DIMOF( pMYKI_TAControl->Directory ); i++ )
        {
            if ( pMYKI_TAControl->ProductInUse == i )
            {
                continue;   /* for-loop */
            }   /* end-of-if */

            if ( myki_br_GetCardProduct( i, &pMYKI_Directory, NULL ) < 0 )
            {
                CsErrx( "BR_LLSC_10_4 : myki_br_GetCardProduct(%d) failed", i );
                return RULE_RESULT_ERROR;
            }   /* end-of-for */

            if ( pMYKI_Directory->Status != TAPP_CONTROL_DIRECTORY_STATUS_ACTIVATED &&
                 pMYKI_Directory->Status != TAPP_CONTROL_DIRECTORY_STATUS_ISSUED    )
            {
                /* Product is neither inactive nor active */
                continue;   /* for-loop */
            }   /* end-of-if */

            if ( myki_br_cd_GetProductType( pMYKI_Directory->ProductId ) != PRODUCT_TYPE_EPASS )
            {
                /* Product is not of type E-Pass */
                continue;   /* for-loop */
            }   /* end-of-if */

            if ( pMYKI_Directory->Status == TAPP_CONTROL_DIRECTORY_STATUS_ISSUED )
            {
                /* Found an inactive E-Pass product, continue checking for active E-Pass */
                if ( iEPassInactive == 0 )
                {
                    /* And selects this E-Pass for scan-off if no active E-Pass found */
                    iEPassInactive  = i;
                }   /* end-of-if */
            }
            else
            {
                /* Found an active E-Pass product, use it */
                iEPassActive    = i;
                break;  /* out-of-for */
            }   /* end-of-if */
        }   /* end-of-for */

        pTAppUsageLog   = &pData->InternalData.UsageLogData;

        /*  5.  If there is a product of type e-pass and that epass is not active -
                and further there is no other [active] product of type epass */
        if ( iEPassInactive > 0 && iEPassActive == 0 )
        {
            /*  then Activate the Inactive e-Pass by performing an Product Update/Activate
                transaction on the e-pass product setting the:
                a.  Start date equal to the scan on date time of the product in use (ie. The provisional product):
                b.  Calculate the expiry based upon the business day of the start date time of the product in use */
            if ( myki_br_GetCardProduct( iEPassInactive, &pMYKI_Directory, NULL ) < 0 )
            {
                CsErrx( "BR_LLSC_10_6 : myki_br_GetCardProduct(%d) failed", iEPassInactive );
                return RULE_RESULT_ERROR;
            }   /* end-of-for */

            if ( myki_br_ldt_ProductUpdate_ActivateFrom( pData, pMYKI_Directory, pMYKI_TAProduct_Provisional->LastUsage.DateTime ) < 0 )
            {
                CsErrx( "BR_LLSC_10_6 : myki_br_ldt_ProductUpdate_ActivateEx() failed" );
                return RULE_RESULT_ERROR;
            }   /* end-of-if */
            iEPassActive    = iEPassInactive;

            /*  c.  Modify usage log
                    i.  Definition:
                        (1) Set Usage - Product validation status Activated = bit 0 = true */
            {
                pTAppUsageLog->isProductValidationStatusSet = TRUE;
                pTAppUsageLog->productValidationStatus     |= TAPP_USAGE_LOG_PROD_VAL_STATUS_ACTIVATE_BITMAP;
            }
        }   /* end-of-if */

        if ( iEPassActive > 0 )
        {
            /*  6.  If there is a product of type e-pass that is activate
                    a.  Using the current location perform a ProductUseage/Scanoff using the epass
                        i.  Set the last usage date time to the  current date time
                        ii. Using the current location to get the Route/Stop/Entry Point */
            if ( myki_br_GetCardProduct( iEPassActive, &pMYKI_Directory, NULL ) < 0 )
            {
                CsErrx( "BR_LLSC_10_6 : myki_br_GetCardProduct(%d) failed", iEPassActive );
                return RULE_RESULT_ERROR;
            }   /* end-of-for */
            if ( myki_br_ldt_ProductUsage_ScanOff( pData, pMYKI_Directory ) < 0 )
            {
                CsErrx( "BR_LLSC_10_6 : myki_br_ldt_ProductUsage_ScanOff() epass failed" );
                return RULE_RESULT_ERROR;
            }   /* end-of-if */
        }
        else
        {
            /*  7.  Else
                    a.  Using the current location perform a ProductUsage/Scan off
                        i.  Using the current location to get the Route/Stop/Entry Point */
            if ( myki_br_ldt_ProductUsage_ScanOff( pData, pMYKI_Directory_Provisional ) < 0 )
            {
                CsErrx( "BR_LLSC_10_6 : myki_br_ldt_ProductUsage_ScanOff() provisional failed" );
                return RULE_RESULT_ERROR;
            }   /* end-of-if */
            pMYKI_Directory                     = pMYKI_Directory_Provisional;
        }   /* end-of-if */

        /*  8.  Create usage log
                a.  Definition: */
        {
            /*      i.  If the TXT Type is not set then set the TxType = Type
                        (Entry Exit = 8, Exit Only =18; Entry Only =19, On board= 20), */
            if ( pTAppUsageLog->transactionType == MYKI_BR_TRANSACTION_TYPE_NONE )
            {
                pTAppUsageLog->transactionType          = myki_br_getTransactionType( pData );
            }   /* end-of-if */

            /*      ii. Determine UsageLog Control Bitmap by lookuping TxUsageType (Done by framework)
                    iii.ProviderID =  same product ServiceProviderID (Done by framework)
                    iv. TxDateTime = Current date time (Done by framework)
                    v.  Location.EntryPoint = EntryPointID (Done by framework)
                    vi. Location.Route = LineID (Done by framework)
                    vii.Location.Stop = Stopid  (Done by framework) */
        }

        /*      b. Value: */
        {
            /*      i.  TxValue = 0 */
            pTAppUsageLog->isTransactionValueSet        = TRUE;
            pTAppUsageLog->transactionValue             = 0;

            /*      ii. NewTPurseBalance = current tpurse balance */
            if ( MYKI_CS_TAPurseBalanceGet( &pMYKI_TAPurseBalance ) < 0 || pMYKI_TAPurseBalance == NULL )
            {
                CsErrx( "BR_LLSC_10_6 : MYKI_CS_TAPurseBalanceGet() failed" );
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

        /*      c.  Usage */
        {
            /*      i.  Zone = Current zone */
            pTAppUsageLog->isZoneSet                    = TRUE;
            pTAppUsageLog->zone                         = pData->DynamicData.currentZone;

            /*      ii. ProvisionalValidationStatus = + Scan off */
            pTAppUsageLog->isProductValidationStatusSet = TRUE;
            pTAppUsageLog->productValidationStatus     |= TAPP_USAGE_LOG_PROD_VAL_STATUS_SCAN_OFF_BITMAP;
        }

        /*      d.  Product: */
        {
            /*      i.  ProductIssuerId - as  used in step 2 */
            pTAppUsageLog->isProductIssuerIdSet         = TRUE;
            pTAppUsageLog->productIssuerId              = pMYKI_Directory->IssuerId;

            /*      ii. ProductId - as used in step 2 */
            pTAppUsageLog->isProductIdSet               = TRUE;
            pTAppUsageLog->productId                    = pMYKI_Directory->ProductId;

            /*      iii.ProductSerialNo - as used in step 2 */
            pTAppUsageLog->isProductSerialNoSet         = TRUE;
            pTAppUsageLog->productSerialNo              = pMYKI_Directory->SerialNo;
        }

        /*  NOTE:   TAppUsageLog entry is added by application framework. */
        pData->InternalData.IsUsageLogUpdated           = TRUE;
        CsDbg( BRLL_RULE, "BR_LLSC_10_6 : Updated TAppUsageLog" );
    }

    CsDbg( BRLL_RULE, "BR_LLSC_10_6 : Executed" );
    return RULE_RESULT_EXECUTED;
}   /* BR_LLSC_10_6( ) */
