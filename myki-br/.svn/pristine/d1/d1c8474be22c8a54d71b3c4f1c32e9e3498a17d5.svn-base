/* -*- mode: c; tabs: 4; -*- */
/*=============================================================================
**
**    Vix Technology                   Licensed software
**    (C) 2014                         All rights reserved
**
**=============================================================================
**
**  Project/Product : MBU
**  Filename        : test_BR_LLSC_6_1.c
**  Author(s)       : An Tran
**
**  Description     :
**      Implements BR_LLSC_6_1 business rule unit-tests.
**
**  Function(s)     :
**      test_BR_LLSC_6_1_XXX        [Public]    unit-test functions
**
**  Information     :
**   Compiler(s)    : ANSI C
**   Target(s)      : Independent
**
**  Subversion      :
**      $Id: $
**      $HeadURL: $
**
**  History         :
**   Vers.  Date        Aut.  Type     Description
**   -----  ----------  ----  -------  ----------------------------------------
**    1.00  26.06.14    ANT   Create
**
**===========================================================================*/

/*
 *      Options
 *      -------
 */

/*
 *      Includes
 *      --------
 */

#include <cs.h>
#include <myki_cardservices.h>
#include <myki_cdd_enums.h>
#include <myki_br.h>
#include <myki_br_rules.h>
#include <myki_br_context_data.h>

#include "test_common.h"

/*
 *      Constants and Macros
 *      --------------------
 */

#define RouteId_1           1
#define RouteStops_1        5
#define RouteId_2           2
#define RouteStops_2        3

/*==========================================================================*
**
**  test_BR_LLSC_6_1_001
**
**  Description     :
**      Unit-test BYPASSED conditions.
**
**  Parameters      :
**      pData           [I/O]   BR context data
**
**  Returns         :
**      TRUE                    test passed
**      FALSE                   test failed
**
**  Notes           :
**
**
**==========================================================================*/

int test_BR_LLSC_6_1_001( MYKI_BR_ContextData_t *pData )
{
    MYKI_CAControl_t       *pMYKI_CAControl         = NULL;
    MYKI_TAControl_t       *pMYKI_TAControl         = NULL;
    MYKI_Directory_t       *pMYKI_DirectoryInUse    = NULL;
    MYKI_TAProduct_t       *pMYKI_TAProductInUse    = NULL;
    int                     ProductInUse            = 1;
    U8_t                    IssuerId                = 13;
    U8_t                    ProductId_NHOUR         = 4;
    U8_t                    OriginZone              = 2;

    if ( pData == NULL )
    {
        return FALSE;
    }

    if ( MYKI_CS_CAControlGet( &pMYKI_CAControl ) < 0 )
    {
        return FALSE;
    }
    pMYKI_CAControl->Status                       = CARD_CONTROL_STATUS_ACTIVATED;
    pMYKI_CAControl->ExpiryDate                   = pData->DynamicData.currentBusinessDate + 1;

    if ( MYKI_CS_TAControlGet( &pMYKI_TAControl ) < 0 || pMYKI_TAControl == NULL )
    {
        return FALSE;
    }

    pMYKI_DirectoryInUse    = &pMYKI_TAControl->Directory[ ProductInUse ];
    if ( MYKI_CS_TAProductGet( ProductInUse - 1, &pMYKI_TAProductInUse ) < 0 )
    {
        return FALSE;
    }

    /*  1.  The product in use field(1) is not 0 (a product is in a scanned-on state). */
    {
        pMYKI_TAControl->ProductInUse               = 0;
        if ( BR_LLSC_6_1( pData ) != RULE_RESULT_BYPASSED ||
             pData->ReturnedData.bypassCode != BYPASS_CODE( 6, 1, 1, 0 ) )
        {
            CsErrx( "Pre-condition 1 failed" );
            return FALSE;
        }
    }

    /*  2.  The current mode of transport(22) of the current device is not equal to Rail */
    {
        pMYKI_TAControl->ProductInUse               = ProductInUse;
        pMYKI_DirectoryInUse->SerialNo              = 1;
        pMYKI_DirectoryInUse->Status                = TAPP_CONTROL_DIRECTORY_STATUS_ACTIVATED;
        pMYKI_DirectoryInUse->IssuerId              = IssuerId;
        pMYKI_DirectoryInUse->ProductId             = ProductId_NHOUR;
        pMYKI_TAProductInUse->LastUsage.ProviderId  = ProviderId_BUS;
        pMYKI_TAProductInUse->LastUsage.Zone        = OriginZone;
        pData->StaticData.serviceProviderId         = ProviderId_RAIL;
        pData->InternalData.TransportMode           = TRANSPORT_MODE_RAIL;
        if ( BR_LLSC_6_1( pData ) != RULE_RESULT_BYPASSED ||
             pData->ReturnedData.bypassCode != BYPASS_CODE( 6, 1, 2, 0 ) )
        {
            CsErrx( "Pre-condition 2 failed" );
            return FALSE;
        }
    }

    return TRUE;
}   /* test_BR_LLSC_6_1_001( ) */

