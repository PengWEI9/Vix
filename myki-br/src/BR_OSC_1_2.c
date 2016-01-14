/* -*- mode: c; tabs: 4; -*- */
/*=============================================================================
**
**    Vix Technology                   Licensed software
**    (C) 2013                         All rights reserved
**
**=============================================================================
**
**  Project/Product : MBU
**  Filename        : BR_OSC_1_2.c
**  Author(s)       : ???
**
**  ID              : BR_OSC_1_2 - NTS0177 v7.2
**
**  Name            : Process Hotlist Block Operator Application
**
**  Data Fields     :
**
**      1.  Actionlist.Type
**      2.  Actionlist.ActionSeqNo
**      3.  Actionlist.BlockingReason
**      4.  OAppControl.ActionSeqNo
**      5.  OAppControl.BlockingReason
**
**  Pre-Conditions  :
**
**      1.  An actionlist entry exists for the card UID.
**      2.  The actionlist type(1) is equal to OAppUpdate/Block.
**      3.  The actionlist action sequence number(2) is between 1 and 15.
**      4.  If the transit application action sequence number(4) is equal to 15:
**          a.  The actionlist action sequence number(2) is equal to 1.
**      5.  Else:
**          a.  The actionlist action sequence number 2 is equal to the operator
**              application action sequence number(4) + 1.
** 
**  Description     :
**
**      1.  Perform a OAppUpdate/Block transaction:
**          a.  Set the operator application blocking reason(5) to the blocking
**              reason specified in the actionlist(3).
**
**  Post-Conditions :
**
**      1.  The operator application is blocked.
**
**  Devices         :
**
**      All devices
**
**  Member(s)       :
**      BR_OSC_1_2              [public]    business rule
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
**  BR_OSC_1_2
**
**  Description     :
**      Implements business rule BR_OSC_1_2.
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

RuleResult_e BR_OSC_1_2( MYKI_BR_ContextData_t *pData )
{
    MYKI_OAControl_t            *pMYKI_OAControl;  
    CT_CardInfo_t               *pCardInfo;
    int                          errcode;

    CsDbg( BRLL_RULE, "BR_OSC_1_2 : Start (Process Hotlist Block Operator Application)" );

    if ( ! pData )
    {
        CsErrx( "BR_OSC_1_2 : Invalid argument(s)" );
        return RULE_RESULT_ERROR;
    }

    if ( MYKI_CS_OAControlGet( &pMYKI_OAControl ) < 0 )
    {
        CsErrx( "BR_OSC_1_2 : MYKI_CS_OAControlGet() failed" );
        return RULE_RESULT_ERROR;
    }

    if ( MYKI_CS_GetCardInfo( &pCardInfo ) < 0 )
    {
        CsErrx( "BR_OSC_1_2 : MYKI_CS_GetCardInfo() failed" );
        return RULE_RESULT_ERROR;
    }

    //  PRE-CONDITIONS
    {
        U8_t    searchActionSequenceNumber  = ( pMYKI_OAControl->ActionSeqNo == 15 ? 1 : ( pMYKI_OAControl->ActionSeqNo + 1 ) );

        // populate the ActionList structure
        if ( pData->ActionList.type != ACTION_OAPP_UPDATE_BLOCK )
        {
            if ( ( errcode = myki_br_GetActionlist( pCardInfo->SerialNumber, ACTION_OAPP_UPDATE_BLOCK, searchActionSequenceNumber, 0 /*DONTCARE*/, pData ) ) < 0 )
            {
                CsDbg( BRLL_RULE, "BR_OSC_1_2 : myki_br_GetActionlist returns error %d", errcode);
                return RULE_RESULT_ERROR;
            }
        }
        
        if ( pData->ActionList.type != ACTION_OAPP_UPDATE_BLOCK )
        {
            CsDbg( BRLL_RULE, "BR_OSC_1_2 : Bypassed - ActionList.type is not ACTION_OAPP_UPDATE_BLOCK." );
            return RULE_RESULT_BYPASSED;
        }

        if ( pData->ActionList.actionlist.OAppUpdate.actionSequenceNo < 1 || pData->ActionList.actionlist.OAppUpdate.actionSequenceNo > 15)
        {
            CsDbg( BRLL_RULE, "BR_OSC_1_2 : Bypassed - ActionList.actionSeqNo (%d) is out of range.", pData->ActionList.actionlist.OAppUpdate.actionSequenceNo );
            return RULE_RESULT_BYPASSED;
        }

        if ( pMYKI_OAControl->ActionSeqNo == 15 )
        {
            if ( pData->ActionList.actionlist.OAppUpdate.actionSequenceNo != 1 )
            {
                CsDbg( BRLL_RULE, "BR_OSC_1_2 : Bypassed - TA ActionList.actionSeqNo is 15 but ActionList.actionSeqNo is not 1" );
                return RULE_RESULT_BYPASSED;
            }
        }
        else 
            if ( pData->ActionList.actionlist.OAppUpdate.actionSequenceNo != ( pMYKI_OAControl->ActionSeqNo + 1 ) )
        {
            CsDbg( BRLL_RULE, "BR_OSC_1_2 : Bypassed - TA ActionList.actionSeqNo is not 15 and ActionList.actionSeqNo != Operator Action SeqNo+1" );
            return RULE_RESULT_BYPASSED;
        }
    }

    // Perform a OAppUpdate/Block transaction:
    if ( myki_br_ldt_Actionlist( pData ) < 0 )
    {
        CsErrx( "BR_OSC_1_2 : myki_br_ldt_Actionlist() failed" );
        return RULE_RESULT_ERROR;
    }

    CsDbg( BRLL_RULE, "BR_OSC_1_2 : Executed" );
    return RULE_RESULT_EXECUTED;
}   /*  BR_OSC_1_2( ) */
