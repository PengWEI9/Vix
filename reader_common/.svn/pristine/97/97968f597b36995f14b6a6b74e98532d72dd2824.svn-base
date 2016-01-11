/* -*- c: tabs: 4 -*- ******************************************************
* Copyright (C) 1998 ERG Limited, All rights reserved
*
* Module name   : msg_get.c
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
* $Source: /export/repository/cvs/core/olsbase/src/msg/msg_get.c,v $
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

static  int             MsgGet(OLSMSGHD *hMsg, key_t key, int msgflg);
           
int32_t
OlsMsgGet(
    OLSMSGHD *hMsg,
    key_t key,
    int msgflg)
{
    int ret;
    
    if (hMsg == NULL )
         return EINVAL;       

    CsDebug(7, (7, "OlsMsg::Get(%p, 0x%x, 0%o)", hMsg, key, msgflg));
    if (hMsg->msgmagic != OLSMSG_MAGIC) 
    {
        CsDebug(3, (3, "OlsMsg::Snd - invalid magic"));
        errno = EINVAL;
        return(-1);
    }                      
    if ((ret = CsMutexLock(&hMsg->msgctl->msg_mutex)) == 0)
    {            
        ret = MsgGet(hMsg, key, msgflg);
        (void) CsMutexUnlock(&hMsg->msgctl->msg_mutex);
    }
    return (ret);
}


static int
MsgGet(
    OLSMSGHD *hMsg,
    key_t key,
    int msgflg)
{
    register struct olsmsgctl *pInst;
    register struct olsmsqid *msqptr;
    int msqid;

    if (hMsg == NULL )
         return EINVAL; 
     
    msqptr = NULL;
    pInst = hMsg->msgctl;

    if (key != IPC_PRIVATE) 
    {
        for (msqid = 0; msqid < pInst->msg_info.msgmni; msqid++) 
        {
            msqptr = &hMsg->msqids[msqid];
            if (msqptr->msg_qbytes != 0 && msqptr->msg_key == key)
                break;
        }
        if (msqid < pInst->msg_info.msgmni) 
        {
            CsDebug(7, (7, "OlsMsg::Get - found public key"));
            if ((msgflg & IPC_CREAT) && (msgflg & IPC_EXCL)) 
            {
                CsDebug(3, (3, "OlsMsg::Get - not exclusive"));
                return(EEXIST);
            }
            goto found;
        }
    }                                    
    
    CsDebug(5, (5, "OlsMsg::Get - need to allocate the msqid_ds"));
    if (key == IPC_PRIVATE || (msgflg & IPC_CREAT))
    {
        for (msqid = 0; msqid < pInst->msg_info.msgmni; msqid++)
        {
            /* Look for an unallocated and unlocked msqid_ds.
             * msqid_ds's can be locked by msgsnd or msgrcv while
             * they are copying the message in/out.  We can't
             * re-use the entry until they release it.
             */
            msqptr = &hMsg->msqids[msqid];
            if (msqptr->msg_qbytes == 0 &&
                    (msqptr->msg_flags & OLSMSG_LOCKED) == 0)
                break;
        }
        if (msqid == pInst->msg_info.msgmni) {
            CsDebug(3, (3, "OlsMsg::Get - no more msqid's available"));
            return(ENOSPC);
        }
        CsDebug(3, (3, "OlsMsg::Get - msqid %d is available", msqid));
        msqptr->msg_key     = key;
        msqptr->msg_flags   = (msgflg & 0777);

    /* Make sure that the returned msqid is unique */
        msqptr->msg_first   = -1;
        msqptr->msg_last    = -1;
        msqptr->msg_cbytes  = 0;
        msqptr->msg_qnum    = 0;
        msqptr->msg_qbytes  = pInst->msg_info.msgmnb;
        msqptr->msg_lspid   = 0;
        msqptr->msg_lrpid   = 0;
        msqptr->msg_stime   = 0;
        msqptr->msg_rtime   = 0;
        msqptr->msg_ctime   = time(NULL);
    } 
    else 
    {
        CsDebug(3, (3, "OlsMsg::Get - "\
            "didn't find it and wasn't asked to create it"));
        errno = ENOENT;
        return (-1);
    }

found:
    return (msqid);
}
