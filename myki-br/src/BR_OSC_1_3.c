/**************************************************************************
*   ID :  OSC_1_3
*    Check OSC Application Status
* 
*    Pre-Conditions
*    1. The operator control status1 is ‘Activated’
*
**************************************************************************/

#include <cs.h>                 // For debug logging
#include <myki_cardservices.h>  // Myki Card Services
#include <LDT.h>                // Logical Device Transaction

#include "myki_br_rules.h"      // API for all BR_xxx modules
#include "BR_Common.h"          // Common utilities, macros, etc

//=============================================================================
//
//
//
//=============================================================================

RuleResult_e BR_OSC_1_3( MYKI_BR_ContextData_t *pData )
{
    MYKI_OAControl_t            *pMYKI_OAControl;  

    CsDbg( BRLL_RULE, "BR_OSC_1_3 : Start (Check OSC Application Status)" );

    if ( ! pData )
    {
        CsErrx( "BR_OSC_1_3 : Invalid argument(s)" );
        return RULE_RESULT_ERROR;
    }

    if ( MYKI_CS_OAControlGet( &pMYKI_OAControl ) < 0 )
    {
        CsErrx( "BR_OSC_1_3 : MYKI_CS_OAControlGet() failed" );
        return RULE_RESULT_ERROR;
    }

	if ( pMYKI_OAControl->Status == OAPP_CONTROL_STATUS_BLOCKED )
	{
        CsDbg( BRLL_RULE, "BR_OSC_1_3 : Bypassed - OApplication status is blocked.");
		pData->ReturnedData.rejectReason    = MYKI_BR_REJECT_REASON_APPLICATION_BLOCKED;
        pData->ReturnedData.blockingReason  = (int)pMYKI_OAControl->BlockingReason;
        return RULE_RESULT_BYPASSED;	
	}
    else if ( pMYKI_OAControl->Status != OAPP_CONTROL_STATUS_ACTIVATED )
    {
        CsDbg( BRLL_RULE, "BR_OSC_1_3 : Bypassed - OApplication status is not activated.");
		pData->ReturnedData.rejectReason = MYKI_BR_REJECT_REASON_CARD_NOT_ACTIVATED; // Reject reason should possibly be application not activated.
        return RULE_RESULT_BYPASSED;
    }

    CsDbg( BRLL_RULE, "BR_OSC_1_3 : After Sprint 4 - returning Executed" );
    return RULE_RESULT_EXECUTED;
}
