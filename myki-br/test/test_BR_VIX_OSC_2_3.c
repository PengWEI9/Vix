/* -*- mode: c; tabs: 4; -*- */
/*=============================================================================
**
**    Vix Technology                   Licensed software
**    (C) 2015                         All rights reserved
**
**=============================================================================
**
**  Project/Product : MBU
**  Filename        : test_BR_VIX_OSC_2_3.c
**  Author(s)       : An Tran
**
**  Description     :
**      Implements BR_VIX_OSC_2_3 business rule unit-tests.
**
**  Function(s)     :
**      test_BR_VIX_OSC_2_3_XXX     [Public]    unit-test functions
**
**  Information     :
**   Compiler(s)    : ANSI C
**   Target(s)      : Independent
**
**  Subversion      :
**      $Id: test_BR_VIX_OSC_2_3.c 69449 2015-08-06 01:54:22Z ehutchis $
**      $HeadURL: https://auperasvn01.aupera.erggroup.com/svn/DPG_SWBase/myki-br/trunk/test/test_BR_VIX_OSC_2_3.c $
**
**  History         :
**   Vers.  Date        Aut.  Type     Description
**   -----  ----------  ----  -------  ----------------------------------------
**    1.00  28.05.15    ANT   Create
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
**  test_BR_VIX_OSC_2_3_001a
**  test_BR_VIX_OSC_2_3_001b
**  test_BR_VIX_OSC_2_3_001c
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

int test_BR_VIX_OSC_2_3_001a( MYKI_BR_ContextData_t *pData )
{
    MYKI_CAControl_t       *pMYKI_CAControl         = NULL;
    MYKI_OAControl_t       *pMYKI_OAControl         = NULL;
    MYKI_OAPIN_t           *pMYKI_OAPIN             = NULL;
    int                     EntryPointId            = 53;
    int                     LineId                  = 66;
    int                     StopId                  = 79;
    int                     ServiceProviderId       = 100;
    int                     MaxPinRetries           = 10;
    int                     InvalidPinCount         = 5;
    char                   *OperatorPin             = "12345678";
    RuleResult_e            RuleResult              = RULE_RESULT_ERROR;

    if ( pData == NULL ||
         MYKI_CS_CAControlGet(  &pMYKI_CAControl    ) != MYKI_CS_OK ||
         MYKI_CS_OAControlGet(  &pMYKI_OAControl    ) != MYKI_CS_OK ||
         MYKI_CS_OAPINGet(      &pMYKI_OAPIN        ) != MYKI_CS_OK )
    {
        return FALSE;
    }

    /*  GIVEN */
    {
        /*  Card image */
        {
            pMYKI_CAControl->Status                                         = CARD_CONTROL_STATUS_ACTIVATED;
            pMYKI_OAControl->Status                                         = OAPP_CONTROL_STATUS_ACTIVATED;
            pMYKI_OAControl->BlockingReason                                 = OAPP_CONTROL_BLOCKING_REASON_RESERVED;
            pMYKI_OAControl->StaffProviderId                                = ( ServiceProviderId + 1 );    /*  != Static.ServiceProviderId */
            pMYKI_OAControl->InvalidPinCount                                = InvalidPinCount;
            memcpy( pMYKI_OAPIN->Pin, OperatorPin, sizeof( pMYKI_OAPIN->Pin ) );
        }

        /*  BR context data */
        {
            pData->DynamicData.entryPointId                                 = EntryPointId;
            pData->DynamicData.lineId                                       = LineId;
            pData->DynamicData.stopId                                       = StopId;
            pData->StaticData.serviceProviderId                             = ServiceProviderId;
            pData->StaticData.maxPinRetries                                 = MaxPinRetries;
        }
    }

    /*  WHEN */
    {
        /*  PIN entered */
        {
            memcpy( pData->DynamicData.operatorPin, OperatorPin, sizeof( pData->DynamicData.operatorPin ) );
        }

        /*  Executes business rule */
        RuleResult  = BR_VIX_OSC_2_3( pData );
    }

    /*  THEN */
    {
        UT_Start( );

        /*  Make sure business rule is bypassed */
        UT_Assert( RuleResult == RULE_RESULT_BYPASSED );
        UT_Assert( pData->ReturnedData.bypassCode == BYPASS_CODE( 2, 3, 2, 0 ) );

        /*  Make sure card image is unchanged */
        UT_Assert( pMYKI_OAControl->InvalidPinCount == InvalidPinCount );

        /*  Make sure reject code is correct */
        UT_Assert( pData->ReturnedData.rejectReason == MYKI_BR_REJECT_REASON_INVALID_SERVICE_PROVIDER );

        return  UT_Result( );
    }
}   /*  test_BR_VIX_OSC_2_3_001a( ) */

