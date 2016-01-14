/**************************************************************************
*   ID :  BR_LLSC_6_7
*
*    Process Border Products
*    Only for current trip is not a border travel then:
*    1.  TAppControl.ProductInUse
*    2.  TAppTProduct.EndDateTime
*    3.  Dynamic.CurrentDateTime
*    4.  TAppTProduct.ControlBitmap
*    5.  Dynamic.CurrentTripZoneLow
*    6.  Dynamic.CurrentTripZoneHigh
*    7.  TAppTProduct.ZoneLow
*    8.  TAppTProduct.ZoneHigh
*    9.  TAppTProduct.PurchaseValue
*    10.  TAppTPurseBalance.Balance
*    11.  TAppCapping.Daily.ZoneLow
*    12.  TAppCapping.Daily.ZoneHigh
*    13.  TAppCapping.Daily.Value
*    14.  Dynamic.FareRouteID
*    15.  Dynamic.CappingContribution
*
*
*   Pre-Requisites
*    1.  The product in use field(1)  is not 0 (a product is in a scanned-on state).
*    2.  A n-Hour product exists where the product Border Status bit in the product control bitmap(4)  is
*        set true .
*    3.  No combination of products exists on the card that cover both the low zone of the current trip(5)
*        and the high zone of the current trip(6)  .
*    4.  The current trip is not a border trip
*
*   Description
*    1.  For each n-Hour border product :
*        a.  If the product covers the low zone of the current trip(5)
*               i.  Perform a ProductUpdate/None transaction that clears the BorderStatus bit of the
*                   product control bitmap(4) .
*        b.  Else, if the low zone of the current trip(5)  is equal to the product low zone(7)  incremented by
*            1, and the BorderSide bit of the product control bitmap(4)  is outbound:
*                i.  If Daily capping low zone(11)  is equal to n-Hour product low zone(7)  and Daily capping
*                    high zone(12)  is also equal to n-Hour product high zone(8)  and if the Daily capping
*                    Value(13)  is equal to the n-Hour product Purchase Value(9) :
*                       (1)  Perform a CappingUpdate/None transaction
*                          (a) Set the daily capping low zone(11)  and daily capping high zone(12)  to the value of
*                              product low zone incremented by 1
*              ii.  Determine the fare for the:
*                       (1)  current trip(5)  low zone
*                       (2)  date/time(3)
*                       (3)  The fare route ID(14) .
*                       (4)  the passenger type
*              iii. Increment the applicable fare by the result of the fare minus the product purchase
*                    value.
*              iv.  Increase the capping contribution(15)  by the result of the fare minus the product
*                   purchase value..
*              v.  Perform a ProductUpdate/Extend transaction:
*                    (1)  Set the product purchase value to the fare.
*                    (2)  Set the product low(7)  and high(8)  zone to the low zone of the current trip(5) .
*                    (3)  Clear the BorderStatus bit of the product control bitmap(4) .
*        c.  If the product covers the high zone of the current trip(6) :
*                i.  Perform a ProductUpdate/None transaction that clears the BorderStatus bit of the
*                    product control bitmap(4) .
*        d.  Else, if high zone of the current trip(6)  is equal to the product high zone(8)  decremented by 1,
*            and the BorderSide bit of the product control bitmap(4)  is inbound:
*              i.  If Daily capping low zone(11)  is equal to the n-Hour product low zone(7)  and Daily capping
*                  high zone(12)  is also equal to n-Hour product high zone(8)  and if the Daily capping value(13)
*                  is equal to the n-Hour product Purchase value(9) :
*                      (1)  Perform a CappingUpdate/None transaction
*                          (a)  Set the daily capping low zone(11)  and daily capping high zone(12)  to the value of
*                               the product low zone decremented by 1
*              ii.  Determine the fare for the:
*                      (1)  High zone of the current trip(6)
*                      (2)  Current date/time(3)
*                      (3)  Fare Route ID(14)
*                      (4)  The passenger type
*              iii.  Increment the applicable fare by the result of the fare minus the product purchase
*                    value
*              iv.  Increase the capping contribution(15)  by the result of the fare minus the product
*                   purchase value.
*              v.  Perform a ProductUpdate/Extend transaction:
*                  (1)  Set the product purchase value to the fare
*                  (2)  Set the product low(7)  and high(8)  zone to the high zone of the current trip(6) .
*                  (3)  Adjust for city saver zone:
*                      (a)   If the product's  low zone(5)  less 1 is a City Saver zone then decrement the
*                            product low zone(7)  by 1
*                      (b)  If the product's  high zone(6)  is a city saver then increment the product high
*                           zone(8)  by 1
*              vi.  Perform a ProductUpdate/None to
*                  (1)  Clear the BorderStatus bit of the product control bitmap(4)
*
*   Post-Conditions
*    1.   Border products covering either the low or high zone of the current trip are processed.
*
*      Devices
*    Fare payment devices
*
 ***********************************************************************/

