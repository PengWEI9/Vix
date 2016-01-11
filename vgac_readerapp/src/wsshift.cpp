/* -*- mode: c; tabs: 4; -*- */
/*=============================================================================
**
**    Vix Technology                   Licensed software
**    (C) 2015                         All rights reserved
**
**=============================================================================
**
**  Project/Product : MVU
**  Filename        : wsshift.cpp
**  Author(s)       : An Tran
**
**  Description     :
*/
/**     @file
**      @brief      Defines functions to process start/end shift.
**      @section    Section_PersistentData Persistent Data
**                  -   @a currentShiftId, contains the current Driver entered
**                      or headless shift identification.
**                  -   @a currentShiftStartCash, contains the cash amount
**                      brought forward from the previous Driver shift (if any).
**                  -   @a currentShiftStartDateTime, contains the current
**                      Driver shift start date/time.
**                  -   @a currentShiftPortionStatus, contains the Driver
**                      shift portion status, ie. "reset", "brought-forward" or
**                      "out-of-balance".
*/
/*  Member(s)       :
**      processLogon                [public]    processes logon request
**      processTableCard            [public]    processes tablecard request
**      processTrip                 [public]    processes trip request
**      processShiftEnd             [public]    processes shift end request
*       processLogOff               [public]    processes logoff request
**      processPeriodCommitCheck    [public]    processes period commit check request
**      processGetDriverTotals      [public]    processes driver totals request
**
**      getTotals                   [private]   get driver totals
**
**  Information     :
**   Compiler(s)    : C++
**   Target(s)      : Independent
**
**  Subversion      :
**      $Id: wsshift.cpp 88451 2016-01-07 00:54:31Z atran $
**      $HeadURL: https://auperasvn01.aupera.erggroup.com/svn/DPG_SWBase/vgac_readerapp/trunk/src/wsshift.cpp $
**
**  History         :
**   Vers.  Date        Aut.  Type     Description
**   -----  ----------  ----  -------  ----------------------------------------
**    1.00  07.10.15    ANT   Create
**    1.01  12.11.15    ANT   Modify   NGBU-837: Added multiple cards detection
**    1.02  18.11.15    ANT   Add      NGBU-848: Added logon/logoff handlers
**    1.03  20.11.15    ANT   Add      TDC-ONLY: (a) Removed generating open/close
**                                     shift on "trip" and (b) Added setting
**                                     headless route id on "logoff".
**    1.04  24.11.15    ANT   Modify   NGBU-851: Inhibited opening/closing
**                                     (extra) headless shift when "logoff".
**    1.05  26.11.15    ANT   Modify   NGBU-897: Rectified mismatch shift and
**                                     (first) shift portion start date/time.
**                                     Rectified operational period commit
**                                     processing.
**    1.06  30.11.15    ANT   Modify   NGBU-903: Skip opening/closing headless
**                                     shift if driver shift ended due to
**                                     another driver is logging in.
**    1.07  16.12.15    ANT   Modify   NGBU-972: Added checking connected to
**                                     DC prior performing period commit.
**
**===========================================================================*/

/*
 *      Includes
 *      --------
 */

#include <string>
#include <cstdlib>

#include <json/json.h>
#include <cs.h>
#include <myki_cardservices.h>
#include <myki_alarm.h>
#include <myki_alarm_names.h>
#include <myki_cdd_enums.h>
#include <myki_cd.h>
#include <myki_br.h>
#include <myki_tmi.h>

#include "app_debug_levels.h"
#include "card_processing_common.h"
#include "card_processing_thread.h"
#include "datastring.h"
#include "gentmi.h"
#include "cardfunctions.h"
#include "wsshift.h"

/*
 *      External References
 *      -------------------
 */

    /*  GAC_PROCESSING_THREAD.CPP - TODO: Move to header file */
extern  "C" MYKI_BR_ContextData_t  *GetBrContextDataOut( void );
extern  bool                        preValidateCard( bool &operatorCard, int nCardsDetected );
extern  bool                        ExecuteBusinessSequence( int numberOfCardsPresent, bool operatorCard, int &passengerCode, SequenceFunction_e businessSequence, bool forceProcessBR );

    /*  DATAWS.CPP */
extern  bool                        s_webSocketConnected;

/*
 *      Local Constants and Macros
 *      --------------------------
 */

    /** Clears all totals in the group */
#define ClearAllTotals( fname, group )\
    if\
    (\
        ( result = TMI_clearCounter( pKey = group "GrossCashAmount"       ) ) < 0 ||\
        ( result = TMI_clearCounter( pKey = group "AnnulledCashAmount"    ) ) < 0 ||\
        ( result = TMI_clearCounter( pKey = group "OverpaidCashAmount"    ) ) < 0 ||\
        ( result = TMI_clearCounter( pKey = group "GrossOtherAmount"      ) ) < 0 ||\
        ( result = TMI_clearCounter( pKey = group "AnnulledOtherAmount"   ) ) < 0 ||\
        ( result = TMI_clearCounter( pKey = group "GrossPaperTickets"     ) ) < 0 ||\
        ( result = TMI_clearCounter( pKey = group "AnnulledPaperTickets"  ) ) < 0 ||\
        ( result = TMI_clearCounter( pKey = group "GrossSundryTickets"    ) ) < 0 ||\
        ( result = TMI_clearCounter( pKey = group "AnnulledSundryTickets" ) ) < 0 ||\
        ( result = TMI_clearCounter( pKey = group "CashCount"             ) ) < 0 ||\
        ( result = TMI_clearCounter( pKey = group "AnnulledCashCount"     ) ) < 0 ||\
        ( result = TMI_clearCounter( pKey = group "OtherCount"            ) ) < 0 ||\
        ( result = TMI_clearCounter( pKey = group "AnnulledOtherCount"    ) ) < 0 ||\
        ( result = TMI_clearCounter( pKey = group "FullPassCount"         ) ) < 0 ||\
        ( result = TMI_clearCounter( pKey = group "ConcessionPassCount"   ) ) < 0\
    )\
    {\
        CsErrx( fname " : TMI_clearCounter('%s') failed (%d)", pKey, result );\
    }

/*
 *      Local Prototypes
 *      ----------------
 */

static  int             getTotals( Json::Value &response );

/*
 *      Global Variables
 *      ----------------
 */

MYKI_BR_ShiftData_t     g_ShiftData         = { OAPP_SHIFT_DATA_STATUS_UNUSED, 0 }; /**< Temporary buffer for shift data */
unsigned int            g_Headless_Shift_Id = 0;	                                /**< Driver Shift ID used in headless mode */

    /**
     *  @brief  Processes logon request.
     *  @param  request Json::Value object containing JSON request message
     *          of the following format,
     *  @code
     *  {
     *      "name":"logon",
     *      "type":"SET",
     *      "operatorid":"5057",
     *      "terminalid":"1",
     *      "role":"driver",
     *      ...
     *  }
     *  @endcode
     *  @return Empty string, ie. don't send response.
     */
