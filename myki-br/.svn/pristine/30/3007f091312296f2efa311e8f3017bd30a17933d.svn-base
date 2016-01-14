/**************************************************************************
*
*   ID :  BR_VIX_0_4
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
//  Executes if the card is an OSC card
//
//=============================================================================

RuleResult_e BR_VIX_0_4( MYKI_BR_ContextData_t *pData )
{
    CsDbg( BRLL_RULE, "BR_VIX_0_4 : Start (Check for OSC)" );

    if ( ! pData )
    {
        CsErrx( "BR_VIX_0_4 : Invalid argument(s)" );
        return RULE_RESULT_ERROR;
    }

    if ( ! pData->DynamicData.isOperator )
    {
        CsDbg( BRLL_RULE, "BR_VIX_0_4 : Bypass ( ! pData->DynamicData.isOperator )" );
        return RULE_RESULT_BYPASSED;
    }

    CsDbg( BRLL_RULE, "BR_VIX_0_4 : Executed" );
    return RULE_RESULT_EXECUTED;
}

