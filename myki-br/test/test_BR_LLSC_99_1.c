/* -*- mode: c; tabs: 4; -*- */
/*=============================================================================
**
**    Vix Technology                   Licensed software
**    (C) 2015                         All rights reserved
**
**=============================================================================
**
**  Project/Product : MBU
**  Filename        : test_BR_LLSC_99_1.c
**  Author(s)       : An Tran
**
**  Description     :
**      Implements BR_LLSC_99_1 business rule unit-tests.
**
**  Function(s)     :
**      test_BR_LLSC_99_1_XXX   [Public]    unit-test functions
**
**  Information     :
**   Compiler(s)    : ANSI C
**   Target(s)      : Independent
**
**  Subversion      :
**      $Id: test_BR_LLSC_99_1.c 73412 2015-09-14 21:30:17Z atran $
**      $HeadURL: https://auperasvn01.aupera.erggroup.com/svn/DPG_SWBase/myki-br/trunk/test/test_BR_LLSC_99_1.c $
**
**  History         :
**   Vers.  Date        Aut.  Type     Description
**   -----  ----------  ----  -------  ----------------------------------------
**    1.00  09.09.15    ANT   Create
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
**  test_BR_LLSC_99_1_001a
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

int test_BR_LLSC_99_1_001a( MYKI_BR_ContextData_t *pData )
{
    MYKI_CAControl_t       *pMYKI_CAControl         = NULL;
    MYKI_TAControl_t       *pMYKI_TAControl         = NULL;
    MYKI_TACapping_t       *pMYKI_TACapping         = NULL;
    MYKI_TAPurseBalance_t  *pMYKI_TAPurseBalance    = NULL;
    MYKI_TAPurseControl_t  *pMYKI_TAPurseControl    = NULL;
    MYKI_TAProduct_t       *pMYKI_TAProduct         = NULL;
    int                     DirIndex                = 2;
    int                     EntryPointId            = 53;
    int                     LineId                  = 66;
    int                     StopId                  = 79;
    int                     ServiceProviderId       = 100;
    int                     Balance                 = 1000;
    int                     NextTxSeqNo             = 5;
    Currency_t              MinTPurseBalance        = 100;
    Currency_t              MaxTPurseBalance        = 10000;
    RuleResult_e            RuleResult              = RULE_RESULT_ERROR;

    if ( pData == NULL ||
         MYKI_CS_CAControlGet(               &pMYKI_CAControl      ) != MYKI_CS_OK ||
         MYKI_CS_TAControlGet(               &pMYKI_TAControl      ) != MYKI_CS_OK ||
         MYKI_CS_TACappingGet(               &pMYKI_TACapping      ) != MYKI_CS_OK ||
         MYKI_CS_TAPurseBalanceGet(          &pMYKI_TAPurseBalance ) != MYKI_CS_OK ||
         MYKI_CS_TAPurseControlGet(          &pMYKI_TAPurseControl ) != MYKI_CS_OK ||
         MYKI_CS_TAProductGet( DirIndex - 1, &pMYKI_TAProduct      ) != MYKI_CS_OK )
    {
        return FALSE;
    }

    /*  GIVEN */
    {
        /*  Card image */
        {
            pMYKI_CAControl->Status                                         = CARD_CONTROL_STATUS_ACTIVATED;

            pMYKI_TAControl->Status                                         = TAPP_CONTROL_STATUS_ACTIVATED;
            pMYKI_TAControl->Directory[ 0 ].Status                          = TAPP_CONTROL_DIRECTORY_STATUS_ACTIVATED;

            pMYKI_TAPurseBalance->Balance                                   = Balance;
            pMYKI_TAPurseControl->NextTxSeqNo                               = NextTxSeqNo;
            pMYKI_TAPurseControl->LastChangeNewBalance                      = Balance;
            pMYKI_TAPurseControl->LastChangeTxSeqNo                         = NextTxSeqNo - 1;

            pMYKI_TAControl->Directory[ DirIndex ].Status                   = TAPP_CONTROL_DIRECTORY_STATUS_ACTIVATED;
            pMYKI_TAControl->Directory[ DirIndex ].ProductId                = PRODUCT_ID_EPASS;
            {
                pMYKI_TAProduct->EndDateTime                                = pData->DynamicData.currentDateTime + 10;
                pMYKI_TAProduct->StartDateTime                              = pMYKI_TAProduct->EndDateTime + 1; /* > EndDateTime */
            }

            pMYKI_TACapping->Daily.Expiry                                   = pData->DynamicData.currentBusinessDate + 1;
            pMYKI_TACapping->Weekly.Expiry                                  = pData->DynamicData.currentBusinessDate + 7 + 1;
        }

        /*  BR context data */
        {
            pData->DynamicData.entryPointId                                 = EntryPointId;
            pData->DynamicData.lineId                                       = LineId;
            pData->DynamicData.stopId                                       = StopId;
            pData->StaticData.serviceProviderId                             = ServiceProviderId;
            pData->Tariff.TPurseMinBalance                                  = MinTPurseBalance;
            pData->Tariff.TPurseMaximumBalance                              = MaxTPurseBalance;
        }
    }

    /*  WHEN */
    {
        /*  Executes business rule */
        RuleResult  = BR_LLSC_99_1( pData );
    }

    /*  THEN */
    {
        UT_Start( );

        /*  Make sure business rule is bypassed */
        UT_Assert( RuleResult == RULE_RESULT_BYPASSED );
        UT_Assert( pData->ReturnedData.bypassCode == BYPASS_CODE( 99, 1, 1, 1 ) );

        /*  Make sure reject code is correct */
        UT_Assert( pData->ReturnedData.rejectReason == MYKI_BR_REJECT_REASON_PRECOMMIT_CHECK_FAILED );

        return  UT_Result( );
    }
}   /*  test_BR_LLSC_99_1_001a( ) */

int test_BR_LLSC_99_1_001b( MYKI_BR_ContextData_t *pData )
{
    MYKI_CAControl_t       *pMYKI_CAControl         = NULL;
    MYKI_TAControl_t       *pMYKI_TAControl         = NULL;
    MYKI_TACapping_t       *pMYKI_TACapping         = NULL;
    MYKI_TAPurseBalance_t  *pMYKI_TAPurseBalance    = NULL;
    MYKI_TAPurseControl_t  *pMYKI_TAPurseControl    = NULL;
    MYKI_TAProduct_t       *pMYKI_TAProduct         = NULL;
    int                     DirIndex                = 2;
    int                     EntryPointId            = 53;
    int                     LineId                  = 66;
    int                     StopId                  = 79;
    int                     ServiceProviderId       = 100;
    int                     Balance                 = 1000;
    int                     NextTxSeqNo             = 5;
    Currency_t              MinTPurseBalance        = 100;
    Currency_t              MaxTPurseBalance        = 10000;
    RuleResult_e            RuleResult              = RULE_RESULT_ERROR;

    if ( pData == NULL ||
         MYKI_CS_CAControlGet(               &pMYKI_CAControl      ) != MYKI_CS_OK ||
         MYKI_CS_TAControlGet(               &pMYKI_TAControl      ) != MYKI_CS_OK ||
         MYKI_CS_TACappingGet(               &pMYKI_TACapping      ) != MYKI_CS_OK ||
         MYKI_CS_TAPurseBalanceGet(          &pMYKI_TAPurseBalance ) != MYKI_CS_OK ||
         MYKI_CS_TAPurseControlGet(          &pMYKI_TAPurseControl ) != MYKI_CS_OK ||
         MYKI_CS_TAProductGet( DirIndex - 1, &pMYKI_TAProduct      ) != MYKI_CS_OK )
    {
        return FALSE;
    }

    /*  GIVEN */
    {
        /*  Card image */
        {
            pMYKI_CAControl->Status                                         = CARD_CONTROL_STATUS_ACTIVATED;

            pMYKI_TAControl->Status                                         = TAPP_CONTROL_STATUS_ACTIVATED;
            pMYKI_TAControl->Directory[ 0 ].Status                          = TAPP_CONTROL_DIRECTORY_STATUS_ACTIVATED;
            pMYKI_TAPurseBalance->Balance                                   = Balance;
            pMYKI_TAPurseControl->NextTxSeqNo                               = NextTxSeqNo;
            pMYKI_TAPurseControl->LastChangeNewBalance                      = Balance;
            pMYKI_TAPurseControl->LastChangeTxSeqNo                         = NextTxSeqNo - 1;

            pMYKI_TAControl->Directory[ DirIndex ].Status                   = TAPP_CONTROL_DIRECTORY_STATUS_ACTIVATED;
            pMYKI_TAControl->Directory[ DirIndex ].ProductId                = PRODUCT_ID_DAILY;
            {
                pMYKI_TAProduct->StartDateTime                              = pData->DynamicData.currentDateTime - 10;
                pMYKI_TAProduct->EndDateTime                                = pMYKI_TAProduct->StartDateTime + HOURS_TO_SECONDS( 24 ) + 1;    /* > 24 HRS */
            }

            pMYKI_TACapping->Daily.Expiry                                   = pData->DynamicData.currentBusinessDate + 1;
            pMYKI_TACapping->Weekly.Expiry                                  = pData->DynamicData.currentBusinessDate + 7 + 1;
        }

        /*  BR context data */
        {
            pData->DynamicData.entryPointId                                 = EntryPointId;
            pData->DynamicData.lineId                                       = LineId;
            pData->DynamicData.stopId                                       = StopId;
            pData->StaticData.serviceProviderId                             = ServiceProviderId;
            pData->Tariff.TPurseMinBalance                                  = MinTPurseBalance;
            pData->Tariff.TPurseMaximumBalance                              = MaxTPurseBalance;
        }
    }

    /*  WHEN */
    {
        /*  Executes business rule */
        RuleResult  = BR_LLSC_99_1( pData );
    }

    /*  THEN */
    {
        UT_Start( );

        /*  Make sure business rule is bypassed */
        UT_Assert( RuleResult == RULE_RESULT_BYPASSED );
        UT_Assert( pData->ReturnedData.bypassCode == BYPASS_CODE( 99, 1, 1, 2 ) );

        /*  Make sure reject code is correct */
        UT_Assert( pData->ReturnedData.rejectReason == MYKI_BR_REJECT_REASON_PRECOMMIT_CHECK_FAILED );

        return  UT_Result( );
    }
}   /*  test_BR_LLSC_99_1_001b( ) */

