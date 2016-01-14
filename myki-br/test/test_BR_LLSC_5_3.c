/* -*- mode: c; tabs: 4; -*- */
/*=============================================================================
**
**    Vix Technology                   Licensed software
**    (C) 2015                         All rights reserved
**
**=============================================================================
**
**  Project/Product : MBU
**  Filename        : test_BR_LLSC_5_3.c
**  Author(s)       : An Tran
**
**  Description     :
**      Implements BR_LLSC_5_3 business rule unit-tests.
**
**  Function(s)     :
**      test_BR_LLSC_5_3_XXX    [Public]    unit-test functions
**
**  Information     :
**   Compiler(s)    : ANSI C
**   Target(s)      : Independent
**
**  Subversion      :
**      $Id: test_BR_LLSC_5_3.c 71738 2015-08-30 22:26:08Z atran $
**      $HeadURL: https://auperasvn01.aupera.erggroup.com/svn/DPG_SWBase/myki-br/trunk/test/test_BR_LLSC_5_3.c $
**
**  History         :
**   Vers.  Date        Aut.  Type     Description
**   -----  ----------  ----  -------  ----------------------------------------
**    1.00  14.08.15    ANT   Create
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

#include    <string.h>
#include    <cs.h>
#include    <myki_cardservices.h>
#include    <myki_cdd_enums.h>
#include    <myki_br.h>
#include    <myki_br_rules.h>
#include    <myki_br_context_data.h>

#include    "test_common.h"

/*==========================================================================*
**
**  test_BR_LLSC_5_3_001a
**  test_BR_LLSC_5_3_001b
**  test_BR_LLSC_5_3_001c
**  test_BR_LLSC_5_3_001d
**  test_BR_LLSC_5_3_001e
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

int test_BR_LLSC_5_3_001a( MYKI_BR_ContextData_t *pData )
{
    MYKI_CAControl_t       *pMYKI_CAControl         = NULL;
    MYKI_TAControl_t       *pMYKI_TAControl         = NULL;
    int                     EntryPointId            = 53;
    int                     LineId                  = 66;
    int                     StopId                  = 79;
    int                     ServiceProviderId       = ProviderId_TRAM;
    RuleResult_e            RuleResult              = RULE_RESULT_ERROR;

    if ( pData == NULL ||
         MYKI_CS_CAControlGet(      &pMYKI_CAControl        ) != MYKI_CS_OK ||
         MYKI_CS_TAControlGet(      &pMYKI_TAControl        ) != MYKI_CS_OK )
    {
        return FALSE;
    }

    /*  GIVEN */
    {
        /*  Card image */
        {
            pMYKI_CAControl->Status                                         = CARD_CONTROL_STATUS_ACTIVATED;
            pMYKI_TAControl->Status                                         = TAPP_CONTROL_STATUS_ACTIVATED;
            pMYKI_TAControl->ProductInUse                                   = 0;    /*  No provisional product */
            pMYKI_TAControl->Directory[ 0 ].Status                          = TAPP_CONTROL_DIRECTORY_STATUS_ACTIVATED;
        }

        /*  BR context data */
        {
            pData->DynamicData.entryPointId                                 = EntryPointId;
            pData->DynamicData.lineId                                       = LineId;
            pData->DynamicData.stopId                                       = StopId;
            pData->StaticData.serviceProviderId                             = ServiceProviderId;
            pData->InternalData.TransportMode                               = ProviderId_TRAM;
        }
    }

    /*  WHEN */
    {
        /*  Executes business rule */
        RuleResult  = BR_LLSC_5_3( pData );
    }

    /*  THEN */
    {
        UT_Start( );

        /*  Make sure business rule is bypassed */
        UT_Assert( RuleResult == RULE_RESULT_BYPASSED );
        UT_Assert( pData->ReturnedData.bypassCode == BYPASS_CODE( 5, 3, 1, 0 ) );

        return  UT_Result( );
    }
}   /*  test_BR_LLSC_5_3_001a( ) */

