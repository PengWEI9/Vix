/**************************************************************************
*   ID :  BR_LLSC_6_20
*
*   VERSION: Post 7.2. Based on email of the 11-Dec-2013
*    Resolve trip Zone range
*    Resolves the trip zone range based upon a list of
*    locations
*
*    1.  TAppControl.ProductInUse
*    2.  TAppTProduct.LastUsageStopId
*    3.  Dynamic.StopId
*    4.  Dynamic.CurrentTripZoneLow
*    5.  Dynamic.CurrentTripZoneHigh
*    6.  TAppTProduct.LastUsageLineId
*    7.  Dynamic.LineId
*    8.  Dynamic.CurrentDateTime
*    9.  TAppControl.PassengerCode
*    10.  Tariff.HeadlessRouteId
*    11.  Dynamic.CurrentTripIsThinZone
*    12.  Dynamic.CurrentTripDirection
*    13.  Dynamic.RouteChangeoverStatus
*    14.  Dynamic.TraverseRouteID
*    15.  Dynamic.TraverseStopID
*    16.  Dynamic DestinationStopID
*    17.  Dynamic.OriginRouteID
*    18.  Dynamic.OriginStopID
*    19.  Dynamic.TraverseDestinationStopID
*    20.  Dynamic.DestinationRouteID
*    21.  Dynamic.IsEarlyBirdTrip
*    22.  Static.ModeofTransport
*    23.  Dynamic.FareRouteID
*    24.  Dynamic.TransportLocations
*    25.  Dynamic.IsForcedScanOff
*    26.  Dynamic.OffPeakDiscountRate
*    27.  Dynamic.IsOffPeak
*    28.  Dynamic.IsResetDefaultFlag
*
*    Local Variables
*        A - THigh - the curren trip zone high – Initial value 1
*        B - TLow – the current zone low – Initlal value 255
*        C – StopCount
*        D – BorderStopCountLow
*        E – BorderStopCountHIgh
*        F – ThinZoneCount
*        G – CurrentBorderLowZone
*        H – CurrentBorderHighZone
*
*    Pre-Conditions
*    1.  The list of locations(24)  contains 2 or more locations
*
*    Descritpion
*    1. For each location in the list of location(24) :
*        a. If THigh(A) is less than smaller of the location.Low Zone, Actual zone, or High Zone then update
*           Thigh(A) to the smaller of the current location.Low Zone, Actual zone, High Zone
*        b. If TLow(B) is greater than larger of the either location.Low Zone, Actual zone, or HighZone then
*           update TLow(B) to the larger of the either current location.Low Zone, Actual zone, High Zone
*        c. If this is not a true location on the low side, i.e low zone is not equal to Actual Zone then
*            i. If this is the first low border location found then set the current border low(G) to the
*               LowZone of the current location
*            ii. If CurrentBorderZoneLow(G) is equal to the low zone then increment border Low Count(D) by
*                1
*        d. If this is not a true location on the high side, i.e high zone is not equal to Actual Zone
*            i. If this is the first High border location found then set the current border high(H) to the
*               HighZone of the current location
*            ii. If CurrentBorderZoneHigh(H) is equal to the highzone then Increment Border High CountE
*                by 1
*        e. If this is a thin zone i.e Low zone(A) +2 is equal to High Zone(B) then increment Thin Zone
*           Count(E) by 1
*        f. Increment Stop Count(C) by 1
*
*    2. If TLow is greater than Thigh then swap TLow and Thigh values
*
*    3. If all locations of the traversal are in the same “thin zone”, i.e. the difference between the inner and
*       the outer zone is exactly 2 (i.e ThinZone count(E) equals Stop countC) then:
*        a. Set the TLow and Thigh to the actual Zone (i.e set TLow(A) and Thigh(B) to TLow + 1)
*        b. If the product in use(1)  is of type n-Hour and if the Dynamic.IsResetDefaultflag = false then
*            i. Perform a ProductUpdate/None transaction for the product in use(1)
*                (1) Set the BorderStatus bit of the product control bitmap to True.
*                (2) Set the BorderSide bit of the product control bitmap to Inbound.
*        c. Set the Dynamic.CurrentTripIsThinZone(11)  to be true
*        d. Set Current Trip Zone Low(4) to TLow(A)  and Current Trip Zone High(5) to THigh(B)
*
*    4. Else, If all locations of the traversal are border stops along the same border I.E either of the
*       following is true.
*            i. The determined trip zone low(A) plus 1 is equal to the determined trip zone high(B) and
*                BorderCountLow(D) is equal to the stop countC; Or
*            ii. The determined trip zone low(A) plus 1 is equal to the determined trip zone high(B) and
*                BorderCountHigh(E) is equal to he stop countC.
*            iii. If TLow(A) +1 = THIgh(B) and all of the following condition is true for each location in the list of
*                locations
*                    (1) TLow(A) is either in the Low, Actual or Highzone, and
*                    (2) THigh(B) is either in the Low, Actual or HighZone
*        b. then:
*        c. Determine the applicable fare for:
*            i. The border low zone (TLowA).
*            ii. Passenger type(9) .
*            iii. Determined fare route(23)
*            iv. Current day of the week/date(8) .
*            v. Current time of day(8) .
*        d. Determine the applicable fare for:
*            i. The border high zone (THighB).
*            ii. Passenger type(9) .
*            iii. Determined fare route(23)
*            iv. Current day of the week/date(8) .
*            ii. Current time of day(8)
*
*        Note: Section 4.e. Has been clarified since NTS0177 V7.2. The text below is the new text
*                           Also affects BR_LLSC_6_13
*         e. If the applicable fare for the border low zone was lower than or equal to the applicable fare for
*            the border high zone set then set the Current trip low zone(4)  and the current trip high zone(5) to
*            the border low zone; else   set the current trip low zone(4) and the current trip high
*            zone(5) to the border high zone.
*        f. If this not a reset default fare (Dynamic.IsResetDefaultFlag(28)  = False) And If the product in
*           use(1)  is of type n-Hour perform a ProductUpdate/None transaction for the product in use(1) :
*                i. Set the BorderStatus bit of the product control bitmap.
*                ii. If the applicable fare for the border low zone was lower than or equal to the applicable
*                    fare for the border high zone, set the BorderSide bit of the product control bitmap to 1,
*                    else set the BorderSide bit to 0.
*
*    5. Else
*       a.  Set the low zone of the current trip(4) to the low zone(A) and set the high zone of the current trip(5)
*           to the high zone(B).
*
*    6. Clear the list of locations(24)
*
*    7. If the current trip is either:
*        a. A force scan off trip(25)  then set the off peak discount rate(26)  to zero (0)
*        b. Or mode of transport of the current device is not equal to rail and the mode of transport of the
*           scan on service provider is equal Rail
*        c. Then set the off peak discount rate(26)  to 0
*
*    8. Else determine if the current trip is an off peak using the :
*        a.
*            i. RuleType is off-peak
*            ii. Scan on time,
*            iii. Scan off date time,
*            iv. Current trip low zone,
*            v. Current trip high zone,
*            vi. The trip direction,
*        b. Set the off peak set the off peak rate to the determined off peak discount rate(26) .
*        c. Set Dynamic.IsOffPeak(27)  = True
*
*    9. Else set the off peak discount rate(26)  to 0
*
*   Post-Conditions
*    1.  The low and high zones of the current trip have been determined.
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

/*----------------------------------------------------------------------------
** FUNCTION           : testCondition_4_iii
**
** DESCRIPTION        : Function to simplify the body of the rule
**                      Performs a part of the testing for execution 4 iii.
**
**
** INPUTS             : pData               - Context
**
** RETURNS            : True if all of the following conditions are met
**                          for each location in the list of locations
**                              (1) TLow(A) is either in the Low, Actual or Highzone, and
**                              (2) THigh(B) is either in the Low, Actual or HighZone
**
----------------------------------------------------------------------------*/

