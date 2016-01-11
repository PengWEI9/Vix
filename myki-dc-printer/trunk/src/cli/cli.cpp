/*=============================================================================
**
**    Vix Technology                   Licensed software
**    (C) 2015                         All rights reserved
**
**=============================================================================
**
**  Project/Product : MBU/NGBU printer
**  Filename        : cli.cpp
**  Author(s)       : ?
**
**  Description     :
**      Implements dcprinter application's command line interface.
**
**  Function(s)     :
**
**  Information     :
**   Compiler(s)    : C++
**   Target(s)      : Independent
**
**  Subversion      :
**      $Id: $
**      $HeadURL: $
**
**
**===========================================================================*/

#include "cli.h"
#include "version.h"

#include <corebasetypes.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>

#include <cs.h>
#include <csfcmd.h>
#include <csf.h>
#include <cslog.h>
#include <csgetopt.h>


namespace Option
{
    static const char ERROROPT    = '?';
    static const char HELP        = 'h';
    static const char BUILD       = 'b';
    static const char COPYRIGHT   = 1;
    static const char VERSION     = 'V';
    static const char NOT         = '!';
    static const char CONSOLE     = 'c';
    static const char VERBOSE     = 'v';
    static const char WARNINGS    = 'w';
    static const char DEBUG_LEVEL = 'd';
    static const char DEBUG_HIRES = 'D';
    static const char LOGGER      = 'l';
    static const char INI_FILE    = 'i';

    static const char* shortOptions = "?hbV!cvwd:D:l:i:";
    static struct option longOptions[] =
    {
        { "help",       no_argument,        NULL,   HELP      },
        { "build",      no_argument,        NULL,   BUILD     },
        { "copyright",  no_argument,        NULL,   COPYRIGHT },
        { "version",    no_argument,        NULL,   VERSION   },
        { "ini",        required_argument,  NULL,   INI_FILE  },
        { NULL }
    };
} // namespace Option

static const CsfUsage_t usage =
{
    CSFCOPYRIGHT,
    "Generic Application",
    BUILDNAME,
    CSFBUILD,
    "usage: %P [-? | -h | --help] [-b | --build] [-V | --version] [--copyright]\n"\
    "       %P [-cvw] [[-d | -D] level] [-l log] {-i inifile}\n"
};

static const CsfHelp_t help[] =
{
    { " -?, -h, --help",        "Display this usage" },
    { " -b, --build",           "Build information" },
    { " -V, -version",          "Version details" },
    { " --copyright",           "Copyright information" },
    { " -[!]c",                 "Enable console output" },
    { " -[!]v",                 "Turn on verbose messages" },
    { " -[!]w",                 "Turn on warning messages" },
    { " -d level",              "Turn on debug messages below the given level" },
    { " -D level",              "Turn on high resolution debug messages below the given level" },
    { " -l log",                "Set logging facility" },
    { " -i inifile",            "Set the initialisation file" },
    { NULL, NULL }
};

namespace
{
	const char descriptionST[] = "Status Command";
	const char* helpST[] =
	{
		"\1"    "Synposis:",
		"\2"        "status",
            		"",
		"\1"    "Description:",
		"\2"        "Query the status of the printer",
		"\2"        "Values x = good, y = bad , etc",
            		"",
		NULL
	};
	int CmdMainST(CmdEnv_t *pEnv, int argc, const char **argv)
	{

        if (argc > 1 && strcmp(argv[1], "<arg1>") == 0)
        {
        }

        CsfCmdPrintf(pEnv, "Called Status Command \n");
		return 0;
	}
}