int test_BR_VIX_OSC_2_3_001b( MYKI_BR_ContextData_t *pData )
{
    MYKI_CAControl_t       *pMYKI_CAControl         = NULL;
    MYKI_OAControl_t       *pMYKI_OAControl         = NULL;
    MYKI_OAPIN_t           *pMYKI_OAPIN             = NULL;
    int                     EntryPointId            = 53;
    int                     LineId                  = 66;
    int                     StopId                  = 79;
    int                     ServiceProviderId       = 100;
    int                     MaxPinRetries           = 10;
    int                     InvalidPinCount         = 5;
    char                   *OperatorPin             = "12345678";
    RuleResult_e            RuleResult              = RULE_RESULT_ERROR;

    if ( pData == NULL ||
         MYKI_CS_CAControlGet(  &pMYKI_CAControl    ) != MYKI_CS_OK ||
         MYKI_CS_OAControlGet(  &pMYKI_OAControl    ) != MYKI_CS_OK ||
         MYKI_CS_OAPINGet(      &pMYKI_OAPIN        ) != MYKI_CS_OK )
    {
        return FALSE;
    }

    /*  GIVEN */
    {
        /*  Card image */
        {
            pMYKI_CAControl->Status                                         = CARD_CONTROL_STATUS_ACTIVATED;
            pMYKI_OAControl->Status                                         = OAPP_CONTROL_STATUS_BLOCKED;
            pMYKI_OAControl->BlockingReason                                 = OAPP_CONTROL_BLOCKING_REASON_HOTLISTED_SAM;   /*  != OAPP_CONTROL_BLOCKING_REASON_INVALID_PIN */
            pMYKI_OAControl->StaffProviderId                                = ServiceProviderId;
            pMYKI_OAControl->InvalidPinCount                                = InvalidPinCount;
            memcpy( pMYKI_OAPIN->Pin, OperatorPin, sizeof( pMYKI_OAPIN->Pin ) );
        }

        /*  BR context data */
        {
            pData->DynamicData.entryPointId                                 = EntryPointId;
            pData->DynamicData.lineId                                       = LineId;
            pData->DynamicData.stopId                                       = StopId;
            pData->StaticData.serviceProviderId                             = ServiceProviderId;
            pData->StaticData.maxPinRetries                                 = MaxPinRetries;
        }
    }

    /*  WHEN */
    {
        /*  PIN entered */
        {
            memcpy( pData->DynamicData.operatorPin, OperatorPin, sizeof( pData->DynamicData.operatorPin ) );
        }

        /*  Executes business rule */
        RuleResult  = BR_VIX_OSC_2_3( pData );
    }

    /*  THEN */
    {
        UT_Start( );

        /*  Make sure business rule is bypassed */
        UT_Assert( RuleResult == RULE_RESULT_BYPASSED );
        UT_Assert( pData->ReturnedData.bypassCode == BYPASS_CODE( 2, 3, 1, 1 ) );

        /*  Make sure card image is unchanged */
        UT_Assert( pMYKI_OAControl->Status == OAPP_CONTROL_STATUS_BLOCKED );
        UT_Assert( pMYKI_OAControl->BlockingReason == OAPP_CONTROL_BLOCKING_REASON_HOTLISTED_SAM );
        UT_Assert( pMYKI_OAControl->InvalidPinCount == InvalidPinCount );

        /*  Make sure reject code is correct */
        UT_Assert( pData->ReturnedData.rejectReason == MYKI_BR_REJECT_REASON_APPLICATION_BLOCKED );

        return  UT_Result( );
    }
}   /*  test_BR_VIX_OSC_2_3_001b( ) */