int test_BR_LLSC_99_1_001c( MYKI_BR_ContextData_t *pData )
{
    MYKI_CAControl_t       *pMYKI_CAControl         = NULL;
    MYKI_TAControl_t       *pMYKI_TAControl         = NULL;
    MYKI_TACapping_t       *pMYKI_TACapping         = NULL;
    MYKI_TAPurseBalance_t  *pMYKI_TAPurseBalance    = NULL;
    MYKI_TAPurseControl_t  *pMYKI_TAPurseControl    = NULL;
    MYKI_TAProduct_t       *pMYKI_TAProduct         = NULL;
    int                     DirIndex                = 2;
    int                     EntryPointId            = 53;
    int                     LineId                  = 66;
    int                     StopId                  = 79;
    int                     ServiceProviderId       = 100;
    int                     Balance                 = 1000;
    int                     NextTxSeqNo             = 5;
    Currency_t              MinTPurseBalance        = 100;
    Currency_t              MaxTPurseBalance        = 10000;
    RuleResult_e            RuleResult              = RULE_RESULT_ERROR;

    if ( pData == NULL ||
         MYKI_CS_CAControlGet(               &pMYKI_CAControl      ) != MYKI_CS_OK ||
         MYKI_CS_TAControlGet(               &pMYKI_TAControl      ) != MYKI_CS_OK ||
         MYKI_CS_TACappingGet(               &pMYKI_TACapping      ) != MYKI_CS_OK ||
         MYKI_CS_TAPurseBalanceGet(          &pMYKI_TAPurseBalance ) != MYKI_CS_OK ||
         MYKI_CS_TAPurseControlGet(          &pMYKI_TAPurseControl ) != MYKI_CS_OK ||
         MYKI_CS_TAProductGet( DirIndex - 1, &pMYKI_TAProduct      ) != MYKI_CS_OK )
    {
        return FALSE;
    }

    /*  GIVEN */
    {
        /*  Card image */
        {
            pMYKI_CAControl->Status                                         = CARD_CONTROL_STATUS_ACTIVATED;
            pMYKI_TAControl->Status                                         = TAPP_CONTROL_STATUS_ACTIVATED;
            pMYKI_TAControl->Directory[ 0 ].Status                          = TAPP_CONTROL_DIRECTORY_STATUS_ACTIVATED;

            pMYKI_TAPurseBalance->Balance                                   = Balance;
            pMYKI_TAPurseControl->NextTxSeqNo                               = NextTxSeqNo;
            pMYKI_TAPurseControl->LastChangeNewBalance                      = Balance;
            pMYKI_TAPurseControl->LastChangeTxSeqNo                         = NextTxSeqNo - 1;

            pMYKI_TAControl->Directory[ DirIndex ].Status                   = TAPP_CONTROL_DIRECTORY_STATUS_ACTIVATED;
            pMYKI_TAControl->Directory[ DirIndex ].ProductId                = PRODUCT_ID_WEEKLY;
            {
                pMYKI_TAProduct->StartDateTime                              = pData->DynamicData.currentDateTime - 10;
                pMYKI_TAProduct->EndDateTime                                = pMYKI_TAProduct->StartDateTime + DAYS_TO_SECONDS( 8 ) + 1;    /* > 8 DAYS */
            }

            pMYKI_TACapping->Daily.Expiry                                   = pData->DynamicData.currentBusinessDate + 1;
            pMYKI_TACapping->Weekly.Expiry                                  = pData->DynamicData.currentBusinessDate + 7 + 1;
        }

        /*  BR context data */
        {
            pData->DynamicData.entryPointId                                 = EntryPointId;
            pData->DynamicData.lineId                                       = LineId;
            pData->DynamicData.stopId                                       = StopId;
            pData->StaticData.serviceProviderId                             = ServiceProviderId;
            pData->Tariff.TPurseMinBalance                                  = MinTPurseBalance;
            pData->Tariff.TPurseMaximumBalance                              = MaxTPurseBalance;
        }
    }

    /*  WHEN */
    {
        /*  Executes business rule */
        RuleResult  = BR_LLSC_99_1( pData );
    }

    /*  THEN */
    {
        UT_Start( );

        /*  Make sure business rule is bypassed */
        UT_Assert( RuleResult == RULE_RESULT_BYPASSED );
        UT_Assert( pData->ReturnedData.bypassCode == BYPASS_CODE( 99, 1, 1, 3 ) );

        /*  Make sure reject code is correct */
        UT_Assert( pData->ReturnedData.rejectReason == MYKI_BR_REJECT_REASON_PRECOMMIT_CHECK_FAILED );

        return  UT_Result( );
    }
}   /*  test_BR_LLSC_99_1_001c( ) */

int test_BR_LLSC_99_1_001d( MYKI_BR_ContextData_t *pData )
{
    MYKI_CAControl_t       *pMYKI_CAControl         = NULL;
    MYKI_TAControl_t       *pMYKI_TAControl         = NULL;
    MYKI_TACapping_t       *pMYKI_TACapping         = NULL;
    MYKI_TAPurseBalance_t  *pMYKI_TAPurseBalance    = NULL;
    MYKI_TAPurseControl_t  *pMYKI_TAPurseControl    = NULL;
    MYKI_TAProduct_t       *pMYKI_TAProduct         = NULL;
    int                     DirIndex                = 2;
    int                     EntryPointId            = 53;
    int                     LineId                  = 66;
    int                     StopId                  = 79;
    int                     ServiceProviderId       = 100;
    int                     Balance                 = 1000;
    int                     NextTxSeqNo             = 5;
    int                     InstanceCount           = 7;
    Currency_t              MinTPurseBalance        = 100;
    Currency_t              MaxTPurseBalance        = 10000;
    RuleResult_e            RuleResult              = RULE_RESULT_ERROR;

    if ( pData == NULL ||
         MYKI_CS_CAControlGet(               &pMYKI_CAControl      ) != MYKI_CS_OK ||
         MYKI_CS_TAControlGet(               &pMYKI_TAControl      ) != MYKI_CS_OK ||
         MYKI_CS_TACappingGet(               &pMYKI_TACapping      ) != MYKI_CS_OK ||
         MYKI_CS_TAPurseBalanceGet(          &pMYKI_TAPurseBalance ) != MYKI_CS_OK ||
         MYKI_CS_TAPurseControlGet(          &pMYKI_TAPurseControl ) != MYKI_CS_OK ||
         MYKI_CS_TAProductGet( DirIndex - 1, &pMYKI_TAProduct      ) != MYKI_CS_OK )
    {
        return FALSE;
    }

    /*  GIVEN */
    {
        /*  Card image */
        {
            pMYKI_CAControl->Status                                         = CARD_CONTROL_STATUS_ACTIVATED;

            pMYKI_TAControl->Status                                         = TAPP_CONTROL_STATUS_ACTIVATED;
            pMYKI_TAControl->Directory[ 0 ].Status                          = TAPP_CONTROL_DIRECTORY_STATUS_ACTIVATED;
            pMYKI_TAPurseBalance->Balance                                   = Balance;
            pMYKI_TAPurseControl->NextTxSeqNo                               = NextTxSeqNo;
            pMYKI_TAPurseControl->LastChangeNewBalance                      = Balance;
            pMYKI_TAPurseControl->LastChangeTxSeqNo                         = NextTxSeqNo - 1;

            pMYKI_TAControl->Directory[ DirIndex ].Status                   = TAPP_CONTROL_DIRECTORY_STATUS_ACTIVATED;
            pMYKI_TAControl->Directory[ DirIndex ].ProductId                = PRODUCT_ID_EPASS;
            {
                pMYKI_TAProduct->InstanceCount                              = InstanceCount;
                pMYKI_TAProduct->StartDateTime                              = pData->DynamicData.currentDateTime - 10;
                pMYKI_TAProduct->EndDateTime                                = pMYKI_TAProduct->StartDateTime + DAYS_TO_SECONDS( InstanceCount + 1 );  /*  != InstanceCount */
            }

            pMYKI_TACapping->Daily.Expiry                                   = pData->DynamicData.currentBusinessDate + 1;
            pMYKI_TACapping->Weekly.Expiry                                  = pData->DynamicData.currentBusinessDate + 7 + 1;
        }

        /*  BR context data */
        {
            pData->DynamicData.entryPointId                                 = EntryPointId;
            pData->DynamicData.lineId                                       = LineId;
            pData->DynamicData.stopId                                       = StopId;
            pData->StaticData.serviceProviderId                             = ServiceProviderId;
            pData->Tariff.TPurseMinBalance                                  = MinTPurseBalance;
            pData->Tariff.TPurseMaximumBalance                              = MaxTPurseBalance;
        }
    }

    /*  WHEN */
    {
        /*  Executes business rule */
        RuleResult  = BR_LLSC_99_1( pData );
    }

    /*  THEN */
    {
        UT_Start( );

        /*  Make sure business rule is bypassed */
        UT_Assert( RuleResult == RULE_RESULT_BYPASSED );
        UT_Assert( pData->ReturnedData.bypassCode == BYPASS_CODE( 99, 1, 1, 4 ) );

        /*  Make sure reject code is correct */
        UT_Assert( pData->ReturnedData.rejectReason == MYKI_BR_REJECT_REASON_PRECOMMIT_CHECK_FAILED );

        return  UT_Result( );
    }
}   /*  test_BR_LLSC_99_1_001d( ) */

