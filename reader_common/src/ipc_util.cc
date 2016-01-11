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

int IpcUseCount(void)
{
    int     ret;
	unsigned	count;

    if (hIPC == NULL)
    {
        CsVerbose("IpcUseCount: message system not initilaised");
        errno = EINVAL;
        return -1;
    }

    if ((ret = CsMutexLock(&hIPC->msgctl->msg_mutex)) != 0)
    {            
        CsDebug(5, (5, "IpcUseCount - failed to get mutex"));
        return -1;
    }

	if (CsShmUseCount(&hIPC->msgshm, &count) != 0)
	{
		CsDebug(5, (5, "IpcUseCount: got count %u from shm", count));
		ret = count;
	}
	else
	{
		CsDebug(5, (5, "IpcUseCount: failed to get count from shm"));
		ret = -1;
	}

	CsMutexUnlock(&hIPC->msgctl->msg_mutex);
	return ret;
}

// although the sense is bool, errors appear as true (safe)
int IpcSendWouldBlock(size_t len)
{
    if (hIPC == NULL)
    {
        CsVerbose("IpcWouldBlock: message system not initilaised");
        errno = EINVAL;
        return -1;
    }

	if (hIPC->msgctl->free_msghdrs == -1)
    {
        CsVerbose("IpcWouldBlock: message system full");
        return 1;
    }

    int	bs = hIPC->msgctl->msg_info.msgssz;
    size_t	blocks = len / bs;
    if (len % bs)
	    blocks++;
    return blocks > hIPC->msgctl->nfree_msgmaps;
}

int IpcReceiveQueueSize(int mqid)
{
    register struct olsmsgctl *pInst;

    if (hIPC == NULL)
    {
        CsVerbose("IpcReceiveQueueSize: message system not initilaised");
        errno = EINVAL;
        return -1;
    }
    pInst = hIPC->msgctl;

    if (mqid < 0 || mqid >= pInst->msg_info.msgmni) {
        CsDebug(3, (3, "IpcReceiveQueueSize - msqid (%d) out of range (0<=msqid<%d)", 
            mqid, pInst->msg_info.msgmni));
        errno = EINVAL;
        return -1;
    }

    return hIPC->msqids[mqid].msg_qnum;
}
