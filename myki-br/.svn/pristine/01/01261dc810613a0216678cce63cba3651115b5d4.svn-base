/* -*- mode: c; tabs: 4; -*- */
/*=============================================================================
**
**    Vix Technology                   Licensed software
**    (C) 2013                         All rights reserved
**
**=============================================================================
**
**  Project/Product : MBU
**  Filename        : test_BR_LLSC_2_13.c
**  Author(s)       : An Tran
**
**  Description     :
**      Implements BR_LLSC_2_13 business rule unit-tests.
**
**  Function(s)     :
**      test_BR_LLSC_2_13_XXX       [Public]    unit-test functions
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
**    1.00  08.11.13    ANT   Create
**    1.01  23.04.15    ANT   Modify   MBU-1352: Set ReturnedData.topupAmount
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

#include <cs.h>
#include <myki_cardservices.h>
#include <myki_cdd_enums.h>
#include <myki_br.h>
#include <myki_br_rules.h>
#include <myki_br_context_data.h>

#include "test_common.h"

/*==========================================================================*
**
**  test_BR_LLSC_2_13_001a
**  test_BR_LLSC_2_13_001b
**  test_BR_LLSC_2_13_001c
**  test_BR_LLSC_2_13_001d
**  test_BR_LLSC_2_13_001e
**  test_BR_LLSC_2_13_001f
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

int test_BR_LLSC_2_13_001a( MYKI_BR_ContextData_t *pData )
{
    MYKI_TAControl_t       *pMYKI_TAControl         = NULL;
    MYKI_TAPurseBalance_t  *pMYKI_TAPurseBalance    = NULL;
    MYKI_TAPurseControl_t  *pMYKI_TAPurseControl    = NULL;
    MYKI_CAControl_t       *pCAControl              = NULL;
    Currency_t              Balance                 = 1000;
    Currency_t              LoadLogTxnValue         = 0;
    Currency_t              ActionValue             = 555;
    int                     ActionSeqNo             = 12;
    int                     NextTxSeqNo             = 42;
    int                     EntryPointId            = 53;
    int                     LineId                  = 66;
    int                     StopId                  = 79;
    int                     ServiceProviderId       = 100;
    RuleResult_e            RuleResult              = RULE_RESULT_ERROR;

    if ( pData == NULL )
    {
        return FALSE;
    }

    if ( MYKI_CS_CAControlGet( &pCAControl ) != MYKI_CS_OK )
    {
        return FALSE;
    }

    if ( MYKI_CS_TAControlGet( &pMYKI_TAControl ) < 0 )
    {
        return FALSE;
    }

    if ( MYKI_CS_TAPurseBalanceGet( &pMYKI_TAPurseBalance ) < 0 )
    {
        return FALSE;
    }

    if ( MYKI_CS_TAPurseControlGet( &pMYKI_TAPurseControl ) < 0 )
    {
        return FALSE;
    }

    /*  GIVEN */
    {
        /*  Card image */
        {
            pMYKI_TAPurseControl->ActionSeqNo                               = ActionSeqNo;
            pMYKI_TAPurseControl->NextTxSeqNo                               = NextTxSeqNo;
            pMYKI_TAPurseBalance->Balance                                   = Balance;
            pMYKI_TAControl->Directory[ 0 ].Status                          = TAPP_CONTROL_DIRECTORY_STATUS_ACTIVATED;
            pMYKI_TAControl->Status                                         = TAPP_CONTROL_STATUS_ACTIVATED;
            pCAControl->Status                                              = CARD_CONTROL_STATUS_ACTIVATED;
        }

        /*  BR context data */
        {
            pData->DynamicData.entryPointId                                 = EntryPointId;
            pData->DynamicData.lineId                                       = LineId;
            pData->DynamicData.stopId                                       = StopId;
            pData->DynamicData.loadLogTxValue                               = \
            LoadLogTxnValue                                                 = 0;
            pData->StaticData.serviceProviderId                             = ServiceProviderId;
        }

        /*  Transaction log */
        {
            pData->InternalData.LoadLogData.transactionType                 = MYKI_BR_TRANSACTION_TYPE_NONE;
            pData->InternalData.LoadLogData.transactionValue                = 0;
        }
    }

    /*  WHEN */
    {
        /*  Actionlist record */
        {
            pData->ActionList.type                                          = 0;    /*  != ACTION_TPURSE_LOAD_DEBIT */
            pData->ActionList.actionlist.tPurseLoad.actionSequenceNo        = ( ActionSeqNo + 1 );
            pData->ActionList.actionlist.tPurseLoad.value                   = ActionValue;
        }

        /*  Executes business rule */
        RuleResult  = BR_LLSC_2_13( pData );
    }

    /*  THEN */
    {
        UT_Start( );

        /*  Make sure business rule is bypassed */
        UT_Assert( RuleResult == RULE_RESULT_BYPASSED );
        UT_Assert( pData->ReturnedData.bypassCode == BYPASS_CODE( 2, 13, 2, 0 ) );

        /*  Make sure card image is unchanged */
        UT_Assert( pMYKI_TAPurseBalance->Balance == Balance );
        UT_Assert( pMYKI_TAPurseControl->ActionSeqNo == ActionSeqNo );
        UT_Assert( pMYKI_TAPurseControl->NextTxSeqNo == NextTxSeqNo );

        /*  Make sure transaction log is unchanged */
        UT_Assert( pData->InternalData.LoadLogData.transactionType == MYKI_BR_TRANSACTION_TYPE_NONE );
        UT_Assert( pData->InternalData.LoadLogData.transactionValue == 0 );

        /*  Make sure BR context data is unchanged */
        UT_Assert( pData->DynamicData.loadLogTxValue == LoadLogTxnValue );
        UT_Assert( pData->ReturnedData.actionApplied == FALSE );
        UT_Assert( pData->ReturnedData.topupAmount == 0 );

        return  UT_Result( );
    }
}   /*  test_BR_LLSC_2_13_001a( ) */

