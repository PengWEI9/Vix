/* -*- mode: c; tabs: 4; -*- */
/*=============================================================================
**
**    Vix Technology                   Licensed software
**    (C) 2015                         All rights reserved
**
**=============================================================================
**
**  Project/Product : MBU
**  Filename        : BR_VIX_OSC_2_1
**  Author(s)       : An Tran
**
**  ID              : BR_VIX_OSC_2_1
**
**  Name            : Validate Operator PIN
**
**  Data Fields     :
**
**      1.  OAppControl.Status
**      2.  OAppControl.StaffProviderId
**      3.  OAppControl.InvalidPINCount
**      4.  Static.ServiceProviderId
**      5.  Static.MaxPINRetries
**      6.  Dynamic.OperatorPIN
**      7.  Internal.IsValidPIN
**
**  Pre-conditions  :
**
**      1.  Operator application status (1) is activated.
**      2.  Staff's service provider id (2) is the same as device service
**          provider id (4).
**      3.  Invalid PIN count (3) does not exceed maximum PIN retries (5).
**      4.  Operator PIN (6) entered.
**
**  Description     :
**
**      1.  Perform OAppPinUpdate/Validate transaction.
**      2.  If PIN is not valid and if exceeded maximum PIN retries (5) then
**          perform OAppUpdate/Block with blocking reason set to 1.
**
**  Post-conditions :
**
**      1.  PIN is valid/invalid (7).
**      2.  OApplication blocked if exceeded maximum PIN retries.
**
**  Member(s)       :
**      BR_VIX_OSC_2_1          [public]    business rule
**
**  Information     :
**   Compiler(s)    : C/C++
**   Target(s)      : Independent
**
**  Subversion      :
**      $Id: BR_VIX_OSC_2_1.c 81636 2015-11-12 04:02:16Z atran $
**      $HeadURL: https://auperasvn01.aupera.erggroup.com/svn/DPG_SWBase/myki-br/trunk/src/BR_VIX_OSC_2_1.c $
**
**  History         :
**   Vers.  Date        Aut.  Type     Description
**   -----  ----------  ----  -------  ----------------------------------------
**    1.00  27.05.15    ANT   Create
**    1.01  02.06.15    ANT   Add      Added blocking OApplication
**    1.03  12.11.15    ANT   Modify   NGBU-838: Added returning blocking reason.
**
**===========================================================================*/

/*
 *      Options
 *      -------
 */

/*
 *      Includes
 *      --------
 */

#include    <stdlib.h>
#include    <string.h>

#include    <cs.h>
#include    <myki_cardservices.h>
#include    <LDT.h>

#include    "myki_br_rules.h"
#include    "BR_Common.h"

/*
 *      Local Constants and Macros
 *      --------------------------
 */

/*
 *      Local Variables
 *      ---------------
 */

/*
 *      Global Variables
 *      ----------------
 */

/*==========================================================================*
**
**  BR_VIX_OSC_2_1
**
**  Description     :
**      Implements business rule BR_VIX_OSC_2_1.
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
**
**==========================================================================*/

