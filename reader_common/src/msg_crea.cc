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
    Message create
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
 
    $Id: msg_crea.c,v 1.2 2001/06/21 12:57:30 ayoung Exp $
*
* Version   Who      Date        Description
  1.0       APY      11/07/98    Created
  1.3       APY      03/06/99    Included path in diad messages
		     20/06/01    Updated to new CSSHM interface.
*
* $Name:  $
* $Source: /export/repository/cvs/core/olsbase/src/msg/msg_crea.c,v $
* $Revision: 1.2 $ $Date: 2001/06/21 12:57:30 $ $State: Exp $ 
* $Author: ayoung $ $Locker:  $
*.........................................................................*/

#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>                          
#include <limits.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <cs.h>
#include "extern.h"
#include "olsmsg.h"

static int32_t Size(OlsMsgInfo_t *pInfo, uint32_t *pSize);
static  void            Init(struct olsmsgctl *pMsg, 
                            OlsMsgInfo_t *pInfo, uint32_t size);

 int32_t
OlsMsgCreate(         
    OLSMSGHD **phMsg, OlsMsgInfo_t *pInfo, const char *pPath)
{
    struct olsmsgctl *pInst;
    OLSMSGHD *hMsg;
 uint32_t size;
 int32_t ret;

    if (phMsg == NULL || pInfo == NULL || pPath == NULL)
        return (EFAULT);
    
    *phMsg = NULL;    
    if ((ret = __OlsMsgSize(pInfo, &size)) != 0)
        return (ret);

    if ((hMsg = (OLSMSGHD *)CsCalloc(1, (uint32_t)sizeof(OLSMSGHD))) == NULL)
        return (ENOMEM);

    if ((ret = CsShmCreate( &hMsg->msgshm, (void **)&pInst,
            (u_int)size, pPath, "olsmsg", O_RDWR | O_EXCL)) != 0)
    {
        CsFree(hMsg);
        CsErrx("OlsMsg::Create - opening message exchange (%s) : %s",
            pPath, strerror(ret));
        return (ret);
    }
        
    __OlsMsgInit(pInst, pInfo, size);
    (void) __OlsMsgRef( hMsg, pInst );
    (void) CsShmUnlock( hMsg->msgshm );
    *phMsg = hMsg;

    return (0);
}


/*  Size ---
 *      Determine requirements,
 */     
 int32_t
__OlsMsgSize(                   
    OlsMsgInfo_t *pInfo, uint32_t *pSize )
{
 uint32_t size = 0;
 int32_t i;

/* Control structure */
    size = (uint32_t)sizeof(struct olsmsgctl);

/* Segments,
    *   msginfo.msgssz should be a power of two for efficiency
    *   reasons.  It is also pretty silly if msginfo.msgssz is
    *   less than 8 or greater than about 256 so ...
    */
    i = 8;
    while (i < 1024 && i != pInfo->msgssz)
        i <<= 1;
    if (i != pInfo->msgssz)
    {
        errno = EINVAL;
        CsErr("OlsMsg::Create - msgssz=%d (0x%x)", 
            pInfo->msgssz, pInfo->msgssz);
        return (errno);
    }
    if (pInfo->msgseg < 0 || pInfo->msgseg > LONG_MAX)
    {        
        CsErr("OlsMsg::Create - msgseg=%d (0x%x)", 
            pInfo->msgseg, pInfo->msgssz);
        return (errno);
    }
    size += pInfo->msgseg * pInfo->msgssz;

/* Message maps */
    size += pInfo->msgseg * (int32_t)sizeof(struct olsmsgmap);

/* Message headers */
    size += pInfo->msgtql * (int32_t)sizeof(struct olsmsg);

/* Message identifiers */
    size += pInfo->msgmni * (int32_t)sizeof(struct olsmsqid);

    CsDebug(5, (5, "OlsMsg::Size - ssz=%d,seg=%d,tql=%d,mni=%d,size=%lu",
        pInfo->msgssz, pInfo->msgseg, pInfo->msgtql, pInfo->msgmni, size));                   
            
    *pSize = size;

    return (0);
}


/*  Init ---
 *      Initialise message region
 */
void
__OlsMsgInit(            
    struct olsmsgctl *pInst, OlsMsgInfo_t *pInfo, uint32_t size )
{
 uint32_t offset;
 int32_t i;

/* Header */
    (void) memset(pInst, 0, size);
    pInst->msg_magic = OLSMSG_MAGIC;
    pInst->msg_ver   = VERSION(OLSMSG_VERSION, 0);
    pInst->msg_size  = size;
    pInst->msg_info  = *pInfo; 
    CsMutexInit(&pInst->msg_mutex, CSSYNC_PROCESS);
    CsCondInit(&pInst->msg_rcvcond, CSSYNC_PROCESS); 
    CsCondInit(&pInst->msg_sndcond, CSSYNC_PROCESS); 
    offset = (uint32_t)sizeof(struct olsmsgctl);

/* Message segments */
    pInst->msg_seq   = offset;
    offset += pInfo->msgseg * pInfo->msgssz;

/* Message maps */
    pInst->msg_maps  = offset;
    {
        struct olsmsgmap *msgmaps   
                = (struct olsmsgmap *)(((char *)pInst) + offset);

        for (i = 0; i < pInfo->msgseg; i++) 
        {
            if (i > 0)
                msgmaps[i-1].next = i;
            msgmaps[i].next = -1;           /* Implies entry is available */
        }
        pInst->free_msgmaps = 0;
        pInst->nfree_msgmaps = pInfo->msgseg;
    }
    offset += pInfo->msgseg * (int32_t)sizeof(struct olsmsgmap);

/* Message headers */
    pInst->msg_hdrs  = offset;
    {
        struct olsmsg *msghdrs      
                = (struct olsmsg *)(((char *)pInst) + offset);

        for (i=0; i < pInfo->msgtql; i++) 
        {
            msghdrs[i].msg_type = 0;
            if (i > 0)
                msghdrs[i-1].msg_next = i;
            msghdrs[i].msg_ident = i;       /* Ident */
            msghdrs[i].msg_next = -1;       /* Implies end of list */
        }
        pInst->free_msghdrs = 0;
    }
    offset += pInfo->msgtql * (int32_t)sizeof(struct olsmsg);

/* Message identifiers */
    pInst->msg_ids   = offset;
    {
        struct olsmsqid *msqids     
                = (struct olsmsqid *)(((char *)pInst) + offset);

        for (i = 0; i < pInfo->msgmni; i++) 
        {
            msqids[i].msg_first = -1;
            msqids[i].msg_last = -1;
            msqids[i].msg_qbytes = 0;       /* Implies entry is available */
        }
    }
    offset += pInfo->msgmni * (int32_t)sizeof(struct olsmsqid);

    CsAssert(offset == pInst->msg_size, "Hmmm");
}
