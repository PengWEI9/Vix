/***************************************************************************
 * Copyright (c) 2014 Vix Technology. All rights reserved
 *
 * Module name   : dc.h
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
 
#ifndef _DC_H
#define _DC_H
 
#include <csfcmd.h>
#include <cs.h>
#include <csf.h>
#include <csthread.h>
#include <csmutex.h>
#include "version.h"
#include <ipc.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <signals.h>    // reader-common signal handling

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <string.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <assert.h>

#include "myki_alarm.h"
#include "myki_alarm_names.h"
#include "myki_avl_ipc.h"
#include "myki_cd.h"
#include "myki_schedule.h"

#include <message_dc.h> 

extern "C"
{
    #include <ccedev.h>
    #include <cceapi.h>
    #include <ccetypes.h>
    #include <udmf.h>
    #include <pil.h>
    #include <cceint.h>
    #include <varop.h>
}

#include "private-libwebsockets.h"

using namespace std;
#include <map>
#include <vector>
#include <queue>
#include <algorithm>
#include <string>
#include <sstream>
#include <iostream>
#include <fstream>
#include <stdexcept>
#include <syslog.h>

#include "libwebsockets.h"
#include "json/json.h"

#include "message_varop.h"

#ifdef LINUX
extern "C" {
#endif

    #include "csgetopt.h"
#ifdef LINUX
} /* extern "C" */
#endif

extern int appQueueId;  
extern int wsQueueId;
extern int asQueueId;
extern int avlTaskId;
extern int printerTaskId;

extern CsMutex_t g_queueLock;
extern CsMutex_t g_rxQueueLock;
extern CsMutex_t g_heartbeatMapLock;
extern CsMutex_t g_LANCommunicationTimerLock;
extern CsMutex_t g_persistLock;
extern CsMutex_t g_stateLock;

extern int g_pollDelayMS;
extern int g_httpPort;
extern int g_InvalidPODTimeout;
extern std::string g_resourcePath;

extern int g_screensaverTimeout;
extern int s_dcStatus;
extern bool s_driverBreakLockAll;
extern int g_autoStartOfTripTimeout;
extern int g_autoEndOfTripTimeout;
extern int g_unrosteredShiftID;
extern int g_unrosteredTripID;
extern int g_headlessShiftID;
extern int g_headlessTripID;
extern int g_shiftTotalsMaskLevel;
extern int g_EPLRecoveryPeriod;     
extern int g_disconnectedWarningTimeout;
extern int g_autoLogoffTimeout; 
extern int g_operationalModeTimeout;
extern bool g_LANCommunicationTimerCheck;
extern uint32_t g_LANCommunicationTimer;
extern int g_LANCommunicationTimeout;
extern int g_manualEndShiftWaitTime;  
extern int g_errorHMIMsgTimeout;
extern int g_successHMIMsgTimeout;      
extern uint32_t g_changeEndsTimer;
extern int g_changeEndsTimeout;
extern time_t  s_passwordLockTimeout;
extern int g_salesCompleteTimeout;
extern int g_currencyRoundingPortion;
extern uint32_t g_driverBreakTimer;
extern int g_driverBreakTimeout;

extern std::map<std::string, time_t> g_heartbeatMap;

extern int s_routeId;
extern int s_stopId;
extern int s_stopZone;                  /**< Current stop zone */
extern int s_overrideStopId;
extern int s_overridePreviousStopId;

