/***************************************************************************
 * Copyright (c) 2014 Vix Technology. All rights reserved
 *
 * Module name   : main.cpp
 * Module type   : 
 * Compiler(s)   : ANSI C
 * Environment(s): LINUX
 *
 * Description:
 *
 *       
 *
 * Contents:
 *
     
 *

 * Version   Who      Date       Description
   1.0       EAH      14/07/2014 Created
 *
 * $Id:  $
 * $HeadURL: $
 *.........................................................................*/

#include <stdlib.h>  

#include "utils.h"
#include "dc.h"
#include <csfproc.h>
#include "cslog.h"

#include <string>
#include <iostream>
#include <stdio.h>

int appQueueId = -1;  
int wsQueueId  = -1;
int asQueueId  = -1;
int avlTaskId  = -1;
int printerTaskId = -1;

int g_pollDelayMS = 5000;
int g_httpPort    = 8080;

int g_InvalidPODTimeout = 5;

/* Core/stack configuration */
#define INI_CORECONFIG "Debug:Core"
#define INI_STACKCONFIG "Debug:Trace"
#define INIDEF_CORECONFIG "/afc/cores/dc.core"
#define INIDEF_STACKCONFIG "/afc/cores/dc.trace"

std::string g_resourcePath;
int         g_screensaverTimeout = 120;
#define     DEFAULT_SERIAL_LINK_SPEED	115200
int         g_serialLinkSpeed = DEFAULT_SERIAL_LINK_SPEED;

char                     g_terminalId[CSFINI_MAXVALUE+1]           = {'\0'};
char                     g_terminalType[CSFINI_MAXVALUE+1]         = {'\0'};
char                     g_serialNo[CSFINI_MAXVALUE+1]             = {'\0'};
char                     g_ipAddress[CSFINI_MAXVALUE+1]            = {'\0'};
char                     g_netmask[CSFINI_MAXVALUE+1]              = {'\0'};
char                     g_gateway[CSFINI_MAXVALUE+1]              = {'\0'};
char                     g_mac[CSFINI_MAXVALUE+1]                  = {'\0'};
char                     g_priDNS[CSFINI_MAXVALUE+1]               = {'\0'};
char                     g_WLANipAddress[CSFINI_MAXVALUE+1]        = {'\0'};
char                     g_WLANnetmask[CSFINI_MAXVALUE+1]          = {'\0'};
char                     g_WLANgateway[CSFINI_MAXVALUE+1]          = {'\0'};
char                     g_WLANmac[CSFINI_MAXVALUE+1]              = {'\0'};
char                     g_WLANpriDNS[CSFINI_MAXVALUE+1]           = {'\0'};
char                     g_GPRSipAddress[CSFINI_MAXVALUE+1]        = {'\0'};
char                     g_GPRSnetmask[CSFINI_MAXVALUE+1]          = {'\0'};
char                     g_GPRSgateway[CSFINI_MAXVALUE+1]          = {'\0'};
char                     g_GPRSmac[CSFINI_MAXVALUE+1]              = {'\0'};
char                     g_GPRSpriDNS[CSFINI_MAXVALUE+1]           = {'\0'};
char                     g_serviceProvidersFile[CSFINI_MAXVALUE+1] = {'\0'};
char                     g_rolesFile[CSFINI_MAXVALUE+1]            = {'\0'};
char                     g_scheduleDatabase[CSFINI_MAXVALUE+1]     = {'\0'};
char                     g_accessPointName[CSFINI_MAXVALUE+1]      = {'\0'};
char                     g_stationId[CSFINI_MAXVALUE+1]            = {'\0'};
char                     g_vehicleId[CSFINI_MAXVALUE+1]            = {'\0'};

bool                     g_autoPrintReceipt                        = false;

bool s_running = true;

IPC_3GState_e            g_3GState                                 = IPC_3G_UNINITIALISED ; 
IPC_3GSignalQuality_e    g_3GSignalQuality                         = IPC_3G_SIGNAL_QUALITY_UNKNOWN;
int                      g_3GSignalQualityValue                    = 0;

bool                    railTariff                                  = false;    /**< Referenced by MYKI_CD (TOBEREMOVED) */

DCType  s_DCType   = DCTYPE_TRAM;
DCState s_DCState = DCSTATE_UNINITIALISED;


Json::Value s_deviceList;
Json::Value s_network;
Json::Value s_trip;
Json::Value s_tablecard;
Json::Value g_roles;
Json::Value g_serviceProviders;


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
    { " -b, --build",            "Build information" },
    { " -V, -version",            "Version details" },
    { " --copyright",            "Copyright information" },
    { " -[!]c",                    "Enable console output" },
    { " -[!]v",                    "Turn on verbose messages" },
    { " -[!]w",                    "Turn on warning messages" },
    { " -d level",                "Turn on debug messages below the given level" },
    { " -D level",                "Turn on high resolution debug messages below the given level" },
    { " -l log",                "Set logging facility" },
    { " -i inifile",            "Set the initialisation file" },
    { NULL, NULL }
};

