/***************************************************************************
 * Copyright (c) 2014 Vix Technology. All rights reserved
 *
 * Module name   : dc.cpp
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
 
#include "dc.h"


#if defined(COBRA) || defined(HOST_LINUX_POWERPC)
    #define DRV_FEATURE_FRAM
    #include <serpent/drv_fram.h>
    #include <sys/ioctl.h>
    #include <netinet/in.h>
    #include <netdb.h>
    #include <net/if.h>    
#endif 

CsMutex_t g_persistLock;
CsMutex_t g_stateLock;
CsMutex_t g_LANCommunicationTimerLock;
uint32_t g_LANCommunicationTimer = 0;
bool g_LANCommunicationTimerCheck = false;
bool g_sendAutoLogOffAtPowerStart = false;
int s_routeId=0;
int s_stopId=0;
int s_stopZone = 0;
int s_overrideStopId=0;
int s_overridePreviousStopId=0;     /*  DEPRECATED! */
int s_status=0;
int s_passwordFailureCount = 0;
bool s_driverBreakLockAll = false;
time_t s_passwordLockTimeout = 0; 
bool s_criticalFailure = false;
bool s_secondaryCriticalFailure = false;
int s_dcStatus = 0;
int g_serviceProviderId = 1001; // TODO Configuration

int g_autoStartOfTripTimeout = 60;
int g_autoEndOfTripTimeout = 60;  
int g_EPLRecoveryPeriod = 20;     
int g_disconnectedWarningTimeout = 30;
int g_LANCommunicationTimeout = 300;
int g_manualEndShiftWaitTime = 30;
int g_errorHMIMsgTimeout = 1000;
int g_successHMIMsgTimeout = 500;
int g_autoLogoffTimeout = 3600;
int g_operationalModeTimeout = 300;
int g_salesCompleteTimeout = 1000;
int g_currencyRoundingPortion = 5;
int g_unrosteredShiftID = 9999;
int g_unrosteredTripID = 9999;

int g_headlessShiftID = 9999;
int g_headlessTripID = 9999;

int g_shiftTotalsMaskLevel = (int)MASK_NONE;

int s_gpsSatellites=0;
double s_longitude=0;
double s_latitude=0;
int    s_speed=0;  // Shown in meters per hour
int    s_eps=0;  // Shown in meters per hour
bool s_overrideEnabled = false;

// populate with default values - to be overriden in loadMykiCdValues()
char g_companyABN[BUFSIZ]      = "37 509 050 593";
char g_companyName[BUFSIZ]     = "Public Transport Victoria";
char g_companyPhone[BUFSIZ]    = "1800 800 007";
char g_companyWebsite[BUFSIZ]  = "ptv.vic.gov.au";
char g_salesMessage[BUFSIZ]    = "";
char g_reversalMessage[BUFSIZ] = "";

U32_t g_receiptInvoiceThreshold = 0;
U32_t g_minimumReceiptAmount    = 0;
bool s_headless=false;

Json::Value s_persistedStatus      = Json::Value();
Json::Value s_persistedRouteStatus = Json::Value();

std::string s_activeTerminalId       = "";
std::string s_activeType             = "";
Json::Value s_activeRoles            = "";
std::string s_activeStaffId          = "";
std::string s_activeServiceProviderId= "";
std::string s_activeCSN              = "";
bool     s_passwordLogin             = false;
bool     s_passwordLock              = false;
bool     s_sosoWhenDriverNotLoggedIn = false;
uint32_t g_passwordLockTimer         = 0;
bool     g_allowTestPasswords        = false;
bool     g_roundingHalfUp            = true;

uint32_t g_changeEndsTimer   = 0;
int      g_changeEndsTimeout = 300;

uint32_t g_driverBreakTimer   = 0;
int      g_driverBreakTimeout = (60*15); // 15 minutes

uint32_t g_statusCheckTimer     =  0;
int      g_statusCheckTimeoutMS = 5000;

uint32_t g_persistTimer         =  0;
int      g_persistTimeoutSecs   = 120;

std::string s_logOffTerminalId        = "";
std::string s_logOffType              = "";
std::string s_logOffStaffId           = "";
std::string s_logOffServiceProviderId = "";
std::map<std::string, Json::Value> s_cardMap;

void StatusCheckTimeout(uint32_t timer);
void PersistTimeout(uint32_t timer);

/**
 * @brief Initializes the Myki CD module by calling the function
 * MYKI_CD_setDeviceParameters.
 *
 * @return 0 on success, -1 on failure.
 */
static int initMykiCd()
{
    char transportModeStr[BUFSIZ];
    MYKI_CD_Mode_t transportMode = MYKI_CD_MODE_UNKNOWN;
    unsigned int serviceProviderId = 0;

    char serviceProviderStr[CSFINI_MAXVALUE];

    if (CsfIniExpand("General:ServiceProviderID", serviceProviderStr, CSFINI_MAXVALUE) != 0)
    {
        CsErrx("initMykiCd: CsfIniExpand failed to get General:ServiceProviderID");
        return -1;
    }

    if (sscanf(serviceProviderStr, "%u", &serviceProviderId) != 1)
    {
        CsErrx("initMykiCd: failed to convert Service Provider ID to int");
        return -1;
    }

    CsDbg(4, "initMykiCd: serviceProviderId: %u", serviceProviderId);

    if (!MYKI_CD_getServiceProviderTransportMode(serviceProviderId,
                transportModeStr, BUFSIZ))
    {
        CsErrx("initMykiCd: MYKI_CD_getServiceProviderTransportMode() failed");
        return -1;
    }

    CsDbg(4, "initMykiCd: transportMode: %s", transportModeStr);

    // convert transport-mode string to ENUM type
    if (strcmp(transportModeStr, "RAIL") == 0)
    {
        transportMode = MYKI_CD_MODE_RAIL;
    }
    else if (strcmp(transportModeStr, "BUS") == 0)
    {
        transportMode = MYKI_CD_MODE_BUS;
    }
    else if (strcmp(transportModeStr, "TRAM") == 0)
    {
        transportMode = MYKI_CD_MODE_TRAM;
    }
    else 
    {
        CsErrx("initMykiCd: Invalid transport mode: %s", transportModeStr);
        return -1;
    }

    char terminalType[CSFINI_MAXVALUE];

    if (CsfIniExpand("General:TerminalType", terminalType, CSFINI_MAXVALUE) != 0)
    {
        CsErrx("initMykiCd: CsfIniExpand failed to get General:TerminalType.");
        return -1;
    }

    CsDbg(4, "initMykiCd: terminalType: %s", terminalType);

    MYKI_CD_setDeviceParameters(terminalType, transportMode, 0, 0, 0, 0, 0, 0 );

    return 0;
}

/**
 * @brief Loads commonly used Myki CD values into global variables.
 */
static void loadMykiCdValues()
{
    if (!MYKI_CD_getCompanyABN(g_companyABN, BUFSIZ - 1))
    {
        CsWarn("loadMykiCdValues: MYKI_CD_getCompanyABN() failed.");
    }

    if (!MYKI_CD_getCompanyName(g_companyName, BUFSIZ - 1))
    {
        CsWarn("loadMykiCdValues: MYKI_CD_getCompanyName() failed.");
    }

    if (!MYKI_CD_getCompanyWebSite(g_companyWebsite, BUFSIZ - 1))
    {
        CsWarn("loadMykiCdValues: MYKI_CD_getCompanyWebsite() failed.");
    }

    if (!MYKI_CD_getCompanyPhone(g_companyPhone, BUFSIZ - 1))
    {
        CsWarn("loadMykiCdValues: MYKI_CD_getCompanyPhone() failed.");
    }

    if (!MYKI_CD_getSalesMessage(g_salesMessage, BUFSIZ - 1))
    {
        CsWarn("loadMykiCdValues: MYKI_CD_getSalesMessage() failed");
    }

    if (!MYKI_CD_getReversalMessage(g_reversalMessage, BUFSIZ - 1))
    {
        CsWarn("loadMykiCdValues: MYKI_CD_getReversalMessage() failed");
    }

    if (!MYKI_CD_getRecieptInvoiceMessageThreshold(&g_receiptInvoiceThreshold))
    {
        CsWarn("loadMykiCdValues: MYKI_CD_getRecieptInvoiceMessageThreshold() failed");
    }    
    
    if (!MYKI_CD_getMinimumReceiptAmount(&g_minimumReceiptAmount))
    {
        CsErrx(": MYKI_CD_getMinimumReceiptAmount() failed.");
    }
    
    CsDbg(4, "loadMykiCdValues: g_companyABN: %s", g_companyABN);
    CsDbg(4, "loadMykiCdValues: g_companyName: %s", g_companyName);
    CsDbg(4, "loadMykiCdValues: g_companyWebsite: %s", g_companyWebsite);
    CsDbg(4, "loadMykiCdValues: g_companyPhone: %s", g_companyPhone);
    CsDbg(4, "loadMykiCdValues: g_salesMessage: %s", g_salesMessage);
    CsDbg(4, "loadMykiCdValues: g_reversalMessage: %s", g_reversalMessage);
    CsDbg(4, "loadMykiCdValues: g_receiptInvoiceThreshold: %d", g_receiptInvoiceThreshold);
}

const char* getTypeString(DCType type)
{
    switch(type)
    {
        case DCTYPE_TRAM: return "Tram";
        case DCTYPE_BUS:  return "Bus";
    }
    
    return "Unknown";
}

const char* get3GSignalQualityString(int value)
{
    switch(value)
    {
        case IPC_3G_SIGNAL_QUALITY_UNKNOWN:         return "Unknown";
        case IPC_3G_SIGNAL_QUALITY_MARGINAL:        return "Marginal";
        case IPC_3G_SIGNAL_QUALITY_OK:              return "OK";
        case IPC_3G_SIGNAL_QUALITY_GOOD:            return "Good";
        case IPC_3G_SIGNAL_QUALITY_EXCELLENT:       return "Excellent";
    }
           
    return "Unknown";
}

const char* get3GStatusString()
{
    switch(g_3GState)
    {
        case IPC_3G_UNINITIALISED:    return "Uninitialised";
        case IPC_3G_DISCONNECTED:     return "Disconnected";
        case IPC_3G_CONNECTING:       return "Connecting";
        case IPC_3G_CONNECTED:        return "Connected";
        case IPC_3G_ERROR:            return "Error";
    }
           
    return "Unknown";
}

const char* getStateString(DCState state)
{
    switch(state)
    {
        case DCSTATE_UNINITIALISED:  return "Uninitialised";
        case DCSTATE_POWER_OFF:      return "Power Off";
        case DCSTATE_INITIALISING:   return "Initialising";
        case DCSTATE_STANDBY:        return "Standby";
        case DCSTATE_DRIVER_BREAK:   return "Driver Break";
        case DCSTATE_TECHNICIAN:     return "Technician";
        case DCSTATE_SUPERVISOR:     return "Supervisor";
        case DCSTATE_OUT_OF_ORDER:   return "Out of Order";
        case DCSTATE_OPERATIONAL:    return "Operational";
        case DCSTATE_TRAVELLING:     return "Travelling";
        case DCSTATE_CHANGE_ENDS:    return "Change Ends";
    }
    
    return "Unknown";
}

    /**
     *  @brief  Returns index of stop within the current route given stop id.
     *  @param  stopId stop id [1..N], -1 to return first stop id.
     *  @return stop index [1..N] or 0 if not on current route.
     */
int
getStopOffset( int stopId )
{
    int         stopIndex       = 0;
    int         firstStopId     = s_trip[ "first_stop" ].empty() ? AVL_STOP_UNKNOWN : getJsonInt( s_trip[ "first_stop" ], "id" );
    int         lastStopId      = s_trip[ "last_stop"  ].empty() ? AVL_STOP_UNKNOWN : getJsonInt( s_trip[ "last_stop"  ], "id" );

    if ( stopId < 0 )
    {
        return  firstStopId;
    }

//  CsDbg( 1, "getStopOffset : firstStopId=%d, lastStopId=%d, stopId=%d", firstStopId, lastStopId, stopId );

    if
    (
        ( firstStopId != AVL_STOP_UNKNOWN && firstStopId > stopId ) ||
        ( lastStopId  != AVL_STOP_UNKNOWN && lastStopId  < stopId )
    )
    {
        /*  Specified stop is not on current route */
        return  0;
    }

    if ( firstStopId == AVL_STOP_UNKNOWN )
    {
        stopIndex   = stopId;
    }
    else
    if ( stopId >= firstStopId )
    {
        stopIndex   = ( stopId - firstStopId + 1 );
    }

//  CsDbg( 1, "getStopOffset : stopIndex=%d", stopIndex );
    return  stopIndex;
}

DCState getState(std::string state)
{
    if(state.compare("Uninitialised")==0) return DCSTATE_UNINITIALISED; 
    if(state.compare("Power Off")==0)     return DCSTATE_POWER_OFF;     
    if(state.compare("Initialising")==0)  return DCSTATE_INITIALISING;  
    if(state.compare("Standby")==0)       return DCSTATE_STANDBY;       
    if(state.compare("Driver Break")==0)  return DCSTATE_DRIVER_BREAK;  
    if(state.compare("Technician")==0)    return DCSTATE_TECHNICIAN;    
    if(state.compare("Supervisor")==0)    return DCSTATE_SUPERVISOR;    
    if(state.compare("Out of Order")==0)  return DCSTATE_OUT_OF_ORDER;  
    if(state.compare("Operational")==0)   return DCSTATE_OPERATIONAL;   
    if(state.compare("Travelling")==0)    return DCSTATE_TRAVELLING;    
    if(state.compare("Change Ends")==0)   return DCSTATE_CHANGE_ENDS;   

    return DCSTATE_UNINITIALISED;
}

DCState getCurrentState()
{
    return s_DCState;
}

