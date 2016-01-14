/* -*- mode: c; tabs: 4; -*- */
/*=============================================================================
**
**    Vix Technology                   Licensed software
**    (C) 2014                         All rights reserved
**
**=============================================================================
**
**  Project/Product : MBU
**  Filename        : test_BR_LLSC_5_9.c
**  Author(s)       : An Tran
**
**  Description     :
**      Implements BR_LLSC_5_9 business rule unit-tests.
**
**  Function(s)     :
**      test_BR_LLSC_5_9_XXX        [Public]    unit-test functions
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
**    1.00  25.06.14    ANT   Create
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
**  test_BR_LLSC_5_9_001
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

int test_BR_LLSC_5_9_001( MYKI_BR_ContextData_t *pData )
{
    MYKI_CAControl_t       *pMYKI_CAControl         = NULL;
    MYKI_TAControl_t       *pMYKI_TAControl         = NULL;

    if ( pData == NULL )
    {
        return FALSE;
    }

    if ( MYKI_CS_CAControlGet( &pMYKI_CAControl ) < 0 )
    {
        return FALSE;
    }
    pMYKI_CAControl->Status                         = CARD_CONTROL_STATUS_ACTIVATED;
    pMYKI_CAControl->ExpiryDate                     = pData->DynamicData.currentBusinessDate + 1;

    if ( MYKI_CS_TAControlGet( &pMYKI_TAControl ) < 0 || pMYKI_TAControl == NULL )
    {
        return FALSE;
    }
    pMYKI_TAControl->Status                         = TAPP_CONTROL_STATUS_ACTIVATED;
    pMYKI_TAControl->NextProductSerialNo            = 1;

    /*  1.  The Mode of transport(16) for the current device is(1) is not Rail */
    {
        pData->StaticData.serviceProviderId         = ProviderId_RAIL;
        pData->InternalData.TransportMode           = TRANSPORT_MODE_RAIL;
        if ( BR_LLSC_5_9( pData ) != RULE_RESULT_BYPASSED ||
             pData->ReturnedData.bypassCode != BYPASS_CODE( 5, 9, 1, 0 ) )
        {
            CsErrx( "test_BR_LLSC_5_9_001 : Pre-condition 1 failed" );
            return FALSE;
        }
    }

    /*  2.  The data for current route/stop is currently not available
            (ie Dynamic.LocationDataUnavailable(15) is true) */
    {
        pData->StaticData.serviceProviderId         = ProviderId_BUS;
        pData->InternalData.TransportMode           = TRANSPORT_MODE_BUS;
        pData->DynamicData.locationDataUnavailable  = FALSE;
        if ( BR_LLSC_5_9( pData ) != RULE_RESULT_BYPASSED ||
             pData->ReturnedData.bypassCode != BYPASS_CODE( 5, 9, 2, 0 ) )
        {
            CsErrx( "test_BR_LLSC_5_9_001 : Pre-condition 2 failed" );
            return FALSE;
        }
    }

    return TRUE;
}   /* test_BR_LLSC_5_9_001( ) */