int test_BR_LLSC_5_3_001b( MYKI_BR_ContextData_t *pData )
{
    MYKI_CAControl_t       *pMYKI_CAControl         = NULL;
    MYKI_TAControl_t       *pMYKI_TAControl         = NULL;
    MYKI_TAProduct_t       *pMYKI_TAProduct         = NULL;
    int                     EntryPointId            = 53;
    int                     LineId                  = 66;
    int                     StopId                  = 79;
    int                     ServiceProviderId       = ProviderId_BUS;
    int                     ProductInUse            = 1;
    int                     DirProductInUse         = ( ProductInUse - 1 );
    RuleResult_e            RuleResult              = RULE_RESULT_ERROR;

    if ( pData == NULL ||
         MYKI_CS_CAControlGet( &pMYKI_CAControl                  ) != MYKI_CS_OK ||
         MYKI_CS_TAControlGet( &pMYKI_TAControl                  ) != MYKI_CS_OK ||
         MYKI_CS_TAProductGet( DirProductInUse, &pMYKI_TAProduct ) != MYKI_CS_OK )
    {
        return FALSE;
    }

    /*  GIVEN */
    {
        /*  Card image */
        {
            pMYKI_CAControl->Status                                         = CARD_CONTROL_STATUS_ACTIVATED;
            pMYKI_TAControl->Status                                         = TAPP_CONTROL_STATUS_ACTIVATED;
            pMYKI_TAControl->ProductInUse                                   = ProductInUse;
            pMYKI_TAControl->Directory[ 0 ].Status                          = TAPP_CONTROL_DIRECTORY_STATUS_ACTIVATED;
            pMYKI_TAControl->Directory[ DirProductInUse ].Status            = TAPP_CONTROL_DIRECTORY_STATUS_ACTIVATED;
            pMYKI_TAProduct->LastUsage.ProviderId                           = ProviderId_TRAM;  /* != Device transport mode */
        }

        /*  BR context data */
        {
            pData->DynamicData.entryPointId                                 = EntryPointId;
            pData->DynamicData.lineId                                       = LineId;
            pData->DynamicData.stopId                                       = StopId;
            pData->StaticData.serviceProviderId                             = ServiceProviderId;
            pData->InternalData.TransportMode                               = myki_br_cd_GetTransportModeForProvider( ServiceProviderId );
        }
    }

    /*  WHEN */
    {
        /*  Executes business rule */
        RuleResult  = BR_LLSC_5_3( pData );
    }

    /*  THEN */
    {
        UT_Start( );

        /*  Make sure business rule is bypassed */
        UT_Assert( RuleResult == RULE_RESULT_BYPASSED );
        UT_Assert( pData->ReturnedData.bypassCode == BYPASS_CODE( 5, 3, 2, 0 ) );

        return  UT_Result( );
    }
}   /*  test_BR_LLSC_5_3_001b( ) */

int test_BR_LLSC_5_3_001c( MYKI_BR_ContextData_t *pData )
{
    MYKI_CAControl_t       *pMYKI_CAControl         = NULL;
    MYKI_TAControl_t       *pMYKI_TAControl         = NULL;
    MYKI_TAProduct_t       *pMYKI_TAProduct         = NULL;
    int                     EntryPointId            = 53;
    int                     LineId                  = 66;
    int                     StopId                  = 79;
    int                     ServiceProviderId       = ProviderId_TRAM;
    int                     ProductInUse            = 1;
    int                     DirProductInUse         = ( ProductInUse - 1 );
    RuleResult_e            RuleResult              = RULE_RESULT_ERROR;

    if ( pData == NULL ||
         MYKI_CS_CAControlGet( &pMYKI_CAControl                  ) != MYKI_CS_OK ||
         MYKI_CS_TAControlGet( &pMYKI_TAControl                  ) != MYKI_CS_OK ||
         MYKI_CS_TAProductGet( DirProductInUse, &pMYKI_TAProduct ) != MYKI_CS_OK )
    {
        return FALSE;
    }

    /*  GIVEN */
    {
        /*  Card image */
        {
            pMYKI_CAControl->Status                                         = CARD_CONTROL_STATUS_ACTIVATED;
            pMYKI_TAControl->Status                                         = TAPP_CONTROL_STATUS_ACTIVATED;
            pMYKI_TAControl->ProductInUse                                   = ProductInUse;
            pMYKI_TAControl->Directory[ 0 ].Status                          = TAPP_CONTROL_DIRECTORY_STATUS_ACTIVATED;
            pMYKI_TAControl->Directory[ DirProductInUse ].Status            = TAPP_CONTROL_DIRECTORY_STATUS_ACTIVATED;
            pMYKI_TAProduct->LastUsage.ProviderId                           = ServiceProviderId;
            pMYKI_TAProduct->LastUsage.Location.EntryPointId                = ( EntryPointId + 1 ); /*  != Device's EntryPointId */
            pMYKI_TAProduct->LastUsage.Location.RouteId                     = LineId;
            pMYKI_TAProduct->LastUsage.Location.StopId                      = StopId;
        }

        /*  BR context data */
        {
            pData->DynamicData.entryPointId                                 = EntryPointId;
            pData->DynamicData.lineId                                       = LineId;
            pData->DynamicData.stopId                                       = StopId;
            pData->StaticData.serviceProviderId                             = ServiceProviderId;
            pData->InternalData.TransportMode                               = myki_br_cd_GetTransportModeForProvider( ServiceProviderId );
        }
    }

    /*  WHEN */
    {
        /*  Executes business rule */
        RuleResult  = BR_LLSC_5_3( pData );
    }

    /*  THEN */
    {
        UT_Start( );

        /*  Make sure business rule is bypassed */
        UT_Assert( RuleResult == RULE_RESULT_BYPASSED );
        UT_Assert( pData->ReturnedData.bypassCode == BYPASS_CODE( 5, 3, 3, 0 ) );

        return  UT_Result( );
    }
}   /*  test_BR_LLSC_5_3_001c( ) */

