/***************************************************************************
 * Copyright (c) 2014 Vix Technology. All rights reserved
 *
 * Module name   : utils
 * Module type   : 
 * Compiler(s)   : C++
 * Environment(s): LINUX
 *
 * Description:
 *  Miscellaneous set of helper and utility functions
 *       
 *
 * Contents:
 *
     
 *

 * Version   Who      Date       Description
   1.0       MRA      02/10/2015 Created
 *
 * $Id:  $
 * $HeadURL: $
 *.........................................................................*/
#include "utils.h"
#include "dc.h"
#include <txnlist/txnmanager.h>
#include <printerbackup/printerbackupmanager.h>
#include <message_base.h>
#include <ipc.h>
#include <csf.h>
#include <boost/lexical_cast.hpp>

using namespace std;
using boost::lexical_cast;

TxnList::TxnManager &txnManager()
{
    static bool loadedFromFile = false;

    try {
        static TxnList::TxnManager instance;
        if (!loadedFromFile) instance.loadFromFile();
        loadedFromFile = true;
        return instance;
    }
    catch (exception &e)
    {
        CsErrx("Failed to initialise TxnManager: %s", e.what());
        throw;
    }
}

PrinterBackup::PrinterBackupManager &printerBackupManager()
{
    static PrinterBackup::PrinterBackupManager instance(printerTaskId);
    return instance;
}

int CsfIniExpandInt(const char *pType, int *value)
{
    if (value == NULL)
    {
        return -1;
    }

    char buf[CSFINI_MAXVALUE];
    int ret = 0;

    if ((ret = CsfIniExpand(pType, buf, CSFINI_MAXVALUE)) != 0)
    {
        CsErrx("CsfIniExpandInt: CsfIniExpand returned %d", ret);
        return ret;
    }

    CsDbg(8, "CsfIniExpand: retrieved %s", buf);

    if ((ret = sscanf(buf, "%d", value)) != 1)
    {
        CsErrx("CsfIniExpandInt: sscanf returned %d", ret);
        return -1;
    }

    return 0;
}

int CsfIniExpandBool(const char *pType, bool *value)
{
    if (value == NULL)
    {
        return -1;
    }

    char buf[CSFINI_MAXVALUE];
    int ret = 0;

    if ((ret = CsfIniExpand(pType, buf, CSFINI_MAXVALUE)) != 0)
    {
        return ret;
    }

    if (buf[0] == 'y' || buf[0] == 'Y')
    {
        *value = true;
    }
    else 
    {
        *value = false;
    }

    return 0;
}

time_t isoStrToTime(const string &timeStr)
{
    struct tm t;

    if (sscanf(timeStr.c_str(), "%d-%d-%dT%d:%d:%d+%*d:00",
            &(t.tm_year),
            &(t.tm_mon),
            &(t.tm_mday),
            &(t.tm_hour),
            &(t.tm_min),
            &(t.tm_sec)) != 6)
    {
        return -1;
    }

    // adjust values to match 'struct tm' spec
    t.tm_year -= 1900;
    t.tm_mon -= 1;

    return mktime(&t);
}

int sendJsonIpcMessage(int messageQueueId, int taskType, 
        const Json::Value &json)
{
    CsDbg(4, "sendJsonIpcMessage: %s", json.toStyledString().c_str());

    IPC_header_t hdr = { taskType, DC_PRINTER_TASK };

    Json::FastWriter writer;
    string jsonStr = writer.write(json);
    return IpcSendMulti(messageQueueId, &hdr, jsonStr.c_str(), jsonStr.size());
}

Json::Value createBaseResponse(const Json::Value &request)
{
    Json::Value response;
	CsTime_t t;
    CsTime(&t);
	char timeBuf[64];
	CsStrTimeISO(&t,64,timeBuf);
    
    response["terminalid"]  = request["terminalid"];
    response["userid"]      = request["userid"];
    response["name"]        = request["name"];
    response["timestamp"]   = timeBuf;
    response["type"]        = request["type"];
    response["data"]        = Json::Value();

    if (request.isMember("string_error"))
    {
        response["string_error"] = request["string_error"];
    }

    response["errorcode"] = 0;

    return response;
}

Json::Value createErrorResponse(const Json::Value &request)
{
    Json::Value response = createBaseResponse(request);
    response["errorcode"] = 1;
    response["string_error"] = "WS_ERROR";
    return response;
}

std::string getLocation()
{
    MYKI_CD_Locations_t location = {};
	MYKI_CD_RouteStop_t* pRouteStops = NULL;
    int routeStopCount = 0;

    if (s_overrideEnabled)
    {
        routeStopCount = MYKI_CD_getRouteStopsStructure(s_routeId, 
                s_overrideStopId, s_overrideStopId, &pRouteStops);
    }
    else
    {
        routeStopCount = MYKI_CD_getRouteStopsStructure(s_routeId, s_stopId, 
                s_stopId, &pRouteStops);
    }

    if (routeStopCount == 1 && 
            MYKI_CD_getLocationsStructure(pRouteStops[0].locationId, &location))
    {
        return location.short_desc;
    }

    return "Default Stop";
}

Json::Value createBasePrinterMsg(Json::Value request)
{
    request["data"]["CompanyPhone"] = g_companyPhone;
    request["data"]["CompanyName"] = g_companyName;
    request["data"]["CompanyABN"] = g_companyABN;
    request["data"]["CompanyWebSite"] = g_companyWebsite;
    request["data"]["BusId"] = g_vehicleId;
    request["data"]["TerminalId"] = request["terminalid"];
    request["data"]["ShiftNumber"] = lexical_cast<int>(s_tablecard["shift_number"].asString());
    request["data"]["Location"] = getLocation();
    request["data"]["ReceiptInvoiceThreshold"] = g_receiptInvoiceThreshold;

    return request;
}
