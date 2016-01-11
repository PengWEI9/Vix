/* -*- mode: c; tabs: 4; -*- */
/*=============================================================================
**
**    Vix Technology                   Licensed software
**    (C) 2014                         All rights reserved
**
**=============================================================================
**
**  Project/Product : NGBU
**  Filename        : ipcextend.cpp
**  Author(s)       : An Tran
**
**  Description     :
**      Implements additional IPC handlers for VGAC READERAPP.
**
**  Member(s)       :
**      IpcHandlerEx            [public]    processes extended IPC messages
**
**      OnLogEvStop             [private]   processes Stop messages
**      OnTripRouteSelection    [private]   processes Trip Selection messages
**
**  Information     :
**   Compiler(s)    : C++
**   Target(s)      : Independent
**
**  Subversion      :
**      $Id: ipcextend.cpp 88278 2016-01-06 01:05:34Z atran $
**      $HeadURL: https://auperasvn01.aupera.erggroup.com/svn/DPG_SWBase/vgac_readerapp/trunk/src/ipcextend.cpp $
**
**  History         :
**   Vers.  Date        Aut.  Type     Description
**   -----  ----------  ----  -------  ----------------------------------------
**    1.00  17.11.15    ANT   Create
**    1.01  01.12.15    ANT   Add      Locking
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

#include <json/json.h>

#include <cs.h>
#include <myki_avl_ipc.h>
#include <myki_br.h>

#include "app_debug_levels.h"
#include "card_processing_common.h"
#include "card_processing_thread.h"
#include "gentmi.h"

/*
 *      Local Constants and Macros
 *      --------------------------
 */

#define RESOURCE_LOCK()                 { CsMutexLock(   &pControl->lock ); }
#define RESOURCE_UNLOCK()               { CsMutexUnlock( &pControl->lock ); }

/*
 *      Local Variables
 *      ---------------
 */

/*
 *      Local Prototypes
 *      ----------------
 */

static  int     OnLogEvStop( IPC_header_t *pIpc );
static  int     OnTripRouteSelection( IPC_header_t *pIpc );

    /**
     *  @brief  Processes extended IPC messages.
     *  @param  pIpc received IPC message.
     *  @return >0 if IPC message consumed;\n
     *          =0 if IPC message not consumed;\n
     *          <0 if failed processing IPC message.
     */
int
IpcHandlerEx( IPC_header_t *pIpc )
{
    int     result      = 0;

    if ( pIpc != NULL )
    {
        switch ( pIpc->type )
        {
        case    AVL_LOGEV_ENTER_STOP:           /*  DROP THROUGH TO NEXT CASE! */
        case    AVL_LOGEV_DEPART_FIRST_STOP:    /*  DROP THROUGH TO NEXT CASE! */
        case    AVL_LOGEV_ENTER_LAST_STOP:      /*  DROP THROUGH TO NEXT CASE! */
        case    AVL_LOGEV_DEPART_STOP:          /*  DROP THROUGH TO NEXT CASE! */
        case    AVL_LOGEV_MANUAL_STOP:          /*  DROP THROUGH TO NEXT CASE! */
        case    AVL_LOGEV_PERIODIC:             /*  DROP THROUGH TO NEXT CASE! */
        case    AVL_LOGEV_NO_STOP_DETECTED:     result  = OnLogEvStop( pIpc );              break;
        case    AVL_LOGEV_DRIVER_CHANGE_END:    /*  TODO */
        case    AVL_LOGEV_DRIVER_BREAK_START:   /*  TODO */
        case    AVL_LOGEV_DRIVER_BREAK_END:     CsWarnx( "IpcExtendHandler : TODO (%d)", pIpc->type );
                                                result  = 0;                                break;
        case    AVL_LOGEV_TRIP_SELECTION:       /*  DROP THROUGH TO NEXT CASE! */
        case    AVL_LOGEV_ROUTE_SELECTION:      /*  DROP THROUGH TO NEXT CASE! */
        case    AVL_LOGEV_HEADLESS_OPERATION:   result  = OnTripRouteSelection( pIpc );     break;
        default:
            break;
        }
    }
    return  result;
}

    /**
     *  @brief  Processes the following messages,\n
     *          -   AVL_LOGEV_ENTER_FIRST_STOP,\n
     *          -   AVL_LOGEV_ENTER_LAST_STOP,\n
     *          -   AVL_LOGEV_DEPART_STOP,\n
     *          -   AVL_LOGEV_MANUAL_STOP,\n
     *          -   AVL_LOGEV_PERIODIC, and\n
     *          -   AVL_LOGEV_NO_STOP_DETECTED.
     *  @param  pIpc received IPC message.
     *  @return 0 if successful; <0 otherwise.
     */
