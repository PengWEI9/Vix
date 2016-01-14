/* -*- mode: c; tabs: 4; -*- */
/*=============================================================================
**
**    Vix Technology                   Licensed software
**    (C) 2013                         All rights reserved
**
**=============================================================================
**
**  Project/Product : MBU
**  Filename        : BR_LLSC_7_3.c
**  Author(s)       : ??
**
**  ID              : BR_LLSC_7_3 - NTS0177 v7.2
**
**  Name            : Force Scan off  - $0 provisional product
**
**  Data Fields     :
**
**      1.  TAppControl.ProductInUse
**      2.  TAppTProduct.ControlBitmap
**      3.  TAppTProduct.ZoneLow
**      4.  TAppTProduct.ZoneHigh
**      5.  Tariff.CitySaverZoneFlag
**      6.  TAppTProudct.EndDateTime
**      7.  Dynamic.CurrentTripDirection
**      8.  Dynamic.IsForcedScanOff
**      9.  Dynamic. ForcedScanOffDate
**      10. Dynamic.CurrentdateTime
**      11. Dynamic.EntryPointID
**      12. Dynamic.LineID
**      13. Dynamic.StopID
**      14. TAppTProduct.StartDateTime
**      15. Static.ServiceProviderID
**      16. Dynamic.Zone
**      17. TAppTPurse.CurrentTPurseBalance
**      18. Dynamic.UsageLog
**      19. Dynamic.CurrentTripZoneLow
**      20. Dynamic.CurrentTripZoneHigh
**
**  Pre-Conditions  :
**
**      1.  The product in use field(1) is not 0 (a product is in a scanned-on state)
**      2.  The provisional bit on the product Control bit map(2) of the product in use is true(1)
**      3.  The product purchase value on this product is $0.00 (ie zero value reserved provisional amount)
**      4.  The Provisional Capping Option is not set to 'Option 3'
**
**  Description     :
**
**      1.  Set IsForcedScanOff(15) = True - ie this is a force scan off sequence
**      2.  Set ForcedScanOffDate(16) to the scan on date(14) of the product in use(1).
**      3.  Set the current trip low zone(19) to the provisional low zone(3).
**      4.  Set the current trip high zone(20) to the provisional high zone(4)
**      5.  If are currently no products of type e-Pass with a Product control status of activated;
**          a.  For each zone between the provisional zone low(5) to provisional zone high(6) check to see if there
**              is an inactive e-pass that covers this zone and where start date time(14) is before or equal to the
**              scan on date(14) of the product in use(1)
**              i.  Activate the Inactive e-Pass by performing an Product Update/Activate transaction on the e-
**                  pass product setting the:
**                  (1) Start date equal to the scan on date time(14) of the product in use (ie. The provisional
**                      product):
**                  (2) Calculate the expiry based upon the business day of the start date time(14) of the product
**                      in use
**                  (3) Modify usage log
**                      (a) Definition:
**                          1.  Set Usage - Product validation status Activated = bit 0 = true
**
**      6.  If there is an activated e-pass product who's zone range covers the force scan off trip zone range
**          entirely such that both the following are true:
**          a.  the e-pass zone low(3) is less than or equal the provisional zone low,
**          b.  and the  epass zone high(4) is greater than or equal to the provisional product zone high then .
**          c.  If such a product does exist then:
**              i.  Perform product update/delete on the product in use.
**              ii. Set this product as the product in use(1) using a TAppUpdate/SetProductInUse transaction
**
**      7. If the product in use is a n-hour provisional product then
**         a.   Perform a ProductSale/Upgrade Transaction on the product in use setting the:
**              i.  Product purchase value to 0
**              ii. Zone Low to the provisional low zone(3)
**              iii.Zone High to the provisional high zone(4).
**              iv. Expiry Date Time  to the lesser of the: current date time(10) or the Product Expiry(6) - ie which
**                  one occurred first
**              v.  Set the provisional bit false.
**              vi. Entry Point ID to the entry point ID of the current device(11)
**              vii.Route ID to the current route ID of the device(12)
**              viii.Stop ID to the current stop id of the device(13)
**
**         b.   If an e-pass was activated at step 3a set then product in use(1) to undefined using a
**              TAppUpdate/SetProductInUse
**
**  Post-Conditions :
**
**      The provisional product has been 'sold'
**      And is ready for penalty scan off
**
**  Devices         :
**
**      Fare payment devices.
**
**  Member(s)       :
**      BR_LLSC_7_3             [public]    business rule
**
**  Information     :
**   Compiler(s)    : C
**   Target(s)      : Independent
**
**  History         :
**   Vers.  Date        Aut.  Type     Description
**   -----  ----------  ----  -------  ----------------------------------------
**    1.00  ??.??.??    ???   Create
**    1.01  24.01.14    ANT   Modify   MBU-664: Rectified as per NTS0177v7.2
**    1.02  07.02.14    ANT   Modify   Implemented undocumented feature!
**                                     deriving ePass EndDateTime
**    1.03  26.02.14    ANT   Modify   Rectified incorrect testing of ePass
**                                     StartDateTime against scan-on date/time
**
**===========================================================================*/

