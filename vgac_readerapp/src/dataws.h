/* -*- mode: c; tabs: 4; -*- */
/*=============================================================================
**
**    Vix Technology                   Licensed software
**    (C) 2015                         All rights reserved
**
**=============================================================================
**
**  Project/Product : MVU
**  Filename        : dataws.h
**  Author(s)       : ?
**
**  Description     :
*/
/**     @file
**      @brief  Declares function prototypes to transfer data via WebSocket.
*/
/*  Member(s)       :
**
**  Information     :
**   Compiler(s)    : C++
**   Target(s)      : Independent
**
**  Subversion      :
**      $Id: dataws.h 76650 2015-10-11 08:18:23Z atran $
**      $HeadURL: https://auperasvn01.aupera.erggroup.com/svn/DPG_SWBase/vgac_readerapp/trunk/src/dataws.h $
**
**  History         :
**   Vers.  Date        Aut.  Type     Description
**   -----  ----------  ----  -------  ----------------------------------------
**    1.00  03.09.15    ANT   Create
**
**===========================================================================*/

#if     !defined( __DATAWS_H_INCLUDED )
#define __DATAWS_H_INCLUDED             1

/*
 *      Options
 *      -------
 */

/*
 *      Includes
 *      --------
 */

#include <string>

#include "card_processing_thread.h"

/*
 *      Prototypes
 *      ----------
 */

int         initWebsocket( const char *pAlarmAddress, const char *pPeerAddress, CardProcessingControl_t *pCardProcessingControl );
void        cleanupWebsocket( );
void        uploadAlarmWebsocket( const char *pPayloadData, long payloadLength );
void        uploadDataWebsocket( const char *pPayloadData, long payloadLength );
std::string getMessageWebsocket( );
void        putMessageWebsocket( std::string &msg );
void        ungetMessageWebsocket( std::string &msg );
void        flushMessageWebsocket( );

#endif  /*  !__DATAWS_H_INCLUDED */
