/* -*- mode: c; tabs: 4; -*- */
/*=============================================================================
**
**    Vix Technology                   Licensed software
**    (C) 2014                         All rights reserved
**
**=============================================================================
**
**  Project/Product : MBU
**  Filename        : BR_LLSC_6_22.c
**  Author(s)       : Morgan Dell
**
**  ID              :  BR_LLSC_6_22 - NTS0177 v7.3
**
**  Name            : Location Data Unavailable at both scan on & scan off
**
**  Data Fields     :
**
**      1.  TAppControl.ProductInUse
**      2.  Dynamic.CurrentTripZoneLow
**      3.  Dynamic.CurrentTripZoneHigh
**      4.  TAppTProduct.LastUsageRouteID
**      5.  TAppTproduct.LastUsageEntryPoint
**      6.  TAppTProduct.EndDateTime
**      7.  Dynamic.CurrentDateTime
**      8.  Dynamic.CurrentTripIsThinZone
**      9.  Dynamic.EntryPointID
**      10. Static.TransportMode
**      11. Dynamic.FareRouteID
**      12. Dynamic.TransportLocations
**      13. Dynamic.CurrentTripDirection
**      14. TAppTproduct.LastUseageServiceProvider
**      15. Dynamic.IsEarlyBirdTrip
**      16. Dynamic.LocationDataUnavailable
**      17. Dynamic.IsForcedScanOff
**
**  Pre-Conditions  :
**
**      1.  The product in use field(1) is not 0 (a product is in a scanned-on state).
**      2.  The current mode of transport(10) is not Rail
**      3.  The location data for current route/stop is currently not available (ie Dynamic.LocationDataUnavailable(16)
**          is true)
**      4.  The entry point(5) and service provider(14) on the product in use is equal to the current service provider &
**          entry point(9) (ie this is the same vehicle)
**      5.  The provisional fare is equal to $0.00
**      6.  This is not a force scan off(17) sequence
**      7.  The expiry time(6) of the product in use is less than the current date time(7) (ie the product has not expired)
**
**  Description     :
**
**      1.  Set IsEarlyBirdTrip(15) to false
**      2.  Set the FareRouteID(11) to none - no route will be considered for pricing
**      3.  Set the Current Trip Direction(13) to Unknown.
**      4.  Perform a ProductSale/Upgrade Transaction on the product in use setting the:
**          a.  Product purchase value to 0
**          b.  Zone Low to the provisional low zone(3)
**          c.  Zone High to the provisional high zone(4).
**          d.  Expiry Date Time  to the current date time(10)
**          e.  Set the provisional bit false.
**          f.  Entry Point ID to the entry point ID of the current device(11)
**          g.  Route ID to the current route ID of the device(12)
**          h.  Stop ID to the current stop id of the device(13)
**
**      In this scenario we will charge nothing as we have no data for the zones which the
**      patron has travelled.
** 
**  Post-Conditions :
**
**      1.  The provisional product has been sold for $0; and will be subsequently invalidated.
**
**      The policy not to charge a patron in this instance has been determined and will be
**      subsequently carried
** 
**  Devices         :
**
**      Fare payment devices
** 
**
**  Member(s)       :
**      BR_LLSC_6_22            [public]    business rule
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
**    1.00  ??.07.14    MD    Create
**    1.01  27.08.14    ANT   Modify   Rectified business rule
**
**===========================================================================*/

#include <cs.h>                 // For debug logging
#include <myki_cardservices.h>  // Myki Card Services
#include <LDT.h>                // Logical Device Transaction

#include "myki_br_rules.h"      // API for all BR_xxx modules
#include "BR_Common.h"          // Common utilities, macros, etc

#define RULE_NAME               "BR_LLSC_6_22"
#define RULE_PREFIX             RULE_NAME " : "

/*==========================================================================*
**
**  BR_LLSC_6_22
**
**  Description     :
**      Implements business rule BR_LLSC_6_22.
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
**
**==========================================================================*/