std::string
processLogon( Json::Value &request )
{
    Json::Value             response;
    Json::FastWriter        fw;
    MYKI_BR_ContextData_t  *pDataIn             = GetCardProcessingThreadContextData();
    std::string             role                =               getJsonString( request, "role"          );
    unsigned int            terminalId          = (unsigned int)getJsonInt(    request, "terminalid"    );
    bool                    isPoDLogin          =               getJsonBool(   request, "passwordlogin" );
    bool                    isDriver            = role == "driver" ? true : false;
    bool                    isForMe             = terminalId == pDataIn->StaticData.deviceId ? true : false;
    unsigned int            currShiftId         = GetCounter( "currentShiftId"  );
    const char             *pKey                = NULL;
    int                     result              = 0;
    int                     shiftNetCashAmount  = 0;
    CsTime_t                now                 = { 0 };

    std::string             currDriverId        = getUserId().get();
    std::string             driverId            = getJsonString(request, "operatorid");

    CsDbg( APP_DEBUG_FLOW, "processLogon : %s '%s' logged in", role.c_str(), driverId.c_str() );

    createBaseResponse( request, response );

    /*  BACKWARD COMPATIBILITY: generating TMI via transferud.sh script */
    if ( isDriver == true && g_iniGenerateTmi == false )
    {
        CloseAndOpenShift();
    }

    if ( isDriver == true && currDriverId != driverId )
    {
        if ( driverId != g_defaultDriverId && currDriverId != g_defaultDriverId )
        {
            CsWarnx( "processLogon : Driver (%s) logged in while Driver (%s) not logged out", driverId.c_str(), 
                    currDriverId.c_str() );
        }

        /*  (Automatically) Closes current/headless shift... */
        CreateShiftCloseTmi( pDataIn );

        /*  Generates operational log for Driver shift... */
        if ( currShiftId != 0 /* HEAD_LESS_SHIFT */ )
        {
            CreateOpLog( pDataIn );
        }

        getUserId() = driverId;

        if ( isPoDLogin == true || isForMe == false )
        {
            /*  Driver logged in with Password of the day */
            memset( &g_ShiftData, 0, sizeof( g_ShiftData ) );

            CsTime( &now );

            g_ShiftData.shiftPortionStatus  = SHIFT_PORTION_RESET;
            g_ShiftData.shiftStartTime      = (int)now.tm_secs;
        }

        /*  Clears shift portion totals */
        ClearAllTotals( "processLogon", "portion" );

        /*  Sanity check */
        if ( g_ShiftData.shiftTotals[ PAYMENT_METHOD_CASH ].SalesValue <
             g_ShiftData.shiftTotals[ PAYMENT_METHOD_CASH ].ReversalsValue )
        {
            /*  Corruption!? */
            CsErrx( "processLogon : Cash sales value(%d) < reversals value(%d)",
                    g_ShiftData.shiftTotals[ PAYMENT_METHOD_CASH ].SalesValue,
                    g_ShiftData.shiftTotals[ PAYMENT_METHOD_CASH ].ReversalsValue );
            shiftNetCashAmount  = 0;
        }
        else
        {
            shiftNetCashAmount  = 
                    g_ShiftData.shiftTotals[ PAYMENT_METHOD_CASH ].SalesValue -
                    g_ShiftData.shiftTotals[ PAYMENT_METHOD_CASH ].ReversalsValue;
        }

        if ( g_ShiftData.shiftTotals[ PAYMENT_METHOD_CASH ].SalesCount <
             g_ShiftData.shiftTotals[ PAYMENT_METHOD_CASH ].ReversalsCount )
        {
            /*  Corruption!? */
            CsErrx( "processLogon : Cash sales count(%d) < reversals count(%d)",
                    g_ShiftData.shiftTotals[ PAYMENT_METHOD_CASH ].SalesCount,
                    g_ShiftData.shiftTotals[ PAYMENT_METHOD_CASH ].ReversalsCount );
        }

        if ( g_ShiftData.shiftTotals[ PAYMENT_METHOD_TPURSE ].SalesValue <
             g_ShiftData.shiftTotals[ PAYMENT_METHOD_TPURSE ].ReversalsValue )
        {
            /*  Corruption!? */
            CsErrx( "processLogon : TPurse sales value(%d) < reversal value(%d)",
                    g_ShiftData.shiftTotals[ PAYMENT_METHOD_TPURSE ].SalesValue,
                    g_ShiftData.shiftTotals[ PAYMENT_METHOD_TPURSE ].ReversalsValue );
        }

        if ( g_ShiftData.shiftTotals[ PAYMENT_METHOD_TPURSE ].SalesCount <
             g_ShiftData.shiftTotals[ PAYMENT_METHOD_TPURSE ].ReversalsCount )
        {
            /*  Corruption!? */
            CsErrx( "processLogon : TPurse sales value(%d) < reversal value(%d)",
                    g_ShiftData.shiftTotals[ PAYMENT_METHOD_TPURSE ].SalesCount,
                    g_ShiftData.shiftTotals[ PAYMENT_METHOD_TPURSE ].ReversalsCount );
        }

        CsDbg( APP_DEBUG_FLOW, "processLogon : DriverId           = %s", driverId.c_str() );
        CsDbg( APP_DEBUG_FLOW, "processLogon : ShiftId            = %d", g_ShiftData.shiftId );
        CsDbg( APP_DEBUG_FLOW, "processLogon : ShiftStartTime     = %d", g_ShiftData.shiftStartTime );
        CsDbg( APP_DEBUG_FLOW, "processLogon : ShiftPortionStatus = %s",
                g_ShiftData.shiftPortionStatus == SHIFT_PORTION_OUT_OF_BALANCE  ? "OUT_OF_BALANCE"  :
                g_ShiftData.shiftPortionStatus == SHIFT_PORTION_RESET           ? "RESET"           :
                g_ShiftData.shiftPortionStatus == SHIFT_PORTION_BROUGHT_FORWARD ? "BROUGHT_FORWARD" : "UNKNOWN" );
        CsDbg( APP_DEBUG_FLOW, "processLogon : ShiftCash          = (%d,%d) (%d,%d)",
                g_ShiftData.shiftTotals[ PAYMENT_METHOD_CASH ].SalesValue,
                g_ShiftData.shiftTotals[ PAYMENT_METHOD_CASH ].SalesCount,
                g_ShiftData.shiftTotals[ PAYMENT_METHOD_CASH ].ReversalsValue,
                g_ShiftData.shiftTotals[ PAYMENT_METHOD_CASH ].ReversalsCount );
        CsDbg( APP_DEBUG_FLOW, "processLogon : ShiftTPurse        = (%d,%d) (%d,%d)",
                g_ShiftData.shiftTotals[ PAYMENT_METHOD_TPURSE ].SalesValue,
                g_ShiftData.shiftTotals[ PAYMENT_METHOD_TPURSE ].SalesCount,
                g_ShiftData.shiftTotals[ PAYMENT_METHOD_TPURSE ].ReversalsValue,
                g_ShiftData.shiftTotals[ PAYMENT_METHOD_TPURSE ].ReversalsCount );

        /*  Sets Driver shift totals (retrieved from card at login) */
        if
        (
            ( result = TMI_setCounter( pKey = "currentShiftPortionStatus",  g_ShiftData.shiftPortionStatus                                      ) ) < 0 ||
            ( result = TMI_setCounter( pKey = "currentShiftId",             g_ShiftData.shiftId                                                 ) ) < 0 ||
            ( result = TMI_setCounter( pKey = "currentShiftStartCash",      shiftNetCashAmount                                                  ) ) < 0 ||
            ( result = TMI_setCounter( pKey = "currentShiftStartDateTime",  g_ShiftData.shiftStartTime                                          ) ) < 0 ||
            ( result = TMI_setCounter( pKey = "shiftGrossCashAmount",       g_ShiftData.shiftTotals[ PAYMENT_METHOD_CASH    ].SalesValue        ) ) < 0 ||
            ( result = TMI_setCounter( pKey = "shiftAnnulledCashAmount",    g_ShiftData.shiftTotals[ PAYMENT_METHOD_CASH    ].ReversalsValue    ) ) < 0 ||
            ( result = TMI_setCounter( pKey = "shiftCashCount",             g_ShiftData.shiftTotals[ PAYMENT_METHOD_CASH    ].SalesCount        ) ) < 0 ||
            ( result = TMI_setCounter( pKey = "shiftAnnulledCashCount",     g_ShiftData.shiftTotals[ PAYMENT_METHOD_CASH    ].ReversalsCount    ) ) < 0 ||
            ( result = TMI_setCounter( pKey = "shiftGrossOtherAmount",      g_ShiftData.shiftTotals[ PAYMENT_METHOD_TPURSE  ].SalesValue        ) ) < 0 ||
            ( result = TMI_setCounter( pKey = "shiftAnnulledOtherAmount",   g_ShiftData.shiftTotals[ PAYMENT_METHOD_TPURSE  ].ReversalsValue    ) ) < 0 ||
            ( result = TMI_setCounter( pKey = "shiftOtherCount",            g_ShiftData.shiftTotals[ PAYMENT_METHOD_TPURSE  ].SalesCount        ) ) < 0 ||
            ( result = TMI_setCounter( pKey = "shiftAnnulledOtherCount",    g_ShiftData.shiftTotals[ PAYMENT_METHOD_TPURSE  ].ReversalsCount    ) ) < 0
        )
        {
            CsErrx( "processLogon : TMI_setCounter('%s') failed (%d)", pKey, result );
        }
        else
        if
        (
            ( result = TMI_clearCounter( pKey = "shiftGrossPaperTickets"        ) ) < 0 ||
            ( result = TMI_clearCounter( pKey = "shiftAnnulledPaperTickets"     ) ) < 0 ||
            ( result = TMI_clearCounter( pKey = "shiftGrossSundryTickets"       ) ) < 0 ||
            ( result = TMI_clearCounter( pKey = "shiftAnnulledSundryTickets"    ) ) < 0 ||
            ( result = TMI_clearCounter( pKey = "shiftFullPassCount"            ) ) < 0 ||
            ( result = TMI_clearCounter( pKey = "shiftConcessionPassCount"      ) ) < 0
        )
        {
            CsErrx( "processLogon : TMI_clearCounter('%s') failed (%d)", pKey, result );
        }

        /*  Starts new shift */
        CreateShiftOpenTmi( pDataIn );

        /*  And persists ALL counters */
        if ( ( result = TMI_saveCounters( ) ) < 0 )
        {
            CsErrx( "processLogon : TMI_saveCounters() failed (%d)", result );
        }
    }

/*  return  fw.write( response );    */
    return  "" /* DONT_SEND_RESPONSE */;
}

    /**
     *  @brief  Processes start shift (tablecard) request.
     *  @param  request Json::Value object containing JSON request message
     *          of the following format,
     *  @code
     *  {
     *      "name":"trip",
     *      "type":"SET",
     *      "userid":"5057",
     *      "terminalid":"1",
     *      "data":
     *      {
     *          "driver_id":"5057",
     *          "shift_number":"1234",
     *          ...
     *      }
     *  }
     *  @endcode
     *  @return Empty string, ie. don't send response.
     */
