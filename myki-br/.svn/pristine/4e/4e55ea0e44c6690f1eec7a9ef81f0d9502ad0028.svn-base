/* -*- mode: c; tabs: 4; -*- */
/*=============================================================================
**
**    Vix Technology                   Licensed software
**    (C) 2015                         All rights reserved
**
**=============================================================================
**
**  Project/Product : MBU
**  Filename        : test_BR_VIX_OSC_3_3.c
**  Author(s)       : Damian Chiem
**
**  Description     :
**      Implements BR_VIX_OSC_3_3 business rule unit-tests.
**
**  Function(s)     :
**      test_BR_VIX_OSC_3_3_XXX     [Public]    unit-test functions
**
**  Information     :
**   Compiler(s)    : ANSI C
**   Target(s)      : Independent
**
2**  Subversion      :
**      $Id: $
**      $HeadURL: $
**
**  History         :
**   Vers.  Date        Aut.  Type     Description
**   -----  ----------  ----  -------  ----------------------------------------
**    1.00  06.10.15    DCH   Create
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

int test_BR_VIX_OSC_3_3_001a( MYKI_BR_ContextData_t *pData )
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
        RuleResult  = BR_VIX_OSC_3_3( pData );
    }

    /*  THEN */
    {
        UT_Start( );

        /*  Make sure business rule is bypassed */
        UT_Assert( RuleResult == RULE_RESULT_BYPASSED );
        UT_Assert( pData->ReturnedData.bypassCode == BYPASS_CODE( 3, 3, 1, 0 ) );

        return  UT_Result( );
    }
}   /*  test_BR_VIX_OSC_3_3_001a( ) */

int test_BR_VIX_OSC_3_3_001b( MYKI_BR_ContextData_t *pData )
{
    MYKI_BR_ShiftData_t             MYKI_BR_ShiftData           = { 0 };
    RuleResult_e                    RuleResult                  = RULE_RESULT_ERROR;

    /*  GIVEN */
    {
        /*  BR context data */
        {
            pData->DynamicData.pShiftData                       = &MYKI_BR_ShiftData;
            MYKI_BR_ShiftData.shiftPortionStatus                = SHIFT_PORTION_OUT_OF_BALANCE; /*  != "RESET" != "BROUGHT-FORWARD" */
        }
    }

    /*  WHEN */
    {
        /*  Executes business rule */
        RuleResult  = BR_VIX_OSC_3_3( pData );
    }

    /*  THEN */
    {
        UT_Start( );

        /*  Make sure business rule is bypassed */
        UT_Assert( RuleResult == RULE_RESULT_BYPASSED );
        UT_Assert( pData->ReturnedData.bypassCode == BYPASS_CODE( 3, 3, 2, 0 ) );

        return  UT_Result( );
    }
}   /*  test_BR_VIX_OSC_3_3_001b( ) */