const char* getCurrentStateString()
{
    return getStateString(s_DCState);
}

DCType getCurrentType()
{
    return s_DCType;
}

const char* getCurrentTypeString()
{
    return getTypeString(s_DCType);
}

bool validatePIN(std::string terminalId, std::string PIN)
{
    if(s_cardMap.find(terminalId)==s_cardMap.end())
        return false;

    Json::Value card = s_cardMap[terminalId];
    if(card.size()<=0)  
    {
        return false;
    }
#if 0 // HTTP POST MODE    
    else
    {
        return card["data"]["valid"].asBool() && PIN.compare(card["data"]["pin"].asString())==0;
    }
#else       
    if(card["data"]["valid"].asBool()==false)
        return false;
    
    Json::Value validateMessage;
    
	CsTime_t t;
    CsTime(&t);
	char timeBuf[64];
	CsStrTimeISO(&t,64,timeBuf);
    
    validateMessage["terminalid"]  = terminalId;
    validateMessage["name"]        = "validatepin";
    validateMessage["type"]        = "get";
    validateMessage["timestamp"]   = timeBuf;
    validateMessage["data"]["pin"] = PIN;

    Json::FastWriter fw;
    sendReaderMessage(fw.write(validateMessage));
    
    return true;
#endif
}

Json::Value getPermissions(int profile, int type)
{
    for(Json::Value::ArrayIndex i=0;i<g_roles.size();i++)
    {
        if(g_roles[i]["type"].empty()==false && g_roles[i]["profile"].empty()==false)
        {
            if(g_roles[i]["type"].asInt()==type && g_roles[i]["profile"].asInt()==profile) // Roles file contains multiples of the same type for each profile.
            {
                if(g_roles[i]["permissions"].empty()==false)
                {             
                    return g_roles[i]["permissions"];
                }
            }  
        }
    }        
    return Json::Value();
}


Json::Value validatePasswordOfTheDay(std::string operatorid, std::string potd)
{
    int             staffRole           = 0;
    char            passwdOfTheDay[ 16 ];
    
    if(s_passwordLock)
    {
        CsDebug(1, (1, "ValidatePasswordOfTheDay password lock in place, password attempt rejected"));
        s_passwordForceLogOff = false;
    }
    else
    {
        for(Json::Value::ArrayIndex i=0;i<g_roles.size();i++)
        {
            if(g_roles[i]["type"].asInt()!=staffRole) // Roles file contains multiples of the same type for each profile.
            {
                staffRole = g_roles[i]["type"].asInt();
                
                // ASSUMPTION: Driver will need to request a new potd after midnight.  Transport day does not apply.
                if ( GeneratePasswordOfTheDay( passwdOfTheDay, operatorid.c_str(), g_serviceProviderId, staffRole ) != NULL )
                {
                    // GeneratePasswordOfTheDay is always in upper case
                    std::transform(potd.begin(), potd.end(), potd.begin(), ::toupper);
          
                    if(potd.compare(passwdOfTheDay)==0  ||
                        (g_allowTestPasswords &&
                        ((potd.compare("D")==0 && g_roles[i]["driver"].empty()==false)
                        || (potd.compare("S")==0 && g_roles[i]["supervisor"].empty()==false)
                        || (potd.compare("W")==0 && g_roles[i]["supervisor"].empty()==false) // W is easier to enter than S
                        || (potd.compare("T")==0 && g_roles[i]["technician"].empty()==false)))
                        )
                    {
                        Json::FastWriter fw;
                        CsDebug(1, (1, "POTD Role: %s ", fw.write(g_roles[i]).c_str()));
                        s_passwordFailureCount = 0;

                        return g_roles[i];
                    }
                }
            }        
        }
        
        s_passwordFailureCount++;
        persistData(true);
    }
    return Json::Value();
}

/*==========================================================================*
**
**  processRefreshEvent
**
**  Description     :
**      Sends a refresh command to the dc browser to reload the html5 content
**
**==========================================================================*/
void processRefreshEvent(IPC_NoPayload_t * m)
{
    sendSimpleEvent("refresh");
}

/*==========================================================================*
**
**  processAVLNotifyEvent
**
**  Description     :
**      Processes IPC_AVL_NOTIFY, AVL notification, message.
**
**  Parameters      :
**      p               [I]     IPC_AVL_NOTIFY message payload
**
**  Returns         :
**      None
**
**  Notes           :
**
**
**==========================================================================*/
const long ms_to_seconds = 3600000;
CsTime_t startTime;
CsTime_t endTime;
bool canStartTimer = false;              
                  
void processAVLNotifyEvent( IPC_AVL_Notify_t* p )
{
    if ( p == NULL )
    {
        CsErrx( "processAVLNotifyEvent : NULL message payload" );
        return;
    }

    // Clear transaction list on stop change
    /*  ANT:    NO LONGER APPLICABLE!
    if((s_overrideEnabled && s_overridePreviousStopId != p->stopId)
            || (!s_overrideEnabled && p->stopId != s_stopId))
    {
        TxnList::clearTransactions();
    }
    */

    CsDebug( 4, ( 4, "processAVLNotifyEvent status %d satsInView %d, satsUsed %d",
            p->status, p->satsInView, p->satsUsed ) );
    CsDebug( 4, ( 4, "processAVLNotifyEvent lat %d, long %d",
            p->latitude, p->longitude ) );
    CsDebug( 4, ( 4, "processAVLNotifyEvent routeId %d, stopId %d/%d, zone %d, %s(%d)",
            p->routeId, p->stopId, p->stopIndex, p->zone,
            p->status == AVL_LOCATION_UNAVAILABLE ? "AVL_LOCATION_UNAVAILABLE" :
            p->status == AVL_LOCATION_STOP_MANUAL ? "AVL_LOCATION_STOP_MANUAL" :
            p->status == AVL_LOCATION_STOP_INSIDE ? "AVL_LOCATION_STOP_INSIDE" :
                                                    "AVL_LOCATION_STOP_OUTSIDE",
            p->status) );

    s_gpsSatellites = p->satsUsed;
    
    if(s_gpsSatellites==0)
    {
        s_latitude  = 0;
        s_longitude = 0;
        s_speed     = 0;  
        s_eps       = 0;  
    }
    else
    {
        s_latitude  = p->latitude;  
        s_longitude = p->longitude;
        s_speed     = p->speed;
        s_eps       = p->eps;
    }
    
    char buf[32];
    
    Json::Value ev;
    ev["name"] = "gps";
    ev["data"]["status"] = p->status;
    snprintf(buf, 32, "%0.7f", (((double)s_latitude)/ms_to_seconds));  
    ev["data"]["latitude"] = buf;
    snprintf(buf, 32, "%0.7f", (((double)s_longitude)/ms_to_seconds)); 
    ev["data"]["longitude"] = buf;
    ev["data"]["satsUsed"]   = p->satsUsed;
    ev["data"]["satsInView"] = p->satsInView;
    
    snprintf(buf, 32, "%0.3f", (((double)s_speed)/1000.0)); 
    ev["data"]["speed"]      = buf;
    snprintf(buf, 32, "%0.3f", (((double)s_eps)/1000.0)); 
    ev["data"]["eps"]      = buf;

    ev["data"]["route"]      = p->routeId;
    ev["data"]["stop"]       = p->stopId;
    ev["data"]["headless"]   = s_headless ? "true":"false";
    
    if ( s_gpsSatellites >= 5 )       // GPS State == GOOD
    {
        (void)MYKI_ALARM_sendSimpleAlarm(MYKI_ALARM_LAN_COMMUNICATION_STATE, 30, MYKI_ALARM_ALERT_CLEARED);   
        ev["data"]["health"] = "OK";
        
        if(canStartTimer)
        {
          canStartTimer = false;                  
        }       
    }
    else if ( s_gpsSatellites >= 3 )  // GPS State == WARNING
    {
        (void)MYKI_ALARM_sendSimpleAlarm(MYKI_ALARM_LAN_COMMUNICATION_STATE, 30, MYKI_ALARM_ALERT_CLEARED);   
        ev["data"]["health"] = "WARNING";
        
        if(canStartTimer)
        {
          canStartTimer = false;                 
        }
    }
    else                              // GPS State == ERROR
    {
        ev["data"]["health"] = "ERROR"; 

        if(!canStartTimer)
        {
          canStartTimer = true;
          CsTime(&startTime);
        }
    }
    
    if(canStartTimer)
    {
        CsTime(&endTime);
        CsTime_t diffTime = CsTimeDiff(&endTime, &startTime);
        
        int millisecs = 0; 
        millisecs = diffTime.tm_secs * 1000 + diffTime.tm_msecs;
        
        if(millisecs > 10000)
        {
          MYKI_ALARM_AdditionalInfo_Values_t  additionalInfo;
          if ( MYKI_ALARM_Init_AdditionalInfo( &additionalInfo ) )
            MYKI_ALARM_SetDescription( &additionalInfo, MYKI_ALARM_PROBLEM, "No GPS signal for more than 10s." ); 
                
          (void)MYKI_ALARM_sendSimpleAlarm2(MYKI_ALARM_LAN_COMMUNICATION_STATE, 30, MYKI_ALARM_ALERT_MINOR, &additionalInfo);      
        }
    }    
    
    Json::FastWriter fw;
    sendDCMessage(fw.write(ev));
    
    // If the new stop is valid and not the stop during the override, then override.
    if ( p->status == AVL_LOCATION_STOP_MANUAL )
    {
        bool    persistStop = false;
        bool    persist     = false;

        if ( s_routeId != p->routeId ) // Review if stop updates should apply
        {
            CsWarnx( "processAVLNotifyEvent : mismatched route (%d/%d)", s_routeId, p->routeId );
            persist         = true;
        }
        if ( s_overrideStopId != p->stopId )
        {
            persist         = true;
        }
        s_overrideEnabled   = true;
        if ( s_overridePreviousStopId == AVL_STOP_UNKNOWN )
        {
            s_overridePreviousStopId    = p->stopId;
        }
        s_overrideStopId                = p->stopId;

        if ( persist     ) persistData( true );
        if ( persistStop ) persistRouteData( true );          
    }
    else if ( s_overrideEnabled && p->stopId != AVL_STOP_UNKNOWN )
    {
        CsDebug( 2, ( 2, "processAVLNotifyEvent : Cancel override new stop %d override stop %d override previous stop %d",
                p->stopId, s_overrideStopId, s_overridePreviousStopId ) ); 

        // cancel the override
        s_overrideEnabled           = false;
        s_overridePreviousStopId    = 0;
        s_overrideStopId            = 0;
    }
    
    if ( s_overrideEnabled == false )
    {
        bool    persist     = false;
        bool    persistStop = false;

        if ( s_routeId != p->routeId ) // Review if stop updates should apply
        {
            CsWarnx( "processAVLNotifyEvent : mismatched route (%d/%d)", s_routeId, p->routeId );
            persist         = true;
        }
        
        if ( persist || s_stopId != p->stopId )
        {
            persistStop     = true;
        }

        //  Send location to readers
        s_routeId   = p->routeId;
        s_stopId    = p->stopId;
        s_stopZone  = p->zone;
        s_status    = p->status;
        
//      if ( persist     ) persistData( true );
//      if ( persistStop ) persistRouteData( true );
        
        // Send stop change to websocket
        cceSendLocation( s_routeId, s_stopId, s_status == AVL_LOCATION_UNAVAILABLE ? FALSE : TRUE );
        
        Json::Value stopMessage;
        stopMessage["name"]                                 = "stop";
        stopMessage["data"]["route_id"]                     = s_routeId;
        stopMessage["data"]["stop_id" /* "stop_index" */]   = p->stopIndex;
        stopMessage["data"]["override"]                     = false;
        stopMessage["data"]["status"]                       = s_status == AVL_LOCATION_UNAVAILABLE ? 0 : 1;
        sendDCMessage( fw.write( stopMessage ) );

        if ( persist     ) persistData( true );
        if ( persistStop ) persistRouteData( true );
    }   
    else
    {
        s_stopZone  = p->zone;

        // REVIEW this else branch, may no longer be required if AVL responds immediately
        cceSendLocation( s_routeId, s_overrideStopId, TRUE );
        
        // Not sure if this needs to be sent.
        Json::Value stopMessage;
        stopMessage["name"]                                 = "stop";
        stopMessage["data"]["route_id"]                     = s_routeId;
        stopMessage["data"]["stop_id" /* "stop_index" */]   = p->stopIndex;
        stopMessage["data"]["override"]                     = true;
        stopMessage["data"]["status"]                       = 1;
        
        sendDCMessage( fw.write( stopMessage ) );
    }    
}   /*  processAVLNotifyEvent( ) */


std::string getOpStatusName(u_long status)
{
    switch(status)
    {
    case DEVICE_OPSTATUS_OUT_OF_SERVICE:
        return "Out of Service";
        break;
    case DEVICE_OPSTATUS_IN_SERVICE:
        return "In Service";
        break;
    case DEVICE_OPSTATUS_DISCONNECTED:
        return "Disconnected";
        break;
 //   case DEVICE_OPSTATUS_OUT_OF_ORDER: // TODO Add extra states
   //     return "Out of Order";
     //   break;
        
    }
    return "Unknown";
}

std::string getDeviceStatusName(u_long status)
{
    switch(status)
    {
    case DEVICE_STATUS_OFFLINE:
        return "Offline";
    case DEVICE_STATUS_ERROR:
        return "Error";
    case DEVICE_STATUS_WARNING:
        return "Warning";
    case DEVICE_STATUS_GOOD:
        return "Good";
    }
    return "Unknown";
}

