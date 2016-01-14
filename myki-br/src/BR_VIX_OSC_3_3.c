/* -*- mode: c; tabs: 4; -*- */
/*=============================================================================
**
**    Vix Technology                   Licensed software
**    (C) 2015                         All rights reserved
**
**=============================================================================
**
**  Project/Product : BR_VIX_OSC_3_3
**  Filename        : BR_VIX_OSC_3_3.c
**  Author(s)       : Damian Chiem
**
**  Description     :
*/
/**     @file
**      @brief      BR_VIX_OSC_3_3 (Close Shift Data) business rule.
**      @section    Section_1 Data Fields
**                  1.  OAppRoles.Role[n].Type
**                  2.  OAppRoles.Role[n].Profile
**                  3.  OAppShiftDataControl.Status
**                  4.  OAppShiftDataControl.ActiveRecordCount
**                  5.  OAppShiftDataControl.ShiftId
**                  6.  OAppShiftDataControl.ShiftSeqNo
**                  7.  OAppShiftDataControl.StartTime
**                  8.  OAppShiftDataControl.CloseTime
**                  9.  OAppShiftDataRecord[n]
**                  10. DynamicData.ShiftData
**                  11. DynamicData.ShiftData.ShiftPortionStatus
**                  12. InternalData.ShiftLog
**
**      @section    Section_2 Pre-Conditions
**                  -#  Shift data(10) buffer provided.
**                  -#  Shift portion status (11) is "Brought-Forward" or "Reset".
**      @section    Section_3 Description
**                  -#  For each shift data record in the shift data buffer (10)
**                      with the payment (record) type defined,
**                      -#  If the shift data record (9) for the payment (record)
**                          type exists on the card and, if one or more data fields
**                          of the shift data record in buffer (10) and from the
**                          card (9) is/are different,
**                          -#  Perform OAppShiftDataUpdate/None with the index of
**                              the existing shift data record (9) on the card to
**                              update the shift data record.
**                      -#  If the shift data record (9) for the payment (record)
**                          type does not exist on the card,
**                          -#  Perform OAppShiftDataUpdate/None with the index of
**                              active record count (4) to add shift data record to
**                              card, and Increment the active record count (4)
**                              by one.
**                  -#  Perform OAppShiftDataControlUpdate/None,
**                      -#  Shift Data Status (3) set to "Suspended".
**                      -#  Set active shift data record count (4) to number of
**                          shift data records with valid data,
**                      -#  Set shift identification (5) to value from shift
**                          data buffer (10) and set shift shift sequence
**                          number (6) to one.
**                      -#  Set shift start time (7) and close time (8) to values
**                          from shift data buffer (10).
**                  -#  Modify shift log (11)
**                      -#  Set shift id, shift sequence number with values from shift data buffer (10)
**                      -#  Set sam id with id of the current installed sam,
**                      -#  Set shift start and close time with values from shift data buffer (10),
**                      -#  Set deposit sequence number to zero,
**                      -#  Set deposit total to the shift net cash total, ie. cash sales less cash reversals, and
**                      -#  Set paper ticket returns, sundry item returns and card returns with value from shift data buffer (10).
**
**      @section    Section_4 Post-Conditions
**                  -#  Shift data is updated to Driver card.
*/
/*  Member(s)       :
**      BR_VIX_OSC_3_3          [public]    business rule
**
**  Information     :
**   Compiler(s)    : C
**   Target(s)      : Independent
**
**  Subversion      :
**      $Id: $
**      $HeadURL: $
**
**  History         :
**   Vers.  Date        Aut.  Type     Description
**   -----  ----------  ----  -------  ----------------------------------------
**    1.00  16.10.15    DCH   Create
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
     *  @brief  BR_VIX_OSC_3_3 business rule.
     *  @param  pData business rule context data.
     *  @return RULE_RESULT_EXECUTED if business rule executed,\n
     *          RULE_RESULT_BYPASSED if business rule bypassed, or\n
     *          RULE_RESULT_ERROR if unexpected error occurred while executing
     *          business rule.
     */
