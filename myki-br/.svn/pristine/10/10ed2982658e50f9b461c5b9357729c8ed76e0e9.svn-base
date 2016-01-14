/**************************************************************************
*   ID :  BR_LLSC_5_7
*    Scan-On Extend Provisional Product
*    1.  TAppControl.ProductInUse
*    2.  TAppControl.ProvisionalFare
*    3.  TAppControl.PassengerCode
*    4.  TAppTProduct.ControlBitmap
*    5.  TAppTProduct.ZoneLow
*    6.  TAppTProduct.ZoneHigh
*    7.  TAppTProduct.LastUsageZone
*    8.  Dynamic.CurrentZone
*    9.  Dynamic.CurrentDateTime
*    10.  Dynamic.ProvisionalZoneLow
*    11.  Dynamic.ProvisionalZoneHigh
*    12.  Tariff.CitySaverZoneFlag
*    13.  Tariff.ConductorLines.TripDirection
*    14.  Static.ServiceProviderID
*    15.  Dynamic.EntryPointID
*    16.  Dynamic.LineID
*    17.  TApp.TProduct.Location.EntryPointId
*    18.  TApp.TProduct.Location.RouteID
*    19.  TApp.TProduct.Location.StopID
*    20.  Tariff.Location
*    21.  Tariff.Location inner border
*    22.  Tariff.Location.zone
*    23.  Tariff.Location.outer border
*    24. Dynamic.TripDirection
*
*   Pre-Conditions
*    1.  The product in use field1 is not 0.
*    2.  The provisional bit of the product control bitmap4 of the product in use1 is 1.
*
*   Description
*    1.  If Tariff.ConductorLines13 is set then use specified trip direction24
*        a.  Convert the last usage location of the provisional location17 in to its inner  border21, actual zone22 and outer border22 components
*    2.  Else
*        a.  For Vehical (route/stop) based devices determine current trip direction
*            i.   If the zone22 of the first stop (stop 1)  on this current route16 is greater than the zone22 of the last stop on the current route16 then  direction is
*                 outbound
*            ii.  Else if  the zone22 of the first stop (stop 1) on this current route16 is less than the zone of the last stop on the current route16 trip direction is
*                 inbound
*            iii. Else the trip direction is undefined
*            iv.  Convert the  provisional products1 last usage route18 and stop19 in to its inner border21 actual zone22 and outer  border23
*        b.  For Rail (station line) based devices determine trip direction
*            i.  If the provisional product's1 last usage zone22 is greater than the zone22 of the current location16 then the trip direction is inbound
*            ii.  if  the  provisional product's1 last usage zone22 is less than the zone22 of the current location16 then the trip direction is outbound Else the trip
*                direction is undefined.
*            iii.  Convert the the provisional product1 last usage location17 in to its inner border21 actual zone22 and outer border23
*    3.  If a product exsitst that is of type e-Pass that is active, or a product of type e-Pass exists which has status of inactive where there is no product of type
*        e-pass that is active then
*    a.  Scenario board within e-Ppass zone & intercepted within inside e-Pass zone:  If the e-pass zone low and e-pass zone high includes either: the low
*    border zone, or actual zone, or high zone border of the provisional product last location and the e-pass zone low and e-pass zone high includes
*    either: the low border zone, or actual border zone, or high zone border of the current location then
*    i.  Perform a Product Update/Extend transaction for the provisional product:
*    ii.  Set the provisional fare2 to 0.
*    iii.  If the trip direction is inbound
*    (a)  Set the product low zone5 to the low zone of the e-pass, If the resulting product low zone5 minus 1 is marked as City Saver12,
*    decrement the resulting product low zone5 by 1.
*    (b)  Set the provisional product high zone to the lessor of the boarding location zone or boarding location low border zone; if the
*    resulting product low zone5 minus 1 is marked as City Saver12, decrement the resulting product low zone5 by 1.
*
*    iv.  Else If the trip direction is outbound
*    (a)  Set the product high zone6 to the high zone of of the e-pass.  If the resulting product high zone6 is marked as City Saver12,
*    increment the resulting product high zone6 by 1.
*    (b)  Set the provisional product low zone to the greater of the boarding location zone or boarding location high border zone; if the
*    resulting product low zone5 minus 1 is marked as City Saver12, decrement the resulting product low zone5 by 1.
*
*    v.  Else If the trip direction is not specified
*    (a)  Set the product low zone5 to the zone of origin10 or  the epass zone low, whichever is lower. If the resulting product low zone5
*    minus 1 is marked as City Saver12, decrement the resulting product low zone5 by 1.
*    (b)  Set the product high zone6 to the zone of origin10 or the e-Pass zone high, whichever is higher. If the resulting product high zone6
*    is marked as City Saver12, increment the resulting product high zone6 by 1.
*
*
*    b.  Scenario board within e-Pass zone & Intercepted outside e-Pass zone:  If the e-pass zone low and e-pass zone high includes either  the low
*    border zone, or actual border zone, or high zone border of the provisional product location last used and the e-pass zone low and e-pass zone high
*    includes neither: the low border zone, or actual border zone, or high zone border of the current location. then
*    i.  Perform a Product Update/Extend transaction for the provisional product:
*    ii.  If the trip direction is inbound
*    (a)  Set the product low zone5 to the decleard destination zone, If the resulting product low zone5 minus 1 is marked as City Saver12,
*    decrement the resulting product low zone5 by 1.
*
*    (b)  Set the provisionsal product high zone to the lesser  of bording location zone or low  border zone of the boarding location; If the
*    resulting product high zone5 is marked as City Saver12, increment the resulting product low zone5 by 1.
*    (c)  Set the provisional fare2 to the fare from the zone low, to zone high for the current date
*
*    iii.  Else if the trip direction is outbound
*    (a)  Set the product high zone to the decleard destination zone, If the resulting product high zone5 is marked as City Saver12, increment
*    the resulting product low zone5 by 1.
*    (b)  Set the provisionsal product low zone to the greater of bording location zone or high Border zone of the boarding location If the
*    resulting product low one5 minus 1 is marked as City Saver12, decrement the resulting product low zone5 by 1.
*    (c)  Set the provisional fare2 to the fare from the zone low, to zone high for the current date
*
*    iv.  Else If the trip direction is not specified
*    (a)  Set the product low zone5 to the zone of origin10 or declared destination zone, whichever is lower. If the resulting product low zone5
*    minus 1 is marked as City Saver12, decrement the resulting product low zone5 by 1.
*    (b)  Set the product high zone6 to the zone of origin10 or declared destination zone, whichever is higher. If the resulting product high zone6
*    is marked as City Saver12, increment the resulting product high zone6 by 1.
*    (c)  Set the provisional fare to the fare from the scan on location to the decleared zone
*
*    c.  Scenario: Scan on outside e-Pass zone range & intercepted inside e-Pass zone:   If the e-pass zone low and e-pass zone high includes neither of:
*    the low border zone, or actual border zone, or high zone border of the provisional product scan on location and further the e-pass zone low and e-
*    pass zone high is within: the low border zone, or actual border zone, or high zone border of the current location then
*    i.  Perform a Product Update/Extend transaction for the provisional product:
*    (1)  If the trip is inbound
*    (a)  Set the product low zone5 to the low zone of the epass, If the resulting product low zone5 minus 1 is marked as City Saver12,
*    decrement the resulting product low zone5 by 1.
*    (b)  Set the provisionsal product high zone to the lesser  of boarding location zone or low borader zone; If the resulting product high zone5
*    is marked as City Saver12, increment the resulting product low zone5 by 1.
*    (c)  Set the provisional fare2 to the fare from the zone low, to zone high for the current date
*
*    (2)  Else if the trip direction is outbound
*    (a)  Set the product high zone5 to the high zone of the epass, If the resulting product high zone5 is marked as City Saver12, increment the
*    resulting product low zone5 by 1.
*    (b)  Set the provisionsal product low zone to the greater  of bording location zone or low border zone; If the resulting product low zone5
*    minus 1 is marked as City Saver12, decrement the resulting product low zone5 by 1.
*    (c)  Set the provisional fare2 to the fare from the zone low, to zone high for the current date
*
*    ii.  Else If the trip direction is not specified
*    (a)  Set the provisional product low zone5 to the zone of origin10 or declared destination zone, whichever is lower. If the resulting product
*    low zone5 minus 1 is marked as City Saver12, decrement the resulting produsct low zone5 by 1.
*    (b)  Set the product high zone6 to the zone of origin10 or declared destination zone, whichever is higher. If the resulting product high zone6
*    is marked as City Saver12, increment the resulting product high zone6 by 1.
*    (c)  Set the provisional fare2 to the fare from the zone low, to zone high for the current date
*
*
*    4.  If there is:
*    a.  No e-pass present or
*    b.  Scenario: scan on outside e-pass zone and intercepted outside e-pass Zone.  If the e-pass zone low and e-pass zone high includes neither : the
*    low border zone, or actual border zone, or high zone border of the provisional product scan on location and further the e-pass zone low and e-pass
*    zone high is not within either: the low border zone, or actual border zone, or high zone border of the current location; then
*    c.  Perform a Product Update/Extend transaction for the provisional product:
*    i.  if the trip direction is inbound
*    (a)  Set the product low zone5 to the decleard destination zone, If the resulting product low zone5 minus 1 is marked as City Saver12,
*    decrement the resulting product low zone5 by 1.
*    (b)  Set the provisionsal product high zone to the lesser of bording location zone or low  border zone; If the resulting product high zone5 is
*    marked as City Saver12, increment the resulting product low zone5 by 1.
*    (c)  Set the provisional fare2 to the fare from the zone low, to zone high for the current date
*
*    ii.  Else if the trip direction is outbound
*    (a)  Set the product high zone5 to the decleard destination zone, If the resulting product high zone5 is marked as City Saver12, increment
*    the resulting product low zone5 by 1.
*    (b)  Set the provisionsal product low zone to the greater of bording location zone or high border zone; If the resulting product low zone5
*    minus 1 is marked as City Saver12, decrement the resulting product low zone5 by 1.
*    (c)  Set the provisional fare2 to the fare from the zone low, to zone high for the current date
*
*
*    iii.  Else If the trip direction is not specified
*    (a)  Set the product low zone5 to the zone of origin10 or declared destination zone, whichever is lower. If the resulting product low zone5
*    minus 1 is marked as City Saver12, decrement the resulting product low zone5 by 1.
*    (b)  Set the product high zone6 to the zone of origin10 or declared destination zone, whichever is higher. If the resulting product high zone6
*    is marked as City Saver12, increment the resulting product high zone6 by 1.
*    (c)  Set the provisional fare2 to the fare from the zone low, to zone high for the current date
*
*
*
*
*    5.  Create usage log
*    a.  Definition:
*    i.  Type (On board= 20),
*    ii.  ProviderID =  ServiceProviderID14
*    iii.  TxDateTime = Current Date time9
*    iv.  Location.EntryPoint = EntryPointID16
*    v.  Location.Route=  LineID17
*    vi.  Location.Stop= Stopid11
*    b.  Value:  not defined
*    c.  Usage:
*    i.  Zone = Dynamic.Zone8
*    ii.  ProductValidationStatus: Scan-On, Provisional
*    d.  Product:
*    i.  ProductIssuerId  = As returned Product Update/Extend
*    ii.  ProductSerialNo =  As returned Product Update/Extend
*    iii.  ProductId = As returned from Product Update/Extend
*      Post-Conditions
*    1.  The provisional product is amended to include the declared zone range of travel.
*    2.  The provisional fare is amended to the actual fare for the declared zone range of travel.
*      Devices
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
//  Get the zone location from given lineId and stopId
//
//  Returns:
//      zone location
//
//=============================================================================

static U16_t getZoneOfStop( int lineId, int stopId )
{
    CsErrx( "BR_LLSC_5_7 : getZoneOfStop() not implemented." );
    return 0;
}

//=============================================================================
//
//  Get the fare between zone low and zone high
//
//  Returns:
//      fare
//
//=============================================================================

static U16_t getZoneFare( int zoneLow, int zoneHigh )
{
    CsErrx( "BR_LLSC_5_7 : getZoneFare() not implemented." );
    return 0;
}

//=============================================================================
//
//  Get a pointer to MYKI_TAProduct_t which is the only one active product of the card,
//  or an inactive product if all the products are inactive. or NULL if not only one active
//  product was found on the card.
//
//  Input:
//      pMYKI_TAControl    pointer to MYKI_TAControl_t
//
//  Output:
//      *ppEpassProduct     a product pointer to MYKI_TAProduct_t
//
//  Returns:
//      zeor              Success
//      none zeor      Error
//
//=============================================================================

static U8_t getEpassProduct(MYKI_TAControl_t    *pMYKI_TAControl, MYKI_TAProduct_t    **ppEpassProduct)
{
    MYKI_Directory_t    *pDirInUse = NULL;
    MYKI_Directory_t    *pDirActive = NULL;
    MYKI_TAProduct_t    *pPdtActive = NULL;
    MYKI_Directory_t    *pDirInactive = NULL;
    MYKI_TAProduct_t    *pPdtInactive = NULL;

    U8_t                 active = 0;
    U8_t                 dir;

    if (!pMYKI_TAControl || !ppEpassProduct)
        return 1;

    //asume no product found
    *ppEpassProduct = NULL;

    for ( dir = 1; dir < DIMOF( pMYKI_TAControl->Directory ); dir++ ) // T-Purse dir entry does not have a product slot, so there's one less product slots than dir entries
    {
        MYKI_Directory_t    *pDirectory = &pMYKI_TAControl->Directory[ dir ]; // Product slot 0-4 maps to dir entry 1-5
        MYKI_TAProduct_t    *pstProduct;

        if (MYKI_CS_TAProductGet( (U8_t)( dir - 1 ), &pstProduct ) >= 0)
        {
            if (pDirectory->Status == TAPP_CONTROL_DIRECTORY_STATUS_ACTIVATED)
            {
                active++;
                pDirActive = pDirectory;
                pPdtActive = pstProduct;
            }
            else
            {
                pDirInactive = pDirectory;
                pPdtInactive = pstProduct;
            }
        }
    }

    if (active > 1)
    {
        CsDbg( BRLL_RULE, "getEpassProduct() : %d active products found.");
        return 0;
    }
    else if (active == 1)
    {
        pDirInUse = pDirActive;
        CsDbg( BRLL_RULE, "getEpassProduct() : Selected active product %d.", pDirInUse->ProductId);

        //select this active product
        *ppEpassProduct = pPdtActive;
    }
    else //inactive products
    {
        if (pPdtInactive == NULL)
        {
            CsDbg( BRLL_RULE, "getEpassProduct() : No suitable inactive products found.");
            return RULE_RESULT_BYPASSED;
        }
        else
        {
            pDirInUse = pDirInactive;
            CsDbg( BRLL_RULE, "BR_LLSC_5_5 : Selected inactive product %d for scan on.", pDirInUse->ProductId);

            //select this inactive product
            *ppEpassProduct = pPdtInactive;
        }
    }

    return 0;
}



//=============================================================================
//
//
//
//=============================================================================

static int  productUpdateExtend( MYKI_BR_ContextData_t *pData, MYKI_Directory_t *pDirectory, ProductUpdate_t *pRequest)
{
    CsErrx( "productUpdateExtend() This has not yet been implemented properly!" );

    memset(pRequest, 0, sizeof( ProductUpdate_t ) );

    pRequest->serialNo            = pDirectory->SerialNo;
    pRequest->serviceProviderId   = pData->StaticData.serviceProviderId;
    pRequest->entryPointId        = pData->DynamicData.entryPointId;
    pRequest->isEntryPointIdSet   = TRUE;
    pRequest->routeId             = pData->DynamicData.lineId;
    pRequest->stopId              = pData->DynamicData.stopId;


#ifdef LDT_SUPPORTED

    if ( MYKI_LDT_ProductUpdateExtend( pRequest, &pData->InternalData.TransactionData ) != LDT_SUCCESS )
    {
        CsErrx( "productUpdateExtend() MYKI_LDT_ProductUpdateExtend() failed" );
        return -1;
    }

#endif

    return 0;
}

//=============================================================================
//
//
//
//=============================================================================

RuleResult_e BR_LLSC_5_7( MYKI_BR_ContextData_t *pData )
{
    MYKI_TAControl_t    *pMYKI_TAControl;
    MYKI_Directory_t    *pDirInUse = NULL;
    MYKI_TAProduct_t    *pPdtInUse = NULL;
    ProductUpdate_t      productUpdateReq;
    int                  ret = LDT_SUCCESS;

    MYKI_TAProduct_t    *pEpassProduct = NULL;

/*
    Data Fields
    1.  TAppControl.ProductInUse                    pMYKI_TAControl->ProductInUse
    2.  TAppControl.ProvisionalFare                 pMYKI_TAControl->ProvisionalFare
    3.  TAppControl.PassengerCode                   pMYKI_TAControl->PassengerCode
    4.  TAppTProduct.ControlBitmap                  pPdtInUse->ControlBitmap
    5.  TAppTProduct.ZoneLow                        pPdtInUse->ZoneLow
    6.  TAppTProduct.ZoneHigh                       pPdtInUse->ZoneHigh
    7.  TAppTProduct.LastUsageZone                  pPdtInUse->LastUsage.Zone
    8.  Dynamic.CurrentZone                         pData->DynamicData.currentZone
    9.  Dynamic.CurrentDateTime                     pData->DynamicData.currentDateTime
    10. Dynamic.ProvisionalZoneLow                  pData->DynamicData.provisionalZoneLow
    11. Dynamic.ProvisionalZoneHigh                 pData->DynamicData.provisionalZoneHigh
    12. Tariff.CitySaverZoneFlag                    pData->Tariff.citySaverZoneFlag
    13. Tariff.ConductorLines.TripDirection         pData->Tariff.conductorLinesTripDirection
    14. Static.ServiceProviderID                    pData->StaticData.serviceProviderId
    15. Dynamic.EntryPointID                        pData->DynamicData.entryPointId
    16. Dynamic.LineID                              pData->DynamicData.lineId
    17. TApp.TProduct.Location.EntryPointId         pPdtInUse->LastUsage.Location.EntryPointId
    18. TApp.TProduct.Location.RouteID              pPdtInUse->LastUsage.Location.RouteId
    19. TApp.TProduct.Location.StopID               pPdtInUse->LastUsage.Location.StopId
    20. Tarrif.Location                             pData->Tariff.location
    21. Tariff.Location inner border                pData->Tariff.locationInnerBorder
    22. Tariff.Location.zone                        pData->Tariff.locationZone
    23. Tariff.Location.outer border                pData->Tariff.locationOuterBorder
*/

    CsDbg( BRLL_RULE, "BR_LLSC_5_7 : Start (Scan-On Extend Provisional Product)" );

    if ( ! pData )
    {
        CsErrx( "BR_LLSC_5_7 : Invalid argument(s)" );
        return RULE_RESULT_ERROR;
    }

    if ( MYKI_CS_TAControlGet(&pMYKI_TAControl) )
    {
        CsErrx( "BR_LLSC_5_7 : MYKI_CS_TAControlGet() failed" );
        return RULE_RESULT_ERROR;
    }

    // Pre-Conditions
    if ( pMYKI_TAControl->ProductInUse <= 0 )
    {
        CsDbg( BRLL_RULE, "BR_LLSC_5_7 : BYPASSED : ProductInUse is %d", pMYKI_TAControl->ProductInUse );
        return RULE_RESULT_BYPASSED;
    }

    if ( MYKI_CS_TAProductGet( (U8_t)( pMYKI_TAControl->ProductInUse - 1 ), &pPdtInUse ) < 0 )
    {
        CsErrx( "BR_LLSC_5_7 : MYKI_CS_TAProductGet() failed" );
        return RULE_RESULT_ERROR;
    }

    pDirInUse = &pMYKI_TAControl->Directory[ pMYKI_TAControl->ProductInUse ];

    if (! (pPdtInUse->ControlBitmap & TAPP_TPRODUCT_CONTROL_PROVISIONAL_BITMAP) )
    {
        CsDbg( BRLL_RULE, "BR_LLSC_5_7 : BYPASSED : TAPP_TPRODUCT_CONTROL_PROVISIONAL_BITMAP not set");
        return RULE_RESULT_BYPASSED;
    }

    // Description

    // 1.  If Tariff.ConductorLines13 is set then use specified trip direction
    if ( pData->Tariff.conductorLinesTripDirection == TRIP_DIRECTION_INBOUND || pData->Tariff.conductorLinesTripDirection == TRIP_DIRECTION_OUTBOUND )
    {
        //  KWS: Not yet implemented!
        CsDbg( BRLL_RULE, "BR_LLSC_5_7 : TODO : Set below values based on pPdtInUse->LastUsage.Location.EntryPointId and TRIP_DIRECTION");

        // a.  Convert the last usage location of the provisional location17 in to its inner  border21, actual zone22 and outer border22 components

        //set Tariff.location from TRIP_DIRECTION and LastUsage.Location.EntryPointId

        //pData->Tariff.locationInnerBorder = ?
        //pData->Tariff.locationZone = ?
        //pData->Tariff.locationOuterBorder =?
    }
    else // 2.  Else
    {
        // a.  For Vehical (route/stop) based devices determine current trip direction
        if ( pData->InternalData.TransportMode == TRANSPORT_MODE_BUS )
        {
            // i.  If the zone22 of the first stop (stop 1)  on this current route16 is greater than the zone22 of the last stop on the current route16 then  direction is outbound
            U16_t firstStopZone = getZoneOfStop( pData->DynamicData.lineId, 0 );
            U16_t lastStopZone = getZoneOfStop( pData->DynamicData.lineId, -1 );

            if ( firstStopZone > lastStopZone )
            {
                pData->Tariff.conductorLinesTripDirection = TRIP_DIRECTION_OUTBOUND;
            }
            // ii. Else if  the zone22 of the first stop (stop 1) on this current route16 is less than the zone of the last stop on the current route16trip direction is inbound
            else if ( firstStopZone < lastStopZone )
            {
                pData->Tariff.conductorLinesTripDirection = TRIP_DIRECTION_INBOUND;
            }
            // iii.    Else the trip direction is undefined
            else
            {
                pData->Tariff.conductorLinesTripDirection = TRIP_DIRECTION_UNKNOWN;
            }

            // iv. Convert the  provisional products1 last usage route18 and stop19 in to its inner border21 actual zone22 and outer  border23

            //set Tariff.location from TRIP_DIRECTION and LastUsage.Location.StopId, RouteID

            //pData->Tariff.locationInnerBorder = ?
            //pData->Tariff.locationZone = ?
            //pData->Tariff.locationOuterBorder =?

            CsDbg( BRLL_RULE, "BR_LLSC_5_7 : TODO : Set above values base on pPdtInUse->LastUsage.Location.StopId, RouteID");
        }
        // b.  For Rail (station line) based devices determine trip direction
        else
        {
            // i.  If the provisional product's1 last usage zone22 is greater than the zone22 of the current location16 then the trip direction is inbound
            if ( pData->Tariff.locationZone > pData->DynamicData.lineId)
            {
                pData->Tariff.conductorLinesTripDirection = TRIP_DIRECTION_INBOUND;
            }

            // ii. if  the  provisional product's1 last usage zone22 is less than the zone22 of the current location16 then the trip direction is outboundElse the trip direction is undefined.
            if ( pData->Tariff.locationZone < pData->DynamicData.lineId)
            {
                pData->Tariff.conductorLinesTripDirection = TRIP_DIRECTION_OUTBOUND;
            }
            else
            {
                pData->Tariff.conductorLinesTripDirection = TRIP_DIRECTION_UNKNOWN;
            }

            // iii.    Convert the the provisional product1 last usage location17 in to its inner border21 actual zone22 and outer border23

            //pData->Tariff.locationInnerBorder = ?
            //pData->Tariff.locationZone = ?
            //pData->Tariff.locationOuterBorder =?
            CsDbg( BRLL_RULE, "BR_LLSC_5_7 : TODO : Set above values base on pPdtInUse->LastUsage.Location.EntryPointId and TRIP_DIRECTION");
        }
    }


    // 3.   If a product exsitst that is of type e-Pass that is active ¨C
    //       then use this product as the active zones range, or a product of type e-Pass exists
    //       which has status of inactive where there is no product of type e-pass that is active
    //       then use this products zone range for the following:
    ret = getEpassProduct(pMYKI_TAControl, &pEpassProduct);

    CsDbg( BRLL_RULE, "BR_LLSC_5_7 : TripDirection = %d", pData->Tariff.conductorLinesTripDirection);
    CsDbg( BRLL_RULE, "BR_LLSC_5_7 : Tariff.locationInnerBorder = %d", pData->Tariff.locationInnerBorder);
    CsDbg( BRLL_RULE, "BR_LLSC_5_7 : Tariff.locationOuterBorder = %d", pData->Tariff.locationOuterBorder);
    CsDbg( BRLL_RULE, "BR_LLSC_5_7 : Tariff.locationZone = %d", pData->Tariff.locationZone);
    CsDbg( BRLL_RULE, "BR_LLSC_5_7 : DynamicData.provisionalZoneLow = %d", pData->DynamicData.provisionalZoneLow);
    CsDbg( BRLL_RULE, "BR_LLSC_5_7 : DynamicData.provisionalZoneHigh = %d", pData->DynamicData.provisionalZoneHigh);
    CsDbg( BRLL_RULE, "BR_LLSC_5_7 : DynamicData.currentZone = %d", pData->DynamicData.currentZone);
    if (pEpassProduct)
    {
        CsDbg( BRLL_RULE, "BR_LLSC_5_7 : pEpassProduct->ZoneLow = %d", pEpassProduct->ZoneLow);
        CsDbg( BRLL_RULE, "BR_LLSC_5_7 : pEpassProduct->ZoneHigh = %d", pEpassProduct->ZoneHigh);
    }

    if ( ret == 0 && pEpassProduct != NULL )
    {
        // a.   Scenario board within e-Ppass zone & intercepted within inside e-Pass zone:

        //     If the e-pass zone low5 and e-pass zone high6 includes either: the low border zone21,
        //     or actual zone22, or high zone border23 of the provisional product last location
        //     and the e-pass zone low5 and e-pass zone high6 includes either: the low border zone21,
        //     or actual border zone22, or high zone border23 of the current location20 then
        if ( (pEpassProduct->ZoneLow <= pData->Tariff.locationInnerBorder ||
               pEpassProduct->ZoneHigh >= pData->Tariff.locationOuterBorder ||
               pEpassProduct->ZoneLow <= pData->Tariff.locationZone ||
               pEpassProduct->ZoneHigh >= pData->Tariff.locationZone) &&
               (pEpassProduct->ZoneLow <= pData->Tariff.location ||
                pEpassProduct->ZoneHigh >= pData->Tariff.location) )
        {
            CsDbg( BRLL_RULE, "BR_LLSC_5_7 : Scenario a");

            // i.   Perform a Product Update/Extend transaction for the provisional product:
            productUpdateExtend(pData, pDirInUse, &productUpdateReq);

            // ii.  Set the provisional fare2 to 0.
            pMYKI_TAControl->ProvisionalFare = 0;

            // iii. If the trip direction is inbound
            // (1)  Set the product low zone5 to the low zone of the e-pass, If the resulting product low zone5 minus 1 is marked as City Saver12, decrement the resulting product low zone5 by 1.
            // (2)  Set the provisional product high zone to the lessor of the boarding location zone or boarding location low border zone; if the resulting product low zone5 minus 1 is marked as City Saver12, decrement the resulting product low zone5 by 1.
            if ( pData->Tariff.conductorLinesTripDirection == TRIP_DIRECTION_INBOUND )
            {
                pPdtInUse->ZoneLow = pEpassProduct->ZoneLow;
                pPdtInUse->ZoneHigh = (pData->Tariff.location < pData->Tariff.locationInnerBorder)? pData->Tariff.location : pData->Tariff.locationInnerBorder;

                ADJUST_FOR_CITYSAVER( pPdtInUse->ZoneLow, pPdtInUse->ZoneHigh );
            }
            // iv.  Else If the trip direction is outbound
            // (1)  Set the product high zone6 to the high zone of of the e-pass.  If the resulting product high zone6 is marked as City Saver12, increment the resulting product high zone6 by 1.
            // (2)  Set the provisional product low zone to the greater of the boarding location zone or boarding location high border zone; if the resulting product low zone5 minus 1 is marked as City Saver12, decrement the resulting product low zone5 by 1.
            else if ( pData->Tariff.conductorLinesTripDirection == TRIP_DIRECTION_OUTBOUND)
            {
                pPdtInUse->ZoneLow = (pData->Tariff.location > pData->Tariff.locationOuterBorder)? pData->Tariff.location : pData->Tariff.locationOuterBorder;
                pPdtInUse->ZoneHigh = pEpassProduct->ZoneHigh;

                ADJUST_FOR_CITYSAVER( pPdtInUse->ZoneLow, pPdtInUse->ZoneHigh );
            }
            // v.   Else If the trip direction is not specified
            // (1)  Set the product low zone5 to the zone of origin10 or  the epass zone low, whichever is lower. If the resulting product low zone5 minus 1 is marked as City Saver12, decrement the resulting product low zone5 by 1.
            // (2)  Set the product high zone6 to the zone of origin10 or the e-Pass zone high, whichever is higher. If the resulting product high zone6 is marked as City Saver12, increment the resulting product high zone6 by 1.
            else
            {
                pPdtInUse->ZoneLow = (pData->DynamicData.provisionalZoneLow < pEpassProduct->ZoneLow) ? pData->DynamicData.provisionalZoneLow : pEpassProduct->ZoneLow;
                pPdtInUse->ZoneHigh = (pData->DynamicData.provisionalZoneHigh > pEpassProduct->ZoneHigh) ? pData->DynamicData.provisionalZoneHigh : pEpassProduct->ZoneHigh;

                ADJUST_FOR_CITYSAVER( pPdtInUse->ZoneLow, pPdtInUse->ZoneHigh );
            }
        }


        // b.   Scenario board within e-Pass zone & Intercepted outside e-Pass zone:

        //    If the e-pass zone low5 and e-pass zone high6 includes either the low border zone21,
        //    or actual zone22, or high zone border23 of the provisional product location last used
        //    and the e-pass zone low5 and e-pass zone high6 includes neither: the low border zone21,
        //    or actual  zone22, or high zone border23 of the current location20. then
        if ( (pEpassProduct->ZoneLow <= pData->Tariff.locationInnerBorder ||
               pEpassProduct->ZoneHigh >= pData->Tariff.locationOuterBorder ||
               pEpassProduct->ZoneLow <= pData->Tariff.locationZone ||
               pEpassProduct->ZoneHigh >= pData->Tariff.locationZone) &&
               (pEpassProduct->ZoneLow > pData->Tariff.location ||
                pEpassProduct->ZoneHigh < pData->Tariff.location) )
        {
            CsDbg( BRLL_RULE, "BR_LLSC_5_7 : Scenario b");

            // i.   Perform a Product Update/Extend transaction for the provisional product:
            productUpdateExtend(pData, pDirInUse, &productUpdateReq);

            // ii.  If the trip direction is inbound
            // (1)  Set the product low zone5 to the decleard destination zone, If the resulting product low zone5 minus 1 is marked as City Saver12, decrement the resulting product low zone5 by 1.
            // (2)  Set the provisionsal product high zone to the lesser of bording location zone or low  border zone of the boarding location; If the resulting product high zone5 is marked as City Saver12, increment the resulting product low zone5 by 1.
            // (3)  Set the provisional fare2 to the fare from the zone low, to zone high for the current date
            if ( pData->Tariff.conductorLinesTripDirection == TRIP_DIRECTION_INBOUND )
            {
                pPdtInUse->ZoneLow = pData->Tariff.locationZone;
                pPdtInUse->ZoneHigh = (pData->Tariff.location < pData->Tariff.locationInnerBorder)? pData->Tariff.location : pData->Tariff.locationInnerBorder;

                ADJUST_FOR_CITYSAVER( pPdtInUse->ZoneLow, pPdtInUse->ZoneHigh );

                pMYKI_TAControl->ProvisionalFare = getZoneFare( pPdtInUse->ZoneLow, pPdtInUse->ZoneHigh );
            }
            // iii. Else if the trip direction is outbound
            // (1)  Set the product high zone to the decleard destination zone, If the resulting product high zone5 is marked as City Saver12, increment the resulting product low zone5 by 1.
            // (2)  Set the provisionsal product low zone to the greater of bording location zone or high Border zone of the boarding location. If the resulting product low one5 minus 1 is marked as City Saver12, decrement the resulting product low zone5 by 1.
            // (3)  Set the provisional fare2 to the fare from the zone low, to zone high for the current date
            else if ( pData->Tariff.conductorLinesTripDirection == TRIP_DIRECTION_OUTBOUND)
            {
                pPdtInUse->ZoneLow = (pData->Tariff.location > pData->Tariff.locationOuterBorder)? pData->Tariff.location : pData->Tariff.locationOuterBorder;
                pPdtInUse->ZoneHigh = pData->Tariff.locationZone;

                ADJUST_FOR_CITYSAVER( pPdtInUse->ZoneLow, pPdtInUse->ZoneHigh );

                pMYKI_TAControl->ProvisionalFare = getZoneFare( pPdtInUse->ZoneLow, pPdtInUse->ZoneHigh );
            }
            // iv.  Else If the trip direction is not specified
            // (1)  Set the product low zone5 to the zone of origin10 or declared destination zone, whichever is lower. If the resulting product low zone5 minus 1 is marked as City Saver12, decrement the resulting product low zone5 by 1.
            // (2)  Set the product high zone6 to the zone of origin10 or declared destination zone, whichever is higher. If the resulting product high zone6 is marked as City Saver12, increment the resulting product high zone6 by 1.
            // (3)  Set the provisional fare to the fare from the scan on location to the decleared zone
            else
            {
                pPdtInUse->ZoneLow = (pData->DynamicData.provisionalZoneLow < pData->Tariff.locationZone) ? pData->DynamicData.provisionalZoneLow : pData->Tariff.locationZone;
                pPdtInUse->ZoneHigh = (pData->DynamicData.provisionalZoneHigh > pEpassProduct->ZoneHigh) ? pData->DynamicData.provisionalZoneHigh : pData->Tariff.locationZone;

                ADJUST_FOR_CITYSAVER( pPdtInUse->ZoneLow, pPdtInUse->ZoneHigh );

                pMYKI_TAControl->ProvisionalFare = getZoneFare( pPdtInUse->ZoneLow, pData->Tariff.locationZone );
            }

        }

        // c.   Scenario: Scan on outside e-Pass zone range & intercepted inside e-Pass zone:
        //    If the e-pass zone low5 and e-pass zone high6 includes neither of: the low border zone21,
        //    or actual zone22, or high zone border23 of the provisional product scan on location and
        //    further the e-pass zone low5 and e-pass zone high6 is within: the low border zone21,
        //    or actual zone22, or high zone border23 of the current location20 then
        if ( (pEpassProduct->ZoneLow > pData->Tariff.locationInnerBorder ||
               pEpassProduct->ZoneHigh < pData->Tariff.locationOuterBorder ||
               pEpassProduct->ZoneLow > pData->Tariff.locationZone ||
               pEpassProduct->ZoneHigh < pData->Tariff.locationZone) &&
               (pEpassProduct->ZoneLow <= pData->Tariff.location ||
                pEpassProduct->ZoneHigh >= pData->Tariff.location) )
        {
            CsDbg( BRLL_RULE, "BR_LLSC_5_7 : Scenario c");

            // i.   Perform a Product Update/Extend transaction for the provisional product:
            productUpdateExtend(pData, pDirInUse, &productUpdateReq);

            // (1)  If the trip is inbound
            // (a)  Set the product low zone5 to the low zone of the epass, If the resulting product low zone5 minus 1 is marked as City Saver12, decrement the resulting product low zone5 by 1.
            // (b)  Set the provisionsal product high zone to the lesser of boarding location zone or low borader zone; If the resulting product high zone5 is marked as City Saver12, increment the resulting product low zone5 by 1.
            // (c)  Set the provisional fare2 to the fare from the zone low, to zone high for the current date
            if ( pData->Tariff.conductorLinesTripDirection == TRIP_DIRECTION_INBOUND )
            {
                pPdtInUse->ZoneLow = pEpassProduct->ZoneLow;
                pPdtInUse->ZoneHigh = (pData->Tariff.location < pData->Tariff.locationInnerBorder)? pData->Tariff.location : pData->Tariff.locationInnerBorder;

                ADJUST_FOR_CITYSAVER( pPdtInUse->ZoneLow, pPdtInUse->ZoneHigh );

                pMYKI_TAControl->ProvisionalFare = getZoneFare( pPdtInUse->ZoneLow, pPdtInUse->ZoneHigh );
            }
            // (2)  Else if the trip direction is outbound
            // (a)  Set the product high zone5 to the high zone of the epass, If the resulting product high zone5 is marked as City Saver12, increment the resulting product low zone5 by 1.
            // (b)  Set the provisionsal product low zone to the greater of bording location zone or low border zone; If the resulting product low zone5 minus 1 is marked as City Saver12, decrement the resulting product low zone5 by 1.
            // (c)  Set the provisional fare2 to the fare from the zone low, to zone high for the current date
            else if ( pData->Tariff.conductorLinesTripDirection == TRIP_DIRECTION_OUTBOUND)
            {
                pPdtInUse->ZoneLow = (pData->Tariff.location > pData->Tariff.locationOuterBorder)? pData->Tariff.location : pData->Tariff.locationOuterBorder;
                pPdtInUse->ZoneHigh = pEpassProduct->ZoneHigh;

                ADJUST_FOR_CITYSAVER( pPdtInUse->ZoneLow, pPdtInUse->ZoneHigh );

                pMYKI_TAControl->ProvisionalFare = getZoneFare( pPdtInUse->ZoneLow, pPdtInUse->ZoneHigh );
            }
            // ii.  Else If the trip direction is not specified
            // (a)  Set the provisional product low zone5 to the zone of origin10 or declared destination zone, whichever is lower. If the resulting product low zone5 minus 1 is marked as City Saver12, decrement the resulting produsct low zone5 by 1.
            // (b)  Set the product high zone6 to the zone of origin10 or declared destination zone, whichever is higher. If the resulting product high zone6 is marked as City Saver12, increment the resulting product high zone6 by 1.
            // (c)  Set the provisional fare2 to the fare from the zone low, to zone high for the current date
            else
            {
                pPdtInUse->ZoneLow = (pData->DynamicData.provisionalZoneLow < pData->Tariff.locationZone) ? pData->DynamicData.provisionalZoneLow : pData->Tariff.locationZone;
                pPdtInUse->ZoneHigh = (pData->DynamicData.provisionalZoneHigh > pEpassProduct->ZoneHigh) ? pData->DynamicData.provisionalZoneHigh : pData->Tariff.locationZone;

                ADJUST_FOR_CITYSAVER( pPdtInUse->ZoneLow, pPdtInUse->ZoneHigh );

                pMYKI_TAControl->ProvisionalFare = getZoneFare( pPdtInUse->ZoneLow, pData->Tariff.locationZone );
            }

        }
    }

    //    4.  If there is:
    //    a.  No e-pass present or
    //    b.  Scenario: scan on outside e-pass zone and intercepted outside e-pass Zone.
    //         If the e-pass zone low5 and e-pass zone high6 includes neither : the low border zone21, or actual  zone22,
    //         or high zone border23 of the provisional product scan on location and further the e-pass zone low zone5
    //         and e-pass zone high zone6 is not within either: the low border zone21, or actual border zone22,
    //         or high zone border23 of the current location20; then
    if (pEpassProduct == NULL || (pEpassProduct != NULL &&
                  (pEpassProduct->ZoneLow < pData->Tariff.locationInnerBorder &&
                   pEpassProduct->ZoneHigh > pData->Tariff.locationOuterBorder &&
                   pEpassProduct->ZoneLow < pData->Tariff.locationZone &&
                   pEpassProduct->ZoneHigh > pData->Tariff.locationZone)))
    {
        CsDbg( BRLL_RULE, "BR_LLSC_5_7 : Scenario 4");

        //    c.  Perform a Product Update/Extend transaction for the provisional product:
        productUpdateExtend(pData, pDirInUse, &productUpdateReq);

        //    i.  if the trip direction is inbound
        //    (1) Set the product low zone5 to the decleard destination zone, If the resulting product low zone5 minus 1 is marked as City Saver12, decrement the resulting product low zone5 by 1.
        //    (2) Set the provisionsal product high zone to the lesser of bording location zone or low  border zone; If the resulting product high zone5 is marked as City Saver12, increment the resulting product low zone5 by 1.
        //    (3) Set the provisional fare2 to the fare from the zone low, to zone high for the current date
        if ( pData->Tariff.conductorLinesTripDirection == TRIP_DIRECTION_INBOUND )
        {
            pPdtInUse->ZoneLow = pEpassProduct->ZoneLow;
            pPdtInUse->ZoneHigh = (pData->Tariff.location < pData->Tariff.locationInnerBorder)? pData->Tariff.location : pData->Tariff.locationInnerBorder;

            ADJUST_FOR_CITYSAVER( pPdtInUse->ZoneLow, pPdtInUse->ZoneHigh );

            pMYKI_TAControl->ProvisionalFare = getZoneFare( pPdtInUse->ZoneLow, pPdtInUse->ZoneHigh );
        }
        //    ii. Else if the trip direction is outbound
        //    (1) Set the product high zone5 to the decleard destination zone, If the resulting product high zone5 is marked as City Saver12, increment the resulting product low zone5 by 1.
        //    (2) Set the provisionsal product low zone to the greater of bording location zone or high border zone; If the resulting product low zone5 minus 1 is marked as City Saver12, decrement the resulting product low zone5 by 1.
        //    (3) Set the provisional fare2 to the fare from the zone low, to zone high for the current date
        else if ( pData->Tariff.conductorLinesTripDirection == TRIP_DIRECTION_OUTBOUND)
        {
            pPdtInUse->ZoneLow = (pData->Tariff.location > pData->Tariff.locationOuterBorder)? pData->Tariff.location : pData->Tariff.locationOuterBorder;
            pPdtInUse->ZoneHigh = pEpassProduct->ZoneHigh;

            ADJUST_FOR_CITYSAVER( pPdtInUse->ZoneLow, pPdtInUse->ZoneHigh );

            pMYKI_TAControl->ProvisionalFare = getZoneFare( pPdtInUse->ZoneLow, pPdtInUse->ZoneHigh );
        }
        //    iii.    Else If the trip direction is not specified
        //    (1) Set the product low zone5 to the zone of origin10 or declared destination zone, whichever is lower. If the resulting product low zone5 minus 1 is marked as City Saver12, decrement the resulting product low zone5 by 1.
        //    (2) Set the product high zone6 to the zone of origin10 or declared destination zone, whichever is higher. If the resulting product high zone6 is marked as City Saver12, increment the resulting product high zone6 by 1.
        //    (3) Set the provisional fare2 to the fare from the zone low, to zone high for the current date
        else
        {
            pPdtInUse->ZoneLow = (pData->DynamicData.provisionalZoneLow < pData->Tariff.locationZone) ? pData->DynamicData.provisionalZoneLow : pData->Tariff.locationZone;
            pPdtInUse->ZoneHigh = (pData->DynamicData.provisionalZoneHigh > pEpassProduct->ZoneHigh) ? pData->DynamicData.provisionalZoneHigh : pData->Tariff.locationZone;

            ADJUST_FOR_CITYSAVER( pPdtInUse->ZoneLow, pPdtInUse->ZoneHigh );

            pMYKI_TAControl->ProvisionalFare = getZoneFare( pPdtInUse->ZoneLow, pData->Tariff.locationZone );
        }

    }

    //  4.c Create usage log
    //      i.  Definition: 
    {
        //      (1) Type (On board= 20), 
        pData->InternalData.UsageLogData.transactionType                = MYKI_BR_TRANSACTION_TYPE_ON_BOARD;

        //      (2) Determine uselog ControlBitmap though look up of the TxUsageType (Done by framework)
        //      (3) ProviderID = ServiceProviderID(14) (Done by framework)
        //      (4) TxDateTime = Current Date time(9) (Done by framework)
        //      (5) Location.EntryPoint = EntryPointID(16) (Done by framework)
        //      (6) Location.Route= LineID(17) (Done by framework)
        //      (7) Location.Stop= Stopid(11) (Done by framework)
    }

    //      ii. Value: not defined 

    //      iii.Usage: 
    {
        //      (1) Zone = Dynamic.Zone(8)
        pData->InternalData.UsageLogData.isZoneSet                      = TRUE;
        pData->InternalData.UsageLogData.zone                           = pData->DynamicData.currentZone;

        //      (2) ProductValidationStatus: Scan-On, Provisional
        pData->InternalData.UsageLogData.isProductValidationStatusSet   = TRUE;
        pData->InternalData.UsageLogData.productValidationStatus       |= TAPP_USAGE_LOG_PROD_VAL_STATUS_SCAN_ON_BITMAP |
                                                                          TAPP_USAGE_LOG_PROD_VAL_STATUS_PROVISIONAL_BITMAP;
    }

    //      v.  Product: 
    {
        //      (1) ProductIssuerId = As returned Product Update/Extend 
        pData->InternalData.UsageLogData.isProductIssuerIdSet           = TRUE;
        pData->InternalData.UsageLogData.productIssuerId                = pDirInUse->IssuerId;

        //      (2) ProductSerialNo = As returned Product Update/Extend 
        pData->InternalData.UsageLogData.isProductSerialNoSet           = TRUE;
        pData->InternalData.UsageLogData.productSerialNo                = pDirInUse->SerialNo;

        //      (3) ProductId = As returned from Product Update/Extend
        pData->InternalData.UsageLogData.isProductIdSet                 = TRUE;
        pData->InternalData.UsageLogData.productId                      = pDirInUse->ProductId;
    }

    //  NOTE:   TAppUsageLog entry is added by application framework.
    pData->InternalData.IsUsageLogUpdated                               = TRUE;
    CsDbg( BRLL_RULE, "BR_LLSC_5_7 : Updated TAppUsageLog" );

    CsDbg( BRLL_RULE, "BR_LLSC_5_7 : Executed" );
    return RULE_RESULT_EXECUTED;
}