int test_BR_LLSC_99_1_001e( MYKI_BR_ContextData_t *pData )
{
    MYKI_CAControl_t       *pMYKI_CAControl         = NULL;
    MYKI_TAControl_t       *pMYKI_TAControl         = NULL;
    MYKI_TACapping_t       *pMYKI_TACapping         = NULL;
    MYKI_TAPurseBalance_t  *pMYKI_TAPurseBalance    = NULL;
    MYKI_TAPurseControl_t  *pMYKI_TAPurseControl    = NULL;
    MYKI_TAProduct_t       *pMYKI_TAProduct         = NULL;
    int                     DirIndex                = 2;
    int                     EntryPointId            = 53;
    int                     LineId                  = 66;
    int                     StopId                  = 79;
    int                     ServiceProviderId       = 100;
    int                     Balance                 = 1000;
    int                     NextTxSeqNo             = 5;
    int                     InstanceCount           = 7;
    Currency_t              MinTPurseBalance        = 100;
    Currency_t              MaxTPurseBalance        = 10000;
    RuleResult_e            RuleResult              = RULE_RESULT_ERROR;

    if ( pData == NULL ||
         MYKI_CS_CAControlGet(               &pMYKI_CAControl      ) != MYKI_CS_OK ||
         MYKI_CS_TAControlGet(               &pMYKI_TAControl      ) != MYKI_CS_OK ||
         MYKI_CS_TACappingGet(               &pMYKI_TACapping      ) != MYKI_CS_OK ||
         MYKI_CS_TAPurseBalanceGet(          &pMYKI_TAPurseBalance ) != MYKI_CS_OK ||
         MYKI_CS_TAPurseControlGet(          &pMYKI_TAPurseControl ) != MYKI_CS_OK ||
         MYKI_CS_TAProductGet( DirIndex - 1, &pMYKI_TAProduct      ) != MYKI_CS_OK )
    {
        return FALSE;
    }

    /*  GIVEN */
    {
        /*  Card image */
        {
            pMYKI_CAControl->Status                                         = CARD_CONTROL_STATUS_ACTIVATED;

            pMYKI_TAControl->Status                                         = TAPP_CONTROL_STATUS_ACTIVATED;
            pMYKI_TAControl->Directory[ 0 ].Status                          = TAPP_CONTROL_DIRECTORY_STATUS_ACTIVATED;
            pMYKI_TAPurseBalance->Balance                                   = Balance;
            pMYKI_TAPurseControl->NextTxSeqNo                               = NextTxSeqNo;
            pMYKI_TAPurseControl->LastChangeNewBalance                      = Balance;
            pMYKI_TAPurseControl->LastChangeTxSeqNo                         = NextTxSeqNo - 1;

            pMYKI_TAControl->Directory[ DirIndex ].Status                   = TAPP_CONTROL_DIRECTORY_STATUS_ACTIVATED;
            pMYKI_TAControl->Directory[ DirIndex ].ProductId                = PRODUCT_ID_EPASS;
            {
                pMYKI_TAProduct->InstanceCount                              = InstanceCount;
                pMYKI_TAProduct->StartDateTime                              = pData->DynamicData.currentDateTime - 10;
                pMYKI_TAProduct->EndDateTime                                = pMYKI_TAProduct->StartDateTime + DAYS_TO_SECONDS( InstanceCount - 1 );  /*  != InstanceCount */
            }

            pMYKI_TACapping->Daily.Expiry                                   = pData->DynamicData.currentBusinessDate + 1;
            pMYKI_TACapping->Weekly.Expiry                                  = pData->DynamicData.currentBusinessDate + 7 + 1;
        }

        /*  BR context data */
        {
            pData->DynamicData.entryPointId                                 = EntryPointId;
            pData->DynamicData.lineId                                       = LineId;
            pData->DynamicData.stopId                                       = StopId;
            pData->StaticData.serviceProviderId                             = ServiceProviderId;
            pData->Tariff.TPurseMinBalance                                  = MinTPurseBalance;
            pData->Tariff.TPurseMaximumBalance                              = MaxTPurseBalance;
        }
    }

    /*  WHEN */
    {
        /*  Executes business rule */
        RuleResult  = BR_LLSC_99_1( pData );
    }

    /*  THEN */
    {
        UT_Start( );

        /*  Make sure business rule is bypassed */
        UT_Assert( RuleResult == RULE_RESULT_BYPASSED );
        UT_Assert( pData->ReturnedData.bypassCode == BYPASS_CODE( 99, 1, 1, 4 ) );

        /*  Make sure reject code is correct */
        UT_Assert( pData->ReturnedData.rejectReason == MYKI_BR_REJECT_REASON_PRECOMMIT_CHECK_FAILED );

        return  UT_Result( );
    }
}   /*  test_BR_LLSC_99_1_001e( ) */

int test_BR_LLSC_99_1_001f( MYKI_BR_ContextData_t *pData )
{
    MYKI_CAControl_t       *pMYKI_CAControl         = NULL;
    MYKI_TAControl_t       *pMYKI_TAControl         = NULL;
    MYKI_TACapping_t       *pMYKI_TACapping         = NULL;
    MYKI_TAPurseBalance_t  *pMYKI_TAPurseBalance    = NULL;
    MYKI_TAPurseControl_t  *pMYKI_TAPurseControl    = NULL;
    MYKI_TAProduct_t       *pMYKI_TAProduct         = NULL;
    int                     DirIndex                = 2;
    int                     EntryPointId            = 53;
    int                     LineId                  = 66;
    int                     StopId                  = 79;
    int                     ServiceProviderId       = 100;
    int                     Balance                 = 1000;
    int                     NextTxSeqNo             = 5;
    Currency_t              MinTPurseBalance        = 100;
    Currency_t              MaxTPurseBalance        = 10000;
    RuleResult_e            RuleResult              = RULE_RESULT_ERROR;

    if ( pData == NULL ||
         MYKI_CS_CAControlGet(               &pMYKI_CAControl      ) != MYKI_CS_OK ||
         MYKI_CS_TAControlGet(               &pMYKI_TAControl      ) != MYKI_CS_OK ||
         MYKI_CS_TACappingGet(               &pMYKI_TACapping      ) != MYKI_CS_OK ||
         MYKI_CS_TAPurseBalanceGet(          &pMYKI_TAPurseBalance ) != MYKI_CS_OK ||
         MYKI_CS_TAPurseControlGet(          &pMYKI_TAPurseControl ) != MYKI_CS_OK ||
         MYKI_CS_TAProductGet( DirIndex - 1, &pMYKI_TAProduct      ) != MYKI_CS_OK )
    {
        return FALSE;
    }

    /*  GIVEN */
    {
        /*  Card image */
        {
            pMYKI_CAControl->Status                                         = CARD_CONTROL_STATUS_ACTIVATED;

            pMYKI_TAControl->Status                                         = TAPP_CONTROL_STATUS_ACTIVATED;
            pMYKI_TAControl->Directory[ 0 ].Status                          = TAPP_CONTROL_DIRECTORY_STATUS_ACTIVATED;
            pMYKI_TAPurseBalance->Balance                                   = Balance;
            pMYKI_TAPurseControl->NextTxSeqNo                               = NextTxSeqNo;
            pMYKI_TAPurseControl->LastChangeNewBalance                      = Balance;
            pMYKI_TAPurseControl->LastChangeTxSeqNo                         = NextTxSeqNo - 1;

            pMYKI_TAControl->Directory[ DirIndex ].Status                   = TAPP_CONTROL_DIRECTORY_STATUS_ISSUED;
            pMYKI_TAControl->Directory[ DirIndex ].ProductId                = PRODUCT_ID_NHOUR; /*  != EPASS */

            pMYKI_TACapping->Daily.Expiry                                   = pData->DynamicData.currentBusinessDate + 1;
            pMYKI_TACapping->Weekly.Expiry                                  = pData->DynamicData.currentBusinessDate + 7 + 1;
        }

        /*  BR context data */
        {
            pData->DynamicData.entryPointId                                 = EntryPointId;
            pData->DynamicData.lineId                                       = LineId;
            pData->DynamicData.stopId                                       = StopId;
            pData->StaticData.serviceProviderId                             = ServiceProviderId;
            pData->Tariff.TPurseMinBalance                                  = MinTPurseBalance;
            pData->Tariff.TPurseMaximumBalance                              = MaxTPurseBalance;
        }
    }

    /*  WHEN */
    {
        /*  Executes business rule */
        RuleResult  = BR_LLSC_99_1( pData );
    }

    /*  THEN */
    {
        UT_Start( );

        /*  Make sure business rule is bypassed */
        UT_Assert( RuleResult == RULE_RESULT_BYPASSED );
        UT_Assert( pData->ReturnedData.bypassCode == BYPASS_CODE( 99, 1, 1, 5 ) );

        /*  Make sure reject code is correct */
        UT_Assert( pData->ReturnedData.rejectReason == MYKI_BR_REJECT_REASON_PRECOMMIT_CHECK_FAILED );

        return  UT_Result( );
    }
}   /*  test_BR_LLSC_99_1_001f( ) */

