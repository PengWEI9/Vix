/* -*- mode: c; tabs: 4; -*- */
/*=============================================================================
**
**    Vix Technology                   Licensed software
**    (C) 2014                         All rights reserved
**
**=============================================================================
**
**  Project/Product : MBU
**  Filename        : BR_LLSC_4_15.c
**  Author(s)       : ???
**
**  ID              : BR_LLSC_4_15 - NTS0177 v7.3
**
**  Name            :  Missing Rail Scan-Off Zone 0/1 - Penalty
**
**  Data Fields     :
**
**      1.  TAppControl.ProductInUse
**      2.  TAppTProduct.LastUsageZone 
**      3.  TAppTProduct.LastUsageLineId 
**      4.  TAppTProduct.LastUsageProviderId 
**      5.  Dynamic.CurrentZone 
**      6.  Static.TransportMode 
**      7.  Dynamic.RouteChangeover 
**      8.  Tariff.ProvisionalCappingOption 
**      9.  Dynamic.FareRouteID 
**
**
**  Pre-Conditions  :
**
**      1.  The product in use field(1) is not 0 (a product is in a scanned-on
**          state).
**      2.  The RouteChangeover field(10) indicates no route changeover
**          condition.
**      3.  The mode of transport as determined by the scan-on service provider(5)
**          of the product in use(1) is equal to rail
**      4.  The mode of transport of the device(6) is not equal to rail
**      5.  The zone of origin(2) or the current zone is physical zone 0 or 1.
**      6.  The Provisional fare capping option(8) is  set to 'Option 3'
**
**  Description     :
**
**      1.  Set the FareRouteID(9) to none - no route will be considered for
**          pricing
**      2.  A forced Scan-off Condition determined.
**
**  Post-Conditions :
**
**      1.  A forced scan off is performed.
**
**  Devices         :
**
**      Fare payment devices
*
**  Member(s)       :
**      BR_LLSC_4_15            [public]    business rule
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
**    1.01  24.06.14    ANT   Add      Implemented the business rule
**
**===========================================================================*/

#include <cs.h>                 // For debug logging
#include <myki_cardservices.h>  // Myki Card Services
#include <LDT.h>                // Logical Device Transaction

#include "myki_br_rules.h"      // API for all BR_xxx modules
#include "BR_Common.h"          // Common utilities, macros, etc

/*==========================================================================*
**
**  BR_LLSC_4_15
**
**  Description     :
**      Implements business rule BR_LLSC_4_15.
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

RuleResult_e BR_LLSC_4_15( MYKI_BR_ContextData_t *pData )
{
    MYKI_TAControl_t       *pMYKI_TAControl     = NULL;
    MYKI_Directory_t       *pMYKI_Directory     = NULL;
    MYKI_TAProduct_t       *pMYKI_TAProduct     = NULL;

    CsDbg( BRLL_RULE, "BR_LLSC_4_15 : Start (Missing Rail Scan-Off Zone 0/1 - Penalty)" );

    if ( pData == NULL )
    {
        CsErrx( "BR_LLSC_4_15 : Invalid argument(s)" );
        return RULE_RESULT_ERROR;
    }

    if ( MYKI_CS_TAControlGet( &pMYKI_TAControl ) < 0 )
    {
        CsErrx( "BR_LLSC_4_15 : MYKI_CS_TAControlGet() failed" );
        return RULE_RESULT_ERROR;
    }

    /*  PRE-CONDITIONS */
    {
        /*  1.  The product in use field(1) is not 0 (a product is in a scanned-on state). */
        if ( pMYKI_TAControl->ProductInUse == 0 )
        {
            CsDbg( BRLL_RULE, "BR_LLSC_4_15 : Bypassed - No products in use" );
            pData->ReturnedData.bypassCode  = BYPASS_CODE( 4, 15, 1, 0 );
            return RULE_RESULT_BYPASSED;
        }

        if ( myki_br_GetCardProduct( pMYKI_TAControl->ProductInUse, &pMYKI_Directory, &pMYKI_TAProduct ) < 0 )
        {
            CsErrx( "BR_LLSC_4_15 : myki_br_GetCardProduct(%d) failed", pMYKI_TAControl->ProductInUse );
            return RULE_RESULT_ERROR;
        }

        /*  2.  The RouteChangeover field[(10)](7) indicates no route changeover condition. */
        if ( pData->DynamicData.routeChangeover != ROUTE_CHANGEOVER_NONE )
        {
            CsDbg( BRLL_RULE, "BR_LLSC_4_15 : Bypassed - Route change over condition (%d) defined", pData->DynamicData.routeChangeover );
            pData->ReturnedData.bypassCode  = BYPASS_CODE( 4, 15, 2, 0 );
            return RULE_RESULT_BYPASSED;
        }

        /*  3.  The mode of transport as determined by the scan-on service provider(5) of the 
                product in use(1) is equal to rail */
        if ( myki_br_cd_GetTransportModeForProvider( pMYKI_TAProduct->LastUsage.ProviderId ) != TRANSPORT_MODE_RAIL )
        {
            CsDbg( BRLL_RULE, "BR_LLSC_4_15 : Bypassed - Scan on transport mode is not RAIL" );
            pData->ReturnedData.bypassCode  = BYPASS_CODE( 4, 15, 3, 0 );
            return RULE_RESULT_BYPASSED;
        }

        /*  4.  The mode of transport of the device(6) is not equal to rail */
        if ( pData->InternalData.TransportMode == TRANSPORT_MODE_RAIL )
        {
            CsDbg( BRLL_RULE, "BR_LLSC_4_15 : Bypassed - Device transport mode is RAIL" );
            pData->ReturnedData.bypassCode  = BYPASS_CODE( 4, 15, 4, 0 );
            return RULE_RESULT_BYPASSED;
        }

        /*  5.  The zone of origin(2) o[r]f the current zone is physical zone 0 or 1. */
        if ( myki_br_isPhysicalZoneOneOrZero( pMYKI_TAProduct->LastUsage.Zone ) == FALSE )
        {
            CsDbg( BRLL_RULE, "BR_LLSC_4_15 : Bypassed - Zone of origin is neither physical zone 0 nor 1" );
            pData->ReturnedData.bypassCode  = BYPASS_CODE( 4, 15, 5, 0 );
            return RULE_RESULT_BYPASSED;
        }

        /*  6.  The Provisional fare capping option(8) is set to 'Option 3' */
        if ( pData->Tariff.provisionalCappingOption != PROVISIONAL_CAPPING_OPTION_3 )
        {
            CsDbg( BRLL_RULE, "BR_LLSC_4_15 : Bypassed - Provisional fare capping option is not 'Option 3'" );
            pData->ReturnedData.bypassCode  = BYPASS_CODE( 4, 15, 6, 0 );
            return RULE_RESULT_BYPASSED;
        }
    }

    /*  PROCESSING */
    {
        /*  1.  Set the FareRouteID(9) to none - no route will be considered for pricing */
        pData->DynamicData.fareRouteId          = 0;
        pData->DynamicData.fareRouteIdIsValid   = FALSE;

        /*  2.  A forced Scan-off Condition determined. */
    }

    return RULE_RESULT_EXECUTED;
}   /*  BR_LLSC_4_15( ) */
