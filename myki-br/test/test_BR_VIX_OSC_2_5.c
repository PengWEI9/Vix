/* -*- mode: c; tabs: 4; -*- */
/*=============================================================================
**
**    Vix Technology                   Licensed software
**    (C) 2015                         All rights reserved
**
**=============================================================================
**
**  Project/Product : MBU
**  Filename        : test_BR_VIX_OSC_2_5.c
**  Author(s)       : An Tran
**
**  Description     :
**      Implements BR_VIX_OSC_2_5 business rule unit-tests.
**
**  Function(s)     :
**      test_BR_VIX_OSC_2_5_XXX     [Public]    unit-test functions
**
**  Information     :
**   Compiler(s)    : ANSI C
**   Target(s)      : Independent
**
2**  Subversion      :
**      $Id: test_BR_VIX_OSC_2_5.c 80137 2015-11-03 02:46:50Z atran $
**      $HeadURL: https://auperasvn01.aupera.erggroup.com/svn/DPG_SWBase/myki-br/trunk/test/test_BR_VIX_OSC_2_5.c $
**
**  History         :
**   Vers.  Date        Aut.  Type     Description
**   -----  ----------  ----  -------  ----------------------------------------
**    1.00  30.10.15    ANT   Create
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
**  test_BR_VIX_OSC_2_5_001a
**  test_BR_VIX_OSC_2_5_001b
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

int test_BR_VIX_OSC_2_5_001a( MYKI_BR_ContextData_t *pData )
{
    MYKI_CAControl_t               *pMYKI_CAControl             = NULL;
    MYKI_OAControl_t               *pMYKI_OAControl             = NULL;
    MYKI_OARoles_t                 *pMYKI_OARoles               = NULL;
    RuleResult_e                    RuleResult                  = RULE_RESULT_ERROR;
    U8_t                            RoleType_Driver             = 1;
    U8_t                            RoleProfile_Driver          = 2;

    if ( pData == NULL ||
         MYKI_CS_OpenCard( MYKI_CS_OPEN_OPERATOR_APP, NULL /*DONTCARE*/             ) != MYKI_CS_OK ||
         MYKI_CS_CAControlGet(                          &pMYKI_CAControl            ) != MYKI_CS_OK ||
         MYKI_CS_OAControlGet(                          &pMYKI_OAControl            ) != MYKI_CS_OK ||
         MYKI_CS_OARolesGet(                            &pMYKI_OARoles              ) != MYKI_CS_OK )
    {
        return FALSE;
    }

    /*  GIVEN */
    {
        /*  Card image */
        {
            pMYKI_CAControl->Status                             = CARD_CONTROL_STATUS_ACTIVATED;
            pMYKI_OAControl->Status                             = OAPP_CONTROL_STATUS_ACTIVATED;
            pMYKI_OAControl->RolesCount                         = 1;
            pMYKI_OARoles->Role[ 0 ].Type                       = RoleType_Driver + 1;  /*  != Driver Type */
            pMYKI_OARoles->Role[ 0 ].Profile                    = RoleProfile_Driver;
        }

        /*  BR context data */
        {
            memset( pData->StaticData.staffRoles, 0, sizeof( pData->StaticData.staffRoles ) );
            pData->StaticData.staffRoles[ 0 ].type              = RoleType_Driver;
            pData->StaticData.staffRoles[ 0 ].profile           = RoleProfile_Driver;
            pData->StaticData.staffRoles[ 0 ].operatorType      = OperatorType_DRIVER;
        }
    }

    /*  WHEN */
    {
        /*  Executes business rule */
        RuleResult  = BR_VIX_OSC_2_5( pData );
    }

    /*  THEN */
    {
        UT_Start( );

        /*  Make sure business rule is bypassed */
        UT_Assert( RuleResult == RULE_RESULT_BYPASSED );
        UT_Assert( pData->ReturnedData.bypassCode == BYPASS_CODE( 2, 5, 1, 0 ) );

        return  UT_Result( );
    }
}   /*  test_BR_VIX_OSC_2_5_001a( ) */

