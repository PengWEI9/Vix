/* -*- mode: c; tabs: 4; -*- */
/*=============================================================================
**
**    Vix Technology                   Licensed software
**    (C) 2013                         All rights reserved
**
**=============================================================================
**
**  Project/Product : MBU
**  Filename        : BR_LLSC_4_19.c
**  Author(s)       : ???
**
**  ID              : BR_LLSC_4_19 - NTS0177 v7.3
**
**  Name            : Determine trip extension
**
**  Data Fields     :
**
**      1.  TAppTProduct.EndDateTime 
**      2.  Dynamic.CurrentDateTime 
**      3.  TAppTProduct.StartDateTime 
**      4.  TAppTProduct.LastUsageZone 
**      5.  Tariff.NHourExtendThreshold 
**      6.  Tariff.NhourExtendPeriodMinutes 
**      7.  Dynamic.AdditionalMinutesThisTrip 
**      8.  Tariff.NHourMaximumDuration 
**      9.  TAppControl.ProductInUse 
**      10.  TAppTProduct.ProductControlBitmap 
**      11.  Dynamic.CurrentTripZoneLow 
**      12.  Dynamic.CurrentTripZoneHigh 
**      13.  Dynamic.NhourDuration
**
**  Pre-Conditions  :
**
**      1.  The product in use(9) is set
**      2.  The product in use(9) is of type provisional as indicated by
**          the product control bitmap(10)
**      3.  The number of additional minutes(7) is currently 0 (zero).
**
**  Description     :
**
**      1.  Count the number of zones travelled from current trip zone
**          low(11) to current trip zone high(12) (not counting any zones
**          marked as city saver)
**      2.  Determine the number of extensions to give: for every whole
**          multiple of zones(5) that can be divided into the zone count
**      3.  Determine the additional minutes(7) applicable: multiply the
**          number of extensions by the number of minutes entitled to[o](6)
**      4.  If the n-hour duration(13) plus the number of additional
**          minutes(7) is greater than maximum n-Hour duration(8) then set
**          the additional minutes(7) to be the converted maximum duration(8)
**          in minutes (ie. covert the maximum duration from hours to minutes)
**      5.  If the number of additional minutes(7) is not 0 (ie an extension
**          is applicable) then
**          a.  Perform a ProductUpdate/Extend on the provisional product
**              i.  Set the provisional expiry to the greater of:
**                  (1) The current expiry date time(1)
**                  (2) The start date time plus(3) the n-hour duration(13)
**                      plus the determined number of additional minutes(7).
**          b.  Set Additional minutes this trip(7) to the determined
**              number of additional minutes
**      6.  Else set the number of additional minutes(7) to -1. (NB:This
**          is done to stop the business rules re-executing this)
**
**  Post-Conditions :
**
**      1.  The additional minutes for this trip has been determined
**
**  Devices         :
**
**      Fare payment devices
**
**  Member(s)       :
**      BR_LLSC_4_19             [public]    business rule
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
**    1.01  23.07.14    ANT   Add      Updated comments, no code change.
**
**===========================================================================*/

#include <cs.h>                 // For debug logging
#include <myki_cardservices.h>  // Myki Card Services
#include <LDT.h>                // Logical Device Transaction

#include "myki_br_rules.h"      // API for all BR_xxx modules
#include "BR_Common.h"          // Common utilities, macros, etc

