#ifndef MYKIPRINTER_H
#define MYKIPRINTER_H

#include <iostream>
#include <stdint.h>
#include <Serial.h>
#include <cstdio>
#include <message_base.h>
#include <message_dc.h>

#include "jsonParser.h"
#include "SerialPrinter.h"
//#include "EpsonCommands.h"


/* Inherited methods
   int print(std::istream& is);
   int print(const std::string& text);
   int sendCommand(const unsigned char *command);
   */	
enum receiptType_e
{
    NONTRANSIT_SALE                     = 0,
    NONTRANSIT_SALE_REVERSE             = 1,
    NONTRANSIT_SURCHARGE                = 2,
    NONTRANSIT_SALE_REVERSE_WITH_TOPUP  = 3,
    NONTRANSIT_SALE_WITH_TOPUP 			= 4,
    NONTRANSIT_INVOICE_WITH_SURCHARGE 	= 5,
    NONTRANSIT_PRODUCT_SALE 			= 6,
    DEFAULT,
};
class  MykiPrinter : public SerialPrinter
{
    public:
        // Constructor: Serial initialization
        MykiPrinter(char *JSONFilePath, int reportRecieveCharacter_SourceQueueId, int reportRecieveCharacter_MessageId, const char* port, unsigned long speed, unsigned int data_bits, char parity, unsigned int stop_bits);
        // Destructor.
        ~MykiPrinter();
        
		// Printing methods per message type
        int  printTopUp(IPC_Printer_TpurseLoad_t *p);
	int  printTopUpReversal(IPC_Reversal_TpurseLoad_t *p);              
	int  printDriverShiftReport(IPC_DriverShiftReport_t *p);              
    int  printNonTransit(Json::Value &jsonData, enum receiptType_e receiptType );
    void processIPCJson(std::string &data);
    int printNonTransitRevsersal(Json::Value &jsonData,enum receiptType_e receiptType);
      
    private:
        MykiPrinter();
		
    bool printingConditionCheck(Json::Value &data,Json::Value &cartItems,Json::Value::Members lines,std::string memberName, unsigned int index,enum receiptType_e receiptType );
    void populatePlaceHolderWithPrint(Json::Value &data,Json::Value &cartItems, Json::Value::Members lines,std::string memberName,
            unsigned int index, Json::Value cartProduct,enum receiptType_e receiptType);
	// Top Up receipt formatting methods
	void populateTopUpPlaceHolder(Json::Value &data, IPC_Printer_TpurseLoad_t *p);		 
	
    // Top Up Reversal receipt formatting methods
	void populateTopUpRevPlaceHolder(Json::Value &data, IPC_Reversal_TpurseLoad_t *p); 

    // Surcharge receipt formatting methods
    void populateSurchargePlaceHolder(Json::Value &data, Json::Value &cartItems, bool processingBodyflag = false);
				
	// Non-Transit receipt formatting methods
    void populateNonTransitPlaceHolder(Json::Value &data, Json::Value &cartitems, enum receiptType_e receiptType,bool processingBodyflag = false);		 

    // Driver shift Report formatting 
    void populateDriverShiftPlaceHolder( Json::Value &data,  IPC_DriverShiftReport_t *driverShiftData ); 
    void populateDriverShiftBodyPlaceHolder(std::string& dataStr, IPC_DriverShiftReport_t *driverShiftData);

    // Standard string formatting and placeholders replacement functions
    void populateCompanyHolders(std::string &dataString, Json::Value cartItems);  
    void populateCommonHolders(std::string &dataString, Json::Value JsonMessage, Json::Value cartSingleItem, enum receiptType_e receiptType); 

    // Printing function
    void render(Json::Value &data, enum receiptType_e receiptType); 
    JSONParser  *m_jsonParser;
};




#endif
