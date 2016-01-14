/* -*- mode: c; tabs: 4; -*- */
/*=============================================================================
**
**    Vix Technology                   Licensed software
**    (C) 2013                         All rights reserved
**
**=============================================================================
**
**  Project/Product : MBU
**  Filename        : BR_LLSC_2_10.c
**  Author(s)       : ???
**
**  ID              : BR_LLSC_2_10 - NTS0177 v7.2
**
**  Name            : Check Transit Application Status
**
**  Data Fields     :
**
**      1.  TAppControl.Status
**
**  Pre-Conditions  :
**
**      1.  The transit application status is not set to Activated.
**
**  Description     :
**
**      1.  Generate a FailureResponse of type InvalidCardState.
**
**  Post-Conditions :
**
**      1.  A FailureResponse is generated indicating an invalid transit
**          application (card) state.
**
**  Member(s)       :
**      BR_LLSC_2_10            [public]    business rule
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
**    1.01  13.11.13    ANT   Add      Added "TODO" generating FailureResponse
**                                     message
**    1.02  03.12.13    ANT   Add      Generating FailureResponse messages
**    1.03  04.12.13    ANT   Modify   No code change. Updated comments.
**    1.03  12.11.15    ANT   Modify   NGBU-838: Added returning blocking reason.
**
**===========================================================================*/

#include <cs.h>                 // For debug logging
#include <myki_cardservices.h>  // Myki Card Services
#include <LDT.h>                // Logical Device Transaction

#include "myki_br_rules.h"      // API for all BR_xxx modules
#include "BR_Common.h"          // Common utilities, macros, etc

/*==========================================================================*
**
**  BR_LLSC_2_10
**
**  Description     :
**      Implements business rule BR_LLSC_2_10.
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

RuleResult_e BR_LLSC_2_10( MYKI_BR_ContextData_t *pData )
{
    MYKI_TAControl_t            *pMYKI_TAControl;

    CsDbg( BRLL_RULE, "BR_LLSC_2_10 : Start (Check Transit Application Status)" );

    if ( ! pData )
    {
        CsErrx( "BR_LLSC_2_10 : Invalid argument(s)" );
        return RULE_RESULT_ERROR;
    }

    if ( MYKI_CS_TAControlGet( &pMYKI_TAControl ) < 0 )
    {
        CsErrx( "BR_LLSC_2_10 : MYKI_CS_TAControlGet() failed" );
        return RULE_RESULT_ERROR;
    }

    /*  PRE-CONDITIONS */
    {
        /*  1.  The transit application status is not set to Activated. */
        if ( pMYKI_TAControl->Status == TAPP_CONTROL_STATUS_ACTIVATED )
        {
            CsDbg( BRLL_RULE, "BR_LLSC_2_10 : Bypass - Transit Application is Activated" );
            return RULE_RESULT_BYPASSED;
        }
    }

    /*  PROCESSING */
    {
        /*  1.  A FailureResponse is generated indicating an invalid transit application (card) state. */
        /*      TODO: MBU-564 - To be confirmed */
        if ( myki_br_ldt_ProductUsage_ScanOn_FailureResponse( pData, MYKI_UD_FAILURE_REASON_INVALID_CARD_STATE ) < 0 )
        {
            CsErrx( "BR_LLSC_2_10 : myki_br_ldt_ProductUsage_ScanOn_FailureResponse() failed" );
            return RULE_RESULT_ERROR;
        }   /* end-of-if */

        if ( pMYKI_TAControl->Status == TAPP_CONTROL_STATUS_BLOCKED )
        {
            pData->ReturnedData.rejectReason    = MYKI_BR_REJECT_REASON_APPLICATION_BLOCKED;
            pData->ReturnedData.blockingReason  = (int)pMYKI_TAControl->BlockingReason;
        }
        else
        {
            pData->ReturnedData.rejectReason    = MYKI_BR_REJECT_REASON_CARD_NOT_ACTIVATED;
        }
    }

    CsDbg( BRLL_RULE, "BR_LLSC_2_10 : Executed" );
    return RULE_RESULT_EXECUTED;
}