#include <cs.h>                 // For debug logging
#include <myki_cardservices.h>  // Myki Card Services
#include <LDT.h>                // Logical Device Transaction

#include "myki_br_rules.h"      // API for all BR_xxx modules
#include "BR_Common.h"          // Common utilities, macros, etc

/*==========================================================================*
**
**  BR_LLSC_7_3
**
**  Description     :
**      Implements business rule BR_LLSC_7_3.
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

RuleResult_e BR_LLSC_7_3( MYKI_BR_ContextData_t *pData )
{
    MYKI_TAControl_t            *pMYKI_TAControl            = NULL;
    MYKI_Directory_t            *pDirectoryInUse            = NULL;
    MYKI_TAProduct_t            *pProductInUse              = NULL;         // Provisional scanned-on product
    MYKI_Directory_t            *pDirectory                 = NULL;
    MYKI_TAProduct_t            *pProduct                   = NULL;         // Candidate product for activation to replace provisional product
    int                         activatedProducts           = 0;
    int                         activatedProductDirIndex    = -1;
    int                         i                           = 0;
  //Time_t                      StartDateTime               = TIME_NOT_SET;
    Time_t                      expiryDateTime              = TIME_NOT_SET;
  //Time_t                      productDuration             = TIME_NOT_SET;

    CsDbg( BRLL_RULE, "BR_LLSC_7_3 : Start (Force Scan off  - $0 provisional product)" );

    if ( ! pData )
    {
        CsErrx( "BR_LLSC_7_3 : Invalid argument(s)" );
        return RULE_RESULT_ERROR;
    }

    if ( MYKI_CS_TAControlGet( &pMYKI_TAControl ) < 0 )
    {
        CsErrx( "BR_LLSC_7_3 : MYKI_CS_TAControlGet() failed" );
        return RULE_RESULT_ERROR;
    }

    //  1. The product in use field(1) is not 0 (a product is in a scanned-on state).

    if ( pMYKI_TAControl->ProductInUse == 0 )
    {
        CsDbg( BRLL_RULE, "BR_LLSC_7_3 : Bypass - No products in use" );
        return RULE_RESULT_BYPASSED;
    }

    //  Get a pointer to the product in use

    if ( myki_br_GetCardProduct( pMYKI_TAControl->ProductInUse, &pDirectoryInUse, &pProductInUse ) < 0 )
    {
        CsErrx( "BR_LLSC_7_3 : myki_br_GetCardProduct(%d) failed", pMYKI_TAControl->ProductInUse );
        return RULE_RESULT_ERROR;
    }

    //  2. The provisional bit on the product control bitmap(2) of the product in use(1) is set to 1.

    if ( ! ( pProductInUse->ControlBitmap & TAPP_TPRODUCT_CONTROL_PROVISIONAL_BITMAP ) )
    {
        CsDbg( BRLL_RULE, "BR_LLSC_7_3 : Bypass - Product Provisional Bit not set" );
        return RULE_RESULT_BYPASSED;
    }

    //  3.  The product purchase value on this product is $0.00 (ie zero value reserved provisional amount)

    if ( pProductInUse->PurchaseValue != 0 )
    {
        CsDbg( BRLL_RULE, "BR_LLSC_7_3 : Bypass - Purchase Value (%d) not 0", pProductInUse->PurchaseValue );
        return RULE_RESULT_BYPASSED;
    }

    //  4.  The Provisional Capping Option is not set to 'Option 3'

    if ( pData->Tariff.provisionalCappingOption == PROVISIONAL_CAPPING_OPTION_3 )
    {
        CsDbg( BRLL_RULE, "BR_LLSC_7_3 : Bypass - Provisional Capping option == 3" );
        return RULE_RESULT_BYPASSED;
    }

    //  1.  Set IsForcedScanOff(15) = True - ie this is a force scan off sequence
    //  2.  Set ForcedScanOffDate(16) to the scan on date(14) of the product in use(1).

    pData->DynamicData.isForcedScanOff          = TRUE;
    pData->DynamicData.forcedScanOffDateTime    = pProductInUse->LastUsage.DateTime;              // KWS: Check: NTS0177 says "ForcedScanOffDate" not DateTime

    //  3.  Set the current trip low zone(19) to the provisional low zone(3).
    //  4.  Set the current trip high zone(20) to the provisional high zone(4)

    CsDbg( BRLL_RULE, "BR_LLSC_7_3 : Setting Dynamic.CurrentTripZone(%d,%d) to (%d,%d)",
        pData->DynamicData.currentTripZoneLow, pData->DynamicData.currentTripZoneHigh,
        pProductInUse->ZoneLow, pProductInUse->ZoneHigh );
    pData->DynamicData.currentTripZoneLow       = pProductInUse->ZoneLow;
    pData->DynamicData.currentTripZoneHigh      = pProductInUse->ZoneHigh;

    //  5.  If are currently no products of type e-Pass with a Product control status of activated;
    //  Precursor to step 5...

    activatedProductDirIndex = -1;  // So we can check later if a product was activated as part of this step

    activatedProducts = 0;
    for ( i = 1; i < DIMOF( pMYKI_TAControl->Directory ); i++ )
    {
        if ( myki_br_GetCardProduct( i, &pDirectory, NULL ) < 0 )
        {
            CsErrx( "BR_LLSC_7_3 : myki_br_GetCardProduct(%d) failed", i );
            return RULE_RESULT_ERROR;
        }

        if
        (
            ( pDirectory->Status == TAPP_CONTROL_STATUS_ACTIVATED ) &&
            ( myki_br_cd_GetProductType( pDirectory->ProductId ) == PRODUCT_TYPE_EPASS )
        )
        {
            activatedProducts++;
            break;
        }
    }

    if ( activatedProducts == 0 )
    {
        //  5.  If are currently no products of type e-Pass with a Product control status of activated;
        //      a.  For each zone between the provisional zone low(5[3]) to provisional zone high(6[4]) check to see if there
        //          is an inactive e-pass that covers this zone and where start date time(14) is before or equal to the
        //          scan on date(14) of the product in use(1)
        //

        for ( i = 1; i < DIMOF( pMYKI_TAControl->Directory ); i++ )
        {
            if ( myki_br_GetCardProduct( i, &pDirectory, NULL ) < 0 )
            {
                CsErrx( "BR_LLSC_7_3 : myki_br_GetCardProduct(%d) failed", i );
                return RULE_RESULT_ERROR;
            }

            if
            (
                ( pDirectory->Status == TAPP_CONTROL_STATUS_ISSUED ) &&
                ( myki_br_cd_GetProductType( pDirectory->ProductId ) == PRODUCT_TYPE_EPASS )
            )
            {
                if ( myki_br_GetCardProduct( i, &pDirectory, &pProduct ) < 0 )
                {
                    CsErrx( "BR_LLSC_7_3 : myki_br_GetCardProduct(%d) failed", i );
                    return RULE_RESULT_ERROR;
                }

                if
                (
                    pProduct->StartDateTime <= pProductInUse->StartDateTime &&
                    (
                        //  Partially covered product
                        ( pProduct->ZoneLow  >= pProductInUse->ZoneLow && pProduct->ZoneLow  <= pProductInUse->ZoneHigh ) ||
                        ( pProduct->ZoneHigh >= pProductInUse->ZoneLow && pProduct->ZoneHigh <= pProductInUse->ZoneHigh ) ||
                        //  Fully covered product
                        ( pProduct->ZoneLow  <= pProductInUse->ZoneLow && pProduct->ZoneHigh >= pProductInUse->ZoneHigh )
                    )
                )
                {
                    //
                    //  5.i.  Activate the Inactive e-Pass by performing an Product Update/Activate transaction on the e-
                    //        pass product setting the:
                    //       (1) Start date equal to the scan on date time(14) of the product in use (ie. The provisional
                    //           product):
                    //       (2) Calculate the expiry based upon the business day of the start date time(14) of the product
                    //           in use
                    //

                    if ( myki_br_ldt_ProductUpdate_ActivateFrom( pData, pDirectory, pProductInUse->LastUsage.DateTime ) < 0 )
                    {
                        CsErrx( "BR_LLSC_7_3 : myki_br_ldt_ProductUpdate_Activate(%d) failed", i );
                        return RULE_RESULT_ERROR;
                    }
                    activatedProductDirIndex = i;

                    //       (3)  Modify usage log
                    //           (a)  Definition:
                    //                 1.  Set Usage - Product validation status Activated = bit 0 = true
                    //
                    pData->InternalData.IsUsageLogUpdated = TRUE;
                    pData->InternalData.UsageLogData.isProductValidationStatusSet = TRUE;
                    pData->InternalData.UsageLogData.productValidationStatus |= TAPP_USAGE_LOG_PROD_VAL_STATUS_ACTIVATE_BITMAP;

                    // Stop after the first product that matches the criteria
                    break;
                }
            }
        }
    }
    else
    {
        //   6. If there is an activated e-pass product who's zone range covers the force scan off trip zone range
        //      entirely such that both the following are true:
        //
        for ( i = 1; i < DIMOF( pMYKI_TAControl->Directory ); i++ )
        {
            if ( myki_br_GetCardProduct( i, &pDirectory, NULL ) < 0 )
            {
                CsErrx( "BR_LLSC_7_3 : myki_br_GetCardProduct(%d) failed", i );
                return RULE_RESULT_ERROR;
            }

            if
            (
                ( pDirectory->Status == TAPP_CONTROL_STATUS_ACTIVATED ) &&
                ( myki_br_cd_GetProductType( pDirectory->ProductId ) == PRODUCT_TYPE_EPASS )
            )
            {
                if ( myki_br_GetCardProduct( i, &pDirectory, &pProduct ) < 0 )
                {
                    CsErrx( "BR_LLSC_7_3 : myki_br_GetCardProduct(%d) failed", i );
                    return RULE_RESULT_ERROR;
                }

                //
                //      a.  the e-pass zone low(3) is less than or equal the provisional zone low,
                //      b.  and the  epass zone high(4) is greater than or equal to the provisional product zone high then .
                //
                if ( pProduct->ZoneLow  <= pProductInUse->ZoneLow  &&
                     pProduct->ZoneHigh >= pProductInUse->ZoneHigh )
                {
                    //
                    //  c.  If such a product does exist then:
                    //      i.   Perform product update/delete on the product in use.
                    //      ii.  Set this product as the product in use(1) using a TAppUpdate/SetProductInUse transaction
                    //
                    if ( myki_br_ldt_ProductUpdate_Invalidate( pData, pDirectoryInUse ) < 0 )            // Assumes "delete" means "invalidate"
                    {
                        CsErrx( "BR_LLSC_7_3 : myki_br_ldt_ProductUpdate_Delete() failed" );
                        return RULE_RESULT_ERROR;
                    }

                    if ( myki_br_ldt_AppUpdate_SetProductInUse( pData, pDirectory ) < 0 )
                    {
                        CsErrx( "BR_LLSC_7_3 : myki_br_ldt_AppUpdate_SetProductInUse() failed" );
                        return RULE_RESULT_ERROR;
                    }

                    pDirectoryInUse     = pDirectory;
                    pProductInUse       = pProduct;

                    break;
                }
            }
        }
    }

    //   7. If the product in use is a n-hour provisional product then
    //      a.  Perform a ProductSale/Upgrade Transaction on the product in use setting the:
    //          i.      Product purchase value to 0
    //          ii.     Zone Low to the provisional low zone(3)
    //          iii.    Zone High to the provisional high zone(4).
    //          iv.     Expiry Date Time  to the lesser of the: current date time(10) or the Product Expiry(6) - ie which
    //                  one occurred first
    //          v.      Set the provisional bit false.
    //          vi.     Entry Point ID to the entry point ID of the current device(11)
    //          vii.    Route ID to the current route ID of the device(12)
    //          viii.   Stop ID to the current stop id of the device(13)
    //

    if ( myki_br_cd_GetProductType( pDirectoryInUse->ProductId ) == PRODUCT_TYPE_NHOUR )
    {
        if ( pData->DynamicData.currentDateTime < pProductInUse->EndDateTime )
            expiryDateTime = pData->DynamicData.currentDateTime;
        else
            expiryDateTime = pProductInUse->EndDateTime;

        if ( myki_br_ldt_ProductSale_Upgrade_SameProduct( pData, pDirectoryInUse,
                pProductInUse->ZoneLow, pProductInUse->ZoneHigh,
                0, expiryDateTime ) < 0 )
        {
            CsErrx( "BR_LLSC_7_3 : myki_br_ldt_ProductSale_Upgrade_SameProduct() failed" );
            return RULE_RESULT_ERROR;
        }
    }

    //
    //  b.  If an e-pass was activated at step 3a set then product in use(1) to undefined using a
    //      TAppUpdate/SetProductInUse
    //
    if ( activatedProductDirIndex >= 0 )
    {
        if ( myki_br_ldt_AppUpdate_SetProductInUse( pData, NULL ) < 0 )
        {
            CsErrx( "BR_LLSC_7_3 : myki_br_ldt_AppUpdate_SetProductInUse() failed" );
            return RULE_RESULT_ERROR;
        }
    }

    CsDbg( BRLL_RULE, "BR_LLSC_7_3 : Executed" );
    return RULE_RESULT_EXECUTED;
}

