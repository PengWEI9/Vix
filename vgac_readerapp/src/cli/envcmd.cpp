
#include <json/json.h>
#include <stdlib.h>
#include <string.h>

#include "corebasetypes.h"
#include "cs.h"
#include "message_reader.h"
#include "ipc.h"
#include "Common.h"
#include "envcmd.h"
#include "LDT.h"
#include "timing_point.h"
#include "card_processing_thread.h"
#include "dataxfer.h"

extern "C" {
#include <myki_br.h>
}

/*
 *      External References
 *      -------------------
 */

    /*  GAC_PROCESSING_THREAD.CPP */
extern  char        g_terminalType[ ];


//extern LDTTransactionData_t ldtTransactionData;

bool config_setDirection(int argc, const char *argv[], void *data )
{
    const char* dir=argv[0];
    MYKI_BR_ContextData_t *pContextData = GetCardProcessingThreadContextData();

    if (strcmp(dir, "entry") == 0)
    {
        pContextData->StaticData.isEntry = TRUE;
        pContextData->StaticData.isExit = FALSE;

        return true;
    }
    else if (strcmp(dir, "exit") == 0)
    {
        pContextData->StaticData.isEntry = FALSE;
        pContextData->StaticData.isExit = TRUE;

        return true;
    }
    else if (strcmp(dir, "platform") == 0)
    {
        pContextData->StaticData.isEntry = TRUE;
        pContextData->StaticData.isExit = TRUE;

        return true;
    }
    else if (strcmp(dir, "onboard") == 0)
    {
        pContextData->StaticData.isEntry = FALSE;
        pContextData->StaticData.isExit = FALSE;

        return true;
    }
    else
    {
        return false;
    }
}