/*==========================================================================*
**
**  safeJsonValueAsString
**
**  Description     :
**      Converts a Json String or Numeric value to a std::string.
**
**  Parameters      :
**      value         [I]     value to attept to convert to a string.
**      wasSet        [O]     (optional) pointer to boolean which will be set
**                              to true if return string was set from value.
**
**  Returns         :
**      string representation of the value if it is a string or numeric.
**      an empty string ("") otherwise.
**
**  Notes           :
**      FIXME probably not as safe as the function name names out.
**      TODO  handle doubles and larger ints.
**
**==========================================================================*/
std::string safeJsonValueAsString(Json::Value& value, bool* wasSet = NULL)
{
    std::string valueStr = "";
    if(wasSet) *wasSet = false;

    if(value.isString() || value.isBool() || value.isNull())
    {
        valueStr = value.asString();
        if(wasSet) *wasSet = true;
    }
    else if(value.isNumeric())
    {
        const size_t BUF_SIZE = 21;
        char buf[BUF_SIZE + 1] = { 0 };
        snprintf(buf, BUF_SIZE, "%u", value.asInt());
        valueStr = buf;
        if(wasSet) *wasSet = true;
    }
    else
    {
        CsWarnx("safeJsonValueAsString is not number nor string. array: %s, object: %s.",
                (value.isArray()  ? "TRUE" : "false"),
                (value.isObject() ? "TRUE" : "false"));
    }
    return valueStr;
}

/*==========================================================================*
**
**  setDeviceTag
**
**  Description     :
**      Updates the devices record for the appropriate tag with the supplied
**      new value if different from current value.
**
**  Parameters      :
**      logicalNm       [I]     IPC_AVL_NOTIFY message payload
**      tag             [I]     device record to update.
**      newValue        [I]     updated value of tag.
**      description     [I]     description for the log.
**      currentValue    [O]     returns the existing value (new if updated)
**                              of tag
**      updateLog       [I]     whether to update the devices log.
**      forceNotify     [I]     update regardless of whether the value has
**                              changed.
**
**  Returns         :
**      true  - if a change occurred.
**      false - otherwise.
**
**  Notes           :
**
**
**==========================================================================*/
bool setDeviceTag(const char *logicalNm, std::string tag, std::string newValue, 
                  std::string description, std::string& currentValue, bool updateLog, bool forceNotify)
{
    bool returnValue = false;    
    Json::Value request;
    Json::Value response;
    Json::Value log;

    Json::FastWriter fw;

    // Set defaults
    response["data"]["description"] = "Unidentified device";
    response["data"]["errorcode"]   = 99;    
    
    std::string logicalName = findLogicalName(logicalNm);
    
    for(Json::Value::ArrayIndex j=0;j<s_deviceList["devices"].size();j++)
    {
        Json::Value id 		    = s_deviceList["devices"][j]["id"];
        Json::Value terminalid 	= s_deviceList["devices"][j]["terminalid"];
        Json::Value status   	= s_deviceList["devices"][j]["status"];
        Json::Value opStatus    = s_deviceList["devices"][j]["opStatus"];
        
        if(!s_deviceList["devices"][j].isMember(tag))
        {
            CsWarnx("setDeviceTag->setDevice%s logicalName: %s tag: %s is not a member of %s. Skipping...",
                    tag.c_str(), logicalName.c_str(), tag.c_str(), 
                    ((terminalid.empty()==false && terminalid.isString()) ? terminalid.asString().c_str() : "unknown terminalid"));
            continue;
        }
        Json::Value oldValue    = s_deviceList["devices"][j][tag];
        bool wasSet = false;
        currentValue = safeJsonValueAsString(oldValue, &wasSet);

        if(!wasSet)
        {
            CsWarnx("setDeviceTag->setDevice%s logicalName: %s unable convert tag: %s to a string"
                    " for member %s. Skipping...", 
                    tag.c_str(), logicalName.c_str(), tag.c_str(), 
                    ((terminalid.empty()==false && terminalid.isString()) ? terminalid.asString().c_str() : "unknown terminalid"));
            continue;
        }

        if((id.empty()==false && id.isString() && id.asString().compare(logicalName)==0) || 
          (terminalid.empty()==false && terminalid.isString() && terminalid.asString().compare(logicalName)==0))
        {
            if(newValue.compare(currentValue) || forceNotify)
            {
                CsDebug(2, (2, "setDeviceTag->setDevice%s %s %s:%s %s", tag.c_str(), 
                            logicalName.c_str(), currentValue.c_str(), 
                            newValue.c_str(), description.c_str()));
                s_deviceList["devices"][j][tag]   = Json::Value(newValue);
                currentValue = s_deviceList["devices"][j][tag].asString();
                    
                returnValue = true;

                if(updateLog || forceNotify)
                {
                    CsTime_t now;
                    CsTime( &now );
                    char timeBuf[64];
                    CsStrTimeSimple(&now, 64, timeBuf);
     
                    log["time"]         = timeBuf;
                    log["status"]       = status;
                    log["opStatus"]     = opStatus;
                    log["description"]  = description;

                    //Update the log with those values who have their own section in the log.
                    //TODO consider refactoring to if(log[tag] != NULL or equivalent instead.
                    if(tag == "status" || tag == "opStatus")
                    {
                        log[tag] = newValue;
                    }

                    s_deviceList["devices"][j]["log"].append(log);      

                    if(s_deviceList["devices"][j]["log"].size()>10)
                    {
                        Json::Value logArrayTmp = Json::Value(Json::arrayValue);
                        for(unsigned int i=1;i<s_deviceList["devices"][j]["log"].size();i++)
                        {
                            logArrayTmp.append(s_deviceList["devices"][j]["log"][i]);
                        }
                        
                        s_deviceList["devices"][j]["log"] = logArrayTmp;
                    }
               
                    response["data"]["devices"]     = s_deviceList["devices"];
                    response["data"]["description"] = "OK";
                    response["data"]["errorcode"]   = 0; 

                    response["name"] = "devicelist";
                    sendDCMessage(fw.write(response));
                }
            }
        }    
    } 

    return returnValue;
}


/*==========================================================================*
**
**  setDeviceTag
**
**  Description     :
**      As the above setDeviceTag function, except does not return the 
**      current tag value.
**
**=========================================================================*/
bool setDeviceTag(const char *logicalNm, std::string tag, std::string newValue, 
                  std::string description, bool updateLog, bool forceNotify)
{
    std::string currentValue;
    return setDeviceTag(logicalNm, tag, newValue, description, currentValue, updateLog, forceNotify);
}


bool setDeviceStatus(const char *logicalNm, u_long deviceStatus, std::string description, bool updateLog, bool forceNotify) 
{
    std::string newStatus = getDeviceStatusName(deviceStatus);
    return setDeviceTag(logicalNm, "status", newStatus, description, updateLog, forceNotify);
}


// returns true if change has occurred
bool setDeviceReaderMode(const char *logicalNm, u_long modeValue, std::string description, bool updateLog, bool forceNotify)
{
    std::string newMode = getModeName(modeValue);
    description+= newMode;                 
    return setDeviceTag(logicalNm, "mode", newMode, description, updateLog, forceNotify);
}


// returns true if change has occurred
bool setDeviceOpStatus(const char *logicalNm, u_long operationalStatus, std::string description, bool updateLog, bool forceNotify)
{
    bool returnValue = false;
    std::string currentOpStatus = "unknown";
    
    std::string newOpStatus = getOpStatusName(operationalStatus);
    returnValue = setDeviceTag(logicalNm, "opStatus", newOpStatus, description, /*OUT*/currentOpStatus, updateLog, forceNotify);

    if (currentOpStatus == "unknown" || currentOpStatus == "Disconnected")
    {
        startLANCommunicationTimer();
    }
    else 
    {
        (void)MYKI_ALARM_sendSimpleAlarm(MYKI_ALARM_DEVICE_MANAGEMENT_STATE, 10, MYKI_ALARM_ALERT_CLEARED);
        stopLANCommunicationTimer();
    }
    return returnValue;
}


bool setDeviceReaderESN(const char *logicalNm, u_long esnValue, std::string description, bool updateLog, bool forceNotify)
{
    const size_t BUF_SIZE = 21;
    char buf[BUF_SIZE + 1] = { 0 };
    snprintf(buf, BUF_SIZE, "%lu", esnValue);
    std::string newESN = buf;
    
    snprintf(buf, BUF_SIZE, "%lX", esnValue);
    std::string hexESN = buf;
    
    description+= hexESN;
    return setDeviceTag(logicalNm, "ESN", newESN, description, updateLog, forceNotify);
}


void processCCEEvent(IPC_CCEEvent_t *p)
{
    Json::Value event;   // will contains the root value after parsing.
    Json::Reader reader;

    // Validate events 
    if(reader.parse(p->jsonstring, event) && event.size()>0) // Must check size otherwise library can segfault
    {         
		Json::FastWriter fw; 
		// Add the source of the event.
		event["source"] = p->source;
        sendSimpleEvent(fw.write(event).c_str());    
    }
    else
    {
        CsErrx("Could not parse event '%s'",p->jsonstring);
    }
}


void process3GState(IPC_3G_State_t *p)
{
    if(g_3GState!=p->state)
    {   

        g_3GState               = p->state;
        g_3GSignalQuality       = p->signalQuality;
        g_3GSignalQualityValue  = p->signalQualityValue;

        getNetworkSettings();
        
        Json::Value ev;
        ev["name"] = "gprs";
        /*ev["data"]["status"]        =  get3GStatusString();
        ev["data"]["signalquality"] =  get3GSignalQualityString(g_3GSignalQuality);
        ev["data"]["signal"]        =  g_3GSignalQualityValue; 
        ev["data"]["ipaddress"]     =  g_GPRSipAddress; */
        ev["data"]["gprsInformation"] = s_network["gprsInformation"];
          
        ev["data"]["health"] = "ERROR";
        switch(g_3GState)
        {
            case IPC_3G_UNINITIALISED:
                ev["data"]["health"] = "ERROR";
                break;
            case IPC_3G_DISCONNECTED:
                ev["data"]["health"] = "ERROR";
                break;
            case IPC_3G_CONNECTING:
                ev["data"]["health"] = "WARNING";
                break;
            case IPC_3G_CONNECTED:
                ev["data"]["health"] = "OK";
                break; 
            default:
                ev["data"]["health"] = "ERROR";
                break;
        }
        
        Json::FastWriter fw;
        sendDCMessage(fw.write(ev));    
    }
}

    /**
     *  @brief  Formats and sends IPC_AVL_SET_TRIP message for
     *          a headless route.
     *  @param  newRouteId (headless) route id.
     */
void
AVL_SetHeadlessTrip( int32_t newRouteId )
{
    CsDebug( 3, ( 3, "AVL_SetHeadlessTrip : %d", newRouteId ) );

    AVL_SetTrip(
            newRouteId,
            AVL_TRIP_DIRECTION_HEADLESS,
            g_headlessShiftID,
            g_headlessTripID,
            (-1) /* CURRENT_TIME */,
            0,
            0,
            0,
            AVL_STOP_UNKNOWN,
            AVL_STOP_UNKNOWN );

    s_overrideEnabled           = false;
    s_overridePreviousStopId    = AVL_STOP_UNKNOWN;
    s_overrideStopId            = AVL_STOP_UNKNOWN;
}

    /**
     *  @brief  Formats and sends IPC_AVL_SET_TRIP message.
     *  @param  newRouteId route id.
     *  @param  newDirection trip direction.
     *  @param  newShiftId shift/table-card id.
     *  @param  newTripId trip id.
     *  @param  newTripStartTime trip start time (minutes since midnight).
     *  @param  newRunId run number.
     *  @param  newDepotId deport id.
     *  @param  newStaffId driver id.
     *  @param  newStopStartId trip start stop id.
     *  @param  newStopEndId trip end stop id.
     */
void
AVL_SetTrip(
    int32_t         newRouteId,
    int32_t         newDirection,
    int32_t         newShiftId,
    int32_t         newTripNumber,
    int32_t         newTripStartTime,
    int32_t         newRunId,
    int32_t         newDepotId,
    int32_t         newStaffId,
    int32_t         newStopStartId,
    int32_t         newStopEndId )
{
    #define MINUTES_PER_HOUR            60

    CsDbg( 2, "AVL_SetTrip : %d %d %d %d %d %d %d %d",
            newRouteId, newDirection, newShiftId, newTripNumber, newTripStartTime, newRunId, newDepotId, newStaffId, newStopStartId, newStopEndId );

    if ( newTripStartTime < 0 )
    {
        /*  Default to current local time. */
        time_t          nowTime     = 0;
        struct tm       nowTm       = { 0 };

        time( &nowTime );
        if ( localtime_r( &nowTime, &nowTm ) != NULL )
        {
            newTripStartTime    = ( nowTm.tm_hour * MINUTES_PER_HOUR ) + nowTm.tm_min;
        }
    }

    s_headless          = AVL_TRIP_DIRECTION_HEADLESS == newDirection ? true : false;
    IPC_AVL_SetTrip_t p =
    {
        {
            IPC_AVL_SET_TRIP,
            (TaskID_t)DC_TASK
        },
        0
    };
    p.newRouteId        = newRouteId;
    p.newDirection      = newDirection;
    p.newShiftId        = newShiftId;
    p.newTripId         = newTripNumber;
    p.newRunId          = newRunId;
    p.newDepotId        = newDepotId;
    p.newStaffId        = newStaffId;
    p.newStopStartId    = s_headless == true ? AVL_STOP_UNKNOWN : newStopStartId;
    p.newStopEndId      = s_headless == true ? AVL_STOP_UNKNOWN : newStopEndId;
    p.newTripStartTime  = newTripStartTime;
    
    IPCSend( avlTaskId, &p, sizeof( p ) );
}

    /**
     *  @brief  Formats and sends IPC_AVL_MANUAL_ENTER_STOP message.
     *  @param  stopId stop id.
     */