/*==========================================================================*
**
**  test_BR_LLSC_6_1_002
**
**  Description     :
**      Unit-test EXECUTED conditions
**      Same Zone/Route/Stop
**
**  Parameters      :
**      pData           [I/O]   BR context data
**
**  Returns         :
**      TRUE                    test passed
**      FALSE                   test failed
**
**  Notes           :
**
**
**==========================================================================*/

int test_BR_LLSC_6_1_002( MYKI_BR_ContextData_t *pData )
{
    MYKI_CAControl_t       *pMYKI_CAControl         = NULL;
    MYKI_TAControl_t       *pMYKI_TAControl         = NULL;
    MYKI_Directory_t       *pMYKI_DirectoryInUse    = NULL;
    MYKI_TAProduct_t       *pMYKI_TAProductInUse    = NULL;
    int                     ProductInUse            = 1;
    U8_t                    IssuerId                = 13;
    U8_t                    ProductId_NHOUR         = 4;
    U8_t                    OriginZone              = 2;
    U16_t                   OriginRouteId           = RouteId_1;
    U8_t                    OriginStopId            = 1;
    U8_t                    DestinationZone         = OriginZone;
    U16_t                   DestinationRouteId      = OriginRouteId;
    U8_t                    DestinationStopId       = OriginStopId;
    U16_t                   HeadlessRouteId         = 10;

    if ( pData == NULL )
    {
        return FALSE;
    }

    if ( MYKI_CS_CAControlGet( &pMYKI_CAControl ) < 0 )
    {
        return FALSE;
    }
    pMYKI_CAControl->Status                       = CARD_CONTROL_STATUS_ACTIVATED;
    pMYKI_CAControl->ExpiryDate                   = pData->DynamicData.currentBusinessDate + 1;

    if ( MYKI_CS_TAControlGet( &pMYKI_TAControl ) < 0 || pMYKI_TAControl == NULL )
    {
        return FALSE;
    }

    pMYKI_DirectoryInUse    = &pMYKI_TAControl->Directory[ ProductInUse ];
    if ( MYKI_CS_TAProductGet( ProductInUse - 1, &pMYKI_TAProductInUse ) < 0 )
    {
        return FALSE;
    }

    {
        pMYKI_TAControl->ProductInUse                       = ProductInUse;
        pMYKI_DirectoryInUse->SerialNo                      = 1;
        pMYKI_DirectoryInUse->Status                        = TAPP_CONTROL_DIRECTORY_STATUS_ACTIVATED;
        pMYKI_DirectoryInUse->IssuerId                      = IssuerId;
        pMYKI_DirectoryInUse->ProductId                     = ProductId_NHOUR;
        pMYKI_TAProductInUse->LastUsage.ProviderId          = ProviderId_BUS;
        pMYKI_TAProductInUse->LastUsage.Zone                = OriginZone;
        pMYKI_TAProductInUse->LastUsage.Location.RouteId    = OriginRouteId;
        pMYKI_TAProductInUse->LastUsage.Location.StopId     = OriginStopId;

        /*  StaticData */
        {
            pData->StaticData.serviceProviderId             = ProviderId_BUS;
        }
        /*  InternalData */
        {
            pData->InternalData.TransportMode               = TRANSPORT_MODE_BUS;
        }
        /*  DynamicData */
        {
            pData->DynamicData.isEarlyBirdTrip              = TRUE;
            pData->DynamicData.currentZone                  = DestinationZone;
            pData->DynamicData.lineId                       = DestinationRouteId;
            pData->DynamicData.stopId                       = DestinationStopId;
            pData->DynamicData.fareRouteIdIsValid           = FALSE;
        }
        /*  Tariff */
        {
            pData->Tariff.headlessRouteId                   = HeadlessRouteId;
        }

        if ( BR_LLSC_6_1( pData ) != RULE_RESULT_EXECUTED )
        {
            CsErrx( "Not executed" );
            return FALSE;
        }

        /*  3.  Set the Current Trip Direction12 to Unknown. */
        if ( pData->DynamicData.currentTripDirection != TRIP_DIRECTION_UNKNOWN )
        {
            CsErrx( "Wrong trip direction" );
            return FALSE;
        }

        /*  6.a.i.(1) Add [then] the current location3 to the list of the locations24 twice. */
        if ( pData->DynamicData.transportLocationsCount != 2 )
        {
            CsErrx( "Wrong transport locations" );
            return FALSE;
        }

        /*  6.a.i.(2) Set the Fare Route to use23 to current route */
        if ( pData->DynamicData.fareRouteIdIsValid == FALSE                     ||
             pData->DynamicData.fareRouteId        != pData->DynamicData.lineId )
        {
            CsErrx( "Wrong FareRouteId" );
            return FALSE;
        }
    }

    return TRUE;
}   /* test_BR_LLSC_6_1_002( ) */