char* time_set(int argc, const char *argv[], void *data )
{
    char* ret = reinterpret_cast<char*>(CsMalloc(256));
    const char* t=argv[0];

    MYKI_BR_ContextData_t *pContextData = GetCardProcessingThreadContextData();
    time_t tm;
    time_t brTime = pContextData->DynamicData.currentDateTime;
    struct tm * timeinfo = localtime ( &brTime );

    ret[0] = 0;//no text
/* doco from acs
usage: time <arguments>

<arguments> =
+            Timewarp forward 10 minutes
-            Timewarp back 10 minutes
set          Timewarp set to the specified time (example: time set 18:00:00)
>            Timewarp forward 3 hours
<            Timewarp back 3 hours
>>           Timewarp forward to next day
<<           Timewarp back to previous day
morning      Timewarp to current morning
monday       Timewarp to next monday
get          Display time
wait xx      Sleep xx milliseconds
pause        Wait for keystroke
now          Clear timewarp
show         Show the time
setfull      Timewarp set to specified date time (example: time setfull 2006:05:15:18:00:00)
*/

    if(strcmp(t, "+")==0)
    {
        timeinfo->tm_min  += 10;
        tm=   mktime(timeinfo) ;
        sprintf(ret,"Warping forward 10 minutes (%02d/%02d/%04d %02d:%02d:%02d).\n",
                                            (int)(timeinfo->tm_mday),
                                            (int)(timeinfo->tm_mon + 1),
                                            (int)(timeinfo->tm_year + 1900),
                                            (int)(timeinfo->tm_hour),
                                            (int)(timeinfo->tm_min),
                                            (int)(timeinfo->tm_sec)
                );
    }else if(strcmp(t, "-")==0)
    {
        timeinfo->tm_min  -= 10;
        tm=   mktime(timeinfo) ;
        sprintf(ret,"Warping back 10 minutes (%02d/%02d/%04d %02d:%02d:%02d).\n",
                                            (int)(timeinfo->tm_mday),
                                            (int)(timeinfo->tm_mon + 1),
                                            (int)(timeinfo->tm_year + 1900),
                                            (int)(timeinfo->tm_hour),
                                            (int)(timeinfo->tm_min),
                                            (int)(timeinfo->tm_sec)
                );
    }    
    else if (strcmp(t, ">>") == 0)
    {
        timeinfo->tm_mday++;

        timeinfo->tm_hour  = 6;
        timeinfo->tm_min   = 0;
        timeinfo->tm_sec   = 0;

        tm=   mktime(timeinfo) ;
        sprintf(ret,"Warping forward to next day (%02d/%02d/%04d %02d:%02d:%02d).\n",
                                            (int)(timeinfo->tm_mday),
                                            (int)(timeinfo->tm_mon + 1),
                                            (int)(timeinfo->tm_year + 1900),
                                            (int)(timeinfo->tm_hour),
                                            (int)(timeinfo->tm_min),
                                            (int)(timeinfo->tm_sec)
                );
    }
    else if (strcmp(t, ">") == 0)
    {
        timeinfo->tm_hour  += 3;

        tm=   mktime(timeinfo) ;
        sprintf(ret,"Warping forward 3 hours (%02d/%02d/%04d %02d:%02d:%02d).\n",
                                            (int)(timeinfo->tm_mday),
                                            (int)(timeinfo->tm_mon + 1),
                                            (int)(timeinfo->tm_year + 1900),
                                            (int)(timeinfo->tm_hour),
                                            (int)(timeinfo->tm_min),
                                            (int)(timeinfo->tm_sec)
                );
    }else if (strcmp(t, "<<") == 0)
    {
        timeinfo->tm_mday--;
        timeinfo->tm_hour  = 6;
        timeinfo->tm_min   = 0;
        timeinfo->tm_sec   = 0;


        tm=   mktime(timeinfo) ;
        sprintf(ret,"Warping back to previous day (%02d/%02d/%04d %02d:%02d:%02d).\n",
                                            (int)(timeinfo->tm_mday),
                                            (int)(timeinfo->tm_mon + 1),
                                            (int)(timeinfo->tm_year + 1900),
                                            (int)(timeinfo->tm_hour),
                                            (int)(timeinfo->tm_min),
                                            (int)(timeinfo->tm_sec)
                );
    }
    else if (strcmp(t, "<") == 0)
    {
        timeinfo->tm_hour  -= 3;

        tm=   mktime(timeinfo) ;
        sprintf(ret,"Warping back 3 hours (%02d/%02d/%04d %02d:%02d:%02d).\n",
                                            (int)(timeinfo->tm_mday),
                                            (int)(timeinfo->tm_mon + 1),
                                            (int)(timeinfo->tm_year + 1900),
                                            (int)(timeinfo->tm_hour),
                                            (int)(timeinfo->tm_min),
                                            (int)(timeinfo->tm_sec)
                );
    }
    else if (strcmp(t, "morning") == 0) // Change to 6 am
    {
        timeinfo->tm_hour  = 6;
        timeinfo->tm_min   = 0;
        timeinfo->tm_sec   = 0;

        tm=   mktime(timeinfo) ;
        sprintf(ret,"Warping back to the morning (%02d/%02d/%04d %02d:%02d:%02d).\n",
                                            (int)(timeinfo->tm_mday),
                                            (int)(timeinfo->tm_mon + 1),
                                            (int)(timeinfo->tm_year + 1900),
                                            (int)(timeinfo->tm_hour),
                                            (int)(timeinfo->tm_min),
                                            (int)(timeinfo->tm_sec)
                );
    }    
    else if (strcmp(t, "monday") == 0) // Change to  next monday
    {      
        timeinfo->tm_hour  = 6;
        timeinfo->tm_min   = 0;
        timeinfo->tm_sec   = 0;
       
        if(timeinfo->tm_wday==0)      timeinfo->tm_mday +=1;
//      else  if(timeinfo->tm_wday==1) timeinfo->tm_mday += 0; // Already monday
        else if(timeinfo->tm_wday==2) timeinfo->tm_mday +=5;
        else if(timeinfo->tm_wday==3) timeinfo->tm_mday +=4;
        else if(timeinfo->tm_wday==4) timeinfo->tm_mday +=3;
        else if(timeinfo->tm_wday==5) timeinfo->tm_mday +=3;
        else if(timeinfo->tm_wday==6) timeinfo->tm_mday +=2;
        tm=   mktime(timeinfo) ;
        sprintf(ret,"Warping to next monday (%02d/%02d/%04d %02d:%02d:%02d).\n",
                                            (int)(timeinfo->tm_mday),
                                            (int)(timeinfo->tm_mon + 1),
                                            (int)(timeinfo->tm_year + 1900),
                                            (int)(timeinfo->tm_hour),
                                            (int)(timeinfo->tm_min),
                                            (int)(timeinfo->tm_sec)
                );
    }
    else if (strcmp(t, "now") == 0)
    {
        time_t brTime;   
        time(&brTime);
        timeinfo = localtime ( &brTime );
        tm=   mktime(timeinfo) ;
        sprintf(ret,"We're back baby.\n");
    }        
    else if (strcmp(t, "set") == 0)  // TODO
    {
//> time set
//Warping to the specified time (27/01/2014 9:08:07 AM).
        sscanf(argv[1], "%d:%d:%d", &timeinfo->tm_hour, &timeinfo->tm_min, &timeinfo->tm_sec);
        tm=   mktime(timeinfo) ;
        sprintf(ret,"Warping to the specified time (%02d/%02d/%04d %02d:%02d:%02d).\n",
                                            (int)(timeinfo->tm_mday),
                                            (int)(timeinfo->tm_mon + 1),
                                            (int)(timeinfo->tm_year + 1900),
                                            (int)(timeinfo->tm_hour),
                                            (int)(timeinfo->tm_min),
                                            (int)(timeinfo->tm_sec)
                );
    }   
    else if (strcmp(t, "get") == 0) 
    {
//> time get
//The current date/time is: 27/01/2014 2:39:08 AM.
        sprintf(ret,"The current date/time is: %02d/%02d/%04d %02d:%02d:%02d.\n",
                                            (int)(timeinfo->tm_mday),
                                            (int)(timeinfo->tm_mon + 1),
                                            (int)(timeinfo->tm_year + 1900),
                                            (int)(timeinfo->tm_hour),
                                            (int)(timeinfo->tm_min),
                                            (int)(timeinfo->tm_sec)
                );
    }   
    else if (strcmp(t, "show") == 0) 
    {
        sprintf(ret,"The current time (%02d/%02d/%04d %02d:%02d:%02d).\n",
                                            (int)(timeinfo->tm_mday),
                                            (int)(timeinfo->tm_mon + 1),
                                            (int)(timeinfo->tm_year + 1900),
                                            (int)(timeinfo->tm_hour),
                                            (int)(timeinfo->tm_min),
                                            (int)(timeinfo->tm_sec)
                );
    }    
    else if (strcmp(t, "pause") == 0)  // TODO
    {
        sprintf(ret,"'time pause' not supported.\n");
        CsWarnx(ret);
    }        
    else if (strcmp(t, "setfull") == 0)
    {
        //setfull does not output the time since we just set it 
        tm = getDateTime(argv[1]);
    }
    else{
        sprintf(ret,"'time %s' called (not implemented)\n",t);
        CsWarnx(ret);
        tm = getDateTime(t);
    }

    CsDebug (9, (9, "Set time to %d", tm));

    //ldtTransactionData.currentDateTime = tm;
    pContextData->DynamicData.currentDateTime = tm;  // TODO CHANGE TO CSTIME
    return ret;
}

