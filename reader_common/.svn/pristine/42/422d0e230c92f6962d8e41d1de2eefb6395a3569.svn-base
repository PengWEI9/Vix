/* -*- c: tabs: 4 -*- ******************************************************
* Copyright (C) 1998-2001 ERG Limited, All rights reserved
*
* Module name   : msg_close.c
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
* $Source: /export/repository/cvs/core/olsbase/src/msg/msg_clos.c,v $
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


int32_t
OlsMsgClose(
    OLSMSGHD *hMsg)
{
    int ret;

    if (hMsg == NULL)
        return (EINVAL);

    if ((ret = CsShmClose( &hMsg->msgshm )) != 0)
    {
        CsErrx("OlsMsg::Close - closing shared region : %s", strerror(ret));
        return (ret);
    }

    hMsg->msgmagic = ~OLSMSG_MAGIC;
    hMsg->msgctl = NULL;

    return (0);
}
