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
   1.01      ANT      11/11/2015 NGBU-789: Rectified retrieving company
                                 details from Tariff
 *
 * $Id:  $
 * $HeadURL: $
 *.........................................................................*/
 
#include "dc.h"
#include "messages/messages.h"
#include "myki_schedule.h"
#include "utils.h"
#include <txnlist/txnlist.h>
#include <printerbackup/printerbackupmanager.h>
#include <boost/lexical_cast.hpp>
#include <boost/range.hpp>

using boost::lexical_cast;

#define VALIDATE_PIN_SUCCESS                    0
#define VALIDATE_PIN_ERROR                      1   /*  Error validating PIN - Catch all */
#define VALIDATE_PIN_ERROR_INVALID              2   /*  Error validating PIN - Invalid PIN */
#define VALIDATE_PIN_ERROR_BLOCKED              3   /*  Error validating PIN - Too many PIN retries */
#define VALIDATE_PIN_ERROR_NOT_OPERATOR_CARD    4   /*  Not operator card */
#define VALIDATE_PIN_ERROR_OPEN_OPERATOR_CARD   5   /*  Failed opening operator card */

std::string process_GET_state(Json::Value &request);
std::string process_GET_devicelist(Json::Value &request);
std::string process_GET_login(Json::Value &request);
std::string process_GET_validatepin(Json::Value &request);
std::string process_SET_validatepin(Json::Value &request);
std::string process_GET_changepin(Json::Value &request);
std::string process_GET_config(Json::Value &request);
std::string process_GET_tpurseload(Json::Value &request);
std::string process_SET_tpurseload(Json::Value &request);

/**
 * @brief Handles "tpurseloadreversal" message from the browser, and passes it
 *        onto readerapp.
 * @param request A JSON object of the following format:
 * @code
 * {
 *     "data" : {
 *         "saleseqno" : 280,
 *         "terminalid" : "90400640"
 *     },
 *     "name" : "tpurseloadreversal",
 *     "terminalid" : "90400640",
 *     "type" : "GET",
 *     "userid" : ""
 * }
 * @endcode
 * @return JSON as a string, in the following format:
 * @code
 * {
 *     "terminalid" : "90400640",
 *     "timestamp" : "2015-08-03T11:28:15+10:00",
 *     "name" : "tpurseloadreversal",
 *     "type" : "GET",
 *     "userid" : null,
 *     "errorcode" : 0,
 *     "string_error" : "WS_SUCCESS",
 *     "data" : {}
 * }
 * @endcode
 */
std::string process_GET_tpursereversal(Json::Value &request);

/**
 * @brief Handles "tpurseloadreversal" message from readerapp, and passes it 
 *        onto the browser.
 *
 * The message sent to the browser looks like the following:
 * @code
 * {
 *     "terminalid" : "90400640",
 *     "timestamp" : "2015-08-03T11:28:15+10:00",
 *     "name" : "tpurseloadreversal",
 *     "type" : "event",
 *     "userid" : null,
 *     "errorcode" : 0,
 *     "data" : { ... }
 * }
 * @endcode
 * Note that the "data" field is taken directly from the request parameter.
 * @param request A JSON object of the following format:
 * @code
 * {
 *     "data" : {
 *         "addedvalue" : 1000,
 *         "cardnumber" : "044318122A2380",
 *         "paymenttype" : "cash",
 *         "reason" : 3,
 *         "remvalue" : 9072,
 *         "saleseqno" : 280,
 *         "txnamount" : 1000,
 *         "txnseqno" : 39
 *     },
 *     "errorcode" : 0,
 *     "name" : "tpurseload",
 *     "terminalid" : "90400640",
 *     "timestamp" : "2015-09-16T14:00:02+10:00",
 *     "type" : "set",
 *     "userid" : ""
 * }
 * @endcode
 * @return An empty string.
 */
std::string process_SET_tpursereversal(Json::Value &request);

/**
 * @brief Handles the "touchonoffmode" message from the browser.
 *
 * Sets an internal boolean flag stating whether touch-on/off is enabled or not.
 * @param request A JSON object of the following format:
 * @code
 * {
 *     "data" : {
 *         "enabled" : true,
 *         "terminalid" : "90400640"
 *     },
 *     "name" : "touchonoffmode",
 *     "terminalid" : "90400640",
 *     "type" : "GET",
 *     "userid" : ""
 * }
 * @encode
 * @return JSON as a string, in the the following format:
 * @code
 * {
 *     "terminalid" : "90400640",
 *     "timestamp" : "2015-08-03T11:28:15+10:00",
 *     "name" : "touchonoffmode",
 *     "type" : "GET",
 *     "userid" : null,
 *     "errorcode" : 0,
 *     "data" : {}
 * }
 * @endcode
 */
std::string process_GET_touchonoffmode(Json::Value &request);

/**
 * @brief Handles the "transactionlist" message from the browser, and returns a list
 *        of transactions that can be reversed.
 * @param request A JSON object of the following format:
 * @code
 * {
 *     "data" : {
 *         "terminalid" : "90400640"
 *     },
 *     "name" : "transactionlist",
 *     "terminalid" : "90400640",
 *     "type" : "GET",
 *     "userid" : ""
 * }
 * @endcode
 * @return JSON as a string, in the following format:
 * @code
 * {
 *     "data" : [
 *         {
 *             "amount" : 1000,
 *             "globalSequenceNumber" : 285,
 *             "paymentType" : "cash",
 *             "serialNumber" : "044318122A2380",
 *             "timestamp" : 1442378961,
 *             "txnSequenceNumber" : 44,
 *             "txnType" : "tpurseload"
 *         }
 *     ],
 *     "errorcode" : 0,
 *     "name" : "transactionlist",
 *     "terminalid" : "90400640",
 *     "timestamp" : "2015-09-16T14:49:25+10:00",
 *     "type" : "GET",
 *     "userid" : ""
 * }
 * @endcode
 */
std::string process_GET_transactionlist(Json::Value &request);

/**
 * @brief Handles the "validatetransitcard" message from readerapp, and passes
 *        the message onto the browser.
 *
 * The message sent to the browser looks like the following:
 * @code
 * {
 *     "terminalid" : "90400640",
 *     "timestamp" : "2015-08-03T11:28:15+10:00",
 *     "name" : "validatetransitcard",
 *     "type" : "event",
 *     "userid" : null,
 *     "errorcode" : 0,
 *     "data" : { ... }
 * }
 * @endcode
 * Note that the "data" field is taken directly from the request parameter.
 * @param request A JSON object of the following format:
 * @code
 * {
 *     "terminalid":"1234",
 *     "name":"validatetransitcard",
 *     "errorcode":0,
 *     "data":
 *     {
 *         "cardnumber":"0123456789ABCDEF",
 *         "remvalue":1500,
 *         "istouchon":1,
 *         "istopupreversal":0,
 *         "isactionlisted":0,
 *         "ischangeofmind":1,
 *         "ismoney":1,
 *         "islowbalance":0,
 *         "fare":375,
 *         "expiry":"24 Dec 15 12:00 pm"
 *         "reason":0
 *     }
 * }
 * @endcode
 * @see vgac_readerapp/trunk/src/cardfunctions.h for more details.
 * @return An empty string.
 * @return JSON as a string, in the following format:
 */
std::string process_SET_validatetransitcard(Json::Value &request);

/**
 * @brief Handles the "countpassenger" message from the browser, and passes it 
 * onto readerapp.
 * @param request A JSON object of the following format:
 * @code
 * {  
 *     "name":"countpassenger",
 *     "type":"GET",
 *     "userid":"",
 *     "terminalid":"90400640",
 *     "data":{  
 *         "count":"full"
 *     } 
 * }
 * @endcode
 * @return JSON as a string, in the following format:
 * @code
 * {
 *     "terminalid" : "90400640",
 *     "timestamp" : "2015-08-03T11:28:15+10:00",
 *     "name" : "countpassenger",
 *     "type" : "GET",
 *     "userid" : null,
 *     "errorcode" : 0,
 *     "string_error" : "WS_SUCCESS",
 *     "data" : {}
 * }
 * @endcode
 */
std::string process_GET_countpassenger(Json::Value &request);

/**
 * @brief Handles the "countpassenger" message from readerapp.
 *
 * The message sent to the browser looks like the following:
 * @code
 * {  
 *     "data":null,
 *     "name":"countpassenger",
 *     "string_error":"WS_STRING",
 *     "terminalid":"90400640",
 *     "timestamp":"2015-09-24T16:28:25+10:00",
 *     "type":"set",
 *     "userid":""
 * }
 * @endcode
 * @param request A JSON object of the following format:
 * @code
 * {  
 *     "data":null,
 *     "name":"countpassenger",
 *     "string_error":"WS_ERROR",
 *     "terminalid":"90400640",
 *     "timestamp":"2015-09-24T16:28:24+10:00",
 *     "type":"set",
 *     "userid":""
 * }
 * @endcode
 * @return An empty string.
 */
std::string process_SET_countpassenger(Json::Value &request);

std::string process_GET_shiftend(Json::Value &request);
std::string process_SET_shiftend(Json::Value &request);

std::string process_GET_drivertotals(Json::Value &request);
std::string process_SET_drivertotals(Json::Value &request);

/**
 * @brief Handles the "printlastreceipt" message from the browser.
 * @param request A JSON object of the following format:
 * @code
 * {  
 *     "name":"printlastreceipt",
 *     "type":"GET",
 *     "userid":"",
 *     "terminalid":"90400640",
 * }
 * @endcode
 * @return JSON as a string, in the following format:
 * @code
 * {
 *     "terminalid" : "90400640",
 *     "timestamp" : "2015-08-03T11:28:15+10:00",
 *     "name" : "printlastreceipt",
 *     "type" : "GET",
 *     "userid" : null,
 *     "errorcode" : 0,
 *     "data" : {}
 * }
 * @endcode
 */
std::string process_GET_printlastreceipt(Json::Value &request);

/**
 * @brief Handles the "listnontransit" message from the browser.
 *
 * Sends a list of all the non-transit products to the browser.
 *
 * @see http://cew.vix.local:8090/display/VM/Non-Transit+Product+Sale+and+Surcharge
 *
 * @param request A JSON object of the following format:
 * @code
 * {
 *     "terminalid":"1234",
 *     "type":"GET",
 *     "name":"listnontransit",
 *     "userid":"D123"
 * }
 * @endcode
 * @return JSON as a string, containing a list of non-transit products, in the 
 * following format:
 * @code
 * {
 *     "terminalid":"1234",
 *     "type":"GET",
 *     "name":"listnontransit",
 *     "userid":"D123",
 *     "data":
 * {
 * "products":
 *     [
 *         { ...non-transit object... },
 *         { ...non-transit object... },
 *         ...
 *     ]
 *     }
 * }
 * @endcode
 */
std::string process_GET_listnontransit(const Json::Value &request);

/**
 * @brief Handles the "listsurcharge" message from the browser.
 *
 * Sends a list of all the surcharge products to the browser.
 *
 * @see http://cew.vix.local:8090/display/VM/Non-Transit+Product+Sale+and+Surcharge
 *
 * @param request A JSON object of the following format:
 * @code
 * {
 *     "terminalid":"1234",
 *     "type":"GET",
 *     "name":"listnontransit",
 *     "userid":"D123"
 * }
 * @endcode
 * @return JSON as a string, containing a list of non-transit products, in the 
 * following format:
 * @code
 * {
 *     "terminalid":"1234",
 *     "type":"GET",
 *     "name":"listnontransit",
 *     "userid":"D123",
 *     "data":
 * {
 * "products":
 *     [
 *         { ...non-transit object... },
 *         { ...non-transit object... },
 *         ...
 *     ]
 *     }
 * }
 * @endcode
 */
std::string process_GET_listsurcharge(const Json::Value &request);

std::string process_POST_Reader(Json::Value &request);
std::string process_POST_CCEEvent(Json::Value &request);
 
#define REST_API_PATH               "/api/api-docs.json/REST"
#define REST_API_PATH_STATUS        "/api/api-docs.json/REST/Status"

#define REST_API_PATH_READER        "/api/api-docs.json/REST/GACReader"
#define REST_API_PATH_CCEEVENT      "/api/api-docs.json/REST/CCEEvent"

#define TYPE_DRIVER_CONSOLE 1
#define TYPE_READER         2

#define TYPE_GET                        "get"
#define TYPE_PUT                        "put"
#define TYPE_SET                        "set"
#define TYPE_HB                         "hb"   // Heartbeat to force client detection of websocket failure

// Get only
#define METHOD_STATE                    "state"
#define METHOD_DEVICESTATUS             "devicestatus"  // REVIEW naming of method
#define METHOD_DEVICEMODE               "devicemode"  // REVIEW naming of method
#define METHOD_DEVICELIST               "devicelist"
#define METHOD_CARDEVENT                "cardevent"
#define METHOD_LOGIN                    "login"
#define METHOD_LOGOFF                   "logoff"
#define METHOD_CHANGEPIN                "changepin"
#define METHOD_REBOOT                   "reboot"
#define METHOD_TABLECARD                "tablecard"
#define METHOD_NETWORK                  "network"
#define METHOD_FORCECOMMS               "forcecomms"
#define METHOD_TRIP                     "trip"
#define METHOD_HEADLESSTRIP             "headlesstrip"
#define METHOD_STOPS                    "stops"
#define METHOD_SHIFTS                   "shifts"
#define METHOD_ALLSHIFTS                "allshifts"
#define METHOD_ENQUIRESTOPS             "enquirestops"
#define METHOD_ENQUIRETABLECARD         "enquiretablecard"
#define METHOD_VALIDATEPIN              "validatepin"
#define METHOD_OVERRIDESTOP             "overridestop"
#define METHOD_DRIVERBREAK              "driverbreak"
#define METHOD_CHANGEENDS               "changeends"
#define METHOD_CONFIG                   "config"
#define METHOD_PING                     "ping"
#define METHOD_ROUTES                   "routes"
#define METHOD_TPURSELOAD               "tpurseload"
#define METHOD_TPURSEREVERSAL           "tpurseloadreversal"
#define METHOD_TRANSACTIONLIST          "transactionlist"
#define METHOD_TOUCHONOFFMODE           "touchonoffmode"
#define METHOD_VALIDATETRANSITCARD      "validatetransitcard"
#define METHOD_COUNTPASSENGER           "countpassenger"
#define METHOD_READERSTATE              "readerstate"
#define METHOD_SHIFTEND                 "shiftend"
#define METHOD_GETDRIVERTOTALS          "getdrivertotals"
#define METHOD_PRINTLASTRECEIPT         "printlastreceipt"
#define METHOD_listnontransit           "listnontransit"
#define METHOD_listsurcharge            "listsurcharge"
#define METHOD_productsales             "productsales"
#define METHOD_productsalesreversal     "productsalesreversal"

// Get and Put
#define METHOD_MODE                     "mode"

struct libwebsocket_context *context;
CsMutex_t g_queueLock;
CsMutex_t g_rxQueueLock;
static std::string  g_card; 
bool s_passwordForceLogOff = false;

#define MAX_BUFFER_SIZE 327680 // 320k buffer otherwise we see errors on the connection when flooded with requests. (Route lists)
#define MAX_ROUTES 2500
#define WS_SEND_BLOCK_SIZE 0xFFFF
static bool s_touchOnOffEnabled = false;

/* callback_event_json protocol */

struct per_session_data__event_json {
    long id;
    bool open;
    long eventFilter;
    int  type; 
    
    bool in_fragment;
    int remaining;
    int i;
    int count;
    unsigned char data[MAX_BUFFER_SIZE];    // Fragment
};


std::map<int, int> 	            g_typeList;
CsMutex_t g_heartbeatMapLock;
std::map<std::string, time_t>           g_heartbeatMap;

std::map<int, std::queue<std::string> > g_TXQueues;
std::map<int, std::queue<std::string> > g_RXQueues;
std::map<int, std::string > 			g_fragmentVector;



static int callback_http(struct libwebsocket_context *context, struct libwebsocket *wsi, enum libwebsocket_callback_reasons reason, void *user, void *in, size_t len);
static int callback_event_dc(struct libwebsocket_context *context, struct libwebsocket *wsi, enum libwebsocket_callback_reasons reason, void *user, void *in, size_t len);
static int callback_event_reader(struct libwebsocket_context *context, struct libwebsocket *wsi, enum libwebsocket_callback_reasons reason, void *user, void *in, size_t len);
static int callback_event_json(struct libwebsocket_context *context, struct libwebsocket *wsi, enum libwebsocket_callback_reasons reason, void *user, void *in, size_t len);

struct per_session_data__http {
	int fd;
};

static struct libwebsocket_protocols protocols[] = {
	/* first protocol must always be HTTP handler */
	{"http-only",       callback_http,          sizeof (struct per_session_data__http),                    0,},
    {"dc-protocol",	    callback_event_dc,	    sizeof(struct per_session_data__event_json), MAX_BUFFER_SIZE,},      
    {"reader-protocol",	callback_event_reader,	sizeof(struct per_session_data__event_json), MAX_BUFFER_SIZE,},    
	{"json-protocol",   callback_event_json,	sizeof(struct per_session_data__event_json), MAX_BUFFER_SIZE,},
	{ NULL, NULL, 0, 0 } /* terminator */
};

struct serveable {
	const char *urlpath;
	const char *mimetype;
}; 

static const struct serveable defaultPage = { "/app/index.html", "text/html" };
static const struct serveable apiPage     = { "/api.html", "text/html" };

/**
 * @brief Populates an IPC_Company_Details_t struct, which is used by the printer 
 *        API, with data from a request object.
 * @param request A JSON object containing data to populate companyDetails with.
 * @param companyDetails The struct to populate.
 * @return 0 always (in future, if failures occurred, -1 would be returned)
 */
static int populatePrinterCompanyDetails(Json::Value &request, 
        IPC_Company_Details_t &companyDetails)
{
    strncpy(companyDetails.companyPhone, g_companyPhone, boost::size(companyDetails.companyPhone));
    strncpy(companyDetails.companyName, g_companyName, boost::size(companyDetails.companyName));
    strncpy(companyDetails.companyABN, g_companyABN, boost::size(companyDetails.companyABN));
    strncpy(companyDetails.companyWebSite, g_companyWebsite, boost::size(companyDetails.companyWebSite));

    CsDbg( 6, "populatePrinterCompanyDetails : companyPhone=%s",   companyDetails.companyPhone   );
    CsDbg( 6, "populatePrinterCompanyDetails : companyName=%s",    companyDetails.companyName    );
    CsDbg( 6, "populatePrinterCompanyDetails : companyABN=%s",     companyDetails.companyABN     );
    CsDbg( 6, "populatePrinterCompanyDetails : companyWebSite=%s", companyDetails.companyWebSite );

    return 0;
}