RuleResult_e BR_LLSC_6_22( MYKI_BR_ContextData_t *pData )
{
    MYKI_TAControl_t       *pMYKI_TAControl     = NULL;
    MYKI_Directory_t       *pDirectoryInUse     = NULL;
    MYKI_TAProduct_t       *pProductInUse       = NULL;

    CsDbg( BRLL_RULE, RULE_PREFIX "Start (Location Data Unavailable at both scan on & scan off)" );

    if ( ! pData )
    {
        CsErrx( RULE_PREFIX "Invalid argument(s)" );
        return RULE_RESULT_ERROR;
    }

    if ( MYKI_CS_TAControlGet( &pMYKI_TAControl ) < 0 )
    {
        CsErrx( RULE_PREFIX "MYKI_CS_TAControlGet() failed" );
        return RULE_RESULT_ERROR;
    }

    // Pre-Conditions

    //
    //  1. The product in use field(1) is not 0 (a product is in a scanned-on state).
    //
    if ( pMYKI_TAControl->ProductInUse == 0 )
    {
        CsDbg( BRLL_RULE, RULE_PREFIX "Bypass - No products in use" );
        pData->ReturnedData.bypassCode = BYPASS_CODE(6, 22, 1, 0);
        return RULE_RESULT_BYPASSED;
    }

    //
    //  2.  The mode of transport by the current device is not rail
    //
    if ( pData->InternalData.TransportMode == TRANSPORT_MODE_RAIL )
    {
        CsDbg( BRLL_RULE, RULE_PREFIX "Bypass - Device Transport Mode is RAIL" );
        pData->ReturnedData.bypassCode = BYPASS_CODE(6, 22, 2, 0);
        return RULE_RESULT_BYPASSED;
    }

    //
    //  3.  The location data for current route/stop is currently not available (ie Dynamic.LocationDataUnavailable(16)
    //      is true)
    //
    if ( pData->DynamicData.locationDataUnavailable == FALSE )
    {
        CsDbg( BRLL_RULE, RULE_PREFIX "Bypass - current route/stop is available" );
        pData->ReturnedData.bypassCode = BYPASS_CODE(6, 22, 3, 0);
        return RULE_RESULT_BYPASSED;
    }

    //  Get product and directory structure in use
    if ( myki_br_GetCardProduct( pMYKI_TAControl->ProductInUse, &pDirectoryInUse, &pProductInUse ) < 0 )
    {
        CsErrx( RULE_PREFIX "myki_br_GetCardProduct(%d) failed", pMYKI_TAControl->ProductInUse );
        return RULE_RESULT_ERROR;
    }

    //
    //  4.  The entry point(5) and service provider(14) on the product in use is equal to the current service provider &
    //      entry point(9) (ie this is the same vehicle)
    //
    if ( pProductInUse->LastUsage.Location.EntryPointId != pData->DynamicData.entryPointId )
    {
        CsDbg( BRLL_RULE, RULE_PREFIX "Bypass - product entry point != current entry point" );
        pData->ReturnedData.bypassCode = BYPASS_CODE(6, 22, 4, 1);
        return RULE_RESULT_BYPASSED;
    }
    if ( pProductInUse->LastUsage.ProviderId != pData->StaticData.serviceProviderId )
    {
        CsDbg( BRLL_RULE, RULE_PREFIX "Bypass - product entry service provider != current service provider" );
        pData->ReturnedData.bypassCode = BYPASS_CODE(6, 22, 4, 2);
        return RULE_RESULT_BYPASSED;
    }

    //
    //  5.  The provisional fare is equal to $0.00
    //
    if ( pMYKI_TAControl->ProvisionalFare != 0 )
    {
        CsDbg( BRLL_RULE, RULE_PREFIX "Bypass - provisional fare not 0" );
        pData->ReturnedData.bypassCode = BYPASS_CODE(6, 22, 5, 0);
        return RULE_RESULT_BYPASSED;
    }

    //
    //  6.  This is not a force scan off(17) sequence
    //
	if ( pData->DynamicData.isForcedScanOff )
    {
        CsDbg( BRLL_RULE, RULE_PREFIX "Bypass - is forced scan off" );
        pData->ReturnedData.bypassCode = BYPASS_CODE(6, 22, 6, 0);
        return RULE_RESULT_BYPASSED;
    }

    //
    //  7.  The expiry time(6) of the product in use is less than the current date time(7) (ie the product has not expired)
    //      Note :  Above statement is self-contradicting - expiry less than current implies the product has expired.
    //              Interpreted as "product is not expired and therefore expiry time is *greater than or equal to* current time.
    //
    if ( pProductInUse->EndDateTime < pData->DynamicData.currentDateTime )
    {
        CsDbg( BRLL_RULE, RULE_PREFIX "Bypass - product in use is expired" );
        pData->ReturnedData.bypassCode = BYPASS_CODE(6, 22, 7, 0);
        return RULE_RESULT_BYPASSED;
    }


    // Execute

    //
    //  1.  Set IsEarlyBirdTrip(15) to false
    //
    pData->DynamicData.isEarlyBirdTrip = FALSE;

    //
    //  2.  Set the FareRouteID(11) to none - no route will be considered for pricing
    //
    pData->DynamicData.fareRouteIdIsValid = FALSE;

    //
    //  3.  Set the Current Trip Direction(13) to Unknown.
    //
    pData->DynamicData.currentTripDirection = TRIP_DIRECTION_UNKNOWN;

    //
    //  4.  Perform a ProductSale/Upgrade Transaction on the product in use setting the:
    //      a.  Product purchase value to 0
    //      b.  Zone Low to the provisional low zone(3)
    //      c.  Zone High to the provisional high zone(4).
    //      d.  Expiry Date Time  to the current date time(10)
    //      e.  Set the provisional bit false.
    //      f.  Entry Point ID to the entry point ID of the current device(11)
    //      g.  Route ID to the current route ID of the device(12)
    //      h.  Stop ID to the current stop id of the device(13)
    //
    // Note: items e, f, g and h are performed in myki_br_ldt_ProductSale_Upgrade
    if ( myki_br_ldt_ProductSale_Upgrade(
                pData,
                pDirectoryInUse,
                pDirectoryInUse->ProductId,             // Product ID
                pProductInUse->ZoneLow,                 // Low Zone
                pProductInUse->ZoneHigh,                // High Zone
                0,                                      // Purchase Value
                pData->DynamicData.currentDateTime      // expiry Date Time. Product will be expired
                ) < 0 )
    {
        CsErrx( RULE_PREFIX "myki_br_ldt_ProductSale_Upgrade() failed" );
        return RULE_RESULT_ERROR;
    }
    
    CsDbg( BRLL_RULE, RULE_PREFIX "Executed");
    return RULE_RESULT_EXECUTED;
}


