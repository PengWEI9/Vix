#ifndef _IPC_H
#define _IPC_H

#include <cs.h>
#include <message_base.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Maximum size of each indivial IPC messsage sent via IpcSendMulti.
 * Note that this value was arbitrarily chosen (though leaning towards 
 * conservative), as there doesn't seem to be any maximum size for IPC messages
 * defined anywhere else.
 */
#define IPC_MULTI_MAX_SIZE 1000

int IpcInit(void);
int IpcJoin(void);
int IpcClose(void);
int IpcGetID(int taskID);
int IpcSend(int mqid, const void *addr, size_t len);
int IpcReceive(int mqid, void *bufaddr, size_t bufsiz, size_t *addr_bytesread);

/**
 * @brief Sends a string IPC message, broken up into multiple messages if the
 * string is too large.
 *
 * @param messageQueueId The queue to add our message to. This can specify 
 * different applications, eg a message queue for a printer application.
 * @param header The header to apply to each message sent.
 * @param data The data to send.
 * @param len Length of data.
 * @return 0 on success, otherwise failure.
 */
int IpcSendMulti(int messageQueueId, const IPC_header_t *header, const char * data, size_t len);

/*
    The following sample shows IPC setup and  raw messages being sent or received.
    The simple raw buffer usage should be replaced with the structures
    described at the bottom of this file.

    int     my_qid;
    int     other_qid;
    IPC_NoPayload_t m; 

    if ((rc = IpcInit()) != 0) CsErrx(...);

    if (my_qid = IpcGetID(APP_TASK) == -1) CsErrx(...);
    if (other_qid = IpcGetID(OTHER_TASK) != 0) CsErrx(...);

    if (IpcSend(other_qid, &m, sizeof m) != 0) CsErr(...)

    if (IpcReceive(myqid, buf, sizeof(buf), &byte_received) !== 0) CsErr(...)
*/

typedef int (*IpcFlushFilter_f)(const void *);
/*
    example filter:

    int myFilter(const void *m) { return (memcmp(m, "FROG", 4) == 0); }
*/

/*
 * utility
 */

int IpcUseCount(void);			/* how many IPC systemusers including 'self' (error < 1) */
int IpcReceiveQueueSize(int mqid);	/* (boolean) */
int IpcSendWouldBlock(size_t len);	/* (boolean) not enough free blocks */
#define IpcWouldBlock IpcSendWouldBlock	/* initial version did not say what would block */

#ifdef __cplusplus
int IpcFlush(int mqid, IpcFlushFilter_f filter = NULL);
#else
int IpcFlush(int mqid, IpcFlushFilter_f filter);
#endif

/*
 * special configuration
 */
typedef struct {
	int	blockInSend;
} IpcConfig_t;

void IpcGetConfig(IpcConfig_t *);
void IpcSetConfig(IpcConfig_t *);

#ifdef __cplusplus
}
#endif

#endif //_IPC_H
