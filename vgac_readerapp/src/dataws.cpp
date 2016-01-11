/* -*- mode: c; tabs: 4; -*- */
/*=============================================================================
**
**    Vix Technology                   Licensed software
**    (C) 2015                         All rights reserved
**
**=============================================================================
**
**  Project/Product : MVU
**  Filename        : dataws.cpp
**  Author(s)       : An Tran
**
**  Description     :
*/
/**     @file
**      @brief  Defines functions to transfer data via WebSocket.
*/
/*  Member(s)       :
**
**  Information     :
**   Compiler(s)    : C++
**   Target(s)      : Independent
**
**  Subversion      :
**      $Id: dataws.cpp 85994 2015-12-09 06:17:29Z atran $
**      $HeadURL: https://auperasvn01.aupera.erggroup.com/svn/DPG_SWBase/vgac_readerapp/trunk/src/dataws.cpp $
**
**  History         :
**   Vers.  Date        Aut.  Type     Description
**   -----  ----------  ----  -------  ----------------------------------------
**    1.00  03.09.15    ANT   Create
**    1.01  16.09.15    ANT   Modify   Rectified segment fault at start-up
**                                     (on UBUNTU platform)
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

#include <string>
#include <queue>

#include <json/json.h>

#include <cs.h>
#include <csmutex.h>
#include <csthread.h>

#include "easywsclient.hpp"
#include "app_debug_levels.h"
#include "card_processing_thread.h"
#include "dataws.h"

/*
 *      Local Constants and Macros
 *      --------------------------
 */

#define APP_DEBUG_COMM              5       /**< Debug level for communication */
#define RETRY_DELAY_SECS            15      /**< Delay (in seconds) before next reconnecting attempt */
#define WS_POLL_TIMEOUT             100     /**< Web socket poll timeout (in milliseconds) */

/*
 *      Local Prototypes
 *      ----------------
 */

static  void       *websocketProcessingThread( void *arg );
static  void        handle_json( const std::string &message );

/*
 *      Local Variables
 *      ---------------
 */

static  bool                        isWebSocketAlive        = false;
static  CsMutex_t                   g_TXQueueLock;
static  CsMutex_t                   g_RXQueueLock;
static  std::queue<std::string>     s_messageTXQueue;
static  std::queue<std::string>     s_messageRXQueue;
static  CsThread_t                  websocketProcessingThreadId;

/*
 *      Global Variables
 *      ----------------
 */
bool s_webSocketConnChanged = false;
bool s_webSocketConnected = false;

    /**
     *  Initialises WebSocket data transfer mechanism.
     *  @param  pAlarmAddress WebSocket alarm destination address.
     *  @param  pPeerAddress WebSocket peer address.
     *  @param  pCardProcessingControl card processing control block.
     *  @return 0 if successful; otherwise errno.
     */
int
initWebsocket( const char *pAlarmAddress, const char *pPeerAddress, CardProcessingControl_t *pCardProcessingControl )
{
    int             result      = 0;
    static  void   *argv[ 2 ]   = { NULL, NULL };

    (void)pAlarmAddress;

    argv[ 0 ]   = (void *)pPeerAddress;
    argv[ 1 ]   = (void *)pCardProcessingControl;

    if ( ( result = CsMutexInit( &g_RXQueueLock , CSSYNC_THREAD ) ) != 0 )
    {
        CsErrx( "initWebsocket : CsMutexInit(g_RXQueueLock) failed (%d)", result );
    }
    else
    if ( ( result = CsMutexInit( &g_TXQueueLock , CSSYNC_THREAD ) ) != 0 )
    {
        CsErrx( "initWebsocket : CsMutexInit(g_TXQueueLock) failed (%d)", result );
    }
    else
    if ( ( result = CsThrCreate( NULL, 0, websocketProcessingThread, (void *)&argv[ 0 ], CSTHR_BOUND, &websocketProcessingThreadId ) ) != 0 )
    {
        CsErrx( "initWebsocket : CsThrCreate() failed (%d)", result );
    }

    return result;
}

    /**
     *  Performs clean-up on WebSocket.
     */
void
cleanupWebsocket( )
{
    int             retries     = 5;

    while ( isWebSocketAlive == true && retries > 0 )
    {
        CsSleep( 0, 200 /*MSECS*/ );
        retries--;
    }
    if ( isWebSocketAlive == false )
    {
        (void)CsMutexDestroy( &g_RXQueueLock );
        (void)CsMutexDestroy( &g_TXQueueLock );
    }
}

    /**
     *  Sends alarm payload via WebSocket.
     *  @param  pAddress payload destination address.
     *  @param  pPayloadData payload buffer address.
     *  @param  payloadLength payload buffer size.
     */
void
uploadAlarmWebsocket( const char *pPayloadData, long payloadLength )
{
    CsErrx( "uploadAlarmWebsocket : WebSocket not supported" );
}

    /**
     *  Sends data payload via WebSocket.
     *  @param  pPayloadData payload buffer address.
     *  @param  payloadLength payload buffer size.
     */
