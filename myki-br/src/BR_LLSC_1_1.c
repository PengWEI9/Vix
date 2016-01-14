/**************************************************************************
 *  ID :  BR_LLSC_1_1
 *
 *  Multiple Card Presentation
 *  None
 *  Multiple smartcards are presented to the device.
 *     Description
 *  Reject all smartcards presented.
 *     Post-Conditions
 *  Only one smartcard may be presented to the device.
 *     Devices
 *  All devices
 *
 *  3.1.3 Capping Counter Assessment
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

RuleResult_e BR_LLSC_1_1( MYKI_BR_ContextData_t *pData )
{
    CsDbg( BRLL_RULE, "BR_LLSC_1_1 : Start (Multiple Card Presentation)" );

    if ( ! pData )
    {
        CsErrx( "BR_LLSC_1_1 : Invalid argument(s)" );
        return RULE_RESULT_ERROR;
    }

    //
    //  Check Pre-conditions
    //

    if ( pData->DynamicData.numberOfCardsPresented == 1 )      // Bypass if a single card is presented
    {
        CsDbg( BRLL_RULE, "BR_LLSC_1_1 : Bypass - pData->DynamicData.numberOfCardsPresented == 1" );
        return RULE_RESULT_BYPASSED;
    }

    pData->ReturnedData.rejectReason = MYKI_BR_REJECT_REASON_MULTIPLE_CARDS;
    CsDbg( BRLL_RULE, "BR_LLSC_1_1 : Executed" );
    return RULE_RESULT_EXECUTED;
}