/*==========================================================================*
**
**  test_BR_LLSC_6_1_003
**
**  Description     :
**      Unit-test EXECUTED conditions
**      OriginZone < DestinationZone
**      OriginRoute = DestinationRoute
**      OriginStop < DestinationStop
**
**  Parameters      :
**      pData           [I/O]   BR context data
**
**  Returns         :
**      TRUE                    test passed
**      FALSE                   test failed
**
**  Notes           :
**
**
**==========================================================================*/

int test_BR_LLSC_6_1_003( MYKI_BR_ContextData_t *pData )
{
    MYKI_CAControl_t       *pMYKI_CAControl         = NULL;
    MYKI_TAControl_t       *pMYKI_TAControl         = NULL;
    MYKI_Directory_t       *pMYKI_DirectoryInUse    = NULL;
    MYKI_TAProduct_t       *pMYKI_TAProductInUse    = NULL;
    int                     ProductInUse            = 1;
    U8_t                    IssuerId                = 13;
    U8_t                    ProductId_NHOUR         = 4;
    U8_t                    OriginZone              = 2;
    U16_t                   OriginRouteId           = RouteId_1;
    U8_t                    OriginStopId            = 1;
    U8_t                    DestinationZone         = 3;
    U16_t                   DestinationRouteId      = OriginRouteId;
    U8_t                    DestinationStopId       = 3;
    U16_t                   HeadlessRouteId         = 10;

    if ( pData == NULL )
    {
        return FALSE;
    }

    if ( MYKI_CS_CAControlGet( &pMYKI_CAControl ) < 0 )
    {
        return FALSE;
    }
    pMYKI_CAControl->Status                       = CARD_CONTROL_STATUS_ACTIVATED;
    pMYKI_CAControl->ExpiryDate                   = pData->DynamicData.currentBusinessDate + 1;

    if ( MYKI_CS_TAControlGet( &pMYKI_TAControl ) < 0 || pMYKI_TAControl == NULL )
    {
        return FALSE;
    }

    pMYKI_DirectoryInUse    = &pMYKI_TAControl->Directory[ ProductInUse ];
    if ( MYKI_CS_TAProductGet( ProductInUse - 1, &pMYKI_TAProductInUse ) < 0 )
    {
        return FALSE;
    }

    {
        pMYKI_TAControl->ProductInUse                       = ProductInUse;
        pMYKI_DirectoryInUse->SerialNo                      = 1;
        pMYKI_DirectoryInUse->Status                        = TAPP_CONTROL_DIRECTORY_STATUS_ACTIVATED;
        pMYKI_DirectoryInUse->IssuerId                      = IssuerId;
        pMYKI_DirectoryInUse->ProductId                     = ProductId_NHOUR;
        pMYKI_TAProductInUse->LastUsage.ProviderId          = ProviderId_BUS;
        pMYKI_TAProductInUse->LastUsage.Zone                = OriginZone;
        pMYKI_TAProductInUse->LastUsage.Location.RouteId    = OriginRouteId;
        pMYKI_TAProductInUse->LastUsage.Location.StopId     = OriginStopId;

        /*  StaticData */
        {
            pData->StaticData.serviceProviderId             = ProviderId_BUS;
        }
        /*  InternalData */
        {
            pData->InternalData.TransportMode               = TRANSPORT_MODE_BUS;
        }
        /*  DynamicData */
        {
            pData->DynamicData.isEarlyBirdTrip              = TRUE;
            pData->DynamicData.currentZone                  = DestinationZone;
            pData->DynamicData.lineId                       = DestinationRouteId;
            pData->DynamicData.stopId                       = DestinationStopId;
            pData->DynamicData.fareRouteIdIsValid           = FALSE;
        }
        /*  Tariff */
        {
            pData->Tariff.headlessRouteId                   = HeadlessRouteId;
        }

        if ( BR_LLSC_6_1( pData ) != RULE_RESULT_EXECUTED )
        {
            CsErrx( "Not executed" );
            return FALSE;
        }

        /*  5.  else if the OriginZone is less than DestinationZone then:
                a.  set the current trip direction12 to Outbound */
        if ( pData->DynamicData.currentTripDirection != TRIP_DIRECTION_OUTBOUND )
        {
            CsErrx( "Wrong trip direction" );
            return FALSE;
        }

        /*  6.a.ii.(1) Add all locations from the scan on stop to the scan off stop location to the list of transport locations24 */
        if ( pData->DynamicData.transportLocationsCount != 3 /* 1-3 */ )
        {
            CsErrx( "Wrong transport locations" );
            return FALSE;
        }

        /*  6.a.ii.(2) Set the Fare Route to use23 to current route */
        if ( pData->DynamicData.fareRouteIdIsValid == FALSE                     ||
             pData->DynamicData.fareRouteId        != pData->DynamicData.lineId )
        {
            CsErrx( "Wrong FareRouteId" );
            return FALSE;
        }
    }

    return TRUE;
}   /* test_BR_LLSC_6_1_003( ) */