extern char                     g_terminalId[CSFINI_MAXVALUE+1]         ;
extern char                     g_terminalType[CSFINI_MAXVALUE+1]       ;
extern char                     g_serialNo[CSFINI_MAXVALUE+1]           ;
extern char                     g_ipAddress[CSFINI_MAXVALUE+1]          ;
extern char                     g_netmask[CSFINI_MAXVALUE+1]            ;
extern char                     g_gateway[CSFINI_MAXVALUE+1]            ;
extern char                     g_mac[CSFINI_MAXVALUE+1]                ;
extern char                     g_priDNS[CSFINI_MAXVALUE+1]             ;
extern char                     g_WLANipAddress[CSFINI_MAXVALUE+1]      ;
extern char                     g_WLANnetmask[CSFINI_MAXVALUE+1]        ;
extern char                     g_WLANgateway[CSFINI_MAXVALUE+1]        ;
extern char                     g_WLANmac[CSFINI_MAXVALUE+1]            ;
extern char                     g_WLANpriDNS[CSFINI_MAXVALUE+1]         ;
extern char                     g_GPRSipAddress[CSFINI_MAXVALUE+1]      ;
extern char                     g_GPRSnetmask[CSFINI_MAXVALUE+1]        ;
extern char                     g_GPRSgateway[CSFINI_MAXVALUE+1]        ;
extern char                     g_GPRSmac[CSFINI_MAXVALUE+1]            ;
extern char                     g_GPRSpriDNS[CSFINI_MAXVALUE+1]         ;
extern char                     g_serviceProvidersFile[CSFINI_MAXVALUE+1];
extern char                     g_rolesFile[CSFINI_MAXVALUE+1]          ;
extern char                     g_stationId[CSFINI_MAXVALUE+1]          ;
extern int                      g_serviceProviderId;
extern char                     g_vehicleId[CSFINI_MAXVALUE+1]          ;

extern char                     g_scheduleDatabase[CSFINI_MAXVALUE+1]   ;
extern char                     g_accessPointName[CSFINI_MAXVALUE+1]   ;

extern bool                     g_autoPrintReceipt;
extern bool                     g_roundingHalfUp;
extern U32_t                    g_minimumReceiptAmount;
extern U32_t                    g_receiptInvoiceThreshold;

extern IPC_3GState_e            g_3GState;
extern IPC_3GSignalQuality_e    g_3GSignalQuality;
extern int                      g_3GSignalQualityValue;

extern bool s_running;

extern Json::Value s_deviceList;
extern Json::Value s_network;
extern Json::Value s_trip;
extern Json::Value s_tablecard;
extern Json::Value g_roles;
extern Json::Value g_serviceProviders;
extern std::map<std::string, Json::Value> s_cardMap;


extern bool s_headless;
extern bool s_criticalFailure;
extern bool s_secondaryCriticalFailure;
extern std::string s_activeTerminalId;
extern std::string s_activeType;
extern Json::Value s_activeRoles;
extern bool s_passwordLogin;
extern bool s_passwordForceLogOff;
extern bool s_passwordLock;
extern int  s_passwordFailureCount;
extern bool g_allowTestPasswords;
extern bool s_sosoWhenDriverNotLoggedIn;

extern std::string s_activeStaffId;
extern std::string s_activeServiceProviderId;
extern std::string s_activeCSN;

/* ShiftEnd message after power off variables */
extern bool g_sendAutoLogOffAtPowerStart;
extern std::string s_logOffTerminalId;
extern std::string s_logOffType;
extern std::string s_logOffStaffId;
extern std::string s_logOffServiceProviderId;

extern char g_companyABN[BUFSIZ];
extern char g_companyName[BUFSIZ];
extern char g_companyPhone[BUFSIZ];
extern char g_companyWebsite[BUFSIZ];
extern char g_salesMessage[BUFSIZ];
extern char g_reversalMessage[BUFSIZ];

extern int s_speed;
extern int s_eps;

typedef struct {
    IPC_header_t    hdr;
    char    data[400];
} IPC_400Bytes_t;

#define MAX_JSONSTRING_SIZE 1024
#define HEARTBEAT_TIMEOUT_SECONDS 20
#define MAX_HOSTNAME_SIZE 64
typedef struct {
    IPC_header_t    hdr;
    char jsonstring[MAX_JSONSTRING_SIZE+1];
	char source[MAX_HOSTNAME_SIZE];
} IPC_CCEEvent_t;

typedef struct {
    IPC_header_t    hdr;
    int id;
} IPC_JSON_t;

#define IPC_HEARTBEAT_REQUEST  87     // send to all processes and threads
#define IPC_HEARTBEAT_RESPONSE 88       

#define IPC_TASK_STARTED 89
#define IPC_TASK_READY 90                 // may be used to pause until others are also ready
#define IPC_TASK_PAUSED 91                // may suspend all threads except for IPC (wiating for run)
#define IPC_TASK_STOPPED 92
#define IPC_TASK_KILLED 93                // SIGKILL PM or signal handler can tell others
#define IPC_TASK_CRASHED 94               // PM or signal handler can tell others

