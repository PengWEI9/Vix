/* -*- mode: c; tabs: 4; -*- */
/*=============================================================================
**
**    Vix Technology                   Licensed software
**    (C) 2015                         All rights reserved
**
**=============================================================================
**
**  Project/Product : MBU
**  Filename        : BR_VIX_OSC_2_3
**  Author(s)       : An Tran
**
**  ID              : BR_VIX_OSC_2_3
**
**  Name            : Update Operator PIN
**
**  Data Fields     :
**
**      1.  OAppControl.Status
**      2.  OAppControl.StaffProviderId
**      3.  Static.ServiceProviderId
**      4.  Dynamic.OperatorPIN
**
**  Pre-conditions  :
**
**      1.  The OApplication status (1) is activated or is blocked with reason
**          code 1.
**      2.  Staff's service provider id (2) is the same as device service
**          provider id (3).
**
**  Description     :
**
**      1.  Perform OAppUpdate/Unblock if OApplication is blocked.
**      2.  Perform OAppPinUpdate/None transaction.
**          a.  Set new operator PIN (4) if entered.
**
**  Post-conditions :
**
**      1.  OApplication is unblocked.
**      2.  Operator PIN updated.
**
**  Member(s)       :
**      BR_VIX_OSC_2_3              [public]    business rule
**
**  Information     :
**   Compiler(s)    : C/C++
**   Target(s)      : Independent
**
**  Subversion      :
**      $Id: BR_VIX_OSC_2_3.c 81636 2015-11-12 04:02:16Z atran $
**      $HeadURL: https://auperasvn01.aupera.erggroup.com/svn/DPG_SWBase/myki-br/trunk/src/BR_VIX_OSC_2_3.c $
**
**  History         :
**   Vers.  Date        Aut.  Type     Description
**   -----  ----------  ----  -------  ----------------------------------------
**    1.00  27.05.15    ANT   Create
**    1.01  02.06.15    ANT   Add      Added unblocking OApplication
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
**  BR_VIX_OSC_2_3
**
**  Description     :
**      Implements business rule BR_VIX_OSC_2_3.
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

RuleResult_e    BR_VIX_OSC_2_3( MYKI_BR_ContextData_t *pData )
{
    MYKI_OAControl_t       *pMYKI_OAControl     = NULL;
    int                     nResult             = 0;

    CsDbg( BRLL_RULE, "BR_VIX_OSC_2_3 : Start (Update Operator PIN)" );

    if ( pData == NULL )
    {
        CsErrx( "BR_VIX_OSC_2_3 : Invalid parameter" );
        return  RULE_RESULT_ERROR;
    }

    if ( ( nResult = MYKI_CS_OAControlGet( &pMYKI_OAControl ) ) < 0 || pMYKI_OAControl == NULL )
    {
        CsErrx( "BR_VIX_OSC_2_3 : MYKI_CS_OAControlGet() failed (%d)", nResult );
        return  RULE_RESULT_ERROR;
    }

    /*  PRE-CONDITIONS */
    {
        /*  1.  The OApplication status (1) is activated or is blocked with reason
                code 1. */
        if ( pMYKI_OAControl->Status == OAPP_CONTROL_STATUS_BLOCKED )
        {
            if ( pMYKI_OAControl->BlockingReason != OAPP_CONTROL_BLOCKING_REASON_INVALID_PIN )
            {
                CsDbg( BRLL_RULE, "BR_VIX_OSC_2_3 : Bypass - OApplication blocked with reason code other than OAPP_CONTROL_BLOCKING_REASON_INVALID_PIN(%d)",
                        OAPP_CONTROL_BLOCKING_REASON_INVALID_PIN );
                pData->ReturnedData.rejectReason    = MYKI_BR_REJECT_REASON_APPLICATION_BLOCKED;
                pData->ReturnedData.blockingReason  = (int)pMYKI_OAControl->BlockingReason;
                pData->ReturnedData.bypassCode      = BYPASS_CODE( 2, 3, 1, 1 );
                return  RULE_RESULT_BYPASSED;
            }
        }
        else
        if ( pMYKI_OAControl->Status != OAPP_CONTROL_STATUS_ACTIVATED )
        {
            CsDbg( BRLL_RULE, "BR_VIX_OSC_2_3 : Bypass - OApplication not activated (%d)", pMYKI_OAControl->Status );
            pData->ReturnedData.rejectReason    = MYKI_BR_REJECT_REASON_APPLICATION_BLOCKED;
            pData->ReturnedData.blockingReason  = (int)OAPP_CONTROL_BLOCKING_REASON_RESERVED;
            pData->ReturnedData.bypassCode      = BYPASS_CODE( 2, 3, 1, 2 );
            return  RULE_RESULT_BYPASSED;
        }

        /*  2.  Staff's service provider id (2) is the same as device service
                provider id (3). */
        if ( pMYKI_OAControl->StaffProviderId != pData->StaticData.serviceProviderId )
        {
            CsDbg( BRLL_RULE, "BR_VIX_OSC_2_3 : Bypass - Wrong service provider (%d/%d)",
                    pMYKI_OAControl->StaffProviderId, pData->StaticData.serviceProviderId );
            pData->ReturnedData.rejectReason    = MYKI_BR_REJECT_REASON_INVALID_SERVICE_PROVIDER;
            pData->ReturnedData.bypassCode      = BYPASS_CODE( 2, 3, 2, 0 );
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

        /*  1.  Perform OAppUpdate/Unblock if OApplication is blocked. */
        if ( pMYKI_OAControl->Status == OAPP_CONTROL_STATUS_BLOCKED )
        {
            memset( &request, 0, sizeof( request ) );
            if ( ( nResult = MYKI_LDT_OAppUpdateUnblock( &request.OAppUpdate, &pData->InternalData.TransactionData ) ) != LDT_SUCCESS )
            {
                CsErrx( "BR_VIX_OSC_2_1 : MYKI_LDT_OAppUpdateUnblock() failed (%d)", nResult );
                return  RULE_RESULT_ERROR;
            }
        }

        /*  2.  Perform OAppPinUpdate/None transaction.
                a.  Set new operator PIN (4) if entered. */
        memset( &request, 0, sizeof( request ) );
        request.OAppPinUpdate.serviceProviderId   = pData->StaticData.serviceProviderId;
        request.OAppPinUpdate.isPinSet            = TRUE;
        memcpy( request.OAppPinUpdate.pin,          pData->DynamicData.operatorPin, sizeof( request.OAppPinUpdate.pin ) );

        if ( ( nResult = MYKI_LDT_OAppPinUpdate( &request.OAppPinUpdate, &pData->InternalData.TransactionData ) ) != LDT_SUCCESS )
        {
            CsErrx( "BR_VIX_OSC_2_3 : MYKI_LDT_OAppPinUpdate() failed (%d)", nResult );
            return  RULE_RESULT_ERROR;
        }
    }

    /*  POST-CONDITIONS */

    CsDbg( BRLL_RULE, "BR_VIX_OSC_2_3 : Executed" );
    return  RULE_RESULT_EXECUTED;
}   /* BR_VIX_OSC_2_3( ) */
