/* -*- mode: c; tabs: 4; -*- */
/*=============================================================================
**
**    Vix Technology                   Licensed software
**    (C) 2013                         All rights reserved
**
**=============================================================================
**
**  Project/Product : MBU
**  Filename        : test_BR_LLSC_1_10.c
**  Author(s)       : An Tran
**
**  Description     :
**      Implements BR_LLSC_1_10 business rule unit test.
**
**  Function(s)     :
**      test_BR_LLSC_1_10       [public]    perform unit test
**
**  Information     :
**   Compiler(s)    : ANSI C
**   Target(s)      : Independent
**
**  History         :
**   Vers.  Date        Aut.  Type     Description
**   -----  ----------  ----  -------  ----------------------------------------
**    1.00  13.09.13    ANT   Create
**    1.01  29.01.14    ANT   Modify   Updated unit-test (Note: Daily/Weekly
**                                     Capping expiry ends at start-of
**                                     business day)
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

#define BRLL_RULE                       4
#define _01Jan2006                      13149

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
**  test_BR_LLSC_1_10_001
**
**  Description     :
**      Unit test BYPASSED pre-conditions.
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

int test_BR_LLSC_1_10_001( MYKI_BR_ContextData_t *pData )
{
    MYKI_TACapping_t       *pMYKI_TACapping     = NULL;

    if ( pData == NULL )
    {
        return FALSE;
    }   /* end-of-if */

    if ( MYKI_CS_TACappingGet( &pMYKI_TACapping ) < 0 || pMYKI_TACapping == NULL )
    {
        return FALSE;
    }   /* end-of-if */

    /*  Pre-Conditions:
            TACapping.Daily.Expiry  > DynamicData.CurrentBusinessDate
            TACapping.Weekly.Expiry > DynamicData.CurrentBusinessDate

        Post-Conditions:
            BYPASSED
    */
    {
        pMYKI_TACapping->Daily.Expiry       = pData->DynamicData.currentBusinessDate + 1;
        pMYKI_TACapping->Weekly.Expiry      = pData->DynamicData.currentBusinessDate + 1;

        if ( BR_LLSC_1_10( pData ) != RULE_RESULT_BYPASSED )
        {
            return FALSE;
        }   /* end-of-if */
    }

    return TRUE;
}   /* test_BR_LLSC_1_10_001( ) */

/*==========================================================================*
**
**  test_BR_LLSC_1_10_002
**
**  Description     :
**      EXCUTED
**      Reset TACapping.Daily
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

int test_BR_LLSC_1_10_002( MYKI_BR_ContextData_t *pData )
{
    MYKI_TACapping_t       *pMYKI_TACapping     = NULL;
    MYKI_TAControl_t       *pMYKI_TAControl     = NULL;
    MYKI_CAControl_t       *pCAControl          = NULL;

    if ( pData == NULL )
    {
        return FALSE;
    }   /* end-of-if */

    if ( MYKI_CS_TAControlGet( &pMYKI_TAControl ) < 0 || pMYKI_TAControl == NULL )
    {
        return FALSE;
    }
    
    if ( MYKI_CS_CAControlGet(&pCAControl) != MYKI_CS_OK )
    {
        return FALSE;
    }
    pMYKI_TAControl->Status                     = TAPP_CONTROL_STATUS_ACTIVATED;
    pCAControl->Status                          = CARD_CONTROL_STATUS_ACTIVATED;

    if ( MYKI_CS_TACappingGet( &pMYKI_TACapping ) < 0 || pMYKI_TACapping == NULL )
    {
        return FALSE;
    }   /* end-of-if */

    /*  Pre-Conditions:
            TACapping.Daily.Expiry  = DynamicData.CurrentBusinessDate
            TACapping.Weekly.Expiry > DynamicData.CurrentBusinessDate

        Post-Conditions:
            EXECUTED
            Reset TACapping.Daily
    */
    {
        pMYKI_TACapping->Daily.Expiry       = pData->DynamicData.currentBusinessDate;
        pMYKI_TACapping->Weekly.Expiry      = pData->DynamicData.currentBusinessDate + 1;

        if ( BR_LLSC_1_10( pData ) != RULE_RESULT_EXECUTED )
        {
            return FALSE;
        }   /* end-of-if */
    }

    return TRUE;
}   /* test_BR_LLSC_1_10_002( ) */