#define IPC_TASK_RUN  95                   // a 'ready' task can proceed
#define IPC_TASK_PAUSE 96
#define IPC_TASK_STOP 97                  // ask
#define IPC_TASK_KILL 98                  // when 'asking' failed, use strong tactics 

#define IPC_WS_REFRESH_EVENT    600
#define IPC_WSP_PROCESS_EVENT   601
#define IPC_WS_RECEIVE_EVENT    602
#define IPC_WS_POLL_EVENT       603
#define IPC_MYKI_ALARM          604
#define IPC_TIMEOUT             605
#define IPC_AS_RECEIVE_EVENT    606
#define IPC_CCE_EVENT           700

// Alarm Configuration
#define INI_ALARMSECTION        "Alarm"
#define INI_ALARMDIRECTORY      INI_ALARMSECTION ":Directory"
#define INIDEF_ALARMDIRECTORY   "/afc/data/alarm/confirmed"
#define INI_ALARMPERSISTENCE    INI_ALARMSECTION ":Persistence"
#define INIDEF_ALARMPERSISTENCE "/afc/data/alarm/current_set"

#define MODE_TOUCHON    1
#define MODE_TOUCHOFF   2
#define MODE_TOUCHONOFF 3

#define DEFAULT_EPL_RECOVERY_PERIOD 20
#define MAX_EPL_RECOVERY_PERIOD 1440
#define MIN_EPL_RECOVERY_PERIOD 0

#define TERMINAL_TYPE_BUS "BDC"

/* list of supported protocols and callbacks */
enum server_protocols {
	/* always first */
	PROTOCOL_HTTP = 0,
    
    SERVER_PROTOCOL_FIRST=1,
    PROTOCOL_EVENT_DC=1,
    PROTOCOL_EVENT_READER,
	PROTOCOL_EVENT_JSON,

	/* always last */
	SERVER_PROTOCOL_COUNT
};

enum DCType
{
	DCTYPE_TRAM = 0,
    DCTYPE_BUS,
};

enum DCState
{
	DCSTATE_UNINITIALISED = 1, // Run-level 1    
    DCSTATE_INITIALISING,  // Run-level 2
    DCSTATE_STANDBY = 3,       // To correspond to run-level 3
    DCSTATE_DRIVER_BREAK,
    DCSTATE_TECHNICIAN,
    DCSTATE_SUPERVISOR,
    DCSTATE_OUT_OF_ORDER,
    DCSTATE_OPERATIONAL,
    DCSTATE_TRAVELLING,
    DCSTATE_CHANGE_ENDS,
    DCSTATE_POWER_OFF,
} ;



enum DeviceOperationalStatus
{
    DEVICE_OPSTATUS_OUT_OF_SERVICE,
    DEVICE_OPSTATUS_IN_SERVICE,
    DEVICE_OPSTATUS_DISCONNECTED,
   // DEVICE_OPSTATUS_OUT_OF_SERVICE_FAULT,
};


enum DCTimeouts
{
    CHANGE_ENDS_TIMEOUT=1,
    DRIVER_BREAK_TIMEOUT,
    STATUS_CHECK_TIMEOUT,
    PASSWORD_LOCK_TIMEOUT,
    LAN_COMM_TIMEOUT,
    PERSIST_TIMEOUT
} ;

#if defined(CSDEBUG) && (CSDEBUG >= 3)
#define IPCSend(q, l, r) do { \
    CsDebug(8, (8, "IPCSend queue %d len %u at line %d file %s", q, r, __LINE__, __FILE__)); \
    if(IpcSend(q, l, r)!=0){ CsErrx("IpcSend failed queue %d Line %d file %s", q, __LINE__, __FILE__);    } } while (0)
#else
#define IPCSend IpcSend
#endif


///////////////////////////////////////////////////////////////////////////////
// DC/State machine

extern int getModeValue(std::string mode);
extern std::string getModeName(int mode);

extern int s_gpsSatellites;
extern double s_longitude;
extern double s_latitude;
extern bool s_overrideEnabled;
extern int s_status;

int         getStopOffset( int stopId );
void persistData( bool bSave );
void persistRouteData( bool bSave );
void* ProcessMessages(void* args);

void processTimeout(IPC_IntPayload_t *p);
void statusCheck();