int test_BR_LLSC_5_3_001d( MYKI_BR_ContextData_t *pData )
{
    MYKI_CAControl_t       *pMYKI_CAControl         = NULL;
    MYKI_TAControl_t       *pMYKI_TAControl         = NULL;
    MYKI_TAProduct_t       *pMYKI_TAProduct         = NULL;
    int                     EntryPointId            = 53;
    int                     LineId                  = 66;
    int                     StopId                  = 79;
    int                     ServiceProviderId       = ProviderId_TRAM;
    int                     ProductInUse            = 1;
    int                     DirProductInUse         = ( ProductInUse - 1 );
    RuleResult_e            RuleResult              = RULE_RESULT_ERROR;

    if ( pData == NULL ||
         MYKI_CS_CAControlGet( &pMYKI_CAControl                  ) != MYKI_CS_OK ||
         MYKI_CS_TAControlGet( &pMYKI_TAControl                  ) != MYKI_CS_OK ||
         MYKI_CS_TAProductGet( DirProductInUse, &pMYKI_TAProduct ) != MYKI_CS_OK )
    {
        return FALSE;
    }

    /*  GIVEN */
    {
        /*  Card image */
        {
            pMYKI_CAControl->Status                                         = CARD_CONTROL_STATUS_ACTIVATED;
            pMYKI_TAControl->Status                                         = TAPP_CONTROL_STATUS_ACTIVATED;
            pMYKI_TAControl->ProductInUse                                   = ProductInUse;
            pMYKI_TAControl->Directory[ 0 ].Status                          = TAPP_CONTROL_DIRECTORY_STATUS_ACTIVATED;
            pMYKI_TAControl->Directory[ DirProductInUse ].Status            = TAPP_CONTROL_DIRECTORY_STATUS_ACTIVATED;
            pMYKI_TAProduct->LastUsage.ProviderId                           = ServiceProviderId;
            pMYKI_TAProduct->LastUsage.Location.EntryPointId                = EntryPointId;
            pMYKI_TAProduct->LastUsage.Location.RouteId                     = ( LineId + 1 );   /*  != Device's current route */
            pMYKI_TAProduct->LastUsage.Location.StopId                      = StopId;
        }

        /*  BR context data */
        {
            pData->DynamicData.entryPointId                                 = EntryPointId;
            pData->DynamicData.lineId                                       = LineId;
            pData->DynamicData.stopId                                       = StopId;
            pData->StaticData.serviceProviderId                             = ServiceProviderId;
            pData->InternalData.TransportMode                               = myki_br_cd_GetTransportModeForProvider( ServiceProviderId );
        }
    }

    /*  WHEN */
    {
        /*  Executes business rule */
        RuleResult  = BR_LLSC_5_3( pData );
    }

    /*  THEN */
    {
        UT_Start( );

        /*  Make sure business rule is bypassed */
        UT_Assert( RuleResult == RULE_RESULT_BYPASSED );
        UT_Assert( pData->ReturnedData.bypassCode == BYPASS_CODE( 5, 3, 3, 0 ) );

        return  UT_Result( );
    }
}   /*  test_BR_LLSC_5_3_001d( ) */

