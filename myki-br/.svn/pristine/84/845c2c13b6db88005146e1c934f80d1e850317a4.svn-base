/**************************************************************************
*   ID :  BR_LLSC_5_8
*    Apply Premium Surcharge
*    1.  TAppControl.ProductInUse
*    2.  TAppTProduct.ControlBitmap
*    3.  Tariff.PremiumSurcharge
*    4.  Dynamic.Zone
*    5.  Static.ServiceProviderID
*    6.  Dynamic.EntryPointID
*    7.  Dynamic.LineID
*    8.  Dynamic.StopID
*    9.  Dynamic.CurrentDateTime
*    10.  UsageLog
*
*   Pre-Conditions
*    1.  The premium surcharge field(3)  is not 0.
*    2.  The product in use(1)  is not 0
*   
*   Description
*    1.  Perform a ProductSale/Surchage using the surcharge product ID on the product in use(1) 
*    2.  Perform a TPurseUsage/Transit transaction for the premium surcharge amount.
*    3.  Create usage log(10) 
*        a.  Definition: not defined
*        b.  Value:
*            i.  TxValue  = TxValue +  the value as returned from step 2
*            ii.  NewTPurseBalance - the new t-purse value
*            iii.  PaymentMethod
*        c.  Usage:
*            i.  Zone = Dynamic.Zone(8) 
*            ii.  ProductValidationStatus:  ProductValidationStatus + Premium Surcharge Bit
*        d.  Product:
*            i.  ProductIssuerId  = As returned Product Update/Extend
*            ii.  ProductSerialNo =  As returned Product Update/Extend
*            iii.  ProductId = As returned from Product Update/Extend
*
*
*   Post-Conditions
*    1.  The premium surcharge is deducted from the T-Purse balance.
*    2.  The premium surcharge bit is set on the product in use.
* 
*   Devices
*    Fare payment devices
*
 ***********************************************************************/

#include <cs.h>                 // For debug logging
#include <myki_cardservices.h>  // Myki Card Services
#include <myki_cdd_enums.h>
#include <LDT.h>                // Logical Device Transaction

#include "myki_br_rules.h"      // API for all BR_xxx modules
#include "BR_Common.h"          // Common utilities, macros, etc

//=============================================================================
//
//
//
//=============================================================================

