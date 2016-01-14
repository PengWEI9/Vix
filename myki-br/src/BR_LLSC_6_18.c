/* -*- mode: c; tabs: 4; -*- */
/*=============================================================================
**
**    Vix Technology                   Licensed software
**    (C) 2013                         All rights reserved
**
**=============================================================================
**
**  Project/Product : MBU
**  Filename        : BR_LLSC_6_18.c
**  Author(s)       : David Purdie
**
**  ID              : BR_LLSC_6_18 - KA0004 v7.0
**
**  Name            : Scan off Early Bird
**
**  Data Fields     :
**
**      1.  TAppControl.ProductInUse
**      2.  DeviceParamater.EarlyBirdCutoffTime
**      3.  DeviceParamater.EarlyBirdStation
**      4.  DeviceParamater.EarlyBirdDayOfWeek
**      5.  TAppControl.ProductControlBitMap
**      6.  Dynamic.IsEarlyBirdTrip
**      7.  DeviceParameter.EarlyBirdProduct
**      8.  DeviceParameter.EndOfTransportDay
**      9.  TAppControl.Product.LastUseage.DateTime
**      10. Dynamic.CurrentTripZoneLow
**      11. Dynamic.CurrentTripZoneHigh
**      12. TAppTproduct.LastUsageServiceProviderID
**      13. Static.ModeOfTransport
**      14. Dynamic.CurrentDateTime
**
**  Pre-Conditions  :
**
**      1.  The product in use field(1) is not 0 (a product is in a scanned-on state)
**          and product control bitmap(5) indicates that it is a Provisional product.
**      2.  The scan on mode of transport for the service provider(12) (for the
**          provisional product) was mode of transport Rail
**      3.  The current location has mode of transport(13) equal to Rail
**      4.  Both the scan on station and the current location station are both
**          on early bird station list
** 
**      Corrected version (Brendan Tagg 29-Jan-14)
**      5.  The provisional product scan on date time plus the normal n-hour 
**          period (eg 2 hours rounded up to the next full hour) is greater than 
**          or equal to than the devices current time(14). 
**          I.e the product is not expired.
**      6.  The current day of week is on the early bird list of valid days(4).
**      7.  The current date time(14) is less than or equal to the Early Bird Cut off time(2)
**      8.  The scan on date time(9) is greater than the start of the current business day(8)
**
**  Description     :
**
**      1.  Set Dynamic.IsEarlyBirdTrip(6) to true this is an early bird trip
**      2.  Perform a ProductSale/Upgrade transaction for the product in use(1)
**          a.  Current trip low zone(10)
**          b.  Current trip high zone(11)
**          c.  Set the product ID to the configured early bird product(7)
**          d.  Set the expiry time to the current date/time(14)
**          e.  Set the purchase value to zero
**
**      Is eary bird is used to stop the daily capping
**      zone ranges being updated.
**
**  Post-Conditions :
**
**      1.  An n-hour product has been brought and used and invalidated.
**
**  Devices         :
**
**      FPDs, Gates
**
**  Member(s)       :
**      BR_LLSC_6_18            [public]    business rule
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
**    1.00  dd.mm.yy    DP    Create
**    1.01  08.05.14    ANT   Modify   MBU-1077: Rectified incorrect
**                                     EarlyBirdCutOffTime validation.
**
**===========================================================================*/

#include <cs.h>                 // For debug logging
#include <myki_cardservices.h>  // Myki Card Services
#include <LDT.h>                // Logical Device Transaction

#include "myki_br_rules.h"      // API for all BR_xxx modules
#include "BR_Common.h"          // Common utilities, macros, etc