/*==========================================================================*
**
**  BR_LLSC_4_19
**
**  Description     :
**      Implements business rule BR_LLSC_4_19.
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

RuleResult_e BR_LLSC_4_19( MYKI_BR_ContextData_t *pData )
{
    MYKI_TAControl_t    *pMYKI_TAControl;
    MYKI_Directory_t    *pDirectory;
    MYKI_TAProduct_t    *pProduct;

    CsDbg( BRLL_RULE, "BR_LLSC_4_19 : Start (Determine trip extension)" );

    if ( ! pData )
    {
        CsErrx( "BR_LLSC_4_19 : Invalid argument(s)" );
        return RULE_RESULT_ERROR;
    }

    if ( MYKI_CS_TAControlGet( &pMYKI_TAControl ) )
    {
        CsErrx( "BR_LLSC_4_19 : MYKI_CS_TAControlGet() failed" );
        return RULE_RESULT_ERROR;
    }

    //--------------------------------------------------------------------------
    //  Pre-Conditions
    //  1.  The product in use(9) is set
    if ( pMYKI_TAControl->ProductInUse == 0 )
    {
        CsDbg( BRLL_RULE, "BR_LLSC_4_19 : Bypass - No product in use" );
        return RULE_RESULT_BYPASSED;
    }

    //  2.  The product in use(9) is of type provisional as indicated by the product control bitmap(10)

    //  Get product structure in use
    if ( myki_br_GetCardProduct( pMYKI_TAControl->ProductInUse, &pDirectory, &pProduct ) < 0 )
    {
        CsErrx( "BR_LLSC_4_19 : myki_br_GetCardProduct( %d ) failed", pMYKI_TAControl->ProductInUse );
        return RULE_RESULT_ERROR;
    }

    if ( !(pProduct->ControlBitmap & TAPP_TPRODUCT_CONTROL_PROVISIONAL_BITMAP ))
    {
        CsDbg( BRLL_RULE, "BR_LLSC_4_19 : Bypass - Product Provisional Bit not set" );
        return RULE_RESULT_BYPASSED;
    }

    //  3.  The number of additional minutes(7) is currently 0 (zero).
    if ( 0 != pData->DynamicData.additionalMinutesThisTrip )
    {
        CsDbg( BRLL_RULE, "BR_LLSC_4_19 : Bypass - AdditionalMinutesThisTrip non zero: %d", pData->DynamicData.additionalMinutesThisTrip );
        return RULE_RESULT_BYPASSED;
    }

    //--------------------------------------------------------------------------
    // Actions
    //
    // Sanity test
    // Low zone should be no higher than high zone
    if ( pData->DynamicData.currentTripZoneLow > pData->DynamicData.currentTripZoneHigh)
    {
        CsErrx( "BR_LLSC_4_19 : Low zone (%d) > high zone(%d)", pData->DynamicData.currentTripZoneLow, pData->DynamicData.currentTripZoneHigh );
        return RULE_RESULT_ERROR;
    }

    //
    //  1.  Count the number of zones travelled from current trip zone
    //      low(11) to current trip zone high(12) (not counting any zones
    //      marked as city saver)
    //  2.  Determine the number of extensions to give: for every whole
    //      multiple of zones(5) that can be divided into the zone count
    //  3.  Determine the additional minutes(7) applicable: multiply the
    //      number of extensions by the number of minutes entitled to[o](6)
    //  4.  If the n-hour duration(13) plus the number of additional
    //      minutes(7) is greater than maximum n-Hour duration(8) then set
    //      the additional minutes(7) to be the converted maximum duration(8)
    //      in minutes (ie. covert the maximum duration from hours to minutes)

    pData->DynamicData.additionalMinutesThisTrip = myki_br_GetAdditionalMinutes( pData, pData->DynamicData.currentTripZoneLow, pData->DynamicData.currentTripZoneHigh );

    //  5.  If the number of additional minutes(7) is not 0 (ie an extension is applicable) then

    if ( pData->DynamicData.additionalMinutesThisTrip != 0 )
    {
        //
        //  Make changes in a copy of the product
        //
        MYKI_TAProduct_t  newProduct = *pProduct;

        //  a.  Perform a ProductUpdate/Extend on the provisional product
        //  i.  Set the provisional expiry to the greater of:
        //      (1)  The current expiry date time(1)
        //      (2)  The start date time plus(3) the n-hour duration(13) plus the determined number of additional minutes(7).

        newProduct.EndDateTime = pProduct->EndDateTime;
        Time_t item2 = myki_br_NHourEndDateTimeEx
                (
                    pData,
                    pProduct->StartDateTime,
                    myki_br_GetProductDuration( pData, PRODUCT_TYPE_NHOUR ),
                    MINUTES_TO_SECONDS( pData->DynamicData.additionalMinutesThisTrip )
                );
        if ( item2 > newProduct.EndDateTime )
        {
            newProduct.EndDateTime = item2;
            if ( myki_br_ldt_ProductUpdate_Extend( pData, pDirectory, pProduct, &newProduct, FALSE, FALSE, FALSE ) < 0 )
            {
                CsErrx( "BR_LLSC_4_19 : myki_br_ldt_ProductUpdate_Extend() failed" );
                return RULE_RESULT_ERROR;
            }
        }
        CsDbg( BRLL_RULE, "BR_LLSC_4_19 : Provisional Expiry:%d", pProduct->EndDateTime);

        //  b.  Set Additional minutes this trip(7) to the determined number of additional minutes

//      pData->DynamicData.additionalMinutesThisTrip = additionalMinutesThisTrip;
    }
    else
    {
        //  6.  Else set the number of additional minutes(7) to -1.
        //     (NB:This is done to stop the business rules re-executing this)
//      pData->DynamicData.additionalMinutesThisTrip = -1;
    }

    CsDbg( BRLL_RULE, "BR_LLSC_4_19 : Additional Minutes:%d", pData->DynamicData.additionalMinutesThisTrip);
    return RULE_RESULT_EXECUTED;
}