/*==========================================================================*
**
**  test_BR_VIX_OSC_3_1_002a
**  test_BR_VIX_OSC_3_1_002b
**  test_BR_VIX_OSC_3_1_002c
**  test_BR_VIX_OSC_3_1_002d
**
**  Description     :
**      Unit-test EXECUTED conditions, update shift records to the card
**
**      test_BR_VIX_OSC_3_1_002a - Update an existing record if it exists on the card
**
**      test_BR_VIX_OSC_3_1_002b - Write the new record to the card if it does not exist
**
**      test_BR_VIX_OSC_3_1_002c - update the existing record with correct indexing
**
**      test_BR_VIX_OSC_3_1_002d - shift summary log has been updated
**
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
int test_BR_VIX_OSC_3_3_002a( MYKI_BR_ContextData_t *pData )
{
    MYKI_CAControl_t               *pMYKI_CAControl             = NULL;
    MYKI_OAControl_t               *pMYKI_OAControl             = NULL;
    MYKI_OARoles_t                 *pMYKI_OARoles               = NULL;
    MYKI_OAShiftDataControl_t      *pMYKI_OAShiftDataControl    = NULL;
    MYKI_OAShiftDataRecordList_t   *pMYKI_OAShiftDataRecordList = NULL;
    RuleResult_e                    RuleResult                  = RULE_RESULT_ERROR;
    MYKI_BR_ShiftData_t             MYKI_BR_ShiftData           = { 0 };
    U8_t                            RoleType_Driver             = 1;
    U8_t                            RoleProfile_Driver          = 2;
    int                             ShiftId_High                = 2;
    int                             ShiftId_Low                 = 871;
    int                             ShiftId                     = ( ( ShiftId_High << 16 ) | ShiftId_Low );
    Time_t                          StartTime                   = 0;
    Time_t                          CloseTime                   = 0;

    if ( pData == NULL ||
         MYKI_CS_OpenCard( MYKI_CS_OPEN_OPERATOR_APP, NULL /*DONTCARE*/     ) != MYKI_CS_OK ||
         MYKI_CS_CAControlGet(              &pMYKI_CAControl                ) != MYKI_CS_OK ||
         MYKI_CS_OAControlGet(              &pMYKI_OAControl                ) != MYKI_CS_OK ||
         MYKI_CS_OARolesGet(                &pMYKI_OARoles                  ) != MYKI_CS_OK ||
         MYKI_CS_OAShiftDataControlGet(     &pMYKI_OAShiftDataControl       ) != MYKI_CS_OK ||
         MYKI_CS_OAShiftDataRecordsGet( 0,  &pMYKI_OAShiftDataRecordList    ) != MYKI_CS_OK )
    {
        return FALSE;
    }

    /*  GIVEN */
    {
        /*  Card image */
        {
            pMYKI_CAControl->Status                                             = CARD_CONTROL_STATUS_ACTIVATED;
            pMYKI_OAControl->Status                                             = OAPP_CONTROL_STATUS_ACTIVATED;
            pMYKI_OAControl->RolesCount                                         = 1;
            pMYKI_OARoles->Role[ 0 ].Type                                       = RoleType_Driver;
            pMYKI_OARoles->Role[ 0 ].Profile                                    = RoleProfile_Driver;
            pMYKI_OAShiftDataControl->Status                                    = OAPP_SHIFT_DATA_STATUS_ACTIVATED;
            pMYKI_OAShiftDataControl->ShiftId                                   = ShiftId_Low;
            pMYKI_OAShiftDataControl->ShiftSeqNo                                = ShiftId_High;
            pMYKI_OAShiftDataControl->ActiveRecordCount                         = 1;
            pMYKI_OAShiftDataControl->StartTime                                 = \
                StartTime                                                       = pData->DynamicData.currentDateTime - HOURS_TO_SECONDS( 1 );
            pMYKI_OAShiftDataControl->CloseTime                                 = TIME_NOT_SET;
           
            /* Shift record data on card */
            pMYKI_OAShiftDataRecordList->NumberOfRecords                        = 1;
            pMYKI_OAShiftDataRecordList->MYKI_ShiftDataRecord[0].RecordType     = PAYMENT_METHOD_TPURSE;
            pMYKI_OAShiftDataRecordList->MYKI_ShiftDataRecord[0].SalesCount     = 22;
            pMYKI_OAShiftDataRecordList->MYKI_ShiftDataRecord[0].SalesValue     = 1000;
            pMYKI_OAShiftDataRecordList->MYKI_ShiftDataRecord[0].ReversalsCount = 11;
            pMYKI_OAShiftDataRecordList->MYKI_ShiftDataRecord[0].ReversalsValue = 200;
        }

        /*  BR context data */
        {
            memset( pData->StaticData.staffRoles, 0, sizeof( pData->StaticData.staffRoles ) );
            pData->StaticData.staffRoles[ 0 ].type                              = RoleType_Driver;
            pData->StaticData.staffRoles[ 0 ].profile                           = RoleProfile_Driver;
            pData->StaticData.staffRoles[ 0 ].operatorType                      = OperatorType_DRIVER;
            pData->DynamicData.pShiftData                                       = &MYKI_BR_ShiftData;
            
            /* Shift record data */
            MYKI_BR_ShiftData.shiftId                                           = ShiftId;
            MYKI_BR_ShiftData.shiftStartTime                                    = StartTime;
            MYKI_BR_ShiftData.shiftEndTime                                      = \
                CloseTime                                                       = pData->DynamicData.currentDateTime;
            MYKI_BR_ShiftData.shiftTotalsTPurse.RecordType                      = PAYMENT_METHOD_TPURSE;
            MYKI_BR_ShiftData.shiftTotalsTPurse.SalesCount                      = 33;
            MYKI_BR_ShiftData.shiftTotalsTPurse.SalesValue                      = 3122;
            MYKI_BR_ShiftData.shiftTotalsTPurse.ReversalsCount                  = 11;
            MYKI_BR_ShiftData.shiftTotalsTPurse.ReversalsValue                  = 200;
            MYKI_BR_ShiftData.paperTicketReturns                                = 1;
            MYKI_BR_ShiftData.sundryItemReturns                                 = 5;
            MYKI_BR_ShiftData.cardReturns                                       = 6;
            MYKI_BR_ShiftData.shiftPortionStatus                                = SHIFT_PORTION_BROUGHT_FORWARD;
        }
    }

    /*  WHEN */
    {
        /*  Executes business rule */
        RuleResult  = BR_VIX_OSC_3_3( pData );
    }

    /*  THEN */
    {
        UT_Start( );

        /*  Make sure business rule is executed */
        UT_Assert( RuleResult == RULE_RESULT_EXECUTED );

        /*  Make sure card image is updated correctly */
        UT_Assert( pMYKI_OAShiftDataControl->Status == OAPP_SHIFT_DATA_STATUS_SUSPENDED );
        UT_Assert( pMYKI_OAShiftDataControl->ShiftId == ShiftId_Low );
        UT_Assert( pMYKI_OAShiftDataControl->ShiftSeqNo == ShiftId_High );
        UT_Assert( pMYKI_OAShiftDataControl->ActiveRecordCount == 1 );
        UT_Assert( pMYKI_OAShiftDataControl->StartTime == StartTime );
        UT_Assert( pMYKI_OAShiftDataControl->CloseTime == CloseTime );

        //UT_Assert( pMYKI_OAShiftDataRecordList->NumberOfRecords == 1 );
        UT_Assert( pMYKI_OAShiftDataRecordList->MYKI_ShiftDataRecord[0].RecordType        == MYKI_BR_ShiftData.shiftTotalsTPurse.RecordType );
        UT_Assert( pMYKI_OAShiftDataRecordList->MYKI_ShiftDataRecord[0].SalesCount        == MYKI_BR_ShiftData.shiftTotalsTPurse.SalesCount );
        UT_Assert( pMYKI_OAShiftDataRecordList->MYKI_ShiftDataRecord[0].SalesValue        == MYKI_BR_ShiftData.shiftTotalsTPurse.SalesValue );
        UT_Assert( pMYKI_OAShiftDataRecordList->MYKI_ShiftDataRecord[0].ReversalsCount    == MYKI_BR_ShiftData.shiftTotalsTPurse.ReversalsCount );
        UT_Assert( pMYKI_OAShiftDataRecordList->MYKI_ShiftDataRecord[0].ReversalsValue    == MYKI_BR_ShiftData.shiftTotalsTPurse.ReversalsValue ); 

        /* Make sure log data from shift buffer is transferred */
        UT_Assert( pData->InternalData.ShiftDataSummaryLog.shiftId                 ==  ShiftId_Low );
        UT_Assert( pData->InternalData.ShiftDataSummaryLog.shiftSequenceNumber     ==  ShiftId_High );
        UT_Assert( pData->InternalData.ShiftDataSummaryLog.samId                   ==  pData->StaticData.deviceSamId );
        UT_Assert( pData->InternalData.ShiftDataSummaryLog.startTime               ==  StartTime );
        UT_Assert( pData->InternalData.ShiftDataSummaryLog.closeTime               ==  CloseTime );
        UT_Assert( pData->InternalData.ShiftDataSummaryLog.depositSequenceNumber   ==  0 );
        UT_Assert( pData->InternalData.ShiftDataSummaryLog.depositTotal            ==  ( MYKI_BR_ShiftData.shiftTotalsCash.SalesValue - 
                                                                                         MYKI_BR_ShiftData.shiftTotalsCash.ReversalsValue) );
        UT_Assert( pData->InternalData.ShiftDataSummaryLog.paperTicketReturns      ==  MYKI_BR_ShiftData.paperTicketReturns );
        UT_Assert( pData->InternalData.ShiftDataSummaryLog.sundryItemReturns       ==  MYKI_BR_ShiftData.sundryItemReturns );
        UT_Assert( pData->InternalData.ShiftDataSummaryLog.cardReturns             ==  MYKI_BR_ShiftData.cardReturns );
        
        UT_Assert( pData->InternalData.IsShiftDataLogUpdated == TRUE );

        return  UT_Result( );
    }
}   /* test_BR_VIX_OSC_3_3_002a( ) */