RuleResult_e    BR_VIX_OSC_2_1( MYKI_BR_ContextData_t *pData )
{
    MYKI_OAControl_t       *pMYKI_OAControl     = NULL;
    int                     nResult             = 0;
    int                     bIsValidPin         = FALSE;

    CsDbg( BRLL_RULE, "BR_VIX_OSC_2_1 : Start (Validate Operator PIN)" );

    if ( pData == NULL )
    {
        CsErrx( "BR_VIX_OSC_2_1 : Invalid parameter" );
        return  RULE_RESULT_ERROR;
    }

    if ( ( nResult = MYKI_CS_OAControlGet( &pMYKI_OAControl ) ) < 0 || pMYKI_OAControl == NULL )
    {
        CsErrx( "BR_VIX_OSC_2_1 : MYKI_CS_OAControlGet() failed (%d)", nResult );
        return  RULE_RESULT_ERROR;
    }

    /*  PRE-CONDITIONS */
    {
        /*  1.  Operator application status (1) is activated. */
        if ( pMYKI_OAControl->Status != OAPP_CONTROL_STATUS_ACTIVATED )
        {
            CsDbg( BRLL_RULE, "BR_VIX_OSC_2_1 : Bypass - OApplication not activated" );
            pData->ReturnedData.rejectReason    = MYKI_BR_REJECT_REASON_APPLICATION_BLOCKED;
            pData->ReturnedData.blockingReason  = (int)pMYKI_OAControl->BlockingReason;
            pData->ReturnedData.bypassCode      = BYPASS_CODE( 2, 1, 1, 0 );
            return  RULE_RESULT_BYPASSED;
        }

        /*  2.  Staff's service provider id (2) is the same as device service
                provider id (4). */
        if ( pMYKI_OAControl->StaffProviderId != pData->StaticData.serviceProviderId )
        {
            CsDbg( BRLL_RULE, "BR_VIX_OSC_2_1 : Bypass - Wrong service provider (%d/%d)",
                    pMYKI_OAControl->StaffProviderId, pData->StaticData.serviceProviderId );
            pData->ReturnedData.rejectReason    = MYKI_BR_REJECT_REASON_INVALID_SERVICE_PROVIDER;
            pData->ReturnedData.bypassCode      = BYPASS_CODE( 2, 1, 2, 0 );
            return  RULE_RESULT_BYPASSED;
        }

        /*  3.  Invalid PIN count (3) does not exceed maximum PIN retries (5). */
        if ( pData->StaticData.maxPinRetries >  0 &&
             pData->StaticData.maxPinRetries <= pMYKI_OAControl->InvalidPinCount )
        {
            CsDbg( BRLL_RULE, "BR_VIX_OSC_2_1 : Bypass - Maximum PIN retries (%d,%d) exceeded",
                    pMYKI_OAControl->InvalidPinCount, pData->StaticData.maxPinRetries );
            pData->ReturnedData.rejectReason    = MYKI_BR_REJECT_REASON_MAX_PIN_RETRIES_EXCEEDED;
            pData->ReturnedData.bypassCode      = BYPASS_CODE( 2, 1, 3, 0 );
            return  RULE_RESULT_BYPASSED;
        }

        /*  3.  Operator PIN (6) entered. */
        if ( strlen( pData->DynamicData.operatorPin ) == 0 )
        {
            CsDbg( BRLL_RULE, "BR_VIX_OSC_2_1 : Bypass - Zero PIN length" );
            pData->ReturnedData.rejectReason    = MYKI_BR_REJECT_REASON_INVALID_PIN;
            pData->ReturnedData.bypassCode      = BYPASS_CODE( 2, 1, 4, 0 );
            return  RULE_RESULT_BYPASSED;
        }
    }

    /*  PROCESSING */
    {
        union
        {
            OAppUpdate_t        OAppUpdate;
            OAppPinUpdate_t     OAppPinUpdate;

        }   request;

        /*  1.  Perform OAppPinUpdate/Validate transaction. */
        memset( &request, 0, sizeof( request ) );
        request.OAppPinUpdate.serviceProviderId = pData->StaticData.serviceProviderId;
        request.OAppPinUpdate.isPinSet          = TRUE;
        memcpy( request.OAppPinUpdate.pin,        pData->DynamicData.operatorPin, sizeof( request.OAppPinUpdate.pin ) );

        if ( ( nResult = MYKI_LDT_OAppPinUpdateValidate( &request.OAppPinUpdate, &pData->InternalData.TransactionData, &bIsValidPin ) ) != LDT_SUCCESS )
        {
            CsErrx( "BR_VIX_OSC_2_1 : MYKI_LDT_OAppPinUpdateValidate() failed (%d)", nResult );
            return  RULE_RESULT_ERROR;
        }

        /*  2.  If PIN is not valid and if exceeded maximum PIN retries (5) then
                perform OAppUpdate/Block with blocking reason set to 1 */
        if ( bIsValidPin == FALSE &&
             pData->StaticData.maxPinRetries >  0 &&
             pData->StaticData.maxPinRetries <= pMYKI_OAControl->InvalidPinCount )
        {
            memset( &request, 0, sizeof( request ) );
            request.OAppUpdate.serviceProviderId    = pData->StaticData.serviceProviderId;
            request.OAppUpdate.blockingReason       = OAPP_CONTROL_BLOCKING_REASON_INVALID_PIN;
            if ( ( nResult = MYKI_LDT_OAppUpdateBlock( &request.OAppUpdate, &pData->InternalData.TransactionData ) ) != LDT_SUCCESS )
            {
                CsErrx( "BR_VIX_OSC_2_1 : MYKI_LDT_OAppUpdateBlock() failed (%d)", nResult );
                return  RULE_RESULT_ERROR;
            }
        }
    }

    /*  POST-CONDITIONS */
    {
        /*  1.  PIN is valid/invalid (7). */
        pData->InternalData.isValidPin  = bIsValidPin;
    }

    CsDbg( BRLL_RULE, "BR_VIX_OSC_2_1 : Executed" );
    return  RULE_RESULT_EXECUTED;
}   /* BR_VIX_OSC_2_1( ) */