#include <cs.h>                 // For debug logging
#include <myki_cardservices.h>  // Myki Card Services
#include <LDT.h>                // Logical Device Transaction
#include <myki_cd.h>                        // myki_cd (Myki Configuration Data)
#include "myki_br_rules.h"      // API for all BR_xxx modules
#include "BR_Common.h"          // Common utilities, macros, etc

//=============================================================================
//
//
//
//=============================================================================

RuleResult_e BR_LLSC_6_7( MYKI_BR_ContextData_t *pData )
{
    MYKI_TAControl_t            *pMYKI_TAControl;
    U32_t                       dir;
    U8_t                        isNhourBorder = FALSE;
    MYKI_CD_PassengerType_t     passenger;
    Currency_t                  fare;
    MYKI_TAProduct_t            UpdatedProduct;
    MYKI_TAProduct_t            *pProductInUse      = NULL;
    MYKI_TACapping_t            *pMYKI_TACapping    = NULL;
    int                         zoneLowCovered      = FALSE;
    int                         zoneHighCovered     = FALSE;
    ProductIterator             iProduct;
    int                         haveActiveEpass     = FALSE;
    int                         isClearBorderStatus = FALSE;

    CsDbg( BRLL_RULE, "BR_LLSC_6_7 : Start (Process Border Products)" );

    //
    //  Pre-conditions
    //

    if ( ! pData )
    {
        CsErrx( "BR_LLSC_6_7 : Invalid argument(s)" );
        return RULE_RESULT_ERROR;
    }

    if ( MYKI_CS_TAControlGet( &pMYKI_TAControl ) < 0 )
    {
        CsErrx( "BR_LLSC_6_7 : MYKI_CS_TAControlGet() failed" );
        return RULE_RESULT_ERROR;
    }

    //  1.  The product in use field(1) is not 0

    if ( pMYKI_TAControl->ProductInUse == 0 )
    {
        CsDbg( BRLL_RULE, "BR_LLSC_6_7 : Bypass - ProductInUse is zero");
        return RULE_RESULT_BYPASSED;
    }

    if ( MYKI_CS_TAProductGet( (U8_t)( pMYKI_TAControl->ProductInUse - 1 ), &pProductInUse ) < 0 )
    {
        CsErrx( "BR_LLSC_6_7 : MYKI_CS_TAProductGet() failed" );
        return RULE_RESULT_ERROR;
    }

    //  2.  A n-Hour product exists where the product Border Status bit in the
    //      product control bitmap(4) is set true

    haveActiveEpass = myki_br_HasActiveEpass( pData, pMYKI_TAControl );

    CsDbg( BRLL_RULE, "BR_LLSC_6_7 : Searching for border products and zone range coverage (%d - %d)", pData->DynamicData.currentTripZoneLow, pData->DynamicData.currentTripZoneHigh );

    myki_br_InitProductIterator( pMYKI_TAControl, &iProduct, PRODUCT_TYPE_UNKNOWN );
    while ( myki_br_ProductIterate( &iProduct ) > 0 )
    {
        CsDbg
        (
            BRLL_RULE,
            "BR_LLSC_6_7 : Checking product %d, type %d, bitmap 0x%02x, zones %d - %d",
            iProduct.index,
            iProduct.currentProduct,
            iProduct.pProduct->ControlBitmap,
            iProduct.pProduct->ZoneLow,
            iProduct.pProduct->ZoneHigh
        );

        if ( iProduct.index == pMYKI_TAControl->ProductInUse )
        {
            CsDbg( BRLL_RULE, "BR_LLSC_6_7 : Ignoring product in use" );
            continue;
        }

        if ( iProduct.pDirectory->Status != TAPP_CONTROL_DIRECTORY_STATUS_ISSUED && iProduct.pDirectory->Status != TAPP_CONTROL_DIRECTORY_STATUS_ACTIVATED )
        {
            CsDbg( BRLL_RULE, "BR_LLSC_6_7 : Ignoring product which is neither issued nor activated" );
            continue;
        }

        if ( iProduct.pProduct->EndDateTime < pProductInUse->StartDateTime )
        {
            CsDbg( BRLL_RULE, "BR_LLSC_6_7 : Ignoring expired product" );
            continue;
        }

        if
        (
            ( iProduct.currentProduct == PRODUCT_TYPE_NHOUR ) &&
            ( iProduct.pProduct->ControlBitmap & TAPP_TPRODUCT_CONTROL_BORDER_STATUS_BITMAP )
        )
        {
            CsDbg( BRLL_RULE, "BR_LLSC_6_7 : Found an nHour border product" );
            isNhourBorder = TRUE;
        }
        else
        {
            //  Do not consider an inactive ePass if there is already an active ePass on the card as the inactive
            //  ePass will not be able to be activated.
            if
            (
                ( iProduct.currentProduct == PRODUCT_TYPE_EPASS ) &&
                ( iProduct.pDirectory->Status == TAPP_CONTROL_DIRECTORY_STATUS_ISSUED ) &&
                ( haveActiveEpass )
            )
            {
                CsDbg( BRLL_RULE, "BR_LLSC_6_7 : Found an inactive ePass, ignoring because card already has an active ePass" );
                continue;
            }

            CsDbg( BRLL_RULE, "BR_LLSC_6_7 : Checking zone range coverage (%d - %d)", iProduct.pProduct->ZoneLow, iProduct.pProduct->ZoneHigh );

            //  Now check to see if the product covers the low zone and/or the high zone
            if ( iProduct.pProduct->ZoneLow <= pData->DynamicData.currentTripZoneLow && iProduct.pProduct->ZoneHigh >= pData->DynamicData.currentTripZoneLow )
            {
                CsDbg( BRLL_RULE, "BR_LLSC_6_7 : Zone Low is covered" );
                zoneLowCovered = TRUE;
            }

            if ( iProduct.pProduct->ZoneLow <= pData->DynamicData.currentTripZoneHigh && iProduct.pProduct->ZoneHigh >= pData->DynamicData.currentTripZoneHigh )
            {
                CsDbg( BRLL_RULE, "BR_LLSC_6_7 : Zone High is covered" );
                zoneHighCovered = TRUE;
            }
        }
    }

    if ( iProduct.error )
    {
        CsErrx( "BR_LLSC_6_7 : Iteration Failure" );
        return RULE_RESULT_ERROR;
    }


    if ( ! isNhourBorder )
    {
        CsDbg( BRLL_RULE, "BR_LLSC_6_7 : Bypass - No border N-Hour product.");
        return RULE_RESULT_BYPASSED;
    }

    //  3. No combination of products exists on the card that cover both the low zone of the current trip(5)
    //     and the high zone of the current trip(6)
    //
    //      Ashish has confirmed that this should be interpreted as :
    //      - The low zone of the current trip is not covered by an active non-border product, OR
    //      - The high zone of the current trip is not covered by an active non-border product
    //
    //      ie, we bypass if both of the boundary zones *are* covered, even if some of the zones
    //          in between *are not* covered.

    if ( zoneLowCovered && zoneHighCovered )
    {
        CsDbg( BRLL_RULE, "BR_LLSC_6_7 : Bypass - Both low and high zones covered");
        return RULE_RESULT_BYPASSED;
    }

    // 4.  The current trip is not a border trip
    //
    //  Get product and directory structure in use */
    if ( MYKI_CS_TAProductGet( (U8_t)( pMYKI_TAControl->ProductInUse - 1 ), &pProductInUse ) < 0 )
    {
        CsErrx( "BR_LLSC_6_7 : MYKI_CS_TAProductGet() failed" );
        return RULE_RESULT_ERROR;
    }

    if (pProductInUse->ControlBitmap & TAPP_TPRODUCT_CONTROL_BORDER_STATUS_BITMAP)
    {
        CsDbg( BRLL_RULE, "BR_LLSC_6_7 : Bypass - Current trip is a border trip" );
        return RULE_RESULT_BYPASSED;
    }

    //
    //  Process
    //

    /* For each n-Hour border product  */

    for ( dir = 1; dir < DIMOF( pMYKI_TAControl->Directory ); dir++ )   //  For each product slot (other than the purse slot)
    {
        MYKI_Directory_t    *pDirectory = NULL;
        MYKI_TAProduct_t    *pProduct = NULL;

        if ( myki_br_GetCardProduct( dir, &pDirectory, NULL ) < 0 )
        {
            CsErrx( "BR_LLSC_6_7 : myki_br_GetCardProduct() failed" );
            return RULE_RESULT_ERROR;
        }

        //  Product is nHour
        if ( myki_br_cd_GetProductType( pDirectory->ProductId ) == PRODUCT_TYPE_NHOUR )
        {
            if ( myki_br_GetCardProduct( dir, &pDirectory, &pProduct ) < 0 )
            {
                CsErrx( "BR_LLSC_6_7 : myki_br_GetCardProduct() failed" );
                return RULE_RESULT_ERROR;
            }

            //  Product is nHour Border
            if ( pProduct->ControlBitmap & TAPP_TPRODUCT_CONTROL_BORDER_STATUS_BITMAP )
            {
                //  1.a.        [If the current trip zone low is not covered by any other product and]
                //              If the product covers the low zone of the current trip
                if
                (
                    zoneLowCovered == FALSE &&
                    pProduct->ZoneLow <= pData->DynamicData.currentTripZoneLow &&
                    pProduct->ZoneHigh >= pData->DynamicData.currentTripZoneLow
                )
                {
                    CsDbg( BRLL_RULE, "BR_LLSC_6_7 : Execute: 1.a");
                    //  1.a.i.      Clear the product's border status
                    CsDbg( BRLL_RULE, "BR_LLSC_6_7 : 1.a.i. myki_br_ldt_ProductUpdate_ClearBorderStatus");
                    if ( myki_br_ldt_ProductUpdate_ClearBorderStatus( pData, pDirectory ) < 0 )
                    {
                        CsErrx( "BR_LLSC_6_7 : myki_br_ldt_ProductUpdate_ClearBorderStatus() failed" );
                        return RULE_RESULT_ERROR;
                    }
                }
                //  1.b.        else [if the current trip zone low is not covered by any other product and]
                //              if the current trip low zone equals the product low zone plus one and the product border direction is outbound
                else if
                (
                    zoneLowCovered == FALSE &&
                    ( ( pProduct->ZoneLow + 1 ) == pData->DynamicData.currentTripZoneLow ) &&
                    ( ( pProduct->ControlBitmap & TAPP_TPRODUCT_CONTROL_BORDER_SIDE_BITMAP ) != 0 ) // Set = outbound
                )
                {
                    CsDbg( BRLL_RULE, "BR_LLSC_6_7 : Execute: 1.b");
                    if ( MYKI_CS_TACappingGet( &pMYKI_TACapping ) < 0 )
                    {
                        CsErrx( "BR_LLSC_6_7 : MYKI_CS_TACappingGet() failed" );
                        return RULE_RESULT_ERROR;
                    }

                    //  1.b.i.
                    //      If Daily capping low zone(11)  is equal to n-Hour product low zone(7)  and Daily capping
                    //      high zone(12)  is also equal to n-Hour product high zone(8)  and if the Daily capping
                    //      Value(13)  is equal to the n-Hour product Purchase Value(9) :
                    if
                    (
                        pMYKI_TACapping->Daily.Zone.Low == pProduct->ZoneLow &&
                        pMYKI_TACapping->Daily.Zone.High == pProduct->ZoneHigh &&
                        pMYKI_TACapping->Daily.Value == pProduct->PurchaseValue
                    )
                    {
                        //  1.b.i.1.    Perform a CappingUpdate/None transaction
                        //  1.b.i.1.a.  Set the daily capping low and high zones to the product low zone plus one

                        CsDbg( BRLL_RULE, "BR_LLSC_6_7 : Execute: 1.b.1");
                        if ( myki_br_ldt_PerformCappingUpdateDailyZones( pData, pProduct->ZoneLow + 1) )
                        {
                            CsErrx("BR_LLSC_6_7 : myki_br_ldt_PerformCappingUpdateDailyZones() failed");
                            return RULE_RESULT_ERROR;
                        }
                    }

                    //  ii.  Determine the fare for the:
                    //           (1)  current trip(5)  low zone
                    //           (2)  date/time(3)
                    //           (3)  The fare route ID(14) .
                    //           (4)  the passenger type
                    //
                    if ( ! MYKI_CD_getPassengerTypeStructure( pMYKI_TAControl->PassengerCode, &passenger ) )
                    {
                        CsErrx( "BR_LLSC_6_7 : getPassengerTypeStructure() failed" );
                        return RULE_RESULT_ERROR;
                    }

                    if
                    (
                        myki_br_getFareStoredValueEx
                        (
                            &fare,
							pData,
							pData->DynamicData.currentTripZoneLow,
                            pData->DynamicData.currentTripZoneLow,
                            pMYKI_TAControl->PassengerCode,
                            pData->DynamicData.currentTripDirection,
                            pData->DynamicData.fareRouteIdIsValid,
                            pData->DynamicData.lineId,
							pProductInUse->StartDateTime,
							pData->DynamicData.currentDateTime
                        ) < 0
                    )
                    {
                        CsErrx( "BR_LLSC_6_7 : myki_br_getFareStoredValueEx() failed" );
                        return RULE_RESULT_ERROR;
                    }

                    //  1.b.iii.    Increment the applicable fare by the result of the fare minus the product purchase value

                    CsDbg( BRLL_FIELD, "BR_LLSC_6_7 : Increasing applicable fare by %d from %d to %d", (fare - pProduct->PurchaseValue), pData->ReturnedData.applicableFare, pData->ReturnedData.applicableFare + (fare - pProduct->PurchaseValue) );
                    pData->ReturnedData.applicableFare += ( fare - pProduct->PurchaseValue );

                    //  1.b.iv.     Increment the capping contribution by the result of the fare minus the product purchase value

                    pData->DynamicData.cappingContribution += (fare - pProduct->PurchaseValue);

                    //  1.b.v.      Perform a product update/extend

                    UpdatedProduct = *pProduct;     //  First, create a copy of the product so we can modify it

                    //  v.  Perform a ProductUpdate/Extend transaction:
                    //        (1)  Set the product purchase value to the fare.
                    //        (2)  Set the product low(7)  and high(8)  zone to the low zone of the current trip(5) .
                    //        (3)  Clear the BorderStatus bit of the product control bitmap(4) .
                    //
                    UpdatedProduct.PurchaseValue    = fare;
                    UpdatedProduct.ZoneHigh         = pData->DynamicData.currentTripZoneLow;
                    UpdatedProduct.ZoneLow          = pData->DynamicData.currentTripZoneLow;
                    UpdatedProduct.ControlBitmap   &= ~TAPP_TPRODUCT_CONTROL_BORDER_STATUS_BITMAP;

                    ADJUST_FOR_CITYSAVER( UpdatedProduct.ZoneLow, UpdatedProduct.ZoneHigh );        // Not in document in this section, but is in section 1.d.v.

                    //  1.b.v.      Perform a product update/extend

                    CsDbg( BRLL_RULE, "BR_LLSC_6_7 : 1.b.v. myki_br_ldt_ProductUpdate_Extend");
                    if ( myki_br_ldt_ProductUpdate_Extend( pData, pDirectory, pProduct, &UpdatedProduct, TRUE, TRUE, TRUE ) < 0 )
                    {
                        CsErrx( "BR_LLSC_6_7 : myki_br_ldt_ProductUpdate_Extend() failed" );
                        return RULE_RESULT_ERROR;
                    }
                }

                //  1.c.    [If the current trip zone high is not covered by any other product and]
                //          If the product covers the high zone of the current trip(6) :
                //          i.  Perform a ProductUpdate/None transaction that clears the BorderStatus bit of the
                //              product control bitmap(4) .
                else if
                (
                    zoneHighCovered == FALSE &&
                    pProduct->ZoneLow <= pData->DynamicData.currentTripZoneHigh &&
                    pProduct->ZoneHigh >= pData->DynamicData.currentTripZoneHigh
                )
                {
CsDbg( BRLL_RULE, "BR_LLSC_6_7 : Execute: 1.c");
                    //  1.c.i.      Clear the product's border status
CsDbg( BRLL_RULE, "BR_LLSC_6_7 : 1.c.i. myki_br_ldt_ProductUpdate_ClearBorderStatus");
                    if (myki_br_ldt_ProductUpdate_ClearBorderStatus(pData, pDirectory) < 0)
                    {
                        CsErrx( "BR_LLSC_6_7 : myki_br_ldt_ProductUpdate_ClearBorderStatus() failed" );
                        return RULE_RESULT_ERROR;
                    }
                }
                //  1.d.    Else, [if the current trip zone high is not covered by any other product and]
                //          if high zone of the current trip(6) is equal to the product high zone(8) decremented by 1,
                //          and the BorderSide bit of the product control bitmap(4) is inbound:
                //
                else if
                (
                    zoneHighCovered == FALSE &&
                    ( ( pProduct->ZoneHigh - 1 ) == pData->DynamicData.currentTripZoneHigh ) &&
                    ( ( pProduct->ControlBitmap & TAPP_TPRODUCT_CONTROL_BORDER_SIDE_BITMAP ) == 0 ) // Clear = inbound
                )
                {
CsDbg( BRLL_RULE, "BR_LLSC_6_7 : Execute: 1.d");
                    //  i.  If Daily capping low zone(11)  is equal to the n-Hour product low zone(7)  and Daily capping
                    //      high zone(12)  is also equal to n-Hour product high zone(8)  and if the Daily capping value(13)
                    //      is equal to the n-Hour product Purchase value(9) :
                    //          (1)  Perform a CappingUpdate/None transaction
                    //              (a)  Set the daily capping low zone(11)  and daily capping high zone(12)  to the value of
                    //                   the product low zone decremented by 1
                    if
                    (
                        pData->DynamicData.cappingEpassZoneLow == pProduct->ZoneLow &&
                        pData->DynamicData.cappingEpassZoneHigh == pProduct->ZoneHigh &&
                        pData->DynamicData.cappingContribution == pProduct->PurchaseValue
                    )
                    {
                        if ( myki_br_ldt_PerformCappingUpdateDailyZones( pData, pProduct->ZoneLow - 1) )
                        {
                            CsErrx("BR_LLSC_6_7 : myki_br_ldt_PerformCappingUpdateDailyZones() failed");
                            return RULE_RESULT_ERROR;
                        }
                    }

                    //  ii.  Determine the fare for the:
                    //          (1)  High zone of the current trip(6)
                    //          (2)  Current date/time(3)
                    //          (3)  Fare Route ID(14)
                    //          (4)  The passenger type
                    //

                    CsDbg( BRLL_RULE, "pMYKI_TAControl->PassengerCode = %d\n", pMYKI_TAControl->PassengerCode );

                    if ( ! MYKI_CD_getPassengerTypeStructure( pMYKI_TAControl->PassengerCode, &passenger ) )
                    {
                        CsErrx( "BR_LLSC_6_7 : getPassengerTypeStructure() failed" );
                        return RULE_RESULT_ERROR;
                    }

                    if
                    (
                        myki_br_getFareStoredValueEx
                        (
                            &fare,
							pData,
                            pData->DynamicData.currentTripZoneHigh,
                            pData->DynamicData.currentTripZoneHigh,
                            pMYKI_TAControl->PassengerCode,
                            pData->DynamicData.currentTripDirection,
                            pData->DynamicData.fareRouteIdIsValid,
                            pData->DynamicData.lineId,
							pProductInUse->StartDateTime,
							pData->DynamicData.currentDateTime
                        ) < 0
                    )
                    {
                        CsErrx( "BR_LLSC_6_7 : myki_br_getFareStoredValueEx() failed" );
                        return RULE_RESULT_ERROR;
                    }

                    //  1.d.iii.    Increment the applicable fare by the result of the fare minus the product purchase value

                    CsDbg( BRLL_FIELD, "BR_LLSC_6_7 : Increasing applicable fare by %d from %d to %d", (fare - pProduct->PurchaseValue), pData->ReturnedData.applicableFare, pData->ReturnedData.applicableFare + (fare - pProduct->PurchaseValue) );
                    pData->ReturnedData.applicableFare += ( fare - pProduct->PurchaseValue );

                    //  1.d.iv.     Increment the capping contribution by the result of the fare minus the product purchase value

                    pData->DynamicData.cappingContribution += (fare - pProduct->PurchaseValue);

                    //  1.d.v.  Perform a ProductUpdate/Extend transaction:
                    //      (1)  Set the product purchase value to the fare
                    //      (2)  Set the product low(7)  and high(8)  zone to the high zone of the current trip(6) .
                    //      (3)  Adjust for city saver zone:
                    //          (a)   If the product's  low zone(5)  less 1 is a City Saver zone then decrement the
                    //                product low zone(7)  by 1
                    //          (b)  If the product's  high zone(6)  is a city saver then increment the product high
                    //               zone(8)  by 1

                    UpdatedProduct = *pProduct;     //  First, create a copy of the product so we can modify it

                    UpdatedProduct.PurchaseValue    = fare;
                    UpdatedProduct.ZoneHigh         = pData->DynamicData.currentTripZoneHigh;
                    UpdatedProduct.ZoneLow          = pData->DynamicData.currentTripZoneHigh;

                    ADJUST_FOR_CITYSAVER( UpdatedProduct.ZoneLow, UpdatedProduct.ZoneHigh );

                    if ( pData->StaticData.AcsCompatibilityMode != FALSE )
                    {
                        isClearBorderStatus     = TRUE;
                    }

CsDbg( BRLL_RULE, "BR_LLSC_6_7 : 1.d.v. myki_br_ldt_ProductUpdate_Extend");
                    if ( myki_br_ldt_ProductUpdate_Extend( pData, pDirectory, pProduct, &UpdatedProduct, isClearBorderStatus, TRUE, TRUE ) < 0 )
                    {
                        CsErrx( "BR_LLSC_6_7 : myki_br_ldt_ProductUpdate_Extend() failed" );
                        return RULE_RESULT_ERROR;
                    }

                    //
                    //  1.d.vi.  Perform a ProductUpdate/None to
                    //      (1)  Clear the BorderStatus bit of the product control bitmap(4)
                    //
                    if ( ( pProduct->ControlBitmap & TAPP_TPRODUCT_CONTROL_BORDER_STATUS_BITMAP ) != 0 )
                    {
CsDbg( BRLL_RULE, "BR_LLSC_6_7 : 1.d.vi. myki_br_ldt_ProductUpdate_ClearBorderStatus");
                        if ( myki_br_ldt_ProductUpdate_ClearBorderStatus( pData, pDirectory ) < 0 )
                        {
                            CsErrx( "BR_LLSC_6_7 : myki_br_ldt_ProductUpdate_ClearBorderStatus() failed" );
                            return RULE_RESULT_ERROR;
                        }
                    }
                }   // 1.d
            }       // Product is a Border nHour
        }           // Product is nHour
    }               // For each product

    CsDbg( BRLL_RULE, "BR_LLSC_6_7 : Executed" );
    return RULE_RESULT_EXECUTED;
}