int test_BR_LLSC_5_3_001e( MYKI_BR_ContextData_t *pData )
{
    MYKI_CAControl_t       *pMYKI_CAControl         = NULL;
    MYKI_TAControl_t       *pMYKI_TAControl         = NULL;
    MYKI_TAProduct_t       *pMYKI_TAProduct         = NULL;
    int                     EntryPointId            = 53;
    int                     LineId                  = 66;
    int                     StopId                  = 79;
    int                     ServiceProviderId       = ProviderId_RAIL;
    int                     ProductInUse            = 1;
    int                     DirProductInUse         = ( ProductInUse - 1 );
    RuleResult_e            RuleResult              = RULE_RESULT_ERROR;

    if ( pData == NULL ||
         MYKI_CS_CAControlGet( &pMYKI_CAControl                  ) != MYKI_CS_OK ||
         MYKI_CS_TAControlGet( &pMYKI_TAControl                  ) != MYKI_CS_OK ||
         MYKI_CS_TAProductGet( DirProductInUse, &pMYKI_TAProduct ) != MYKI_CS_OK )
    {
        return FALSE;
    }

    /*  GIVEN */
    {
        /*  Card image */
        {
            pMYKI_CAControl->Status                                         = CARD_CONTROL_STATUS_ACTIVATED;
            pMYKI_TAControl->Status                                         = TAPP_CONTROL_STATUS_ACTIVATED;
            pMYKI_TAControl->ProductInUse                                   = ProductInUse;
            pMYKI_TAControl->Directory[ 0 ].Status                          = TAPP_CONTROL_DIRECTORY_STATUS_ACTIVATED;
            pMYKI_TAControl->Directory[ DirProductInUse ].Status            = TAPP_CONTROL_DIRECTORY_STATUS_ACTIVATED;
            pMYKI_TAProduct->LastUsage.ProviderId                           = ServiceProviderId;
            pMYKI_TAProduct->LastUsage.Location.EntryPointId                = ( EntryPointId + 1 ); /*  != Device's EntryPointId */
            pMYKI_TAProduct->LastUsage.Location.RouteId                     = LineId;
            pMYKI_TAProduct->LastUsage.Location.StopId                      = StopId;
        }

        /*  BR context data */
        {
            pData->DynamicData.entryPointId                                 = EntryPointId;
            pData->DynamicData.lineId                                       = LineId;
            pData->DynamicData.stopId                                       = StopId;
            pData->StaticData.serviceProviderId                             = ServiceProviderId;
            pData->InternalData.TransportMode                               = myki_br_cd_GetTransportModeForProvider( ServiceProviderId );
        }
    }

    /*  WHEN */
    {
        /*  Executes business rule */
        RuleResult  = BR_LLSC_5_3( pData );
    }

    /*  THEN */
    {
        UT_Start( );

        /*  Make sure business rule is bypassed */
        UT_Assert( RuleResult == RULE_RESULT_BYPASSED );
        UT_Assert( pData->ReturnedData.bypassCode == BYPASS_CODE( 5, 3, 3, 0 ) );

        return  UT_Result( );
    }
}   /*  test_BR_LLSC_5_3_001e( ) */

