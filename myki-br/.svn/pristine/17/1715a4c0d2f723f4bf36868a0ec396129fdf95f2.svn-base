/* -*- mode: c; tabs: 4; -*- */
/*=============================================================================
**
**    Vix Technology                   Licensed software
**    (C) 2015                         All rights reserved
**
**=============================================================================
**
**  Project/Product : MBU
**  Filename        : test_BR_LLSC_4_20.c
**  Author(s)       : An Tran
**
**  Description     :
**      Implements BR_LLSC_4_20 business rule unit-tests.
**
**  Function(s)     :
**      test_BR_LLSC_4_20           [public]    unit-test functions
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
**    1.00  15.01.16    ANT   Create
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
**  test_BR_LLSC_4_20_001x
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

int test_BR_LLSC_4_20_001a( MYKI_BR_ContextData_t *pData )
{
    MYKI_CAControl_t               *pMYKI_CAControl             = NULL;
    MYKI_TAControl_t               *pMYKI_TAControl             = NULL;
    RuleResult_e                    RuleResult                  = RULE_RESULT_ERROR;
    int                             ServiceProviderId           = ProviderId_BUS;
    int                             EntryPointId                = 6609;

    if ( pData == NULL ||
         MYKI_CS_OpenCard(     MYKI_CS_OPEN_TRANSIT_APP, NULL /*DONTCARE*/  ) != MYKI_CS_OK ||
         MYKI_CS_CAControlGet( &pMYKI_CAControl                             ) != MYKI_CS_OK ||
         MYKI_CS_TAControlGet( &pMYKI_TAControl                             ) != MYKI_CS_OK )
    {
        return  RULE_RESULT_ERROR;
    }

    /*  GIVEN */
    {
        /*  Card image */
        {
            pMYKI_CAControl->Status                                     = CARD_CONTROL_STATUS_ACTIVATED;
            pMYKI_TAControl->Status                                     = TAPP_CONTROL_STATUS_ACTIVATED;
            pMYKI_TAControl->ProductInUse                               = 0;    /*  No product in used */
        }

        /*  BR context data */
        {
            pData->InternalData.TransportMode                           = TRANSPORT_MODE_BUS;
            pData->StaticData.serviceProviderId                         = ServiceProviderId;
            pData->DynamicData.entryPointId                             = EntryPointId;
        }
    }

    /*  WHEN */
    {
        /*  Executes business rule */
        RuleResult  = BR_LLSC_4_20( pData );
    }

    /*  THEN */
    {
        UT_Start( );

        /*  Make sure business rule is bypassed */
        UT_Assert( RuleResult == RULE_RESULT_BYPASSED );
        UT_Assert( pData->ReturnedData.bypassCode == BYPASS_CODE( 4, 20, 1, 0 ) );

        return  UT_Result( );
    }
}   /*  test_BR_LLSC_4_20_001a( ) */

