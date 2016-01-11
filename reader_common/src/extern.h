#ifndef EXTERN_H_INCLUDED
#define EXTERN_H_INCLUDED
/* -*- c: tabs: 4 -*- ******************************************************
* Copyright (C) 1998-2001 ERG Limited, All rights reserved
*
* Module name   : msg_create.c
* Module type   : OLS messaging
* Compiler(s)   : ANSI C
* Environment(s): Solaris
*
* Description:
*
    Private header
*
* Version   Who      Date        Description
  1.0       APY      11/07/98    Created
	    APY      20/06/01	 OLSMSG_VERSION(2) for new CSSHM.
*          
* $Name:  $
* $Source: /export/repository/cvs/core/olsbase/src/msg/extern.h,v $
* $Revision: 1.2 $ $Date: 2001/06/21 12:57:30 $ $State: Exp $ 
* $Author: ayoung $ $Locker:  $
*.........................................................................*/

#include <errno.h>
//#include <olsmsg.h>                 /* Public header */
#include "olsmsg.h"
#include <csmutex.h>

__BEGIN_DECLS

#ifndef EIDRM
#define EIDRM           EINVAL
#endif
#ifndef ENOMSG
#define ENOMSG          EAGAIN
#endif

#define OLSMSG_MAGIC    0x01020304  /* Structure magic */

#define OLSMSG_VERSION  2           /* Interface version */

#define OLSMSG_LOCKED   01000       /* Is this msqid_ds locked? */
#ifndef OLSMSGSSZ
#define OLSMSGSSZ       8           /* Each segment must be 2^N int32_t */
#endif
#ifndef OLSMSGSEG
#define OLSMSGSEG       2048        /* must be less than 2^31 */
#endif
#define OLSMSGMAX       (OLSMSGSSZ*OLSMSGSEG)
#ifndef OLSMSGMNB
#define OLSMSGMNB       2048        /* max # of bytes in a queue */
#endif
#ifndef OLSMSGMNI
#define OLSMSGMNI       40
#endif
#ifndef OLSMSGTQL
#define OLSMSGTQL       40
#endif

#define VERSION(a,b)    (((uint32_t)(a) << 16) | (b))

#define msgRef(I)       ((I) == -1 ? NULL : hMsg->msghdrs+(I))
#define msgIdent(P)     ((P)->msg_ident)

struct olsmsgmap {
 int32_t next;          /* next segment in buffer */
                                    /* -1 -> available */
                                    /* 0..(MSGSEG-1) -> idx of next segment */
};

struct olsmsgctl {
 uint32_t msg_magic;      /* Structure magic */
 uint32_t msg_ver;        /* Version */
 uint32_t msg_size;       /* Size of region */
    OlsMsgInfo_t    msg_info;       /* Copy of info block */
    CsMutex_t       msg_mutex;      /* Mutex */
    CsCond_t        msg_rcvcond;
    CsCond_t        msg_sndcond;
                                    /* --- Maintainence */
 uint32_t nfree_msgmaps;  /* # of free map entries */
 int32_t free_msgmaps;   /* head of free map entries list */
 int32_t free_msghdrs;   /* list of free msg headers */
                                    /* --- Starting offset of tables */
 uint32_t msg_seq;
 uint32_t msg_maps;
 uint32_t msg_hdrs;
 uint32_t msg_ids;
};
                    
extern int32_t __OlsMsgSize(OlsMsgInfo_t *pInfo, uint32_t *pSize);
extern  void        __OlsMsgInit(struct olsmsgctl *pInst, 
                        OlsMsgInfo_t *pInfo, uint32_t size);

extern int32_t __OlsMsgRef(OLSMSGHD *phMsg, struct olsmsgctl *pMsg);
extern  void        __OlsMsgFree(OLSMSGHD *phMsg, struct olsmsg *msghdr);

__END_DECLS

#endif  /*EXTERN_H_INCLUDED*/


