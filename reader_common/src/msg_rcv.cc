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
                     27/09/98    Absolute timeout   
*
* $Name:  $
* $Source: /export/repository/cvs/core/olsbase/src/msg/msg_rcv.c,v $
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

 int32_t OlsMsgRcv
            (
               OLSMSGHD *hMsg, int32_t msqid, 
               void *msgp, size_t msgsz, int32_t msgtyp, int32_t msgflg, size_t *pSize,
               const CsTime_t *pAbsTime  
            )

    Description:                  
 
        The OlsMsgRcv() functions reads a message from the queue 
        associated with the message queue identifier specified by 
        'hMsg' and 'msqid' and places it in the user defined structure
        pointed to by 'msgp'.   The structure must contain a message
        type field followed by the area for the message text (see the 
        structure OlsMsg_t, defined within OlsMsgSnd()).  'mtype'
        is the  received message's type as specified by the sending
        process.  mtext is the text of the message.  msgsz specifies
        the size in bytes of mtext.

        The received message is truncated to msgsz bytes  if  it  is
        larger than 'msgsz'  and  (msgflg & OLSMSG_NOERROR) is true.  The
        truncated part of the message is lost and no indication of then
        truncation is given to the calling process.

        'msgtyp' specifies the type of message requested as follows:

            If 'msgtyp' is 0, the  first  message  on  the  queue  is
            received.

            If 'msgtyp' is greater than 0, the first message of type 'msgtyp'
            is received.
         
            If 'msgtyp' is less than 0, the first message of the lowest type 
            that is less than or equal to the absolute  value of 'msgtyp'
            is received.

        'pAbstime' specifies the action to be taken if a message of  the
        desired type is not on the queue.  These are as follows:

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

        EACCES      Operation permission is denied to the calling
                    process.
                              
        EIDRM       'msgid' has been removed.
        
        EINTR       The wait was interrupted by a signal or fork().    
        
        EAGAIN      The message can not be sent for one of the 
                    conditions cited above.
---------------------------------------------------------------------------*/
 int32_t
OlsMsgRcv(
    OLSMSGHD *hMsg, int32_t msqid,
    void *msgp, size_t msgsz, int32_t msgtyp, int32_t msgflg, size_t *pSize,
    const CsTime_t *pAbstime)
{
    int32_t ret;
    
    if (hMsg == NULL )
         return EINVAL; 

    CsDebug(7, (7, "OlsMsg::Rcv(%p, %d, %p, %d, %ld, %d, %p, %p)",        
        hMsg, msqid, msgp, msgsz, msgtyp, msgflg, pSize, pAbstime));
    if (hMsg->msgmagic != OLSMSG_MAGIC) 
    {
        CsDebug(3, (3, "OlsMsg::Rcv - invalid magic"));
        return(EINVAL);
    }                      
    if ((ret = CsMutexLock(&hMsg->msgctl->msg_mutex)) == 0)
    {            
        ret = _OlsMsgRcv(hMsg, msqid, msgp, msgsz, msgtyp, msgflg, pSize, pAbstime);
        (void) CsMutexUnlock(&hMsg->msgctl->msg_mutex);
        if (ret == 0)                         
            (void) CsCondBcast(&hMsg->msgctl->msg_rcvcond);        
    }
    return (ret);
}           


