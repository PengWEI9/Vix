/*
 * Support module for OLS messages
 *
 * message.h hides the OLS function names etc.
 */

#include <ipc.h>
#include <cs.h>
#include <cspath.h>
#include "olsmsg.h"
#include "extern.h"
#include "olsmsg_priv.h"   // unprotected send and receive
#include "ipc_priv.h"
#include <stdlib.h>
#include <algorithm>
#include <cstring>

using namespace std;

#define IPCTYPE_ALL 0
int IpcSend(int mqid, const void *addr, size_t len)
{
	int     ret;

    if (hIPC == NULL || hIPC->msgmagic != OLSMSG_MAGIC) 
    {
        CsDebug(3, (3, "IpcSend - invalid magic"));
        return(EINVAL);
    }                      
    if ((ret = CsMutexLock(&hIPC->msgctl->msg_mutex)) != 0)
    {
        CsDebug(5, (5, "IpcSend - failed to get mutex"));
		return (ret);
    }

	while (IpcSendWouldBlock(len))
	{
		if (!IpcConfig.blockInSend)
		{
			CsWarnx("IpcSend: queue %d, length %d: would block, not enough free blocks", mqid, len);
			CsMutexUnlock(&hIPC->msgctl->msg_mutex);
			return ENOMEM;
		}
		else
		{
			int maxQueueId = 0;
			unsigned int maxQueueCount = hIPC->msqids[maxQueueId].msg_qnum;
			for ( int i = 1; i < hIPC->msgctl->msg_info.msgmni; ++i )
			{
				if ( maxQueueCount < hIPC->msqids[i].msg_qnum )
				{
					maxQueueCount = hIPC->msqids[i].msg_qnum;
					maxQueueId = i;
				}
			}

			CsErrx("IpcSend: queue %d, length %d: not enough space, discarding message from queue %d, queue count %u", mqid, len, maxQueueId, maxQueueCount);
			size_t bufSize = 256;
			void* buf = malloc(bufSize);
			if ( buf == NULL )
			{
				CsErrx("IpcSend: malloc size %d failed for discard message", bufSize);
				CsMutexUnlock(&hIPC->msgctl->msg_mutex);
				return ENOMEM;
			}
			else
			{
				size_t receiveSize = 0;
				while ( E2BIG == _OlsMsgRcv(hIPC, maxQueueId, buf, bufSize, IPCTYPE_ALL, 0, &receiveSize, (const CsTime_t *)-1) )
				{
					bufSize *= 2;
					buf = realloc(buf, bufSize);
					if ( buf == NULL )
					{
						CsErrx("IpcSend: realloc size %d failed for discard message", bufSize);
						CsMutexUnlock(&hIPC->msgctl->msg_mutex);
						return ENOMEM;
					}
				}
				if ( receiveSize > 0 )
				{
					char* hex = new char[receiveSize*2+1];
					for ( unsigned int i = 0; i < receiveSize; ++i )
						sprintf(hex + i*2, "%02x", int(((unsigned char*)buf)[i]));
					CsErrx("IpcSend: message discarded from queue %d is %s", maxQueueId, hex);
					delete[] hex;
				}
			}
			free(buf);
		}
	}

	CsDebug(9, (9, "IpcSend: %d bytes to mqid %d", len, mqid));
	ret = _OlsMsgSnd(hIPC, mqid, (void *)addr, len, NULL);
        CsCondBcast(&hIPC->msgctl->msg_sndcond);	/* wake waiting IpcReceive */
	CsMutexUnlock(&hIPC->msgctl->msg_mutex);
	return ret;
}

int IpcSendMulti(int messageQueueId, const IPC_header_t *header, 
        const char * data, size_t len)
{
    if (header == NULL)
    {
        CsErrx("IPCSendMulti: header is NULL");
        return -1;
    }

    // TODO replace with actual max
    const size_t maxLen = IPC_MULTI_MAX_SIZE - sizeof(IPC_PayloadMulti_t);

    int ret = 0;

    // Total number of messsages to send
    const size_t numBlocks = len / maxLen + (len % maxLen != 0);

    // divide data into substrings and send each as an individual message
    for (size_t i = 0; i < numBlocks && ret == 0; ++i)
    {
        const size_t start = i * maxLen;
        const size_t end = min(start + maxLen, len);
        const size_t dataSize = end - start;
        const size_t msgSize = dataSize + sizeof(IPC_PayloadMulti_t);

        // allocate block of data for msg
        char * buf = new char[msgSize]();
        IPC_PayloadMulti_t * msg = reinterpret_cast<IPC_PayloadMulti_t*>(buf);

        msg->hdr = *header;
        msg->blockSequenceNumber = i + 1;
        msg->maxSize = len;
        msg->numberOfBlocks = numBlocks;
        msg->blockSize = dataSize;

        // note that data[1] gives us an extra byte for null terminator
        strncpy(msg->data, data + start, dataSize);

        CsDbg(5, "IPCSendMulti: blockSequenceNumber: %d, maxSize: %d, "
                "numberOfBlocks: %d, blockSize: %d, data: %s",
                msg->blockSequenceNumber,
                msg->maxSize,
                msg->numberOfBlocks,
                msg->blockSize,
                msg->data);

        // Send IPC message
        ret = IpcSend(messageQueueId, reinterpret_cast<void*>(msg), msgSize);
        delete[] buf;
    }

    return ret;
}
