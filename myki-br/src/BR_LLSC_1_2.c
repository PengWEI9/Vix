/**************************************************************************
*   ID :  BR_LLSC_1_2
*
*   Maintain Capping Counters
*   1.  TAppCapping.Daily.Expiry
*   2.  TAppCapping.Daily.Value
*   3.  TAppCapping.Daily.ZoneLow
*   4.  TAppCapping.Daily.ZoneHigh
*   5.  TAppCapping.Weekly.Expiry
*   6.  TAppCapping.Weekly.Value
*   7.  TAppCapping.Weekly.ZoneLow
*   8.  TAppCapping.Weekly.ZoneHigh
*   9.  Dynamic.CurrentDateTime
*   10. Dynamic.CurrentTripZoneLow
*   11. Dynamic.CurrentTripZoneHigh
*   12. Tariff.ApplicableFare
*
*   13. Tariff.EndOfBusinessDay
*   14. Tariff.CitySaverZoneFlag
*   15. TAppControl.ProductInUse
*   16. TAppTProduct.EndDateTime
*   17. Dynamic.CappingContribution
*   18. Dynamic.PreviousDailyCapZoneLow
*   19. Dynamic.PreviousDailyCapZoneHigh
*   20. Dynamic.CappingEpassZoneLow
*   21. Dynamic.CappingEpassZoneHigh
*   22. Dynamic.IsForcedScanOff
*   23. Dynamic.IsEarlyBirdTrip
*   24. Dynamic.ForceScanOffDateTime
*
*    ==========================================================================
*    David Purdie (16-Jan-2014)
*    I disagree with the BR definition that I currently have.
*    The Gold Test results cannot match the provided definitions
*    I'm assuming ( based on observation) that
*        * Precondition 1 is valid
*        * Description is wrong and should include words to indicate
*            * Always perform a Perform a CappingUpdate/None transaction
*            * Return RULE_RESULT_BYPASSED if no change was made to the Capping Object
*    ==========================================================================
*
*   Pre Condition
*   1.  The current trip is not an early bird trip (ie IsEarlyBirdTrip is false)
*
*
*       Description
*   1.  Perform a CappingUpdate/None transaction
*   2.  Copy the existing capping Daily capping zone low3 and Daily capping zone high4 to previous Daily capping zone low18 and previous capping zone
*       high19.
*
*   3.  If the Daily capping zone low3 is not equal to either the: low zone of the current trip10 or  capping e-pass zone low20  ;or if the Daily capping zone high4 is
*       not equal to either the: high zone of the current trip11 or capping e-pass zone high21  or if the capping contribution amount17 is not equal to zero then:
*       a.  If the low daily capping zone3 is zero (0); then set low daily capping zone3  to EpassCappingZoneLow20  else set low daily capping zone3 to lower of
*           low daily capping zone3 and  CappingEpassZoneLow2
*       b.  Set  high daily capping zone4  to higher of high daily capping zone4  and CappingEpassZoneHigh21
*       c.  If the high zone of the current trip11 is less than the low daily capping zone3, or the low daily capping zone3 is 0, set the low daily capping zone3 to
*           the high zone of the current trip11.
*       d.  If the low zone of the current trip10 is less than the low daily capping zone3, or the low daily capping zone3 is 0, set the low daily capping zone3 to
*           the low zone of the current trip10.
*       e.  If the low zone of the current trip10 is greater than the high daily capping zone4, or the high daily capping zone4 is 0, set the high daily capping
*           zone4 to the low zone of the current trip10.
*       f.  If the high zone of the current trip11 is greater than the high daily capping zone4, or the high daily capping zone4 is 0, set the high daily capping
*           zone4 to the high zone of the current trip11.
*       g.  If the capping contribution17 is not equal to zero; update daily capping fare total2 by the capping contribution17
*       h.  If the low daily capping zone3 minus 1 is marked as City Saver14 decrement the low daily capping zone3 by 1.
*       i.  If the high daily capping zone4 is marked as City Saver14 increment the high daily capping zone4 by 1.
*
*   4.  If the Weekly capping zone low7 is not equal to either the: low zone of the current trip10 or capping e-pass zone low20 ; or if the Weekly capping zone
*       high4 is not equal to either the: high zone of the current trip11 or capping e-pass zone high21 ; or if  the capping contribution amount17 is not equal to zero
*       then:
*       a.  If the low weekly capping zone 7 is zero(0)  then set the low weekly capping zone7 to the EpassCappingZoneLow20 else set low weekly  capping
*           zone7 to lower of low weekly capping zone7and  CappingEpassZoneLow20
*       b.  Set  high weekly  capping zone4  to higher of high weekly capping zone8  and CappingEpassZoneHigh21
*       c.  If the high zone of the current trip11 is less than the low weekly capping zone7, or the low weekly capping zone7 is 0, set the low weekly capping
*           zone7 to the high zone of the current trip11.
*       d.  If the low zone of the current trip10 is less than the low weekly capping zone7, or the low weekly capping zone7 is 0, set the low weekly capping
*           zone7 to the low zone of the current trip10.
*       e.  If the low zone of the current trip10 is greater than the high weekly capping zone8, or the high weekly capping zone8 is 0, set the high weekly
*           capping zone8 to the low zone of the current trip10.
*       f.  If the high zone of the current trip11 is greater than the high weekly capping zone8, or the high weekly capping zone8 is 0, set the high weekly
*           capping zone8 to the high zone of the current trip11.
*       g.  If the capping contribution17 is not equal to zero 0 then increase the Weekly capping fare total6 by the  capping contribution17 amount
*       h.  If the low weekly capping zone7  less 1 is marked as City Saver14 decrement the low weekly capping zone7 by 1,
*       i.  If the high weekly capping zone8 is marked as City Saver14 increment the high weekly capping zone8 by 1.
*
*   Post-Conditions
*       The daily and weekly capping counters have been updated.
*
*   Devices
*       Fare payment devices
*
 ***********************************************************************/

