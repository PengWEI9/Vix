/* -*- mode: c; tabs: 4; -*- */
/*=============================================================================
**
**    Vix Technology                   Licensed software
**    (C) 2013                         All rights reserved
**
**=============================================================================
**
**  Project/Product : MBU
**  Filename        : BR_LLSC_1_10.c
**  Author(s)       : An Tran
**
**  ID              : BR_LLSC_1_10
**
**  Name            : Reset Capping Counters
**
**  Data Fields     :
**
**      1.  TAppCapping.Daily.Expiry        (days from reference date)
**      2.  TAppCapping.Daily.Value
**      3.  TAppCapping.Daily.ZoneLow
**      4.  TAppCapping.Daily.ZoneHigh
**      5.  TAppCapping.Weekly.Expiry       (days from reference date)
**      6.  TAppCapping.Weekly.Value
**      7.  TAppCapping.Weekly.ZoneLow
**      8.  TAppCapping.Weekly.ZoneHigh
**      9.  Dynamic.CurrentDateTime         (seconds since 1/1/1970 00:00:00)
**          Dyanmic.CurrentBusinessDate     (days from reference date)
**      10. Tarrif.EndOfBusinessDay         (minutes since midnight)
**
**  Pre-Conditions  :
**
**      1.  Either the daily capping counter expiry is less than the current
**          date/time (ie the daily capping counter has expired) or the weekly
**          counter expiry is less than the current date/time (ie the weekly
**          capping counter has expired)
**
**  Description     :
**
**      1.  Perform a CappingUpdate /None
**          a.  If the daily capping counter expiry is less than the current
**              date/time:
**              i.  Set the low daily capping zone and high daily capping zone
**                  to 0.
**              ii. Set the daily capping fares total to 0.
**              iii.Set the daily capping counter expiry to end of business day
**                  time on the following day.
**          b.  If the weekly counter expiry is less than the current date/time:
**              i.  Set the low weekly capping zone and high weekly capping zone
**                  to 0.
**              ii. Set the weekly capping fares total to 0.
**              iii.Set the weekly capping counter expiry to end of business day
**                  time of the following Monday.
**
**  Post-Conditions :
**
**      The daily and weekly capping counters have been reset
**
**  Function(s)     :
**      BR_LLSC_1_10                [public]    LLSC_1_10 business rule
**
**  Information     :
**   Compiler(s)    : ANSI C
**   Target(s)      : Independent
**
**  History         :
**   Vers.  Date        Aut.  Type     Description
**   -----  ----------  ----  -------  ----------------------------------------
**    1.00  12.09.13    ANT   Create
**    1.01  29.01.14    ANT   Modify   Rectified TACapping.Daily.Expiry and
**                                     TACapping.Weekly.Expiry checking
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
**  BR_LLSC_1_10
**
**  Description     :
**      Implements business BR_LLSC_1_10.
**
**  Parameters      :
**      pData           [I/O]   BR context data
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

RuleResult_e BR_LLSC_1_10( MYKI_BR_ContextData_t *pData )
{
    MYKI_TACapping_t       *pMYKI_TACapping     = NULL;
    int                     isDailyReset        = FALSE;
    int                     isWeeklyReset       = FALSE;

    CsDbg( BRLL_RULE, "BR_LLSC_1_10 : Start (Reset Capping Counters)" );

    if ( pData == NULL )
    {
        CsErrx( "BR_LLSC_1_10 : Invalid argument(s)" );
        return RULE_RESULT_ERROR;
    }   /* end-of-if */

    if ( MYKI_CS_TACappingGet( &pMYKI_TACapping ) < 0 )
    {
        CsErrx( "BR_LLSC_1_10: MYKI_CS_TACappingGet() failed" );
        return RULE_RESULT_ERROR;
    }   /* end-of-if */

    /* PRE-CONDITIONS */
    {
        /*  1.  Either the daily capping counter expiry is less than the
                current date/time (ie the daily capping counter has expired)
                or the weekly counter expiry is less than the current
                date/time (ie the weekly capping counter has expired) */

        //  NOTE:   TACapping.Daily.Expiry and TACapping.Weekly.Expiry contain an integer
        //          representing the number of days since 01/01/2006. The relevent capping
        //          counter (daily or weekly) expires at the *start* of the business day
        //          indicated by this number. Therefore
        //          (a) When setting this value, set it to the following day (daily) or
        //              the following Monday (weekly)
        //          (b) When comparing it for expiry, it is expired if it is less than
        //              *or equal* to the current date - note that this is different to
        //              the document which says "less than", not "less than or equal".

        CsDbg( BRLL_RULE, "BR_LLSC_1_10 : Daily Expiry (%d), Weekly Expiry (%d), Current (%d)",
            pMYKI_TACapping->Daily.Expiry, pMYKI_TACapping->Weekly.Expiry, pData->DynamicData.currentBusinessDate );

        if ( pMYKI_TACapping->Daily.Expiry <= pData->DynamicData.currentBusinessDate )
        {
            isDailyReset    = TRUE;
        }   /* end-of-if */

        if ( pMYKI_TACapping->Weekly.Expiry <= pData->DynamicData.currentBusinessDate )
        {
            isWeeklyReset   = TRUE;
        }   /* end-of-if */

        if ( isDailyReset == FALSE && isWeeklyReset == FALSE )
        {
            CsDbg( BRLL_RULE, "BR_LLSC_1_10 : BYPASSED - Daily and/or Weekly capping counter not expired" );
            return RULE_RESULT_BYPASSED;
        }   /* end-of-if */
    }

    /* PROCESSING */
    {
        /*  1.  Perform a CappingUpdate /None */
        /*      a.  If the daily capping counter expiry is less than the current date/time: */
        /*          i.  Set the low daily capping zone and high daily capping zone to 0. */
        /*          ii. Set the daily capping fares total to 0. */
        /*          iii.Set the daily capping counter expiry to end of business day time on the following day. */
        /*      b.  If the weekly counter expiry is less than the current date/time: */
        /*          i.  Set the low weekly capping zone and high weekly capping zone to 0. */
        /*          ii. Set the weekly capping fares total to 0. */
        /*          iii.Set the weekly capping counter expiry to end of business day time of the following Monday. */
        if ( myki_br_ldt_PerformCappingReset( pData, isDailyReset, isWeeklyReset ) < 0 )
        {
            CsErrx( "BR_LLSC_1_10 : myki_br_ldt_PerformCappingReset() failed" );
            return RULE_RESULT_ERROR;
        }   /* end-of-if */
    }

    CsDbg( BRLL_RULE, "BR_LLSC_1_10 : Executed" );
    return RULE_RESULT_EXECUTED;
}   /* BR_LLSC_1_10( ) */