namespace
{
    const char commandST[] = "status";
    const char descriptionST[] = "Status Command";
    const char* helpST[] =
    {
        "\1"    "Synopsis:",
        "\2"        "status",
                    "",
        "\1"    "Description:",
        "\2"        "Report the status",
                    "",
        NULL
    };
    int CmdMainST(CmdEnv_t *pEnv, int argc, const char **argv)
    {
        CsfCmdPrintf(pEnv, "type=%s\n",             getCurrentTypeString());
        CsfCmdPrintf(pEnv, "state=%s\n",            getCurrentStateString());
        CsfCmdPrintf(pEnv, "gps satellites=%d\n",   s_gpsSatellites);
        CsfCmdPrintf(pEnv, "longtitude=%d\n",       s_longitude); 
        CsfCmdPrintf(pEnv, "latitude=%d\n",         s_latitude);        
        CsfCmdPrintf(pEnv, "speed=%d\n",            s_speed); 
        CsfCmdPrintf(pEnv, "eps=%d\n",              s_eps); 
        CsfCmdPrintf(pEnv, "AVL route id=%d\n",     s_routeId);
        CsfCmdPrintf(pEnv, "AVL stopid=%d\n",       s_stopId);
        CsfCmdPrintf(pEnv, "StartStop=%d\n",        getStopOffset( -1 /*RETURN_FIRST_STOP_ID*/ ));
        CsfCmdPrintf(pEnv, "override stop id=%d\n", s_overrideStopId);
        CsfCmdPrintf(pEnv, "AVL status=%s\n",       s_status== AVL_LOCATION_UNAVAILABLE?"Unavailable":"Available");  
        CsfCmdPrintf(pEnv, "3G status=%s\n",        get3GStatusString());  
        
      

        MYKI_ALARM_AlarmListPair_t alarms[256];
        U32_t size = 256;
		int ret = MYKI_ALARM_listSimpleAlarms(alarms, &size);
		if ( ret == 0 )
        {		
			for(U32_t i=0;i<size;i++)
			{
				CsfCmdPrintf(pEnv, "Alarm[%d]=%s-%u\n", i, MYKI_ALARM_unitTypeToString(alarms[i].unitType), alarms[i].alarmId);
			}
		}
		else
		{
			CsfCmdPrintf(pEnv, "Alarm list error %d\n", ret);
		}
        
        return 0;
    }
    CmdToken_t statusCommand =
    {
        commandST, descriptionST, CmdMainST, helpST
    };
}

///////////////////////////////////////////////////////////////////////////////

namespace
{
    const char commandEA[] = "enableall";
    const char descriptionEA[] = "Enable All Readers Command";
    const char* helpEA[] =
    {
        "\1"    "Synposis:",
        "\2"        "enableall",
                    "",
        "\1"    "Description:",
        "\2"        "Enable All Readers",
                    "",
        NULL
    };
    int CmdMainEA(CmdEnv_t *pEnv, int argc, const char **argv)
    {
        sendVaropToAll(VO_READER_ENABLED, 1);
        return 0;
    }
    CmdToken_t enableAllCommand =
    {
        commandEA, descriptionEA, CmdMainEA, helpEA
    };
}

///////////////////////////////////////////////////////////////////////////////

namespace
{
    const char commandDA[] = "disableall";
    const char descriptionDA[] = "Disable All Readers Command";
    const char* helpDA[] =
    {
        "\1"    "Synposis:",
        "\2"        "disableall",
                    "",
        "\1"    "Description:",
        "\2"        "Disable All Readers",
                    "",
        NULL
    };
    int CmdMainDA(CmdEnv_t *pEnv, int argc, const char **argv)
    {
        sendVaropToAll(VO_READER_ENABLED, 0);
        return 0;
    }
    CmdToken_t disableAllCommand =
    {
        commandDA, descriptionDA, CmdMainDA, helpDA
    };
}

///////////////////////////////////////////////////////////////////////////////