std::string
processTableCard( Json::Value &request )
{
    Json::Value                     response;
    Json::FastWriter                fw;
//  MYKI_BR_ContextData_t          *pDataIn                     = GetCardProcessingThreadContextData();
    std::string                     driverId                    = getJsonString(request, "userid");
    unsigned int                    shiftId                     = (unsigned int)getJsonInt( request[ "data" ], "shift_number"   );
    unsigned int                    currShiftId                 = GetCounter( "currentShiftId"  );
    MYKI_BR_ShiftPortionStatus_e    currentShiftPortionStatus   = (MYKI_BR_ShiftPortionStatus_e)
                                                                  GetCounter( "currentShiftPortionStatus" );
    time_t                          shiftPortionStartDateTime   = (time_t)GetCounter( "shiftOpenDateTime" );
    const char                     *pKey                        = NULL;
    int                             result                      = 0;

    CsDbg( APP_DEBUG_FLOW, "processTableCard : driver_id='%s' shift_number='%d'", driverId.c_str(), shiftId );

    createBaseResponse( request, response );

    if ( shiftId != currShiftId )
    {
        /*  Starting new Driver shift */
        CsDbg( APP_DEBUG_FLOW, "processTableCard : ShiftId            = %d/%d", shiftId, currShiftId );

        if ( ( result = TMI_setCounter( "currentShiftId", shiftId ) ) < 0 )
        {
            CsErrx( "processTableCard : TMI_setCounter('currentShiftId') failed (%d)", result );
        }

        if ( currentShiftPortionStatus != SHIFT_PORTION_OUT_OF_BALANCE )
        {
            /*  Resets Driver shift totals */
            ClearAllTotals( "processTableCard", "shift" );

            if ( ( result = TMI_clearCounter( pKey = "currentShiftStartCash" ) ) < 0 )
            {
                CsErrx( "processTableCard : TMI_clearCounter('%s') failed (%d)", pKey, result );
            }

            if
            (
                ( result = TMI_setCounter( pKey = "currentShiftPortionStatus", SHIFT_PORTION_RESET            ) ) < 0 ||
                ( result = TMI_setCounter( pKey = "currentShiftStartDateTime", (int)shiftPortionStartDateTime ) ) < 0
            )
            {
                CsErrx( "processTableCard : TMI_setCounter('%s') failed (%d)", pKey, result );
            }

            CsDbg( APP_DEBUG_FLOW, "processTableCard : ShiftStartTime     = %d", shiftPortionStartDateTime );
            CsDbg( APP_DEBUG_FLOW, "processTableCard : ShiftPortionStatus = RESET" );
        }

        /*  And persists ALL counters */
        if ( ( result = TMI_saveCounters( ) ) < 0 )
        {
            CsErrx( "processTableCard : TMI_saveCounters() failed (%d)", result );
        }
    }

    /*  Returns totals */
/*  getTotals( response );  */

    response[ "string_error" ]      = getResultString( WS_SUCCESS );

/*  return  fw.write( response );    */
    return  "" /* DONT_SEND_RESPONSE */;
}

    /**
     *  @brief  Processes start trip request.
     *  @param  request Json::Value object containing JSON request message
     *          of the following format,
     *  @code
     *  {
     *      "name":"trip",
     *      "type":"SET",
     *      "userid":"5057",
     *      "terminalid":"1",
     *      "data":
     *      {
     *          "route":
     *          {
     *              "route_id":17873,
     *              ...
     *          }
     *          "route_code":"LR030_1_019",
     *          "route_id":17873,
     *          "run_id":"1",
     *          "start_time":"07:43:21",
     *          ...
     *      }
     *  }
     *  @endcode
     *  @return Empty string, ie. don't send response.
     */