int test_BR_VIX_OSC_2_3_001c( MYKI_BR_ContextData_t *pData )
{
    MYKI_CAControl_t       *pMYKI_CAControl         = NULL;
    MYKI_OAControl_t       *pMYKI_OAControl         = NULL;
    MYKI_OAPIN_t           *pMYKI_OAPIN             = NULL;
    int                     EntryPointId            = 53;
    int                     LineId                  = 66;
    int                     StopId                  = 79;
    int                     ServiceProviderId       = 100;
    int                     MaxPinRetries           = 10;
    int                     InvalidPinCount         = 5;
    char                   *OperatorPin             = "12345678";
    RuleResult_e            RuleResult              = RULE_RESULT_ERROR;

    if ( pData == NULL ||
         MYKI_CS_CAControlGet(  &pMYKI_CAControl    ) != MYKI_CS_OK ||
         MYKI_CS_OAControlGet(  &pMYKI_OAControl    ) != MYKI_CS_OK ||
         MYKI_CS_OAPINGet(      &pMYKI_OAPIN        ) != MYKI_CS_OK )
    {
        return FALSE;
    }

    /*  GIVEN */
    {
        /*  Card image */
        {
            pMYKI_CAControl->Status                                         = CARD_CONTROL_STATUS_ACTIVATED;
            pMYKI_OAControl->Status                                         = OAPP_CONTROL_STATUS_ISSUED;   /*  != OAPP_CONTROL_STATUS_ACTIVATED */
            pMYKI_OAControl->BlockingReason                                 = OAPP_CONTROL_BLOCKING_REASON_RESERVED;
            pMYKI_OAControl->StaffProviderId                                = ServiceProviderId;
            pMYKI_OAControl->InvalidPinCount                                = InvalidPinCount;
            memcpy( pMYKI_OAPIN->Pin, OperatorPin, sizeof( pMYKI_OAPIN->Pin ) );
        }

        /*  BR context data */
        {
            pData->DynamicData.entryPointId                                 = EntryPointId;
            pData->DynamicData.lineId                                       = LineId;
            pData->DynamicData.stopId                                       = StopId;
            pData->StaticData.serviceProviderId                             = ServiceProviderId;
            pData->StaticData.maxPinRetries                                 = MaxPinRetries;
        }
    }

    /*  WHEN */
    {
        /*  PIN entered */
        {
            memcpy( pData->DynamicData.operatorPin, OperatorPin, sizeof( pData->DynamicData.operatorPin ) );
        }

        /*  Executes business rule */
        RuleResult  = BR_VIX_OSC_2_3( pData );
    }

    /*  THEN */
    {
        UT_Start( );

        /*  Make sure business rule is bypassed */
        UT_Assert( RuleResult == RULE_RESULT_BYPASSED );
        UT_Assert( pData->ReturnedData.bypassCode == BYPASS_CODE( 2, 3, 1, 2 ) );

        /*  Make sure card image is unchanged */
        UT_Assert( pMYKI_OAControl->Status == OAPP_CONTROL_STATUS_ISSUED );
        UT_Assert( pMYKI_OAControl->BlockingReason == OAPP_CONTROL_BLOCKING_REASON_RESERVED );
        UT_Assert( pMYKI_OAControl->InvalidPinCount == InvalidPinCount );

        /*  Make sure reject code is correct */
        UT_Assert( pData->ReturnedData.rejectReason == MYKI_BR_REJECT_REASON_APPLICATION_BLOCKED );

        return  UT_Result( );
    }
}   /*  test_BR_VIX_OSC_2_3_001c( ) */

/*==========================================================================*
**
**  test_BR_VIX_OSC_2_3_002
**
**  Description     :
**      Unit-test EXECUTED conditions, reset PIN.
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

int test_BR_VIX_OSC_2_3_002( MYKI_BR_ContextData_t *pData )
{
    MYKI_CAControl_t       *pMYKI_CAControl         = NULL;
    MYKI_OAControl_t       *pMYKI_OAControl         = NULL;
    MYKI_OAPIN_t           *pMYKI_OAPIN             = NULL;
    int                     EntryPointId            = 53;
    int                     LineId                  = 66;
    int                     StopId                  = 79;
    int                     ServiceProviderId       = 100;
    int                     MaxPinRetries           = 10;
    int                     InvalidPinCount         = 5;
    char                   *OperatorPin             = "12345678";
    RuleResult_e            RuleResult              = RULE_RESULT_ERROR;

    if ( pData == NULL ||
         MYKI_CS_CAControlGet(  &pMYKI_CAControl    ) != MYKI_CS_OK ||
         MYKI_CS_OAControlGet(  &pMYKI_OAControl    ) != MYKI_CS_OK ||
         MYKI_CS_OAPINGet(      &pMYKI_OAPIN        ) != MYKI_CS_OK )
    {
        return FALSE;
    }

    /*  GIVEN */
    {
        /*  Card image */
        {
            pMYKI_CAControl->Status                                         = CARD_CONTROL_STATUS_ACTIVATED;
            pMYKI_OAControl->Status                                         = OAPP_CONTROL_STATUS_ACTIVATED;
            pMYKI_OAControl->BlockingReason                                 = OAPP_CONTROL_BLOCKING_REASON_RESERVED;
            pMYKI_OAControl->StaffProviderId                                = ServiceProviderId;
            pMYKI_OAControl->InvalidPinCount                                = InvalidPinCount;
            memcpy( pMYKI_OAPIN->Pin, OperatorPin, sizeof( pMYKI_OAPIN->Pin ) );
        }

        /*  BR context data */
        {
            pData->DynamicData.entryPointId                                 = EntryPointId;
            pData->DynamicData.lineId                                       = LineId;
            pData->DynamicData.stopId                                       = StopId;
            pData->StaticData.serviceProviderId                             = ServiceProviderId;
            pData->StaticData.maxPinRetries                                 = MaxPinRetries;
        }
    }

    /*  WHEN */
    {
        /*  No new PIN entered */
        {
            memset( pData->DynamicData.operatorPin, 0, sizeof( pData->DynamicData.operatorPin ) );
        }

        /*  Executes business rule */
        RuleResult  = BR_VIX_OSC_2_3( pData );
    }

    /*  THEN */
    {
        UT_Start( );

        /*  Make sure business rule is executed */
        UT_Assert( RuleResult == RULE_RESULT_EXECUTED );

        /*  Make sure card image is updated correctly */
        UT_Assert( pMYKI_OAPIN->Pin[0]=='\0' );
        UT_Assert( pMYKI_OAControl->Status == OAPP_CONTROL_STATUS_ACTIVATED );
        UT_Assert( pMYKI_OAControl->BlockingReason == OAPP_CONTROL_BLOCKING_REASON_RESERVED );
        UT_Assert( pMYKI_OAControl->InvalidPinCount == 0 );

        return  UT_Result( );
    }
}   /* test_BR_VIX_OSC_2_3_002( ) */

