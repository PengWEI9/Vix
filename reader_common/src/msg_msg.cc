/* -*- c: tabs: 4 -*- ******************************************************
* Copyright (C) 1998 ERG Limited, All rights reserved
*
* Module name   : msg_msg.c
* Module type   : OLS messaging
* Compiler(s)   : ANSI C
* Environment(s): Solaris
*
* Description:
*
    Message open
*
* Version   Who      Date        Description
  1.0       APY      11/07/98    Created
*
* $Name:  $
* $Source: /export/repository/cvs/core/olsbase/src/msg/msg_msg.c,v $
* $Revision: 1.1.1.1 $ $Date: 2000/12/19 01:32:22 $ $State: Exp $ 
* $ProjectName: /export/dev/olss/ols/base.pj $ $ProjectRevision: 1.1 $
* $Author: raj $ $Locker:  $
*.........................................................................*/

#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <cs.h>
#include "extern.h"
#include "olsmsg.h"


 int32_t
__OlsMsgRef(
    OLSMSGHD *hMsg,
    struct olsmsgctl *pMsg)
{
    OlsMsgInfo_t *pInfo;
 uint32_t offset;

    pInfo = &pMsg->msg_info;
 
    hMsg->msgmagic = OLSMSG_MAGIC;
 
    hMsg->msgctl   = pMsg;
    offset = (uint32_t)sizeof(struct olsmsgctl);

    hMsg->msgpool  = (((char *)pMsg) + offset);
    offset += pInfo->msgseg * pInfo->msgssz;

    hMsg->msgmaps  = (struct olsmsgmap *)(((char *)pMsg) + offset);
    offset += pInfo->msgseg * sizeof(struct olsmsgmap);

    hMsg->msghdrs  = (struct olsmsg *)(((char *)pMsg) + offset);
    offset += pInfo->msgtql * sizeof(struct olsmsg);

    hMsg->msqids   = (struct olsmsqid *)(((char *)pMsg) + offset);
    offset += pInfo->msgmni * sizeof(struct olsmsqid);

    return (0);
}


void
__OlsMsgFree(
    OLSMSGHD *hMsg,
    struct olsmsg *msghdr)
{
    struct olsmsgctl *pInst;
 int32_t next;

    pInst = hMsg->msgctl;

    while (msghdr->msg_ts > 0)
    {
        if (msghdr->msg_spot < 0 || msghdr->msg_spot >=
                pInst->msg_info.msgseg)
            CsAssertion("msghdr->msg_spot out of range");

        next = hMsg->msgmaps[msghdr->msg_spot].next;
        hMsg->msgmaps[msghdr->msg_spot].next = pInst->free_msgmaps;
        pInst->free_msgmaps = msghdr->msg_spot;
        pInst->nfree_msgmaps++;
        msghdr->msg_spot = next;

        if (msghdr->msg_ts >= pInst->msg_info.msgssz)
            msghdr->msg_ts -= pInst->msg_info.msgssz;
        else msghdr->msg_ts = 0;
    }

    if (msghdr->msg_spot != -1)
        CsAssertion("msghdr->msg_spot != -1");

    msghdr->msg_next = pInst->free_msghdrs;
    pInst->free_msghdrs = msgIdent(msghdr);
}
