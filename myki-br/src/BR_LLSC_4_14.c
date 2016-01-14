/* -*- mode: c; tabs: 4; -*- */
/*=============================================================================
**
**    Vix Technology                   Licensed software
**    (C) 2014                         All rights reserved
**
**=============================================================================
**
**  Project/Product : MBU
**  Filename        : BR_LLSC_4_14.c
**  Author(s)       : ???
**
**  ID              : BR_LLSC_4_14 - NTS0177 v7.3
**
**  Name            :  Missing Rail Scan-Off Not Zone 0/1
**
**  Data Fields     :
**
**      1.  TAppControl.ProductInUse 
**      2.  TAppTProduct.LastUsageZone 
**      3.  TAppTProduct.LastUsageProviderId 
**      4.  Static.TransportMode 
**      5.  Dynamic.CurrentZone 
**      6.  TAppTProduct.EndDateTime 
**      7.  Dynamic.FareRouteID
**
**  Pre-Conditions  :
**
**      1.  The product in use field(1) is not 0 (a product is in a scanned-on
**          state).
**      2.  The service provider of the scan on is not equal to the service
**          provider of the device.
**      3.  The service provider of the provisional product(3) (as determined
**          by the product in use(1)) was a Rail service provider and the zone
**          of origin(2) is not physical zone 0 or 1.
**      4.  The service provider of the provisional product(3) (as determined
**          by the product in use(1)) was a Rail service provider and the
**          current device is not a Rail device(4)
**
**
**  Description     :
**      1.  Set the FareRouteID(7) to none - no route will be considered for
**          pricing
**      2.  Perform a Forced Scan-Off.
**
**  Post-Conditions
**      1.  A forced scan-off is performed.
**
**  Devices         :
**      Fare payment devices
**
**  Member(s)       :
**      BR_LLSC_4_14            [public]    business rule
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
**  BR_LLSC_4_14
**
**  Description     :
**      Implements business rule BR_LLSC_4_14.
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

RuleResult_e BR_LLSC_4_14( MYKI_BR_ContextData_t *pData )
{
    MYKI_TAControl_t       *pMYKI_TAControl     = NULL;
    MYKI_Directory_t       *pMYKI_Directory     = NULL;
    MYKI_TAProduct_t       *pMYKI_TAProduct     = NULL;

    CsDbg( BRLL_RULE, "BR_LLSC_4_14 : Start (Missing Rail Scan-Off Not Zone 0/1)" );

    if ( pData == NULL )
    {
        CsErrx( "BR_LLSC_4_14 : Invalid argument(s)" );
        return RULE_RESULT_ERROR;
    }

    if ( MYKI_CS_TAControlGet( &pMYKI_TAControl ) < 0 )
    {
        CsErrx( "BR_LLSC_4_14 : MYKI_CS_TAControlGet() failed" );
        return RULE_RESULT_ERROR;
    }
  
    /*  PRE-CONDITIONS */
    {
        /*  1.  The product in use field(1) is not 0 (a product is in a scanned-on state). */
        if ( pMYKI_TAControl->ProductInUse == 0 )
        {
            CsDbg( BRLL_RULE, "BR_LLSC_4_14 : Bypassed - No products in use" );
            pData->ReturnedData.bypassCode  = BYPASS_CODE( 4, 14, 1, 0 );
            return RULE_RESULT_BYPASSED;
        }

        if ( myki_br_GetCardProduct( pMYKI_TAControl->ProductInUse, &pMYKI_Directory, &pMYKI_TAProduct ) < 0 )
        {
            CsErrx( "BR_LLSC_4_14 : myki_br_GetCardProduct(%d) failed", pMYKI_TAControl->ProductInUse );
            return RULE_RESULT_ERROR;
        }

        /*  2.  The service provider of the scan on is not equal to the service provider of the device. */
        if ( pMYKI_TAProduct->LastUsage.ProviderId == pData->StaticData.serviceProviderId )
        {
            CsDbg( BRLL_RULE, "BR_LLSC_4_14 : Bypassed - Scan on provider (%d) matches device provider", pMYKI_TAProduct->LastUsage.ProviderId );
            pData->ReturnedData.bypassCode  = BYPASS_CODE( 4, 14, 2, 0 );
            return RULE_RESULT_BYPASSED;
        }

        /*  3.  The service provider of the provisional product(3) (as determined by the product in use(1))
                was a Rail service provider */
        if ( myki_br_cd_GetTransportModeForProvider( pMYKI_TAProduct->LastUsage.ProviderId ) != TRANSPORT_MODE_RAIL )
        {
            CsDbg( BRLL_RULE, "BR_LLSC_4_14 : Bypassed - Scan on transport mode is not RAIL" );
            pData->ReturnedData.bypassCode  = BYPASS_CODE( 4, 14, 3, 0 );
            return RULE_RESULT_BYPASSED;
        }

        /*      and the zone of origin(2) is not physical zone 0 or 1. */
        if ( myki_br_isPhysicalZoneOneOrZero( pMYKI_TAProduct->LastUsage.Zone ) != FALSE )
        {
            CsDbg( BRLL_RULE, "BR_LLSC_4_14 : Bypassed - Zone of origin is physical zone 0 or 1" );
            pData->ReturnedData.bypassCode  = BYPASS_CODE( 4, 14, 3, 1 );
            return RULE_RESULT_BYPASSED;
        }

        /*  4.  The service provider of the provisional product(3) (as determined by the product in use(1))
                was a Rail service provider and the current device is not a Rail device(4) */
        if ( pData->InternalData.TransportMode == TRANSPORT_MODE_RAIL )
        {
            CsDbg( BRLL_RULE, "BR_LLSC_4_14 : Bypassed - Device transport mode is RAIL" );
            pData->ReturnedData.bypassCode  = BYPASS_CODE( 4, 14, 4, 0 );
            return RULE_RESULT_BYPASSED;
        }
    }

    /*  PROCESSING */
    {
        /*  1.  Set the FareRouteID(7) to none - no route will be considered for pricing */
        pData->DynamicData.fareRouteId          = 0;
        pData->DynamicData.fareRouteIdIsValid   = FALSE;

        /*  2.  Perform a Forced Scan-Off. Done in SEQ_LLSC_10 */
    }

    return RULE_RESULT_EXECUTED;
}   /*  BR_LLSC_4_14( ) */
