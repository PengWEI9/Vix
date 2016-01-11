/* -*- c: tabs: 4 -*- ******************************************************
* Copyright (C) 1998-99 ERG Limited, All rights reserved
*
* Module name   : msg_open.c
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
  1.3       APY      03/06/99    - Verify version
                                 - Included path in diad messages
*
* $Name:  $
* $Source: /export/repository/cvs/core/olsbase/src/msg/msg_open.c,v $
* $Revision: 1.2 $ $Date: 2001/06/21 12:57:31 $ $State: Exp $
* $ProjectName: /export/dev/olss/ols/base.pj $ $ProjectRevision: 1.1 $
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

#define RETRIES        0


int32_t
OlsMsgOpen(
    OLSMSGHD **phMsg, const char *pPath )
{
    struct olsmsgctl *pInst;
    OLSMSGHD *hMsg;
    int attempt, ret;

    if (phMsg == NULL || pPath == NULL)
        return (EFAULT);

    *phMsg = NULL;
    if ((hMsg = (OLSMSGHD *)CsCalloc(1, sizeof(OLSMSGHD))) == NULL)
        return (ENOMEM);

    for (attempt = 0;; attempt++)
    {
        if ((ret = CsShmOpen(&hMsg->msgshm,
                (void **)&pInst, pPath, "olsmsg", O_RDWR)) == 0)
        {
	    (void) CsShmLock(hMsg->msgshm);              
            (void) __OlsMsgRef(hMsg, pInst);
            if (pInst->msg_ver == VERSION(OLSMSG_VERSION, 0))
	    {
		(void) CsShmUnlock(hMsg->msgshm);              
                break;
	    }
	    (void) CsShmUnlock( hMsg->msgshm );
            (void) CsShmClose( &hMsg->msgshm );
            CsWarn("OlsMsg::Open - version mismatch (%s)", pPath);
            ret = EINVAL;
        }

        if (ret != ENOENT || attempt >= RETRIES)
        {
            CsVerbose("OlsMsg::Open - opening message exchange (%s) : %s", pPath,
                    (attempt >= RETRIES) ? "too many attempts" : strerror(ret));
            CsFree(hMsg);
            return (ret);
        }
        (void) sleep(1 << attempt);
    }
    *phMsg = hMsg;
    return (0);
}