/**
 * @brief Populates an IPC_Common_Details_t struct, which is used by the printer
 *        API, with data from a request object.
 * @param request A JSON object containing data to populate commonDetails with.
 * @param companyDetails The struct to populate.
 * @return 0 on success, -1 on failure.
 */
static int populatePrinterCommonDetails(Json::Value &request, 
        IPC_Common_Details_t &commonDetails, const std::string &salesMessage)
{
    try {
        strncpy(commonDetails.busID, g_vehicleId, IPC_STRING_MAX_LENGTH);

        commonDetails.terminalID = lexical_cast<int>(request["terminalid"].asString());

        if (!s_tablecard["shift_number"].isNull())
        {
            commonDetails.shiftNumber = lexical_cast<int>(s_tablecard["shift_number"].asString());
        }
    }
    catch (boost::bad_lexical_cast &e)
    {
        CsErrx("populatePrinterCommonDetails: %s", e.what());
        return -1;
    }

    if (request["data"]["revtxnseqno"].isNull())
    {
	    commonDetails.transactionNumber = request["data"]["saleseqno"].asInt();
    }
    else
    {
	    commonDetails.transactionNumber = request["data"]["revsaleseqno"].asInt();
    }
    
    commonDetails.transactionDate = time(NULL);

	strncpy(commonDetails.mykiReference,
            request["data"]["cardnumber"].asCString(), IPC_STRING_MAX_LENGTH - 1);

    strncpy(commonDetails.location, getLocation().c_str(), IPC_STRING_MAX_LENGTH - 1);

	CsDebug(4, (4,"location %s", commonDetails.location));

    strncpy(commonDetails.salesMessage, salesMessage.c_str(), IPC_STRING_MAX_LENGTH -1);

    return 0;
}

int processWebsockets()
{
    for(int i =SERVER_PROTOCOL_FIRST;i<SERVER_PROTOCOL_COUNT;i++)
    {
        libwebsocket_callback_on_writable_all_protocol(&protocols[i]);
    }

    int n = libwebsocket_service(context, 50); // Returns immediately when an idle connection is connected... revert to timer implementation
    if(n!=0)
        CsDebug(5, (5, "libwebsocket_service return %d", n));
    return n;    
}

int32_t CsStrTimeISO(const CsTime_t *pTm, int32_t len, char *pBuffer )
{
     struct tm tmBuf;
     int ret;
     time_t tempSecs;
 
     if (pBuffer == NULL)
     {
         errno = EINVAL;
         return (-1);
     }

	 int tz =  -(pTm->tm_tzone/60); // timezone offset appears to be inverted.

     tempSecs = pTm->tm_secs;
     (void) localtime_r(&tempSecs, &tmBuf);
     ret = snprintf(pBuffer, len,
             "%04d-%02d-%02dT%02d:%02d:%02d%s%02d:%02d",
				tmBuf.tm_year+1900, tmBuf.tm_mon+1, tmBuf.tm_mday,  
                tmBuf.tm_hour, tmBuf.tm_min, tmBuf.tm_sec, 
				tz>0?"+":"-", 
				abs((tz - (tz%60))/60) , 
				tz%60); 
 
     return ret;
}

int32_t CsStrTimeSimple(const CsTime_t *pTm, int32_t len, char *pBuffer )
{
     struct tm tmBuf;
     int ret;
     time_t tempSecs;
 
     if (pBuffer == NULL)
     {
         errno = EINVAL;
         return (-1);
     }

     tempSecs = pTm->tm_secs;
     (void) localtime_r(&tempSecs, &tmBuf);
     ret = snprintf(pBuffer, len,
             "%02d/%02d/%04d %02d:%02d:%02d %s",
				tmBuf.tm_mday, tmBuf.tm_mon+1, tmBuf.tm_year+1900,  
                tmBuf.tm_hour < 13 ? ( tmBuf.tm_hour ) : ( tmBuf.tm_hour % 12 ), tmBuf.tm_min, tmBuf.tm_sec, (tmBuf.tm_hour<12)?"am":"pm"); 
 
     return ret;
}

LWS_VISIBLE void lwsl_log_handler(int level, const char *line)
{
	if(level==LLL_ERR)          //LL_ERR==0
		CsErrx(line);
	else if(level==LLL_WARN)    //LLL_WARN==1
		CsWarnx(line);
	else // The rest map from notice=2 to latency 9.
		CsDebug(level, (level, line));
}

static struct lws_context_creation_info lws_info;
    
int _initialiseWebsocket(lws_context_creation_info *pInfo)
{
    if(pInfo==NULL) return -1;   
    
    CsDebug(1, (1, "_initialiseWebsocket %d",  g_httpPort)); 
    
    memset(pInfo, 0, sizeof *pInfo);
	pInfo->port       = g_httpPort;
    pInfo->iface      = NULL; // Allow connection from any interface
	pInfo->protocols  = protocols;    
    lws_set_log_level(0, lwsl_log_handler);
    
	context = libwebsocket_create_context(pInfo);
	if (context == NULL) 
    {
		CsErrx("libwebsocket_create_context failed");
		return -1;
	}    
    
    return 0;
}    

int InitialiseWebsocket()
{
    CsDebug(1, (1, "InitialiseWebsocket")); 
	setlogmask(LOG_UPTO (LOG_DEBUG));
    int ret = _initialiseWebsocket(&lws_info);
    if(ret!=0) // Initialise here to ensure that the socket is available
    {
		CsErrx("libwebsocket init failed %d", ret);
		return -1;
	}        

    return 0;
}

void writeResponse_200OK(struct libwebsocket* wsi, const char* response)
{
    char buf[1024];

    snprintf(buf, 1024, 
        "HTTP/1.0 200 OK\x0d\x0a"
        "Server: VIX\x0d\x0a"
        "Access-Control-Allow-Origin: *\x0d\x0a"
        "Content-Type: application/json\x0d\x0a"
            "Content-Length: %u\x0d\x0a\x0d\x0a"
            "%s",
            (unsigned int)strlen(response), response);

    CsDebug(2, (2, "REST API: write %s",buf));   
    libwebsocket_write(wsi, (unsigned char*) buf, strlen(buf), LWS_WRITE_HTTP);    
}

void writeResponse_Error(struct libwebsocket* wsi, int responseCode)
{
    char buf[1024];
    if(responseCode==400)
    {
        snprintf(buf, 1024, "HTTP/1.0 400 ID Not found\x0d\x0a");
    }
    else  if(responseCode==404)
    {
        snprintf(buf, 1024, "HTTP/1.0 404 Not found\x0d\x0a");
    }
    else //if(responseCode==500)
    {
        snprintf(buf, 1024, "HTTP/1.0 500 Internal server error\x0d\x0a");
    }        
    
    CsDebug(2, (2, "REST API: write %s",buf));   
    libwebsocket_write(wsi, (unsigned char*) buf, strlen(buf), LWS_WRITE_HTTP);    
}

// Case insensitive REST API comparison
bool isApi(const char* uri, const char* api)
{
	return strncasecmp(uri, api, strlen(api))==0;
}

// Get the ID (the string after the last slash) on the request url 
char* getID(char* uri)
{
    char *token;
    char seps[] = "/";
    char* result = NULL;
	char* saveptr;
    
    token = strtok_r(uri, seps, &saveptr);
    while(token!=NULL)
    {
        if(token!=NULL)
            result = token;        
        token = strtok_r(NULL, seps, &saveptr); 
    }
    
    return result;   
}

int processREST_GET(struct libwebsocket* wsi, const char* uri)
{
    Json::Value request;   // will contains the root value after parsing.

    char uriCopy[256];
    strncpy(uriCopy, uri, 256);
    char *id = getID(uriCopy);
    
    CsDebug(6, (6, "processREST_GET URI %s content is %s null", uri, wsi->content==NULL?"":"not"));

    if(id!=NULL)
    {
        request["terminalid"] = std::string(id);
    }

    writeResponse_Error(wsi, 404);

    return -1; // -1 means the connection is closed.  IN asynch mode this will return zero until an response is retrieved.
}

int processREST_POST(struct libwebsocket* wsi, const char* uri)
{
    Json::Value request;   // will contains the root value after parsing.
	Json::Reader reader;
    
    CsDebug(6, (6, "processREST_POST URI %s content is %s null", uri, wsi->content==NULL?"":"not"));
    if(wsi->content!=NULL)
    {
        if(reader.parse(wsi->content,request) && request.size()>0) // Must check size otherwise library can segfault
        {    
            if(isApi(uri, REST_API_PATH_READER))
            {
                writeResponse_200OK(wsi, process_POST_Reader(request).c_str());
            }    
            else if(isApi(uri, REST_API_PATH_CCEEVENT))
            {
                writeResponse_200OK(wsi, process_POST_CCEEvent(request).c_str());
            }
        }
        else
        {
            writeResponse_Error(wsi, 400);
        }
    }
    else
    {
        writeResponse_Error(wsi, 400);
    }
    
    return -1;
}

int processREST_PUT(struct libwebsocket* wsi, const char* uri)
{
    Json::Value request;   // will contains the root value after parsing.
	Json::Reader reader;
     
    CsDebug(6, (6, "processREST_PUT URI %s content is %s null", uri, wsi->content==NULL?"":"not"));
    
    if(reader.parse(wsi->content,request) && request.size()>0) // Must check size otherwise library can segfault
    {    
        request["type"] = Json::Value("PUT");
        if(request.get("terminalid", "").asString().length()==0) // If there is no terminal id already
        {
            char uriCopy[256];
            strncpy(uriCopy, uri, 256);
            char *id = getID(uriCopy);
            if(id!=NULL)
            {
                request["terminalid"] = Json::Value(id);
            }
        }
        
        // NO PUT APIs supported at the moment.
      
        writeResponse_Error(wsi, 404);
      
    }    
    return -1;
}

int processREST(struct libwebsocket* wsi, const char* uri)
{
    try
    {       
        if(wsi->http_method==WSI_TOKEN_GET_URI)
        {
            return processREST_GET(wsi, uri);
        }
        else if(wsi->http_method==WSI_TOKEN_PUT_URI )
        {
            return processREST_PUT(wsi, uri);
        }
        else if(wsi->http_method==WSI_TOKEN_POST_URI)
        {
            return processREST_POST(wsi, uri);
        }
        else // DELETE not supported for this API
        {
            writeResponse_Error(wsi, 404);
        }
    }
    catch(std::runtime_error &e)
    {
        CsErrx("processREST caught runtime error: ", e.what());
        writeResponse_Error(wsi, 500); // Internal Server Error
    }
    return -1;                    
}

static int callback_http(struct libwebsocket_context *context,
		struct libwebsocket *wsi,
		enum libwebsocket_callback_reasons reason, void *user,
							   void *in, size_t len)
{
#if 0 // Enable if connection filtering is required.
	char client_name[128];
	char client_ip[128];
#endif
	char buf[1024];

	int n, m;
	
	static unsigned char buffer[32768];
	
	struct per_session_data__http *pss =
			(struct per_session_data__http *)user;

	switch (reason) {
	case LWS_CALLBACK_HTTP:
        {
            char *requested_uri = (char *) in;
            
            CsDebug(2, (2, "requested URI %s",requested_uri));
           
            if (strcmp(requested_uri, "/") == 0) 
            {
                snprintf(buf, 1024, "%s%s", g_resourcePath.c_str(), defaultPage.urlpath);
               
                if (libwebsockets_serve_http_file(context, wsi, buf, defaultPage.mimetype))
                    return -1; /* through completion or error, close the socket */
            } 
            else if (strcmp(requested_uri, "/api") == 0) 
            {    
                snprintf(buf, 1024, "%s%s", g_resourcePath.c_str(), apiPage.urlpath);
               
                if (libwebsockets_serve_http_file(context, wsi, buf, apiPage.mimetype))
                    return -1; /* through completion or error, close the socket */                
            }    
            else if (strncmp(requested_uri, REST_API_PATH, strlen(REST_API_PATH)) == 0) 
            {                 
                return processREST(wsi,requested_uri);
            }            
            else 
            {
                // allocate enough memory for the resource path
                snprintf(buf, 1024, "%s%s", g_resourcePath.c_str(), requested_uri);
               
                CsDebug(2, (2, "resource path: %s",buf));
               
                char *extension = strrchr(buf, '.');
                std::string mime;
               
                // choose mime type based on the file extension
                if (extension == NULL)                      { mime = "text/plain";} 
                else if (strcmp(extension, ".png") == 0)    { mime = "image/png";} 
                else if (strcmp(extension, ".jpg") == 0)    { mime = "image/jpg";} 
                else if (strcmp(extension, ".gif") == 0)    { mime = "image/gif";} 
                else if (strcmp(extension, ".html") == 0)   { mime = "text/html";} 
                else if (strcmp(extension, ".css") == 0)    { mime = "text/css";}
                else if (strcmp(extension, ".js") == 0)     { mime = "application/javascript";}
                else if (strcmp(extension, ".json") == 0)   { mime = "application/json";} 
                else                                        { mime = "text/plain";}
                    
                if (libwebsockets_serve_http_file(context, wsi, buf, mime.c_str()))
                    return -1; /* through completion or error, close the socket */
            }
            /*
             * notice that the sending of the file completes asynchronously,
             * we'll get a LWS_CALLBACK_HTTP_FILE_COMPLETION callback when
             * it's done
             */
        }
		break;

	case LWS_CALLBACK_HTTP_FILE_COMPLETION:
		/* kill the connection after we sent one file */
		return -1;  // TODO reconsider

	case LWS_CALLBACK_HTTP_WRITEABLE:
		/*
		 * we can send more of whatever it is we were sending
		 */

		do {
			n = read(pss->fd, buffer, sizeof buffer);
			/* problem reading, close conn */
			if (n < 0)
				goto bail;
			/* sent it all, close conn */
			if (n == 0)
				goto bail;
			/*
			 * because it's HTTP and not websocket, don't need to take
			 * care about pre and postamble
			 */
			m = libwebsocket_write(wsi, buffer, n, LWS_WRITE_HTTP);
			if (m < 0)
				/* write failed, close conn */
				goto bail;
			if (m != n)
				/* partial write, adjust */
				lseek(pss->fd, m - n, SEEK_CUR);

		} while (!lws_send_pipe_choked(wsi));
		usleep(10*1000); // Sleep 10 ms. // HACK Throttling of bad connections to maintain availability
		libwebsocket_callback_on_writable(context, wsi);
		break;

bail:
		close(pss->fd);
		return -1;

	/*
	 * callback for confirming to continue with client IP appear in
	 * protocol 0 callback since no websocket protocol has been agreed
	 * yet.  You can just ignore this if you won't filter on client IP
	 * since the default uhandled callback return is 0 meaning let the
	 * connection continue.
	 */

	case LWS_CALLBACK_FILTER_NETWORK_CONNECTION:
#if 0
		libwebsockets_get_peer_addresses(context, wsi, (int)(long)in, client_name,
			     sizeof(client_name), client_ip, sizeof(client_ip));

		fprintf(stderr, "Received network connect from %s (%s)\n",
							client_name, client_ip);
#endif
		/* if we returned non-zero from here, we kill the connection */
		break;

	default:
		break;
	}

	return 0;
}

static int getNewID(int type) 
{
    static int id =0;
    CsMutexLock(&g_queueLock);
        g_TXQueues[id] = std::queue<std::string>();
        g_typeList[id] = type;
    CsMutexUnlock(&g_queueLock);
    CsMutexLock(&g_rxQueueLock);
        g_RXQueues[id] = std::queue<std::string>();
    CsMutexUnlock(&g_rxQueueLock);    
    return id++;
}

static void releaseID(int id)  // TODO recycle ids.
{
    CsMutexLock(&g_queueLock);
        g_TXQueues.erase(id);
        g_typeList.erase(id);       
    CsMutexUnlock(&g_queueLock);
    
    CsMutexLock(&g_rxQueueLock);
        g_RXQueues.erase(id);
    CsMutexUnlock(&g_rxQueueLock);
}

int getMessage(unsigned int id, unsigned char *p, int maxLength)
{
    int returnValue = 0;
    
    CsMutexLock(&g_queueLock);
    map<int, std::queue<std::string> >::iterator iter = g_TXQueues.find(id); 
    if (g_TXQueues.end() != iter ) 
    {
        if(iter->second.empty()==false)
        {
            returnValue = snprintf((char*)p, maxLength, "%s", iter->second.front().c_str());
            iter->second.pop();
        }
    }   
    CsMutexUnlock(&g_queueLock);
    return returnValue;
}

int getReceiveMessage(unsigned int id, std::string &buf)
{    
    int returnValue = 0;
    CsMutexLock(&g_rxQueueLock);
    map<int, std::queue<std::string> >::iterator iter = g_RXQueues.find(id);
    if (g_RXQueues.end() != iter ) 
    {
        if(iter->second.empty()==false)
        {
            buf = iter->second.front();
            iter->second.pop();
            returnValue = 1;
        }
    }
    CsMutexUnlock(&g_rxQueueLock);
    return returnValue;
}

// Sends a response to a specified listener id.
void sendMessage(int id, std::string message)
{   
    CsDebug(4, (4, "sendMessage %d %s", id, message.c_str()));

	CsMutexLock(&g_queueLock);
    map<int, std::queue<std::string> >::iterator iter = g_TXQueues.find(id);
    if (g_TXQueues.end() != iter ) 
    {
        iter->second.push(message);       
    }
    else
    {
        CsDebug(2, (2, "sendMessage %d not found", id));
    }
	CsMutexUnlock(&g_queueLock);	
}

