/**************************************************************************
*   ID :  BR_LLSC_5_6
*    Scan-On Provisional Product
*    1.  TAppControl.ProvisionalFare
*    2.  TAppControl.PassengerCode
*    3.  TAppControl.Directory.ProductId
*    4.  TAppTProduct.PurchaseValue
*    5.  TAppTProduct.ControlBitmap
*    6.  TAppTProduct.ZoneLow
*    7.  TAppTProduct.ZoneHigh
*    8.  Static.TransportMode
*    9.  Dynamic.CurrentDateTime
*    10.  Dynamic.CurrentZone
*    11.  Dynamic.StopId
*    12.  Dynamic.ProvisionalZoneLow
*    13.  Dynamic.ProvisionalZoneHigh
*    14.  Tariff.CitySaverZoneFlag
*    15.  Static.ServiceProviderID
*    16.  Dynamic.EntryPointID
*    17.  Dynamic.LineID
*
*   Pre-Conditions
*    1.  None
*
*   The following description has been amended post KA0004 V7.2
*   This is reflected in the text below and should be reflected in the next version of KA0004
*   Description
*    1.  Set the provisional fare(1)  to the correct value for:
*           a.  Usage type (stored value).
*           b.  Current zone(10) .
*           c.  Mode of travel(8) .
*           d.  Route /Stop(11)  OR StationID.
*    2.  Passenger type(2) . (full fare or concession only)
*    3.  If an exception rule for provisional fare exists for the:
*           a.  Passenger type(2) .
*           b.  Provisional low zone(12)  and provisional high zone(13) .
*           c.  Current day of the week/date(9) .
*
*           *****************************************************************************
*           Clarification from Brendon (27-Nov-13).
*             * Uses the DifferentialPricesProvisional.xml as source data
*             * Same search criteria as used for DifferentialPrices
*                * ie: Route, ZoneCount, TimeOfDay even if they are not documented(yet).
*             * Also: ZoneCount is the diff between the Provisional ZoneLow/High
*           *****************************************************************************
*    4.  Then set the provisional fare(1)  to this exception value.
*    5.  Perform a ProductSale transaction for the provisional product:
*        a.  Set the product ID(3)  to n-Hour.
*        b.  If the provisional low zone(12)  minus 1 is marked as City Saver(14)  set the product low zone(6)  to the provisional low zone(12)  -1, else set the product low
*            zone(6)  to the provisional low zone(12) .
*        c.  If the provisional high zone(13)  is marked as City Saver(14)  set the product high zone(7)  to the provisional high zone(13)  plus 1, else set the product high
*            zone(7)  to the provisional high zone(13) .
*        d.  Set the product purchase value(4)  to the provisional fare(1) .
*        e.  Set the provisional bit of the product control bitmap(5)  to 1.
*    6.  Perform a ProductUpdate/Activate transaction for the provisional product.
*    7.  Perform a ProductUsage/ScanOnStoredValue transaction for the provisional product.
*    8.  Create usage log
*        a.  Definition:
*            i.   TxType =  As per Sequence Type (Entry Exit = 7, Exit Only =18; Entry Only =19, On board= 20),
*            ii.  ProviderID =  ServiceProviderID(15)
*            iii. TxDateTime = Current Date time(9)
*            iv.  Location.EntryPoint = EntryPointID(16)
*            v.   Location.Route=  LineID(17)
*            vi.  Location.Stop= Stopid(11)
*        b.  Value:  not defined
*        c.  Usage:
*            i.  Zone = Dynamic.Zone(10)
*            ii. ProductValidationStatus:
*                (1)  Set bit 2 Scan-On true ,
*                (2)  Set bit 4  Provisional  true
*        d.  Product:
*            i.   ProductIssuerId  = As returned from ProductSale transaction at step 7
*            ii.  ProductSerialNo =  As returned from ProductSale transaction at step 7
*            iii. ProductId = As returned from ProductSale transaction at step 7
*
*   Post-Conditions
*    1.  A provisional fare is stored on the smartcard.
*    2.  A provisional product is created and activated.
*    3.  A provisional product is in a scanned-on state.
*
*   Devices
*    Fare payment devices
*
*
 ***********************************************************************/