int test_BR_LLSC_99_1_001g( MYKI_BR_ContextData_t *pData )
{
    MYKI_CAControl_t       *pMYKI_CAControl         = NULL;
    MYKI_TAControl_t       *pMYKI_TAControl         = NULL;
    MYKI_TACapping_t       *pMYKI_TACapping         = NULL;
    MYKI_TAPurseBalance_t  *pMYKI_TAPurseBalance    = NULL;
    MYKI_TAPurseControl_t  *pMYKI_TAPurseControl    = NULL;
    MYKI_TAProduct_t       *pMYKI_TAProduct         = NULL;
    int                     DirIndex                = 2;
    int                     EntryPointId            = 53;
    int                     LineId                  = 66;
    int                     StopId                  = 79;
    int                     ServiceProviderId       = 100;
    int                     Balance                 = 1000;
    int                     NextTxSeqNo             = 5;
    Currency_t              MinTPurseBalance        = 100;
    Currency_t              MaxTPurseBalance        = 10000;
    RuleResult_e            RuleResult              = RULE_RESULT_ERROR;

    if ( pData == NULL ||
         MYKI_CS_CAControlGet(               &pMYKI_CAControl      ) != MYKI_CS_OK ||
         MYKI_CS_TAControlGet(               &pMYKI_TAControl      ) != MYKI_CS_OK ||
         MYKI_CS_TACappingGet(               &pMYKI_TACapping      ) != MYKI_CS_OK ||
         MYKI_CS_TAPurseBalanceGet(          &pMYKI_TAPurseBalance ) != MYKI_CS_OK ||
         MYKI_CS_TAPurseControlGet(          &pMYKI_TAPurseControl ) != MYKI_CS_OK ||
         MYKI_CS_TAProductGet( DirIndex - 1, &pMYKI_TAProduct      ) != MYKI_CS_OK )
    {
        return FALSE;
    }

    /*  GIVEN */
    {
        /*  Card image */
        {
            pMYKI_CAControl->Status                                         = CARD_CONTROL_STATUS_ACTIVATED;

            pMYKI_TAControl->Status                                         = TAPP_CONTROL_STATUS_ACTIVATED;
            pMYKI_TAControl->Directory[ 0 ].Status                          = TAPP_CONTROL_DIRECTORY_STATUS_ACTIVATED;
            pMYKI_TAPurseBalance->Balance                                   = Balance;
            pMYKI_TAPurseControl->NextTxSeqNo                               = NextTxSeqNo;
            pMYKI_TAPurseControl->LastChangeNewBalance                      = Balance;
            pMYKI_TAPurseControl->LastChangeTxSeqNo                         = NextTxSeqNo - 1;

            pMYKI_TAControl->Directory[ DirIndex ].Status                   = TAPP_CONTROL_DIRECTORY_STATUS_ACTIVATED;
            pMYKI_TAControl->Directory[ DirIndex ].ProductId                = PRODUCT_ID_EPASS;     /*  != NHOUR */
            {
                pMYKI_TAProduct->ControlBitmap                             |= TAPP_TPRODUCT_CONTROL_PROVISIONAL_BITMAP;
                pMYKI_TAProduct->InstanceCount                              = 0;
                pMYKI_TAProduct->StartDateTime                              = pData->DynamicData.currentDateTime - 10;
                pMYKI_TAProduct->EndDateTime                                = pMYKI_TAProduct->StartDateTime + DAYS_TO_SECONDS( 1 );
            }

            pMYKI_TACapping->Daily.Expiry                                   = pData->DynamicData.currentBusinessDate + 1;
            pMYKI_TACapping->Weekly.Expiry                                  = pData->DynamicData.currentBusinessDate + 7 + 1;
        }

        /*  BR context data */
        {
            pData->DynamicData.entryPointId                                 = EntryPointId;
            pData->DynamicData.lineId                                       = LineId;
            pData->DynamicData.stopId                                       = StopId;
            pData->StaticData.serviceProviderId                             = ServiceProviderId;
            pData->Tariff.TPurseMinBalance                                  = MinTPurseBalance;
            pData->Tariff.TPurseMaximumBalance                              = MaxTPurseBalance;
        }
    }

    /*  WHEN */
    {
        /*  Executes business rule */
        RuleResult  = BR_LLSC_99_1( pData );
    }

    /*  THEN */
    {
        UT_Start( );

        /*  Make sure business rule is bypassed */
        UT_Assert( RuleResult == RULE_RESULT_BYPASSED );
        UT_Assert( pData->ReturnedData.bypassCode == BYPASS_CODE( 99, 1, 1, 6 ) );

        /*  Make sure reject code is correct */
        UT_Assert( pData->ReturnedData.rejectReason == MYKI_BR_REJECT_REASON_PRECOMMIT_CHECK_FAILED );

        return  UT_Result( );
    }
}   /*  test_BR_LLSC_99_1_001g( ) */

int test_BR_LLSC_99_1_001h( MYKI_BR_ContextData_t *pData )
{
    MYKI_CAControl_t       *pMYKI_CAControl         = NULL;
    MYKI_TAControl_t       *pMYKI_TAControl         = NULL;
    MYKI_TACapping_t       *pMYKI_TACapping         = NULL;
    MYKI_TAPurseBalance_t  *pMYKI_TAPurseBalance    = NULL;
    MYKI_TAPurseControl_t  *pMYKI_TAPurseControl    = NULL;
    MYKI_TAProduct_t       *pMYKI_TAProduct         = NULL;
    int                     ProductInUse            = 1;
    int                     DirIndex                = 2;
    int                     EntryPointId            = 53;
    int                     LineId                  = 66;
    int                     StopId                  = 79;
    int                     ServiceProviderId       = 100;
    int                     Balance                 = 1000;
    int                     NextTxSeqNo             = 5;
    Currency_t              MinTPurseBalance        = 100;
    Currency_t              MaxTPurseBalance        = 10000;
    RuleResult_e            RuleResult              = RULE_RESULT_ERROR;

    if ( pData == NULL ||
         MYKI_CS_CAControlGet(               &pMYKI_CAControl      ) != MYKI_CS_OK ||
         MYKI_CS_TAControlGet(               &pMYKI_TAControl      ) != MYKI_CS_OK ||
         MYKI_CS_TACappingGet(               &pMYKI_TACapping      ) != MYKI_CS_OK ||
         MYKI_CS_TAPurseBalanceGet(          &pMYKI_TAPurseBalance ) != MYKI_CS_OK ||
         MYKI_CS_TAPurseControlGet(          &pMYKI_TAPurseControl ) != MYKI_CS_OK ||
         MYKI_CS_TAProductGet( DirIndex - 1, &pMYKI_TAProduct      ) != MYKI_CS_OK )
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
            pMYKI_TAPurseBalance->Balance                                   = Balance;
            pMYKI_TAPurseControl->NextTxSeqNo                               = NextTxSeqNo;
            pMYKI_TAPurseControl->LastChangeNewBalance                      = Balance;
            pMYKI_TAPurseControl->LastChangeTxSeqNo                         = NextTxSeqNo - 1;

            DirIndex                                                        = ProductInUse + 1;     /*  != Product in use */
            pMYKI_TAControl->Directory[ DirIndex ].Status                   = TAPP_CONTROL_DIRECTORY_STATUS_ACTIVATED;
            pMYKI_TAControl->Directory[ DirIndex ].ProductId                = PRODUCT_ID_NHOUR;
            {
                pMYKI_TAProduct->ControlBitmap                             |= TAPP_TPRODUCT_CONTROL_PROVISIONAL_BITMAP;
                pMYKI_TAProduct->InstanceCount                              = 0;
                pMYKI_TAProduct->StartDateTime                              = pData->DynamicData.currentDateTime - 10;
                pMYKI_TAProduct->EndDateTime                                = pMYKI_TAProduct->StartDateTime + HOURS_TO_SECONDS( 2 );
            }

            pMYKI_TACapping->Daily.Expiry                                   = pData->DynamicData.currentBusinessDate + 1;
            pMYKI_TACapping->Weekly.Expiry                                  = pData->DynamicData.currentBusinessDate + 7 + 1;
        }

        /*  BR context data */
        {
            pData->DynamicData.entryPointId                                 = EntryPointId;
            pData->DynamicData.lineId                                       = LineId;
            pData->DynamicData.stopId                                       = StopId;
            pData->StaticData.serviceProviderId                             = ServiceProviderId;
            pData->Tariff.TPurseMinBalance                                  = MinTPurseBalance;
            pData->Tariff.TPurseMaximumBalance                              = MaxTPurseBalance;
        }
    }

    /*  WHEN */
    {
        /*  Executes business rule */
        RuleResult  = BR_LLSC_99_1( pData );
    }

    /*  THEN */
    {
        UT_Start( );

        /*  Make sure business rule is bypassed */
        UT_Assert( RuleResult == RULE_RESULT_BYPASSED );
        UT_Assert( pData->ReturnedData.bypassCode == BYPASS_CODE( 99, 1, 1, 7 ) );

        /*  Make sure reject code is correct */
        UT_Assert( pData->ReturnedData.rejectReason == MYKI_BR_REJECT_REASON_PRECOMMIT_CHECK_FAILED );

        return  UT_Result( );
    }
}   /*  test_BR_LLSC_99_1_001h( ) */

