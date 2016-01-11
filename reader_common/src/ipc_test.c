
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

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
	set_debug(1);

	if (IpcInit() != 0)
	{
		CsErrx("%s: IpcInit failed", argv[0]);
		return 1;
	}

	if (argc > 1)
	{
		/* CsGetoptl() drags in a lot of code, use strcmp for now */
		if (strcmp(argv[1], "--check-block") == 0)
		{
			int		rc;
			IpcConfig_t	cf;
			IpcConfig_t	cftmp;
			char	*junk;
			size_t	n = hIPC->msgctl->msg_info.msgseg * hIPC->msgctl->msg_info.msgssz;

			set_debug(3);
			CsVerbose("Message system configured with %d blocks if %d bytes", hIPC->msgctl->msg_info.msgseg, hIPC->msgctl->msg_info.msgssz);

			IpcGetConfig(&cf);
			cftmp = cf;
			cftmp.blockInSend = 0;
			IpcSetConfig(&cftmp);

			junk = malloc(n);
			junk[0] = 1;
			IpcSend(IpcGetID(2), junk, n);

			if (IpcWouldBlock(1))
				CsVerbose("IpcWouldBlock reported");

			rc = IpcSend(IpcGetID(99), &rc, 1);

			IpcSetConfig(&cf);
			free(junk);
			return (rc == 0) ? 0 : 1;
		}
		else if (strcmp(argv[1], "--send-10") == 0)
		{
			int		rc;
			char	junk[] = "This is a junk message";

			set_debug(9);
			IpcSend(IpcGetID(2), junk, sizeof junk);
			IpcSend(IpcGetID(2), junk, sizeof junk);
			CsVerbose("IpcReceiveQueueSize = %d (exp 2)", IpcReceiveQueueSize(IpcGetID(2)));

			IpcSend(IpcGetID(4), junk, sizeof junk);
			IpcSend(IpcGetID(5), junk, sizeof junk);

			IpcSend(IpcGetID(6), junk, sizeof junk);
			IpcSend(IpcGetID(6), junk, sizeof junk);

			IpcSend(IpcGetID(7), junk, sizeof junk);
			IpcSend(IpcGetID(8), junk, sizeof junk);

			IpcSend(IpcGetID(111), junk, sizeof junk);
			rc = \
			IpcSend(IpcGetID(112), junk, sizeof junk);
			while (1)
				sleep(1);
			return (rc == 0) ? 0 : 1;
		}
		else if (strcmp(argv[1], "--autopurge") == 0)
		{
			int		rc, i;
			char	junk[] =
				"{\n"
				"    \"SystemGenTime\": \"2013-12-12T01:02:03+11:00\",\n"
				"    \"DeviceGenTime\": \"2013-12-12T01:02:03+11:00\",\n"
				"    \"EventUnitType\": \"PowerState\",\n"
				"    \"EventDesc\": \"Alarm 20\",\n"
				"    \"EventId\": \"20\",\n"
				"    \"EventSeverity\": \"Major\",\n"
				"    \"EventState\": \"Cleared\",\n"
				"    \"EventUid\": \"12345678-1234-1234-123456781234\",\n"
				"    \"SystemGenLocationId\": \"54321\",\n"
				"    \"DeviceGenLocationId\": \"54321\",\n"
				"    \"DeviceId\": \"54321\"";

			set_debug(9);
			for ( i = 0; i < 1000; ++i )
			{
				rc = IpcSend(IpcGetID(i % 10), junk, sizeof junk);
				if ( rc != 0 )
					return 1;
			}
			return 0;
		}
		CsErrx("%s: unexpected option %s", argv[0], argv[1]);
		return 1;
	}

	CsErrx("%s: missing command", argv[0]);

	return 1;
}