int test_BR_LLSC_4_20_001b( MYKI_BR_ContextData_t *pData )
{
    MYKI_CAControl_t               *pMYKI_CAControl             = NULL;
    MYKI_TAControl_t               *pMYKI_TAControl             = NULL;
    MYKI_TAProduct_t               *pMYKI_TAProduct             = NULL;
    RuleResult_e                    RuleResult                  = RULE_RESULT_ERROR;
    int                             ServiceProviderId           = ProviderId_BUS;
    int                             EntryPointId                = 6609;
    int                             ProductInUse                = 1;
    int                             DirProductInUse             = ( ProductInUse - 1 );

    if ( pData == NULL ||
         MYKI_CS_OpenCard(     MYKI_CS_OPEN_TRANSIT_APP, NULL /*DONTCARE*/  ) != MYKI_CS_OK ||
         MYKI_CS_CAControlGet( &pMYKI_CAControl                             ) != MYKI_CS_OK ||
         MYKI_CS_TAControlGet( &pMYKI_TAControl                             ) != MYKI_CS_OK ||
         MYKI_CS_TAProductGet( DirProductInUse, &pMYKI_TAProduct            ) != MYKI_CS_OK )
    {
        return  FALSE;
    }

    /*  GIVEN */
    {
        /*  Card image */
        {
            pMYKI_CAControl->Status                                     = CARD_CONTROL_STATUS_ACTIVATED;
            pMYKI_TAControl->Status                                     = TAPP_CONTROL_STATUS_ACTIVATED;
            pMYKI_TAControl->ProductInUse                               = ProductInUse;
            pMYKI_TAControl->Directory[ ProductInUse ].ProductId        = PRODUCT_ID_NHOUR;
            pMYKI_TAControl->Directory[ ProductInUse ].Status           = TAPP_CONTROL_DIRECTORY_STATUS_ACTIVATED;
            pMYKI_TAProduct->ControlBitmap                              = 0;    /*  != TAPP_TPRODUCT_CONTROL_PROVISIONAL_BITMAP */
        }

        /*  BR context data */
        {
            pData->InternalData.TransportMode                           = TRANSPORT_MODE_BUS;
            pData->StaticData.serviceProviderId                         = ServiceProviderId;
            pData->DynamicData.entryPointId                             = EntryPointId;
        }
    }

    /*  WHEN */
    {
        /*  Executes business rule */
        RuleResult  = BR_LLSC_4_20( pData );
    }

    /*  THEN */
    {
        UT_Start( );

        /*  Make sure business rule is bypassed */
        UT_Assert( RuleResult == RULE_RESULT_BYPASSED );
        UT_Assert( pData->ReturnedData.bypassCode == BYPASS_CODE( 4, 20, 2, 0 ) );

        return  UT_Result( );
    }
}   /*  test_BR_LLSC_4_20_001b( ) */

int test_BR_LLSC_4_20_001c( MYKI_BR_ContextData_t *pData )
{
    MYKI_CAControl_t               *pMYKI_CAControl             = NULL;
    MYKI_TAControl_t               *pMYKI_TAControl             = NULL;
    MYKI_TAProduct_t               *pMYKI_TAProduct             = NULL;
    RuleResult_e                    RuleResult                  = RULE_RESULT_ERROR;
    int                             ServiceProviderId           = ProviderId_BUS;
    int                             EntryPointId                = 6609;
    int                             ProductInUse                = 1;
    int                             DirProductInUse             = ( ProductInUse - 1 );

    if ( pData == NULL ||
         MYKI_CS_OpenCard(     MYKI_CS_OPEN_TRANSIT_APP, NULL /*DONTCARE*/  ) != MYKI_CS_OK ||
         MYKI_CS_CAControlGet( &pMYKI_CAControl                             ) != MYKI_CS_OK ||
         MYKI_CS_TAControlGet( &pMYKI_TAControl                             ) != MYKI_CS_OK ||
         MYKI_CS_TAProductGet( DirProductInUse, &pMYKI_TAProduct            ) != MYKI_CS_OK )
    {
        return  FALSE;
    }

    /*  GIVEN */
    {
        /*  Card image */
        {
            pMYKI_CAControl->Status                                     = CARD_CONTROL_STATUS_ACTIVATED;
            pMYKI_TAControl->Status                                     = TAPP_CONTROL_STATUS_ACTIVATED;
            pMYKI_TAControl->ProductInUse                               = ProductInUse;
            pMYKI_TAControl->Directory[ ProductInUse ].ProductId        = PRODUCT_ID_NHOUR;
            pMYKI_TAControl->Directory[ ProductInUse ].Status           = TAPP_CONTROL_DIRECTORY_STATUS_ACTIVATED;
            pMYKI_TAProduct->ControlBitmap                              = TAPP_TPRODUCT_CONTROL_PROVISIONAL_BITMAP;
            pMYKI_TAProduct->EndDateTime                                = pData->DynamicData.currentDateTime + HOURS_TO_SECONDS( 1 );   /*  Not expired */
            pMYKI_TAProduct->StartDateTime                              = pMYKI_TAProduct->EndDateTime       - HOURS_TO_SECONDS( 2 );
        }

        /*  BR context data */
        {
            pData->InternalData.TransportMode                           = TRANSPORT_MODE_BUS;
            pData->DynamicData.entryPointId                             = EntryPointId;
            pData->StaticData.serviceProviderId                         = ServiceProviderId;
        }
    }

    /*  WHEN */
    {
        /*  Executes business rule */
        RuleResult  = BR_LLSC_4_20( pData );
    }

    /*  THEN */
    {
        UT_Start( );

        /*  Make sure business rule is bypassed */
        UT_Assert( RuleResult == RULE_RESULT_BYPASSED );
        UT_Assert( pData->ReturnedData.bypassCode == BYPASS_CODE( 4, 20, 3, 0 ) );

        return  UT_Result( );
    }
}   /*  test_BR_LLSC_4_20_001c( ) */