int test_BR_LLSC_99_1_001i( MYKI_BR_ContextData_t *pData )
{
    MYKI_CAControl_t       *pMYKI_CAControl         = NULL;
    MYKI_TAControl_t       *pMYKI_TAControl         = NULL;
    MYKI_TACapping_t       *pMYKI_TACapping         = NULL;
    MYKI_TAPurseBalance_t  *pMYKI_TAPurseBalance    = NULL;
    MYKI_TAPurseControl_t  *pMYKI_TAPurseControl    = NULL;
    MYKI_TAProduct_t       *pMYKI_TAProduct         = NULL;
    int                     DirIndex                = 2;
    int                     EntryPointId            = 53;
    int                     LineId                  = 66;
    int                     StopId                  = 79;
    int                     ServiceProviderId       = 100;
    int                     Balance                 = 1000;
    int                     NextTxSeqNo             = 5;
    Currency_t              MinTPurseBalance        = 100;
    Currency_t              MaxTPurseBalance        = 10000;
    RuleResult_e            RuleResult              = RULE_RESULT_ERROR;

    if ( pData == NULL ||
         MYKI_CS_CAControlGet(               &pMYKI_CAControl      ) != MYKI_CS_OK ||
         MYKI_CS_TAControlGet(               &pMYKI_TAControl      ) != MYKI_CS_OK ||
         MYKI_CS_TACappingGet(               &pMYKI_TACapping      ) != MYKI_CS_OK ||
         MYKI_CS_TAPurseBalanceGet(          &pMYKI_TAPurseBalance ) != MYKI_CS_OK ||
         MYKI_CS_TAPurseControlGet(          &pMYKI_TAPurseControl ) != MYKI_CS_OK ||
         MYKI_CS_TAProductGet( DirIndex - 1, &pMYKI_TAProduct      ) != MYKI_CS_OK )
    {
        return FALSE;
    }

    /*  GIVEN */
    {
        /*  Card image */
        {
            pMYKI_CAControl->Status                                         = CARD_CONTROL_STATUS_ACTIVATED;

            pMYKI_TAControl->Status                                         = TAPP_CONTROL_STATUS_ACTIVATED;
            pMYKI_TAControl->Directory[ 0 ].Status                          = TAPP_CONTROL_DIRECTORY_STATUS_ACTIVATED;
            pMYKI_TAPurseBalance->Balance                                   = Balance;
            pMYKI_TAPurseControl->NextTxSeqNo                               = NextTxSeqNo;
            pMYKI_TAPurseControl->LastChangeNewBalance                      = Balance;
            pMYKI_TAPurseControl->LastChangeTxSeqNo                         = NextTxSeqNo - 1;

            pMYKI_TAControl->Directory[ DirIndex ].Status                   = TAPP_CONTROL_DIRECTORY_STATUS_ACTIVATED;
            pMYKI_TAControl->Directory[ DirIndex ].ProductId                = PRODUCT_ID_NHOUR;
            {
                pMYKI_TAProduct->InstanceCount                              = 0;
                pMYKI_TAProduct->StartDateTime                              = pData->DynamicData.currentDateTime - 10;
                pMYKI_TAProduct->EndDateTime                                = pMYKI_TAProduct->StartDateTime + HOURS_TO_SECONDS( 2 );
            }

            pMYKI_TACapping->Daily.Expiry                                   = pData->DynamicData.currentBusinessDate + 1 + 1;   /*  INCORRECT! */
            pMYKI_TACapping->Weekly.Expiry                                  = pData->DynamicData.currentBusinessDate + 7 + 1;
        }

        /*  BR context data */
        {
            pData->DynamicData.entryPointId                                 = EntryPointId;
            pData->DynamicData.lineId                                       = LineId;
            pData->DynamicData.stopId                                       = StopId;
            pData->StaticData.serviceProviderId                             = ServiceProviderId;
            pData->Tariff.TPurseMinBalance                                  = MinTPurseBalance;
            pData->Tariff.TPurseMaximumBalance                              = MaxTPurseBalance;
        }
    }

    /*  WHEN */
    {
        /*  Executes business rule */
        RuleResult  = BR_LLSC_99_1( pData );
    }

    /*  THEN */
    {
        UT_Start( );

        /*  Make sure business rule is bypassed */
        UT_Assert( RuleResult == RULE_RESULT_BYPASSED );
        UT_Assert( pData->ReturnedData.bypassCode == BYPASS_CODE( 99, 1, 2, 0 ) );

        /*  Make sure reject code is correct */
        UT_Assert( pData->ReturnedData.rejectReason == MYKI_BR_REJECT_REASON_PRECOMMIT_CHECK_FAILED );

        return  UT_Result( );
    }
}   /*  test_BR_LLSC_99_1_001i( ) */

