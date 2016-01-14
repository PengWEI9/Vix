/**************************************************************************
*   ID :  BR_LLSC_4_4
*    Change of Mind with Provisional Product
*    1.  TAppControl.ProductInUse
*    2.  TAppTProduct.ControlBitmap
*    3.  TAppTProduct.LastUsageEntryPointId
*    4.  TAppTProduct.LastUsageDateTime
*    5.  Dynamic.EntryPointId
*    6.  Dynamic.CurrentDateTime
*    7.  Tariff.ChangeOfMindPeriod
*    8.  TAppTProduct.LastUsageEntryPoint
*    9.  Static.ModeOfTransport
*    10.  Static.ServiceProviderID
*    11.  TAppControl.Directory.IssuerId
*    12.  TAppControl.Directory.SerialNo
*    13.  Dynamic.CurrentZone
*    14.  Dynamic.CurrentDateTime
*    15.  Usagelog
*    1.  The product in use field1 is not 0 (a product is in a scanned-on state).
*    2.  The provisional bit on the product control bitmap2 of the product in use1 is set to 1.
*    3.  The scan-on date/time4 of the product in use1 plus the change of mind period7 is greater than or equal to the current date/time6.
*    4.  If the Mode of transport by the current device is Rail
*    a.  The scan-on location3 of the product in use1 is equal to the current location5.
*    5.  Else the scan-on location3 of the product in use1 is equal to the current location5 and the scan on Route/Stop3 is equal to the current route/stop of the
*    device.
*    6.  Create usage log
*    a.  Definition:
*    i.  TAppLoadLog.ControlBitmap: Product, Usage
*    ii.  TxType = Change of Mind (14)
*    iii.  ProviderID =  ServiceProviderID10
*    b.  Value:  not defined
*    c.  Usage:
*    i.  Zone = Dynamic.Zone
*    ii.  ProductValidationStatus =: Scan-off,
*    d.  Product:
*    i.  ProductIssuerId  = As returned from ProductUsage/Reverse transaction at step 1
*    ii.  ProductSerialNo =  As returned from ProductUsage/Reverse transaction at step 1
*    iii.  ProductId = As returned from ProductUsage/Reverse transaction at step 1
*    7.
*
*      Description
*    1.  Perform a ProductUsage/Reverse transaction for the product in use1.
*    2.  Perform a ProductSale/Reverse transaction for the product in use1.
*      Post-Conditions
*    1.  The provisional fare is removed from the smartcard.
*    2.  The provisional product is invalidated.
*      Devices
*    Non-directional fare payment devices, exit gates
*
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

RuleResult_e BR_LLSC_4_4( MYKI_BR_ContextData_t *pData )
{
    MYKI_TAControl_t   *pMYKI_TAControl     = NULL;
    MYKI_Directory_t   *pDirectory          = NULL;
    MYKI_TAProduct_t   *pProduct            = NULL;
    U8_t                productIssuerId     = 0;
    U8_t                productId           = 0;
    U16_t               productSerialNo     = 0;

    CsDbg( BRLL_RULE, "BR_LLSC_4_4 : Start (Change of Mind with Provisional Product)" );

    if ( ! pData )
    {
        CsErrx( "BR_LLSC_4_4 : Invalid argument(s)" );
        return RULE_RESULT_ERROR;
    }

    if ( MYKI_CS_TAControlGet( &pMYKI_TAControl ) < 0 )
    {
        CsErrx( "BR_LLSC_4_4 : MYKI_CS_TAControlGet() failed" );
        return RULE_RESULT_ERROR;
    }

    //  1. The product in use field1 is not 0 (a product is in a scanned-on state).

    if ( pMYKI_TAControl->ProductInUse == 0 )
    {
        CsDbg( BRLL_RULE, "BR_LLSC_4_4 : Bypass - No products in use" );
        pData->ReturnedData.bypassCode  = BYPASS_CODE( 4, 4, 1, 0 );
        return RULE_RESULT_BYPASSED;
    }

    //  Get a pointer to the product in use

    if ( myki_br_GetCardProduct( pMYKI_TAControl->ProductInUse, &pDirectory, &pProduct ) < 0 )
    {
        CsErrx( "BR_LLSC_4_4 : myki_br_GetCardProduct( %d ) failed", pMYKI_TAControl->ProductInUse );
        return RULE_RESULT_ERROR;
    }

    //  2. The provisional bit on the product control bitmap2 of the product in use1 is set to 1.

    if ( ! ( pProduct->ControlBitmap & TAPP_TPRODUCT_CONTROL_PROVISIONAL_BITMAP ) )
    {
        CsDbg( BRLL_RULE, "BR_LLSC_4_4 : Bypass - Product Provisional Bit not set" );
        pData->ReturnedData.bypassCode  = BYPASS_CODE( 4, 4, 2, 0 );
        return RULE_RESULT_BYPASSED;
    }

    //  3. The scan-on date/time4 of the product in use1 plus the change of mind period7 is greater than or equal to the current date/time6.

    if ( pProduct->LastUsage.DateTime + pData->Tariff.changeOfMindPeriod < pData->DynamicData.currentDateTime )
    {
        CsDbg( BRLL_RULE, "BR_LLSC_4_4 : Bypass - Scan-on date/time (%d) + Change of mind period (%d) < Current date/time (%d)", pProduct->LastUsage.DateTime, pData->Tariff.changeOfMindPeriod, pData->DynamicData.currentDateTime );
        pData->ReturnedData.bypassCode  = BYPASS_CODE( 4, 4, 3, 0 );
        return RULE_RESULT_BYPASSED;
    }

    //  4. If the Mode of transport by the current device is Rail
    //  a. The scan-on location3 of the product in use1 is equal to the current location5.
    //  5. Else the scan-on location3 of the product in use1 is equal to the current location5 and the scan on Route/Stop3 is equal to the current route/stop of the
    //  device.

    if ( ! myki_br_CurrentLocationIsScanOnLocation( pData, pProduct ) )
    {
        CsDbg( BRLL_RULE, "BR_LLSC_4_4 : Bypass - Scan-on location != Current location" );
        pData->ReturnedData.bypassCode  = BYPASS_CODE( 4, 4, 4, 0 );
        return RULE_RESULT_BYPASSED;
    }

    // 1.  Perform a ProductUsage/Reverse transaction for the product in use1.

    if ( myki_br_ldt_ProductUsage_Reverse( pData, pDirectory ) < 0 )
    {
        CsErrx( "BR_LLSC_4_4 : myki_br_ldt_ProductUsage_Reverse failed" );
        return RULE_RESULT_ERROR;
    }

    //  Saves provisional product details for later used to generate TAppUsageLog record.
    productIssuerId     = pDirectory->IssuerId;
    productId           = pDirectory->ProductId;
    productSerialNo     = pDirectory->SerialNo;

    // 2.  Perform a ProductSale/Reverse transaction for the product in use1.

    if ( myki_br_ldt_ProductSale_Reverse( pData, pDirectory ) < 0 )
    {
        CsErrx( "BR_LLSC_4_4 : myki_br_ldt_ProductSale_Reverse failed" );
        return RULE_RESULT_ERROR;
    }

    if ( pData->StaticData.AcsCompatibilityMode != FALSE )
    {
        //  NOTE:   a) ACS generates TAppUsageLog with zero Product details.
        //          b) ProductSale/Reverse does not clear product serial number.
        productIssuerId     = pDirectory->IssuerId;
        productId           = pDirectory->ProductId;
        productSerialNo     = 0;
    }

    //  6.  Create Usage Log
    //      a.  Definition: 
    {
        //      i.  TAppUsageLog.ControlBitmap: Product, Usage (Done by framework)
        //      ii. TxType = Change of Mind (14)
        pData->InternalData.UsageLogData.transactionType                = MYKI_BR_TRANSACTION_TYPE_CHANGE_OF_MIND;
        //      iii.ProviderID = ServiceProviderID(10) (Done by framework)
        //      iv. Set TxDateTime as current Date time(14) (Done by framework)
    }

    //  b.  Value: not defined 

    //  c.  Usage:
    {
        //  i.  Zone = Dynamic.Zone 
        pData->InternalData.UsageLogData.isZoneSet                      = TRUE;
        pData->InternalData.UsageLogData.zone                           = pData->DynamicData.currentZone;

        //  ii. ProductValidationStatus =: Scan-off,
        pData->InternalData.UsageLogData.isProductValidationStatusSet   = TRUE;
        pData->InternalData.UsageLogData.productValidationStatus       |= TAPP_USAGE_LOG_PROD_VAL_STATUS_SCAN_OFF_BITMAP;
    }

    //  d.  Product: 
    {
        //  i.  ProductIssuerId = As returned from ProductUsage/Reverse transaction at step 1 
        pData->InternalData.UsageLogData.isProductIssuerIdSet           = TRUE;
        pData->InternalData.UsageLogData.productIssuerId                = productIssuerId;

        //  ii. ProductSerialNo = As returned from ProductUsage/Reverse transaction at step 1 
        pData->InternalData.UsageLogData.isProductSerialNoSet           = TRUE;
        pData->InternalData.UsageLogData.productSerialNo                = productSerialNo;

        //  iii.ProductId = As returned from ProductUsage/Reverse transaction at step 
        pData->InternalData.UsageLogData.isProductIdSet                 = TRUE;
        pData->InternalData.UsageLogData.productId                      = productId;
    }

    //  NOTE:   TAppUsageLog entry is added by application framework.
    pData->InternalData.IsUsageLogUpdated                               = TRUE;
    CsDbg( BRLL_RULE, "BR_LLSC_4_4 : Updated TAppUsageLog" );

    pData->ReturnedData.acceptReason = MYKI_BR_ACCEPT_CHANGE_OF_MIND;
    CsDbg( BRLL_RULE, "BR_LLSC_4_4 : Executed" );
    return RULE_RESULT_EXECUTED;
}
