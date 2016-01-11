/*
 * Support module for OLS messages
 *
 * message.h hides the OLS function names etc.
 */

#include <ipc.h>
#include <cs.h>
#include <csf.h>
#include <stdlib.h>
#include <errno.h>
#include "extern.h"
#include "olsmsg.h"
#include "olsmsg_priv.h"   // unprotected send and receive
#include "ipc_priv.h"

#define IPCTYPE_ALL 0
int IpcFlush(int mqid, IpcFlushFilter_f IpcFlushFilter)
{
    void    *buf;
    size_t  buf_size = 256;
    size_t  n;
    int     ret;
    struct olsmsqid *msqptr;
    uint32_t    queueLength, queueCounter;

    if (hIPC == NULL)
    {
        CsErr("IpcFlush: message system not initilaised");
        errno = EINVAL;
        return -1;
    }

    if ((buf = malloc(buf_size)) == NULL)
    {
        CsErr("IpcFlush: could not allocate memory");
        errno = ENOMEM;
        return -1;
    }
    if ((ret = CsMutexLock(&hIPC->msgctl->msg_mutex)) != 0)
    {            
        CsDebug(5, (5, "IpcFlush - failed to get mutex"));
        free(buf);
        return ret;
    }
    if (mqid >= hIPC->msgctl->msg_info.msgmni)
    {            
        CsDebug(5, (5, "IpcFlush - mqid %d is invalid", mqid));
        CsMutexUnlock(&hIPC->msgctl->msg_mutex);
        free(buf);
        errno = E2BIG;
        return -1;
    }
    msqptr = &hIPC->msqids[mqid];
    queueLength = msqptr->msg_qnum;
    CsDebug(5, (5, "IpcFlush: Started with %d messages", queueLength));

    queueCounter = 0;
    while (queueCounter < queueLength)
    {
        ret = _OlsMsgRcv(hIPC, mqid, buf, buf_size, IPCTYPE_ALL, 0, &n, (const CsTime_t *)-1);
        if (ret == E2BIG)
        {
            void    *buf2;
            buf_size *= 2;
            if ((buf2 = realloc(buf, buf_size)) == NULL)
            {
                CsErr("IpcFlush: could not allocate memory");
                errno = ENOMEM;
                ret = -1;
                break;
            }

            buf = buf2;
            continue;   // try that one again
        }
        else
        if (ret != 0)
        {
            break;
        }
        else
        {
            if (IpcFlushFilter && IpcFlushFilter(buf) == 0)
                _OlsMsgSnd(hIPC, mqid, buf, n, NULL);  // put back on the end of the queue
        }

        ++queueCounter;
    }

    CsDebug(1, (1, "IpcFlush: discarded %d messages", queueLength - msqptr->msg_qnum));
    if (queueLength > msqptr->msg_qnum)                         
        CsCondBcast(&hIPC->msgctl->msg_rcvcond);    // freed space, those waiting can proceed 
    CsMutexUnlock(&hIPC->msgctl->msg_mutex);
    free(buf);
    return (errno == ENOMSG) ? 0 : ret;
}