int test_BR_VIX_OSC_3_3_002b( MYKI_BR_ContextData_t *pData )
{
    MYKI_CAControl_t               *pMYKI_CAControl             = NULL;
    MYKI_OAControl_t               *pMYKI_OAControl             = NULL;
    MYKI_OARoles_t                 *pMYKI_OARoles               = NULL;
    MYKI_OAShiftDataControl_t      *pMYKI_OAShiftDataControl    = NULL;
    MYKI_OAShiftDataRecordList_t   *pMYKI_OAShiftDataRecordList = NULL;
    RuleResult_e                    RuleResult                  = RULE_RESULT_ERROR;
    MYKI_BR_ShiftData_t             MYKI_BR_ShiftData           = { 0 };
    U8_t                            RoleType_Driver             = 1;
    U8_t                            RoleProfile_Driver          = 2;
    int                             ShiftId_High                = 2;
    int                             ShiftId_Low                 = 871;
    int                             ShiftId                     = ( ( ShiftId_High << 16 ) | ShiftId_Low );
    Time_t                          StartTime                   = 0;
    Time_t                          CloseTime                   = 0;

    if ( pData == NULL ||
         MYKI_CS_OpenCard( MYKI_CS_OPEN_OPERATOR_APP, NULL /*DONTCARE*/     ) != MYKI_CS_OK ||
         MYKI_CS_CAControlGet(              &pMYKI_CAControl                ) != MYKI_CS_OK ||
         MYKI_CS_OAControlGet(              &pMYKI_OAControl                ) != MYKI_CS_OK ||
         MYKI_CS_OARolesGet(                &pMYKI_OARoles                  ) != MYKI_CS_OK ||
         MYKI_CS_OAShiftDataControlGet(     &pMYKI_OAShiftDataControl       ) != MYKI_CS_OK ||
         MYKI_CS_OAShiftDataRecordsGet( 0,  &pMYKI_OAShiftDataRecordList    ) != MYKI_CS_OK )
    {
        return FALSE;
    }

    /*  GIVEN */
    {
        /*  Card image */
        {
            pMYKI_CAControl->Status                                             = CARD_CONTROL_STATUS_ACTIVATED;
            pMYKI_OAControl->Status                                             = OAPP_CONTROL_STATUS_ACTIVATED;
            pMYKI_OAControl->RolesCount                                         = 1;
            pMYKI_OARoles->Role[ 0 ].Type                                       = RoleType_Driver;
            pMYKI_OARoles->Role[ 0 ].Profile                                    = RoleProfile_Driver;
            pMYKI_OAShiftDataControl->Status                                    = OAPP_SHIFT_DATA_STATUS_ACTIVATED;     
            pMYKI_OAShiftDataControl->ShiftId                                   = ShiftId_Low;
            pMYKI_OAShiftDataControl->ShiftSeqNo                                = ShiftId_High;
            pMYKI_OAShiftDataControl->ActiveRecordCount                         = 0;
            pMYKI_OAShiftDataControl->StartTime                                 = \
                StartTime                                                       = pData->DynamicData.currentDateTime - HOURS_TO_SECONDS( 1 );
            pMYKI_OAShiftDataControl->CloseTime                                 = TIME_NOT_SET;

            /* No active records set */   
            pMYKI_OAShiftDataRecordList->NumberOfRecords                        = 0;
        }

        /*  BR context data */
        {
            memset( pData->StaticData.staffRoles, 0, sizeof( pData->StaticData.staffRoles ) );
            pData->StaticData.staffRoles[ 0 ].type                              = RoleType_Driver;
            pData->StaticData.staffRoles[ 0 ].profile                           = RoleProfile_Driver;
            pData->StaticData.staffRoles[ 0 ].operatorType                      = OperatorType_DRIVER;
            pData->DynamicData.pShiftData                                       = &MYKI_BR_ShiftData;       
            
            /* Shift record data */
            MYKI_BR_ShiftData.shiftId                                           = ShiftId;
            MYKI_BR_ShiftData.shiftStartTime                                    = StartTime;
            MYKI_BR_ShiftData.shiftEndTime                                      = \
                CloseTime                                                       = pData->DynamicData.currentDateTime;
            MYKI_BR_ShiftData.shiftTotalsTPurse.RecordType                      = PAYMENT_METHOD_TPURSE;
            MYKI_BR_ShiftData.shiftTotalsTPurse.SalesCount                      = 22;
            MYKI_BR_ShiftData.shiftTotalsTPurse.SalesValue                      = 1000;
            MYKI_BR_ShiftData.shiftTotalsTPurse.ReversalsCount                  = 11;
            MYKI_BR_ShiftData.shiftTotalsTPurse.ReversalsValue                  = 200;
            MYKI_BR_ShiftData.paperTicketReturns                                = 1;
            MYKI_BR_ShiftData.sundryItemReturns                                 = 5;
            MYKI_BR_ShiftData.cardReturns                                       = 6;
            MYKI_BR_ShiftData.shiftPortionStatus                                = SHIFT_PORTION_RESET;
        }
    }

    /*  WHEN */
    {
        /*  Executes business rule */
        RuleResult  = BR_VIX_OSC_3_3( pData );
    }

    /*  THEN */
    {
        UT_Start( );

        /*  Make sure business rule is executed */
        UT_Assert( RuleResult == RULE_RESULT_EXECUTED );

        /*  Make sure card image updated correctly */
        UT_Assert( pMYKI_OAShiftDataControl->Status == OAPP_SHIFT_DATA_STATUS_SUSPENDED );
        UT_Assert( pMYKI_OAShiftDataControl->ShiftId == ShiftId_Low );
        UT_Assert( pMYKI_OAShiftDataControl->ShiftSeqNo == ShiftId_High );
        UT_Assert( pMYKI_OAShiftDataControl->ActiveRecordCount == 1 );
        UT_Assert( pMYKI_OAShiftDataControl->StartTime == StartTime );
        UT_Assert( pMYKI_OAShiftDataControl->CloseTime == CloseTime );

        //UT_Assert( pMYKI_OAShiftDataRecordList->NumberOfRecords == 1 );
        UT_Assert( pMYKI_OAShiftDataRecordList->MYKI_ShiftDataRecord[0].RecordType        == pData->DynamicData.pShiftData->shiftTotalsTPurse.RecordType );
        UT_Assert( pMYKI_OAShiftDataRecordList->MYKI_ShiftDataRecord[0].SalesCount        == pData->DynamicData.pShiftData->shiftTotalsTPurse.SalesCount );
        UT_Assert( pMYKI_OAShiftDataRecordList->MYKI_ShiftDataRecord[0].SalesValue        == pData->DynamicData.pShiftData->shiftTotalsTPurse.SalesValue );
        UT_Assert( pMYKI_OAShiftDataRecordList->MYKI_ShiftDataRecord[0].ReversalsCount    == pData->DynamicData.pShiftData->shiftTotalsTPurse.ReversalsCount );
        UT_Assert( pMYKI_OAShiftDataRecordList->MYKI_ShiftDataRecord[0].ReversalsValue    == pData->DynamicData.pShiftData->shiftTotalsTPurse.ReversalsValue );

        /* Make sure log data from shift buffer is transferred */
        UT_Assert( pData->InternalData.ShiftDataSummaryLog.shiftId                 ==  ShiftId_Low );
        UT_Assert( pData->InternalData.ShiftDataSummaryLog.shiftSequenceNumber     ==  ShiftId_High );
        UT_Assert( pData->InternalData.ShiftDataSummaryLog.samId                   ==  pData->StaticData.deviceSamId );
        UT_Assert( pData->InternalData.ShiftDataSummaryLog.startTime               ==  StartTime );
        UT_Assert( pData->InternalData.ShiftDataSummaryLog.closeTime               ==  CloseTime );
        UT_Assert( pData->InternalData.ShiftDataSummaryLog.depositSequenceNumber   ==  0 );
        UT_Assert( pData->InternalData.ShiftDataSummaryLog.depositTotal            ==  ( MYKI_BR_ShiftData.shiftTotalsCash.SalesValue - 
                                                                                         MYKI_BR_ShiftData.shiftTotalsCash.ReversalsValue) );
        UT_Assert( pData->InternalData.ShiftDataSummaryLog.paperTicketReturns      ==  MYKI_BR_ShiftData.paperTicketReturns );
        UT_Assert( pData->InternalData.ShiftDataSummaryLog.sundryItemReturns       ==  MYKI_BR_ShiftData.sundryItemReturns );
        UT_Assert( pData->InternalData.ShiftDataSummaryLog.cardReturns             ==  MYKI_BR_ShiftData.cardReturns );
        
        UT_Assert( pData->InternalData.IsShiftDataLogUpdated == TRUE );

        return  UT_Result( );
    }
}   /* test_BR_VIX_OSC_3_3_002b( ) */

