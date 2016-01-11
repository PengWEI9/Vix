/* -*- c: tabs: 4 -*- ******************************************************
* Copyright (C) 1998 ERG Limited, All rights reserved
*
* Module name   : msg_snd.c
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
* $Source: /export/repository/cvs/core/olsbase/src/msg/msg_snd.c,v $
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
#include "olsmsg_priv.h"
                            
                       
/*----------------------------------------------------------------------------
    Synposis:

 int32_t OlsMsgSnd
            (
               OLSMSGHD *hMsg,
 int32_t msqid, void *msgp, size_t msgsz,
               const CsTime_t *pAbsTime  
            )

    Description:                  

         The OlsMsgSnd() sends a message to the queue associated with the
         message queue identifier specified by 'hMsg' and 'msgid'. 'msgp'
         points to a user defined buffer that must contain first a field 
         of type int32_t integer that will specify the type of the message,
         and then a data portion that will hold the text of the message.
         The following is an example of members that might be in a user 
         defined buffer.                
                       
            typedef struct {
 int32_t mtype;          (* message type (+ve integer) *)
                char    mtext[1];       (* message body *)
            } OlsMsg_t;                                  
                        
         'mtype' is a positive integer that can be used by the receiving 
         process for message  selection. 'mtext' is any text of length
         'msgsz' bytes. 'msgsz' can range from 0 to a system imposed maximum.
         
         'pAbstime' specifies the action to be taken if one or more of the 
         following are true:
         
            The number of bytes already on the queue is equal to 
            'msg_qbytes'.

            The total number of messages on all queues system-wide
            is equal to the system-imposed limit.

         These actions are as follows:

            If (pAbstime == (NULL)-1) is true, the message is not sent
            and the calling process returns immediately.

            If (pAbstime == NULL) is true, the calling process suspends 
            execution until one of the following occurs:

                o   The condition responsible for the suspension no longer
                    exists, in which case the message is sent.
                  
                o   'msqid' is removed from the system (see OlsMsgCtl()). 
                    When this occurs, the value of EIDRM is returned.

                o   The calling process receives a signal that is to be
                    caught.  In this case the message is not sent and the 
                    calling process resumes execution in the manner    
                    prescribed in signal(3C).        
                  
            otherwise, the calling process suspends execution until on of
            the following occurs:  
            
                o   The absolute time specified by 'pAbstime' has passed.

                o   The condition responsible for the suspension no longer
                    exists, in which case the message is sent.
                  
                o   'msqid' is removed from the system (see OlsMsgCtl()). 
                    When this occurs, the value of EIDRM is returned.

                o   The calling process receives a signal that is to be
                    caught.  In this case the message is not sent and the 
                    calling process resumes execution in the manner    
                    prescribed in signal(3C).        
   
    Returns: int32_t -   OlsMsgSnd returns 0 on success; otherwise it returns
                        a non-zero values to indicate the error condition. 
                        
                        
    Errors:         
    
        EINVAL      'hMsg' does not correspond to a valid message queue

                    'mtyped' is less than 1.
                    
                    'msgsz is less than zero or greater than the 
                    system-imposed limit.
        
        EACCES      Operation permission is denied to the calling
                    process.
                              
        EIDRM       'msgid' has been removed.
        
        EINTR       The wait was interrupted by a signal or fork().    
        
        EAGAIN      The message can not be sent for one of the 
                    conditions cited above.
---------------------------------------------------------------------------*/
 int32_t
OlsMsgSnd(
    OLSMSGHD *hMsg,
 int32_t msqid, void *msgp, size_t msgsz,
    const CsTime_t *pAbstime)
{
    int32_t ret;
    if (hMsg == NULL )
        return EINVAL; 
    
    CsDebug(7, (7, "OlsMsg::Snd(%p, %d, %p, %d, %p)",
        hMsg, msqid, msgp, msgsz, pAbstime));
    if (hMsg->msgmagic != OLSMSG_MAGIC) 
    {
        CsDebug(3, (3, "OlsMsg::Snd - invalid magic"));
        return(EINVAL);
    }                      
    if ((ret = CsMutexLock(&hMsg->msgctl->msg_mutex)) == 0)
    {            
        ret = _OlsMsgSnd(hMsg, msqid, msgp, msgsz, pAbstime);
        (void) CsMutexUnlock(&hMsg->msgctl->msg_mutex);
        (void) CsCondBcast(&hMsg->msgctl->msg_sndcond);
    }
    else
    {
        CsDebug(5, (5, "OlsMsg::Snd - failed to get mutex"));
    }
    return (ret);
}


