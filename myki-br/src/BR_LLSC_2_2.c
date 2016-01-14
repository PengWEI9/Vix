/* -*- mode: c; tabs: 4; -*- */
/*=============================================================================
**
**    Vix Technology                   Licensed software
**    (C) 2013                         All rights reserved
**
**=============================================================================
**
**  Project/Product : MBU
**  Filename        : BR_LLSC_2_2.c
**  Author(s)       : ???
**
**  ID              : BR_LLSC_2_2 - NTS0177 v7.2
**
**  Name            : Process Hotlist Block T-Purse
**
**  Data Fields     :
**
**      1.  Actionlist.Type 
**      2.  Actionlist.ActionSeqNo 
**      3.  TAppControl.ActionSeqNo
**
**  Pre-Conditions  :
**
**      1.  An actionlist entry exists for the card UID. 
**      2.  The actionlist type is equal to TPurseUpdate/Block. 
**      3.  The actionlist action sequence number is between 1 and 15. 
**      4.  If the transit application action sequence number is equal to 15: 
**          a.  The actionlist action sequence number is equal to 1. 
**      5.  Else:
**          a.  The actionlist action sequence number is equal to the transit
**              application action sequence number + 1.
**
**  Description     :
**
**      1.  Perform a TPurseUpdate/Block transaction.
**
**  Post-Conditions :
**
**      1.  The t-Purse is blocked.
**
**  Member(s)       :
**      BR_LLSC_2_2             [public]    business rule
**
**  Information     :
**   Compiler(s)    : C
**   Target(s)      : Independent
**
**  Subversion      :
**      $Id: $
**      $HeadURL: $
**
**  History         :
**   Vers.  Date        Aut.  Type     Description
**   -----  ----------  ----  -------  ----------------------------------------
**    1.00  ??.??.??    ???   Create
**    1.01  04.12.13    ANT   Modify   No code change. Updated comments.
**    1.02  29.10.14    ANT   Modify   Changed myki_br_GetActionlist() signature.
**
**===========================================================================*/

#include <cs.h>                 // For debug logging
#include <myki_cardservices.h>  // Myki Card Services
#include <LDT.h>                // Logical Device Transaction
#include <myki_actionlist.h>    // Actionlist
#include "myki_br_rules.h"      // API for all BR_xxx modules
#include "BR_Common.h"          // Common utilities, macros, etc

/*==========================================================================*
**
**  BR_LLSC_2_2
**
**  Description     :
**      Implements business rule BR_LLSC_2_2.
**
**  Parameters      :
**      pData               [I/O]   BR context data
**
**  Returns         :
**      RULE_RESULT_EXECUTED
**      RULE_RESULT_BYPASSED
**      RULE_RESULT_ERROR
**
**  Notes           :
**
**==========================================================================*/

