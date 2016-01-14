/* -*- mode: c; tabs: 4; -*- */
/*=============================================================================
**
**    Vix Technology                   Licensed software
**    (C) 2015                         All rights reserved
**
**=============================================================================
**
**  Project/Product : MBU
**  Filename        : test_BR_VIX_LLSC_21_1.c
**  Author(s)       : Sveta Atseva
**
**  Description     :
**      Implements BR_VIX_OSC_2_1 business rule unit-tests.
**
**  Function(s)     :
**      test_BR_VIX_LLSC_21_1_XXX   [Public]    unit-test functions
**
**  Information     :
**   Compiler(s)    : ANSI C
**   Target(s)      : Independent
**
**  Subversion      :
**      $Id: test_BR_VIX_LLSC_21_1.c  $
**      $HeadURL: https://auperasvn01.aupera.erggroup.com/svn/DPG_SWBase/myki-br/trunk/test/test_BR_VIX_LLSC_21_1.c $
**
**  History         :
**   Vers.  Date        Aut.  Type     Description
**   -----  ----------  ----  -------  ----------------------------------------
**    1.00  15.12.15    SAT   Create
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
**  test_BR_VIX_LLSC_21_1_001a
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

int test_BR_VIX_LLSC_21_1_001a( MYKI_BR_ContextData_t *pData )
{
    MYKI_CAControl_t       *pMYKI_CAControl         = NULL;
    MYKI_TAControl_t       *pMYKI_TAControl         = NULL;
    MYKI_TAPurseBalance_t  *pMYKI_TAPurseBalance    = NULL;
    MYKI_TAPurseControl_t  *pMYKI_TAPurseControl    = NULL;
    int                     EntryPointId            = 53;
    int                     LineId                  = 66;
    int                     StopId                  = 79;
    int                     ServiceProviderId       = 100;
    int                     Balance                 = 1000;
    int                     NextTxSeqNo             = 5;
    RuleResult_e            RuleResult              = RULE_RESULT_ERROR;

    if ( pData == NULL ||
         MYKI_CS_CAControlGet(      &pMYKI_CAControl        ) != MYKI_CS_OK ||
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
            pMYKI_CAControl->Status                                         = CARD_CONTROL_STATUS_ACTIVATED;
            pMYKI_TAControl->Status                                         = TAPP_CONTROL_STATUS_ACTIVATED;
            pMYKI_TAControl->Directory[ 0 ].Status                          = TAPP_CONTROL_DIRECTORY_STATUS_ACTIVATED;
            pMYKI_TAPurseBalance->Balance                                   = Balance;
            pMYKI_TAPurseControl->NextTxSeqNo                               = NextTxSeqNo;
        }

        /*  BR context data */
        {
            pData->DynamicData.entryPointId                                 = EntryPointId;
            pData->DynamicData.lineId                                       = LineId;
            pData->DynamicData.stopId                                       = StopId;
            pData->StaticData.serviceProviderId                             = ServiceProviderId;
        }
    }

    /*  WHEN */
    {
        /*  TPurse top-up amount is not entered */
        {
            pData->DynamicData.tPurseDebitAmount                            = -100;
        }

        /*  Executes business rule */
        RuleResult  = BR_VIX_LLSC_21_1( pData );
    }

    /*  THEN */
    {
        UT_Start( );

        /*  Make sure business rule is bypassed */
        UT_Assert( RuleResult == RULE_RESULT_BYPASSED );
        UT_Assert( pData->ReturnedData.bypassCode == BYPASS_CODE( 21, 1, 1, 0 ) );

        /*  Make sure card image is unchanged */
        UT_Assert( pMYKI_TAPurseBalance->Balance == Balance );
        UT_Assert( pMYKI_TAPurseControl->NextTxSeqNo == NextTxSeqNo );

        /*  Make sure reject code is correct */
        UT_Assert( pData->ReturnedData.rejectReason == MYKI_BR_REJECT_REASON_INVALID_AMOUNT );

        return  UT_Result( );
    }
}   /*  test_BR_VIX_LLSC_21_1_001a( ) */