bool polling_enable()
{
    IPC_IntPayload_t m;

    m.hdr.type   = IPC_CSC_DETECTION_ENABLE;
    m.hdr.source = APP_TASK;
    m.data       = 0;

    if (IpcSend(IpcGetID(APP_TASK), &m, sizeof(m)) == -1)
         return false;

    return true;
}

bool polling_disable()
{
    IPC_IntPayload_t m;

    m.hdr.type   = IPC_CSC_DETECTION_DISABLE;
    m.hdr.source = APP_TASK;
    m.data       = 0;

    if (IpcSend(IpcGetID(APP_TASK), &m, sizeof(m)) == -1)
         return false;

    return true;
}

bool timingpoint_enable()
{
    DAF_TimingPoint_EnableCapture(1);

    return true;
}

bool timingpoint_disable()
{
    DAF_TimingPoint_EnableCapture(0);

    return true;
}

/*==========================================================================*
**
**  config_setLocation
**
**  Description     :
**      Processes "L" command, ie. set location.
**
**  Parameters      :
**      argc                [I]     number of arguments
**      argv                [I]     argument values
**                                  argv[0] = RouteId
**                                  argv[1] = StopId
**                                  argv[2] = EntryPointId (OPT)
**                                  argv[3] = LocationDataUnavailable (OPT)
**      data                [I/O]   user defined data
**
**  Returns         :
**      TRUE                        success
**      FALSE                       otherwise
**
**  Notes           :
**
**==========================================================================*/

