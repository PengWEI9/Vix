/* -*- mode: c; tabs: 4; -*- */
/*=============================================================================
**
**    Vix Technology                   Licensed software
**    (C) 2013                         All rights reserved
**
**=============================================================================
**
**  Project/Product : MBU
**  Filename        : BR_LLSC_6_12.c
**  Author(s)       : ?
**
**  ID              : BR_LLSC_6_12 - KA0004 v7.0
**
**  Name            : Process Border Trip with Full Product Coverage
**
**  Data Fields     :
**
**      1.  TAppControl.ProductInUse
**      2.  TAppTProduct.ControlBitmap
**      3.  TAppTProduct.ZoneLow
**      4.  TAppTProduct.ZoneHigh
**      5.  Dynamic.CurrentTripZoneLow
**      6.  Dynamic.CurrentTripZoneHigh
**      7.  Tariff.CitySaverZoneFlag
**      8.  Dynamic.ProposedMergezone.Low
**      9.  Dynamic.ProposedMergezone.High
**      10. Dynamic.CurrentTrip.IsThinZone
**      11. Dynamic.ProposedMergeFare
**      12. Dynamic.ProposedMergeDirection
**      13. TAppControl.Directory.Status
**      14. Dynamic.CappingContribution
**      15. Dynamic.FareRouteID
**      16. TAppControl.PassengerCode
**      17. TAppTProduct.StartDateTime
**      18. Dynamic.CurrentTripDirection
**      19. Dynamic.CurrenDateTime
**      20. Dynamic.Usagelog
**
**  Pre-Conditions  :
**
**      1.  The Border Status Bit of  the product control bit map(2)  of the product in use(1)  is true
**      2.  At least 1 other product with product control status of Active or Issued (excluding the product in use )
**          which covers the existing trip zone range(5),(6) such that:
**          a.  If the current trip zone low(5)  is less than or equal to the product zone high(4)  and the current trip
**              zone low(5)  is greater than or equal to the product zone low(3)
**          b.  If the Border status bit of the product control bitmap(2)  for the product in use(1)  is out-bound and the
**              product low zone minus 1 is equal to the current trip zone low(5)
**          c.  If the Border status bit of the product control bitmap(2)   for the product in use(1)  is in-bound and the
**              product high(4)  zone plus 1 is equal the current trip zone low(5)
**          d.  If the Current trip is a thin zone(10)  and the product low zone(4)  minus 1 is equal to the current trip
**              zone low(5)
**          e.  The product is a border product (border status bit on the product control bitmap(2)  is true) such that
**              the low zone of the border product is equal to the low zone of the current trip and further, the
**              border side bit of the product control bitmap of the product is equal to the border side bit of the
**              product in use (ie the current trip)
**
**  Description     :
**
**      1.  If the border status bit(2)  on the product in use(1)  is inbound then increase the combined zone range
**          high(9)  zone by 1
**          If the border status bit(2)  on the product in use(1)  is outbound then decrease the combined zone range
**          low(8)  zone by 1
**      2.  Check to see if an existing product covers the zones of the current border trip.
**          a.  For each product on the card that has product control status(13)  of Activated and (the Provisional
**              bit of the product control bitmap(2)  is not set .
**              i.  If any of the following is true:
**                  (1) If the current trip zone low(5)  is less than or equal to the product zone high(4)  and the
**                      current trip zone low(5)  is greater than or equal to the product zone low(3)  OR
**
**                  <UPDATED_SPECIFICATION JIRA_ID="MBURFI-38">
**                  (2) The border side(2) bit of product control bitmap for the current trip is set out-bound and
**                      the product Low zone minus 1 covers the current trip zone low(5) then set Trip Zone Low and Trip Zone High to Product Zone Low OR
**                  (3) The border side(2) bit of product control bitmap for the current trip is set to in-bound and
**                      the product zone high plus 1 covers the current trip low zone(5) then set Trip Zone Low and Trip Zone High to Product Zone High OR
**                  (4) The current trip is Thin Zone trip (ie Dynamic.CurrentTrip.IsThinZone(10) is true) and
**                      the product Low Zone minus 1 covers the current trip low zone(5) then set Trip Zone Low and Trip Zone High to Product Zone Low OR
**                  </UPDATED_SPECIFICATION>
**
**                  <ORIGINAL_SPECIFICATION>
**                  (2) The border side(2)  bit of product control bitmap for the current trip is set out-bound and the
**                      product high zone or the product high zone plus 1 covers the current trip zone low(5)  OR
**                  (3) The border side(2)  bit of product control bitmap for the current trip is set to in-bound and
**                      the product zone high or product zone high minus 1 covers the current trip low zone(5)  OR
**                  (4) The current trip is  Thin Zone trip (ie Dynamic.CurrentTrip.IsThinZone(10)  is true) and the
**                      product zone high or product high minus 1 covers the current trip low zone(5)
**                  </ORIGINAL_SPECIFICATION>
**
**                  (5) The product is a border product (border status bit on the product control bitmap(2) is true)
**                      such that the low zone of the border product is equal to the low zone of the current trip
**                      and further, the border side bit of the product control bitmap of the product is equal to
**                      the border side bit of the product in use (ie the current trip)
**              ii. If the proposed trip zone range(8) ,(9) still contain the seed values (ie
**                  Dynamic.PropsoedMergeZoneLow = 255 and ProposedMergeTripZone High = 0)
**                  (1) then
**                      (a) Set the proposed merge zone low(8)  to the trip zone low(5) , and the proposed merge
**                          zone high(9)  to the trip zone high(6) .
**                  (2) Else
**                      (a) if the current trip zone low(5)  is less than proposed merge zone low(8)  then set the
**                          proposed merge zone low(8)  to the trip zone low(5)
**                      (b) if the current trip zone high(9)  is greater than the proposed merge zone high(9)  then set
**                          the proposed merge zone high(9)  to the current trip zone high(6)
**                  (3) Reset the combined zone range fare(11)  to the fare determined for the:
**                      (a) Proposed merge zone range(8) ,(9)
**                      (b) passenger type(16)
**                      (c) date of travel(17)
**                      (d) fare route id(15)
**                      (e) current trip direction(18)
**                  (4) Set the combined zone range off peak fare to the combined zone range fare multiplied
**                      by 1 less the off peak discount rate.
**              iii.Perform a ProductUpdate/Invalidate on the product in use(1)  (which is the provisional product)
**              iv. Perform TAppUpdate/SetProductInUse to mark the product that covers the trip as the as the
**                  product in use(1)
**      3.  else;
**          a.  If there is no active e-Pass product and if a inactive e-Pass product exists and the e-Pass start
**              date time(17)  is less  than the start time of the product in use(1)  (which is the provisional) where the
**              inactive e-Pass covers such as either of the following are true:
**              i.  The border side(2)  bit of product control bitmap for the current trip is set out bound and the
**                  inactive e-Pass product high zone or the product high zone plus 1 covers the current trip
**                  zone low(5)  OR
**              ii. The border side(2)  bit of product control bitmap for the current trip is set to inbound and the
**                  inactive e-Pass product zone high or product zone high minus 1 covers the current trip low
**                  zone(5)  OR
**              iii.The current trip is a Thin Zone trip (ie Dynamic.CurrentTrip.IsThinZone(10)  is true) and the
**                  inactive e-Pass product zone high or product high minus 1 covers the current trip low zone(5)
**          b.  Then:
**              i.  Activate the Inactive e-Pass by performing an ProductUpdate/Activate transaction
**              ii. Update the low zone of the combined zone range(8)  if the e-Pass product zone low(3)  is less
**                  than low zone of the combined zone range(8)
**              iii.Update the high zone of the combined zone range(9)  if the e-Pass product zone high(4)  is
**                  greater than the high zone of the combined zone range(9)
**              iv. Update the combined zone range fare(11)  to the fare determined for the:
**                  (1) combined zone range(8) ,(9)
**                  (2) passenger type(16)
**                  (3) date of travel(17)
**                  (4) fare route id(15)
**                  (5) current trip direction(18)
**              v.  Set the combined zone range off peak fare to the combined zone range fare multiplied by 1
**                  less the off peak discount rate.
**              vi. Perform a product update/invalidate on the product in use(1)
**              vii.Modify usage log
**                  (1) Definition:
**                  (2) Set Usage - Product validation status Activated = bit 0 = true
**              viii.Mark the e-Pass product as the product in use using a TAppUpdate/SetProductInUse
**
**  Post-Conditions :
**
**  Notes           :
**      By Definition a thin zone trip is only in 1 zone.
**
**      These rules also will be successful if a thin zone is present.
**      Is the border trip out adjacent to an existing
**      product such that the border side direction is
**      on the same side as the adjacent product zone
**      then we are fully covered.
**
**      With thin zones we mark the product in bound
**      so only 1 condition to test for
**
**  Member(s)       :
**      BR_LLSC_6_12            [public]    business rule
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
**    1.01  24.01.14    ANT   Modify   MBU-661: Rectified invalidating wrong
**                                     product causing BR to abort.
**                                     MBURFI-38: Implemented updated
**                                     BR_LLSC_6_12 specification
**    1.02  06.02.14    ANT   Modify   Rectified incorrect implementation and
**                                     mimic KAMCO reader
**    1.03  17.09.14    ANT   Modify   MBU-1207: Amended searching scan off
**                                     product from first product in directory
**
**===========================================================================*/