int test_BR_VIX_OSC_3_3_002c( MYKI_BR_ContextData_t *pData )
{
    MYKI_CAControl_t               *pMYKI_CAControl             = NULL;
    MYKI_OAControl_t               *pMYKI_OAControl             = NULL;
    MYKI_OARoles_t                 *pMYKI_OARoles               = NULL;
    MYKI_OAShiftDataControl_t      *pMYKI_OAShiftDataControl    = NULL;
    MYKI_OAShiftDataRecordList_t   *pMYKI_OAShiftDataRecordList = NULL;
    RuleResult_e                    RuleResult                  = RULE_RESULT_ERROR;
    MYKI_BR_ShiftData_t             MYKI_BR_ShiftData           = { 0 };
    U8_t                            RoleType_Driver             = 1;
    U8_t                            RoleProfile_Driver          = 2;
    int                             ShiftId_High                = 2;
    int                             ShiftId_Low                 = 871;
    int                             ShiftId                     = ( ( ShiftId_High << 16 ) | ShiftId_Low );
    Time_t                          StartTime                   = 0;
    Time_t                          CloseTime                   = 0;

    if ( pData == NULL ||
         MYKI_CS_OpenCard( MYKI_CS_OPEN_OPERATOR_APP, NULL /*DONTCARE*/     ) != MYKI_CS_OK ||
         MYKI_CS_CAControlGet(              &pMYKI_CAControl                ) != MYKI_CS_OK ||
         MYKI_CS_OAControlGet(              &pMYKI_OAControl                ) != MYKI_CS_OK ||
         MYKI_CS_OARolesGet(                &pMYKI_OARoles                  ) != MYKI_CS_OK ||
         MYKI_CS_OAShiftDataControlGet(     &pMYKI_OAShiftDataControl       ) != MYKI_CS_OK ||
         MYKI_CS_OAShiftDataRecordsGet( 0,  &pMYKI_OAShiftDataRecordList    ) != MYKI_CS_OK )
    {
        return FALSE;
    }

    /*  GIVEN */
    {
        /*  Card image */
        {
            pMYKI_CAControl->Status                                             = CARD_CONTROL_STATUS_ACTIVATED;
            pMYKI_OAControl->Status                                             = OAPP_CONTROL_STATUS_ACTIVATED;
            pMYKI_OAControl->RolesCount                                         = 1;
            pMYKI_OARoles->Role[ 0 ].Type                                       = RoleType_Driver;
            pMYKI_OARoles->Role[ 0 ].Profile                                    = RoleProfile_Driver;
            pMYKI_OAShiftDataControl->Status                                    = OAPP_SHIFT_DATA_STATUS_ACTIVATED;     
            pMYKI_OAShiftDataControl->ActiveRecordCount                         = 2;
            pMYKI_OAShiftDataControl->ShiftId                                   = ShiftId_Low;
            pMYKI_OAShiftDataControl->ShiftSeqNo                                = ShiftId_High;
            pMYKI_OAShiftDataControl->StartTime                                 = \
                StartTime                                                       = pData->DynamicData.currentDateTime - HOURS_TO_SECONDS( 1 );
            pMYKI_OAShiftDataControl->CloseTime                                 = TIME_NOT_SET;

            /* Shift record data on card */
            pMYKI_OAShiftDataRecordList->NumberOfRecords                        = 2;
            pMYKI_OAShiftDataRecordList->MYKI_ShiftDataRecord[0].RecordType     = PAYMENT_METHOD_CASH;
            pMYKI_OAShiftDataRecordList->MYKI_ShiftDataRecord[0].SalesCount     = 22;
            pMYKI_OAShiftDataRecordList->MYKI_ShiftDataRecord[0].SalesValue     = 1000;
            pMYKI_OAShiftDataRecordList->MYKI_ShiftDataRecord[0].ReversalsCount = 11;
            pMYKI_OAShiftDataRecordList->MYKI_ShiftDataRecord[0].ReversalsValue = 200;            
            pMYKI_OAShiftDataRecordList->MYKI_ShiftDataRecord[1].RecordType     = PAYMENT_METHOD_TPURSE;
            pMYKI_OAShiftDataRecordList->MYKI_ShiftDataRecord[1].SalesCount     = 22;
            pMYKI_OAShiftDataRecordList->MYKI_ShiftDataRecord[1].SalesValue     = 1000;
            pMYKI_OAShiftDataRecordList->MYKI_ShiftDataRecord[1].ReversalsCount = 11;
            pMYKI_OAShiftDataRecordList->MYKI_ShiftDataRecord[1].ReversalsValue = 200;            
        }

        /*  BR context data */
        {
            memset( pData->StaticData.staffRoles, 0, sizeof( pData->StaticData.staffRoles ) );
            pData->StaticData.staffRoles[ 0 ].type                              = RoleType_Driver;
            pData->StaticData.staffRoles[ 0 ].profile                           = RoleProfile_Driver;
            pData->StaticData.staffRoles[ 0 ].operatorType                      = OperatorType_DRIVER;
            pData->DynamicData.pShiftData                                       = &MYKI_BR_ShiftData;       

            /* Shift record data */
            MYKI_BR_ShiftData.shiftId                                           = ShiftId;
            MYKI_BR_ShiftData.shiftStartTime                                    = StartTime;
            MYKI_BR_ShiftData.shiftEndTime                                      = \
                CloseTime                                                       = pData->DynamicData.currentDateTime;
            MYKI_BR_ShiftData.shiftTotalsTPurse.RecordType                      = PAYMENT_METHOD_TPURSE;
            MYKI_BR_ShiftData.shiftTotalsTPurse.SalesCount                      = 1;
            MYKI_BR_ShiftData.shiftTotalsTPurse.SalesValue                      = 222;
            MYKI_BR_ShiftData.shiftTotalsTPurse.ReversalsCount                  = 5;
            MYKI_BR_ShiftData.shiftTotalsTPurse.ReversalsValue                  = 212;
            MYKI_BR_ShiftData.shiftTotalsCash.RecordType                        = PAYMENT_METHOD_CASH;
            MYKI_BR_ShiftData.shiftTotalsCash.SalesCount                        = 22;
            MYKI_BR_ShiftData.shiftTotalsCash.SalesValue                        = 1000;
            MYKI_BR_ShiftData.shiftTotalsCash.ReversalsCount                    = 11;
            MYKI_BR_ShiftData.shiftTotalsCash.ReversalsValue                    = 200;
            MYKI_BR_ShiftData.paperTicketReturns                                = 1;
            MYKI_BR_ShiftData.sundryItemReturns                                 = 5;
            MYKI_BR_ShiftData.cardReturns                                       = 6;
            MYKI_BR_ShiftData.shiftPortionStatus                                = SHIFT_PORTION_BROUGHT_FORWARD;
        }
    }

    /*  WHEN */
    {
        /*  Executes business rule */
        RuleResult  = BR_VIX_OSC_3_3( pData );
    }

    /*  THEN */
    {
        UT_Start( );

        /*  Make sure business rule is executed */
        UT_Assert( RuleResult == RULE_RESULT_EXECUTED );

        /*  Make sure shift data on card image loaded on record list is correct */
        UT_Assert( pMYKI_OAShiftDataControl->Status == OAPP_SHIFT_DATA_STATUS_SUSPENDED );
        UT_Assert( pMYKI_OAShiftDataControl->ShiftId == ShiftId_Low );
        UT_Assert( pMYKI_OAShiftDataControl->ShiftSeqNo == ShiftId_High );
        UT_Assert( pMYKI_OAShiftDataControl->StartTime == StartTime );
        UT_Assert( pMYKI_OAShiftDataControl->CloseTime == CloseTime );
        UT_Assert( pMYKI_OAShiftDataControl->ActiveRecordCount == 2 );

        UT_Assert( pMYKI_OAShiftDataRecordList->MYKI_ShiftDataRecord[0].RecordType        == MYKI_BR_ShiftData.shiftTotalsCash.RecordType );
        UT_Assert( pMYKI_OAShiftDataRecordList->MYKI_ShiftDataRecord[0].SalesCount        == MYKI_BR_ShiftData.shiftTotalsCash.SalesCount );
        UT_Assert( pMYKI_OAShiftDataRecordList->MYKI_ShiftDataRecord[0].SalesValue        == MYKI_BR_ShiftData.shiftTotalsCash.SalesValue );
        UT_Assert( pMYKI_OAShiftDataRecordList->MYKI_ShiftDataRecord[0].ReversalsCount    == MYKI_BR_ShiftData.shiftTotalsCash.ReversalsCount );
        UT_Assert( pMYKI_OAShiftDataRecordList->MYKI_ShiftDataRecord[0].ReversalsValue    == MYKI_BR_ShiftData.shiftTotalsCash.ReversalsValue );        

        UT_Assert( pMYKI_OAShiftDataRecordList->MYKI_ShiftDataRecord[1].RecordType        == MYKI_BR_ShiftData.shiftTotalsTPurse.RecordType );
        UT_Assert( pMYKI_OAShiftDataRecordList->MYKI_ShiftDataRecord[1].SalesCount        == MYKI_BR_ShiftData.shiftTotalsTPurse.SalesCount );
        UT_Assert( pMYKI_OAShiftDataRecordList->MYKI_ShiftDataRecord[1].SalesValue        == MYKI_BR_ShiftData.shiftTotalsTPurse.SalesValue );
        UT_Assert( pMYKI_OAShiftDataRecordList->MYKI_ShiftDataRecord[1].ReversalsCount    == MYKI_BR_ShiftData.shiftTotalsTPurse.ReversalsCount );
        UT_Assert( pMYKI_OAShiftDataRecordList->MYKI_ShiftDataRecord[1].ReversalsValue    == MYKI_BR_ShiftData.shiftTotalsTPurse.ReversalsValue );

        /* Make sure log data from shift buffer is transferred */
        UT_Assert( pData->InternalData.ShiftDataSummaryLog.shiftId                 ==  ShiftId_Low );
        UT_Assert( pData->InternalData.ShiftDataSummaryLog.shiftSequenceNumber     ==  ShiftId_High );
        UT_Assert( pData->InternalData.ShiftDataSummaryLog.samId                   ==  pData->StaticData.deviceSamId );
        UT_Assert( pData->InternalData.ShiftDataSummaryLog.startTime               ==  StartTime );
        UT_Assert( pData->InternalData.ShiftDataSummaryLog.closeTime               ==  CloseTime );
        UT_Assert( pData->InternalData.ShiftDataSummaryLog.depositSequenceNumber   ==  0 );
        UT_Assert( pData->InternalData.ShiftDataSummaryLog.depositTotal            ==  ( MYKI_BR_ShiftData.shiftTotalsCash.SalesValue - 
                                                                                         MYKI_BR_ShiftData.shiftTotalsCash.ReversalsValue) );
        UT_Assert( pData->InternalData.ShiftDataSummaryLog.paperTicketReturns      ==  MYKI_BR_ShiftData.paperTicketReturns );
        UT_Assert( pData->InternalData.ShiftDataSummaryLog.sundryItemReturns       ==  MYKI_BR_ShiftData.sundryItemReturns );
        UT_Assert( pData->InternalData.ShiftDataSummaryLog.cardReturns             ==  MYKI_BR_ShiftData.cardReturns );
        
        UT_Assert( pData->InternalData.IsShiftDataLogUpdated == TRUE );

        return  UT_Result( );
    }
}   /* test_BR_VIX_OSC_3_3_002c( ) */
