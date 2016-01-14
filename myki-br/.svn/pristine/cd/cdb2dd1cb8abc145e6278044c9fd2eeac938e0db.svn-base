/**************************************************************************
*   ID :  BR_LLSC_6_13
*
*    Process border Trip - with no coverage
*
*    1.  TAppControl.ProductInUse
*    2.  TAppTProduct.ControlBitmap
*    3.  TAppTProduct.ZoneLow
*    4.  TAppTProduct.ZoneHigh
*    5.  Dynamic.CurrentTripZoneLow
*    6.  Dynamic.CurrentTripZoneHigh
*    7.  Dynamic.CurrentTrip.IsThinZone
*    8.  Dynamic.ProposedMergezone.Low
*    9.  Dynamic.ProposedMergezone.High
*    10.  Dynamic.ProposedMergeFare
*    11.  Dynamic.ProposedMergeDirection
*    12.  Tariff.ApplicableFare
*    13.  Dynamic.CappingContribution
*    14.  Dynamic.FareRouteID
*
*
*   Pre-Conditions - With clarification from Ashish (14-Jan-2014)
*    1.  The product in use(1)  border status control bitmap has a border status of 1
*    2.  If the current trip low zone(5) is less than the low zone of the proposed merge zone range(8) 'OR'
*    3.  If the current trip high zone(6) is greater than the high zone of the proposed merge zone range(9)
*
*   Description
*    1.  Calculate the fare for the
*       a.  actual zone current trip using:
*          i.  The current trip low zone(5)  and the current trip high zone(6)
*          ii.  Passenger type
*          iii.  Determined fare route(14)
*          iv.  Current date of the time
*       b.  If the current trip is a thin zone ie Dynamic.CurrentTrip.IsThinZone(7)  is true then
*          i.  Determine which fare is the lowest fare; for inner, actual, outer zones fares covered by the
*              thin zone.
*          ii.  Calculate the fare for the outer zone using:
*             (1)  Current trip low zone(5)  plus 1 and Current trip high zone(6)  plus 1 for the Low and High
*                  zones.
*             (2)  Passenger type
*             (3)  Determined fare route(14)
*             (4)  Current date time
*          iii.  Calculate the fare for the low zone using:
*             (1)  Current trip low zone(5)  less 1 and Current trip high zone(6)  minus 1 for the Low and High
*                  zones.
*             (2)  Passenger type
*             (3)  Determined fare route(14)
*             (4)  Current date time
*          iv.  Increment the applicable fare(12)  by the lesser of the 3 fares.
*          v.  Increment the capping contribution(13)   by the lesser of the 3 fares
*          vi.  Perform a ProductSale/Upgrade transaction for the product in(1)  use with:
*              (1)  set the purchase value as cheapest of the 3 fares
*              (2)  Clear the provisional(2)  bit on the current product
*
*        Note: Section c.iii. Has been clarified since NTS0177 V7.2. The text below is the new text
*                             Uses values set in BR_LLSC_6_20
*       c.  Else;
*           i.  Increment the applicable fare(12)  by the fare for the current trip
*           ii.  Increment the capping contribution(13)  by the fare for the current trip
*           iii.  Perform a product ProductSale/Upgrade for the product in use(1)
*               (1)  Clear the provisional bit(2)  of the product control bit map
*               (2)  Set the purchase value of the product to the value of the fare calculated in step 1
*               (3)  Current Trip zone Low(5)
*               (4)  Current Trip Zone High(6)
*
*   Post Condition
*    1.  Provisional Product is updated to n-Hour Product
*
*   Devices
*      Fare payment devices
*
 ***********************************************************************/

#include <cs.h>                 // For debug logging
#include <myki_cardservices.h>  // Myki Card Services
#include <LDT.h>                // Logical Device Transaction
#include <myki_cd.h>            // myki_cd (Myki Configuration Data)
#include "myki_br_rules.h"      // API for all BR_xxx modules
#include "BR_Common.h"          // Common utilities, macros, etc

#define BR_LLSC_MIN(a,b,c) ((((a < b)?(a):(b))< c)?((a < b)?(a):(b)):(c))

//=============================================================================
//
//
//
//=============================================================================

