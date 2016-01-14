/* -*- mode: c; tabs: 4; -*- */
/*=============================================================================
**
**    Vix Technology                   Licensed software
**    (C) 2014                         All rights reserved
**
**=============================================================================
**
**  Project/Product : MBU
**  Filename        : BR_LLSC_6_23.c
**  Author(s)       : Morgan Dell
**
**  ID              : BR_LLSC_6_23 - KA0004 v7.3
**
**  Name            : Location Data was Unavailable at scan on 
**
**  Data Fields     :
**
**      1.  TAppControl.ProductInUse 
**      2.  Dynamic.CurrentTripZoneLow 
**      3.  Dynamic.CurrentTripZoneHigh 
**      4.  TAppTProduct.LastUsageRouteID 
**      5.  TAppTproduct.LastUsageEntryPoint 
**      6.  Dynamic.LineId 
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
**      1.  The product in use field(1) is not 0 (a product is in a scanned-on
**          state).
**      2.  The data for current route/stop is currently available
**          (ie Dynamic.LocationDataUnavailable(16) is false)
**      3.  The entry point(9) and service provider on the product in use is equal
**          to the current service provider & entry point (ie this is the same
**          vehicle)
**      4.  Any of the following 3 rules is true:
**          a.  The provisional fare is equal to $0.00 and there is no e-pass present
**          b.  The provisional fare is equal to $0.00 and an activated e-pass exists
**          c.  The provisional fare is equal to $0.00 and No activate epass exists
**              and No e-pass product with status issued exists that covers the
**              current zone range such that the: e-Pass zone range (zone low to zone
**              high) covers the current location border zone Low and location border
**              zone high.
**
**      !!! CLARIFICATION !!!
**      4c. "...No e-pass product with status issued exists that covers the current zone range"
**          should be interpreted as "...There exists an e-pass product with status issued that
**          covers the current zone range..."
**          The word 'covers' means e-pass zone low/high are within the current zone range.
**
**      5.  This is not a force scan off sequence(17)
**      6.  The current mode of transport(10) is not equal to Rail
**
**  Description     :
**      1.  Set IsEarlyBirdTrip(15) to false
**      2.  Set the FareRouteID(11) to none - no route will be considered for pricing
**      3.  Using the current route stop determine the location; append the current
**          location to the list of transport locations(12) twice. (As we will only
**          charge him from Scan off  location to Scan off location)
**      4.  Set the Current Trip Direction(13) to Unknown.
** 
**  Notes           :
**
**      In this scenario we will charge from the current location to the current
**      location to prevent over charging
** 
**  Post-Conditions :
**
**      1.  The locations for use in zone range determination have been appended
**          to the list of locations
**
**  Devices         :
**
**    Fare payment devices
**
**  Member(s)       :
**      BR_LLSC_6_23            [public]    business rule
**
**  Information     :
**   Compiler(s)    : C
**   Target(s)      : Independent
**
**  History         :
**   Vers.  Date        Aut.  Type     Description
**   -----  ----------  ----  -------  ----------------------------------------
**    1.00  ??.07.14    MD    Create
**    1.01  25.07.14    ANT   Modify   Replaced Tariff.locationInnerBorder
**                                     and Tariff.locationOuterBorder with
**                                     DynamicData.currentInnerZone and
**                                     DynamicData.currentOuterZone.
**    1.02  26.08.14    ANT   Modify   Rectified pre-condition 3 and 4c
**
**===========================================================================*/

#include <cs.h>                 // For debug logging
#include <myki_cardservices.h>  // Myki Card Services
#include <LDT.h>                // Logical Device Transaction

#include "myki_br_rules.h"      // API for all BR_xxx modules
#include "BR_Common.h"          // Common utilities, macros, etc

#define RULE_NAME                       "BR_LLSC_6_23"
#define RULE_PREFIX                     RULE_NAME " : "