#include <cs.h>                 // For debug logging
#include <myki_cardservices.h>  // Myki Card Services
#include <LDT.h>                // Logical Device Transaction

#include "myki_br_rules.h"      // API for all BR_xxx modules
#include "BR_Common.h"          // Common utilities, macros, etc

//=============================================================================
//
//  Repeat the following for the Daily counters, then the Weekly counters
//
//=============================================================================

RuleResult_e BR_LLSC_1_2( MYKI_BR_ContextData_t *pData )
{
    MYKI_TACapping_t            *pMYKI_TACapping        = NULL;
    MYKI_TACapping_t            *pMYKI_TACapping_Old    = NULL;
    MYKI_TACapping_t            MYKI_TACapping_New;
    int                         isDailyUpdate           = FALSE;
    int                         isWeeklyUpdate          = FALSE;
    int                         isChanged               = FALSE;
    Currency_t                  newCappingValue         = 0;

    CsDbg( BRLL_RULE, "BR_LLSC_1_2 : Start (Maintain Capping Counters)" );

    if ( ! pData )
    {
        CsErrx( "BR_LLSC_1_2 : Invalid argument(s)" );
        return RULE_RESULT_ERROR;
    }

    //
    //  Check Pre-conditions
    //

//  1. The current trip is not an early bird trip (ie IsEarlyBirdTrip is false)

    if ( pData->DynamicData.isEarlyBirdTrip )      // Bypass if this *is* an early bird trip
    {
        CsDbg( BRLL_RULE, "BR_LLSC_1_2 : Bypass - pData->DynamicData.isEarlyBirdTrip" );
        return RULE_RESULT_BYPASSED;
    }

    //
    //  Execute Rule
    //

    if ( MYKI_CS_TACappingGet( &pMYKI_TACapping ) < 0 )
    {
        CsErrx( "BR_LLSC_1_2 : MYKI_CS_TACappingGet() failed" );
        return RULE_RESULT_ERROR;
    }

    /*  NOTE:   Making changes to local copy of TAppCapping */
    memcpy( &MYKI_TACapping_New, pMYKI_TACapping, sizeof( MYKI_TACapping_New ) );
    pMYKI_TACapping_Old = pMYKI_TACapping;
    pMYKI_TACapping = &MYKI_TACapping_New;

    //  1.  Perform a CappingUpdate/None transaction (See below)

    //  2.  Copy the existing capping Daily capping zone low3 and Daily capping zone high4 to previous Daily capping zone low18 and previous capping zone high19.

    pData->DynamicData.previousDailyCapZoneLow  = pMYKI_TACapping->Daily.Zone.Low;
    pData->DynamicData.previousDailyCapZoneHigh = pMYKI_TACapping->Daily.Zone.High;

    CsDbg( BRLL_RULE, "BR_LLSC_1_2 Daily Lows : %d, %d, %d", pMYKI_TACapping->Daily.Zone.Low, pData->DynamicData.currentTripZoneLow, pData->DynamicData.cappingEpassZoneLow );
    CsDbg( BRLL_RULE, "BR_LLSC_1_2 Daily Highs: %d, %d, %d", pMYKI_TACapping->Daily.Zone.High, pData->DynamicData.currentTripZoneHigh, pData->DynamicData.cappingEpassZoneHigh );
    CsDbg( BRLL_RULE, "BR_LLSC_1_2 DynamicData.cappingContribution:%d", pData->DynamicData.cappingContribution);

    if
    (
        (
            //  3.  If the Daily capping zone low3 is not equal to either the: low zone of the current trip10
            //      or capping e-pass zone low20;
            ( pMYKI_TACapping->Daily.Zone.Low != pData->DynamicData.currentTripZoneLow  ) ||
            ( pMYKI_TACapping->Daily.Zone.Low != pData->DynamicData.cappingEpassZoneLow )
        ) ||
        (
            //      or if the Daily capping zone high4 is not equal to either the: high zone of the current trip11
            //      or capping e-pass zone high21
            ( pMYKI_TACapping->Daily.Zone.High != pData->DynamicData.currentTripZoneHigh  ) ||
            ( pMYKI_TACapping->Daily.Zone.High != pData->DynamicData.cappingEpassZoneHigh )
        ) ||
        (
            //      or if the capping contribution amount17 is not equal to zero then:
            ( pData->DynamicData.cappingContribution != 0 )
        )
    )
    {
        //  Note: Reversed (e) and (f) to keep the code in the same order as (c) and (d)

        //  a. If the low daily capping zone3 is zero (0); then set low daily capping zone3  to EpassCappingZoneLow20  else set low daily capping zone3 to lower of
        //  low daily capping zone3 and  CappingEpassZoneLow2

        if ( ( pMYKI_TACapping->Daily.Zone.Low == 0 ) || ( pMYKI_TACapping->Daily.Zone.Low > pData->DynamicData.cappingEpassZoneLow ) )
        {
            pMYKI_TACapping->Daily.Zone.Low = pData->DynamicData.cappingEpassZoneLow;
        }

        //  b. Set  high daily capping zone4  to higher of high daily capping zone4  and CappingEpassZoneHigh21
        //  Note: Added check for high daily capping zone == 0 for consistency with other checks in this rule - it makes no differene to the result

        if ( ( pMYKI_TACapping->Daily.Zone.High == 0 ) || ( pMYKI_TACapping->Daily.Zone.High < pData->DynamicData.cappingEpassZoneHigh ) )
        {
            pMYKI_TACapping->Daily.Zone.High = pData->DynamicData.cappingEpassZoneHigh;
        }

        //  c. If the high zone of the current trip11 is less than the low daily capping zone3, or the low daily capping zone3 is 0, set the low daily capping zone3 to
        //  the high zone of the current trip11.
        //  Note: From this point on neither the low nor high daily capping zone should be (0) as they should have been set to a real zone in the checks above

        if ( ( pMYKI_TACapping->Daily.Zone.Low == 0 ) || ( pMYKI_TACapping->Daily.Zone.Low > pData->DynamicData.currentTripZoneHigh ) )
        {
            pMYKI_TACapping->Daily.Zone.Low = pData->DynamicData.currentTripZoneHigh;
        }

        //  d. If the low zone of the current trip10 is less than the low weekly capping zone7, or the low weekly capping zone7 is 0, set the low weekly capping
        //  zone7 to the low zone of the current trip10.

        if ( ( pMYKI_TACapping->Daily.Zone.Low == 0 ) || ( pMYKI_TACapping->Daily.Zone.Low > pData->DynamicData.currentTripZoneLow ) )
        {
            pMYKI_TACapping->Daily.Zone.Low = pData->DynamicData.currentTripZoneLow;
        }

        //  f. If the high zone of the current trip11 is greater than the high daily capping zone4, or the high daily capping zone4 is 0, set the high daily capping
        //  zone4 to the high zone of the current trip11.

        if ( ( pMYKI_TACapping->Daily.Zone.High == 0 ) || ( pMYKI_TACapping->Daily.Zone.High < pData->DynamicData.currentTripZoneHigh ) )
        {
            pMYKI_TACapping->Daily.Zone.High = pData->DynamicData.currentTripZoneHigh;
        }

        //  e. If the low zone of the current trip10 is greater than the high daily capping zone4, or the high daily capping zone4 is 0, set the high daily capping
        //  zone4 to the low zone of the current trip10.

        if ( ( pMYKI_TACapping->Daily.Zone.High == 0 ) || ( pMYKI_TACapping->Daily.Zone.High < pData->DynamicData.currentTripZoneLow ) )
        {
            pMYKI_TACapping->Daily.Zone.High = pData->DynamicData.currentTripZoneLow;
        }

        //  g. If the capping contribution17 is not equal to zero 0 then increase the daily capping fare total6 by the  capping contribution17 amount

        newCappingValue                 = (Currency_t)pMYKI_TACapping->Daily.Value;
        newCappingValue                += pData->DynamicData.cappingContribution;
        pMYKI_TACapping->Daily.Value    = (U32_t)( newCappingValue > 0 ? newCappingValue : 0 );

        //  h. If the low daily capping zone7  less 1 is marked as City Saver14 decrement the low daily capping zone7 by 1,
        //  i. If the high daily capping zone8 is marked as City Saver14 increment the high daily capping zone8 by 1.

        ADJUST_FOR_CITYSAVER( pMYKI_TACapping->Daily.Zone.Low, pMYKI_TACapping->Daily.Zone.High );

        isDailyUpdate   = TRUE;
    }

    CsDbg( BRLL_RULE, "BR_LLSC_1_2 Weekly Lows : %d, %d, %d", pMYKI_TACapping->Weekly.Zone[ 0 ].Low, pData->DynamicData.currentTripZoneLow, pData->DynamicData.cappingEpassZoneLow);
    CsDbg( BRLL_RULE, "BR_LLSC_1_2 Weekly Highs: %d, %d, %d", pMYKI_TACapping->Weekly.Zone[ 0 ].High,  pData->DynamicData.currentTripZoneHigh, pData->DynamicData.cappingEpassZoneHigh);
    CsDbg( BRLL_RULE, "BR_LLSC_1_2 DynamicData.cappingContribution:%d", pData->DynamicData.cappingContribution);

    if
    (
        (
            //  4.  If the Weekly capping zone low3 is not equal to either the: low zone of the current trip10
            //      or capping e-pass zone low20;
            ( pMYKI_TACapping->Weekly.Zone[ 0 ].Low != pData->DynamicData.currentTripZoneLow  ) ||
            ( pMYKI_TACapping->Weekly.Zone[ 0 ].Low != pData->DynamicData.cappingEpassZoneLow )
        ) ||
        (
            //      or if the Weekly capping zone high4 is not equal to either the: high zone of the current trip11
            //      or capping e-pass zone high21
            ( pMYKI_TACapping->Weekly.Zone[ 0 ].High != pData->DynamicData.currentTripZoneHigh  ) ||
            ( pMYKI_TACapping->Weekly.Zone[ 0 ].High != pData->DynamicData.cappingEpassZoneHigh )
        ) ||
        (
            //      or if the capping contribution amount17 is not equal to zero then:
            ( pData->DynamicData.cappingContribution != 0 )
        )
    )
    {
        //  Note: Reversed (e) and (f) to keep the code in the same order as (c) and (d)

        //  a. If the low weekly capping zone3 is zero (0); then set low weekly capping zone3  to EpassCappingZoneLow20  else set low weekly capping zone3 to lower of
        //  low weekly capping zone3 and  CappingEpassZoneLow2

        if ( pMYKI_TACapping->Weekly.Zone[ 0 ].Low == 0 )
        {
            pMYKI_TACapping->Weekly.Zone[ 0 ].Low = pData->DynamicData.cappingEpassZoneLow;
        }
        else if ( pMYKI_TACapping->Weekly.Zone[ 0 ].Low > pData->DynamicData.cappingEpassZoneLow )
        {
            pMYKI_TACapping->Weekly.Zone[ 0 ].Low = pData->DynamicData.cappingEpassZoneLow;
        }

        //  b. Set  high weekly capping zone4  to higher of high weekly capping zone4  and CappingEpassZoneHigh21

        if ( pMYKI_TACapping->Weekly.Zone[ 0 ].High < pData->DynamicData.cappingEpassZoneHigh )
        {
            pMYKI_TACapping->Weekly.Zone[ 0 ].High = pData->DynamicData.cappingEpassZoneHigh;
        }

        //  c. If the high zone of the current trip11 is less than the low weekly capping zone3, or the low weekly capping zone3 is 0, set the low weekly capping zone3 to
        //  the high zone of the current trip11.

        if ( ( pMYKI_TACapping->Weekly.Zone[ 0 ].Low == 0 ) || ( pMYKI_TACapping->Weekly.Zone[ 0 ].Low > pData->DynamicData.currentTripZoneHigh ) )
        {
            pMYKI_TACapping->Weekly.Zone[ 0 ].Low = pData->DynamicData.currentTripZoneHigh;
        }

        //  d. If the low zone of the current trip10 is less than the low weekly capping zone7, or the low weekly capping zone7 is 0, set the low weekly capping
        //  zone7 to the low zone of the current trip10.

        if ( ( pMYKI_TACapping->Weekly.Zone[ 0 ].Low == 0 ) || ( pMYKI_TACapping->Weekly.Zone[ 0 ].Low > pData->DynamicData.currentTripZoneLow ) )
        {
            pMYKI_TACapping->Weekly.Zone[ 0 ].Low = pData->DynamicData.currentTripZoneLow;
        }

        //  f. If the high zone of the current trip11 is greater than the high weekly capping zone4, or the high weekly capping zone4 is 0, set the high weekly capping
        //  zone4 to the high zone of the current trip11.

        if ( ( pMYKI_TACapping->Weekly.Zone[ 0 ].High == 0 ) || ( pMYKI_TACapping->Weekly.Zone[ 0 ].High < pData->DynamicData.currentTripZoneHigh ) )
        {
            pMYKI_TACapping->Weekly.Zone[ 0 ].High = pData->DynamicData.currentTripZoneHigh;
        }

        //  e. If the low zone of the current trip10 is greater than the high weekly capping zone4, or the high weekly capping zone4 is 0, set the high weekly capping
        //  zone4 to the low zone of the current trip10.

        if ( ( pMYKI_TACapping->Weekly.Zone[ 0 ].High == 0 ) || ( pMYKI_TACapping->Weekly.Zone[ 0 ].High < pData->DynamicData.currentTripZoneLow ) )
        {
            pMYKI_TACapping->Weekly.Zone[ 0 ].High = pData->DynamicData.currentTripZoneLow;
        }

        //  g. If the capping contribution17 is not equal to zero 0 then increase the Weekly capping fare total6 by the  capping contribution17 amount

        newCappingValue                 = (Currency_t)pMYKI_TACapping->Weekly.Value;
        newCappingValue                += pData->DynamicData.cappingContribution;
        pMYKI_TACapping->Weekly.Value   = (U32_t)( newCappingValue > 0 ? newCappingValue : 0 );

        //  h. If the low weekly capping zone7  less 1 is marked as City Saver14 decrement the low weekly capping zone7 by 1,
        //  i. If the high weekly capping zone8 is marked as City Saver14 increment the high weekly capping zone8 by 1.

        ADJUST_FOR_CITYSAVER( pMYKI_TACapping->Weekly.Zone[ 0 ].Low, pMYKI_TACapping->Weekly.Zone[ 0 ].High );

        isWeeklyUpdate  = TRUE;
    }


    //
    //  Detect changes to the Capping Object
    //  Must do this before we perform the capping update as myki_br_ldt_PerformCappingUpdate will
    //  update pMYKI_TACapping_Old
    //
    //  The following test may not be sufficient - but given the lack of information in the
    //  document its my best guess.
    //

    if (memcmp(pMYKI_TACapping, pMYKI_TACapping_Old, sizeof(*pMYKI_TACapping) ))
    {
        isChanged = TRUE;
    }


    //  1.  Perform a CappingUpdate/None transaction
    //
    //      Note :  We are currently doing this in all cases even though the document says we should
    //              only do it if the capping object has changed. This is currently waiting on a request
    //              for clarification from ACS.

    if ( myki_br_ldt_PerformCappingUpdate( pData, pMYKI_TACapping, isDailyUpdate, isWeeklyUpdate ) < 0 )
    {
        CsErrx( "BR_LLSC_1_2 : myki_br_ldt_PerformCappingUpdate() failed" );
        return RULE_RESULT_ERROR;
    }

    //
    //  Bypass if capping contribution is not positive - this is purely an optimisation to
    //  avoid having to call about half a dozen subsequent rules which are guaranteed not to do
    //  anything if the capping contribution is less than or equal to zero.
    //

    if ( pData->DynamicData.cappingContribution <= 0 )
    {
        CsDbg( BRLL_RULE, "BR_LLSC_1_2 : Bypass - Capping Contribution (%d) <= 0", pData->DynamicData.cappingContribution );
        return RULE_RESULT_BYPASSED;
    }

    //  All done - return executed

    CsDbg( BRLL_RULE, "BR_LLSC_1_2 : Executed");
    return RULE_RESULT_EXECUTED;
}

