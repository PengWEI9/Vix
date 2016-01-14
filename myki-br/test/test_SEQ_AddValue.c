/* -*- mode: c; tabs: 4; -*- */
/*=============================================================================
**
**    Vix Technology                   Licensed software
**    (C) 2015                         All rights reserved
**
**=============================================================================
**
**  Project/Product : MBU
**  Filename        : test_SEQ_AddValue.c
**  Author(s)       : An Tran
**
**  Description     :
**      Implements Add Value sequence unit-tests.
**
**  Function(s)     :
**      test_SEQ_AddValue   [Public]    unit-test functions
**
**  Information     :
**   Compiler(s)    : ANSI C
**   Target(s)      : Independent
**
**  Subversion      :
**      $Id: test_SEQ_AddValue.c 71444 2015-08-26 07:06:16Z atran $
**      $HeadURL: https://auperasvn01.aupera.erggroup.com/svn/DPG_SWBase/myki-br/trunk/test/test_SEQ_AddValue.c $
**
**  History         :
**   Vers.  Date        Aut.  Type     Description
**   -----  ----------  ----  -------  ----------------------------------------
**    1.00  05.08.15    ANT   Create
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
**  test_SEQ_AddValue_001
**
**  Description     :
**      TPurse value added.
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

int test_SEQ_AddValue_001( MYKI_BR_ContextData_t *pData )
{
    MYKI_CAControl_t       *pMYKI_CAControl         = NULL;
    MYKI_CAIssuer_t        *pMYKI_CAIssuer          = NULL;
    MYKI_TAControl_t       *pMYKI_TAControl         = NULL;
    MYKI_TAPurseBalance_t  *pMYKI_TAPurseBalance    = NULL;
    MYKI_TAPurseControl_t  *pMYKI_TAPurseControl    = NULL;
    int                     EntryPointId            = 53;
    int                     LineId                  = 66;
    int                     StopId                  = 79;
    int                     ServiceProviderId       = 100;
    int                     TopupAmount             = 550;
    int                     Balance                 = 1000;
    int                     NextTxSeqNo             = 5;
    Currency_t              MinimumAddValue         = 100;
    Currency_t              MaximumAddValue         = 5000;
    Currency_t              MaximumTPurseBalance    = 10000;
    BusinessRulesResult_e   RuleResult              = MYKI_BR_RESULT_ERROR;

    if ( pData == NULL ||
         MYKI_CS_CAControlGet(      &pMYKI_CAControl        ) != MYKI_CS_OK ||
         MYKI_CS_CAIssuerGet(       &pMYKI_CAIssuer         ) != MYKI_CS_OK ||
         MYKI_CS_TAControlGet(      &pMYKI_TAControl        ) != MYKI_CS_OK ||
         MYKI_CS_TAPurseBalanceGet( &pMYKI_TAPurseBalance   ) != MYKI_CS_OK ||
         MYKI_CS_TAPurseControlGet( &pMYKI_TAPurseControl   ) != MYKI_CS_OK )
    {
        return FALSE;
    }

    /*  GIVEN */
    {
        /*  Card image */
        {
            MYKI_CS_OpenCard( MYKI_CS_OPEN_DEFAULT, NULL );
            pMYKI_CAControl->Status                                         = CARD_CONTROL_STATUS_ACTIVATED;
            pMYKI_CAControl->ExpiryDate                                     = pData->DynamicData.currentBusinessDate + DAYS_TO_SECONDS( 2 );
            pMYKI_CAIssuer->OperationMode                                   = 0;
            pMYKI_TAControl->Status                                         = TAPP_CONTROL_STATUS_ACTIVATED;
            pMYKI_TAControl->BlockingReason                                 = TAPP_CONTROL_BLOCKING_REASON_RESERVED;
            pMYKI_TAControl->ExpiryDate                                     = pData->DynamicData.currentBusinessDate + DAYS_TO_SECONDS( 1 );
            pMYKI_TAControl->Directory[ 0 ].Status                          = TAPP_CONTROL_DIRECTORY_STATUS_ACTIVATED;
            pMYKI_TAPurseBalance->Balance                                   = Balance;
            pMYKI_TAPurseControl->NextTxSeqNo                               = NextTxSeqNo;
        }

        /*  BR context data */
        {
            pData->DynamicData.entryPointId                                 = EntryPointId;
            pData->DynamicData.lineId                                       = LineId;
            pData->DynamicData.stopId                                       = StopId;
            pData->DynamicData.numberOfCardsPresented                       = 1;
            pData->DynamicData.isTransit                                    = TRUE;
            pData->DynamicData.isOperator                                   = FALSE;
            pData->Tariff.addValueEnabled                                   = 1;
            pData->Tariff.minimumAddValue                                   = MinimumAddValue;
            pData->Tariff.maximumAddValue                                   = MaximumAddValue;
            pData->Tariff.TPurseMaximumBalance                              = MaximumTPurseBalance;
            pData->StaticData.serviceProviderId                             = ServiceProviderId;
        }
    }

    /*  WHEN */
    {
        /*  Top-up amount entered */
        {
            pData->DynamicData.tpurseLoadAmount                             = TopupAmount;
        }

        /*  Executes business sequence */
        RuleResult  = (BusinessRulesResult_e)MYKI_BR_ExecuteSequence( pData, SEQ_FUNCTION_ADD_VALUE );
    }

    /*  THEN */
    {
        UT_Start( );

        /*  Make sure business rule is executed */
        UT_Assert( RuleResult == MYKI_BR_RESULT_SUCCESS );

        /*  Make sure card image is correctly updated */
        UT_Assert( pMYKI_TAPurseBalance->Balance == ( Balance + TopupAmount ) );
        UT_Assert( pMYKI_TAPurseControl->NextTxSeqNo == ( NextTxSeqNo + 1 ) );

        /*  Make sure accept/reject code is correct */
        UT_Assert( pData->ReturnedData.rejectReason == MYKI_BR_REJECT_REASON_NOT_APPLICABLE );
        UT_Assert( pData->ReturnedData.acceptReason == MYKI_BR_ACCEPT_TPURSE_LOADED );

        /*  Already generate log(s) */
        pData->InternalData.IsUsageLogUpdated   = FALSE;
        pData->InternalData.IsLoadLogUpdated    = FALSE;

        return  UT_Result( );
    }
}   /*  test_SEQ_AddValue_001( ) */