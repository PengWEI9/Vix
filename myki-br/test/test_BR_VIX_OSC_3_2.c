/* -*- mode: c; tabs: 4; -*- */
/*=============================================================================
**
**    Vix Technology                   Licensed software
**    (C) 2015                         All rights reserved
**
**=============================================================================
**
**  Project/Product : MBU
**  Filename        : test_BR_VIX_OSC_3_2.c
**  Author(s)       : An Tran
**
**  Description     :
**      Implements BR_VIX_OSC_3_2 business rule unit-tests.
**
**  Function(s)     :
**      test_BR_VIX_OSC_3_2_XXX     [Public]    unit-test functions
**
**  Information     :
**   Compiler(s)    : ANSI C
**   Target(s)      : Independent
**
2**  Subversion      :
**      $Id: test_BR_VIX_OSC_3_2.c 88449 2016-01-07 00:32:47Z atran $
**      $HeadURL: https://auperasvn01.aupera.erggroup.com/svn/DPG_SWBase/myki-br/trunk/test/test_BR_VIX_OSC_3_2.c $
**
**  History         :
**   Vers.  Date        Aut.  Type     Description
**   -----  ----------  ----  -------  ----------------------------------------
**    1.00  27.10.15    ANT   Create
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
**  test_BR_VIX_OSC_3_2_001a
**  test_BR_VIX_OSC_3_2_001b
**  test_BR_VIX_OSC_3_2_001c
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

int test_BR_VIX_OSC_3_2_001a( MYKI_BR_ContextData_t *pData )
{
    RuleResult_e            RuleResult              = RULE_RESULT_ERROR;

    /*  GIVEN */
    {
        /*  BR context data */
        {
            pData->DynamicData.pShiftData  = NULL;
        }
    }

    /*  WHEN */
    {
        /*  Executes business rule */
        RuleResult  = BR_VIX_OSC_3_2( pData );
    }

    /*  THEN */
    {
        UT_Start( );

        /*  Make sure business rule is bypassed */
        UT_Assert( RuleResult == RULE_RESULT_BYPASSED );
        UT_Assert( pData->ReturnedData.bypassCode == BYPASS_CODE( 3, 2, 1, 0 ) );

        return  UT_Result( );
    }
}   /*  test_BR_VIX_OSC_3_2_001a( ) */

int test_BR_VIX_OSC_3_2_001b( MYKI_BR_ContextData_t *pData )
{
    MYKI_CAControl_t           *pMYKI_CAControl             = NULL;
    MYKI_OAControl_t           *pMYKI_OAControl             = NULL;
    MYKI_OAShiftDataControl_t  *pMYKI_OAShiftDataControl    = NULL;
    RuleResult_e                RuleResult                  = RULE_RESULT_ERROR;
    MYKI_BR_ShiftData_t         MYKI_BR_ShiftData           = { 0 };
    int                         NumberOfHoursBetweenShifts  = 12;
    int                         MaxShiftDownTime            = 2;
    Time_t                      StartTime                   = 0;
    Time_t                      CloseTime                   = 0;

    if ( pData == NULL ||
         MYKI_CS_OpenCard( MYKI_CS_OPEN_OPERATOR_APP, NULL /*DONTCARE*/             ) != MYKI_CS_OK ||
         MYKI_CS_CAControlGet(                          &pMYKI_CAControl            ) != MYKI_CS_OK ||
         MYKI_CS_OAControlGet(                          &pMYKI_OAControl            ) != MYKI_CS_OK ||
         MYKI_CS_OAShiftDataControlGet(                 &pMYKI_OAShiftDataControl   ) != MYKI_CS_OK )
    {
        return FALSE;
    }

    /*  GIVEN */
    {
        /*  Card image */
        {
            pMYKI_CAControl->Status                             = CARD_CONTROL_STATUS_ACTIVATED;
            pMYKI_OAControl->Status                             = OAPP_CONTROL_STATUS_ACTIVATED;

            pMYKI_OAShiftDataControl->Status                    = OAPP_SHIFT_DATA_STATUS_SUSPENDED;
            pMYKI_OAShiftDataControl->StartTime                 = \
                StartTime                                       = pData->DynamicData.currentDateTime - HOURS_TO_SECONDS( NumberOfHoursBetweenShifts );
            pMYKI_OAShiftDataControl->CloseTime                 = \
                CloseTime                                       = pData->DynamicData.currentDateTime - HOURS_TO_SECONDS( MaxShiftDownTime );
        }

        /*  BR context data */
        {
            pData->StaticData.numberOfHoursBetweenShifts        = NumberOfHoursBetweenShifts;
            pData->StaticData.maxShiftDownTime                  = MaxShiftDownTime;
            pData->DynamicData.pShiftData                       = &MYKI_BR_ShiftData;
        }
    }

    /*  WHEN */
    {
        /*  Executes business rule */
        RuleResult  = BR_VIX_OSC_3_2( pData );
    }

    /*  THEN */
    {
        UT_Start( );

        /*  Make sure business rule is bypassed */
        UT_Assert( RuleResult == RULE_RESULT_BYPASSED );
        UT_Assert( pData->ReturnedData.bypassCode == BYPASS_CODE( 3, 2, 2, 0 ) );

        /*  Make sure card image is unchanged */
        UT_Assert( pMYKI_OAShiftDataControl->Status == OAPP_SHIFT_DATA_STATUS_SUSPENDED );
        UT_Assert( pMYKI_OAShiftDataControl->StartTime == StartTime );
        UT_Assert( pMYKI_OAShiftDataControl->CloseTime == CloseTime );

        return  UT_Result( );
    }
}   /*  test_BR_VIX_OSC_3_2_001b( ) */