namespace
{
    const char  commandSendLoc[ ]       = "sendloc";
    const char  descriptionSendLoc[ ]   = "Send Location To All Readers Command";
    const char *helpSendLoc[ ]          =
    {
        "\1"    "Synposis:",
        "\2"        "sendloc <route-id> <stop-id> [available]",
                    "",
        "\1"    "Description:",
        "\2"        "Sends location to all readers.",
                    "",
        "\1"    "Parameters:",
        "\2"        "route-id   route id [1-65535]",
                    "stop-id    stop id [1-255]",
                    "available  true if location is available (default); false otherwise",
                    "",
        NULL
    };
    int CmdMainSendLoc( CmdEnv_t *pEnv, int argc, const char **argv )
    {
        #define MaxRouteId              (int32_t)0xffff
        #define MaxStopId               (int32_t)0xff
        #define SendLocArg_Name         0
        #define SendLocArg_RouteId      1
        #define SendLocArg_StopId       2
        #define SendLocArg_GpsStatus    3

        int32_t             nRouteId        = 0;
        int32_t             nStopId         = 0;
        int32_t             bAvailable      = TRUE;
        const char         *pArgv           = NULL;

        switch ( argc )
        {
        case    ( SendLocArg_GpsStatus + 1 ):
            if ( ( pArgv = argv[ SendLocArg_GpsStatus ] ) == NULL )
            {
                /*  SHOULD NEVER HAPPEN! */
                return  -1;
            }
            switch ( pArgv[ 0 ] )
            {
            case    't':    /*  [t]rue */   case    'T':    /*  [T]rue */
            case    'y':    /*  [y]es */    case    'Y':    /*  [Y]es */
            case    '1':
                bAvailable  = TRUE;
                break;

            case    'f':    /*  [f]alse */  case    'F':    /*  [F]alse */
            case    'n':    /*  [n]o */     case    'N':    /*  [N]o */
            case    '0':
                bAvailable  = FALSE;
                break;

            default:
                /*  Others! */
                CsfCmdPrintf( pEnv, "Invalid Location Availability Status (%s)\n", pArgv );
                return  -1;
            }
            /*  DROP THROUGH TO NEXT CASE! */

        case    ( SendLocArg_StopId + 1 ):
            if ( ( pArgv = argv[ SendLocArg_StopId ] ) == NULL )
            {
                /*  SHOULD NEVER HAPPEN! */
                return  -1;
            }
            nStopId     = (int32_t)atoi( pArgv );
            if ( nStopId <= 0 || nStopId > MaxStopId )
            {
                CsfCmdPrintf( pEnv, "Invalid Stop ID (%s)\n", pArgv );
                return  -1;
            }

            if ( ( pArgv = argv[ SendLocArg_RouteId ] ) == NULL )
            {
                /*  SHOULD NEVER HAPPEN! */
                return  -1;
            }
            nRouteId    = (int32_t)atoi( pArgv );
            if ( nRouteId <= 0 || nRouteId > MaxRouteId )
            {
                CsfCmdPrintf( pEnv, "Invalid Route ID (%s)\n", pArgv );
                return  -1;
            }

            {
                int                 nResult         = 0;
                int                 dcQueueId       = IpcGetID( DC_TASK );
                IPC_AVL_Notify_t    ipcAvlNotify    =
                {
                    {                       /*  header */
                        IPC_AVL_NOTIFY,
                        DC_TASK
                    },
                    9,                      /*  satsInView */
                    5,                      /*  satsUsed */
                    AVL_LOCATION_STOP_INSIDE, /*  status */
                    -136080745,             /*  latitude */
                    521744980,              /*  longitude */
                    0,                      /*  routeId */
                    0                       /*  stopId */
                };

                ipcAvlNotify.routeId    = nRouteId;
                ipcAvlNotify.stopId     = nStopId;
                ipcAvlNotify.status     = ( bAvailable != FALSE ? AVL_LOCATION_STOP_INSIDE : AVL_LOCATION_UNAVAILABLE );
                if ( ( nResult = IpcSend( dcQueueId, &ipcAvlNotify, sizeof( ipcAvlNotify ) ) ) != 0 )
                {
                    CsfCmdPrintf( pEnv, "IpcSend() failed (%d)\n", nResult );
                    return  -1;
                }
            }
            break;

        default:
            return  -1;
        }
        CsfCmdPrintf( pEnv, "OK\n" );
        return  0;
    }   /*  CmdMainSendLoc( ) */
    CmdToken_t  sendLocCommand          =
    {
        commandSendLoc, descriptionSendLoc, CmdMainSendLoc, helpSendLoc
    };
}

///////////////////////////////////////////////////////////////////////////////

namespace
{
    const char commandRefresh[] = "refresh";
    const char descriptionRefresh[] = "Refresh the browser page";
    const char* helpRefresh[] =
    {
        "\1"    "Synposis:",
        "\2"        "refresh",
                    "",
        "\1"    "Description:",
        "\2"        "Any connected websockets will be sent the refresh command",
                    "",
        NULL
    };
    int CmdMainRefresh(CmdEnv_t *pEnv, int argc, const char **argv)
    {       
	    IPC_NoPayload_t poll = { { IPC_WS_REFRESH_EVENT, (TaskID_t)DC_TASK } };
        IpcSend(appQueueId, &poll, sizeof poll);    
        CsfCmdPrintf(pEnv, "OK\n");
        return 0;
    }
    CmdToken_t refreshCommand =
    {
        commandRefresh, descriptionRefresh, CmdMainRefresh, helpRefresh
    };
}