int test_BR_LLSC_99_1_001j( MYKI_BR_ContextData_t *pData )
{
    MYKI_CAControl_t       *pMYKI_CAControl         = NULL;
    MYKI_TAControl_t       *pMYKI_TAControl         = NULL;
    MYKI_TACapping_t       *pMYKI_TACapping         = NULL;
    MYKI_TAPurseBalance_t  *pMYKI_TAPurseBalance    = NULL;
    MYKI_TAPurseControl_t  *pMYKI_TAPurseControl    = NULL;
    MYKI_TAProduct_t       *pMYKI_TAProduct         = NULL;
    int                     DirIndex                = 2;
    int                     EntryPointId            = 53;
    int                     LineId                  = 66;
    int                     StopId                  = 79;
    int                     ServiceProviderId       = 100;
    int                     Balance                 = 1000;
    int                     NextTxSeqNo             = 5;
    Currency_t              MinTPurseBalance        = 100;
    Currency_t              MaxTPurseBalance        = 10000;
    RuleResult_e            RuleResult              = RULE_RESULT_ERROR;

    if ( pData == NULL ||
         MYKI_CS_CAControlGet(               &pMYKI_CAControl      ) != MYKI_CS_OK ||
         MYKI_CS_TAControlGet(               &pMYKI_TAControl      ) != MYKI_CS_OK ||
         MYKI_CS_TACappingGet(               &pMYKI_TACapping      ) != MYKI_CS_OK ||
         MYKI_CS_TAPurseBalanceGet(          &pMYKI_TAPurseBalance ) != MYKI_CS_OK ||
         MYKI_CS_TAPurseControlGet(          &pMYKI_TAPurseControl ) != MYKI_CS_OK ||
         MYKI_CS_TAProductGet( DirIndex - 1, &pMYKI_TAProduct      ) != MYKI_CS_OK )
    {
        return FALSE;
    }

    /*  GIVEN */
    {
        /*  Card image */
        {
            pMYKI_CAControl->Status                                         = CARD_CONTROL_STATUS_ACTIVATED;

            pMYKI_TAControl->Status                                         = TAPP_CONTROL_STATUS_ACTIVATED;
            pMYKI_TAControl->Directory[ 0 ].Status                          = TAPP_CONTROL_DIRECTORY_STATUS_ACTIVATED;
            pMYKI_TAPurseBalance->Balance                                   = Balance;
            pMYKI_TAPurseControl->NextTxSeqNo                               = NextTxSeqNo;
            pMYKI_TAPurseControl->LastChangeNewBalance                      = Balance;
            pMYKI_TAPurseControl->LastChangeTxSeqNo                         = NextTxSeqNo - 1;

            pMYKI_TAControl->Directory[ DirIndex ].Status                   = TAPP_CONTROL_DIRECTORY_STATUS_ACTIVATED;
            pMYKI_TAControl->Directory[ DirIndex ].ProductId                = PRODUCT_ID_NHOUR;
            {
                pMYKI_TAProduct->InstanceCount                              = 0;
                pMYKI_TAProduct->StartDateTime                              = pData->DynamicData.currentDateTime - 10;
                pMYKI_TAProduct->EndDateTime                                = pMYKI_TAProduct->StartDateTime + HOURS_TO_SECONDS( 2 );
            }

            pMYKI_TACapping->Daily.Expiry                                   = pData->DynamicData.currentBusinessDate + 1;
            pMYKI_TACapping->Weekly.Expiry                                  = pData->DynamicData.currentBusinessDate + 7 + 1 + 1;   /*  INCORRECT! */
        }

        /*  BR context data */
        {
            pData->DynamicData.entryPointId                                 = EntryPointId;
            pData->DynamicData.lineId                                       = LineId;
            pData->DynamicData.stopId                                       = StopId;
            pData->StaticData.serviceProviderId                             = ServiceProviderId;
            pData->Tariff.TPurseMinBalance                                  = MinTPurseBalance;
            pData->Tariff.TPurseMaximumBalance                              = MaxTPurseBalance;
        }
    }

    /*  WHEN */
    {
        /*  Executes business rule */
        RuleResult  = BR_LLSC_99_1( pData );
    }

    /*  THEN */
    {
        UT_Start( );

        /*  Make sure business rule is bypassed */
        UT_Assert( RuleResult == RULE_RESULT_BYPASSED );
        UT_Assert( pData->ReturnedData.bypassCode == BYPASS_CODE( 99, 1, 3, 0 ) );

        /*  Make sure reject code is correct */
        UT_Assert( pData->ReturnedData.rejectReason == MYKI_BR_REJECT_REASON_PRECOMMIT_CHECK_FAILED );

        return  UT_Result( );
    }
}   /*  test_BR_LLSC_99_1_001j( ) */

int test_BR_LLSC_99_1_001k( MYKI_BR_ContextData_t *pData )
{
    MYKI_CAControl_t       *pMYKI_CAControl         = NULL;
    MYKI_TAControl_t       *pMYKI_TAControl         = NULL;
    MYKI_TACapping_t       *pMYKI_TACapping         = NULL;
    MYKI_TAPurseBalance_t  *pMYKI_TAPurseBalance    = NULL;
    MYKI_TAPurseControl_t  *pMYKI_TAPurseControl    = NULL;
    MYKI_TAProduct_t       *pMYKI_TAProduct         = NULL;
    int                     DirIndex                = 2;
    int                     EntryPointId            = 53;
    int                     LineId                  = 66;
    int                     StopId                  = 79;
    int                     ServiceProviderId       = 100;
    int                     Balance                 = 1000;
    int                     NextTxSeqNo             = 5;
    Currency_t              MinTPurseBalance        = 100;
    Currency_t              MaxTPurseBalance        = 10000;
    RuleResult_e            RuleResult              = RULE_RESULT_ERROR;

    if ( pData == NULL ||
         MYKI_CS_CAControlGet(               &pMYKI_CAControl      ) != MYKI_CS_OK ||
         MYKI_CS_TAControlGet(               &pMYKI_TAControl      ) != MYKI_CS_OK ||
         MYKI_CS_TACappingGet(               &pMYKI_TACapping      ) != MYKI_CS_OK ||
         MYKI_CS_TAPurseBalanceGet(          &pMYKI_TAPurseBalance ) != MYKI_CS_OK ||
         MYKI_CS_TAPurseControlGet(          &pMYKI_TAPurseControl ) != MYKI_CS_OK ||
         MYKI_CS_TAProductGet( DirIndex - 1, &pMYKI_TAProduct      ) != MYKI_CS_OK )
    {
        return FALSE;
    }

    /*  GIVEN */
    {
        /*  Card image */
        {
            pMYKI_CAControl->Status                                         = CARD_CONTROL_STATUS_ACTIVATED;

            pMYKI_TAControl->Status                                         = TAPP_CONTROL_STATUS_ACTIVATED;
            pMYKI_TAControl->Directory[ 0 ].Status                          = TAPP_CONTROL_DIRECTORY_STATUS_ACTIVATED;
            Balance                                                         = \
            pMYKI_TAPurseBalance->Balance                                   = - ( MaxTPurseBalance * 2 ) - 1;   /*  INCORRECT! */
            pMYKI_TAPurseControl->NextTxSeqNo                               = NextTxSeqNo;
            pMYKI_TAPurseControl->LastChangeNewBalance                      = Balance;
            pMYKI_TAPurseControl->LastChangeTxSeqNo                         = NextTxSeqNo - 1;

            pMYKI_TAControl->Directory[ DirIndex ].Status                   = TAPP_CONTROL_DIRECTORY_STATUS_ACTIVATED;
            pMYKI_TAControl->Directory[ DirIndex ].ProductId                = PRODUCT_ID_NHOUR;
            {
                pMYKI_TAProduct->InstanceCount                              = 0;
                pMYKI_TAProduct->StartDateTime                              = pData->DynamicData.currentDateTime - 10;
                pMYKI_TAProduct->EndDateTime                                = pMYKI_TAProduct->StartDateTime + HOURS_TO_SECONDS( 2 );
            }

            pMYKI_TACapping->Daily.Expiry                                   = pData->DynamicData.currentBusinessDate + 1;
            pMYKI_TACapping->Weekly.Expiry                                  = pData->DynamicData.currentBusinessDate + 7 + 1;
        }

        /*  BR context data */
        {
            pData->DynamicData.entryPointId                                 = EntryPointId;
            pData->DynamicData.lineId                                       = LineId;
            pData->DynamicData.stopId                                       = StopId;
            pData->StaticData.serviceProviderId                             = ServiceProviderId;
            pData->Tariff.TPurseMinBalance                                  = MinTPurseBalance;
            pData->Tariff.TPurseMaximumBalance                              = MaxTPurseBalance;
        }
    }

    /*  WHEN */
    {
        /*  Executes business rule */
        RuleResult  = BR_LLSC_99_1( pData );
    }

    /*  THEN */
    {
        UT_Start( );

        /*  Make sure business rule is bypassed */
        UT_Assert( RuleResult == RULE_RESULT_BYPASSED );
        UT_Assert( pData->ReturnedData.bypassCode == BYPASS_CODE( 99, 1, 4, 1 ) );

        /*  Make sure reject code is correct */
        UT_Assert( pData->ReturnedData.rejectReason == MYKI_BR_REJECT_REASON_PRECOMMIT_CHECK_FAILED );

        return  UT_Result( );
    }
}   /*  test_BR_LLSC_99_1_001k( ) */