static int
callback_event_json(struct libwebsocket_context *context,
			struct libwebsocket *wsi,
			enum libwebsocket_callback_reasons reason,
					       void *user, void *in, size_t len)
{
	int n, m;
	unsigned char buf[LWS_SEND_BUFFER_PRE_PADDING + MAX_BUFFER_SIZE + LWS_SEND_BUFFER_POST_PADDING];
	unsigned char *p = &buf[LWS_SEND_BUFFER_PRE_PADDING];
	struct per_session_data__event_json *pss = (struct per_session_data__event_json *)user;

	switch (reason) 
    {
    case LWS_CALLBACK_ESTABLISHED:
        {
            //CsDebug(2, (2, "callback_event_json: LWS_CALLBACK_ESTABLISHED type %d", pss->type));
            pss->open = true;
            pss->id = getNewID(pss->type);
            pss->in_fragment = false;
            pss->remaining = 0;
            pss->i         = 0;
            pss->count     = 0;
   
            g_fragmentVector[pss->id] = std::string("");
     
        }
		break;
    case LWS_CALLBACK_CLOSED:
        CsDebug(6, (6, "callback_event_json: LWS_CALLBACK_CLOSED"));
		releaseID(pss->id);
        pss->open = false;  
        break;
    case LWS_CALLBACK_FILTER_PROTOCOL_CONNECTION:
        CsDebug(6, (6, "callback_event_json: LWS_CALLBACK_FILTER_PROTOCOL_CONNECTION"));
        // Ignore
        break;
	case LWS_CALLBACK_SERVER_WRITEABLE:
        //CsDebug(6, (6, "callback_event_json: LWS_CALLBACK_SERVER_WRITEABLE"));
        
        if(pss->in_fragment || (n = getMessage(pss->id, pss->data, MAX_BUFFER_SIZE))>0)
        {
            int sendSize = 0;
            libwebsocket_write_protocol flags  = (libwebsocket_write_protocol)(((int)LWS_WRITE_NO_FIN)|((int)LWS_WRITE_TEXT));
            
            if(pss->in_fragment)
            {
                pss->count++;
                flags = (libwebsocket_write_protocol)(((int)LWS_WRITE_NO_FIN)|((int)LWS_WRITE_CONTINUATION));
            }
            else
            {
                pss->count = 0;
                pss->remaining = n;
                pss->i =0;                
            }
            
            // Note fragments are not supported by the webkit browser in Qt 4.8
            if(pss->remaining<WS_SEND_BLOCK_SIZE)
            {
                // Return value from libwebsocket_write is too inconsistent, assume 4k buffers
                sendSize = pss->remaining;
                flags = pss->count==0?LWS_WRITE_TEXT:LWS_WRITE_CONTINUATION;
                memcpy(p, pss->data+pss->i, pss->remaining);
                m = libwebsocket_write(wsi, p, pss->remaining, flags);
            }
            else
            {
                sendSize = WS_SEND_BLOCK_SIZE;
                memcpy(p, pss->data+pss->i, WS_SEND_BLOCK_SIZE);
                m = libwebsocket_write(wsi, p, WS_SEND_BLOCK_SIZE, flags);
            }

            if (m < 0) 
            {
                CsDebug(1, (1, "callback_event_json: LWS_CALLBACK_SERVER_WRITEABLE ERROR %d writing to the socket", m));
                pss->remaining   = 0;
                pss->in_fragment = false;
            }
            else
            {
                pss->in_fragment = false;
            }            

            // Header size varies
            pss->i         += sendSize;
            pss->remaining -= sendSize;
            
            CsDebug(9, (9, "callback_event_json: m %d  i %d rem %d pre %d post %d", m,pss->i,pss->remaining, LWS_SEND_BUFFER_PRE_PADDING, LWS_SEND_BUFFER_POST_PADDING));
            pss->in_fragment = pss->remaining>0;
        }
		break;

	case LWS_CALLBACK_RECEIVE:      
        {    
            //CsDebug(9, (9, "LWS_CALLBACK_RECEIVE %d %s", len, (const char*)in));
            const size_t remaining = libwebsockets_remaining_packet_payload(wsi);
            
            g_fragmentVector[pss->id].append((const char*) in, len);
            if(remaining==0 && libwebsocket_is_final_fragment(wsi))
            {
                IPC_JSON_t m = { {IPC_WS_RECEIVE_EVENT, (TaskID_t)DC_CMD_TASK }};
                m.id = pss->id;
               

                CsMutexLock(&g_rxQueueLock);
                g_RXQueues[pss->id].push(g_fragmentVector[pss->id]);
                CsMutexUnlock(&g_rxQueueLock);
                
                g_fragmentVector[pss->id].clear();
                IPCSend(wsQueueId, &m, sizeof m);                     
            }
        }
		break;
    case LWS_CALLBACK_PROTOCOL_INIT:
        CsDebug(6, (6, "callback_event_json: LWS_CALLBACK_PROTOCOL_INIT"));
        break;
    case LWS_CALLBACK_PROTOCOL_DESTROY:
        CsDebug(6, (6, "callback_event_json: LWS_CALLBACK_PROTOCOL_DESTROY"));
        break;        
	default:
        CsDebug(1, (1, "Unhandled callback_event_json: %d", reason));
		break;
	}

	return 0;
}


static int
callback_event_type(struct libwebsocket_context *context,
			struct libwebsocket *wsi,
			enum libwebsocket_callback_reasons reason,
					       void *user, void *in, size_t len, int type)
{
	struct per_session_data__event_json *pss = (struct per_session_data__event_json *)user;

	switch (reason) 
    {
    case LWS_CALLBACK_ESTABLISHED:
        {
            CsDebug(3, (3, "callback_event_dc: LWS_CALLBACK_ESTABLISHED set as driver console"));
            pss->type = type;
        }
		break;
    default: 
        break;
	}

	return callback_event_json(context,wsi, reason, user, in, len);
}

static int
callback_event_dc(struct libwebsocket_context *context,
			struct libwebsocket *wsi,
			enum libwebsocket_callback_reasons reason,
					       void *user, void *in, size_t len)
{
	return callback_event_type(context,wsi, reason, user, in, len, TYPE_DRIVER_CONSOLE);
}

static int
callback_event_reader(struct libwebsocket_context *context,
			struct libwebsocket *wsi,
			enum libwebsocket_callback_reasons reason,
					       void *user, void *in, size_t len)
{
	return callback_event_type(context,wsi, reason, user, in, len, TYPE_READER);
}

// Sends an event to all DC listeners
void sendMessageByType(std::string message, int type)
{
    CsDebug(5, (5, "sendMessageByType %s", message.c_str()));
	CsMutexLock(&g_queueLock);
    typedef std::map<int, int >::iterator it_type;
    
    for(it_type iterator = g_typeList.begin(); iterator != g_typeList.end(); iterator++)
    {
        if(type==iterator->second)
        {
            int id = iterator->first;
                        
            map<int, std::queue<std::string> >::iterator txIter = g_TXQueues.find(id);
            if (g_TXQueues.end() != txIter ) /// Send to any that has any elements (TODO Revise)
            {
                txIter->second.push(message);       
            }  
            else
            {
                CsDebug(2, (2, "id %d not found", iterator->first));
            }
        }
    }
	CsMutexUnlock(&g_queueLock);	
}

// Sends an event to all DC listeners
void sendDCMessage(std::string message)
{
    sendMessageByType(message, TYPE_DRIVER_CONSOLE);
}

void sendReaderMessage(std::string message)
{
    sendMessageByType(message, TYPE_READER);
}

void sendSimpleEvent(const char* event)
{
    Json::Value ev;
    ev["event"] = event;
    
    Json::FastWriter fw;
    sendDCMessage(fw.write(ev));
}

// Sends an event to all listeners
void sendEvent(std::string message)
{
	CsMutexLock(&g_queueLock);
    CsDebug(2, (2, "sendEvent (%d queues) %s", g_TXQueues.size(), message.c_str()));
    typedef std::map<int, std::queue<std::string> >::iterator it_type;
    
    for(it_type iterator = g_TXQueues.begin(); iterator != g_TXQueues.end(); iterator++)
    {
        iterator->second.push(message);
    }
	CsMutexUnlock(&g_queueLock);	
}

void createBaseEvent(Json::Value &request, Json::Value &response)
{
	CsTime_t t;
    CsTime(&t);
	char timeBuf[64];
	CsStrTimeISO(&t,64,timeBuf);
    
    response["terminalid"]  = 0;
    response["timestamp"]   = timeBuf;
    response["type"]        = "event";
    response["data"]        = Json::Value();
}

    /**
     *  @brief  Processes login by operator card or password of the day.
     *  @param  autochangeend true if driver is changing end; false otherwise.
     *  @param  candidateServiceProviderId login candidate service provider id.
     *  @param  candidateStaffId login candidate staff id.
     *  @param  candidateTerminalId login candidate terminal id.
     *  @param  passwordLogin true if login by password of the day;
     *          false if login by operator card.
     *  @param  role login candidate role.
     *  @param  roles available roles.
     *  @param  candidateCSN login candidate card serial number (if login by
     *          operator card).
     *  @return JSON values containing login details.
     */
Json::Value login(
    bool            autochangeend,
    std::string     candidateServiceProviderId,
    std::string     candidateStaffId,
    std::string     candidateTerminalId,
    bool            passwordLogin,
    std::string     role,
    Json::Value     roles,
    std::string     candidateCSN )
{
    Json::FastWriter fw;
    Json::Value data;
    Json::Value shifts;
    CsDebug(3, (3, "login  %s %s %s", candidateServiceProviderId.c_str(), candidateStaffId.c_str(), s_activeTerminalId.c_str()));
    
    char sp[33];
    char staffid[33];
    snprintf(sp, 32, "%s", candidateServiceProviderId.c_str());
    snprintf(staffid, 32, "%s", candidateStaffId.c_str());
    
    if(role=="driver")
        shifts = MYKI_Schedule_getShifts(g_scheduleDatabase, sp, staffid);
    
    if ( s_activeStaffId.empty() == false && candidateStaffId != s_activeStaffId )
    {
        /*  Operator is logging in while a Driver is currently logged in */
        CsDebug(3, (3, "processShiftEnd done for %s", s_activeTerminalId.c_str()));
        bool oldRole = (role == "driver");
        processShiftEnd(oldRole);
        sendLogOffMessages(false,true, "");
    }
        
    if(s_activeTerminalId!="")
    {            
        if(candidateTerminalId.compare(s_activeTerminalId)!=0 || // If login is on the other terminal 
            s_DCState == DCSTATE_TECHNICIAN) // or if a login occurs on the same terminal (edge case on browser restart).
        { 
            sendLogOffMessages(true, false, candidateTerminalId);
        }
    }
    
    s_activeTerminalId          = candidateTerminalId;
    s_activeStaffId             = candidateStaffId;
    s_activeCSN                 = candidateCSN;
    s_activeServiceProviderId   = candidateServiceProviderId;
    s_activeType                = role;
    s_activeRoles               = roles;         
    s_passwordLogin             = passwordLogin;
    
    CsDebug(2, (2, "login - terminal %s user %s sp %s type %s ", s_activeTerminalId.c_str(),s_activeStaffId.c_str(), s_activeServiceProviderId.c_str(), s_activeType.c_str()));    
    if(s_activeRoles.empty()==false && passwordLogin==false)
    {
        for(Json::Value::ArrayIndex i=0;i<s_activeRoles.size();i++)
        {
            if(s_activeRoles[i]["profile"].empty()==false && s_activeRoles[i]["type"].empty()==false)
            {
                int profile = atoi(s_activeRoles[i]["profile"].asString().c_str()); // TODO Improved Error checking
                int type    = atoi(s_activeRoles[i]["type"].asString().c_str());

                s_activeRoles[i]["permissions"] = getPermissions(profile, type);
            }
        }
    }
       
    CsDebug(2, (2, "login - send state event"));
    // Workaround notify front end that a driver has logged in and therefore the card details screen needs to be shown.
    sendStateEvent(role.compare("driver")==0); // Reworked here to ensure that the terminal and staff setting are changed before sending the operational event.
  
    Json::Value logon;
    logon["name"]               = "logon";
    logon["terminalid"]         = s_activeTerminalId;
    logon["operatorid"]         = s_activeStaffId;
    logon["serviceproviderid"]  = s_activeServiceProviderId;
    logon["role"]               = role;   
    
    logon["autochangeend"]      = autochangeend;
    logon["passwordlogin"]      = s_passwordLogin;
    logon["roles"]              = s_activeRoles;
    sendDCMessage(fw.write(logon));

    //  Let READERAPP knows a Driver has logged in
    sendReaderMessage( fw.write( logon ) );
      
    data["shifts"]          = shifts;
    data["type"]            = "operator";
    data["role"]            = role;
    data["roles"]           = s_activeRoles;
    
  
    data["staffid"]         = s_activeStaffId;
    if (!s_activeCSN.empty())
        data["CSN"]         = s_activeCSN;
    data["autochangeend"]   = autochangeend;
    data["processing"]      = false;
    data["result"]          = true; 
    
    persistData(true);
    persistRouteData(true);
    return data;
}

std::string process_GET_login(Json::Value &request)
{
    Json::FastWriter fw;
	Json::Value response = createBaseResponse(request);
    Json::Value  roles, serviceproviders;
    bool validLogin=false;
    bool autochangeend = false;
    bool passwordLogin = false;
    
    CsDebug(1, (1, "process_GET_login %s", fw.write(request).c_str()));

    Json::Value   POTDrole = Json::Value();

    std::string role;
    std::string candidateCSN = "";
    
    std::string candidateTerminalId         = request["terminalid"].asString();
    std::string candidateStaffId            = request["data"]["staffid"].asString();
    std::string candidateServiceProviderId  = request["data"]["serviceprovider"].asString();    

    if(request["data"]["autochangeend"].empty()==false && request["data"]["autochangeend"].isBool())
    {
        autochangeend = request["data"]["autochangeend"].asBool();
    }
 
    response["data"]["processing"] = false; 
    
    if(autochangeend)
    {
        role    = s_activeType;
        roles   = s_activeRoles;
        candidateCSN = s_activeCSN;
        
        validLogin = true;
    }
    else if(request["data"]["PIN"].empty()==false)
    {
        if(validatePIN(candidateTerminalId, request["data"]["PIN"].asString()))
        {
            response["data"]["result"]      = true; 
            response["data"]["processing"]  = true; 
            response["data"]["error"]       = "processing";    
        }
        else
        {
            response["data"]["result"]      = false; 
            response["data"]["processing"]  = false; 
            response["data"]["error"]       = "Invalid request";        
        }
        // return processing message and wait for reader to respond.  (A timeout may also be required)
        return fw.write(response);   
       
    }
    else if(request["data"]["password"].empty()==false || request["data"]["operatorId"].empty()==false)
    {
        std::stringstream spId;
        spId << g_serviceProviderId;
        candidateServiceProviderId = spId.str(); 
        candidateStaffId = request["data"]["operatorId"].asString();
        passwordLogin = true;
        
        if(candidateStaffId.compare(s_activeStaffId)==0) autochangeend = true;
        
        POTDrole=validatePasswordOfTheDay(candidateStaffId, request["data"]["password"].asString());
        if(POTDrole.empty()==false)
        {
            validLogin = true;
            
            if(POTDrole["technician"].empty()==false && POTDrole["technician"].asInt()!=0)
                role = "technician";
            else if(POTDrole["supervisor"].empty()==false && POTDrole["supervisor"].asInt()!=0)
                role = "supervisor";
            else if(POTDrole["driver"].empty()==false && POTDrole["driver"].asInt()!=0)
                role = "driver";        
            else
                validLogin = false;
            roles.append(POTDrole);       
        }
    }
    
    if(validLogin)
    {
        // TODO Revise code to be simpler.  If this is a tech login and the state is already tech or we can transition to tech thne proceed
        // or... if we can proceed to operational
        if
        (
            ( role == "technician" && processStateChange( DCSTATE_TECHNICIAN,  true ) == 0 ) ||
            ( role == "supervisor" && processStateChange( DCSTATE_SUPERVISOR,  true ) == 0 ) || 
            (                         processStateChange( DCSTATE_OPERATIONAL, true ) == 0 )
        )
        {
            response[ "data" ]  = login( autochangeend, candidateServiceProviderId, candidateStaffId,
                                            candidateTerminalId, passwordLogin, role, roles, candidateCSN );
            switch ( s_DCState )
            {
            case    DCSTATE_TECHNICIAN:
            case    DCSTATE_SUPERVISOR:
                /*  Starts headless mode operation */
                setHeadlessMode( );
                break;

            default:
                /*  Others, DONOTHING! */
                break;
            }
        }
        else
        {
            response["data"]["result"] = false; 
            switch(s_DCState)
            {
                case DCSTATE_OUT_OF_ORDER:
                    response["data"]["error"]  = "Out of order";
                    break;     
                case DCSTATE_TECHNICIAN:
                    response["data"]["error"]  = "Cannot login at this time. Technician is logged in.";    
                    break;         
				case DCSTATE_SUPERVISOR:
					response["data"]["error"]  = "Cannot login at this time. Supervisor is logged in.";    
                    break;
                default:
                    response["data"]["error"]  = "Cannot login at this time";
                    break;
            }
        }            
    }
    else
    {
        response["data"]["result"] = false; 
        
        if(passwordLogin)
        {
            if(s_passwordFailureCount>=3)
            {
                response["data"]["error"]  =  "Password login disabled"; 
                lockPasswordEntry(g_InvalidPODTimeout*60);
            }
            else
            {
                response["data"]["error"]  =  "Invalid Password";
            }
        }
        else
        {
			switch (request["errorcode"].asInt())
			{
			case VALIDATE_PIN_ERROR:
				response["data"]["error"]  =  "System Error";
				break;
			case  VALIDATE_PIN_ERROR_INVALID:
				response["data"]["error"]  =  "Invalid PIN";
				break;
			case  VALIDATE_PIN_ERROR_BLOCKED:
				response["data"]["error"]  =  "Operator Application Blocked";
				break;
			case  VALIDATE_PIN_ERROR_NOT_OPERATOR_CARD:
				response["data"]["error"]  =  "Not An Operator Card";
				break;
			case VALIDATE_PIN_ERROR_OPEN_OPERATOR_CARD:
				response["data"]["error"]  =  "Failed Opening the Operator Card";
				break;
			default:
				response["data"]["error"]  =  "Unknown Error";
				break;
        	}
        }
    }

    response["passwordlogin"] = passwordLogin;

    return fw.write(response);        
}