static int testCondition_4_iii(MYKI_BR_ContextData_t *pData, U8_t TLow, U8_t THigh)
{
    int lindex;

    for (lindex = 0; lindex < pData->DynamicData.transportLocationsCount; lindex++)
    {
        if (!(
              (    TLow == pData->DynamicData.transportLocations[lindex].inner_zone
                || TLow == pData->DynamicData.transportLocations[lindex].zone
                || TLow == pData->DynamicData.transportLocations[lindex].outer_zone
              )
            &&
              (    THigh == pData->DynamicData.transportLocations[lindex].inner_zone
                || THigh == pData->DynamicData.transportLocations[lindex].zone
                || THigh == pData->DynamicData.transportLocations[lindex].outer_zone
               )
            ))
        {
            return FALSE;
        }

    }
   return TRUE;
}

/*----------------------------------------------------------------------------
** FUNCTION           : isOffPeakTrip
**
** DESCRIPTION        : Determine if the current trip is an off peak trip by
**                      examining the CD Differential Pricing Rules
**
**                      Implements the fragnment
**                      Determine if the current trip is an off peak using the :
**                          i.      RuleType is off-peak
**                          ii.     Scan on time,
**                          iii.    Scan off date time,
**                          iv.     Current trip low zone,
**                          v.      Current trip high zone,
**                          vi.     The trip direction,
**
**                      Based on myki_br_getFareStoredValue()
**
** INPUTS             : pData               - Context
**                      passengerCode       - Passenger Code
**                      pRate               - Ref to place to store Rate
**                                            Null not acceptable.
**
** RETURNS            : TRUE                - Is an OffPeak trip
**                                          - Returns data in pData->Tariff
**                      FALSE               - Is Not an Off Peak Trip
**
----------------------------------------------------------------------------*/