/*==========================================================================*
**
**  BR_LLSC_6_23
**
**  Description     :
**      Implements business rule BR_LLSC_6_23.
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

RuleResult_e BR_LLSC_6_23( MYKI_BR_ContextData_t *pData )
{
    MYKI_TAControl_t        *pMYKI_TAControl       = NULL;
    MYKI_Directory_t        *pDirectoryInUse       = NULL;
    MYKI_TAProduct_t        *pProductInUse         = NULL;
    MYKI_Directory_t        *pMYKI_Directory       = NULL;
    MYKI_TAProduct_t        *pMYKI_TAProduct       = NULL;
    int                      i                     = 0;
    int                      epassFound            = FALSE;
    int                      activatedEpassFound   = FALSE;
    int                      issuedEpassFound      = FALSE;

    CsDbg( BRLL_RULE, "BR_LLSC_6_23 : Start (Location Data was Unavailable at scan on)" );

    if ( ! pData )
    {
        CsErrx( "BR_LLSC_6_23 : Invalid argument(s)" );
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
        pData->ReturnedData.bypassCode = BYPASS_CODE(6, 23, 1, 0);
        return RULE_RESULT_BYPASSED;
    }

    //
    //  2.  The data for current route/stop is currently available (ie Dynamic.LocationDataUnavailable(16) is false)
    //
    if ( pData->DynamicData.locationDataUnavailable == TRUE )
    {
        CsDbg( BRLL_RULE, RULE_PREFIX "Bypass - current route/stop is unavailable" );
        pData->ReturnedData.bypassCode = BYPASS_CODE(6, 23, 2, 0);
        return RULE_RESULT_BYPASSED;
    }


    //  Get product and directory structure in use
    if ( myki_br_GetCardProduct( pMYKI_TAControl->ProductInUse, &pDirectoryInUse, &pProductInUse ) < 0 )
    {
        CsErrx( RULE_PREFIX "myki_br_GetCardProduct(%d) failed", pMYKI_TAControl->ProductInUse );
        return RULE_RESULT_ERROR;
    }

    //
    //  3.  The entry point(9) and service provider on the product in use is equal to the current service provider &
    //      entry point (ie this is the same vehicle)
    //
    if ( pProductInUse->LastUsage.Location.EntryPointId != pData->DynamicData.entryPointId )
    {
        CsDbg( BRLL_RULE, RULE_PREFIX "Bypass - product entry point(%d) != current entry point(%d)",
                pProductInUse->LastUsage.Location.EntryPointId,
                pData->DynamicData.entryPointId );
        pData->ReturnedData.bypassCode = BYPASS_CODE( 6, 23, 3, 1 );
        return RULE_RESULT_BYPASSED;
    }
    if ( pProductInUse->LastUsage.ProviderId != pData->StaticData.serviceProviderId )
    {
        CsDbg( BRLL_RULE, RULE_PREFIX "Bypass - product entry service provider(%d) != current service provider(%d)",
                pProductInUse->LastUsage.ProviderId,
                pData->StaticData.serviceProviderId );
        pData->ReturnedData.bypassCode = BYPASS_CODE( 6, 23, 3, 2 );
        return RULE_RESULT_BYPASSED;
    }

    //
    //  4.  Any of the following 3 rules is true:
    //    a. The provisional fare is equal to $0.00 and there is no e-pass present
    //    b. The provisional fare is equal to $0.00 and an activated e-pass exists
    //    c. The provisional fare is equal to $0.00 and No activate epass exists and No e-pass product with
    //       status issued exists that covers the current zone range such that the: e-Pass zone range (zone low
    //       to zone high) covers the current location border zone Low and location border zone high.
    //
    if ( pMYKI_TAControl->ProvisionalFare != 0 )
    {
        CsDbg( BRLL_RULE, RULE_PREFIX "Bypass - provisional fare not 0" );
        pData->ReturnedData.bypassCode = BYPASS_CODE(6, 23, 4, 0);
        return RULE_RESULT_BYPASSED;
    }
    CsDbg( BRLL_RULE, RULE_PREFIX "Provisional fare is 0, continuing" );

    for ( i = 1; !activatedEpassFound && (i < DIMOF( pMYKI_TAControl->Directory )); ++i )
    {
        switch ( pMYKI_TAControl->Directory[ i ].Status )
        {
        case TAPP_CONTROL_DIRECTORY_STATUS_INITIALISED:
        case TAPP_CONTROL_DIRECTORY_STATUS_ISSUED:
        case TAPP_CONTROL_DIRECTORY_STATUS_ACTIVATED:
        case TAPP_CONTROL_DIRECTORY_STATUS_BLOCKED:
            // Product is present
            break;
        default:
            // Product is not present
            continue; // go back to the for loop
        }

        if ( myki_br_GetCardProduct( i, &pMYKI_Directory, &pMYKI_TAProduct ) < 0 )
        {
            CsErrx( RULE_PREFIX "myki_br_GetCardProduct(%d) failed", i );
            return RULE_RESULT_ERROR;
        }

        if ( myki_br_cd_GetProductType( pMYKI_Directory->ProductId ) != PRODUCT_TYPE_EPASS )
        {
            CsDbg( BRLL_RULE, RULE_PREFIX "Product at directory index %d is not an e-pass, ignoring", i );
            continue;
        }

        epassFound = TRUE;

        if ( pMYKI_Directory->Status == TAPP_CONTROL_STATUS_ACTIVATED )
        {
            CsDbg( BRLL_RULE, RULE_PREFIX "Product at directory index %d is an activated e-pass", i );
            activatedEpassFound = TRUE;
        }

        // 4 c. The provisional fare is equal to $0.00 and No activate epass exists and No e-pass product with
        //      status issued exists that covers the current zone range such that the: e-Pass zone range (zone low
        //      to zone high) covers the current location border zone Low and location border zone high.
        else if ( pMYKI_Directory->Status == TAPP_CONTROL_STATUS_ISSUED )
        {
            if (
                (pMYKI_TAProduct->ZoneLow  >= pData->DynamicData.currentInnerZone) &&
                (pMYKI_TAProduct->ZoneHigh <= pData->DynamicData.currentOuterZone)
               )
            {
                //  Found issued (inactive) ePass that covers the current zone range.
                issuedEpassFound = TRUE;
            }
        }
    }

    //  4 a. The provisional fare is equal to $0.00 and there is no e-pass present
    if ( epassFound == FALSE )
    {
        CsDbg( BRLL_RULE, RULE_PREFIX "No e-passes present, continuing" );
    }
    // 4 b. The provisional fare is equal to $0.00 and an activated e-pass exists
    else if ( activatedEpassFound == TRUE )
    {
        CsDbg( BRLL_RULE, RULE_PREFIX "Found activated e-pass, continuing" );
    }
    //  4c. The provisional fare is equal to $0.00 and No activate epass exists and No e-pass product with
    //      status issued exists that covers the current zone range such that the: e-Pass zone range (zone low
    //      to zone high) covers the current location border zone Low and location border zone high.
    //
    //  PLEASE REFER TO !!! CLARIFICATION !!! IN FILE HEADER
    //
    else if ( issuedEpassFound == TRUE )
    {
        CsDbg( BRLL_RULE, RULE_PREFIX "Found issued e-pass that covers current zone range, continuing" );
    }
    else
    {
        CsDbg( BRLL_RULE, RULE_PREFIX "Bypass - e-passes present but none matching pre-condition 4b or 4c");
        pData->ReturnedData.bypassCode = BYPASS_CODE(6, 23, 4, 1);
        return RULE_RESULT_BYPASSED;
    }

    //
    //  5.  This is not a force scan off sequence(17)
    //
    if ( pData->DynamicData.isForcedScanOff )
    {
        CsDbg( BRLL_RULE, RULE_PREFIX "Bypass - is forced scan off" );
        pData->ReturnedData.bypassCode = BYPASS_CODE(6, 23, 5, 0);
        return RULE_RESULT_BYPASSED;
    }

    //
    //  6.  The current mode of transport(10) is not equal to Rail
    //
    if ( pData->InternalData.TransportMode == TRANSPORT_MODE_RAIL )
    {
        CsDbg( BRLL_RULE, RULE_PREFIX "Bypass - Device Transport Mode is RAIL" );
        pData->ReturnedData.bypassCode = BYPASS_CODE(6, 23, 6, 0);
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
    //  3.  Using the current route stop determine the location; append the current location to the list of transport
    //      locations(12) twice. (As we will only charge him from Scan off  location to Scan off location)
    //
    if ( myki_br_AddStopToTransportLocationsEx( pData, pData->DynamicData.lineId, pData->DynamicData.stopId, 2 ) < 0 )
    {
        CsErrx( RULE_PREFIX "myki_br_AddStopToTransportLocationsEx() failed" );
        return RULE_RESULT_ERROR;
    }

    //
    //  4.  Set the Current Trip Direction(13) to Unknown.
    //
    pData->DynamicData.currentTripDirection = TRIP_DIRECTION_UNKNOWN;

    CsDbg( BRLL_RULE, RULE_PREFIX "Executed");
    return RULE_RESULT_EXECUTED;
}