/*==========================================================================*
**
**  test_BR_LLSC_5_9_002
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

int test_BR_LLSC_5_9_002( MYKI_BR_ContextData_t *pData )
{
    MYKI_CAControl_t       *pMYKI_CAControl         = NULL;
    MYKI_TAControl_t       *pMYKI_TAControl         = NULL;
    MYKI_TAProduct_t       *pMYKI_TAProductInUse    = NULL;
    int                     ProductInUse            = 1;

    if ( pData == NULL )
    {
        return FALSE;
    }

    if ( MYKI_CS_CAControlGet( &pMYKI_CAControl ) < 0 )
    {
        return FALSE;
    }
    pMYKI_CAControl->Status                         = CARD_CONTROL_STATUS_ACTIVATED;
    pMYKI_CAControl->ExpiryDate                     = pData->DynamicData.currentBusinessDate + 1;

    if ( MYKI_CS_TAControlGet( &pMYKI_TAControl ) < 0 || pMYKI_TAControl == NULL )
    {
        return FALSE;
    }
    pMYKI_TAControl->Status                         = TAPP_CONTROL_STATUS_ACTIVATED;
    pMYKI_TAControl->NextProductSerialNo            = 1;

    {
        pData->StaticData.serviceProviderId         = ProviderId_BUS;
        pData->StaticData.isEntry                   = FALSE;
        pData->StaticData.isExit                    = FALSE;
        pData->InternalData.TransportMode           = TRANSPORT_MODE_BUS;
        pData->DynamicData.locationDataUnavailable  = TRUE;
        MYKI_CD_setCitySaverZone( 0 );
        if ( BR_LLSC_5_9( pData ) != RULE_RESULT_EXECUTED )
        {
            CsErrx( "test_BR_LLSC_5_9_002 : Not executed" );
            return FALSE;
        }

        if ( ( ProductInUse = pMYKI_TAControl->ProductInUse ) == 0 )
        {
            CsErrx( "test_BR_LLSC_5_9_002 : ProductInUse = 0" );
            return FALSE;
        }

        if ( MYKI_CS_TAProductGet( ProductInUse - 1, &pMYKI_TAProductInUse ) < 0 )
        {
            CsErrx( "test_BR_LLSC_5_9_002 : MYKI_CS_TAProductGet() failed" );
            return FALSE;
        }

        if ( pMYKI_TAProductInUse->ZoneLow  != pData->DynamicData.provisionalZoneLow  ||
             pMYKI_TAProductInUse->ZoneHigh != pData->DynamicData.provisionalZoneHigh )
        {
            CsErrx( "test_BR_LLSC_5_9_002 : Wrong Zone(%d,%d)", pMYKI_TAProductInUse->ZoneLow, pMYKI_TAProductInUse->ZoneHigh );
            return FALSE;
        }
    }

    return TRUE;
}   /* test_BR_LLSC_5_9_002( ) */

/*==========================================================================*
**
**  test_BR_LLSC_5_9_003
**
**  Description     :
**      Unit-test EXECUTED conditions. ZoneLow = CityZone
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

int test_BR_LLSC_5_9_003( MYKI_BR_ContextData_t *pData )
{
    MYKI_CAControl_t       *pMYKI_CAControl         = NULL;
    MYKI_TAControl_t       *pMYKI_TAControl         = NULL;
    MYKI_TAProduct_t       *pMYKI_TAProductInUse    = NULL;
    int                     ProductInUse            = 1;

    if ( pData == NULL )
    {
        return FALSE;
    }

    if ( MYKI_CS_CAControlGet( &pMYKI_CAControl ) < 0 )
    {
        return FALSE;
    }
    pMYKI_CAControl->Status                         = CARD_CONTROL_STATUS_ACTIVATED;
    pMYKI_CAControl->ExpiryDate                     = pData->DynamicData.currentBusinessDate + 1;

    if ( MYKI_CS_TAControlGet( &pMYKI_TAControl ) < 0 || pMYKI_TAControl == NULL )
    {
        return FALSE;
    }
    pMYKI_TAControl->Status                         = TAPP_CONTROL_STATUS_ACTIVATED;
    pMYKI_TAControl->NextProductSerialNo            = 1;

    {
        U8_t    CityZone                            = pData->DynamicData.provisionalZoneLow - 1;
        pData->StaticData.serviceProviderId         = ProviderId_BUS;
        pData->StaticData.isEntry                   = FALSE;
        pData->StaticData.isExit                    = FALSE;
        pData->InternalData.TransportMode           = TRANSPORT_MODE_BUS;
        pData->DynamicData.locationDataUnavailable  = TRUE;
        MYKI_CD_setCitySaverZone( CityZone );
        {
            if ( BR_LLSC_5_9( pData ) != RULE_RESULT_EXECUTED )
            {
                CsErrx( "test_BR_LLSC_5_9_003 : Not executed" );
                return FALSE;
            }
        }
        MYKI_CD_setCitySaverZone( 0 );

        if ( ( ProductInUse = pMYKI_TAControl->ProductInUse ) == 0 )
        {
            CsErrx( "test_BR_LLSC_5_9_003 : ProductInUse = 0" );
            return FALSE;
        }

        if ( MYKI_CS_TAProductGet( ProductInUse - 1, &pMYKI_TAProductInUse ) < 0 )
        {
            CsErrx( "test_BR_LLSC_5_9_003 : MYKI_CS_TAProductGet() failed" );
            return FALSE;
        }

        if ( pMYKI_TAProductInUse->ZoneLow  != CityZone                               ||
             pMYKI_TAProductInUse->ZoneHigh != pData->DynamicData.provisionalZoneHigh )
        {
            CsErrx( "test_BR_LLSC_5_9_003 : Wrong Zone(%d,%d)", pMYKI_TAProductInUse->ZoneLow, pMYKI_TAProductInUse->ZoneHigh );
            return FALSE;
        }
    }

    return TRUE;
}   /* test_BR_LLSC_5_9_003( ) */