RuleResult_e BR_LLSC_5_8( MYKI_BR_ContextData_t *pData )
{
    MYKI_TAControl_t            *pMYKI_TAControl;
    MYKI_Directory_t            *pDirInUse = NULL;
    MYKI_TAProduct_t            *pPdtInUse = NULL;
    MYKI_TAPurseBalance_t       *pPurseBalance   = NULL;

    CsDbg( BRLL_RULE, "BR_LLSC_5_8 : Start (Apply Premium Surcharge)" );

    if ( ! pData )
    {
        CsErrx( "BR_LLSC_5_8 : Invalid argument(s)" );
        return RULE_RESULT_ERROR;
    }

    if ( MYKI_CS_TAControlGet( &pMYKI_TAControl ) < 0 )
    {
        CsErrx( "BR_LLSC_5_8 : MYKI_CS_TAControlGet() failed" );
        return RULE_RESULT_ERROR;
    }

    /*  Data Fields
        1.  TAppControl.ProductInUse            pMYKI_TAControl->ProductInUse
        2.  TAppTProduct.ControlBitmap          pPdtInUse->ControlBitmap
        3.  Tariff.PremiumSurcharge             pData->Tariff.premiumSurcharge
        4.  Dynamic.Zone                        pData->DynamicData.Zone
        5.  Static.ServiceProviderID            pData->StaticData.serviceProviderId
        6.  Dynamic.EntryPointID                pData->DynamicData.entryPointId
        7.  Dynamic.LineID                      pData->DynamicData.lineId
        8.  Dynamic.StopID                      pData->DynamicData.stopId
        9.  Dynamic.CurrentDateTime             pData->DynamicData.currentDateTime
        10. UsageLog                            pData->DynamicData.usageLog
        */


    /*  Pre-Conditions */

    // 1.  The premium surcharge field(3)  is not 0.
    if (pData->Tariff.premiumSurcharge == 0)
    {
        CsDbg( BRLL_RULE, "BR_LLSC_5_8 : Pre-condition BR_LLSC_5_8.1 not met. premiumSurcharge is zero");
        return RULE_RESULT_BYPASSED;
    }


    // 2.  The product in use(1)  is not 0
    if (pMYKI_TAControl->ProductInUse == 0)
    {
        CsDbg( BRLL_RULE, "BR_LLSC_5_8 : Pre-condition BR_LLSC_5_8.2 not met. ProductInUse is zero");
        return RULE_RESULT_BYPASSED;
    }

    /* Get product and directory structure in use */
    if ( MYKI_CS_TAProductGet( (U8_t)( pMYKI_TAControl->ProductInUse - 1 ), &pPdtInUse ) < 0 )
    {
        CsErrx( "BR_LLSC_5_8 : MYKI_CS_TAProductGet() failed" );
        return RULE_RESULT_ERROR;
    }

    pDirInUse = &pMYKI_TAControl->Directory[ pMYKI_TAControl->ProductInUse ];


    /*  Description */

    // 1.  Perform a ProductSale/Surchage using the surcharge product ID on the product in use1
    // Need a support from LDT for ProductSale/Surchage
    // myki_br_ldt_ProductSale_Surcharge ( pData, pDirInUse );

    // 2.  Perform a TPurseUsage/Transit transaction for the premium surcharge amount.
    // Don't know what the surcharge value is, for now assume pData->Tariff.premiumSurcharge
    if ( myki_br_ldt_PurseUsage_Transit( pData, pData->Tariff.premiumSurcharge ) < 0 )
    {
        CsErrx("BR_LLSC_6_8 : myki_br_ldt_PurseUsage_Transit() failed");
        return RULE_RESULT_ERROR;
    }

    if ( MYKI_CS_TAPurseBalanceGet( &pPurseBalance ) >= 0 )
    {
        pData->ReturnedData.remainingBalance = pPurseBalance->Balance;
    }

    //  3.  Create usage log
    //      a.  Definition: not defined 

    //      b.  Value: 
    {
        //      i.  TxValue = TxValue + the value as returned fromstep 2 
        pData->InternalData.UsageLogData.isTransactionValueSet          = TRUE;
        pData->InternalData.UsageLogData.transactionValue              += pData->Tariff.premiumSurcharge;

        //      ii. NewTPurseBalance – the new t-purse value 
        pData->InternalData.UsageLogData.isNewTPurseBalanceSet          = TRUE;
        pData->InternalData.UsageLogData.newTPurseBalance               = pData->ReturnedData.remainingBalance;

        //      iii.PaymentMethod 
        pData->InternalData.UsageLogData.isPaymentMethodSet             = TRUE;
        pData->InternalData.UsageLogData.paymentMethod                  = TAPP_USAGE_LOG_PAYMENT_METHOD_TPURSE;
    }

    //      c.  Usage: 
    {
        //      i.  Zone = Dynamic.Zone(8)
        pData->InternalData.UsageLogData.isZoneSet                      = TRUE;
        pData->InternalData.UsageLogData.zone                           = pData->DynamicData.currentZone;

        //      ii. ProductValidationStatus: ProductValidationStatus + Premium Surcharge Bit 
        pData->InternalData.UsageLogData.isProductValidationStatusSet   = TRUE;
        pData->InternalData.UsageLogData.productValidationStatus       |= TAPP_USAGE_LOG_PROD_VAL_STATUS_PREMIUM_BITMAP;
    }

    //  d.  Product: 
    {
        //      i.  ProductIssuerId = As returned Product Update/Extend 
        pData->InternalData.UsageLogData.isProductIssuerIdSet           = TRUE;
        pData->InternalData.UsageLogData.productIssuerId                = pDirInUse->IssuerId;

        //      ii. ProductSerialNo = As returned Product Update/Extend 
        pData->InternalData.UsageLogData.isProductSerialNoSet           = TRUE;
        pData->InternalData.UsageLogData.productSerialNo                = pDirInUse->SerialNo;

        //      iii.ProductId = As returned from Product Update/Extend
        pData->InternalData.UsageLogData.isProductIdSet                 = TRUE;
        pData->InternalData.UsageLogData.productId                      = pDirInUse->ProductId;
    }

    //  NOTE:   TAppUsageLog entry is added by application framework.
    pData->InternalData.IsUsageLogUpdated                               = TRUE;
    CsDbg( BRLL_RULE, "BR_LLSC_5_8 : Updated TAppUsageLog" );

    CsDbg( BRLL_RULE, "BR_LLSC_5_8 : Executed");
    return RULE_RESULT_EXECUTED;
}

