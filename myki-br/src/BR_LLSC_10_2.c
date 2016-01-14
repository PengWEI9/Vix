/**************************************************************************
*   ID :  BR_LLSC_10_2
*
*    Travel Access Pass -invalidate Provisional
*
*    1.  TAppControl.ProductInUse
*    2.  TAppTProduct.LastUsageZone
*    3.  TAppTProduct.LastUsageLineId
*    4.  TAppTProduct.LastUsageProviderId
*    5.  Dynamic.CurrentZone
*    6.  Static.TransportMode
*    7.  Dynamic.RouteChangeover
*    8.  Tariff.ProvisionalCappingOption
*    9.  Dynamic.FareRouteID
*    10.  TApp.PassengerControl
*    11.  Tariff.ConcessionType.isDDA
*
*
*    1.  The product control bit map indicates that there is a provisional product on the card
*    2.   The passenger code(10) is listed in the concession types as DDA applicable(2) (true) .
*
*      Description
*    1.  Using the current location perform a ProductSale/Upgrade
*    a.  Set the provisional bit on the product sale bit map to false
*    b.  Set the purchase value to 0.00
*
*    2.  Using the current location perform a ProductUseage/Scan off
*    a.  Set the date time to the lesser of the current date time or the product expiry
*    b.  Using the current location to get the Route/Stop/Entry Point
*
*    3.  Perform a ProductUpdate/Invalidate on the product in use
*
*
*      Post-Conditions
*    DDA determination completed
*
*      Devices
*    All validation devices
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

RuleResult_e BR_LLSC_10_2( MYKI_BR_ContextData_t *pData )
{
    CsDbg( BRLL_RULE, "BR_LLSC_10_2 : Start (Travel Access Pass - invalidate Provisional)" );

    if ( ! pData )
    {
        CsErrx( "BR_LLSC_10_2 : Invalid argument(s)" );
        return RULE_RESULT_ERROR;
    }

    CsDbg( BRLL_RULE, "BR_LLSC_10_2 : Not referenced by any sequence - returning Bypassed" );
    return RULE_RESULT_BYPASSED;
}