int test_BR_VIX_OSC_3_2_001c( MYKI_BR_ContextData_t *pData )
{
    MYKI_CAControl_t           *pMYKI_CAControl             = NULL;
    MYKI_OAControl_t           *pMYKI_OAControl             = NULL;
    MYKI_OAShiftDataControl_t  *pMYKI_OAShiftDataControl    = NULL;
    RuleResult_e                RuleResult                  = RULE_RESULT_ERROR;
    MYKI_BR_ShiftData_t         MYKI_BR_ShiftData           = { 0 };
    int                         NumberOfHoursBetweenShifts  = 12;
    int                         MaxShiftDownTime            = 2;
    Time_t                      StartTime                   = 0;
    Time_t                      CloseTime                   = 0;

    if ( pData == NULL ||
         MYKI_CS_OpenCard( MYKI_CS_OPEN_OPERATOR_APP, NULL /*DONTCARE*/             ) != MYKI_CS_OK ||
         MYKI_CS_CAControlGet(                          &pMYKI_CAControl            ) != MYKI_CS_OK ||
         MYKI_CS_OAControlGet(                          &pMYKI_OAControl            ) != MYKI_CS_OK ||
         MYKI_CS_OAShiftDataControlGet(                 &pMYKI_OAShiftDataControl   ) != MYKI_CS_OK )
    {
        return FALSE;
    }

    /*  GIVEN */
    {
        /*  Card image */
        {
            pMYKI_CAControl->Status                             = CARD_CONTROL_STATUS_ACTIVATED;
            pMYKI_OAControl->Status                             = OAPP_CONTROL_STATUS_ACTIVATED;

            pMYKI_OAShiftDataControl->Status                    = OAPP_SHIFT_DATA_STATUS_ACTIVATED;
            pMYKI_OAShiftDataControl->StartTime                 = \
                StartTime                                       = pData->DynamicData.currentDateTime - HOURS_TO_SECONDS( NumberOfHoursBetweenShifts );
            pMYKI_OAShiftDataControl->CloseTime                 = \
                CloseTime                                       = TIME_NOT_SET;
        }

        /*  BR context data */
        {
            pData->StaticData.numberOfHoursBetweenShifts        = NumberOfHoursBetweenShifts;
            pData->StaticData.maxShiftDownTime                  = MaxShiftDownTime;
            pData->DynamicData.pShiftData                       = &MYKI_BR_ShiftData;
        }
    }

    /*  WHEN */
    {
        /*  Executes business rule */
        RuleResult  = BR_VIX_OSC_3_2( pData );
    }

    /*  THEN */
    {
        UT_Start( );

        /*  Make sure business rule is bypassed */
        UT_Assert( RuleResult == RULE_RESULT_BYPASSED );
        UT_Assert( pData->ReturnedData.bypassCode == BYPASS_CODE( 3, 2, 2, 0 ) );

        /*  Make sure card image is unchanged */
        UT_Assert( pMYKI_OAShiftDataControl->Status == OAPP_SHIFT_DATA_STATUS_ACTIVATED );
        UT_Assert( pMYKI_OAShiftDataControl->StartTime == StartTime );
        UT_Assert( pMYKI_OAShiftDataControl->CloseTime == CloseTime );

        return  UT_Result( );
    }
}   /*  test_BR_VIX_OSC_3_2_001c( ) */

