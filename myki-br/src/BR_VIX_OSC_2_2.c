/* -*- mode: c; tabs: 4; -*- */
/*=============================================================================
**
**    Vix Technology                   Licensed software
**    (C) 2015                         All rights reserved
**
**=============================================================================
**
**  Project/Product : MBU
**  Filename        : BR_VIX_OSC_2_2
**  Author(s)       : An Tran
**
**  ID              : BR_VIX_OSC_2_2
**
**  Name            : Check Valid Operator PIN
**
**  Data Fields     :
**
**      1.  Internal.IsValidPIN
**
**  Pre-conditions  :
**
**      1.  PIN is valid (1).
**
**  Description     :
**
**  Post-conditions :
**
**      1.  PIN is valid.
**
**  Member(s)       :
**      BR_VIX_OSC_2_2          [public]    business rule
**
**  Information     :
**   Compiler(s)    : C/C++
**   Target(s)      : Independent
**
**  Subversion      :
**      $Id: BR_VIX_OSC_2_2.c 63860 2015-06-02 03:03:04Z atran $
**      $HeadURL: https://auperasvn01.aupera.erggroup.com/svn/DPG_SWBase/myki-br/trunk/src/BR_VIX_OSC_2_2.c $
**
**  History         :
**   Vers.  Date        Aut.  Type     Description
**   -----  ----------  ----  -------  ----------------------------------------
**    1.00  27.05.15    ANT   Create
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
**  BR_VIX_OSC_2_2
**
**  Description     :
**      Implements business rule BR_VIX_OSC_2_2.
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

RuleResult_e    BR_VIX_OSC_2_2( MYKI_BR_ContextData_t *pData )
{
    MYKI_OAControl_t       *pMYKI_OAControl     = NULL;
    int                     nResult             = 0;

    CsDbg( BRLL_RULE, "BR_VIX_OSC_2_2 : Start (Check Valid Operator PIN)" );

    if ( pData == NULL )
    {
        CsErrx( "BR_VIX_OSC_2_2 : Invalid parameter" );
        return  RULE_RESULT_ERROR;
    }

    if ( ( nResult = MYKI_CS_OAControlGet( &pMYKI_OAControl ) ) < 0 || pMYKI_OAControl == NULL )
    {
        CsErrx( "BR_VIX_OSC_2_2 : MYKI_CS_OAControlGet() failed (%d)", nResult );
        return  RULE_RESULT_ERROR;
    }

    /*  PRE-CONDITIONS */
    {
        /*  1.  PIN is valid (1). */
        if ( pData->InternalData.isValidPin == FALSE )
        {
            if ( pMYKI_OAControl->Status == OAPP_CONTROL_STATUS_BLOCKED )
            {
                CsDbg( BRLL_RULE, "BR_VIX_OSC_2_2 : Bypass - Exceeded PIN retries" );
                pData->ReturnedData.rejectReason    = MYKI_BR_REJECT_REASON_MAX_PIN_RETRIES_EXCEEDED;
                pData->ReturnedData.bypassCode      = BYPASS_CODE( 2, 2, 1, 1 );
            }
            else
            {
                CsDbg( BRLL_RULE, "BR_VIX_OSC_2_2 : Bypass - PIN is invalid" );
                pData->ReturnedData.rejectReason    = MYKI_BR_REJECT_REASON_INVALID_PIN;
                pData->ReturnedData.bypassCode      = BYPASS_CODE( 2, 2, 1, 2 );
            }
            return  RULE_RESULT_BYPASSED;
        }
    }

    /*  PROCESSING - DONOTHING! */

    /*  POST-CONDITIONS */

    CsDbg( BRLL_RULE, "BR_VIX_OSC_2_2 : Executed" );
    return  RULE_RESULT_EXECUTED;
}   /* BR_VIX_OSC_2_2( ) */
