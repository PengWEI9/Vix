/* -*- mode: c; tabs: 4; -*- */
/*=============================================================================
**
**    Vix Technology                   Licensed software
**    (C) 2014                         All rights reserved
**
**=============================================================================
**
**  Project/Product : MBU
**  Filename        : BR_LLSC_5_9.c
**  Author(s)       : ???
**
**  ID              : BR_LLSC_5_9 - NTS0177 v7.3
**
**  Name            : Location Data Unavailable at  Scan-On
**
**  Data Fields     :
**
**      1.  TAppControl.Directory.ProductId 
**      2.  TAppTProduct.PurchaseValue 
**      3.  TAppTProduct.ControlBitmap 
**      4.  TAppTProduct.ZoneLow 
**      5.  TAppTProduct.ZoneHigh 
**      6.  Static.TransportMode 
**      7.  Dynamic.CurrentDateTime 
**      8.  Dynamic.CurrentZone 
**      9.  Dynamic.StopId 
**      10. Dynamic.ProvisionalZoneLow 
**      11. Dynamic.ProvisionalZoneHigh 
**      12. Tariff.CitySaverZoneFlag 
**      13. TAppControl.ProductInUse 
**      14. TAppTProduct.ControlBitmap 
**      15. Dynamic.LocationDataUnavailable 
**      16. Static.ModeOfTransport 
**      17. Static.ServiceProviderId 
**      18. Dynamic.EntryPointID 
**      19. Dynamic.LineID 
**      20. Tariff.NHourDuration
**
**
**  Pre-Conditions  :
**
**      1.  The Mode of transport(16) for the current device is(1) is not Rail
**      2.  The data for current route/stop is currently not available
**          (ie Dynamic.LocationDataUnavailable(15) is true)
**
**  Description     :
**
**      1.  Perform a ProductSale transaction for the provisional product: 
**          a.  Set the product ID(1) to n-Hour. 
**          b.  Set the provisional zone(4,5) according to the current route/stop
**          c.  Set the vehicle/route/stop to the currently known vehicle/route/stop
**          d.  If the provisional low zone(10) minus 1 is marked as City Saver(12)
**              set the product low zone(6) to the provisional low zone(10) -1,
**              else set the product low zone(6) to the provisional low zone(10). 
**          e.  If the provisional high zone(11) is marked as City Saver(12)
**              set the product high zone(7) to the provisional high zone(11) plus 1,
**              else set the product high zone(7) to the provisional high zone(11). 
**          f.  Set the provisional bit of the product control bitmap(3) to 1.
**          g.  Set the product purchase value(2) to 0.0
**          h.  Start date time to the current datetime(7) (Do not round seconds)
**          i.  End date time to the determined hour duration(20)
**      2.  Perform a ProductUpdate/Activate transaction forthe provisional product. 
**      3.  Perform a ProductUsage/ScanOnStoredValue transaction for the provisional
**          product. 
**      4.  Create usage log 
**          a.  Definition:
**              i.  TxType = As per Sequence Type (Entry Exit = 7, Exit Only = 18;
**                  Entry Only = 19, On board = 20),
**              ii. Determine uselog ControlBitmap though look up of the TxUsageType 
**              iii.ProviderID = ServiceProviderID(17)
**              iv. TxDateTime = Current Date time(7)
**              v.  Location.EntryPoint = EntryPointID(18)
**              vi. Location.Route = LIne(19)
**              vii.Location.Stop = Stopid(9)
**          b.  Value: not defined
**          c.  Usage: 
**              i.  Zone = Dynamic.Zone(8)
**              ii. ProductValidationStatus: 
**                  (1) Set bit 2 Scan-On true,
**                  (2) Set bit 4 Provisional true
**          d.  Product:
**              i.  ProductIssuerId = As returned from ProductSale transaction at step 7 
**              ii. ProductSerialNo = As returned from ProductSaletransaction at step 7 
**              5.  ProductId = As returned from ProductSale transaction at step 7
**
**  Post-Conditions :
**
**      1.  A provisional product for the current route with a value of $0.00 has
**          been created.
**
**  Devices         :
**
**      Fare payment devices
**
**  Member(s)       :
**      BR_LLSC_5_9             [public]    business rule
**
**  Information     :
**   Compiler(s)    : C
**   Target(s)      : Independent
**
**  Subversion      :
**      $Id: $
**      $HeadURL: $
**
**  History         :
**   Vers.  Date        Aut.  Type     Description
**   -----  ----------  ----  -------  ----------------------------------------
**    1.00  ??.??.??    ???   Create
**    1.01  25.06.14    ANT   Add      Implemented the rest of business rule
**
**===========================================================================*/

