/* -*- mode: c; tabs: 4; -*- */
/*=============================================================================
**
**    Vix Technology                   Licensed software
**    (C) 2014                         All rights reserved
**
**=============================================================================
**
**  Project/Product : MBU
**  Filename        : test_BR_LLSC_4_15.c
**  Author(s)       : An Tran
**
**  Description     :
**      Implements BR_LLSC_4_15 business rule unit-tests.
**
**  Function(s)     :
**      test_BR_LLSC_4_15_XXX       [Public]    unit-test functions
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
**    1.00  24.06.14    ANT   Create
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

/*==========================================================================*
**
**  test_BR_LLSC_4_15_001
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

int test_BR_LLSC_4_15_001( MYKI_BR_ContextData_t *pData )
{
    MYKI_CAControl_t       *pMYKI_CAControl         = NULL;
    MYKI_TAControl_t       *pMYKI_TAControl         = NULL;
    MYKI_Directory_t       *pMYKI_DirectoryInUse    = NULL;
    MYKI_TAProduct_t       *pMYKI_TAProductInUse    = NULL;
    int                     ProductInUse            = 1;
    U8_t                    IssuerId                = 13;
    U8_t                    ProductId_NHOUR         = 4;
    U16_t                   ProviderId_RAIL_2       = 301;
    U8_t                    PhysicalZone_0          = ( 0 + 1 );
    U8_t                    PhysicalZone_1          = ( 1 + 1 );
    U8_t                    PhysicalZone_2          = ( 2 + 1 );

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
        pMYKI_TAControl->ProductInUse           = 0;
        if ( BR_LLSC_4_15( pData ) != RULE_RESULT_BYPASSED ||
             pData->ReturnedData.bypassCode != BYPASS_CODE( 4, 15, 1, 0 ) )
        {
            CsErrx( "Pre-condition 1 failed" );
            return FALSE;
        }
    }

    /*  2.  The RouteChangeover field[(10)](7) indicates no route changeover condition. */
    {
        pMYKI_TAControl->ProductInUse               = ProductInUse;
        pMYKI_DirectoryInUse->SerialNo              = 1;
        pMYKI_DirectoryInUse->Status                = TAPP_CONTROL_DIRECTORY_STATUS_ACTIVATED;
        pMYKI_DirectoryInUse->IssuerId              = IssuerId;
        pMYKI_DirectoryInUse->ProductId             = ProductId_NHOUR;
        pMYKI_TAProductInUse->LastUsage.ProviderId  = ProviderId_RAIL;
        pData->DynamicData.routeChangeover          = ROUTE_CHANGEOVER_LEGITIMATE_WITH_ROUTE_CHANGEOVER;
        pData->StaticData.serviceProviderId         = ProviderId_BUS;
        pData->InternalData.TransportMode           = TRANSPORT_MODE_BUS;
        if ( BR_LLSC_4_15( pData ) != RULE_RESULT_BYPASSED ||
             pData->ReturnedData.bypassCode != BYPASS_CODE( 4, 15, 2, 0 ) )
        {
            CsErrx( "Pre-condition 2 failed" );
            return FALSE;
        }
    }

    /*  3.  The mode of transport as determined by the scan-on service provider(5) of the
            product in use(1) is equal to rail */
    {
        pMYKI_TAControl->ProductInUse               = ProductInUse;
        pMYKI_DirectoryInUse->SerialNo              = 1;
        pMYKI_DirectoryInUse->Status                = TAPP_CONTROL_DIRECTORY_STATUS_ACTIVATED;
        pMYKI_DirectoryInUse->IssuerId              = IssuerId;
        pMYKI_DirectoryInUse->ProductId             = ProductId_NHOUR;
        pMYKI_TAProductInUse->LastUsage.ProviderId  = ProviderId_TRAM;
        pData->DynamicData.routeChangeover          = ROUTE_CHANGEOVER_NONE;
        pData->StaticData.serviceProviderId         = ProviderId_BUS;
        pData->InternalData.TransportMode           = TRANSPORT_MODE_BUS;
        if ( BR_LLSC_4_15( pData ) != RULE_RESULT_BYPASSED ||
             pData->ReturnedData.bypassCode != BYPASS_CODE( 4, 15, 3, 0 ) )
        {
            CsErrx( "Pre-condition 3 failed" );
            return FALSE;
        }
    }

    /*  4.  The mode of transport of the device(6) is not equal to rail */
    {
        pMYKI_TAControl->ProductInUse               = ProductInUse;
        pMYKI_DirectoryInUse->SerialNo              = 1;
        pMYKI_DirectoryInUse->Status                = TAPP_CONTROL_DIRECTORY_STATUS_ACTIVATED;
        pMYKI_DirectoryInUse->IssuerId              = IssuerId;
        pMYKI_DirectoryInUse->ProductId             = ProductId_NHOUR;
        pMYKI_TAProductInUse->LastUsage.ProviderId  = ProviderId_RAIL;
        pMYKI_TAProductInUse->LastUsage.Zone        = PhysicalZone_0;
        pData->DynamicData.routeChangeover          = ROUTE_CHANGEOVER_NONE;
        pData->StaticData.serviceProviderId         = ProviderId_RAIL_2;
        pData->InternalData.TransportMode           = TRANSPORT_MODE_RAIL;
        if ( BR_LLSC_4_15( pData ) != RULE_RESULT_BYPASSED ||
             pData->ReturnedData.bypassCode != BYPASS_CODE( 4, 15, 4, 0 ) )
        {
            CsErrx( "Pre-condition 4 failed" );
            return FALSE;
        }
    }

    /*  5.  The zone of origin(2) o[r]f the current zone is physical zone 0 or 1. */
    {
        pMYKI_TAControl->ProductInUse               = ProductInUse;
        pMYKI_DirectoryInUse->SerialNo              = 1;
        pMYKI_DirectoryInUse->Status                = TAPP_CONTROL_DIRECTORY_STATUS_ACTIVATED;
        pMYKI_DirectoryInUse->IssuerId              = IssuerId;
        pMYKI_DirectoryInUse->ProductId             = ProductId_NHOUR;
        pMYKI_TAProductInUse->LastUsage.ProviderId  = ProviderId_RAIL;
        pMYKI_TAProductInUse->LastUsage.Zone        = PhysicalZone_2;
        pData->DynamicData.routeChangeover          = ROUTE_CHANGEOVER_NONE;
        pData->StaticData.serviceProviderId         = ProviderId_BUS;
        pData->InternalData.TransportMode           = TRANSPORT_MODE_BUS;
        if ( BR_LLSC_4_15( pData ) != RULE_RESULT_BYPASSED ||
             pData->ReturnedData.bypassCode != BYPASS_CODE( 4, 15, 5, 0 ) )
        {
            CsErrx( "Pre-condition 5 failed" );
            return FALSE;
        }
    }

    /*  6.  The Provisional fare capping option(8) is set to 'Option 3' */
    {
        pMYKI_TAControl->ProductInUse               = ProductInUse;
        pMYKI_DirectoryInUse->SerialNo              = 1;
        pMYKI_DirectoryInUse->Status                = TAPP_CONTROL_DIRECTORY_STATUS_ACTIVATED;
        pMYKI_DirectoryInUse->IssuerId              = IssuerId;
        pMYKI_DirectoryInUse->ProductId             = ProductId_NHOUR;
        pMYKI_TAProductInUse->LastUsage.ProviderId  = ProviderId_RAIL;
        pMYKI_TAProductInUse->LastUsage.Zone        = PhysicalZone_0;
        pData->DynamicData.routeChangeover          = ROUTE_CHANGEOVER_NONE;
        pData->StaticData.serviceProviderId         = ProviderId_BUS;
        pData->InternalData.TransportMode           = TRANSPORT_MODE_BUS;
        pData->Tariff.provisionalCappingOption      = PROVISIONAL_CAPPING_OPTION_USE_PROVISIONAL_FARE;
        if ( BR_LLSC_4_15( pData ) != RULE_RESULT_BYPASSED ||
             pData->ReturnedData.bypassCode != BYPASS_CODE( 4, 15, 6, 0 ) )
        {
            CsErrx( "Pre-condition 6 (0) failed" );
            return FALSE;
        }

        pMYKI_TAControl->ProductInUse               = ProductInUse;
        pMYKI_DirectoryInUse->SerialNo              = 1;
        pMYKI_DirectoryInUse->Status                = TAPP_CONTROL_DIRECTORY_STATUS_ACTIVATED;
        pMYKI_DirectoryInUse->IssuerId              = IssuerId;
        pMYKI_DirectoryInUse->ProductId             = ProductId_NHOUR;
        pMYKI_TAProductInUse->LastUsage.ProviderId  = ProviderId_RAIL;
        pMYKI_TAProductInUse->LastUsage.Zone        = PhysicalZone_1;
        pData->DynamicData.routeChangeover          = ROUTE_CHANGEOVER_NONE;
        pData->StaticData.serviceProviderId         = ProviderId_BUS;
        pData->InternalData.TransportMode           = TRANSPORT_MODE_BUS;
        pData->Tariff.provisionalCappingOption      = PROVISIONAL_CAPPING_OPTION_USE_PROVISIONAL_FARE;
        if ( BR_LLSC_4_15( pData ) != RULE_RESULT_BYPASSED ||
             pData->ReturnedData.bypassCode != BYPASS_CODE( 4, 15, 6, 0 ) )
        {
            CsErrx( "Pre-condition 6 (1) failed" );
            return FALSE;
        }
    }

    return TRUE;
}   /* test_BR_LLSC_4_15_001( ) */

