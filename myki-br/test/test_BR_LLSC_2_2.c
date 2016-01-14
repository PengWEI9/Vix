/* -*- mode: c; tabs: 4; -*- */
/*=============================================================================
**
**    Vix Technology                   Licensed software
**    (C) 2013                         All rights reserved
**
**=============================================================================
**
**  Project/Product : MBU
**  Filename        : test_BR_LLSC_2_2.c
**  Author(s)       : An Tran
**
**  Description     :
**      Implements BR_LLSC_2_2 business rule unit-tests.
**
**  Function(s)     :
**      test_BR_LLSC_2_2_XXX       [Public]    unit-test functions
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
**    1.00  15.11.13    ANT   Create
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
**  test_BR_LLSC_2_2_001
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

int test_BR_LLSC_2_2_001( MYKI_BR_ContextData_t *pData )
{
    MYKI_TAControl_t       *pMYKI_TAControl         = NULL;
    MYKI_TAPurseBalance_t  *pMYKI_TAPurseBalance    = NULL;
    MYKI_TAPurseControl_t  *pMYKI_TAPurseControl    = NULL;

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
        CsErrx( "BR_LLSC_2_2 : MYKI_CS_TAPurseBalanceGet() failed" );
        return RULE_RESULT_ERROR;
    }

    if ( MYKI_CS_TAPurseControlGet( &pMYKI_TAPurseControl ) < 0 )
    {
        CsErrx( "BR_LLSC_2_2 : MYKI_CS_TAPurseControlGet() failed" );
        return RULE_RESULT_ERROR;
    }

    /*  Pre-Conditions:
            Actionlist.Type != ACTION_TPURSE_UPDATE_BLOCK

        Post-Conditions:
            BYPASSED - Actionlist.Type != ACTION_TPURSE_UPDATE_BLOCK
    */
    {
		memset(&pData->ActionList, 0, sizeof(pData->ActionList));
        pData->ActionList.type                              = 0;
        pData->ActionList.actionlist.tPurseUpdate.actionSequenceNo                       = 0;
       // pData->ActionList.actionlist.tPurseUpdate.productSerialNo                   = 0;
        if ( BR_LLSC_2_2( pData ) != RULE_RESULT_BYPASSED )
        {
            return FALSE;
        }   /* end-of-if */
    }

    /*  Pre-Conditions:
            Actionlist.Type = ACTION_TPURSE_UPDATE_BLOCK
            Actionlist.ActionSeqNo < 1

        Post-Conditions:
            BYPASSED - Actionlist.ActionSeqNo < 1
    */
    {
		memset(&pData->ActionList, 0, sizeof(pData->ActionList));
        pData->ActionList.type                              = ACTION_TPURSE_UPDATE_BLOCK;
        pData->ActionList.actionlist.tPurseUpdate.actionSequenceNo                  = 0;
        //pData->ActionList.actionlist.tPurseUpdate.productSerialNo                   = 0;
        if ( BR_LLSC_2_2( pData ) != RULE_RESULT_BYPASSED )
        {
            return FALSE;
        }   /* end-of-if */
    }

    /*  Pre-Conditions:
            Actionlist.Type = ACTION_TPURSE_UPDATE_BLOCK
            Actionlist.ActionSeqNo > 15

        Post-Conditions:
            BYPASSED - Actionlist.ActionSeqNo > 15
    */
    {
		memset(&pData->ActionList, 0, sizeof(pData->ActionList));
        pData->ActionList.type                              = ACTION_TPURSE_UPDATE_BLOCK;
        pData->ActionList.actionlist.tPurseUpdate.actionSequenceNo                  = 16;
        //pData->ActionList.actionlist.tPurseUpdate.productSerialNo                   = 0;
        if ( BR_LLSC_2_2( pData ) != RULE_RESULT_BYPASSED )
        {
            return FALSE;
        }   /* end-of-if */
    }

    /*  Pre-Conditions:
            Actionlist.Type = ACTION_TPURSE_UPDATE_BLOCK
            Actionlist.ActionSeqNo = 2 (ie. != 1)
            TAppProduct.ActionSeqNo = 15

        Post-Conditions:
            BYPASSED - Wrong Actionlist.ActionSeqNo
    */
   {
		memset(&pData->ActionList, 0, sizeof(pData->ActionList));
       pData->ActionList.type                              = ACTION_TPURSE_UPDATE_BLOCK;
       pData->ActionList.actionlist.tPurseUpdate.actionSequenceNo                       = 2;
       ///pData->ActionList.actionlist.tPurseUpdate.productSerialNo                   = nProductSerialNo;
       pMYKI_TAControl->Directory[ 0 ].Status              = TAPP_CONTROL_DIRECTORY_STATUS_ACTIVATED;
       pMYKI_TAPurseControl->ActionSeqNo                   = 15;
       if ( BR_LLSC_2_2( pData ) != RULE_RESULT_BYPASSED )
       {
           return FALSE;
       }   /* end-of-if */
   }

    /*  Pre-Conditions:
            Actionlist.Type = ACTION_TPURSE_UPDATE_BLOCK
            Actionlist.ActionSeqNo = 14 (!= 13)
            TAppProduct.ActionSeqNo = 12

        Post-Conditions:
            BYPASSED - Wrong Actionlist.ActionSeqNo
    */
    {
		memset(&pData->ActionList, 0, sizeof(pData->ActionList));
        pData->ActionList.type                              = ACTION_TPURSE_UPDATE_BLOCK;
        pData->ActionList.actionlist.tPurseUpdate.actionSequenceNo                       = 14;
        //pData->ActionList.actionlist.tPurseUpdate.productSerialNo                   = nProductSerialNo;
        pMYKI_TAControl->Directory[ 0 ].Status              = TAPP_CONTROL_DIRECTORY_STATUS_ACTIVATED;
        pMYKI_TAPurseControl->ActionSeqNo                   = 12;
        if ( BR_LLSC_2_2( pData ) != RULE_RESULT_BYPASSED )
        {
            return FALSE;
        }   /* end-of-if */
    }

    return TRUE;
}   /* test_BR_LLSC_2_2_001( ) */