/*==========================================================================*
**
**  test_BR_LLSC_6_1_004
**
**  Description     :
**      Unit-test EXECUTED conditions
**      OriginZone > DestinationZone
**      OriginRoute = DestinationRoute
**      OriginStop > DestinationStop
**
**  Parameters      :
**      pData           [I/O]   BR context data
**
**  Returns         :
**      TRUE                    test passed
**      FALSE                   test failed
**
**  Notes           :
**
**
**==========================================================================*/

int test_BR_LLSC_6_1_004( MYKI_BR_ContextData_t *pData )
{
    MYKI_CAControl_t       *pMYKI_CAControl         = NULL;
    MYKI_TAControl_t       *pMYKI_TAControl         = NULL;
    MYKI_Directory_t       *pMYKI_DirectoryInUse    = NULL;
    MYKI_TAProduct_t       *pMYKI_TAProductInUse    = NULL;
    int                     ProductInUse            = 1;
    U8_t                    IssuerId                = 13;
    U8_t                    ProductId_NHOUR         = 4;
    U8_t                    OriginZone              = 3;
    U16_t                   OriginRouteId           = RouteId_1;
    U8_t                    OriginStopId            = 3;
    U8_t                    DestinationZone         = 2;
    U16_t                   DestinationRouteId      = OriginRouteId;
    U8_t                    DestinationStopId       = 2;
    U16_t                   HeadlessRouteId         = 10;

    if ( pData == NULL )
    {
        return FALSE;
    }

    if ( MYKI_CS_CAControlGet( &pMYKI_CAControl ) < 0 )
    {
        return FALSE;
    }
    pMYKI_CAControl->Status                       = CARD_CONTROL_STATUS_ACTIVATED;
    pMYKI_CAControl->ExpiryDate                   = pData->DynamicData.currentBusinessDate + 1;

    if ( MYKI_CS_TAControlGet( &pMYKI_TAControl ) < 0 || pMYKI_TAControl == NULL )
    {
        return FALSE;
    }

    pMYKI_DirectoryInUse    = &pMYKI_TAControl->Directory[ ProductInUse ];
    if ( MYKI_CS_TAProductGet( ProductInUse - 1, &pMYKI_TAProductInUse ) < 0 )
    {
        return FALSE;
    }

    {
        pMYKI_TAControl->ProductInUse                       = ProductInUse;
        pMYKI_DirectoryInUse->SerialNo                      = 1;
        pMYKI_DirectoryInUse->Status                        = TAPP_CONTROL_DIRECTORY_STATUS_ACTIVATED;
        pMYKI_DirectoryInUse->IssuerId                      = IssuerId;
        pMYKI_DirectoryInUse->ProductId                     = ProductId_NHOUR;
        pMYKI_TAProductInUse->LastUsage.ProviderId          = ProviderId_BUS;
        pMYKI_TAProductInUse->LastUsage.Zone                = OriginZone;
        pMYKI_TAProductInUse->LastUsage.Location.RouteId    = OriginRouteId;
        pMYKI_TAProductInUse->LastUsage.Location.StopId     = OriginStopId;

        /*  StaticData */
        {
            pData->StaticData.serviceProviderId             = ProviderId_BUS;
        }
        /*  InternalData */
        {
            pData->InternalData.TransportMode               = TRANSPORT_MODE_BUS;
        }
        /*  DynamicData */
        {
            pData->DynamicData.isEarlyBirdTrip              = TRUE;
            pData->DynamicData.currentZone                  = DestinationZone;
            pData->DynamicData.lineId                       = DestinationRouteId;
            pData->DynamicData.stopId                       = DestinationStopId;
            pData->DynamicData.fareRouteIdIsValid           = FALSE;
        }
        /*  Tariff */
        {
            pData->Tariff.headlessRouteId                   = HeadlessRouteId;
        }

        if ( BR_LLSC_6_1( pData ) != RULE_RESULT_EXECUTED )
        {
            CsErrx( "Not executed" );
            return FALSE;
        }

        /*  4.  if the OriginZone is greater than the Destination zone then:
                a.  Set the current trip direction12 to Inbound */
        if ( pData->DynamicData.currentTripDirection != TRIP_DIRECTION_INBOUND )
        {
            CsErrx( "Wrong trip direction" );
            return FALSE;
        }

        /*  6.a.iii.(1) Add all locations from the scan on stop location to end of the route to the list of transport locations24 */
        /*  6.a.iii.(2) Add all locations from the first stop on the route to the devices current stop id to the list of transport locations24 */
        if ( pData->DynamicData.transportLocationsCount != 5 /* 3-5 + 1-2 */ )
        {
            CsErrx( "Wrong transport locations" );
            return FALSE;
        }

        /*  6.a.iii.(3) Set the Fare Route to use23 to current route */
        if ( pData->DynamicData.fareRouteIdIsValid == FALSE                     ||
             pData->DynamicData.fareRouteId        != pData->DynamicData.lineId )
        {
            CsErrx( "Wrong FareRouteId" );
            return FALSE;
        }
    }

    return TRUE;
}   /* test_BR_LLSC_6_1_004( ) */

