/**************************************************************************
*   ID :  OSC_1_4
*    Authorized Officer Card Processing
*
*    Pre-Conditions
*    1. Authorized Officer Card has been validated
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

RuleResult_e BR_OSC_1_4( MYKI_BR_ContextData_t *pData )
{
    MYKI_OAControl_t           *pMYKI_OAControl         = NULL;
    MYKI_OAInspectorData_t     *pMYKI_OAInspectorData   = NULL;
    int                         nResult                 = 0;

    CsDbg( BRLL_RULE, "BR_OSC_1_4 : Start (Authorized Officer Card Processing)" );

    if ( ! pData )
    {
        CsErrx( "BR_OSC_1_4 : Invalid argument(s)" );
        return  RULE_RESULT_ERROR;
    }

    if ( ( nResult = MYKI_CS_OAControlGet( &pMYKI_OAControl ) ) < 0 )
    {
        CsErrx( "BR_OSC_1_4 : MYKI_CS_OAControlGet() failed (%d)", nResult );
        return  RULE_RESULT_ERROR;
    }

    /*  PRE-CONDITIONS */
    {
        if ( pData->DynamicData.isAOCard == FALSE )
        {
            if ( ( nResult = myki_br_isOfRole( pData, OperatorType_AUTHORISED_OFFICER ) ) < 0 )
            {
                CsDbg( BRLL_RULE, "BR_OSC_1_4 : myki_br_isOfRole() failed" );
                return  RULE_RESULT_ERROR;
            }
            pData->DynamicData.isAOCard = nResult > 0 ? TRUE : FALSE;
        }

        if ( pData->DynamicData.isAOCard == FALSE )
        {
            CsDbg( BRLL_RULE, "BR_OSC_1_4 : Bypassed - Not authorized office's card" );
            pData->ReturnedData.bypassCode      = BYPASS_CODE( 1, 4, 1, 0 );
            return  RULE_RESULT_BYPASSED;
        }

        if ( ( nResult = MYKI_CS_OAInspectorDataGet( &pMYKI_OAInspectorData ) ) != MYKI_CS_OK )
        {
            CsErrx( "BR_OSC_1_4 : MYKI_CS_OAInspectorDataGet() failed (%d)", nResult );
            return  RULE_RESULT_ERROR;
        }

        if ( pMYKI_OAControl->StaffProviderId != pData->StaticData.serviceProviderId )
        {
            CsDbg( BRLL_RULE, "BR_OSC_1_4 : Bypassed - Wrong service provider (%d/%d)",
                    pMYKI_OAControl->StaffProviderId, pData->StaticData.serviceProviderId );
            pData->ReturnedData.rejectReason    = MYKI_BR_REJECT_REASON_INVALID_SERVICE_PROVIDER;
            pData->ReturnedData.bypassCode      = BYPASS_CODE( 1, 4, 2, 0 );
            return  RULE_RESULT_BYPASSED;
        }
    }

    /*  PROCESSING */
    {
        if ( ( nResult = myki_br_ldt_OAppUpdate_AOCardProcessing( pData ) ) < 0 )
        {
            CsErrx( "BR_OSC_1_4 : myki_br_ldt_OAppUpdate_AOCardProcessing() failed (%d)", nResult );
            return  RULE_RESULT_ERROR;
        }
    }

    CsDbg( BRLL_RULE, "BR_OSC_1_4 : Executed" );
    return RULE_RESULT_EXECUTED;
}