/*==========================================================================*
**
**  test_BR_LLSC_5_3_002a
**  test_BR_LLSC_5_3_002b
**  test_BR_LLSC_5_3_002c
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

int test_BR_LLSC_5_3_002a( MYKI_BR_ContextData_t *pData )
{
    MYKI_CAControl_t       *pMYKI_CAControl         = NULL;
    MYKI_TAControl_t       *pMYKI_TAControl         = NULL;
    MYKI_TAProduct_t       *pMYKI_TAProduct         = NULL;
    int                     EntryPointId            = 53;
    int                     LineId                  = 66;
    int                     StopId                  = 79;
    int                     ServiceProviderId       = ProviderId_TRAM;
    int                     ProductInUse            = 1;
    int                     DirProductInUse         = ( ProductInUse - 1 );
    RuleResult_e            RuleResult              = RULE_RESULT_ERROR;

    if ( pData == NULL ||
         MYKI_CS_CAControlGet( &pMYKI_CAControl                  ) != MYKI_CS_OK ||
         MYKI_CS_TAControlGet( &pMYKI_TAControl                  ) != MYKI_CS_OK ||
         MYKI_CS_TAProductGet( DirProductInUse, &pMYKI_TAProduct ) != MYKI_CS_OK )
    {
        return FALSE;
    }

    /*  GIVEN */
    {
        /*  Card image */
        {
            pMYKI_CAControl->Status                                         = CARD_CONTROL_STATUS_ACTIVATED;
            pMYKI_TAControl->Status                                         = TAPP_CONTROL_STATUS_ACTIVATED;
            pMYKI_TAControl->ProductInUse                                   = ProductInUse;
            pMYKI_TAControl->Directory[ 0 ].Status                          = TAPP_CONTROL_DIRECTORY_STATUS_ACTIVATED;
            pMYKI_TAControl->Directory[ DirProductInUse ].Status            = TAPP_CONTROL_DIRECTORY_STATUS_ACTIVATED;
            pMYKI_TAProduct->LastUsage.ProviderId                           = ServiceProviderId;
            pMYKI_TAProduct->LastUsage.Location.EntryPointId                = EntryPointId;
            pMYKI_TAProduct->LastUsage.Location.RouteId                     = LineId;
            pMYKI_TAProduct->LastUsage.Location.StopId                      = StopId;
        }

        /*  BR context data */
        {
            pData->DynamicData.entryPointId                                 = EntryPointId;
            pData->DynamicData.lineId                                       = LineId;
            pData->DynamicData.stopId                                       = StopId;
            pData->StaticData.serviceProviderId                             = ServiceProviderId;
            pData->InternalData.TransportMode                               = myki_br_cd_GetTransportModeForProvider( ServiceProviderId );
        }
    }

    /*  WHEN */
    {
        /*  Executes business rule */
        RuleResult  = BR_LLSC_5_3( pData );
    }

    /*  THEN */
    {
        UT_Start( );

        /*  Make sure business rule is executed */
        UT_Assert( RuleResult == RULE_RESULT_EXECUTED );

        /*  Make sure reject code is correct */
        UT_Assert( pData->ReturnedData.rejectReason == MYKI_BR_REJECT_REASON_ALREADY_TOUCH_ON );

        return  UT_Result( );
    }
}   /* test_BR_LLSC_5_3_002a( ) */

int test_BR_LLSC_5_3_002b( MYKI_BR_ContextData_t *pData )
{
    MYKI_CAControl_t       *pMYKI_CAControl         = NULL;
    MYKI_TAControl_t       *pMYKI_TAControl         = NULL;
    MYKI_TAProduct_t       *pMYKI_TAProduct         = NULL;
    int                     EntryPointId            = 53;
    int                     LineId                  = 66;
    int                     StopId                  = 79;
    int                     ServiceProviderId       = ProviderId_TRAM;
    int                     ProductInUse            = 1;
    int                     DirProductInUse         = ( ProductInUse - 1 );
    RuleResult_e            RuleResult              = RULE_RESULT_ERROR;

    if ( pData == NULL ||
         MYKI_CS_CAControlGet( &pMYKI_CAControl                  ) != MYKI_CS_OK ||
         MYKI_CS_TAControlGet( &pMYKI_TAControl                  ) != MYKI_CS_OK ||
         MYKI_CS_TAProductGet( DirProductInUse, &pMYKI_TAProduct ) != MYKI_CS_OK )
    {
        return FALSE;
    }

    /*  GIVEN */
    {
        /*  Card image */
        {
            pMYKI_CAControl->Status                                         = CARD_CONTROL_STATUS_ACTIVATED;
            pMYKI_TAControl->Status                                         = TAPP_CONTROL_STATUS_ACTIVATED;
            pMYKI_TAControl->ProductInUse                                   = ProductInUse;
            pMYKI_TAControl->Directory[ 0 ].Status                          = TAPP_CONTROL_DIRECTORY_STATUS_ACTIVATED;
            pMYKI_TAControl->Directory[ DirProductInUse ].Status            = TAPP_CONTROL_DIRECTORY_STATUS_ACTIVATED;
            pMYKI_TAProduct->LastUsage.ProviderId                           = ServiceProviderId;
            pMYKI_TAProduct->LastUsage.Location.EntryPointId                = EntryPointId;
            pMYKI_TAProduct->LastUsage.Location.RouteId                     = LineId;
            pMYKI_TAProduct->LastUsage.Location.StopId                      = ( StopId + 1 );   /*  != Device's current stop */
        }

        /*  BR context data */
        {
            pData->DynamicData.entryPointId                                 = EntryPointId;
            pData->DynamicData.lineId                                       = LineId;
            pData->DynamicData.stopId                                       = StopId;
            pData->StaticData.serviceProviderId                             = ServiceProviderId;
            pData->InternalData.TransportMode                               = myki_br_cd_GetTransportModeForProvider( ServiceProviderId );
        }
    }

    /*  WHEN */
    {
        /*  Executes business rule */
        RuleResult  = BR_LLSC_5_3( pData );
    }

    /*  THEN */
    {
        UT_Start( );

        /*  Make sure business rule is executed */
        UT_Assert( RuleResult == RULE_RESULT_EXECUTED );

        /*  Make sure reject code is correct */
        UT_Assert( pData->ReturnedData.rejectReason == MYKI_BR_REJECT_REASON_ALREADY_TOUCH_ON );

        return  UT_Result( );
    }
}   /* test_BR_LLSC_5_3_002b( ) */