int test_BR_LLSC_2_13_001b( MYKI_BR_ContextData_t *pData )
{
    MYKI_TAControl_t       *pMYKI_TAControl         = NULL;
    MYKI_TAPurseBalance_t  *pMYKI_TAPurseBalance    = NULL;
    MYKI_TAPurseControl_t  *pMYKI_TAPurseControl    = NULL;
    MYKI_CAControl_t       *pCAControl              = NULL;
    Currency_t              Balance                 = 1000;
    Currency_t              LoadLogTxnValue         = 0;
    Currency_t              ActionValue             = 555;
    int                     ActionSeqNo             = 12;
    int                     NextTxSeqNo             = 42;
    int                     EntryPointId            = 53;
    int                     LineId                  = 66;
    int                     StopId                  = 79;
    int                     ServiceProviderId       = 100;
    RuleResult_e            RuleResult              = RULE_RESULT_ERROR;

    if ( pData == NULL )
    {
        return FALSE;
    }

    if ( MYKI_CS_CAControlGet( &pCAControl ) != MYKI_CS_OK )
    {
        return FALSE;
    }

    if ( MYKI_CS_TAControlGet( &pMYKI_TAControl ) < 0 )
    {
        return FALSE;
    }

    if ( MYKI_CS_TAPurseBalanceGet( &pMYKI_TAPurseBalance ) < 0 )
    {
        return FALSE;
    }

    if ( MYKI_CS_TAPurseControlGet( &pMYKI_TAPurseControl ) < 0 )
    {
        return FALSE;
    }

    /*  GIVEN */
    {
        /*  Card image */
        {
            pMYKI_TAPurseControl->ActionSeqNo                               = ActionSeqNo;
            pMYKI_TAPurseControl->NextTxSeqNo                               = NextTxSeqNo;
            pMYKI_TAPurseBalance->Balance                                   = Balance;
            pMYKI_TAControl->Directory[ 0 ].Status                          = TAPP_CONTROL_DIRECTORY_STATUS_ACTIVATED;
            pMYKI_TAControl->Status                                         = TAPP_CONTROL_STATUS_ACTIVATED;
            pCAControl->Status                                              = CARD_CONTROL_STATUS_ACTIVATED;
        }

        /*  BR context data */
        {
            pData->DynamicData.entryPointId                                 = EntryPointId;
            pData->DynamicData.lineId                                       = LineId;
            pData->DynamicData.stopId                                       = StopId;
            pData->DynamicData.loadLogTxValue                               = \
            LoadLogTxnValue                                                 = 0;
            pData->StaticData.serviceProviderId                             = ServiceProviderId;
        }

        /*  Transaction log */
        {
            pData->InternalData.LoadLogData.transactionType                 = MYKI_BR_TRANSACTION_TYPE_NONE;
            pData->InternalData.LoadLogData.transactionValue                = 0;
        }
    }

    /*  WHEN */
    {
        /*  Actionlist record */
        {
            pData->ActionList.type                                          = ACTION_TPURSE_LOAD_DEBIT;
            pData->ActionList.actionlist.tPurseLoad.actionSequenceNo        = 0;    /*  < 1 */
            pData->ActionList.actionlist.tPurseLoad.value                   = ActionValue;
        }

        /*  Executes business rule */
        RuleResult  = BR_LLSC_2_13( pData );
    }

    /*  THEN */
    {
        UT_Start( );

        /*  Make sure business rule is bypassed */
        UT_Assert( RuleResult == RULE_RESULT_BYPASSED );
        UT_Assert( pData->ReturnedData.bypassCode == BYPASS_CODE( 2, 13, 2, 0 ) );

        /*  Make sure card image is unchanged */
        UT_Assert( pMYKI_TAPurseBalance->Balance == Balance );
        UT_Assert( pMYKI_TAPurseControl->ActionSeqNo == ActionSeqNo );
        UT_Assert( pMYKI_TAPurseControl->NextTxSeqNo == NextTxSeqNo );

        /*  Make sure transaction log is unchanged */
        UT_Assert( pData->InternalData.LoadLogData.transactionType == MYKI_BR_TRANSACTION_TYPE_NONE );
        UT_Assert( pData->InternalData.LoadLogData.transactionValue == 0 );

        /*  Make sure BR context data is unchanged */
        UT_Assert( pData->DynamicData.loadLogTxValue == LoadLogTxnValue );
        UT_Assert( pData->ReturnedData.actionApplied == FALSE );
        UT_Assert( pData->ReturnedData.topupAmount == 0 );

        return  UT_Result( );
    }
}   /*  test_BR_LLSC_2_13_001b( ) */