void
AVL_ManualEnterStop( int32_t stopId )
{
    CsDebug( 2, ( 2, "AVL_ManualEnterStop : %d", stopId ) );
    IPC_IntPayload_t    IpcAvlManualEnterStop   =
    {
        /*  header */
        {
            IPC_AVL_MANUAL_ENTER_STOP,  /*  type */
            (TaskID_t)DC_TASK           /*  source */
        },
        stopId                          /*  data */
    };  /*  IpcAvlManualEnterStop */

    IPCSend( avlTaskId, &IpcAvlManualEnterStop, sizeof( IpcAvlManualEnterStop ) );
}

void AVL_ManualLeaveStop()
{
    CsDebug(3, (3, "AVL_ManualLeaveStop"));
    IPC_NoPayload_t    IpcAvlManualLeaveStop   =
    {
        /*  header */
        {
            IPC_AVL_MANUAL_LEAVE_STOP,  /*  type */
            (TaskID_t)DC_TASK           /*  source */
        }
    };  /*  AVL_ManuallyLeaveStop */

    IPCSend(avlTaskId, &IpcAvlManualLeaveStop, sizeof IpcAvlManualLeaveStop); 
}

void* ProcessMessages(void* args) 
{
    CsDebug(1, (1, "ProcessMessages appQueue %u %u", appQueueId, DC_TASK));
    s_running = true;
      
    // Set initial system state
    setState(DCSTATE_INITIALISING);
    
    // Clear the IPC Queue
    IpcFlush(appQueueId);
    
    {
        CsfTimerCreate2(StatusCheckTimeout, "StatusCheckTimer", &g_statusCheckTimer);
        TimerSpec_t ts = { { 0, 0 }, { 0, 0 } };
        ts.it_interval.tm_msecs = g_statusCheckTimeoutMS;       
        ts.it_value.tm_secs    = 1;
        CsfTimerSet(g_statusCheckTimer, TMR_RELTIME, &ts);     
    }
    
    {
        CsfTimerCreate2(PersistTimeout, "PersistTimer", &g_persistTimer);
        TimerSpec_t ts = { { 0, 0 }, { 0, 0 } };    
        ts.it_interval.tm_secs = g_persistTimeoutSecs;       
        ts.it_value.tm_secs    = 5;
        CsfTimerSet(g_persistTimer, TMR_RELTIME, &ts);     
    }
    
    IPC_NoPayload_t poll = { { IPC_WS_POLL_EVENT, (TaskID_t)DC_TASK } };
    IPCSend(appQueueId, &poll, sizeof poll); 
	
    InitialiseAsynchronousProcessingThread();
    InitialiseWebsocketProcessingThread();
        
    while ( s_running ) 
    {
        char    receiveBuf[4096];
        size_t  bytesReceived = 0;
        if ( IpcReceive(appQueueId, receiveBuf, sizeof receiveBuf, &bytesReceived) == -1 )
        {
            CsErrx("IpcReceive failed");
        }
        else
        {
            IPC_NoPayload_t *m = (IPC_NoPayload_t *)receiveBuf;

            if ( bytesReceived == 0 )
            {
                CsErrx("ProcessMessages got empty message");
                continue;
            }
            if ( bytesReceived == sizeof receiveBuf )
            {
                CsErrx("ProcessMessages got maximum sized message, consider increasing buffer size");
            }
            
            switch (m->hdr.type) 
            {
            case IPC_WS_REFRESH_EVENT:
                processRefreshEvent((IPC_NoPayload_t *)m);
                break;
                
            case IPC_WS_RECEIVE_EVENT: // Not expected 
                CsErrx("ProcessMessages got type IPC_WS_RECEIVE_EVENT from task %d", m->hdr.source);
                break;
                
            case IPC_WS_POLL_EVENT:
                {
                    processWebsockets();
                        
                    // TODO Fix CPU Hog when connections are present.
                    usleep(50*1000); // Sleep 50 ms.
                    IPCSend(appQueueId, &poll, sizeof poll);     
                }
                break;
                
            case IPC_AVL_NOTIFY:
                processAVLNotifyEvent((IPC_AVL_Notify_t*)m);
                break;
                
            case IPC_3G_STATE:
                process3GState((IPC_3G_State_t *)m);
                break;                
                
            case IPC_CCE_EVENT:
                // Extract the next message from the test file and send it
                processCCEEvent((IPC_CCEEvent_t *)m);
                break;
                
            case IPC_HEARTBEAT_REQUEST:
                CsDebug(3, (3, "ProcessMessages got IPC_HEARTBEAT_REQUEST"));
                m->hdr.type     = IPC_HEARTBEAT_RESPONSE;
                m->hdr.source   = (TaskID_t)DC_TASK; // TODO Review
                IPCSend(appQueueId, m, sizeof *m);
                break;
                
            case IPC_MYKI_ALARM:
                CsDebug(3, (3, "ProcessMessages got IPC_MYKI_ALARM"));
                // REVIEW Determine if alarm needs to be processed
                break;
            case IPC_TIMEOUT:
                processTimeout((IPC_IntPayload_t*)m);
                break;
            ////////////////////////////////////
                
            case IPC_HEARTBEAT_RESPONSE:
                CsDebug(3, (3, "ProcessMessages got IPC_HEARTBEAT_RESPONSE"));
                break;
                               
            case IPC_TASK_STARTED:
                CsDebug(3, (3, "ProcessMessages got IPC_TASK_STARTED"));
                // TODO: Resend trip to AVL.
                if(m->hdr.source==MYKI_AVL_TaskId)
                {
                    CsDebug(1, (1, "ProcessMessages: AVL IPC_TASK_STARTED resend trip"));
                }                   
                break;
                
            case IPC_TASK_STOPPED:
                // One of the proxys has stopped.
                CsDebug(3, (3, "ProcessMessages got IPC_TASK_STOPPED"));                   
                break;
                
            case IPC_TASK_STOP:
                CsDebug(1, (1, "ProcessMessages got IPC_TASK_STOP"));                   
                s_running = false;
                // TODO other shutdown and optionally call this method.
                break;
               
            default:
                CsErrx("ProcessMessages got type '%d'(%d) from task %d", m->hdr.type, bytesReceived, m->hdr.source);
                break;
            }
        }
    }
    
    return args;
}

// Persist data:
// To store current state, position, logged in user. 
void persistData( bool bSave )
{ 
    CsMutexLock(&g_persistLock);
    
    #define DATA_FILE_PRIMARY           "/afc/etc/dc.data"
    #define DATA_FILE_SECONDARY         "/afc/etc/dc.data.bak"
    
    const char                         *pPath       = DATA_FILE_PRIMARY;
    const char                         *pPathBackup = DATA_FILE_SECONDARY;
     
    if ( bSave != false )
    {
        CsTime_t t;
        CsTime(&t);
        char timeBuf[64];
        CsStrTimeISO(&t,64,timeBuf);  

        CsDebug(1, (1, "Save persistent data state %s user %s terminal %s", getStateString(s_DCState), s_activeStaffId.c_str(), s_activeTerminalId.c_str()));
        
        s_persistedStatus["time"]                    = timeBuf;// Review
        s_persistedStatus["unixtime"]                = (int) time(NULL);
        s_persistedStatus["state"]                   = getStateString(s_DCState);
        s_persistedStatus["activeTerminalId"]        = s_activeTerminalId.c_str();
        s_persistedStatus["activeType"]              = s_activeType.c_str();
        s_persistedStatus["activeRoles"]             = s_activeRoles;
        s_persistedStatus["activeStaffId"]           = s_activeStaffId.c_str();
        s_persistedStatus["activeCSN"]               = s_activeCSN.c_str();
        s_persistedStatus["activeServiceProviderId"] = s_activeServiceProviderId.c_str();
        s_persistedStatus["tablecard"]               = s_tablecard;
        s_persistedStatus["trip"]                    = s_trip;
        s_persistedStatus["routeId"]                 = s_routeId;
        s_persistedStatus["stopId"]                  = s_stopId;  // Persisted but not used at this juncture.
        s_persistedStatus["headless"]                = s_headless;
        s_persistedStatus["driverBreakLockAll"]      = s_driverBreakLockAll?1:0;
        s_persistedStatus["passwordLockTimeout"]     = (int)s_passwordLockTimeout;
        s_persistedStatus["passwordFailureCount"]    = s_passwordFailureCount;
                
        Json::FastWriter fw;
        std::string data = fw.write(s_persistedStatus);
        std::ofstream                       ofs;
        char                                pathNew[ 128 ];

        memset( pathNew, 0, sizeof( pathNew ) );
        strncpy( pathNew, pPath, ( sizeof( pathNew ) - ( 4 + 1 ) ) );
        strcat( pathNew, ".new" );
        remove( pathNew );
        ofs.open( pathNew, std::ofstream::out | std::ofstream::trunc );
        if ( ofs.is_open( ) )
        {
            ofs << data;
        }
        ofs.close( );

        remove( pPathBackup );
        rename( pPath, pPathBackup );
        rename( pathNew, pPath ) ;     
    }
    else
    {
        CsDebug(2, (2, "Load persistent data"));
        std::ifstream                       ifs;
        Json::Reader reader;
        Json::Value jsonfile;
            
        ifs.open( pPath, std::ifstream::binary );
        if ( ifs.is_open() )
        {        
            std::stringstream               buf;
            buf << ifs.rdbuf( );
            ifs.close( );
            
            if(reader.parse(buf,jsonfile) && jsonfile.size()>0) // Must check size otherwise library can segfault
            {    
                CsDebug(2, (2, "Load persistent data loaded"));
                s_persistedStatus = jsonfile;               
            }
            else
            {
                CsErrx("Could not load persistent data.");
            }
        }
        else
        {
            CsWarnx("Persistent data does not exist.");
        }
    }
    
    CsMutexUnlock(&g_persistLock);
}   //  persistData( )


void persistRouteData( bool bSave )
{
    CsMutexLock(&g_persistLock);
    #define ROUTE_DATA_FILE_PRIMARY           "/afc/etc/dcroute.data"
    #define ROUTE_DATA_FILE_SECONDARY         "/afc/etc/dcroute.data.bak"
    
    const char                         *pPath       = ROUTE_DATA_FILE_PRIMARY;
    const char                         *pPathBackup = ROUTE_DATA_FILE_SECONDARY;
     
    if ( bSave != false )
    {
        CsDebug(1, (1, "Save persistent stop data state %s user %s terminal %s stop %d override stop %d", getStateString(s_DCState), s_activeStaffId.c_str(), s_activeTerminalId.c_str(), s_stopId, s_overrideStopId));
         
        CsTime_t t;
        CsTime(&t);
        char timeBuf[64];
        CsStrTimeISO(&t,64,timeBuf);  

        s_persistedRouteStatus["unixtime"]                = (int) time(NULL);
        s_persistedRouteStatus["routeId"]                 = s_routeId;
        s_persistedRouteStatus["stopId"]                  = s_stopId;  
        s_persistedRouteStatus["headless"]                = s_headless;
        s_persistedRouteStatus["overrideEnabled"]         = s_overrideEnabled;
        s_persistedRouteStatus["overrideStopId"]          = s_overrideStopId;
        s_persistedRouteStatus["overridePreviousStopId"]  = s_overridePreviousStopId;
        
        Json::FastWriter fw;
        std::string data = fw.write(s_persistedRouteStatus);
        std::ofstream                       ofs;
        char                                pathNew[ 128 ];

        memset( pathNew, 0, sizeof( pathNew ) );
        strncpy( pathNew, pPath, ( sizeof( pathNew ) - ( 4 + 1 ) ) );
        strcat( pathNew, ".new" );
        remove( pathNew );
        ofs.open( pathNew, std::ofstream::out | std::ofstream::trunc );
        if ( ofs.is_open( ) )
        {
            ofs << data;
        }
        ofs.close( );

        remove( pPathBackup );
        rename( pPath, pPathBackup );
        rename( pathNew, pPath ) ;     
    }
    else
    {
        CsDebug(2, (2, "Load persistent route data"));
        std::ifstream                       ifs;
        Json::Reader reader;
        Json::Value jsonfile;
            
        ifs.open( pPath, std::ifstream::binary );
        if ( ifs.is_open() )
        {        
            std::stringstream               buf;
            buf << ifs.rdbuf( );
            ifs.close( );
            
            if(reader.parse(buf,jsonfile) && jsonfile.size()>0) // Must check size otherwise library can segfault
            {    
                CsDebug(2, (2, "Load persistent route data loaded"));
                s_persistedRouteStatus = jsonfile;               
            }
            else
            {
                CsErrx("Could not load persistent route data.");
            }
        }
        else
        {
            CsWarnx("Persistent data does not exist.");
        }
    }
    CsMutexUnlock(&g_persistLock);
}   //  persistRouteData( )

