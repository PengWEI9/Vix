/**************************************************************************
*   ID :  BR_LLSC_6_9
*    Assert Validation Rules
*    1.  TAppControl.ProductInUse
*    2.  TAppControl.Directory.Status
*    3.  TAppControl.Directory.ProductId
*    4.  TAppTProduct.ControlBitmap
*    5.  TAppTProduct.EndDateTime
*    6.  TAppTProduct.LastUsageDateTime
*    7.  Dynamic.LineId
* 
*   Pre-Conditions
*    1.  The mode of transport by the current device is not rail
*    2.  The product in use field(1)  is not 0 (a product is in a scanned-on state).
*    3.  The provisional bit of the product control bitmap(4)  is set to 1.
*    4.  One or more stored value products exist on the smartcard with a product status(2)  of Active.
*    5.  The product expiry(5)  of one or more of the stored value products is greater than or equal to the scan-on date/time(6)  of the product in use(1) .
*    6.  A validation rule exists based on the product ID(3)  of one or more stored value products and the route ID/line ID(7)  of the current trip.
* 
*    Description
*    1.  Perform a ProductUsage/ScanOff transaction for the applicable product Perform a ProductUpdate/Invalidate transaction for the provisional product
* 
*    Post-Conditions
*    1.  A stored value product for which a validation rule applies has been scanned-off.
*    2.  The provisional product has been invalidated.
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

#define RULE_NAME "BR_LLSC_6_9"
#define RULE_PREFIX RULE_NAME " : "

//=============================================================================
//
//
//
//=============================================================================

RuleResult_e BR_LLSC_6_9( MYKI_BR_ContextData_t *pData )
{
    MYKI_TAControl_t        *pMYKI_TAControl       = NULL;
    MYKI_Directory_t        *pDirectoryInUse       = NULL;
    MYKI_TAProduct_t        *pProductInUse         = NULL;
    MYKI_Directory_t        *pMYKI_Directory       = NULL;
    MYKI_TAProduct_t        *pMYKI_TAProduct       = NULL;
    MYKI_Directory_t        *pStoredValueDirectory = NULL;
    int                      i                     = 0;
    int                      storedValueProduct    = 0;

    CsDbg( BRLL_RULE, RULE_PREFIX "Start (Assert Validation Rules)" );

    if ( ! pData )
    {
        CsErrx( RULE_PREFIX "Invalid argument(s)" );
        return RULE_RESULT_ERROR;
    }

    if ( MYKI_CS_TAControlGet( &pMYKI_TAControl ) < 0 )
    {
        CsErrx( RULE_PREFIX "MYKI_CS_TAControlGet() failed" );
        return RULE_RESULT_ERROR;
    }

    // Pre-Conditions

    //
    //  1.  The mode of transport by the current device is not rail
    //
    if ( pData->InternalData.TransportMode == TRANSPORT_MODE_RAIL )
    {
        CsDbg( BRLL_RULE, RULE_PREFIX "Bypass - Device Transport Mode is RAIL" );
        pData->ReturnedData.bypassCode = BYPASS_CODE(6, 9, 1, 0);
        return RULE_RESULT_BYPASSED;
    }

    //
    //  2.  The product in use field(1)  is not 0 (a product is in a scanned-on state).
    //
    if ( pMYKI_TAControl->ProductInUse == 0 )
    {
        CsDbg( BRLL_RULE, RULE_PREFIX "Bypass - No products in use" );
        pData->ReturnedData.bypassCode = BYPASS_CODE(6, 9, 2, 0);
        return RULE_RESULT_BYPASSED;
    }

    //  Get product and directory structure in use
    if ( myki_br_GetCardProduct( pMYKI_TAControl->ProductInUse, &pDirectoryInUse, &pProductInUse ) < 0 )
    {
        CsErrx( RULE_PREFIX "myki_br_GetCardProduct(%d) failed", pMYKI_TAControl->ProductInUse );
        return RULE_RESULT_ERROR;
    }

    //
    //  3.  The provisional bit of the product control bitmap(4)  is set to 1.
    //
    if ( ( pProductInUse->ControlBitmap & TAPP_TPRODUCT_CONTROL_PROVISIONAL_BITMAP ) == 0 )
    {
        CsDbg( BRLL_RULE, RULE_PREFIX "Bypass - ProductInUse is not a provisional product");
        pData->ReturnedData.bypassCode = BYPASS_CODE(6, 9, 3, 0);
        return RULE_RESULT_BYPASSED;
    }

    //
    //  4.  One or more stored value products exist on the smartcard with a product status(2)  of Active.
    //  5.  The product expiry(5)  of one or more of the stored value products is greater than or equal to the scan-on date/time(6)  of the product in use(1) .
    //  6.  A validation rule exists based on the product ID(3)  of one or more stored value products and the route ID/line ID(7)  of the current trip.
    //
    for ( i = 1; (storedValueProduct == 0) && (i < DIMOF( pMYKI_TAControl->Directory )); ++i )
    {
        if ( i == pMYKI_TAControl->ProductInUse )
        {
            CsDbg( BRLL_RULE, RULE_PREFIX "Product at directory index %d is the product in use, ignoring", i );
            continue;
        }
  
        //  4.  One or more stored value products exist on the smartcard with a product status(2)  of Active.
        if ( pMYKI_TAControl->Directory[ i ].Status != TAPP_CONTROL_DIRECTORY_STATUS_ACTIVATED )
        {
            CsDbg( BRLL_RULE, RULE_PREFIX "Product at directory index %d is not active, ignoring", i );
            continue;
        }
        CsDbg( BRLL_RULE, RULE_PREFIX "Product at directory index %d is active", i );

        // 5.  The product expiry(5)  of one or more of the stored value products is greater than or equal to the scan-on date/time(6)  of the product in use(1) .
        if ( myki_br_GetCardProduct( i, &pMYKI_Directory, &pMYKI_TAProduct ) < 0 )
        {
            CsErrx( RULE_PREFIX "myki_br_GetCardProduct( %d ) failed", i );
            return RULE_RESULT_ERROR;
        }

        if ( pMYKI_TAProduct->EndDateTime < pProductInUse->LastUsage.DateTime )
        {
            CsDbg( BRLL_RULE, RULE_PREFIX "Product at directory index %d is expired, ignoring (%d < %d)", i, (int)pMYKI_TAProduct->EndDateTime, (int)pProductInUse->LastUsage.DateTime );
            continue;
        }
        CsDbg( BRLL_RULE, RULE_PREFIX "Product at directory index %d is not expired (%d >= %d)", i, (int)pMYKI_TAProduct->EndDateTime, (int)pProductInUse->LastUsage.DateTime );

        //  6.  A validation rule exists based on the product ID(3)  of one or more stored value products and the route ID/line ID(7)  of the current trip.

        // Feedback from Keane as to what a "validation rule" is received in an email
        // from Brendan Tagg to Morgan Dell and Richard Jack on 7 July 2014 states:
        //
        //     The good news is that BR_LLSC_6_9 doesn’t have to be implemented as there
        //     is no associated tariff for it work with
        //
        // Interpreting this to mean there is no tariff data for validation rules, which
        // means a validation rule can never exist for the product and route/line
        CsDbg( BRLL_RULE, RULE_PREFIX "Product at directory index %d has no validation rule, ignoring", i );
        continue;


        // Found a product matching all pre-conditions
        storedValueProduct = i;
        pStoredValueDirectory = pMYKI_Directory;
    }

    if ( storedValueProduct == 0 )
    {
        CsDbg( BRLL_RULE, RULE_PREFIX "Bypass - no matching stored value product" );
        pData->ReturnedData.bypassCode = BYPASS_CODE(6, 9, 4, 0);
        return RULE_RESULT_BYPASSED;
    }


    // Execute

    //
    //  1.  Perform a ProductUsage/ScanOff transaction for the applicable product
    //
    CsDbg( BRLL_CHOICE, RULE_PREFIX "ProductInUse: %d  StoredValueProduct: %d", pMYKI_TAControl->ProductInUse, storedValueProduct );

    //  myki_br_ldt_ProductUsage_ScanOff will perform either ScanOff or ForcedScanOff  
    if ( myki_br_ldt_ProductUsage_ScanOff( pData, pStoredValueDirectory ) < 0 )
    {
        CsErrx( RULE_PREFIX "Error calling myki_br_ldt_ProductUsage_ScanOff" );
        return RULE_RESULT_ERROR;
    }

    //
    //  1.  Perform a ProductUpdate/Invalidate transaction for the provisional product
    //
    if ( myki_br_ldt_ProductUpdate_Invalidate( pData, pDirectoryInUse ) < 0 )
    {
        CsErrx( RULE_PREFIX "myki_br_ldt_ProductUpdate_Invalidate() failed" );
        return RULE_RESULT_ERROR;
    }

    CsDbg( BRLL_RULE, RULE_PREFIX "Executed");
    return RULE_RESULT_EXECUTED;
}


