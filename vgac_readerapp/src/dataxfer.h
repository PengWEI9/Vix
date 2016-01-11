/* -*- mode: c; tabs: 4; -*- */
/*=============================================================================
**
**    Vix Technology                   Licensed software
**    (C) 2015                         All rights reserved
**
**=============================================================================
**
**  Project/Product : MVU
**  Filename        : dataxfer.h
**  Author(s)       : An Tran
**
**  Description     :
*/
/**     @file
**      @brief  Declares function prototypes to transfer data.
*/
/*  Member(s)       :
**
**  Information     :
**   Compiler(s)    : C++
**   Target(s)      : Independent
**
**  Subversion      :
**      $Id: dataxfer.h 76650 2015-10-11 08:18:23Z atran $
**      $HeadURL: https://auperasvn01.aupera.erggroup.com/svn/DPG_SWBase/vgac_readerapp/trunk/src/dataxfer.h $
**
**  History         :
**   Vers.  Date        Aut.  Type     Description
**   -----  ----------  ----  -------  ----------------------------------------
**    1.00  07.09.15    ANT   Create
**
**===========================================================================*/

#if     !defined( __DATAXFER_H_INCLUDED )
#define __DATAXFER_H_INCLUDED           1

/*
 *      Includes
 *      --------
 */

#include <string>

#include "card_processing_thread.h"

/*
 *      Local Constants and Macros
 *      --------------------------
 */

/*
 *      Local Variables
 *      ---------------
 */

/*
 *      Global Variables
 *      ----------------
 */

int         initDataTransfer( const char *pAlarmAddress, const char *pPeerAddress, CardProcessingControl_t *pCardProcessingControl );
void        uploadAlarm( const char *pPayloadData, long payloadLength );
void        uploadData( const char *pPayloadData, long payloadLength );
std::string getMessage( );
void        ungetMessage( std::string &msg );
void        putMessage( std::string &msg );
void        flushMessage( );
void        cleanupDataTransfer( );

#endif  /*  !__DATAXFER_H_INCLUDED */
