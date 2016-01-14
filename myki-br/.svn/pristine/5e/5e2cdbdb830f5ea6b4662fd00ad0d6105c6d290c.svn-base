/**************************************************************************
*   ID :  BR_LLSC_3_2
*    Autoload Product
*    1.  TAppControl.Directory.Status
*    2.  TAppTProduct.ControlBitmap
*    3.  TAppTProduct.StartDateTime
*    4.  TAppTProduct.EndDateTime
*    5.  TAppTProduct.SerialNo
*    6.  Dynamic.CurrentDateTime
*    7.  Tariff.InactiveEPassValidity
*    8.  Static.ProductAutoEnabled
*    9.  Dynamic.CurrentDateTime
*    10.  Static.ServiceProviderID
*    11.  Dynamic.EntryPointID
*    12.  Dynamic.LineID
*    13.  Dynamic.StopID
*    14.  TAppTProduct.ServiceProvder
*    15.  TAppTProduct.PurchaseValue
*    16.  TAppTProduct.NextTxSeqNo
*    17.  TAppTPurse.TPurseBalance
*
*    1.  The autoload bit in the product control bitmap(2) is set to 1 and the device product autoload flag(8) is set to 1.
*    2.  The relevant product status1 is set to Active.
*    3.  The product expiry(4) is less than the current date/time(6) (the product has expired).
* 
*    Description
*    1.  For each product:
*           a.  Perform a ProductSale/Autoload transaction using the same product serial number(5):
*                i.  Set the product start date3 to the current date/time(6).
*                ii.  Set the inactive product expiry date4 to the inactive e-Pass expiry date(7).
*      Post-Conditions
*    1.  An e-Pass product is reloaded for the number of days originally purchased.
* 
*    Devices
*    All devices
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

RuleResult_e BR_LLSC_3_2( MYKI_BR_ContextData_t *pData )
{
    MYKI_TAControl_t            *pMYKI_TAControl;
    MYKI_Directory_t            *pDirectory;
    MYKI_TAProduct_t            *pProduct;

    CsDbg( BRLL_RULE, "BR_LLSC_3_2 : Start (Autoload Product)" );

    if ( ! pData )
    {
        CsErrx( "BR_LLSC_3_2 : Invalid argument(s)" );
        return RULE_RESULT_ERROR;
    }

    if ( MYKI_CS_TAControlGet( &pMYKI_TAControl ) < 0 )
    {
        CsErrx( "BR_LLSC_3_2 : MYKI_CS_TAControlGet() failed" );
        return RULE_RESULT_ERROR;
    }

    if ( pMYKI_TAControl->ProductInUse == 0 )
    {
        CsDbg( BRLL_RULE, "BR_LLSC_3_2 : Bypass : ProductInUse is zero");
        return RULE_RESULT_BYPASSED;
    }

    /* Get product and directory structure in use */
    if ( myki_br_GetCardProduct( pMYKI_TAControl->ProductInUse, &pDirectory, &pProduct ) < 0 )
    {
        CsErrx( "BR_LLSC_3_2 : myki_br_GetCardProduct( %d ) failed", pMYKI_TAControl->ProductInUse );
        return RULE_RESULT_ERROR;
    }

    if ( ! ( pProduct->ControlBitmap & TAPP_TPRODUCT_CONTROL_AUTOLOAD_BITMAP &&
        pData->StaticData.productAutoEnabled == 1 ) )
    {
        CsDbg( BRLL_RULE,"BR_LLSC_3_2 : Bypassed - productAutoEnabled or TAPP_TPRODUCT_CONTROL_AUTOLOAD_BITMAP not set." );
        return RULE_RESULT_BYPASSED;
    }

    if ( pDirectory->Status != TAPP_CONTROL_DIRECTORY_STATUS_ACTIVATED )
    {
        CsDbg( BRLL_RULE, "BR_LLSC_3_2 : Bypassed - Product is not active.");
        return RULE_RESULT_BYPASSED;
    }

    if ( pProduct->EndDateTime > pData->DynamicData.currentDateTime )
    {
        CsDbg( BRLL_RULE, "BR_LLSC_3_2 : Bypassed - Product has expired.");
        return RULE_RESULT_BYPASSED;
    }

    //  a.  Perform a ProductSale/Autoload transaction using the same product serial number(5):
    //       i.  Set the product start date3 to the current date/time(6).
    //       ii.  Set the inactive product expiry date4 to the inactive e-Pass expiry date(7).
    //  

    //-----------------------------------------------------------------------------
    //  Implementation note.
    //  ProductSale/Autoload not currently supported by LDT
    //  ProductSale/Autoload functionality not currently (Dec-2013) required
    //
    //  No need to implement this at the moment
    //

    CsErrx( "BR_LLSC_3_2 : Not yet required/implemented - returning Bypassed" );
    return RULE_RESULT_BYPASSED;
}