/*==========================================================================*
**
**  test_BR_LLSC_1_10_003
**
**  Description     :
**      EXCUTED
**      Reset TACapping.Weekly
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

int test_BR_LLSC_1_10_003( MYKI_BR_ContextData_t *pData )
{
    MYKI_TACapping_t       *pMYKI_TACapping     = NULL;
    MYKI_TAControl_t       *pMYKI_TAControl     = NULL;
    MYKI_CAControl_t       *pCAControl          = NULL;

    if ( pData == NULL )
    {
        return FALSE;
    }   /* end-of-if */

    if ( MYKI_CS_TAControlGet( &pMYKI_TAControl ) < 0 || pMYKI_TAControl == NULL )
    {
        return FALSE;
    }
    
    if ( MYKI_CS_CAControlGet(&pCAControl) != MYKI_CS_OK )
    {
        return FALSE;
    }
    pMYKI_TAControl->Status                     = TAPP_CONTROL_STATUS_ACTIVATED;
    pCAControl->Status                          = CARD_CONTROL_STATUS_ACTIVATED;

    if ( MYKI_CS_TACappingGet( &pMYKI_TACapping ) < 0 || pMYKI_TACapping == NULL )
    {
        return FALSE;
    }   /* end-of-if */

    /*  Pre-Conditions:
            TACapping.Daily.Expiry  > DynamicData.CurrentBusinessDate
            TACapping.Weekly.Expiry = DynamicData.CurrentBusinessDate

        Post-Conditions:
            EXECUTED
            Reset TACapping.Weekly
    */
    {
        pMYKI_TACapping->Daily.Expiry       = pData->DynamicData.currentBusinessDate + 1;
        pMYKI_TACapping->Weekly.Expiry      = pData->DynamicData.currentBusinessDate;

        if ( BR_LLSC_1_10( pData ) != RULE_RESULT_EXECUTED )
        {
            return FALSE;
        }   /* end-of-if */
    }

    return TRUE;
}   /* test_BR_LLSC_1_10_003( ) */

/*==========================================================================*
**
**  test_BR_LLSC_1_10_004
**
**  Description     :
**      EXCUTED
**      Reset TACapping.Daily
**      Reset TACapping.Weekly
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

int test_BR_LLSC_1_10_004( MYKI_BR_ContextData_t *pData )
{
    MYKI_TACapping_t       *pMYKI_TACapping     = NULL;
    MYKI_TAControl_t       *pMYKI_TAControl     = NULL;
    MYKI_CAControl_t       *pCAControl          = NULL;

    if ( pData == NULL )
    {
        return FALSE;
    }   /* end-of-if */

    if ( MYKI_CS_TAControlGet( &pMYKI_TAControl ) < 0 || pMYKI_TAControl == NULL )
    {
        return FALSE;
    }
    
    if ( MYKI_CS_CAControlGet(&pCAControl) != MYKI_CS_OK )
    {
        return FALSE;
    }
    pMYKI_TAControl->Status                     = TAPP_CONTROL_STATUS_ACTIVATED;
    pCAControl->Status                          = CARD_CONTROL_STATUS_ACTIVATED;

    if ( MYKI_CS_TACappingGet( &pMYKI_TACapping ) < 0 || pMYKI_TACapping == NULL )
    {
        return FALSE;
    }   /* end-of-if */

    /*  Pre-Conditions:
            TACapping.Daily.Expiry  < DynamicData.CurrentBusinessDate
            TACapping.Weekly.Expiry < DynamicData.CurrentBusinessDate

        Post-Conditions:
            EXECUTED
            Reset TACapping.Daily
            Reset TACapping.Weekly
    */
    {
        pMYKI_TACapping->Daily.Expiry       = pData->DynamicData.currentBusinessDate - 1;
        pMYKI_TACapping->Weekly.Expiry      = pData->DynamicData.currentBusinessDate - 1;

        if ( BR_LLSC_1_10( pData ) != RULE_RESULT_EXECUTED )
        {
            return FALSE;
        }   /* end-of-if */
    }

    return TRUE;
}   /* test_BR_LLSC_1_10_004( ) */