int test_BR_LLSC_2_13_001c( MYKI_BR_ContextData_t *pData )
{
    MYKI_TAControl_t       *pMYKI_TAControl         = NULL;
    MYKI_TAPurseBalance_t  *pMYKI_TAPurseBalance    = NULL;
    MYKI_TAPurseControl_t  *pMYKI_TAPurseControl    = NULL;
    MYKI_CAControl_t       *pCAControl              = NULL;
    Currency_t              Balance                 = 1000;
    Currency_t              LoadLogTxnValue         = 0;
    Currency_t              ActionValue             = 555;
    int                     ActionSeqNo             = 12;
    int                     NextTxSeqNo             = 42;
    int                     EntryPointId            = 53;
    int                     LineId                  = 66;
    int                     StopId                  = 79;
    int                     ServiceProviderId       = 100;
    RuleResult_e            RuleResult              = RULE_RESULT_ERROR;

    if ( pData == NULL )
    {
        return FALSE;
    }

    if ( MYKI_CS_CAControlGet( &pCAControl ) != MYKI_CS_OK )
    {
        return FALSE;
    }

    if ( MYKI_CS_TAControlGet( &pMYKI_TAControl ) < 0 )
    {
        return FALSE;
    }

    if ( MYKI_CS_TAPurseBalanceGet( &pMYKI_TAPurseBalance ) < 0 )
    {
        return FALSE;
    }

    if ( MYKI_CS_TAPurseControlGet( &pMYKI_TAPurseControl ) < 0 )
    {
        return FALSE;
    }

    /*  GIVEN */
    {
        /*  Card image */
        {
            pMYKI_TAPurseControl->ActionSeqNo                               = ActionSeqNo;
            pMYKI_TAPurseControl->NextTxSeqNo                               = NextTxSeqNo;
            pMYKI_TAPurseBalance->Balance                                   = Balance;
            pMYKI_TAControl->Directory[ 0 ].Status                          = TAPP_CONTROL_DIRECTORY_STATUS_ACTIVATED;
            pMYKI_TAControl->Status                                         = TAPP_CONTROL_STATUS_ACTIVATED;
            pCAControl->Status                                              = CARD_CONTROL_STATUS_ACTIVATED;
        }

        /*  BR context data */
        {
            pData->DynamicData.entryPointId                                 = EntryPointId;
            pData->DynamicData.lineId                                       = LineId;
            pData->DynamicData.stopId                                       = StopId;
            pData->DynamicData.loadLogTxValue                               = \
            LoadLogTxnValue                                                 = 0;
            pData->StaticData.serviceProviderId                             = ServiceProviderId;
        }

        /*  Transaction log */
        {
            pData->InternalData.LoadLogData.transactionType                 = MYKI_BR_TRANSACTION_TYPE_NONE;
            pData->InternalData.LoadLogData.transactionValue                = 0;
        }
    }

    /*  WHEN */
    {
        /*  Actionlist record */
        {
            pData->ActionList.type                                          = ACTION_TPURSE_LOAD_DEBIT;
            pData->ActionList.actionlist.tPurseLoad.actionSequenceNo        = 16;   /*  > 15 */
            pData->ActionList.actionlist.tPurseLoad.value                   = ActionValue;
        }

        /*  Executes business rule */
        RuleResult  = BR_LLSC_2_13( pData );
    }

    /*  THEN */
    {
        UT_Start( );

        /*  Make sure business rule is bypassed */
        UT_Assert( RuleResult == RULE_RESULT_BYPASSED );
        UT_Assert( pData->ReturnedData.bypassCode == BYPASS_CODE( 2, 13, 2, 0 ) );

        /*  Make sure card image is unchanged */
        UT_Assert( pMYKI_TAPurseBalance->Balance == Balance );
        UT_Assert( pMYKI_TAPurseControl->ActionSeqNo == ActionSeqNo );
        UT_Assert( pMYKI_TAPurseControl->NextTxSeqNo == NextTxSeqNo );

        /*  Make sure transaction log is unchanged */
        UT_Assert( pData->InternalData.LoadLogData.transactionType == MYKI_BR_TRANSACTION_TYPE_NONE );
        UT_Assert( pData->InternalData.LoadLogData.transactionValue == 0 );

        /*  Make sure BR context data is unchanged */
        UT_Assert( pData->DynamicData.loadLogTxValue == LoadLogTxnValue );
        UT_Assert( pData->ReturnedData.actionApplied == FALSE );
        UT_Assert( pData->ReturnedData.topupAmount == 0 );

        return  UT_Result( );
    }
}   /*  test_BR_LLSC_2_13_001c( ) */

int test_BR_LLSC_2_13_001d( MYKI_BR_ContextData_t *pData )
{
    MYKI_TAControl_t       *pMYKI_TAControl         = NULL;
    MYKI_TAPurseBalance_t  *pMYKI_TAPurseBalance    = NULL;
    MYKI_TAPurseControl_t  *pMYKI_TAPurseControl    = NULL;
    MYKI_CAControl_t       *pCAControl              = NULL;
    Currency_t              Balance                 = 1000;
    Currency_t              LoadLogTxnValue         = 0;
    Currency_t              ActionValue             = 555;
    int                     NextTxSeqNo             = 42;
    int                     EntryPointId            = 53;
    int                     LineId                  = 66;
    int                     StopId                  = 79;
    int                     ServiceProviderId       = 100;
    RuleResult_e            RuleResult              = RULE_RESULT_ERROR;

    if ( pData == NULL )
    {
        return FALSE;
    }

    if ( MYKI_CS_CAControlGet( &pCAControl ) != MYKI_CS_OK )
    {
        return FALSE;
    }

    if ( MYKI_CS_TAControlGet( &pMYKI_TAControl ) < 0 )
    {
        return FALSE;
    }

    if ( MYKI_CS_TAPurseBalanceGet( &pMYKI_TAPurseBalance ) < 0 )
    {
        return FALSE;
    }

    if ( MYKI_CS_TAPurseControlGet( &pMYKI_TAPurseControl ) < 0 )
    {
        return FALSE;
    }

    /*  GIVEN */
    {
        /*  Card image */
        {
            pMYKI_TAPurseControl->ActionSeqNo                               = 15;
            pMYKI_TAPurseControl->NextTxSeqNo                               = NextTxSeqNo;
            pMYKI_TAPurseBalance->Balance                                   = Balance;
            pMYKI_TAControl->Directory[ 0 ].Status                          = TAPP_CONTROL_DIRECTORY_STATUS_ACTIVATED;
            pMYKI_TAControl->Status                                         = TAPP_CONTROL_STATUS_ACTIVATED;
            pCAControl->Status                                              = CARD_CONTROL_STATUS_ACTIVATED;
        }

        /*  BR context data */
        {
            pData->DynamicData.entryPointId                                 = EntryPointId;
            pData->DynamicData.lineId                                       = LineId;
            pData->DynamicData.stopId                                       = StopId;
            pData->DynamicData.loadLogTxValue                               = \
            LoadLogTxnValue                                                 = 0;
            pData->StaticData.serviceProviderId                             = ServiceProviderId;
        }

        /*  Transaction log */
        {
            pData->InternalData.LoadLogData.transactionType                 = MYKI_BR_TRANSACTION_TYPE_NONE;
            pData->InternalData.LoadLogData.transactionValue                = 0;
        }
    }

    /*  WHEN */
    {
        /*  Actionlist record */
        {
            pData->ActionList.type                                          = ACTION_TPURSE_LOAD_DEBIT;
            pData->ActionList.actionlist.tPurseLoad.actionSequenceNo        = 2;    /*  != 1 */
            pData->ActionList.actionlist.tPurseLoad.value                   = ActionValue;
        }

        /*  Executes business rule */
        RuleResult  = BR_LLSC_2_13( pData );
    }

    /*  THEN */
    {
        UT_Start( );

        /*  Make sure business rule is bypassed */
        UT_Assert( RuleResult == RULE_RESULT_BYPASSED );
        UT_Assert( pData->ReturnedData.bypassCode == BYPASS_CODE( 2, 13, 2, 0 ) );

        /*  Make sure card image is unchanged */
        UT_Assert( pMYKI_TAPurseBalance->Balance == Balance );
        UT_Assert( pMYKI_TAPurseControl->ActionSeqNo == 15 );
        UT_Assert( pMYKI_TAPurseControl->NextTxSeqNo == NextTxSeqNo );

        /*  Make sure transaction log is unchanged */
        UT_Assert( pData->InternalData.LoadLogData.transactionType == MYKI_BR_TRANSACTION_TYPE_NONE );
        UT_Assert( pData->InternalData.LoadLogData.transactionValue == 0 );

        /*  Make sure BR context data is unchanged */
        UT_Assert( pData->DynamicData.loadLogTxValue == LoadLogTxnValue );
        UT_Assert( pData->ReturnedData.actionApplied == FALSE );
        UT_Assert( pData->ReturnedData.topupAmount == 0 );

        return  UT_Result( );
    }
}   /*  test_BR_LLSC_2_13_001d( ) */