#include <cs.h>                             // cs (Core Services) - For debug logging
#include <myki_cardservices.h>              // daf_myki_api (Myki Card Services)
#include <myki_cdd_enums.h>                 // myki_ldt (Myki Logical Device Transaction)
#include <LDT.h>                            // myki_ldt (Myki Logical Device Transaction)
#include <myki_cd.h>                        // myki_cd (Myki Configuration Data)

#include "myki_br_rules.h"                  // API for all BR_xxx modules
#include "BR_Common.h"                      // Common utilities, macros, etc

/*----------------------------------------------------------------------------
** FUNCTION           : processExceptionRule
**
** DESCRIPTION        : Process a part of this rule
**
**          3.  If an exception rule for provisional fare exists for the:
**                 a.  Passenger type(2) .
**                 b.  Provisional low zone(12) and provisional high zone(13).
**                 c.  Current day of the week/date(9) .
**          4.  Then set the provisional fare(1) to this exception value.
**
**                      Based on myki_br_getFareStoredValue()
**
** INPUTS             : pData           - Context Data
**                      passengerCode   - Passenger Code
**                      pFare           - Ref to current provisional fare
**                                        May be updated.
**                                        Assumed to be valid pointer
**
** RETURNS            : 0               - All is well
**                                        *pFare may be updated
**                      -1              - Error encountered.
**                                        Error detail has been reported
**
----------------------------------------------------------------------------*/

static  int  processExceptionRule(MYKI_BR_ContextData_t *pData, U8_t passengerCode, int zoneLow, int zoneHigh, int *pFare)
{
    MYKI_CD_DifferentialPricingRequest_t        pricingRequest;
    MYKI_CD_DifferentialPricingProvisional_t    pricingResult;

    //  Init data statructures
    memset( &pricingResult, 0, sizeof( pricingResult) );

    //  Create the basic request, then add in fields specific to this operation
    if (myki_br_buildDifferencePriceRequest(
                pData,
                &pricingRequest,
                zoneLow,
                zoneHigh,
                passengerCode,
                0,
                pData->DynamicData.currentDateTime,
                pData->DynamicData.currentDateTime) < 0)
    {
        //  Error. Details have been reported
        return -1;
    }

    if ( MYKI_CD_getDifferentialPriceProvisionalStructure(&pricingRequest, &pricingResult ) == FALSE )
    {
        // No Exception found. This is not an error
        return 0;
    }

    CsDbg
    (
        BRLL_RULE,
        "BR_LLSC_5_6 : Process Exception Rule : pass = %d, low = %d, high = %d, returned type = %d, discount = %d",
        passengerCode,
        zoneLow,
        zoneHigh,
        pricingResult.discount_type,
        pricingResult.discount
    );

    switch (pricingResult.discount_type)
    {
    case MYKI_CD_DISCOUNT_TYPE_PERCENT:
        *pFare = CalculateDiscountedFare( *pFare, pricingResult.discount );
        break;

    case MYKI_CD_DISCOUNT_TYPE_STATIC:
        *pFare = pricingResult.discount;
        break;

    default:
        CsErrx( "BR_LLSC_5_6 : processExceptionRule - unsupported discount type");
        return -1;
    }
    return 0;

}

//=============================================================================
//
//
//
//=============================================================================