int test_BR_LLSC_4_20_001d( MYKI_BR_ContextData_t *pData )
{
    MYKI_CAControl_t               *pMYKI_CAControl             = NULL;
    MYKI_TAControl_t               *pMYKI_TAControl             = NULL;
    MYKI_TAProduct_t               *pMYKI_TAProduct             = NULL;
    RuleResult_e                    RuleResult                  = RULE_RESULT_ERROR;
    int                             ServiceProviderId           = ProviderId_BUS;
    int                             EntryPointId                = 6609;
    int                             ProductInUse                = 1;
    int                             DirProductInUse             = ( ProductInUse - 1 );

    if ( pData == NULL ||
         MYKI_CS_OpenCard(     MYKI_CS_OPEN_TRANSIT_APP, NULL /*DONTCARE*/  ) != MYKI_CS_OK ||
         MYKI_CS_CAControlGet( &pMYKI_CAControl                             ) != MYKI_CS_OK ||
         MYKI_CS_TAControlGet( &pMYKI_TAControl                             ) != MYKI_CS_OK ||
         MYKI_CS_TAProductGet( DirProductInUse, &pMYKI_TAProduct            ) != MYKI_CS_OK )
    {
        return  FALSE;
    }

    /*  GIVEN */
    {
        /*  Card image */
        {
            pMYKI_CAControl->Status                                     = CARD_CONTROL_STATUS_ACTIVATED;
            pMYKI_TAControl->Status                                     = TAPP_CONTROL_STATUS_ACTIVATED;
            pMYKI_TAControl->ProductInUse                               = ProductInUse;
            pMYKI_TAControl->Directory[ ProductInUse ].ProductId        = PRODUCT_ID_NHOUR;
            pMYKI_TAControl->Directory[ ProductInUse ].Status           = TAPP_CONTROL_DIRECTORY_STATUS_ACTIVATED;
            pMYKI_TAProduct->ControlBitmap                              = TAPP_TPRODUCT_CONTROL_PROVISIONAL_BITMAP;
            pMYKI_TAProduct->LastUsage.Location.EntryPointId            = EntryPointId + 1;     /* != Device's EntryPointId */
            pMYKI_TAProduct->LastUsage.ProviderId                       = ServiceProviderId;
            pMYKI_TAProduct->EndDateTime                                = pData->DynamicData.currentDateTime - HOURS_TO_SECONDS( 1 );
            pMYKI_TAProduct->StartDateTime                              = pMYKI_TAProduct->EndDateTime       - HOURS_TO_SECONDS( 2 );
        }

        /*  BR context data */
        {
            pData->InternalData.TransportMode                           = TRANSPORT_MODE_BUS;
            pData->DynamicData.entryPointId                             = EntryPointId;
            pData->StaticData.serviceProviderId                         = ServiceProviderId;
        }
    }

    /*  WHEN */
    {
        /*  Executes business rule */
        RuleResult  = BR_LLSC_4_20( pData );
    }

    /*  THEN */
    {
        UT_Start( );

        /*  Make sure business rule is bypassed */
        UT_Assert( RuleResult == RULE_RESULT_BYPASSED );
        UT_Assert( pData->ReturnedData.bypassCode == BYPASS_CODE( 4, 20, 4, 1 ) );

        return  UT_Result( );
    }
}   /*  test_BR_LLSC_4_20_001d( ) */

