/* -*- mode: c; tabs: 4; -*- */
/*=============================================================================
**
**    Vix Technology                   Licensed software
**    (C) 2015                         All rights reserved
**
**=============================================================================
**
**  Project/Product : NGBU
**  Filename        : BR_VIX_OSC_3_2.c
**  Author(s)       : An Tran
**
**  Description     :
*/
/**     @file
**      @brief      BR_VIX_OSC_3_2 (Reset Shift Data) business rule
**      @section    Section_1 Data Fields
**                  1.  OAppShiftDataControl.Status
**                  2.  OAppShiftDataControl.ActiveRecordCount
**                  3.  OAppShiftDataControl.ShiftId
**                  4.  OAppShiftDataControl.ShiftSeqNo
**                  5.  OAppShiftDataControl.StartTime
**                  6.  OAppShiftDataControl.CloseTime
**                  7.  OAppShiftDataRecord[n]
**                  8.  StaticData.NumberOfHoursBetweenShifts
**                  9.  StaticData.MaxShiftDownTime
**                  10. DynamicData.CurrentDateTime
**                  11. DynamicData.ShiftData
**                  12. DynamicData.ShiftData.ShiftPortionStatus
**      @section    Section_2 Pre-conditions
**                  -#  Shift data buffer (11) is provided
**                  -#  Any of the following conditions is true,
**                      -#  The shift data status (1) is "Unused",
**                      -#  The shift start time (5) plus the number of hours
**                          between shifts (8) is earlier than the current
**                          date/time (10),
**                      -#  This shift close time (6) is set and the shift close
**                          time (6) plus the maximum shift down time (9) is
**                          earlier than then current date/time (10).
**      @section    Section_3 Description
**                  -#  Perform OAppShiftDataControlUpdate/None,
**                      -#  Set shift data status (1) to "Activated".
**                      -#  Set shift identification (3) to zero.
**                      -#  Set shift sequence number (4) to zero.
**                      -#  Set start time (5) to the current date/time (10).
**                      -#  Set close time (6) to zero.
**                  -#  Reset Driver shift data (11).
**                  -#  Set shift portion status (12) to "Reset".
**      @section    Section_4 Post-conditions
**                  -#  New Driver shift started.
*/
/*  Member(s)       :
**      BR_VIX_OSC_3_2          [public]    business rule
**
**  Information     :
**   Compiler(s)    : C++
**   Target(s)      : Independent
**
**  Subversion      :
**      $Id: BR_VIX_OSC_3_2.c 88449 2016-01-07 00:32:47Z atran $
**      $HeadURL: https://auperasvn01.aupera.erggroup.com/svn/DPG_SWBase/myki-br/trunk/src/BR_VIX_OSC_3_2.c $
**
**  History         :
**   Vers.  Date        Aut.  Type     Description
**   -----  ----------  ----  -------  ----------------------------------------
**    1.00  27.10.15    ANT   Create
**    1.01  27.11.15    ANT   Modify   NGBU-897: Rectified ShiftId as binary
**
**===========================================================================*/

/*
 *      Includes
 *      --------
 */

#include <cs.h>
#include <myki_cardservices.h>
#include "myki_br_rules.h"
#include "BR_Common.h"

    /**
     *  @brief  BR_VIX_OSC_3_2 business rule.
     *  @param  pData business rule context data.
     *  @return RULE_RESULT_EXECUTED if business rule executed,\n
     *          RULE_RESULT_BYPASSED if business rule bypassed, or\n
     *          RULE_RESULT_ERROR if unexpected error occurred while executing
     *          business rule.
     */