RuleResult_e BR_LLSC_6_13( MYKI_BR_ContextData_t *pData )
{
    MYKI_TAControl_t    *pMYKI_TAControl;
    MYKI_TAProduct_t    *pProductInUse = NULL;
    MYKI_Directory_t    *pDirectoryInUse = NULL;

    // KIM REMOVE
    //MYKI_CD_PassengerType_t     passenger;
    //MYKI_CD_FaresProvisional_t  faresProvisional;
    //U32_t                       actualZoneFare;
    // END KIM REMOVE
    
    // KIM ADD
    Currency_t          actualZoneFare = 0;
    // END KIM ADD
    
    int                         upgradeZoneHigh, upgradeZoneLow;

    CsDbg( BRLL_RULE, "BR_LLSC_6_13 : Start (Process border Trip - with no coverage)" );

    if ( ! pData )
    {
        CsErrx( "BR_LLSC_6_13 : Invalid argument(s)" );
        return RULE_RESULT_ERROR;
    }

    if ( MYKI_CS_TAControlGet( &pMYKI_TAControl ) < 0 )
    {
        CsErrx( "BR_LLSC_6_13 : MYKI_CS_TAControlGet() failed" );
        return RULE_RESULT_ERROR;
    }

    /* Pre-Conditions */
    /* 1.  The product in use(1)  border status control bitmap has a border status of 1 */
    if ( pMYKI_TAControl->ProductInUse == 0 )
    {
        CsDbg( BRLL_RULE, "BR_LLSC_6_13 : BYPASSED : ProductInUse is zero");
        return RULE_RESULT_BYPASSED;
    }

    /* Get product and directory structure in use */
    if ( MYKI_CS_TAProductGet( (U8_t)( pMYKI_TAControl->ProductInUse - 1 ), &pProductInUse ) < 0 )
    {
        CsErrx( "BR_LLSC_6_13 : MYKI_CS_TAProductGet() failed" );
        return RULE_RESULT_ERROR;
    }

    pDirectoryInUse = &pMYKI_TAControl->Directory[ pMYKI_TAControl->ProductInUse ];

    if ( ! (pProductInUse->ControlBitmap & TAPP_TPRODUCT_CONTROL_BORDER_STATUS_BITMAP) )
    {
        CsDbg( BRLL_RULE, "BR_LLSC_6_13 : BYPASSED : Border Status Bit is false");
        return RULE_RESULT_BYPASSED;
    }

    //   With clarification from Ashish (14-Jan-2014)
    //   2.  If the current trip low zone(5) is less than the low zone of the proposed merge zone range(8) 'OR'
    //   3.  If the current trip high zone(6) is greater than the high zone of the proposed merge zone range(9)
    //
    if ( !(    pData->DynamicData.currentTripZoneLow  < pData->InternalData.ProposedMerge.ZoneLow
            || pData->DynamicData.currentTripZoneHigh > pData->InternalData.ProposedMerge.ZoneHigh) )
    {
        CsDbg( BRLL_RULE, "BR_LLSC_6_13 : BYPASSED : Some coverage detected");
        return RULE_RESULT_BYPASSED;
    }

    /* 1. Calculate the fare */

    /* 1.a Calculate the fare for the actual zone */

    // KIM REMOVE
    /*
     if ( ! MYKI_CD_getPassengerTypeStructure( pMYKI_TAControl->PassengerCode, &passenger ) )
     {
     CsErrx( "BR_LLSC_6_13 : getPassengerTypeStructure() failed" );
     return RULE_RESULT_ERROR;
     }
     
    if ( ! MYKI_CD_getFaresProvisionalStructure( pData->DynamicData.currentTripZoneLow, pData->DynamicData.currentTripZoneHigh, &faresProvisional ) )
    {
        CsErrx( "BR_LLSC_6_13 : getFaresProvisionalStructure() failed" );
        return RULE_RESULT_ERROR;
    }

    if ( passenger.percent == 0 )
        actualZoneFare = faresProvisional.sv_full;
    else
        actualZoneFare = faresProvisional.sv_conc;
     */
    // END KIM REMOVE

    // KIM ADD
    if
        (
         myki_br_getFareStoredValueEx
         (
          &actualZoneFare,
          pData,
          pData->DynamicData.currentTripZoneLow,
          pData->DynamicData.currentTripZoneHigh,
          pMYKI_TAControl->PassengerCode,
          TRIP_DIRECTION_UNKNOWN,
          pData->DynamicData.fareRouteIdIsValid,
          pData->DynamicData.fareRouteId,
          pProductInUse->StartDateTime,
          pData->DynamicData.currentDateTime
          ) < 0
         )
    {
        CsErrx( "BR_LLSC_6_15 : myki_br_getFareStoredValueEx() for actual zone failed" );
        return RULE_RESULT_ERROR;
    }
    // END KIM ADD
    
    //
    //  1.b If the current trip is a thin zone ie Dynamic.CurrentTrip.IsThinZone(7)  is true then
    //
    if (  pData->DynamicData.currentTripIsThinZone )
    {
        //  Determine which fare is the lowest fare; for inner, actual, outer zones fares covered by the thin zone.
        // KIM REMOVE
        //U32_t  outerZoneFare;
        //U32_t  innerZoneFare;
        //U32_t  fare;
        // END KIM REMOVE
        // KIM ADD
        Currency_t outerZoneFare;
        Currency_t innerZoneFare;
        Currency_t fare;
        // END KIM ADD

        //  ii.  Calculate the fare for the outer zone using:
        //     (1)  Current trip low zone(5)  plus 1 and Current trip high zone(6)  plus 1 for the Low and High zones.
        //     (2)  Passenger type
        //     (3)  Determined fare route(14)
        //     (4)  Current date time
        //
        
        // KIM REMOVE
        /*
        if ( ! MYKI_CD_getFaresProvisionalStructure(
                        (U8_t)( pData->DynamicData.currentTripZoneLow + 1 ),
                        (U8_t)( pData->DynamicData.currentTripZoneHigh + 1 ),
                        &faresProvisional ) )
        {
            CsErrx( "BR_LLSC_6_13 : getFaresProvisionalStructure() failed" );
            return RULE_RESULT_ERROR;
        }

        if ( passenger.percent == 0 )
            outerZoneFare = faresProvisional.sv_full;
        else
            outerZoneFare = faresProvisional.sv_conc;
        */
        // END KIM REMOVE

        // KIM ADD
        if
            (
             myki_br_getFareStoredValueEx
             (
              &outerZoneFare,
              pData,
              pData->DynamicData.currentTripZoneLow + 1,
              pData->DynamicData.currentTripZoneHigh + 1,
              pMYKI_TAControl->PassengerCode,
              TRIP_DIRECTION_UNKNOWN,
              pData->DynamicData.fareRouteIdIsValid,
              pData->DynamicData.fareRouteId,
              pProductInUse->StartDateTime,
              pData->DynamicData.currentDateTime
              ) < 0
             )
        {
            CsErrx( "BR_LLSC_6_15 : myki_br_getFareStoredValueEx() for outer zone failed" );
            return RULE_RESULT_ERROR;
        }
        
        // END KIM ADD

        //  iii.  Calculate the fare for the low zone using:
        //     (1)  Current trip low zone(5)  less 1 and Current trip high zone(6)  minus 1 for the Low and High zones.
        //     (2)  Passenger type
        //     (3)  Determined fare route(14)
        //     (4)  Current date time
        //
        
        // KIM REMOVE
        /*
        if ( ! MYKI_CD_getFaresProvisionalStructure(
                        (U8_t)( pData->DynamicData.currentTripZoneLow - 1 ),
                        (U8_t)( pData->DynamicData.currentTripZoneHigh - 1 ),
                        &faresProvisional ) )
        {
            CsErrx( "BR_LLSC_6_13 : getFaresProvisionalStructure() failed" );
            return RULE_RESULT_ERROR;
        }

        if ( passenger.percent == 0 )
            innerZoneFare = faresProvisional.sv_full;
        else
            innerZoneFare = faresProvisional.sv_conc;
         */
        // END KIM REMOVE
        
        // KIM ADD
        if
            (
             myki_br_getFareStoredValueEx
             (
              &innerZoneFare,
              pData,
              pData->DynamicData.currentTripZoneLow - 1,
              pData->DynamicData.currentTripZoneHigh - 1,
              pMYKI_TAControl->PassengerCode,
              TRIP_DIRECTION_UNKNOWN,
              pData->DynamicData.fareRouteIdIsValid,
              pData->DynamicData.fareRouteId,
              pProductInUse->StartDateTime,
              pData->DynamicData.currentDateTime
              ) < 0
             )
        {
            CsErrx( "BR_LLSC_6_15 : myki_br_getFareStoredValueEx() for inner zone failed" );
            return RULE_RESULT_ERROR;
        }
        // END KIM ADD
        
        //
        //  iv.  Increment the applicable fare(12)  by the lesser of the 3 fares.
        //  v.   Increment the capping contribution(13)   by the lesser of the 3 fares
        //
        fare = BR_LLSC_MIN(actualZoneFare, innerZoneFare, outerZoneFare);

        CsDbg( BRLL_FIELD, "BR_LLSC_6_13 : Increasing applicable fare by %d from %d to %d", fare, pData->ReturnedData.applicableFare, pData->ReturnedData.applicableFare + fare );
        pData->ReturnedData.applicableFare += fare;

        CsDbg( BRLL_FIELD, "BR_LLSC_6_13 : Increasing capping contribution by %d from %d to %d", fare, pData->DynamicData.cappingContribution, pData->DynamicData.cappingContribution + fare );
        pData->DynamicData.cappingContribution += fare;

        //  vi.  Perform a ProductSale/Upgrade transaction for the product in(1)  use with:
        //      (1) set the purchase value as cheapest of the 3 fares
        //      (2) Clear the provisional(2)  bit on the current product

        //      *** UPDATED SPECIFICATION
        //      (3) Current Trip zone Low(5)  (as setup by 6_20)
        //      (4) Current Trip Zone High(6) (as setup by 6_20)

        //
        //  Operation combined with 'c' below
        //  Set up values to be used
        //
        actualZoneFare = fare;

        //  
        //
        upgradeZoneHigh = pData->DynamicData.currentTripZoneHigh;
        upgradeZoneLow = pData->DynamicData.currentTripZoneLow;
    }
    else
    {
        //  c.  Else;
        //      i.  Increment the applicable fare(12)  by the fare for the current trip
        //      ii.  Increment the capping contribution(13)  by the fare for the current trip
        //      iii.  Perform a product ProductSale/Upgrade for the product in use(1)
        //          (1)  Clear the provisional bit(2)  of the product control bit map
        //          (2)  Set the purchase value of the product to the value of the fare calculated in step 1
        //          (3)  Current Trip zone Low(5)  (as setup by 6_20)
        //          (4)  Current Trip Zone High(6) (as setup by 6_20)
        //
        //  Operation combined with 'b.vi' above
        //  Set up values to be used

        pData->ReturnedData.applicableFare += actualZoneFare;
        pData->DynamicData.cappingContribution += actualZoneFare;

        CsDbg( BRLL_RULE, "BR_LLSC_6_13 : Use values setup in BR_LLSC_6_20");
        upgradeZoneHigh = pData->DynamicData.currentTripZoneHigh;
        upgradeZoneLow = pData->DynamicData.currentTripZoneLow;
    }

    //
    //  Perform a ProductSale/Upgrade transaction
    //  as per 'b' and 'c'
    //
    pProductInUse->ControlBitmap &= (~TAPP_TPRODUCT_CONTROL_PROVISIONAL_BITMAP);

    if ( myki_br_ldt_ProductSale_Upgrade_SameProduct( pData,
                                                      pDirectoryInUse,
                                                      upgradeZoneLow,
                                                      upgradeZoneHigh,
                                                      actualZoneFare,
                                                      pProductInUse->EndDateTime ) < 0 )
    {
        CsErrx( "BR_LLSC_6_13 : myki_br_ldt_ProductSale_Upgrade_SameProduct() failed" );
        return RULE_RESULT_ERROR;
    }


    CsDbg( BRLL_RULE, "BR_LLSC_6_13 : EXECUTED" );
    return RULE_RESULT_EXECUTED;
}