/*==========================================================================*
**
**  test_BR_VIX_LLSC_21_1_001b
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
int test_BR_VIX_LLSC_21_1_001b( MYKI_BR_ContextData_t *pData )
{
    MYKI_CAControl_t       *pMYKI_CAControl         = NULL;
    MYKI_TAControl_t       *pMYKI_TAControl         = NULL;
    MYKI_TAPurseBalance_t  *pMYKI_TAPurseBalance    = NULL;
    MYKI_TAPurseControl_t  *pMYKI_TAPurseControl    = NULL;
    int                     EntryPointId            = 53;
    int                     LineId                  = 66;
    int                     StopId                  = 79;
    int                     ServiceProviderId       = 100;
    int                     Balance                 = 100;
    int                     NextTxSeqNo             = 5;
    RuleResult_e            RuleResult              = RULE_RESULT_ERROR;

    if ( pData == NULL ||
         MYKI_CS_CAControlGet(      &pMYKI_CAControl        ) != MYKI_CS_OK ||
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
            pMYKI_CAControl->Status                                         = CARD_CONTROL_STATUS_ACTIVATED;
            pMYKI_TAControl->Status                                         = TAPP_CONTROL_STATUS_ACTIVATED;
            pMYKI_TAControl->Directory[ 0 ].Status                          = TAPP_CONTROL_DIRECTORY_STATUS_ACTIVATED;
            pMYKI_TAPurseBalance->Balance                                   = Balance;
            pMYKI_TAPurseControl->NextTxSeqNo                               = NextTxSeqNo;
        }

        /*  BR context data */
        {
            pData->DynamicData.entryPointId                                 = EntryPointId;
            pData->DynamicData.lineId                                       = LineId;
            pData->DynamicData.stopId                                       = StopId;
            pData->StaticData.serviceProviderId                             = ServiceProviderId;
        }
    }

    /*  WHEN */
    {
        /*  TPurse top-up amount is not entered */
        {
            pData->DynamicData.tPurseDebitAmount                            = 120;
        }

        /*  Executes business rule */
        RuleResult  = BR_VIX_LLSC_21_1( pData );
    }

    /*  THEN */
    {
        UT_Start( );

        /*  Make sure business rule is bypassed */
        UT_Assert( RuleResult == RULE_RESULT_BYPASSED );
        UT_Assert( pData->ReturnedData.bypassCode == BYPASS_CODE( 21, 1, 2, 0 ) );

        /*  Make sure card image is unchanged */
        UT_Assert( pMYKI_TAPurseBalance->Balance == Balance );
        UT_Assert( pMYKI_TAPurseControl->NextTxSeqNo == NextTxSeqNo );

        /*  Make sure reject code is correct */
        UT_Assert( pData->ReturnedData.rejectReason == MYKI_BR_REJECT_REASON_INSUFFICIENT_FUND );

        return  UT_Result( );
    }
}   /*  test_BR_VIX_LLSC_21_1_001b( ) */




/*==========================================================================*
**
**  test_BR_VIX_LLSC_21_1_002a
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

int test_BR_VIX_LLSC_21_1_002a( MYKI_BR_ContextData_t *pData )
{
    MYKI_CAControl_t       *pMYKI_CAControl         = NULL;
    MYKI_TAControl_t       *pMYKI_TAControl         = NULL;
    MYKI_TAPurseBalance_t  *pMYKI_TAPurseBalance    = NULL;
    MYKI_TAPurseControl_t  *pMYKI_TAPurseControl    = NULL;
    int                     EntryPointId            = 53;
    int                     LineId                  = 66;
    int                     StopId                  = 79;
    int                     ServiceProviderId       = 100;
    int                     DebitAmount             = 550;
    int                     Balance                 = 1000;
    int                     NextTxSeqNo             = 5;
    RuleResult_e            RuleResult              = RULE_RESULT_ERROR;

    if ( pData == NULL ||
         MYKI_CS_CAControlGet(      &pMYKI_CAControl        ) != MYKI_CS_OK ||
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
            pMYKI_TAControl->Status                                         = TAPP_CONTROL_STATUS_ACTIVATED;
            pMYKI_TAControl->Directory[ 0 ].Status                          = TAPP_CONTROL_DIRECTORY_STATUS_ACTIVATED;
            pMYKI_TAPurseBalance->Balance                                   = Balance;
            pMYKI_TAPurseControl->NextTxSeqNo                               = NextTxSeqNo;
        }

        /*  BR context data */
        {
            pData->DynamicData.entryPointId                                 = EntryPointId;
            pData->DynamicData.lineId                                       = LineId;
            pData->DynamicData.stopId                                       = StopId;
            pData->StaticData.serviceProviderId                             = ServiceProviderId;
        }
    }

    /*  WHEN */
    {
        /*  TPurse top-up amount is not entered */
        {
            pData->DynamicData.tPurseDebitAmount                            = DebitAmount;
        }

        /*  Executes business rule */
        RuleResult  = BR_VIX_LLSC_21_1( pData );
    }

    /*  THEN */
    {
        UT_Start( );

        /*  Make sure business rule is bypassed */
        UT_Assert( RuleResult == RULE_RESULT_EXECUTED );

        /*  Make sure card image is correct */
        UT_Assert( pMYKI_TAPurseBalance->Balance == ( Balance - DebitAmount ) );
        UT_Assert( pMYKI_TAPurseControl->NextTxSeqNo == ( NextTxSeqNo + 1 ) );

        /*  Make sure reject code is correct */
        UT_Assert( pData->ReturnedData.acceptReason == MYKI_BR_ACCEPT_TPURSE_DEBITED );
        UT_Assert( pData->ReturnedData.remainingBalance == ( Balance - DebitAmount ) );
        UT_Assert( pData->ReturnedData.txnSeqNo == NextTxSeqNo );

        return  UT_Result( );
    }
}   /* test_BR_VIX_LLSC_21_1_002a( ) */