static int isOffPeakTrip(MYKI_BR_ContextData_t *pData, MYKI_TAControl_t *pMYKI_TAControl, U8_t passengerCode, int * pRate)
{

    MYKI_CD_DifferentialPricingRequest_t        pricingRequest;
    MYKI_CD_DifferentialPricing_t               pricingResult;
    MYKI_Directory_t                            *pDirectory = NULL;
    MYKI_TAProduct_t                            *pProduct   = NULL;

    //  Init data statructures
    memset( &pricingResult, 0, sizeof( pricingResult) );

    if ( pMYKI_TAControl->ProductInUse == 0 )
        return FALSE;

    if ( myki_br_GetCardProduct( pMYKI_TAControl->ProductInUse, &pDirectory, &pProduct ) < 0 )
        return FALSE;

    //  Create the basic request, then add in fields specific to this operation
    if (myki_br_buildDifferencePriceRequest(
                pData,
                &pricingRequest,
                pData->DynamicData.currentTripZoneLow,
                pData->DynamicData.currentTripZoneHigh,
                passengerCode,
                pData->DynamicData.currentTripDirection,
                pProduct->StartDateTime,
                pData->DynamicData.currentDateTime) < 0)
    {
        //  Error. Details have been reported
        return -1;
    }
    pricingRequest.rule_type = MYKI_CD_DIFFERENTIAL_PRICING_RULE_TYPE_OFFPEAK;

    if ( MYKI_CD_getDifferentialPriceStructure(&pricingRequest, &pricingResult ) == FALSE )
    {
        return FALSE;
    }

    CsDbg
    (
        BRLL_RULE,
        "BR_LLSC_6_20 : isOffPeakTrip : pass = %d, low = %d, high = %d, returned type = %d, discount = %d",
        passengerCode,
        pData->DynamicData.currentTripZoneLow,
        pData->DynamicData.currentTripZoneHigh,
        pricingResult.discount_type,
        pricingResult.applied_discount
    );

    //  Assist in tracing rules
    CsDbg( BRLL_RULE, "BR_LLSC_6_20 : isOffPeakTrip: rule %d (%s), discount = %d", pricingResult.id, pricingResult.short_desc, pricingResult.applied_discount);

    //
    //  The consumer can only handle a discount rate. Thus the only supported
    //  pricing is a Percaentage. Any other type will be ignored and reported
    //
    if (pricingResult.discount_type != MYKI_CD_DISCOUNT_TYPE_PERCENT )
    {
        CsErrx( "BR_LLSC_6_20 : isOffPeakTrip - unsupported discount type");
        return FALSE;
    }

    if (pRate)
    {
        *pRate = pricingResult.applied_discount;
        return TRUE;
    }

    CsErrx( "BR_LLSC_6_20 : isOffPeakTrip - incorrect use. Null Rate pointer");
    return FALSE;
}