namespace
{
	const char descriptionInit[] = "Process initialisation";
	const char* helpInit[] =
	{
		"\1"    "Synposis:",
		"\2"        "init [[-w] run-level]",
            		"",
		"\1"    "Description:",
		"\2"        "init is the general spawner. Its primary role is to enable and",
		"\2"        "disable services base upon the selected run-level.",
					"",
		"\2"        "If no parameters are given, then init prints the status of the",
		"\2"        "current run-level.  Otherwise init queues the run-level change",
		"\2"        "to the specified level.",
					"",
		"\1"    "Options:",
		"\2"        "-w        Wait the completion of the run-level change",
					"",
		"\1"    "Run-levels:",
		"\2"        "0         Terminate",
		"\2"        "1         Initialising",
		"\2"        "2         Out of service",
		"\2"        "3         Normal operation",
		"\2"        "4         Validating",
		NULL
	};
	int CmdMainInit(CmdEnv_t *pEnv, int argc, const char **argv)
	{
		CsfCmdOpt_t opt;
		int wFlag = 0;
		int c;
		int runlevel = 3;
		int result = runlevel;

		CsfCmdOptinit(&opt, pEnv->sh_stdout);
		while ( (c = CsfCmdOptget(&opt, argc, argv, "w")) != EOF )
		{
			switch ( c )
			{
			case 'w':
				wFlag++;
				break;
			default:
				CsfCmdPrintf(pEnv, "Unknown flag '%c'\n", opt.optopt);
				break;
			}
		}

		if ( argc > opt.optind )
		{
			int newRunLevel = -1;
			char waste;
			if (
				(sscanf(argv[opt.optind], "%d%c", &newRunLevel, &waste) != 1) ||
				(newRunLevel < 0) ||
				(newRunLevel > 4)
			   )
			{
				CsfCmdPrintf(pEnv, "Unknown run-level '%s'\n", argv[opt.optind]);
			}
			else if ( newRunLevel == 0 )
			{
				raise(SIGTERM);
				CsSleep(0, 10);
				exit(0);
			}
			else
			{
				CsfCmdPrintf(pEnv, "Don't know how to change to run-level %d\n", newRunLevel);
			}
		}
		else
		{
			CsfCmdPrintf(pEnv, "Run-level is %d\n", runlevel );
		}

		return result;
	}
}

