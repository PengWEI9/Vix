/**************************************************************************
*   ID :  BR_LLSC_4_10
*    Missing Scan-Off Different Service Provider
*    1.  TAppControl.ProductInUse
*    2.  TAppTProduct.LastUsageZone
*    3.  TAppTProduct.LastUsageProviderId
*    4.  Static.TransportMode
*    5.  Dynamic.CurrentZone
*    6.  TAppTProduct.EndDateTime
*    7.  Dynamic.FareRouteID
*    8.  Static.ServiceProviderID
*    9.  Static.TransportMode
*    10.  Dynamic.CurrentDateTime
*
*
*   Pre-Conditions
*    1.  The product in use field(1)  is not 0 (a product is in a scanned-on state).
*    2.  The service provider(3)  of the scan on is not equal to the service provider of the device(8) .
*    3.  The mode of transport(9)  of the provisional product (as determined by the product in use) is not equal to Rail(9)
*
*   Description
*    1.  Set the FareRouteID(7)  to none - no route will be considered for pricing
*    2.  Perform a Forced Scan-Off.
*
*   Post-Conditions
*    1.  A forced scan-off is performed.
*
*   Devices
*    Fare payment devices
*
*
 ***********************************************************************/

#include <cs.h>                 // For debug logging
#include <myki_cardservices.h>  // Myki Card Services
#include <LDT.h>                // Logical Device Transaction

#include "myki_br_rules.h"      // API for all BR_xxx modules
#include "BR_Common.h"          // Common utilities, macros, etc

//=============================================================================
//
//
//
//=============================================================================

RuleResult_e BR_LLSC_4_10( MYKI_BR_ContextData_t *pData )
{
    MYKI_TAControl_t    *pMYKI_TAControl;
    MYKI_Directory_t    *pDirectory;
    MYKI_TAProduct_t    *pProduct;
    TransportMode_e     transportMode;

    CsDbg( BRLL_RULE, "BR_LLSC_4_10 : Start (Missing Scan-Off Different Service Provider)" );

    if ( ! pData )
    {
        CsErrx( "BR_LLSC_4_10 : Invalid argument(s)" );
        return RULE_RESULT_ERROR;
    }

    if ( MYKI_CS_TAControlGet( &pMYKI_TAControl ) )
    {
        CsErrx( "BR_LLSC_4_10 : MYKI_CS_TAControlGet() failed" );
        return RULE_RESULT_ERROR;
    }

    //  1.  The product in use field(1)  is not 0 (a product is in a scanned-on state).
    if ( pMYKI_TAControl->ProductInUse == 0 )
    {
        CsDbg( BRLL_RULE, "BR_LLSC_4_10 : Bypass - No product in use" );
        return RULE_RESULT_BYPASSED;
    }

    /* Get product and directory structure in use */
    if ( myki_br_GetCardProduct( pMYKI_TAControl->ProductInUse, &pDirectory, &pProduct ) < 0 )
    {
        CsErrx( "BR_LLSC_4_10 : myki_br_GetCardProduct( %d ) failed", pMYKI_TAControl->ProductInUse );
        return RULE_RESULT_ERROR;
    }

    //
    //  2.  The service provider(3)  of the scan on is not equal to the service provider of the device(8) .
    //
    CsDbg( BRLL_RULE, "BR_LLSC_4_10 : LastUsage.ProviderId: %d, StaticData.serviceProviderId:%d", pProduct->LastUsage.ProviderId,  pData->StaticData.serviceProviderId);
    if ( pProduct->LastUsage.ProviderId == pData->StaticData.serviceProviderId )
    {
        CsDbg( BRLL_RULE, "BR_LLSC_4_10 : Bypass - Scan on provider (%d) matches device provider", pProduct->LastUsage.ProviderId );
        return RULE_RESULT_BYPASSED;
    }

    //
    //  3.  The mode of transport(9)  of the provisional product (as determined by
    //      the product in use) is not equal to Rail(9)
    //
    transportMode = myki_br_cd_GetTransportModeForProvider( pProduct->LastUsage.ProviderId );
    if ( transportMode == TRANSPORT_MODE_UNKNOWN )
    {
        CsErrx( "BR_LLSC_4_10 : myki_br_cd_GetTransportModeForProvider. Unknown mode for: %d", pProduct->LastUsage.ProviderId );
        return RULE_RESULT_ERROR;
    }

    if ( transportMode == TRANSPORT_MODE_RAIL )
    {
        CsDbg( BRLL_CHOICE, "BR_LLSC_4_10 : Bypass - Scan on transport mode is RAIL" );
        return RULE_RESULT_BYPASSED;
    }

    //
    //  Description
    //  1. Set the FareRouteID7 to none - no route will be considered for pricing
    //
    pData->DynamicData.fareRouteIdIsValid = FALSE;

    //
    //  2.  Perform a Forced Scan-Off.
    //
    //  It appears that this is not actually done here, but it will be done elsewhere
    //  in the processing sequence.
    //

    CsDbg( BRLL_RULE, "BR_LLSC_4_10 : Executed" );
    return RULE_RESULT_EXECUTED;
}