/*==========================================================================*
**
**  test_BR_LLSC_2_2_002
**
**  Description     :
**      Unit-test EXECUTED conditions, disable auto-load.
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

int test_BR_LLSC_2_2_002( MYKI_BR_ContextData_t *pData )
{
    MYKI_TAControl_t       *pMYKI_TAControl         = NULL;
    MYKI_TAPurseBalance_t  *pMYKI_TAPurseBalance    = NULL;
    MYKI_TAPurseControl_t  *pMYKI_TAPurseControl    = NULL;
    MYKI_CAControl_t       *pMYKI_CAControl         = NULL;
    int                     AlServiceProvider       = 51;
    int                     EntryPointId            = 53;
    int                     LineId                  = 66;
    int                     StopId                  = 79;
    int                     ServiceProviderId       = 100;

    if ( pData == NULL )
    {
        return FALSE;
    }   /* end-of-if */

    if ( MYKI_CS_TAControlGet( &pMYKI_TAControl ) < 0 || pMYKI_TAControl == NULL )
    {
        return FALSE;
    }   /* end-of-if */

    if ( MYKI_CS_CAControlGet( &pMYKI_CAControl ) != MYKI_CS_OK )
    {
        return FALSE;
    }

    if ( MYKI_CS_TAPurseBalanceGet( &pMYKI_TAPurseBalance ) < 0 )
    {
        CsErrx( "BR_LLSC_2_2 : MYKI_CS_TAPurseBalanceGet() failed" );
        return RULE_RESULT_ERROR;
    }

    if ( MYKI_CS_TAPurseControlGet( &pMYKI_TAPurseControl ) < 0 )
    {
        CsErrx( "BR_LLSC_2_2 : MYKI_CS_TAPurseControlGet() failed" );
        return RULE_RESULT_ERROR;
    }

    /*  Pre-Conditions:
            Actionlist.Type = ACTION_TPURSE_UPDATE_BLOCK
            Actionlist.ActionSeqNo = 13
            TAppProduct.ActionSeqNo = 12

        Post-Conditions:
            Purse blocked
    */
   {
		memset(&pData->ActionList, 0, sizeof(pData->ActionList));
       pData->ActionList.type                              = ACTION_TPURSE_UPDATE_BLOCK;
       pData->ActionList.actionlist.tPurseUpdate.actionSequenceNo                       = 13;
       pData->ActionList.actionlist.tPurseUpdate.serviceProviderId                 = AlServiceProvider;
       pData->ActionList.actionlist.tPurseUpdate.isAutoload                        = FALSE;
       pMYKI_TAPurseControl->ActionSeqNo                   = 12;
       pMYKI_TAControl->Directory[ 0 ].Status              = TAPP_CONTROL_DIRECTORY_STATUS_ACTIVATED;
       pMYKI_TAControl->Status                             = TAPP_CONTROL_STATUS_ACTIVATED;
       pMYKI_CAControl->Status                             = CARD_CONTROL_STATUS_ACTIVATED;
       pData->DynamicData.entryPointId                     = EntryPointId;
       pData->DynamicData.lineId                           = LineId;
       pData->DynamicData.stopId                           = StopId;
       pData->StaticData.serviceProviderId                 = ServiceProviderId;
       if ( BR_LLSC_2_2( pData ) != RULE_RESULT_EXECUTED )
       {
           return FALSE;
       }   /* end-of-if */
   }

    return TRUE;
}   /* test_BR_LLSC_2_2_002( ) */