int test_BR_LLSC_5_3_002c( MYKI_BR_ContextData_t *pData )
{
    MYKI_CAControl_t       *pMYKI_CAControl         = NULL;
    MYKI_TAControl_t       *pMYKI_TAControl         = NULL;
    MYKI_TAProduct_t       *pMYKI_TAProduct         = NULL;
    int                     EntryPointId            = 53;
    int                     LineId                  = 66;
    int                     StopId                  = 79;
    int                     ServiceProviderId       = ProviderId_RAIL;
    int                     ProductInUse            = 1;
    int                     DirProductInUse         = ( ProductInUse - 1 );
    RuleResult_e            RuleResult              = RULE_RESULT_ERROR;

    if ( pData == NULL ||
         MYKI_CS_CAControlGet( &pMYKI_CAControl                  ) != MYKI_CS_OK ||
         MYKI_CS_TAControlGet( &pMYKI_TAControl                  ) != MYKI_CS_OK ||
         MYKI_CS_TAProductGet( DirProductInUse, &pMYKI_TAProduct ) != MYKI_CS_OK )
    {
        return FALSE;
    }

    /*  GIVEN */
    {
        /*  Card image */
        {
            pMYKI_CAControl->Status                                         = CARD_CONTROL_STATUS_ACTIVATED;
            pMYKI_TAControl->Status                                         = TAPP_CONTROL_STATUS_ACTIVATED;
            pMYKI_TAControl->ProductInUse                                   = ProductInUse;
            pMYKI_TAControl->Directory[ 0 ].Status                          = TAPP_CONTROL_DIRECTORY_STATUS_ACTIVATED;
            pMYKI_TAControl->Directory[ DirProductInUse ].Status            = TAPP_CONTROL_DIRECTORY_STATUS_ACTIVATED;
            pMYKI_TAProduct->LastUsage.ProviderId                           = ServiceProviderId;
            pMYKI_TAProduct->LastUsage.Location.EntryPointId                = EntryPointId;
            pMYKI_TAProduct->LastUsage.Location.RouteId                     = ( LineId + 1 );   /*  Not applicable for RAIL */
            pMYKI_TAProduct->LastUsage.Location.StopId                      = ( StopId + 1 );   /*  Not applicable for RAIL */
        }

        /*  BR context data */
        {
            pData->DynamicData.entryPointId                                 = EntryPointId;
            pData->DynamicData.lineId                                       = LineId;
            pData->DynamicData.stopId                                       = StopId;
            pData->StaticData.serviceProviderId                             = ServiceProviderId;
            pData->InternalData.TransportMode                               = myki_br_cd_GetTransportModeForProvider( ServiceProviderId );
        }
    }

    /*  WHEN */
    {
        /*  Executes business rule */
        RuleResult  = BR_LLSC_5_3( pData );
    }

    /*  THEN */
    {
        UT_Start( );

        /*  Make sure business rule is executed */
        UT_Assert( RuleResult == RULE_RESULT_EXECUTED );

        /*  Make sure reject code is correct */
        UT_Assert( pData->ReturnedData.rejectReason == MYKI_BR_REJECT_REASON_ALREADY_TOUCH_ON );

        return  UT_Result( );
    }
}   /* test_BR_LLSC_5_3_002c( ) */