extern DCType  s_DCType;
extern DCState s_DCState;
Json::Value getPermissions(int profile, int type);
void setGPRSState(int state);
int setState(DCState newState, bool broadcastOnNoChange=false, bool lock=false);
int processStateChange(DCState newState, bool lock);
void sendStateEvent(bool carddetail=false);

void lockPasswordEntry(int duration);
void startPasswordLockTimer();
void stopPasswordLockTimer();

void startLANCommunicationTimer();
void stopLANCommunicationTimer();
void LANCommunicationTimeout(uint32_t timer);

bool validatePIN(std::string terminalId, std::string PIN);
char* GeneratePasswordOfTheDay(char *pPassword, const char *pStaffId, int serviceProviderId, int staffRole);
Json::Value validatePasswordOfTheDay(std::string operatorid, std::string password);

const char* getTypeString(DCType type);
const char* getStateString(DCState state);
const char* get3GStatusString();
DCType getCurrentType();
DCState getCurrentState();
const char* getCurrentTypeString();
const char* getCurrentStateString();
bool setDeviceStatus(const char *logicalNm, u_long deviceStatus, std::string desc, bool updateLog, bool forceNotify);
bool setDeviceOpStatus(const char *logicalNm, u_long operationalStatus, std::string desc, bool updateLog, bool forceNotify);
bool setDeviceReaderMode(const char *logicalNm, u_long modeValue, std::string description, bool updateLog, bool forceNotify);
bool setDeviceReaderESN(const char *logicalNm, u_long esnValue, std::string description, bool updateLog, bool forceNotify);
std::string getDeviceStatusName(u_long status);

extern  "C" void getNetworkSettings();
int getESN();
std::string getSAM();

std::string readCommsStatus();

    /*  Get JSON value helpers */
int         getJsonInt( Json::Value &value, const char *pName );
std::string getJsonString( Json::Value &value, const char *pName );
bool        getJsonBool( Json::Value &value, const char *pName );

///////////////////////////////////////////////////////////////////////////////
// Web sockets
void InitialiseWebsocketProcessingThread();
void InitialiseAsynchronousProcessingThread();
void InitialiseCommsStatusProcessingThread();
int InitialiseWebsocket();
void sendSimpleEvent(const char* event);
int processWebsockets();
void createBaseEvent(Json::Value &request, Json::Value &response);
void sendEvent(std::string message);
void sendDCMessage(std::string message);
void sendReaderMessage(std::string message);
void processAutoLogOff();
void processShiftEnd(bool bNewDriver);
void sendLogOffMessages(bool sendDCMessage, bool sendReaderMessage, const std::string& newTerminalId);

int32_t CsStrTimeISO(const CsTime_t *pTm, int32_t len, char *pBuffer );
int32_t CsStrTimeSimple(const CsTime_t *pTm, int32_t len, char *pBuffer );

///////////////////////////////////////////////////////////////////////////////
// AVL messages
void AVL_SetHeadlessTrip(int32_t newRouteId);
void AVL_SetTrip(int32_t newRouteId, int32_t newDirection, int32_t newShiftId, int32_t newTripNumber, int32_t tripStartTime, int32_t newRunId, int32_t newDepotId, int32_t newStaffId, int32_t newStopStartId, int32_t newStopEndId);

void AVL_ManualEnterStop(int32_t stopId);
void AVL_ManualLeaveStop();
void setHeadlessMode();

///////////////////////////////////////////////////////////////////////////////
// CCE
int initCCE();
CceRv_t sendVarop(const char* logicalName, unsigned long varOp, unsigned long varId, unsigned long varValue);
CceRv_t sendVaropToAll(  unsigned long varOp,  unsigned long varValue);
CceRv_t getStatus(const char* logicalId, CceDcrStatus_t* pStatus);
CceRv_t requestVaropToAll(unsigned long varId);
CceRv_t requestStatusToAll();
std::string findLogicalName(std::string terminalId);

    /**
     *      Sends VO_CURRENT_LOCATION, current location, to FPDms.
     *      @param  routeId     (I) route id.
     *      @param  stopId      (I) stop id.
     *      @param  isAvailable (I) TRUE if location is available; FALSE otherwise.
     *      @return 0 if successful; <0 failed.
     */
int cceSendLocation( int32_t routeId, int32_t stopId, int32_t isAvailable );

#endif // _DC_H
