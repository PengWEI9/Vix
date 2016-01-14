/**************************************************************************
*   ID :  BR_LLSC_6_10
*    Process Single Trip Products
*    1.  TAppControl.ProductInUse
*    2.  TAppTProduct.ControlBitmap
*    3.  TAppTProduct.ZoneLow
*    4.  TAppTProduct.ZoneHigh
*    5.  Dynamic.CurrentTripZoneLow
*    6.  Dynamic.CurrentTripZoneHigh
*    7.  Tariff.CitySaverZoneFlag
*    8.  Dynamic.CappingContribution
*    9.  Tariff.ApplicableFare
*    10. Dynamic.BorderProductsToExclude
*
*   Pre-Conditions
*    1.  The product in use field(1) is not 0 (a product is in a scanned-on state).
*    2.  The provisional bit of the product control bitmap(2) is set to 1.
*    3.  One or more single trip products exist on the card that meet
*        the following conditions:
*        a.  The product has not expired.
*        b.  The product zone(3) is within the low zone of the current trip(5)
*            and the high zone of the current trip(6); or
*        c.  the product zone(3) is marked as City Saver(7) and the product zone(3)
*            incremented by one is equal to the low zone of the current trip(5).
*        d.  No other product on the card covers the single trip product zone(3).
*
*   Description
*    1.  For each applicable single trip product perform a ProductSale/Upgrade transaction:
*        a.  Set the Product Id to n-Hour.
*        b.  If the product zone(3) of the single trip product is marked as a
*            City Saver(7) increment the product high zone(4) by 1.
*        c.  Lookup the applicable fare for the n-Hour product for the determined
*            zone range (using no specified route id) and increment the
*            purchase value by the difference between the n-Hour fare and
*            the single trip fare.
*        d.  Increment the capping contribution fare(8) by the difference
*            between the n-Hour fare and the single trip fare.
*    2.  For each upgraded product check to see if an adjacent border product
*        exists such that:
*            a.  The Border product zone low is equal to the upgraded
*                product zone high or
*            b.  The Border product zone low is equal to the upgraded
*                product zone high plus 1 and the border side bit is set to
*                inbound. Or
*            c.  The Border product zone low is equal to the upgraded
*                product zone low or
*            d.  The Border Product zone Low is equal to the upgraded
*                product zone low less 1 and the border side bit is
*                set to outbound
*            e.  This product has full coverage and needs accounted for:
*                i.   Reduce the applicable fare(9) by the product
*                     value of the border product.
*                ii.  Reduce the capping contribution fare(8) by the
*                     product value of the border product.
*                iii. Perform a product/invalidate to remove
*                     the border product
*
*   Post-Conditions
*    1.  The single trip products have been processed.
*
*   Devices
*    Fare payment devices
*
*    Business context
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

RuleResult_e BR_LLSC_6_10( MYKI_BR_ContextData_t *pData )
{
    MYKI_TAControl_t            *pMYKI_TAControl;
    MYKI_Directory_t            *pDirectoryInUse = NULL;
    MYKI_TAProduct_t            *pProductInUse   = NULL;
    ProductIterator             iSingle;
    int                         rv;

    CsDbg( BRLL_RULE, "BR_LLSC_6_10 : Start (Process Single Trip Products)" );

    if ( ! pData )
    {
        CsErrx( "BR_LLSC_6_10 : Invalid argument(s)" );
        return RULE_RESULT_ERROR;
    }

    if ( MYKI_CS_TAControlGet( &pMYKI_TAControl ) < 0 )
    {
        CsErrx( "BR_LLSC_6_10 : MYKI_CS_TAControlGet() failed" );
        return RULE_RESULT_ERROR;
    }
    
    //----------------------------------------------------------------------------
    //  Pre-Conditions
    //   1.  The product in use field(1) is not 0 (a product is in a scanned-on state).

    if (pMYKI_TAControl->ProductInUse == 0)
    {
        CsDbg( BRLL_RULE, "BR_LLSC_6_10 : Bypass - No products in use" );
        pData->ReturnedData.bypassCode = 1;
        return RULE_RESULT_BYPASSED;
    }

    //
    //   2.  The provisional bit of the product control bitmap(2) is set to 1.
    //
    //  Get product and directory structure in use
    if ( myki_br_GetCardProduct( pMYKI_TAControl->ProductInUse, &pDirectoryInUse, &pProductInUse ) < 0 )
    {
        CsErrx( "BR_LLSC_6_10 : myki_br_GetCardProduct() failed" );
        return RULE_RESULT_ERROR;
    }
    
    if ( ( pProductInUse->ControlBitmap & TAPP_TPRODUCT_CONTROL_PROVISIONAL_BITMAP ) == 0 )
    {
        CsDbg( BRLL_RULE, "BR_LLSC_6_10 : Bypass - ProductInUse is not a provisional product");
        pData->ReturnedData.bypassCode = 2;
        return RULE_RESULT_BYPASSED;
    }

    //
    //   3.  One or more single trip products exist on the card that meet
    //       the following conditions:
    //       a.  The product has not expired.
    //       b.  The product zone(3) is within the low zone of the current trip(5)
    //           and the high zone of the current trip(6); or
    //       c.  the product zone(3) is marked as City Saver(7) and the product zone(3)
    //           incremented by one is equal to the low zone of the current trip(5).
    //       d.  No other product on the card covers the single trip product zone(3).
    //

    //============================================================================
    //  Implementation Note
    //  The system does not have single trip products
    //  If we find one then treat it as an error
    //
    myki_br_InitProductIterator(pMYKI_TAControl, &iSingle, PRODUCT_TYPE_SINGLE);
    rv = myki_br_ProductIterate(&iSingle);

    if ( rv < 0 )
    {
        CsErrx("BR_LLSC_6_10 : myki_br_ProductIterate() failed");
        return RULE_RESULT_ERROR;
    }
    else if ( rv > 0 )
    {
        CsErrx("BR_LLSC_6_10 : Single trip product found - not supported");
        pData->ReturnedData.bypassCode = 3;
        return RULE_RESULT_ERROR;
    }

    CsDbg( BRLL_RULE, "BR_LLSC_6_10 : Bypass - No Matching Product");
    pData->ReturnedData.bypassCode = 4;
    return RULE_RESULT_BYPASSED;
}