int test_BR_LLSC_99_1_001l( MYKI_BR_ContextData_t *pData )
{
    MYKI_CAControl_t       *pMYKI_CAControl         = NULL;
    MYKI_TAControl_t       *pMYKI_TAControl         = NULL;
    MYKI_TACapping_t       *pMYKI_TACapping         = NULL;
    MYKI_TAPurseBalance_t  *pMYKI_TAPurseBalance    = NULL;
    MYKI_TAPurseControl_t  *pMYKI_TAPurseControl    = NULL;
    MYKI_TAProduct_t       *pMYKI_TAProduct         = NULL;
    int                     DirIndex                = 2;
    int                     EntryPointId            = 53;
    int                     LineId                  = 66;
    int                     StopId                  = 79;
    int                     ServiceProviderId       = 100;
    int                     Balance                 = 1000;
    int                     NextTxSeqNo             = 5;
    Currency_t              MinTPurseBalance        = 100;
    Currency_t              MaxTPurseBalance        = 10000;
    RuleResult_e            RuleResult              = RULE_RESULT_ERROR;

    if ( pData == NULL ||
         MYKI_CS_CAControlGet(               &pMYKI_CAControl      ) != MYKI_CS_OK ||
         MYKI_CS_TAControlGet(               &pMYKI_TAControl      ) != MYKI_CS_OK ||
         MYKI_CS_TACappingGet(               &pMYKI_TACapping      ) != MYKI_CS_OK ||
         MYKI_CS_TAPurseBalanceGet(          &pMYKI_TAPurseBalance ) != MYKI_CS_OK ||
         MYKI_CS_TAPurseControlGet(          &pMYKI_TAPurseControl ) != MYKI_CS_OK ||
         MYKI_CS_TAProductGet( DirIndex - 1, &pMYKI_TAProduct      ) != MYKI_CS_OK )
    {
        return FALSE;
    }

    /*  GIVEN */
    {
        /*  Card image */
        {
            pMYKI_CAControl->Status                                         = CARD_CONTROL_STATUS_ACTIVATED;

            pMYKI_TAControl->Status                                         = TAPP_CONTROL_STATUS_ACTIVATED;
            pMYKI_TAControl->Directory[ 0 ].Status                          = TAPP_CONTROL_DIRECTORY_STATUS_ACTIVATED;
            Balance                                                         = \
            pMYKI_TAPurseBalance->Balance                                   = ( MaxTPurseBalance * 2 ) + 1;     /*  INCORRECT! */
            pMYKI_TAPurseControl->NextTxSeqNo                               = NextTxSeqNo;
            pMYKI_TAPurseControl->LastChangeNewBalance                      = Balance;
            pMYKI_TAPurseControl->LastChangeTxSeqNo                         = NextTxSeqNo - 1;

            pMYKI_TAControl->Directory[ DirIndex ].Status                   = TAPP_CONTROL_DIRECTORY_STATUS_ACTIVATED;
            pMYKI_TAControl->Directory[ DirIndex ].ProductId                = PRODUCT_ID_NHOUR;
            {
                pMYKI_TAProduct->InstanceCount                              = 0;
                pMYKI_TAProduct->StartDateTime                              = pData->DynamicData.currentDateTime - 10;
                pMYKI_TAProduct->EndDateTime                                = pMYKI_TAProduct->StartDateTime + HOURS_TO_SECONDS( 2 );
            }

            pMYKI_TACapping->Daily.Expiry                                   = pData->DynamicData.currentBusinessDate + 1;
            pMYKI_TACapping->Weekly.Expiry                                  = pData->DynamicData.currentBusinessDate + 7 + 1;
        }

        /*  BR context data */
        {
            pData->DynamicData.entryPointId                                 = EntryPointId;
            pData->DynamicData.lineId                                       = LineId;
            pData->DynamicData.stopId                                       = StopId;
            pData->StaticData.serviceProviderId                             = ServiceProviderId;
            pData->Tariff.TPurseMinBalance                                  = MinTPurseBalance;
            pData->Tariff.TPurseMaximumBalance                              = MaxTPurseBalance;
        }
    }

    /*  WHEN */
    {
        /*  Executes business rule */
        RuleResult  = BR_LLSC_99_1( pData );
    }

    /*  THEN */
    {
        UT_Start( );

        /*  Make sure business rule is bypassed */
        UT_Assert( RuleResult == RULE_RESULT_BYPASSED );
        UT_Assert( pData->ReturnedData.bypassCode == BYPASS_CODE( 99, 1, 4, 2 ) );

        /*  Make sure reject code is correct */
        UT_Assert( pData->ReturnedData.rejectReason == MYKI_BR_REJECT_REASON_PRECOMMIT_CHECK_FAILED );

        return  UT_Result( );
    }
}   /*  test_BR_LLSC_99_1_001l( ) */

int test_BR_LLSC_99_1_001m( MYKI_BR_ContextData_t *pData )
{
    MYKI_CAControl_t       *pMYKI_CAControl         = NULL;
    MYKI_TAControl_t       *pMYKI_TAControl         = NULL;
    MYKI_TACapping_t       *pMYKI_TACapping         = NULL;
    MYKI_TAPurseBalance_t  *pMYKI_TAPurseBalance    = NULL;
    MYKI_TAPurseControl_t  *pMYKI_TAPurseControl    = NULL;
    MYKI_TAProduct_t       *pMYKI_TAProduct         = NULL;
    int                     DirIndex                = 2;
    int                     EntryPointId            = 53;
    int                     LineId                  = 66;
    int                     StopId                  = 79;
    int                     ServiceProviderId       = 100;
    int                     Balance                 = 1000;
    int                     NextTxSeqNo             = 5;
    Currency_t              MinTPurseBalance        = 100;
    Currency_t              MaxTPurseBalance        = 10000;
    RuleResult_e            RuleResult              = RULE_RESULT_ERROR;

    if ( pData == NULL ||
         MYKI_CS_CAControlGet(               &pMYKI_CAControl      ) != MYKI_CS_OK ||
         MYKI_CS_TAControlGet(               &pMYKI_TAControl      ) != MYKI_CS_OK ||
         MYKI_CS_TACappingGet(               &pMYKI_TACapping      ) != MYKI_CS_OK ||
         MYKI_CS_TAPurseBalanceGet(          &pMYKI_TAPurseBalance ) != MYKI_CS_OK ||
         MYKI_CS_TAPurseControlGet(          &pMYKI_TAPurseControl ) != MYKI_CS_OK ||
         MYKI_CS_TAProductGet( DirIndex - 1, &pMYKI_TAProduct      ) != MYKI_CS_OK )
    {
        return FALSE;
    }

    /*  GIVEN */
    {
        /*  Card image */
        {
            pMYKI_CAControl->Status                                         = CARD_CONTROL_STATUS_ACTIVATED;

            pMYKI_TAControl->Status                                         = TAPP_CONTROL_STATUS_ACTIVATED;
            pMYKI_TAControl->Directory[ 0 ].Status                          = TAPP_CONTROL_DIRECTORY_STATUS_ACTIVATED;
            pMYKI_TAPurseBalance->Balance                                   = Balance;
            pMYKI_TAPurseControl->NextTxSeqNo                               = NextTxSeqNo;
            pMYKI_TAPurseControl->LastChangeNewBalance                      = Balance + 1;  /*  != Balance */

            pMYKI_TAControl->Directory[ DirIndex ].Status                   = TAPP_CONTROL_DIRECTORY_STATUS_ACTIVATED;
            pMYKI_TAControl->Directory[ DirIndex ].ProductId                = PRODUCT_ID_NHOUR;
            {
                pMYKI_TAProduct->InstanceCount                              = 0;
                pMYKI_TAProduct->StartDateTime                              = pData->DynamicData.currentDateTime - 10;
                pMYKI_TAProduct->EndDateTime                                = pMYKI_TAProduct->StartDateTime + HOURS_TO_SECONDS( 2 );
            }

            pMYKI_TACapping->Daily.Expiry                                   = pData->DynamicData.currentBusinessDate + 1;
            pMYKI_TACapping->Weekly.Expiry                                  = pData->DynamicData.currentBusinessDate + 7 + 1;
        }

        /*  BR context data */
        {
            pData->DynamicData.entryPointId                                 = EntryPointId;
            pData->DynamicData.lineId                                       = LineId;
            pData->DynamicData.stopId                                       = StopId;
            pData->StaticData.serviceProviderId                             = ServiceProviderId;
            pData->Tariff.TPurseMinBalance                                  = MinTPurseBalance;
            pData->Tariff.TPurseMaximumBalance                              = MaxTPurseBalance;
        }
    }

    /*  WHEN */
    {
        /*  Executes business rule */
        RuleResult  = BR_LLSC_99_1( pData );
    }

    /*  THEN */
    {
        UT_Start( );

        /*  Make sure business rule is bypassed */
        UT_Assert( RuleResult == RULE_RESULT_BYPASSED );
        UT_Assert( pData->ReturnedData.bypassCode == BYPASS_CODE( 99, 1, 5, 0 ) );

        /*  Make sure reject code is correct */
        UT_Assert( pData->ReturnedData.rejectReason == MYKI_BR_REJECT_REASON_PRECOMMIT_CHECK_FAILED );

        return  UT_Result( );
    }
}   /*  test_BR_LLSC_99_1_001m( ) */