bool hasCriticalFailure()
{
    if(s_secondaryCriticalFailure)
        return true;
	MYKI_ALARM_AlarmListPair_t alarms[256];
	U32_t size = 256;
	int ret = MYKI_ALARM_listSimpleAlarms(alarms, &size);
	if ( ret == 0 )
	{	
		for(U32_t i=0;i<size;i++)
		{
			// List of critical alarms taken from NTS Alarm List (NTS1797)
			
			switch ( alarms[i].unitType )
			{
			case MYKI_ALARM_UNKNOWN_UNIT_TYPE:
				// Not Applicable
				break;
			case MYKI_ALARM_OPERATIONAL_STATE: 
				
				if(alarms[i].alarmId==10 || alarms[i].alarmId==100 || alarms[i].alarmId==110 || alarms[i].alarmId==200)
                {
					return true;
				}
				break;
			case MYKI_ALARM_E_CARD_ENCODER_STATE: 
                // May not be relevant as user and use the password of the day to login.
				break;
				
			case MYKI_ALARM_UPS_STATE: 
				break;
			case MYKI_ALARM_INTERNAL_HW_COMM_STATE: 
				break;
			case MYKI_ALARM_TOUCH_SCREEN_STATE: 
				// Not Applicable
				break;	
			case MYKI_ALARM_INTRUSION_STATE: 
				// Not Applicable
				break;
			case MYKI_ALARM_STORAGE_MEDIUM_STATE: 
				if(alarms[i].alarmId==20) 
				{	
					return true;
				}			
				break;
			case MYKI_ALARM_POWER_STATE: 
				break;
			case MYKI_ALARM_BATTERY_STATE: 
				break;
			case MYKI_ALARM_VALIDATOR_SYSTEM_STATE: 
				break;
			case MYKI_ALARM_BARRIER_SYSTEM_STATE: 
				// Not Applicable
				break;
			case MYKI_ALARM_DEVICE_MANAGEMENT_STATE: 
				if(alarms[i].alarmId==220)
				{	
                    // Verify
//					return true;
				}				
				break;
			case MYKI_ALARM_TIME_SYNCHRONIZATION_STATE: 
				if(alarms[i].alarmId==20)
				{	
					return true;
				}				
				break;
			case MYKI_ALARM_TRANSACTION_LOG_STATE: 
				if(alarms[i].alarmId==20)
				{	
					return true;
				}				
				break;
			case MYKI_ALARM_SAM_KEY_UPDATE_STATE: 
				//return true;
                // Verify
				break;
			default:
				// Do nothing
				break;
			}		
		}		
	}		

	return false;
}

void LANCommunicationTimeout(uint32_t timer)
{
    CsDebug(2, (2, "LANCommunicationTimeout func"));
    IPC_IntPayload_t   m = { {IPC_TIMEOUT, DC_CMD_TASK }, LAN_COMM_TIMEOUT};
    IPCSend(appQueueId, &m, sizeof m); 
}

void ChangeEndsTimeout(uint32_t timer)
{
    IPC_IntPayload_t   m = { {IPC_TIMEOUT, DC_CMD_TASK }, CHANGE_ENDS_TIMEOUT};
    IPCSend(appQueueId, &m, sizeof m);
}

void PasswordLockTimeout(uint32_t timer)
{
    IPC_IntPayload_t   m = { {IPC_TIMEOUT, DC_CMD_TASK }, PASSWORD_LOCK_TIMEOUT};
    IPCSend(appQueueId, &m, sizeof m);
}

void stopPasswordLockTimer()
{
    if(g_passwordLockTimer!=0)
    {
        TimerSpec_t     v = { {0}, {0} }; // Stop the timer
        CsfTimerSet(g_passwordLockTimer, TMR_RELTIME, &v);
    }
    
    s_passwordLockTimeout = 0;
}

void startPasswordLockTimer(int duration)
{
    stopPasswordLockTimer();
    CsDebug(2, (2, "startPasswordLockTimer %ds", duration));
    if(g_passwordLockTimer==0)
    {
        CsfTimerCreate2(PasswordLockTimeout, "PasswordLock timer", &g_passwordLockTimer);
    }
        
    TimerSpec_t ts = { { 0, 0 }, { 0, 0 } };
    ts.it_value.tm_secs = duration;
    CsfTimerSet(g_passwordLockTimer, TMR_RELTIME, &ts); 
    
    s_passwordLockTimeout = time(NULL) + duration;
    persistData( true );
}

void stopLANCommunicationTimer()
{
    CsDebug(2, (2, "stopLANCommunicationTimer"));
    CsMutexLock(&g_LANCommunicationTimerLock);
    if(g_LANCommunicationTimer != 0)
    {
        TimerSpec_t     v = { {0}, {0} };
        CsfTimerSet(g_LANCommunicationTimer, TMR_RELTIME, &v);
    }
    g_LANCommunicationTimerCheck = false;
    CsMutexUnlock(&g_LANCommunicationTimerLock);
}

void startLANCommunicationTimer()
{
    CsDebug(2, (2, "startLANCommunicationTimer"));
    CsMutexLock(&g_LANCommunicationTimerLock);
    if(g_LANCommunicationTimer == 0)
        CsfTimerCreate2(LANCommunicationTimeout, "LANCommunicationTimer", &g_LANCommunicationTimer);
    if(g_LANCommunicationTimerCheck == false)
    {
	g_LANCommunicationTimerCheck = true;
        TimerSpec_t ts = { { 0, 0 }, { 0, 0 } };
        ts.it_value.tm_secs = g_LANCommunicationTimeout;
        CsfTimerSet(g_LANCommunicationTimer, TMR_RELTIME, &ts);
    }
    CsMutexUnlock(&g_LANCommunicationTimerLock);
}

void stopChangeEndsTimer()
{
    if(g_changeEndsTimer!=0)
    {
        TimerSpec_t     v = { {0}, {0} }; // Stop the timer
        CsfTimerSet(g_changeEndsTimer, TMR_RELTIME, &v);
    }
}

void startChangeEndsTimer(int time)
{
    stopChangeEndsTimer();
    CsDebug(2, (2, "startChangeEndsTimer %ds", time));
    if(g_changeEndsTimer==0)
        CsfTimerCreate2(ChangeEndsTimeout, "ChangeEnds timer", &g_changeEndsTimer);
        
    TimerSpec_t ts = { { 0, 0 }, { 0, 0 } };
    ts.it_value.tm_secs    = time;
    CsfTimerSet(g_changeEndsTimer, TMR_RELTIME, &ts); 
}

void DriverBreakTimeout(uint32_t timer)
{
    CsDebug(5, (5, "DriverBreakTimeout"));
    IPC_IntPayload_t   m = { {IPC_TIMEOUT, DC_CMD_TASK }, DRIVER_BREAK_TIMEOUT};
    IPCSend(appQueueId, &m, sizeof m);
}

void StatusCheckTimeout(uint32_t timer)
{
    CsDebug(7, (7, "StatusCheckTimeout"));
    IPC_IntPayload_t   m = { {IPC_TIMEOUT, DC_CMD_TASK }, STATUS_CHECK_TIMEOUT};
    IPCSend(appQueueId, &m, sizeof m);
}

void PersistTimeout(uint32_t timer)
{
    CsDebug(7, (7, "PersistTimeout"));
    IPC_IntPayload_t   m = { {IPC_TIMEOUT, DC_CMD_TASK }, PERSIST_TIMEOUT};
    IPCSend(appQueueId, &m, sizeof m);
}

void stopDriverBreakTimer()
{
    if(g_driverBreakTimer!=0)
    {
        TimerSpec_t     v = { {0}, {0} }; // Stop the timer
        CsfTimerSet(g_driverBreakTimer, TMR_RELTIME, &v);
    }
}

void startDriverBreakTimer()
{
    stopDriverBreakTimer();
    
    CsDebug(2, (2, "startDriverBreakTimer %ds", g_driverBreakTimeout));
    if(g_driverBreakTimer==0)
        CsfTimerCreate2(DriverBreakTimeout, "Driver Break timer", &g_driverBreakTimer);
        
    TimerSpec_t ts = { { 0, 0 }, { 0, 0 } };
    ts.it_value.tm_secs    = g_driverBreakTimeout;
    CsfTimerSet(g_driverBreakTimer, TMR_RELTIME, &ts); 
}

//Check for alarms
void statusCheck()
{
    s_criticalFailure = hasCriticalFailure();
    if(s_criticalFailure || s_secondaryCriticalFailure)
    {
        if(s_DCState==DCSTATE_OUT_OF_ORDER || s_DCState==DCSTATE_TECHNICIAN)
        {
            // Do nothing
        }
        else
        {
            setState(DCSTATE_OUT_OF_ORDER);
        }
    }
    else if(s_DCState==DCSTATE_OUT_OF_ORDER)
    {
        s_criticalFailure = false;
		CsDebug(2, (2, "statusCheck calling processAutoLogOff"));
		processAutoLogOff(); // discuss
        // setState(DCSTATE_STANDBY);
    }        

    // Check the heart beat map to see if any driver consoles have not been respondng
    // NOTE Could be affected by large time sync but should recover and only affects the
    // status icon
    time_t now = time(NULL);
    CsMutexLock(&g_heartbeatMapLock);
        typedef std::map<std::string, time_t >::iterator it_type;

        for(it_type iterator = g_heartbeatMap.begin(); iterator != g_heartbeatMap.end(); iterator++)
        {
            if(iterator->second>0 && (now - iterator->second)>HEARTBEAT_TIMEOUT_SECONDS)
            {
                // REFACTOR device status changes
                bool changed = setDeviceStatus(iterator->first.c_str(), DEVICE_STATUS_ERROR, "Disconnected", false, false); 
                setDeviceOpStatus(iterator->first.c_str(), DEVICE_OPSTATUS_DISCONNECTED, "Disconnected", changed, true);               
               
                iterator->second = 0; // REVIEW should entry be removed from map.  Map is unlikely to grow.
            }
        }
    CsMutexUnlock(&g_heartbeatMapLock);
    
    CsDebug(4, (4, "Queues: DC:%d, WS:%d, AS:%d, AVL:%d", 
        IpcReceiveQueueSize(appQueueId),
        IpcReceiveQueueSize(wsQueueId),
        IpcReceiveQueueSize(asQueueId),
        IpcReceiveQueueSize(avlTaskId)));
}

void LANCommTimeoutCheck()
{
    CsDebug(3, (3, "LANCommTimeoutCheck"));
    bool combinedDeviceStatus = false;

    stopLANCommunicationTimer(); // Should not be required since this is a result of the timer

    for(Json::Value::ArrayIndex i=0;i<s_deviceList["devices"].size();i++)
    {
        Json::Value id          = s_deviceList["devices"][i]["id"];
        Json::Value terminalid  = s_deviceList["devices"][i]["terminalid"];
        Json::Value opStatus    = s_deviceList["devices"][i]["opStatus"];

        if(id.empty()==false && terminalid.empty()==false)
        {
            if( opStatus == "unknown" || opStatus == "Disconnected")	
                combinedDeviceStatus = true;
        }
    }
    if (combinedDeviceStatus == true)
        (void)MYKI_ALARM_sendSimpleAlarm(MYKI_ALARM_DEVICE_MANAGEMENT_STATE, 10, MYKI_ALARM_ALERT_WARNING);
    else
        (void)MYKI_ALARM_sendSimpleAlarm(MYKI_ALARM_DEVICE_MANAGEMENT_STATE, 10, MYKI_ALARM_ALERT_CLEARED);
}

void processTimeout(IPC_IntPayload_t *p)
{
    switch(p->data)
    {
        case CHANGE_ENDS_TIMEOUT:
            if(s_DCState==DCSTATE_CHANGE_ENDS) setState(DCSTATE_DRIVER_BREAK);
            break;
        case DRIVER_BREAK_TIMEOUT:
            if(s_DCState==DCSTATE_DRIVER_BREAK) 
			{ 
				CsDebug(2, (2, "DRIVER_BREAK_TIMEOUT  calling processAutoLogOff"));
				processAutoLogOff();
			} 
            break;
        case STATUS_CHECK_TIMEOUT:
            statusCheck();
            break;
        case PASSWORD_LOCK_TIMEOUT:
            {
                CsDebug(1, (1, "Password lock removed"));
                s_passwordLock          = false;
                s_passwordForceLogOff   = false;
                s_passwordFailureCount  = 0;
                s_passwordLockTimeout   = 0;
                (void)MYKI_ALARM_sendSimpleAlarm(MYKI_ALARM_INTRUSION_STATE, 10, MYKI_ALARM_ALERT_CLEARED);
                persistData(true);
            }
            break;
        case LAN_COMM_TIMEOUT:
            LANCommTimeoutCheck();
            break;
        case PERSIST_TIMEOUT:
            persistData(true);
            break;
    }
}

bool loadRoles()
{
    Json::Reader reader;
    bool result = false;
        
    std::ifstream rolesData(g_rolesFile, std::ifstream::binary);
    
    if(reader.parse( rolesData, g_roles) && g_roles.size()>0)
    {
        result = true;
    }
    else
    {
        CsErrx("loadRoles failed to load from %s", g_rolesFile);
    }
    
    return result;
}

bool loadServiceProviders()
{
    Json::Reader reader;
    bool result = false;
        
    std::ifstream serviceprovidersData(g_serviceProvidersFile, std::ifstream::binary);
    
    if(reader.parse( serviceprovidersData, g_serviceProviders) && g_serviceProviders.size()>0)
    {
        result = true;
    }
    else
    {
        CsErrx("loadServiceProviders failed to load from %s", g_serviceProvidersFile);
    }
    
    return result;
}

#define DEVICE_ADAPTOR_NAME "eth0"
#define WLAN_DEVICE_ADAPTOR_NAME "wlan0"
#define GPRS_DEVICE_ADAPTOR_NAME "ppp0"

int getESN()
{
#if defined(COBRA) || defined(HOST_LINUX_POWERPC)
    int r = DrvFramInit();
    if ( r < 0 )
    {
        CsWarnx( "Error initialising FRAM - will not get device ID" );
       //TODO         SetAlarm( AlarmVALErrorFRAM );
    }
    else
    {
        FrmManufactStruc mInfo;
        r = DrvFramReadManfact(&mInfo);
        if ( r < 0 )
        {
            CsWarnx( "Error reading FRAM - will not get device ID" );
         //TODO   SetAlarm( AlarmVALErrorFRAM );
        }
        else
        {
     //TODO       ClearAlarm( AlarmVALErrorFRAM   );
            return mInfo.id.esn;
        }
    }
#endif
    return 0;
}