int test_BR_VIX_OSC_2_5_001b( MYKI_BR_ContextData_t *pData )
{
    MYKI_CAControl_t               *pMYKI_CAControl             = NULL;
    MYKI_OAControl_t               *pMYKI_OAControl             = NULL;
    MYKI_OARoles_t                 *pMYKI_OARoles               = NULL;
    RuleResult_e                    RuleResult                  = RULE_RESULT_ERROR;
    U8_t                            RoleType_Driver             = 1;
    U8_t                            RoleProfile_Driver          = 2;

    if ( pData == NULL ||
         MYKI_CS_OpenCard( MYKI_CS_OPEN_OPERATOR_APP, NULL /*DONTCARE*/             ) != MYKI_CS_OK ||
         MYKI_CS_CAControlGet(                          &pMYKI_CAControl            ) != MYKI_CS_OK ||
         MYKI_CS_OAControlGet(                          &pMYKI_OAControl            ) != MYKI_CS_OK ||
         MYKI_CS_OARolesGet(                            &pMYKI_OARoles              ) != MYKI_CS_OK )
    {
        return FALSE;
    }

    /*  GIVEN */
    {
        /*  Card image */
        {
            pMYKI_CAControl->Status                             = CARD_CONTROL_STATUS_ACTIVATED;
            pMYKI_OAControl->Status                             = OAPP_CONTROL_STATUS_ACTIVATED;
            pMYKI_OAControl->RolesCount                         = 1;
            pMYKI_OARoles->Role[ 0 ].Type                       = RoleType_Driver;
            pMYKI_OARoles->Role[ 0 ].Profile                    = RoleProfile_Driver + 1;   /*  != Driver Profile */
        }

        /*  BR context data */
        {
            memset( pData->StaticData.staffRoles, 0, sizeof( pData->StaticData.staffRoles ) );
            pData->StaticData.staffRoles[ 0 ].type              = RoleType_Driver;
            pData->StaticData.staffRoles[ 0 ].profile           = RoleProfile_Driver;
            pData->StaticData.staffRoles[ 0 ].operatorType      = OperatorType_DRIVER;
        }
    }

    /*  WHEN */
    {
        /*  Executes business rule */
        RuleResult  = BR_VIX_OSC_2_5( pData );
    }

    /*  THEN */
    {
        UT_Start( );

        /*  Make sure business rule is bypassed */
        UT_Assert( RuleResult == RULE_RESULT_BYPASSED );
        UT_Assert( pData->ReturnedData.bypassCode == BYPASS_CODE( 2, 5, 1, 0 ) );

        return  UT_Result( );
    }
}   /*  test_BR_VIX_OSC_2_5_001b( ) */

/*==========================================================================*
**
**  test_BR_VIX_OSC_2_5_002a
**
**  Description     :
**      Unit-test EXECUTED conditions, Invalid PIN.
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

int test_BR_VIX_OSC_2_5_002a( MYKI_BR_ContextData_t *pData )
{
    MYKI_CAControl_t               *pMYKI_CAControl             = NULL;
    MYKI_OAControl_t               *pMYKI_OAControl             = NULL;
    MYKI_OARoles_t                 *pMYKI_OARoles               = NULL;
    RuleResult_e                    RuleResult                  = RULE_RESULT_ERROR;
    U8_t                            RoleType_Driver             = 1;
    U8_t                            RoleProfile_Driver          = 2;
    U8_t                            RoleType_OA                 = 3;
    U8_t                            RoleProfile_OA              = 4;

    if ( pData == NULL ||
         MYKI_CS_OpenCard( MYKI_CS_OPEN_OPERATOR_APP, NULL /*DONTCARE*/             ) != MYKI_CS_OK ||
         MYKI_CS_CAControlGet(                          &pMYKI_CAControl            ) != MYKI_CS_OK ||
         MYKI_CS_OAControlGet(                          &pMYKI_OAControl            ) != MYKI_CS_OK ||
         MYKI_CS_OARolesGet(                            &pMYKI_OARoles              ) != MYKI_CS_OK )
    {
        return FALSE;
    }

    /*  GIVEN */
    {
        /*  Card image */
        {
            pMYKI_CAControl->Status                             = CARD_CONTROL_STATUS_ACTIVATED;
            pMYKI_OAControl->Status                             = OAPP_CONTROL_STATUS_ACTIVATED;
            pMYKI_OAControl->RolesCount                         = 2;
            pMYKI_OARoles->Role[ 0 ].Type                       = RoleType_OA;
            pMYKI_OARoles->Role[ 0 ].Profile                    = RoleProfile_OA;
            pMYKI_OARoles->Role[ 1 ].Type                       = RoleType_Driver;
            pMYKI_OARoles->Role[ 1 ].Profile                    = RoleProfile_Driver;
        }

        /*  BR context data */
        {
            memset( pData->StaticData.staffRoles, 0, sizeof( pData->StaticData.staffRoles ) );
            pData->StaticData.staffRoles[ 0 ].type              = RoleType_OA;
            pData->StaticData.staffRoles[ 0 ].profile           = RoleProfile_OA;
            pData->StaticData.staffRoles[ 0 ].operatorType      = OperatorType_AUTHORISED_OFFICER;
            pData->StaticData.staffRoles[ 1 ].type              = RoleType_Driver;
            pData->StaticData.staffRoles[ 1 ].profile           = RoleProfile_Driver;
            pData->StaticData.staffRoles[ 1 ].operatorType      = OperatorType_DRIVER;
        }
    }

    /*  WHEN */
    {
        /*  Executes business rule */
        RuleResult  = BR_VIX_OSC_2_5( pData );
    }

    /*  THEN */
    {
        UT_Start( );

        /*  Make sure business rule is executed */
        UT_Assert( RuleResult == RULE_RESULT_EXECUTED );

        return  UT_Result( );
    }
}   /* test_BR_VIX_OSC_2_5_002a( ) */