/*----------------------------------------------------------------------------
** FUNCTION           : updateNHourProduct
**
** DESCRIPTION        : Perform a part of the Rule that is used more than once
**                      Essentially:
**
**             If this not a reset default fare (Dynamic.IsResetDefaultFlag(28)  = False) And If the product in
**             use(1)  is of type n-Hour perform a ProductUpdate/None transaction for the product in use(1) :
**                  i.  Set the BorderStatus bit of the product control bitmap.
**                  ii. set the BorderSide bit ...
**
**
**
** INPUTS             : pData               - Context
**                      pMYKI_TAControl     - TA Control
**                      borderSide          - Required value of the border side bit
**
** RETURNS            :  1                  - No Error. Condition not met
**                       0                  - No Error. Condition met
**                      -1                  - Error. Details already reported
**
----------------------------------------------------------------------------*/

int updateNHourProduct(MYKI_BR_ContextData_t *pData,  MYKI_TAControl_t *pMYKI_TAControl, BorderStatus_t borderSide )
{
    MYKI_Directory_t    *pDirectory = NULL;
    MYKI_TAProduct_t    *pProduct = NULL;

    //-----------------------------------------------------------------------------
    //  Implementation note: Dynamic.IsResetDefaultflag will always be FALSE
    //  Reason: It is only set in BR_LL_SC_6_25 which is not a part of any sequence
    //          that we currently implement.
    //-----------------------------------------------------------------------------

    if (pMYKI_TAControl->ProductInUse != 0 && MYKI_CS_TAProductGet( pMYKI_TAControl->ProductInUse - 1, &pProduct ) == 0)
    {
        pDirectory = &pMYKI_TAControl->Directory[ pMYKI_TAControl->ProductInUse ];
        if ( myki_br_cd_GetProductType( pDirectory->ProductId ) == PRODUCT_TYPE_NHOUR)
        {
            //  Create a copy of the product so that can modify it
            MYKI_TAProduct_t UpdatedProduct = *pProduct;

            myki_br_setProductBorderStatus ( &UpdatedProduct, TRUE );
            myki_br_SetBitmapBorderStatus( &UpdatedProduct.ControlBitmap, borderSide );

            if ( myki_br_ldt_ProductUpdate( pData, pDirectory, pProduct, &UpdatedProduct ) < 0 )
            {
                CsErrx( "BR_LLSC_6_20 : myki_br_ldt_ProductUpdate() failed" );
                return -1;
            }
            return 0;
        }
    }
    return 1;
}


//=============================================================================
//
//
//
//=============================================================================