bool config_setLocation( int argc, const char *argv[], void *data )
{
    #define configSetLocation_RouteId       0
    #define configSetLocation_StopId        1
    #define configSetLocation_EntryPointId  2
    #define configSetLocation_Unavailable   3

    const char             *rtId            = argv[ configSetLocation_RouteId ];
    const char             *stopId          = argv[ configSetLocation_StopId  ];
    const char             *epId            = argc >= ( configSetLocation_EntryPointId + 1 ) ? argv[ configSetLocation_EntryPointId ] : NULL;
    const char             *locUnav         = argc >= ( configSetLocation_Unavailable  + 1 ) ? argv[ configSetLocation_Unavailable  ] : NULL;
    bool                    ret             = false;
    unsigned long           id              = 0;
    char                    waste           = 0;
    MYKI_BR_ContextData_t  *pContextData    = GetCardProcessingThreadContextData();
    IPC_IntPayload_t        ipcSetLocation;
    int                     nLineId         = pContextData->DynamicData.lineId;
    int                     nStopId         = pContextData->DynamicData.stopId;
    int                     nEntryPointId   = pContextData->DynamicData.entryPointId;
    bool                    locAvailable    = true;
    int                     nResult         = 0;
    int                     AppQueueId      = IpcGetID( APP_TASK );

    CsDebug( 9, ( 9, "Set location to %s, %s, %s, %s",
        ( rtId    == NULL ? "(null)" : rtId    ),
        ( stopId  == NULL ? "(null)" : stopId  ),
        ( epId    == NULL ? "(null)" : epId    ),
        ( locUnav == NULL ? "(null)" : locUnav ) ) );

    memset( &ipcSetLocation, 0, sizeof( ipcSetLocation ) );

#define SETVALUE(INPUT, OUTPUT, MAXVALUE, DESC) \
    if ( INPUT != 0 && INPUT[0] != '-' ) \
    { \
        if ( sscanf(INPUT, "%ld%c", &id, &waste) != 1 || id > (unsigned long)(MAXVALUE) ) \
        { \
            CsDebug(9, (9, "Invalid %s '%s'", DESC, INPUT)); \
        } \
        else \
        { \
            OUTPUT = id; \
            CsDebug(9, (9, "Successfully set %s to '%s', %d", DESC, INPUT, int(OUTPUT))); \
            ret = true; \
        } \
    } \
    else \
    { \
        CsDebug(9, (9, "Default %s is %d", DESC, (int)OUTPUT)); \
    }
    SETVALUE( rtId,   nLineId,       0xffff,     "route ID"       );
    SETVALUE( stopId, nStopId,       0xff,       "stop ID"        );
    SETVALUE( epId,   nEntryPointId, 0x7fffffff, "entry point ID" );
#undef SETVALUE

    if ( ret == true )
    {
        pContextData->DynamicData.entryPointId  = nEntryPointId;

        if ( locUnav != NULL )
        {
            locAvailable   =
                ( 
                    locUnav[ 0 ] == 't' || locUnav[ 0 ] == 'T' ||   /*  [Tt]rue */
                    locUnav[ 0 ] == 'y' || locUnav[ 0 ] == 'Y' ||   /*  [Yy]es */
                    locUnav[ 0 ] == '1'
                )   ? false : true;
        }

        ipcSetLocation.hdr.type     = IPC_SET_LOCATION;
        ipcSetLocation.hdr.source   = APP_TASK;
        ipcSetLocation.data         = (   nLineId               & 0x0000ffff ) |
                                      ( ( nStopId << 16 )       & 0x00ff0000 ) |
                                      ( ( locAvailable != false ? 0x80000000 : 0x00000000 ) );

        if ( ( nResult = IpcSend( AppQueueId, &ipcSetLocation, sizeof( ipcSetLocation ) ) ) != 0 )
        {
            CsErrx( "config_setLocation : IpcSend failed (%d)", nResult );
            ret = false;
        }
    }

    return ret;
}

/*==========================================================================*
**
**  config_setServiceProvider
**
**  Description     :
**      Processes "configure setServiceProvider" command.
**
**  Parameters      :
**      argc                [I]     number of arguments
**      argv                [I]     argument values
**                                  argv[0] = ServiceProviderId
**      data                [I/O]   user defined data
**
**  Returns         :
**      TRUE                        success
**      FALSE                       otherwise
**
**  Notes           :
**
**==========================================================================*/

