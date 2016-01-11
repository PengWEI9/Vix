#include <string>
#include <fstream>
#include "jsonParser.h"
#include <json/json.h>
#include <cs.h>
#include <string.h>
#include <iostream>
#include <string>
#include <stdio.h>
#include <stdlib.h>

time_t isoStrToTime(const std::string &timeStr);
//using namespace std;
JSONParser::JSONParser(char *JSONFilePath)
{
    // parse file , ignoring comments inside the json file
    std::ifstream receiptsData(JSONFilePath, std::ifstream::binary);

    bool parsingSuccessful = m_jsonTemplate.parse( receiptsData, root, false);
    if ( !parsingSuccessful )
    {
        // report to the user the failure and their locations in the document.
        CsErrx("JSONParser::JSONParser Failed to parse Receipt.json file."); 
    }
}

/*==========================================================================*
 **
 **  getJasonParser
 **
 **  Description     :
 **      constructor Get new Json parser for new IPC message format 
 **      
 **  Parameters      :
 **      string data, is Json message receiving form DC through IPC   
 **
 **  Returns         :
 **      None
 **
 **  Notes           :
 **
 **==========================================================================*/
JSONParser::JSONParser(std::string data)
{
    // parse file , ignoring comments inside the json file

    bool parsingSuccessful = m_jsonTemplate.parse(data,root_newMsgFormat, false);
    if ( !parsingSuccessful )
    {
        // report to the user the failure and their locations in the document.
        CsErrx("JSONParser::JSONParser Failed to parse IPC Json msg for non-transitSale "); 
    }
}


JSONParser::~JSONParser()
{

}

const Json::Value JSONParser::getTitle(const receipt_e &receipt)
{

    Json::Value receiptTitle;   // will contain the root value after parsing.

    switch(receipt)
    {
    case TaxInvoice:
        receiptTitle = root["Receipts"]["TaxInvoice"]["Title"];			
        break;
    case Surcharge:
        receiptTitle = root["Receipts"]["Surcharge"]["Title"];
        break;
    case Reversal:
        receiptTitle = root["Receipts"]["Reversal"]["Title"];
        break;
    case Paper:
        receiptTitle = root["Receipts"]["Paper"]["Title"];
        break;
    case TopUp:
        receiptTitle = root["Receipts"]["TopUp"]["Title"];			

        break;
    case NonTransit:
        receiptTitle = root["Receipts"]["NonTransit"]["Title"];			
        break;
    case DriverShift:
        receiptTitle = root["Receipts"]["ShiftReport"]["Title"];
        break;
    case NonTransitReversal:
        receiptTitle = root["Receipts"]["NonTransitReversal"]["Title"];
        break;
    default:
        CsWarnx("JSONParser::getTitle Unknown type");
        break;
    }
    return receiptTitle;
}

const Json::Value JSONParser::getHeader(const receipt_e &receipt)
{

    Json::Value receiptHeader;   // will contains the root value after parsing.

    switch(receipt)
    {
    case DriverShift:
        receiptHeader = root["Receipts"]["ShiftReport"]["Header"];
        break;
    case Surcharge:
        receiptHeader = root["Receipts"]["Surcharge"]["Header"];
        break;
    default:
        receiptHeader = root["Receipts"]["GeneralHeader"];
        break;
    }
    return receiptHeader;
}


const Json::Value JSONParser::getNewMsgFormatJason()
{
    return root_newMsgFormat;
}

const Json::Value JSONParser::getBody(const receipt_e &receipt)
{

    Json::Value receiptBody;   // will contains the root value after parsing.

    switch(receipt)
    {
    case TaxInvoice:
        receiptBody = root["Receipts"]["TaxInvoice"]["Body"];
        break;
    case Surcharge:
        receiptBody = root["Receipts"]["Surcharge"]["Body"];
        break;
    case Reversal:
        receiptBody = root["Receipts"]["Reversal"]["Body"];
        break;
    case Paper:
        receiptBody = root["Receipts"]["Paper"]["Body"];
        break;
    case TopUp:
        receiptBody = root["Receipts"]["TopUp"]["Body"];
        break;
    case NonTransit:
        receiptBody = root["Receipts"]["NonTransit"]["Body"];			
        break;
    case DriverShift:
        receiptBody = root["Receipts"]["ShiftReport"]["Body"];
        break;
    case NonTransitReversal:
        receiptBody = root["Receipts"]["NonTransitReversal"]["Body"];
        break;
    default:
        CsWarnx("JSONParser::getBody Unknown type");
        break;
    }

    return receiptBody;
}


const Json::Value JSONParser::getFooter(const receipt_e &receipt)
{

    Json::Value receiptFooter;   // will contains the root value after parsing.

    switch(receipt)
    {
    case DriverShift:
        receiptFooter = root["Receipts"]["ShiftReport"]["Footer"];
        break;
    case Surcharge:
        receiptFooter = root["Receipts"]["Surcharge"]["Footer"];
        break;
    default:
        receiptFooter = root["Receipts"]["GeneralFooter"];
        break;
    }
    return receiptFooter;
}

const Json::Value JSONParser::getProductBody(const receipt_e &receipt)
{
    Json::Value ProductBody;   // will contains the root value after parsing.

    switch(receipt)
    {
    case NonTransitProductBody:
        ProductBody = root["Receipts"]["NonTransit"]["ProductBody"];
        break;
    case SurchargeProductBody:
        ProductBody = root["Receipts"]["Surcharge"]["ProductBody"];
        break;
    case TopUpProducBody:
        ProductBody = root["Receipts"]["TopUp"]["ProductBody"];
        break;
    default:
        ProductBody = root["Receipts"];
        break;
    }
    return ProductBody;
}

const Json::Value JSONParser::getBodyHeader(const receipt_e &receipt)
{
    Json::Value BodyHeader;   // will contains the root value after parsing.

    switch(receipt)
    {
    case NonTransitReversal:
        BodyHeader = root["Receipts"]["NonTransitReversal"]["BodyHeader"];
        break;
    default:
        BodyHeader = root["Receipts"];
        break;
    }
    return BodyHeader;
}
time_t isoStrToTime(const std::string &timeStr)
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

    time_t tmp = mktime(&t);
    // std::cout << "isoStrToTime: tmp: " << tmp <<std::endl;
    return tmp;
}
