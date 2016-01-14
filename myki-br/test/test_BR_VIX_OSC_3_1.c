/* -*- mode: c; tabs: 4; -*- */
/*=============================================================================
**
**    Vix Technology                   Licensed software
**    (C) 2015                         All rights reserved
**
**=============================================================================
**
**  Project/Product : MBU
**  Filename        : test_BR_VIX_OSC_3_1.c
**  Author(s)       : An Tran
**
**  Description     :
**      Implements BR_VIX_OSC_3_1 business rule unit-tests.
**
**  Function(s)     :
**      test_BR_VIX_OSC_3_1_XXX     [Public]    unit-test functions
**
**  Information     :
**   Compiler(s)    : ANSI C
**   Target(s)      : Independent
**
2**  Subversion      :
**      $Id: test_BR_VIX_OSC_3_1.c 88449 2016-01-07 00:32:47Z atran $
**      $HeadURL: https://auperasvn01.aupera.erggroup.com/svn/DPG_SWBase/myki-br/trunk/test/test_BR_VIX_OSC_3_1.c $
**
**  History         :
**   Vers.  Date        Aut.  Type     Description
**   -----  ----------  ----  -------  ----------------------------------------
**    1.00  06.10.15    ANT   Create
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
**  test_BR_VIX_OSC_3_1_001a
**  test_BR_VIX_OSC_3_1_001b
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

int test_BR_VIX_OSC_3_1_001a( MYKI_BR_ContextData_t *pData )
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
        RuleResult  = BR_VIX_OSC_3_1( pData );
    }

    /*  THEN */
    {
        UT_Start( );

        /*  Make sure business rule is bypassed */
        UT_Assert( RuleResult == RULE_RESULT_BYPASSED );
        UT_Assert( pData->ReturnedData.bypassCode == BYPASS_CODE( 3, 1, 1, 0 ) );

        return  UT_Result( );
    }
}   /*  test_BR_VIX_OSC_3_1_001a( ) */

int test_BR_VIX_OSC_3_1_001b( MYKI_BR_ContextData_t *pData )
{
    MYKI_CAControl_t               *pMYKI_CAControl             = NULL;
    MYKI_OAControl_t               *pMYKI_OAControl             = NULL;
    MYKI_OAShiftDataControl_t      *pMYKI_OAShiftDataControl    = NULL;
    RuleResult_e                    RuleResult                  = RULE_RESULT_ERROR;
    MYKI_BR_ShiftData_t             MYKI_BR_ShiftData           = { 0 };
    Time_t                          StartTime                   = TIME_NOT_SET;
    int                             ShiftId_High                = 2;
    int                             ShiftId_Low                 = 871;
    int                             ShiftId                     = ( ( ShiftId_High << 16 ) | ShiftId_Low );

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
            pMYKI_OAShiftDataControl->Status                    = OAPP_SHIFT_DATA_STATUS_ACTIVATED; /*  != "SUSPENDED" or "CLOSED" */
            pMYKI_OAShiftDataControl->ShiftId                   = ShiftId_Low;
            pMYKI_OAShiftDataControl->ShiftSeqNo                = ShiftId_High;
            pMYKI_OAShiftDataControl->StartTime                 = \
                StartTime                                       = pData->DynamicData.currentDateTime - HOURS_TO_SECONDS( 1 );
            pMYKI_OAShiftDataControl->CloseTime                 = TIME_NOT_SET;
        }

        /*  BR context data */
        {
            pData->DynamicData.pShiftData                       = &MYKI_BR_ShiftData;
        }
    }

    /*  WHEN */
    {
        /*  Executes business rule */
        RuleResult  = BR_VIX_OSC_3_1( pData );
    }

    /*  THEN */
    {
        UT_Start( );

        /*  Make sure business rule is bypassed */
        UT_Assert( RuleResult == RULE_RESULT_BYPASSED );
        UT_Assert( pData->ReturnedData.bypassCode == BYPASS_CODE( 3, 1, 2, 0 ) );

        /*  Make sure card image is unchanged */
        UT_Assert( pMYKI_OAShiftDataControl->Status == OAPP_SHIFT_DATA_STATUS_ACTIVATED );
        UT_Assert( pMYKI_OAShiftDataControl->ShiftId == ShiftId_Low );
        UT_Assert( pMYKI_OAShiftDataControl->ShiftSeqNo == ShiftId_High );
        UT_Assert( pMYKI_OAShiftDataControl->StartTime == StartTime );
        UT_Assert( pMYKI_OAShiftDataControl->CloseTime == TIME_NOT_SET );

        return  UT_Result( );
    }
}   /*  test_BR_VIX_OSC_3_1_001b( ) */