extern  "C" void getNetworkSettings()
{
#if defined(COBRA) || defined(HOST_LINUX_POWERPC)
    int fd;
    struct ifreq ifr;
    #define HWADDR_len 6

    fd = socket(AF_INET, SOCK_DGRAM, 0);

    /* I want to get an IPv4 IP address */
    ifr.ifr_addr.sa_family = AF_INET;
    strncpy(ifr.ifr_name, DEVICE_ADAPTOR_NAME, IFNAMSIZ-1); // TODO Get the wifi set up.

    if(ioctl(fd, SIOCGIFADDR, &ifr)==0)
    {
        strncpy(g_ipAddress, inet_ntoa(( (struct sockaddr_in *)&ifr.ifr_addr )->sin_addr), CSFINI_MAXVALUE);
    }
    else
    {
        g_ipAddress[0] = '\0';
    }
    close(fd);
    
    /*if(ioctl(fd, SIOCGIFNETMASK, &ifr)==0)
    {
        strncpy(g_netmask, inet_ntoa(( (struct sockaddr_in *)&ifr.ifr_addr )->sin_addr) , CSFINI_MAXVALUE);
    }
    else
    {
        g_netmask[0]= '\0';
    }

    if(ioctl(fd, SIOCGIFHWADDR, &ifr)==0)
    {
    for (i=0; i<HWADDR_len; i++)
    {
        sprintf(&g_mac[i*2],"%02X",((unsigned char*)ifr.ifr_hwaddr.sa_data)[i]);
    }
    }
    else
    {
        g_mac[0] = '\0';
    }

    close(fd);

    {
        FILE* fp = popen("route -n | grep eth0  | grep 'UG[ \t]' | awk '{print $2}'", "r");
        if(fp!=NULL)
        {
            if(fgets(g_gateway, sizeof(g_gateway), fp) == NULL)
            {
                CsDebug(4, (4, "getNetworkSettings : could not determine default network gateway"));
                g_gateway[0] = '\0';
            }
            pclose(fp);
        }
    }    

    strncpy(g_priDNS, "N/A", CSFINI_MAXVALUE);

    CsDebug(3, (3, " IP ADDRESS: %s", g_ipAddress));
    CsDebug(3, (3, " NETMASK   : %s", g_netmask));
    CsDebug(3, (3, " GATEWAY   : %s", g_gateway));
    CsDebug(3, (3, " MAC       : %s", g_mac));
    

    fd = socket(AF_INET, SOCK_DGRAM, 0);

    ifr.ifr_addr.sa_family = AF_INET;
    strncpy(ifr.ifr_name, WLAN_DEVICE_ADAPTOR_NAME, IFNAMSIZ-1); // TODO Get the wifi set up.

    if(ioctl(fd, SIOCGIFADDR, &ifr)==0)
    {
        strncpy(g_WLANipAddress, inet_ntoa(( (struct sockaddr_in *)&ifr.ifr_addr )->sin_addr), CSFINI_MAXVALUE);
    }
    else
    {
        g_WLANipAddress[0] = '\0';
    }

    if(ioctl(fd, SIOCGIFNETMASK, &ifr)==0)
    {
        strncpy(g_WLANnetmask, inet_ntoa(( (struct sockaddr_in *)&ifr.ifr_addr )->sin_addr) , CSFINI_MAXVALUE);
    }
    else
    {
        g_WLANmac[0] = '\0';
    }
    if(ioctl(fd, SIOCGIFHWADDR, &ifr)==0)
    {
        for (i=0; i<HWADDR_len; i++)
        {
            sprintf(&g_WLANmac[i*2],"%02X",((unsigned char*)ifr.ifr_hwaddr.sa_data)[i]);
        }
    }
    else
    {
        g_WLANmac[0] = '\0';
    }


    close(fd);

    {
        FILE* fp = popen("route -n | grep wlan0  | grep 'UG[ \t]' | awk '{print $2}'", "r");
        if(fp!=NULL)
        {
            if(fgets(g_WLANgateway, sizeof(g_gateway), fp) == NULL)
            {
                CsDebug(4, (4, "getNetworkSettings : could not determine default network gateway"));
                g_WLANgateway[0] = '\0';
            }
            pclose(fp);
        }
    }    


    CsDebug(3, (3, " IP ADDRESS: %s", g_WLANipAddress));
    CsDebug(3, (3, " NETMASK   : %s", g_WLANnetmask));
    CsDebug(3, (3, " GATEWAY   : %s", g_WLANgateway));
    CsDebug(3, (3, " MAC       : %s", g_WLANmac));   */ 
    
    ///////////////////////////////////////////////////////////////////////////
    

    fd = socket(AF_INET, SOCK_DGRAM, 0);

    /* I want to get an IPv4 IP address */
    ifr.ifr_addr.sa_family = AF_INET;
    strncpy(ifr.ifr_name, GPRS_DEVICE_ADAPTOR_NAME, IFNAMSIZ-1); 

    if(ioctl(fd, SIOCGIFADDR, &ifr)==0)
    {
        strncpy(g_GPRSipAddress, inet_ntoa(( (struct sockaddr_in *)&ifr.ifr_addr )->sin_addr), CSFINI_MAXVALUE);
    }
    else
    {
        g_GPRSipAddress[0]='\0';
    }

    if(ioctl(fd, SIOCGIFNETMASK, &ifr)==0)
    {
        strncpy(g_GPRSnetmask, inet_ntoa(( (struct sockaddr_in *)&ifr.ifr_addr )->sin_addr) , CSFINI_MAXVALUE);
    }
    else
    {
        g_GPRSnetmask[0]='\0';
    }
    close(fd);

    //TODO Investigate what should be the proper MAC Address for 3G
    /*if(ioctl(fd, SIOCGIFHWADDR, &ifr))
    {
        for (i=0; i<HWADDR_len; i++)
        {
            sprintf(&g_GPRSmac[i*2],"%02X",((unsigned char*)ifr.ifr_hwaddr.sa_data)[i]);
        }
    }
    else
    {
        g_GPRSmac[0] = '\0';
    }

    close(fd);

    {
        FILE* fp = popen("route -n | grep wlan0  | grep 'UG[ \t]' | awk '{print $2}'", "r");
        if(fp!=NULL)
        {
            if(fgets(g_GPRSgateway, sizeof(g_gateway), fp) == NULL)
            {
                CsDebug(4, (4, "getNetworkSettings : could not determine default network gateway"));
                g_GPRSgateway[0] = '\0';
            }
            pclose(fp);
        }
    } */   


    CsDebug(3, (3, " IP ADDRESS: %s", g_GPRSipAddress));
    CsDebug(3, (3, " NETMASK   : %s", g_GPRSnetmask));
    CsDebug(3, (3, " GATEWAY   : %s", g_GPRSgateway));
    //CsDebug(3, (3, " MAC       : %s", g_GPRSmac));       

#else
    // Not required for non cobra deployments
    strncpy(g_ipAddress,   "", CSFINI_MAXVALUE);
    strncpy(g_netmask,     "", CSFINI_MAXVALUE);
    strncpy(g_gateway,     "", CSFINI_MAXVALUE);
    strncpy(g_priDNS,      "", CSFINI_MAXVALUE);
    strncpy(g_WLANipAddress,   "", CSFINI_MAXVALUE);
    strncpy(g_WLANnetmask,     "", CSFINI_MAXVALUE);
    strncpy(g_WLANgateway,     "", CSFINI_MAXVALUE);
    strncpy(g_WLANpriDNS,      "", CSFINI_MAXVALUE);
#endif
    snprintf(g_serialNo, CSFINI_MAXVALUE, "%X", getESN());
    
    Json::Value keyValue;

    /*s_network["lanInformation"] = Json::Value(Json::arrayValue);
    keyValue["name"]  = "IP Address";
    keyValue["value"] = g_ipAddress;
    s_network["lanInformation"].append(keyValue);
    keyValue["name"]  = "Netmask";
    keyValue["value"] = g_netmask;
    s_network["lanInformation"].append(keyValue);
    keyValue["name"]  = "Gateway";
    keyValue["value"] = g_gateway;
    s_network["lanInformation"].append(keyValue);
    keyValue["name"]  = "DNS";
    keyValue["value"] = g_priDNS;
    s_network["lanInformation"].append(keyValue);
    keyValue["name"]  = "MAC Address";
    keyValue["value"] = g_mac;
    s_network["lanInformation"].append(keyValue);   
    
    s_network["wifiInformation"] = Json::Value(Json::arrayValue);
    keyValue["name"]  = "IP Address";
    keyValue["value"] = g_WLANipAddress;
    s_network["wifiInformation"].append(keyValue);
    keyValue["name"]  = "Netmask";
    keyValue["value"] = g_WLANnetmask;
    s_network["wifiInformation"].append(keyValue);
    keyValue["name"]  = "Gateway";
    keyValue["value"] = g_WLANgateway;
    s_network["wifiInformation"].append(keyValue);
    keyValue["name"]  = "DNS";
    keyValue["value"] = g_WLANpriDNS;
    s_network["wifiInformation"].append(keyValue);
    keyValue["name"]  = "MAC Address";
    keyValue["value"] = g_WLANmac;
    s_network["wifiInformation"].append(keyValue);*/     

    strcat(g_GPRSipAddress, " (");
    strcat(g_GPRSipAddress, g_GPRSnetmask);
    strcat(g_GPRSipAddress, ")"); 

    s_network["gprsInformation"] = Json::Value(Json::arrayValue);
    keyValue["name"]  = "Status";
    keyValue["value"] = get3GStatusString(); 
    s_network["gprsInformation"].append(keyValue);
    keyValue["name"]  = "IP Address";
    keyValue["value"] = g_GPRSipAddress;    
    s_network["gprsInformation"].append(keyValue);
    keyValue["name"]  = "DHCP Server";
    keyValue["value"] = "N/A";
    s_network["gprsInformation"].append(keyValue);
    keyValue["name"]  = "MAC Address";
    keyValue["value"] = "N/A";
    s_network["gprsInformation"].append(keyValue);     
}

int InitialiseDatabases()
{
    char iniBuf[CSFINI_MAXVALUE+1];
    bool result = true;

    if( result )
    {
        if ( CsfIniExpand("General:RolesJson", g_rolesFile, sizeof(g_rolesFile)) != 0 )
        {
            CsWarnx("Roles file location not present in configuration files");
            g_rolesFile[0] = 0;
        } 
        
        result = loadRoles();
    }    

    if( result )
    {
        if ( CsfIniExpand("General:ServiceProvidersJson", g_serviceProvidersFile, sizeof(g_serviceProvidersFile)) != 0 )
        {
            CsWarnx("Service Providers File location not present in configuration files");
            g_serviceProvidersFile[0] = 0;
        }

        result = loadServiceProviders();
    }   
    
   
    if ( CsfIniExpand("General:CDDatabase", iniBuf, sizeof(iniBuf)) == 0 )
    {
        CsDebug(1, (1, "InitialiseDatabases open %s", iniBuf));
        result |= MYKI_CD_openCDDatabase(iniBuf);
    }
    else
    {
        CsDebug(1, (1, "InitialiseDatabases open default"));
        result |= MYKI_CD_openCDDatabase(NULL);
    }
    
     
    if ( result )
    {
        if ( MYKI_CD_getStartDateTime() == 0 )
        {
            CsErrx("InitialiseDatabases - no active CD version");
            result = false;
        }
    }
    
    if(result)
    {
        (void)MYKI_ALARM_sendSimpleAlarm(MYKI_ALARM_OPERATIONAL_STATE, 100, MYKI_ALARM_ALERT_CLEARED);
    }
    else
    {
        (void)MYKI_ALARM_sendSimpleAlarm(MYKI_ALARM_OPERATIONAL_STATE, 100, MYKI_ALARM_ALERT_CRITICAL);
    }

    initMykiCd();
    loadMykiCdValues();
    
    return result;
}

void setHeadlessMode()
{   
    CsDebug(2, (2, "setHeadlessMode"));
    // TODO load serviceProvider from ini or through websocket, or through cradle
    U16_t routeId = 0;
    if(MYKI_CD_getHeadlessModeRoute(g_serviceProviderId, &routeId))  
    {        
        // Review should the dc call cceSendLocation
        AVL_SetHeadlessTrip(routeId); // Route officically changes when avl reports it
    }
    else
    {
        CsWarnx("MYKI_CD_getHeadlessModeRoute did not return a result for service provider %d", g_serviceProviderId);
        s_routeId = 0;
        s_stopId  = 1;
        cceSendLocation( s_routeId, s_stopId, TRUE );
        AVL_SetHeadlessTrip(s_routeId);
    }
    
    s_headless = true;
}


void lockPasswordEntry(int duration)
{
    // Only start the timer if not already in progress.
    if(s_passwordLock==false)
    {
        CsDebug(1, (1, "PASSWORD LOCK ENABLED"));
        s_passwordLock = true;
        startPasswordLockTimer(duration);

        if(s_DCState==DCSTATE_DRIVER_BREAK || s_DCState==DCSTATE_CHANGE_ENDS)
        {
            s_passwordForceLogOff = true;
        }
        else
        {
            s_passwordForceLogOff = false;
        }
    }
    else
    {
        s_passwordForceLogOff = false;
    }

    (void)MYKI_ALARM_sendSimpleAlarm(MYKI_ALARM_INTRUSION_STATE, 10, MYKI_ALARM_ALERT_MINOR);
}

