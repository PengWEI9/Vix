/**************************************************************************
*   ID :  BR_LLSC_7_4
*
*   Expired Tariff Full Product Coverage
*
*   Data Fields
*   1.  TAppControl.ProductInUse
*   2.  TAppControl.ProvisionalFare
*   3.  TAppControl.PassengerCode
*   4.  TAppControl.Directory.ProductId
*   5.  TAppTProduct.ControlBitmap
*   6.  TAppTProduct.ProductPurchaseValue
*   7.  Dynamic.ProvisionalZoneLow
*   8.  Dynamic.ProvisionalZoneHigh
*   9.  Dynamic.CurrentTripZoneLow
*   10. Dynamic.CurrentTripZoneHigh
*   11. Tariff.BaseFare
*   12. Tariff.ApplicableFare
*   13. Tariff.ProvisionalCappingOption
*   14. Dynamic.IsForcedScanOff
*   15. Dynamic.ForcedScanOffDate
*   16. TAppTProduct.StartDateTime
*   17. Dynamic.CurrentZone
*   18. Dynamic.CurrentDateTime
*   19. Static.ServiceProviderID
*   20. Dynamic.EntryPointID
*   21. Dynamic.LineID
*   22. Dynamic.StopID
*
*   Pre-Conditions
*   1.  The product in use field(1) is not 0 (a product is in a scanned-on state).
*   2.  The Provisional Capping Option is not set to ‘Option 3’
*   3.  The Tariff does not have valid tariff data scan on date(14) of the product in use(1).
*   4.  The provisional bit on the product Control bit map(2) of the product in use is true(1)
*   5.  The product purchase value on this product is not $0.00 (ie not zero value reserved provisional amount)
*   6.  For the provisional product zone range (zone low to zone high) verify that
*       every zone has product coverage as follows:
*       a.  An activated product (that is not the provisional product) covers each zone
*       b.  There is currently no product of type epass active; and a product of type epass that is inactive epass exists that:
*           i.  covers one or more of the zones of the provisional product.
*           ii. where start date time(14) is before than the scan on date(14) of the product in use(1)
*
*   Description
*   1.  Set IsForcedScanOff(12) = True - ie this is a force scan off sequence
*   2.  Set ForcedScanOffDate(16) to the valid from date of the provisional product(16).
*   3.  If an inactive epass was used as part of precondition 6b then:
*   a.  Activate the Inactive e-Pass by performing an Product Update/Activate transaction on the e-pass product setting the:
*       i.  Start date equal to the scan on date time(14) of the product in use (ie. The provisional product):
*       ii. Calculate the epass expiry based upon the business day of the start date time(14) of the product in use
*       iii.    Modify usage log
*           (1) Definition:
*           (2) Set Usage - Product validation status Activated = bit 0 = true
*   4.  Perform ProductUpdate/invalidate transaction on the product in use(1) if the provisional bit on product control bitmap(2) is set.
*   5.  For each activated valid product  on the card check to see if a single product covers the current trip zone range(5),(6)
*       a.  If such a product does exist, set this product as the product in use(1) using a TAppUpdate/SetProductInUse transaction
*       b.  Else set the product in use(1) to undefined using a TAppUpdate/SetProductInUse
*
*   Post-Conditions
*   1.  The provisional product has been invalidated, and ready for product usage and scan off.
*
*   Devices
*   Fare payment devices
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

RuleResult_e BR_LLSC_7_4( MYKI_BR_ContextData_t *pData )
{
    MYKI_TAControl_t                            *pMYKI_TAControl;
    MYKI_Directory_t                            *pDirectoryInUse = NULL;
    MYKI_TAProduct_t                            *pProductInUse   = NULL;
    MYKI_Directory_t                            *pDirectory;
    MYKI_TAProduct_t                            *pProduct;
    int                                          dir;
    int                                          inactiveEpassDir;
    myki_br_CheckProvisionProductZoneCoverage_t  zoneCoverage;

    CsDbg( BRLL_RULE, "BR_LLSC_7_4 : Start (Expired Tariff Partial Product Coverage)" );

    if ( ! pData )
    {
        CsErrx( "BR_LLSC_7_4 : Invalid argument(s)" );
        return RULE_RESULT_ERROR;
    }

    if ( MYKI_CS_TAControlGet( &pMYKI_TAControl ) < 0 )
    {
        CsErrx( "BR_LLSC_7_4 : MYKI_CS_TAControlGet() failed" );
        return RULE_RESULT_ERROR;
    }

    //   Pre-Conditions
    //   1. The product in use field(1) is not 0 (a product is in a scanned-on state).
    //
    if (pMYKI_TAControl->ProductInUse == 0)
    {
        CsDbg( BRLL_RULE, "BR_LLSC_7_4 : Bypass - No products in use" );
        pData->ReturnedData.bypassCode = 1;
        return RULE_RESULT_BYPASSED;
    }

    //  Get product and directory structure in use
    if ( myki_br_GetCardProduct( pMYKI_TAControl->ProductInUse, &pDirectoryInUse, &pProductInUse ) < 0 )
    {
        CsErrx( "BR_LLSC_7_4 : myki_br_GetCardProduct() failed" );
        return RULE_RESULT_ERROR;
    }

    //
    //   2. The Provisional Capping Option is not set to ‘Option 3’
    //
    if ( pData->Tariff.provisionalCappingOption == PROVISIONAL_CAPPING_OPTION_3 )
    {
        CsDbg( BRLL_RULE, "BR_LLSC_7_4 : Bypass - Provisional Capping Option = 3" );
        pData->ReturnedData.bypassCode = 2;
        return RULE_RESULT_BYPASSED;
    }

    //
    //   3. The Tariff does not have valid tariff data scan on date(14) of the product in use(1).
    //
    //   Clarification from Brendon.
    //   The Valid Tariff Data test is based on the StartDate in the header of the CD XML
    //   Tariff is not valid if the card has been written before the Tariff Start Date
    // 
    //   Implementation Note: Confusing double negative
    //      If we have a valid tarif then the prerequisite is NOT statisfied.
    //      This rule deals with the case where the tariff is invalid
    //

    if ( MYKI_CD_getStartDateTime() <= pProductInUse->LastUsage.DateTime )
    {
        CsDbg( BRLL_RULE, "BR_LLSC_7_4 : Bypass - Valid Tariff");
        pData->ReturnedData.bypassCode = 3;
        return RULE_RESULT_BYPASSED;
    }

    //
    //  4.  The provisional bit on the product Control bit map(2) of the product in use is true(1)
    //
    if ( ( pProductInUse->ControlBitmap & TAPP_TPRODUCT_CONTROL_PROVISIONAL_BITMAP ) == 0 )
    {
        CsDbg( BRLL_RULE, "BR_LLSC_7_4 : Bypass - ProductInUse is not a provisional product");
        pData->ReturnedData.bypassCode = 4;
        return RULE_RESULT_BYPASSED;
    }

    //
    //   5. The product purchase value on this product is not $0.00 (ie not zero value reserved provisional amount)
    //
    if ( pProductInUse->PurchaseValue == 0 )
    {
        CsDbg( BRLL_RULE, "BR_LLSC_7_4 : Bypass - ProductInUse provisional product fare is zero");
        pData->ReturnedData.bypassCode = 5;
        return RULE_RESULT_BYPASSED;
    }

    //  6.  For the provisional product zone range (zone low to zone high) verify that
    //      every zone has product coverage as follows:
    //      a.  An activated product (that is not the provisional product) covers each zone
    //      b.  There is currently no product of type epass active; and a product of type epass that is inactive epass exists that:
    //          i.  covers one or more of the zones of the provisional product.
    //          ii. where start date time(14) is before than the scan on date(14) of the product in use(1)
    //
    zoneCoverage = myki_br_CheckProvisionProductZoneCoverage(pData, pMYKI_TAControl, pProductInUse, &inactiveEpassDir);
    if ( zoneCoverage == CHECK_PROVISION_PRODUCTZONE_COVERAGE_ERROR )
    {
        CsErrx( "BR_LLSC_7_4 : myki_br_CheckProvisionProductZoneCoverage() failed" );
        return RULE_RESULT_ERROR;
    }
    else if ( zoneCoverage == CHECK_PROVISION_PRODUCTZONE_COVERAGE_PARTIAL )
    {
        CsDbg( BRLL_RULE, "BR_LLSC_7_4 : Bypass - Partial coverage detected");
        pData->ReturnedData.bypassCode = 6;
        return RULE_RESULT_BYPASSED;
    }
    else if ( zoneCoverage == CHECK_PROVISION_PRODUCTZONE_COVERAGE_FULL )
    {
        //  Full coverage detected
        //  Indicate (to Test Harness) that all preconditions have been met
        pData->ReturnedData.bypassCode = 7;
    }
    else
    {
        //  Full coverage possible - if we use inactive ePass
        //  Indicate (to Test Harness) that all preconditions have been met
        pData->ReturnedData.bypassCode = 8;
    }

    //----------------------------------------------------------------------------
    //
    //  Description
    //  1.  Set IsForcedScanOff(12) = True - ie this is a force scan off sequence
    //
    pData->DynamicData.isForcedScanOff = TRUE;

    //
    //  2.  Set ForcedScanOffDate(16) to the valid from date of the provisional product(16).
    //
    pData->DynamicData.forcedScanOffDateTime = pProductInUse->StartDateTime;

    //
    //  3.  If an inactive epass was used as part of precondition 6b then:
    //  a.  Activate the Inactive e-Pass by performing an Product Update/Activate transaction
    //      on the e-pass product setting the:
    //      i.  Start date equal to the scan on date time(14) of the product in use (ie. The provisional product):
    //      ii. Calculate the epass expiry based upon the business day of the start date time(14) of the product in use
    //      iii.    Modify usage log
    //          (1) Definition:
    //          (2) Set Usage - Product validation status Activated = bit 0 = true
    //

    if ( zoneCoverage == CHECK_PROVISION_PRODUCTZONE_COVERAGE_FULL_WITH_INACTIVE_EPASS )
    {
        //
        //  The directory entry of the Inactive ePass is known and stored in
        //  inactiveEpassDir.
        //

        //
        //  Note: The myki_br_ldt_ProductUpdate_Activate routine is used elsewhere
        //        where the same operation is required. ie: Activate and set
        //        start date, expiry date and modify usage log.
        //
        if ( myki_br_ldt_ProductUpdate_Activate( pData, &pMYKI_TAControl->Directory[ inactiveEpassDir ] ) < 0 )
        {
            CsErrx( "BR_LLSC_7_4 : myki_br_ldt_ProductUpdate_Activate() failed" );
            return RULE_RESULT_ERROR;
        }

        // Modify Usage Log
        pData->InternalData.UsageLogData.isProductValidationStatusSet = TRUE;
        pData->InternalData.UsageLogData.productValidationStatus |= TAPP_USAGE_LOG_PROD_VAL_STATUS_ACTIVATE_BITMAP;
    }

    //
    //  4.  Perform ProductUpdate/invalidate transaction on the product in use(1)
    //      if the provisional bit on product control bitmap(2) is set.
    //

    //  Refresh the data - this time we get the DirectoryInUse information too
    //  Get product and directory structure in use
    if ( myki_br_GetCardProduct( pMYKI_TAControl->ProductInUse, &pDirectoryInUse, &pProductInUse ) < 0 )
    {
        CsErrx( "BR_LLSC_7_4 : myki_br_GetCardProduct() failed" );
        return RULE_RESULT_ERROR;
    }

    if (pProductInUse->ControlBitmap & TAPP_TPRODUCT_CONTROL_PROVISIONAL_BITMAP )
    {
        if (myki_br_ldt_ProductUpdate_Invalidate(pData, pDirectoryInUse) < 0)
        {
            CsErrx("BR_LLSC_7_4 : myki_br_ldt_ProductUpdate_Invalidate() failed");
            return RULE_RESULT_ERROR;
        }
    }

    //
    //  5.  For each activated valid product on the card check to see if a
    //      single product covers the current trip zone range(5),(6)
    //          a.  If such a product does exist, set this product as the product
    //              in use(1) using a TAppUpdate/SetProductInUse transaction
    //          b.  Else set the product in use(1) to undefined using
    //              a TAppUpdate/SetProductInUse
    //
    //  Note: These operations are also done in BR_LLSC_7_4
    //        This code has been stolen from BR_LLSC_7_4
    //

    for ( dir = 1; dir < DIMOF( pMYKI_TAControl->Directory ); dir++ )
    {
        pDirectory = &pMYKI_TAControl->Directory[ dir ]; // Product slot 0-4 maps to dir entry 1-5

        if ( pDirectory->Status == TAPP_CONTROL_DIRECTORY_STATUS_ACTIVATED )
        {
            if ( MYKI_CS_TAProductGet( (U8_t)( dir - 1 ), &pProduct ) < 0  )
            {
                CsErrx( "BR_LLSC_7_4 : MYKI_CS_TAProductGet( %d ) failed", dir - 1 );
                return RULE_RESULT_ERROR;
            }

            if (    ( pProduct->ZoneLow <= pData->DynamicData.currentTripZoneLow )
                 && ( pProduct->ZoneHigh >= pData->DynamicData.currentTripZoneHigh ) )
            {
                // Break out of the for loop with pDirectory pointing to the valid product
                break;
            }
        }

        // If this is not our last dir entry, this will be set again at the top of the next iteration
        // If this is out last dir entry, we'll leave the loop with this set to NULL - used later
        pDirectory = NULL;
    }

    //  If at least one product exists that covers the current trip (pDirectory is not NULL), then set that product to be the product in use.
    //  If no products exist that cover the curren trip (pDirectory is NULL), then set the product in use to undefined.
    //  Note: Both these cases are handled within myki_br_ldt_AppUpdate_SetProductInUse() based on the value of pDirectory.

    if ( myki_br_ldt_AppUpdate_SetProductInUse( pData, pDirectory ) < 0 )
    {
        CsErrx( "BR_LLSC_7_4 : myki_br_ldt_AppUpdate_SetProductInUse() failed" );
        return RULE_RESULT_ERROR;
    }

    CsDbg( BRLL_RULE, "BR_LLSC_7_4 : Executed" );
    return RULE_RESULT_EXECUTED;
}