std::string process_GET_validatepin(Json::Value &request)
{
    Json::Value dcMessage;
    Json::FastWriter fw;
   
    std::string terminalId=request["terminalid"].asString();
	CsTime_t t;
    CsTime(&t);
	char timeBuf[64];
	CsStrTimeISO(&t,64,timeBuf);    
    dcMessage["name"]        = "validatepin";
    dcMessage["terminalid"]  = terminalId;
    dcMessage["timestamp"]   = timeBuf;
    dcMessage["type"]        = "event";
    dcMessage["data"]        = Json::Value();   

    Json::Value card = s_cardMap[terminalId];    
     
    std::string serviceproviderid;
    std::string type             ;
    std::string operatorId       ;
    std::string role             ;
    Json::Value roles            ;
    bool valid = false;
    bool validserviceprovider = false;
    std::string CSN              ;
        
    CsDebug(1, (1, "process_GET_validatepin card %s", fw.write(card).c_str()));        
    if(card.empty()==false && card["data"].empty()==false)
    {
        serviceproviderid    = card["data"]["serviceprovider"].asString();
        type                 = card["data"]["type"].asString();
        operatorId           = card["data"]["staffid"].asString();
        role                 = card["data"]["role"].asString();  
        roles                = card["data"]["roles"];   
        valid                = card["data"]["valid"].asBool();
        validserviceprovider = card["data"]["validserviceprovider"].asBool();
        CSN                  = card["data"]["CSN"].asString();
    }    
    CsDebug(1, (1, "process_GET_validatepin operatorId %s, role %s %s", operatorId.c_str(), role.c_str(), fw.write(request).c_str()));
    if(operatorId.empty()==false && valid && validserviceprovider && 
        ( (request["errorcode"].isInt() && request["errorcode"].asInt()==0)    ))
    {
        CsDebug(1, (1, "process_GET_validatepin isValid %s proceed to login %s", role.c_str(), operatorId.c_str()));
         
        // If this is a tech login and the state is already tech or we can transition to tech and then proceed to the tech screen
        // or... we can login if the state change to operational allows it
        if
        (
            ( role == "technician" && processStateChange( DCSTATE_TECHNICIAN,  true ) == 0 ) ||
            ( role == "supervisor" && processStateChange( DCSTATE_SUPERVISOR,  true ) == 0 ) ||
            (                         processStateChange( DCSTATE_OPERATIONAL, true ) ==0  )
        )
        {
            dcMessage["data"] = login(false, serviceproviderid, operatorId, terminalId, false, role, roles, CSN);
            switch ( s_DCState )
            {
            case    DCSTATE_TECHNICIAN:
            case    DCSTATE_SUPERVISOR:
                /*  Starts headless mode operation */
                setHeadlessMode( );
                break;

            default:
                /*  Others, DONOTHING! */
                break;
            }
        }
        else
        {
            dcMessage["data"]["result"] = false; 
            switch(s_DCState)
            {
                case DCSTATE_OUT_OF_ORDER:
                    dcMessage["data"]["error"]  = "Out of order";    
                    break;     
                case DCSTATE_TECHNICIAN:
                    dcMessage["data"]["error"]  = "Cannot login at this time. Technician is logged in.";    
                    break;         
			    case DCSTATE_SUPERVISOR:
                    dcMessage["data"]["error"]  = "Cannot login at this time. Supervisor is logged in.";    
                    break;      
                default:
                    dcMessage["data"]["error"]  = "Cannot login at this time";
                    break;
            }
        }            
    }
    else
    {
        dcMessage["data"]["result"] = false; 
		switch (request["errorcode"].asInt())
		{
			case VALIDATE_PIN_ERROR:
				dcMessage["data"]["error"]  =  "System Error";
				break;
			case  VALIDATE_PIN_ERROR_INVALID:
				dcMessage["data"]["error"]  =  "Invalid PIN";
				break;
			case  VALIDATE_PIN_ERROR_BLOCKED:
				dcMessage["data"]["error"]  =  "Operator Application Blocked";
				break;
			case  VALIDATE_PIN_ERROR_NOT_OPERATOR_CARD:
				dcMessage["data"]["error"]  =  "Not An Operator Card";
				break;
			case VALIDATE_PIN_ERROR_OPEN_OPERATOR_CARD:
				dcMessage["data"]["error"]  =  "Failed Opening the Operator Card";
				break;
			default:
				dcMessage["data"]["error"]  =  "Unknown Error";
				break;
        }
    }     

    sendDCMessage(fw.write(dcMessage));
    return ""; // Do not respond to this message
}

std::string process_GET_logoff(Json::Value &request)
{
    Json::Reader reader;
	Json::Value response = createBaseResponse(request);
    
    Json::FastWriter fw;
    std::string candidateTerminalId         = request["terminalid"].asString();
    
    if(s_activeTerminalId!="")
    {
        if(candidateTerminalId.compare(s_activeTerminalId)==0)
        {
            CsDebug(1, (1, "logoff resetting vars"));
            Json::Value logoff;
            logoff["terminalid"]    = s_activeTerminalId;
            logoff["newterminalid"] = "";
            logoff["name"]          = "logoff";
            sendDCMessage(fw.write(logoff));

            //  Let READERAPP knows Driver logged off
            sendReaderMessage( fw.write( logoff ) );
            
            
            s_activeTerminalId  = "";
            s_activeStaffId     = "";
            s_activeCSN         = "";
            s_activeType        = "";
            s_activeRoles       = "";
            s_passwordLogin     = false;        
            
            setState(DCSTATE_STANDBY, false, true); 

            txnManager().clear();
        }
    }            
    
    return fw.write(response);        
}

std::string process_GET_changepin(Json::Value &request)
{
	Json::Value response = createBaseResponse(request);
    Json::FastWriter fw;
    
    if(request["data"]["pin"].empty()==false && request["data"]["newpin"].empty()==false)
    {    
        if(request["data"]["pin"].asString().compare(request["data"]["newpin"].asString())==0)
        {
            response["errorcode"] = 0;    
            response["processing"] = true;  
            CsDebug(1, (1, "process_GET_changepin %s", request["terminalid"].asString().c_str()));
 
            Json::Value message;
            
            CsTime_t t;
            CsTime(&t);
            char timeBuf[64];
            CsStrTimeISO(&t,64,timeBuf);
            
            message["terminalid"]  = request["terminalid"];
            message["name"]        = "changepin";
            message["type"]        = "set";
            message["timestamp"]   = timeBuf;
	    message["data"]["oldpin"] = request["data"]["oldpin"];
            message["data"]["newpin"] = request["data"]["newpin"];
	    message["data"]["type"] = request["data"]["type"];

            Json::FastWriter fw;
            sendReaderMessage(fw.write(message));            
        }
        else
        {
            response["errorcode"] = 2;    
        }
    }
    else
    {
        response["errorcode"] = 1;    
    }
    return fw.write(response);        
}


std::string process_SET_changepin(Json::Value &request)
{
    Json::Value dcMessage;
    Json::FastWriter fw;
     
    CsDebug(1, (1, "process_SET_changepin %s", request["terminalid"].asString().c_str()));
     
    std::string terminalId=request["terminalid"].asString();
	CsTime_t t;
    CsTime(&t);
	char timeBuf[64];
	CsStrTimeISO(&t,64,timeBuf);    
    dcMessage["name"]        = "changepin";
    dcMessage["terminalid"]  = terminalId;
    dcMessage["timestamp"]   = timeBuf;
    dcMessage["type"]        = "event";
    dcMessage["data"]        = Json::Value();   

    dcMessage["data"]["errorcode"]  = request["errorcode"];


    sendDCMessage(fw.write(dcMessage));
    return ""; // Do not respond to this message
}

std::string process_GET_driverbreak(Json::Value &request)
{
    Json::FastWriter fw;
    Json::Value response = createBaseResponse(request);
  
    s_driverBreakLockAll = false;
    if(request["data"]["lockall"].isBool())
        s_driverBreakLockAll = request["data"]["lockall"].asBool();
    else if(request["data"]["lockall"].isInt())
        s_driverBreakLockAll = request["data"]["lockall"].asInt()!=0;

    response["errorcode"] = setState(DCSTATE_DRIVER_BREAK, false, true); 
   
    return fw.write(response);             
}

std::string process_GET_changeends(Json::Value &request)
{
    Json::FastWriter fw;
    Json::Value response = createBaseResponse(request);

    response["errorcode"] = setState(DCSTATE_CHANGE_ENDS, false, true); 
   
    return fw.write(response);             
}

    /**
     *  @brief  Processes GET "overridestop" request from browser.
     *  @param  request JSON request object of following format,
     *  @code
     *  {
     *      "name":"overridestop",
     *      "type":"GET",
     *      "userid":"",
     *      "terminalid":"1",
     *      "data":
     *      {
     *          "stop_id":2
     *      }
     *  }
     *  @endcode
     */
std::string
process_GET_overridestop( Json::Value &request )
{
    Json::FastWriter    fw;
    Json::Value         response = createBaseResponse(request);

    int id                      = request[ "data" ].empty() ?
                                  0 :
                                  getJsonInt( request[ "data" ], "stop_id" );
    s_overridePreviousStopId    = s_stopId;
    s_overrideStopId            = id;
    s_overrideEnabled           = true;

    persistRouteData( true );
    
    AVL_ManualEnterStop( s_overrideStopId );

    /*  NOTE:   Will update stop on receiving IPC_AVL_NOTIFY message */
     
    response[ "errorcode" ]     = "0";

    return fw.write( response );
}

std::string  process_SET_ping(Json::Value &request)
{
    Json::FastWriter fw;
    Json::Value response = createBaseResponse(request);
    std::string destination = request["data"]["destination"].asString();
    
    // Need to 
    char pingCommand[256]; 
    std::string result;
    char line[256] = {'\0'}; 
    snprintf(pingCommand, 256, "ping %s -c 4", destination.c_str());
    
    CsDebug(1, (1, "PING: COMMAND: %s", pingCommand));
    
    // TODO: Marshall onto another thread to ensure that this does not a block the websocket
    {
        FILE* fp = popen(pingCommand, "r");
        if(fp!=NULL)
        {
            while(fgets(line, sizeof(line), fp)!= NULL)
            {
                CsDebug(1, (1, "PING: Append: %s", line));
                result += line;
            }
            pclose(fp);
            
            response["errorcode"] = "0";
        }
        else
        {
            CsErrx("PING: could not popen");
            response["errorcode"] = "-1";
            response["error"] = "PING: could not popen";
        }            
    }    
    
    response["result"]    = result;
    
    return fw.write(response);         
}

/*==========================================================================*
**
**  LogicalZoneToPhysicalZone
**
**  Description     :
**      Convert a Logical Zone into a Physical Zone
**
**  Parameters      :
**      value    [I]     U8_t
**
**  Returns         :
**      Physical Zone     U8_t                       
**
**
**==========================================================================*/
U8_t LogicalZoneToPhysicalZone(U8_t value)
{
    if(value>1) value--;
        
    return value;
}

#define ALLSHIFTS_PAGE_SIZE 200
void process_GET_allshifts(int id, Json::Value &request)
{
    Json::FastWriter fw;
    Json::Value response = createBaseResponse(request);
    Json::Value data;
    Json::Value result;
    CsDebug(1, (1, "process_GET_allshifts - start"));
              
    if(request["data"]["serviceprovider"].empty()==false)
    {   
        char sp[33];
        sp[0] = '\0';
        if(request["data"]["serviceprovider"].isString())
        {
            snprintf(sp, 32, "%s", request["data"]["serviceprovider"].asString().c_str());
        }
        else if(request["data"]["serviceprovider"].isInt())
        {
            snprintf(sp, 32, "%d", request["data"]["serviceprovider"].asInt());
        }

        if(sp[0] != '\0')
        {
            CsDebug(1, (1, "process_GET_allshifts %s", sp));
            
            response["errorcode"] = "0";
            
            int frame = 0;

            result = MYKI_Schedule_getAllShifts(g_scheduleDatabase, sp);
            unsigned int resultCount = result["shifts"].size();
            
            CsDebug(3, (3, "process_GET_allshifts got %d entries", resultCount));
            data["total"]  = ( resultCount + (ALLSHIFTS_PAGE_SIZE-(resultCount%ALLSHIFTS_PAGE_SIZE)) ) / ALLSHIFTS_PAGE_SIZE;
            data["shifts"] = Json::Value(Json::arrayValue);
            
            for(Json::Value::ArrayIndex i=0;i<resultCount;i++)
            { 
                data["shifts"].append(result["shifts"][i]);
            
                if((i%ALLSHIFTS_PAGE_SIZE)==(ALLSHIFTS_PAGE_SIZE-1) || i==(resultCount-1))
                {
                    data["page"]   = ++frame;
                    response["errorcode"] = "0";
                    response["data"]      = data;    
                    sendMessage(id, fw.write(response));
                    
                    data["shifts"] = Json::Value(Json::arrayValue);
               }    
            }                
        }        
        else
        {
            CsErrx("process_GET_allshifts missing data");
            response["errorcode"] = "1";    
            sendMessage(id, fw.write(response));            
        }
    }   
    else
    {
        CsErrx("process_GET_allshifts missing data");
        response["errorcode"] = "1";
        sendMessage(id, fw.write(response));
    }
}

std::string process_GET_shifts(Json::Value &request)
{
    Json::FastWriter fw;
    Json::Value response = createBaseResponse(request);
    Json::Value shifts;
              
    if(request["data"]["serviceprovider"].empty()==false && request["data"]["staffid"].isString())
    {   
        char sp[33];
        char staffid[33];
        sp[0] = '\0';
        staffid[0] = '\0';
        if(request["data"]["serviceprovider"].isString())
        {
            snprintf(sp, 32, "%s", request["data"]["serviceprovider"].asString().c_str());
        }
        else if(request["data"]["serviceprovider"].isInt())
        {
            snprintf(sp, 32, "%d", request["data"]["serviceprovider"].asInt());
        }
        
        snprintf(staffid, 32, "%s", request["data"]["staffid"].asString().c_str());
        
        if(sp[0] != '\0' && staffid[0] != '\0')
        {        
            CsErrx("process_GET_shifts missing data");
            response["errorcode"] = "1";      
        }
        else
        {
            CsDebug(1, (1, "process_GET_shifts %s %s", sp, staffid));
            response["errorcode"] = "0";
            response["data"]      = MYKI_Schedule_getShifts(g_scheduleDatabase, sp, staffid);      
        }
    }   
    else
    {
        CsErrx("process_GET_shifts missing data");
        response["errorcode"] = "1";
    }

    return fw.write(response);     
}

std::string process_GET_enquiretablecard(Json::Value &request)
{
    Json::FastWriter fw;
    Json::Value response = createBaseResponse(request);
    Json::Value shifts;
              
    if(request["data"]["serviceprovider"].isString() && request["data"]["shift_number"].isString())
    {
        char sp[33];
        char shift_number[33];
        snprintf(sp,           32, "%s", request["data"]["serviceprovider"].asString().c_str());
        snprintf(shift_number, 32, "%s", request["data"]["shift_number"].asString().c_str());
        CsDebug(1, (1, "MYKI_Schedule_getTablecardsByShiftNumber %s %s", sp, shift_number));
        
        response["errorcode"] = "0";
        response["data"]      = MYKI_Schedule_getTablecardsByShiftNumber(g_scheduleDatabase, sp, shift_number);      
    }   
    else
    {
        CsErrx("MYKI_Schedule_getTablecardsByShiftNumber missing data");
        response["errorcode"] = "1";
    }

    return fw.write(response);     
}

std::string  process_SET_stops(Json::Value &request)
{
    Json::FastWriter fw;
    Json::Value response = createBaseResponse(request);
    Json::Value stops;
    MYKI_CD_Locations_t     Location;
    MYKI_CD_RouteStop_t*    pRouteStops     = NULL;
    int                     routeStopsCount = 0;    
       
    int routeId      = 0;
    int startStopId = request["data"]["startStopId"].asInt();
    int endStopId   = request["data"]["endStopId"].asInt();
    
    if(startStopId<0) startStopId = 0;
    if(endStopId>255) endStopId   = 255;
    
    if(request["data"]["id"].isInt())
        routeId = request["data"]["id"].asInt();
    else
        routeId = atol(request["data"]["id"].asString().c_str());
        
    CsDebug(1, (1, "MYKI_CD_getRouteStopsStructure %d %d %d", routeId, startStopId, endStopId));
    
    routeStopsCount = MYKI_CD_getRouteStopsStructure( routeId, startStopId, endStopId, &pRouteStops );
    
    stops["id"]          = routeId;
    stops["code"]        = request["code"];
    stops["startStopId"] = startStopId;
    stops["endStopId"]   = endStopId;
    stops["stops"]       = Json::Value(Json::arrayValue);
    
    if(routeStopsCount <= 0)
    {
        CsErrx("process_GET_stops - MYKI_CD_getRouteStopsStructure returned %d", routeStopsCount);
        response["errorcode"] = "1";
    }    
    else
    {
        for(int i =0; i<routeStopsCount;i++)
        {
            // TODO: REVIEW: Duplication of code between AVL and DC.
            memset( &Location, 0, sizeof( Location ) );
            if ( MYKI_CD_getLocationsStructure( pRouteStops[i].locationId, &Location ) == FALSE )
            {
                CsErrx( "process_GET_stops(%d) failed", pRouteStops[i].locationId );
               // return;
            }
        
            char zoneBuffer[64];
            
            Json::Value stop;   
            stop["id"]                      = pRouteStops[i].stopId; 
            stop["name"]                    = Location.short_desc; 
            stop["longitude"]               = Location.gps_long;
            stop["latitude"]                = Location.gps_lat;
            stop["longitude_tolerance"]     = Location.gps_long_tolerance;
            stop["latitude_tolerance"]      = Location.gps_lat_tolerance;            

            // Build zone name 
            if(LogicalZoneToPhysicalZone(Location.inner_zone)==LogicalZoneToPhysicalZone(Location.outer_zone))
            {
                snprintf(zoneBuffer, 64, "%d", LogicalZoneToPhysicalZone(Location.zone));
            }
            else
            {
                snprintf(zoneBuffer, 64, "%d/%d", LogicalZoneToPhysicalZone(Location.inner_zone), LogicalZoneToPhysicalZone(Location.outer_zone));
            }
            
            stop["zone"] = zoneBuffer;
            
            stops["stops"][i] = stop;
        }
        response["errorcode"] = "0";
        response["data"]      = stops;      
    }    
	response["name"] = METHOD_STOPS;
	sendDCMessage(fw.write(response));          
	return fw.write(response);
}
std::string  process_GET_stops(Json::Value &request)
{
    Json::FastWriter fw;
    Json::Value response = createBaseResponse(request);
    Json::Value stops;
    MYKI_CD_Locations_t     Location;
    MYKI_CD_RouteStop_t*    pRouteStops     = NULL;
    int                     routeStopsCount = 0;    
       
    int routeId      = 0;
    int startStopId = request["data"]["startStopId"].asInt();
    int endStopId   = request["data"]["endStopId"].asInt();
    
    if(startStopId<0) startStopId = 0;
    if(endStopId>255) endStopId   = 255;
    
    if(request["data"]["id"].isInt())
        routeId = request["data"]["id"].asInt();
    else
        routeId = atol(request["data"]["id"].asString().c_str());
        
    CsDebug(1, (1, "MYKI_CD_getRouteStopsStructure %d %d %d", routeId, startStopId, endStopId));
    
    routeStopsCount = MYKI_CD_getRouteStopsStructure( routeId, startStopId, endStopId, &pRouteStops );
    
    stops["id"]          = routeId;
    stops["code"]        = request["code"];
    stops["startStopId"] = startStopId;
    stops["endStopId"]   = endStopId;
    stops["stops"]       = Json::Value(Json::arrayValue);
    
    if(routeStopsCount <= 0)
    {
        CsErrx("process_GET_stops - MYKI_CD_getRouteStopsStructure returned %d", routeStopsCount);
        response["errorcode"] = "1";
    }    
    else
    {
        for(int i =0; i<routeStopsCount;i++)
        {
            // TODO: REVIEW: Duplication of code between AVL and DC.
            memset( &Location, 0, sizeof( Location ) );
            if ( MYKI_CD_getLocationsStructure( pRouteStops[i].locationId, &Location ) == FALSE )
            {
                CsErrx( "process_GET_stops(%d) failed", pRouteStops[i].locationId );
               // return;
            }
        
            char zoneBuffer[64];
            
            Json::Value stop;   
            stop["id"]                      = pRouteStops[i].stopId; 
            stop["name"]                    = Location.short_desc; 
            stop["longitude"]               = Location.gps_long;
            stop["latitude"]                = Location.gps_lat;
            stop["longitude_tolerance"]     = Location.gps_long_tolerance;
            stop["latitude_tolerance"]      = Location.gps_lat_tolerance;            

            // Build zone name 
            if(LogicalZoneToPhysicalZone(Location.inner_zone)==LogicalZoneToPhysicalZone(Location.outer_zone))
            {
                snprintf(zoneBuffer, 64, "%d", LogicalZoneToPhysicalZone(Location.zone));
            }
            else
            {
                snprintf(zoneBuffer, 64, "%d/%d", LogicalZoneToPhysicalZone(Location.inner_zone), LogicalZoneToPhysicalZone(Location.outer_zone));
            }
            
            stop["zone"] = zoneBuffer;
            
            stops["stops"][i] = stop;
        }
        response["errorcode"] = "0";
        response["data"]      = stops;      
    }    

    return fw.write(response);          
}