/*==========================================================================*
**
**  test_BR_VIX_OSC_3_1_002a
**  test_BR_VIX_OSC_3_1_002b
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

int test_BR_VIX_OSC_3_1_002a( MYKI_BR_ContextData_t *pData )
{
    MYKI_CAControl_t               *pMYKI_CAControl             = NULL;
    MYKI_OAControl_t               *pMYKI_OAControl             = NULL;
    MYKI_OAShiftDataControl_t      *pMYKI_OAShiftDataControl    = NULL;
    MYKI_OAShiftDataRecordList_t   *pMYKI_OAShiftDataRecordList = NULL;
    RuleResult_e                    RuleResult                  = RULE_RESULT_ERROR;
    MYKI_BR_ShiftData_t             MYKI_BR_ShiftData           = { 0 };
    Time_t                          StartTime                   = TIME_NOT_SET;
    Time_t                          CloseTime                   = TIME_NOT_SET;
    int                             ShiftId_High                = 2;
    int                             ShiftId_Low                 = 871;
    int                             ShiftId                     = ( ( ShiftId_High << 16 ) | ShiftId_Low );

    if ( pData == NULL ||
         MYKI_CS_OpenCard( MYKI_CS_OPEN_OPERATOR_APP, NULL /*DONTCARE*/     ) != MYKI_CS_OK ||
         MYKI_CS_CAControlGet(              &pMYKI_CAControl                ) != MYKI_CS_OK ||
         MYKI_CS_OAControlGet(              &pMYKI_OAControl                ) != MYKI_CS_OK ||
         MYKI_CS_OAShiftDataControlGet(     &pMYKI_OAShiftDataControl       ) != MYKI_CS_OK ||
         MYKI_CS_OAShiftDataRecordsGet( 0,  &pMYKI_OAShiftDataRecordList    ) != MYKI_CS_OK )
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
            pMYKI_OAShiftDataControl->ActiveRecordCount         = 0;
            pMYKI_OAShiftDataControl->ShiftId                   = ShiftId_Low;
            pMYKI_OAShiftDataControl->ShiftSeqNo                = ShiftId_High;
            pMYKI_OAShiftDataControl->StartTime                 = \
                StartTime                                       = pData->DynamicData.currentDateTime - HOURS_TO_SECONDS( 4 );
            pMYKI_OAShiftDataControl->CloseTime                 = \
                CloseTime                                       = pData->DynamicData.currentDateTime - HOURS_TO_SECONDS( 1 );
        }

        /*  BR context data */
        {
            pData->DynamicData.pShiftData                       = &MYKI_BR_ShiftData;
        }
    }

    /*  WHEN */
    {
        /*  Executes business rule */
        RuleResult  = BR_VIX_OSC_3_1( pData );
    }

    /*  THEN */
    {
        UT_Start( );

        /*  Make sure business rule is bypassed */
        UT_Assert( RuleResult == RULE_RESULT_EXECUTED );

        /*  Make sure card image updated correctly */
        UT_Assert( pMYKI_OAShiftDataControl->Status == OAPP_SHIFT_DATA_STATUS_ACTIVATED );
        UT_Assert( pMYKI_OAShiftDataControl->ActiveRecordCount == 0 );
        UT_Assert( pMYKI_OAShiftDataControl->ShiftId == ShiftId_Low );
        UT_Assert( pMYKI_OAShiftDataControl->ShiftSeqNo == ShiftId_High );
        UT_Assert( pMYKI_OAShiftDataControl->StartTime == StartTime );
        UT_Assert( pMYKI_OAShiftDataControl->CloseTime == TIME_NOT_SET );

        /*  Make sure returned shift data is correct */
        UT_Assert( MYKI_BR_ShiftData.shiftStatus == OAPP_SHIFT_DATA_STATUS_ACTIVATED );
        UT_Assert( MYKI_BR_ShiftData.shiftId == ShiftId );
        UT_Assert( MYKI_BR_ShiftData.shiftStartTime == StartTime );
        UT_Assert( MYKI_BR_ShiftData.shiftEndTime == TIME_NOT_SET );
        UT_Assert( MYKI_BR_ShiftData.shiftTotals[ PAYMENT_METHOD_TPURSE ].RecordType == PAYMENT_METHOD_TPURSE );
        UT_Assert( MYKI_BR_ShiftData.shiftTotals[ PAYMENT_METHOD_TPURSE ].SalesCount == 0 );
        UT_Assert( MYKI_BR_ShiftData.shiftTotals[ PAYMENT_METHOD_TPURSE ].SalesValue == 0 );
        UT_Assert( MYKI_BR_ShiftData.shiftTotals[ PAYMENT_METHOD_TPURSE ].ReversalsCount == 0 );
        UT_Assert( MYKI_BR_ShiftData.shiftTotals[ PAYMENT_METHOD_TPURSE ].ReversalsValue == 0 );
        UT_Assert( MYKI_BR_ShiftData.shiftTotals[ PAYMENT_METHOD_CASH   ].RecordType == PAYMENT_METHOD_CASH );
        UT_Assert( MYKI_BR_ShiftData.shiftTotals[ PAYMENT_METHOD_CASH   ].SalesCount == 0 );
        UT_Assert( MYKI_BR_ShiftData.shiftTotals[ PAYMENT_METHOD_CASH   ].SalesValue == 0 );
        UT_Assert( MYKI_BR_ShiftData.shiftTotals[ PAYMENT_METHOD_CASH   ].ReversalsCount == 0 );
        UT_Assert( MYKI_BR_ShiftData.shiftTotals[ PAYMENT_METHOD_CASH   ].ReversalsValue == 0 );
        UT_Assert( MYKI_BR_ShiftData.shiftPortionStatus == SHIFT_PORTION_BROUGHT_FORWARD );

        return  UT_Result( );
    }
}   /* test_BR_VIX_OSC_3_1_002a( ) */