std::string
processTrip( Json::Value &request )
{
    Json::Value             response;
    Json::FastWriter        fw;
    MYKI_BR_ContextData_t  *pDataIn             = GetCardProcessingThreadContextData();
    unsigned int            routeId             = (unsigned int)getJsonInt(    request[ "data" ], "route_id"        );
    std::string             tripStartTime       =               getJsonString( request[ "data" ], "start_time"      );
//  unsigned int            currRouteId         = (unsigned int)pDataIn->DynamicData.lineId;
    const char             *pKey                = NULL;
    int                     result              = 0;

    CsDbg( APP_DEBUG_FLOW, "processTrip : route_id='%d' start_time='%s'", routeId, tripStartTime.c_str() );

    createBaseResponse( request, response );

    /*  NOTE:   Setting route id to inhibit closing/opening (headless) shift on receiving
                location update. */
    pDataIn->DynamicData.lineId = routeId;

    /*  Clears trip totals */
    if ( routeId == (unsigned int)pDataIn->Tariff.headlessRouteId )
    {
        CsDbg( APP_DEBUG_DETAIL, "processTrip : Starting headless route (%d)", routeId );
    }
    else
    {
        ClearAllTotals( "processTrip", "trip" );

        /*  And persists ALL counters */
        if ( ( result = TMI_saveCounters( ) ) < 0 )
        {
            CsErrx( "processTrip : TMI_saveCounters() failed (%d)", result );
        }
    }

    /*  Returns totals */
/*  getTotals( response );  */

    response[ "string_error" ]      = getResultString( WS_SUCCESS );

/*  return  fw.write( response );    */
    return  "" /* DONT_SEND_RESPONSE */;
}

    /**
     *  @brief  Processes shift end request.
     *  @param  request Json::Value object containing JSON request message
     *          of the following format,
     *  @code
     *  {
     *      "name":"shiftend",
     *      "type":"SET",
     *      "userid":"5057",
     *      "terminalid":"1",
     *      "data":
     *      {
     *          "portion":false,
     *          "cardupdate":1,
     *          "newdriver":true
     *      }
     *  }
     *  @endcode
     *  @return The string object representing JSON response message of
     *          the following format,
     *  @code
     *  {
     *      "name":"shiftend",
     *      "type":"SET",
     *      "userid":"5057",
     *      "terminalid":"1",
     *      "data":
     *      {
     *          "portion":false,
     *          "driver_id":"5057",
     *          "shift_number":"1234",
     *          "shift_start_cash":1234,
     *          "shift_start_datetime":12345,
     *          "shift_end_datetime":12345,
     *          "shift_portion_start_datetime":12345,
     *          "shift_portion_status":"BROUGHT_FORWARD",
     *          "tripGrossCashAmount":1234,
     *          "tripAnnulledCashAmount":1234,
     *          "tripNetCashAmount":1234,
     *          "tripOverpaidCashAmount":1234,
     *          "tripGrossOtherAmount":1234,
     *          "tripAnnulledOtherAmount":1234,
     *          "tripNetOtherAmount":1234,
     *          "tripGrossPaperTickets":1234,
     *          "tripAnnulledPaperTickets":1234,
     *          "tripNetPaperTickets":1234,
     *          "tripGrossSundryTickets":1234,
     *          "tripAnnulledSundryTickets":1234,
     *          "tripNetSundryTickets":1234,
     *          "tripCashCount":1234,
     *          "tripAnnulledCashCount":1234,
     *          "tripOtherCount":1234,
     *          "tripAnnulledOtherCount":1234,
     *          "tripFullPassCount":1234,
     *          "tripConcessionPassCount":1234,
     *          "portionGrossCashAmount":1234,
     *          "portionAnnulledCashAmount":1234,
     *          "portionNetCashAmount":1234,
     *          "portionOverpaidCashAmount":1234,
     *          "portionGrossOtherAmount":1234,
     *          "portionAnnulledOtherAmount":1234,
     *          "portionNetOtherAmount":1234,
     *          "portionGrossPaperTickets":1234,
     *          "portionAnnulledPaperTickets":1234,
     *          "portionNetPaperTickets":1234,
     *          "portionGrossSundryTickets":1234,
     *          "portionAnnulledSundryTickets":1234,
     *          "portionNetSundryTickets":1234,
     *          "portionCashCount":1234,
     *          "portionAnnulledCashCount":1234,
     *          "portionOtherCount":1234,
     *          "portionAnnulledOtherCount":1234,
     *          "portionFullPassCount":1234,
     *          "portionConcessionPassCount":1234,
     *          "shiftGrossCashAmount":1234,
     *          "shiftAnnulledCashAmount":1234,
     *          "shiftNetCashAmount":1234,
     *          "shiftOverpaidCashAmount":1234,
     *          "shiftGrossOtherAmount":1234,
     *          "shiftAnnulledOtherAmount":1234,
     *          "shiftNetOtherAmount":1234,
     *          "shiftGrossPaperTickets":1234,
     *          "shiftAnnulledPaperTickets":1234,
     *          "shiftNetPaperTickets":1234,
     *          "shiftGrossSundryTickets":1234,
     *          "shiftAnnulledSundryTickets":1234,
     *          "shiftNetSundryTickets":1234,
     *          "shiftCashCount":1234,
     *          "shiftAnnulledCashCount":1234,
     *          "shiftOtherCount":1234,
     *          "shiftAnnulledOtherCount":1234,
     *          "shiftFullPassCount":1234,
     *          "shiftConcessionPassCount":1234,
     *          "string_reason":"REASON_NOT_APPLICABLE"
     *      }
     *      "string_error":"WS_SUCCESS"
     *  }
     *  @endcode
     */