#include <cs.h>                 // For debug logging
#include <myki_cardservices.h>  // Myki Card Services
#include <LDT.h>                // Logical Device Transaction

#include "myki_br_rules.h"      // API for all BR_xxx modules
#include "BR_Common.h"          // Common utilities, macros, etc

//=============================================================================
//
//  Check if an active E-Pass product exists
//
//  Input:
//      pMYKI_TAControl    pointer to MYKI_TAControl_t
//
//
//  Returns:
//      TRUE
//      FALSE
//
//=============================================================================

static U8_t isActiveEPassExist( MYKI_TAControl_t    *pMYKI_TAControl )
{
    U8_t dir;
    MYKI_Directory_t    *pDirectory = NULL;

    if ( ! pMYKI_TAControl )
        return FALSE;

    for ( dir = 1; dir < DIMOF( pMYKI_TAControl->Directory ); dir++ ) // T-Purse dir entry does not have a product slot, so there's one less product slots than dir entries
    {
        pDirectory = &pMYKI_TAControl->Directory[ dir ]; // Product slot 0-4 maps to dir entry 1-5

        if ( pDirectory->Status == TAPP_CONTROL_DIRECTORY_STATUS_ACTIVATED &&
             myki_br_cd_GetProductType( pDirectory->ProductId ) == PRODUCT_TYPE_EPASS )
        {
            return TRUE;
        }
    }

    return FALSE;
}

