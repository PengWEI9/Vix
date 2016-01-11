/*
 * Support module for OLS messages
 *
 * message.h hides the OLS function names etc.
 */

#include <ipc.h>
#include <cs.h>
#include <cspath.h>
#include "olsmsg.h"
#include <stdlib.h> /* getenv(), ... */

#ifdef COBRA
#define    SHARED_MEMORY_PATH    "/tmp"
#else
//#error Cobra only
#define    SHARED_MEMORY_PATH    "/tmp"
#endif

static const char *shared_memory_path = SHARED_MEMORY_PATH;

OlsMsgInfo_t     info =
{
	 255,            /* max chars in a message */
	 10,             /* max message queue identifiers */
	 ~0,             /* max chars in a queue */
	 255,            /* max messages in system */
	 128,            /* size of a message segment */
	 256             /* number of message segments */
};

OLSMSGHD *hIPC = NULL;

IpcConfig_t	IpcConfig = {
	1	/* normally block in IpcSend */
};

void IpcGetConfig(IpcConfig_t *pCF) { if (pCF) *pCF = IpcConfig; }
void IpcSetConfig(IpcConfig_t *pCF) { if (pCF) IpcConfig = *pCF; }

int IpcInit(void)
{
	const char *ep;

	if (hIPC != NULL)
	{
		CsDebug(3, (3, "IpcInit: already initialised"));
		return 0;
	}

	if ((ep = getenv(SHELLVAR_AFC_SHM_PATH)) != NULL)
		shared_memory_path = ep;

	if (OlsMsgOpen(&hIPC, shared_memory_path) == 0)
		return 0;

	CsDebug(3, (3, "IpcInit: Tried open - system not created, trying create then open"));
	if (OlsMsgCreate(&hIPC, &info, shared_memory_path) == 0)
		if (OlsMsgOpen(&hIPC, shared_memory_path) == 0)
			return 0;
		else
			CsErr("IpcInit: failed to open message queue");
	else
		CsErr("IpcInit: failed to create message queue");

	return -1;
}

int IpcClose(void)
{

    int rv = OlsMsgClose(hIPC);
    hIPC = NULL;
    return rv;
}
int IpcGetID(int taskID)
{
    int     qid;
    key_t   k = ftok(shared_memory_path, taskID);

    qid = OlsMsgGet(hIPC, k, IPC_CREAT);
    CsDebug(7, (7, "IpcGetID:TaskID %d key %d qid %d", taskID, k, qid));
    return qid;
}
#define IPCTYPE_ALL 0
int IpcReceive(int mqid, void *bufaddr, size_t bufsiz, size_t *addr_bytesread)
{
    return OlsMsgRcv(hIPC, mqid, bufaddr, bufsiz, IPCTYPE_ALL, 0, addr_bytesread, NULL);
}
int IpcJoin(void)
{
     return OlsMsgJoin(&hIPC, &info, shared_memory_path);
}