bool config_setServiceProvider( int argc, const char *argv[], void *data )
{
    const char             *spId            = argv[ 0 ];
    bool                    ret             = false;
    unsigned long           id              = atoi( spId );
    MYKI_BR_ContextData_t  *pContextData    = GetCardProcessingThreadContextData( );

    if ( id == 0 )
    {
        CsDebug( 9, ( 9, "%s is an invalid service provider id\n", spId ) );
        ret = false;
    }
    else
    if ( id != pContextData->StaticData.serviceProviderId )
    {
        /*  Service provider id changed => Reinitialises BR context Static data */
        pContextData->StaticData.serviceProviderId  = id;
        InitialiseBRContextFromCD( pContextData );

        ret = true;
    }   /*  end-of-if */

    return ret;
}

bool config_setDeviceId(int argc, const char *argv[], void *data)
//const char * deviceId)
{
    const char * deviceId= argv[0];
    bool ret;
    unsigned long id = atoi(deviceId);
    MYKI_BR_ContextData_t *pContextData = GetCardProcessingThreadContextData();

    if (id == 0)
    {
        CsDebug(9, (9, "%s is an invalid device id\n", deviceId));
        ret = false;
    }
    else
    {
        pContextData->StaticData.deviceId = id;
        ret = true;
    }

    return ret;
}

/*==========================================================================*
**
**  config_enquiry
**
**  Description     :
**      Processes "config enquiry" command. The returned null terminated
**      string has the following format,
**
**      ** Device Settings **********************
**      Device:
**        Direction          : Platform
**        Device id          : 13364231
**        TransportMode      : BUS
**        ServiceProviderId  : 171
**        ChangeOfMindPeriod : 0
**        HeadlessRouteId    : 0
**
**      Sale:
**        AddValueEnabled    : Yes
**        AddValue           : 100-10000
**        TPurseMaxBalance   : 10000
**
**      Network:
**        Time               : 0: 2015-02-25 10:43:56
**        RouteId/LineId     : 2
**        StopId             : 3
**        EntryPointId       : 18750
**        ZoneId             : 0
**        InnerZone          : 0
**        OuterZone          : 0
**        Location           : Available
**      
**      Company:
**        Name               : Public Transport Victoria
**        Name Short         : PTV
**        Address            : 750 Collins Street Victoria
**        Call Centre Name   : PTV call centre
**        Phone              : 1800 800 007
**        Website            : ptv.vic.gov.au
**        ABN                : 37 509 050 593
**      *****************************************
**
**  Parameters      :
**      None
**
**  Returns         :
**      XXX                         printable configuration data
**
**  Notes           :
**
**==========================================================================*/

