/**************************************************************************
*
*   ID :  BR_VIX_0_5
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
//  Executes if the device is an EntryExit (platform) device
//  ie, isEntry = TRUE and isExit = TRUE
//
//=============================================================================

RuleResult_e BR_VIX_0_5( MYKI_BR_ContextData_t *pData )
{
    CsDbg( BRLL_RULE, "BR_VIX_0_5 : Start (Check Entry/Exit (platform))" );

    if ( ! pData )
    {
        CsErrx( "BR_VIX_0_5 : Invalid argument(s)" );
        return RULE_RESULT_ERROR;
    }

    if ( ! pData->StaticData.isEntry )
    {
        CsDbg( BRLL_RULE, "BR_VIX_0_5 : Bypass ( ! pData->StaticData.isEntry )" );
        return RULE_RESULT_BYPASSED;
    }

    if ( ! pData->StaticData.isExit )
    {
        CsDbg( BRLL_RULE, "BR_VIX_0_5 : Bypass ( ! pData->StaticData.isExit )" );
        return RULE_RESULT_BYPASSED;
    }

    CsDbg( BRLL_RULE, "BR_VIX_0_5 : Executed" );
    return RULE_RESULT_EXECUTED;
}