std::string
processShiftEnd( Json::Value &request )
{
    Json::Value             response;
    Json::FastWriter        fw;
    MYKI_BR_ContextData_t  *pDataIn             = GetCardProcessingThreadContextData();
    MYKI_BR_ContextData_t  *pDataOut            = GetBrContextDataOut();
    const char             *pKey                = NULL;
    unsigned int            terminalId          = (unsigned int)getJsonInt(  request,           "terminalid" );
//  bool                    bIsPortion          =               getJsonBool( request[ "data" ], "portion" );
    bool                    bIsCardUpdate       =               getJsonInt(  request[ "data" ], "cardupdate" ) == 1 ? true : false;
    bool                    bIsNewDriverLogIn   =               getJsonBool( request[ "data" ], "newdriver" );
    bool                    isForMe             = terminalId == pDataIn->StaticData.deviceId ? true : false;
    int                     result              = 0;
    CsTime_t                now                 = { 0 };
    bool                    bCanEndShift        = true;
    bool                    isOperatorCard      = false;
    int                     passengerCode       = 0;
    int                     oneCardDetected     = 1;

    CsDbg( APP_DEBUG_FLOW, "processShiftEnd" );

    createBaseResponse( request, response );

    CsTime( &now );

    if ( bIsCardUpdate == true && isForMe == true )
    {
        CsDbg( APP_DEBUG_FLOW, "processShiftEnd : update driver card" );

        memset( &g_ShiftData, 0, sizeof( g_ShiftData ) );
        g_ShiftData.shiftStatus                         = OAPP_SHIFT_DATA_STATUS_SUSPENDED;
        g_ShiftData.shiftId                             = GetCounter( "currentShiftId"              );
        g_ShiftData.shiftStartTime                      = GetCounter( "currentShiftStartDateTime"   );
        g_ShiftData.shiftEndTime                        = now.tm_secs;
        g_ShiftData.shiftTotalsTPurse.RecordType        = PAYMENT_METHOD_TPURSE;
        g_ShiftData.shiftTotalsTPurse.SalesCount        = GetCounter( "shiftOtherCount"             );
        g_ShiftData.shiftTotalsTPurse.SalesValue        = GetCounter( "shiftGrossOtherAmount"       );
        g_ShiftData.shiftTotalsTPurse.ReversalsCount    = GetCounter( "shiftAnnulledOtherCount"     );
        g_ShiftData.shiftTotalsTPurse.ReversalsValue    = GetCounter( "shiftAnnulledOtherAmount"    );
        g_ShiftData.shiftTotalsCash.RecordType          = PAYMENT_METHOD_CASH;
        g_ShiftData.shiftTotalsCash.SalesCount          = GetCounter( "shiftCashCount"              );
        g_ShiftData.shiftTotalsCash.SalesValue          = GetCounter( "shiftGrossCashAmount"        );
        g_ShiftData.shiftTotalsCash.ReversalsCount      = GetCounter( "shiftAnnulledCashCount"      );
        g_ShiftData.shiftTotalsCash.ReversalsValue      = GetCounter( "shiftAnnulledCashAmount"     );
        g_ShiftData.shiftPortionStatus                  = (MYKI_BR_ShiftPortionStatus_e)
                                                          GetCounter( "currentShiftPortionStatus"   );

        if ( preValidateCard( isOperatorCard, oneCardDetected ) == true )
        {
            if ( isOperatorCard == true )
            {
                /*  Execute business sequence */
                pDataIn->DynamicData.pShiftData                 = &g_ShiftData;
                if ( ExecuteBusinessSequence( 1 /*ONE_CARD*/, isOperatorCard, passengerCode, SEQ_FUNCTION_UPDATE_SHIFT_DATA, false ) == false )
                {
                    switch ( pDataOut->ReturnedData.rejectReason )
                    {
                    default:
                        response[ "data" ][ "string_reason" ]   = getRejectReasonString( pDataOut->ReturnedData.rejectReason );
                        response[ "string_error" ]              = getResultString( WS_ERROR );
                        break;
                    }
                    bCanEndShift    = false;
                }
                pDataIn->DynamicData.pShiftData                 = NULL;
            }
            else
            {
                /*  Not OPERATOR card!? */
                CsErrx( "processShiftEnd : none OPERATOR card detected" );
                response[ "string_error" ]  = getResultString( WS_ERROR_NOT_OPERATOR_CARD );
                bCanEndShift    = false;
            }
        }
        else
        if ( oneCardDetected > 1 )
        {
            /*  Multiple cards detected */
            response[ "data" ][ "string_reason" ]   = getRejectReasonString( MYKI_BR_REJECT_REASON_MULTIPLE_CARDS );
            response[ "string_error" ]              = getResultString( WS_ERROR );
            bCanEndShift    = false;
        }
        else
        {
            /*  Failed (re)opening OPERATOR card */
            CsErrx( "processShiftEnd : failed (re)opening OPERATOR card" );
            response[ "string_error" ]  = getResultString( WS_ERROR_OPEN_CARD );
            bCanEndShift    = false;
        }
    }

    if ( bCanEndShift != false )
    {
        /*  Returns totals */
        getTotals( response );
        response[ "data" ][ "cardupdate"            ]   = request[ "data" ][ "cardupdate" ];
        response[ "data" ][ "portion"               ]   = request[ "data" ][ "portion"    ];
        response[ "data" ][ "shift_end_datetime"    ]   = (int)now.tm_secs;

        /*  Closes current shift... */
        CreateShiftCloseTmi( pDataIn );

        /*  Generates operational log for Driver shift... */
        CreateOpLog( pDataIn );

        /*  Clears all totals */
        ClearAllTotals( "processShiftEnd", "trip"    );
        ClearAllTotals( "processShiftEnd", "portion" );
        ClearAllTotals( "processShiftEnd", "shift"   );

        getUserId() = g_defaultDriverId;

        /*  And starts headless mode shift */
        if
        (
            ( result = TMI_clearCounter( pKey = "currentShiftId"            ) ) < 0 ||
            ( result = TMI_clearCounter( pKey = "currentShiftStartCash"     ) ) < 0 ||
            ( result = TMI_clearCounter( pKey = "currentShiftStartDateTime" ) ) < 0 ||
            ( result = TMI_clearCounter( pKey = "currentShiftPortionStatus" ) ) < 0
        )
        {
            CsErrx( "processShiftEnd : TMI_clearCounter('%s')", pKey );
        }

        if ( bIsNewDriverLogIn == true )
        {
            /*  Driver shift is closed due to another Driver is logging in.
                Skips opening (and closing) headless shift */
            CsDbg( APP_DEBUG_FLOW, "processShiftEnd : ending shift when another driver is logging in" );
        }
        else
        {
            CreateShiftOpenTmi( pDataIn );
        }

        /*  And persists ALL counters */
        if ( ( result = TMI_saveCounters( ) ) < 0 )
        {
            CsErrx( "processShiftEnd : TMI_saveCounters() failed (%d)", result );
        }

        response[ "data" ][ "string_reason"         ]   = getAcceptReasonString( MYKI_BR_ACCEPT_REASON_NOT_APPLICABLE );
        response[ "string_error" ]                      = getResultString( WS_SUCCESS );
    }

    return isForMe == true ? fw.write( response ) : "";
}

    /**
     *  @brief  Processes log off request.
     *  @param  request Json::Value object containing JSON request message
     *          of the following format,
     *  @code
     *  {
     *      "name":"logoff",
     *      "type":"SET",
     *      "terminalid":"1",
     *      ...
     *  }
     *  @endcode
     *  @return Empty string, ie. don't send response.
     */