int test_BR_LLSC_4_20_001e( MYKI_BR_ContextData_t *pData )
{
    MYKI_CAControl_t               *pMYKI_CAControl             = NULL;
    MYKI_TAControl_t               *pMYKI_TAControl             = NULL;
    MYKI_TAProduct_t               *pMYKI_TAProduct             = NULL;
    RuleResult_e                    RuleResult                  = RULE_RESULT_ERROR;
    int                             ServiceProviderId           = ProviderId_BUS;
    int                             EntryPointId                = 6609;
    int                             ProductInUse                = 1;
    int                             DirProductInUse             = ( ProductInUse - 1 );

    if ( pData == NULL ||
         MYKI_CS_OpenCard(     MYKI_CS_OPEN_TRANSIT_APP, NULL /*DONTCARE*/  ) != MYKI_CS_OK ||
         MYKI_CS_CAControlGet( &pMYKI_CAControl                             ) != MYKI_CS_OK ||
         MYKI_CS_TAControlGet( &pMYKI_TAControl                             ) != MYKI_CS_OK ||
         MYKI_CS_TAProductGet( DirProductInUse, &pMYKI_TAProduct            ) != MYKI_CS_OK )
    {
        return  FALSE;
    }

    /*  GIVEN */
    {
        /*  Card image */
        {
            pMYKI_CAControl->Status                                     = CARD_CONTROL_STATUS_ACTIVATED;
            pMYKI_TAControl->Status                                     = TAPP_CONTROL_STATUS_ACTIVATED;
            pMYKI_TAControl->ProductInUse                               = ProductInUse;
            pMYKI_TAControl->Directory[ ProductInUse ].ProductId        = PRODUCT_ID_NHOUR;
            pMYKI_TAControl->Directory[ ProductInUse ].Status           = TAPP_CONTROL_DIRECTORY_STATUS_ACTIVATED;
            pMYKI_TAProduct->ControlBitmap                              = TAPP_TPRODUCT_CONTROL_PROVISIONAL_BITMAP;
            pMYKI_TAProduct->LastUsage.Location.EntryPointId            = EntryPointId;
            pMYKI_TAProduct->LastUsage.ProviderId                       = ProviderId_TRAM;  /*  != Device's Service Provider Id */
            pMYKI_TAProduct->EndDateTime                                = pData->DynamicData.currentDateTime - HOURS_TO_SECONDS( 1 );
            pMYKI_TAProduct->StartDateTime                              = pMYKI_TAProduct->EndDateTime       - HOURS_TO_SECONDS( 2 );
        }

        /*  BR context data */
        {
            pData->InternalData.TransportMode                           = TRANSPORT_MODE_BUS;
            pData->DynamicData.entryPointId                             = EntryPointId;
            pData->StaticData.serviceProviderId                         = ServiceProviderId;
        }
    }

    /*  WHEN */
    {
        /*  Executes business rule */
        RuleResult  = BR_LLSC_4_20( pData );
    }

    /*  THEN */
    {
        UT_Start( );

        /*  Make sure business rule is bypassed */
        UT_Assert( RuleResult == RULE_RESULT_BYPASSED );
        UT_Assert( pData->ReturnedData.bypassCode == BYPASS_CODE( 4, 20, 4, 2 ) );

        return  UT_Result( );
    }
}   /*  test_BR_LLSC_4_20_001e( ) */

