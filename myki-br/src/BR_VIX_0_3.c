/**************************************************************************
*
*   ID :  BR_VIX_0_3
*
*
 ***********************************************************************/

#include <cs.h>                 // For debug logging
#include <myki_cardservices.h>  // Myki Card Services
#include <LDT.h>                // Logical Device Transaction

#include "myki_br_rules.h"      // API for all BR_xxx modules
#include "BR_Common.h"          // Common utilities, macros, etc

//=============================================================================
//
//  Executes if the card is an LLSC card
//
//=============================================================================

RuleResult_e BR_VIX_0_3( MYKI_BR_ContextData_t *pData )
{
    CsDbg( BRLL_RULE, "BR_VIX_0_3 : Start (Check for LLSC)" );

    if ( ! pData )
    {
        CsErrx( "BR_VIX_0_3 : Invalid argument(s)" );
        return RULE_RESULT_ERROR;
    }

    if ( ! pData->DynamicData.isTransit )
    {
        CsDbg( BRLL_RULE, "BR_VIX_0_3 : Bypass ( ! pData->DynamicData.isTransit )" );
        return RULE_RESULT_BYPASSED;
    }

    CsDbg( BRLL_RULE, "BR_VIX_0_3 : Executed" );
    return RULE_RESULT_EXECUTED;
}