std::string
processLogOff( Json::Value &request )
{
    Json::Value             response;
    Json::FastWriter        fw;
    MYKI_BR_ContextData_t  *pDataIn             = GetCardProcessingThreadContextData();
    std::string             currDriverId        = getUserId().get();
    unsigned int            currShiftId         = GetCounter( "currentShiftId"  );

    const char             *pKey                = NULL;
    int                     result              = 0;

    CsDbg( APP_DEBUG_FLOW, "processLogOff : currentDriverId='%s' currShiftId=%d", currDriverId.c_str(), currShiftId );

    createBaseResponse( request, response );

    if ( g_iniGenerateTmi == false )
    {
        CloseAndOpenShift();
    }

    /*  NOTE:   Setting route id to inhibit closing/opening (headless) shift on receiving
                location update. */
    pDataIn->DynamicData.lineId = pDataIn->Tariff.headlessRouteId;

    if ( currDriverId != g_defaultDriverId )
    {
        CsDbg( APP_DEBUG_DETAIL, "processLogOff : end Driver(%s) shift", currDriverId.c_str());

        /*  Closes current Driver shift... */
        CreateShiftCloseTmi( pDataIn );

        /*  Generates operational log for Driver shift... */
        CreateOpLog( pDataIn );

        /*  Clears all totals */
        ClearAllTotals( "processLogOff", "trip"    );
        ClearAllTotals( "processLogOff", "portion" );
        ClearAllTotals( "processLogOff", "shift"   );

        getUserId() = g_defaultDriverId;

        /*  And starts headless mode shift */
        if
        (
            ( result = TMI_clearCounter( pKey = "currentShiftId"            ) ) < 0 ||
            ( result = TMI_clearCounter( pKey = "currentShiftStartCash"     ) ) < 0 ||
            ( result = TMI_clearCounter( pKey = "currentShiftStartDateTime" ) ) < 0 ||
            ( result = TMI_clearCounter( pKey = "currentShiftPortionStatus" ) ) < 0
        )
        {
            CsErrx( "processLogOff : TMI_clearCounter('%s')", pKey );
        }
        CreateShiftOpenTmi( pDataIn );

        /*  And persists ALL counters */
        if ( ( result = TMI_saveCounters( ) ) < 0 )
        {
            CsErrx( "processLogOff : TMI_saveCounters() failed (%d)", result );
        }
    }

    response[ "data" ][ "string_reason"         ]   = getAcceptReasonString( MYKI_BR_ACCEPT_REASON_NOT_APPLICABLE );
    response[ "string_error" ]                      = getResultString( WS_SUCCESS );

/*  return  fw.write( response );    */
    return  "" /* DONT_SEND_RESPONSE */;
}

    /**
     *  @brief  Processes operational period commit check request.
     *  @param  request Json::Value object containing JSON request message
     *          of the following format,
     *  @code
     *  {
     *      "name":"periodcommitcheck",
     *      "type":"SET",
     *      "terminalid":"1"
     *  }
     *  @endcode
     *  @return Empty string, ie. don't send response.
     */