///////////////////////////////////////////////////////////////////////////////

namespace
{
    const char descriptionCD[] = "control CD";
    const char* helpCD[] =
    {
        "\1"    "Synopsis:",
        "\2"        "cd load {filename}",
        "\2"        "cd info [-a]",
                    "",
        "\1"    "Description:",
        "\2"        "Load new CD from filename, or report info about CD",
        NULL
    };

    int CmdMainCD(CmdEnv_t *pEnv, int argc, const char **argv)
    {
        int ret = 1;
        if ( argc < 2 )
        {
            CsfCmdPrintf(pEnv, "cd: not enough arguments\n");
        }
        else if ( argc > 3 )
        {
            CsfCmdPrintf(pEnv, "cd: too many arguments\n");
        }
        else if ( strcmp(argv[1], "load") == 0 )
        {
            if ( argc != 3 )
            {
                CsfCmdPrintf(pEnv, "cd load: not enough arguments\n");
            }
            else
            {
                // TODO Check implications of loading CD whilst in operation.  May require logoff.                
                int cdRet = MYKI_CD_openCDDatabase(argv[2]);

                if ( cdRet )
                {
                    (void)MYKI_ALARM_sendSimpleAlarm(MYKI_ALARM_OPERATIONAL_STATE, 100, MYKI_ALARM_ALERT_CLEARED);
                    CsfCmdPrintf(pEnv, "cd: successfully loaded CD from '%s'\n", argv[2]);
                    CsVerbose("cd:successfully loaded from '%s'", argv[2]);
                    
                    Json::FastWriter fw;
                    Json::Value configMessage;   
                    char tariffVersion[256];
                    snprintf(tariffVersion, 255, "%d.%d",  MYKI_CD_getMajorVersion(), MYKI_CD_getMinorVersion());
                    configMessage["name"] = "config";
                    configMessage["data"]["tariff"] = tariffVersion;
                    sendDCMessage(fw.write(configMessage));    
                    
                    ret = 0;
                }
                else
                {
                    // REVIEW: The alarm raised here will put the DC out of service.
                    (void)MYKI_ALARM_sendSimpleAlarm(MYKI_ALARM_OPERATIONAL_STATE, 100, MYKI_ALARM_ALERT_CRITICAL);
                    CsfCmdPrintf(pEnv, "cd: could not load CD from '%s'\n", argv[2]);
                }
            }
        }
        else if ( strcmp(argv[1], "info") == 0 )
        {
            //TariffInformation:  Major version 147 Minor version 0 Activation Date 11/09/2013 3:00:00 AM
            CsVerbose("cd: Source=%s Major version %d Minor version %d Activation Date %d", 
                MYKI_CD_getSource(),
                MYKI_CD_getMajorVersion(),
                MYKI_CD_getMinorVersion(),
                MYKI_CD_getStartDateTime());
            if ( (argc >= 3) && (strcmp(argv[2], "-a") == 0) )
            {
                CsfCmdPrintf(pEnv, "Tariff %d.%d\n", MYKI_CD_getMajorVersion(), MYKI_CD_getMinorVersion());
            }
            else
            {
                CsfCmdPrintf(
                    pEnv,
                    "cd: Source=%s Major version %d Minor version %d Activation Date %d\n",
                    MYKI_CD_getSource(),
                    MYKI_CD_getMajorVersion(),
                    MYKI_CD_getMinorVersion(),
                    MYKI_CD_getStartDateTime()
                    );
            }

            ret = 0;
        }
        else
        {
            CsfCmdPrintf(pEnv, "cd: unknown sub-command '%s'\n", argv[1]);
        }

        return ret;
        
    }
    CmdToken_t CDCommand =
    {
        "cd", descriptionCD, CmdMainCD, helpCD
    };
}

///////////////////////////////////////////////////////////////////////////////

namespace
{
    const char descriptionSchedule[] = "Control Schedule database";
    const char* helpSchedule[] =
    {
        "\1"    "Synopsis:",
        "\2"        "schedule load {filename}",
        "\2"        "schedule info [-a]",
                    "",
        "\1"    "Description:",
        "\2"        "Load new schedule from filename, or report info about schedule",
        NULL
    };