RuleResult_e
BR_VIX_OSC_3_2( MYKI_BR_ContextData_t *pData )
{
    MYKI_OAShiftDataControl_t      *pMYKI_OAShiftDataControl    = NULL;
    MYKI_BR_ShiftData_t            *pShiftData                  = NULL;
    int                             nResult                     = 0;

    CsDbg( BRLL_RULE, "BR_VIX_OSC_3_2 : Start (Reset Shift Data)" );

    if ( pData == NULL )
    {
        CsErrx( "BR_VIX_OSC_3_2 : Invalid parameter" );
        return  RULE_RESULT_ERROR;
    }

    if ( ( nResult = MYKI_CS_OAShiftDataControlGet( &pMYKI_OAShiftDataControl ) ) != MYKI_CS_OK )
    {
        CsErrx( "BR_VIX_OSC_3_1 : MYKI_CS_OAShiftDataControlGet() failed (%d)", nResult );
        return  RULE_RESULT_ERROR;
    }

    /*  PRE-CONDITIONS */
    {
        /*  1.  Shift data buffer (11) is provided */
        if ( ( pShiftData = pData->DynamicData.pShiftData ) == NULL )
        {
            CsDbg( BRLL_RULE, "BR_VIX_OSC_3_2 : Bypassed - Shift data buffer not provided" );
            pData->ReturnedData.bypassCode  = BYPASS_CODE( 3, 2, 1, 0 );
            return  RULE_RESULT_BYPASSED;
        }

        /*  2.  Any of the following conditions is true */
        do
        {
            /*  a.  The shift data status (1) is "Unused", */
            if ( pMYKI_OAShiftDataControl->Status == OAPP_SHIFT_DATA_STATUS_UNUSED )
            {
                break;
            }
            CsDbg( BRLL_RULE, "BR_VIX_OSC_3_2 : OAppShiftDataControl.Status(%d) != UNUSED", pMYKI_OAShiftDataControl->Status );

            /*  b.  The shift start time (5) plus the number of hours between shifts (8)
                    is earlier than the current date/time (10), */
            if ( ( pMYKI_OAShiftDataControl->StartTime + HOURS_TO_SECONDS( pData->StaticData.numberOfHoursBetweenShifts ) ) < pData->DynamicData.currentDateTime )
            {
                break;
            }
            CsDbg( BRLL_RULE, "BR_VIX_OSC_3_2 : OAppShiftDataControl.StartTime(%d) + numberOfHoursBetweenShifts(%d) >= CurrentDateTime(%d)",
                    pMYKI_OAShiftDataControl->StartTime,
                    pData->StaticData.numberOfHoursBetweenShifts,
                    pData->DynamicData.currentDateTime );

            /*  c.  This shift close time (6) is set and the shift close time (6)
                    plus the maximum shift down time (9) is earlier than then
                    current date/time (10). */
            if ( pMYKI_OAShiftDataControl->CloseTime != TIME_NOT_SET )
            {
                if ( ( pMYKI_OAShiftDataControl->CloseTime + HOURS_TO_SECONDS( pData->StaticData.maxShiftDownTime ) ) < pData->DynamicData.currentDateTime )
                {
                    break;
                }
                CsDbg( BRLL_RULE, "BR_VIX_OSC_3_2 : OAppShiftDataControl.CloseTime(%d) + MaxShiftDownTime(%d) >= CurrentDateTime(%d)",
                        pMYKI_OAShiftDataControl->CloseTime,
                        pData->StaticData.maxShiftDownTime,
                        pData->DynamicData.currentDateTime );
            }
            else
            {
                CsDbg( BRLL_RULE, "BR_VIX_OSC_3_2 : OAppShiftDataControl.CloseTime == TIME_NOT_SET" );
            }
            CsDbg( BRLL_RULE, "BR_VIX_OSC_3_2 : Bypassed" );
            pData->ReturnedData.bypassCode  = BYPASS_CODE( 3, 2, 2, 0 );
            return  RULE_RESULT_BYPASSED;
        }
        while ( 0 /* ONCE_ONLY */ );
    }

    /*  PROCESSING */
    {
        /*  1.  Perform OAppShiftDataControlUpdate/None,
                a.  Set shift data status (1) to "Activated".
                b.  Set shift identification (3) to zero.
                c.  Set shift sequence number (4) to zero.
                d.  Set start time (5) to the current date/time (10).
                e.  Set close time (6) to zero. */
        pShiftData->shiftStatus         = OAPP_SHIFT_DATA_STATUS_ACTIVATED;
        pShiftData->shiftId             = 0;
        pShiftData->shiftStartTime      = pData->DynamicData.currentDateTime;
        pShiftData->shiftEndTime        = TIME_NOT_SET;
        if ( myki_br_ldt_OAppShiftControlUpdate( pData, pMYKI_OAShiftDataControl->ActiveRecordCount ) < 0 )
        {
            CsErrx( "BR_VIX_OSC_3_2 : myki_br_ldt_OAppShiftControlUpdate() failed" );
            return  RULE_RESULT_ERROR;
        }

        /*  2.  Reset Driver shift data (11). */
        pShiftData->paperTicketReturns                  = 0;
        pShiftData->sundryItemReturns                   = 0;
        pShiftData->cardReturns                         = 0;
        pShiftData->shiftTotalsTPurse.RecordType        = PAYMENT_METHOD_TPURSE;
        pShiftData->shiftTotalsTPurse.SalesCount        = 0;
        pShiftData->shiftTotalsTPurse.SalesValue        = 0;
        pShiftData->shiftTotalsTPurse.ReversalsCount    = 0;
        pShiftData->shiftTotalsTPurse.ReversalsValue    = 0;
        pShiftData->shiftTotalsCash.RecordType          = PAYMENT_METHOD_CASH;
        pShiftData->shiftTotalsCash.SalesCount          = 0;
        pShiftData->shiftTotalsCash.SalesValue          = 0;
        pShiftData->shiftTotalsCash.ReversalsCount      = 0;
        pShiftData->shiftTotalsCash.ReversalsValue      = 0;

        /*  3.  Set shift portion status (12) to "Reset". */
        pShiftData->shiftPortionStatus                  = SHIFT_PORTION_RESET;
    }

    /*  POST-CONDITIONS */
    {
        /*  1.  Shift portion is reset.
            2.  New Driver shift started. */
    }

    CsDbg( BRLL_RULE, "BR_VIX_OSC_3_2 : Executed" );

    return  RULE_RESULT_EXECUTED;
}