/*==========================================================================*
**
**  test_BR_LLSC_4_15_002
**
**  Description     :
**      Unit-test EXECUTED conditions.
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

int test_BR_LLSC_4_15_002( MYKI_BR_ContextData_t *pData )
{
    MYKI_CAControl_t       *pMYKI_CAControl         = NULL;
    MYKI_TAControl_t       *pMYKI_TAControl         = NULL;
    MYKI_Directory_t       *pMYKI_DirectoryInUse    = NULL;
    MYKI_TAProduct_t       *pMYKI_TAProductInUse    = NULL;
    int                     ProductInUse            = 1;
    U8_t                    IssuerId                = 13;
    U8_t                    ProductId_NHOUR         = 4;
    U8_t                    PhysicalZone_1          = ( 1 + 1 );

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
        pMYKI_TAControl->ProductInUse               = ProductInUse;
        pMYKI_DirectoryInUse->SerialNo              = 1;
        pMYKI_DirectoryInUse->Status                = TAPP_CONTROL_DIRECTORY_STATUS_ACTIVATED;
        pMYKI_DirectoryInUse->IssuerId              = IssuerId;
        pMYKI_DirectoryInUse->ProductId             = ProductId_NHOUR;
        pMYKI_TAProductInUse->LastUsage.ProviderId  = ProviderId_RAIL;
        pMYKI_TAProductInUse->LastUsage.Zone        = PhysicalZone_1;
        pData->DynamicData.routeChangeover          = ROUTE_CHANGEOVER_NONE;
        pData->StaticData.serviceProviderId         = ProviderId_BUS;
        pData->InternalData.TransportMode           = TRANSPORT_MODE_BUS;
        pData->Tariff.provisionalCappingOption      = PROVISIONAL_CAPPING_OPTION_3;
        pData->DynamicData.fareRouteIdIsValid       = TRUE;
        if ( BR_LLSC_4_15( pData ) != RULE_RESULT_EXECUTED )
        {
            return FALSE;
        }

        /*  1.  Set the FareRouteID(7) to none - no route will be considered for pricing */
        if ( pData->DynamicData.fareRouteIdIsValid != FALSE )
        {
            CsErrx( "FareRouteId not set to none" );
            return FALSE;
        }
    }

    return TRUE;
}   /* test_BR_LLSC_4_15_002( ) */
