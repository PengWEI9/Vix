/* -*- mode: c; tabs: 4; -*- */
/*=============================================================================
**
**    Vix Technology                   Licensed software
**    (C) 2014                         All rights reserved
**
**=============================================================================
**
**  Project/Product : MBU
**  Filename        : message_base.h
**  Author(s)       : ?
**
**  Description     :
**      This header file contains:
**
**      Task IDs
**      Message types (example)
**      generic messages - IPC_NoPayload_t is just a message header
**
**      Note that not all tasks expect to receive message, for example
**      the ECU proxy's receive thread sends a message or rx char.
**
**  Member(s)       :
**
**  Information     :
**   Compiler(s)    : C/C++
**   Target(s)      : Independent
**
**  Subversion      :
**      $Id: $
**      $HeadURL: $
**
**  History         :
**   Vers.  Date        Aut.  Type     Description
**   -----  ----------  ----  -------  ----------------------------------------
**    1.00  dd.mm.yy    ?     Create
**    1.01  19.02.15    ANT   Modify   Clean-up
**
**===========================================================================*/

#ifndef __MESSAGE_BASE_H_INCLUDED
#define __MESSAGE_BASE_H_INCLUDED

/*
 *      Includes
 *      --------
 */

#include <stddef.h>		            // offsetof macro

#include <cs.h>

enum
{
    IPC_HEARTBEAT_REQUEST = 87,     // send to all processes and threads
    IPC_HEARTBEAT_RESPONSE,         // lucky!

    IPC_TASK_STARTED,
    IPC_TASK_READY,                 // may be used to pause until others are also ready
    IPC_TASK_PAUSED,                // may suspend all threads except for IPC (wiating for run)
    IPC_TASK_STOPPED,
    IPC_TASK_KILLED,                // SIGKILL PM or signal handler can tell others
    IPC_TASK_CRASHED,               // PM or signal handler can tell others

    IPC_TASK_RUN,                   // a 'ready' task can proceed
    IPC_TASK_PAUSE,
    IPC_TASK_STOP,                  // ask
    IPC_TASK_KILL,                  // when 'asking' failed, use strong tactics 

    IPC_TASK_spareY,                // *** after this is used, renumber

    IPC_management_last_plus1,
};

typedef int32_t         IPC_t;      /**< IPC message type */
typedef int32_t         TaskID_t;   /**< Task/Process identification */

/*
 * Messages shall be defined as follows:
 */

typedef struct {
    IPC_t       type;
    TaskID_t    source;
} IPC_header_t;

typedef struct {
    IPC_header_t    hdr;
} IPC_NoPayload_t;

typedef struct {
    IPC_header_t    hdr;
    int32_t        data;
} IPC_IntPayload_t;

    /**
     *  Variable length message support - IPC_Generic_t
     */
typedef struct {
    IPC_header_t    hdr;
    uint16_t        len;
    char            data[1];
} IPC_Generic_t;
#define IPC_GENERIC_SIZE(n)		((n) + offsetof(IPC_Generic_t, data))	/* original preserved for compat. */
#define IPC_GENERIC_SIZE_p(p)	IPC_GENERIC_SIZE((p)->len)				/* pointer vesion is easier */

/*
    Use IPC_GENERIC_SIZE() to define a message buffer to send:

    char	buf[ IPC_GENERIC_SIZE(READBUFFERSIZE) ];

    IPC_Generic_t *pMsg = (IPC_Generic_t *)buf;

    BytesRead = read(fd, pMsg->data, READBUFFERSIZE);
	pMsg->len = BytesRead;
	pMsg->hdr.type = IPC_SOME_DATA;

    When receiving an IPC:
    Once received, when the type is known, verify the size:
    IPC_Generic_t *g = (IPC_Generic_t *)m;

    if (IPC_GENERIC_SIZE_p(g) != rxsiz)		<- look up using the pointer version
        CsErr("EcuSerial::main: message size error exp/actual %d/%d", IPC_GENERIC_SIZE_p(g), rxsiz);
    else
        process(g->data)
*/

typedef struct
{
    /*! the general IPC message header */
    IPC_header_t hdr;
    /*! the total number of data bytes */
    int maxSize;
    /*! the total number of (fragmented) data blocks to make up the JSON request message */
    int numberOfBlocks;
    /*! the data block sequence number starting from 1 to numberOfBlocks */
    int blockSequenceNumber;
    /*! the number of bytes in data block */
    int blockSize;
    /*! the (start of) content of the variable size data block */
    char data[1];

} IPC_PayloadMulti_t;

typedef struct {
    IPC_header_t    hdr;
    char            buf[64];
} IPC_64Bytes_t;

#endif  //  __MESSAGE_BASE_H_INCLUDED