std::string
processPeriodCommitCheck( Json::Value &request )
{
    Json::Value             response;
    Json::FastWriter        fw;
    MYKI_BR_ContextData_t  *pDataIn             = GetCardProcessingThreadContextData();

    CsDbg( APP_DEBUG_FLOW, "processPeriodCommitCheck" );

    createBaseResponse( request, response );

    if ( s_webSocketConnected == true )
    {
        /*  Checks and (if required) performs operational period commit */
        CheckAndCommitPeriod( pDataIn );

        response[ "data" ][ "string_reason"         ]   = getAcceptReasonString( MYKI_BR_ACCEPT_REASON_NOT_APPLICABLE );
        response[ "string_error" ]                      = getResultString( WS_SUCCESS );
    }
    else
    {
        CsWarnx( "processPeriodCommitCheck : not connected to DC" );

        response[ "data" ][ "string_reason"         ]   = getRejectReasonString( MYKI_BR_REJECT_REASON_NOT_APPLICABLE );
        response[ "string_error" ]                      = getResultString( WS_ERROR );
    }

/*  return  fw.write( response );    */
    return  "" /* DONT_SEND_RESPONSE */;
}

    /**
     *  @brief  Processes driver totals request.
     *  @param  request Json::Value object containing JSON request message
     *          of the following format,
     *  @code
     *  {
     *      "name":"getdrivertotals",
     *      "type":"SET",
     *      "userid":"5057",
     *      "terminalid":"1",
     *  }
     *  @endcode
     *  @return The string object representing JSON response message of
     *          the following format,
     *  @code
     *  {
     *      "name":"getdrivertotals",
     *      "type":"SET",
     *      "userid":"5057",
     *      "terminalid":"1",
     *      "data":
     *      {
     *          "driver_id":"5057",
     *          "shift_number":1234,
     *          "shift_start_cash":1234,
     *          "shift_start_datetime":12345,
     *          "shift_portion_start_datetime":12345,
     *          "shift_portion_status":"BROUGHT_FORWARD",
     *          "tripGrossCashAmount":1234,
     *          "tripAnnulledCashAmount":1234,
     *          "tripNetCashAmount":1234,
     *          "tripOverpaidCashAmount":1234,
     *          "tripGrossOtherAmount":1234,
     *          "tripAnnulledOtherAmount":1234,
     *          "tripNetOtherAmount":1234,
     *          "tripGrossPaperTickets":1234,
     *          "tripAnnulledPaperTickets":1234,
     *          "tripNetPaperTickets":1234,
     *          "tripGrossSundryTickets":1234,
     *          "tripAnnulledSundryTickets":1234,
     *          "tripNetSundryTickets":1234,
     *          "tripCashCount":1234,
     *          "tripAnnulledCashCount":1234,
     *          "tripOtherCount":1234,
     *          "tripAnnulledOtherCount":1234,
     *          "tripFullPassCount":1234,
     *          "tripConcessionPassCount":1234,
     *          "portionGrossCashAmount":1234,
     *          "portionAnnulledCashAmount":1234,
     *          "portionNetCashAmount":1234,
     *          "portionOverpaidCashAmount":1234,
     *          "portionGrossOtherAmount":1234,
     *          "portionAnnulledOtherAmount":1234,
     *          "portionNetOtherAmount":1234,
     *          "portionGrossPaperTickets":1234,
     *          "portionAnnulledPaperTickets":1234,
     *          "portionNetPaperTickets":1234,
     *          "portionGrossSundryTickets":1234,
     *          "portionAnnulledSundryTickets":1234,
     *          "portionNetSundryTickets":1234,
     *          "portionCashCount":1234,
     *          "portionAnnulledCashCount":1234,
     *          "portionOtherCount":1234,
     *          "portionAnnulledOtherCount":1234,
     *          "portionFullPassCount":1234,
     *          "portionConcessionPassCount":1234,
     *          "shiftGrossCashAmount":1234,
     *          "shiftAnnulledCashAmount":1234,
     *          "shiftNetCashAmount":1234,
     *          "shiftOverpaidCashAmount":1234,
     *          "shiftGrossOtherAmount":1234,
     *          "shiftAnnulledOtherAmount":1234,
     *          "shiftNetOtherAmount":1234,
     *          "shiftGrossPaperTickets":1234,
     *          "shiftAnnulledPaperTickets":1234,
     *          "shiftNetPaperTickets":1234,
     *          "shiftGrossSundryTickets":1234,
     *          "shiftAnnulledSundryTickets":1234,
     *          "shiftNetSundryTickets":1234,
     *          "shiftCashCount":1234,
     *          "shiftAnnulledCashCount":1234,
     *          "shiftOtherCount":1234,
     *          "shiftAnnulledOtherCount":1234,
     *          "shiftFullPassCount":1234,
     *          "shiftConcessionPassCount":1234
     *      }
     *      "string_error":"WS_SUCCESS"
     *  }
     *  @endcode
     */
std::string
processGetDriverTotals( Json::Value &request )
{
    Json::Value             response;
    Json::FastWriter        fw;

    CsDbg( APP_DEBUG_FLOW, "processGetDriverTotals" );

    createBaseResponse( request, response );

    /*  Returns totals */
    getTotals( response );

    response[ "type"                            ]   = "set";
    response[ "data" ][ "string_reason"         ]   = getAcceptReasonString( MYKI_BR_ACCEPT_REASON_NOT_APPLICABLE );
    response[ "string_error" ]                      = getResultString( WS_SUCCESS );

    return fw.write( response );
}

    /**
     *  Adds totals to JSON response.
     *  @param  response JSON response value.
     *  @return 0 if successful; -1 otherwise.
     */