#define ROUTE_PAGE_SIZE 200
void process_GET_routes(int id, Json::Value &request)
{
    Json::FastWriter fw;
    Json::Value response = createBaseResponse(request);
    Json::Value data;
    

    int serviceProvider = request["data"]["serviceprovider"].asInt();
    data["serviceprovider"] = serviceProvider;
     
    MYKI_CD_RouteInfo_t routes[MAX_ROUTES];  // TODO FIX Yarra trams has ~1900 routes
    int resultCount = MYKI_CD_getRoutesByServiceProvider(serviceProvider, routes, MAX_ROUTES);
    if(resultCount <= 0)
    {
        CsErrx("process_GET_routes - MYKI_CD_getRoutesByServiceProvider(%d) returned %d", serviceProvider, resultCount);
        response["errorcode"] = "1";
        sendMessage(id, fw.write(response));
    }    
    else    
    {
        int frame      = 0;
        data["routes"] = Json::Value(Json::arrayValue);
        data["total"]  = ( resultCount + (ROUTE_PAGE_SIZE-(resultCount%ROUTE_PAGE_SIZE)) ) / ROUTE_PAGE_SIZE;
        
        CsDebug(1, (1, "MYKI_CD_getRoutesByServiceProvider(%d) returned %d", serviceProvider, resultCount));
        for(int i=0; i<resultCount;i++)
        {
            Json::Value route;   
            route["master_route"]   = routes[i].master_route;
            route["route_id"]       = routes[i].route_id;
            route["code"]           = routes[i].code; 
            route["short_desc"]     = routes[i].short_desc;  
            route["long_desc"]      = routes[i].long_desc;  
            
            data["routes"].append(route);     

            if((i%ROUTE_PAGE_SIZE)==(ROUTE_PAGE_SIZE-1) || i==(resultCount-1))
            {
                
                data["page"]   = ++frame;
                response["errorcode"] = "0";
                response["data"]      = data;    
                sendMessage(id, fw.write(response));
                
                data["routes"] = Json::Value(Json::arrayValue);
                
            }
        }
            
    }

    
}

std::string  process_GET_tablecard(Json::Value &request)
{
    Json::Value response = createBaseResponse(request);
    
    response["errorcode"] = "0";
    response["data"]      = s_tablecard;      
    
    Json::FastWriter fw;
    return fw.write(response);          
}

    /**
     *  @brief  Processes SET "tablecard" request from browser.
     *  @param  request JSON object of the following format,
     *  @code
     *  {
     *      "name":"tablecard",
     *      "type":"SET",
     *      "userid":"",
     *      "terminalid":"1",
     *      "data":
     *      {
     *          "trips":
     *          [
     *              {
     *                  "depot":"A",
     *                  "run_id":"1",
     *                  "route_code":"LR030_1_019",
     *                  "route_id":17873,
     *                  "route":
     *                  {
     *                      "code":"30",
     *                      "long_desc":"St Vincents Plaza - Docklands via La Trobe St",
     *                      "master_route":1880,
     *                      "route_id":17873,
     *                      "short_desc":"LR030_1_019",
     *                      "$$hashKey":"object:105",
     *                      "stops":
     *                      [
     *                          {
     *                              "id":1,
     *                              "latitude":"-37.807840",
     *                              "latitude_tolerance":"0.001",
     *                              "longitude":"144.972837",
     *                              "longitude_tolerance":"0.001",
     *                              "name":"10-Nicholson/Victoria",
     *                              "zone":"1",
     *                              "$$hashKey":"object:108"
     *                          },
     *                          ...
     *                      ]
     *                  },
     *                  "first_stop":
     *                  {
     *                      "id":1,
     *                      "latitude":"-37.807840",
     *                      "latitude_tolerance":"0.001",
     *                      "longitude":"144.972837",
     *                      "longitude_tolerance":"0.001",
     *                      "name":"10-Nicholson/Victoria",
     *                      "zone":"1",
     *                      "$$hashKey":"object:108"
     *                  },
     *                  "first_stop_name":"10-Nicholson/Victoria",
     *                  "last_stop":
     *                  {
     *                      "id":2,
     *                      "latitude":"-37.808230",
     *                      "latitude_tolerance":"0.0006",
     *                      "longitude":"144.976455",
     *                      "longitude_tolerance":"0.001",
     *                      "name":"12-St Vincents Plaza",
     *                      "zone":"1",
     *                      "$$hashKey":"object:109"
     *                  },
     *                  "last_stop_name":"12-St Vincents Plaza",
     *                  "start_time":"07:43:21 PM"
     *              }
     *          ],
     *          "loadedDefer":
     *          {
     *              "promise":
     *              {
     *                  "$$state":
     *                  {
     *                      "status":0
     *                  }
     *              }
     *          },
     *          "driverId":"5057",
     *          "startDateTime":"2015-09-28T19:43:21.572Z",
     *          "tablecardNumber":"1234"
     *      }
     *  }
     *  @endcode
     */
std::string
process_SET_tablecard( Json::Value &request )
{
    Json::Value response = createBaseResponse(request);
    Json::FastWriter fw;
    
    s_tablecard = request["data"]; // TODO
    CsDebug(3, (3, "process_SET_tablecard s_tablecard : %s", fw.write(s_tablecard).c_str()));
    int ret = setState(DCSTATE_OPERATIONAL, true, true);
    response["errorcode"] = ret;

    /*  Forward "tablecard" message to READERAPP */
    request[ "userid" ] = s_activeStaffId;
    sendReaderMessage( fw.write( request ) );

    return fw.write(response);          
}

std::string  process_GET_trip(Json::Value &request)
{
    Json::Value response = createBaseResponse(request);
    
    response["errorcode"] = "0";
    response["data"]      = s_trip;      

    Json::FastWriter fw;
    return fw.write(response);          
}

std::string process_SET_devicestatus(Json::Value &request)
{
    Json::Value response = createBaseResponse(request);
    std::string terminalId = request["data"]["terminalid"].asString();
    std::string online = request["data"]["status"].asString();

    CsDebug(2, (2, "process_SET_devicestatus %s %s", terminalId.c_str(), online.c_str()));
    if(terminalId.compare("*")==0) // TODO REVIEW this approach with the wildcard.  Is it appropriate to use throughout the API?
    {
        // TODO ADD New states into myki-alarm
        (void)MYKI_ALARM_sendSimpleAlarm(MYKI_ALARM_VEHICLE_EQUIPMENT_STATE, 30, MYKI_ALARM_ALERT_WARNING);
        (void)MYKI_ALARM_sendSimpleAlarm(MYKI_ALARM_VEHICLE_EQUIPMENT_STATE, 30, MYKI_ALARM_ALERT_CLEARED);
        sendVaropToAll(VO_READER_ENABLED, online.compare("online")==0?1:0);
        response["errorcode"] = 0;
    }
    else
    {
        (void)MYKI_ALARM_sendSimpleAlarm(MYKI_ALARM_VEHICLE_EQUIPMENT_STATE, 30, MYKI_ALARM_ALERT_WARNING);
        (void)MYKI_ALARM_sendSimpleAlarm(MYKI_ALARM_VEHICLE_EQUIPMENT_STATE, 30, MYKI_ALARM_ALERT_CLEARED);        
        CceRv_t Rv = sendVarop(terminalId.c_str(), VAROP_SET, VO_READER_ENABLED,  online.compare("online")==0?1:0);
        response["errorcode"] = CceMsOk( Rv ) ? 0 : -1;
    }

    Json::FastWriter fw;
    return fw.write(response);          
}



int getModeValue(std::string mode)
{
    if(mode.compare("touchon")==0)    return MODE_TOUCHON;
    if(mode.compare("touchoff")==0)   return MODE_TOUCHOFF;
    if(mode.compare("touchonoff")==0) return MODE_TOUCHONOFF;
    
    return 0; // Default operation
}

std::string getModeName(int mode)
{
    switch (mode)
    {
        case MODE_TOUCHON:
            return "touchon";
            break;
        case MODE_TOUCHOFF:
            return "touchoff";
            break;
        case MODE_TOUCHONOFF:
            return "touchonoff";
            break;
    }
    
    return "default"; // Default operation
}


std::string process_SET_devicemode(Json::Value &request)
{
    Json::Value response = createBaseResponse(request);
    std::string terminalId = request["data"]["terminalid"].asString();
    std::string mode = request["data"]["mode"].asString();
    int modeValue = getModeValue(mode);

    CsDebug(1, (1, "process_SET_devicemode %s %s %d", terminalId.c_str(), mode.c_str(), modeValue));
    if(terminalId.compare("*")==0) // TODO REVIEW this approach with the wildcard.  Is it appropriate to use throughout the API?
    {
        // TODO ADD New states into myki-alarm
        (void)MYKI_ALARM_sendSimpleAlarm(MYKI_ALARM_VEHICLE_EQUIPMENT_STATE, 30, MYKI_ALARM_ALERT_WARNING);
        (void)MYKI_ALARM_sendSimpleAlarm(MYKI_ALARM_VEHICLE_EQUIPMENT_STATE, 30, MYKI_ALARM_ALERT_CLEARED);
        sendVaropToAll(VO_READER_MODE, modeValue);
         
        response["errorcode"] = 0;
    }
    else
    {
        (void)MYKI_ALARM_sendSimpleAlarm(MYKI_ALARM_VEHICLE_EQUIPMENT_STATE, 30, MYKI_ALARM_ALERT_WARNING);
        (void)MYKI_ALARM_sendSimpleAlarm(MYKI_ALARM_VEHICLE_EQUIPMENT_STATE, 30, MYKI_ALARM_ALERT_CLEARED);        
        CceRv_t Rv = sendVarop(terminalId.c_str(), VAROP_SET, VO_READER_MODE,  modeValue);
        response["errorcode"] = CceMsOk( Rv ) ? 0 : -1;
    }

    Json::FastWriter fw;
    return fw.write(response);          
}


std::string  process_SET_reboot(Json::Value &request)
{
    Json::Value response = createBaseResponse(request);
    std::string terminalId = request["data"]["terminalid"].asString();
    
    CsDebug(1, (1, "process_SET_reboot %s", terminalId.c_str()));
    if(terminalId.compare("*")==0) 
    {
        sendVaropToAll(VO_REBOOT, 1);
        response["errorcode"] = 0;
    }
    else
    {
        CceRv_t Rv = sendVarop(terminalId.c_str(), VAROP_SET, VO_REBOOT, 1);
        response["errorcode"] = CceMsOk( Rv ) ? 0 : -1;
    }

    Json::FastWriter fw;
    // Send the reboot to all listening driver consoles (matched on terminalid)
    Json::Value rebootMessage;
    rebootMessage["name"] = "reboot";
    rebootMessage["data"]["terminalid"] = terminalId;
    sendDCMessage(fw.write(rebootMessage));    

    return fw.write(response);          
}

void setCardData(std::string terminalid, Json::Value card)
{
    
}


/*==========================================================================*
**
**  isRole
**
**  Description     :
**      Validates provided role against json roles file
**
**  Parameters      :
**      None
**
**  Returns         :
**      None
**
**==========================================================================*/
bool isRole(std::string rolename, int type, int profile)
{
    if(g_roles.isArray())
    {
        for(Json::Value::ArrayIndex i=0;i<g_roles.size();i++)
        {
            if(g_roles[i]["profile"].isInt() && g_roles[i]["type"].isInt() && g_roles[i][rolename].isInt()) 
            {
                if(g_roles[i]["profile"].asInt()==profile && g_roles[i]["type"]==type && g_roles[i][rolename].asInt()==1) 
                {
                    return true;
                }  
            }             
        }
    }
    return false;
}

std::string process_HB_message(Json::Value &request)
{
	Json::Value::ArrayIndex index=0;
    // Update the map
    if(request["terminalid"].isString())
    {
        std::string terminalId = request["terminalid"].asString();
        if(terminalId.empty()==false)
        {
            CsMutexLock(&g_heartbeatMapLock);
            g_heartbeatMap[terminalId] = time(NULL);
            CsMutexUnlock(&g_heartbeatMapLock);
        }
    }
    
    if(request.isMember("data")==false)
        CsWarnx("process_HB_message 'data' is NULL. Skipping ESN update.");
    else if(request["data"].isObject()==false)
        CsWarnx("process_HB_message 'data' is NOT array. Skipping ESN update.");
    else if(request["data"].isMember("ESN")==false)
        CsWarnx("process_HB_message 'ESN' is not part of 'data'. Skipping ESN update.");
    else
    {
        for(Json::Value::ArrayIndex i=0; i<s_deviceList["devices"].size();i++)
        {
            Json::Value terminalid = s_deviceList["devices"][i]["terminalid"];
            if(terminalid.empty()==false && request["terminalid"]==terminalid)
            {
                s_deviceList["devices"][i]["ESN"] = request["data"]["ESN"];
				index =i;
                break;
            }
        }
    }

	Json::Value terminalid = s_deviceList["devices"][index]["terminalid"];

    if (!terminalid.isNull())
    {
        bool changed = setDeviceStatus(terminalid.asCString(), DEVICE_STATUS_GOOD, "Connected", false, false); 
        setDeviceOpStatus(terminalid.asCString(), DEVICE_OPSTATUS_IN_SERVICE, "Connected", changed, false);    
    }

    Json::Value response = createBaseResponse(request);
    Json::FastWriter fw;
     
    return fw.write(response);      
}

std::string process_SET_cardevent(Json::Value &request)
{
    Json::Value response = createBaseResponse(request);
    Json::Value card =Json::Value();
    Json::FastWriter fw;
    //  {"data":{"cardnumber":"042F1919721D80","expiry":"","pin":"1234","roles":[{"profile":"1","type":"11"}],"serviceprovider":"1001","staffid":"5070","type":"operator","valid":true},"event":"cardpresented","name":"cardevent","terminalid":"2","type":null,"userid":null}
    // {"data":{"description":"success"},"errorcode":0,"name":"cardevent","terminalid":"2","timestamp":"2015-02-19T16:33:23+10:00","type":null,"userid":null}
    std::string event = request["event"].asString();
    
    if(event.empty()) event = request["name"].asString();
   
    CsDebug(1, (1, "process_SET_cardevent %s", event.c_str()));
    if(request["event"]== "cardpresented")
    {
        card = request; 
        card["data"]["initialuse"] = (card["data"]["pin"].asString().empty());

        bool isDriver       = false;
        bool isTechnician   = false;
        bool isSupervisor   = false;
        bool isAO           = false;
        bool validSP        = false;
        card["data"]["role"] = "unidentified";
        
        int serviceProviderId=-1;
        if(card["data"]["serviceprovider"].isInt())
            serviceProviderId = card["data"]["serviceprovider"].asInt();
        else
            serviceProviderId = atoi(card["data"]["serviceprovider"].asString().c_str());

        if(serviceProviderId>0)
        {            
            for(Json::Value::ArrayIndex i=0;validSP==false && i<g_serviceProviders.size();i++)
            {
                if(g_serviceProviders[i].asInt()==serviceProviderId)  
                    validSP = true;
            }
        }
        
        card["data"]["validserviceprovider"] = validSP; 
        
        if(card["data"]["roles"].isArray())
        {
                   CsDebug(1, (1, "process_SET_cardevent role count %d", card["data"]["roles"].size()));
            for(Json::Value::ArrayIndex i=0;i<card["data"]["roles"].size();i++)
            {            
                int type     = 0;
                int profile  = 0;
                if(card["data"]["roles"][i]["type"].isInt())
                    type = card["data"]["roles"][i]["type"].asInt();
                else
                    type = atoi(card["data"]["roles"][i]["type"].asString().c_str());
                
                if(card["data"]["roles"][i]["profile"].isInt())
                    profile = card["data"]["roles"][i]["profile"].asInt();
                else
                    profile = atoi(card["data"]["roles"][i]["profile"].asString().c_str());    
                
                
                CsDebug(2, (2, "process_SET_cardevent validate type %d profile %d", type, profile));
                isDriver     = isDriver     || isRole("driver",     type, profile);
                isTechnician = isTechnician || isRole("technician", type, profile);
                isSupervisor = isSupervisor || isRole("supervisor", type, profile);
                isAO         = isAO         || isRole("ao",         type, profile);
            }
        }
 //       CsDebug(1, (1, "process_SET_cardevent %d", __LINE__));
        
        if     (isSupervisor) card["data"]["role"] = "supervisor";
        else if(isTechnician) card["data"]["role"] = "technician";
        else if(isDriver)     card["data"]["role"] = "driver";
        else if(isAO)         card["data"]["role"] = "ao";
#ifdef REDACT_PIN        
        card["data"]["pin"] = "<REMOVED>";   
#endif
        //_card = fw.write(s_card);
        CsDebug(1, (1, "Terminal:%s Set Card %s", request["terminalid"].asString().c_str(), fw.write(card).c_str()));
        
        s_cardMap[request["terminalid"].asString()] = card;  // Is a mutex required to protect the map?
		g_card = fw.write(card);
        CsDebug(1, (1, "process_SET_cardevent dcmessage %s", g_card.c_str()));
        sendDCMessage(g_card);
    }
    else if(request["event"]=="badcardpresented")
    {
        s_cardMap[request["terminalid"].asString()] = card;
        g_card = "";
        sendDCMessage(fw.write(request));
    }    
    else if(request["event"]=="cardremoved")
    {
        s_cardMap[request["terminalid"].asString()] = card;
        g_card = "";        
        sendDCMessage(fw.write(request)); // TODO set isPrimary            
    }
    else if(request["event"]=="cardinit")
    {
        g_card = "";
        s_cardMap[request["terminalid"].asString()] = card;
    }
    else if(request["event"]=="cardshutdown")        
    {
        s_cardMap.erase(request["terminalid"].asString());
    }

    response["errorcode"] = 0;

    bool isTransitCard = (!request["data"].isNull() 
            && !request["data"]["type"].isNull() 
            && request["data"]["type"].asString() == "transit");

    if (isTransitCard && s_touchOnOffEnabled)
    {
        response["name"] = METHOD_VALIDATETRANSITCARD;
    }

    return fw.write(response);      
}

    /**
     *  @brief  Processes SET "trip" request from browser.
     *  @param  request JSON request object of the following format,
     *  @code
     *  {
     *      "name":"trip",
     *      "type":"SET",
     *      "userid":"",
     *      "terminalid":"1",
     *      "data":
     *      {
     *          "depot":"A",
     *          "first_stop":
     *          {
     *              "$$hashKey":"object:108",
     *              "id":1,
     *              "latitude":"-37.807840",
     *              "latitude_tolerance":"0.001",
     *              "longitude":"144.972837",
     *              "longitude_tolerance":"0.001",
     *              "name":"10-Nicholson/Victoria",
     *              "zone":"1"
     *          },
     *          "first_stop_name":"10-Nicholson/Victoria",
     *          "last_stop":
     *          {
     *              "$$hashKey":"object:109",
     *              "id":2,
     *              "latitude":"-37.808230",
     *              "latitude_tolerance":"0.0006",
     *              "longitude":"144.976455",
     *              "longitude_tolerance":"0.001",
     *              "name":"12-St Vincents Plaza",
     *              "zone":"1"
     *          },
     *          "last_stop_name":"12-St Vincents Plaza",
     *          "route":
     *          {
     *              "$$hashKey":"object:105",
     *              "code":"30",
     *              "long_desc":"St Vincents Plaza - Docklands via La Trobe St",
     *              "master_route":1880,
     *              "route_id":17873,
     *              "short_desc":"LR030_1_019",
     *              "stops":
     *              [
     *                  {
     *                      "$$hashKey":"object:108",
     *                      "id":1,
     *                      "latitude":"-37.807840",
     *                      "latitude_tolerance":"0.001",
     *                      "longitude":"144.972837",
     *                      "longitude_tolerance":"0.001",
     *                      "name":"10-Nicholson/Victoria",
     *                      "zone":"1"
     *                  },
     *                  ...
     *              ]
     *          },
     *          "route_code":"LR030_1_019",
     *          "route_id":17873,
     *          "run_id":"1",
     *          "start_time":"07:43:21 PM"
     *      }
     *  }
     *  @endcode
     *  @return The string object respresenting JSON response message
     *          of the following format,
     *  @code
     *  @endcode
     */