RuleResult_e
BR_VIX_OSC_3_3( MYKI_BR_ContextData_t *pData )
{
    MYKI_OAShiftDataControl_t      *pMYKI_OAShiftDataControl    = NULL;
    MYKI_BR_ShiftData_t            *pShiftData                  = NULL;
    U8_t                            i                           = 0;
    int                             nResult                     = 0;
    int                             ShiftId                     = 0;

    CsDbg( BRLL_RULE, "BR_VIX_OSC_3_3 : Start (Update Closed Shift Data)" );

    if ( pData == NULL )
    {
        CsErrx( "BR_VIX_OSC_3_3 : Invalid parameter" );
        return  RULE_RESULT_ERROR;
    }
   
    if ( ( nResult = MYKI_CS_OAShiftDataControlGet( &pMYKI_OAShiftDataControl ) ) != MYKI_CS_OK )
    {
        CsErrx( "BR_VIX_OSC_3_3 : MYKI_CS_OAShiftDataControlGet() failed (%d)", nResult );
        return  RULE_RESULT_ERROR;
    }

    /*  PRE-CONDITIONS */
    {
        /*  1.  Shift data(10) buffer provided. */
        if ( ( pShiftData = pData->DynamicData.pShiftData ) == NULL )
        {
            CsDbg( BRLL_RULE, "BR_VIX_OSC_3_3 : Bypassed - Shift data buffer not provided" );
            pData->ReturnedData.bypassCode  = BYPASS_CODE( 3, 3, 1, 0 );
            return  RULE_RESULT_BYPASSED;
        }

        /*  2.  Shift portion status (11) is "Brought-Forward" or "Reset". */
        if ( pShiftData->shiftPortionStatus != SHIFT_PORTION_RESET &&
             pShiftData->shiftPortionStatus != SHIFT_PORTION_BROUGHT_FORWARD )
        {
            CsDbg( BRLL_RULE, "BR_VIX_OSC_3_3 : Bypassed - shift portion status (%d) is not 'RESET' nor 'BROUGHT-FORWARD'",
                    pShiftData->shiftPortionStatus );
            pData->ReturnedData.bypassCode  = BYPASS_CODE( 3, 3, 2, 0 );
            return  RULE_RESULT_BYPASSED;
        }
    }  

    /*  PROCESSING */
    {
        /* 1.  For each shift data record in buffer (10)
         */ 
        for( i = 0; i < PAYMENT_METHOD_MAX; i++ )
        {
            /*
                1a. If the shift data record (9) for the payment (record) type exists on the card and, if one or more data fields of the shift data record in buffer (10) and from the card (9) is/are different,
                Perform OAppShiftDataUpdate/None with the index of the existing shift data record (9) on the card to update the shift data record.
                
                1b. If the shift data record (9) for the payment (record) type does not exist on the card,
                Perform OAppShiftDataUpdate/None with the index of active record count (4) to add shift data record to card, and
                Increment the active record count (4) by one.
            */
            if ( ( nResult = myki_br_ldt_OAppShiftUpdate( pData, i, pMYKI_OAShiftDataControl ) ) != 0 )
            {
                CsErrx( "BR_VIX_OSC_3_3 : Failed to execute myki_br_ldt_OAppShiftUpdate()" );
                return RULE_RESULT_ERROR;
            }
        }

        /*  2.  Perform OAppShiftDataControlUpdate/None 
                a.  Set shift data status (3) to "Suspended" */
        pShiftData->shiftStatus     = OAPP_SHIFT_DATA_STATUS_SUSPENDED;
        /*      b.  Set active shift data record count (4) to number of shift data records with valid data. */
        /*      c.  Set shift identification (5) to value from shift data buffer (10). */
        ShiftId                     = pShiftData->shiftId;
        /*      d.  Shift start time (7) and close time (8) set to values from shift data buffer (10) */
        if ( ( nResult = myki_br_ldt_OAppShiftControlUpdate( pData, pMYKI_OAShiftDataControl->ActiveRecordCount) ) != 0  )
        {
            CsErrx( "BR_VIX_OSC_3_3 : Failed to execute myki_br_ldt_OAppShiftControlUpdate()" );
            return RULE_RESULT_ERROR;
        }

        /*
         * 3.  Modify shift log (11)
         * 3a. Set shift id, shift sequence number with values from shift data buffer (10),
         * 3b. Set sam id with id of the current installed sam,
         * 3c. Set shift start and close time with values from shift data buffer (10),
         * 3d. Set deposit sequence number to zero,
         * 3e. Set deposit total to the shift net cash total, ie. cash sales less cash reversals, and
         * 3f. Set paper ticket returns, sundry item returns and card returns with value from shift data buffer (10).
        */
        pData->InternalData.ShiftDataSummaryLog.shiftId                 = pMYKI_OAShiftDataControl->ShiftId;
        pData->InternalData.ShiftDataSummaryLog.shiftSequenceNumber     = pMYKI_OAShiftDataControl->ShiftSeqNo;
        pData->InternalData.ShiftDataSummaryLog.samId                   = pData->StaticData.deviceSamId;
        pData->InternalData.ShiftDataSummaryLog.startTime               = pShiftData->shiftStartTime;
        pData->InternalData.ShiftDataSummaryLog.closeTime               = pShiftData->shiftEndTime;
        pData->InternalData.ShiftDataSummaryLog.depositSequenceNumber   = 0;
        pData->InternalData.ShiftDataSummaryLog.depositTotal            = ( pShiftData->shiftTotalsCash.SalesValue - 
                                                                            pShiftData->shiftTotalsCash.ReversalsValue );
        pData->InternalData.ShiftDataSummaryLog.paperTicketReturns      = pShiftData->paperTicketReturns;
        pData->InternalData.ShiftDataSummaryLog.sundryItemReturns       = pShiftData->sundryItemReturns;
        pData->InternalData.ShiftDataSummaryLog.cardReturns             = pShiftData->cardReturns;
        
        pData->InternalData.IsShiftDataLogUpdated                       = TRUE;
        
        /* Post-Conditions
         * Shift data is updated to Driver Card
         */

        CsDbg( BRLL_RULE, "BR_VIX_OSC_3_3 : Executed" );
        return RULE_RESULT_EXECUTED;
    }      
}