char* config_enquiry( int argc, const char *argv[], void *data )
{
    #define RETURN_BUFFER_SIZE      4096

    char                   *ret             = reinterpret_cast<char*>(CsMalloc(RETURN_BUFFER_SIZE)); // TODO: Review use of malloc here
    char                    linebuf[ 255 ];

    if ( argc == 0 )
    {
        char                    transportMode[ 64 ];    // This should be a constant defined in the MYKI_CD API
        MYKI_BR_ContextData_t  *pContextData    = GetCardProcessingThreadContextData();
        struct tm               tmbuf;
        time_t                  currentTime     = pContextData->DynamicData.currentDateTime;

        if(currentTime==0) // Display current time if override has not been set
        {
            CsTime_t enquiryTime;
            CsTime( &enquiryTime );
            
            currentTime       = enquiryTime.tm_secs;
        }
        localtime_r( &currentTime, &tmbuf );

        sprintf(ret,"** Device Settings **********************\nDevice:\n");

        if ( ! MYKI_CD_getServiceProviderTransportMode( (U16_t)pContextData->StaticData.serviceProviderId, transportMode, sizeof( transportMode ) ) )
        {
            CsErrx( "myki_br_cd_GetTransportModeForProvider() MYKI_CD_getServiceProviderTransportMode() failed" );                                                  
            sprintf(transportMode, "Error");
            //return TRANSPORT_MODE_UNKNOWN;                                                                                                                          
        }      

        sprintf(linebuf, "  Direction          : %s\n"
                         "  Device id          : %u\n"
                         "  TransportMode      : %s\n"
                         "  ServiceProviderId  : %d\n"
                         "  ChangeOfMindPeriod : %d\n"
                         "  HeadlessRouteId    : %d\n"
            , (pContextData->StaticData.isEntry == TRUE ? (pContextData->StaticData.isExit == TRUE ? "Platform" : "entry") : (pContextData->StaticData.isExit == TRUE ? "exit" : "on-board"))
            , (unsigned int)(pContextData->StaticData.deviceId)
            , transportMode
            , (unsigned int)(pContextData->StaticData.serviceProviderId)
            , pContextData->Tariff.changeOfMindPeriod
            , pContextData->Tariff.headlessRouteId
        );
        strcat(ret,linebuf);

        if ( strcmp( g_terminalType, "BDC" ) == 0 )
        {
            strcat( ret, "\nSale:\n" );
            sprintf( linebuf,
                         "  AddValueEnabled    : %s\n"
                         "  AddValue           : %d-%d\n"
                         "  TPurseMaxBalance   : %d\n",
                    pContextData->Tariff.addValueEnabled ? "Yes" : "No",
                    pContextData->Tariff.minimumAddValue, pContextData->Tariff.maximumAddValue,
                    pContextData->Tariff.TPurseMaximumBalance
            );
            strcat( ret, linebuf );
        }

        strcat(ret,"\nNetwork:\n");
        sprintf(linebuf, "  Time               : %u: %04d-%02d-%02d %02d:%02d:%02d\n"
                         "  RouteId/LineId     : %u\n"
                         "  StopId             : %u\n"
                         "  EntryPointId       : %u\n"
                         "  ZoneId             : %u\n"
                         "  InnerZone          : %u\n"
                         "  OuterZone          : %u\n"
                         "  Location           : %s\n",
            (unsigned int)(pContextData->DynamicData.currentDateTime),
            (int)(tmbuf.tm_year + 1900),
            (int)(tmbuf.tm_mon + 1),
            (int)(tmbuf.tm_mday),
            (int)(tmbuf.tm_hour),
            (int)(tmbuf.tm_min),
            (int)(tmbuf.tm_sec),	
            (unsigned int)(pContextData->DynamicData.lineId),
            (unsigned int)(pContextData->DynamicData.stopId),
            (unsigned int)(pContextData->DynamicData.entryPointId),
            (unsigned int)(pContextData->DynamicData.currentZone),
            (unsigned int)(pContextData->DynamicData.currentInnerZone),
            (unsigned int)(pContextData->DynamicData.currentOuterZone),
            ( pContextData->DynamicData.locationDataUnavailable != 0 ? "Unavailable" : "Available" )
        );
        strcat(ret,linebuf);

        strcat(ret,"*****************************************\n");
    }
    else
    {
        int     index   = 0;

        memset( ret, 0, RETURN_BUFFER_SIZE );
        while ( index < argc )
        {
            MYKI_CD_DeviceParameter_t   cdDeviceParameter;

            memset( &cdDeviceParameter, 0, sizeof( cdDeviceParameter ) );
            MYKI_CD_getDeviceParameterStructure( argv[ index ], &cdDeviceParameter );
            CsSnprintf( linebuf, sizeof( linebuf ), "%s='%s'\n", argv[ index ],
                    cdDeviceParameter.value[ 0 ] != 0 ? cdDeviceParameter.value : cdDeviceParameter.default_value );
            strncat( ret, linebuf, RETURN_BUFFER_SIZE - strlen( ret ) - 1 );
            index++;
        }
    }
    return ret;
}

    /**
     *  @brief  Processes "tmi createOpLog" CLI command.
     *  This function formats and 'sends' a WebService message
     *  to reader application to check and perform (if required)
     *  operational period commit.
     *  @return true if successful; false otherwise.
     */
bool tmi_createOpLog()
{
    Json::Value                 request;
    Json::FastWriter            fw;
    MYKI_BR_ContextData_t      *pData               = GetCardProcessingThreadContextData( );
    char                        TerminalId[ 32 ]    = { 0 };
    std::string                 requestJson;

    CsSnprintf( TerminalId, sizeof( TerminalId ), "%d", pData->StaticData.deviceId );

    request[ "terminalid" ] = TerminalId;
    request[ "name"       ] = "periodcommitcheck";
    requestJson             = fw.write( request );
    ungetMessage( requestJson );

    return true;
}
