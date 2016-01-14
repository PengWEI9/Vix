/**************************************************************************
*   ID :  BR_LLSC_6_8
*
*    Finalize Scan-Off
*    1.    TAppControl.ProductInUse
*    2.    TAppTProduct.ControlBitmap
*    3.    TAppTProduct.EndDateTime
*    4.    TAppTProduct.LastUsageZone
*    5.    Dynamic.CurrentZone
*    6.    Dynamic.CurrentTripZoneLow
*    7.    Dynamic.CurrentTripZoneHigh
*    8.    Tariff.ApplicableFare
*    9.    Tariff.NHourExtendThreshold
*    10.   Tariff.NHourExtendPeriod
*    11.   Dynamic.ExtendedProducts
*    12.   Tariff.CitySaverZoneFlag
*    13.   Dynamic.CurrentZone
*    14.   Dynamic.CurrentDateTime
*    15.   Static.ServiceProviderID
*    16.   Dynamic.EntryPointID
*    17.   Dynamic.LineID
*    18.   Dynamic.StopID
*    19.   TAppTPurse.TPurseBalanace
*    20.   Dynamic.IsOffpeak
*    21.   Dynamic.IsForceScanOff
*    22.   Dynamic.USageLog
*    23.   Dynamic.OriginatingInformation
*
*
* Pre-Requisites
*    1.    None
*
* Description
*    1.  If the product in use(1) is set then
*        a.    If the current trip is not a force scan off  (i.e isForceScanOff = false) then
*                   perform a ProductUsage/ScanOff transaction with originating information(23).
*        b.    Else
*                   perform a ProductUsage/ForceScanOff transaction with originating information(23).
*
*    2.  Else if product in use(1) is not set
*        a.    If the current trip is not a force scan off (i.e isForceScanOff =false) then
*            i.     For every product that is active and that the product covers part of the current trip zone range(6,7)
*                       perform a ProductUseage/ScanOff transaction with originating information(23).
*        b.    Else
*            i.     For every product that is active and that the product covers part of the current trip zone range(6,7)
*                       perform a ProductUsageForce/ScanOff transaction with originating information(23).
*
*    3.  For each n-Hour product within the trip zone range
*        a.    Perform a ProductUpdate/none
*           i.    Clear the premium surcharge bit of the product control bitmap(2) 
*    4.  If the applicable fare(8) is greater than 0, perform a TPurseUsage/Transit transaction for the applicable fare(8)
*    5.  If the applicable fare(8) is less than 0,    perform a TPurseUsage/Reverse transaction for the applicable fare(8)
*    6. Create usage log
*         a. Definition:
*              i. If the TXT Type is not set then set the TxType = As per Sequence Type (Entry Exit = 7, Exit Only =18; Entry Only =19, On board= 20),
*              ii. Determine uselog ControlBitmap though look up of the TxUsageType
*              iii. ProviderID = ServiceProviderID(15) 
*              iv. TxDateTime = Current Date time(14) 
*              v. Location.EntryPoint = EntryPointID(16) 
*              vi. Location.Route= LineID(17) 
*              vii. Location.Stop= Stopid(11) 
*         b. Value:
*              i. TxValue = applicable fare(8) 
*              ii. NewTPurseBalance = the value returned from step 4 or 5
*              iii. PaymentMethod = 1 – tpurse.
*         c. Usage:
*              i. Zone = Dynamic.Zone(10) 
*              ii. ProductValidationStatus:
*                   (1) Note: set by other Business rules
*                   (2) Note: Set elsewhere
*                   (3) Set bit 2- Scan off = true
*                   (4) If isForceScanoff = True , set bit 3- Forced
*                   (5) Note: Set elsewhere
*                   (6) Note:Bit 5 is controlled via tpuse LDT
*                   (7) If isOffPeak = true, set bit 6 Offpeak = true
*                   (8) Note: IsPremium is set elsewhere.
*         d. Product:
*              i. ProductIssuerId = As returned from Scan off
*              ii. ProductSerialNo = As returned from Scan off; choose any
*              iii. ProductId = As returned from Scan off
*
* Post-Conditions
*    1.    An applicable fare has been deducted from the T-Purse balance.
*    2.    The product in use has been scanned-off.
*    3.    No products are in a scanned-on state.
*
* Devices
*    Fare payment devices
*
 ***********************************************************************/

