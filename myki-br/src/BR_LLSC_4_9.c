/* -*- mode: c; tabs: 4; -*- */
/*=============================================================================
**
**    Vix Technology                   Licensed software
**    (C) 2013                         All rights reserved
**
**=============================================================================
**
**  Project/Product : MBU
**  Filename        : BR_LLSC_4_9.c
**  Author(s)       : Morgan Dell
**
**  ID              : BR_LLSC_4_9
**
**  Name            : Missing Scan-off Same Service Provider Different Entry Point
**
**  Data Fields     :
**
**      1. TAppControl.ProductInUse
**      2. TAppTProduct.EndDateTime
**      3. TAppTProduct.LastUsageZone
**      4. TAppTProduct.LastUsageLineId
**      5. TAppTProduct.LastUsageProviderId
**      6. Static.TransportMode
**      7. Dynamic.CurrentZone
**      8. Dynamic.CurrentDateTime
**      9. Dynamic.LineId
**      10. Dynamic.RouteChangeover
**      11. Dynamic.EntryPointId
**      12. TAppTProduct.LastUseageEntryPointId
**      13. Dynamic.FareRouteID
**      14. Static.ServiceProviderID
**      15. Static.TransportMode
**
**  Pre-Conditions  :
**      1. The product in use field (1) is not 0 (a product is in a scanned-on state).
**      2. The RouteChangeover field (10) indicates no route changeover condition.
**      3. The scan on Service Provider (5) is equal to the service provider of the current device (14)
**      4. The mode of transport (15) as determined by the current device service provider is not equal to Rail.
**      5. The mode of transport (15) as determined by the service provider of the product in use is not equal to Rail.
**      6. The scan on entry point (12) from the product in use (1) is not equal to the Entry point on the device (11)
**
**  Description     :
**      1. Set the FareRouteID (13) to none – no route will be considered for pricing
**      2. Force scan off is performed
**
**  Post-Conditions :
**      1. Force scan off is performed
**
**  Devices         :
**      Fare payment devices
**
**  Member(s)       :
**      BR_LLSC_4_9             [public]    business rule
**
**  Information     :
**   Compiler(s)    : C
**   Target(s)      : Independent
**
**  History         :
**   Vers.  Date        Aut.     Type     Description
**   -----  ----------  ----     -------  ----------------------------------------
**    1.00  2013.10.02  Morgan   Create
**    1.01  2014.02.05  ANT      Modify   Rectified checking device entry point
**
**===========================================================================*/

/*
 *      Includes
 *      --------
 */

#include <cs.h>                 // For debug logging
#include <myki_cardservices.h>  // Myki Card Services
#include <LDT.h>                // Logical Device Transaction

#include "myki_br_rules.h"      // API for all BR_xxx modules
#include "BR_Common.h"          // Common utilities, macros, etc

/*==========================================================================*
**
**  BR_LLSC_4_9
**
**  Description     :
**      Implements business rule BR_LLSC_4_9.
**
**  Parameters      :
**      pData               [I/O]   BR context data
**
**  Returns         :
**      RULE_RESULT_EXECUTED
**      RULE_RESULT_BYPASSED
**      RULE_RESULT_ERROR
**
**==========================================================================*/

RuleResult_e BR_LLSC_4_9( MYKI_BR_ContextData_t *pData )
{
    MYKI_TAControl_t    *pMYKI_TAControl;
    MYKI_Directory_t    *pDirectory;
    MYKI_TAProduct_t    *pProduct;

    CsDbg( BRLL_RULE, "BR_LLSC_4_9 : Start (Missing scan-off same service provider different entry point)" );

    if ( ! pData )
    {
        CsErrx( "BR_LLSC_4_9 : Invalid argument(s)" );
        return RULE_RESULT_ERROR;
    }

    /* 1. The product in use field (1) is not 0 (a product is in a scanned-on state). */

    if ( MYKI_CS_TAControlGet( &pMYKI_TAControl ) < 0 )
    {
        CsErrx( "BR_LLSC_4_9 : MYKI_CS_TAControlGet() failed" );
        return RULE_RESULT_ERROR;
    }

    if ( pMYKI_TAControl->ProductInUse == 0 )
    {
        CsDbg( BRLL_RULE, "BR_LLSC_4_9 : Bypass - No products in use" );
        return RULE_RESULT_BYPASSED;
    }

    /* 2. The RouteChangeover field (10) indicates no route changeover condition. */

    if ( pData->DynamicData.routeChangeover != ROUTE_CHANGEOVER_NONE )
    {
        CsDbg( BRLL_RULE, "BR_LLSC_4_9 : Bypass - Route Changeover condition is not NONE" );
        return RULE_RESULT_BYPASSED;
    }

    /* Get a pointer to the product in use */

    if ( myki_br_GetCardProduct( pMYKI_TAControl->ProductInUse, &pDirectory, &pProduct ) < 0 )
    {
        CsErrx( "BR_LLSC_4_9 : myki_br_GetCardProduct( %d ) failed", pMYKI_TAControl->ProductInUse );
        return RULE_RESULT_ERROR;
    }

    /* 3. The scan on Service Provider (5) is equal to the service provider of the current device (14) */

    if ( pProduct->LastUsage.ProviderId != pData->StaticData.serviceProviderId )
    {
        CsDbg( BRLL_RULE, "BR_LLSC_4_9 : Bypass - Scan-on Service Provider != Device Service Provider" );
        return RULE_RESULT_BYPASSED;
    }

    /* 4. The mode of transport (15) as determined by the current device service provider is not equal to Rail.
     * and
     * 5. The mode of transport (15) as determined by the service provider of the product in use is not equal to Rail.
     * both service providers are the same as per check 3 above */

    if ( pData->InternalData.TransportMode == TRANSPORT_MODE_RAIL )
    {
        CsDbg( BRLL_RULE, "BR_LLSC_4_9 : Bypass - Device Transport Mode == RAIL" );
        return RULE_RESULT_BYPASSED;
    }

    /* 6. The scan on entry point (12) from the product in use (1) is not equal to the Entry point on the device (11) */

    if ( pProduct->LastUsage.Location.EntryPointId == pData->DynamicData.entryPointId )
    {
        CsDbg( BRLL_RULE, "BR_LLSC_4_9 : Bypass - Scan-on Entry Point == Device Entry Point" );
        return RULE_RESULT_BYPASSED;
    }

	/* 1. Set the FareRouteID (13) to none – no route will be considered for pricing */
    pData->DynamicData.fareRouteIdIsValid = FALSE;

    CsDbg( BRLL_RULE, "BR_LLSC_4_9 : Executed" );
    return RULE_RESULT_EXECUTED;
}


