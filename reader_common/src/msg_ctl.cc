/* -*- c: tabs: 4 -*- ******************************************************
* Copyright (C) 1998 ERG Limited, All rights reserved
*
* Module name   : msg_ctl.c
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
* $Source: /export/repository/cvs/core/olsbase/src/msg/msg_ctl.c,v $
* $Revision: 1.1.1.1 $ $Date: 2000/12/19 01:32:22 $ $State: Exp $ 
* $ProjectName: /export/dev/olss/ols/base.pj $ $ProjectRevision: 1.1 $
* $Author: raj $ $Locker:  $
*.........................................................................*/

#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <cs.h>
#include "extern.h"
#include "olsmsg.h"
                         
static  int             MsgCtl(OLSMSGHD *hMsg, 
                            int msqid, int cmd, struct olsmsqid *buf);

int32_t
OlsMsgCtl(               
    OLSMSGHD *hMsg,
    int msqid, int cmd,
    struct olsmsqid *buf)
{
    int ret;
   
    if (hMsg == NULL || buf == NULL )
         return EINVAL;    

    CsDebug(3, (3, "OlsMsg::Ctl(%p, %d, %d, %p)", hMsg, msqid, cmd, buf));
    if (hMsg->msgmagic != OLSMSG_MAGIC) 
    {
        CsDebug(3, (3, "OlsMsg::Ctl - invalid magic"));
        return(EINVAL);
    }                      
    if ((ret = CsMutexLock(&hMsg->msgctl->msg_mutex)) == 0)
    {            
        ret = MsgCtl(hMsg, msqid, cmd, buf);
        (void) CsMutexUnlock(&hMsg->msgctl->msg_mutex);
    }
    return (ret);
}


static int
MsgCtl(
    OLSMSGHD *hMsg,
    int msqid, int cmd,
    struct olsmsqid *buf)
{
    register struct olsmsgctl *pInst;
    register struct olsmsqid *msqptr;
    int ret;
    
    if (hMsg == NULL || buf == NULL)
         ret = EINVAL;       

    pInst = hMsg->msgctl;
    
    if (msqid < 0 || msqid >= pInst->msg_info.msgmni) {
        CsDebug(3, (3, "OlsMsg::Ctl - msqid (%d) out of range (0<=msqid<%d)",
            msqid, pInst->msg_info.msgmni));
        return(EINVAL);
    }

    msqptr = &hMsg->msqids[msqid];

    if (msqptr->msg_qbytes == 0) {
        CsDebug(3, (3, "OlsMsg::Ctl - no such msqid"));
        return(EINVAL);
    }

    ret = 0;
    switch (cmd) 
    {
    case IPC_RMID:
        {
            struct olsmsg *msghdr;
     
        /* Free the message headers */
            msghdr = msgRef(msqptr->msg_first);
            while (msghdr != NULL) 
            {
                struct olsmsg *msghdr_tmp;

            /* Free the segments of each message */
                msqptr->msg_cbytes -= msghdr->msg_ts;
                msqptr->msg_qnum--;
                msghdr_tmp = msghdr;
                msghdr = msgRef(msghdr->msg_next);
                __OlsMsgFree(hMsg, msghdr_tmp);
            }           
            
            CsDebug(3, (3, "OlsMsg::Ctl - RMID cbytes=%lu, qnum=%lu",
                msqptr->msg_cbytes, msqptr->msg_qnum));
            if (msqptr->msg_cbytes != 0)
                CsAssertion("OlsMsg::Ctl - msg_cbytes is screwed up");
            if (msqptr->msg_qnum != 0)
                CsAssertion("OlsMsg::Ctl - msg_qnum is screwed up");

            msqptr->msg_qbytes = 0;                     /* Mark it as free */
                                  
            (void)CsCondBcast(&pInst->msg_rcvcond);     /* Tell world */
            (void)CsCondBcast(&pInst->msg_sndcond);
        }
        break;                                          
        
    case IPC_SET:
        if (buf->msg_qbytes > pInst->msg_info.msgmnb) {
            CsDebug(3, (3, "OlsMsg::Ctl - "\
                "can't increase msg_qbytes beyond %d (truncating)",
                pInst->msg_info.msgmnb));
            buf->msg_qbytes = pInst->msg_info.msgmnb;   /* restrict qbytes */
        }
        if (buf->msg_qbytes == 0) {
            CsDebug(3, (3, "OlsMsg::Ctl - can't reduce msg_qbytes to 0"));
            return(EINVAL);                             /* non-standard errno! */
        }
        msqptr->msg_flags  = (msqptr->msg_flags & ~0777) |
            (buf->msg_flags & 0777);
        msqptr->msg_qbytes = buf->msg_qbytes;
        msqptr->msg_ctime  = time(NULL);
        break;

    case IPC_STAT:
        (void)memcpy(buf, (const char *)msqptr, sizeof(struct olsmsqid));
        break;

    default:
        CsDebug(3, (3, "OlsMsg::Ctl - invalid command %d", cmd));
        return(EINVAL);
    }

    return(ret);
}
