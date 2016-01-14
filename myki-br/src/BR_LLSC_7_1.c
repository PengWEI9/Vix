/**************************************************************************
*   ID :  BR_LLSC_7_1
*
*    Forced  Scan Off Penalty
*
*    1.  TAppControl.ProductInUse
*    2.  TAppControl.ProvisionalFare
*    3.  TAppControl.PassengerCode
*    4.  TAppControl.Directory.ProductId
*    5.  TAppTProduct.ControlBitmap
*    6.  TAppTProduct.ProductPurchaseValue
*    7.  Dynamic.ProvisionalZoneLow
*    8.  Dynamic.ProvisionalZoneHigh
*    9.  Dynamic.CurrentTripZoneLow
*    10.  Dynamic.CurrentTripZoneHigh
*    11.  Tariff.BaseFare
*    12.  Tariff.ApplicableFare
*    13.  Tariff.ProvisionalCappingOption
*    14.  Dynamic.IsForcedScanOff
*    15.  Dynamic.ForcedScanOffDate
*    16.  TAppTProduct.StartDateTime
*    17.  Dynamic.CurrentZone
*    18.  Dynamic.CurrentDateTime
*    19.  Static.ServiceProviderID
*    20.  Dynamic.EntryPointID
*    21.  Dynamic.LineID
*    22.  Dynamic.StopID
*
*    Pre-Conditions
*    1.  The product in use field(1) is not 0 (a product is in a scanned-on state).
*    2.  The Provisional Capping Option is set to 'Option 3'
*    3.  The provisional bit on the product Control bit map(2) of the product in use is true(1)
*    4.  The product purchase value on this product is not $0.00 (ie not zero value reserved provisional
*        amount)
*
*    Description
*    1.  Set IsForcedScanOff(14) = True - ie this is a force scan off sequence
*    2.  Set ForcedScanOffDate(15) to the valid from date of the provisional product(16).
*    3.  Set the current trip low zone(9)  to the provisional low zone(7) .
*    4.  Set the current trip high zone(10)  to the provisional high zone(8)
*    5.  Perform a ProductSale/Upgrade transaction for the product in use(1):
*           i.  Set the product ID(4) to n-Hour.
*           ii.  Set purchase value of the product to product value of the provisional fare.
*           iii.  Set the provisional bit of the product control bitmap(5) to 0.
*           iv.  Set the expiry time of the product to the current date time
*    6.  Increment the applicable fare field by the product value amount in the provisional product.
*
*    Post-Conditions
*    1.  The provisional fare has been stored for pending deduction.
*    2.  The provisional product has been upgraded to an n-Hour product.
*
*    Devices
*    Fare payment devices
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

RuleResult_e BR_LLSC_7_1( MYKI_BR_ContextData_t *pData )
{
    MYKI_TAControl_t    *pMYKI_TAControl = NULL;
    MYKI_Directory_t    *pDirectoryInUse = NULL;
    MYKI_TAProduct_t    *pProductInUse   = NULL;
    U32_t               purchaseValue    = 0;

    CsDbg( BRLL_RULE, "BR_LLSC_7_1 : Start (Forced  Scan Off Penalty)" );

    if ( ! pData )
    {
        CsErrx( "BR_LLSC_7_1 : Invalid argument(s)" );
        return RULE_RESULT_ERROR;
    }

    if ( MYKI_CS_TAControlGet( &pMYKI_TAControl ) < 0 )
    {
        CsErrx( "BR_LLSC_7_1 : MYKI_CS_TAControlGet() failed" );
        return RULE_RESULT_ERROR;
    }

    ////////////////////////
    // Preconditions
    ////////////////////////

    // 1.  The product in use field(1) is not 0 (a product is in a scanned-on state).
    if ( pMYKI_TAControl->ProductInUse == 0 )
    {
        CsDbg( BRLL_RULE, "BR_LLSC_7_1 : BYPASSED : ProductInUse is zero");
        return RULE_RESULT_BYPASSED;
    }

    /* Get product and directory structure in use */
    if ( myki_br_GetCardProduct( pMYKI_TAControl->ProductInUse, &pDirectoryInUse, &pProductInUse ) < 0 )
    {
        CsErrx( "BR_LLSC_7_1 : myki_br_GetCardProduct() failed" );
        return RULE_RESULT_ERROR;
    }

    // 2.  The Provisional Capping Option is set to 'Option 3'
    if ( pData->Tariff.provisionalCappingOption   != PROVISIONAL_CAPPING_OPTION_3 )
    {
        CsDbg( BRLL_RULE, "BR_LLSC_7_1 : BYPASSED : provisionalCappingOption is not 3");
        return RULE_RESULT_BYPASSED;
    }

    //  3.  The provisional bit on the product Control bit map(2) of the product in use is true(1)

    if ( ( pProductInUse->ControlBitmap & TAPP_TPRODUCT_CONTROL_PROVISIONAL_BITMAP ) == 0 )
    {
        CsDbg( BRLL_RULE, "BR_LLSC_7_1 : BYPASSED : ProductInUse is not a provisional product");
        return RULE_RESULT_BYPASSED;
    }

    // 4.  The product purchase value on this product is not $0.00 (ie not zero value reserved provisional amount)

    if ( pProductInUse->PurchaseValue == 0 )
    {
        CsDbg( BRLL_RULE, "BR_LLSC_7_1 : BYPASSED : ProductInUse provisional product fare is zero");
        return RULE_RESULT_BYPASSED;
    }


    ////////////////////////
    // Execution
    ////////////////////////

    //  1.  Set IsForcedScanOff(14) = True - ie this is a force scan off sequence
    //  2.  Set ForcedScanOffDate(15) to the valid from date of the provisional product(16).

    pData->DynamicData.isForcedScanOff = TRUE;
    pData->DynamicData.forcedScanOffDateTime = pProductInUse->StartDateTime;

    //  3.  Set the current trip low zone(9)  to the provisional low zone(7) .
    //  4.  Set the current trip high zone(10)  to the provisional high zone(8)

    pData->DynamicData.currentTripZoneLow = pData->DynamicData.provisionalZoneLow;
    pData->DynamicData.currentTripZoneHigh = pData->DynamicData.provisionalZoneHigh;

    //   5.  Perform a ProductSale/Upgrade transaction for the product in use(1):
    //          i.   Set the product ID(4) to n-Hour.
    pDirectoryInUse->ProductId = myki_br_cd_GetProductId(PRODUCT_TYPE_NHOUR);

    // ii. Set purchase value of the product to product value of the provisional fare.
    //
    //
    // Which one is the provisional fare? pProductInUse->PurchaseValue or pMYKI_TAControl->ProvisionalFare
    // Anyway, they normally should have the same value
    purchaseValue = pMYKI_TAControl->ProvisionalFare;

    // iii. Set the provisional bit of the product control bitmap(5) to 0.

    pProductInUse->ControlBitmap &= ~TAPP_TPRODUCT_CONTROL_PROVISIONAL_BITMAP;

    //  iv.  Set the expiry time of the product to the current date time
    pProductInUse->EndDateTime = pData->DynamicData.currentDateTime;

    // ProductSale/Upgrade
    if ( myki_br_ldt_ProductSale_Upgrade_SameProduct( pData,
                                            pDirectoryInUse,
                                            pData->DynamicData.currentTripZoneLow,
                                            pData->DynamicData.currentTripZoneHigh,
                                            purchaseValue,
                                            pProductInUse->EndDateTime ) < 0 )
    {
        CsErrx( "BR_LLSC_7_1 : myki_br_ldt_ProductSale_Upgrade_SameProduct() failed" );
        return RULE_RESULT_ERROR;
    }

    // 6.  Increment the applicable fare field by the product value amount in the provisional product.

    CsDbg( BRLL_FIELD, "BR_LLSC_7_1 : Increasing applicable fare by %d from %d to %d", purchaseValue, pData->ReturnedData.applicableFare, pData->ReturnedData.applicableFare + purchaseValue );
    pData->ReturnedData.applicableFare += purchaseValue;

    CsDbg( BRLL_RULE, "BR_LLSC_7_1 : Executed" );
    return RULE_RESULT_EXECUTED;
}