std::string
process_SET_trip( Json::Value &request )
{
    #define MINUTES_PER_HOUR            60

    Json::FastWriter    fw;
    Json::Value         response = createBaseResponse(request);

    s_trip                      = request["data"];

    CsDebug( 3, ( 3, "process_SET_trip s_tablecard : %s", fw.write( s_tablecard ).c_str( ) ) );
    CsDebug( 3, ( 3, "process_SET_trip s_trip :      %s", fw.write( s_trip      ).c_str( ) ) );

    int32_t routeId             = (int32_t)getJsonInt( s_trip,                   "route_id"      );
    int32_t shiftId             = (int32_t)getJsonInt( s_tablecard,              "shift_number"  );
    int32_t depotId             = (int32_t)getJsonInt( s_trip,                   "depot_id"      );
    int32_t runId               = (int32_t)getJsonInt( s_trip,                   "run_id"        );
    //UNUSED int32_t tripId              = (int32_t)getJsonInt( s_trip,                   "trip_id"       );
    int32_t tripNumber          = (int32_t)getJsonInt( s_trip,                   "trip_number"   );
    std::string tripStartTimeStr=       getJsonString( s_trip,                   "start_time"    );
    int32_t     tripStartTime   = -1;
    int32_t firstStopId         = s_trip[ "first_stop" ].empty() ?
                                  0 :
                                  (int32_t)getJsonInt( s_trip[ "first_stop" ],   "id"            );
    int32_t lastStopId          = s_trip[ "last_stop" ].empty() ?
                                  0 :
                                  (int32_t)getJsonInt( s_trip[ "last_stop"  ],   "id"            );
    int32_t staffId             = s_activeStaffId.empty() ?
                                  0 :
                                  (int32_t)atol( s_activeStaffId.c_str( ) );
        
    // Note inbound is not applicable as separate route ids are used into the city and out.
    int direction               = s_trip[ "route_code" ].isString( ) ? AVL_TRIP_DIRECTION_OUTBOUND : AVL_TRIP_DIRECTION_HEADLESS;

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
    s_overrideEnabled           = false;
    s_overridePreviousStopId    = 0;
    s_overrideStopId            = 0;

    int ret                     = setState( DCSTATE_OPERATIONAL, true, true );
    response[ "errorcode" ]     = ret;

    /*  Forward SET "trip" to READERAPP */
    request[ "userid" ]         = s_activeStaffId;
    sendReaderMessage( fw.write( request ) );

    return fw.write( response );
}

std::string  process_SET_headlesstrip(Json::Value &request)
{
    Json::FastWriter    fw;
    Json::Value response = createBaseResponse(request);

    s_trip                      = request["data"];

    CsDebug( 3, ( 3, "process_SET_trip s_trip :      %s", fw.write( s_trip      ).c_str( ) ) );

    setHeadlessMode();

    response["data"] = s_trip;

    return fw.write( response ); 
}

std::string  process_GET_state(Json::Value &request)
{
	Json::Value response = createBaseResponse(request);

    response["state"] = getStateString(s_DCState); 
    response["activeterminalid"]          = s_activeTerminalId;
    response["activeoperatorid"]          = s_activeStaffId;
    response["activeserviceproviderid"]   = s_activeServiceProviderId;
    response["activetype"]                = s_activeType;
    response["activeroles"]               = s_activeRoles;
    response["headlessMode"]              = s_headless;
    response["passwordlogin"]             = s_passwordLogin;
    response["criticalfailure"]           = s_criticalFailure;    
    if (!s_activeCSN.empty())
        response["activeCSN"]             = s_activeCSN;
 
    Json::FastWriter fw;
    return fw.write(response);        
}

std::string  process_GET_network(Json::Value &request)
{
	Json::Value response = createBaseResponse(request);

    getNetworkSettings();

    response["errorcode"] = "0";
    response["data"]      = s_network;      // TODO Deal with error conditions such as empty  
    response["data"]["health"] = "ERROR";
    
    switch(g_3GState)
    {
        case IPC_3G_UNINITIALISED:
            response["data"]["health"] = "ERROR";
            break;
        case IPC_3G_DISCONNECTED:
            response["data"]["health"] = "ERROR";
            break;
        case IPC_3G_ERROR:
            response["data"]["health"] = "ERROR";
            break;            
        case IPC_3G_CONNECTING:
            response["data"]["health"] = "WARNING";
            break;
        case IPC_3G_CONNECTED:
            response["data"]["health"] = "OK";
            break;            
    } 
 
    Json::FastWriter fw;
    return fw.write(response);        
}

std::string  process_GET_devicelist(Json::Value &request)
{
	Json::Value response = createBaseResponse(request);

    if(s_deviceList.size()==0)
    {
        response["errorcode"] = "1";
    }    
    else
    {
        response["errorcode"] = "0";
        response["data"]      = s_deviceList;      // TODO Deal with error conditions such as empty  
    }    
 
    Json::FastWriter fw;
    return fw.write(response);        
}

std::string getKernelVersion()
{
    std::string returnValue = "";
    FILE* fp = popen("uname -r", "r");
    if(fp!=NULL)
    {
        char line[255];
        if(fgets(line, sizeof(line), fp) != NULL)
        {            
            returnValue = line;
            
            CsDebug(2, (2, "getKernelVersion result %s", returnValue.c_str()));
        }
        
        pclose(fp);
    }   
    else
    {
    }
    
    return returnValue;
}

std::string getDeviceType()
{
    std::string returnValue = "";
    FILE* fp = fopen("/etc/devicetype", "r");
    if(fp!=NULL)
    {
        char line[255];
        while(fgets(line, sizeof(line), fp) != NULL)
        {            
            returnValue += line;
        }
        pclose(fp);
        CsDebug(2, (2, "getDeviceType %s", returnValue.c_str()));
    }   
    else
    {
       CsErrx("Error on fopen  /etc/devicetype");
    }
    
    return returnValue;
}

std::string getSAM()
{
    //CsDebug(2, (2, "getSAM"));
    std::string returnValue = "";
    FILE* fp = popen("/afc/bin/cmdutil -m -c 'sam info' /tmp/myki-app.pipe", "r");
    if(fp!=NULL)
    {
        char line[255];
        while(fgets(line, sizeof(line), fp) != NULL)
        {            
            returnValue += line;
        }
        pclose(fp);
        
        CsDebug(2, (2, "getSAM %s", returnValue.c_str()));
    } 
    else
    {
       CsErrx("Error on popen  /afc/bin/cmdutil -m -c 'sam info' /tmp/myki-app.pipe");
    }
    
    return returnValue;
}

Json::Value getPackageList()
{
    Json::Value returnValue = Json::Value();
    
    CsDebug(2, (2, "getPackageList"));
    FILE* fp = popen("dpkg --list | grep  mbu | awk -v OFS='\t' '{print $2,$3}'", "r");
    if(fp!=NULL)
    {
        char line[255];
        std::string dpkgLine;
        while(fgets(line, sizeof(line), fp) != NULL)
        {  
            dpkgLine += line;
        }
        pclose(fp);
        
        if(dpkgLine.empty()==false)
        {
            std::stringstream ss(dpkgLine);
            char package[128];
            char version[128];

            while(ss.getline( package, 127, '\t') && ss.getline(version,127, '\n')) 
            {
                if(package[0]!='\0' && version[0]!='\0')
                    returnValue[package] = version;
            }                
        }
    }   
    else
    {
       CsErrx("getPackageList Error on popen  dpkg --list | grep  mbu | awk -v OFS='\t' '{print $2,$3}'");     
    }
    
    return returnValue;
}

std::string  process_GET_config( Json::Value &request)
{
    Json::Reader reader;
    Json::Value response = createBaseResponse(request);
    Json::Value  roles, serviceproviders;
    Json::Value  sw, hw;
    Json::FastWriter fw;
    
    std::ifstream serviceprovidersData(g_serviceProvidersFile, std::ifstream::binary);
    std::ifstream rolesData(g_rolesFile, std::ifstream::binary);

    std::string terminalId = response["terminalid"].asString();
    
    CsDebug(1, (1, "Report connection from %s",terminalId.c_str()));
    bool changed = setDeviceStatus(terminalId.c_str(), DEVICE_STATUS_GOOD, "Connected", false, false); 
    setDeviceOpStatus(terminalId.c_str(), DEVICE_OPSTATUS_IN_SERVICE, "Connected", changed, true);   

    u_long esnValue = 0;
    if(request.isMember("data")==true && request["data"].isObject()==true && 
       request["data"].isMember("ESN")==true);
    {
        if(request["data"]["ESN"].isUInt())
            esnValue = request["data"]["ESN"].asUInt();
        else if(request["data"]["ESN"].isString())
        {
           esnValue = strtoul(request["data"]["ESN"].asCString(), /*endptr*/NULL, /*base*/0);
        }

        if(esnValue != 0)
        {
            setDeviceReaderESN(terminalId.c_str(), esnValue, "DC ESN:", /*updateLog*/true, /*forceNotify*/true);
        }
    }

    U16_t headlessRouteId = 0;    
    if(MYKI_CD_getHeadlessModeRoute(g_serviceProviderId, &headlessRouteId))
    {
      MYKI_CD_Locations_t Location;
      MYKI_CD_RouteStop_t* pRouteStops = NULL;
      response["data"]["headlessRouteId"] = headlessRouteId; 
      int routeStopsCount = MYKI_CD_getRouteStopsStructure( headlessRouteId, 1, 1, &pRouteStops );
      
      if(routeStopsCount > 0)
      {
        memset( &Location, 0, sizeof( Location ) );
        if ( MYKI_CD_getLocationsStructure( pRouteStops[0].locationId, &Location ) )
        {   
          response["data"]["headlessStopName"] = Location.short_desc;        
        } 
      }
    }       
        
    response["errorcode"] = 0;
    response["data"]["screensavertimeout"]        = g_screensaverTimeout; // screensaver
    response["data"]["autostartoftriptimeout"]    = g_autoStartOfTripTimeout;
    response["data"]["autoLogoffTimeout"]         = g_autoLogoffTimeout;
    response["data"]["operationalModeTimeout"]    = g_operationalModeTimeout;
    response["data"]["LANCommunicationTimeout"]   = g_LANCommunicationTimeout;    
    response["data"]["unrosteredShiftID"]         = g_unrosteredShiftID;
    response["data"]["unrosteredTripID"]          = g_unrosteredTripID;
    response["data"]["autoEndOfTripTimeout"]      = g_autoEndOfTripTimeout;
    response["data"]["serviceproviderid"]         = g_serviceProviderId; 
    response["data"]["manualendshiftwaittime"]    = g_manualEndShiftWaitTime;
    response["data"]["successhmimsgtimeout"]      = g_successHMIMsgTimeout;
    response["data"]["errorhmimsgtimeout"]        = g_errorHMIMsgTimeout;
    response["data"]["salesCompleteTimeout"]      = g_salesCompleteTimeout;
    response["data"]["SOSOWhenDriverNotLoggedIn"] = s_sosoWhenDriverNotLoggedIn;
    response["data"]["currencyRoundingPortion"]   = g_currencyRoundingPortion;
    response["data"]["roundingHalfUp"]            = g_roundingHalfUp;
    //response["data"]["terminalid"]              = g_terminalId; // TODO: REVIEW  - primary has a different terminal id
    
    char tariff[256];
    char scheduleVersion[256] = {'\0'};
    std::string expiryDate = "";
    int major =0;
    int minor =0;
    int version = 0;

    if(MYKI_Schedule_getVersion(g_scheduleDatabase, version, major, minor)!=0)
    {
        CsErrx("MYKI_Schedule_getVersion call failed %s", g_scheduleDatabase);
        response["errorcode"] = 1;
    }
    else
    {
        expiryDate = MYKI_Schedule_getExpiryDate(g_scheduleDatabase, g_serviceProviderId, g_unrosteredShiftID);
        if (expiryDate.empty())
        {
            CsErrx("MYKI_Schedule_getExpiryDate failed %s", g_scheduleDatabase);
            response["errorcode"] = 1;
        }
    }   

    snprintf(scheduleVersion, 255, "%d.%d.%d",  version, major, minor);
    snprintf(tariff, 255, "%d.%d",  MYKI_CD_getMajorVersion(), MYKI_CD_getMinorVersion());

    response["data"]["tariff"]  = tariff;
    response["data"]["schedule"] = scheduleVersion;
    response["data"]["scheduledate"] = expiryDate.c_str();
    response["data"]["version"] = BUILDNAME;


    U32_t maxAddValue = 0;
    U32_t minAddValue = 0;
    S32_t maxTPurseBalance = 0;
    U8_t addValueEnabled = 0;
    U8_t cashEnabled = 0;
    U8_t tpurseEnabled = 0;
    //bool autoPrintReceipt = false;
    int tpurseLoadLabel1 = 0;
    int tpurseLoadLabel2 = 0;
    int tpurseLoadLabel3 = 0;

    if (!MYKI_CD_getMaximumAddValue(&maxAddValue))
    {
        CsErrx("process_GET_config: MYKI_CD_getMaximumAddValue() failed.");
        response["errorcode"] = 1;
    }

    if (!MYKI_CD_getMinimumAddValue(&minAddValue))
    {
        CsErrx("process_GET_config: MYKI_CD_getMinimumAddValue() failed.");
        response["errorcode"] = 1;
    }

    if (!MYKI_CD_getMaximumTPurseBalance(&maxTPurseBalance))
    {
        CsErrx("process_GET_config: MYKI_CD_getMaximumTPurseBalance() failed.");
        response["errorcode"] = 1;
    }

    if (!MYKI_CD_getAddValueEnabled(&addValueEnabled))
    {
        CsErrx("process_GET_config: MYKI_CD_getAddValueEnabled() failed.");
        response["errorcode"] = 1;
    }

    if (!MYKI_CD_getPaymentMethodEnabled(g_terminalType, g_serviceProviderId, "Cash", &cashEnabled))
    {
        CsErrx("process_GET_config: MYKI_CD_getPaymentMethodEnabled() Cash failed.");
        response["errorcode"] = 1;
    }

    if (!MYKI_CD_getPaymentMethodEnabled(g_terminalType, g_serviceProviderId, "TPurse", &tpurseEnabled))
    {
        CsErrx("process_GET_config: MYKI_CD_getPaymentMethodEnabled() TPurse failed.");
        response["errorcode"] = 1;
    }

    if (CsfIniExpandInt("TPE:AddValueToSmartCard1", &tpurseLoadLabel1) != 0)
    {
        CsErrx("process_GET_config: failed to retrieve TPE:AddValueToSmartCard1.");
        response["errorcode"] = 1;
    }

    if (CsfIniExpandInt("TPE:AddValueToSmartCard2", &tpurseLoadLabel2) != 0)
    {
        CsErrx("process_GET_config: failed to retrieve TPE:AddValueToSmartCard2.");
        response["errorcode"] = 1;
    }

    if (CsfIniExpandInt("TPE:AddValueToSmartCard3", &tpurseLoadLabel3) != 0)
    {
        CsErrx("process_GET_config: failed to retrieve TPE:AddValueToSmartCard3.");
        response["errorcode"] = 1;
    }

    response["data"]["maxaddvalue"] = maxAddValue;
    response["data"]["minaddvalue"] = minAddValue;
    response["data"]["maxtpursebalance"] = maxTPurseBalance;
    response["data"]["addvalueenabled"] = (addValueEnabled == 1);
    response["data"]["cashenabled"] = (cashEnabled == 1);
    response["data"]["tpurseenabled"] = (tpurseEnabled ==1);
    response["data"]["autoprintreceipt"] = g_autoPrintReceipt;
    response["data"]["tpurseloadlabel1"] = tpurseLoadLabel1;
    response["data"]["tpurseloadlabel2"] = tpurseLoadLabel2;
    response["data"]["tpurseloadlabel3"] = tpurseLoadLabel3;
    response["data"]["shiftTotalsMaskLevel"] = g_shiftTotalsMaskLevel;
    response["data"]["unrosteredShiftID"] = g_unrosteredShiftID;

    if(reader.parse( rolesData, roles) && roles.size()>0)
    {
        response["data"]["roles"] = roles;
    }
    if(reader.parse( serviceprovidersData, serviceproviders) && serviceproviders.size()>0)
    {
        response["data"]["serviceproviders"] = serviceproviders;
    }
    
    hw["linux"] = getKernelVersion();
    hw["devicetype"] = getDeviceType();
    hw["Primary ESN"] = getESN();  
    hw["Primary SAM"] = getSAM(); 
    
    sw = getPackageList();
    response["data"]["manifest"]["sw"] = sw;
    response["data"]["manifest"]["hw"] = hw;
    
    // Send the current card, if present, to a new connection
    if(s_cardMap.find(terminalId)!=s_cardMap.end())
    {
        Json::Value card = s_cardMap[terminalId];
        sendDCMessage(fw.write(card));
    }        
    
    std::string r = fw.write(response);
    return fw.write(response);        
}