/*==========================================================================*
**
**  test_BR_LLSC_6_1_005
**
**  Description     :
**      Unit-test EXECUTED conditions
**      OriginZone = DestinationZone
**      OriginRoute != DestinationRoute
**
**  Parameters      :
**      pData           [I/O]   BR context data
**
**  Returns         :
**      TRUE                    test passed
**      FALSE                   test failed
**
**  Notes           :
**
**
**==========================================================================*/

int test_BR_LLSC_6_1_005( MYKI_BR_ContextData_t *pData )
{
    MYKI_CAControl_t       *pMYKI_CAControl         = NULL;
    MYKI_TAControl_t       *pMYKI_TAControl         = NULL;
    MYKI_Directory_t       *pMYKI_DirectoryInUse    = NULL;
    MYKI_TAProduct_t       *pMYKI_TAProductInUse    = NULL;
    int                     ProductInUse            = 1;
    U8_t                    IssuerId                = 13;
    U8_t                    ProductId_NHOUR         = 4;
    U8_t                    OriginZone              = 2;
    U16_t                   OriginRouteId           = RouteId_1;
    U8_t                    OriginStopId            = 1;
    U8_t                    DestinationZone         = OriginZone;
    U16_t                   DestinationRouteId      = RouteId_2;
    U8_t                    DestinationStopId       = 2;
    U16_t                   HeadlessRouteId         = 10;

    if ( pData == NULL )
    {
        return FALSE;
    }

    if ( MYKI_CS_CAControlGet( &pMYKI_CAControl ) < 0 )
    {
        return FALSE;
    }
    pMYKI_CAControl->Status                       = CARD_CONTROL_STATUS_ACTIVATED;
    pMYKI_CAControl->ExpiryDate                   = pData->DynamicData.currentBusinessDate + 1;

    if ( MYKI_CS_TAControlGet( &pMYKI_TAControl ) < 0 || pMYKI_TAControl == NULL )
    {
        return FALSE;
    }

    pMYKI_DirectoryInUse    = &pMYKI_TAControl->Directory[ ProductInUse ];
    if ( MYKI_CS_TAProductGet( ProductInUse - 1, &pMYKI_TAProductInUse ) < 0 )
    {
        return FALSE;
    }

    {
        pMYKI_TAControl->ProductInUse                       = ProductInUse;
        pMYKI_DirectoryInUse->SerialNo                      = 1;
        pMYKI_DirectoryInUse->Status                        = TAPP_CONTROL_DIRECTORY_STATUS_ACTIVATED;
        pMYKI_DirectoryInUse->IssuerId                      = IssuerId;
        pMYKI_DirectoryInUse->ProductId                     = ProductId_NHOUR;
        pMYKI_TAProductInUse->LastUsage.ProviderId          = ProviderId_BUS;
        pMYKI_TAProductInUse->LastUsage.Zone                = OriginZone;
        pMYKI_TAProductInUse->LastUsage.Location.RouteId    = OriginRouteId;
        pMYKI_TAProductInUse->LastUsage.Location.StopId     = OriginStopId;

        /*  StaticData */
        {
            pData->StaticData.serviceProviderId             = ProviderId_BUS;
        }
        /*  InternalData */
        {
            pData->InternalData.TransportMode               = TRANSPORT_MODE_BUS;
        }
        /*  DynamicData */
        {
            pData->DynamicData.isEarlyBirdTrip              = TRUE;
            pData->DynamicData.currentZone                  = DestinationZone;
            pData->DynamicData.lineId                       = DestinationRouteId;
            pData->DynamicData.stopId                       = DestinationStopId;
            pData->DynamicData.fareRouteIdIsValid           = FALSE;
        }
        /*  Tariff */
        {
            pData->Tariff.headlessRouteId                   = HeadlessRouteId;
        }

        if ( BR_LLSC_6_1( pData ) != RULE_RESULT_EXECUTED )
        {
            CsErrx( "Not executed" );
            return FALSE;
        }

        /*  3.  Set the Current Trip Direction12 to Unknown. */
        if ( pData->DynamicData.currentTripDirection != TRIP_DIRECTION_UNKNOWN )
        {
            CsErrx( "Wrong trip direction" );
            return FALSE;
        }

        /*  6.b.(1) Add all locations from the scan on route and stop to end of the route to the list of transport locations24 */
        /*  6.b.(2) Add all locations from the scan off route stop 1 to the current stop to the list of transport locations24 */
        if ( pData->DynamicData.transportLocationsCount != 6 /* 1-5 + 2 */ )
        {
            CsErrx( "Wrong transport locations" );
            return FALSE;
        }

        /*  6.a.iii.(3) Set the Fare Route to use23 to current route */
        if ( pData->DynamicData.fareRouteIdIsValid == FALSE                     ||
             pData->DynamicData.fareRouteId        != pData->DynamicData.lineId )
        {
            CsErrx( "Wrong FareRouteId" );
            return FALSE;
        }
    }

    return TRUE;
}   /* test_BR_LLSC_6_1_005( ) */