/*==========================================================================*
**
**  test_BR_VIX_OSC_2_3_003
**
**  Description     :
**      Unit-test EXECUTED conditions, new PIN entered.
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

int test_BR_VIX_OSC_2_3_003( MYKI_BR_ContextData_t *pData )
{
    MYKI_CAControl_t       *pMYKI_CAControl         = NULL;
    MYKI_OAControl_t       *pMYKI_OAControl         = NULL;
    MYKI_OAPIN_t           *pMYKI_OAPIN             = NULL;
    int                     EntryPointId            = 53;
    int                     LineId                  = 66;
    int                     StopId                  = 79;
    int                     ServiceProviderId       = 100;
    int                     MaxPinRetries           = 10;
    int                     InvalidPinCount         = 5;
    char                   *OperatorPin             = "12345678";
    char                   *NewOperatorPin          = "87654321";
    RuleResult_e            RuleResult              = RULE_RESULT_ERROR;

    if ( pData == NULL ||
         MYKI_CS_CAControlGet(  &pMYKI_CAControl    ) != MYKI_CS_OK ||
         MYKI_CS_OAControlGet(  &pMYKI_OAControl    ) != MYKI_CS_OK ||
         MYKI_CS_OAPINGet(      &pMYKI_OAPIN        ) != MYKI_CS_OK )
    {
        return FALSE;
    }

    /*  GIVEN */
    {
        /*  Card image */
        {
            pMYKI_CAControl->Status                                         = CARD_CONTROL_STATUS_ACTIVATED;
            pMYKI_OAControl->Status                                         = OAPP_CONTROL_STATUS_BLOCKED;
            pMYKI_OAControl->BlockingReason                                 = OAPP_CONTROL_BLOCKING_REASON_INVALID_PIN;
            pMYKI_OAControl->StaffProviderId                                = ServiceProviderId;
            pMYKI_OAControl->InvalidPinCount                                = InvalidPinCount;
            memcpy( pMYKI_OAPIN->Pin, OperatorPin, sizeof( pMYKI_OAPIN->Pin ) );
        }

        /*  BR context data */
        {
            pData->DynamicData.entryPointId                                 = EntryPointId;
            pData->DynamicData.lineId                                       = LineId;
            pData->DynamicData.stopId                                       = StopId;
            pData->StaticData.serviceProviderId                             = ServiceProviderId;
            pData->StaticData.maxPinRetries                                 = MaxPinRetries;
        }
    }

    /*  WHEN */
    {
        /*  New PIN entered */
        {
            memcpy( pData->DynamicData.operatorPin, NewOperatorPin, sizeof( pData->DynamicData.operatorPin ) );
        }

        /*  Executes business rule */
        RuleResult  = BR_VIX_OSC_2_3( pData );
    }

    /*  THEN */
    {
        UT_Start( );

        /*  Make sure business rule is executed */
        UT_Assert( RuleResult == RULE_RESULT_EXECUTED );

        /*  Make sure card image is updated correctly */
        UT_Assert( memcmp( pMYKI_OAPIN->Pin, NewOperatorPin, sizeof( pMYKI_OAPIN->Pin ) ) == 0 );
        UT_Assert( pMYKI_OAControl->Status == OAPP_CONTROL_STATUS_ACTIVATED );
        UT_Assert( pMYKI_OAControl->BlockingReason == OAPP_CONTROL_BLOCKING_REASON_RESERVED );
        UT_Assert( pMYKI_OAControl->InvalidPinCount == 0 );

        return  UT_Result( );
    }
}   /* test_BR_VIX_OSC_2_3_003( ) */
