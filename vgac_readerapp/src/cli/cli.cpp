/* -*- mode: c; tabs: 4; -*- */
/*=============================================================================
**
**    Vix Technology                   Licensed software
**    (C) 2014                         All rights reserved
**
**=============================================================================
**
**  Project/Product : MBU
**  Filename        : cli.cpp
**  Author(s)       : ?
**
**  Description     :
**      Implements reader application's command line interface.
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
**  History         :
**   Vers.  Date        Aut.  Type     Description
**   -----  ----------  ----  -------  ----------------------------------------
**    1.00  dd.mm.yy    ???   Create
**    1.01  27.10.14    ANT   Add      Added 'ntpcb' command, ie. NTP callback
**    1.02  16.07.15    ANT   Add      MTU-161: Added "config" command to
**                                     (importantly) enable setting location
**
**===========================================================================*/

#include <json/json.h>

#include "message_reader.h"
#include <ct.h>
#include <csmutex.h>
#include <csthread.h>
#include <unistd.h>

#include "myki_cd.h"
#include "myki_br.h"
#include "myki_actionlist.h"
#include "myki_alarm.h"
#include "myki_alarm_names.h"
#include "cli.h"
#include "version.h"
#include "csfcmd.h"
#include "csf.h"
#include "cslog.h"
#include "Common.h"
#include "LDTStructs.h"
#include "LDT_cmd.h"
#include "LDT.h"
#include <myki_tmi.h>
#include "VCard.h"
#include <string.h>
#include "envcmd.h"
#include "timing_point.h"
#include "card_processing_common.h"
#include "card_processing_thread.h"
#include "ipc.h"
#include "app_debug_levels.h"
#include "myki_cardservices.h"
#include "dataxfer.h"
#include "utcmd.h"

// should be defined in LDT.h, but old version does not
#ifndef LDT_UDSN_FILE_PATH
#error LDT_UDSN_FILE_PATH not defined
#endif

#define ERROR_USAGE "Check the usage of this command with help command\n"

/*
 *      External References
 *      -------------------
 */

    /*  CARDFUNCTIONS.CPP */
extern  OAppShiftDataStatus_t   g_overwriteShiftStatus;
extern  Time_t                  g_overwriteShiftStartTime;
extern  Time_t                  g_overwriteShiftCloseTime;


typedef struct 
{
   const char *cmd;
   const char *subcmd;
   bool  (*func1)();
   bool  (*func2)(int argc, const char *argv[], void *data);
   char* (*func3)();
   char* (*func4)(int argc, const char *argv[], void *data);
   int minArgs; 
   int maxArgs; 
   void *data;
   const char *err;
} CmdFunc_t;

CmdFunc_t cf[] = {
    {"card"   , "detect"            , NULL, card_detect                      , NULL, NULL,  1,  1, NULL,                         ERROR_USAGE},  
    {"card"   , "load"              , NULL, card_load                        , NULL, NULL,  1,  2, NULL,                         ERROR_USAGE},  
    {"card"   , "write"             , NULL, card_write , NULL , NULL,                       1,  1, NULL,                         ERROR_USAGE},  
    {"card"   , "close"             , card_close , NULL, NULL , NULL,                       0,  0, NULL,                         ERROR_USAGE},  
    {"card"   , "notify"            , card_notify, NULL, NULL , NULL,                       0,  0, NULL,                         ERROR_USAGE},  
    {"card"   , "enquiry"           , NULL, NULL, card_enquiry, NULL,                       0,  0, NULL,                         ERROR_USAGE},  
    {"card"   , "image"             , NULL, card_image  , NULL, NULL,                       2,  2, NULL,                         ERROR_USAGE},
    {"card"   , "unconfirmed"       , NULL, card_unconfirmed,                  NULL, NULL,  1,  1, NULL,                         ERROR_USAGE},
    {"card"   , "snapshot"          , NULL, card_snapshot,                     NULL, NULL,  0,  1, NULL,                         ERROR_USAGE},
    {"card",    "validatetransitcard",  card_validatetransitcard, NULL, NULL, NULL,         0,  0, NULL,                         ERROR_USAGE},

    {"config" , "setDirection"      , NULL, config_setDirection, NULL, NULL,                1,  1, NULL,   ERROR_USAGE},  
    {"config" , "setLocation"       , NULL, config_setLocation, NULL,NULL,                  2,  4, NULL,   ERROR_USAGE},  
    {"config" , "setServiceProvider", NULL, config_setServiceProvider, NULL, NULL,          1,  1, NULL,   ERROR_USAGE},  
    {"config" , "setDeviceId"       , NULL, config_setDeviceId, NULL, NULL,                 1,  1, NULL,   ERROR_USAGE},
    {"config" , "setTime"           , NULL, NULL, NULL, time_set,                           1,  2, NULL,   NULL},  
    {"config" , "enquiry"           , NULL, NULL, NULL, config_enquiry,                     0,  5, NULL,   ERROR_USAGE},
    {"tmi"    , "createOpLog"       , tmi_createOpLog, NULL, NULL, NULL,                    0,  0, NULL,   ERROR_USAGE}
};

char* executeCmd(CmdEnv_t* pEnv, const char* cmd, const char* subcmd, int argc, const char* argv[])
{
    bool isExecuted = false;
    bool ret = true;
    char *pRet = NULL;
    int cmdSize = sizeof(cf) / sizeof(CmdFunc_t);

    for (int i = 0; i < cmdSize; i++)
    {
        if (strcmp(cmd, cf[i].cmd) == 0 && strcmp(subcmd, cf[i].subcmd) == 0)
        {
            isExecuted = true;

            CsDebug(9, (9, "Executing command %s-%s", cmd, subcmd));

            if (cf[i].minArgs > 0 && argv == NULL)
            {
                CsErrx(cf[i].err);
                CsfCmdPrintf(pEnv, cf[i].err);
                return pRet;
            }

            //if (strcmp(subcmd, "enquiry") == 0)
            //{
                //pRet = cf[i].func3();
                //break;
            //}
            if (argc == 0 && cf[i].minArgs  ==0  && cf[i].maxArgs ==0 ) //== 0 && cf[i].data == NULL)
            {
                if( cf[i].func1)
                {
                    CsDebug(9, (9, "calling func1()" ));
                    ret = cf[i].func1();
                }
                if( cf[i].func3)
                {
                    CsDebug(9, (9, "calling func3()" ));
                    pRet = cf[i].func3();
                }
            }
            else if (cf[i].minArgs  <= argc  &&  argc <= cf[i].maxArgs  ) 
            {
                if( cf[i].func2)
                {
                    CsDebug(9, (9, "calling func2()" ));
                    ret = cf[i].func2(argc, argv, cf[i].data);
                }
                if( cf[i].func4)
                {
                    CsDebug(9, (9, "calling func4()" ));
                    pRet = cf[i].func4(argc, argv, cf[i].data);
                }
            }
            else
            {
                CsErrx("Command %s-%s has not been defined", cmd, subcmd);  
                isExecuted = false;
            }

            if (ret == false)
            {
                CsfCmdPrintf(pEnv, "Command %s-%s failed, see log\n", cmd, subcmd);
                CsErrx("Command %s-%s failed", cmd, subcmd);
            }
            else
            {
                    CsfCmdPrintf(pEnv, "Ok\n");
            }

            break;
        }
    }

    if (!isExecuted)
    {
        CsfCmdPrintf(pEnv, "Command %s-%s was not found\n", cmd, subcmd);
        CsErrx( "Command: %s-%s was not found", cmd, subcmd ); 
    }

    return pRet;
}

namespace {
    const char  appversionDesc[] = "appversion";
    const char  initDesc[] = "process initialisation";
    const char  statusDesc[] = "status command";
//  const char  productDesc[] = "product sale command";
    const char  tpurseDesc[] = "purse sale command";
    const char  cardDesc[] = "card command";
    const char  configDesc[] = "config command";
    const char  debugDesc[] = "control debug levels";
    const char  udsnDesc[] = "control udsn";
    const char  cdDesc[] = "control CD";
    const char  actionlistDesc[] = "control actionlist";
    const char  samDesc[] = "control SAM file";
    const char  ntpcbDesc[]         = "set time synchronisation info";
	const char  testDesc[]          = "test command";
    const char  timeDesc[] = "time configuration command";

