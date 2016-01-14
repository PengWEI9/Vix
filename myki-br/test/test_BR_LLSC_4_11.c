/* -*- mode: c; tabs: 4; -*- */
/*=============================================================================
**
**    Vix Technology                   Licensed software
**    (C) 2013                         All rights reserved
**
**=============================================================================
**
**  Project/Product : MBU
**  Filename        : test_BR_LLSC_4_11.c
**  Author(s)       : David Purdie
**
**  Description     :
**      Implements BR_LLSC_4_11 business rule unit-tests.
**
**  Function(s)     :
**      Template                [Public]    template function
**
**  Information     :
**   Compiler(s)    : ANSI C
**   Target(s)      : Independent
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
 *      External References
 *      -------------------
 */

/*
 *      Local Constants and Macros
 *      --------------------------
 */

/*
 *      Local Data Types
 *      ----------------
 */

/*
 *      Local Function Prototypes
 *      -------------------------
 */

/*
 *      Local Variables
 *      ---------------
 */

/*
 *      Global Variables
 *      ----------------
 */

/*==========================================================================*
**
**  test_BR_LLSC_4_11_001
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

int test_BR_LLSC_4_11_001( MYKI_BR_ContextData_t *pData )
{
    MYKI_TACapping_t       *pMYKI_TACapping         = NULL;
    MYKI_TAControl_t       *pMYKI_TAControl         = NULL;
    MYKI_TAProduct_t       *pMYKI_TAProduct         = NULL;
    U8_t                    nDirIndex               = 2;

    if ( pData == NULL )
    {
        return FALSE;
    }   /* end-of-if */

    if ( MYKI_CS_TACappingGet( &pMYKI_TACapping ) < 0 || pMYKI_TACapping == NULL )
    {
        return FALSE;
    }   /* end-of-if */

    if ( MYKI_CS_TAControlGet( &pMYKI_TAControl ) < 0 || pMYKI_TAControl == NULL )
    {
        return FALSE;
    }   /* end-of-if */

    if ( MYKI_CS_TAProductGet( (U8_t)( nDirIndex - 1 ), &pMYKI_TAProduct ) < 0 || pMYKI_TAProduct == NULL )
    {
        return FALSE;
    }   /* end-of-if */


    /*
    ** Test with bad data pointer
    */
    if ( BR_LLSC_4_11(NULL) != RULE_RESULT_ERROR )
    {
        return FALSE;
    }

    /*
    ** Test No Product in use
    */
    pMYKI_TAControl->ProductInUse = 0;
    if (BR_LLSC_4_11(pData) != RULE_RESULT_BYPASSED)
    {
        return FALSE;
    }

    /*
    ** All is good
    */
    return TRUE;
}

/*==========================================================================*
**
**  test_BR_LLSC_4_11_002
**
**  Description     :
**      Unit-test Execute conditions.
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

int test_BR_LLSC_4_11_002( MYKI_BR_ContextData_t *pData )
{
    MYKI_TACapping_t       *pMYKI_TACapping         = NULL;
    MYKI_TAControl_t       *pMYKI_TAControl         = NULL;
    MYKI_TAProduct_t       *pMYKI_TAProduct         = NULL;
    U8_t                    nDirIndex               = 2;
    U16_t                   EntryPointId            = 1087;
    U16_t                   RouteId                 = 87;
    U8_t                    StopId                  = 121;

    if ( pData == NULL )
    {
        return FALSE;
    }   /* end-of-if */

    if ( MYKI_CS_TACappingGet( &pMYKI_TACapping ) < 0 || pMYKI_TACapping == NULL )
    {
        return FALSE;
    }   /* end-of-if */

    if ( MYKI_CS_TAControlGet( &pMYKI_TAControl ) < 0 || pMYKI_TAControl == NULL )
    {
        return FALSE;
    }   /* end-of-if */

    if ( MYKI_CS_TAProductGet( (U8_t)( nDirIndex - 1 ), &pMYKI_TAProduct ) < 0 || pMYKI_TAProduct == NULL )
    {
        return FALSE;
    }   /* end-of-if */

    /*
     *      Static.ModeOfTransport = RAIL
     *      TAppProduct.LastUsage.ProviderId = BUS
     */
    {
        pData->InternalData.TransportMode                   = TRANSPORT_MODE_RAIL;
        pMYKI_TAControl->ProductInUse                       = nDirIndex;
        pMYKI_TAControl->Directory[ nDirIndex ].Status      = TAPP_CONTROL_STATUS_ACTIVATED;
        pMYKI_TAProduct->LastUsage.ProviderId               = ProviderId_BUS;
        pMYKI_TAProduct->LastUsage.Location.EntryPointId    = EntryPointId;
        pMYKI_TAProduct->LastUsage.Location.RouteId         = RouteId;
        pMYKI_TAProduct->LastUsage.Location.StopId          = StopId;
        pData->DynamicData.isOriginatingInformationSet      = FALSE;
        memset( &pData->DynamicData.originatingInformation, 0, sizeof( OriginatingInformation_t ) );
        if ( BR_LLSC_4_11( pData ) != RULE_RESULT_EXECUTED )
        {
            return FALSE;
        }

        /*
        ** Validate results
        */
        if ( pData->DynamicData.routeChangeover                                 != ROUTE_CHANGEOVER_NONE    ||
             pData->DynamicData.isOriginatingInformationSet                     == FALSE                    ||
             pData->DynamicData.originatingInformation.serviceProviderId        != ProviderId_BUS           ||
             pData->DynamicData.originatingInformation.location.EntryPointId    != EntryPointId             ||
             pData->DynamicData.originatingInformation.location.RouteId         != RouteId                  ||
             pData->DynamicData.originatingInformation.location.StopId          != StopId                   )
        {
            return FALSE;
        }
    }

    /*
     *      Static.ModeOfTransport = RAIL
     *      TAppProduct.LastUsage.ProviderId = RAIL
     */
    {
        pData->InternalData.TransportMode                   = TRANSPORT_MODE_RAIL;
        pMYKI_TAControl->ProductInUse                       = nDirIndex;
        pMYKI_TAControl->Directory[ nDirIndex ].Status      = TAPP_CONTROL_STATUS_ACTIVATED;
        pMYKI_TAProduct->LastUsage.ProviderId               = ProviderId_RAIL;
        pMYKI_TAProduct->LastUsage.Location.EntryPointId    = EntryPointId;
        pMYKI_TAProduct->LastUsage.Location.RouteId         = RouteId;
        pMYKI_TAProduct->LastUsage.Location.StopId          = StopId;
        pData->DynamicData.isOriginatingInformationSet      = FALSE;
        memset( &pData->DynamicData.originatingInformation, 0, sizeof( OriginatingInformation_t ) );
        if ( BR_LLSC_4_11( pData ) != RULE_RESULT_EXECUTED )
        {
            return FALSE;
        }

        /*
        ** Validate results
        */
        if ( pData->DynamicData.routeChangeover                                 != ROUTE_CHANGEOVER_NONE    ||
             pData->DynamicData.isOriginatingInformationSet                     != FALSE                    )
        {
            return FALSE;
        }
    }

    /*
    ** All is good
    */
    return TRUE;
}