bool processPersistedStatus()
{
    time_t now = time(NULL);
    
        CsDebug(1, (1, "processPersistedStatus"));
    // Load data from file
    persistData(false);
    persistRouteData(false);
     Json::FastWriter fw;
     
    CsDebug(1, (1, "processPersistedStatus %s", fw.write(s_persistedStatus).c_str()));
    
    if(s_persistedStatus["unixtime"].empty()==false && s_persistedStatus["unixtime"].isInt())
    {
        
        // 1. check that the status file is not older than TPE:PersistedStateExpiry (minutes)
        int timeDiff = (int)now - s_persistedStatus["unixtime"].asInt();
        
        CsDebug(1, (1, "processPersistedStatus t1 %d now %d diff %d ", s_persistedStatus["unixtime"].asInt(), now,timeDiff ));
        if(timeDiff>=0 && timeDiff<(g_EPLRecoveryPeriod*60))
        {
            CsDebug(1, (1, "processPersistedStatus status has not expired"));
            // 2. Check that the state string is valid
            if(s_persistedStatus["state"].empty()==false && s_persistedStatus["state"].isString())
            {
                DCState persistedState = getState(s_persistedStatus["state"].asString());
                if(persistedState!=DCSTATE_UNINITIALISED && persistedState!=DCSTATE_INITIALISING && persistedState!=DCSTATE_OUT_OF_ORDER)
                {
                    CsDebug(1, (1, "processPersistedStatus valid state %d %s", persistedState, s_persistedStatus["state"].asString().c_str()));
                    
                    s_headless                  = s_persistedStatus["headless"].asBool();
                    s_activeTerminalId          = s_persistedStatus["activeTerminalId"].asString();             
                    s_activeType                = s_persistedStatus["activeType"].asString(); 
                    s_activeRoles               = s_persistedStatus["activeRoles"]; 
                    s_activeStaffId             = s_persistedStatus["activeStaffId"].asString(); 
                    s_activeServiceProviderId   = s_persistedStatus["activeServiceProviderId"].asString(); 
                    s_tablecard                 = s_persistedStatus["tablecard"]; 
                    s_trip                      = s_persistedStatus["trip"]; 
                    s_routeId                   = s_persistedStatus["routeId"].asInt(); 
                                        
                    if (s_persistedStatus["activeCSN"].empty()==false)
                        s_activeCSN             = s_persistedStatus["activeCSN"].asString();

                    if(s_persistedStatus["passwordLockTimeout"].empty()==false && s_persistedStatus["passwordFailureCount"].empty()==false)
                    {
                        s_passwordLockTimeout       = s_persistedStatus["passwordLockTimeout"].asInt(); 
                        s_passwordFailureCount      = s_persistedStatus["passwordFailureCount"].asInt(); 
                        time_t now = time(NULL);
                        if(s_passwordLockTimeout<=now)
                        {
                            s_passwordLockTimeout  = 0;
                            s_passwordFailureCount = 0;
                        }
                        else
                        {
                            int duration = s_passwordLockTimeout - now;
                            
                            if(duration>g_InvalidPODTimeout*60) 
                                duration = g_InvalidPODTimeout*60; // Enforce the maximum in case of time changes etc.
                            lockPasswordEntry(duration);   
                        }
                            
                    }
                    else
                    {
                        s_passwordLockTimeout = 0;
                    }
                    
                    
                    
                    // Review whether stop should apply on restart or avl + gps should handle the location.
                    int timeDiffStop = 0;
                
                    if(s_persistedRouteStatus["unixtime"].empty()==false)
                    {
                        timeDiffStop = (int)now - s_persistedRouteStatus["unixtime"].asInt();
                    }
                    
                    if(timeDiffStop>=0 && timeDiffStop<(g_EPLRecoveryPeriod*60) && s_persistedRouteStatus["routeId"].asInt()==s_routeId)
                    {
                        
                        // Different file is used for the stop details to reduce the writes to the flash.
                        s_stopId                                          = s_persistedRouteStatus["stopId"].asInt();  
                        s_overrideEnabled                                 = s_persistedRouteStatus["overrideEnabled"].asBool();
                        s_overrideStopId                                  = s_persistedRouteStatus["overrideStopId"].asInt();
                        s_overridePreviousStopId                          = s_persistedRouteStatus["overridePreviousStopId"].asInt();
                        
                        CsDebug(1, (1, "processPersistedStatus load stop details stop %d, override %d ", s_stopId, s_overrideStopId));
                    }
                    else
                    {
                        s_overridePreviousStopId    = 0;
                        s_overrideStopId            = 0;
                        s_overrideEnabled           = false;     
                    }                    
                    
                    // 3. only reload data is there is an active login (go straight to headless)
                    if( s_activeTerminalId != "") // No device is logged in, then enter headless mode. 
                    {
                        CsDebug(1, (1, "processPersistedStatus Terminal id %s", s_activeTerminalId.c_str()));
                        bool validState =false;
                        switch(persistedState)
                        {
                            case DCSTATE_TECHNICIAN:
                            case DCSTATE_SUPERVISOR:       
                                s_headless = true; // Ensure that headless is applied in technician or supervisor mode
                                // Fall through
                            case DCSTATE_OPERATIONAL:
                            case DCSTATE_CHANGE_ENDS:
                            case DCSTATE_DRIVER_BREAK:
                                validState = true;
                                break;          
                            default:
                            
                            // All other states cannot be resumed, e.g.  Out of service
                                break;
                        }
                        
                        // Relock the devices.  Note we are not concerned with setting the timers with the correct timeout at this point.
                        if(persistedState==DCSTATE_DRIVER_BREAK &&  s_persistedStatus["driverBreakLockAll"].empty()==false)
                        {
                            s_driverBreakLockAll = s_persistedStatus["driverBreakLockAll"].asInt();
                        }
                        else if (persistedState==DCSTATE_OPERATIONAL && (strcmp(g_terminalType, "BDC") == 0))
                        {
                            persistedState = DCSTATE_DRIVER_BREAK;
                            s_driverBreakLockAll = false;
                        } 
                        if(validState)
                        {
                            if(s_trip["route_id"].empty())
                            {
                                s_headless = true;
                            }    
                            
                            if ( s_headless == false )
                            {
                                // TODO REFACTOR REMOVE DUPLICATION //
                                int32_t     routeId             = (int32_t)getJsonInt(    s_trip,                   "route_id"      );
                                int32_t     shiftId             = (int32_t)getJsonInt(    s_tablecard,              "shift_number"  );
                                int32_t     depotId             = (int32_t)getJsonInt(    s_trip,                   "depot_id"      );
                                int32_t     runId               = (int32_t)getJsonInt(    s_trip,                   "run_id"        );
                       //UNUSED int32_t     tripId              = (int32_t)getJsonInt(    s_trip,                   "trip_id"       );
                                int32_t     tripNumber          = (int32_t)getJsonInt(    s_trip,                   "trip_number"   );
                                std::string tripStartTimeStr    =          getJsonString( s_trip,                   "start_time"    );
                                int32_t     tripStartTime       = -1;
                                int32_t     firstStopId         = s_trip[ "first_stop" ].empty() ?
                                                                  0 :
                                                                  (int32_t)getJsonInt(    s_trip[ "first_stop" ],   "id"            );
                                int32_t     lastStopId          = s_trip[ "last_stop" ].empty() ?
                                                                  0 :
                                                                  (int32_t)getJsonInt(    s_trip[ "last_stop"  ],   "id"            );
                                int32_t     staffId             = s_activeStaffId.empty() ?
                                                                  0 :
                                                                  (int32_t)atol( s_activeStaffId.c_str( ) );

                                // Note inbound is not applicable as separate route ids are used into the city and out.
                                int         direction           = s_trip[ "route_code" ].isString( ) ? AVL_TRIP_DIRECTION_OUTBOUND : AVL_TRIP_DIRECTION_HEADLESS;

                                if ( tripStartTimeStr.empty() == false )
                                {
                                    int32_t tripStartTimeHH     = 0;
                                    int32_t tripStartTimeMM     = 0;
                                    int32_t tripStartTimeSS     = 0;

                                    if ( sscanf( tripStartTimeStr.c_str(), "%d:%d:%d", &tripStartTimeHH, &tripStartTimeMM, &tripStartTimeSS ) >= ( 1 /*HH*/ + 1 /*MM*/ ) )
                                    {
                                        tripStartTime   = ( tripStartTimeHH * MINUTES_PER_HOUR ) + tripStartTimeMM;
                                    }
                                }

                                //CsDebug(1, (1, "process_SET_trip route %d %s", routeId, direction==AVL_TRIP_DIRECTION_HEADLESS?"Headless":"Outbound"));
                                AVL_SetTrip( routeId, direction, shiftId, tripNumber, tripStartTime, runId, depotId, staffId, firstStopId, lastStopId );

                                if ( s_overrideEnabled )
                                {
                                    CsDebug( 1, ( 1, "processPersistedStatus Set Override stop %d start stop %d", s_overrideStopId, firstStopId ) );
                                    AVL_ManualEnterStop( s_overrideStopId );
                                }    
                            }
                            else
                            {
                                setHeadlessMode( );
                            }
                            return setState( persistedState, true ) == 0; 
                        }
                    }
                }
            }
        }
        else
        {
            if(strcmp(g_terminalType, TERMINAL_TYPE_BUS) == 0)
            {
                if(s_persistedStatus["state"].empty()==false && s_persistedStatus["state"].isString())
                {
                    DCState persistedState = getState(s_persistedStatus["state"].asString());
                    
                    if(persistedState!=DCSTATE_UNINITIALISED && persistedState!=DCSTATE_INITIALISING && persistedState!=DCSTATE_OUT_OF_ORDER)
                    {
                        CsDebug(1, (1, "processPersistedStatus call endshift %s", s_persistedStatus["activeTerminalId"].asString().c_str()));
                        
                        if(s_persistedStatus["activeTerminalId"].empty()==false && s_persistedStatus["activeTerminalId"].isString())
                        {   
                            s_logOffTerminalId        = s_persistedStatus["activeTerminalId"].asString();
                            s_logOffType              = s_persistedStatus["activeType"].asString();
                            s_logOffStaffId           = s_persistedStatus["activeStaffId"].asString();
                            s_logOffServiceProviderId = s_persistedStatus["activeServiceProviderId"].asString();

                            CsDebug(1, (1, "processPersistedStatus g_sendAutoLogOffAtPowerStart set to TRUE"));
                            g_sendAutoLogOffAtPowerStart = true;
                            
                        }                        
                    }
                }
            }
        }
    }
    
    return false;
}

void sendStateEvent(bool cardDetails)
{
    Json::Value ev;
    ev["name"] = "state";
    ev["state"] = getStateString(s_DCState);
    ev["activeterminalid"]          = s_activeTerminalId;
    ev["activeoperatorid"]          = s_activeStaffId;
    ev["activeserviceproviderid"]   = s_activeServiceProviderId;
    ev["activetype"]                = s_activeType;
    ev["activeroles"]               = s_activeRoles;
    ev["headlessMode"]              = s_headless;
    ev["passwordlogin"]             = s_passwordLogin;
    ev["passwordlock"]              = s_passwordLock;
    ev["criticalfailure"]           = (s_criticalFailure || s_secondaryCriticalFailure);
    ev["carddetails"]               = cardDetails;
    if (!s_activeCSN.empty())
        ev["activeCSN"]             = s_activeCSN;
    
    Json::FastWriter fw;
    sendDCMessage(fw.write(ev));
}

///////////////////////////////////////////////////////////////////////////////
// State Machine handlers /////////////////////////////////////////////////////

// Called when the driver console enters the uninitialised state
void stateUninitialised()
{
    s_DCState = DCSTATE_UNINITIALISED;
       
    // Nothing to do at this point in time.
    
    // Clear alarms
    (void)MYKI_ALARM_sendSimpleAlarm(MYKI_ALARM_DEVICE_MANAGEMENT_STATE,  10, MYKI_ALARM_ALERT_CLEARED);
    (void)MYKI_ALARM_sendSimpleAlarm(MYKI_ALARM_INTRUSION_STATE,          10, MYKI_ALARM_ALERT_CLEARED);
    (void)MYKI_ALARM_sendSimpleAlarm(MYKI_ALARM_OPERATIONAL_STATE,       100, MYKI_ALARM_ALERT_CLEARED);
    
    // TODO Add timer to stop tight-loop of state changes on initialisation failure
    setState(DCSTATE_INITIALISING);
}

void stateInitialising()
{
    s_DCState = DCSTATE_INITIALISING;

    // Start any other required services/APIs
    getNetworkSettings();

    if(InitialiseDatabases())
    {
        if(processPersistedStatus()==false)
        {
            setState(DCSTATE_STANDBY);
        }
    }
    else
    {
        setState(DCSTATE_STANDBY); // Restricted access (technician) only
    }
}
   
void stateStandby()
{
//    The DC enters standby mode when: -
//     No Operator has logged in to the DC
//     The Driver logs out
//     A Supervisor has logged out of the DC
//     A Technician has logged out of the DC
//     The DC was in operational mode and there had been a period of inactivity (configurable timeout)
//     The DC is in Driver break mode and a different Operator Smartcard is presented
//     The DC is in Change Ends mode and a different Operator Smartcard is presented

    s_DCState = DCSTATE_STANDBY;
    
    if( s_activeTerminalId == "") // No device is logged in, then enter headless mode. // TODO: Verify
        setHeadlessMode();
        
    stopChangeEndsTimer();
    stopDriverBreakTimer();        
    
    if(hasCriticalFailure()) { setState(DCSTATE_OUT_OF_ORDER); return; } 
}

void stateOutOfOrder()
{
// This state will exist if the DC cannot operate due to:-
//  Failure to load the application
//  Failure of a major hardware component that affects general operation

    
    s_passwordFailureCount = 0;
    s_DCState = DCSTATE_OUT_OF_ORDER;
    if(hasCriticalFailure()==false && s_secondaryCriticalFailure==false) { CsDebug(2, (2, "stateOutOfOrder calling processAutoLogOff")); processAutoLogOff();/*setState(DCSTATE_STANDBY);*/ return; } 
    s_criticalFailure = true;
    stopChangeEndsTimer();
    stopDriverBreakTimer();
    setHeadlessMode();
    // This state awaits for the critical failure to clear
}

void stateOperational()
{
//The DC is in operational mode when the Driver is logged in and: -
// The Bus/Tram is within the GPS footprint of a stop on the current route or // IMPORTANT (IGNORE)
// The Driver has not selected to put the device into travelling mode or
// The Driver has manually switched to operational mode from travelling mode     // TODO Verify (Travelling mode no longer exists)

    
    s_DCState = DCSTATE_OPERATIONAL;
    s_driverBreakLockAll= false;
    if(hasCriticalFailure()) { setState(DCSTATE_OUT_OF_ORDER); return; } 
    
    stopChangeEndsTimer();
    stopDriverBreakTimer();
}

