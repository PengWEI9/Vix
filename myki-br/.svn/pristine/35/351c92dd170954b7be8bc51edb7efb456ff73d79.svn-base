/* -*- mode: c; tabs: 4; -*- */
/*=============================================================================
**
**    Vix Technology                   Licensed software
**    (C) 2013                         All rights reserved
**
**=============================================================================
**
**  Project/Product : MBU
**  Filename        : BR_LLSC_0_B.c
**  Author(s)       : ?
**
**  ID              : BR_LLSC_0_B - NTS0177 v7.2
**
**  Description     :
**      Belongs to SEQ_LLSC_0A: Card Transaction Initialization
**
**      CardControl.Checksum = calculated checksum
**      CardIssuer.Certificate = generated certificate
**      CardControl.ExpiryDate >= Dynamic.CurrentDate
**      CardControl.Status = Activated
**      TAppControl.Checksum = calculated checksum
**      TAppIssuer.Certificate = generated certificate
**      TAppControl.ExpiryDate >= Dynamic.CurrentDate
**      TAppControl.Status = Initalised OR Issued OR Activated  Or Blocked
**
**  Member(s)       :
**      BR_LLSC_0_B             [public]    business rule
**
**  Information     :
**   Compiler(s)    : C
**   Target(s)      : Independent
**
**  History         :
**   Vers.  Date        Aut.  Type     Description
**   -----  ----------  ----  -------  ----------------------------------------
**    1.00  ??.??.13    ?     Create
**    1.01  13.02.14    ANT   Modify   Mimic KAMCO reader functionality
**                                     ignoring CardControl.ExpiryDate and
**                                     TAppControl.ExpiryDate
**    1.02  08.05.14    ANT   Modify   Rectified checking for crypto error
**    1.03  12.11.15    ANT   Modify   NGBU-838: Added returning blocking reason.
**
**===========================================================================*/

#include <cs.h>                 // For debug logging
#include <myki_cardservices.h>  // Myki Card Services
#include <LDT.h>                // Logical Device Transaction
#include <myki_cdd_enums.h>     // Card data definitions

#include "myki_br_rules.h"      // API for all BR_xxx modules
#include "BR_Common.h"          // Common utilities, macros, etc

/*==========================================================================*
**
**  BR_LLSC_0_B
**
**  Description     :
**      Implements business rule BR_LLSC_0_B.
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

RuleResult_e BR_LLSC_0_B( MYKI_BR_ContextData_t *pData )
{
    MYKI_CAIssuer_t             *pMYKI_CAIssuer     = NULL;
    MYKI_CAControl_t            *pMYKI_CAControl    = NULL;
    MYKI_TAIssuer_t             *pMYKI_TAIssuer     = NULL;
    MYKI_TAControl_t            *pMYKI_TAControl    = NULL;
	int                         result              = 0;

    CsDbg( BRLL_RULE, "BR_LLSC_0_B : Start (Initial Card Sanity Check)" );

    if ( ! pData )
    {
        CsErrx( "BR_LLSC_0_B : Invalid argument(s)" );
        return RULE_RESULT_ERROR;
    }

    if ( ( result = MYKI_CS_CAIssuerGet( &pMYKI_CAIssuer ) ) < 0 )
    {
        CsErrx( "BR_LLSC_0_B : MYKI_CS_CAIssuerGet() failed result: %d", result );
		if ( result == CRYPTO_AUTH_FAIL_ERR )
			pData->ReturnedData.rejectReason = MYKI_BR_REJECT_REASON_CRYPTO_AUTH_FAIL;
		else
			pData->ReturnedData.rejectReason = MYKI_BR_REJECT_REASON_UNREADABLE;		
        return RULE_RESULT_ERROR;
    }

    if ( ( result = MYKI_CS_CAControlGet( &pMYKI_CAControl ) ) < 0 )
    {
        CsErrx( "BR_LLSC_0_B : MYKI_CS_CAControlGet() failed result: %d", result );
		if ( result == CRYPTO_AUTH_FAIL_ERR )
			pData->ReturnedData.rejectReason = MYKI_BR_REJECT_REASON_CRYPTO_AUTH_FAIL;
		else
			pData->ReturnedData.rejectReason = MYKI_BR_REJECT_REASON_UNREADABLE;			
        return RULE_RESULT_ERROR;
    }

    if ( ( result = MYKI_CS_TAIssuerGet( &pMYKI_TAIssuer ) ) < 0 )
    {
        CsErrx( "BR_LLSC_0_B : MYKI_CS_TAIssuerGet() failed result: %d", result );
		if ( result == CRYPTO_AUTH_FAIL_ERR )
			pData->ReturnedData.rejectReason = MYKI_BR_REJECT_REASON_CRYPTO_AUTH_FAIL;
		else
			pData->ReturnedData.rejectReason = MYKI_BR_REJECT_REASON_UNREADABLE;			
        return RULE_RESULT_ERROR;
    }

    if ( ( result = MYKI_CS_TAControlGet( &pMYKI_TAControl ) ) < 0 )
    {
        CsErrx( "BR_LLSC_0_B : MYKI_CS_TAControlGet() failed result: %d", result );
		if ( result == CRYPTO_AUTH_FAIL_ERR )
			pData->ReturnedData.rejectReason = MYKI_BR_REJECT_REASON_CRYPTO_AUTH_FAIL;
		else
			pData->ReturnedData.rejectReason = MYKI_BR_REJECT_REASON_UNREADABLE;			
        return RULE_RESULT_ERROR;
    }

    //
    //  Check Pre-conditions
    //

/*  Handled by Myki Card Services
    if ( pMYKI_CAControl->Checksum != 0 ) // pMYKI_CAControl->CalculatedChecksum )
    {
        CsDbg( BRLL_RULE, "BR_LLSC_0_B : Bypass - pMYKI_CAControl->Checksum (0x%04x) != 0 ) // pMYKI_CAControl->CalculatedChecksum", pMYKI_CAControl->Checksum );
        return RULE_RESULT_BYPASSED;
    }

    if ( pMYKI_CAIssuer->Certificate != pMYKI_CAIssuer->GeneratedCertificate )
    {
        CsDbg( BRLL_RULE, "BR_LLSC_0_B : Bypass - pMYKI_CAIssuer->Certificate != pMYKI_CAIssuer->GeneratedCertificate" );
        return RULE_RESULT_BYPASSED;
    }
*/

    if ( pMYKI_CAControl->Status == TAPP_CONTROL_STATUS_ACTIVATED && pMYKI_CAControl->ExpiryDate < pData->DynamicData.currentBusinessDate )
    {
        CsDbg( BRLL_RULE, "BR_LLSC_0_B : Bypass - CardControl.ExpiryDate(%d) < Dynamic.CurrentBusinessDate(%d)",
            pMYKI_CAControl->ExpiryDate, pData->DynamicData.currentBusinessDate );
        pData->ReturnedData.rejectReason = MYKI_BR_REJECT_REASON_EXPIRED;
        pData->ReturnedData.cardExpiryDate = pMYKI_CAControl->ExpiryDate;
        return RULE_RESULT_BYPASSED;
    }
	if ( pMYKI_CAControl->Status == CARD_CONTROL_STATUS_BLOCKED )
    {
        CsDbg( BRLL_RULE, "BR_LLSC_0_B : Bypass - pMYKI_CAControl->Status (%d) == CARD_CONTROL_STATUS_BLOCKED (%d)", pMYKI_CAControl->Status, CARD_CONTROL_STATUS_BLOCKED );
        pData->ReturnedData.rejectReason    = MYKI_BR_REJECT_REASON_CARD_BLOCKED;
        pData->ReturnedData.blockingReason  = (int)pMYKI_CAControl->BlockingReason;
        return RULE_RESULT_BYPASSED;
    }
    else if ( pMYKI_CAControl->Status != CARD_CONTROL_STATUS_ACTIVATED )
    {
        CsDbg( BRLL_RULE, "BR_LLSC_0_B : Bypass - pMYKI_CAControl->Status (%d) != CARD_CONTROL_STATUS_ACTIVATED (%d)", pMYKI_CAControl->Status, CARD_CONTROL_STATUS_ACTIVATED );
        pData->ReturnedData.rejectReason = MYKI_BR_REJECT_REASON_CARD_NOT_ACTIVATED;
        return RULE_RESULT_BYPASSED;
    }