int test_BR_LLSC_4_20_001f( MYKI_BR_ContextData_t *pData )
{
    MYKI_CAControl_t               *pMYKI_CAControl             = NULL;
    MYKI_TAControl_t               *pMYKI_TAControl             = NULL;
    MYKI_TAProduct_t               *pMYKI_TAProduct             = NULL;
    RuleResult_e                    RuleResult                  = RULE_RESULT_ERROR;
    int                             ServiceProviderId           = ProviderId_RAIL;
    int                             EntryPointId                = 6609;
    int                             ProductInUse                = 1;
    int                             DirProductInUse             = ( ProductInUse - 1 );

    if ( pData == NULL ||
         MYKI_CS_OpenCard(     MYKI_CS_OPEN_TRANSIT_APP, NULL /*DONTCARE*/  ) != MYKI_CS_OK ||
         MYKI_CS_CAControlGet( &pMYKI_CAControl                             ) != MYKI_CS_OK ||
         MYKI_CS_TAControlGet( &pMYKI_TAControl                             ) != MYKI_CS_OK ||
         MYKI_CS_TAProductGet( DirProductInUse, &pMYKI_TAProduct            ) != MYKI_CS_OK )
    {
        return  FALSE;
    }

    /*  GIVEN */
    {
        /*  Card image */
        {
            pMYKI_CAControl->Status                                     = CARD_CONTROL_STATUS_ACTIVATED;
            pMYKI_TAControl->Status                                     = TAPP_CONTROL_STATUS_ACTIVATED;
            pMYKI_TAControl->ProductInUse                               = ProductInUse;
            pMYKI_TAControl->Directory[ ProductInUse ].ProductId        = PRODUCT_ID_NHOUR;
            pMYKI_TAControl->Directory[ ProductInUse ].Status           = TAPP_CONTROL_DIRECTORY_STATUS_ACTIVATED;
            pMYKI_TAProduct->ControlBitmap                              = TAPP_TPRODUCT_CONTROL_PROVISIONAL_BITMAP;
            pMYKI_TAProduct->LastUsage.Location.EntryPointId            = EntryPointId;
            pMYKI_TAProduct->LastUsage.ProviderId                       = ProviderId_TRAM;  /*  != TRANSPORT_MODE_RAIL */
            pMYKI_TAProduct->EndDateTime                                = pData->DynamicData.currentDateTime - HOURS_TO_SECONDS( 1 );
            pMYKI_TAProduct->StartDateTime                              = pMYKI_TAProduct->EndDateTime       - HOURS_TO_SECONDS( 2 );
        }

        /*  BR context data */
        {
            pData->InternalData.TransportMode                           = TRANSPORT_MODE_RAIL;
            pData->DynamicData.entryPointId                             = EntryPointId;
            pData->StaticData.serviceProviderId                         = ServiceProviderId;
        }
    }

    /*  WHEN */
    {
        /*  Executes business rule */
        RuleResult  = BR_LLSC_4_20( pData );
    }

    /*  THEN */
    {
        UT_Start( );

        /*  Make sure business rule is bypassed */
        UT_Assert( RuleResult == RULE_RESULT_BYPASSED );
        UT_Assert( pData->ReturnedData.bypassCode == BYPASS_CODE( 4, 20, 4, 3 ) );

        return  UT_Result( );
    }
}   /*  test_BR_LLSC_4_20_001f( ) */