    int CmdMainSchedule(CmdEnv_t *pEnv, int argc, const char **argv)
    {
        int ret = 1;
        if ( argc < 2 )
        {
            CsfCmdPrintf(pEnv, "schedule: not enough arguments\n");
        }
        else if ( argc > 3 )
        {
            CsfCmdPrintf(pEnv, "schedule: too many arguments\n");
        }
        else if ( strcmp(argv[1], "load") == 0 )
        {
            if ( argc != 3 )
            {
                CsfCmdPrintf(pEnv, "schedule load: not enough arguments\n");
            }
            else
            {
                int major =0;
                int minor =0;
                int version = 0;
                
                int scheduleRet = MYKI_Schedule_getVersion(argv[2], version, major, minor);
                std::string expiryDate = MYKI_Schedule_getExpiryDate(argv[2], g_serviceProviderId, g_unrosteredShiftID);

                if ( scheduleRet==0 && !expiryDate.empty() )
                {
                    
                    CsfCmdPrintf(pEnv, "schedule: successfully loaded CD from '%s'\n", argv[2]);
                    CsVerbose("cd:successfully loaded from '%s'", argv[2]);
                    
                    strncpy(g_scheduleDatabase, argv[2],CSFINI_MAXVALUE);
                    
                    Json::Value configMessage;   
                    Json::FastWriter fw;
                    char scheduleVersion[256];
                    snprintf(scheduleVersion, 255, "%d.%d.%d",  version, major, minor);
                    configMessage["name"] = "config";
                    configMessage["data"]["schedule"] = scheduleVersion;
                    configMessage["data"]["scheduledate"] = expiryDate.c_str();
                    sendDCMessage(fw.write(configMessage));                    
                                        

                    // TODO Notify front end - reportConfig.
                    ret = 0;
                }
                else
                {
                    CsfCmdPrintf(pEnv, "schedule: could not load Schedule from '%s'\n", argv[2]);
                }
            }
        }
        else if ( strcmp(argv[1], "info") == 0 )
        {
            int major    = 0;
            int minor    = 0;
            int version  = 0;
            
            int scheduleRet = MYKI_Schedule_getVersion(g_scheduleDatabase, version, major, minor);

	    time_t startDate = MYKI_Schedule_getStartDate(g_scheduleDatabase);

            CsVerbose("schedule: ret %d Source=%s Major version %d Minor version %d Activation Date %d",
                scheduleRet,            
                g_scheduleDatabase,
                major,
                minor,
		startDate );

            if ( (argc >= 3) && (strcmp(argv[2], "-a") == 0) )
            {
		struct  tm ctm  = { 0 };
		localtime_r( &startDate, &ctm );
                CsfCmdPrintf(pEnv, "Schedule %d.%d %04d-%02d-%02dT%02d:%02d:%02d\n", major, minor,
                        ctm.tm_year + 1900, ctm.tm_mon + 1, ctm.tm_mday,
                        ctm.tm_hour, ctm.tm_min, ctm.tm_sec );
            }
            else
            {
                CsfCmdPrintf(
                    pEnv,
                    "schedule: Source=%s Major version %d Minor version %d Activation Date %d\n", 
                    g_scheduleDatabase,
                    major,
                    minor,
		    startDate );
            }

            ret = 0;
        }
        else
        {
            CsfCmdPrintf(pEnv, "schedule: unknown sub-command '%s'\n", argv[1]);
        }

        return ret;
                
    }
    CmdToken_t scheduleCommand =
    {
        "schedule", descriptionSchedule, CmdMainSchedule, helpSchedule
    };
}


///////////////////////////////////////////////////////////////////////////////

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
        "\2"        "1         Uninitialised",
        "\2"        "2         Initialising",
        "\2"        "3+        In-Service",
        NULL
    };
    int CmdMainInit(CmdEnv_t *pEnv, int argc, const char **argv)
    {
        CsfCmdOpt_t opt;
        int wFlag = 0;
        int c;
        int result = 0;

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
			// Note the runlevel is used by process-manager to determine the availabilty of a process.
            result = s_DCState; 

            CsfCmdPrintf(pEnv, "Run-level is %d\n", result);
            
            if(result<3)
            {
                CsErrx("init: s_DCState is %d", result);
                result = 3;
            }
        }

        return result;
    }
    CmdToken_t initCommand =
    {
        "init", descriptionInit, CmdMainInit, helpInit
    };
}


///////////////////////////////////////////////////////////////////////////////

namespace
{
    const char debugDesc[] = "control debug levels";
      const char* debugHelp[] =
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