int test_BR_LLSC_2_13_001e( MYKI_BR_ContextData_t *pData )
{
    MYKI_TAControl_t       *pMYKI_TAControl         = NULL;
    MYKI_TAPurseBalance_t  *pMYKI_TAPurseBalance    = NULL;
    MYKI_TAPurseControl_t  *pMYKI_TAPurseControl    = NULL;
    MYKI_CAControl_t       *pCAControl              = NULL;
    Currency_t              Balance                 = 1000;
    Currency_t              LoadLogTxnValue         = 0;
    Currency_t              ActionValue             = 555;
    int                     ActionSeqNo             = 12;
    int                     NextTxSeqNo             = 42;
    int                     EntryPointId            = 53;
    int                     LineId                  = 66;
    int                     StopId                  = 79;
    int                     ServiceProviderId       = 100;
    RuleResult_e            RuleResult              = RULE_RESULT_ERROR;

    if ( pData == NULL )
    {
        return FALSE;
    }

    if ( MYKI_CS_CAControlGet( &pCAControl ) != MYKI_CS_OK )
    {
        return FALSE;
    }

    if ( MYKI_CS_TAControlGet( &pMYKI_TAControl ) < 0 )
    {
        return FALSE;
    }

    if ( MYKI_CS_TAPurseBalanceGet( &pMYKI_TAPurseBalance ) < 0 )
    {
        return FALSE;
    }

    if ( MYKI_CS_TAPurseControlGet( &pMYKI_TAPurseControl ) < 0 )
    {
        return FALSE;
    }

    /*  GIVEN */
    {
        /*  Card image */
        {
            pMYKI_TAPurseControl->ActionSeqNo                               = ActionSeqNo;
            pMYKI_TAPurseControl->NextTxSeqNo                               = NextTxSeqNo;
            pMYKI_TAPurseBalance->Balance                                   = Balance;
            pMYKI_TAControl->Directory[ 0 ].Status                          = TAPP_CONTROL_DIRECTORY_STATUS_ACTIVATED;
            pMYKI_TAControl->Status                                         = TAPP_CONTROL_STATUS_ACTIVATED;
            pCAControl->Status                                              = CARD_CONTROL_STATUS_ACTIVATED;
        }

        /*  BR context data */
        {
            pData->DynamicData.entryPointId                                 = EntryPointId;
            pData->DynamicData.lineId                                       = LineId;
            pData->DynamicData.stopId                                       = StopId;
            pData->DynamicData.loadLogTxValue                               = \
            LoadLogTxnValue                                                 = 0;
            pData->StaticData.serviceProviderId                             = ServiceProviderId;
        }

        /*  Transaction log */
        {
            pData->InternalData.LoadLogData.transactionType                 = MYKI_BR_TRANSACTION_TYPE_NONE;
            pData->InternalData.LoadLogData.transactionValue                = 0;
        }
    }

    /*  WHEN */
    {
        /*  Actionlist record */
        {
            pData->ActionList.type                                          = ACTION_TPURSE_LOAD_DEBIT;
            pData->ActionList.actionlist.tPurseLoad.actionSequenceNo        = ActionSeqNo + 2;  /*  != TAPurseControl.ActionSeqNo */
            pData->ActionList.actionlist.tPurseLoad.value                   = ActionValue;
        }

        /*  Executes business rule */
        RuleResult  = BR_LLSC_2_13( pData );
    }

    /*  THEN */
    {
        UT_Start( );

        /*  Make sure business rule is bypassed */
        UT_Assert( RuleResult == RULE_RESULT_BYPASSED );
        UT_Assert( pData->ReturnedData.bypassCode == BYPASS_CODE( 2, 13, 2, 0 ) );

        /*  Make sure card image is unchanged */
        UT_Assert( pMYKI_TAPurseBalance->Balance == Balance );
        UT_Assert( pMYKI_TAPurseControl->ActionSeqNo == ActionSeqNo );
        UT_Assert( pMYKI_TAPurseControl->NextTxSeqNo == NextTxSeqNo );

        /*  Make sure transaction log is unchanged */
        UT_Assert( pData->InternalData.LoadLogData.transactionType == MYKI_BR_TRANSACTION_TYPE_NONE );
        UT_Assert( pData->InternalData.LoadLogData.transactionValue == 0 );

        /*  Make sure BR context data is unchanged */
        UT_Assert( pData->DynamicData.loadLogTxValue == LoadLogTxnValue );
        UT_Assert( pData->ReturnedData.actionApplied == FALSE );
        UT_Assert( pData->ReturnedData.topupAmount == 0 );

        return  UT_Result( );
    }
}   /*  test_BR_LLSC_2_13_001e( ) */