int32_t
_OlsMsgRcv(
    OLSMSGHD *hMsg, int32_t msqid,
    void *msgp, size_t msgsz, int32_t msgtyp, int32_t msgflg, size_t *pSize,
    const CsTime_t *pAbstime)
{
    register struct olsmsgctl *pInst;
    register struct olsmsqid *msqptr;
    register struct olsmsg *msghdr;
    short next;
    size_t len;
    int32_t ret;
 
    if(hMsg==NULL)
    {
        CsErrx("_OlsMsgRcv hMsg==NULL");
        return(EINVAL);
    } 

    pInst = hMsg->msgctl;

    if (msqid < 0 || msqid >= pInst->msg_info.msgmni) {
        CsDebug(3, (3, "OlsMsg::Rcv - msqid (%d) out of range (0<=msqid<%d)", 
            msqid, pInst->msg_info.msgmni));
        return(EINVAL);
    }
    msqptr = &hMsg->msqids[msqid];
               
    if (msqptr->msg_qbytes == 0) 
    {
        CsDebug(3, (3, "OlsMsg::Rcv - no such message queue id"));
        return(EINVAL);
    }

    msghdr = NULL;
    while (msghdr == NULL) 
    {
        if (msgtyp == 0) 
        {
            msghdr = msgRef(msqptr->msg_first);
            if (msghdr != NULL) 
            {
                if (msgsz < msghdr->msg_ts && (msgflg & OLSMSG_NOERROR) == 0) 
                {       
                    CsDebug(3, (3, "OlsMsg::Rcv - first message "\
                        "on the queue is too big (want %d, got %d)",
                        msgsz, msghdr->msg_ts));
                    return(E2BIG);
                }
                if (msqptr->msg_first == msqptr->msg_last) 
                {
                    msqptr->msg_first = -1;
                    msqptr->msg_last = -1;
                }
                else 
                {
                    msqptr->msg_first = msghdr->msg_next;
                    if (msqptr->msg_first == -1)
                        CsAssertion("OlsMsg::Rcv - "\
                           "msg_first/last screwed up #1");
                }
            }
        } 

        else 
        {                                                        
            struct olsmsg *previous;
            int32_t *prev;

            previous = NULL;
            prev = &(msqptr->msg_first);
            while ((msghdr = msgRef(*prev)) != NULL) 
            {
                /*
                 * Is this message's type an exact match or is this message's
                 * type less than or equal to the absolute value of a 
                 * negative msgtyp?
                 *
                 * Note:    That the second half of this test can NEVER be 
                 *          true if msgtyp is positive since msg_type is 
                 *          always positive!
                 */         
                if (msgtyp == msghdr->msg_type || msghdr->msg_type <= -msgtyp)
                {
                    CsDebug(3, (3, "OlsMsg::Rcv - found message "\
                        "type %d, requested %d", msghdr->msg_type, msgtyp));
                    if (msgsz < msghdr->msg_ts && 
                           (msgflg & OLSMSG_NOERROR) == 0)
                    {
                        CsDebug(3, (3, "OlsMsg::Rcv - requested message "\
                           "on the queue is too big (want %d, got %d)",
                            msgsz, msghdr->msg_ts));
                        return(E2BIG);
                    }
                    *prev = msghdr->msg_next;
                    if (msgIdent(msghdr) == msqptr->msg_last)
                    {
                        if (previous == NULL)
                        {
                            if (prev != &msqptr->msg_first)
                                CsAssertion("OlsMsg::Rcv - "\
                                    "msg_first/last screwed up #2");
                            msqptr->msg_first = -1;
                            msqptr->msg_last = -1;
                        } 
                        else 
                        {
                            if (prev == &msqptr->msg_first)
                                CsAssertion("OlsMsg::Rcv - "\
                                    "msg_first/last screwed up #3");
                            msqptr->msg_last = msgIdent(previous);
                        }
                    }
                    break;
                }
                previous = msghdr;
                prev = &(msghdr->msg_next);
            }
        }                    
        
    /* We've either extracted the msghdr for the appropriate message or 
        *   there isn't one. If there is one then bail out of this loop.
        */
        if (msghdr != NULL)
            break;

    /* Hmph!  No message found.  Does the user want to wait? */
        if (pAbstime == (const CsTime_t *)-1)
        {
            CsDebug(3, (3, "OlsMsg::Rcv - no "\
               "appropriate message found (msgtyp=%d)", msgtyp));
            return(ENOMSG);
        }
        
    /* Wait for something to happen */
        CsDebug(7, (7, "OlsMsg::Rcv - goodnight"));
        if (pAbstime == (const CsTime_t *)NULL)
            ret = CsCondWait(&pInst->msg_sndcond, &pInst->msg_mutex);
        else {  
            ret = CsCondWaittm(&pInst->msg_sndcond, &pInst->msg_mutex, pAbstime);  
        }
        CsDebug(7, (7, "OlsMsg::Rcv - good morning, ret=%d", ret));
        if (ret != 0) 
        {                      
            if (ret == ETIME || ret == EAGAIN) 
            {                              
                CsDebug(3, (3, "OlsMsg::Rcv - request time-out"));
                return (EAGAIN);
            }
            CsDebug(3, (3, "OlsMsg::Rcv - interrupted system call"));
            return(EINTR);
        }

     /* Make sure that the msq queue still exists */
        if (msqptr->msg_qbytes == 0) {
            CsDebug(3, (3, "OlsMsg::Rcv - msqid deleted"));
            return(EIDRM);
        }
    }

/* Return the message to the user,
    *   First, do the bookkeeping (before we risk being interrupted).
    */
    msqptr->msg_cbytes -= msghdr->msg_ts;
    msqptr->msg_qnum--;
    CsDebug(7, (7, "OlsMsg::Rcv - cbytes=%lu, qnum=%lu",
        msqptr->msg_cbytes, msqptr->msg_qnum));

/* Make msgsz the actual amount that we'll be returning.
    *   Note that this effectively truncates the message if it is too int32_t     *   (since msgsz is never increased).
    */
    CsDebug(7, (7, "OlsMsg::Rcv - found a message, msgsz=%d, msg_ts=%d", 
        msgsz, msghdr->msg_ts));
    if (msgsz > msghdr->msg_ts)
        msgsz = msghdr->msg_ts;

/* Return the type to the user */
    (void)memcpy(msgp, (const char *)&(msghdr->msg_type), 
        sizeof(msghdr->msg_type));
    msgp = (char *)msgp + sizeof(msghdr->msg_type);

/* Return the segments to the user */
    next = (short)msghdr->msg_spot;
    
    for (len = 0; len < msgsz; len += pInst->msg_info.msgssz)
    {
        size_t tlen;

    /* Segment length */
        if (msgsz > pInst->msg_info.msgssz)
            tlen = pInst->msg_info.msgssz;
        else tlen = msgsz;

    /* Verify chain pointers */
        if (next <= -1) {
            CsAssertion("OlsMsg::Rcv - next too low #3");
        } else if (next >= pInst->msg_info.msgseg) {
            CsAssertion("OlsMsg::Rcv - next out of range #3");
        }

    /* Copy block */
        (void)memcpy(msgp, (const char *)
            hMsg->msgpool + (next*pInst->msg_info.msgssz), tlen);
        msgp = (char *)msgp + tlen;
        next = (short)hMsg->msgmaps[next].next;
    }

/* Done, return the actual number of bytes copied out. */
    __OlsMsgFree(hMsg, msghdr);
    if(pSize !=NULL)
    {
        *pSize = msgsz;
    }
    return (0);
}
    