/*==========================================================================*
**
**  BR_LLSC_6_18
**
**  Description     :
**      Implements business rule BR_LLSC_6_18.
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

RuleResult_e BR_LLSC_6_18( MYKI_BR_ContextData_t *pData )
{
    MYKI_TAControl_t            *pMYKI_TAControl            = NULL;
    MYKI_TAProduct_t            *pMYKI_TAProduct            = NULL;
    MYKI_Directory_t            *pMYKI_Directory            = NULL;
    struct tm                   *pTm                        = NULL;
    int                          providerId                 = 0;
    Time_t                       endDateTime                = TIME_NOT_SET;
    U16_t                        earlyBirdCutOffTime        = 0;
    Time_t                       earlyBirdCutOffDateTime    = TIME_NOT_SET;
    Time_t                       startOfBusinessDateTime    = TIME_NOT_SET;

    CsDbg( BRLL_RULE, "BR_LLSC_6_18 : Start (Scan off Early Bird)" );

    if ( ! pData )
    {
        CsErrx( "BR_LLSC_6_18 : Invalid argument(s)" );
        return RULE_RESULT_ERROR;
    }

    //----------------------------------------------------------------------------
    //  Pre-Conditions
    //  1.  The product in use field(1) is not 0 (a product is in a scanned-on state)
    //      and product control bitmap(5) indicates that it is a Provisional product.
    //
    if (MYKI_CS_TAControlGet( &pMYKI_TAControl ) < 0)
    {
        CsErrx( "BR_LLSC_6_18 : MYKI_CS_TAControlGet() failed" );
        return RULE_RESULT_ERROR;
    }

    if (pMYKI_TAControl->ProductInUse == 0)
    {
        CsDbg( BRLL_RULE, "BR_LLSC_6_18 : Bypass - No products in use" );
        pData->ReturnedData.bypassCode = 1;
        return RULE_RESULT_BYPASSED;
    }

    if ( myki_br_GetCardProduct( pMYKI_TAControl->ProductInUse, &pMYKI_Directory, &pMYKI_TAProduct ) < 0 )
    {
        CsErrx( "BR_LLSC_6_18 : MYKI_CS_TAProductGet( %d ) failed", pMYKI_TAControl->ProductInUse );
        return RULE_RESULT_ERROR;
    }

    if ( ! ( pMYKI_TAProduct->ControlBitmap & TAPP_TPRODUCT_CONTROL_PROVISIONAL_BITMAP ) )
    {
        CsDbg( BRLL_RULE, "BR_LLSC_6_18 : Bypass - Product Provisional Bit not set" );
        pData->ReturnedData.bypassCode = 2;
        return RULE_RESULT_BYPASSED;
    }

    //
    //  2.  The scan on mode of transport for the service provider(12) (for the
    //      provisional product) was mode of transport Rail
    //
    providerId = myki_br_cd_GetTransportModeForProvider( pMYKI_TAProduct->LastUsage.ProviderId );
    if ( providerId != TRANSPORT_MODE_RAIL )
    {
        CsDbg( BRLL_CHOICE, "BR_LLSC_6_18 : Bypass - Scan on transport mode (%d) is not RAIL", providerId);
        pData->ReturnedData.bypassCode = 3;
        return RULE_RESULT_BYPASSED;
    }

    //
    //  3.  The current location has mode of transport(13) equal to Rail
    //
    if ( pData->InternalData.TransportMode != TRANSPORT_MODE_RAIL )
    {
        CsDbg( BRLL_RULE, "BR_LLSC_6_18 : Bypass - Device Transport Mode is NOT RAIL" );
        pData->ReturnedData.bypassCode = 4;
        return RULE_RESULT_BYPASSED;
    }

    //
    //  4.  Both the scan on station and the current location station are both
    //      on early bird station list
    //
    if ( !MYKI_CD_isEarlyBirdStation(pMYKI_TAProduct->LastUsage.Location.EntryPointId) )
    {
        CsDbg( BRLL_RULE, "BR_LLSC_6_18 : Bypass - Scan on station not in early bird list" );
        pData->ReturnedData.bypassCode = 5;
        return RULE_RESULT_BYPASSED;
    }

    if ( !MYKI_CD_isEarlyBirdStation( (U16_t)pData->DynamicData.entryPointId ) )
    {
        CsDbg( BRLL_RULE, "BR_LLSC_6_18 : Bypass - Current station not in early bird list" );
        pData->ReturnedData.bypassCode = 6;
        return RULE_RESULT_BYPASSED;
    }

    //
    //  5.  The provisional product scan on date time plus the normal n-hour 
    //      period (eg 2 hours rounded up to the next full hour) is greater than 
    //      or equal to than the devices current time(14). 
    //      I.e the product is not expired.
    //
    endDateTime = pMYKI_TAProduct->LastUsage.DateTime + MINUTES_TO_SECONDS( pData->Tariff.nHourPeriodMinutes );
    myki_br_RoundUpEndTime( pData, &endDateTime );
    if ( endDateTime < pData->DynamicData.currentDateTime)
    {
        CsDbg( BRLL_RULE, "BR_LLSC_6_18 : Bypass - Scan on time not within nHour window" );
        pData->ReturnedData.bypassCode = 7;
        return RULE_RESULT_BYPASSED;
    }

    //
    //  6.  The current day of week is on the early bird list of valid days(4).
    //
    if ( pData->DynamicData.currentDateTime == TIME_NOT_SET )
    {
        CsErrx( "BR_LLSC_6_18 : CommonDate not set" );
        return RULE_RESULT_ERROR;
    }

    pTm = localtime( (time_t*)&pData->DynamicData.currentDateTime );
    if ( pTm == NULL )
    {
        CsErrx( "BR_LLSC_6_18 : localtime(%u) failed", pData->DynamicData.currentDateTime );
        return RULE_RESULT_ERROR;
    }

    if ( !MYKI_CD_isEarlyBirdDay(pTm->tm_wday) )
    {
        CsDbg( BRLL_RULE, "BR_LLSC_6_18 : Bypass - Not an early bird day(%d)", pTm->tm_wday );
        pData->ReturnedData.bypassCode = 8;
        return RULE_RESULT_BYPASSED;
    }

    //
    //  7.  The current date time(14) is less than or equal to the Early Bird Cut off time(2)
    //
    if ( ( earlyBirdCutOffTime = MYKI_CD_getEarlyBirdCutOffTime() ) == 0 )
    {
        CsDbg( BRLL_RULE, "BR_LLSC_6_18 : Bypass - Early Bird Cut Off Time not configured" );
        pData->ReturnedData.bypassCode = 12;
        return RULE_RESULT_BYPASSED;
    }

    startOfBusinessDateTime = myki_br_StartOfBusinessDateTime( pData, pData->DynamicData.currentDateTime );

    //
    //  Determine early bird cut-off date/time based on the current business date
    //
    earlyBirdCutOffDateTime = startOfBusinessDateTime -
                                    MINUTES_TO_SECONDS( pData->Tariff.endOfBusinessDay ) +
                                    MINUTES_TO_SECONDS( earlyBirdCutOffTime            );

    if ( pData->DynamicData.currentDateTime > earlyBirdCutOffDateTime )
    {
        CsDbg( BRLL_RULE, "BR_LLSC_6_18 : Bypass - Past Earlybird Cut Off Time" );
        pData->ReturnedData.bypassCode = 9;
        return RULE_RESULT_BYPASSED;
    }

    //
    //  8.  The scan on date time(9) is greater than the start of the current business day(8)
    //
    if ( pMYKI_TAProduct->LastUsage.DateTime <= startOfBusinessDateTime )
    {
        CsDbg( BRLL_RULE, "BR_LLSC_6_18 : Bypass - Before start of business day" );
        pData->ReturnedData.bypassCode = 10;
        return RULE_RESULT_BYPASSED;
    }

    //
    //  Indicate to Test Harness that all Pre-Conditions have been satisfied
    //
    pData->ReturnedData.bypassCode = 11;

    //----------------------------------------------------------------------------
    //  Description
    //  1.  Set Dynamic.IsEarlyBirdTrip(6) to true this is an early bird trip
    //
    pData->DynamicData.isEarlyBirdTrip = TRUE;

    //
    //  2.  Perform a ProductSale/Upgrade transaction for the product in use(1)
    //      a.  Current trip low zone(10)
    //      b.  Current trip high zone(11)
    //      c.  Set the product ID to the configured early bird product(7)
    //      d.  Set the expiry time to the current date/time(14)
    //      e.  Set the purchase value to zero
    //
    if ( myki_br_ldt_ProductSale_Upgrade(
                pData,
                pMYKI_Directory,
                MYKI_CD_getEarlyBirdProductId(),        // Product ID
                pData->DynamicData.currentTripZoneLow,  // Low Zone
                pData->DynamicData.currentTripZoneHigh, // High Zone
                0,                                      // Purchase Value
                pData->DynamicData.currentDateTime      // expiry Date Time. Product will be expired
                ) < 0 )
    {
        CsErrx( "BR_LLSC_6_18 : myki_br_ldt_ProductSale_Upgrade() failed" );
        return RULE_RESULT_ERROR;
    }

    CsDbg( BRLL_RULE, "BR_LLSC_6_18 : Executed" );
    return RULE_RESULT_EXECUTED;
}