/*==========================================================================*
**
**  test_BR_VIX_OSC_3_2_002a
**  test_BR_VIX_OSC_3_2_002b
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

int test_BR_VIX_OSC_3_2_002a( MYKI_BR_ContextData_t *pData )
{
    MYKI_CAControl_t           *pMYKI_CAControl             = NULL;
    MYKI_OAControl_t           *pMYKI_OAControl             = NULL;
    MYKI_OAShiftDataControl_t  *pMYKI_OAShiftDataControl    = NULL;
    RuleResult_e                RuleResult                  = RULE_RESULT_ERROR;
    MYKI_BR_ShiftData_t         MYKI_BR_ShiftData           = { 0 };
    int                         NumberOfHoursBetweenShifts  = 12;
    int                         MaxShiftDownTime            = 2;

    if ( pData == NULL ||
         MYKI_CS_OpenCard( MYKI_CS_OPEN_OPERATOR_APP, NULL /*DONTCARE*/             ) != MYKI_CS_OK ||
         MYKI_CS_CAControlGet(                          &pMYKI_CAControl            ) != MYKI_CS_OK ||
         MYKI_CS_OAControlGet(                          &pMYKI_OAControl            ) != MYKI_CS_OK ||
         MYKI_CS_OAShiftDataControlGet(                 &pMYKI_OAShiftDataControl   ) != MYKI_CS_OK )
    {
        return FALSE;
    }

    /*  GIVEN */
    {
        /*  Card image */
        {
            pMYKI_CAControl->Status                             = CARD_CONTROL_STATUS_ACTIVATED;
            pMYKI_OAControl->Status                             = OAPP_CONTROL_STATUS_ACTIVATED;

            pMYKI_OAShiftDataControl->Status                    = OAPP_SHIFT_DATA_STATUS_UNUSED;
            pMYKI_OAShiftDataControl->ShiftId                   = 9999;
            pMYKI_OAShiftDataControl->ShiftSeqNo                = 88;
            pMYKI_OAShiftDataControl->StartTime                 = 78261;
            pMYKI_OAShiftDataControl->CloseTime                 = 87917;
        }

        /*  BR context data */
        {
            pData->StaticData.numberOfHoursBetweenShifts        = NumberOfHoursBetweenShifts;
            pData->StaticData.maxShiftDownTime                  = MaxShiftDownTime;
            pData->DynamicData.pShiftData                       = &MYKI_BR_ShiftData;
        }
    }

    /*  WHEN */
    {
        /*  Executes business rule */
        RuleResult  = BR_VIX_OSC_3_2( pData );
    }

    /*  THEN */
    {
        UT_Start( );

        /*  Make sure business rule is bypassed */
        UT_Assert( RuleResult == RULE_RESULT_EXECUTED );

        /*  Make sure card image is updated correctly */
        UT_Assert( pMYKI_OAShiftDataControl->Status == OAPP_SHIFT_DATA_STATUS_ACTIVATED );
        UT_Assert( pMYKI_OAShiftDataControl->ShiftId == 0 );
        UT_Assert( pMYKI_OAShiftDataControl->ShiftSeqNo == 0 );
        UT_Assert( pMYKI_OAShiftDataControl->StartTime == pData->DynamicData.currentDateTime );
        UT_Assert( pMYKI_OAShiftDataControl->CloseTime == 0 );

        /*  Make sure shift data set correctly */
        UT_Assert( MYKI_BR_ShiftData.shiftStatus == OAPP_SHIFT_DATA_STATUS_ACTIVATED );
        UT_Assert( MYKI_BR_ShiftData.shiftId == 0 );
        UT_Assert( MYKI_BR_ShiftData.shiftStartTime == pData->DynamicData.currentDateTime );
        UT_Assert( MYKI_BR_ShiftData.shiftEndTime == TIME_NOT_SET );
        UT_Assert( MYKI_BR_ShiftData.paperTicketReturns == 0 );
        UT_Assert( MYKI_BR_ShiftData.sundryItemReturns == 0 );
        UT_Assert( MYKI_BR_ShiftData.cardReturns == 0 );
        UT_Assert( MYKI_BR_ShiftData.shiftTotalsTPurse.RecordType == PAYMENT_METHOD_TPURSE );
        UT_Assert( MYKI_BR_ShiftData.shiftTotalsTPurse.SalesCount == 0 );
        UT_Assert( MYKI_BR_ShiftData.shiftTotalsTPurse.SalesValue == 0 );
        UT_Assert( MYKI_BR_ShiftData.shiftTotalsTPurse.ReversalsCount == 0 );
        UT_Assert( MYKI_BR_ShiftData.shiftTotalsTPurse.ReversalsValue == 0 );
        UT_Assert( MYKI_BR_ShiftData.shiftTotalsCash.RecordType == PAYMENT_METHOD_CASH );
        UT_Assert( MYKI_BR_ShiftData.shiftTotalsCash.SalesCount == 0 );
        UT_Assert( MYKI_BR_ShiftData.shiftTotalsCash.SalesValue == 0 );
        UT_Assert( MYKI_BR_ShiftData.shiftTotalsCash.ReversalsCount == 0 );
        UT_Assert( MYKI_BR_ShiftData.shiftTotalsCash.ReversalsValue == 0 );
        UT_Assert( MYKI_BR_ShiftData.shiftPortionStatus == SHIFT_PORTION_RESET );

        return  UT_Result( );
    }
}   /* test_BR_VIX_OSC_3_2_002a( ) */

