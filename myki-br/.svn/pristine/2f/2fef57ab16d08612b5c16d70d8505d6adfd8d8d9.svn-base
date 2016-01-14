/* -*- mode: c; tabs: 4; -*- */
/*=============================================================================
**
**    Vix Technology                   Licensed software
**    (C) 2013                         All rights reserved
**
**=============================================================================
**
**  Project/Product : MBU
**  Filename        : BR_LLSC_10_1.c
**  Author(s)       : An Tran
**
**  ID              : BR_LLSC_10_1 - KA0004 v7.0
**
**  Name            : DDA - Check for applicability
**
**  Data Fields     :
**
**      1.  TAppControl.PassengerCode
**      2.  Tarrif.ConcessionType.isDDA
**
**  Pre-Conditions  :
**
**      1.  The passenger code(1) is listed in the passenger types as DDA applicable(2) (true).
**
**  Description     :
**
**      None
**
**  Post-Conditions :
**
**      DDA determination completed
**
**  Member(s)       :
**      BR_LLSC_10_1            [public]    business rule
**
**  History         :
**   Vers.  Date        Aut.  Type     Description
**   -----  ----------  ----  -------  ----------------------------------------
**    1.00  18.10.13    ANT   Create
**    1.01  22.10.13    ANT   Modify   Calling myki_br_IsDDA()
**
**===========================================================================*/

/*
 *      Includes
 *      --------
 */

#include <cs.h>                         /* For debug logging */
#include <myki_cardservices.h>          /* Myki Card Services */
#include <LDT.h>                        /* Logical Device Transaction */

#include "myki_br_rules.h"              /* API for all BR_xxx modules */
#include "BR_Common.h"                  /* Common utilities, macros, etc */

/*==========================================================================*
**
**  BR_LLSC_10_1
**
**  Description     :
**      Implements business rule BR_LLSC_10_1.
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

RuleResult_e BR_LLSC_10_1( MYKI_BR_ContextData_t *pData )
{
    MYKI_TAControl_t       *pMYKI_TAControl         = NULL;
    int                     IsDDA                   = FALSE;

    CsDbg( BRLL_RULE, "BR_LLSC_10_1 : Start (DDA - Check for applicability)" );

    if ( pData == NULL )
    {
        CsErrx( "BR_LLSC_10_1 : Invalid argument(s)" );
        return RULE_RESULT_ERROR;
    }   /* end-of-if */

    if ( MYKI_CS_TAControlGet( &pMYKI_TAControl ) < 0 )
    {
        CsErrx( "BR_LLSC_10_1 : MYKI_CS_TAControlGet() failed" );
        return RULE_RESULT_ERROR;
    }   /* end-of-if */

    /* PRE-CONDITIONS */
    {
        IsDDA   = myki_br_IsDDA( pMYKI_TAControl->PassengerCode );
        if ( IsDDA < 0 )
        {
            CsErrx( "BR_LLSC_10_1 : myki_br_IsDDA() failed" );
            return RULE_RESULT_ERROR;
        }   /* end-of-if */

        if ( IsDDA == FALSE )
        {
            CsDbg( BRLL_RULE, "BR_LLSC_10_1 : BYPASSED - passenger type is not DDA" );
            pData->Tariff.concessionTypeIsDDA   = FALSE;
            return RULE_RESULT_BYPASSED;
        }   /* end-of-if */
    }

    /* PROCESSING */
    {
        pData->Tariff.concessionTypeIsDDA   = TRUE;
    }

    CsDbg( BRLL_RULE, "BR_LLSC_10_1 : Executed" );
    return RULE_RESULT_EXECUTED;
}   /* BR_LLSC_10_1( ) */