std::string process_GET_tpurseload(Json::Value &request)
{
    Json::Value response = createBaseResponse(request);

    if(request["data"]["amount"].empty()==false)
    {

        response["errorcode"] = 0;
        response["processing"] = true;
        CsDebug(1, (1, "process_GET_tpurseload %s", request["terminalid"].asString().c_str()));

        Json::Value message = createBaseResponse(request);
        message["name"] = METHOD_TPURSELOAD;
        message["type"] = "set";
        message["data"] = request["data"];

        Json::FastWriter readerFw;
        sendReaderMessage(readerFw.write(message));
    }
    else
    {
        response["errorcode"] = 1;
    }

    Json::FastWriter fw;
    return fw.write(response);
}

std::string process_SET_tpurseload(Json::Value &request)
{
    Json::Value dcMessage = createBaseResponse(request);
    Json::FastWriter fw;

    CsDebug(1, (1, "process_SET_tpurseload %s", request["terminalid"].asCString()));

    dcMessage["name"]        = METHOD_TPURSELOAD;
    dcMessage["type"]        = "event";
    dcMessage["data"]        = request["data"];
    dcMessage["errorcode"]  = request["errorcode"];
    dcMessage["string_error"] = request["string_error"];

    sendDCMessage(fw.write(dcMessage));

    if ( request["errorcode"] != 0)
    {
        return "";
    }

    // Create printer msg

	IPC_Printer_TpurseLoad_t printerMsg = {};
    printerMsg.hdr.type = IPC_PRINTER_TPURSELOAD;
    printerMsg.hdr.source = DC_PRINTER_TASK;

	printerMsg.receiptInvoiceThreshold = g_receiptInvoiceThreshold;

	printerMsg.amountTopUp = request["data"]["addedvalue"].asUInt();
	printerMsg.amountNewBalance = request["data"]["remvalue"].asInt();
	printerMsg.amountTotal = request["data"]["txnamount"].asInt();

    if (populatePrinterCompanyDetails(request, printerMsg.companyData) != 0)
    {
        CsErrx("process_SET_tpurseload: populatePrinterCompanyDetails() failed.");
        return "";
    }

    if (populatePrinterCommonDetails(request, printerMsg.commonData, g_salesMessage) != 0)
    {
        CsErrx("process_SET_tpurseload: populatePrinterCommonDetails() failed.");
        return "";
    }

    CsDebug(4, (4, "process_SET_tpurseload: print receipt:\n"
                "transactionNumber: %d\n"
                "topup amount: %u\n"
                "new balance: %u\n"
                "amount total: %u",
                printerMsg.commonData.transactionNumber,
                printerMsg.amountTopUp,
                printerMsg.amountNewBalance,
                printerMsg.amountTotal));

	IPCSend(printerTaskId, &printerMsg, sizeof(printerMsg));
    printerBackupManager().save(printerMsg);

    // Add the transaction to our persistent transaction list

    time_t timestamp = isoStrToTime(request["timestamp"].asString());
    
    if (timestamp == -1)
    {
        CsErrx("jsonRequestToTxn: failed to parse timestamp.");
        return "";
    }

    try {
        // tpurseload can only paid with cash
        TxnList::TransactionPtr txnPtr(new TxnList::CashTransaction(txnManager(), 
                    timestamp)); 

        txnManager().addTransaction(request["data"]["saleseqno"].asInt(), txnPtr);

        txnPtr->addCartItem(1, TxnList::CartItemPtr(new TxnList::TopupCartItem(
                        "LLSC",
                        request["data"]["txnamount"].asInt(),
                        300,
                        "TPurse Load",
                        false,
                        request["data"]["txnseqno"].asInt(),
                        request["data"]["cardnumber"].asString())));

        txnManager().saveToFile();
    }
    catch (exception &e)
    {
        CsErrx("process_SET_tpurseload: %s", e.what());
    }

    return ""; // Do not respond to this message
}

std::string process_GET_tpursereversal(Json::Value &request)
{
    Json::FastWriter fw;
    Json::Value response = createBaseResponse(request);

    CsDebug(1, (1, "process_GET_tpursereversal %s", request["terminalid"].asCString()));

    if (request["data"]["saleseqno"].empty())
    {
        return fw.write(response);
    }

    response["errorcode"] = 0;
    response["processing"] = true;

    Json::Value message;

    message["terminalid"]  = request["terminalid"];
    message["type"] = "set";
    message["name"] = METHOD_TPURSEREVERSAL;

    int saleSeqNo = request["data"]["saleseqno"].asInt();

    TxnList::TransactionPtr txnPtr = txnManager().getTransaction(saleSeqNo);

    if (txnPtr == NULL && txnPtr->getCartItem(1) != NULL)
    {
        CsErrx("process_GET_tpursereversal: Txn %d not found.", saleSeqNo);
        response["errorcode"] = 1;
        return fw.write(response);
    }

    message["data"] = txnPtr->toJson();
    Json::FastWriter readerFw;
    sendReaderMessage(readerFw.write(message));

    return fw.write(response);
}

std::string process_SET_tpursereversal(Json::Value &request)
{
    Json::Value dcMessage = createBaseResponse(request);
    Json::FastWriter fw;

    CsDebug(1, (1, "process_SET_tpursereversal %s", request["terminalid"].asCString()));

    dcMessage["name"] = METHOD_TPURSEREVERSAL;
    dcMessage["type"] = "event";
    dcMessage["data"] = request["data"];
    dcMessage["errorcode"]  = request["errorcode"];
    dcMessage["string_error"] = request["string_error"];

    sendDCMessage(fw.write(dcMessage));

    if (request["errorcode"] != 0)
    {
        return "";
    }

    int saleSeqNo = request["data"]["saleseqno"].asInt();

    TxnList::TransactionPtr txnPtr = txnManager().getTransaction(saleSeqNo);

    if (txnPtr == NULL)
    {
        CsErrx("process_SET_tpursereversal: transaction not found.");
        return "";
    }

    try {
        txnPtr->reverseCartItem(1);
        txnManager().saveToFile();
    }
    catch (runtime_error &e)
    {
        CsErrx("process_SET_tpursereversal: %s", e.what());
        return "";
    }


    // Create printer msg
    IPC_Reversal_TpurseLoad_t printerMsg = {};
    printerMsg.hdr.type = IPC_PRINTER_TPURSELOAD_REV;
    printerMsg.hdr.source = DC_PRINTER_TASK;

    printerMsg.amountTopUp = request["data"]["deductedvalue"].asUInt();
    printerMsg.amountNewBalance = request["data"]["remvalue"].asInt();
    printerMsg.amountTotal = request["data"]["deductedvalue"].asInt();

    printerMsg.origTransactionNumber = saleSeqNo; 
    printerMsg.origDate = txnPtr->getTimestamp();

    if (populatePrinterCompanyDetails(request, printerMsg.companyData) != 0)
    {
        CsErrx("process_SET_tpursereversal: populatePrinterCompanyDetails() failed.");
        return "";
    }

    if (populatePrinterCommonDetails(request, printerMsg.commonData, g_reversalMessage) != 0)
    {
        CsErrx("process_SET_tpursereversal: populatePrinterCommonDetails() failed.");
        return "";
    }

    CsDebug(4, (4, "process_SET_tpursereversal: print receipt:\n"
                "transaction number: %d\n"
                "amount topup: %u\n"
                "new balance: %d\n"
                "amount total: %d\n"
                "original txn number: %d\n"
                "original date: %u",
                printerMsg.commonData.transactionNumber,
                printerMsg.amountTopUp,
                printerMsg.amountNewBalance,
                printerMsg.amountTotal,
                printerMsg.origTransactionNumber,
                printerMsg.origDate));

    IPCSend(printerTaskId, &printerMsg, sizeof printerMsg);
    printerBackupManager().save(printerMsg);

    return ""; // Do not respond to this message
}

std::string process_GET_touchonoffmode(Json::Value &request)
{
    CsDebug(1, (1, "process_GET_touchonoffmode", request["terminalid"].asCString()));

    Json::Value response = createBaseResponse(request);

    s_touchOnOffEnabled = request["data"]["enabled"].asBool();

    response["errorcode"] = 0;
    response["name"] = METHOD_TOUCHONOFFMODE;

    Json::FastWriter fw;
    return fw.write(response);
}

std::string process_GET_transactionlist(Json::Value &request)
{
    CsDebug(1, (1, "process_GET_transactionlist %s", request["terminalid"].asCString()));

    Json::Value response = createBaseResponse(request);

    response["errorcode"] = 0;
    response["name"] = METHOD_TRANSACTIONLIST;
    response["data"]["transactions"] = txnManager().toJson();
    string jsonStr = response["data"].toStyledString();

    CsDbg(4, "process_GET_transactionlist: sending JSON: %s", jsonStr.c_str());

    Json::FastWriter fw;
    return fw.write(response);
}

std::string process_SET_validatetransitcard(Json::Value &request)
{
    CsDebug(1, (1, "process_SET_validatetransitcard %s", 
                request["terminalid"].asCString()));

    Json::Value dcMessage = createBaseResponse(request);
    Json::FastWriter fw;

    dcMessage["name"] = METHOD_VALIDATETRANSITCARD;
    dcMessage["data"] = request["data"];
    dcMessage["errorcode"] = request["errorcode"];
    dcMessage["string_error"] = request["string_error"];

    sendDCMessage(fw.write(dcMessage));

    return "";
}

std::string process_GET_countpassenger(Json::Value &request)
{
    CsDebug(1, (1, "process_GET_countpassenger: %s", request["terminalid"].asCString()));

    Json::FastWriter fw;
    Json::Value response = createBaseResponse(request);

    response["errorcode"] = 0;
    response["processing"] = true;

    Json::Value message = request;
    message["type"] = "set";

    sendReaderMessage(fw.write(message));
    return fw.write(response);
}

std::string process_SET_countpassenger(Json::Value &request)
{
    CsDebug(1, (1, "process_SET_countpassenger: %s", request["terminalid"].asCString()));

    Json::Value message = createBaseResponse(request);
    Json::FastWriter fw;

    message["data"] = request["data"];
    message["string_error"] = request["string_error"];

    sendDCMessage(fw.write(message));

    return "";
}

std::string process_GET_drivertotals(Json::Value &request)
{
    CsDebug(1, (1, "process_GET_drivertotals %s", request["terminalid"].asCString()));

    Json::FastWriter fw;
    Json::Value response = createBaseResponse(request);

    response["processing"] = true;

    sendReaderMessage(fw.write(request));

    return fw.write(response);
}

std::string process_SET_drivertotals(Json::Value &request)
{
    CsDebug(1, (1, "process_SET_drivertotals %s", request["terminalid"].asCString()));

    Json::FastWriter fw;
    Json::Value browserMsg = createBaseResponse(request);
    browserMsg["data"] = request["data"];

    sendDCMessage(fw.write(browserMsg));

    return "";
}

std::string process_GET_shiftend(Json::Value &request)
{
    CsDebug(1, (1, "process_GET_shiftend: %s", request["terminalid"].asCString()));

    Json::FastWriter fw;
    Json::Value response = createBaseResponse(request);

    response["processing"] = true;
    
    // except for 'type' field, pass message from browser to readerapp as-is
    Json::Value message = request;
    message["type"] = "set";

    sendReaderMessage(fw.write(message));

    return fw.write(response);
}

    /**
     *  @brief  Processes SET "shiftend" (response) message sent from READERAPP.
     *  Forwards "shiftend" resonse message from READERAPP to BROWSER and
     *  send IPC_PRINTER_DRIVER_SHIFT_REP message to DCPRINTER to print shift end
     *  report.
     *  @param  request JSON message from READERAPP.
     *  @return Empty string, ie. don't send any message back to READERAPP.
     */
std::string process_SET_shiftend(Json::Value &request)
{
    CsDebug(1, (1, "process_SET_shiftend: %s", request["terminalid"].asCString()));

    bool        isPortion                   = getJsonBool(   request[ "data" ], "portion"    );
    bool        isCardUpdate                = getJsonInt(    request[ "data" ], "cardupdate" ) != 0 ? true : false;
    std::string driverId                    = getJsonString( request[ "data" ], "driver_id"  );
    std::string shiftPortionStatus          = getJsonString( request[ "data" ], "shift_portion_status" );

    time_t shiftStartTime                   = (time_t)getJsonInt( request[ "data" ], "shift_start_datetime" );
    time_t lastPortionStartTime             = (time_t)getJsonInt( request[ "data" ], "portion_start_datetime" );
    char footer[IPC_STRING_MAX_LENGTH]      = "";

    Json::FastWriter fw;
    // pass message as-is from readerapp to the browser
    sendDCMessage(fw.write(request));

    if (request["string_error"] != "WS_SUCCESS")
    {
        CsErrx("process_SET_shiftend: string_error = %s", request["string_error"].asCString());
        return "";
    }

    string footerIniKey = isCardUpdate ? "Printer:EndShiftReportFooter" : "Printer:ManualEndShiftReportFooter";

    if (CsfIniExpand(footerIniKey.c_str(), footer, IPC_STRING_MAX_LENGTH) != 0)
    {
        CsErrx("process_SET_shiftend: failed to retrieve ini setting: %s", footerIniKey.c_str());
    }

    CsDebug(5, (5, "process_SET_shiftend: construct shift report"));

    IPC_DriverShiftReport_t printerMsg = {};

    printerMsg.hdr.type                         = IPC_PRINTER_DRIVER_SHIFT_REP;
    printerMsg.hdr.source                       = DC_PRINTER_TASK;
    printerMsg.reportType                       = ( isCardUpdate == false ? REPORT_MANUAL       :
                                                    isPortion    == false ? REPORT_END_OF_SHIFT : REPORT_SHIFT_PORTION );
    printerMsg.totalsMaskLevel                  = (IPC_ShiftTotalsMaskLevel_e)g_shiftTotalsMaskLevel; 

    strncpy( printerMsg.shiftDetails.operatorID,  driverId.c_str(), IPC_STRING_MAX_LENGTH );
    printerMsg.shiftDetails.shiftNumber         =         getJsonInt( request[ "data" ], "shift_number"         );
    strncpy( printerMsg.shiftDetails.busID,       g_vehicleId, IPC_STRING_MAX_LENGTH );
    printerMsg.shiftDetails.shiftStartDate      = shiftStartTime;
    printerMsg.shiftDetails.lastPortionStartDate= lastPortionStartTime;
    printerMsg.shiftDetails.shiftEndDate        = (time_t)getJsonInt( request[ "data" ], "shift_end_datetime"   );

    IPC_ShiftCashStatus_t &shiftCash = printerMsg.shiftDetails.shiftCash;

    shiftCash.cashAmount                        = getJsonInt( request[ "data" ], "shift_start_cash"             );
    shiftCash.cashStatus                        = shiftPortionStatus == "RESET"           ? CASH_NONE            :
                                                  shiftPortionStatus == "BROUGHT_FORWARD" ? CASH_BROUGHT_FORWARD :
                                                                                            CASH_OUT_OF_BALANCE;

    printerMsg.cashTotals.grossAmount           = getJsonInt( request[ "data" ], "shiftGrossCashAmount"         );
    printerMsg.cashTotals.annulledAmount        = getJsonInt( request[ "data" ], "shiftAnnulledCashAmount"      );
    printerMsg.cashTotals.netAmount             = getJsonInt( request[ "data" ], "shiftNetCashAmount"           );
    printerMsg.otherTotals.grossAmount          = getJsonInt( request[ "data" ], "shiftGrossOtherAmount"        );
    printerMsg.otherTotals.annulledAmount       = getJsonInt( request[ "data" ], "shiftAnnulledOtherAmount"     );
    printerMsg.otherTotals.netAmount            = getJsonInt( request[ "data" ], "shiftNetOtherAmount"          );

    printerMsg.paperTickets.grossTickets        = getJsonInt( request[ "data" ], "shiftGrossPaperTickets"       );
    printerMsg.paperTickets.annulledTickets     = getJsonInt( request[ "data" ], "shiftAnnulledPaperTickets"    );
    printerMsg.paperTickets.netTickets          = getJsonInt( request[ "data" ], "shiftNetPaperTickets"         );

    printerMsg.sundryTickets.grossTickets       = getJsonInt( request[ "data" ], "shiftGrossSundryTickets"      );
    printerMsg.sundryTickets.annulledTickets    = getJsonInt( request[ "data" ], "shiftAnnulledSundryTickets"   );
    printerMsg.sundryTickets.netTickets         = getJsonInt( request[ "data" ], "shiftNetSundryTickets"        );

    printerMsg.fullPassCount                    = getJsonInt( request[ "data" ], "shiftFullPassCount"           );
    printerMsg.concessionPassCount              = getJsonInt( request[ "data" ], "shiftConcessionPassCount"     );

    printerMsg.annulledTickets                  = getJsonInt( request[ "data" ], "shiftAnnulledCashCount"       );
    printerMsg.netCash                          = getJsonInt( request[ "data" ], "shiftNetCashAmount"           );
    strncpy(printerMsg.footer, footer, IPC_STRING_MAX_LENGTH);

    CsDebug(4, (4, "process_SET_shiftend: print shift report:\n"
                "operator id : %s\n" 
                "shift number: %d\n"
                "bus id: %s\n"
                "shift start date: %d\n"
                "shift end date: %d"
                "footer: %s",
                printerMsg.shiftDetails.operatorID,
                printerMsg.shiftDetails.shiftNumber,
                printerMsg.shiftDetails.busID,
                printerMsg.shiftDetails.shiftStartDate,
                printerMsg.shiftDetails.shiftEndDate,
                footer));

    IPCSend(printerTaskId, &printerMsg, sizeof(printerMsg));
    printerBackupManager().save(printerMsg);

    return "";
}

