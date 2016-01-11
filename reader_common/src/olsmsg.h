#ifndef OLSMSG_H_INCLUDED
#define OLSMSG_H_INCLUDED
/* -*- c: tabs: 4 -*- ******************************************************
* Copyright (C) 1998-99 ERG Limited, All rights reserved
*
* Module name   : olsmsg.h
* Module type   : OLS messaging
* Compiler(s)   : ANSI C
* Environment(s): Solaris
*
* Description:
*
    Public header
*
* Source
*
    Copyright 1993 Daniel Boulet and RTMX Inc.
    
    This system call was implemented by Daniel Boulet under contract 
    from RTMX.
    
    Redistribution and use in source forms, with and without modification,
    are permitted provided that this entire comment appears intact.
    
    Redistribution in binary form may occur without any restrictions.
    Obviously, it would be nice if you gave credit where credit is due
    but requiring it would be too onerous.
 
    This software is provided ``AS IS'' without any warranties of any kind.
*
* Version   Who      Date        Description
  1.0       APY      11/07/98    Created (based on SYSV messaging)
*
* $Name:  $
* $Source: /export/repository/cvs/core/olsbase/src/msg/olsmsg.h,v $
* $Revision: 1.1.1.1 $ $Date: 2000/12/19 01:32:22 $ $State: Exp $ 
* $ProjectName: /export/dev/olss/ols/base.pj $ $ProjectRevision: 1.1 $
* $Author: raj $ $Locker:  $
*.........................................................................*/

#include <sys/ipc.h>            /* SYSV messaging */

__BEGIN_DECLS

#ifdef WIN32
#define ETIME WAIT_TIMEOUT
#endif

#define OLSMSG_NOERROR 0x0001   /* don't complain about too int32_t msgs */

struct olsmsqid {
 int32_t msg_first;          /* first message in the queue */
 int32_t msg_last;           /* last message in the queue */
 int32_t msg_flags;
    key_t   msg_key;
 uint32_t msg_cbytes;         /* number of bytes in use on the queue */
 uint32_t msg_qnum;           /* number of msgs in the queue */
 uint32_t msg_qbytes;         /* max # of bytes on the queue */
 int32_t msg_lspid;          /* pid of last msgsnd() */
 int32_t msg_lrpid;          /* pid of last msgrcv() */
    time_t  msg_stime;          /* time of last msgsnd() */
    time_t  msg_rtime;          /* time of last msgrcv() */
    time_t  msg_ctime;          /* time of last msgctl() */
 int32_t msg_pad[1];
};

struct olsmsg {
 int32_t msg_ident;          /* identifier */
 int32_t msg_next;           /* next msg in the chain */
 int32_t msg_type;           /* type of this message */
                                /* >0 -> type of this message */
                                /* 0 -> free header */
    u_short msg_ts;             /* Size of this message */
 int32_t msg_spot;           /* Location of start of msg in buffer */
};
                                                             

/*
 * Structure describing a message
 */     
typedef struct {
 int32_t mtype;              /* message type (+ve integer) */
    char    mtext[1];           /* message body */
} OlsMsg_t;


/*
 * Based on the configuration parameters described in an SVR2
 * config(1m) man page.
 *
 * Each message is broken up and stored in segments that are msgssz
 * bytes long.  For efficiency reasons, this should be a power of
 * two.  Also, it doesn't make sense if it is less than 8 or greater
 * than about 256.  Consequently, Msginit checks that msgssz is a
 * power of two between 8 and 1024 inclusive.
 */
typedef struct {
 int32_t msgmax,                 /* max chars in a message */
        msgmni,                 /* max message queue identifiers */
        msgmnb,                 /* max chars in a queue */
        msgtql,                 /* max messages in system */
        msgssz,                 /* size of a message segment */
        msgseg;                 /* number of message segments */
} OlsMsgInfo_t;

typedef struct {
 uint32_t msgmagic;  /* Structure magic */
    CSSHMHD          msgshm;    /* Shared memory handle */
    struct olsmsgctl *msgctl;   /* internal */
    char             *msgpool;  /* internal */
    struct olsmsgmap *msgmaps;  /* internal */
    struct olsmsg    *msghdrs;  /* internal */
    struct olsmsqid  *msqids;   /* internal */
} OLSMSGHD;

extern int32_t OlsMsgCreate __P((OLSMSGHD **phMsg,
                        OlsMsgInfo_t *pInfo, const char *pPath));
extern int32_t OlsMsgOpen __P((OLSMSGHD **phMsg, const char *pPath));
extern int32_t OlsMsgJoin __P((OLSMSGHD **phMsg, 
                        OlsMsgInfo_t *pInfo, const char *pPath));
extern int32_t OlsMsgClose __P((OLSMSGHD *hMsg));

extern int32_t OlsMsgSys __P((OLSMSGHD *, int, ...));
extern int32_t OlsMsgCtl __P((OLSMSGHD *, int, int, struct olsmsqid *));
extern int32_t OlsMsgGet __P((OLSMSGHD *, key_t, int));
extern int32_t OlsMsgSnd __P((OLSMSGHD *hMsg, int32_t msqid, 
                        void *msqp, size_t msgsz, const CsTime_t *pAbstime));
extern int32_t OlsMsgRcv __P((OLSMSGHD *hMsg, int32_t msqid,
                        void *msqp, size_t msgsz, int32_t msgtyp, 
 int32_t msgflg, size_t *pSize, const CsTime_t *pAbstime));
__END_DECLS

#endif   /*OLSMSG_H_INCLUDED*/