void
uploadDataWebsocket( const char *pPayloadData, long payloadLength )
{
    if ( pPayloadData != NULL )
    {
        CsDebug( APP_DEBUG_FLOW, ( APP_DEBUG_FLOW, ">>> %s\n", pPayloadData ) );
        CsMutexLock( &g_TXQueueLock );
        {
            s_messageTXQueue.push( std::string( pPayloadData ) );
        }
        CsMutexUnlock( &g_TXQueueLock );
    }
    else
    {
        CsErrx( "uploadDataWebsocket : cannot upload%s", pPayloadData == NULL ? " DATA IS NULL" : "" );
    }
}

    /**
     *  Gets received JSON formatted message (if any) from WebSocket.
     *  @return received JSON formatted message.
     */
std::string
getMessageWebsocket( )
{
    std::string     msg;

    CsMutexLock( &g_RXQueueLock );
    {
        if ( !s_messageRXQueue.empty() )
        {
            msg = s_messageRXQueue.front();

            s_messageRXQueue.pop();
        }
    }
    CsMutexUnlock( &g_RXQueueLock );

    return msg;
}

    /**
     *  Ungets JSON formatted message to WebSocket.
     *  @param  msg JSON formatted message to be pushed back to receive
     *          message queue.
     */
void
ungetMessageWebsocket( std::string &msg )
{
    handle_json( msg );
}

    /**
     *  Puts JSON formatted message to WebSocket.
     *  @param  msg JSON formatted message to be sent.
     */
void
putMessageWebsocket( std::string &msg )
{
    CsDebug( APP_DEBUG_FLOW, ( APP_DEBUG_FLOW, ">>> %s\n", msg.c_str() ) );
    CsMutexLock( &g_TXQueueLock );
    {
        s_messageTXQueue.push( msg );
    }
    CsMutexUnlock( &g_TXQueueLock );
}

    /**
     *  Flushes WebSocket received message queue.
     */
void
flushMessageWebsocket( )
{
    CsMutexLock( &g_RXQueueLock );
    {
        std::string     msg;

        while ( !s_messageRXQueue.empty() )
        {
            s_messageRXQueue.pop();
        }
    }
    CsMutexUnlock( &g_RXQueueLock );
}

    /**
     *  WebSocket worker thread.
     *  @param  arg thread start-up argument(s).
     */
static  void
*websocketProcessingThread( void *arg )
{
    void                              **pArgv           = (void **)arg;
    const char                         *pPeerAddress    = reinterpret_cast<const char *>( pArgv[ 0 ] );
    CardProcessingControl_t            *control         = reinterpret_cast<CardProcessingControl_t *>( pArgv[ 1 ] );
    easywsclient::WebSocket::pointer    ws              = NULL;

    isWebSocketAlive    = true;

    while ( !control->terminate )
    {
        CsDebug( APP_DEBUG_FLOW, ( APP_DEBUG_FLOW, "websocketProcessingThread : Opening endpoint %s", pPeerAddress ) );
        s_webSocketConnChanged = true;
        s_webSocketConnected = false;
        ws  = easywsclient::WebSocket::from_url( pPeerAddress );
        if ( ws == NULL )
        {
            // Should consider cleaning the send/TX queue at this point
            CsSleep( RETRY_DELAY_SECS, 0 );
        }
        else
        {
            s_webSocketConnected = true;
            while ( ws->getReadyState() != easywsclient::WebSocket::CLOSED &&
                    ws->getReadyState() != easywsclient::WebSocket::CLOSING )
            {
                ws->poll( WS_POLL_TIMEOUT );

                // Process any messages
                ws->dispatch( handle_json );

                // Send messages out
                CsMutexLock( &g_TXQueueLock );
                {
                    while ( ws->getReadyState() != easywsclient::WebSocket::CLOSED &&
                            ws->getReadyState() != easywsclient::WebSocket::CLOSING &&
                            s_messageTXQueue.empty() == false )
                    {
                        std::string         message = s_messageTXQueue.front();

                        if ( ws->send( message ) )
                        {
                            s_messageTXQueue.pop();
                        }
                    }
                }
                CsMutexUnlock( &g_TXQueueLock );
            }

            if ( ws )
            {
                ws->close();
                delete ws;
                ws  = NULL;
            }
        }
    }

    // Clean up on terminate
    CsDebug( APP_DEBUG_FLOW, ( APP_DEBUG_FLOW, "websocketProcessingThread : Clean up" ) );
    CsMutexLock( &g_TXQueueLock );
    {
        /*  Flushes pending transmit queue */
        while ( s_messageTXQueue.empty() == false )
        {
            s_messageTXQueue.pop();
        }
    }
    CsMutexUnlock( &g_TXQueueLock );

    if ( ws )
    {
        ws->close();
        delete ws;
        ws = NULL;
    }

    isWebSocketAlive    = false;

    return 0;
}

    /**
     *  Web socket message received callback function.
     *  @param  message received JSON formatted message.
     */
static  void
handle_json( const std::string &message )
{
    CsDebug( APP_DEBUG_COMM, ( APP_DEBUG_COMM, "<<< %s\n", message.c_str() ) );

    CsMutexLock( &g_RXQueueLock );
    {
        s_messageRXQueue.push( message );
    }
    CsMutexUnlock( &g_RXQueueLock );
}