/*==========================================================================*
**
**  test_BR_LLSC_6_1_006
**
**  Description     :
**      Unit-test EXECUTED conditions
**      OriginZone = DestinationZone
**      OriginRoute = HeadlessRoute
**
**  Parameters      :
**      pData           [I/O]   BR context data
**
**  Returns         :
**      TRUE                    test passed
**      FALSE                   test failed
**
**  Notes           :
**
**
**==========================================================================*/

int test_BR_LLSC_6_1_006( MYKI_BR_ContextData_t *pData )
{
    MYKI_CAControl_t       *pMYKI_CAControl         = NULL;
    MYKI_TAControl_t       *pMYKI_TAControl         = NULL;
    MYKI_Directory_t       *pMYKI_DirectoryInUse    = NULL;
    MYKI_TAProduct_t       *pMYKI_TAProductInUse    = NULL;
    int                     ProductInUse            = 1;
    U8_t                    IssuerId                = 13;
    U8_t                    ProductId_NHOUR         = 4;
    U8_t                    OriginZone              = 2;
    U16_t                   OriginRouteId           = RouteId_1;
    U8_t                    OriginStopId            = 1;
    U8_t                    DestinationZone         = OriginZone;
    U16_t                   DestinationRouteId      = RouteId_2;
    U8_t                    DestinationStopId       = 1;
    U16_t                   HeadlessRouteId         = 1;

    if ( pData == NULL )
    {
        return FALSE;
    }

    if ( MYKI_CS_CAControlGet( &pMYKI_CAControl ) < 0 )
    {
        return FALSE;
    }
    pMYKI_CAControl->Status                       = CARD_CONTROL_STATUS_ACTIVATED;
    pMYKI_CAControl->ExpiryDate                   = pData->DynamicData.currentBusinessDate + 1;

    if ( MYKI_CS_TAControlGet( &pMYKI_TAControl ) < 0 || pMYKI_TAControl == NULL )
    {
        return FALSE;
    }

    pMYKI_DirectoryInUse    = &pMYKI_TAControl->Directory[ ProductInUse ];
    if ( MYKI_CS_TAProductGet( ProductInUse - 1, &pMYKI_TAProductInUse ) < 0 )
    {
        return FALSE;
    }

    {
        pMYKI_TAControl->ProductInUse                       = ProductInUse;
        pMYKI_DirectoryInUse->SerialNo                      = 1;
        pMYKI_DirectoryInUse->Status                        = TAPP_CONTROL_DIRECTORY_STATUS_ACTIVATED;
        pMYKI_DirectoryInUse->IssuerId                      = IssuerId;
        pMYKI_DirectoryInUse->ProductId                     = ProductId_NHOUR;
        pMYKI_TAProductInUse->LastUsage.ProviderId          = ProviderId_BUS;
        pMYKI_TAProductInUse->LastUsage.Zone                = OriginZone;
        pMYKI_TAProductInUse->LastUsage.Location.RouteId    = OriginRouteId;
        pMYKI_TAProductInUse->LastUsage.Location.StopId     = OriginStopId;

        /*  StaticData */
        {
            pData->StaticData.serviceProviderId             = ProviderId_BUS;
        }
        /*  InternalData */
        {
            pData->InternalData.TransportMode               = TRANSPORT_MODE_BUS;
        }
        /*  DynamicData */
        {
            pData->DynamicData.isEarlyBirdTrip              = TRUE;
            pData->DynamicData.currentZone                  = DestinationZone;
            pData->DynamicData.lineId                       = DestinationRouteId;
            pData->DynamicData.stopId                       = DestinationStopId;
            pData->DynamicData.fareRouteIdIsValid           = FALSE;
        }
        /*  Tariff */
        {
            pData->Tariff.headlessRouteId                   = HeadlessRouteId;
        }

        if ( BR_LLSC_6_1( pData ) != RULE_RESULT_EXECUTED )
        {
            CsErrx( "Not executed" );
            return FALSE;
        }

        /*  3.  Set the Current Trip Direction12 to Unknown. */
        if ( pData->DynamicData.currentTripDirection != TRIP_DIRECTION_UNKNOWN )
        {
            CsErrx( "Wrong trip direction" );
            return FALSE;
        }

        /*  7.a. Add  the scan-on location and the current location to the list of transport locations24 for determining the zone range */
        if ( pData->DynamicData.transportLocationsCount != 2 )
        {
            CsErrx( "Wrong transport locations" );
            return FALSE;
        }

        /*  7.b. Set the Fare Route to use23 to none - no route will be used in pricing. */
        if ( pData->DynamicData.fareRouteIdIsValid != FALSE )
        {
            CsErrx( "Wrong FareRouteId" );
            return FALSE;
        }
    }

    return TRUE;
}   /* test_BR_LLSC_6_1_006( ) */