/*==========================================================================*
**
**  BR_LLSC_6_12
**
**  Description     :
**      Implements business rule BR_LLSC_6_12.
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

RuleResult_e BR_LLSC_6_12( MYKI_BR_ContextData_t *pData )
{
    MYKI_TAControl_t    *pMYKI_TAControl    = NULL;
    U8_t                 dir                = 0;
    MYKI_TAProduct_t    *pProductInUse      = NULL;
    MYKI_Directory_t    *pDirectoryInUse    = NULL;
//    MYKI_Directory_t    *pDirectoryInUseOld = NULL;
//    U8_t                 foundProduct= FALSE;
    MYKI_Directory_t    *pDirectory = NULL;
    MYKI_TAProduct_t    *pProduct = NULL;


    CsDbg( BRLL_RULE, "BR_LLSC_6_12 : Start (Process Border Trip with Full Product Coverage)" );

    if ( ! pData )
    {
        CsErrx( "BR_LLSC_6_12 : Invalid argument(s)" );
        return RULE_RESULT_ERROR;
    }

    if ( MYKI_CS_TAControlGet( &pMYKI_TAControl ) < 0 )
    {
        CsErrx( "BR_LLSC_6_12 : MYKI_CS_TAControlGet() failed" );
        return RULE_RESULT_ERROR;
    }

    if ( pMYKI_TAControl->ProductInUse == 0 )
    {
        CsDbg( BRLL_RULE, "BR_LLSC_6_12 : Bypass - ProductInUse is zero");
        return RULE_RESULT_BYPASSED;
    }

    /* Get product and directory structure in use */
    if ( myki_br_GetCardProduct( pMYKI_TAControl->ProductInUse, &pDirectoryInUse, &pProductInUse ) < 0 )
    {
        CsErrx( "BR_LLSC_6_12 : myki_br_GetCardProduct() failed" );
        return RULE_RESULT_ERROR;
    }

    /* Pre-Conditions */

    /* 1.  The Border Status Bit of  the product control bit map(2) of the product in use(1) is true */
    if ( ! ( pProductInUse->ControlBitmap & TAPP_TPRODUCT_CONTROL_BORDER_STATUS_BITMAP ) )
    {
        CsDbg( BRLL_RULE, "BR_LLSC_6_12 : Bypass - Border Status Bit is false");
        return RULE_RESULT_BYPASSED;
    }

    /* 2.  Find the product with product control status of Active or Issued (excluding the product in use )
            which covers the existing trip zone range such that: */
    for ( dir = 1; dir < DIMOF( pMYKI_TAControl->Directory ); dir++ )
    {
        if ( dir == pMYKI_TAControl->ProductInUse )     // Ignore the product in use
            continue;

        if ( myki_br_GetCardProduct( dir, &pDirectory, NULL ) < 0 )
        {
            CsErrx( "BR_LLSC_6_12 : myki_br_GetCardProduct() failed" );
            return RULE_RESULT_ERROR;
        }

        if ( pDirectory->Status == TAPP_CONTROL_DIRECTORY_STATUS_ACTIVATED || pDirectory->Status == TAPP_CONTROL_DIRECTORY_STATUS_ISSUED )
        {
            if ( myki_br_GetCardProduct( dir, &pDirectory, &pProduct ) < 0 )
            {
                CsErrx( "BR_LLSC_6_12 : myki_br_GetCardProduct(%d) failed", dir );
                return RULE_RESULT_ERROR;
            }

            if ( pDirectory->Status == TAPP_CONTROL_DIRECTORY_STATUS_ISSUED && myki_br_cd_GetProductType( pDirectory->ProductId ) == PRODUCT_TYPE_EPASS )
            {
                if ( pProduct->StartDateTime > pProductInUse->StartDateTime )
                    continue;
            }

            //
            // 2.a.  If the current trip zone low(5) is less than or equal to the product zone high(4) and the current trip
            //       zone low(5) is greater than or equal to the product zone low(3)
            //
            if (    pData->DynamicData.currentTripZoneLow <= pProduct->ZoneHigh
                 && pData->DynamicData.currentTripZoneLow >= pProduct->ZoneLow )
                break;

            //
            // 2.b.  If the Border status bit of the product control bitmap(2) for the product in use(1) is out-bound and the
            //       product low zone minus 1 is equal to the current trip zone low(5)
            //
            // 2.c.  If the Border status bit of the product control bitmap(2) for the product in use(1) is in-bound and the
            //       product high(4) zone plus 1 is equal the current trip zone low(5)
            //

            if ( pProductInUse->ControlBitmap & TAPP_TPRODUCT_CONTROL_BORDER_STATUS_BITMAP )        // Border trip (ie border product in use)
            {
                /* 2.b */
                if  ( pProductInUse->ControlBitmap & TAPP_TPRODUCT_CONTROL_BORDER_SIDE_BITMAP )     // Outbound border trip
                {
                    if ( ( pProduct->ZoneLow - 1 ) == pData->DynamicData.currentTripZoneLow )
                        break;
                }
                /* 2.c */
                else                                                                    // Inbound border product
                {
                    if ( ( pProduct->ZoneHigh + 1 ) == pData->DynamicData.currentTripZoneLow )
                        break;
                }
            }

            // 2.d.  If the Current trip is a thin zone(10) and the product low zone(4) minus 1 is equal to the current trip
            //       zone low(5)
            //
            if (    ( pData->DynamicData.currentTripIsThinZone )
                 && ( ( pProduct->ZoneLow - 1 ) == pData->DynamicData.currentTripZoneLow ) )
                break;

            // 2.e.  The product is a border product (border status bit on the product control bitmap(2)  is true) such that
            //       the low zone of the border product is equal to the low zone of the current trip and further, the
            //       border side bit of the product control bitmap of the product is equal to the border side bit of the
            //       product in use (ie the current trip)
            //
            if ( pProduct->ControlBitmap & TAPP_TPRODUCT_CONTROL_BORDER_STATUS_BITMAP )
            {
                if
                (
                    (   pProduct->ZoneLow == pData->DynamicData.currentTripZoneLow ) &&
                    ( ( pProduct->ControlBitmap & TAPP_TPRODUCT_CONTROL_BORDER_SIDE_BITMAP ) == ( pProductInUse->ControlBitmap & TAPP_TPRODUCT_CONTROL_BORDER_SIDE_BITMAP ) )
                )
                {
                    break;
                }
            }
        }
    }

    //  End of ForLoop to determine one if one of several conditions exists
    //  If we have broken out of the loop then we have a matching product
    //  If the terminating condition exists, then we did not break out of the loop
    //
    if ( dir < 1 || dir >= DIMOF( pMYKI_TAControl->Directory ) )
    {
        CsDbg( BRLL_RULE, "BR_LLSC_6_12 : Bypass - Cannot find another product that meets pre-condition 2" );
        return RULE_RESULT_BYPASSED;
    }

    //------------------------------------------------------------------------------------------------------------
    //  Description
    //  1.  If the border status bit(2)  on the product in use(1)  is inbound then increase the combined zone range
    //      high(9)  zone by 1
    //      If the border status bit(2)  on the product in use(1)  is outbound then decrease the combined zone range
    //      low(8)  zone by 1
    //
    //  Note: We already know it's a border product because of precondition 1, so no need to check status, just side.
    //
    //  Note : Major deviation from the document here, code here is as advised by Ashish.
    //

    CsDbg( BRLL_RULE, "BR_LLSC_6_12 : Before : Current Trip Low Zone = %d, High Zone = %d", pData->DynamicData.currentTripZoneLow, pData->DynamicData.currentTripZoneHigh );

    if ( pProductInUse->ControlBitmap & TAPP_TPRODUCT_CONTROL_BORDER_STATUS_BITMAP )        // Border trip (ie border product in use)
    {
        if  ( pProductInUse->ControlBitmap & TAPP_TPRODUCT_CONTROL_BORDER_SIDE_BITMAP )     // Outbound border trip
        {
            if ( ( pProduct->ZoneLow - 1 ) == pData->DynamicData.currentTripZoneLow )
            {
                pData->DynamicData.currentTripZoneLow  = pProduct->ZoneLow;
                pData->DynamicData.currentTripZoneHigh = pProduct->ZoneLow;
            }
        }
        else                                                                                // Inbound border trip
        {
            if ( ( pProduct->ZoneHigh + 1 ) == pData->DynamicData.currentTripZoneLow )
            {
                pData->DynamicData.currentTripZoneLow  = pProduct->ZoneHigh;
                pData->DynamicData.currentTripZoneHigh = pProduct->ZoneHigh;
            }
        }
    }

    if ( pData->DynamicData.currentTripIsThinZone )
    {
        if ( ( pProduct->ZoneLow - 1 ) == pData->DynamicData.currentTripZoneLow )
        {
            pData->DynamicData.currentTripZoneLow  = pProduct->ZoneLow;
            pData->DynamicData.currentTripZoneHigh = pProduct->ZoneLow;
        }
    }

    CsDbg( BRLL_RULE, "BR_LLSC_6_12 : After  : Current Trip Low Zone = %d, High Zone = %d", pData->DynamicData.currentTripZoneLow, pData->DynamicData.currentTripZoneHigh );

    //
    //  We've found a product that covers our current border trip.
    //  pDirectory / pProduct point to this product
    //

    if ( pDirectory->Status == TAPP_CONTROL_DIRECTORY_STATUS_ACTIVATED )
    {
        //  2.a.ii.1.
        //  2.a.ii.2.

        if ( pData->DynamicData.currentTripZoneLow < pData->InternalData.ProposedMerge.ZoneLow )
        {
            CsDbg( BRLL_RULE, "BR_LLSC_6_12 : Setting Proposed Merge Zone Low to %d", pData->DynamicData.currentTripZoneLow );
            pData->InternalData.ProposedMerge.ZoneLow = pData->DynamicData.currentTripZoneLow;
        }

        if ( pData->DynamicData.currentTripZoneHigh > pData->InternalData.ProposedMerge.ZoneHigh )
        {
            CsDbg( BRLL_RULE, "BR_LLSC_6_12 : Setting Proposed Merge Zone High to %d", pData->DynamicData.currentTripZoneHigh );
            pData->InternalData.ProposedMerge.ZoneHigh = pData->DynamicData.currentTripZoneHigh;
        }

        //  2.a.ii.3.  Update the combined zone range fare(11)  to the fare determined for the:
        //      (1)  combined zone range(8) ,(9)
        //      (2)  passenger type(16)
        //      (3)  date of travel(17)
        //      (4)  fare route id(15)
        //      (5)  current trip direction(18)

        if
        (
            myki_br_getFareStoredValueEx
            (
                &pData->InternalData.ProposedMerge.Fare,
				pData,
                pData->InternalData.ProposedMerge.ZoneLow,
                pData->InternalData.ProposedMerge.ZoneHigh,
                pMYKI_TAControl->PassengerCode,
                pData->InternalData.ProposedMerge.TripDirection,
                pData->DynamicData.fareRouteIdIsValid,
                pData->DynamicData.fareRouteId,
				pProductInUse->StartDateTime,
				pData->DynamicData.currentDateTime
            ) < 0
        )
        {
            CsErrx("BR_LLSC_6_12 : myki_br_getFareStoredValueEx() failed");
            return RULE_RESULT_ERROR;
        }

        //  2.a.ii.4.  Set the combined zone range off peak fare to the combined zone range fare multiplied by 1
        //          less the off peak discount rate.

        pData->InternalData.ProposedMerge.OffPeakFare = CalculateDiscountedFare(
                                                            pData->InternalData.ProposedMerge.Fare,
                                                            pData->DynamicData.offPeakDiscountRate );

        //  2.a.iii.  Perform a ProductUpdate/Invalidate on the product in use(1)  (which is the provisional product)
        //
        if ( myki_br_ldt_ProductUpdate_Invalidate( pData, pDirectoryInUse ) < 0 )
        {
            CsErrx( "BR_LLSC_6_12 : myki_br_ldt_ProductUpdate_Invalidate() failed" );
            return RULE_RESULT_ERROR;
        }
        //  2.a.iv.   Perform TAppUpdate/SetProductInUse to mark the product that covers the trip as the as the
        //            product in use(1)
        if ( myki_br_ldt_AppUpdate_SetProductInUse( pData, pDirectory ) < 0 )
        {
            CsErrx( "BR_LLSC_6_12 : myki_br_ldt_AppUpdate_SetProductInUse() failed" );
            return RULE_RESULT_ERROR;
        }
    }
    else /* 3 */
    {
        //  3.  else;
        //      a.  If there is no active e-Pass product and if a inactive e-Pass product exists and the e-Pass start
        //          date time(17)  is less  than the start time of the product in use(1)  (which is the provisional) where the
        //          inactive e-Pass covers such as either of the following are true:
        //

        if ( ! isActiveEPassExist(pMYKI_TAControl) )
        {
            //  3.b.i.   Activate the Inactive e-Pass by performing an ProductUpdate/Activate transaction
            //
            if ( myki_br_ldt_ProductUpdate_Activate( pData, pDirectory ) < 0 )
            {
                CsErrx( "BR_LLSC_6_12 : myki_br_ldt_ProductUpdate_Activate() failed" );
                return RULE_RESULT_ERROR;
            }

            //
            //  3.b.ii.  Update the low zone of the combined zone range(8)  if the e-Pass product zone low(3)  is less
            //           than low zone of the combined zone range(8)
            //  3.b.iii. Update the high zone of the combined zone range(9)  if the e-Pass product zone high(4)  is
            //           greater than the high zone of the combined zone range(9)
            //
            if ( pProduct->ZoneLow < pData->InternalData.ProposedMerge.ZoneLow )
                pData->InternalData.ProposedMerge.ZoneLow = pProduct->ZoneLow;

            if ( pProduct->ZoneHigh > pData->InternalData.ProposedMerge.ZoneHigh )
                pData->InternalData.ProposedMerge.ZoneHigh = pProduct->ZoneHigh;

            //  3.b.iv.  Update the combined zone range fare(11)  to the fare determined for the:
            //      (1)  combined zone range(8) ,(9)
            //      (2)  passenger type(16)
            //      (3)  date of travel(17)
            //      (4)  fare route id(15)
            //      (5)  current trip direction(18)
            //

            if
            (
                myki_br_getFareStoredValueEx
                (
                    &pData->InternalData.ProposedMerge.Fare,
                    pData,
                    pData->InternalData.ProposedMerge.ZoneLow,
                    pData->InternalData.ProposedMerge.ZoneHigh,
                    pMYKI_TAControl->PassengerCode,
                    pData->InternalData.ProposedMerge.TripDirection,
                    pData->DynamicData.fareRouteIdIsValid,
                    pData->DynamicData.fareRouteId,
                    pProductInUse->StartDateTime,
                    pData->DynamicData.currentDateTime
                ) < 0
            )
            {
                CsErrx("BR_LLSC_6_12 : myki_br_getFareStoredValueEx() failed");
                return RULE_RESULT_ERROR;
            }


            //
            //  3.b.v.  Set the combined zone range off peak fare to the combined zone range fare multiplied by 1
            //          less the off peak discount rate.
            //

            pData->InternalData.ProposedMerge.OffPeakFare = CalculateDiscountedFare(
                                                                pData->InternalData.ProposedMerge.Fare,
                                                                pData->DynamicData.offPeakDiscountRate );

            //
            //  3.b.vi.  Perform a product update/invalidate on the product in use(1)
            //

            if ( myki_br_ldt_ProductUpdate_Invalidate( pData, pDirectoryInUse ) < 0 )
            {
                CsErrx( "BR_LLSC_6_12 : myki_br_ldt_ProductUpdate_Invalidate() failed" );
                return RULE_RESULT_ERROR;
            }

            //  3.b.vii.  Modify usage log
            //         (1) Definition:
            //         (2) Set Usage - Product validation status Activated = bit 0 = true
            //

            pData->InternalData.IsUsageLogUpdated = TRUE;
            pData->InternalData.UsageLogData.isProductValidationStatusSet = TRUE;
            pData->InternalData.UsageLogData.productValidationStatus |= TAPP_USAGE_LOG_PROD_VAL_STATUS_ACTIVATE_BITMAP;

            //  3.b.viii.  Mark the e-Pass product as the product in use using a TAppUpdate/SetProductInUse
            //

            if ( myki_br_ldt_AppUpdate_SetProductInUse( pData, pDirectory ) < 0 )
            {
                CsErrx( "BR_LLSC_6_12 : myki_br_ldt_AppUpdate_SetProductInUse() failed" );
                return RULE_RESULT_ERROR;
            }
        }
    }

    CsDbg( BRLL_RULE, "BR_LLSC_6_12 : EXECUTED" );
    return RULE_RESULT_EXECUTED;
}


