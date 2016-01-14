/**************************************************************************
*
*   ID :  BR_VIX_0_1
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
//  Executes if the device is an Entry-Only device
//  ie, isEntry = TRUE and isExit = FALSE
//
//=============================================================================

RuleResult_e BR_VIX_0_1( MYKI_BR_ContextData_t *pData )
{
    CsDbg( BRLL_RULE, "BR_VIX_0_1 : Start (Check Entry-Only)" );

    if ( ! pData )
    {
        CsErrx( "BR_VIX_0_1 : Invalid argument(s)" );
        return RULE_RESULT_ERROR;
    }

    if ( ! pData->StaticData.isEntry )
    {
        CsDbg( BRLL_RULE, "BR_VIX_0_1 : Bypass ( ! pData->StaticData.isEntry )" );
        return RULE_RESULT_BYPASSED;
    }

    if ( pData->StaticData.isExit )
    {
        CsDbg( BRLL_RULE, "BR_VIX_0_1 : Bypass ( pData->StaticData.isExit )" );
        return RULE_RESULT_BYPASSED;
    }

    CsDbg( BRLL_RULE, "BR_VIX_0_1 : Executed" );
    return RULE_RESULT_EXECUTED;
}