int test_BR_LLSC_4_20_001g( MYKI_BR_ContextData_t *pData )
{
    MYKI_CAControl_t               *pMYKI_CAControl             = NULL;
    MYKI_TAControl_t               *pMYKI_TAControl             = NULL;
    MYKI_TAProduct_t               *pMYKI_TAProduct             = NULL;
    RuleResult_e                    RuleResult                  = RULE_RESULT_ERROR;
    int                             ServiceProviderId           = ProviderId_BUS;
    int                             EntryPointId                = 6609;
    int                             ProductInUse                = 1;
    int                             DirProductInUse             = ( ProductInUse - 1 );

    if ( pData == NULL ||
         MYKI_CS_OpenCard(     MYKI_CS_OPEN_TRANSIT_APP, NULL /*DONTCARE*/  ) != MYKI_CS_OK ||
         MYKI_CS_CAControlGet( &pMYKI_CAControl                             ) != MYKI_CS_OK ||
         MYKI_CS_TAControlGet( &pMYKI_TAControl                             ) != MYKI_CS_OK ||
         MYKI_CS_TAProductGet( DirProductInUse, &pMYKI_TAProduct            ) != MYKI_CS_OK )
    {
        return  FALSE;
    }

    /*  GIVEN */
    {
        /*  Card image */
        {
            pMYKI_CAControl->Status                                     = CARD_CONTROL_STATUS_ACTIVATED;
            pMYKI_TAControl->Status                                     = TAPP_CONTROL_STATUS_ACTIVATED;
            pMYKI_TAControl->ProductInUse                               = ProductInUse;
            pMYKI_TAControl->Directory[ ProductInUse ].ProductId        = PRODUCT_ID_NHOUR;
            pMYKI_TAControl->Directory[ ProductInUse ].Status           = TAPP_CONTROL_DIRECTORY_STATUS_ACTIVATED;
            pMYKI_TAProduct->ControlBitmap                              = TAPP_TPRODUCT_CONTROL_PROVISIONAL_BITMAP;
            pMYKI_TAProduct->LastUsage.Location.EntryPointId            = EntryPointId;
            pMYKI_TAProduct->LastUsage.ProviderId                       = ServiceProviderId;
            pMYKI_TAProduct->EndDateTime                                = pData->DynamicData.currentDateTime - HOURS_TO_SECONDS( 1 );
            pMYKI_TAProduct->StartDateTime                              = pMYKI_TAProduct->EndDateTime       - HOURS_TO_SECONDS( 2 );
        }

        /*  BR context data */
        {
            pData->InternalData.TransportMode                           = TRANSPORT_MODE_BUS;
            pData->DynamicData.entryPointId                             = EntryPointId;
            pData->StaticData.serviceProviderId                         = ServiceProviderId;
            pData->Tariff.maximumTripTolerance                          = 0;    /*  None */
        }
    }

    /*  WHEN */
    {
        /*  Executes business rule */
        RuleResult  = BR_LLSC_4_20( pData );
    }

    /*  THEN */
    {
        UT_Start( );

        /*  Make sure business rule is bypassed */
        UT_Assert( RuleResult == RULE_RESULT_BYPASSED );
        UT_Assert( pData->ReturnedData.bypassCode == BYPASS_CODE( 4, 20, 5, 0 ) );

        return  UT_Result( );
    }
}   /*  test_BR_LLSC_4_20_001g( ) */

int test_BR_LLSC_4_20_001h( MYKI_BR_ContextData_t *pData )
{
    MYKI_CAControl_t               *pMYKI_CAControl             = NULL;
    MYKI_TAControl_t               *pMYKI_TAControl             = NULL;
    MYKI_TAProduct_t               *pMYKI_TAProduct             = NULL;
    RuleResult_e                    RuleResult                  = RULE_RESULT_ERROR;
    int                             ServiceProviderId           = ProviderId_BUS;
    int                             EntryPointId                = 6609;
    int                             ProductInUse                = 1;
    int                             DirProductInUse             = ( ProductInUse - 1 );
    int                             MaximumTripTolerance        = 15;

    if ( pData == NULL ||
         MYKI_CS_OpenCard(     MYKI_CS_OPEN_TRANSIT_APP, NULL /*DONTCARE*/  ) != MYKI_CS_OK ||
         MYKI_CS_CAControlGet( &pMYKI_CAControl                             ) != MYKI_CS_OK ||
         MYKI_CS_TAControlGet( &pMYKI_TAControl                             ) != MYKI_CS_OK ||
         MYKI_CS_TAProductGet( DirProductInUse, &pMYKI_TAProduct            ) != MYKI_CS_OK )
    {
        return  FALSE;
    }

    /*  GIVEN */
    {
        /*  Card image */
        {
            pMYKI_CAControl->Status                                     = CARD_CONTROL_STATUS_ACTIVATED;
            pMYKI_TAControl->Status                                     = TAPP_CONTROL_STATUS_ACTIVATED;
            pMYKI_TAControl->ProductInUse                               = ProductInUse;
            pMYKI_TAControl->Directory[ ProductInUse ].ProductId        = PRODUCT_ID_NHOUR;
            pMYKI_TAControl->Directory[ ProductInUse ].Status           = TAPP_CONTROL_DIRECTORY_STATUS_ACTIVATED;
            pMYKI_TAProduct->ControlBitmap                              = TAPP_TPRODUCT_CONTROL_PROVISIONAL_BITMAP;
            pMYKI_TAProduct->LastUsage.Location.EntryPointId            = EntryPointId;
            pMYKI_TAProduct->LastUsage.ProviderId                       = ServiceProviderId;
            pMYKI_TAProduct->EndDateTime                                = pData->DynamicData.currentDateTime - MINUTES_TO_SECONDS( MaximumTripTolerance );  /*  Exceeds tolerance */
            pMYKI_TAProduct->StartDateTime                              = pMYKI_TAProduct->EndDateTime - HOURS_TO_SECONDS( 2 );
        }

        /*  BR context data */
        {
            pData->InternalData.TransportMode                           = TRANSPORT_MODE_BUS;
            pData->DynamicData.entryPointId                             = EntryPointId;
            pData->StaticData.serviceProviderId                         = ServiceProviderId;
            pData->Tariff.maximumTripTolerance                          = MaximumTripTolerance;
        }
    }

    /*  WHEN */
    {
        /*  Executes business rule */
        RuleResult  = BR_LLSC_4_20( pData );
    }

    /*  THEN */
    {
        UT_Start( );

        /*  Make sure business rule is bypassed */
        UT_Assert( RuleResult == RULE_RESULT_BYPASSED );
        UT_Assert( pData->ReturnedData.bypassCode == BYPASS_CODE( 4, 20, 6, 0 ) );

        return  UT_Result( );
    }
}   /*  test_BR_LLSC_4_20_001h( ) */

