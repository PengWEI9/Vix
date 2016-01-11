#ifndef JSONPARSER_H
#define JSONPARSER_H

// myki specific
#include <string>
#include <cstdio>
#include <json/json.h>
#include <message_dc.h>

enum receipt_e
{
    Unknown     = 0,
    TaxInvoice  = 1,
    Surcharge   = 2,
    Reversal    = 3,
    Paper       = 4,
    TopUp       = 5,
    DriverShift = 6, 
    NonTransit  = 7, 
    NonTransitProductBody= 8, 
    SurchargeProductBody= 9, 
    TopUpProducBody= 10, 
    NonTransitReversal= 11, 
    __last
};

class JSONParser
{
    public:
        // constructor
        JSONParser(char *JSONFilePath);
        JSONParser(std::string data);
        // destructor.
        ~JSONParser();
        
		const Json::Value getTitle(const receipt_e &receipt);
        const Json::Value getHeader(const receipt_e &receipt);
        const Json::Value getBody(const receipt_e &receipt);
        const Json::Value getFooter(const receipt_e &receipt);
        const Json::Value getCartItems();
        const Json::Value getNewMsgFormatJason();
        const Json::Value getProductBody(const receipt_e &receipt);
        const Json::Value getBodyHeader(const receipt_e &receipt);

    private:
        Json::Value  root;
        Json::Value  root_newMsgFormat;
        Json::Reader m_jsonTemplate;
        std::string  m_taxInvoice;
        std::string  m_reversal;
        std::string  m_paper;
        std::string  m_surcharge;
};


#endif