RuleResult_e BR_LLSC_5_6( MYKI_BR_ContextData_t *pData )
{
    MYKI_TAControl_t            *pMYKI_TAControl    = NULL;
    MYKI_Directory_t            *pDirectory         = NULL;
    MYKI_CD_FaresProvisional_t  faresProvisional;
    MYKI_CD_PassengerType_t     passenger;
    int                         zoneLow;
    int                         zoneHigh;
    int                         purchaseValue;
    int                         dirIndex;

    CsDbg( BRLL_RULE, "BR_LLSC_5_6 : Start (Scan-On Provisional Product)" );

    if ( ! pData )
    {
        CsErrx( "BR_LLSC_5_6 : Invalid argument(s)" );
        return RULE_RESULT_ERROR;
    }

    if ( MYKI_CS_TAControlGet( &pMYKI_TAControl ) < 0 )
    {
        CsErrx( "BR_LLSC_5_6 : MYKI_CS_TAControlGet() failed" );
        return RULE_RESULT_ERROR;
    }

    // Description
    //   1.  Set the provisional fare(1)  to the correct value for:
    //       a.  Usage type (stored value).
    //       b.  Current zone(10) .
    //       c.  Mode of travel(8) .
    //       d.  Route /Stop(11)  OR StationID.

    if ( ! MYKI_CD_getFaresProvisionalStructure( pData->DynamicData.provisionalZoneLow, pData->DynamicData.provisionalZoneHigh, &faresProvisional ) )
    {
        CsErrx( "BR_LLSC_5_6 : getFaresProvisionalStructure( %d, %d ) failed", pData->DynamicData.provisionalZoneLow, pData->DynamicData.provisionalZoneHigh );
        return RULE_RESULT_ERROR;
    }

    //   2.  Passenger type(2) . (full fare or concession only)

    if ( ! MYKI_CD_getPassengerTypeStructure( pMYKI_TAControl->PassengerCode, &passenger ) )
    {
        CsErrx( "BR_LLSC_5_6 : getPassengerTypeStructure( %d ) failed", pMYKI_TAControl->PassengerCode );
        return RULE_RESULT_ERROR;
    }

    //  Based on NTS0177 as stated, limit the provisional fare to either full or concession.
    //  Normally I'd expect to apply the percentage discount to the full fare, but because of
    //  this limitation we simply apply the full fare if the discount is 0%, or the concession
    //  fare otherwise.
    //  In any case, we're using the "sv_xxx" fare as this is a stored value transaction (1.a above).

    if ( passenger.percent == 0 )
        purchaseValue = faresProvisional.sv_full;
    else
        purchaseValue = faresProvisional.sv_conc;

    //
    //   3.  If an exception rule for provisional fare exists for the:
    //          a.  Passenger type(2) .
    //          b.  Provisional low zone(12)  and provisional high zone(13) .
    //          c.  Current day of the week/date(9) .
    //              (Implementation Note: And various other not yet documented criteria)
    //   4.  Then set the provisional fare(1) to this exception value.
    //

    if (processExceptionRule(pData, pMYKI_TAControl->PassengerCode, pData->DynamicData.provisionalZoneLow, pData->DynamicData.provisionalZoneHigh, &purchaseValue) < 0)
    {
        //  Error details already reported
        return RULE_RESULT_ERROR;
    }

    //  5   Perform a ProductSale transaction for the provisional product using:

    // 5.a. Set the product ID to n-Hour.
    //      Nothing is done here as the product ID is passed directly to the function call to myki_br_ldt_ProductSale() further down.

    // 5.b.  If the provisional low zone minus 1 is marked as City Saver
    //       set the product low zone to the provisional low zone minus 1,
    //       else set the product low zone to the provisional low zone.

    // 5.c.   If the provisional high zone is marked as City Saver
    //        set the product high zone to the provisional high zone plus 1,
    //        else set the product high zone to the provisional high zone.

    zoneLow = pData->DynamicData.provisionalZoneLow;
    zoneHigh = pData->DynamicData.provisionalZoneHigh;
    ADJUST_FOR_CITYSAVER( zoneLow, zoneHigh );

    // 5.d.   Set the product purchase value to the provisional fare.
    pMYKI_TAControl->ProvisionalFare = purchaseValue;

    // 5.e.  Set the provisional bit of the product control bitmap(5)  to 1
    //       Nothing done here. Will be set within myki_br_ldt_ProductSale().
    //       Using the isProvisional argument

    // 5.   Perform a ProductSale transaction for a provisional product
    //      And finally perform the ProductSale transaction
    //
    if ( ( dirIndex = myki_br_ldt_ProductSale( pData, myki_br_cd_GetProductId(PRODUCT_TYPE_NHOUR), zoneLow, zoneHigh, purchaseValue, TRUE ) ) < 0 )
    {
        CsErrx( "BR_LLSC_5_6 : myki_br_ldt_ProductSale() failed" );
        return RULE_RESULT_ERROR;
    }

    CsDbg
    (
        BRLL_RULE,
        "BR_LLSC_5_6 : myki_br_ldt_ProductSale( id = %d, zl = %d, zh = %d, pv = %d ) returned index %d",
        myki_br_cd_GetProductId( PRODUCT_TYPE_NHOUR ),
        zoneLow,
        zoneHigh,
        purchaseValue,
        dirIndex
    );

    pDirectory = &pMYKI_TAControl->Directory[ dirIndex ];   // No need to check dirIndex range as it's guaranteed correct (or < 0) by myki_br_ldt_ProductSale()

    if ( myki_br_ldt_AppUpdate_SetProductInUse( pData, pDirectory ) < 0 )
    {
        CsErrx( "BR_LLSC_5_6 : myki_br_ldt_AppUpdate_SetProductInUse() failed" );
        return RULE_RESULT_ERROR;
    }

    // 6.   Perform a ProductUpdate/Activate transaction for the provisional product.

    if ( myki_br_ldt_ProductUpdate_Activate( pData, pDirectory ) < 0 )
    {
        CsErrx( "BR_LLSC_5_6 : myki_br_ldt_ProductUpdate_Activate failed" );
        return RULE_RESULT_ERROR;
    }

    // 7.   Perform a ProductUsage/ScanOnStoredValue  transaction for the provisional product.

    if ( myki_br_ldt_ProductUsage_ScanOnStoredValue( pData, pDirectory ) < 0 )
    {
        CsErrx( "BR_LLSC_5_6 : myki_br_ldt_ProductUsage_ScanOnStoredValue failed" );
        return RULE_RESULT_ERROR;
    }

    //  8.  Create usage log
    //      a.  Definition: 
    {
        //      i.  TxType = As per Sequence Type (Entry Exit = 7, Exit Only =18; Entry Only =19, On board= 20), 
        if ( pData->InternalData.UsageLogData.transactionType == MYKI_BR_TRANSACTION_TYPE_NONE )
        {
            pData->InternalData.UsageLogData.transactionType = myki_br_getTransactionType( pData );
        } 

        //      ii. Determine uselog ControlBitmap though look up of the TxUsageType (Done by framework)
        //      iii.ProviderID = ServiceProviderID(15) (Done by framework)
        //      iv. TxDateTime = Current Date time(9) (Done by framework)
        //      v.  Location.EntryPoint = EntryPointID(16) (Done by framework)
        //      vi. Location.Route= LineID(17) (Done by framework)
        //      vii.Location.Stop= Stopid(11) (Done by framework)
    }

    //      b.  Value: not defined

    //      c.  Usage: 
    {
        //      i.  Zone = Dynamic.Zone(10)
        pData->InternalData.UsageLogData.isZoneSet                      = TRUE;
        pData->InternalData.UsageLogData.zone                           = pData->DynamicData.currentZone;

        //      ii.  ProductValidationStatus: 
        //          (1) Set bit 2 Scan-On true , 
        //          (2) Set bit 4 Provisional true 
        pData->InternalData.UsageLogData.isProductValidationStatusSet   = TRUE;
        pData->InternalData.UsageLogData.productValidationStatus       |= TAPP_USAGE_LOG_PROD_VAL_STATUS_SCAN_ON_BITMAP |
                                                                          TAPP_USAGE_LOG_PROD_VAL_STATUS_PROVISIONAL_BITMAP;
    }

    //      d.  Product: 
    {
        //      i.  ProductIssuerId = As returned from ProductSale transaction at step 7 
        pData->InternalData.UsageLogData.isProductIssuerIdSet           = TRUE;
        pData->InternalData.UsageLogData.productIssuerId                = pDirectory->IssuerId;

        //      ii. ProductSerialNo = As returned from ProductSaletransaction at step 7 
        pData->InternalData.UsageLogData.isProductSerialNoSet           = TRUE;
        pData->InternalData.UsageLogData.productSerialNo                = pDirectory->SerialNo;

        //      iii.ProductId = As returned from ProductSale transaction at step 7
        pData->InternalData.UsageLogData.isProductIdSet                 = TRUE;
        pData->InternalData.UsageLogData.productId                      = pDirectory->ProductId;
    }

    //  NOTE:   TAppUsageLog entry is added by application framework.
    pData->InternalData.IsUsageLogUpdated                               = TRUE;
    CsDbg( BRLL_RULE, "BR_LLSC_5_6 : Updated TAppUsageLog" );

    CsDbg( BRLL_RULE, "BR_LLSC_5_6 : Executed" );
    return RULE_RESULT_EXECUTED;
}