static
int
getTotals( Json::Value &response )
{
    unsigned int                    grossValue          = 0;
    unsigned int                    annulledValue       = 0;
    MYKI_BR_ShiftPortionStatus_e    shiftPortionStatus  = ( MYKI_BR_ShiftPortionStatus_e )GetCounter( "currentShiftPortionStatus" );
    MYKI_BR_ContextData_t          *pDataIn             = GetCardProcessingThreadContextData();

    /*  Current shift data */
    response[ "data" ][ "driver_id"                     ] =                   getUserId().get();

    if ( pDataIn->DynamicData.lineId == pDataIn->Tariff.headlessRouteId )
    {
        response[ "data" ][ "shift_number" ] = g_Headless_Shift_Id;
    }
    else 
    {
        response[ "data" ][ "shift_number" ] = GetCounter( "currentShiftId" );
    }

    response[ "data" ][ "shift_start_cash"              ] =                   GetCounter( "currentShiftStartCash"           );
    response[ "data" ][ "shift_start_datetime"          ] =                   GetCounter( "currentShiftStartDateTime"       );
    response[ "data" ][ "portion_start_datetime"        ] =                   GetCounter( "shiftOpenDateTime"               );
    response[ "data" ][ "shift_portion_status"          ] = shiftPortionStatus == SHIFT_PORTION_OUT_OF_BALANCE  ? "OUT_OF_BALANCE"  :
                                                            shiftPortionStatus == SHIFT_PORTION_RESET           ? "RESET"           :
                                                            shiftPortionStatus == SHIFT_PORTION_BROUGHT_FORWARD ? "BROUGHT_FORWARD" : "UNKNOWN";

    /*  Driver Trip totals */
    response[ "data" ][ "tripGrossCashAmount"           ] = grossValue      = GetCounter( "tripGrossCashAmount"             );
    response[ "data" ][ "tripAnnulledCashAmount"        ] = annulledValue   = GetCounter( "tripAnnulledCashAmount"          );
    response[ "data" ][ "tripNetCashAmount"             ] = ( grossValue < annulledValue ? 0 : grossValue - annulledValue   );
    response[ "data" ][ "tripOverpaidCashAmount"        ] =                   GetCounter( "tripOverpaidCashAmount"          );
    response[ "data" ][ "tripGrossOtherAmount"          ] = grossValue      = GetCounter( "tripGrossOtherAmount"            );
    response[ "data" ][ "tripAnnulledOtherAmount"       ] = annulledValue   = GetCounter( "tripAnnulledOtherAmount"         );
    response[ "data" ][ "tripNetOtherAmount"            ] = ( grossValue < annulledValue ? 0 : grossValue - annulledValue   );
    response[ "data" ][ "tripGrossPaperTickets"         ] = grossValue      = GetCounter( "tripGrossPaperTickets"           );
    response[ "data" ][ "tripAnnulledPaperTickets"      ] = annulledValue   = GetCounter( "tripAnnulledPaperTickets"        );
    response[ "data" ][ "tripNetPaperTickets"           ] = ( grossValue < annulledValue ? 0 : grossValue - annulledValue   );
    response[ "data" ][ "tripGrossSundryTickets"        ] = grossValue      = GetCounter( "tripGrossSundryTickets"          );
    response[ "data" ][ "tripAnnulledSundryTickets"     ] = annulledValue   = GetCounter( "tripAnnulledSundryTickets"       );
    response[ "data" ][ "tripNetSundryTickets"          ] = ( grossValue < annulledValue ? 0 : grossValue - annulledValue   );
    response[ "data" ][ "tripCashCount"                 ] =                   GetCounter( "tripCashCount"      	            );
    response[ "data" ][ "tripAnnulledCashCount"         ] =                   GetCounter( "tripAnnulledCashCount"           );
    response[ "data" ][ "tripOtherCount"                ] =                   GetCounter( "tripOtherCount"                  );
    response[ "data" ][ "tripAnnulledOtherCount"        ] =                   GetCounter( "tripAnnulledOtherCount"          );
    response[ "data" ][ "tripFullPassCount"             ] =                   GetCounter( "tripFullPassCount"               );
    response[ "data" ][ "tripConcessionPassCount"       ] =                   GetCounter( "tripConcessionPassCount"         );

    /*  Driver Shift portion totals */
    response[ "data" ][ "portionGrossCashAmount"        ] = grossValue      = GetCounter( "portionGrossCashAmount"          );
    response[ "data" ][ "portionAnnulledCashAmount"     ] = annulledValue   = GetCounter( "portionAnnulledCashAmount"       );
    response[ "data" ][ "portionNetCashAmount"          ] = ( grossValue < annulledValue ? 0 : grossValue - annulledValue   );
    response[ "data" ][ "portionOverpaidCashAmount"     ] =                   GetCounter( "portionOverpaidCashAmount"       );
    response[ "data" ][ "portionGrossOtherAmount"       ] = grossValue      = GetCounter( "portionGrossOtherAmount"         );
    response[ "data" ][ "portionAnnulledOtherAmount"    ] = annulledValue   = GetCounter( "portionAnnulledOtherAmount"      );
    response[ "data" ][ "portionNetOtherAmount"         ] = ( grossValue < annulledValue ? 0 : grossValue - annulledValue   );
    response[ "data" ][ "portionGrossPaperTickets"      ] = grossValue      = GetCounter( "portionGrossPaperTickets"        );
    response[ "data" ][ "portionAnnulledPaperTickets"   ] = annulledValue   = GetCounter( "portionAnnulledPaperTickets"     );
    response[ "data" ][ "portionNetPaperTickets"        ] = ( grossValue < annulledValue ? 0 : grossValue - annulledValue   );
    response[ "data" ][ "portionGrossSundryTickets"     ] = grossValue      = GetCounter( "portionGrossSundryTickets"       );
    response[ "data" ][ "portionAnnulledSundryTickets"  ] = annulledValue   = GetCounter( "portionAnnulledSundryTickets"    );
    response[ "data" ][ "portionNetSundryTickets"       ] = ( grossValue < annulledValue ? 0 : grossValue - annulledValue   );
    response[ "data" ][ "portionCashCount"              ] =                   GetCounter( "portionCashCount"   		    );
    response[ "data" ][ "portionAnnulledCashCount"      ] =                   GetCounter( "portionAnnulledCashCount"        );
    response[ "data" ][ "portionOtherCount"             ] =                   GetCounter( "portionOtherCount"               );
    response[ "data" ][ "portionAnnulledOtherCount"     ] =                   GetCounter( "portionAnnulledOtherCount"       );
    response[ "data" ][ "portionFullPassCount"          ] =                   GetCounter( "portionFullPassCount"            );
    response[ "data" ][ "portionConcessionPassCount"    ] =                   GetCounter( "portionConcessionPassCount"      );

    /*  Driver shift totals */
    response[ "data" ][ "shiftGrossCashAmount"          ] = grossValue      = GetCounter( "shiftGrossCashAmount"            );
    response[ "data" ][ "shiftAnnulledCashAmount"       ] = annulledValue   = GetCounter( "shiftAnnulledCashAmount"         );
    response[ "data" ][ "shiftNetCashAmount"            ] = ( grossValue < annulledValue ? 0 : grossValue - annulledValue   );
    response[ "data" ][ "shiftOverpaidCashAmount"       ] =                   GetCounter( "shiftOverpaidCashAmount"         );
    response[ "data" ][ "shiftGrossOtherAmount"         ] = grossValue      = GetCounter( "shiftGrossOtherAmount"           );
    response[ "data" ][ "shiftAnnulledOtherAmount"      ] = annulledValue   = GetCounter( "shiftAnnulledOtherAmount"        );
    response[ "data" ][ "shiftNetOtherAmount"           ] = ( grossValue < annulledValue ? 0 : grossValue - annulledValue   );
    response[ "data" ][ "shiftGrossPaperTickets"        ] = grossValue      = GetCounter( "shiftGrossPaperTickets"          );
    response[ "data" ][ "shiftAnnulledPaperTickets"     ] = annulledValue   = GetCounter( "shiftAnnulledPaperTickets"       );
    response[ "data" ][ "shiftNetPaperTickets"          ] = ( grossValue < annulledValue ? 0 : grossValue - annulledValue   );
    response[ "data" ][ "shiftGrossSundryTickets"       ] = grossValue      = GetCounter( "shiftGrossSundryTickets"         );
    response[ "data" ][ "shiftAnnulledSundryTickets"    ] = annulledValue   = GetCounter( "shiftAnnulledSundryTickets"      );
    response[ "data" ][ "shiftNetSundryTickets"         ] = ( grossValue < annulledValue ? 0 : grossValue - annulledValue   );
    response[ "data" ][ "shiftCashCount"                ] =                   GetCounter( "shiftCashCount"		    );
    response[ "data" ][ "shiftAnnulledCashCount"        ] =                   GetCounter( "shiftAnnulledCashCount"          );
    response[ "data" ][ "shiftOtherCount"               ] =                   GetCounter( "shiftOtherCount"                 );
    response[ "data" ][ "shiftAnnulledOtherCount"       ] =                   GetCounter( "shiftAnnulledOtherCount"         );
    response[ "data" ][ "shiftFullPassCount"            ] =                   GetCounter( "shiftFullPassCount"              );
    response[ "data" ][ "shiftConcessionPassCount"      ] =                   GetCounter( "shiftConcessionPassCount"        );

    return  0;
}