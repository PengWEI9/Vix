/* -*- c: tabs: 4 -*- ******************************************************
* Copyright (C) 1998-2001 ERG Limited, All rights reserved
*
* Module name   : msg_join.c
* Module type   : OLS messaging
* Compiler(s)   : ANSI C
* Environment(s): Solaris
*
* Description:
*
    Message join (create or open)
*
* Version   Who      Date        Description
  1.0       APY      03/06/99    Created
*
* $Name:  $
* $Source: /export/repository/cvs/core/olsbase/src/msg/msg_join.c,v $
* $Revision: 1.2 $ $Date: 2001/06/21 12:57:30 $ $State: Exp $
* $Author: ayoung $ $Locker:  $
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

#define ATTEMPTS    2

static int32_t Same(struct olsmsgctl *pInst, OlsMsgInfo_t *pInfo);

 int32_t
OlsMsgJoin(           
    OLSMSGHD **phMsg, OlsMsgInfo_t *pInfo, const char *pPath)
{
    struct olsmsgctl *pInst;
    OLSMSGHD *hMsg = NULL;
    int32_t attempt, ret;
    uint32_t size;

    if (phMsg == NULL || pInfo == NULL || pPath == NULL)
         ret = EINVAL;

    else if ((ret = __OlsMsgSize(pInfo, &size)) == 0)
    {
        if ((hMsg = (OLSMSGHD *)CsCalloc(1, sizeof(OLSMSGHD))) == NULL)
            ret = ENOMEM;

        else for (attempt=0; ret == 0 && attempt < ATTEMPTS; attempt++)
        {
            if (attempt)
                (void) sleep(1 << attempt);
                
            if ((ret = CsShmCreate( &hMsg->msgshm, (void **)&pInst,
			(u_int)size, pPath, "olsmsg", O_RDWR | O_EXCL)) == 0)
            {
                __OlsMsgInit(pInst, pInfo, size);
                (void) __OlsMsgRef(hMsg, pInst);
		(void) CsShmUnlock(hMsg->msgshm);
                break;
            }

            if (ret == EEXIST)
            {
                if ((ret = CsShmOpen(&hMsg->msgshm,
                        (void **)&pInst, pPath, "olsmsg", O_RDWR)) == 0)
                {
                /* Attached, verify environment */
		    (void) CsShmLock(hMsg->msgshm);
                    (void) __OlsMsgRef(hMsg, pInst);
                    if (Same(pInst, pInfo))
		    {
			(void) CsShmUnlock(hMsg->msgshm);
                        break; 
		    }
 
                /* Close the shared region and if we're the only
                    *   attached process the result in that the
                    *   regions shall be destroyed.  On the 2nd
                    *   attempt return EINVAL.
                    */
		    (void) CsShmUnlock( hMsg->msgshm );
                    (void) CsShmClose( &hMsg->msgshm );
                    if (attempt)
                    {
                        CsWarn("OlsMsg::Join - configuration mismatch");
                        ret = EINVAL;
                    }
                }
                else if (ret == ENOENT)
                {
                    ret = 0;
                }
            }
        }
    }

    if (ret == 0)
    {
        *phMsg = hMsg;
    }
    else
    {
        if (phMsg)
            *phMsg = NULL;
        (void) CsFree(hMsg);
        CsErrx("OlsMsg::Join - opening message exchange (%s) : %s",
            (pPath ? pPath : "(nul)"), strerror(ret));
    }
    return (ret);
}

     
static int32_t
Same(struct olsmsgctl *pInst, OlsMsgInfo_t *pInfo)
{
    if (pInst->msg_ver == VERSION(OLSMSG_VERSION, 0))
    {
        if (pInfo->msgmax == pInst->msg_info.msgmax &&      /* max chars in a message */
             pInfo->msgmni == pInst->msg_info.msgmni &&     /* max message queue identifiers */
              pInfo->msgmnb == pInst->msg_info.msgmnb &&    /* max chars in a queue */
               pInfo->msgtql == pInst->msg_info.msgtql &&   /* max messages in system */
                pInfo->msgssz == pInst->msg_info.msgssz &&  /* size of a message segment */
                  pInfo->msgseg == pInst->msg_info.msgseg)  /* number of message segments */
        {
            return (1);
        }
    }
    return (0);
}
 