/*==========================================================================*
**
**  test_BR_LLSC_5_9_004
**
**  Description     :
**      Unit-test EXECUTED conditions. ZoneHigh = CityZone
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

int test_BR_LLSC_5_9_004( MYKI_BR_ContextData_t *pData )
{
    MYKI_CAControl_t       *pMYKI_CAControl         = NULL;
    MYKI_TAControl_t       *pMYKI_TAControl         = NULL;
    MYKI_TAProduct_t       *pMYKI_TAProductInUse    = NULL;
    int                     ProductInUse            = 1;

    if ( pData == NULL )
    {
        return FALSE;
    }

    if ( MYKI_CS_CAControlGet( &pMYKI_CAControl ) < 0 )
    {
        return FALSE;
    }
    pMYKI_CAControl->Status                         = CARD_CONTROL_STATUS_ACTIVATED;
    pMYKI_CAControl->ExpiryDate                     = pData->DynamicData.currentBusinessDate + 1;

    if ( MYKI_CS_TAControlGet( &pMYKI_TAControl ) < 0 || pMYKI_TAControl == NULL )
    {
        return FALSE;
    }
    pMYKI_TAControl->Status                         = TAPP_CONTROL_STATUS_ACTIVATED;
    pMYKI_TAControl->NextProductSerialNo            = 1;

    {
        U8_t    CityZone                            = pData->DynamicData.provisionalZoneHigh + 1;
        pData->StaticData.serviceProviderId         = ProviderId_BUS;
        pData->StaticData.isEntry                   = FALSE;
        pData->StaticData.isExit                    = FALSE;
        pData->InternalData.TransportMode           = TRANSPORT_MODE_BUS;
        pData->DynamicData.locationDataUnavailable  = TRUE;
        MYKI_CD_setCitySaverZone( CityZone );
        {
            if ( BR_LLSC_5_9( pData ) != RULE_RESULT_EXECUTED )
            {
                CsErrx( "test_BR_LLSC_5_9_004 : Not executed" );
                return FALSE;
            }
        }
        MYKI_CD_setCitySaverZone( 0 );

        if ( ( ProductInUse = pMYKI_TAControl->ProductInUse ) == 0 )
        {
            CsErrx( "test_BR_LLSC_5_9_004 : ProductInUse = 0" );
            return FALSE;
        }

        if ( MYKI_CS_TAProductGet( ProductInUse - 1, &pMYKI_TAProductInUse ) < 0 )
        {
            CsErrx( "test_BR_LLSC_5_9_004 : MYKI_CS_TAProductGet() failed" );
            return FALSE;
        }

        if ( pMYKI_TAProductInUse->ZoneLow  != pData->DynamicData.provisionalZoneLow ||
             pMYKI_TAProductInUse->ZoneHigh != CityZone                              )
        {
            CsErrx( "test_BR_LLSC_5_9_004 : Wrong Zone(%d,%d)", pMYKI_TAProductInUse->ZoneLow, pMYKI_TAProductInUse->ZoneHigh );
            return FALSE;
        }
    }

    return TRUE;
}   /* test_BR_LLSC_5_9_004( ) */