int test_BR_VIX_OSC_3_1_002b( MYKI_BR_ContextData_t *pData )
{
    MYKI_CAControl_t               *pMYKI_CAControl             = NULL;
    MYKI_OAControl_t               *pMYKI_OAControl             = NULL;
    MYKI_OAShiftDataControl_t      *pMYKI_OAShiftDataControl    = NULL;
    MYKI_OAShiftDataRecordList_t   *pMYKI_OAShiftDataRecordList = NULL;
    RuleResult_e                    RuleResult                  = RULE_RESULT_ERROR;
    MYKI_BR_ShiftData_t             MYKI_BR_ShiftData           = { 0 };
    Time_t                          StartTime                   = TIME_NOT_SET;
    Time_t                          CloseTime                   = TIME_NOT_SET;
    int                             ShiftId_High                = 2;
    int                             ShiftId_Low                 = 871;
    int                             ShiftId                     = ( ( ShiftId_High << 16 ) | ShiftId_Low );
    int                             SalesCountCash              = 3;
    int                             SalesValueCash              = 1300;
    int                             ReversalsCountCash          = 2;
    int                             ReversalsValueCash          = 760;
    int                             SalesCountTPurse            = 8;
    int                             SalesValueTPurse            = 980;
    int                             ReversalsCountTPurse        = 4;
    int                             ReversalsValueTPurse        = 320;

    if ( pData == NULL ||
         MYKI_CS_OpenCard( MYKI_CS_OPEN_OPERATOR_APP, NULL /*DONTCARE*/     ) != MYKI_CS_OK ||
         MYKI_CS_CAControlGet(              &pMYKI_CAControl                ) != MYKI_CS_OK ||
         MYKI_CS_OAControlGet(              &pMYKI_OAControl                ) != MYKI_CS_OK ||
         MYKI_CS_OAShiftDataControlGet(     &pMYKI_OAShiftDataControl       ) != MYKI_CS_OK ||
         MYKI_CS_OAShiftDataRecordsGet( 0,  &pMYKI_OAShiftDataRecordList    ) != MYKI_CS_OK )
    {
        return FALSE;
    }

    /*  GIVEN */
    {
        /*  Card image */
        {
            pMYKI_CAControl->Status                             = CARD_CONTROL_STATUS_ACTIVATED;
            pMYKI_OAControl->Status                             = OAPP_CONTROL_STATUS_ACTIVATED;

            pMYKI_OAShiftDataControl->Status                    = OAPP_SHIFT_DATA_STATUS_CLOSED;
            pMYKI_OAShiftDataControl->ActiveRecordCount         = 2;
            pMYKI_OAShiftDataControl->ShiftId                   = ShiftId_Low;
            pMYKI_OAShiftDataControl->ShiftSeqNo                = ShiftId_High;
            pMYKI_OAShiftDataControl->StartTime                 = \
                StartTime                                       = pData->DynamicData.currentDateTime - HOURS_TO_SECONDS( 4 );
            pMYKI_OAShiftDataControl->CloseTime                 = \
                CloseTime                                       = pData->DynamicData.currentDateTime - HOURS_TO_SECONDS( 1 );

            pMYKI_OAShiftDataRecordList->NumberOfRecords                            = 2;
            pMYKI_OAShiftDataRecordList->MYKI_ShiftDataRecord[ 0 ].RecordType       = PAYMENT_METHOD_CASH;
            pMYKI_OAShiftDataRecordList->MYKI_ShiftDataRecord[ 0 ].SalesCount       = SalesCountCash;
            pMYKI_OAShiftDataRecordList->MYKI_ShiftDataRecord[ 0 ].SalesValue       = SalesValueCash;
            pMYKI_OAShiftDataRecordList->MYKI_ShiftDataRecord[ 0 ].ReversalsCount   = ReversalsCountCash;
            pMYKI_OAShiftDataRecordList->MYKI_ShiftDataRecord[ 0 ].ReversalsValue   = ReversalsValueCash;
            pMYKI_OAShiftDataRecordList->MYKI_ShiftDataRecord[ 1 ].RecordType       = PAYMENT_METHOD_TPURSE;
            pMYKI_OAShiftDataRecordList->MYKI_ShiftDataRecord[ 1 ].SalesCount       = SalesCountTPurse;
            pMYKI_OAShiftDataRecordList->MYKI_ShiftDataRecord[ 1 ].SalesValue       = SalesValueTPurse;
            pMYKI_OAShiftDataRecordList->MYKI_ShiftDataRecord[ 1 ].ReversalsCount   = ReversalsCountTPurse;
            pMYKI_OAShiftDataRecordList->MYKI_ShiftDataRecord[ 1 ].ReversalsValue   = ReversalsValueTPurse;
        }

        /*  BR context data */
        {
            pData->DynamicData.pShiftData                       = &MYKI_BR_ShiftData;
        }
    }

    /*  WHEN */
    {
        /*  Executes business rule */
        RuleResult  = BR_VIX_OSC_3_1( pData );
    }

    /*  THEN */
    {
        UT_Start( );

        /*  Make sure business rule is bypassed */
        UT_Assert( RuleResult == RULE_RESULT_EXECUTED );

        /*  Make sure card image updated correctly */
        UT_Assert( pMYKI_OAShiftDataControl->Status == OAPP_SHIFT_DATA_STATUS_ACTIVATED );
        UT_Assert( pMYKI_OAShiftDataControl->ActiveRecordCount == 2 );
        UT_Assert( pMYKI_OAShiftDataControl->ShiftId == ShiftId_Low );
        UT_Assert( pMYKI_OAShiftDataControl->ShiftSeqNo == ShiftId_High );
        UT_Assert( pMYKI_OAShiftDataControl->StartTime == StartTime );
        UT_Assert( pMYKI_OAShiftDataControl->CloseTime == TIME_NOT_SET );

        UT_Assert( pMYKI_OAShiftDataRecordList->NumberOfRecords == 2 );
        UT_Assert( pMYKI_OAShiftDataRecordList->MYKI_ShiftDataRecord[ 0 ].RecordType == PAYMENT_METHOD_CASH );
        UT_Assert( pMYKI_OAShiftDataRecordList->MYKI_ShiftDataRecord[ 0 ].SalesCount == SalesCountCash );
        UT_Assert( pMYKI_OAShiftDataRecordList->MYKI_ShiftDataRecord[ 0 ].SalesValue == SalesValueCash );
        UT_Assert( pMYKI_OAShiftDataRecordList->MYKI_ShiftDataRecord[ 0 ].ReversalsCount == ReversalsCountCash );
        UT_Assert( pMYKI_OAShiftDataRecordList->MYKI_ShiftDataRecord[ 0 ].ReversalsValue == ReversalsValueCash );
        UT_Assert( pMYKI_OAShiftDataRecordList->MYKI_ShiftDataRecord[ 1 ].RecordType == PAYMENT_METHOD_TPURSE );
        UT_Assert( pMYKI_OAShiftDataRecordList->MYKI_ShiftDataRecord[ 1 ].SalesCount == SalesCountTPurse );
        UT_Assert( pMYKI_OAShiftDataRecordList->MYKI_ShiftDataRecord[ 1 ].SalesValue == SalesValueTPurse );
        UT_Assert( pMYKI_OAShiftDataRecordList->MYKI_ShiftDataRecord[ 1 ].ReversalsCount == ReversalsCountTPurse );
        UT_Assert( pMYKI_OAShiftDataRecordList->MYKI_ShiftDataRecord[ 1 ].ReversalsValue == ReversalsValueTPurse );

        /*  Make sure returned shift data is correct */
        UT_Assert( MYKI_BR_ShiftData.shiftStatus == OAPP_SHIFT_DATA_STATUS_ACTIVATED );
        UT_Assert( MYKI_BR_ShiftData.shiftId == ShiftId );
        UT_Assert( MYKI_BR_ShiftData.shiftStartTime == StartTime );
        UT_Assert( MYKI_BR_ShiftData.shiftEndTime == TIME_NOT_SET );
        UT_Assert( MYKI_BR_ShiftData.shiftTotals[ PAYMENT_METHOD_TPURSE ].RecordType == PAYMENT_METHOD_TPURSE );
        UT_Assert( MYKI_BR_ShiftData.shiftTotals[ PAYMENT_METHOD_TPURSE ].SalesCount == SalesCountTPurse );
        UT_Assert( MYKI_BR_ShiftData.shiftTotals[ PAYMENT_METHOD_TPURSE ].SalesValue == SalesValueTPurse );
        UT_Assert( MYKI_BR_ShiftData.shiftTotals[ PAYMENT_METHOD_TPURSE ].ReversalsCount == ReversalsCountTPurse );
        UT_Assert( MYKI_BR_ShiftData.shiftTotals[ PAYMENT_METHOD_TPURSE ].ReversalsValue == ReversalsValueTPurse );
        UT_Assert( MYKI_BR_ShiftData.shiftTotals[ PAYMENT_METHOD_CASH   ].RecordType == PAYMENT_METHOD_CASH );
        UT_Assert( MYKI_BR_ShiftData.shiftTotals[ PAYMENT_METHOD_CASH   ].SalesCount == SalesCountCash );
        UT_Assert( MYKI_BR_ShiftData.shiftTotals[ PAYMENT_METHOD_CASH   ].SalesValue == SalesValueCash );
        UT_Assert( MYKI_BR_ShiftData.shiftTotals[ PAYMENT_METHOD_CASH   ].ReversalsCount == ReversalsCountCash );
        UT_Assert( MYKI_BR_ShiftData.shiftTotals[ PAYMENT_METHOD_CASH   ].ReversalsValue == ReversalsValueCash );
        UT_Assert( MYKI_BR_ShiftData.shiftPortionStatus == SHIFT_PORTION_BROUGHT_FORWARD );

        return  UT_Result( );
    }
}   /* test_BR_VIX_OSC_3_1_002b( ) */