std::string process_GET_printlastreceipt(Json::Value &request)
{
    CsDebug(1, (1, "process_GET_printlastreceipt: %s", 
                request["terminalid"].asCString()));

    Json::Value response = createBaseResponse(request);

    try {
        printerBackupManager().reprint();
    }
    catch (exception &e)
    {
        CsErrx("process_GET_printlastreceipt: %s", e.what());
        response["errorcode"] = 1;
    }

    return Json::FastWriter().write(response);
}

std::string process_GET_listnontransit(const Json::Value &request)
{
    CsDbg(1, "process_GET_listnontransit: %s", request["terminalid"].asCString());

    Json::Value response = createBaseResponse(request);

    response["errorcode"] = 0;
    response["name"] = METHOD_listnontransit;

    // ThirdParty specifies non-transit products
    if (!MYKI_CD_getProducts(response["data"], s_stopZone, "ThirdParty", TIME_NOT_SET, 
                lexical_cast<int>(s_activeServiceProviderId), "BDC"))
    {
        CsErrx("process_GET_listnontransit: MYKI_CD_getproducts failed");
        return "";
    }

    Json::FastWriter fw;
    return fw.write(response);
}

std::string process_GET_listsurcharge(const Json::Value &request)
{
    CsDbg(1, "process_GET_listsurcharge: %s", request["terminalid"].asCString());

    Json::Value response = createBaseResponse(request);

    response["errorcode"] = 0;
    response["name"] = METHOD_listsurcharge;

    // PremiumSurcharge specifies surcharge products
    if (!MYKI_CD_getProducts(response["data"], s_stopZone, "PremiumSurcharge", 
                TIME_NOT_SET, 
                lexical_cast<int>(s_activeServiceProviderId), "BDC"))
    {
        CsErrx("process_GET_listnontransit: MYKI_CD_getproducts failed");
        return "";
    }

    CsDbg(4, "process_GET_listsurcharge: response: %s", 
            response.toStyledString().c_str());

    Json::FastWriter fw;
    return fw.write(response);
}

std::string process_SET_readerstate(Json::Value &request)
{
    CsDebug(1, (1, "process_SET_readerstate: %s", request["terminalid"].asCString()));

    if(request["event"]=="changeOutOfOrderState")
    {
        //We only care about messages from another terminal.
        //Messages from this terminal will be taken into account with hasCriticalFailure
        if(request["terminalid"].asString().compare(g_terminalId))
        {
            s_secondaryCriticalFailure = (request["outOfOrder"].asInt() != 0);
            CsDebug(4, (4, "Secondary DC state change critical: %s(%d)",
                    (s_secondaryCriticalFailure ? "TRUE" : "false"),
                     request["outOfOrder"].asInt()));
            statusCheck(); //Call status check to immediately update our state.
        }
        
        CsDebug(1, (1, "process_SET_readerstate g_sendAutoLogOffAtPowerStart: %s", g_sendAutoLogOffAtPowerStart ? "true" : "false"));
        if(g_sendAutoLogOffAtPowerStart)
        {
            s_activeTerminalId          = s_logOffTerminalId;
            s_activeType                = s_logOffType; 
            s_activeStaffId             = s_logOffStaffId; 
            s_activeServiceProviderId   = s_logOffServiceProviderId;            
          
            processAutoLogOff();
            
            s_logOffTerminalId        = "";
            s_logOffType              = "";
            s_logOffStaffId           = "";
            s_logOffServiceProviderId = "";
            
            g_sendAutoLogOffAtPowerStart = false;
        }

    }

    return "";
}

std::string process_POST_CCEEvent(Json::Value &request)
{
    Json::FastWriter fw; 
    Json::Value response;
    int retval = 0;
     
    IPC_CCEEvent_t m = { {IPC_CCE_EVENT, (TaskID_t)DC_CMD_TASK }};
    strncpy(m.jsonstring, 	fw.write(request).c_str(), 	MAX_JSONSTRING_SIZE);
	strncpy(m.source, 		g_terminalId, 				MAX_HOSTNAME_SIZE);
	
    IPCSend(appQueueId, &m, sizeof m);  
    
    if(retval==0)
    {
        response["errorcode"]           = retval;
        response["data"]["description"] = "success";
    }    
    else
    {
        response["errorcode"]           = retval;
        response["data"]["description"] = "failure";
    }

    
    return fw.write(response);      
}

std::string process_POST_Reader(Json::Value &request)
{
    return process_SET_cardevent(request);
}


std::string  process_unkown_message(Json::Value &request)
{
    Json::Value response = createBaseResponse(request);
    response["data"]["description"] = std::string("Method not valid");
   
    Json::FastWriter fw; 
    return fw.write(response);    
}

void parseMessage(int id, std::string &message)
{
    Json::Value request;   // Will contain the root value after parsing.
	Json::Reader reader;
    
    CsDebug(4, (4, "parseMessage %s", message.c_str()));
    try
    {
        if (reader.parse(message.c_str(),request) && request.size() > 0) // Must check request.size otherwise library can segfault
        {
            string type       =  request.get("type", "" ).asString();
            string name       =  request.get("name", "" ).asString();
           
            std::transform(type.begin(), type.end(), type.begin(), ::tolower);
            std::transform(name.begin(), name.end(), name.begin(), ::tolower);

            if (type == TYPE_GET)
            {
                if (name == METHOD_CONFIG)
                    sendMessage(id, process_GET_config(request)); 
                else if (name == METHOD_STATE)
                    sendMessage(id, process_GET_state(request));                     
                else if (name == METHOD_DEVICELIST)
                    sendMessage(id, process_GET_devicelist(request));  
                else if (name == METHOD_NETWORK)
                    sendMessage(id, process_GET_network(request));  
                else if (name == METHOD_LOGIN)
				{
                    sendMessage(id, process_GET_login(request));  
					if ( s_passwordForceLogOff) 
					{
						CsDebug(2, (2, "password disabled calling  processAutoLogOff"));
						processAutoLogOff();
					}
                    
				} 
                else if (name == METHOD_VALIDATEPIN)
                    sendMessage(id, process_GET_validatepin(request));
                else if (name == METHOD_LOGOFF)
                    sendMessage(id, process_GET_logoff(request));                  
                else if (name == METHOD_CHANGEPIN)
                    sendMessage(id, process_GET_changepin(request));  
                else if (name == METHOD_STOPS)
                    sendMessage(id, process_GET_stops(request));
                else if (name == METHOD_SHIFTS)
                    sendMessage(id, process_GET_shifts(request));                
                else if (name == METHOD_ALLSHIFTS)
                    process_GET_allshifts(id,request);  // special case, send shifts in batches.
                else if (name == METHOD_ENQUIRESTOPS)
                    sendMessage(id, process_GET_stops(request));                
                else if (name == METHOD_ENQUIRETABLECARD)
                    sendMessage(id, process_GET_enquiretablecard(request));                                
                else if (name == METHOD_ROUTES)
                    process_GET_routes(id, request); // special case, send routes in batches.
                else if (name == METHOD_OVERRIDESTOP)
                    sendMessage(id, process_GET_overridestop(request));                
                else if (name == METHOD_DRIVERBREAK)
                    sendMessage(id, process_GET_driverbreak(request));  
                else if (name == METHOD_CHANGEENDS)
                    sendMessage(id, process_GET_changeends(request));  
                else if (name == METHOD_TABLECARD)
                    sendMessage(id, process_GET_tablecard(request));
                else if (name == METHOD_TRIP)
                    sendMessage(id, process_GET_trip(request));
                else if (name == METHOD_TPURSELOAD)
                    sendMessage(id, process_GET_tpurseload(request));
                else if (name == METHOD_TPURSEREVERSAL)
                    sendMessage(id, process_GET_tpursereversal(request));
                else if (name == METHOD_TRANSACTIONLIST)
                    sendMessage(id, process_GET_transactionlist(request));
                else if (name == METHOD_TOUCHONOFFMODE)
                    sendMessage(id, process_GET_touchonoffmode(request));
                else if (name == METHOD_COUNTPASSENGER)
                    sendMessage(id, process_GET_countpassenger(request));
                else if (name == METHOD_SHIFTEND)
                    sendMessage(id, process_GET_shiftend(request));
                else if (name == METHOD_GETDRIVERTOTALS)
                    sendMessage(id, process_GET_drivertotals(request));
                else if (name == METHOD_PRINTLASTRECEIPT)
                    sendMessage(id, process_GET_printlastreceipt(request));
                else if (name == METHOD_listnontransit)
                    sendMessage(id, process_GET_listnontransit(request));
                else if (name == METHOD_listsurcharge)
                    sendMessage(id, process_GET_listsurcharge(request));
                else if (name == METHOD_productsales)
                    sendMessage(id, process_GET_productsales(request));
                else if (name == METHOD_productsalesreversal)
                    sendMessage(id, process_GET_productsalesreversal(request));
// Remove autoreply to prevent feedback loops
//                else
//                    sendMessage(id, process_unkown_message(request));  
            }
            else if (type == TYPE_PUT || type == TYPE_SET)
            {
                if (name == METHOD_TABLECARD)
                    sendMessage(id, process_SET_tablecard(request));
                else if (name == METHOD_TRIP)         
                    sendMessage(id, process_SET_trip(request));                                                                                      
                else if (name == METHOD_HEADLESSTRIP)
                    sendMessage(id, process_SET_headlesstrip(request));
                else if (name == METHOD_DEVICESTATUS)
                    sendMessage(id, process_SET_devicestatus(request));   
                else if (name == METHOD_DEVICEMODE)
                    sendMessage(id, process_SET_devicemode(request));                     
                else if (name == METHOD_PING)
                    sendMessage(id, process_SET_ping(request));                    
                else if (name == METHOD_REBOOT)
                    sendMessage(id, process_SET_reboot(request));   
                else if (name == METHOD_CARDEVENT)
                    sendMessage(id, process_SET_cardevent(request));      
                else if (name == METHOD_CHANGEPIN)
                    sendMessage(id, process_SET_changepin(request));
                else if (name == METHOD_TPURSELOAD)
                    sendMessage(id, process_SET_tpurseload(request));
                else if (name == METHOD_TPURSEREVERSAL)
                    sendMessage(id, process_SET_tpursereversal(request));
                else if (name == METHOD_VALIDATETRANSITCARD)
                    sendMessage(id, process_SET_validatetransitcard(request));
                else if (name == METHOD_COUNTPASSENGER)
                    sendMessage(id, process_SET_countpassenger(request));
                else if (name == METHOD_READERSTATE)
                    sendMessage(id, process_SET_readerstate(request));
				else if (name == METHOD_STOPS)
                    sendMessage(id, process_SET_stops(request));
                else if (name == METHOD_SHIFTEND)
                    sendMessage(id, process_SET_shiftend(request));
                else if (name == METHOD_GETDRIVERTOTALS)
                    sendMessage(id, process_SET_drivertotals(request));
                else if (name == METHOD_productsales)
                    sendMessage(id, process_SET_productsales(request));
                else if (name == METHOD_productsalesreversal)
                    sendMessage(id, process_SET_productsalesreversal(request));
// Remove autoreply to prevent feedback loops
//                else
//                    sendMessage(id, process_unkown_message(request));
            }
            else if (type== TYPE_HB)
            {
                sendMessage(id, process_HB_message(request));
            }            
            else if (type.empty())
            {
                // Do nothing
            }
            else
            {
// Remove autoreply to prevent feedback loops
                CsDebug(2, (2, "parseMessage could not parse message with type %s", type.c_str()));
//                sendMessage(id, process_unkown_message(request)); 
            }
        }
        else
        {
            CsDebug(2, (2, "parseMessage could not parse message"));
        }
    }
    catch(std::runtime_error &e)
    {
        CsErrx("parseMessage caught runtime error: %s", e.what());
    }
}

void processReceiveEvent(IPC_JSON_t *p)
{
    std::string buf;
    if(getReceiveMessage(p->id, buf))
    { 
        parseMessage(p->id, buf);          
    }
}

extern "C" void* WebsocketProcessingThread(void* arg)
{     
    // Clear the IPC Queue
    IpcFlush(wsQueueId);
    
    while ( s_running ) 
    {
        char    receiveBuf[4096];
        size_t  bytesReceived = 0;
        if ( IpcReceive(wsQueueId, receiveBuf, sizeof receiveBuf, &bytesReceived) == -1 )
        {
            CsErrx("IpcReceive failed");
        }
        else
        {
            IPC_NoPayload_t *m = (IPC_NoPayload_t *)receiveBuf;

            if ( bytesReceived == 0 )
            {
                CsErrx("WS::process: got empty message");
                continue;
            }
            if ( bytesReceived == sizeof receiveBuf )
            {
                CsErrx("WS::process:: got maximum sized message, consider increasing buffer size");
            }
            
            switch (m->hdr.type) 
            {                 
            case IPC_WS_RECEIVE_EVENT:
                processReceiveEvent((IPC_JSON_t *)m);         
                break;                
            case IPC_HEARTBEAT_REQUEST:
                CsDebug(3, (3, "WSP::process: got IPC_HEARTBEAT_REQUEST"));
                m->hdr.type     = IPC_HEARTBEAT_RESPONSE;
                m->hdr.source   = (TaskID_t)DC_TASK; // TODO Review
                IPCSend(appQueueId, m, sizeof *m);
                break;
                
            case IPC_HEARTBEAT_RESPONSE:
                CsDebug(3, (3, "WSP::process: got IPC_HEARTBEAT_RESPONSE"));
                break;
                
            case IPC_TASK_STARTED:
                CsDebug(3, (3, "WSP::process: got IPC_TASK_STARTED"));
                break;
                
            case IPC_TASK_STOPPED:
                // One of the proxys has stopped.
                CsDebug(3, (3, "WSP::process: got IPC_TASK_STOPPED"));                   
                break;
                
            case IPC_TASK_STOP:
                CsDebug(3, (3, "WSP::process: got IPC_TASK_STOP"));                   
                s_running = false;
                // TODO other shutdown and optionally call this method.
                break;
               
            default:
                CsErrx("WSP::process: got type '%d'(%d) from task %d", m->hdr.type, bytesReceived, m->hdr.source);
                break;
            }
        }
    }
    
	return 0;
}

void InitialiseWebsocketProcessingThread()
{
	CsThread_t threadId;
    CsThrCreate(NULL, 0, WebsocketProcessingThread, 0, CSTHR_BOUND, &threadId);
}


// TODO Remove or add a means to determine if processing should be async.
extern "C" void* AsynchronousProcessingThread(void* arg)
{     
    CsDebug(3, (3, "AsynchronousProcessingThread"));
    // Clear the IPC Queue
    IpcFlush(asQueueId);
    
    while ( s_running ) 
    {
        char    receiveBuf[4096];
        size_t  bytesReceived = 0;
        if ( IpcReceive(asQueueId, receiveBuf, sizeof receiveBuf, &bytesReceived) == -1 )
        {
            CsErrx("IpcReceive failed");
        }
        else
        {
            IPC_NoPayload_t *m = (IPC_NoPayload_t *)receiveBuf;

            if ( bytesReceived == 0 )
            {
                CsErrx("ASP::process: got empty message");
                continue;
            }
            if ( bytesReceived == sizeof receiveBuf )
            {
                CsErrx("ASP::process:: got maximum sized message, consider increasing buffer size");
            }
            
            switch (m->hdr.type) 
            {                 
            case IPC_WS_RECEIVE_EVENT:
            case IPC_AS_RECEIVE_EVENT:
				CsDebug(6, (6, "ASP::process: got IPC_WS_RECEIVE_EVENT"));
                processReceiveEvent((IPC_JSON_t *)m);
                break;    
            }
        }
    }
    CsDebug(3, (3, "AsynchronousProcessingThread - end"));
    
    return 0;
}

void InitialiseAsynchronousProcessingThread()
{
	CsThread_t threadId;
    CsThrCreate(NULL, 0, AsynchronousProcessingThread, 0, CSTHR_BOUND, &threadId);
}

void processAutoLogOff()
{
  Json::FastWriter fw; 
  
  processShiftEnd(false);
  
  s_activeTerminalId  = "";
  s_activeStaffId     = "";
  s_activeCSN         = "";
  s_activeType        = "";
  s_activeRoles       = "";
  s_passwordLogin     = false; 
  
  CsDebug(1, (1, "processAutoLogOff  resetting vars"));
  
  sendLogOffMessages(true, false, "");
  s_passwordForceLogOff = false;
  setState(DCSTATE_STANDBY, false, true);
}

void processShiftEnd(bool bNewDriver)
{
    Json::FastWriter fw; 
    
    CsDebug(4, (4, "processShiftEnd g_terminalType %s s_activeType %s", g_terminalType, s_activeType.c_str()));
    
    if(strcmp(g_terminalType, TERMINAL_TYPE_BUS) == 0 && s_activeType.compare("driver") == 0)
    {
        Json::Value message; 
        message["terminalid"]         = s_activeTerminalId;
        message["name"]               = "shiftend";
        message["type"]               = "set";
        message["data"]["portion"]    = "0";
        message["data"]["cardupdate"] = "0";
        message["data"]["newdriver"]  = bNewDriver;

        sendReaderMessage(fw.write(message));
    }
}

void sendLogOffMessages(bool flagDCMessage, bool flagReaderMessage, const std::string& newTerminalId)
{
    Json::Value logoff;
    Json::FastWriter fw; 
    
    logoff["terminalid"]    = s_activeTerminalId;
    logoff["newterminalid"] = newTerminalId;
    logoff["name"]          = "logoff";
  
    if(flagDCMessage)
    {
        CsDebug(1, (1, "sendLogOffMessages  sendDCMessage"));  
        sendDCMessage(fw.write(logoff));
    }
    
    if(flagReaderMessage)
    {
        CsDebug(1, (1, "sendLogOffMessages  sendReaderMessage"));  
        sendReaderMessage(fw.write(logoff));
    }
}