#include <cs.h>                 // For debug logging
#include <myki_cardservices.h>  // Myki Card Services
#include <LDT.h>                // Logical Device Transaction
#include "BR_Common.h"
#include <myki_cardservices.h>
#include <LDT.h>

#include "myki_br_rules.h"      // API for all BR_xxx modules
#include "BR_Common.h"          // Common utilities, macros, etc

//=============================================================================
//
//
//
//=============================================================================

RuleResult_e BR_LLSC_6_8( MYKI_BR_ContextData_t *pData )
{

    MYKI_TAControl_t        *pMYKI_TAControl = NULL;
    MYKI_TAProduct_t        *pMYKI_TAProduct = NULL;
    MYKI_Directory_t        *pDirectoryInUse = NULL;
    MYKI_TAPurseBalance_t   *pPurseBalance   = NULL;
    ProductType_e           productType;
    int i;

    CsDbg( BRLL_RULE, "BR_LLSC_6_8 : Start (Finalize Scan-Off)" );

    if ( ! pData )
    {
        CsErrx( "BR_LLSC_6_8 : Invalid argument(s)" );
        return RULE_RESULT_ERROR;
    }


    //Pre-Requisites
    //none

    //Execution

    if ( MYKI_CS_TAControlGet( &pMYKI_TAControl ) < 0 )
    {
        CsErrx( "BR_LLSC_6_8 : MYKI_CS_TAControlGet failed" );
        return RULE_RESULT_ERROR;
    }

    //  Execution:1
    //  1.  If the product in use(1) is set then
    //      a.    If the current trip is not a force scan off  (i.e isForceScanOff = false) then
    //                 perform a ProductUsage/ScanOff transaction with originating information(23).
    //      b.    Else
    //                 perform a ProductUsage/ForceScanOff transaction with originating information(23).
    //  
    if ( pMYKI_TAControl->ProductInUse != 0 )
    {

        CsDbg( BRLL_CHOICE, "BR_LLSC_6_8 : ProductInUse : %d", pMYKI_TAControl->ProductInUse );

        pDirectoryInUse = &pMYKI_TAControl->Directory[ pMYKI_TAControl->ProductInUse ];

        // Populate returned data
        if ( MYKI_CS_TAProductGet( (U8_t)( pMYKI_TAControl->ProductInUse - 1 ), &pMYKI_TAProduct ) == 0 )
        {
            pData->ReturnedData.productId = pDirectoryInUse->ProductId;
            pData->ReturnedData.isProvisional =  (pMYKI_TAProduct->ControlBitmap & TAPP_TPRODUCT_CONTROL_PROVISIONAL_BITMAP) ? 1 : 0;
            pData->ReturnedData.zoneLow   = pMYKI_TAProduct->ZoneLow;
            pData->ReturnedData.zoneHigh  = pMYKI_TAProduct->ZoneHigh;
            pData->ReturnedData.expiryDateTime = pMYKI_TAProduct->EndDateTime;
        }

        //  myki_br_ldt_ProductUsage_ScanOff will perform either ScanOff or ForcedScanOff  
        if ( myki_br_ldt_ProductUsage_ScanOff( pData, pDirectoryInUse ) < 0 )
        {
            CsErrx("BR_LLSC_6_8 : Error calling myki_br_ldt_ProductUsage_ScanOff");
            return RULE_RESULT_ERROR;
        }
    }
    else
    {
        //  Execution:2
        //  Else if product in use(1) is not set
        //    a.    If the current trip is not a force scan off (i.e isForceScanOff =false) then
        //        i.     For every product that is active and that the product covers part of the current trip zone range(6,7)
        //               perform a ProductUseage/ScanOff transaction with originating information(23).
        //    b.    Else
        //        i.     For every product that is active and that the product covers part of the current trip zone range(6,7)
        //               perform a ProductUsageForce/ScanOff transaction with originating information(23).

        for ( i = 1; i < DIMOF( pMYKI_TAControl->Directory ); i++ )
        {
            MYKI_Directory_t   *pDirectory  = NULL;

            if ( myki_br_GetCardProduct( i, &pDirectory, NULL ) < 0 )
            {
                CsErrx( "BR_LLSC_6_8 : myki_br_GetCardProduct(%d) failed", i );
                return RULE_RESULT_ERROR;
            }

            // 2.a.i The product status is Active
            // 2.a.i The product covers part of the current trip zone range

            if ( pDirectory->Status == TAPP_CONTROL_DIRECTORY_STATUS_ACTIVATED )
            {
                if ( myki_br_GetCardProduct( i, &pDirectory, &pMYKI_TAProduct ) < 0 )
                {
                    CsErrx( "BR_LLSC_6_8 : myki_br_GetCardProduct(%d) failed", i );
                    return RULE_RESULT_ERROR;
                }

                CsDbg( BRLL_CHOICE, "BR_LLSC_6_8 : Dynamic.CurrentTripZone(%d,%d) TAppProduct[%d].Zone(%d,%d)",
                        pData->DynamicData.currentTripZoneLow, pData->DynamicData.currentTripZoneHigh,
                        i, pMYKI_TAProduct->ZoneLow, pMYKI_TAProduct->ZoneHigh );

                if
                (
                    ( pData->DynamicData.currentTripZoneLow  >= pMYKI_TAProduct->ZoneLow  &&
                      pData->DynamicData.currentTripZoneLow  <= pMYKI_TAProduct->ZoneHigh ) ||
                    ( pData->DynamicData.currentTripZoneHigh >= pMYKI_TAProduct->ZoneLow  &&
                      pData->DynamicData.currentTripZoneHigh <= pMYKI_TAProduct->ZoneHigh )
                )
                {
                    CsDbg( BRLL_CHOICE, "BR_LLSC_6_8 : Product %d is active and in range - scan off", i );

                    // Populate returned data
                    pData->ReturnedData.productId       = pDirectory->ProductId;
                    pData->ReturnedData.isProvisional   = (pMYKI_TAProduct->ControlBitmap & TAPP_TPRODUCT_CONTROL_PROVISIONAL_BITMAP) ? 1 : 0;
                    pData->ReturnedData.zoneLow         = pMYKI_TAProduct->ZoneLow;
                    pData->ReturnedData.zoneHigh        = pMYKI_TAProduct->ZoneHigh;
                    pData->ReturnedData.expiryDateTime  = pMYKI_TAProduct->EndDateTime;

                    //  myki_br_ldt_ProductUsage_ScanOff will perform either ScanOff or ForcedScanOff  
                    if ( myki_br_ldt_ProductUsage_ScanOff( pData, pDirectory ) < 0 )
                    {
                        CsErrx("BR_LLSC_6_8 : myki_br_ldt_ProductUsage_ScanOff() failed");
                        return RULE_RESULT_ERROR;
                    }

                    // pick whatever product that used for scan-off
                    pDirectoryInUse = pDirectory;
                }
            }
        }
    }

    //  3.  For each n-Hour product within the trip zone range
    //       a.    Perform a ProductUpdate/none
    //          i.    Clear the premium surcharge bit of the product control bitmap(2) 
    for ( i = 1; i < DIMOF( pMYKI_TAControl->Directory ); i++ )
    {
        MYKI_Directory_t   *pDirectory  = NULL;

        if ( myki_br_GetCardProduct( i, &pDirectory, NULL ) < 0 )
        {
            CsErrx( "BR_LLSC_6_8 : myki_br_GetCardProduct(%d) failed", i );
            return RULE_RESULT_ERROR;
        }

        if ( pDirectory->Status == TAPP_CONTROL_DIRECTORY_STATUS_ACTIVATED )
        {
            //  NOTE:   Entitlement product can be of type PRODUCT_TYPE_UNKNOWN!
            //
            productType = myki_br_cd_GetProductType( pDirectory->ProductId );

            if ( productType == PRODUCT_TYPE_NHOUR )
            {
                if ( myki_br_GetCardProduct( i, &pDirectory, &pMYKI_TAProduct ) < 0 )
                {
                    CsErrx( "BR_LLSC_6_8 : myki_br_GetCardProduct(%d) failed", i );
                    return RULE_RESULT_ERROR;
                }

                if
                (
                    pMYKI_TAProduct->ZoneLow  >= pData->DynamicData.currentTripZoneLow  &&
                    pMYKI_TAProduct->ZoneHigh <= pData->DynamicData.currentTripZoneHigh
                )
                {
                    CsDbg( BRLL_CHOICE, "BR_LLSC_6_8 : Product %d is nHour and in range - clear premium surcharge", i );
                    if ( myki_br_ldt_ProductUpdate_ClearPremiumSurcharge( pData, pDirectory ) < 0 )
                    {
                        CsErrx("BR_LLSC_6_8 : myki_br_ldt_ProductUpdate_ClearPremiumSurcharge() failed");
                        return RULE_RESULT_ERROR;
                    }
                }
            }
        }
    }

    //  4.  If the applicable fare(8) is greater than 0, perform a TPurseUsage/Transit transaction for the applicable fare(8)
    //  5.  If the applicable fare(8) is less than 0,    perform a TPurseUsage/Reverse transaction for the applicable fare(8)
    if ( pData->ReturnedData.applicableFare > 0 )
    {
        if ( myki_br_ldt_PurseUsage_Transit( pData, pData->ReturnedData.applicableFare ) < 0 )
        {
            CsErrx("BR_LLSC_6_8 : myki_br_ldt_PurseUsage_Transit() failed");
            return RULE_RESULT_ERROR;
        }
    }
    else if ( pData->ReturnedData.applicableFare < 0 )
    {
        if ( myki_br_ldt_PurseUsage_Reverse( pData, -pData->ReturnedData.applicableFare ) < 0 )     // Note : Negative fare is changed to positive refund amount
        {
            CsErrx("BR_LLSC_6_8 : myki_br_ldt_PurseUsage_Reverse() failed");
            return RULE_RESULT_ERROR;
        }
    }
    else
    {
        CsDbg( BRLL_CHOICE, "BR_LLSC_6_8 : applicableFare is zero");
    }

    if ( MYKI_CS_TAPurseBalanceGet( &pPurseBalance ) >= 0 )
    {
        pData->ReturnedData.remainingBalance = pPurseBalance->Balance;
    }

    // 6. Create usage log
    //     a. Definition:
    //          i.   If the TXT Type is not set then set the TxType = As per Sequence Type (Entry Exit = 7, Exit Only =18; Entry Only =19, On board= 20),
    //
    pData->InternalData.IsUsageLogUpdated = TRUE;
    if (pData->InternalData.UsageLogData.transactionType == MYKI_BR_TRANSACTION_TYPE_NONE)
    {
        pData->InternalData.UsageLogData.transactionType = myki_br_getTransactionType(pData); 
    }

    //
    //          ii.  Determine uselog ControlBitmap though look up of the TxUsageType
    //               Implementation Note: This is done by the BR Framework in MYKI_BR_Execute() after sequence has been run
    // 
    //          iii. ProviderID = ServiceProviderID(15) 
    //          iv.  TxDateTime = Current Date time(14) 
    //          v.   Location.EntryPoint = EntryPointID(16) 
    //          vi.  Location.Route= LineID(17)
    //          vii. Location.Stop= Stopid(11) 
    //
    pData->InternalData.UsageLogData.providerId = pData->StaticData.serviceProviderId; 
    pData->InternalData.UsageLogData.transactionDateTime = pData->DynamicData.currentDateTime;
    pData->InternalData.UsageLogData.entryPointId = pData->DynamicData.entryPointId;
    pData->InternalData.UsageLogData.routeId = pData->DynamicData.lineId;
    pData->InternalData.UsageLogData.stopId = pData->DynamicData.stopId;

    // Value
    //     b. Value:
    //          i. TxValue = applicable fare(8) 
    //          ii. NewTPurseBalance = the value returned from step 4 or 5
    //          iii. PaymentMethod = 1 – tpurse.
    // 
    pData->InternalData.UsageLogData.isTransactionValueSet          = TRUE;
    pData->InternalData.UsageLogData.transactionValue               = pData->ReturnedData.applicableFare;
    pData->InternalData.UsageLogData.isNewTPurseBalanceSet          = TRUE;
    pData->InternalData.UsageLogData.newTPurseBalance               = pData->ReturnedData.remainingBalance;
    pData->InternalData.UsageLogData.isPaymentMethodSet             = TRUE;
    pData->InternalData.UsageLogData.paymentMethod                  = TAPP_USAGE_LOG_PAYMENT_METHOD_TPURSE;

    //     c. Usage:
    //          i. Zone = Dynamic.Zone(10)
    // 
    pData->InternalData.UsageLogData.isZoneSet                      = TRUE;
    pData->InternalData.UsageLogData.zone                           = pData->DynamicData.currentZone;

    //  
    //          ii. ProductValidationStatus:
    //               (1) Note: set by other Business rules
    //               (2) Note: Set elsewhere
    //               (3) Set bit 2- Scan off = true
    //               (4) If isForceScanoff = True , set bit 3- Forced
    //               (5) Note: Set elsewhere
    //               (6) Note:Bit 5 is controlled via tpuse LDT
    //               (7) If isOffPeak = true, set bit 6 Offpeak = true
    //               (8) Note: IsPremium is set elsewhere.
    //
    pData->InternalData.UsageLogData.isProductValidationStatusSet   = TRUE;
    pData->InternalData.UsageLogData.productValidationStatus       |= TAPP_USAGE_LOG_PROD_VAL_STATUS_SCAN_OFF_BITMAP;
    if ( pData->DynamicData.isForcedScanOff )
    {
        pData->InternalData.UsageLogData.productValidationStatus   |= TAPP_USAGE_LOG_PROD_VAL_STATUS_FORCED_BITMAP;
    }

    // TODO: MBU-687, MBU-701
    // Reinstate this code to set the OffPeak in the product usage log. Currently not being set correctly
    // in ACS ETH in 6_20 so we are removing it from here to pass the tests.
    // Reinstating this as it breaks more test without this than with this.
    if ( pData->DynamicData.isOffPeak )
    {
        pData->InternalData.UsageLogData.productValidationStatus   |= TAPP_USAGE_LOG_PROD_VAL_STATUS_OFFPEAK_BITMAP;
    }

    //     d. Product:
    //          i. ProductIssuerId = As returned from Scan off
    //          ii. ProductSerialNo = As returned from Scan off; choose any
    //          iii. ProductId = As returned from Scan off

    if (pDirectoryInUse != NULL)
    {
        pData->InternalData.UsageLogData.isProductIssuerIdSet           = TRUE;
        pData->InternalData.UsageLogData.productIssuerId                = pDirectoryInUse->IssuerId;
        pData->InternalData.UsageLogData.isProductSerialNoSet           = TRUE;
        pData->InternalData.UsageLogData.productSerialNo                = pDirectoryInUse->SerialNo;
        pData->InternalData.UsageLogData.isProductIdSet                 = TRUE;
        pData->InternalData.UsageLogData.productId                      = pDirectoryInUse->ProductId;
    }

    CsDbg( BRLL_RULE, "BR_LLSC_6_8 : Executed" );
    return RULE_RESULT_EXECUTED;
}