int test_BR_LLSC_2_13_001f( MYKI_BR_ContextData_t *pData )
{
    MYKI_TAControl_t       *pMYKI_TAControl         = NULL;
    MYKI_TAPurseBalance_t  *pMYKI_TAPurseBalance    = NULL;
    MYKI_TAPurseControl_t  *pMYKI_TAPurseControl    = NULL;
    MYKI_CAControl_t       *pCAControl              = NULL;
    Currency_t              Balance                 = 1000;
    Currency_t              LoadLogTxnValue         = 0;
    Currency_t              ActionValue             = 555;
    int                     ActionSeqNo             = 12;
    int                     NextTxSeqNo             = 42;
    int                     EntryPointId            = 53;
    int                     LineId                  = 66;
    int                     StopId                  = 79;
    int                     ServiceProviderId       = 100;
    RuleResult_e            RuleResult              = RULE_RESULT_ERROR;

    if ( pData == NULL )
    {
        return FALSE;
    }

    if ( MYKI_CS_CAControlGet( &pCAControl ) != MYKI_CS_OK )
    {
        return FALSE;
    }

    if ( MYKI_CS_TAControlGet( &pMYKI_TAControl ) < 0 )
    {
        return FALSE;
    }

    if ( MYKI_CS_TAPurseBalanceGet( &pMYKI_TAPurseBalance ) < 0 )
    {
        return FALSE;
    }

    if ( MYKI_CS_TAPurseControlGet( &pMYKI_TAPurseControl ) < 0 )
    {
        return FALSE;
    }

    /*  GIVEN */
    {
        /*  Card image - TPurse Blocked */
        {
            pMYKI_TAPurseControl->ActionSeqNo                               = ActionSeqNo;
            pMYKI_TAPurseControl->NextTxSeqNo                               = NextTxSeqNo;
            pMYKI_TAPurseBalance->Balance                                   = Balance;
            pMYKI_TAControl->Directory[ 0 ].Status                          = TAPP_CONTROL_DIRECTORY_STATUS_BLOCKED;
            pMYKI_TAControl->Status                                         = TAPP_CONTROL_STATUS_ACTIVATED;
            pCAControl->Status                                              = CARD_CONTROL_STATUS_ACTIVATED;
        }

        /*  BR context data */
        {
            pData->DynamicData.entryPointId                                 = EntryPointId;
            pData->DynamicData.lineId                                       = LineId;
            pData->DynamicData.stopId                                       = StopId;
            pData->DynamicData.loadLogTxValue                               = \
            LoadLogTxnValue                                                 = 0;
            pData->StaticData.serviceProviderId                             = ServiceProviderId;
        }

        /*  Transaction log */
        {
            pData->InternalData.LoadLogData.transactionType                 = MYKI_BR_TRANSACTION_TYPE_NONE;
            pData->InternalData.LoadLogData.transactionValue                = 0;
        }
    }

    /*  WHEN */
    {
        /*  Actionlist record */
        {
            pData->ActionList.type                                          = ACTION_TPURSE_LOAD_DEBIT;
            pData->ActionList.actionlist.tPurseLoad.actionSequenceNo        = ActionSeqNo + 1;
            pData->ActionList.actionlist.tPurseLoad.value                   = ActionValue;
        }

        /*  Executes business rule */
        RuleResult  = BR_LLSC_2_13( pData );
    }

    /*  THEN */
    {
        UT_Start( );

        /*  Make sure business rule is bypassed */
        UT_Assert( RuleResult == RULE_RESULT_BYPASSED );
        UT_Assert( pData->ReturnedData.bypassCode == BYPASS_CODE( 2, 13, 6, 0 ) );

        /*  Make sure card image is unchanged */
        UT_Assert( pMYKI_TAPurseBalance->Balance == Balance );
        UT_Assert( pMYKI_TAPurseControl->ActionSeqNo == ActionSeqNo );
        UT_Assert( pMYKI_TAPurseControl->NextTxSeqNo == NextTxSeqNo );

        /*  Make sure transaction log is unchanged */
        UT_Assert( pData->InternalData.LoadLogData.transactionType == MYKI_BR_TRANSACTION_TYPE_NONE );
        UT_Assert( pData->InternalData.LoadLogData.transactionValue == 0 );

        /*  Make sure BR context data is unchanged */
        UT_Assert( pData->DynamicData.loadLogTxValue == LoadLogTxnValue );
        UT_Assert( pData->ReturnedData.actionApplied == FALSE );
        UT_Assert( pData->ReturnedData.topupAmount == 0 );

        return  UT_Result( );
    }
}   /*  test_BR_LLSC_2_13_001f( ) */