int test_BR_VIX_OSC_3_2_002b( MYKI_BR_ContextData_t *pData )
{
    MYKI_CAControl_t           *pMYKI_CAControl             = NULL;
    MYKI_OAControl_t           *pMYKI_OAControl             = NULL;
    MYKI_OAShiftDataControl_t  *pMYKI_OAShiftDataControl    = NULL;
    RuleResult_e                RuleResult                  = RULE_RESULT_ERROR;
    MYKI_BR_ShiftData_t         MYKI_BR_ShiftData           = { 0 };
    int                         NumberOfHoursBetweenShifts  = 12;
    int                         MaxShiftDownTime            = 2;
    Time_t                      StartTime                   = 0;
    Time_t                      CloseTime                   = 0;

    if ( pData == NULL ||
         MYKI_CS_OpenCard( MYKI_CS_OPEN_OPERATOR_APP, NULL /*DONTCARE*/             ) != MYKI_CS_OK ||
         MYKI_CS_CAControlGet(                          &pMYKI_CAControl            ) != MYKI_CS_OK ||
         MYKI_CS_OAControlGet(                          &pMYKI_OAControl            ) != MYKI_CS_OK ||
         MYKI_CS_OAShiftDataControlGet(                 &pMYKI_OAShiftDataControl   ) != MYKI_CS_OK )
    {
        return FALSE;
    }

    /*  GIVEN */
    {
        /*  Card image */
        {
            pMYKI_CAControl->Status                             = CARD_CONTROL_STATUS_ACTIVATED;
            pMYKI_OAControl->Status                             = OAPP_CONTROL_STATUS_ACTIVATED;

            pMYKI_OAShiftDataControl->Status                    = OAPP_SHIFT_DATA_STATUS_SUSPENDED;
            pMYKI_OAShiftDataControl->ShiftId                   = 9999;
            pMYKI_OAShiftDataControl->ShiftSeqNo                = 88;
            pMYKI_OAShiftDataControl->StartTime                 = \
                StartTime                                       = pData->DynamicData.currentDateTime - HOURS_TO_SECONDS( NumberOfHoursBetweenShifts ) - 1;
            pMYKI_OAShiftDataControl->CloseTime                 = \
                CloseTime                                       = pData->DynamicData.currentDateTime - HOURS_TO_SECONDS( MaxShiftDownTime + 1 );
        }

        /*  BR context data */
        {
            pData->StaticData.numberOfHoursBetweenShifts        = NumberOfHoursBetweenShifts;
            pData->StaticData.maxShiftDownTime                  = MaxShiftDownTime;
            pData->DynamicData.pShiftData                       = &MYKI_BR_ShiftData;
        }
    }

    /*  WHEN */
    {
        /*  Executes business rule */
        RuleResult  = BR_VIX_OSC_3_2( pData );
    }

    /*  THEN */
    {
        UT_Start( );

        /*  Make sure business rule is bypassed */
        UT_Assert( RuleResult == RULE_RESULT_EXECUTED );

        /*  Make sure card image is updated correctly */
        UT_Assert( pMYKI_OAShiftDataControl->Status == OAPP_SHIFT_DATA_STATUS_ACTIVATED );
        UT_Assert( pMYKI_OAShiftDataControl->ShiftId == 0 );
        UT_Assert( pMYKI_OAShiftDataControl->ShiftSeqNo == 0 );
        UT_Assert( pMYKI_OAShiftDataControl->StartTime == pData->DynamicData.currentDateTime );
        UT_Assert( pMYKI_OAShiftDataControl->CloseTime == 0 );

        /*  Make sure shift data set correctly */
        UT_Assert( MYKI_BR_ShiftData.shiftStatus == OAPP_SHIFT_DATA_STATUS_ACTIVATED );
        UT_Assert( MYKI_BR_ShiftData.shiftId == 0 );
        UT_Assert( MYKI_BR_ShiftData.shiftStartTime == pData->DynamicData.currentDateTime );
        UT_Assert( MYKI_BR_ShiftData.shiftEndTime == TIME_NOT_SET );
        UT_Assert( MYKI_BR_ShiftData.paperTicketReturns == 0 );
        UT_Assert( MYKI_BR_ShiftData.sundryItemReturns == 0 );
        UT_Assert( MYKI_BR_ShiftData.cardReturns == 0 );
        UT_Assert( MYKI_BR_ShiftData.shiftTotalsTPurse.RecordType == PAYMENT_METHOD_TPURSE );
        UT_Assert( MYKI_BR_ShiftData.shiftTotalsTPurse.SalesCount == 0 );
        UT_Assert( MYKI_BR_ShiftData.shiftTotalsTPurse.SalesValue == 0 );
        UT_Assert( MYKI_BR_ShiftData.shiftTotalsTPurse.ReversalsCount == 0 );
        UT_Assert( MYKI_BR_ShiftData.shiftTotalsTPurse.ReversalsValue == 0 );
        UT_Assert( MYKI_BR_ShiftData.shiftTotalsCash.RecordType == PAYMENT_METHOD_CASH );
        UT_Assert( MYKI_BR_ShiftData.shiftTotalsCash.SalesCount == 0 );
        UT_Assert( MYKI_BR_ShiftData.shiftTotalsCash.SalesValue == 0 );
        UT_Assert( MYKI_BR_ShiftData.shiftTotalsCash.ReversalsCount == 0 );
        UT_Assert( MYKI_BR_ShiftData.shiftTotalsCash.ReversalsValue == 0 );
        UT_Assert( MYKI_BR_ShiftData.shiftPortionStatus == SHIFT_PORTION_RESET );

        return  UT_Result( );
    }
}   /* test_BR_VIX_OSC_3_2_002b( ) */