static
int
OnLogEvStop( IPC_header_t *pIpc )
{
    CardProcessingControl_t    *pControl    = GetCardProcessingControl( );
    int                         result      = -1;

    if ( pIpc != NULL )
    {
        RESOURCE_LOCK()
        {
            IPC_AVL_EventLog_t     *pIpcAvlEventLog = (IPC_AVL_EventLog_t *)pIpc;
            MYKI_BR_ContextData_t  *pData           = GetCardProcessingThreadContextData( );
            int                     evLogType       = EvLogType_Unknown;
            int                     routeId         = pIpcAvlEventLog->routeId;
            int                     stopId          = pIpcAvlEventLog->stopId;
            double                  latitude        = pIpcAvlEventLog->latitude;
            double                  longitude       = pIpcAvlEventLog->longitude;
            bool                    gpsAvailable    = pIpcAvlEventLog->gpsAvailable;
            char                   *pStopName       = pIpcAvlEventLog->stopName;
            int                     noStopDuration  = 0;

            switch ( pIpcAvlEventLog->header.type )
            {
            case    AVL_LOGEV_ENTER_STOP:           evLogType       = EvLogType_EnterStop;          break;
            case    AVL_LOGEV_DEPART_FIRST_STOP:    evLogType       = EvLogType_DepartFirstStop;    break;
            case    AVL_LOGEV_ENTER_LAST_STOP:      evLogType       = EvLogType_EnterLastStop;      break;
            case    AVL_LOGEV_DEPART_STOP:          evLogType       = EvLogType_DepartStop;         break;
            case    AVL_LOGEV_MANUAL_STOP:          evLogType       = EvLogType_ManualStop;         break;
            case    AVL_LOGEV_PERIODIC:             evLogType       = EvLogType_Periodic;           break;
            case    AVL_LOGEV_NO_STOP_DETECTED:     noStopDuration  = pIpcAvlEventLog->noStopDuration;
                                                    evLogType       = EvLogType_NoStopDetected;     break;
            default:
                CsErrx( "OnLogEvStop : unexpected message type (%d)", pIpcAvlEventLog->header.type );
                evLogType   = EvLogType_Unknown;
                result      = -1;
                break;
            }

            if ( evLogType != EvLogType_Unknown )
            {
                result  = CreateEvStop( pData, evLogType, routeId, stopId, pStopName, gpsAvailable, latitude, longitude, noStopDuration );
            }
        }
        RESOURCE_UNLOCK()
    }
    return  result;
}

    /**
     *  @brief  Processes the following messages,\n
     *          -   AVL_LOGEV_HEADLESS_OPERATION,\n
     *          -   AVL_LOGEV_TRIP_SELECTION, and\n
     *          -   AVL_LOGEV_ROUTE_SELECTION.
     *  @param  pIpc received IPC message.
     *  @return 0 if successful; <0 otherwise.
     */
static
int
OnTripRouteSelection( IPC_header_t *pIpc )
{
    CardProcessingControl_t    *pControl    = GetCardProcessingControl( );
    int                         result      = -1;

    if ( pIpc != NULL )
    {
        RESOURCE_LOCK()
        {
            IPC_AVL_EventLog_t     *pIpcAvlEventLog = (IPC_AVL_EventLog_t *)pIpc;
            MYKI_BR_ContextData_t  *pData           = GetCardProcessingThreadContextData( );
            int                     evLogType       = pIpcAvlEventLog->header.type == AVL_LOGEV_TRIP_SELECTION  ? EvLogType_TripSelection  :
                                                      pIpcAvlEventLog->header.type == AVL_LOGEV_ROUTE_SELECTION ? EvLogType_RouteSelection : EvLogType_HeadlessOperation;
            int                     shiftId         = 0;
            int                     tripId          = 0;
            int                     tripStartTime   = 0;
            int                     routeId         = 0;
            int                     firstStopId     = 0;
            int                     lastStopId      = 0;

            if ( evLogType == EvLogType_HeadlessOperation )
            {
                CsTime_t            now             = { 0 };
                struct tm           nowTm           = { 0 };

                CsTime( &now );
                localtime_r( (time_t*)&now.tm_secs, &nowTm );
                tripStartTime   = ( nowTm.tm_hour * 60 /*MINUTES*/ ) + nowTm.tm_min;
            }

            shiftId         = pIpcAvlEventLog->shiftId;
            tripId          = pIpcAvlEventLog->tripId;
            tripStartTime   = pIpcAvlEventLog->tripStartTime;
            routeId         = pIpcAvlEventLog->routeId;
            firstStopId     = pIpcAvlEventLog->firstStopId;
            lastStopId      = pIpcAvlEventLog->lastStopId;
            result          = CreateTripRouteSelection( pData, evLogType, shiftId, tripId, tripStartTime, routeId, firstStopId, lastStopId );
        }
        RESOURCE_UNLOCK()
    }
    return  result;
}
