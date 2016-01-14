/**************************************************************************
*   ID :  BR_LLSC_0_A
*
*   Belongs to SEQ_LLSC_0A: Card Transaction Initialization
*
*   CardIssuer.OperationMode = OperationMode.TestMode
*
 ***********************************************************************/

#include <cs.h>                 // For debug logging
#include <myki_cardservices.h>  // Myki Card Services
#include <LDT.h>                // Logical Device Transaction

#include <myki_br_card_constants.h>
#include "myki_br_rules.h"      // API for all BR_xxx modules
#include "BR_Common.h"          // Common utilities, macros, etc

//=============================================================================
//
//
//
//=============================================================================

RuleResult_e BR_LLSC_0_A( MYKI_BR_ContextData_t *pData )
{
    MYKI_CAIssuer_t            *pMYKI_CAIssuer  = NULL;
	int                         result          = 0;

    CsDbg( BRLL_RULE, "BR_LLSC_0_A : Start (Check for Test Mode)" );

    if ( ! pData )
    {
        CsErrx( "BR_LLSC_0_A : Invalid argument(s)" );
        return RULE_RESULT_ERROR;
    }

    if ( ( result = MYKI_CS_CAIssuerGet( &pMYKI_CAIssuer ) ) < 0 )
    {
        CsErrx( "BR_LLSC_0_A : MYKI_CS_CAIssuerGet() failed result: %d", result );
		if ( result == CRYPTO_AUTH_FAIL_ERR )
			pData->ReturnedData.rejectReason = MYKI_BR_REJECT_REASON_CRYPTO_AUTH_FAIL;
		else
			pData->ReturnedData.rejectReason = MYKI_BR_REJECT_REASON_UNREADABLE;
        return RULE_RESULT_ERROR;
    }

    //
    //  Check Pre-conditions
    //

    if ( ! ( pMYKI_CAIssuer->OperationMode & OPERATION_MODE_TEST ) )
    {
        CsDbg(BRLL_RULE, "BR_LLSC_0_A : Bypass - ! ( pMYKI_CAIssuer->OperationMode & OPERATION_MODE_TEST )" );
        return RULE_RESULT_BYPASSED;
    }

    //
    //  Execute Rule
    //

    pData->ReturnedData.isTestMode = TRUE;  // Tell application it should operate in test mode with this card

    CsDbg( BRLL_RULE, "BR_LLSC_0_A : Executed" );
    return RULE_RESULT_EXECUTED;
}

