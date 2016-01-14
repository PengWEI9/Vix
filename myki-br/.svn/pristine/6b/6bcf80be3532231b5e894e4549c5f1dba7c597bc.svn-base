/* -*- mode: c; tabs: 4; -*- */
/*=============================================================================
**
**    Vix Technology                   Licensed software
**    (C) 2013                         All rights reserved
**
**=============================================================================
**
**  Project/Product : MBU
**  Filename        : test_BR_LLSC_10_1.c
**  Author(s)       : An Tran
**
**  Description     :
**      Implements BR_LLSC_10_1 business rule unit-tests.
**
**  Function(s)     :
**      Template                [Public]    template function
**
**  Information     :
**   Compiler(s)    : ANSI C
**   Target(s)      : Independent
**
**  History         :
**   Vers.  Date        Aut.  Type     Description
**   -----  ----------  ----  -------  ----------------------------------------
**    1.00  18.10.13    ANT   Create
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

#include <cs.h>
#include <myki_cardservices.h>
#include <myki_cdd_enums.h>
#include <myki_br.h>
#include <myki_br_rules.h>
#include <myki_br_context_data.h>

#include "test_common.h"

/*
 *      External References
 *      -------------------
 */

/*
 *      Local Constants and Macros
 *      --------------------------
 */

/*
 *      Local Data Types
 *      ----------------
 */

/*
 *      Local Function Prototypes
 *      -------------------------
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
**  test_BR_LLSC_10_1_001
**
**  Description     :
**      Unit-test BYPASSED conditions.
**
**  Parameters      :
**      pData           [I/O]   BR context data
**
**  Returns         :
**      TRUE                    test passed
**      FALSE                   test failed
**
**  Notes           :
**
**
**==========================================================================*/

int test_BR_LLSC_10_1_001( MYKI_BR_ContextData_t *pData )
{
    MYKI_TAControl_t       *pMYKI_TAControl         = NULL;
//  int                     PassengerCode_DDA       = 57;
    int                     PassengerCode_Others    = 1;

    if ( pData == NULL )
    {
        return FALSE;
    }   /* end-of-if */

    if ( MYKI_CS_TAControlGet( &pMYKI_TAControl ) < 0 || pMYKI_TAControl == NULL )
    {
        return FALSE;
    }   /* end-of-if */

    /*  Pre-Conditions:
            TAppControl.PassengerCode = 1 (ie. != DDA passenger type)

        Post-Conditions:
            BYPASSED - no ProductInUse
    */
    {
        pMYKI_TAControl->PassengerCode  = PassengerCode_Others;
        if ( BR_LLSC_10_1( pData ) != RULE_RESULT_BYPASSED )
        {
            return FALSE;
        }   /* end-of-if */
    }

    return TRUE;
}   /* test_BR_LLSC_10_1_001( ) */

/*==========================================================================*
**
**  test_BR_LLSC_10_1_002
**
**  Description     :
**      Unit-test EXECUTED conditions, Passenger is DDA.
**
**  Parameters      :
**      pData           [I/O]   BR context data
**
**  Returns         :
**      TRUE                    test passed
**      FALSE                   test failed
**
**  Notes           :
**
**
**==========================================================================*/

int test_BR_LLSC_10_1_002( MYKI_BR_ContextData_t *pData )
{
    MYKI_TAControl_t       *pMYKI_TAControl         = NULL;
    int                     PassengerCode_DDA       = 57;
//  int                     PassengerCode_Others    = 1;

    if ( pData == NULL )
    {
        return FALSE;
    }   /* end-of-if */

    if ( MYKI_CS_TAControlGet( &pMYKI_TAControl ) < 0 || pMYKI_TAControl == NULL )
    {
        return FALSE;
    }   /* end-of-if */

    /*  Pre-Conditions:
            TAppControl.PassengerCode = 57 (ie. == DDA Passenger type)

        Post-Conditions:
            Tariff.ConcessionType.IsDDA = TRUE
    */
    {
        pMYKI_TAControl->PassengerCode  = PassengerCode_DDA;
        if ( BR_LLSC_10_1( pData ) != RULE_RESULT_EXECUTED )
        {
            return FALSE;
        }   /* end-of-if */
    }

    return TRUE;
}   /* test_BR_LLSC_10_1_002( ) */