RuleResult_e BR_LLSC_2_2( MYKI_BR_ContextData_t *pData )
{
    MYKI_TAControl_t           *pMYKI_TAControl         = NULL;
    MYKI_TAPurseBalance_t      *pMYKI_TAPurseBalance    = NULL;
    MYKI_TAPurseControl_t      *pMYKI_TAPurseControl    = NULL;
    CT_CardInfo_t              *pCardInfo               = NULL;
    int                         errcode                 = 0;

    CsDbg( BRLL_RULE, "BR_LLSC_2_2 : Start (Process Hotlist Block T-Purse)" );

    if ( ! pData )
    {
        CsErrx( "BR_LLSC_2_2 : Invalid argument(s)" );
        return RULE_RESULT_ERROR;
    }

    if ( MYKI_CS_TAControlGet( &pMYKI_TAControl ) < 0 )
    {
        CsErrx( "BR_LLSC_2_2 : MYKI_CS_TAControlGet() failed" );
        return RULE_RESULT_ERROR;
    }

    if ( MYKI_CS_GetCardInfo( &pCardInfo ) < 0 )
    {
        CsErrx( "BR_LLSC_2_2 : MYKI_CS_GetCardInfo() failed" );
        return RULE_RESULT_ERROR;
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

    /*  PRE-CONDITIONS */
    {
        U8_t    searchActionSequenceNumber  = ( pMYKI_TAPurseControl->ActionSeqNo == 15 ? 1 : ( pMYKI_TAPurseControl->ActionSeqNo + 1 ) );

        // populate the ActionList structure
        if ( pData->ActionList.type == ACTION_TPURSE_UPDATE_BLOCK )
        {
            if ( pData->ActionList.actionlist.tPurseUpdate.actionSequenceNo == searchActionSequenceNumber )
            {
                CsDbg( BRLL_RULE, "BR_LLSC_2_2 : found desired actionlist in cache" );
            }
            else
            {
                pData->ActionList.type  = ACTION_NULL;
            }
        }

        if ( pData->ActionList.type != ACTION_TPURSE_UPDATE_BLOCK )
        {
            if ( ( errcode = myki_br_GetActionlist( pCardInfo->SerialNumber, ACTION_TPURSE_UPDATE_BLOCK, searchActionSequenceNumber, 0 /*DONTCARE*/, pData ) ) < 0 )
            {
                CsDbg( BRLL_RULE, "BR_LLSC_2_2 : myki_br_GetActionlist returns error %d", errcode);
                return RULE_RESULT_ERROR;
            }
        }

        /*  1.  An actionlist entry exists for the card UID.
            2.  The actionlist type is equal to TPurseUpdate/Block. */
        if ( pData->ActionList.type != ACTION_TPURSE_UPDATE_BLOCK )
        {
            CsDbg( BRLL_RULE, "BR_LLSC_2_2 : Bypassed - ActionList.type is not ACTION_TPURSE_UPDATE_BLOCK." );
            pData->ReturnedData.bypassCode  = BYPASS_CODE( 2, 2, 2, 0 );
            return RULE_RESULT_BYPASSED;
        }

        /*  3.  The actionlist action sequence number is between 1 and 15. */
        if ( pData->ActionList.actionlist.tPurseUpdate.actionSequenceNo < 1 ||  pData->ActionList.actionlist.tPurseUpdate.actionSequenceNo > 15 )
        {
            CsDbg( BRLL_RULE, "BR_LLSC_2_2 : Bypassed - ActionList.actionSeqNo (%d) is out of range.",
                 pData->ActionList.actionlist.tPurseUpdate.actionSequenceNo );
            pData->ReturnedData.bypassCode  = BYPASS_CODE( 2, 2, 3, 0 );
            return RULE_RESULT_BYPASSED;
        }

        /*  4.  If the transit application action sequence number is equal to 15:
                a.  The actionlist action sequence number is equal to 1.
            5.  Else:
                a.  The actionlist action sequence number is equal to the transit
                    application action sequence number + 1. */
        /*  NOTE:   BR_LLSC_2_2 pre-conditions are incorrectly specified, actionlist
                    sequence number should be checked against TAppPurseControl.ActionSeqNo */
        CsDbg( BRLL_RULE, "BR_LLSC_2_2 : pMYKI_TAPurseControl->ActionSeqNo = %d, pData->ActionList.actionSeqNo = %d",
            pMYKI_TAPurseControl->ActionSeqNo,  pData->ActionList.actionlist.tPurseUpdate.actionSequenceNo );

        if ( pMYKI_TAPurseControl->ActionSeqNo == 15 )
        {
            if ( pData->ActionList.actionlist.tPurseUpdate.actionSequenceNo != 1 )
            {
                CsDbg( BRLL_RULE, "BR_LLSC_2_2 : Bypassed - pMYKI_TAPurseControl->ActionSeqNo = 15, ActionList.actionSeqNo != 1");
                pData->ReturnedData.bypassCode  = BYPASS_CODE( 2, 2, 4, 0 );
                return RULE_RESULT_BYPASSED;
            }
        }
        else 
        if ( pData->ActionList.actionlist.tPurseUpdate.actionSequenceNo != ( pMYKI_TAPurseControl->ActionSeqNo + 1 ) )
        {
            CsDbg( BRLL_RULE, "BR_LLSC_2_2 : Bypassed - pMYKI_TAPurseControl->ActionSeqNo + 1 != ActionList.actionSeqNo");
            pData->ReturnedData.bypassCode  = BYPASS_CODE( 2, 2, 5, 0 );
            return RULE_RESULT_BYPASSED;
        }
    }

    /*  PROCESSING */
    {
        /*  1.  Perform a TPurseUpdate/Block  transaction: */
        if ( myki_br_ldt_Actionlist( pData ) < 0 )
        {
            CsErrx( "BR_LLSC_2_2 : myki_br_ldt_Actionlist() failed" );
            return RULE_RESULT_ERROR;
        }

        /*  NOTE:   TAppPurseControl.ActionSeqNo is updated by LDT */
    }

    /*  POST-CONDITIONS */
    {
        /*  1.  The t-Purse is blocked. */
        pData->ReturnedData.actionApplied       = TRUE;
    }

    CsDbg( BRLL_RULE, "BR_LLSC_2_2 : Executed" );
    return RULE_RESULT_EXECUTED;
}