int test_BR_VIX_OSC_3_2_002c( MYKI_BR_ContextData_t *pData )
{
    MYKI_CAControl_t           *pMYKI_CAControl             = NULL;
    MYKI_OAControl_t           *pMYKI_OAControl             = NULL;
    MYKI_OAShiftDataControl_t  *pMYKI_OAShiftDataControl    = NULL;
    RuleResult_e                RuleResult                  = RULE_RESULT_ERROR;
    MYKI_BR_ShiftData_t         MYKI_BR_ShiftData           = { 0 };
    int                         NumberOfHoursBetweenShifts  = 12;
    int                         MaxShiftDownTime            = 2;
    Time_t                      StartTime                   = 0;
    Time_t                      CloseTime                   = 0;

    if ( pData == NULL ||
         MYKI_CS_OpenCard( MYKI_CS_OPEN_OPERATOR_APP, NULL /*DONTCARE*/             ) != MYKI_CS_OK ||
         MYKI_CS_CAControlGet(                          &pMYKI_CAControl            ) != MYKI_CS_OK ||
         MYKI_CS_OAControlGet(                          &pMYKI_OAControl            ) != MYKI_CS_OK ||
         MYKI_CS_OAShiftDataControlGet(                 &pMYKI_OAShiftDataControl   ) != MYKI_CS_OK )
    {
        return FALSE;
    }

    /*  GIVEN */
    {
        /*  Card image */
        {
            pMYKI_CAControl->Status                             = CARD_CONTROL_STATUS_ACTIVATED;
            pMYKI_OAControl->Status                             = OAPP_CONTROL_STATUS_ACTIVATED;

            pMYKI_OAShiftDataControl->Status                    = OAPP_SHIFT_DATA_STATUS_SUSPENDED;
            pMYKI_OAShiftDataControl->ShiftId                   = 9999;
            pMYKI_OAShiftDataControl->ShiftSeqNo                = 88;
            pMYKI_OAShiftDataControl->StartTime                 = \
                StartTime                                       = pData->DynamicData.currentDateTime - HOURS_TO_SECONDS( NumberOfHoursBetweenShifts + 1 );
            pMYKI_OAShiftDataControl->CloseTime                 = \
                CloseTime                                       = pData->DynamicData.currentDateTime - HOURS_TO_SECONDS( MaxShiftDownTime ) - 1;
        }

        /*  BR context data */
        {
            pData->StaticData.numberOfHoursBetweenShifts        = NumberOfHoursBetweenShifts;
            pData->StaticData.maxShiftDownTime                  = MaxShiftDownTime;
            pData->DynamicData.pShiftData                       = &MYKI_BR_ShiftData;
        }
    }

    /*  WHEN */
    {
        /*  Executes business rule */
        RuleResult  = BR_VIX_OSC_3_2( pData );
    }

    /*  THEN */
    {
        UT_Start( );

        /*  Make sure business rule is bypassed */
        UT_Assert( RuleResult == RULE_RESULT_EXECUTED );

        /*  Make sure card image is updated correctly */
        UT_Assert( pMYKI_OAShiftDataControl->Status == OAPP_SHIFT_DATA_STATUS_ACTIVATED );
        UT_Assert( pMYKI_OAShiftDataControl->ShiftId == 0 );
        UT_Assert( pMYKI_OAShiftDataControl->ShiftSeqNo == 0 );
        UT_Assert( pMYKI_OAShiftDataControl->StartTime == pData->DynamicData.currentDateTime );
        UT_Assert( pMYKI_OAShiftDataControl->CloseTime == 0 );

        /*  Make sure shift data set correctly */
        UT_Assert( MYKI_BR_ShiftData.shiftStatus == OAPP_SHIFT_DATA_STATUS_ACTIVATED );
        UT_Assert( MYKI_BR_ShiftData.shiftId == 0 );
        UT_Assert( MYKI_BR_ShiftData.shiftStartTime == pData->DynamicData.currentDateTime );
        UT_Assert( MYKI_BR_ShiftData.shiftEndTime == TIME_NOT_SET );
        UT_Assert( MYKI_BR_ShiftData.paperTicketReturns == 0 );
        UT_Assert( MYKI_BR_ShiftData.sundryItemReturns == 0 );
        UT_Assert( MYKI_BR_ShiftData.cardReturns == 0 );
        UT_Assert( MYKI_BR_ShiftData.shiftTotalsTPurse.RecordType == PAYMENT_METHOD_TPURSE );
        UT_Assert( MYKI_BR_ShiftData.shiftTotalsTPurse.SalesCount == 0 );
        UT_Assert( MYKI_BR_ShiftData.shiftTotalsTPurse.SalesValue == 0 );
        UT_Assert( MYKI_BR_ShiftData.shiftTotalsTPurse.ReversalsCount == 0 );
        UT_Assert( MYKI_BR_ShiftData.shiftTotalsTPurse.ReversalsValue == 0 );
        UT_Assert( MYKI_BR_ShiftData.shiftTotalsCash.RecordType == PAYMENT_METHOD_CASH );
        UT_Assert( MYKI_BR_ShiftData.shiftTotalsCash.SalesCount == 0 );
        UT_Assert( MYKI_BR_ShiftData.shiftTotalsCash.SalesValue == 0 );
        UT_Assert( MYKI_BR_ShiftData.shiftTotalsCash.ReversalsCount == 0 );
        UT_Assert( MYKI_BR_ShiftData.shiftTotalsCash.ReversalsValue == 0 );
        UT_Assert( MYKI_BR_ShiftData.shiftPortionStatus == SHIFT_PORTION_RESET );

        return  UT_Result( );
    }
}   /* test_BR_VIX_OSC_3_2_002c( ) */