/*==========================================================================*
**
**  test_BR_LLSC_4_20_002x
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

int test_BR_LLSC_4_20_002a( MYKI_BR_ContextData_t *pData )
{
    MYKI_CAControl_t               *pMYKI_CAControl             = NULL;
    MYKI_TAControl_t               *pMYKI_TAControl             = NULL;
    MYKI_TAProduct_t               *pMYKI_TAProduct             = NULL;
    RuleResult_e                    RuleResult                  = RULE_RESULT_ERROR;
    int                             ServiceProviderId           = ProviderId_BUS;
    int                             EntryPointId                = 6609;
    int                             ProductInUse                = 1;
    int                             DirProductInUse             = ( ProductInUse - 1 );
    int                             MaximumTripTolerance        = 15;

    if ( pData == NULL ||
         MYKI_CS_OpenCard(     MYKI_CS_OPEN_TRANSIT_APP, NULL /*DONTCARE*/  ) != MYKI_CS_OK ||
         MYKI_CS_CAControlGet( &pMYKI_CAControl                             ) != MYKI_CS_OK ||
         MYKI_CS_TAControlGet( &pMYKI_TAControl                             ) != MYKI_CS_OK ||
         MYKI_CS_TAProductGet( DirProductInUse, &pMYKI_TAProduct            ) != MYKI_CS_OK )
    {
        return  FALSE;
    }

    /*  GIVEN */
    {
        /*  Card image */
        {
            pMYKI_CAControl->Status                                     = CARD_CONTROL_STATUS_ACTIVATED;
            pMYKI_TAControl->Status                                     = TAPP_CONTROL_STATUS_ACTIVATED;
            pMYKI_TAControl->ProductInUse                               = ProductInUse;
            pMYKI_TAControl->Directory[ ProductInUse ].ProductId        = PRODUCT_ID_NHOUR;
            pMYKI_TAControl->Directory[ ProductInUse ].Status           = TAPP_CONTROL_DIRECTORY_STATUS_ACTIVATED;
            pMYKI_TAProduct->ControlBitmap                              = TAPP_TPRODUCT_CONTROL_PROVISIONAL_BITMAP;
            pMYKI_TAProduct->LastUsage.Location.EntryPointId            = EntryPointId;
            pMYKI_TAProduct->LastUsage.ProviderId                       = ServiceProviderId;
            pMYKI_TAProduct->EndDateTime                                = pData->DynamicData.currentDateTime - MINUTES_TO_SECONDS( MaximumTripTolerance ) + 1;
            pMYKI_TAProduct->StartDateTime                              = pMYKI_TAProduct->EndDateTime - HOURS_TO_SECONDS( 2 );
        }

        /*  BR context data */
        {
            pData->InternalData.TransportMode                           = TRANSPORT_MODE_BUS;
            pData->DynamicData.entryPointId                             = EntryPointId;
            pData->StaticData.serviceProviderId                         = ServiceProviderId;
            pData->Tariff.maximumTripTolerance                          = MaximumTripTolerance;
        }
    }

    /*  WHEN */
    {
        /*  Executes business rule */
        RuleResult  = BR_LLSC_4_20( pData );
    }

    /*  THEN */
    {
        UT_Start( );

        /*  Make sure business rule is bypassed */
        UT_Assert( RuleResult == RULE_RESULT_EXECUTED );

        /*  Ensure card image is correctly updated */
        UT_Assert( pMYKI_TAProduct->EndDateTime == pData->DynamicData.currentDateTime );

        return  UT_Result( );
    }
}   /*  test_BR_LLSC_4_20_002a( ) */