// The DC Shall reach this state during the power off process until end.
void statePowerOff()
{
    persistData(true);
    persistRouteData(true);
    
    s_DCState = DCSTATE_POWER_OFF;
    
    
    s_driverBreakLockAll= false;
 
    stopChangeEndsTimer();
    stopDriverBreakTimer();
}

// DEPRECATED //// DEPRECATED //// DEPRECATED //// DEPRECATED //// DEPRECATED //// DEPRECATED //
// When the DC is in travel mode it will not respond to any commands other than a request to change to 
// operational mode. The status bar and the positional bar will be visible to the Driver but they cannot be 
// interacted with. If configured to do so, the DC will automatically navigate to travelling mode when it is 
// outside the defined footprint of a stop on the current route. Alternatively the Driver can enable travel 
// mode manually from the main product view in operational mode
void stateTravelling()
{
    s_driverBreakLockAll= false;
    s_DCState = DCSTATE_TRAVELLING;
    if(hasCriticalFailure()) { setState(DCSTATE_OUT_OF_ORDER); return; } 
}

// The Driver has selected Driver break rather than log out or change ends.
void stateDriverBreak()
{
    s_DCState = DCSTATE_DRIVER_BREAK;
    if(hasCriticalFailure()) { s_driverBreakLockAll = false; setState(DCSTATE_OUT_OF_ORDER); return; } 
    
    CsDebug(2, (2, "stateDriverBreak %s", s_driverBreakLockAll?"locked":"not-locked"));
    sendVaropToAll(VO_DRIVER_BREAK, s_driverBreakLockAll?1:0);
    
    stopChangeEndsTimer();
    startDriverBreakTimer();
}

// The Driver has selected change ends. Controlled equipment is still enabled (unlocked). If the Driver 
// does not represent their Smartcard within 5 minutes (configurable) then the state changes to Driver 
// break.
void stateChangeEnds()
{
    s_DCState = DCSTATE_CHANGE_ENDS;
    if(hasCriticalFailure()) { setState(DCSTATE_OUT_OF_ORDER); return; } 

    // start timer
    startChangeEndsTimer(g_changeEndsTimeout);
}

// The TDC enters this mode when a Technician logs in, default headless route will be 
// running, so that the FPDs can remain in service. The device will remain in this mode until the 
// Supervisor/Technician removes their Smartcard from the reader at which point the device will return to 
// standby mode.
void stateTechnician()
{
    s_DCState = DCSTATE_TECHNICIAN;
    //setHeadlessMode();
}

// The TDC enters this mode when a Supervisor logs in, default headless route will be 
// running, so that the FPDs can remain in service. The device will remain in this mode until the 
// Supervisor/Technician removes their Smartcard from the reader at which point the device will return to 
// standby mode.
void stateSupervisor()
{
    s_DCState = DCSTATE_SUPERVISOR;
    //setHeadlessMode();
    // Review if supervisor login can occur in oos.
    if(hasCriticalFailure()) { setState(DCSTATE_OUT_OF_ORDER); return; } 
}

int processStateChange(DCState newState, bool lock)
{
    int result = 0;
    
    if(lock) // CsResource_t would be preferable, however the implementation in CS is broken
    {
        CsMutexLock(&g_stateLock);
    }
    
	CsDebug(4, (4, "processStateChange current State %s newState %s", getStateString(s_DCState), getStateString(newState)));
    // The following code defines the state machine for the driver console.
    if(newState!=s_DCState) 
    {
        switch(s_DCState)
        {
        ///////////////////////////////////////////////////////////////////////////
        case DCSTATE_UNINITIALISED:
            switch(newState)
            {
                case DCSTATE_POWER_OFF:     statePowerOff();      break;
                case DCSTATE_INITIALISING:  stateInitialising();  break;
                default:  result= -1; CsErrx("Invalid state change  %s -> %s", getStateString(s_DCState), getStateString(newState)); break;
            }
            break;
        ///////////////////////////////////////////////////////////////////////////
        case DCSTATE_POWER_OFF:
            switch(newState)
            {
                case DCSTATE_UNINITIALISED: stateUninitialised(); break;
                case DCSTATE_INITIALISING:  stateInitialising();  break;
                default:  result= -1; CsErrx("Invalid state change  %s -> %s", getStateString(s_DCState), getStateString(newState)); break;
            }
            break;        
        ///////////////////////////////////////////////////////////////////////////
        case DCSTATE_INITIALISING:
            switch(newState)
            {
                case DCSTATE_UNINITIALISED:  stateUninitialised(); break;
                case DCSTATE_POWER_OFF:      statePowerOff();      break;    
                case DCSTATE_TECHNICIAN:     stateTechnician();    break;         
                case DCSTATE_SUPERVISOR:     stateSupervisor();    break;      
                case DCSTATE_OPERATIONAL:    stateOperational();   break;  
                case DCSTATE_TRAVELLING:     stateTravelling();    break;
                case DCSTATE_CHANGE_ENDS:    stateChangeEnds();    break;
                case DCSTATE_DRIVER_BREAK:   stateDriverBreak();   break;            
                case DCSTATE_STANDBY:        stateStandby();       break;                
                default:  result= -1; CsErrx("Invalid state change  %s -> %s", getStateString(s_DCState), getStateString(newState)); break;
            }    
            break;
        ///////////////////////////////////////////////////////////////////////////
        case DCSTATE_STANDBY:
            switch(newState)
            {
                case DCSTATE_POWER_OFF:      statePowerOff();       break;                
                case DCSTATE_TECHNICIAN:     stateTechnician();     break;         
                case DCSTATE_SUPERVISOR:     stateSupervisor();     break;        
                case DCSTATE_OUT_OF_ORDER:   stateOutOfOrder();     break; 
                case DCSTATE_OPERATIONAL:    stateOperational();    break; 
                default:  result= -1; CsErrx("Invalid state change  %s -> %s", getStateString(s_DCState), getStateString(newState)); break;
            }        
            break;
        ///////////////////////////////////////////////////////////////////////////
        case DCSTATE_DRIVER_BREAK:
            switch(newState)
            {
                case DCSTATE_POWER_OFF:      statePowerOff();       break;
                case DCSTATE_OPERATIONAL:    stateOperational();    break;
                case DCSTATE_OUT_OF_ORDER:   stateOutOfOrder();     break;
                case DCSTATE_STANDBY:        stateStandby();        break;
                default:  result= -1; CsErrx("Invalid state change  %s -> %s", getStateString(s_DCState), getStateString(newState)); break;
            }       
            break;
        ///////////////////////////////////////////////////////////////////////////
        case DCSTATE_TECHNICIAN:
            switch(newState)
            {
                case DCSTATE_POWER_OFF:      statePowerOff();       break;
                case DCSTATE_STANDBY:        stateStandby();        break;
				case DCSTATE_SUPERVISOR:     stateSupervisor();     break;
                default:  result= -1; CsErrx("Invalid state change  %s -> %s", getStateString(s_DCState), getStateString(newState)); break;
            }        
            break;    
        ///////////////////////////////////////////////////////////////////////////
        case DCSTATE_SUPERVISOR:
            switch(newState)
            {
				case DCSTATE_TECHNICIAN:     result = -1;     break;
                case DCSTATE_POWER_OFF:      statePowerOff();       break;
                case DCSTATE_STANDBY:        stateStandby();        break;
                //case DCSTATE_OPERATIONAL:    stateOperational();        break;
                default:  result= -1; CsErrx("Invalid state change  %s -> %s", getStateString(s_DCState), getStateString(newState)); break;
            }        
            break;            
        ///////////////////////////////////////////////////////////////////////////
        case DCSTATE_OUT_OF_ORDER:
            switch(newState)
            {
                case DCSTATE_POWER_OFF:      statePowerOff();       break;
                case DCSTATE_TECHNICIAN:     stateTechnician();     break;
                case DCSTATE_STANDBY:        stateStandby();        break;
                default:  result= -1; CsErrx("Invalid state change  %s -> %s", getStateString(s_DCState), getStateString(newState)); break;
            }       
            break;
        ///////////////////////////////////////////////////////////////////////////
        case DCSTATE_OPERATIONAL:
            switch(newState)
            {
                case DCSTATE_POWER_OFF:      statePowerOff();       break;
                case DCSTATE_TRAVELLING:     stateTravelling();     break;
                case DCSTATE_CHANGE_ENDS:    stateChangeEnds();     break;
                case DCSTATE_DRIVER_BREAK:   stateDriverBreak();    break;
                case DCSTATE_OUT_OF_ORDER:   stateOutOfOrder();     break;
                case DCSTATE_TECHNICIAN:     stateTechnician();     break;
                case DCSTATE_SUPERVISOR:     stateSupervisor();     break;
                case DCSTATE_STANDBY:        stateStandby();        break;
                case DCSTATE_OPERATIONAL:    stateOperational();    break;
                default:  result= -1; CsErrx("Invalid state change  %s -> %s", getStateString(s_DCState), getStateString(newState)); break;
            }        
            break;
        ///////////////////////////////////////////////////////////////////////////
        case DCSTATE_TRAVELLING: 
            switch(newState)
            {
                case DCSTATE_POWER_OFF:      statePowerOff();       break;
                case DCSTATE_OPERATIONAL:    stateOperational();    break;
                case DCSTATE_OUT_OF_ORDER:   stateOutOfOrder();     break;
                case DCSTATE_STANDBY:        stateStandby();        break;
                default:  result= -1; CsErrx("Invalid state change  %s -> %s", getStateString(s_DCState), getStateString(newState)); break;
            }        
            break;
        ///////////////////////////////////////////////////////////////////////////
        case DCSTATE_CHANGE_ENDS:    
            switch(newState)
            {
                case DCSTATE_POWER_OFF:      statePowerOff();       break;
                case DCSTATE_OPERATIONAL:    stateOperational();    break;
                case DCSTATE_DRIVER_BREAK:   stateDriverBreak();    break;
                case DCSTATE_OUT_OF_ORDER:   stateOutOfOrder();     break;
                case DCSTATE_STANDBY:        stateStandby();        break;
                default: result= -1; CsErrx("Invalid state change  %s -> %s", getStateString(s_DCState), getStateString(newState)); break;
            }     
            break;
        ///////////////////////////////////////////////////////////////////////////
        }
    }
    
    if(lock) // CsResource_t would be preferable, however the implementation in CS is broken
    {
        CsMutexUnlock(&g_stateLock);
    }    
    return result;
}
                 
// Returns zero on success                 
int setState(DCState newState, bool broadcastOnNoChange/*=false */, bool lock/*=false*/)
{
    if(lock)
    {
        // Not an ideal solution, however pushing a state change event into the queue may result in 
        // unexpected changes in behaviour of the application.
        // The reentrant version of CsMutex_t, CsResource_t, currently has at least two serious faults and
        // is not production ready.
        CsMutexLock(&g_stateLock);  
    }
    
    int result = 0;
    if(s_DCState==newState) 
    {
        if(broadcastOnNoChange)
        {
             sendStateEvent();
        }
    }
    else
    {
        CsDebug(4, (4, "Set State %s -> %s", getStateString(s_DCState), getStateString(newState)));
        result = processStateChange(newState, false);
        
        if(result==0)
        {
            if(s_DCState!=DCSTATE_UNINITIALISED  && s_DCState!=DCSTATE_INITIALISING)
            {
                CsDebug(1, (1, "setState persistData")); 
                // UPDATE STATE
                persistData(true);
                persistRouteData(true);
                sendStateEvent();
            }
        }
    }
    if(lock)
    {
        CsMutexUnlock(&g_stateLock);
    }
    return result;
}

    /**
     *  Retrieves integer value from JSON value.
     *  @param  value JSON values.
     *  @param  pName JSON field name.
     *  @return Integer value of the specified JSON field.
     */
int
getJsonInt( Json::Value &value, const char *pName )
{
    int             iValue  = 0;

    if ( pName != NULL && value.empty() == false && value[ pName ].empty() == false )
    {
        if ( value[ pName ].isInt() )
            iValue  = value[ pName ].asInt();
        else
        if ( value[ pName ].isString() )
        {
            std::string sValue  = getJsonString( value, pName );
            iValue              = atoi( sValue.c_str() );
        }
    }
    return  iValue;
}

    /**
     *  Retrieves string value from JSON value.
     *  @param  value JSON values.
     *  @param  pName JSON field name.
     *  @return String value of the specified JSON field.
     */
std::string
getJsonString( Json::Value &value, const char *pName )
{
    std::string         sValue;

    if ( pName != NULL && value.empty() == false && value[ pName ].empty() == false )
    {
        if ( value[ pName ].isString() )
        {
            sValue   = value[ pName ].asString();
        }
        else
        if ( value[ pName ].isInt() )
        {
            int     iValue  = value[ pName ].asInt();
            char    buf[ 32 ];

            CsSnprintf( buf, sizeof( buf ), "%d", iValue );
            sValue  = buf;
        }
    }
    return  sValue;
}

    /**
     *  Retrieves boolean value from JSON value.
     *  @param  value JSON values.
     *  @param  pName JSON field name.
     *  @return Boolean value of the specified JSON field.
     */
bool
getJsonBool( Json::Value &value, const char *pName )
{
    bool                bValue  = false;

    if ( pName != NULL && value.empty() == false && value[ pName ].empty() == false )
    {
        if ( value[ pName ].isBool() )
        {
            bValue  = value[ pName ].asBool();
        }
        else
        if ( value[ pName ].isInt() )
        {
            bValue  = getJsonInt( value, pName ) != 0 ? true : false;
        }
    }
    return  bValue;
}
