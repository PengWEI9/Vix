/**************************************************************************
*   ID :  BR_LLSC_6_2
*    Scan-Off e-Pass
*    1.  TAppControl.ProductInUse
*    2.  TAppControl.Directory.ProductId
*    3.  TAppTProduct.ZoneLow
*    4.  TAppTProduct.ZoneHigh
*    5.  Dynamic.CurrentZone
*
*    Pre-Conditions
*    1.  The product in use field(1)  is not 0 (a product is in a scanned-on state).
*    2.  The product ID(2)  of the scanned-on product is e-Pass.
*    3.  The current zone(5)  is within the product low zone(3)  and product high zone(4)  of the e-Pass product.
* 
*    Description
*    1.  Perform a ProductUsage/ScanOff transaction for the e-Pass product.
* 
*    Post-Conditions
*    1.  The scanned-on e-Pass product has been scanned-off.
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

RuleResult_e BR_LLSC_6_2( MYKI_BR_ContextData_t *pData )
{
    MYKI_TAControl_t    *pMYKI_TAControl;
    MYKI_Directory_t    *pDirectory = NULL;
    MYKI_TAProduct_t    *pProduct = NULL;
    ProductType_e       productType;

    CsDbg( BRLL_RULE, "BR_LLSC_6_2 : Start (Scan-Off e-Pass)" );

    if ( ! pData )
    {
        CsErrx( "BR_LLSC_6_2 : Invalid argument(s)" );
        return RULE_RESULT_ERROR;
    }

    if ( MYKI_CS_TAControlGet( &pMYKI_TAControl ) < 0 )
    {
        CsErrx( "BR_LLSC_6_2 : MYKI_CS_TAControlGet() failed" );
        return RULE_RESULT_ERROR;
    }

    if ( pMYKI_TAControl->ProductInUse == 0 )
    {
        CsDbg( BRLL_RULE, "BR_LLSC_6_2 : Bypass - ProductInUse is zero");
        return RULE_RESULT_BYPASSED;
    }

    if ( myki_br_GetCardProduct( pMYKI_TAControl->ProductInUse, &pDirectory, &pProduct ) < 0 )
    {
        CsErrx( "BR_LLSC_6_2 : Failed to get product %d details", pMYKI_TAControl->ProductInUse );
        return RULE_RESULT_ERROR;
    }

    if ( ( productType = myki_br_cd_GetProductType( pDirectory->ProductId ) ) == PRODUCT_TYPE_UNKNOWN )
    {
        CsErrx( "BR_LLSC_6_2 : Product ID %d is unknown" );
        return RULE_RESULT_ERROR;
    }

    if ( productType != PRODUCT_TYPE_EPASS )
    {
        CsDbg( BRLL_RULE, "BR_LLSC_6_2 : Bypass - Directory %d is not e-Pass product", pMYKI_TAControl->ProductInUse );
        return RULE_RESULT_BYPASSED;
    }

    if ( pData->DynamicData.currentZone > pProduct->ZoneHigh || pData->DynamicData.currentZone < pProduct->ZoneLow )
    {
        CsDbg( BRLL_RULE, "BR_LLSC_6_2 : Bypass - current zone is out of product zone");
        return RULE_RESULT_BYPASSED;
    }

    if ( myki_br_ldt_ProductUsage_ScanOff( pData, pDirectory ) < 0 )
    {
        CsErrx("BR_LLSC_6_2 : myki_br_ldt_ProductUsage_ScanOff() failed");
        return RULE_RESULT_ERROR;
    }

    CsDbg( BRLL_RULE, "BR_LLSC_6_2 : Executed" );
    return RULE_RESULT_EXECUTED;
}