int32_t
_OlsMsgSnd(
    OLSMSGHD *hMsg,
 int32_t msqid, void *msgp, size_t msgsz,
    const CsTime_t *pAbstime)
{
    register struct olsmsgctl *pInst;
    register struct olsmsqid *msqptr;
    register struct olsmsg *msghdr;
 int32_t needed, ret;
 int32_t next;         
        
    pInst = hMsg->msgctl;
     
    if (msqid < 0 || msqid >= pInst->msg_info.msgmni) {
        CsDebug(3, (3, "OlsMsg::Snd - msqid (%d) out of range (0<=msqid<%d)",
            msqid, pInst->msg_info.msgmni));
        return(EINVAL);
    }
    msqptr = &hMsg->msqids[msqid];

    if (msqptr->msg_qbytes == 0) {
        CsDebug(3, (3, "OlsMsg::Snd - no such message queue id"));
        return(EINVAL);
    }

    needed = (int) ((msgsz + pInst->msg_info.msgssz - 1) / pInst->msg_info.msgssz);
    CsDebug(7, (7, "OlsMsg::Snd - msgsz=%d, msgssz=%d, needed=%d",
        msgsz, pInst->msg_info.msgssz, needed));

    for (;;)
    {
 int32_t need_more_resources = 0;

    /* check msgsz
        *   (inside this loop in case msg_qbytes changes while we sleep)
        */
        if (msgsz > msqptr->msg_qbytes) {
            CsDebug(3, (3, "OlsMsg::Snd - msgsz > msqptr->msg_qbytes"));
            return(EINVAL);
        }
        if (msqptr->msg_flags & OLSMSG_LOCKED) {
            CsDebug(3, (3, "OlsMsg::Snd - msqid is locked"));
            need_more_resources = 1;
        }
        if (msgsz + msqptr->msg_cbytes > msqptr->msg_qbytes) {
            CsDebug(3, (3, "OlsMsg::Snd - msgsz + msg_cbytes > msg_qbytes"));
            need_more_resources = 1;
        }
        if (needed > pInst->nfree_msgmaps) {
            CsDebug(3, (3, "OlsMsg::Snd - needed > nfree_msgmaps"));
            need_more_resources = 1;
        }
        if (pInst->free_msghdrs == -1) {
            CsDebug(3, (3, "OlsMsg::Snd - no more msghdrs"));
            need_more_resources = 1;
        }

        if (need_more_resources) {
 int32_t we_own_it;                        
            
            if (pAbstime == (const CsTime_t *)-1) {
                CsDebug(3, (3, "OlsMsg::Snd - "\
                    "need more resources but caller doesn't want to wait"));
                return(EAGAIN);
            }

            if ((msqptr->msg_flags & OLSMSG_LOCKED) != 0) {
                CsDebug(3, (3, "OlsMsg::Snd - we don't own the olsmsgid"));
                we_own_it = 0;
            } else {
            /* Force later arrivals to wait for our request */
                CsDebug(3, (3, "OlsMsg::Snd - we own the olsmsgid"));
                msqptr->msg_flags |= OLSMSG_LOCKED;
                we_own_it = 1;
            }

            CsDebug(3, (3, "OlsMsg::Snd - goodnight")); 
            if (pAbstime == (const CsTime_t *)NULL)
                ret = CsCondWait(&pInst->msg_rcvcond, &pInst->msg_mutex);
            else {  
                ret = CsCondWaittm(&pInst->msg_rcvcond, &pInst->msg_mutex, pAbstime);
            }
            CsDebug(3, (3, "OlsMsg::Snd - good morning, ret=%d", ret));
            if (we_own_it)
                msqptr->msg_flags &= ~OLSMSG_LOCKED;
            if (ret != 0) 
            {                      
                if (ret == ETIME || ret == EAGAIN) 
                {                              
                    CsDebug(3, (3, "OlsMsg::Snd - request time-out"));
                    return (EAGAIN);
                }
                CsDebug(3, (3, "OlsMsg::Snd - interrupted system call"));
                return(EINTR);
            }

        /* Make sure that the msq queue still exists */
            if (msqptr->msg_qbytes == 0) 
            {
                CsDebug(3, (3, "OlsMsg::Snd - msqid deleted"));
                return(EIDRM);
            }
        } 
        else 
        {
            CsDebug(7, (7, "OlsMsg::Snd - got all the resources that we need"));
            break;
        }
    }

/* We have the resources that we need (Make sure!) */
    if (msqptr->msg_flags & OLSMSG_LOCKED)
        CsAssertion("OlsMsg::Snd - msg_flags & OLSMSG_LOCKED");
    if (needed > pInst->nfree_msgmaps)
        CsAssertion("OlsMsg::Snd - needed > nfree_msgmaps");
    if (msgsz + msqptr->msg_cbytes > msqptr->msg_qbytes)
        CsAssertion("OlsMsg::Snd - msgsz + msg_cbytes > msg_qbytes");
    if (pInst->free_msghdrs == -1)
        CsAssertion("OlsMsg::Snd - no more msghdrs");

/* Re-lock the olsmsgid in case we page-fault when copying in the message */
    if ((msqptr->msg_flags & OLSMSG_LOCKED) != 0)
        CsAssertion("OlsMsg::Snd - olsmsgid is already locked");
    msqptr->msg_flags |= OLSMSG_LOCKED;

/* Allocate a message header */
    msghdr = msgRef(pInst->free_msghdrs);
    pInst->free_msghdrs = msghdr->msg_next;
    msghdr->msg_spot = -1;
    msghdr->msg_ts = (unsigned short)msgsz;

/* Allocate space for the message */
    while (needed > 0) 
    {
        if (pInst->nfree_msgmaps <= 0)
            CsAssertion("OlsMsg::Snd - not enough msgmaps");
        if (pInst->free_msgmaps == -1)
            CsAssertion("OlsMsg::Snd - nil free_msgmaps");
        next = pInst->free_msgmaps;
        if (next <= -1)
            CsAssertion("OlsMsg::Snd - next too low #1");
        if (next >= pInst->msg_info.msgseg)
            CsAssertion("OlsMsg::Snd - next out of range #1");
        CsDebug(7, (7, "OlsMsg::Snd - "\
            "allocating segment %d to message", next));
        pInst->free_msgmaps = hMsg->msgmaps[next].next;
        pInst->nfree_msgmaps--;
        hMsg->msgmaps[next].next = msghdr->msg_spot;
        msghdr->msg_spot = next;
        needed--;
    }

/* Copy in the message type */
    (void) memcpy(&msghdr->msg_type, msgp, sizeof(msghdr->msg_type));
    msgp = (char *)msgp + sizeof(msghdr->msg_type);

/* Validate the message type */
    if (msghdr->msg_type < 1) 
    {
        __OlsMsgFree(hMsg, msghdr);
        msqptr->msg_flags &= ~OLSMSG_LOCKED;
        CsDebug(3, (3, "OlsMsg::Snd - mtype (%d) < 1", msghdr->msg_type));
        return(EINVAL);
    }

/* Copy in the message body */
    ret = 0;
    next = msghdr->msg_spot;
    while (msgsz > 0)
    {
        size_t tlen;

        if (msgsz > pInst->msg_info.msgssz)
            tlen = pInst->msg_info.msgssz;
        else tlen = msgsz;

        if (next <= -1) {
            CsAssertion("next too low #2");
        } else if (next >= pInst->msg_info.msgseg) {
            CsAssertion("next out of range #2");
        }
        memcpy(&hMsg->msgpool[next * pInst->msg_info.msgssz], msgp, tlen);
        msgsz -= tlen;
        msgp = (char *)msgp + tlen;
        next = hMsg->msgmaps[next].next;
    }
    if (next != -1)
        CsAssertion("didn't use all the msg segments");

/* We've got the message. Unlock the olsmsgid */
    msqptr->msg_flags &= ~OLSMSG_LOCKED;

/* Make sure that the olsmsgid is still allocated */
    if (msqptr->msg_qbytes == 0) 
    {
        __OlsMsgFree(hMsg, msghdr);
        return(EIDRM);
    }

/* Put the message into the queue */
    if (msqptr->msg_first == -1) {
        msqptr->msg_first = msgIdent(msghdr);
        msqptr->msg_last  = msgIdent(msghdr);
    } else {
        msgRef(msqptr->msg_last)->msg_next = msgIdent(msghdr);
        msqptr->msg_last  = msgIdent(msghdr);
    }
    msgRef(msqptr->msg_last)->msg_next = -1;

    msqptr->msg_cbytes += msghdr->msg_ts;
    msqptr->msg_qnum++;
    CsDebug(7, (7, "OlsMsg::Snd - cbytes=%lu, qnum=%lu",
        msqptr->msg_cbytes, msqptr->msg_qnum));

#if (0)
    msqptr->msg_lspid = getpid();
    msqptr->msg_stime = time(NULL);
#endif
    return(0);
}