RuleResult_e BR_LLSC_6_20( MYKI_BR_ContextData_t *pData )
{
    MYKI_TAControl_t    *pMYKI_TAControl;
    int                 lindex;
    MYKI_TAProduct_t    *pProductInUse      = NULL;
    MYKI_Directory_t    *pDirectoryInUse    = NULL;

    //
    //  Algorithm specified local variables
    //
    U8_t    THigh = 1;                          //  A
    U8_t    TLow = 255;                         //  B
    int     StopCount = 0;                      //  C
    int     BorderStopCountLow = 0;             //  D
    int     BorderStopCountHigh = 0;            //  E
    int     ThinZoneCount = 0;                  //  F
    int     CurrentBorderLowZone = -1;          //  G
    int     CurrentBorderHighZone = -1;         //  H

    CsDbg( BRLL_RULE, "BR_LLSC_6_20 : Start (Resolve trip Zone range)" );

    if ( ! pData )
    {
        CsErrx( "BR_LLSC_6_20 : Invalid argument(s)" );
        return RULE_RESULT_ERROR;
    }

    //-------------------------------------------------------------------------
    //
    //  Pre-conditions
    //
    //-------------------------------------------------------------------------

    // 1.   The list of locations(24) contains 2 or more locations
    if ( pData->DynamicData.transportLocationsCount < 2 )
    {
        CsDbg( BRLL_RULE, "BR_LLSC_6_20 : Bypass - Dynamic.TransportLocations is less than 2 locations." );
        pData->ReturnedData.bypassCode = 1;
        return RULE_RESULT_BYPASSED;
    }

    //
    //  All preconditions satisfied
    //  Indicate this fact to the automated unit test
    //
    pData->ReturnedData.bypassCode = 2;

    //-------------------------------------------------------------------------
    //
    //  Actions
    //
    //-------------------------------------------------------------------------

    if ( MYKI_CS_TAControlGet( &pMYKI_TAControl ) < 0 )
    {
        CsErrx( "BR_LLSC_6_20 : MYKI_CS_TAControlGet() failed" );
        return RULE_RESULT_ERROR;
    }

    /* Get product and directory structure in use */
    if ( pMYKI_TAControl->ProductInUse == 0 )
    {
        CsErrx( "BR_LLSC_6_20 : No product in use" );
        return RULE_RESULT_ERROR;
    }

    if ( myki_br_GetCardProduct( pMYKI_TAControl->ProductInUse, &pDirectoryInUse, &pProductInUse ) < 0 )
    {
        CsErrx( "BR_LLSC_6_12 : myki_br_GetCardProduct(%d) failed", pMYKI_TAControl->ProductInUse );
        return RULE_RESULT_ERROR;
    }

    //  1. For each location in the list of location(24) :
    int firstLowBorderLocationFound = FALSE;
    int firstHighBorderLocationFound = FALSE;

    for (lindex = 0; lindex < pData->DynamicData.transportLocationsCount; lindex++)
    {
        U8_t zone;
        U8_t zoneUnderTest;

        //  a. If THigh(A) is less than smaller of the location.Low Zone, Actual zone, or High Zone then update
        //     Thigh(A) to the smaller of the current location.Low Zone, Actual zone, High Zone
        //
        zoneUnderTest = 255;
        zone = pData->DynamicData.transportLocations[lindex].inner_zone;
        if (zone < zoneUnderTest)
        {
            zoneUnderTest = zone;
        }

        zone = pData->DynamicData.transportLocations[lindex].zone;
        if (zone < zoneUnderTest)
        {
            zoneUnderTest = zone;
        }

        zone = pData->DynamicData.transportLocations[lindex].outer_zone;
        if (zone < zoneUnderTest)
        {
            zoneUnderTest = zone;
        }

        if (THigh < zoneUnderTest)
        {
            THigh = zoneUnderTest;
        }

        //
        //  b. If TLow(B) is greater than larger of the either location.Low Zone, Actual zone, or HighZone then
        //     update TLow(B) to the larger of the either current location.Low Zone, Actual zone, High Zone
        //
        zoneUnderTest = 1;
        zone = pData->DynamicData.transportLocations[lindex].inner_zone;
        if (zone > zoneUnderTest)
        {
            zoneUnderTest = zone;
        }

        zone = pData->DynamicData.transportLocations[lindex].zone;
        if (zone > zoneUnderTest)
        {
            zoneUnderTest = zone;
        }

        zone = pData->DynamicData.transportLocations[lindex].outer_zone;
        if (zone > zoneUnderTest)
        {
            zoneUnderTest = zone;
        }

        if (TLow > zoneUnderTest)
        {
            TLow = zoneUnderTest;
        }

        //
        //  c. If this is not a true location on the low side, i.e low zone is not equal to Actual Zone then
        //      i.  If this is the first low border location found then set the current border low(G) to the
        //          LowZone of the current location
        //      ii. If CurrentBorderZoneLow(G) is equal to the low zone then increment border Low Count(D) by 1
        //
        if (pData->DynamicData.transportLocations[lindex].inner_zone != pData->DynamicData.transportLocations[lindex].zone)
        {
            // i.
            if (!firstLowBorderLocationFound)
            {
                firstLowBorderLocationFound = TRUE;
                CurrentBorderLowZone = pData->DynamicData.transportLocations[lindex].inner_zone;
            }

            //  ii.
            if (CurrentBorderLowZone == pData->DynamicData.transportLocations[lindex].inner_zone)
            {
                BorderStopCountLow++;
            }
        }

        //
        //  d. If this is not a true location on the high side, i.e high zone is not equal to Actual Zone
        //      i.  If this is the first High border location found then set the current border high(H) to the
        //          HighZone of the current location
        //      ii. If CurrentBorderZoneHigh(H) is equal to the highzone then Increment Border High Count(E) by 1
        //
        if (pData->DynamicData.transportLocations[lindex].outer_zone != pData->DynamicData.transportLocations[lindex].zone)
        {
            // i.
            if (!firstHighBorderLocationFound)
            {
                firstHighBorderLocationFound = TRUE;
                CurrentBorderHighZone = pData->DynamicData.transportLocations[lindex].outer_zone;
            }

            //  ii.
            if (CurrentBorderHighZone == pData->DynamicData.transportLocations[lindex].outer_zone)
            {
                BorderStopCountHigh++;
            }
        }

        //
        //  e. If this is a thin zone i.e Low zone(A) +2 is equal to High Zone(B) then increment Thin Zone Count(E) by 1
        //
        if (pData->DynamicData.transportLocations[lindex].inner_zone + 2 == pData->DynamicData.transportLocations[lindex].outer_zone)
        {
            ThinZoneCount++;
        }

        //
        //  f. Increment Stop Count(C) by 1
        //
        StopCount++;
    }

    //  2. If TLow is greater than Thigh then swap TLow and Thigh values
    //
    if (TLow > THigh)
    {
        U8_t    tmp = TLow;
        TLow = THigh;
        THigh = tmp;
    }

//CsDbg( BRLL_RULE, "BR_LLSC_6_20 TLow                 : %d", TLow );
//CsDbg( BRLL_RULE, "BR_LLSC_6_20 THigh                : %d", THigh );
//CsDbg( BRLL_RULE, "BR_LLSC_6_20 StopCount            : %d", StopCount );
//CsDbg( BRLL_RULE, "BR_LLSC_6_20 BorderStopCountLow   : %d", BorderStopCountLow );
//CsDbg( BRLL_RULE, "BR_LLSC_6_20 BorderStopCountHigh  : %d", BorderStopCountHigh );
//CsDbg( BRLL_RULE, "BR_LLSC_6_20 ThinZoneCount        : %d", ThinZoneCount );
//CsDbg( BRLL_RULE, "BR_LLSC_6_20 CurrentBorderLowZone : %d", CurrentBorderLowZone );
//CsDbg( BRLL_RULE, "BR_LLSC_6_20 CurrentBorderHighZone: %d", CurrentBorderHighZone );

    //
    //  3. If all locations of the traversal are in the same “thin zone”, i.e. the difference between the inner and
    //     the outer zone is exactly 2 (i.e ThinZone count(E) equals Stop countC) then:
    //
    if (ThinZoneCount == StopCount)
    {
        pData->ReturnedData.bypassCode = 3;
        //      a. Set the TLow and Thigh to the actual Zone (i.e set TLow(A) and Thigh(B) to TLow + 1)
        //
        TLow = TLow + 1;
        THigh = TLow;

        //      b. If the product in use(1) is of type n-Hour and if the Dynamic.IsResetDefaultflag = false then
        //          i. Perform a ProductUpdate/None transaction for the product in use(1)
        //              (1) Set the BorderStatus bit of the product control bitmap to True.
        //              (2) Set the BorderSide bit of the product control bitmap to Inbound.
        //

        if (updateNHourProduct(pData,pMYKI_TAControl, BORDER_STATUS_INBOUND) < 0)
        {
            //  Error already reported
            return RULE_RESULT_ERROR;
        }

        //      c. Set the Dynamic.CurrentTripIsThinZone(11) to be true
        //
        pData->DynamicData.currentTripIsThinZone = TRUE;

        //  d. Set Current Trip Zone Low(4) to TLow(A)  and Current Trip Zone High(5) to THigh(B)
        //
        pData->DynamicData.currentTripZoneLow = TLow;
        pData->DynamicData.currentTripZoneHigh = THigh;
    }

        //  4. Else, If all locations of the traversal are border stops along the same border I.E either of the
        //     following is true.
        //          i. The determined trip zone low(A) plus 1 is equal to the determined trip zone high(B) and
        //              BorderCountLow(D) is equal to the stop count(C); Or
        //          ii. The determined trip zone low(A) plus 1 is equal to the determined trip zone high(B) and
        //              BorderCountHigh(E) is equal to he stop countC.
        //          iii. If TLow(A) +1 = THIgh(B) and all of the following condition is true for each location in the list of
        //              locations
        //                  (1) TLow(A) is either in the Low, Actual or Highzone, and
        //                  (2) THigh(B) is either in the Low, Actual or HighZone
        //
    else if (    (TLow + 1 == THigh && BorderStopCountLow  == StopCount)
         || (TLow + 1 == THigh && BorderStopCountHigh == StopCount)
         || (TLow + 1 == THigh && testCondition_4_iii(pData, TLow, THigh))
        )
    {
        Currency_t  fareLow;
        Currency_t  fareHigh;
        pData->ReturnedData.bypassCode = 4;

        //      b. then:
        //      c. Determine the applicable fare for:
        //          i.   The border low zone (TLow(A)).
        //          ii.  Passenger type(9) .
        //          iii. Determined fare route(23)
        //          iv.  Current day of the week/date(8) .
        //          v.   Current time of day(8) .
        if
        (
            myki_br_getFareStoredValueEx
            (
                &fareLow,
                pData,
                TLow,
                TLow,
                pMYKI_TAControl->PassengerCode,
                pData->DynamicData.currentTripDirection,
                pData->DynamicData.fareRouteIdIsValid,
                pData->DynamicData.lineId,
                pProductInUse->StartDateTime,
                pData->DynamicData.currentDateTime
            ) < 0
        )
        {
            CsErrx( "BR_LLSC_6_20 : myki_br_getFareStoredValueEx(TLow) failed" );
            return RULE_RESULT_ERROR;
        }

        //      d. Determine the applicable fare for:
        //          i. The border high zone (THigh(B)).
        //          ii. Passenger type(9) .
        //          iii. Determined fare route(23)
        //          iv. Current day of the week/date(8) .
        //          ii. Current time of day(8)
        //
        if
        (
            myki_br_getFareStoredValueEx
            (
                &fareHigh,
                pData,
                THigh,
                THigh,
                pMYKI_TAControl->PassengerCode,
                pData->DynamicData.currentTripDirection,
                pData->DynamicData.fareRouteIdIsValid,
                pData->DynamicData.lineId,
                pProductInUse->StartDateTime,
                pData->DynamicData.currentDateTime
            ) < 0
        )
        {
            CsErrx( "BR_LLSC_6_20 : myki_br_getFareStoredValueEx(THigh) failed" );
            return RULE_RESULT_ERROR;
        }

        //      Note: Section 4.e. Has been clarified since NTS0177 V7.2. The text below is the new text
        //                         Also affects BR_LLSC_6_13
        //  e. If the applicable fare for the border low zone was lower than or equal to the applicable fare for
        //     the border high zone set then set the Current trip low zone(4)  and the current trip high zone(5) to
        //     the border low zone; else   set the current trip low zone(4) and the current trip high
        //     zone(5) to the border high zone.
        //
        if (fareLow <= fareHigh)
        {
            pData->DynamicData.currentTripZoneLow = TLow;
            pData->DynamicData.currentTripZoneHigh = TLow;
        }
        else
        {
            pData->DynamicData.currentTripZoneLow = THigh;
            pData->DynamicData.currentTripZoneHigh = THigh;
        }
        CsDbg( BRLL_RULE, "BR_LLSC_6_20 : Setting currentTripZoneLow and currentTripZoneHigh to same value: %d", pData->DynamicData.currentTripZoneHigh );

        //      f. If this not a reset default fare (Dynamic.IsResetDefaultFlag(28)  = False) And If the product in
        //         use(1)  is of type n-Hour perform a ProductUpdate/None transaction for the product in use(1) :
        //              i.  Set the BorderStatus bit of the product control bitmap.
        //              ii. If the applicable fare for the border low zone was lower than or equal to the applicable
        //                  fare for the border high zone, set the BorderSide bit of the product control bitmap to 1,
        //                  else set the BorderSide bit to 0.
        //
        if (updateNHourProduct(pData,pMYKI_TAControl, fareLow <= fareHigh ? BORDER_STATUS_OUTBOUND : BORDER_STATUS_INBOUND) < 0)
        {
            //  Error already reported
            return RULE_RESULT_ERROR;
        }
    }
    else
    {

        //  5. Else
        //     a. Set the low zone of the current trip(4) to the l low zone(A) and set the high zone of the
        //        current trip(5) to the high zone(B).
        //
        pData->DynamicData.currentTripZoneLow = TLow;
        pData->DynamicData.currentTripZoneHigh = THigh;
    }
    CsDbg( BRLL_RULE, "BR_LLSC_6_20 currentTripZoneLow: %d, currentTripZoneHigh: %d", pData->DynamicData.currentTripZoneLow, pData->DynamicData.currentTripZoneHigh );

    //
    //  6. Clear the list of locations(24)
    //
    pData->DynamicData.transportLocationsCount = 0;

    //
    //  7. If the current trip is either:
    //      a. A force scan off trip(25)  then set the off peak discount rate(26)  to zero (0)
    //      b. Or mode of transport of the current device is not equal to rail and the mode of transport of the
    //         scan on service provider is equal Rail
    //      c. Then set the off peak discount rate26 to 0
    //
    //  Paraphrase :
    //      If either :
    //      -   The forced scan-off flag is already set, or
    //      -   The current device mode of transport *is not* RAIL and the scan-on mode of transport *is* RAIL
    //      Then set the offpeak discount rate to (0).
    //

    if
    (
        ( pData->DynamicData.isForcedScanOff ) ||
        (
            ( pData->InternalData.TransportMode != TRANSPORT_MODE_RAIL ) &&
            ( myki_br_cd_GetTransportModeForProvider( pProductInUse->LastUsage.ProviderId ) == TRANSPORT_MODE_RAIL )
        )
    )
    {
        //      c. Then set the off peak discount rate(26)  to 0
        //
        pData->DynamicData.offPeakDiscountRate = 0;
        pData->ReturnedData.bypassCode += 100;
        CsDbg( BRLL_RULE, "BR_LLSC_6_20 Forced scan off. Rate: 0" );
    }

    //  8. Else determine if the current trip is an off peak using the :
    //      a.
    //          i.   RuleType is off-peak
    //          ii.  Scan on time,
    //          iii. Scan off date time,
    //          iv.  Current trip low zone,
    //          v.   Current trip high zone,
    //          vi.  The trip direction,
    //
    else if (isOffPeakTrip(pData, pMYKI_TAControl, pMYKI_TAControl->PassengerCode, &pData->DynamicData.offPeakDiscountRate) )
    {
        //
        //      b. Set the off peak set the off peak rate to the determined off peak discount rate(26) .
        //      *** Done as a part of isOffPeakTrip

        //
        //      c. Set Dynamic.IsOffPeak(27)  = True
        //
        pData->DynamicData.isOffPeak = TRUE;
        pData->ReturnedData.bypassCode += 200;
        CsDbg( BRLL_RULE, "BR_LLSC_6_20 Off Peak. Rate: %d",pData->DynamicData.offPeakDiscountRate );
    }

    //
    //  9. Else set the off peak discount rate(26)  to 0
    //
    else
    {
        pData->DynamicData.offPeakDiscountRate = 0;
        pData->ReturnedData.bypassCode += 300;
        CsDbg( BRLL_RULE, "BR_LLSC_6_20 Not off Peak Discount Rate: 0");
    }

    CsDbg( BRLL_RULE, "BR_LLSC_6_20 : Executed" );
    return RULE_RESULT_EXECUTED;
}