#include <cs.h>                 // For debug logging
#include <myki_cardservices.h>  // Myki Card Services
#include <LDT.h>                // Logical Device Transaction

#include "myki_br_rules.h"      // API for all BR_xxx modules
#include "BR_Common.h"          // Common utilities, macros, etc

/*==========================================================================*
**
**  BR_LLSC_5_9
**
**  Description     :
**      Implements business rule BR_LLSC_5_9
**
**  Parameters      :
**      pData               [I/O]   BR context data
**
**  Returns         :
**      RULE_RESULT_EXECUTED
**      RULE_RESULT_BYPASSED
**      RULE_RESULT_ERROR
**
**  Notes           :
**
**==========================================================================*/

RuleResult_e BR_LLSC_5_9( MYKI_BR_ContextData_t *pData )
{
    MYKI_TAControl_t           *pMYKI_TAControl     = NULL;
    MYKI_Directory_t           *pDirectory          = NULL;
    int                         DirIndex            = -1;
    TAppUsageLog_t             *pTAppUsageLog       = NULL;

    CsDbg( BRLL_RULE, "BR_LLSC_5_9 : Start (Location Data Unavailable at  Scan-On)" );

    if ( pData == NULL )
    {
        CsErrx( "BR_LLSC_5_9 : Invalid argument(s)" );
        return RULE_RESULT_ERROR;
    }

    if ( MYKI_CS_TAControlGet( &pMYKI_TAControl ) < 0 )
    {
        CsErrx( "BR_LLSC_5_9 : MYKI_CS_TAControlGet() failed" );
        return RULE_RESULT_ERROR;
    }

    /*  PRE-CONDITIONS */
    {
        /*  1.  The Mode of transport(16) for the current device is(1) is not Rail */
        if ( pData->InternalData.TransportMode == TRANSPORT_MODE_RAIL )
        {
            CsDbg( BRLL_RULE, "BR_LLSC_5_9 : Bypassed - Device Transport Mode is RAIL" );
            pData->ReturnedData.bypassCode  = BYPASS_CODE( 5, 9, 1, 0 );
            return RULE_RESULT_BYPASSED;
        }

        /*  2.  The data for current route/stop is currently not available
                (ie Dynamic.LocationDataUnavailable(15) is true) */
        if ( pData->DynamicData.locationDataUnavailable == FALSE )
        {
            CsDbg( BRLL_RULE, "BR_LLSC_5_9 : Bypass - current route/stop is available" );
            pData->ReturnedData.bypassCode  = BYPASS_CODE( 5, 9, 2, 0 );
            return RULE_RESULT_BYPASSED;
        }
    }

    /*  PROCESSING */
    {
        int ProductId       = 0;
        int ZoneLow         = 0;
        int ZoneHigh        = 0;
        int PurchaseAmount  = 0;
        int ProvisionalBit  = FALSE;

        /*  Clears Actionlist details (if any) */
        memset( &pData->ActionList.actionlist.productSale, 0, sizeof( ProductSale_t ) );

        /*  1.  Perform a ProductSale transaction for the provisional product:
            a.  Set the product ID(1) to n-Hour. */
        ProductId           = myki_br_cd_GetProductId( PRODUCT_TYPE_NHOUR );

        /*  b.  Set the provisional zone(4,5) according to the current route/stop
            c.  Set the vehicle/route/stop to the currently known vehicle/route/stop
            d.  If the provisional low zone(10) minus 1 is marked as City Saver(12)
                set the product low zone(6) to the provisional low zone(10) -1,
                else set the product low zone(6) to the provisional low zone(10). 
            e.  If the provisional high zone(11) is marked as City Saver(12)
                set the product high zone(7) to the provisional high zone(11) plus 1,
                else set the product high zone(7) to the provisional high zone(11). */
        ZoneLow             = pData->DynamicData.provisionalZoneLow;
        ZoneHigh            = pData->DynamicData.provisionalZoneHigh;
        ADJUST_FOR_CITYSAVER( ZoneLow, ZoneHigh );

        /*  f.  Set the provisional bit of the product control bitmap(3) to 1. */
        ProvisionalBit      = TRUE;

        /*  g.  Set the product purchase value(2) to 0.0 */
        PurchaseAmount      = 0;

        /*  h.  Start date time to the current datetime(7) (Do not round seconds)
            i.  End date time to the determined hour duration(20) */
        DirIndex    = myki_br_ldt_ProductSale( pData, ProductId, ZoneLow, ZoneHigh, PurchaseAmount, ProvisionalBit );
        if ( DirIndex < 0 )
        {
            CsErrx( "BR_LLSC_5_9 : myki_br_ldt_ProductSale() failed" );
            return RULE_RESULT_ERROR;
        }
        pDirectory  = &pMYKI_TAControl->Directory[ DirIndex ];

        /*  2.  Perform a ProductUpdate/Activate transaction forthe provisional product.  */
        if ( myki_br_ldt_ProductUpdate_Activate( pData, pDirectory ) < 0 )
        {
            CsErrx( "BR_LLSC_5_9 : myki_br_ldt_ProductUpdate_Activate() failed" );
            return RULE_RESULT_ERROR;
        }

        /*  3.  Perform a ProductUsage/ScanOnStoredValue transaction for the provisional product.  */
        if ( myki_br_ldt_ProductUsage_ScanOnStoredValue( pData, pDirectory ) < 0 )
        {
            CsErrx( "BR_LLSC_5_9 : myki_br_ldt_ProductUsage_ScanOnStoredValue() failed" );
            return RULE_RESULT_ERROR;
        }

        /*  4.  Create usage log */
        pTAppUsageLog                                   = &pData->InternalData.UsageLogData;

        /*  a.  Definition: */
        {
            /*  i.  TxType = As per Sequence Type (Entry Exit = 7, Exit Only = 18; Entry Only = 19, On board = 20), */
            pTAppUsageLog->transactionType              = myki_br_getTransactionType( pData );

            /*  ii. Determine uselog ControlBitmap though look up of the TxUsageType (Done by framework) */
            /*  iii.ProviderID = ServiceProviderID(17) (Done by framework) */
            /*  iv. TxDateTime = Current Date time(7) (Done by framework) */
            /*  v.  Location.EntryPoint = EntryPointID(18) (Done by framework) */
            /*  vi. Location.Route = LIne(19) (Done by framework) */
            /*  vii.Location.Stop = Stopid(9) (Done by framework) */
        }
        /*  b.  Value: not defined */
        /*  c.  Usage: */
        {
            /*  i.  Zone = Dynamic.Zone(8) */
            pTAppUsageLog->isZoneSet                    = TRUE;
            pTAppUsageLog->zone                         = pData->DynamicData.currentZone;

            /*  ii. ProductValidationStatus: 
                    (1) Set bit 2 Scan-On true,
                    (2) Set bit 4 Provisional true */
            pTAppUsageLog->isProductValidationStatusSet = TRUE;
            pTAppUsageLog->productValidationStatus     |= ( TAPP_USAGE_LOG_PROD_VAL_STATUS_PROVISIONAL_BITMAP |
                                                            TAPP_USAGE_LOG_PROD_VAL_STATUS_SCAN_ON_BITMAP     );
        }
        /*  d.  Product: */
        {
            /*  i.  ProductIssuerId = As returned from ProductSale transaction at step [7]1 */
            pTAppUsageLog->isProductIssuerIdSet         = TRUE;
            pTAppUsageLog->productIssuerId              = pDirectory->IssuerId;

            /*  ii. ProductSerialNo = As returned from ProductSale transaction at step [7]1 */
            pTAppUsageLog->isProductSerialNoSet         = TRUE;
            pTAppUsageLog->productSerialNo              = pDirectory->SerialNo;

            /*  [5]iii.ProductId = As returned from ProductSale transaction at step [7]1 */
            pTAppUsageLog->isProductIdSet               = TRUE;
            pTAppUsageLog->productId                    = pDirectory->ProductId;
        }

        /*  NOTE:   TAppUsageLog entry is added by application framework. */
        pData->InternalData.IsUsageLogUpdated           = TRUE;
        CsDbg( BRLL_RULE, "BR_LLSC_5_9 : Updated TAppUsageLog" );
    }

    CsDbg( BRLL_RULE, "BR_LLSC_5_9 : Executed");
    return RULE_RESULT_EXECUTED;
}   /*  BR_LLSC_5_9( ) */