    int CmdMainDebug(CmdEnv_t *pEnv, int argc, const char **argv)
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
            if ( CsfIniExpand("MykiApp Debug:LocalLogFile", buf, sizeof(buf)) == 0 )
            {
                CsLogRotate(-1);
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
    CmdToken_t debugCommand =
    {
        "debug", debugDesc, CmdMainDebug, debugHelp
    };
}

extern "C" int HandleCommandLine(int argc, char** argv)
{
    bool displayHelp      = false;
    bool displayBuild     = false;
    bool displayCopyright = false;
    bool displayVersion   = false;
    bool argError         = false;
    char* iniFile         = strdup("/afc/etc/dc.ini");    
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
    if ( CsfCmdRegister(&statusCommand) != 0 )          CsErrx("CsfCmdRegister failed for 'status'");
    else if ( CsfCmdRegister(&initCommand) != 0 )       CsErrx("CsfCmdRegister failed for 'init'");
    else if ( CsfCmdRegister(&refreshCommand) != 0 )    CsErrx("CsfCmdRegister failed for 'refresh'");    
    else if ( CsfCmdRegister(&scheduleCommand) != 0 )   CsErrx("CsfCmdRegister failed for 'schedule'");    
    else if ( CsfCmdRegister(&CDCommand) != 0 )         CsErrx("CsfCmdRegister failed for 'cd'");    
    else if ( CsfCmdRegister(&enableAllCommand) != 0 )  CsErrx("CsfCmdRegister failed for 'enableAll'");    
    else if ( CsfCmdRegister(&disableAllCommand) != 0 ) CsErrx("CsfCmdRegister failed for 'disableAll'");
    else if ( CsfCmdRegister(&sendLocCommand) != 0 )    CsErrx("CsfCmdRegister failed for 'sendloc'");
    else if ( CsfCmdRegister(&debugCommand) != 0 )      CsErrx("CsfCmdRegister failed for 'debug'");
    else if ( CsfCmdServer("", "Command", NULL, 0, "DC Command Server\n", "DC cmd>" ) != 0 )
    {
        CsErrx("CsfCmdServer failed to start the command server");
    }

    free(iniFile);
    return 0;
}

int alarmInitialise()
{
	char directory[256] = {'\0'};
	if ( CsfIniExpand(INI_ALARMDIRECTORY, directory, sizeof(directory)) != 0 )
	{
		strcpy(directory, INIDEF_ALARMDIRECTORY);
	}
	char persistence[256] = {'\0'};
	if ( CsfIniExpand(INI_ALARMPERSISTENCE, persistence, sizeof(persistence)) != 0 )
	{
		strcpy(persistence, INIDEF_ALARMPERSISTENCE);
	}
	return MYKI_ALARM_init((TaskID_t)DC_TASK, DC_TASK /* actually EMM reader proxy */, IPC_MYKI_ALARM, 0, 0, directory, persistence);
}

void clearAlarms()
{
    (void)MYKI_ALARM_sendSimpleAlarm(MYKI_ALARM_INTRUSION_STATE, 10, MYKI_ALARM_ALERT_CLEARED);
}

void FurtherSignalHandling(int sig)
{
    if (appQueueId == -1)   // too early
    {
        CsErrx("'extra' signal handler could not send messages");
        return;
    }

    switch (sig)
    {
    case SIGSEGV:
    case SIGILL:
        /*FALLTHROUGH*/
    case SIGKILL:
    case SIGTERM:
        CsDebug(1, (1, "signal handler SIGTERM %d", SIGTERM));
        (void)MYKI_ALARM_sendSimpleAlarm(MYKI_ALARM_OPERATIONAL_STATE, 50, MYKI_ALARM_ALERT_WARNING);
        
    case SIGINT:
    case SIGPIPE:
        {
            CsDebug(1, (3, "'extra' signal handler - signal %d", sig));
        }
        break;

    /* ignored */
    case SIGALRM:
    case SIGUSR1:
    case SIGUSR2:
        break;

    default:
        CsDebug(1, (1, "'extra' signal handler ignored signal %d", sig));
        break;
    }
}

void LoadIniFile()
{
    char iniBuf[CSFINI_MAXVALUE + 1];
      
    g_resourcePath = "./";
    if ( CsfIniExpand("General:ResourcePath", iniBuf, CSFINI_MAXVALUE) == 0 )
    {
        g_resourcePath = iniBuf;
    }

    if ( CsfIniExpand("General:TerminalId", g_terminalId, sizeof(g_terminalId)) != 0 )
    {
        CsWarnx("Terminal ID not present in configuration files");
        g_terminalId[0] = 0;
    }  
        
    if ( CsfIniExpand("General:TerminalType", g_terminalType, sizeof(g_terminalType)) != 0 )
    {
        CsWarnx("Terminal Type not present in configuration files");
        g_terminalType[0] = 0;
    }

    if ( CsfIniExpand("General:RolesJson", g_serviceProvidersFile, CSFINI_MAXVALUE) != 0 )
    {
        CsWarnx("RolesJson not present in configuration files");
        g_serviceProvidersFile[0] = 0;
    }

    if ( CsfIniExpand("General:ServiceProvidersJson", g_serviceProvidersFile, CSFINI_MAXVALUE) != 0 )
    {
        CsWarnx("ServiceProvidersJson not present in configuration files");
        g_serviceProvidersFile[0] = 0;
    }
    
    if ( CsfIniExpand("General:ScheduleDatabase", g_scheduleDatabase, CSFINI_MAXVALUE) != 0 )
    {
        CsWarnx("ScheduleDatabase not present in configuration files");
        g_scheduleDatabase[0] = 0;
    }   
    
    if ( CsfIniExpand("General:AccessPointName", g_accessPointName, CSFINI_MAXVALUE) != 0 )
    {
        CsWarnx("AccessPointName not present in configuration files");
        g_accessPointName[0] = 0;
    }       

    if ( CsfIniExpand("General:StationId", g_stationId, CSFINI_MAXVALUE) != 0 )
    {
        CsWarnx("StationId not present in configuration files");
        g_stationId[0] = 0;
    } 

    /*  Vehicle registration number */
    if ( CsfIniExpand("General:VehicleId", g_vehicleId, CSFINI_MAXVALUE) != 0 )
    {
        CsWarnx("VehicleId not present in configuration files");
        g_vehicleId[0] = 0;
    } 

    
    /* AllowTestPasswords */
    if ( CsfIniExpand("General:AllowTestPasswords", iniBuf, CSFINI_MAXVALUE) == 0 )
    {
        g_allowTestPasswords = (iniBuf[0]=='Y' || iniBuf[0]=='y' || iniBuf[0]=='T' || iniBuf[0]=='t' || iniBuf[0]=='1');
        CsWarnx("Use of testing Password of the Day is %s", g_allowTestPasswords?"enabled":"disabled");
    }     
    
    /* Allow SOSO at headless mode when driver not logged in */
    if ( CsfIniExpand("TPE:SOSOWhenDriverNotLoggedIn", iniBuf, CSFINI_MAXVALUE) == 0 )
    {
        s_sosoWhenDriverNotLoggedIn = (iniBuf[0]=='Y' || iniBuf[0]=='y' || iniBuf[0]=='T' || iniBuf[0]=='t' || iniBuf[0]=='1');
        CsWarnx("Allow SOSO when driver not logged is %s", s_sosoWhenDriverNotLoggedIn ? "enabled" : "disabled");
    } 

    if (CsfIniExpandBool("TPE:AutoPrintReceipt", &g_autoPrintReceipt) != 0)
    {
        CsWarnx("TPE:AutoPrintReceipt not present in congiruration files");
    }
    
    if (CsfIniExpandBool("TPE:RoundingHalfUp", &g_roundingHalfUp) != 0)
    {
        CsWarnx("TPE:RoundingHalfUp not present in congiruration files");
    }
    
    #define GetNumericConfig(CFG, VALUE, DEFAULT) \
     do { \
         if ( CsfIniExpand(CFG, iniBuf, CSFINI_MAXVALUE) == 0 ) \
         { \
             VALUE = atoi(iniBuf); \
         } \
         else \
         { \
             VALUE = DEFAULT; \
         } \
     } while (0)    
    

        GetNumericConfig("General:PollDelayMS",         g_pollDelayMS,            5000);
        GetNumericConfig("General:HttpPort",            g_httpPort,               7681);
        GetNumericConfig("General:ServiceProviderID",   g_serviceProviderId,      1001);
        GetNumericConfig("General:ScreensaverTimeout",  g_screensaverTimeout,     120);
        
        GetNumericConfig("TPE:SalesCompleteTimeout",    g_salesCompleteTimeout, 1000);
        GetNumericConfig("TPE:CurrencyRoundingPortion", g_currencyRoundingPortion, 5);
        GetNumericConfig("TPE:AutoStartOfTripTimeout",  g_autoStartOfTripTimeout, 60);
        GetNumericConfig("TPE:ChangeEndsTimeout",       g_changeEndsTimeout,      300);
        GetNumericConfig("TPE:DriverBreakTimeout",      g_driverBreakTimeout,     900);
        GetNumericConfig("TPE:InvalidPODTimeout",       g_InvalidPODTimeout,      5);        
        GetNumericConfig("TPE:EPLRecoveryPeriod",       g_EPLRecoveryPeriod,      DEFAULT_EPL_RECOVERY_PERIOD);
        if(g_EPLRecoveryPeriod<MIN_EPL_RECOVERY_PERIOD || g_EPLRecoveryPeriod>MAX_EPL_RECOVERY_PERIOD)
        {
            CsErrx("TPE:EPLRecoveryPeriod out of range (min %d, max %d, value %d) setting to default %d", 
                    MIN_EPL_RECOVERY_PERIOD, MAX_EPL_RECOVERY_PERIOD, g_EPLRecoveryPeriod, DEFAULT_EPL_RECOVERY_PERIOD);
            g_EPLRecoveryPeriod = DEFAULT_EPL_RECOVERY_PERIOD;
        }
        GetNumericConfig("TPE:DisconnectedWarningTimeout",  g_disconnectedWarningTimeout, 30);
        GetNumericConfig("TPE:LANCommunicationTimeout",     g_LANCommunicationTimeout, 300);
        
        GetNumericConfig("TPE:AutoLogoffTimeout",           g_autoLogoffTimeout, 3600);
        GetNumericConfig("TPE:OperationalModeTimeout",      g_operationalModeTimeout, 300);
        
        GetNumericConfig("TPE:AutoEndOfTripTimeout",        g_autoEndOfTripTimeout, 60);
        GetNumericConfig("TPE:ManualEndShiftWaitTime",      g_manualEndShiftWaitTime, 30);
        GetNumericConfig("TPE:ErrorHMIMsgTimeout",          g_errorHMIMsgTimeout, 1000);
        GetNumericConfig("TPE:SuccessHMIMsgTimeout",        g_successHMIMsgTimeout, 500);
                
        GetNumericConfig("GPRS:SerialLinkSpeed",            g_serialLinkSpeed, DEFAULT_SERIAL_LINK_SPEED);  
        
        GetNumericConfig("TPE:UnrosteredShiftID",           g_unrosteredShiftID, 9999);
        GetNumericConfig("TPE:UnrosteredTripID",            g_unrosteredTripID, 9999);
        
        GetNumericConfig("TPE:HeadlessTripID",              g_headlessTripID, 9999);
        GetNumericConfig("TPE:HeadlessShiftID",             g_headlessShiftID, 9999);
        
        GetNumericConfig("TPE:ShiftTotalsMaskLevel",        g_shiftTotalsMaskLevel, (int)MASK_NONE);

	// AVL Settings
        //GetNumericConfig("TPE:NumberOfHoursBetweenShifts",    g_NumberOfHoursBetweenShifts, 20);  
        //GetNumericConfig("TPE:OffRouteTimeout",     g_OffRouteTimeout, 20);  

        // TODO Add TPE Parameters from FS.

    #undef GetNumericConfig		

    if (g_InvalidPODTimeout < 1 || g_InvalidPODTimeout > 60)
    {
        CsErrx("Configured value for InvalidPODTimeout is not within the limit. Setting it to default");
        g_InvalidPODTimeout = 5;
    }
}

void InitTimerService()
{
    CsTime_t    v = { 0 , 50 };    // N ms ticker
    if (CsfTimerInit(&v) != 0)
        CsErrx("Unable to create timer service");
}

int main(int argc, char** argv)
{
    SigInit(FurtherSignalHandling);

    if ( HandleCommandLine(argc, argv) != 0 )
        return 10;
    
    if (CsfProcInit(true, true, NULL, INI_CORECONFIG, INI_STACKCONFIG, INIDEF_CORECONFIG, INIDEF_STACKCONFIG) != 0)
        CsErrx("main: could not initialise core/stack dump on error");
        
    alarmInitialise();
    clearAlarms();

    if ( IpcInit() != 0 )
    {
        CsErrx("IpcInit() failed");
        return -1;
    }
    
    LoadIniFile();
    
    CsMutexInit(&g_rxQueueLock , CSSYNC_THREAD);
    CsMutexInit(&g_queueLock , CSSYNC_THREAD);
    CsMutexInit(&g_heartbeatMapLock , CSSYNC_THREAD);
    CsMutexInit(&g_persistLock , CSSYNC_THREAD);
    CsMutexInit(&g_LANCommunicationTimerLock, CSSYNC_THREAD);
    CsMutexInit(&g_stateLock, CSSYNC_THREAD);
    
    appQueueId     = IpcGetID(DC_TASK);
    wsQueueId      = IpcGetID(WS_TASK);
    asQueueId      = IpcGetID(AS_TASK);
    avlTaskId      = IpcGetID(MYKI_AVL_TaskId);    
    printerTaskId  = IpcGetID(DC_PRINTER_TASK); 

    if(InitialiseWebsocket()!=0)
    {
      return -1;
    }

    // Create Timer Service
    InitTimerService();
       
    getNetworkSettings();        
    
    while(initCCE()!=0)
    {
        CsErrx("Cannot connect to CCE waiting 5 seconds");
        CsSleep(5, 0);
    }   
    
    // Execute application
    ProcessMessages(NULL);

    if (IpcClose() != 0)
        CsErrx("IpcClose: failed to close");

    CsMutexDestroy(&g_rxQueueLock );
    CsMutexDestroy(&g_queueLock );
    CsMutexDestroy(&g_heartbeatMapLock );
    CsMutexDestroy(&g_LANCommunicationTimerLock );
    CsMutexDestroy(&g_persistLock );
    CsMutexDestroy(&g_stateLock );
    return 0;
}