/*==========================================================================*
**
**  test_BR_LLSC_2_13_002
**
**  Description     :
**      Unit-test EXECUTED conditions, Dynamic.TxType = 0 and
**      Dynamic.TxValue = 0.
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

int test_BR_LLSC_2_13_002( MYKI_BR_ContextData_t *pData )
{
    MYKI_TAControl_t       *pMYKI_TAControl         = NULL;
    MYKI_TAPurseBalance_t  *pMYKI_TAPurseBalance    = NULL;
    MYKI_TAPurseControl_t  *pMYKI_TAPurseControl    = NULL;
    MYKI_CAControl_t       *pCAControl              = NULL;
    Currency_t              Balance                 = 1000;
    Currency_t              LoadLogTxnValue         = 0;
    Currency_t              ActionValue             = 555;
    int                     ActionSeqNo             = 12;
    int                     NextTxSeqNo             = 42;
    int                     EntryPointId            = 53;
    int                     LineId                  = 66;
    int                     StopId                  = 79;
    int                     ServiceProviderId       = 100;
    RuleResult_e            RuleResult              = RULE_RESULT_ERROR;

    if ( pData == NULL )
    {
        return FALSE;
    }   /* end-of-if */

    if ( MYKI_CS_TAControlGet( &pMYKI_TAControl ) < 0 || pMYKI_TAControl == NULL )
    {
        return FALSE;
    }   /* end-of-if */

    if ( MYKI_CS_TAPurseBalanceGet( &pMYKI_TAPurseBalance ) < 0 )
    {
        return FALSE;
    }

    if ( MYKI_CS_TAPurseControlGet( &pMYKI_TAPurseControl ) < 0 )
    {
        return FALSE;
    }   /* end-of-if */

    if ( MYKI_CS_CAControlGet( &pCAControl ) != MYKI_CS_OK )
    {
        return FALSE;
    }

    /*  GIVEN */
    {
        /*  Card image */
        {
            pMYKI_TAPurseControl->ActionSeqNo                               = ActionSeqNo;
            pMYKI_TAPurseControl->NextTxSeqNo                               = NextTxSeqNo;
            pMYKI_TAPurseBalance->Balance                                   = Balance;
            pMYKI_TAControl->Directory[ 0 ].Status                          = TAPP_CONTROL_DIRECTORY_STATUS_ACTIVATED;
            pMYKI_TAControl->Status                                         = TAPP_CONTROL_STATUS_ACTIVATED;
            pCAControl->Status                                              = CARD_CONTROL_STATUS_ACTIVATED;
        }

        /*  BR context data */
        {
            pData->DynamicData.entryPointId                                 = EntryPointId;
            pData->DynamicData.lineId                                       = LineId;
            pData->DynamicData.stopId                                       = StopId;
            pData->DynamicData.loadLogTxValue                               = \
            LoadLogTxnValue                                                 = 0;    /*  Dynamic.LoadLogTxnValue = 0 */
            pData->StaticData.serviceProviderId                             = ServiceProviderId;
        }

        /*  Transaction log */
        {
            pData->InternalData.LoadLogData.transactionType                 = MYKI_BR_TRANSACTION_TYPE_NONE;
            pData->InternalData.LoadLogData.transactionValue                = 0;
        }
    }

    /*  WHEN */
    {
        /*  Actionlist record */
        {
            pData->ActionList.type                                          = ACTION_TPURSE_LOAD_DEBIT;
            pData->ActionList.actionlist.tPurseLoad.isActionSequenceNoSet   = TRUE;
            pData->ActionList.actionlist.tPurseLoad.actionSequenceNo        = ActionSeqNo + 1;
            pData->ActionList.actionlist.tPurseLoad.value                   = ActionValue;
        }

        /*  Executes business rule */
        RuleResult  = BR_LLSC_2_13( pData );
    }

    /*  THEN */
    {
        UT_Start( );

        /*  Make sure business rule is executed */
        UT_Assert( RuleResult == RULE_RESULT_EXECUTED );

        /*  Make sure card image is correct */
        UT_Assert( pMYKI_TAPurseBalance->Balance == ( Balance - ActionValue ) );
        UT_Assert( pMYKI_TAPurseControl->ActionSeqNo == ( ActionSeqNo + 1 ) );
        UT_Assert( pMYKI_TAPurseControl->NextTxSeqNo == ( NextTxSeqNo + 1 ) );

        /*  Make sure transaction log is correct */
        UT_Assert( pData->InternalData.LoadLogData.transactionType == MYKI_BR_TRANSACTION_TYPE_TPURSE_AL_LOAD_VALUE );
        UT_Assert( pData->InternalData.LoadLogData.transactionValue == abs( LoadLogTxnValue - ActionValue ) );
        UT_Assert( pData->InternalData.LoadLogData.transactionSequenceNumber == NextTxSeqNo );

        /*  Make sure BR context data is correct */
        UT_Assert( pData->DynamicData.loadLogTxValue == ( LoadLogTxnValue - ActionValue ) );
        UT_Assert( pData->ReturnedData.actionApplied == TRUE );
        UT_Assert( pData->ReturnedData.topupAmount == ( - ActionValue ) );

        return  UT_Result( );
    }
}   /* test_BR_LLSC_2_13_002( ) */

/*==========================================================================*
**
**  test_BR_LLSC_2_13_003
**
**  Description     :
**      Unit-test EXECUTED conditions, Dynamic.TxType = 25 and
**      Dynamic.TxValue < Actionlist.Value.
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

int test_BR_LLSC_2_13_003( MYKI_BR_ContextData_t *pData )
{
    MYKI_TAControl_t       *pMYKI_TAControl         = NULL;
    MYKI_TAPurseBalance_t  *pMYKI_TAPurseBalance    = NULL;
    MYKI_TAPurseControl_t  *pMYKI_TAPurseControl    = NULL;
    MYKI_CAControl_t       *pCAControl              = NULL;
    Currency_t              Balance                 = 1000;
    Currency_t              LoadLogTxnValue         = 0;
    Currency_t              ActionValue             = 555;
    int                     ActionSeqNo             = 12;
    int                     NextTxSeqNo             = 42;
    int                     EntryPointId            = 53;
    int                     LineId                  = 66;
    int                     StopId                  = 79;
    int                     ServiceProviderId       = 100;
    RuleResult_e            RuleResult              = RULE_RESULT_ERROR;

    if ( pData == NULL )
    {
        return FALSE;
    }   /* end-of-if */

    if ( MYKI_CS_TAControlGet( &pMYKI_TAControl ) < 0 || pMYKI_TAControl == NULL )
    {
        return FALSE;
    }   /* end-of-if */

    if ( MYKI_CS_TAPurseBalanceGet( &pMYKI_TAPurseBalance ) < 0 )
    {
        return FALSE;
    }

    if ( MYKI_CS_TAPurseControlGet( &pMYKI_TAPurseControl ) < 0 )
    {
        return FALSE;
    }   /* end-of-if */

    if ( MYKI_CS_CAControlGet( &pCAControl ) != MYKI_CS_OK )
    {
        return FALSE;
    }

    /*  GIVEN */
    {
        /*  Card image */
        {
            pMYKI_TAPurseControl->ActionSeqNo                               = ActionSeqNo;
            pMYKI_TAPurseControl->NextTxSeqNo                               = NextTxSeqNo;
            pMYKI_TAPurseBalance->Balance                                   = Balance;
            pMYKI_TAControl->Directory[ 0 ].Status                          = TAPP_CONTROL_DIRECTORY_STATUS_ACTIVATED;
            pMYKI_TAControl->Status                                         = TAPP_CONTROL_STATUS_ACTIVATED;
            pCAControl->Status                                              = CARD_CONTROL_STATUS_ACTIVATED;
        }

        /*  BR context data */
        {
            pData->DynamicData.entryPointId                                 = EntryPointId;
            pData->DynamicData.lineId                                       = LineId;
            pData->DynamicData.stopId                                       = StopId;
            pData->DynamicData.loadLogTxValue                               = \
            LoadLogTxnValue                                                 = ActionValue - 500;    /*  Dynamic.LoadLogTxnValue < Actionlist.Value */
            pData->StaticData.serviceProviderId                             = ServiceProviderId;
        }

        /*  Transaction log */
        {
            pData->InternalData.LoadLogData.transactionType                 = MYKI_BR_TRANSACTION_TYPE_AUTO_LOAD_DISABLE;
            pData->InternalData.LoadLogData.transactionValue                = LoadLogTxnValue;
        }
    }

    /*  WHEN */
    {
        /*  Actionlist record */
        {
            pData->ActionList.type                                          = ACTION_TPURSE_LOAD_DEBIT;
            pData->ActionList.actionlist.tPurseLoad.isActionSequenceNoSet   = TRUE;
            pData->ActionList.actionlist.tPurseLoad.actionSequenceNo        = ActionSeqNo + 1;
            pData->ActionList.actionlist.tPurseLoad.value                   = ActionValue;
        }

        /*  Executes business rule */
        RuleResult  = BR_LLSC_2_13( pData );
    }

    /*  THEN */
    {
        UT_Start( );

        /*  Make sure business rule is executed */
        UT_Assert( RuleResult == RULE_RESULT_EXECUTED );

        /*  Make sure card image is correct */
        UT_Assert( pMYKI_TAPurseBalance->Balance == ( Balance - ActionValue ) );
        UT_Assert( pMYKI_TAPurseControl->ActionSeqNo == ( ActionSeqNo + 1 ) );
        UT_Assert( pMYKI_TAPurseControl->NextTxSeqNo == ( NextTxSeqNo + 1 ) );

        /*  Make sure transaction log is correct */
        UT_Assert( pData->InternalData.LoadLogData.transactionType == MYKI_BR_TRANSACTION_TYPE_TPURSE_AL_LOAD_VALUE );
        UT_Assert( pData->InternalData.LoadLogData.transactionValue == abs( LoadLogTxnValue - ActionValue ) );
        UT_Assert( pData->InternalData.LoadLogData.transactionSequenceNumber == NextTxSeqNo );

        /*  Make sure BR context data is correct */
        UT_Assert( pData->DynamicData.loadLogTxValue == ( LoadLogTxnValue - ActionValue ) );
        UT_Assert( pData->ReturnedData.actionApplied == TRUE );
        UT_Assert( pData->ReturnedData.topupAmount == ( - ActionValue ) );

        return  UT_Result( );
    }
}   /* test_BR_LLSC_2_13_003( ) */