int test_BR_LLSC_99_1_001n( MYKI_BR_ContextData_t *pData )
{
    MYKI_CAControl_t       *pMYKI_CAControl         = NULL;
    MYKI_TAControl_t       *pMYKI_TAControl         = NULL;
    MYKI_TACapping_t       *pMYKI_TACapping         = NULL;
    MYKI_TAPurseBalance_t  *pMYKI_TAPurseBalance    = NULL;
    MYKI_TAPurseControl_t  *pMYKI_TAPurseControl    = NULL;
    MYKI_TAProduct_t       *pMYKI_TAProduct         = NULL;
    int                     DirIndex                = 2;
    int                     EntryPointId            = 53;
    int                     LineId                  = 66;
    int                     StopId                  = 79;
    int                     ServiceProviderId       = 100;
    int                     Balance                 = 1000;
    int                     NextTxSeqNo             = 5;
    Currency_t              MinTPurseBalance        = 100;
    Currency_t              MaxTPurseBalance        = 10000;
    RuleResult_e            RuleResult              = RULE_RESULT_ERROR;

    if ( pData == NULL ||
         MYKI_CS_CAControlGet(               &pMYKI_CAControl      ) != MYKI_CS_OK ||
         MYKI_CS_TAControlGet(               &pMYKI_TAControl      ) != MYKI_CS_OK ||
         MYKI_CS_TACappingGet(               &pMYKI_TACapping      ) != MYKI_CS_OK ||
         MYKI_CS_TAPurseBalanceGet(          &pMYKI_TAPurseBalance ) != MYKI_CS_OK ||
         MYKI_CS_TAPurseControlGet(          &pMYKI_TAPurseControl ) != MYKI_CS_OK ||
         MYKI_CS_TAProductGet( DirIndex - 1, &pMYKI_TAProduct      ) != MYKI_CS_OK )
    {
        return FALSE;
    }

    /*  GIVEN */
    {
        /*  Card image */
        {
            pMYKI_CAControl->Status                                         = CARD_CONTROL_STATUS_ACTIVATED;

            pMYKI_TAControl->Status                                         = TAPP_CONTROL_STATUS_ACTIVATED;
            pMYKI_TAControl->Directory[ 0 ].Status                          = TAPP_CONTROL_DIRECTORY_STATUS_ACTIVATED;
            pMYKI_TAPurseBalance->Balance                                   = Balance;
            pMYKI_TAPurseControl->NextTxSeqNo                               = NextTxSeqNo;
            pMYKI_TAPurseControl->LastChangeNewBalance                      = Balance;
            pMYKI_TAPurseControl->LastChangeTxSeqNo                         = NextTxSeqNo - 2;  /*  INCORRECT! */

            pMYKI_TAControl->Directory[ DirIndex ].Status                   = TAPP_CONTROL_DIRECTORY_STATUS_ACTIVATED;
            pMYKI_TAControl->Directory[ DirIndex ].ProductId                = PRODUCT_ID_NHOUR;
            {
                pMYKI_TAProduct->InstanceCount                              = 0;
                pMYKI_TAProduct->StartDateTime                              = pData->DynamicData.currentDateTime - 10;
                pMYKI_TAProduct->EndDateTime                                = pMYKI_TAProduct->StartDateTime + HOURS_TO_SECONDS( 2 );
            }

            pMYKI_TACapping->Daily.Expiry                                   = pData->DynamicData.currentBusinessDate + 1;
            pMYKI_TACapping->Weekly.Expiry                                  = pData->DynamicData.currentBusinessDate + 7 + 1;
        }

        /*  BR context data */
        {
            pData->DynamicData.entryPointId                                 = EntryPointId;
            pData->DynamicData.lineId                                       = LineId;
            pData->DynamicData.stopId                                       = StopId;
            pData->StaticData.serviceProviderId                             = ServiceProviderId;
            pData->Tariff.TPurseMinBalance                                  = MinTPurseBalance;
            pData->Tariff.TPurseMaximumBalance                              = MaxTPurseBalance;
        }
    }

    /*  WHEN */
    {
        /*  Executes business rule */
        RuleResult  = BR_LLSC_99_1( pData );
    }

    /*  THEN */
    {
        UT_Start( );

        /*  Make sure business rule is bypassed */
        UT_Assert( RuleResult == RULE_RESULT_BYPASSED );
        UT_Assert( pData->ReturnedData.bypassCode == BYPASS_CODE( 99, 1, 6, 0 ) );

        /*  Make sure reject code is correct */
        UT_Assert( pData->ReturnedData.rejectReason == MYKI_BR_REJECT_REASON_PRECOMMIT_CHECK_FAILED );

        return  UT_Result( );
    }
}   /*  test_BR_LLSC_99_1_001n( ) */

/*==========================================================================*
**
**  test_BR_LLSC_99_1_002
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

int test_BR_LLSC_99_1_002( MYKI_BR_ContextData_t *pData )
{
    MYKI_CAControl_t       *pMYKI_CAControl         = NULL;
    MYKI_TAControl_t       *pMYKI_TAControl         = NULL;
    MYKI_TACapping_t       *pMYKI_TACapping         = NULL;
    MYKI_TAPurseBalance_t  *pMYKI_TAPurseBalance    = NULL;
    MYKI_TAPurseControl_t  *pMYKI_TAPurseControl    = NULL;
    MYKI_TAProduct_t       *pMYKI_TAProduct_NHour   = NULL;
    MYKI_TAProduct_t       *pMYKI_TAProduct_EPass   = NULL;
    int                     DirIndex_NHour          = 2;
    int                     DirIndex_EPass          = 3;
    int                     EntryPointId            = 53;
    int                     LineId                  = 66;
    int                     StopId                  = 79;
    int                     ServiceProviderId       = 100;
    int                     Balance                 = 1000;
    int                     NextTxSeqNo             = 5;
    int                     InstanceCount           = 7;
    Currency_t              MinTPurseBalance        = 100;
    Currency_t              MaxTPurseBalance        = 10000;
    RuleResult_e            RuleResult              = RULE_RESULT_ERROR;

    if ( pData == NULL ||
         MYKI_CS_CAControlGet(                     &pMYKI_CAControl       ) != MYKI_CS_OK ||
         MYKI_CS_TAControlGet(                     &pMYKI_TAControl       ) != MYKI_CS_OK ||
         MYKI_CS_TACappingGet(                     &pMYKI_TACapping       ) != MYKI_CS_OK ||
         MYKI_CS_TAPurseBalanceGet(                &pMYKI_TAPurseBalance  ) != MYKI_CS_OK ||
         MYKI_CS_TAPurseControlGet(                &pMYKI_TAPurseControl  ) != MYKI_CS_OK ||
         MYKI_CS_TAProductGet( DirIndex_NHour - 1, &pMYKI_TAProduct_NHour ) != MYKI_CS_OK ||
         MYKI_CS_TAProductGet( DirIndex_EPass - 1, &pMYKI_TAProduct_EPass ) != MYKI_CS_OK )
    {
        return FALSE;
    }

    /*  GIVEN */
    {
        /*  Card image */
        {
            pMYKI_CAControl->Status                                         = CARD_CONTROL_STATUS_ACTIVATED;

            pMYKI_TAControl->Status                                         = TAPP_CONTROL_STATUS_ACTIVATED;
            pMYKI_TAControl->Directory[ 0 ].Status                          = TAPP_CONTROL_DIRECTORY_STATUS_ACTIVATED;
            pMYKI_TAControl->ProductInUse                                   = DirIndex_NHour;
            pMYKI_TAPurseBalance->Balance                                   = Balance;
            pMYKI_TAPurseControl->NextTxSeqNo                               = NextTxSeqNo;
            pMYKI_TAPurseControl->LastChangeNewBalance                      = Balance;
            pMYKI_TAPurseControl->LastChangeTxSeqNo                         = NextTxSeqNo - 1;

            pMYKI_TAControl->Directory[ DirIndex_NHour ].Status             = TAPP_CONTROL_DIRECTORY_STATUS_ACTIVATED;
            pMYKI_TAControl->Directory[ DirIndex_NHour ].ProductId          = PRODUCT_ID_NHOUR;
            {
                pMYKI_TAProduct_NHour->ControlBitmap                       |= TAPP_TPRODUCT_CONTROL_PROVISIONAL_BITMAP;
                pMYKI_TAProduct_NHour->InstanceCount                        = 0;
                pMYKI_TAProduct_NHour->StartDateTime                        = pData->DynamicData.currentDateTime - 10;
                pMYKI_TAProduct_NHour->EndDateTime                          = pMYKI_TAProduct_NHour->StartDateTime + HOURS_TO_SECONDS( 2 );
            }

            pMYKI_TAControl->Directory[ DirIndex_EPass ].Status             = TAPP_CONTROL_DIRECTORY_STATUS_ACTIVATED;
            pMYKI_TAControl->Directory[ DirIndex_EPass ].ProductId          = PRODUCT_ID_EPASS;
            {
                pMYKI_TAProduct_EPass->InstanceCount                        = InstanceCount;
                pMYKI_TAProduct_EPass->StartDateTime                        = pData->DynamicData.currentDateTime - 10;
                pMYKI_TAProduct_EPass->EndDateTime                          = pMYKI_TAProduct_EPass->StartDateTime + DAYS_TO_SECONDS( InstanceCount );
            }

            pMYKI_TACapping->Daily.Expiry                                   = pData->DynamicData.currentBusinessDate + 1;
            pMYKI_TACapping->Weekly.Expiry                                  = pData->DynamicData.currentBusinessDate + 7 + 1;
        }

        /*  BR context data */
        {
            pData->DynamicData.entryPointId                                 = EntryPointId;
            pData->DynamicData.lineId                                       = LineId;
            pData->DynamicData.stopId                                       = StopId;
            pData->StaticData.serviceProviderId                             = ServiceProviderId;
            pData->Tariff.TPurseMinBalance                                  = MinTPurseBalance;
            pData->Tariff.TPurseMaximumBalance                              = MaxTPurseBalance;
        }
    }

    /*  WHEN */
    {
        /*  Executes business rule */
        RuleResult  = BR_LLSC_99_1( pData );
    }

    /*  THEN */
    {
        UT_Start( );

        /*  Make sure business rule is executed */
        UT_Assert( RuleResult == RULE_RESULT_EXECUTED );

        return  UT_Result( );
    }
}   /*  test_BR_LLSC_99_1_002( ) */