/* -*- mode: c; tabs: 4; -*- */
/*=============================================================================
**
**    Vix Technology                   Licensed software
**    (C) 2013                         All rights reserved
**
**=============================================================================
**
**  Project/Product : MBU
**  Filename        : BR_LLSC_4_11.c
**  Author(s)       : ?
**
**  ID              : BR_LLSC_4_11 - NTS0177 v7.2
**
**  Name            : Set Route Changeover Status
**
**  Data Fields     :
**
**      1.  TAppControl.ProductInUse
**      2.  Dynamic.RouteChangeover
**      3.  Static.ModeOfTransport
**      4.  Dynamic.OriginatingInformation
**
**  Pre-Conditions  :
**      1.  The product in use field1 is not 0 (a product is in a scanned-on state).
**
**  Description     :
**      1.  Set the route change over status2 to None.
**
**      <UPDATED_SPECIICATION>
**      2.  If either the Mode of transport3 of the device is not Rail OR
**          if the service provider of the provisional product (as determined
**          by the product in use1) is  not  Rail8 then
**          Populate originiating25 route information – which is used at
**          scan off; from the card: Store service provider8, entry point6,
**          route id5, and stop id7.
**      </UPDATED_SPECIICATION>
**
**      <ORIGINAL_SPECIFICATION>
**      2.	If the Mode of transport3 of the device is not Rail and the service
**          provider of the provisional product (as determined by the product in use1)
**          is also not Rail8 then
**          Populate originiating25 route information - which is used at scan off;
**          from the card:  Store  service provider8, entry point6, route id5, and stop id7
**      </ORIGINAL_SPECIFICATION>
**
**  Post-Conditions :
**      1.  Route Changeover Condition is determined and stored in RouteChangeover field5.
**
**  Devices         :
**      Fare payment devices
**
**  Note            :
**      The term "Route Changeover" refers to the following scenario: A tram
**      (or possibly bus) is traveling along its route and it gets to the end of
**      the line. The TDC/BDC will offer (or auto-select) the next trip/route to
**      the driver who may then confirm it. In the case where patrons are getting
**      on at the same time as others are getting off, then either the boarding
**      patrons will be scannedon to the old route, or the alighting patrons will
**      scan-off when it is on the new route (depending on whether the driver has
**      changed trips/routes already). A patron boarding with a scan-on route
**      different from the scanoff route with locations other than the first and
**      last stop is tracked based on the vehicle id. The route considered to
**      determine the fare will be the scanoff route and the scanon location would
**      be first stop of the scanoff route.
**
**  Member(s)       :
**      BR_LLSC_4_11            [public]    business rule
**
**  Information     :
**   Compiler(s)    : C
**   Target(s)      : Independent
**
**  History         :
**   Vers.  Date        Aut.  Type     Description
**   -----  ----------  ----  -------  ----------------------------------------
**    1.00  ??.??.13    ?     Create
**    1.01  05.02.14    ANT   Modify   Implemented updated specification
**
**===========================================================================*/

#include <cs.h>                 // For debug logging
#include <myki_cardservices.h>  // Myki Card Services
#include <LDT.h>                // Logical Device Transaction

#include "myki_br_rules.h"      // API for all BR_xxx modules
#include "BR_Common.h"          // Common utilities, macros, etc

//=============================================================================
//
//  Determine whether a changeover condition exists (see NTS0177, BR_LLSC_4_11).
//
//  On rail, route changeover does not apply so this rule is irrelevant, and
//  since routeChangeover is set to NONE by default, then this rule can
//  simply return. Executed or Bypassed doesn't matter since it's ignored
//  by the calling sequence anyway.
//
//=============================================================================

RuleResult_e BR_LLSC_4_11( MYKI_BR_ContextData_t *pData )
{
    MYKI_TAControl_t   *pMYKI_TAControl         = NULL;
    MYKI_Directory_t   *pMYKI_DirectoryInUse    = NULL;
    MYKI_TAProduct_t   *pMYKI_TAProductInUse    = NULL;

    CsDbg( BRLL_RULE, "BR_LLSC_4_11 : Start (Set Route Changeover Status)" );

    if ( ! pData )
    {
        CsErrx( "BR_LLSC_4_11 : Invalid argument(s)" );
        return RULE_RESULT_ERROR;
    }

    if ( MYKI_CS_TAControlGet( &pMYKI_TAControl ) )
    {
        CsErrx( "BR_LLSC_4_11 : MYKI_CS_TAControlGet() failed" );
        return RULE_RESULT_ERROR;
    }

    //  Pre-Conditions
    {
        //  1.  The product in use field1 is not 0 (a product is in a scanned-on state).
        if ( pMYKI_TAControl->ProductInUse == 0 )
        {
            CsDbg( BRLL_RULE, "BR_LLSC_4_11 : Bypass - No product in use" );
            return RULE_RESULT_BYPASSED;
        }
    }

    //  Description
    {
        if ( myki_br_GetCardProduct( pMYKI_TAControl->ProductInUse, &pMYKI_DirectoryInUse, &pMYKI_TAProductInUse ) < 0 )
        {
            CsErrx( "BR_LLSC_4_11 : myki_br_GetCardProduct(%d) failed", pMYKI_TAControl->ProductInUse );
            return RULE_RESULT_ERROR;
        }

        //  1.  Set the route change over status(2) to None.
        pData->DynamicData.routeChangeover = ROUTE_CHANGEOVER_NONE;

        //  *** UPDATED SPECIFICATION
        //  2.  If either the Mode of transport3 of the device is not Rail OR
        //      if the service provider of the provisional product (as determined
        //      by the product in use1) is  not  Rail8 then
        //      Populate originiating25 route information – which is used at
        //      scan off; from the card: Store service provider8, entry point6,
        //      route id5, and stop id7.
        if
        (
            pData->InternalData.TransportMode != TRANSPORT_MODE_RAIL ||
            myki_br_cd_GetTransportModeForProvider( pMYKI_TAProductInUse->LastUsage.ProviderId ) != TRANSPORT_MODE_RAIL
        )
        {
            pData->DynamicData.originatingInformation.serviceProviderId = \
                    pMYKI_TAProductInUse->LastUsage.ProviderId;
            memcpy( &pData->DynamicData.originatingInformation.location,
                    &pMYKI_TAProductInUse->LastUsage.Location,
                    sizeof( MYKI_Location_t ) );
            pData->DynamicData.isOriginatingInformationSet  = TRUE;
        }
    }

    CsDbg( BRLL_RULE, "BR_LLSC_4_11 : Executed (Rail)" );
    return RULE_RESULT_EXECUTED;
}

