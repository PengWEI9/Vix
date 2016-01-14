/* -*- mode: c; tabs: 4; -*- */
/*=============================================================================
**
**    Vix Technology                   Licensed software
**    (C) 2013                         All rights reserved
**
**=============================================================================
**
**  Project/Product : MBU
**  Filename        : BR_LLSC_2_9.c
**  Author(s)       : ???
**
**  ID              : BR_LLSC_2_9 - NTS0177 v7.2
**
**  Name            : Process Actionlist Update Transit Application
**
**  Data Fields     :
**
**      1.  Actionlist.Type
**      2.  Actionlist.ActionSeqNo
**      3.  TAppControl.Status
**      4.  TAppControl.ActionSeqNo
**      5.  Dynamic.LoadLog
**      6.  ActionList.TAppControl.PassengerCode
**
**  Pre-Conditions  :
**
**      1.  An actionlist entry exists for the card UID.
**      2.  The actionlist type is equal to TAppUpdate/None.
**      3.  The actionlist action sequence number is between 1 and 15.
**      4.  If the transit application action sequence number is equal to 15.
**          a.  The actionlist action sequence number is equal to 1.
**      5.  Else:
**          a.  The actionlist action sequence number is equal to the transit
**              application action sequence number + 1.
**      6.  The transit application status is equal to Activated.
**
**  Description     :
**
**      1.  Perform a TAppUpdate/None transaction.
**
**  Post-Conditions :
**
**      1.  The transit application has been updated.
**
**  Member(s)       :
**      BR_LLSC_2_9             [public]    business rule
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
**    1.01  15.11.13    ANT   Modify   Removed updating
**                                     TAppControl.ActionSeqNo as done by LDT
**    1.02  04.12.13    ANT   Modify   No code change. Updated comments.
**    1.03  29.10.14    ANT   Modify   Changed myki_br_GetActionlist() signature.
**
**===========================================================================*/

#include <cs.h>                 // For debug logging
#include <myki_cardservices.h>  // Myki Card Services
#include <LDT.h>                // Logical Device Transaction

#include "myki_br_rules.h"      // API for all BR_xxx modules
#include "BR_Common.h"          // Common utilities, macros, etc