/*  Handled by Myki Card Services
    if ( pMYKI_TAControl->Checksum != 0 ) // pMYKI_TAControl->CalculatedChecksum )
    {
        CsDbg( BRLL_RULE, "BR_LLSC_0_B : Bypass - pMYKI_TAControl->Checksum (0x%04x) != 0 ) // pMYKI_TAControl->CalculatedChecksum", pMYKI_TAControl->Checksum );
        return RULE_RESULT_BYPASSED;
    }

    if ( pMYKI_TAIssuer->Certificate != pMYKI_TAIssuer->GeneratedCertificate )
    {
        CsDbg( BRLL_RULE, "BR_LLSC_0_B : Bypass - pMYKI_TAIssuer->Certificate != pMYKI_TAIssuer->GeneratedCertificate" );
        return RULE_RESULT_BYPASSED;
    }
*/
 
    if ( pMYKI_TAControl->Status == TAPP_CONTROL_STATUS_ACTIVATED &&  pMYKI_TAControl->ExpiryDate < pData->DynamicData.currentBusinessDate )        // KWS: Different types!
    {
        CsDbg( BRLL_RULE, "BR_LLSC_0_B : Bypass - TAppControl.ExpiryDate(%d) < Dynamic.CurrentBusinessDate(%d) TAControl status(%d)",
            pMYKI_TAControl->ExpiryDate, pData->DynamicData.currentBusinessDate ,  pMYKI_TAControl->Status);
        pData->ReturnedData.rejectReason = MYKI_BR_REJECT_REASON_EXPIRED;
        pData->ReturnedData.cardExpiryDate = pMYKI_TAControl->ExpiryDate;
        return RULE_RESULT_BYPASSED;
    }

    if
    (
        ( pMYKI_TAControl->Status != TAPP_CONTROL_STATUS_INITIALISED ) &&
        ( pMYKI_TAControl->Status != TAPP_CONTROL_STATUS_ISSUED      ) &&
        ( pMYKI_TAControl->Status != TAPP_CONTROL_STATUS_ACTIVATED   ) &&
        ( pMYKI_TAControl->Status != TAPP_CONTROL_STATUS_BLOCKED     )
    )
    {
        CsDbg( BRLL_RULE, "BR_LLSC_0_B : Bypass - pMYKI_TAControl->Status != APP_STATUS_INITIALISED, etc" );
        pData->ReturnedData.rejectReason    = MYKI_BR_REJECT_REASON_APPLICATION_BLOCKED;
        pData->ReturnedData.blockingReason  = (int)TAPP_CONTROL_BLOCKING_REASON_RESERVED;
        return RULE_RESULT_BYPASSED;
    }

    //
    //  Execute Rule
    //

    CsDbg( BRLL_RULE, "BR_LLSC_0_B : Executed" );
    return RULE_RESULT_EXECUTED;
}

