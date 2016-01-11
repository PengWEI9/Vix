
#include <string.h>
#include <unistd.h>

#include <cs.h>
#include "olsmsg.h"
#include "ipc.h"
#include "ipc_priv.h"
#include "extern.h"


void
set_debug(int level)
{
	CsMsgOptset(CSMSGOP_DEBUG, &level, (uint32_t)sizeof(int32_t));
}

int
main(int argc, char *argv[])
{
	unsigned	count;
	char* progname = 0;
	int32_t	flag = 1;

	if ((progname = strrchr(argv[0], '/')) == NULL && (progname = strrchr(argv[0], '\\')) == NULL)
		progname = argv[0];
	else
		progname++;

	CsMsgOptset(CSMSGOP_PROGNAME, progname, (int)strlen(progname));
	CsMsgOptset(CSMSGOP_CONSOLE, &flag, (uint32_t)sizeof(int32_t));
	CsMsgOptset(CSMSGOP_VERBOSE, &flag, (uint32_t)sizeof(int32_t));
	CsMsgOptset(CSMSGOP_WARNING, &flag, (uint32_t)sizeof(int32_t));
	set_debug(0);

	IpcInit();

	if (argc > 1)
	{
		/* CsGetoptl() drags in a lot of code, use strcmp for now */
		if (strcmp(argv[1], "--destroy") == 0)
		{
			int32_t	rc;

			if (hIPC == NULL)
			{
				return 0;
			}
			rc = CsShmDestroy(&hIPC->msgshm);
			hIPC = NULL;

			return (rc == 0) ? 0 : 1;
		}
		if (strcmp(argv[1], "--unused") == 0)
		{
			if (CsShmUseCount(&hIPC->msgshm, &count) == 0 && count == 1)
			{
				CsVerbose("%s: removing unused SHM", argv[0]);
				CsShmDestroy(&hIPC->msgshm);
			}
			return 0;
		}
		if (strcmp(argv[1], "--flush") == 0)	/* clear all queues */
		{
			struct olsmsqid *msqptr;
			int				msqid;

			set_debug(1);

			for (msqid = 0; msqid < hIPC->msgctl->msg_info.msgmni; msqid++) 
			{
				msqptr = &hIPC->msqids[msqid];
				if (msqptr->msg_qnum > 0)
				{
					CsDebug(1, (1, "%s: flushing from mqid %d", argv[0], msqid));
					IpcFlush(msqid, NULL);
				}
			}
			return 0;
		}
		CsErrx("%s: unexpected option %s", argv[0], argv[1]);
		return 1;
	}

	if (CsShmUseCount(&hIPC->msgshm, &count) == 0)
		CsVerbose("%s: number of uses is %u (incl. this)", argv[0],  count);
	else
		CsErrx("%s: failed to determine use count", argv[0]);

	return 0;
}
