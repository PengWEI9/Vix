/* -*- mode: c; tabs: 4; -*- */
/*=============================================================================
**
**    Vix Technology                   Licensed software
**    (C) 2013                         All rights reserved
**
**=============================================================================
**
**  Project/Product : MBU
**  Filename        : test_BR_LLSC_2_14.c
**  Author(s)       : David Purdie
**  Description     :
**      Implements BR_LLSC_2_14 business rule unit-tests.
**
**  Function(s)     :
**      Template                [Public]    template function
**
**  Information     :
**   Compiler(s)    : ANSI C
**   Target(s)      : Independent
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
#include "BR_Common.h"

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

/*----------------------------------------------------------------------------
** FUNCTION           : runTest
**
** DESCRIPTION        : Run the test and examine the results
**
**
** INPUTS             : pData           - Context Data
**                      rc              - Expected Result Code
**                      bc              - Expected bypass Code
**
** RETURNS            : TRUE        - Test was OK
**                      FALSE       - Test Failed
**
----------------------------------------------------------------------------*/

static int runTest( MYKI_BR_ContextData_t *pData, int rc, int bc)
{
    //ShowCardImage( "test1_before.txt" );
    int rv = BR_LLSC_2_14(pData);

    if ( rv != rc )
    {
        CsVerbose("test_BR_LLSC_2_14 - Unexpected exit code. Got:%d, Expected:%d\n", rv, rc );
        return FALSE;
    }

    if ( pData && pData->ReturnedData.bypassCode != bc)
    {
        CsVerbose("test_BR_LLSC_2_14 - Unexpected ByPassCode. Got:%d, Expected:%d\n", pData->ReturnedData.bypassCode, bc);
        return FALSE;
    }
    return TRUE;
}

/*==========================================================================*
**
**  test_BR_LLSC_2_14_001
**
**  Description     :
**      Process Hotlist By Sam Id
**      Unit-test Execute conditions.
**              Null Data Pointer
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

int test_BR_LLSC_2_14_001(MYKI_BR_ContextData_t *pData)
{
    /*
    **  Run the Test and examine the results
    */
    return runTest(NULL, RULE_RESULT_ERROR, 0);
}

/*==========================================================================*
**
**  test_BR_LLSC_2_14_002
**
**  Description     :
**      Process Hotlist By Sam Id
**      Unit-test Execute conditions.
**              No Matching HostList entry
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

int test_BR_LLSC_2_14_002(MYKI_BR_ContextData_t *pData)
{
    /*
    **  Setup test conditions
    */
    MYKI_TAPurseBalance_t  *pMYKI_TAPurseBalance    = NULL;
    MYKI_TAPurseControl_t  *pMYKI_TAPurseControl    = NULL;

    if ( MYKI_CS_TAPurseBalanceGet( &pMYKI_TAPurseBalance ) < 0 )
    {
        return FALSE;
    }

    if ( MYKI_CS_TAPurseControlGet( &pMYKI_TAPurseControl ) < 0 )
    {
        return FALSE;
    }

    pMYKI_TAPurseControl->LastChangeSamId   = 123456789L;
    MYKI_ACTIONLIST_setDeviceHotlisted(5454L);

    /*
    **  Run the Test and examine the results
    */
    return runTest(pData, RULE_RESULT_BYPASSED, BYPASS_CODE( 2, 14, 1, 0 ));
}

/*==========================================================================*
**
**  test_BR_LLSC_2_14_003
**
**  Description     :
**      Process Hotlist By Sam Id
**      Unit-test Execute conditions.
**              Matching Hot List Entry
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

int test_BR_LLSC_2_14_003(MYKI_BR_ContextData_t *pData)
{

    /*
    **  Setup test conditions
    */
    MYKI_TAPurseBalance_t  *pMYKI_TAPurseBalance    = NULL;
    MYKI_TAPurseControl_t  *pMYKI_TAPurseControl    = NULL;

    if ( MYKI_CS_TAPurseBalanceGet( &pMYKI_TAPurseBalance ) < 0 )
    {
        return FALSE;
    }

    if ( MYKI_CS_TAPurseControlGet( &pMYKI_TAPurseControl ) < 0 )
    {
        return FALSE;
    }

    pMYKI_TAPurseControl->LastChangeSamId   = 123456789L;
    MYKI_ACTIONLIST_setDeviceHotlisted(pMYKI_TAPurseControl->LastChangeSamId);

    CardImage1(pData);
    pData->ActionList.actionlist.tAppUpdate.actionSequenceNo = 10;

    /*
    **  Run the Test and examine the results
    */
    return runTest(pData, RULE_RESULT_EXECUTED, BYPASS_CODE( 2, 14, 255, 0 ));
}