namespace
{
	const char descriptionDBG[] = "control debug levels";
	const char* helpDBG[] =
	{
        "\1"    "Synposis:",
        "\2"        "debug [stat | set {level} | load {filename} | rotate]",
                    "",
        "\1"    "Description:",
        "\2"        "debug displays and optionally changes the debugging level.  It",
        "\2"        "is important to note that changes will be lost when the program",
        "\2"        "restarts.",
                    "",
        "\1"    "Commands:",
        "\2"        "stat      Display the current debugging levels.  This is the",
        "\5"                   "default command if no command is supplied.",
        "\2"        "set       Change the current debugging levels.  Possible values",
        "\5"                   "for {level} are:",
        "\5"                   "  E     Only log error messages.",
        "\5"                   "  W     As for 'E', but include warning messages.",
        "\5"                   "  V     As for 'W', but include verbose messages.",
        "\5"                   "  I     Alias for 'V'.",
        "\5"                   "  0..9  As for 'V', but include debug messages at or",
        "\5"                   "        below level.",
        "\2"        "load      Load per-module debug settings from {filename}.",
        "\2"        "rotate    Rotate the log file, if logging to file.",
        NULL
	};
	int CmdMainDBG(CmdEnv_t *pEnv, int argc, const char* argv[])
	{
		enum { DEBUG_STATUS, DEBUG_SET, DEBUG_LOAD, DEBUG_ROTATE } userCommand = DEBUG_STATUS;
		int expectedArgs = 2;
		if ( argc > 1 )
		{
			if ( CsStricmp(argv[1], "stat") == 0 )
				userCommand = DEBUG_STATUS;
			else if ( CsStricmp(argv[1], "rotate") == 0 )
				userCommand = DEBUG_ROTATE;
			else if ( CsStricmp(argv[1], "set") == 0 )
			{
				userCommand = DEBUG_SET;
				expectedArgs = 3;
			}
			else if ( CsStricmp(argv[1], "load") == 0 )
			{
				userCommand = DEBUG_LOAD;
				expectedArgs = 3;
			}
			else
			{
				CsfCmdPrintf(pEnv, "%s: unknown command '%s'\n", argv[0], argv[1]);
				return 1;
			}
		}
		if ( argc != expectedArgs )
		{
			CsfCmdPrintf(pEnv, "%s: Too %s arguments\n", argv[0], (argc < expectedArgs ? "few" : "many"));
			return 1;
		}

		if ( userCommand == DEBUG_SET )	
		{
			int warningMessages = 0;
			int verboseMessages = 0;
			int debugEnabled    = 0;
			int debugLevel      = 0;
			char waste;
			if ( argc != 3 )
			{
				CsfCmdPrintf(pEnv, "%s: set command requires an argument\n", argv[0]);
				return 1;
			}
			else if ( CsStricmp(argv[2], "E") == 0 )
			{
				// no-op
			}
			else if ( CsStricmp(argv[2], "W") == 0 )
			{
				warningMessages = 1;
			}
			else if ( (CsStricmp(argv[2], "V") == 0) || (CsStricmp(argv[2], "I") == 0) )
			{
				warningMessages = 1;
				verboseMessages = 1;
			}
			else if ( sscanf(argv[2], "%d%c", &debugLevel, &waste) == 1 )
			{
				warningMessages = 1;
				verboseMessages = 1;
				debugEnabled    = 1;
				if ( (debugLevel < 0) || (debugLevel > 9) )
				{
					CsfCmdPrintf(pEnv, "%s: %d is not a valid debug level\n", argv[0], debugLevel);
					return 1;
				}
			}
			else
			{
				CsfCmdPrintf(pEnv, "%s: Invalid argument to set - '%s'\n", argv[0], argv[2]);
				return 1;
			}

			// Turn on verbose messages so we can log what's happening
			int flag = 1;
			CsMsgOptset(CSMSGOP_VERBOSE, &flag, (uint32_t)sizeof(flag));
			CsVerbose("User set debug level to '%s'", argv[2]);
			CsMsgOptset(CSMSGOP_WARNING, &warningMessages, (uint32_t)sizeof(warningMessages));
			CsMsgOptset(CSMSGOP_VERBOSE, &verboseMessages, (uint32_t)sizeof(verboseMessages));
			CsMsgOptset(CSMSGOP_DEBUG, &debugLevel, (uint32_t)sizeof(debugLevel));
			CsMsgOptset(CSMSGOP_DEBUGENABLED, &debugEnabled, (uint32_t)sizeof(debugEnabled));
		}
		else if ( userCommand == DEBUG_LOAD )
		{
			if ( argc != 3 )
			{
				CsfCmdPrintf(pEnv, "%s: load command requires a filename\n", argv[0]);
				return 1;
			}
			else if ( access(argv[2], R_OK) != 0 )
			{
				CsfCmdPrintf(pEnv, "%s: File '%s' is not readable\n", argv[0], argv[2]);
				return 1;
			}
			else
			{
				(void)CsDbgLoad(argv[2]); /* CsDbgLoad always returns 0, even if it fails */
			}
		}
		else if ( userCommand == DEBUG_ROTATE )
		{
			char buf[128];
			if ( CsfIniExpand("Debug:LocalLogFile", buf, sizeof(buf)) == 0 )
			{
				CsLogRotate();
				CsfCmdPrintf(pEnv, "Rotated log\n\n");
			}
			else
			{
				CsfCmdPrintf(pEnv, "Could not rotate log file - not logging to file\n\n");
			}
		}

		int flag = 0;
		int length = 0;
		CsfCmdPrintf(pEnv, "Current debugging status:\n");
		CsfCmdPrintf(pEnv, "Error messages  : enabled\n");
		CsMsgOptget(CSMSGOP_WARNING, &flag, &length);
		CsfCmdPrintf(pEnv, "Warning messages: %sabled\n", flag == 0 ? "dis" : "en");
		CsMsgOptget(CSMSGOP_VERBOSE, &flag, &length);
		CsfCmdPrintf(pEnv, "Verbose messages: %sabled\n", flag == 0 ? "dis" : "en");
		CsMsgOptget(CSMSGOP_DEBUGENABLED, &flag, &length);
		CsfCmdPrintf(pEnv, "Debug messages  : %sabled", flag == 0 ? "dis" : "en");
		if ( flag != 0 )
		{
			CsMsgOptget(CSMSGOP_DEBUG, &flag, &length);
			CsfCmdPrintf(pEnv, ", level %d", flag);
		}
		CsfCmdPrintf(pEnv, "\n");
		return 0;
	}
}
CmdToken_t commands[] =
{
	/*	Name,		Desc,				Handler,		Help */
	{	"status",	descriptionST,		CmdMainST,		helpST }, 
	{	"init",		descriptionInit,	CmdMainInit,	helpInit },
	{	"debug",	descriptionDBG,		CmdMainDBG,		helpDBG }, 
	{	NULL	,	NULL,		NULL,		NULL }, 
};

int RegisterCommands(CmdToken_t *pCmds)
{
	if (pCmds == NULL)
	{
		 CsDebug(1, (1, "RegisterCommands: NULL\n"));
		return -1;
	}
	for ( ; pCmds->c_name != NULL; pCmds++)
	{
		CsDebug(3, (3, "RegisterCommands: command '%s'\n", pCmds->c_name));
		if (CsfCmdRegister(pCmds) != 0)
		{
			CsDebug(1, (1, "RegisterCommands: failed for command '%s'\n", pCmds->c_name));
			return -1;
		}
	}
	return 0;
}