int test_BR_LLSC_4_20_002b( MYKI_BR_ContextData_t *pData )
{
    MYKI_CAControl_t               *pMYKI_CAControl             = NULL;
    MYKI_TAControl_t               *pMYKI_TAControl             = NULL;
    MYKI_TAProduct_t               *pMYKI_TAProduct             = NULL;
    RuleResult_e                    RuleResult                  = RULE_RESULT_ERROR;
    int                             ServiceProviderId           = ProviderId_RAIL;
    int                             EntryPointId                = 6609;
    int                             ProductInUse                = 1;
    int                             DirProductInUse             = ( ProductInUse - 1 );
    int                             MaximumTripTolerance        = 15;

    if ( pData == NULL ||
         MYKI_CS_OpenCard(     MYKI_CS_OPEN_TRANSIT_APP, NULL /*DONTCARE*/  ) != MYKI_CS_OK ||
         MYKI_CS_CAControlGet( &pMYKI_CAControl                             ) != MYKI_CS_OK ||
         MYKI_CS_TAControlGet( &pMYKI_TAControl                             ) != MYKI_CS_OK ||
         MYKI_CS_TAProductGet( DirProductInUse, &pMYKI_TAProduct            ) != MYKI_CS_OK )
    {
        return  FALSE;
    }

    /*  GIVEN */
    {
        /*  Card image */
        {
            pMYKI_CAControl->Status                                     = CARD_CONTROL_STATUS_ACTIVATED;
            pMYKI_TAControl->Status                                     = TAPP_CONTROL_STATUS_ACTIVATED;
            pMYKI_TAControl->ProductInUse                               = ProductInUse;
            pMYKI_TAControl->Directory[ ProductInUse ].ProductId        = PRODUCT_ID_NHOUR;
            pMYKI_TAControl->Directory[ ProductInUse ].Status           = TAPP_CONTROL_DIRECTORY_STATUS_ACTIVATED;
            pMYKI_TAProduct->ControlBitmap                              = TAPP_TPRODUCT_CONTROL_PROVISIONAL_BITMAP;
            pMYKI_TAProduct->LastUsage.Location.EntryPointId            = EntryPointId;
            pMYKI_TAProduct->LastUsage.ProviderId                       = ServiceProviderId;
            pMYKI_TAProduct->EndDateTime                                = pData->DynamicData.currentDateTime - MINUTES_TO_SECONDS( MaximumTripTolerance ) + 1;
            pMYKI_TAProduct->StartDateTime                              = pMYKI_TAProduct->EndDateTime - HOURS_TO_SECONDS( 2 );
        }

        /*  BR context data */
        {
            pData->InternalData.TransportMode                           = TRANSPORT_MODE_RAIL;
            pData->DynamicData.entryPointId                             = EntryPointId;
            pData->StaticData.serviceProviderId                         = ServiceProviderId;
            pData->Tariff.maximumTripTolerance                          = MaximumTripTolerance;
        }
    }

    /*  WHEN */
    {
        /*  Executes business rule */
        RuleResult  = BR_LLSC_4_20( pData );
    }

    /*  THEN */
    {
        UT_Start( );

        /*  Make sure business rule is bypassed */
        UT_Assert( RuleResult == RULE_RESULT_EXECUTED );

        /*  Ensure card image is correctly updated */
        UT_Assert( pMYKI_TAProduct->EndDateTime == pData->DynamicData.currentDateTime );

        return  UT_Result( );
    }
}   /*  test_BR_LLSC_4_20_002b( ) */
