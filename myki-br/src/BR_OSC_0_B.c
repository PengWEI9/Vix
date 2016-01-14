/**************************************************************************
*   ID :  BR_OSC_0_B
*
*    Belongs to SEQ_OSC_0A: Card Transaction Initialization
*
*   CardControl.Checksum = calculated checksum
*   CardIssuer.Certificate = generated certificate
*   CardControl.ExpiryDate >= Dynamic.CurrentDate
*   CardControl.Status = Activated
*   OAppControl.Checksum = calculated checksum
*   OAppIssuer.Certificate = generated certificate
*   OAppControl.ExpiryDate >= Dynamic.CurrentDate
*   OAppControl.Status = Initalised OR Issued OR Activated  Or Blocked
*
*
 ***********************************************************************/

#include <cs.h>                 // For debug logging
#include <myki_cardservices.h>  // Myki Card Services
#include <LDT.h>                // Logical Device Transaction
#include <myki_cdd_enums.h>     // Card data definitions

#include "myki_br_rules.h"      // API for all BR_xxx modules
#include "BR_Common.h"          // Common utilities, macros, etc

//=============================================================================
//
//
//
//=============================================================================

RuleResult_e BR_OSC_0_B( MYKI_BR_ContextData_t *pData )
{
    MYKI_CAIssuer_t             *pMYKI_CAIssuer;
    MYKI_CAControl_t            *pMYKI_CAControl;
    MYKI_OAIssuer_t             *pMYKI_OAIssuer;
    MYKI_OAControl_t            *pMYKI_OAControl;

    CsDbg( BRLL_RULE, "BR_OSC_0_B : Start (Initial Card Sanity Check)" );

    if ( ! pData )
    {
        CsErrx( "BR_OSC_0_B : Invalid argument(s)" );
        return RULE_RESULT_ERROR;
    }

    if ( MYKI_CS_CAIssuerGet( &pMYKI_CAIssuer ) < 0 )
    {
        CsErrx( "BR_OSC_0_B : MYKI_CS_CAIssuerGet() failed" );
        return RULE_RESULT_ERROR;
    }

    if ( MYKI_CS_CAControlGet( &pMYKI_CAControl ) < 0 )
    {
        CsErrx( "BR_OSC_0_B : MYKI_CS_CAControlGet() failed" );
        return RULE_RESULT_ERROR;
    }

    if ( MYKI_CS_OAIssuerGet( &pMYKI_OAIssuer ) < 0 )
    {
        CsErrx( "BR_OSC_0_B : MYKI_CS_OAIssuerGet() failed" );
        return RULE_RESULT_ERROR;
    }

    if ( MYKI_CS_OAControlGet( &pMYKI_OAControl ) < 0 )
    {
        CsErrx( "BR_OSC_0_B : MYKI_CS_OAControlGet() failed" );
        return RULE_RESULT_ERROR;
    }

    //
    //  Check Pre-conditions
    //

/*  Handled by Myki Card Services
    if ( pMYKI_CAControl->Checksum != 0 ) // pMYKI_CAControl->CalculatedChecksum )
    {
        CsDbg( BRLL_RULE, "BR_OSC_0_B : Bypass - pMYKI_CAControl->Checksum (0x%04x) != 0 ) // pMYKI_CAControl->CalculatedChecksum", pMYKI_CAControl->Checksum );
        return RULE_RESULT_BYPASSED;
    }

    if ( pMYKI_CAIssuer->Certificate != pMYKI_CAIssuer->GeneratedCertificate )
    {
        CsDbg( BRLL_RULE, "BR_OSC_0_B : Bypass - pMYKI_CAIssuer->Certificate != pMYKI_CAIssuer->GeneratedCertificate" );
        return RULE_RESULT_BYPASSED;
    }
*/

    if ( pMYKI_CAControl->ExpiryDate < pData->DynamicData.currentBusinessDate )        // KWS: Different types!
    {
        CsDbg( BRLL_RULE, "BR_OSC_0_B : Bypass - pMYKI_CAControl->ExpiryDate (%d) < pData->DynamicData.currentBusinessDate (%d)", pMYKI_CAControl->ExpiryDate, pData->DynamicData.currentBusinessDate );
        pData->ReturnedData.rejectReason = MYKI_BR_REJECT_REASON_EXPIRED;
        pData->ReturnedData.cardExpiryDate = pMYKI_CAControl->ExpiryDate;
        return RULE_RESULT_BYPASSED;
    }
	if ( pMYKI_CAControl->Status == CARD_CONTROL_STATUS_BLOCKED )
    {
        CsDbg( BRLL_RULE, "BR_OSC_0_B : Bypass - pMYKI_CAControl->Status (%d) == CARD_CONTROL_STATUS_BLOCKED (%d)", pMYKI_CAControl->Status, CARD_CONTROL_STATUS_BLOCKED );
        pData->ReturnedData.rejectReason    = MYKI_BR_REJECT_REASON_CARD_BLOCKED;
        pData->ReturnedData.blockingReason  = (int)pMYKI_CAControl->BlockingReason;
        return RULE_RESULT_BYPASSED;
    }
    else if ( pMYKI_CAControl->Status != CARD_CONTROL_STATUS_ACTIVATED )
    {
        CsDbg( BRLL_RULE, "BR_OSC_0_B : Bypass - pMYKI_CAControl->Status (%d) != CARD_CONTROL_STATUS_ACTIVATED (%d)", pMYKI_CAControl->Status, CARD_CONTROL_STATUS_ACTIVATED );
        pData->ReturnedData.rejectReason = MYKI_BR_REJECT_REASON_CARD_NOT_ACTIVATED;
        return RULE_RESULT_BYPASSED;
    }

/*  Handled by Myki Card Services
    if ( pMYKI_OAControl->Checksum != 0 ) // pMYKI_OAControl->CalculatedChecksum )
    {
        CsDbg( BRLL_RULE, "BR_OSC_0_B : Bypass - pMYKI_OAControl->Checksum (0x%04x) != 0 ) // pMYKI_OAControl->CalculatedChecksum", pMYKI_OAControl->Checksum );
        return RULE_RESULT_BYPASSED;
    }

    if ( pMYKI_OAIssuer->Certificate != pMYKI_OAIssuer->GeneratedCertificate )
    {
        CsDbg( BRLL_RULE, "BR_OSC_0_B : Bypass - pMYKI_OAIssuer->Certificate != pMYKI_OAIssuer->GeneratedCertificate" );
        return RULE_RESULT_BYPASSED;
    }
*/

    if ( pMYKI_OAControl->Status == OAPP_CONTROL_STATUS_ACTIVATED && pMYKI_OAControl->ExpiryDate < pData->DynamicData.currentBusinessDate )        // KWS: Different types!
    {
        CsDbg( BRLL_RULE, "BR_OSC_0_B : Bypass - pMYKI_OAControl->ExpiryDate (%d) < pData->DynamicData.currentBusinessDate (%d)", pMYKI_OAControl->ExpiryDate, pData->DynamicData.currentBusinessDate );
        pData->ReturnedData.rejectReason = MYKI_BR_REJECT_REASON_EXPIRED;
        pData->ReturnedData.cardExpiryDate = pMYKI_CAControl->ExpiryDate;
        return RULE_RESULT_BYPASSED;
    }

    if
    (  // Prefix with "!" so the conditions below can be the same as the document (ie we bypass if the pre-conditions *are not* (ie "!") met)
        !(
            ( pMYKI_OAControl->Status == OAPP_CONTROL_STATUS_INITIALISED ) ||
            ( pMYKI_OAControl->Status == OAPP_CONTROL_STATUS_ISSUED      ) ||
            ( pMYKI_OAControl->Status == OAPP_CONTROL_STATUS_ACTIVATED   ) ||
            ( pMYKI_OAControl->Status == OAPP_CONTROL_STATUS_BLOCKED && pMYKI_OAControl->BlockingReason != OAPP_CONTROL_BLOCKING_REASON_DISABLED )
        )
    )
    {
        CsDbg( BRLL_RULE, "BR_OSC_0_B : Bypass - pMYKI_OAControl->Status != APP_STATUS_INITIALISED, etc" );
        pData->ReturnedData.rejectReason    = MYKI_BR_REJECT_REASON_APPLICATION_BLOCKED;
        pData->ReturnedData.blockingReason  = pMYKI_OAControl->Status == OAPP_CONTROL_STATUS_BLOCKED ?
                                                    (int)pMYKI_OAControl->BlockingReason :
                                                    (int)OAPP_CONTROL_BLOCKING_REASON_RESERVED;
        return RULE_RESULT_BYPASSED;
    }

    //
    //  Execute Rule
    //

    CsDbg( BRLL_RULE, "BR_OSC_0_B : Executed" );
    return RULE_RESULT_EXECUTED;

}
