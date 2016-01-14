/**************************************************************************
*   ID :  BR_LLSC_7_5
*
*   Expired Tariff Partial Product Coverage
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
*       one or more zones does not have a  product coverage as follows:
*       a.  An activated product (that is not the provisional product) covers each zone
*       b.  There is currently no product of type epass active; and a product of type epass that is inactive epass exists that:
*           i.  covers one or more of the zones of the provisional product.
*           ii. where start date time(14) is before than the scan on date(14) of the product in use(1)
*
*   Description
*   1.  Set IsForcedScanOff(15) = True - ie this is a force scan off sequence
*   2.  Set ForcedScanOffDate(16) to the scan on date(17) of the provisional product.
*   3.  Set the current trip low zone(9) to the provisional low zone(3).
*   4.  Set the current trip high zone(10) to the provisional high zone(4).
*   5.  If the product control bit map(2) on the provisional product indicates that this is a border product then perform ProductUpdate/None to clear the board status bit
*
*   Post-Conditions
*   1. We are ready to start scaning off the product.
*
*   Devices Fare payment devices
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

RuleResult_e BR_LLSC_7_5( MYKI_BR_ContextData_t *pData )
{
    MYKI_TAControl_t                            *pMYKI_TAControl;
    MYKI_Directory_t                            *pDirectoryInUse = NULL;
    MYKI_TAProduct_t                            *pProductInUse   = NULL;
    myki_br_CheckProvisionProductZoneCoverage_t  zoneCoverage;

    CsDbg( BRLL_RULE, "BR_LLSC_7_5 : Start (Expired Tariff Partial Product Coverage)" );

    if ( ! pData )
    {
        CsErrx( "BR_LLSC_7_5 : Invalid argument(s)" );
        return RULE_RESULT_ERROR;
    }

    if ( MYKI_CS_TAControlGet( &pMYKI_TAControl ) < 0 )
    {
        CsErrx( "BR_LLSC_7_5 : MYKI_CS_TAControlGet() failed" );
        return RULE_RESULT_ERROR;
    }

    //   Pre-Conditions
    //   1. The product in use field(1) is not 0 (a product is in a scanned-on state).
    //
    if (pMYKI_TAControl->ProductInUse == 0)
    {
        CsDbg( BRLL_RULE, "BR_LLSC_7_5 : Bypass - No products in use" );
        pData->ReturnedData.bypassCode = 1;
        return RULE_RESULT_BYPASSED;
    }

    //  Get product and directory structure in use
    if ( myki_br_GetCardProduct( pMYKI_TAControl->ProductInUse, &pDirectoryInUse, &pProductInUse ) < 0 )
    {
        CsErrx( "BR_LLSC_7_5 : myki_br_GetCardProduct() failed" );
        return RULE_RESULT_ERROR;
    }

    //
    //   2. The Provisional Capping Option is not set to ‘Option 3’
    //
    if ( pData->Tariff.provisionalCappingOption == PROVISIONAL_CAPPING_OPTION_3 )
    {
        CsDbg( BRLL_RULE, "BR_LLSC_7_5 : Bypass - Provisional Capping Option = 3" );
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
    //  KWS : Commented out this precondition to see what impact it has while Keane discuss the issue.

    if ( MYKI_CD_getStartDateTime() <= pProductInUse->LastUsage.DateTime )
    {
        CsDbg( BRLL_RULE, "BR_LLSC_7_5 : Bypass - Valid Tariff - CD Start Date/Time (%d) <= Product Last Usage Date/Time (%d)", MYKI_CD_getStartDateTime(), pProductInUse->LastUsage.DateTime );
        pData->ReturnedData.bypassCode = 3;
        return RULE_RESULT_BYPASSED;
    }

    //
    //  4.  The provisional bit on the product Control bit map(2) of the product in use is true(1)
    //
    if ( ( pProductInUse->ControlBitmap & TAPP_TPRODUCT_CONTROL_PROVISIONAL_BITMAP ) == 0 )
    {
        CsDbg( BRLL_RULE, "BR_LLSC_7_5 : Bypass - ProductInUse is not a provisional product");
        pData->ReturnedData.bypassCode = 4;
        return RULE_RESULT_BYPASSED;
    }

    //
    //   5. The product purchase value on this product is not $0.00 (ie not zero value reserved provisional amount)
    //
    if ( pProductInUse->PurchaseValue == 0 )
    {
        CsDbg( BRLL_RULE, "BR_LLSC_7_5 : Bypass - ProductInUse provisional product fare is zero");
        pData->ReturnedData.bypassCode = 5;
        return RULE_RESULT_BYPASSED;
    }

    //
    //  6.  For the provisional product zone range (zone low to zone high) verify that
    //      one or more zones does not have a product coverage as follows:
    //      a.  An activated product (that is not the provisional product) covers each zone
    //      b.  There is currently no product of type epass active; and a product of type epass that is inactive epass exists that:
    //          i.  covers one or more of the zones of the provisional product.
    //          ii. where start date time(14) is before than the scan on date(14) of the product in use(1)

    zoneCoverage = myki_br_CheckProvisionProductZoneCoverage(pData, pMYKI_TAControl, pProductInUse, NULL);
    if ( zoneCoverage == CHECK_PROVISION_PRODUCTZONE_COVERAGE_ERROR )
    {
        CsErrx( "BR_LLSC_7_5 : myki_br_CheckProvisionProductZoneCoverage() failed" );
        return RULE_RESULT_ERROR;
    }
    else if ( zoneCoverage != CHECK_PROVISION_PRODUCTZONE_COVERAGE_PARTIAL )
    {
        CsDbg( BRLL_RULE, "BR_LLSC_7_5 : Bypass - Full coverage detected");
        pData->ReturnedData.bypassCode = 8;
        return RULE_RESULT_BYPASSED;
    }

    //
    //  Indicate (to Test Harness) that all preconditions have been met
    //
    pData->ReturnedData.bypassCode = 6;

    //   Description
    //   1. Set IsForcedScanOff(15) = True - ie this is a force scan off sequence
    //
    pData->DynamicData.isForcedScanOff = TRUE;

    //
    //   2. Set ForcedScanOffDate(16) to the scan on date(17) of the provisional product.
    //
    pData->DynamicData.forcedScanOffDateTime = pProductInUse->StartDateTime;

    //
    //   3. Set the current trip low zone(9) to the provisional low zone(3).
    //
//    pData->DynamicData.currentTripZoneLow = pData->DynamicData.provisionalZoneLow;
    pData->DynamicData.currentTripZoneLow = pProductInUse->ZoneLow;

    //
    //   4. Set the current trip high zone(10) to the provisional high zone(4).
    //
//    pData->DynamicData.currentTripZoneHigh = pData->DynamicData.provisionalZoneHigh;
    pData->DynamicData.currentTripZoneHigh = pProductInUse->ZoneHigh;

    //
    //   5. If the product control bit map(2) on the provisional product indicates that this is a border
    //      product then perform ProductUpdate/None to clear the board status bit
    if (pProductInUse->ControlBitmap & TAPP_TPRODUCT_CONTROL_BORDER_STATUS_BITMAP )
    {
        if ( myki_br_ldt_ProductUpdate_ClearBorderStatus( pData, pDirectoryInUse ) < 0 )
        {
            CsErrx( "BR_LLSC_7_5 : myki_br_ldt_ProductUpdate_ClearBorderStatus() failed" );
            return RULE_RESULT_ERROR;
        }
    }

    CsDbg( BRLL_RULE, "BR_LLSC_7_5 : Executed" );
    return RULE_RESULT_EXECUTED;
}