    const char* appversionHelp[] =
    {
        "\1"    "Synposis:",
        "\2"        "appversion",
                    "",
        "\1"    "Description:",
        "\2"        "If verbose output is specified, all arguments are echoed.",
                    "",
        NULL
        };

    const char tmiDesc[] = "create TMI files";

    const char* timeHelp[] =
    {
        "\1"    "Synposis:",
        "\2"        "time <HH:MM:SS>",
        "",
        "\1"    "Description:",
        "\2"        "This command is to set current time.",
        "",
        NULL
    };

    const char* initHelp[] =
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
        "\2"        "1         Process start",
        "\2"        "2         Initialising",
        "\2"        "3         Waiting to be enabled",
        "\2"        "4         Normal operation",
        "\2"        "5         Card detected",
        "\2"        "6         Waiting for ECU",
        "\2"        "7         Performing validation",
        "\2"        "8,9       Waiting for card removal",
        "\2"        "10        Card processing complete",
        NULL
        };


    const char* statusHelp[] =
    {
        "\1"    "Synposis:",
        "\2"        "status",
                    "",
        "\1"    "Description:",
        "\2"        "Report the current status of the reader",
        NULL
        };

        
    const char* cardHelp[]  =
    {
        "\1"    "Synposis:",
        "\2"        "card [ detect <card Type> | load <file> | save <file> | close | image <dir> <UID> | enquiry | notify | unconfirmed <true|false> | snapshot <true|false> ]",
                    "",
        "\1"    "Description:",
        "\2"        "This command is used for card operations.",
                    "",
        NULL
        };

    const char* configHelp[] =
    {
        "\1"    "Synposis:",
        "\2"        "config setDirection {entry|exit}",
        "\2"        "config setDeviceId {deviceId}",
        "\2"        "config setLocation {route stop entry-point}",
        "\2"        "config setServiceProvider {spId}",
        "\2"        "config setTime {time}",
        "\2"        "config enquiry",
        "\2"        "config enquiry {device-parameter}",
                    "",
        "\1"    "Description:",
        "\2"        "This is environment configuration command.",
                    "",
        NULL
        };

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

    const char* udsnHelp[] =
    {
        "\1"    "Synposis:",
        "\2"        "udsn {number}",
                    "",
        "\1"    "Description:",
        "\2"        "Set the current UDSN",
        NULL
        };

    const char* cdHelp[] =
    {
        "\1"    "Synposis:",
        "\2"        "cd load {filename}",
        "\2"        "cd info [-a]",
                    "",
        "\1"    "Description:",
        "\2"        "Load new CD from filename, or report info about CD",
        NULL
        };

    const char* actionlistHelp[] =
    {
        "\1"    "Synposis:",
        "\2"        "actionlist load {filename} [commit]",
        "\2"        "actionlist delta {filename}",
        "\2"        "actionlist info [-a]",
                    "",
        "\1"    "Description:",
        "\2"        "Load new actionlist or delta from filename, or report info",
        NULL
        };

    const char* samHelp[] =
    {
        "\1"    "Synposis:",
        "\2"        "sam info",
                    "",
        "\1"    "Description:",
        "\2"        "Print the SAM id",
        NULL
        };
    const char *ntpcbHelp[ ]    =
    {
        "\1"    "Synposis:",
        "\2"        "ntpcb {step|stratum|periodic} {offset} {stratum} [{freq_drift_ppm} [{poll_interval}]]",
        "\1"    "Description:",
        "\2"        "Sets time synchronisation information",
        NULL
    };  /*  ntpcbHelp[ ] */

    const char* testHelp[]  =
    {
        "\1"    "Synposis:",
        "\2"        "test condition condition-code",
                    "",
        "\1"    "Description:",
        "\2"        "Sets test condition (1=bypass BR_LLSC_0_1)",
                    "",
        NULL
        };

    const char *tmiHelp[] = 
    {
        "\1"  "Synopsis:",
        "\2"  "tmi { createOpLog }",
        "\1"  "Description:",
        "\2"  "Creates TMI files",
        NULL
    };

    int appversionCommand(CmdEnv_t* pEnv, int argc, const char* argv[])
        {
            CsfCmdPrintf(pEnv, "%s %s\n%s\n", BUILDNAME, BUILDDATE, GetModuleInfo());
            return 0;
        }

    int initCommand( CmdEnv_t* pEnv, int argc, const char* argv[] )
    {
        CsfCmdOpt_t                 opt;
        int                         wFlag   = 0;
        int                         c;
        int                         result  = 0;
        CardProcessingControl_t*    control = GetCardProcessingControl( );

        CsfCmdOptinit( &opt, pEnv->sh_stdout );
        while ( ( c = CsfCmdOptget( &opt, argc, argv, "w" ) ) != EOF )
        {
            switch ( c )
            {
            case 'w':
                wFlag++;
                break;

            default:
                CsfCmdPrintf( pEnv, "Unknown flag '%c'\n", opt.optopt );
                break;
            }
        }

        if ( argc > opt.optind )
        {
            int     newRunLevel = -1;
            int     reason      = (int)CP_DISABLE_REASON_DEFAULT;
            char    waste;

            if
            (
                ( sscanf( argv[ opt.optind ], "%d%c", &newRunLevel, &waste ) != 1 ) ||
                ( newRunLevel < (int)CARD_PROCESSING_STATE_TERMINATING            ) ||
                ( newRunLevel > (int)CARD_PROCESSING_STATE_TRANSACTION_COMPLETE   )
            )
            {
#ifdef TEST_BUILD
                if(newRunLevel==99) // Test feature
                {
                    CsfCmdPrintf( pEnv, "Simulate Deadlock\n", argv[ opt.optind ] );
                    CsMutexLock( &control->lock );
                }
                else
#endif 
                {
                    CsfCmdPrintf( pEnv, "Unknown run-level '%s'\n", argv[ opt.optind ] );
                }
            }
            else
            {
                switch( newRunLevel )
                {
                case    (int)CARD_PROCESSING_STATE_TERMINATING:
                    TerminateCardProcessing( );
                    break;

                case    (int)CARD_PROCESSING_STATE_WAITING_FOR_ENABLE:
                    if
                    (
                        ( argc > ( opt.optind + 1 ) ) &&
                        (
                            sscanf( argv[ opt.optind + 1 ], "%d%c", &reason, &waste ) != 1 ||
                            reason < (int)CP_DISABLE_REASON_DEFAULT
                        )
                    )
                    {
                        CsfCmdPrintf( pEnv, "Unknown reason '%s'\n", argv[ opt.optind + 1 ] );
                    }
                    else
                    {
                        CsMutexLock( &control->lock );
                        {
                            control->gateReady  = false;
                            SetCardProcessingState(
                                    CARD_PROCESSING_STATE_WAITING_FOR_ENABLE,
                                    TRUE /* RESOURCE_LOCKDED */,
                                    (CP_REASON_e)reason );
                        }
                        CsMutexUnlock( &control->lock );
                    }   /*  end-of-if */
                    break;

                case    (int)CARD_PROCESSING_STATE_WAITING_FOR_CARD:
                    if
                    (
                        ( argc > ( opt.optind + 1 ) ) &&
                        (
                            sscanf( argv[ opt.optind + 1 ], "%d%c", &reason, &waste ) != 1 ||
                            reason < (int)CP_DISABLE_REASON_DEFAULT // Why not just zero
                        )
                    )
                    {
                        CsfCmdPrintf( pEnv, "Unknown reason '%s'\n", argv[ opt.optind + 1 ] );
                    }
                    else
                    {
                        CsMutexLock( &control->lock );
                        {
                            control->gateReady  = true;
                            CsCondBcast( &control->changed );

                            if ( control->state == CARD_PROCESSING_STATE_WAITING_FOR_ENABLE )
                            {
                                SetCardProcessingState(
                                        CARD_PROCESSING_STATE_WAITING_FOR_CARD,
                                        TRUE /* RESOURCE_LOCKDED */,
                                        (CP_REASON_e)reason );
                            }   /*  end-of-if */
                        }
                        CsMutexUnlock( &control->lock );
                    }
                    break;

                default:
                    CsfCmdPrintf( pEnv, "Don't know how to change to run-level %d\n", newRunLevel );
                    break;
                }   /*  end-of-switch */
            }
        }
        else
        {
            /*  NOTE:   Changed to static variable to avoid potential stack corruption if
                        IPC thread responds to internal heartbeat after time out! */
            static  int internalHeartbeatResult = 0;

            result                  = (int)control->state;
            internalHeartbeatResult = (int)CARD_PROCESSING_STATE_UNKNOWN;

            /*  Checks if IPC thread is still running... */
            {
                int appQueueId  = IpcGetID( APP_TASK ); 
                
                IPC_InternalHeartbeat_t msg;
                msg.hdr.type            = IPC_INTERNAL_HEARTBEAT;
                msg.hdr.source          = APP_TASK;
                msg.fromProcessId       = getpid();
                msg.result              = &internalHeartbeatResult;

                if(appQueueId>=0 && IpcSend(appQueueId , &msg, sizeof( msg ) )>=0)
                {
                    // Wait for response for 8 seconds.  If delay is longer than 10 seconds process manager
                    // sets the run level to 0 (terminating) and does not take any further action.
                    for ( int waits = 0; ( waits < 80 ) && ( internalHeartbeatResult == (int)CARD_PROCESSING_STATE_UNKNOWN ); ++waits )
                    {
                        /*  IPC thread has not responded to internal heartbeat message... */
                        CsSleep( 0, 100 );
                    }
                }
                else
                {
                    CsErrx( "Init CLI command: Could not send IPC message app queue id %d", appQueueId);
                }
            }

            if ( internalHeartbeatResult == (int)CARD_PROCESSING_STATE_UNKNOWN )
            {
                CsfCmdPrintf( pEnv, "!!!!! Internal heartbeat did not respond !!!!!\nActual run-level is %d but returning %d\n", result, (int)CARD_PROCESSING_STATE_UNKNOWN );
                CsErrx( "!!!!! Internal heartbeat did not respond !!!!! - Actual run-level is %d but returning %d", result, (int)CARD_PROCESSING_STATE_UNKNOWN );
                result  = (int)CARD_PROCESSING_STATE_UNKNOWN;
            }
            else
            {
                result  = internalHeartbeatResult;
                CsfCmdPrintf( pEnv, "Run-level is %d\n", result );
            }
        }

        return result;
    }   /*  initCommand( ) */


    int statusCommand(CmdEnv_t* pEnv, int argc, const char* argv[])
    {
        CardProcessingControl_t*    control = GetCardProcessingControl( );
        char buf[100];
        
        CsfCmdPrintf(pEnv, "State=%d\n", control->state); // TODO Show as string
        CsfCmdPrintf(pEnv, "Reason=%d\n", control->reason);
        //CsfCmdPrintf(pEnv, "gateReady=%d\n", control->gateReady?1:0);
        //CsfCmdPrintf(pEnv, "cardDetectEnabled=%d\n", control->cardDetectEnabled?1:0);
        //CsfCmdPrintf(pEnv, "operatorCardDetectEnabled=%d\n", control->operatorCardDetectEnabled?1:0);
        CsfCmdPrintf(pEnv, "OutOfOrder=%d\n", control->outOfOrder?1:0);
        CsfCmdPrintf(pEnv, "SamId=%08X\n", GetCardProcessingThreadContextData()->StaticData.deviceSamId);
        CsfCmdPrintf(pEnv, "InputVoltage=%.3fV\n",  (float)(g_inputVoltage) / 1000.0);
        
        CsStrTime( &g_startTime, int(sizeof( buf )), buf );        
        CsfCmdPrintf(pEnv, "StartTime=%s\n", buf);        
        CsfCmdPrintf(pEnv, "TransactionsSinceStart=%d\n", g_totalTransactions);             
        CsfCmdPrintf(pEnv, "RejectedTransactionsSinceStart=%d\n", g_totalRejectedTransactions);             
        CsfCmdPrintf(pEnv, "AcceptedTransactionsSinceStart=%d\n", g_totalAcceptedTransactions);   
        
        // TODO Add: Device Type (FPDs, FPDg)
        // TODO Add: 
        // TODO Add: More status fields...
        // TODO Add:
        
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

    int configCommand(CmdEnv_t* pEnv, int argc, const char* argv[])
    {
        if (argc >= 2)
        {    
            char *pRet;
            pRet = executeCmd( pEnv, argv[ 0 ], argv[ 1 ], argc - 2, &argv[ 2 ] );

            if (pRet) 
            {
                CsfCmdPrintf(pEnv, pRet);
                CsFree(pRet);
            }
        }
        else
        {
            CsfCmdPrintf(pEnv, "More arguments are required\n");
        }

        return 0;
    }

    int cdCommand(CmdEnv_t* pEnv, int argc, const char* argv[])
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
                CardProcessingControl_t* pControl = GetCardProcessingControl();
                CsMutexLock(&pControl->lock);
                while (
                        (pControl->state != CARD_PROCESSING_STATE_WAITING_FOR_ENABLE) &&
                        (pControl->state != CARD_PROCESSING_STATE_WAITING_FOR_CARD)
                      )
                {
                    CsCondWait(&pControl->changed, &pControl->lock);
                }
                int cdRet = MYKI_CD_openCDDatabase(argv[2]);
                CsMutexUnlock(&pControl->lock);

                if ( cdRet )
                {
                    CsfCmdPrintf(pEnv, "cd: successfully loaded CD from '%s'\n", argv[2]);
                    CsVerbose("cd:successfully loaded from '%s'", argv[2]);

                    /*  Updates default BR context data */
                    {
                        MYKI_BR_ContextData_t*  pDefaultContextData = GetCardProcessingThreadContextData();
                        InitialiseBRContextFromCD( pDefaultContextData );
                    }

                    ret = 0;
                }
                else
                {
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
                time_t          ltm     = MYKI_CD_getStartDateTime();
                struct  tm      ctm     = { 0 };

                localtime_r( &ltm, &ctm );
                CsfCmdPrintf( pEnv, "Tariff %d.%d %04d-%02d-%02dT%02d:%02d:%02d\n",
                        MYKI_CD_getMajorVersion(), MYKI_CD_getMinorVersion(),
                        ctm.tm_year + 1900, ctm.tm_mon + 1, ctm.tm_mday,
                        ctm.tm_hour, ctm.tm_min, ctm.tm_sec );
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

    int actionlistCommand( CmdEnv_t *pEnv, int argc, const char *argv[] )
    {
/*
usage: actionlist load|delta|info

  Note: Path names containing whitespace characters are currently not supported.
*/
        #define actionlist_Name             0
        #define actionlist_SubCommand       1
        #define actionlist_info_Option      2
        #define actionlist_load_Path        2
        #define actionlist_load_Activate    3
        #define actionlist_delta_Path       2
        #define actionlist_ArgcMin          2
        #define actionlist_ArgcMax          4

        int ret = 1;

        if ( argc < actionlist_ArgcMin )
        {
            CsfCmdPrintf( pEnv, "actionlist: not enough arguments\n" );
        }
        else if ( argc > actionlist_ArgcMax )
        {
            CsfCmdPrintf( pEnv, "actionlist: too many arguments\n" );
        }
        else if ( strcmp( argv[ actionlist_SubCommand ], "info" ) == 0 )
        {
            if ( argc > actionlist_info_Option && strcmp( argv[ actionlist_info_Option ], "-a" ) == 0 )
            {
//              CsfCmdPrintf( pEnv, "Actionlist %d\n", MYKI_ACTIONLIST_getSequence() );
                CsfCmdPrintf( pEnv, MYKI_ACTIONLIST_getDetails() );
            }
            else
            {
                CsfCmdPrintf( pEnv, "actionlist: sequence info %s\n", MYKI_ACTIONLIST_getSequenceInfo() );
            }
            ret = 0;
        }
        else if ( strcmp( argv[ actionlist_SubCommand ], "load" ) == 0 )
        {
            /*  Load FULL actionlist */
            CardProcessingControl_t    *pControl    = GetCardProcessingControl();

            /*  Waits for reader application to be IDLE */
            CsMutexLock( &pControl->lock );
            while (
                    (pControl->state != CARD_PROCESSING_STATE_WAITING_FOR_ENABLE) &&
                    (pControl->state != CARD_PROCESSING_STATE_WAITING_FOR_CARD)
                  )
            {
                CsCondWait(&pControl->changed, &pControl->lock);
            }
            CsDebug( 7, ( 7, "Loading full actionlist argc is %d, args are 0=<%s> 1=<%s> 2=<%s> 3=<%s>",
                    argc,
                    argv[ actionlist_Name       ],
                    argv[ actionlist_SubCommand ],
                    argv[ actionlist_load_Path  ],
                    argc > actionlist_load_Activate ? argv[ actionlist_load_Activate ] : "(null)" ) );
            
            int loadRet = MYKI_CD_openFullActionlistDatabase( argv[ actionlist_load_Path ] );
            CsMutexUnlock( &pControl->lock );

            if ( loadRet )
            {
                CsfCmdPrintf( pEnv, "actionlist: successfully loaded actionlist from '%s'\n", argv[ actionlist_load_Path ] );
                if ( argc > actionlist_load_Activate )
                {
                    switch ( argv[ actionlist_load_Activate ][ 0 ] )
                    {
                    case    'A':    case    'a':    /*  [Aa]ctivate */
                    case    'C':    case    'c':    /*  [Cc]ommit   */
                    case    'T':    case    't':    /*  [Tt]rue     */
                    case    'Y':    case    'y':    /*  [Yy]es      */
                    case    '1':
                        /*  Closes DELTA actionlist database */
                        MYKI_CD_closeDeltaActionlistDatabase( );
                        break;

                    default:
                        /*  DONOTHING! */
                        break;
                    }
                }
                ret = 0;
            }
            else
            {
                (void)MYKI_ALARM_sendSimpleAlarm( MYKI_ALARM_DEVICE_MANAGEMENT_STATE, 160, MYKI_ALARM_ALERT_WARNING );
                CsfCmdPrintf( pEnv, "actionlist: could not load actionlist from '%s'\n", argv[ actionlist_load_Path ] );
            }
        }
        else if ( strcmp( argv[ actionlist_SubCommand ], "delta") == 0 )
        {
            CardProcessingControl_t    *pControl    = GetCardProcessingControl();

            /*  Waits for reader application to be IDLE */
            CsMutexLock( &pControl->lock );
            while (
                    (pControl->state != CARD_PROCESSING_STATE_WAITING_FOR_ENABLE) &&
                    (pControl->state != CARD_PROCESSING_STATE_WAITING_FOR_CARD)
                  )
            {
                CsCondWait( &pControl->changed, &pControl->lock );
            }
            CsDebug( 7, ( 7, "Loading delta actionlist from  file <%s>", argv[ actionlist_delta_Path ] ) );

            int loadRet = MYKI_CD_openDeltaActionlistDatabase( argv[ actionlist_delta_Path ] );
            CsMutexUnlock( &pControl->lock );

            if ( loadRet )
            {
                CsfCmdPrintf( pEnv, "actionlist: successfully loaded actionlist from '%s'\n", argv[ actionlist_delta_Path ] );
                ret = 0;
            }
            else
            {
                (void)MYKI_ALARM_sendSimpleAlarm( MYKI_ALARM_DEVICE_MANAGEMENT_STATE, 160, MYKI_ALARM_ALERT_WARNING );
                CsfCmdPrintf( pEnv, "actionlist: could not load actionlist from '%s'\n", argv[ actionlist_delta_Path ] );
            }
        }
        else
        {
            CsfCmdPrintf( pEnv, "actionlist: invalid sub-command '%s'\n", argv[ actionlist_SubCommand ] );
        }
        return ret;
    }

    int cardCommand(CmdEnv_t* pEnv, int argc, const char* argv[])
    {
        char *pRet;
        if (argc == 2)
        {
            pRet = executeCmd(pEnv, argv[0], argv[1], 0, (const char**)NULL);

            if (pRet) 
            {
                CsfCmdPrintf(pEnv, pRet);
                CsFree(pRet);
            }
        }
        else if (argc >= 3)
        {
            executeCmd(pEnv, argv[0], argv[1], argc -2  , argv + 2);
        }
        else
        {
            CsfCmdPrintf(pEnv,  ERROR_USAGE);
        }

        return 0;
    }

    int timeCommand(CmdEnv_t* pEnv, int argc, const char* argv[])
    {
        char* pRet =0;
        if (argc > 1)
        {
            pRet = executeCmd(pEnv, "config", "setTime", argc-1, argv + 1);
        }
        else
        {
            CsfCmdPrintf(pEnv,ERROR_USAGE);
        }
        if
            (pRet) 
            {
                CsfCmdPrintf(pEnv, pRet);
                CsFree(pRet);
            }
        return 0;
    }

    int debugCommand(CmdEnv_t *pEnv, int argc, const char* argv[])
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

    int udsnCommand(CmdEnv_t *pEnv, int argc, const char* argv[])
    {
        if ( argc != 2 )
        {
            CsfCmdPrintf(pEnv, "Need one argument %s- the new UDSN\n", (argc > 2 ? "only " : ""));
        }
        else
        {
            unsigned int newUdsn = atoi(argv[1]);
            FILE* f = fopen(LDT_UDSN_FILE_PATH, "w");
            if ( f != 0 )
            {
                fprintf(f, "%u\n", newUdsn);
                fclose(f);
                CsfCmdPrintf(pEnv, "Ok\n");
            }
            else
            {
                CsfCmdPrintf(pEnv, "Could not open UDSN file\n");
            }
        }
        return 0;
    }

    int ecuCommand(CmdEnv_t *pEnv, int argc, const char* argv[])
    {
        IPC_IntPayload_t    message;

        message.data        = 0;
        switch ( argc )
        {
        case    3:
            /*  Enable/Disable reason code */
            message.data    = atoi( argv[ 2 ] );
            /*  DROP THROUGH TO NEXT CASE! */

        case    2:
            if ( CsStricmp( argv[ 1 ], "enable" ) == 0 )
            {
                message.hdr.type    = IPC_CSC_ENABLE;
            }
            else if ( CsStricmp( argv[ 1 ], "disable" ) == 0 )
            {
                message.hdr.type    = IPC_CSC_DISABLE;
            }
            else
            {
                CsfCmdPrintf( pEnv, "Unknown argument '%s'", argv[ 1 ] );
                return 1;
            }   /*  end-of-if */
            message.hdr.source      = APP_TASK;

            IpcSend( IpcGetID( APP_TASK ), &message, sizeof( message ) );
            CsfCmdPrintf( pEnv, "Ok\n" );
            break;

        case    1:
            CsfCmdPrintf( pEnv, "ECU %sready\n", ( GetCardProcessingControl()->gateReady ? "" : "not " ) );
            break;

        default:
            CsfCmdPrintf( pEnv, "Too many arguments\n" );
            return 1;
        }   /*  end-of-switch */
        return 0;
    }

    int samCommand(CmdEnv_t* pEnv, int argc, const char* argv[])
    {
        int ret = 1;
        if ( argc < 2 )
        {
            CsfCmdPrintf(pEnv, "sam: need a sub-command\n");
        }
        else if ( argc > 3 )
        {
            CsfCmdPrintf(pEnv, "sam: too many arguments\n");
        }
        else if ( strcmp(argv[1], "load") == 0 )
        {
            if ( argc != 3 )
            {
                CsfCmdPrintf(pEnv, "sam load: need a filename\n");
            }
            else if ( access(argv[2], R_OK) != 0 )
            {
                CsfCmdPrintf(pEnv, "sam load: file '%s' is not readable\n", argv[2]);
            }
            else
            {
                CardProcessingControl_t* pControl = GetCardProcessingControl();
                CsMutexLock(&pControl->lock);
                while (
                        (pControl->state != CARD_PROCESSING_STATE_WAITING_FOR_ENABLE) &&
                        (pControl->state != CARD_PROCESSING_STATE_WAITING_FOR_CARD)
                      )
                {
                    CsCondWait(&pControl->changed, &pControl->lock);
                }
                char* fileName = strdup(argv[2]);
                int samRet = MYKI_CS_LoadKSAMKeyExportFile(fileName);
                free(fileName);
                CsMutexUnlock(&pControl->lock);

                if ( samRet == MYKI_CS_OK )
                {
                    CsfCmdPrintf(pEnv, "sam: successfully loaded SAM file '%s'\n", argv[2]);
                    CsVerbose("sam: successfully loaded SAM file '%s'", argv[2]);
                    ret = 0;
                }
                else
                {
                    const char* errorMsg = "";
                    switch ( samRet )
                    {
                    case MYKI_CS_ERR_CRC_VERIFY:
                        errorMsg = "CRC error";
                        break;
                    case MYKI_CS_ERR_INVALID_FILE:
                        errorMsg = "Invalid file";
                        break;
                    default:
                        errorMsg = "Unknown error";
                    }
                    CsfCmdPrintf(pEnv, "sam: could not load SAM file '%s' - %s (%d)\n", argv[2], errorMsg, samRet);
                    CsWarnx("sam: could not load SAM file '%s' - %s (%d)", argv[2], errorMsg, samRet);
                }
            }

            (void)MYKI_ALARM_sendSimpleAlarm(MYKI_ALARM_SAM_KEY_UPDATE_STATE, 10, (ret == 0 ? MYKI_ALARM_ALERT_CLEARED : MYKI_ALARM_ALERT_WARNING));
        }
        else if ( strcmp(argv[1], "info") == 0 )
        {
            CsfCmdPrintf(pEnv, "sam %08X\n", GetCardProcessingThreadContextData()->StaticData.deviceSamId); 
        }        
        else
        {
            CsfCmdPrintf(pEnv, "sam: unknown sub-command '%s'\n", argv[1]);
        }

        return ret;
    }

    /*==========================================================================*
    **
    **  ntpcbCommand
    **
    **  Description     :
    **      Processes "ntpcb" command, ie. NTPD callback.
    **
    **  Parameters      :
    **      pEnv                [I]     CLI environment
    **      argc                [I]     number of CLI arguments
    **      argv                [I]     CLI argument values
    **
    **  Returns         :
    **      None
    **
    **  Notes           :
    **
    **==========================================================================*/

    int ntpcbCommand( CmdEnv_t *pEnv, int argc, const char *argv[ ] )
    {
        #define NTPCB_COMMAND           0
        #define NTPCB_EVENT             1
        #define NTPCB_OFFSET            2
        #define NTPCB_STRATUM           3
        #define NTPCB_FREQ_DRIFT_PPM    4
        #define NTPCB_POLL_INTERVAL     5

        const char         *pEvent          = NULL;
        const char         *pOffset         = NULL;
        const char         *pStratum        = NULL;
        const char         *pFreqDriftPPM   = NULL;
        const char         *pPollInterval   = NULL;

        switch ( argc )
        {
        /*  Optional */
        case ( NTPCB_POLL_INTERVAL  + 1 ):  pPollInterval   = argv[ NTPCB_POLL_INTERVAL     ];
        case ( NTPCB_FREQ_DRIFT_PPM + 1 ):  pFreqDriftPPM   = argv[ NTPCB_FREQ_DRIFT_PPM    ];
        /*  Mandatory */
        case ( NTPCB_STRATUM        + 1 ):  pStratum        = argv[ NTPCB_STRATUM           ];
                                            pOffset         = argv[ NTPCB_OFFSET            ];
                                            pEvent          = argv[ NTPCB_EVENT             ];
                                            break;

        default:                            CsfCmdPrintf( pEnv, "ntpcb: Too few/many argument(s)\n" );
                                            return 1;
        }   /*  end-of-switch */

        TimeSyncUpdate( pEvent, pOffset, pStratum, pFreqDriftPPM, pPollInterval );

        CsfCmdPrintf( pEnv, "OK\n" );
        return 0;
    }   /*  ntpcbCommand( ) */

    /*==========================================================================*
    **
    **  testCommand
    **
    **  Description     :
    **      Processes "test" command.
    **
    **  Parameters      :
    **      pEnv                [I]     CLI environment
    **      argc                [I]     number of CLI arguments
    **      argv                [I]     CLI argument values
    **
    **  Returns         :
    **      None
    **
    **  Notes           :
    **
    **==========================================================================*/

    int testCommand( CmdEnv_t *pEnv, int argc, const char *argv[] )
    {
        #define TEST_COMMAND            0
        #define TEST_SUBCOMMAND         1
        #define TEST_CONDITION_CODE     2

        if ( argc < ( TEST_SUBCOMMAND + 1 ) )
        {
            CsfCmdPrintf( pEnv, "test: missing sub-command\n" );
            return  1;
        }
        else
        if ( CsStricmp( argv[ TEST_SUBCOMMAND ], "condition" ) == 0 )
        {
            MYKI_BR_ContextData_t  *pData       = GetCardProcessingThreadContextData( );
            int                     condition   = TEST_CONDITION_NONE;

            pData->TestData.condition           = TEST_CONDITION_NONE;
            switch ( argc - ( TEST_SUBCOMMAND + 1 ) )
            {
            case    1:
                condition   = atoi( argv[ TEST_CONDITION_CODE ] );
                if ( condition != TEST_CONDITION_NONE &&
                     condition != TEST_CONDITION_BYPASS_BR_LLSC_0_1 )
                {
                    CsfCmdPrintf( pEnv, "test: Invalid test condition (%d)\n", condition );
                    return  1;
                }
                /*  DROP THROUGH TO NEXT CASE! */

            case    0:
                pData->TestData.condition       = condition;
                break;
                
            default:
                CsfCmdPrintf( pEnv, "test: Too many arguments\n" );
                return  1;
            }
        }
        else
        {
            CsfCmdPrintf( pEnv, "test: Invalid sub-command '%s'\n", argv[ TEST_SUBCOMMAND ] );
            return  1;
        }

        CsfCmdPrintf( pEnv, "OK\n" );
        return  0;
    }   /*  testCommand( ) */

    const char  shiftDesc[ ]    = "open and close shift";
    const char *shiftHelp[ ]    =
    {
        "\1"    "Synposis:",
        "\2"        "shift open [driver-id [shift-id]]",
        "\2"        "shift close",
        "\2"        "shift portion [status [start-time] [close-time]]",
        "\1"    "Description:",
        "\2"        "Open or close shift",
        NULL
    };

        /**
         *  @brief  Processes 'shift' open/close command.
         *  @param  pEnv command line interface environment.
         *  @param  argc number of arguments (including command name).
         *  @param  argv argument values.
         *  @return 0 if successful; otherwise failed.
         */
    int shiftCommand( CmdEnv_t *pEnv, int argc, const char *argv[ ] )
    {
        #define SHIFT_COMMAND           0
        #define SHIFT_SUBCOMMAND        1

        MYKI_BR_ContextData_t      *pData           = GetCardProcessingThreadContextData( );

        if ( argc >= ( SHIFT_SUBCOMMAND + 1 ) && strcmp( argv[ SHIFT_SUBCOMMAND ], "portion" ) == 0 )
        {
            #define SHIFT_STATUS            2
            #define SHIFT_START_TIME        3
            #define SHIFT_CLOSE_TIME        4

            OAppShiftDataStatus_t   ShiftStatus     = OAPP_SHIFT_DATA_STATUS_SUSPENDED;
            Time_t                  ShiftStartTime  = TIME_NOT_SET;
            Time_t                  ShiftCloseTime  = TIME_NOT_SET;
            CsTime_t                now             = { 0 };

            CsTime( &now );

            switch ( argc )
            {
            case    ( SHIFT_CLOSE_TIME  + 1 ):
                ShiftCloseTime  = (Time_t)atoi( argv[ SHIFT_CLOSE_TIME ] );
                /*  DROP THROUGH TO NEXT CASE! */

            case    ( SHIFT_START_TIME  + 1 ):
                ShiftStartTime  = (Time_t)atoi( argv[ SHIFT_START_TIME ] );
                /*  DROP THROUGH TO NEXT CASE! */

            case    ( SHIFT_STATUS      + 1 ):
                if ( argv[ SHIFT_STATUS ][ 0 ] == 'A' || argv[ SHIFT_STATUS ][ 0 ] == 'a' )
                {
                    ShiftStatus = OAPP_SHIFT_DATA_STATUS_ACTIVATED;
                }
                else
                if ( argv[ SHIFT_STATUS ][ 0 ] == 'S' || argv[ SHIFT_STATUS ][ 0 ] == 's' ||
                     argv[ SHIFT_STATUS ][ 0 ] == 'C' || argv[ SHIFT_STATUS ][ 0 ] == 'c' )
                {
                    ShiftStatus = OAPP_SHIFT_DATA_STATUS_SUSPENDED;
                }
                else
                {
                    CsfCmdPrintf( pEnv, "%s: invalid shift status '%s'", argv[ SHIFT_STATUS ] );
                    return  1;
                }
                /*  DROP THROUGH TO NEXT CASE! */

            case    ( SHIFT_SUBCOMMAND  + 1 ):
                g_overwriteShiftStatus      = ShiftStatus;
                g_overwriteShiftStartTime   = ShiftStartTime;
                g_overwriteShiftCloseTime   = ShiftStatus == OAPP_SHIFT_DATA_STATUS_SUSPENDED &&
                                              ShiftCloseTime == TIME_NOT_SET ?
                                                    now.tm_secs :
                                                    ShiftCloseTime;
                break;

            default:
                CsfCmdPrintf( pEnv, "%s: too many arguments\n", argv[ SHIFT_COMMAND ] );
                return  1;
            }
        }
        else
        {
            #define SHIFT_DRIVER_ID         2
            #define SHIFT_SHIFT_ID          3

            std::string             DriverId        = "0";
            int                     ShiftId         = 0;
            bool                    IsShiftClose    = false;

            switch ( argc )
            {
            case    ( SHIFT_SHIFT_ID    + 1 ):
                ShiftId     = atoi( argv[ SHIFT_SHIFT_ID ] );
                /*  DROP THROUGH TO NEXT CASE! */

            case    ( SHIFT_DRIVER_ID   + 1 ):
                DriverId    = argv[ SHIFT_DRIVER_ID ];
                /*  DROP THROUGH TO NEXT CASE! */

            case    ( SHIFT_SUBCOMMAND  + 1 ):
                if ( strcmp( argv[ SHIFT_SUBCOMMAND ], "open" ) == 0 )
                    IsShiftClose    = false;
                else
                if ( strcmp( argv[ SHIFT_SUBCOMMAND ], "close" ) == 0 )
                    IsShiftClose    = true;
                else
                {
                    CsfCmdPrintf( pEnv, "%s: invalid subcommand '%s'\n",
                            argv[ SHIFT_COMMAND ], argv[ SHIFT_SUBCOMMAND ] );
                    return  1;
                }
                break;

            case    ( SHIFT_COMMAND     + 1 ):
                break;

            default:
                CsfCmdPrintf( pEnv, "%s: too many arguments\n", argv[ SHIFT_COMMAND ] );
                return  1;
            }

            {
                Json::Value         request;
                Json::FastWriter    fw;
                char                TerminalId[ 32 ]    = { 0 };
                std::string         requestJson;

                CsSnprintf( TerminalId, sizeof( TerminalId ), "%d", pData->StaticData.deviceId );
                if ( IsShiftClose == false )
                {
                    request[ "terminalid"       ]               = TerminalId;
                    request[ "name"             ]               = "logon";
                    request[ "operatorid"       ]               = DriverId;
                    request[ "role"             ]               = "driver";
                    request[ "passwordlogin"    ]               = true;
                    requestJson                                 = fw.write( request );
                    ungetMessage( requestJson );

                    request[ "terminalid" ]                     = TerminalId;
                    request[ "name"       ]                     = "tablecard";
                    request[ "userid"     ]                     = DriverId;
                    request[ "data"       ][ "shift_number" ]   = ShiftId;
                    requestJson                                 = fw.write( request );
                    ungetMessage( requestJson );
                }
                else
                {
                    request[ "terminalid" ]                     = TerminalId;
                    request[ "name"       ]                     = "shiftend";
                    request[ "userid"     ]                     = DriverId;
                    requestJson                                 = fw.write( request );
                    ungetMessage( requestJson );
                }
            }
        }
        CsfCmdPrintf( pEnv, "OK\n" );
        return  0;
    }

    const char  counterDesc[ ]  = "clear/get/set counter";
    const char *counterHelp[ ]  =
    {
        "\1"    "Synposis:",
        "\2"        "counter clear [all|counter-key ...]",
        "\2"        "counter get   [all|counter-key ...]",
        "\2"        "counter set   counter-key counter-value ...]",
        "\2"        "counter inc   counter-key increment-value ...]",
        "\1"    "Description:",
        "\2"        "Clear/Get/Set counter value",
        NULL
    };

        /**
         *  @brief  Processes 'counter' clear/get/set command.
         *  @param  pEnv command line interface environment.
         *  @param  argc number of arguments (including command name).
         *  @param  argv argument values.
         *  @return 0 if successful; otherwise failed.
         */
    int counterCommand( CmdEnv_t *pEnv, int argc, const char *argv[ ] )
    {
        #define COUNTER_COMMAND                 0
        #define COUNTER_SUBCOMMAND              1
        #define COUNTER_KEY                     2
        #define COUNTER_VALUE                   3
        #define COUNTER_SUBCOMMAND_CLEAR        0
        #define COUNTER_SUBCOMMAND_GET          1
        #define COUNTER_SUBCOMMAND_SET          2
        #define COUNTER_SUBCOMMAND_INCREMENT    3

        int             subCommand              = COUNTER_SUBCOMMAND_GET;
        const char     *pCommand                = argv[ COUNTER_COMMAND ];
        const char     *pKey                    = NULL;
        unsigned int    value                   = 0;
        const char     *pErrInvalidSubCommand   = "%s: invalid sub-command '%s'\n";
        const char     *pErrTooFewArguments     = "%s: missing argument(s)\n";
        const char     *pErrClearCounter        = "%s: failed clearing '%s' counter\n";
        const char     *pErrClearAllCounters    = "%s: failed clearing all counters\n";
        const char     *pErrGetCounter          = "%s: failed retrieving '%s' counter value\n";
        const char     *pErrSetCounter          = "%s: failed setting '%s' counter value\n";
        const char     *pErrIncCounter          = "%s: failed incrementing '%s' counter value\n";
        const char     *pErrSaveCounters        = "%s: failed saving counters\n";
        const char     *pErrNotImplemented      = "%s: not implemented\n";
        int             result                  = 0;
        int             maxCounters             = 0;
        int             i                       = 0;
        CardProcessingControl_t
                       *pControl                = GetCardProcessingControl( );
        #define RESOURCE_LOCK()                 { CsMutexLock(   &pControl->lock ); }
        #define RESOURCE_UNLOCK()               { CsMutexUnlock( &pControl->lock ); }

        if ( argc >= ( COUNTER_SUBCOMMAND + 1 ) )
        {
            if      ( strcmp( argv[ COUNTER_SUBCOMMAND ], "clear" ) == 0 )  subCommand  = COUNTER_SUBCOMMAND_CLEAR;
            else if ( strcmp( argv[ COUNTER_SUBCOMMAND ], "get"   ) == 0 )  subCommand  = COUNTER_SUBCOMMAND_GET;
            else if ( strcmp( argv[ COUNTER_SUBCOMMAND ], "set"   ) == 0 )  subCommand  = COUNTER_SUBCOMMAND_SET;
            else if ( strcmp( argv[ COUNTER_SUBCOMMAND ], "inc"   ) == 0 )  subCommand  = COUNTER_SUBCOMMAND_INCREMENT;

            switch ( subCommand )
            {
            case    COUNTER_SUBCOMMAND_CLEAR:
                if ( argc < ( COUNTER_KEY + 1 ) )
                {
                    /*  Must have at least one key */
                    CsfCmdPrintf( pEnv, pErrTooFewArguments, pCommand );
                    return  1;
                }
                /*  DROP THROUGH TO NEXT CASE! */

            case    COUNTER_SUBCOMMAND_GET:
                /*  Extracts counter keys */
                maxCounters     = argc - COUNTER_KEY;
                break;

            case    COUNTER_SUBCOMMAND_SET:
            case    COUNTER_SUBCOMMAND_INCREMENT:
                if ( argc < ( COUNTER_VALUE + 1 ) )
                {
                    /*  Must have at least one key/value pair */
                    CsfCmdPrintf( pEnv, pErrTooFewArguments, pCommand );
                    return  1;
                }

                /*  Extracts counter key/value pairs */
                maxCounters     = argc - COUNTER_KEY;
                if ( ( maxCounters & 1 ) != 0 )
                {
                    /*  Missing value */
                    CsfCmdPrintf( pEnv, pErrTooFewArguments, pCommand );
                    return  1;
                }
                maxCounters    /= ( 1 /*KEY*/ + 1 /*VALUE*/ );
                break;

            default:
                /*  Unknown sub-command */
                CsfCmdPrintf( pEnv, pErrInvalidSubCommand, pCommand, argv[ COUNTER_SUBCOMMAND ] );
                return  1;
            }
        }

        switch ( subCommand )
        {
        case    COUNTER_SUBCOMMAND_CLEAR:
            RESOURCE_LOCK()
            {
                if ( strcmp( argv[ COUNTER_KEY ], "all" ) == 0 )
                {
                    if ( TMI_clearAllCounters() < 0 )
                    {
                        CsfCmdPrintf( pEnv, pErrClearAllCounters, pCommand, "all" );
                        RESOURCE_UNLOCK()
                        return 1;
                    }
                }
                else
                {
                    i   = 0;
                    while ( i < maxCounters )
                    {
                        pKey    = argv[ i + COUNTER_KEY ];
                        if ( ( result = TMI_clearCounter( pKey ) ) < 0 )
                        {
                            CsfCmdPrintf( pEnv, pErrClearCounter, pCommand, pKey );
                            RESOURCE_UNLOCK()
                            return  1;
                        }
                        i++;
                    }
                }

                /*  And persists counter values */
                if ( ( result = TMI_saveCounters() ) < 0 )
                {
                    CsfCmdPrintf( pEnv, pErrSaveCounters, pCommand );
                    RESOURCE_UNLOCK()
                    return  1;
                }
            }
            RESOURCE_UNLOCK()
            break;

        case    COUNTER_SUBCOMMAND_GET:
            if ( maxCounters == 0 || strcmp( argv[ COUNTER_KEY ], "all" ) == 0 )
            {
                /*  Retrieves all counter values */
                CsfCmdPrintf( pEnv, pErrNotImplemented, pCommand );
                return  1;
            }
            i   = 0;
            while ( i < maxCounters )
            {
                pKey    = argv[ i + COUNTER_KEY ];
                if ( ( result = TMI_getCounter( pKey, &value ) ) < 0 )
                {
                    CsfCmdPrintf( pEnv, pErrGetCounter, pCommand, pKey );
                    return  1;
                }
                CsfCmdPrintf( pEnv, "%s=%d\n", pKey, value );
                i++;
            }
            return  0;

        case    COUNTER_SUBCOMMAND_SET:
            RESOURCE_LOCK()
            {
                i   = 0;
                while ( i < maxCounters )
                {
                    pKey    =       argv[ ( i * 2 ) + COUNTER_KEY   ];
                    value   = atoi( argv[ ( i * 2 ) + COUNTER_VALUE ] );
                    if ( ( result = TMI_setCounter( pKey, value ) ) < 0 )
                    {
                        CsfCmdPrintf( pEnv, pErrSetCounter, pCommand, pKey );
                        RESOURCE_UNLOCK()
                        return  1;
                    }
                    CsfCmdPrintf( pEnv, "%s=%d\n", pKey, value );
                    i++;
                }

                /*  And persists counter values */
                if ( ( result = TMI_saveCounters() ) < 0 )
                {
                    CsfCmdPrintf( pEnv, pErrSaveCounters, pCommand );
                    RESOURCE_UNLOCK()
                    return  1;
                }
            }
            RESOURCE_UNLOCK()
            return  0;

        case    COUNTER_SUBCOMMAND_INCREMENT:
            RESOURCE_LOCK()
            {
                i   = 0;
                while ( i < maxCounters )
                {
                    pKey    =       argv[ ( i * 2 ) + COUNTER_KEY   ];
                    value   = atoi( argv[ ( i * 2 ) + COUNTER_VALUE ] );
                    if ( ( result = TMI_incrementCounterBy( pKey, &value, value ) ) < 0 )
                    {
                        CsfCmdPrintf( pEnv, pErrIncCounter, pCommand, pKey );
                        RESOURCE_UNLOCK()
                        return  1;
                    }
                    CsfCmdPrintf( pEnv, "%s=%d\n", pKey, value );
                    i++;
                }

                /*  And persists counter values */
                if ( ( result = TMI_saveCounters() ) < 0 )
                {
                    CsfCmdPrintf( pEnv, pErrSaveCounters, pCommand );
                    RESOURCE_UNLOCK()
                    return  1;
                }
            }
            RESOURCE_UNLOCK()
            return  0;
        }

        CsfCmdPrintf( pEnv, "OK\n" );
        return  0;
    }

    /**
     * @brief Processes tmi commands.
     *
     * @param  pEnv command line interface environment.
     * @param  argc number of arguments (including command name).
     * @param  argv argument values.
     * @return 0 if successful; otherwise failed.
     */
    int tmiCommand(CmdEnv_t* pEnv, int argc, const char* argv[])
    {
        char *pRet = NULL;

        if (argc == 2)
        {
            pRet = executeCmd(pEnv, argv[0], argv[1], 0, NULL);
            
            if (pRet != NULL)
            {
                CsfCmdPrintf(pEnv, pRet);
                CsFree(pRet);
            }

            return 0;
        }

        return 1;
    }

    const char  productDesc[ ]  = "list/sale product";
    const char *productHelp[ ]  =
    {
        "\1"    "Synposis:",
        "\2"        "product list [product-type [product-zone]]",
        "\2"        "product sale [product-type [product-zone]]",
        "\1"    "Description:",
        "\2"        "List/Sale products",
        NULL
    };

        /**
         *  @brief  Processes 'product' list/sales command.
         *  @param  pEnv command line interface environment.
         *  @param  argc number of arguments (including command name).
         *  @param  argv argument values.
         *  @return 0 if successful; otherwise failed.
         */
    int productCommand( CmdEnv_t *pEnv, int argc, const char *argv[ ] )
    {
        #define PRODUCT_COMMAND     0
        #define PRODUCT_SUBCOMMAND  1
        #define PRODUCT_TYPE        2
        #define PRODUCT_ZONE        3

        MYKI_BR_ContextData_t  *pData                   = GetCardProcessingThreadContextData();
        const char             *pErrInvalidSubCommand   = "%s: invalid sub-command '%s'\n";
        const char             *pErrTooManyArguments    = "%s: too many argument(s)\n";
        const char             *pErrGetProducts         = "%s: failed getting saleable products\n";
        const char             *pErrNoProduct           = "%s: no saleable product found\n";
        const char             *pCommand                = argv[ PRODUCT_COMMAND ];
        const char             *pSubCommand             = "list";
        const char             *pType                   = "ThirdParty";
        int                     zone                    = 1;
        Json::Value             products;
        Json::Value             request;
        std::string             requestJson;
        Json::FastWriter        fw;

        switch ( argc )
        {
        case    ( PRODUCT_ZONE + 1 ):
            zone        = atoi( argv[ PRODUCT_ZONE ] );
            /*  DROP THROUGH TO NEXT CASE! */

        case    ( PRODUCT_TYPE + 1 ):
            pType       = argv[ PRODUCT_TYPE ];
            /*  DROP THROUGH TO NEXT CASE! */

        case    ( PRODUCT_SUBCOMMAND + 1 ):
            pSubCommand = argv[ PRODUCT_SUBCOMMAND ];
            /*  DROP THROUGH TO NEXT CASE! */

        case    ( PRODUCT_COMMAND + 1 ):
            if ( strcmp( pSubCommand, "list" ) == 0 )
            {
                if ( MYKI_CD_getProducts( products, zone, pType, TIME_NOT_SET,
                        pData->StaticData.serviceProviderId ) == false )
                {
                    CsfCmdPrintf( pEnv, pErrGetProducts, pCommand );
                    return  1;
                }
                else
                if ( products[ "products" ].size() == 0 )
                {
                    CsfCmdPrintf( pEnv, pErrNoProduct, pCommand );
                }
                else
                {
                    for ( Json::ValueIterator itr = products[ "products" ].begin();
                            itr != products[ "products" ].end();
                            itr++ )
                    {
                        Json::Value    &product     = ( *itr );

                        CsfCmdPrintf( pEnv, "Product.id         = %d\n", product[ "id"         ].asInt()     );
                        CsfCmdPrintf( pEnv, "       .issuer_id  = %d\n", product[ "issuer_id"  ].asInt()     );
                        CsfCmdPrintf( pEnv, "       .type       = %s\n", product[ "type"       ].asCString() );
                        CsfCmdPrintf( pEnv, "       .subtype    = %s\n", product[ "subtype"    ].asCString() );
                        CsfCmdPrintf( pEnv, "       .short_desc = %s\n", product[ "short_desc" ].asCString() );
                        CsfCmdPrintf( pEnv, "       .long_desc  = %s\n", product[ "long_desc"  ].asCString() );
                        CsfCmdPrintf( pEnv, "       .Price      = %d\n", product[ "Price"      ].asInt()     );
                    }
                }
            }
            else
            if ( strcmp( pSubCommand, "sales" ) == 0 )
            {
                if ( MYKI_CD_getProducts( products, zone, pType, TIME_NOT_SET,
                        pData->StaticData.serviceProviderId ) == false )
                {
                    CsfCmdPrintf( pEnv, pErrGetProducts, pCommand );
                    return  1;
                }
                else
                if ( products[ "products" ].size() == 0 )
                {
                    CsfCmdPrintf( pEnv, pErrNoProduct, pCommand );
                }
                else
                {
                    int         quantity            = 0;
                    char        TerminalId[ 32 ]    = { 0 };
                    Json::Value cartItems           = Json::Value( Json::arrayValue );

                    CsSnprintf( TerminalId, sizeof( TerminalId ), "%d", pData->StaticData.deviceId );
                    request[ "terminalid" ]         = TerminalId;
                    request[ "name"       ]         = "productsales";
                    request[ "userid"     ]         = "DRIVER1";
                    request[ "data"       ]         = Json::Value();
                    for ( Json::ValueIterator itr = products[ "products" ].begin();
                            itr != products[ "products" ].end();
                            itr++ )
                    {
                        Json::Value    &product         = ( *itr );
                        Json::Value     cartItem;

                        quantity                        = quantity == 1 ? 2 : 1;
                        cartItem                        = product;
                        cartItem[ "quantity"  ]         = quantity;
                        cartItems.append( cartItem );
                    }
                    request[ "data" ][ "cartitems"   ]  = cartItems;
                    request[ "data" ][ "paymenttype" ]  = "cash";
                    requestJson                         = fw.write( request );
                    ungetMessage( requestJson );
                    CsfCmdPrintf( pEnv, "OK\n" );
                }
            }
            else
            {
                CsfCmdPrintf( pEnv, pErrInvalidSubCommand, pCommand, pSubCommand );
                return  1;
            }
            break;

        default:
            CsfCmdPrintf( pEnv, pErrTooManyArguments, pCommand );
            return  1;
        }
        return  0;
    }

    CmdToken_t commands[] =
    {
        /*  Name,           Desc,               Handler,                Help */
        {   "appversion",   appversionDesc,     appversionCommand,      appversionHelp      },
        {   "init",         initDesc,           initCommand,            initHelp            },
        {   "status",       statusDesc,         statusCommand,          statusHelp          },
        {   "card",         cardDesc,           cardCommand,            cardHelp            },
        {   "cd",           cdDesc,             cdCommand,              cdHelp              },
        {   "actionlist",   actionlistDesc,     actionlistCommand,      actionlistHelp      },
        {   "config",       configDesc,         configCommand,          configHelp          },
        {   "time",         timeDesc,           timeCommand,            timeHelp            },
        {   "debug",        debugDesc,          debugCommand,           debugHelp           },
        {   "udsn",         udsnDesc,           udsnCommand,            udsnHelp            },
        {   "sam",          samDesc,            samCommand,             samHelp             },
        {   "ntpcb",        ntpcbDesc,          ntpcbCommand,           ntpcbHelp           },
        {   "test",         testDesc,           testCommand,            testHelp            },
        {   "shift",        shiftDesc,          shiftCommand,           shiftHelp           },
        {   "counter",      counterDesc,        counterCommand,         counterHelp         },
        {   "tmi",          tmiDesc,            tmiCommand,             tmiHelp             },
        {   "product",      productDesc,        productCommand,         productHelp         },
    };
}

#define MSG_WELCOME "\n\nVix Technology Myki Application\n" CSFCOPYRIGHT "\n" BUILDNAME "\n\n"
#define MSG_PROMPT  "Myki-app>"
#define MSG_COMMANDNAME "myki"
#define MSG_COMMANDSECTION "Command"


void cliClose(void)
{
    for ( unsigned int i = 0; i < (sizeof(commands)/sizeof(CmdToken_t)); i++ )
    {   
        CsfCmdUnregister(commands[i].c_name);
    }
    //CsfCmdMacclose();
    CsfCmdServerClose();
}

int cliInitialise(void)
{
    int ret = 0;

    if ( (ret = CsfCmdServer(MSG_COMMANDNAME, MSG_COMMANDSECTION, 0, 0, MSG_WELCOME, MSG_PROMPT)) == 0 )
    {
        for ( unsigned int i = 0; i < (sizeof(commands)/sizeof(CmdToken_t)); i++ )
        {
            CsfCmdRegister(&commands[i]);
        }
    }

    CsfCmdMacAdd(   "1",    "config setDirection entry\n"
                            "card notify\n"
                            "card validatetransitcard\n" );
    CsfCmdMacAdd(   "2",    "config setDirection exit\n"
                            "card notify\n"
                            "card validatetransitcard\n" );
    CsfCmdMacAdd(   "3",    "config setDirection platform\n"
                            "card notify\n"
                            "card validatetransitcard\n" );
    //CsfCmdMacAdd("9","card enquiry\n");

        //  $1  RouteId
        //  $2  StopId
        //  $3  EntryPointId
        //  $3  LocationDataUnavailable (optional, true/false*)
    CsfCmdMacAdd(   "L",    "config setLocation $1 $2 $3 $4\n" );
    
    //CsfCmdMacAdd("tp","timingpoint $1\n");
    CsfCmdMacAdd(   "usp",  "config setServiceProvider $1\n");
    CsfCmdMacAdd(   "qds",  "config enquiry\n");
    CsfCmdMacAdd(   "gtv",  "cd info\n");

    /*  Adds unit-test CLI commands */
    AddUnitTestCommands( );

    return ret;
}