/*==========================================================================*
**
**  BR_LLSC_2_9
**
**  Description     :
**      Implements business rule BR_LLSC_2_9.
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

RuleResult_e BR_LLSC_2_9( MYKI_BR_ContextData_t *pData )
{
    MYKI_TAControl_t    *pMYKI_TAControl;
    CT_CardInfo_t       *pCardInfo;
    int                  errcode;

    CsDbg( BRLL_RULE, "BR_LLSC_2_9 : Start (Process Actionlist Update Transit Application)" );

    if ( ! pData )
    {
        CsErrx( "BR_LLSC_2_9 : Invalid argument(s)" );
        return RULE_RESULT_ERROR;
    }
    
    if ( MYKI_CS_TAControlGet( &pMYKI_TAControl ) < 0 )
    {
        CsErrx( "BR_LLSC_2_9 : MYKI_CS_TAControlGet() failed" );
        return RULE_RESULT_ERROR;
    }
        
    if ( MYKI_CS_GetCardInfo( &pCardInfo ) < 0 )
    {
        CsErrx( "BR_LLSC_2_9 : MYKI_CS_GetCardInfo() failed" );
        return RULE_RESULT_ERROR;
    }

    /*  PRE-CONDITIONS */
    {
        U8_t    searchActionSequenceNumber  = ( pMYKI_TAControl->ActionSeqNo == 15 ? 1 : ( pMYKI_TAControl->ActionSeqNo + 1 ) );

        // populate the ActionList structure
        if ( pData->ActionList.type == ACTION_TAPP_UPDATE_NONE )
        {
            if ( pData->ActionList.actionlist.tAppUpdate.actionSequenceNo == searchActionSequenceNumber )
            {
                CsDbg( BRLL_RULE, "BR_LLSC_2_9 : found desired actionlist in cache" );
            }
            else
            {
                pData->ActionList.type  = ACTION_NULL;
            }
        }

        if ( pData->ActionList.type != ACTION_TAPP_UPDATE_NONE )
        {
            if ( ( errcode = myki_br_GetActionlist( pCardInfo->SerialNumber, ACTION_TAPP_UPDATE_NONE, searchActionSequenceNumber, 0 /*DONTCARE*/, pData ) ) < 0 )
            {
                CsDbg( BRLL_RULE, "BR_LLSC_2_9 : myki_br_GetActionlist returns error %d", errcode);
                return RULE_RESULT_ERROR;
            }
        }

        /*  1.  An actionlist entry exists for the card UID.
            2.  The actionlist type is equal to TAppUpdate/None. */
        if ( pData->ActionList.type != ACTION_TAPP_UPDATE_NONE )
        {
            CsDbg( BRLL_RULE, "BR_LLSC_2_9 : Bypassed - ActionList.type is not ACTION_TAPP_UPDATE_NONE." );
            pData->ReturnedData.bypassCode  = BYPASS_CODE( 2, 9, 2, 0 );
            return RULE_RESULT_BYPASSED;
        }

        /*  3.  The actionlist action sequence number is between 1 and 15. */
        if ( pData->ActionList.actionlist.tAppUpdate.actionSequenceNo < 1 || pData->ActionList.actionlist.tAppUpdate.actionSequenceNo > 15 )
        {
            CsDbg( BRLL_RULE, "BR_LLSC_2_9 : Bypassed - ActionList.actionSeqNo (%d) is out of range.",
                pData->ActionList.actionlist.tAppUpdate.actionSequenceNo );
            pData->ReturnedData.bypassCode  = BYPASS_CODE( 2, 9, 3, 0 );
            return RULE_RESULT_BYPASSED;
        }

        /*  4.  If the transit application action sequence number is equal to 15.
                a.  The actionlist action sequence number is equal to 1.
            5.  Else:
                a.  The actionlist action sequence number is equal to the transit
                    application action sequence number + 1. */
        CsDbg( BRLL_RULE, "BR_LLSC_2_9 : pMYKI_TAControl->ActionSeqNo = %d, ActionList.actionSeqNo = %d",
            pMYKI_TAControl->ActionSeqNo, pData->ActionList.actionlist.tAppUpdate.actionSequenceNo );

        if ( pMYKI_TAControl->ActionSeqNo == 15 )
        {
            if ( pData->ActionList.actionlist.tAppUpdate.actionSequenceNo != 1 )
            {
                CsDbg( BRLL_RULE, "BR_LLSC_2_9 : Bypassed - pMYKI_TAControl->ActionSeqNo = 15, ActionList.actionSeqNo != 1");
                pData->ReturnedData.bypassCode  = BYPASS_CODE( 2, 9, 4, 0 );
                return RULE_RESULT_BYPASSED;
            }
        }
        else 
        if ( pData->ActionList.actionlist.tAppUpdate.actionSequenceNo != ( pMYKI_TAControl->ActionSeqNo + 1 ) )
        {
            CsDbg( BRLL_RULE, "BR_LLSC_2_9 : Bypassed - pMYKI_TAControl->ActionSeqNo + 1 != ActionList.actionSeqNo");
            pData->ReturnedData.bypassCode  = BYPASS_CODE( 2, 9, 5, 0 );
            return RULE_RESULT_BYPASSED;
        }

        /*  6.  The transit application status3 is equal to Activated. */
        if ( pMYKI_TAControl->Status != TAPP_CONTROL_STATUS_ACTIVATED )
        {
            CsDbg( BRLL_RULE, "BR_LLSC_2_9 : Bypassed - TApplication status is not activated.");
            pData->ReturnedData.bypassCode  = BYPASS_CODE( 2, 9, 6, 0 );
            return RULE_RESULT_BYPASSED;
        }
    }

    /*  PROCESSING */
    {
        /*  1.  Perform a TAppUpdate/None transaction. */
        if ( myki_br_ldt_Actionlist( pData ) < 0 )
        {
            CsErrx( "BR_LLSC_2_9 : myki_br_ldt_Actionlist() failed" );
            return RULE_RESULT_ERROR;
        }

        /*  NOTE:   TAppControl.ActionSeqNo is updated by LDT */
    }

    /*  POST-CONDITIONS */
    {
        /*  1.  The transit application has been updated. */
        pData->ReturnedData.actionApplied       = TRUE;
    }

    CsDbg( BRLL_RULE, "BR_LLSC_2_9 : Executed" );
    
    return RULE_RESULT_EXECUTED;

}