int HandleCommandLine(int argc, char** argv)
{
	bool displayHelp      = false;
	bool displayBuild     = false;
	bool displayCopyright = false;
	bool displayVersion   = false;
	bool argError         = false;
	char* iniFile         = NULL;
	std::vector<int> commandLineDebugSettings;

	int flag = 1;
	int c;
	CsGetoptReset();
	while ( (c = CsGetoptl(argc, argv, Option::shortOptions, Option::longOptions, 1)) != EOF )
	{
		switch ( c )
		{
		case Option::ERROROPT:
			if ( optopt == Option::ERROROPT )
				displayHelp = true;
			else
				argError = true;
			break;
		case Option::HELP: displayHelp = true; break;
		case Option::BUILD: displayBuild = true; break;
		case Option::COPYRIGHT: displayCopyright = true; break;
		case Option::VERSION: displayVersion = true; break;
		case Option::NOT: flag = 0; break;
		case Option::CONSOLE:
			CsMsgOptset(CSMSGOP_CONSOLE, &flag, (uint32_t)sizeof(int32_t));
			flag = 1;
			commandLineDebugSettings.push_back(CSMSGOP_CONSOLE);
			break;
		case Option::VERBOSE:
			CsMsgOptset(CSMSGOP_VERBOSE, &flag, (uint32_t)sizeof(int32_t));
			flag = 1;
			commandLineDebugSettings.push_back(CSMSGOP_VERBOSE);
			break;
		case Option::WARNINGS:
			CsMsgOptset(CSMSGOP_WARNING, &flag, (uint32_t)sizeof(int32_t));
			flag = 1;
			commandLineDebugSettings.push_back(CSMSGOP_WARNING);
			break;
		case Option::DEBUG_HIRES:
			CsMsgOptset(CSMSGOP_HIGHRES, &flag, (uint32_t)sizeof(int32_t));
            commandLineDebugSettings.push_back(CSMSGOP_HIGHRES);
            /*FALLTHROUGH*/
		case Option::DEBUG_LEVEL:
			flag = atoi(optarg);
			if ( (flag <= 0) || (flag > 9) )
			{
				fprintf(stderr, "illegal debug level -- %s\n", optarg);
				argError = true;
			}
			CsMsgOptset(CSMSGOP_DEBUG, &flag, (uint32_t)sizeof(int32_t));
			commandLineDebugSettings.push_back(CSMSGOP_DEBUG);
            commandLineDebugSettings.push_back(CSMSGOP_DEBUGENABLED);
			flag = 1;
			break;
		case Option::LOGGER:
			CsMsgOptset(CSMSGOP_LOGGER, &optarg, (uint32_t)sizeof(char*));
			commandLineDebugSettings.push_back(CSMSGOP_LOGGER);
			break;
		case Option::INI_FILE:
			if ( iniFile != 0 )
				free(iniFile);
			iniFile = strdup(optarg);
			break;
		default:
			argError = true;
		} // switch

		if ( (c != Option::NOT) && (flag != 1) )
		{
			argError = true;
			fprintf(stderr, "! applied to incorrect argument '%c'\n", c);
		}
	} // while getting options

	if ( argc != optind )
	{
		argError = true;

		if ( argc > optind )
		{
			char* name = strrchr(argv[0], CsGetcds());
			fprintf(stderr, "%s: unexpected argument -- %s\n", (name == 0 ? argv[0] : name+1), argv[optind]);
		}
	}

	// If all the args are ok, do some logical testing of the arguments.
	if ( !argError && !displayCopyright && !displayBuild && !displayVersion && !displayHelp )
	{
		// check arguments here
		if ( (iniFile == 0) || (iniFile[0] == '\0') )
		{
			fprintf(stderr, "Must supply a .ini file\n");
			argError = true;
		}
	}

	// See if we should display a usage message
	if ( argError )
		CsfUsage(0, argv[0], &usage, help);
	if ( displayBuild )
		fprintf(stderr, "%s\n", VersionString());
	if ( displayVersion && !displayCopyright )
		fprintf(stderr, "%s %s\n", usage.ou_version, usage.ou_build);
	if ( displayCopyright )
		CsfUsage(2, argv[0], &usage, help);
	if ( displayHelp )
		CsfUsage(1, argv[0], &usage, help);
	if ( displayBuild || displayVersion )
		exit(3);

	// Open the ini file
	CsfIniOpen(iniFile, "");
	if ( commandLineDebugSettings.empty() )
	{
		CsMsgOptsetFromIni(0, 0);
	}
	else
	{
		int* temp = new int[commandLineDebugSettings.size()];
		std::copy(commandLineDebugSettings.begin(), commandLineDebugSettings.end(), temp);
		CsMsgOptsetFromIni(temp, commandLineDebugSettings.size());
		delete[] temp;
	}

	// Set up the command server stuff
	if (RegisterCommands(commands) != 0 )
	{
		CsDebug(1, (1, "Register commands failed"));
	}
	else if ( CsfCmdServer("", "Command", NULL, 0, "DC printer Command Server\n", "dcPrinter cmd>" ) != 0 )
    {
		CsErrx("CsfCmdServer failed to start the command server");
    }

	free(iniFile);
    return 0;
}





