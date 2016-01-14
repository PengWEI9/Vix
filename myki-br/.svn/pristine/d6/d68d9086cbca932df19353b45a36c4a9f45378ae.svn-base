/**************************************************************************
*   ID :  BR_LLSC_6_3
*    Scan-Off e-Pass with Restricted Out-of-Zone
*    1.  TAppControl.ProductInUse
*    2.  TAppControl.Directory.ProductId
*    3.  TAppTProduct.ZoneLow
*    4.  TAppTProduct.ZoneHigh
*    5.  Dynamic.CurrentZone
*    6.  Tariff.AllowEPassOutOfZone
*
*    Pre-Conditions
*    1.  The product in use field(1) is not 0 (a product is in a scanned-on state).
*    2.  The product ID(2) of the scanned-on product is e-Pass.
*    3.  The flag is set to disallow out-of-zone travel with an e-Pass product(6).
*    4.  The current zone(5) is not within the product low zone(3) and product high zone(4) of the e-Pass product.
*
*    Description
*    1.  Reject the smartcard
*
*    Post-Conditions
*    1.  The smartcard is rejected.
*
*    Devices
*    Fare payment devices
*
*    Business Context
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

RuleResult_e BR_LLSC_6_3( MYKI_BR_ContextData_t *pData )
{
    MYKI_TAControl_t *pMYKI_TAControl;
    MYKI_TAProduct_t *pMYKI_TAProduct;

    CsDbg( BRLL_RULE, "BR_LLSC_6_3 : Start (Scan-Off e-Pass with Restricted Out-of-Zone)" );

    if ( ! pData )
    {
        CsErrx( "BR_LLSC_6_3 : Invalid argument(s)" );
        return RULE_RESULT_ERROR;
    }

    //----------------------------------------------------------------------------
    //  Pre-Conditions
    //  1.  The product in use field(1) is not 0 (a product is in a scanned-on state).
    //
    if ( MYKI_CS_TAControlGet( &pMYKI_TAControl ) )
    {
        CsErrx( "BR_LLSC_4_18 : MYKI_CS_TAControlGet() failed" );
        return RULE_RESULT_ERROR;
    }

    if (pMYKI_TAControl->ProductInUse == 0)
    {
        CsDbg( BRLL_RULE, "BR_LLSC_6_3 : Bypass - No products in use" );
        pData->ReturnedData.bypassCode = 1;
        return RULE_RESULT_BYPASSED;
    }

    //
    //  2.  The product ID(2) of the scanned-on product is e-Pass.
    //
    if (myki_br_cd_GetProductType( pMYKI_TAControl->Directory[ pMYKI_TAControl->ProductInUse ].ProductId ) != PRODUCT_TYPE_EPASS )
    {
        CsDbg( BRLL_RULE, "BR_LLSC_6_3 : Bypass - Product is not an ePass" );
        pData->ReturnedData.bypassCode = 2;
        return RULE_RESULT_BYPASSED;
    }

    //
    //  3.  The flag is set to disallow out-of-zone travel with an e-Pass
    //      product(6).
    //
    if ( pData->Tariff.allowEPassOutOfZone )
    {
        CsDbg( BRLL_RULE, "BR_LLSC_6_3 : Bypass - pTarrif->allowEPassOutOfZone is TRUE");
        pData->ReturnedData.bypassCode = 3;
        return RULE_RESULT_BYPASSED;
    }

    //
    //  4.  The current zone(5) is not within the product low zone(3) and
    //      product high zone(4) of the e-Pass product.
    //
    if (MYKI_CS_TAProductGet( (U8_t)( pMYKI_TAControl->ProductInUse - 1 ), &pMYKI_TAProduct ) < 0)
    {
        CsErrx( "BR_LLSC_6_3 : myki_br_GetCardProduct() MYKI_CS_TAProductGet( %d ) failed", pMYKI_TAControl->ProductInUse - 1 );
        return RULE_RESULT_ERROR;
    }

    if (pData->DynamicData.currentZone <= pMYKI_TAProduct->ZoneHigh &&
        pData->DynamicData.currentZone >= pMYKI_TAProduct->ZoneLow )
    {
        CsDbg(BRLL_RULE, "BR_LLSC_6_3 : Current is within product zone");
        pData->ReturnedData.bypassCode = 4;
        return RULE_RESULT_BYPASSED;
    }

    //
    //  Indicate, to the unit test, that all prerequisites have been met
    //
    pData->ReturnedData.bypassCode = 5;

    //----------------------------------------------------------------------------
    //  Description
    //  1.  Reject the smartcard
    //
    pData->ReturnedData.rejectReason = MYKI_BR_REJECT_REASON_INVALID_ZONE;

    CsDbg( BRLL_RULE, "BR_LLSC_6_3 : Executed" );
    return RULE_RESULT_EXECUTED;
}