/*==========================================================================*
**
**  test_BR_LLSC_2_13_004
**
**  Description     :
**      Unit-test EXECUTED conditions, Dynamic.TxType = 2 and
**      Dynamic.TxValue < Actionlist.Value.
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

int test_BR_LLSC_2_13_004( MYKI_BR_ContextData_t *pData )
{
    MYKI_TAControl_t       *pMYKI_TAControl         = NULL;
    MYKI_TAPurseBalance_t  *pMYKI_TAPurseBalance    = NULL;
    MYKI_TAPurseControl_t  *pMYKI_TAPurseControl    = NULL;
    MYKI_CAControl_t       *pCAControl              = NULL;
    Currency_t              Balance                 = 1000;
    Currency_t              LoadLogTxnValue         = 55;
    Currency_t              ActionValue             = 555;
    int                     ActionSeqNo             = 12;
    int                     NextTxSeqNo             = 42;
    int                     EntryPointId            = 53;
    int                     LineId                  = 66;
    int                     StopId                  = 79;
    int                     ServiceProviderId       = 100;
    RuleResult_e            RuleResult              = RULE_RESULT_ERROR;

    if ( pData == NULL )
    {
        return FALSE;
    }   /* end-of-if */

    if ( MYKI_CS_TAControlGet( &pMYKI_TAControl ) < 0 || pMYKI_TAControl == NULL )
    {
        return FALSE;
    }   /* end-of-if */

    if ( MYKI_CS_TAPurseBalanceGet( &pMYKI_TAPurseBalance ) < 0 )
    {
        return FALSE;
    }

    if ( MYKI_CS_TAPurseControlGet( &pMYKI_TAPurseControl ) < 0 )
    {
        return FALSE;
    }   /* end-of-if */

    if ( MYKI_CS_CAControlGet(&pCAControl) != MYKI_CS_OK )
    {
        return FALSE;
    }

    /*  GIVEN */
    {
        /*  Card image */
        {
            pMYKI_TAPurseControl->ActionSeqNo                               = ActionSeqNo;
            pMYKI_TAPurseControl->NextTxSeqNo                               = NextTxSeqNo;
            pMYKI_TAPurseBalance->Balance                                   = Balance;
            pMYKI_TAControl->Directory[ 0 ].Status                          = TAPP_CONTROL_DIRECTORY_STATUS_ACTIVATED;
            pMYKI_TAControl->Status                                         = TAPP_CONTROL_STATUS_ACTIVATED;
            pCAControl->Status                                              = CARD_CONTROL_STATUS_ACTIVATED;
        }

        /*  BR context data */
        {
            pData->DynamicData.entryPointId                                 = EntryPointId;
            pData->DynamicData.lineId                                       = LineId;
            pData->DynamicData.stopId                                       = StopId;
            pData->DynamicData.loadLogTxValue                               = \
            LoadLogTxnValue                                                 = ActionValue - 500;    /*  Dynamic.LoadLogTxnValue < Actionlist.Value */
            pData->StaticData.serviceProviderId                             = ServiceProviderId;
        }

        /*  Transaction log */
        {
            pData->InternalData.LoadLogData.transactionType                 = MYKI_BR_TRANSACTION_TYPE_LOAD_PRODUCT;
            pData->InternalData.LoadLogData.transactionValue                = LoadLogTxnValue;
        }
    }

    /*  WHEN */
    {
        /*  Actionlist record */
        {
            pData->ActionList.type                                          = ACTION_TPURSE_LOAD_DEBIT;
            pData->ActionList.actionlist.tPurseLoad.isActionSequenceNoSet   = TRUE;
            pData->ActionList.actionlist.tPurseLoad.actionSequenceNo        = ActionSeqNo + 1;
            pData->ActionList.actionlist.tPurseLoad.value                   = ActionValue;
        }

        /*  Executes business rule */
        RuleResult  = BR_LLSC_2_13( pData );
    }

    /*  THEN */
    {
        UT_Start( );

        /*  Make sure business rule is executed */
        UT_Assert( RuleResult == RULE_RESULT_EXECUTED );

        /*  Make sure card image is correct */
        UT_Assert( pMYKI_TAPurseBalance->Balance == ( Balance - ActionValue ) );
        UT_Assert( pMYKI_TAPurseControl->ActionSeqNo == ( ActionSeqNo + 1 ) );
        UT_Assert( pMYKI_TAPurseControl->NextTxSeqNo == ( NextTxSeqNo + 1 ) );

        /*  Make sure transaction log is correct */
        UT_Assert( pData->InternalData.LoadLogData.transactionType == MYKI_BR_TRANSACTION_TYPE_MULTIPLE_ACTION_LIST );
        UT_Assert( pData->InternalData.LoadLogData.transactionValue == 0 );
        UT_Assert( pData->InternalData.LoadLogData.transactionSequenceNumber == NextTxSeqNo );

        /*  Make sure BR context data is correct */
        UT_Assert( pData->DynamicData.loadLogTxValue == ( LoadLogTxnValue - ActionValue ) );
        UT_Assert( pData->ReturnedData.actionApplied == TRUE );
        UT_Assert( pData->ReturnedData.topupAmount == ( - ActionValue ) );

        return  UT_Result( );
    }
}   /* test_BR_LLSC_2_13_004( ) */