/*==========================================================================*
**
**  test_BR_LLSC_1_10_005
**
**  Description     :
**      EXCUTED
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

int test_BR_LLSC_1_10_005( MYKI_BR_ContextData_t *pData )
{
    MYKI_TACapping_t       *pMYKI_TACapping     = NULL;
    MYKI_TAControl_t       *pMYKI_TAControl     = NULL;
    MYKI_CAControl_t       *pCAControl          = NULL;

    if ( pData == NULL )
    {
        return FALSE;
    }   /* end-of-if */

    if ( MYKI_CS_TAControlGet( &pMYKI_TAControl ) < 0 || pMYKI_TAControl == NULL )
    {
        return FALSE;
    }
    
    if ( MYKI_CS_CAControlGet(&pCAControl) != MYKI_CS_OK )
    {
        return FALSE;
    }
    pMYKI_TAControl->Status                     = TAPP_CONTROL_STATUS_ACTIVATED;
    pCAControl->Status                          = CARD_CONTROL_STATUS_ACTIVATED;

    if ( MYKI_CS_TACappingGet( &pMYKI_TACapping ) < 0 || pMYKI_TACapping == NULL )
    {
        return FALSE;
    }   /* end-of-if */

    /*  Pre-Conditions:
            TACapping.Daily.Expiry  > DynamicData.CurrentBusinessDate
            TACapping.Weekly.Expiry < DynamicData.CurrentBusinessDate
            DynamicData.CurrentBusinessDate is Sunday.
        Post-Conditions:
            Weekly.Expiry   = DynamicData.CurrentBusinessDate + 1
    */
    {
        pData->DynamicData.currentBusinessDate  = 2821;     /* Sunday 22/09/2013 (since 01/01/2006) */
        pMYKI_TACapping->Daily.Expiry           = pData->DynamicData.currentBusinessDate + 1;
        pMYKI_TACapping->Weekly.Expiry          = pData->DynamicData.currentBusinessDate - 1;

        CsDbg( BRLL_RULE, "test_BR_LLSC_1_10_005() expected Weekly.Expiry = %d", pData->DynamicData.currentBusinessDate + 1 );

        if ( BR_LLSC_1_10( pData ) != RULE_RESULT_EXECUTED )
        {
            return FALSE;
        }   /* end-of-if */
    }

    /*  Pre-Conditions:
            TACapping.Daily.Expiry  > DynamicData.CurrentBusinessDate
            TACapping.Weekly.Expiry < DynamicData.CurrentBusinessDate
            DynamicData.CurrentBusinessDate is Saturday.
        Post-Conditions:
            Weekly.Expiry   = DynamicData.CurrentBusinessDate + 2
    */
    {
        pData->DynamicData.currentBusinessDate  = 2820;     /* Saturday 21/09/2013 (since 01/01/2006) */
        pMYKI_TACapping->Daily.Expiry           = pData->DynamicData.currentBusinessDate + 1;
        pMYKI_TACapping->Weekly.Expiry          = pData->DynamicData.currentBusinessDate - 1;

        CsDbg( BRLL_RULE, "test_BR_LLSC_1_10_005() expected Weekly.Expiry = %d", pData->DynamicData.currentBusinessDate + 2 );

        if ( BR_LLSC_1_10( pData ) != RULE_RESULT_EXECUTED )
        {
            return FALSE;
        }   /* end-of-if */
    }

    /*  Pre-Conditions:
            TACapping.Daily.Expiry  > DynamicData.CurrentBusinessDate
            TACapping.Weekly.Expiry < DynamicData.CurrentBusinessDate
            DynamicData.CurrentBusinessDate is Friday.
        Post-Conditions:
            Weekly.Expiry   = DynamicData.CurrentBusinessDate + 3
    */
    {
        pData->DynamicData.currentBusinessDate  = 2819;     /* Friday 20/09/2013 (since 01/01/2006) */
        pMYKI_TACapping->Daily.Expiry           = pData->DynamicData.currentBusinessDate + 1;
        pMYKI_TACapping->Weekly.Expiry          = pData->DynamicData.currentBusinessDate - 1;

        CsDbg( BRLL_RULE, "test_BR_LLSC_1_10_005() expected Weekly.Expiry = %d", pData->DynamicData.currentBusinessDate + 3 );

        if ( BR_LLSC_1_10( pData ) != RULE_RESULT_EXECUTED )
        {
            return FALSE;
        }   /* end-of-if */
    }

    /*  Pre-Conditions:
            TACapping.Daily.Expiry  > DynamicData.CurrentBusinessDate
            TACapping.Weekly.Expiry < DynamicData.CurrentBusinessDate
            DynamicData.CurrentBusinessDate is Thursday.
        Post-Conditions:
            Weekly.Expiry   = DynamicData.CurrentBusinessDate + 4
    */
    {
        pData->DynamicData.currentBusinessDate  = 2818;     /* Thursday 19/09/2013 (since 01/01/2006) */
        pMYKI_TACapping->Daily.Expiry           = pData->DynamicData.currentBusinessDate + 1;
        pMYKI_TACapping->Weekly.Expiry          = pData->DynamicData.currentBusinessDate - 1;

        CsDbg( BRLL_RULE, "test_BR_LLSC_1_10_005() expected Weekly.Expiry = %d", pData->DynamicData.currentBusinessDate + 4 );

        if ( BR_LLSC_1_10( pData ) != RULE_RESULT_EXECUTED )
        {
            return FALSE;
        }   /* end-of-if */
    }

    /*  Pre-Conditions:
            TACapping.Daily.Expiry  > DynamicData.CurrentBusinessDate
            TACapping.Weekly.Expiry < DynamicData.CurrentBusinessDate
            DynamicData.CurrentBusinessDate is Wednesday.
        Post-Conditions:
            Weekly.Expiry   = DynamicData.CurrentBusinessDate + 5
    */
    {
        pData->DynamicData.currentBusinessDate  = 2817;     /* Wednesday 18/09/2013 (since 01/01/2006) */
        pMYKI_TACapping->Daily.Expiry           = pData->DynamicData.currentBusinessDate + 1;
        pMYKI_TACapping->Weekly.Expiry          = pData->DynamicData.currentBusinessDate - 1;

        CsDbg( BRLL_RULE, "test_BR_LLSC_1_10_005() expected Weekly.Expiry = %d", pData->DynamicData.currentBusinessDate + 5 );

        if ( BR_LLSC_1_10( pData ) != RULE_RESULT_EXECUTED )
        {
            return FALSE;
        }   /* end-of-if */
    }

    /*  Pre-Conditions:
            TACapping.Daily.Expiry  > DynamicData.CurrentBusinessDate
            TACapping.Weekly.Expiry < DynamicData.CurrentBusinessDate
            DynamicData.CurrentBusinessDate is Tuesday.
        Post-Conditions:
            Weekly.Expiry   = DynamicData.CurrentBusinessDate + 6
    */
    {
        pData->DynamicData.currentBusinessDate  = 2816;     /* Tuesday 17/09/2013 (since 01/01/2006) */
        pMYKI_TACapping->Daily.Expiry           = pData->DynamicData.currentBusinessDate + 1;
        pMYKI_TACapping->Weekly.Expiry          = pData->DynamicData.currentBusinessDate - 1;

        CsDbg( BRLL_RULE, "test_BR_LLSC_1_10_005() expected Weekly.Expiry = %d", pData->DynamicData.currentBusinessDate + 6 );

        if ( BR_LLSC_1_10( pData ) != RULE_RESULT_EXECUTED )
        {
            return FALSE;
        }   /* end-of-if */
    }

    /*  Pre-Conditions:
            TACapping.Daily.Expiry  > DynamicData.CurrentBusinessDate
            TACapping.Weekly.Expiry < DynamicData.CurrentBusinessDate
            DynamicData.CurrentBusinessDate is Monday.
        Post-Conditions:
            Weekly.Expiry   = DynamicData.CurrentBusinessDate + 7
    */
    {
        pData->DynamicData.currentBusinessDate  = 2815;     /* Monday 16/09/2013 (since 01/01/2006) */
        pMYKI_TACapping->Daily.Expiry           = pData->DynamicData.currentBusinessDate + 1;
        pMYKI_TACapping->Weekly.Expiry          = pData->DynamicData.currentBusinessDate - 1;

        CsDbg( BRLL_RULE, "test_BR_LLSC_1_10_005() expected Weekly.Expiry = %d", pData->DynamicData.currentBusinessDate + 7 );

        if ( BR_LLSC_1_10( pData ) != RULE_RESULT_EXECUTED )
        {
            return FALSE;
        }   /* end-of-if */
    }

    return TRUE;
}   /* test_BR_LLSC_1_10_005( ) */

