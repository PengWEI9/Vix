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
#ifndef DC_UTILS_H
#define DC_UTILS_H

#include <string>
#include <json/json.h>

// forward declarations
namespace PrinterBackup { class PrinterBackupManager; }
namespace TxnList { class TxnManager; };

/**
 * @return Static instance of TxnManager object.
 */
TxnList::TxnManager &txnManager();

 /**
 * @return Static instance of PrinterBackupManager object.
 */
PrinterBackup::PrinterBackupManager &printerBackupManager();

/**
 * @brief Retrieves CS INI value as an integral.
 * @param pType The name of the value to lookup.
 * @param value An out-parameter to store integral value in.
 * @return 0 for success, otherwise failure.
 */
int CsfIniExpandInt(const char *pType, int *value);

/**
 * @brief Retrieves CS INI value as a boolean.
 * @param pType The name of the value to lookup.
 * @param value An out-parameter to store boolean value in.
 * @return 0 for success, otherwise failure.
 */
int CsfIniExpandBool(const char *pType, bool *value);

/**
 * @brief Converts an ISO-8601-format date string to a timestamp.
 * @param timeStr The string to convert.
 * @return A time_t corresponding to the given string passed asn argument.
 * @return -1 if the date string could not be converted.
 */
time_t isoStrToTime(const std::string &timeStr);

/**
 * @brief Sends JSON as IPC message, broken up into multiple messages if 
 * necessary.
 * @param messageQueueId The queue to add our message to. This can specify
 * different applications, eg a message queue for a printer application.
 * @param taskType The particular message type, eg IPC_PRINTER_NONTRANSIT. 
 * @param json The JSON data to send.
 */
int sendJsonIpcMessage(int messageQueueId, int taskType, 
        const Json::Value &json);

/**
 * @brief Creates a Json object populated with common request fields, to serve
 * as a response message.
 * @param request
 */
Json::Value createBaseResponse(const Json::Value &request);

/**
 * @brief Creates a Json object populated with common request fields, as well
 * as error fields.
 * @param request
 */
Json::Value createErrorResponse(const Json::Value &request);

/**
 * @brief Returns the current device location. If none can be found, "Default 
 * Stop" is returned.
 *
 * @return Device location.
 */
std::string getLocation();

/**
 * @brief Makes a copy of the given JSON object, then populates it with common
 * printer data, such as company details.
 * @param request A JSON message from readerapp, upon which we build our printer
 * message.
 * @return A copy of the request object, augmented with common printer message 
 * data.
 */
Json::Value createBasePrinterMsg(Json::Value request);

#endif