/*==========================================================================*
**
**  test_BR_LLSC_2_13_005
**
**  Description     :
**      Unit-test EXECUTED conditions, Dynamic.TxType = 2 and
**      Dynamic.TxValue > Actionlist.Value.
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

int test_BR_LLSC_2_13_005( MYKI_BR_ContextData_t *pData )
{
    MYKI_TAControl_t       *pMYKI_TAControl         = NULL;
    MYKI_TAPurseBalance_t  *pMYKI_TAPurseBalance    = NULL;
    MYKI_TAPurseControl_t  *pMYKI_TAPurseControl    = NULL;
    MYKI_CAControl_t       *pCAControl              = NULL;
    Currency_t              Balance                 = 1000;
    Currency_t              LoadLogTxnValue         = 0;
    Currency_t              ActionValue             = 555;
    int                     NextTxSeqNo             = 42;
    int                     EntryPointId            = 53;
    int                     LineId                  = 66;
    int                     StopId                  = 79;
    int                     ServiceProviderId       = 100;
    RuleResult_e            RuleResult              = RULE_RESULT_ERROR;

    if ( pData == NULL )
    {
        return FALSE;
    }   /* end-of-if */

    if ( MYKI_CS_TAControlGet( &pMYKI_TAControl ) < 0 || pMYKI_TAControl == NULL )
    {
        return FALSE;
    }   /* end-of-if */

    if ( MYKI_CS_TAPurseBalanceGet( &pMYKI_TAPurseBalance ) < 0 )
    {
        return FALSE;
    }

    if ( MYKI_CS_TAPurseControlGet( &pMYKI_TAPurseControl ) < 0 )
    {
        return FALSE;
    }   /* end-of-if */

    if ( MYKI_CS_CAControlGet(&pCAControl) != MYKI_CS_OK )
    {
        return FALSE;
    }

    /*  GIVEN */
    {
        /*  Card image */
        {
            pMYKI_TAPurseControl->ActionSeqNo                               = 15;
            pMYKI_TAPurseControl->NextTxSeqNo                               = NextTxSeqNo;
            pMYKI_TAPurseBalance->Balance                                   = Balance;
            pMYKI_TAControl->Directory[ 0 ].Status                          = TAPP_CONTROL_DIRECTORY_STATUS_ACTIVATED;
            pMYKI_TAControl->Status                                         = TAPP_CONTROL_STATUS_ACTIVATED;
            pCAControl->Status                                              = CARD_CONTROL_STATUS_ACTIVATED;
        }

        /*  BR context data */
        {
            pData->DynamicData.entryPointId                                 = EntryPointId;
            pData->DynamicData.lineId                                       = LineId;
            pData->DynamicData.stopId                                       = StopId;
            pData->DynamicData.loadLogTxValue                               = \
            LoadLogTxnValue                                                 = ActionValue + 45;     /*  Dynamic.LoadLogTxnValue > Actionlist.Value*/
            pData->StaticData.serviceProviderId                             = ServiceProviderId;
        }

        /*  Transaction log */
        {
            pData->InternalData.LoadLogData.transactionType                 = MYKI_BR_TRANSACTION_TYPE_LOAD_PRODUCT;
            pData->InternalData.LoadLogData.transactionValue                = LoadLogTxnValue;
        }
    }

    /*  WHEN */
    {
        /*  Actionlist record */
        {
            pData->ActionList.type                                          = ACTION_TPURSE_LOAD_DEBIT;
            pData->ActionList.actionlist.tPurseLoad.isActionSequenceNoSet   = TRUE;
            pData->ActionList.actionlist.tPurseLoad.actionSequenceNo        = 1;
            pData->ActionList.actionlist.tPurseLoad.value                   = ActionValue;
        }

        /*  Executes business rule */
        RuleResult  = BR_LLSC_2_13( pData );
    }

    /*  THEN */
    {
        UT_Start( );

        /*  Make sure business rule is executed */
        UT_Assert( RuleResult == RULE_RESULT_EXECUTED );

        /*  Make sure card image is correct */
        UT_Assert( pMYKI_TAPurseBalance->Balance == ( Balance - ActionValue ) );
        UT_Assert( pMYKI_TAPurseControl->ActionSeqNo == 1 );
        UT_Assert( pMYKI_TAPurseControl->NextTxSeqNo == ( NextTxSeqNo + 1 ) );

        /*  Make sure transaction log is correct */
        UT_Assert( pData->InternalData.LoadLogData.transactionType == MYKI_BR_TRANSACTION_TYPE_MULTIPLE_ACTION_LIST );
        UT_Assert( pData->InternalData.LoadLogData.transactionValue == ( LoadLogTxnValue - ActionValue ) );
        UT_Assert( pData->InternalData.LoadLogData.transactionSequenceNumber == NextTxSeqNo );

        /*  Make sure BR context data is correct */
        UT_Assert( pData->DynamicData.loadLogTxValue == ( LoadLogTxnValue - ActionValue ) );
        UT_Assert( pData->ReturnedData.actionApplied == TRUE );
        UT_Assert( pData->ReturnedData.topupAmount == ( - ActionValue ) );

        return  UT_Result( );
    }
}   /* test_BR_LLSC_2_13_005( ) */
