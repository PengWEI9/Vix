#include <iostream>
#include <iomanip>
#include <sstream> 
#include <cerrno>
#include <cstdlib>
#include <time.h>
#include <csfcmd.h>
#include <string.h>
#include <Serial.h>
#include "MykiPrinter.h"

#define TIME_STR_MAX_LENGTH	24 

using namespace std;
const int MAX_ROW_LENGTH = 36;            //Max characters per row in Normal font
const int MAX_FOOTER_ROW_LENGTH = 24;
/*
   Helper functions for myki printer
   */

/**
 * @brief Breaks up text into lines so that each line does not exceed maxLen.
 *
 * Text always breaks on words - so a word never gets chopped in half.
 * @parma text
 * @param maxLen
 * @return 
 */
static std::string wrapLine(const std::string &text, size_t maxLen)
{
	std::istringstream instream(text);
	std::ostringstream outstream;

	std::string tmpWord;
	std::size_t offset = 0;

	// iterate word-by-word
	while (instream >> tmpWord)
	{
		// insert newline if tmpWord, including space, will push us over limit
		if (outstream.tellp() && 
				(tmpWord.size() + outstream.tellp() - offset >= maxLen))
		{
			outstream << std::endl;
			offset = outstream.tellp();
		}
		// insert space on all words but the first of every line
		else if (outstream.tellp())
		{
			outstream << " ";
		}

		outstream << tmpWord;
	}

	return outstream.str();
}

void replaceString( std::string& str, const std::string& oldStr, const std::string& newStr );

std::string formatCardNumber( std::string cardNumber );
std::string formatAmount( int amount, bool mask = false );
std::string formatIDNumber( int nNumber );
std::string toString( time_t& time, char * timeString, int maxTimeStringLength );
std::string toString( int number, bool doMask );
std::string toTxnTimeString( time_t& time, char * timeString, int maxTimeStringLength );
time_t isoStrToTime(const std::string &timeStr);
std::string MonthNumberToNameConversion(int monthNo);
std::string processProductListString(std::string itemName,std::string itemPrice,bool isRightPosition,bool &hasNoGstItem, bool GSTApplicable);
std::string processItemNoString(std::string itemName,std::string itemPrice,bool isRightPosition);
std::string stringWraping(std::string data);

	template <typename T>
std::string NumberToString(T num)
{
	using namespace std;
	stringstream converter;
	converter << num;
	return converter.str();
}

/*==========================================================================*
 **
 **  MykiPrinter
 **
 **  Description     :
 **      Constructor that initialises the connection to the serial port. 
 **
 **      Also subscribes a task id and an IPC message to the serial class provided by 
 **      'reportRecieveCharacter_SourceQueueId' and 'reportRecieveCharacter_SourceQueueId'.
 **      This will be used by the read thread internally in the serial class. 
 **      Any received data coming from the serial port will be reported to the message queue 
 **      provided by reportRecieveCharacter_SourceQueueId.
 **      
 **  Parameters      :
 **      JSONFilePath                                       [I]     Path to JSON template
 **      reportRecieveCharacter_SourceQueueId               [I]     Task ID
 **      reportRecieveCharacter_MessageId                   [I]     IPC message ID
 **      port                                               [I]     Serial port to connect to
 **      speed                                              [I]     speed baud rate
 **      data_bits                                          [I]     typically 8
 **      parity                                             [I]     'n', 'o', 'e', 'm', 's' 
 **      stop_bits                                          [I]     1 or 2
 **
 **  Returns         :
 **      None
 **
 **  Notes           :
 **
 **==========================================================================*/
MykiPrinter::MykiPrinter(char *JSONFilePath, 
		int reportRecieveCharacter_SourceQueueId, 
		int reportRecieveCharacter_MessageId,
		const char* port, 
		unsigned long speed, 
		unsigned int data_bits, 
		char parity, 
		unsigned int stop_bits)
: SerialPrinter(reportRecieveCharacter_SourceQueueId, 
		reportRecieveCharacter_MessageId, 
		port, 
		speed, 
		data_bits, 
		parity, 
		stop_bits)
{
	m_jsonParser = new JSONParser(JSONFilePath);
}

/*==========================================================================*
 **
 **  printTopUpReversal
 **
 **  Description     :
 **      Public interface of the application to send request to print a Top up receipt
 **      
 **  Parameters      :
 **      pTopUpRevStruct              IPC message containing fields of the receipt
 **
 **  Returns         :
 **      None
 **
 **  Notes           :
 **
 **==========================================================================*/
int MykiPrinter::printTopUpReversal(IPC_Reversal_TpurseLoad_t *pTopUpRevStruct)
{
	const enum receipt_e tax_receipt = Reversal;

	Json::Value titleData	= m_jsonParser->getTitle(tax_receipt);
	Json::Value headerData	= m_jsonParser->getHeader(tax_receipt);     
	Json::Value bodyData    = m_jsonParser->getBody(tax_receipt); 
	Json::Value footerData  = m_jsonParser->getFooter(tax_receipt);     

	populateTopUpRevPlaceHolder( titleData,  pTopUpRevStruct );
	populateTopUpRevPlaceHolder( headerData, pTopUpRevStruct );
	populateTopUpRevPlaceHolder( bodyData,   pTopUpRevStruct );
	populateTopUpRevPlaceHolder( footerData, pTopUpRevStruct );

	// Final line feeds and cut
	sendCommand(LINE_FEED);
	sendCommand(LINE_FEED);
	sendCommand(LINE_FEED);
	sendCommand(LINE_FEED);

	sendCommand(PARTIAL_CUT);

	return 0;
}

/*==========================================================================*
 **
 **  populateTopUpRevPlaceHolder
 **
 **  Description     :
 **      Helper function to replace any placeholders provided in the template
 **      
 **  Parameters      :
 **      data                          json template containing the fields to render
 **      pPrinterMsg                   pointer to Top Up IPC data structure
 **
 **  Returns         :
 **      None
 **
 **  Notes           :
 **      Refer to the json template on how to structure it.
 **==========================================================================*/
void MykiPrinter::populateTopUpRevPlaceHolder(Json::Value &data, IPC_Reversal_TpurseLoad_t *pReversalStruct)
{
	// Replace specific receipt place holders with values
	Json::Value::Members lines = data.getMemberNames();
	Json::Value  cartSingleItem;
	enum receiptType_e receiptType = DEFAULT;

	for ( unsigned int index = 0; index < lines.size(); index++ )
	{
		std::string memberName = lines[index];

		if( data[memberName]["hasPlaceHolder"].asBool() )
		{
			std::string temp = data[memberName]["data"].asString();    

			CsDebug(3, (3, "MykiPrinter::populatePlaceHolder:: temp:%s",temp.c_str()));

			this->populateCompanyHolders( temp, &(pReversalStruct->companyData) );	            

			// Top Up specific fields
			if( temp.find("[AmountTopUp]") != std::string::npos ) 
				replaceString( temp, "[AmountTopUp]", formatAmount(pReversalStruct->amountTopUp) ); 

			if( temp.find("[AmountNewBalance]") != std::string::npos ) 
				replaceString( temp, "[AmountNewBalance]", formatAmount(pReversalStruct->amountNewBalance) );  

			if( temp.find("[AmountTotal]") != std::string::npos ) 
				replaceString( temp, "[AmountTotal]", formatAmount(pReversalStruct->amountTotal) ); 

			if( temp.find("[OrigShiftNumber]") != std::string::npos ) 
				replaceString( temp, "[OrigShiftNumber]",  formatIDNumber( pReversalStruct->commonData.shiftNumber ));

			if( temp.find("[OrigTransactionNumber]") != std::string::npos ) 
				replaceString( temp, "[OrigTransactionNumber]",  formatIDNumber( pReversalStruct->origTransactionNumber ));

			if( temp.find("[OrigDate]") != std::string::npos ) 
			{
				char formattedTime[TIME_STR_MAX_LENGTH] = "";
				replaceString( temp, "[OrigDate]", toTxnTimeString( pReversalStruct->origDate, formattedTime, TIME_STR_MAX_LENGTH ) ) ;
			}	

			this->populateCommonHolders( temp, &(pReversalStruct->commonData),cartSingleItem, DEFAULT);		 

			CsDebug(3, (3, "MykiPrinter::populatePlaceHolder:: 2 temp:%s",temp.c_str()));

			data[memberName]["data"] = temp;
		}

		// Format output and print
		this->render( data[memberName], receiptType );
	}	
}
/*==========================================================================*
 **
 **  processIPCJson
 **
 **  Description     :
 **  Taking Json message and branch to different processing function
 **      
 **  Parameters      :
 **                     data this is json message stored as string receiving from DC through IPC
 **  Returns         :
 **      None
 **
 **  Notes           :
 **
 **==========================================================================*/

void MykiPrinter::processIPCJson(std::string &data)
{
	Json::Value jsonData;
	Json::Reader reader;
	reader.parse(data, jsonData);

	std::cout << jsonData.toStyledString() << std::endl;

	if (jsonData["name"].asString().compare("productsalesreversal") == 0)
		printNonTransitRevsersal(jsonData, NONTRANSIT_SALE_REVERSE);

	else if (jsonData["name"].asString().compare("productsales") == 0)
		printNonTransit(jsonData,NONTRANSIT_SALE);
}

/*==========================================================================*
 **
 **  printNonTransit
 **
 **  Description     :
 **      Public interface of the application to send request to print a non-transit receipt
 **      
 **  Parameters      :
 **                     data this is json message stored as string receiving from DC through IPC
 **  Returns         :
 **      None
 **
 **  Notes           :
 **
 **==========================================================================*/
int MykiPrinter::printNonTransit(Json::Value &jsonData, enum receiptType_e receiptType )
{    
	if (!jsonData["data"]["surcharge"].empty() && !jsonData["data"]["surcharge"]["cartitems"].empty())
	{
		Json::Value surchargeJson = jsonData;
		surchargeJson["data"]["cartitems"] = surchargeJson["data"]["surcharge"]["cartitems"];
		surchargeJson["data"]["amounttotal"] = surchargeJson["data"]["surcharge"]["amounttotal"];
		surchargeJson["data"]["amountgst"] = surchargeJson["data"]["surcharge"]["amountgst"];
		surchargeJson["data"]["amountrounding"] = surchargeJson["data"]["surcharge"]["amountrounding"];
		surchargeJson["data"]["amountpaid"] = surchargeJson["data"]["surcharge"]["amountpaid"];
		surchargeJson["data"].removeMember("surcharge");

		JSONParser parser(surchargeJson.toStyledString());

		Json::Value titleData	= m_jsonParser->getTitle(Surcharge);
		Json::Value headerData	= m_jsonParser->getHeader(Surcharge);     
		Json::Value bodyData    = m_jsonParser->getBody(Surcharge); 
		Json::Value footerData  = m_jsonParser->getFooter(Surcharge);

		populateNonTransitPlaceHolder( titleData, surchargeJson,NONTRANSIT_SURCHARGE);
		populateNonTransitPlaceHolder( headerData, surchargeJson, NONTRANSIT_SURCHARGE);
		populateNonTransitPlaceHolder( bodyData, surchargeJson, NONTRANSIT_SURCHARGE,true);
		populateNonTransitPlaceHolder( footerData, surchargeJson, NONTRANSIT_SURCHARGE);

		sendCommand(LINE_FEED);
		sendCommand(LINE_FEED);
		sendCommand(LINE_FEED);
		sendCommand(LINE_FEED);

		sendCommand(PARTIAL_CUT);
	}

	if (!jsonData["data"].isMember("PrintTaxInvoice")
			|| jsonData["data"]["PrintTaxInvoice"].asBool())
	{
		Json::Value titleData	= m_jsonParser->getTitle(NonTransit);
		Json::Value headerData	= m_jsonParser->getHeader(NonTransit);     
		Json::Value bodyData    = m_jsonParser->getBody(NonTransit); 
		Json::Value footerData  = m_jsonParser->getFooter(NonTransit);

		populateNonTransitPlaceHolder( titleData, jsonData,receiptType);
		populateNonTransitPlaceHolder( headerData, jsonData,receiptType);
		populateNonTransitPlaceHolder( bodyData, jsonData,receiptType,true );
		populateNonTransitPlaceHolder( footerData, jsonData,receiptType);

		sendCommand(LINE_FEED);
		sendCommand(LINE_FEED);
		sendCommand(LINE_FEED);
		sendCommand(LINE_FEED);

		sendCommand(PARTIAL_CUT);
	}

	return 0;
}

/*==========================================================================*
 **
 **  printNonTransitRevsersal
 **
 **  Description     :
 **      Public interface of the application to send request to print a non-transit reversal receipt
 **      
 **  Parameters      :
 **                     data this is json message stored as string receiving from DC through IPC
 **  Returns         :
 **      None
 **
 **  Notes           :
 **
 **==========================================================================*/
int MykiPrinter::printNonTransitRevsersal(Json::Value &jsonData,enum receiptType_e receiptType)
{    
	if (!jsonData["data"].isMember("PrintTaxInvoice")
			|| jsonData["data"]["PrintTaxInvoice"].asBool())
	{
		Json::Value titleData	= m_jsonParser->getTitle(NonTransitReversal);
		Json::Value headerData	= m_jsonParser->getHeader(NonTransitReversal);     
		Json::Value bodyHeader= m_jsonParser->getBodyHeader(NonTransitReversal); 
		Json::Value bodyData    = m_jsonParser->getBody(NonTransitReversal); 
		Json::Value footerData  = m_jsonParser->getFooter(NonTransitReversal);

		populateNonTransitPlaceHolder( titleData, jsonData,receiptType );
		populateNonTransitPlaceHolder( headerData, jsonData,receiptType);
		populateNonTransitPlaceHolder(bodyHeader , jsonData,receiptType);
		populateNonTransitPlaceHolder( bodyData, jsonData,receiptType,true);
		populateNonTransitPlaceHolder( footerData, jsonData,receiptType);

		sendCommand(LINE_FEED);
		sendCommand(LINE_FEED);
		sendCommand(LINE_FEED);
		sendCommand(LINE_FEED);

		sendCommand(PARTIAL_CUT);
	}
	return 0;
}

/*==========================================================================*
 **
 **  populateNonTransitPlaceHolder
 **
 **  Description     :
 **      Helper function to replace any placeholders provided in the template
 **      
 **  Parameters      :
 **      data                  json template containing the fields to render
 **      cartItem              json msg sending from DC through IPC
 **      enum receiptType      type of the receipt for printing 
 **      processingBodyflag    true if need to process product cart item
 **   Returns         :
 **      None
 **
 **  Notes           :
 **      Refer to the json template on how to structure it.
 **==========================================================================*/

void MykiPrinter::populateNonTransitPlaceHolder(Json::Value &data, Json::Value &cartItems, 
		enum receiptType_e receiptType, bool processingBodyflag )
{
	// Replace specific receipt place holders with values
	std::string tmpItem;
	stringstream convert;
	Json::Value product;

	if(processingBodyflag == true)
	{
		Json::Value tmpJson = cartItems["data"]["cartitems"];
		for(unsigned int indexCart= 0; indexCart < tmpJson.size(); indexCart ++)
		{
			tmpItem.clear();
			Json::Value cartitems = tmpJson[indexCart];
			if(cartitems["type"].asString().compare("ThirdParty") == 0)
			{
				Json::Value productBody  = m_jsonParser->getProductBody(NonTransitProductBody);
				Json::Value::Members lines = productBody.getMemberNames();
				for ( unsigned int index = 0; index < lines.size(); index++ )
				{
					std::string memberName = lines[index];
					if (printingConditionCheck(productBody,cartItems,lines, memberName,  index,DEFAULT ))
					{
						continue;
					}
					populatePlaceHolderWithPrint(productBody,cartItems,lines, memberName, index, cartitems,NONTRANSIT_PRODUCT_SALE);
				}	
			}
			else if(cartitems["type"].asString().compare("PremiumSurcharge") == 0)
			{
				Json::Value productBody  = m_jsonParser->getProductBody(SurchargeProductBody);
				Json::Value::Members lines = productBody.getMemberNames();
				unsigned int index = 0;
				for (  index = 0; index < lines.size(); index++ )
				{
					std::string memberName = lines[index];
					if (printingConditionCheck(productBody,cartItems,lines, memberName,  index,receiptType))
					{
						continue;
					}
					if(receiptType == NONTRANSIT_SURCHARGE)
						populatePlaceHolderWithPrint(productBody,cartItems,lines, memberName, index, cartitems,receiptType);
					else
						populatePlaceHolderWithPrint(productBody,cartItems,lines, memberName, index, cartitems,NONTRANSIT_INVOICE_WITH_SURCHARGE);
				}
			}
			else if(cartitems["type"].asString().compare("LLSC") == 0)
			{
				Json::Value productBody  = m_jsonParser->getProductBody(TopUpProducBody);
				Json::Value::Members lines = productBody.getMemberNames();
				for ( unsigned int index = 0; index < lines.size(); index++ )
				{
					std::string memberName = lines[index];
					if (printingConditionCheck(productBody,cartItems,lines, memberName,  index,DEFAULT))
					{
						continue;
					}
					if(receiptType == NONTRANSIT_SALE_REVERSE)
						populatePlaceHolderWithPrint(productBody,cartItems,lines, memberName, index, cartitems,NONTRANSIT_SALE_REVERSE_WITH_TOPUP);
					else
						populatePlaceHolderWithPrint(productBody,cartItems,lines, memberName, index, cartitems,NONTRANSIT_SALE_WITH_TOPUP);
				}	
			}
		}
		processingBodyflag = false;
	}

	Json::Value::Members lines = data.getMemberNames();
	for ( unsigned int index = 0; index < lines.size(); index++ )
	{
		std::string memberName = lines[index];
		if (printingConditionCheck(data,cartItems,lines, memberName,  index,DEFAULT))
		{
			continue;
		}
		populatePlaceHolderWithPrint(data,cartItems,lines, memberName, index, product,receiptType);
	}	

}


bool MykiPrinter::printingConditionCheck(Json::Value &data,Json::Value &cartItems,Json::Value::Members lines,std::string memberName, unsigned int index,enum receiptType_e receiptType )
{
	if( data[memberName].isMember("condition") )
	{
		if( !data[memberName]["condition"].asString().compare("RIMP")  && 
				(cartItems["data"]["ReceiptInvoiceThreshold"].asInt() - cartItems["data"]["amountpaid"].asInt() >= 0.01 ) 
		  )
		{
			return true;
		}  		
		if( !data[memberName]["condition"].asString().compare("GST")  && 
				(cartItems["data"]["amountgst"].asInt() == 0) 
		  )
		{
			return true;
		}  		
		if( !data[memberName]["condition"].asString().compare("Cash")  && 
				(cartItems["data"]["paymenttype"].asString().compare("tpurse")) == 0
		  )
		{
			return true;
		}  		
		if( !data[memberName]["condition"].asString().compare("Tpurse")  && 
				(cartItems["data"]["paymenttype"].asString().compare("cash")) == 0 
		  )
		{
			return true;
		}  		
		if( !data[memberName]["condition_1"].asString().compare("Rounding")  && 
				cartItems["data"]["amountrounding"].asInt() == 0

		  )
		{
			return true;
		} 
		if( !data[memberName]["condition"].asString().compare("SurchargeTicketOnly")  && 
				receiptType != NONTRANSIT_SURCHARGE

		  )
		{
			return true;
		} 
		return false;
	}
	else
		return false;
}

void MykiPrinter::populatePlaceHolderWithPrint(Json::Value &data,Json::Value &cartItems, Json::Value::Members lines,
		std::string memberName, unsigned int index,Json::Value cartProduct,enum receiptType_e receiptType)
{
	if( data[memberName]["hasPlaceHolder"].asBool() )
	{
		std::string temp = data[memberName]["data"].asString();    

		CsDebug(3, (3, "MykiPrinter::populatePlaceHolder:: temp:%s",temp.c_str()));

		if( temp.find("[AmountTotal]") != std::string::npos ) 
		{
			if(receiptType == NONTRANSIT_SALE_REVERSE)
			{
				replaceString( temp, "[AmountTotal]", formatAmount(cartItems["data"]["amountrefunded"].asInt()) ); 
			}
			else 
			{
				replaceString( temp, "[AmountTotal]", formatAmount(cartItems["data"]["amountpaid"].asInt()) ); 

			}
		}

		if( temp.find("[ItemNumber]") != std::string::npos ) 
			replaceString( temp, "[ItemNumber]",NumberToString(cartProduct["itemnumber"].asInt()) );  

		if( temp.find("[Gst]") != std::string::npos )
		{
			if(cartProduct["GSTApplicable"].asBool())
			{
				if(receiptType == NONTRANSIT_SURCHARGE)
					replaceString( temp, "[Gst]","" );  
				else
					replaceString( temp, "[Gst]","*" );  
			}
			else
				replaceString( temp, "[Gst]","" );  
		}

		if( temp.find("[ProductName]") != std::string::npos ) 
		{
			if(receiptType == NONTRANSIT_SURCHARGE || receiptType == NONTRANSIT_INVOICE_WITH_SURCHARGE)
			{
				if(cartProduct["short_desc"].asString().size() >(unsigned int) MAX_ROW_LENGTH)
					replaceString( temp, "[ProductName]",cartProduct["short_desc"].asString().erase(MAX_ROW_LENGTH,std::string::npos));  
				else
					replaceString( temp, "[ProductName]",cartProduct["short_desc"].asString() );  
			}
			else
				replaceString( temp, "[ProductName]",cartProduct["short_desc"].asString() );  
		}

		if( temp.find("[Price]") != std::string::npos ) 
			replaceString( temp, "[Price]",formatAmount(cartProduct["Price"].asInt()) );  

		if(temp.find("[LongDescription]") != std::string::npos) 
			replaceString( temp, "[LongDescription]",stringWraping(cartProduct["long_desc"].asString())); 

		if( temp.find("[AmountNewBalance]") != std::string::npos ) 
		{        
			if(receiptType == NONTRANSIT_SALE_REVERSE_WITH_TOPUP || receiptType == NONTRANSIT_SALE_WITH_TOPUP)
			{
				replaceString(temp, "[AmountNewBalance]",formatAmount( cartProduct["addvaluemedia"]["remvalue"].asInt()));
			}	
			else 
			{
				replaceString(temp, "[AmountNewBalance]", formatAmount( cartItems["data"]["paymentmedia"]["remvalue"].asInt()));
			}
		}
		if( !temp.find("[RIMP]") != std::string::npos ) 
			replaceString( temp, "[RIMP]", formatAmount(cartItems["data"]["ReceiptInvoiceThreshold"].asInt()));

		if( !temp.find("[GST]") != std::string::npos ) 
			replaceString( temp, "[GST]", formatAmount(cartItems["data"]["amountgst"].asInt()) );

		if( !temp.find("[AmountTotalBeforeRounding]") != std::string::npos ) 
			replaceString( temp, "[AmountTotalBeforeRounding]", formatAmount(cartItems["data"]["amounttotal"].asInt()) );

		if( !temp.find("[AmountRounding]") != std::string::npos ) 
			replaceString( temp, "[AmountRounding]", formatAmount(cartItems["data"]["amountrounding"].asInt()) );

		if( !temp.find("[AmountRounding]") != std::string::npos ) 
			replaceString( temp, "[AmountRounding]", formatAmount(cartItems["data"]["amountrounding"].asInt()) );
		this->populateCommonHolders( temp,cartItems,cartProduct,receiptType);		 

		data[memberName]["data"] = temp;
	}
	this->render( data[memberName], receiptType );
}


/*==========================================================================*
 **
 **  printTopUp
 **
 **  Description     :
 **      Public interface of the application to send request to print a Top up receipt
 **      
 **  Parameters      :
 **      pTopUpStruct          IPC message containing fields of the receipt
 **
 **  Returns         :
 **      None
 **
 **  Notes           :
 **
 **==========================================================================*/
int MykiPrinter::printTopUp(IPC_Printer_TpurseLoad_t *pTopUpStruct)
{
	const enum receipt_e tax_receipt = TopUp;

	Json::Value titleData	= m_jsonParser->getTitle(tax_receipt);
	Json::Value headerData	= m_jsonParser->getHeader(tax_receipt);     
	Json::Value bodyData    = m_jsonParser->getBody(tax_receipt); 
	Json::Value footerData  = m_jsonParser->getFooter(tax_receipt);     

	populateTopUpPlaceHolder( titleData,  pTopUpStruct );
	populateTopUpPlaceHolder( headerData, pTopUpStruct );
	populateTopUpPlaceHolder( bodyData,   pTopUpStruct );
	populateTopUpPlaceHolder( footerData, pTopUpStruct );

	// Final line feeds and cut
	sendCommand(LINE_FEED);
	sendCommand(LINE_FEED);
	sendCommand(LINE_FEED);
	sendCommand(LINE_FEED);

	sendCommand(PARTIAL_CUT);
	return 0;
}


/*==========================================================================*
 **
 **  populateTopUpPlaceHolder
 **
 **  Description     :
 **      Helper function to replace any placeholders provided in the template
 **      
 **  Parameters      :
 **      data                  json template containing the fields to render
 **      pPrinterMsg           pointer to Top Up IPC data structure
 **
 **  Returns         :
 **      None
 **
 **  Notes           :
 **      Refer to the json template on how to structure it.
 **==========================================================================*/
void MykiPrinter::populateTopUpPlaceHolder(Json::Value &data, IPC_Printer_TpurseLoad_t *pTopUpStruct)
{
	// Replace specific receipt place holders with values
	Json::Value::Members lines = data.getMemberNames();
	Json::Value  cartSingleItem;
	enum receiptType_e receiptType = DEFAULT;

	for ( unsigned int index = 0; index < lines.size(); index++ )
	{
		std::string memberName = lines[index];
		if( data[memberName].isMember("condition") )
			if( !data[memberName]["condition"].asString().compare("RIMP")  && 
					( pTopUpStruct->receiptInvoiceThreshold - pTopUpStruct->amountTotal >= 0.01 ) 
			  )
			{
				continue;
			}  		

		if( data[memberName]["hasPlaceHolder"].asBool() )
		{

			std::string temp = data[memberName]["data"].asString();    

			CsDebug(3, (3, "MykiPrinter::populatePlaceHolder:: temp:%s",temp.c_str()));

			this->populateCompanyHolders( temp, &(pTopUpStruct->companyData) );	

			//Top Up specific fields
			if( temp.find("[AmountTopUp]") != std::string::npos ) 
				replaceString( temp, "[AmountTopUp]", formatAmount(pTopUpStruct->amountTopUp) ); 

			if( temp.find("[AmountNewBalance]") != std::string::npos ) 
				replaceString( temp, "[AmountNewBalance]", formatAmount(pTopUpStruct->amountNewBalance) );  

			if( temp.find("[AmountTotal]") != std::string::npos ) 
				replaceString( temp, "[AmountTotal]", formatAmount(pTopUpStruct->amountTotal) ); 

			if( !temp.find("[RIMP]") != std::string::npos ) 
				replaceString( temp, "[RIMP]", formatAmount(pTopUpStruct->receiptInvoiceThreshold) );

			this->populateCommonHolders( temp, &(pTopUpStruct->commonData),cartSingleItem,DEFAULT );		 

			data[memberName]["data"] = temp;
		}

		// Format output and print
		this->render( data[memberName], receiptType);
	}	

}


/*==========================================================================*
 **
 **  printDriverShiftReport
 **
 **  Description     :
 **      Public interface of the application to send request to print a specified Driver Shift Report 
 **      
 **  Parameters      :
 **      driverShiftData          pointer to IPC_Driver_Shift_Report_t structure containing all the fields to be printed on the reportr
 **
 **  Returns         :
 **      None
 **
 **  Notes           :
 **
 **==========================================================================*/
int MykiPrinter::printDriverShiftReport(IPC_DriverShiftReport_t *driverShiftData)
{
	const enum receipt_e driverReport = DriverShift;
	Json::Value titleData	= m_jsonParser->getTitle( driverReport );
	Json::Value headerData	= m_jsonParser->getHeader( driverReport );     
	Json::Value bodyData   	= m_jsonParser->getBody( driverReport ); 
	Json::Value footerData  = m_jsonParser->getFooter( driverReport );     

	populateDriverShiftPlaceHolder( titleData,  driverShiftData );
	populateDriverShiftPlaceHolder( headerData, driverShiftData );
	populateDriverShiftPlaceHolder( bodyData,   driverShiftData );
	populateDriverShiftPlaceHolder( footerData, driverShiftData );

	// Final line feeds and cut
	sendCommand(LINE_FEED);
	sendCommand(LINE_FEED);
	sendCommand(LINE_FEED);
	sendCommand(LINE_FEED);

	sendCommand(PARTIAL_CUT);

	return 0;
}


/*==========================================================================*
 **
 **  populateDriverShiftPlaceHolder
 **
 **  Description     :
 **      Helper function to replace any placeholders provided in the template
 **      IS_FIRST_PORTION( 
 **  Parameters      :
 **      reportData            		json template containing the fields to render
 **      driverShiftData      		pointer to IPC_Driver_Shift_Report_t structure containing data to be printed on the Driver Shift Report
 **
 **  Returns         :
 **      None
 **
 **  Notes           :
 **==========================================================================*/


void MykiPrinter::populateDriverShiftPlaceHolder(Json::Value &reportData, IPC_DriverShiftReport_t *driverShiftData )
{
	Json::Value::Members lines = reportData.getMemberNames();
	IPC_DriverShiftDetails_t *driverShiftDetails = &(driverShiftData->shiftDetails);
	enum receiptType_e receiptType = DEFAULT;

	for ( unsigned int index = 0; index < lines.size(); index++ ) {
		std::string memberName = lines[index];

		// Determine whether Cash Brought Forward  or Out of Balance line should be present in the report
		// The Brought Forward amount will be printed for all but the first shift portion even if it is $0.00
		if( reportData[memberName].isMember("condition") ) {

			if( ( !reportData[memberName]["condition"].asString().compare("BF")  && 
						( driverShiftDetails->shiftCash.cashStatus == CASH_OUT_OF_BALANCE  ||
						  driverShiftDetails->shiftStartDate ==  driverShiftDetails->lastPortionStartDate ) ) ||
					( !reportData[memberName]["condition"].asString().compare("OB")  && 
					  ( driverShiftDetails->shiftCash.cashStatus != CASH_OUT_OF_BALANCE ) ) ) { 
				continue;
			}
		}	

		if( reportData[memberName]["hasPlaceHolder"].asBool() ) {
			std::string temp = reportData[memberName]["data"].asString();    

			CsDebug(3, (3, "MykiPrinter::populateDriverShiftPlaceHolder:: temp:%s",temp.c_str()));

			bool isManualReport = (driverShiftData->reportType == REPORT_MANUAL);
			bool isFirstPortion = (driverShiftDetails->shiftStartDate == driverShiftDetails->lastPortionStartDate);

			// don't show "Net Cash BF" on manual shift-end, or the when it's the first portion of a shift
			if (temp.find("[AmountBroughtForward]") != std::string::npos && (isManualReport || isFirstPortion))
			{
				continue;
			}

			// Shift Report Title
			if( temp.find("[ReportName]") != std::string::npos ) {
				std::string repTitle;
				switch( driverShiftData->reportType ) {
				case REPORT_END_OF_SHIFT:
					repTitle = "END OF SHIFT";
					break;
				case REPORT_SHIFT_PORTION:
					repTitle = "SHIFT PORTION";
					break;
				case REPORT_MANUAL:
					repTitle = "MANUAL END OF SHIFT";
					break;
				default:
					CsErrx("MykiPrinter:: Unknown Driver report type \n");
					break;		
				} 
				replaceString( temp, "[ReportName]", repTitle ); 
			}

			// Shift Report Header fields
			replaceString( temp, "[OperatorID]", driverShiftDetails->operatorID ); 
			if ( temp.find( "[ShiftNumber]" ) != std::string::npos ) 
				replaceString( temp, "[ShiftNumber]", toString(driverShiftDetails->shiftNumber, false ) ); 
			replaceString( temp, "[BusPlateN]", driverShiftDetails->busID );

			char formattedTime[TIME_STR_MAX_LENGTH] = "";
			if ( temp.find( "[DateTimeShiftStart]" ) != std::string::npos ) 
				replaceString( temp, "[DateTimeShiftStart]", toString( driverShiftDetails->shiftStartDate, formattedTime, TIME_STR_MAX_LENGTH ) ) ; 
			if ( temp.find( "[DateTimePortionStart]" ) != std::string::npos ) 
				replaceString( temp, "[DateTimePortionStart]", toString( driverShiftDetails->lastPortionStartDate, formattedTime, TIME_STR_MAX_LENGTH ) ) ; 
			if ( temp.find( "[DateTimeShiftEnd]" ) != std::string::npos ) 
				replaceString( temp, "[DateTimeShiftEnd]", toString( driverShiftDetails->shiftEndDate, formattedTime, TIME_STR_MAX_LENGTH ) ); 
			if ( temp.find( "[AmountBroughtForward]" ) != std::string::npos ) 
				replaceString( temp, "[AmountBroughtForward]", formatAmount( driverShiftDetails->shiftCash.cashAmount, driverShiftData->totalsMaskLevel != MASK_NONE ) ); 

			// Cash Summary fields
			if( temp.find( "[AmountPortionCash]" ) != std::string::npos ) {
				int portionNetCash = driverShiftData->netCash - driverShiftDetails->shiftCash.cashAmount; 
				replaceString( temp, "[AmountPortionCash]", formatAmount( portionNetCash, driverShiftData->totalsMaskLevel != MASK_NONE ) ); 
			}
			if( temp.find( "[AmountNetCash]" ) != std::string::npos )
				replaceString( temp, "[AmountNetCash]", formatAmount( driverShiftData->netCash, driverShiftData->totalsMaskLevel != MASK_NONE ) ); 

			if (temp.find("[ShiftReportFooter]") != std::string::npos)
				replaceString(temp, "[ShiftReportFooter]", wrapLine(driverShiftData->footer, MAX_FOOTER_ROW_LENGTH));
			// Shift Report body fields
			this->populateDriverShiftBodyPlaceHolder( temp, driverShiftData );


			reportData[memberName]["data"] = temp;
		}
		// Format output and print
		this->render( reportData[memberName], receiptType);
	}
}


/*==========================================================================*
 **
 **  populateDriverShiftBodyPlaceHolder
 **
 **  Description     :
 **      Helper function to replace any placeholders in the "body" of the Driver Shift report template
 **      
 **  Parameters      :
 **      dataStr            		json template line containing "data" element
 **      driverShiftData		pointer to IPC_Driver_Shift_Report_t structure containing data to be printed on  Driver Shift Report
 **
 **  Returns         :
 **      None
 **
 **  Notes           :
 **      Refer to the json template on how to structure it.
 **==========================================================================*/
void MykiPrinter::populateDriverShiftBodyPlaceHolder(std::string& dataStr, IPC_DriverShiftReport_t *driverShiftData)
{
	IPC_ShiftTotalsMaskLevel_e maskLevel = driverShiftData->totalsMaskLevel; 
	// Shift Report body fields
	if( dataStr.find( "[AmountGrossCash]" ) != std::string::npos )
		replaceString( dataStr, "[AmountGrossCash]", formatAmount( driverShiftData->cashTotals.grossAmount, maskLevel != MASK_NONE ) ); 
	if( dataStr.find( "[AmountAnnulledCash]" ) != std::string::npos )
		replaceString( dataStr, "[AmountAnnulledCash]", formatAmount (driverShiftData->cashTotals.annulledAmount, maskLevel != MASK_NONE ) ); 
	if( dataStr.find( "[AmountNetCash]" ) != std::string::npos )
		replaceString( dataStr, "[AmountNetCash]", formatAmount(driverShiftData->cashTotals.netAmount, maskLevel != MASK_NONE) ) ; 

	if( dataStr.find( "[AmountGrossOther]" ) != std::string::npos )
		replaceString( dataStr, "[AmountGrossOther]", formatAmount( driverShiftData->otherTotals.grossAmount, maskLevel != MASK_NONE ) ); 
	if( dataStr.find( "[AmountAnnulledOther]" ) != std::string::npos )
		replaceString( dataStr, "[AmountAnnulledOther]", formatAmount( driverShiftData->otherTotals.annulledAmount, maskLevel != MASK_NONE ) ); 
	if( dataStr.find( "[AmountNetOther]" ) != std::string::npos )
		replaceString( dataStr, "[AmountNetOther]", formatAmount( driverShiftData->otherTotals.netAmount, maskLevel != MASK_NONE ) ); 

	if( dataStr.find( "[GrossPaperTickets]" ) != std::string::npos )
		replaceString( dataStr, "[GrossPaperTickets]", toString( driverShiftData->paperTickets.grossTickets, maskLevel == MASK_ALL ) ); 
	if( dataStr.find( "[AnnulledPaperTickets]" ) != std::string::npos )
		replaceString( dataStr, "[AnnulledPaperTickets]", toString( driverShiftData->paperTickets.annulledTickets, maskLevel == MASK_ALL ) ); 
	if( dataStr.find( "[NetPaperTickets]" ) != std::string::npos )
		replaceString( dataStr, "[NetPaperTickets]", toString( driverShiftData->paperTickets.netTickets, maskLevel == MASK_ALL ) ); 

	if( dataStr.find( "[GrossSundryTickets]" ) != std::string::npos )
		replaceString( dataStr, "[GrossSundryTickets]", toString( driverShiftData->sundryTickets.grossTickets, maskLevel == MASK_ALL ) ); 
	if( dataStr.find( "[AnnulledSundryTickets]" ) != std::string::npos )
		replaceString( dataStr, "[AnnulledSundryTickets]", toString( driverShiftData->sundryTickets.annulledTickets, maskLevel == MASK_ALL ) ); 
	if( dataStr.find( "[NetSundryTickets]" ) != std::string::npos )
		replaceString( dataStr, "[NetSundryTickets]", toString( driverShiftData->sundryTickets.netTickets, maskLevel == MASK_ALL ) ); 

	if( dataStr.find( "[FullPassCount]" ) != std::string::npos )
		replaceString( dataStr, "[FullPassCount]", toString( driverShiftData->fullPassCount, maskLevel == MASK_ALL ) ); 
	if( dataStr.find( "[ConcessionPassCount]" ) != std::string::npos )
		replaceString( dataStr, "[ConcessionPassCount]", toString( driverShiftData->concessionPassCount, maskLevel == MASK_ALL ) ); 

	if( dataStr.find( "[AnnulledTickets]" )  != std::string::npos )
		replaceString( dataStr, "[AnnulledTickets]", toString( driverShiftData->annulledTickets, maskLevel == MASK_ALL ) ); 
}

/*==========================================================================*
 **
 **  render
 **
 **  Description     :
 **      renders the json template replacing any placeholders provided in the IPC message and 
 **      applies the necessary fonts specified by the json template.
 **      
 **  Parameters      :
 **      data                  json template containing the fields to render
 **
 **  Returns         :
 **      None
 **
 **  Notes           :
 **      Refer to the json template on how to structure it.
 **==========================================================================*/
void MykiPrinter::render(Json::Value &data, enum receiptType_e receiptType)//, std::string memberName) 
{
	// Don't print empty strings
	if (data["data"].asString().empty())
	{
		return;
	}
	// Search for emphasize attributes 
	data["isBold"].asBool() ? sendCommand(SET_BOLD) : sendCommand(UNSET_BOLD);

	// Search for font attributes
	data["font"].asString().compare("small") ?  sendCommand(SMALL_FONT) : sendCommand(DEFAULT_FONT);

	// Search for justification attributes
	std::string temp = data["data"].asString();

	std::string strRightHolder("[Right]");
	std::size_t nRightPos  = temp.find(strRightHolder);

	std::string strMiddleHolder("[Middle]");
	std::size_t nMiddlePos = temp.find(strMiddleHolder);		

	std::string dataWithoutRightHolder = temp;
	std::string::size_type index;
	if((index =temp.find(strRightHolder)) != std::string::npos)
		dataWithoutRightHolder.erase(index,strRightHolder.length());

	if(dataWithoutRightHolder.size() > (unsigned int)MAX_ROW_LENGTH && receiptType == NONTRANSIT_PRODUCT_SALE )
	{
		unsigned int firstLineTruncPositoin = dataWithoutRightHolder.substr(0,MAX_ROW_LENGTH).find_last_of(" ");

		print(dataWithoutRightHolder.substr(0, firstLineTruncPositoin));
		std::cout<<"print: "<<dataWithoutRightHolder.substr(0, firstLineTruncPositoin)<<std::endl;
		sendCommand(LINE_FEED);

		std::string secondLine = dataWithoutRightHolder.substr(firstLineTruncPositoin+1, dataWithoutRightHolder.size()-(firstLineTruncPositoin-1));
		secondLine.insert(secondLine.find_last_of("$")," [Right]");

		// Cut line before [Right] holder and append spaces to fill MAX_ROW_LENGTH, then print
		int countSpace = MAX_ROW_LENGTH -secondLine.length() + strRightHolder.length();
		std::size_t secondLineRightPos = secondLine.find(strRightHolder);

		if( countSpace < 0 )
			countSpace = 0;

		print(secondLine.substr(0, secondLineRightPos - 1).append(countSpace, ' ') );
		print(secondLine.substr((secondLineRightPos )+ strRightHolder.length()) );

		std::cout  << "print: " <<secondLine.substr(0, secondLineRightPos - 1).append(countSpace, ' ')
			<<secondLine.substr(secondLineRightPos + strRightHolder.length()) << std::endl;
		sendCommand(LINE_FEED);
		return;
	}



	if( ( nRightPos != std::string::npos ) || ( nMiddlePos != std::string::npos ) )
	{				
		if ( nRightPos != std::string::npos )
		{
			// Cut line before [Right] holder and append spaces to fill MAX_ROW_LENGTH, then print
			int countSpace = MAX_ROW_LENGTH - temp.length() + strRightHolder.length();

			if( countSpace < 0 )
				countSpace = 0;

			print( temp.substr(0, nRightPos - 1).append(countSpace, ' ') );
			print( temp.substr(nRightPos + strRightHolder.length()) );

			std::cout  << "print: " << temp.substr(0, nRightPos - 1).append(countSpace, ' ')
				<< temp.substr(nRightPos + strRightHolder.length()) << std::endl;
		}
		else
		{
			//Print everything before [Right] and leave the amount of space of
			//between ] in [Right] to end of the row:
			//FullLine:|[content]+++[Right]===[fill]"--------|
			//Prints:  |[content]+++[Right]--------          |
			print( temp.substr( 0, nMiddlePos ) );

			//Print the rest:
			//Prints:  |****************************===[fill]|
			print( temp.substr( nMiddlePos + strMiddleHolder.length() ) );
		}		            
	} 
	else
	{
		if(data["data"].asString().size() >(unsigned int) MAX_ROW_LENGTH)
		{
			std::string longStr = data["data"].asString();
			unsigned int numberOfLines = longStr.size()/MAX_ROW_LENGTH;
			for(unsigned int index = 0; index < numberOfLines+1; index++)
				std::cout << "print: " <<longStr.substr(index* (unsigned int)MAX_ROW_LENGTH, MAX_ROW_LENGTH)<< std::endl;
			print( data["data"].asString() ); 
		}
		else 
		{
			// Print default justification
			std::cout  << "print: " << data["data"].asString() << std::endl;
			print( data["data"].asString() ); 

		}
	}

	sendCommand(LINE_FEED);
}

/*==========================================================================*
 **
 **  populateCommonHolders
 **
 **  Description     :
 **      Replaces general placeholders provided in the JSON template file with 
 **      their values from IPC structure.
 **      
 **  Parameters      :
 **      dataString           json template line with data
 **      pPrinterMsg          pointer to data structure containing general fields valid for all receipts
 **
 **  Returns         :
 **      None
 **
 **  Notes           :
 **      None
 **==========================================================================*/
void MykiPrinter::populateCommonHolders(std::string &dataString, Json::Value cartItems,Json::Value cartSingleItem, enum receiptType_e receiptType)
{
	if( dataString.find("[MykiReference]") != std::string::npos ) 
	{        
		if(receiptType == NONTRANSIT_SALE_REVERSE_WITH_TOPUP)
		{
			replaceString( dataString, "[MykiReference]", formatCardNumber( cartSingleItem["addvaluemedia"]["cardnumber"].asString()));
		}	
		else 
		{
			replaceString( dataString, "[MykiReference]", formatCardNumber( cartItems["data"]["paymentmedia"]["cardnumber"].asString()));
		}
	}
	if( dataString.find("[TransactionNumber]") != std::string::npos )
		replaceString( dataString, "[TransactionNumber]",  formatIDNumber( cartItems["data"]["saleseqno"].asInt()) );


	if( dataString.find("[OrigTransactionNumber]") != std::string::npos )
	{
		if(receiptType == NONTRANSIT_SALE_REVERSE || receiptType == NONTRANSIT_SALE_REVERSE_WITH_TOPUP)
			replaceString( dataString, "[OrigTransactionNumber]",  formatIDNumber( cartItems["data"]["revsaleseqno"].asInt()) );
		else
			replaceString( dataString, "[OrigTransactionNumber]",  formatIDNumber( cartItems["data"]["saleseqno"].asInt()) );
	}

	if( dataString.find("[TerminalID]") != std::string::npos ) 
	{
		if(cartItems["data"]["TerminalId"].isString())
		{
			replaceString( dataString, "[TerminalID]", cartItems["data"]["TerminalId"].asString() );              
		}
		else
		{
			replaceString( dataString, "[TerminalID]", NumberToString(cartItems["data"]["TerminalId"].asInt()) );              
		}
	}
	if( dataString.find("[ShiftNumber]") != std::string::npos ) 
	{
		if(cartItems["data"]["ShiftNumber"].isString())
		{
			replaceString( dataString, "[ShiftNumber]", cartItems["data"]["ShiftNumber"].asString() );              
		}
		else
		{
			replaceString( dataString,"[ShiftNumber]" ,formatIDNumber(cartItems["data"]["ShiftNumber"].asInt()) );              
		}
	}

	if( dataString.find("[Location]") != std::string::npos) 
		replaceString( dataString, "[Location]",stringWraping(cartItems["data"]["Location"].asString())); 
    if( dataString.find("[StopID]") != std::string::npos ) 
    {        
        replaceString( dataString, "[StopID]", NumberToString( cartItems["data"]["stop_id"].asInt()));
    }	
    if( dataString.find("RouteID") != std::string::npos ) 
    {        
        replaceString( dataString, "[RouteID]", NumberToString(cartItems["data"]["route_id"].asInt()));
    }	

	if( dataString.find("[BusID]") != std::string::npos ) 
		replaceString( dataString, "[BusID]",  cartItems["data"]["BusId"].asString());

	if( dataString.find("[StopID]") != std::string::npos ) 
	{        
		replaceString( dataString, "[StopID]", NumberToString( cartItems["data"]["stop_id"].asInt()));
	}	
	if( dataString.find("RouteID") != std::string::npos ) 
	{        
		replaceString( dataString, "[RouteID]", NumberToString(cartItems["data"]["route_id"].asInt()));
	}	

	if( dataString.find("[Message]") != std::string::npos ) 
	{
		if(receiptType == NONTRANSIT_SALE_REVERSE)
			replaceString( dataString, "[Message]",  wrapLine(cartItems["data"]["ReversalMessage"].asString(), MAX_ROW_LENGTH));	
		else
			replaceString( dataString, "[Message]",  wrapLine(cartItems["data"]["SalesMessage"].asString(), MAX_ROW_LENGTH));	
	}


	// Get current date
	if( dataString.find("[Date]") != std::string::npos )
	{
		char formattedTime[TIME_STR_MAX_LENGTH] = "";
		time_t  temTime = isoStrToTime(cartItems["timestamp"].asString());
		toTxnTimeString(temTime, formattedTime, TIME_STR_MAX_LENGTH );
		replaceString( dataString, "[Date]", formattedTime ) ; 
	}	

	if( dataString.find("[OrigDate]") != std::string::npos )
	{
		char formattedTime[TIME_STR_MAX_LENGTH] = "";
		time_t  temTime = isoStrToTime(cartItems["data"]["saletimestamp"].asString());
		toTxnTimeString(temTime, formattedTime, TIME_STR_MAX_LENGTH );
		replaceString( dataString, "[OrigDate]", formattedTime) ; 
	}
}

/*==========================================================================*
 **
 **  populateCompanyHolders
 **
 **  Description     :
 **      Replaces company placeholders provided in the JSON template file with 
 **      their values from IPC structure.
 **      
 **  Parameters      :
 **      dataString           json template line with data
 **      pCompanyData         pointer to data structure containing company data valid for all receipts
 **
 **  Returns         :
 **      None
 **
 **  Notes           :
 **      None
 **==========================================================================*/
void MykiPrinter::populateCompanyHolders(std::string &dataString, Json::Value cartItems )
{
	// Company data	
	if( dataString.find("[CompanyPhone]") != std::string::npos ) 
		replaceString( dataString, "[CompanyPhone]", cartItems["data"]["CompanyPhone"].asString()); 

	if( dataString.find("[CompanyName]") != std::string::npos ) 
		replaceString( dataString, "[CompanyName]",cartItems["data"]["CompanyName"].asString());

	if( dataString.find("[CompanyABN]") != std::string::npos ) 
		replaceString( dataString, "[CompanyABN]", cartItems["data"]["CompanyABN"].asString() );

	if( dataString.find("[CompanyWebSite]") != std::string::npos ) 
		replaceString( dataString, "[CompanyWebSite]", cartItems["data"]["CompanyWebSite"].asString() );
}


MykiPrinter::~MykiPrinter()
{
	delete m_jsonParser;
}

/*==========================================================================*
 **
 **  replaceString
 **
 **  Description     :
 **      Replaces all occurrences of a string in another string
 **      
 **  Parameters      :
 **      str                 string to be manipuated
 **      oldStr              string to be replaced
 **      newStr              string to put in
 **  Returns         :
 **      None
 **
 **==========================================================================*/
void replaceString(std::string& str, const std::string& oldStr, const std::string& newStr)
{
	size_t pos = 0;
	while( (pos = str.find(oldStr, pos)) != std::string::npos )
	{
		str.replace(pos, oldStr.length(), newStr);
		pos += newStr.length();
	}
}


/*==========================================================================*
 **
 **  formatCardNumber
 **
 **  Description     :
 **      Formats myki card number value to N NNNNN XXXX XNNN N
 **      
 **  Parameters      :
 **      string         Card number value
 **  Returns         :
 **      Formatted string
 **
 **==========================================================================*/
std::string formatCardNumber( std::string cardNumber )
{
	std::string strMyKi = cardNumber;

	CsDebug(3, (3, "formatCardNumber:: cardNumber %s",cardNumber.c_str()));

	if(cardNumber.length() < 15)
	{
		// Reference is shorter than expected, do not format, just print
		CsWarnx("formatCardNumber MykiReference %s is shorter than expected", cardNumber.c_str());
		return strMyKi;
	}

	// Cut symbols between 7 and 11 and mask them with XXXXX
	strMyKi = cardNumber.substr(0,6) + "XXXXX" + cardNumber.substr(11);    

	// Add some spaces to format according to spec
	strMyKi.insert(1, 1, ' ');
	strMyKi.insert(7, 1, ' ');
	strMyKi.insert(12, 1, ' ');
	strMyKi.insert(17, 1, ' ');

	CsDebug(3, (3, "formatCardNumber:: strMyKi:%s",strMyKi.c_str()));

	return strMyKi;
}


/*==========================================================================*
 **
 **  formatAmount
 **
 **  Description     :
 **      Converts amount in cents into formatted dollar/cents value or a masks it as ******
 **      
 **  Parameters      :
 **      amount             amount coming from DC (it is multiplied by 100 and stored as int)
 **	 doMask		    flag indicating whether the amoubt should be masked	
 **  Returns         :
 **      std::string         formatted string
 **
 **==========================================================================*/
std::string formatAmount( int amount, bool doMask )
{
	if (doMask)
	{
		return "*******";
	}

	std::stringstream stream;

	// place '-' left of '$'
	if (amount < 0)
	{
		stream << '-';
		amount = abs(amount);
	}

	stream << '$' << std::fixed << std::setprecision(2) << (amount / 100.0);
	return stream.str();
}

/*==========================================================================*
 **
 **  formatIDNumber
 **
 **  Description     :
 **      Formats transaction ID and shift number to NNNN 
 **      
 **  Parameters      :
 **      nNumber             initial ID value
 **
 **  Returns         :
 **      std::string         formatted string
 **
 **==========================================================================*/
std::string formatIDNumber( int nNumber )
{
	std::ostringstream strAmount;

	strAmount << std::setfill('0') << std::setw(4) << nNumber;

	return strAmount.str();
}

/*==========================================================================*
 **
 ** toString 
 **
 **  Description     :
 **      Converts int to string withouut any formatting 
 **      
 **  Parameters      :
 **      number             int value to convert
 **	 doMask		    flag indicating whether the number should be masked	
 **
 **  Returns         :
 **      std::string        value as string
 **
 **==========================================================================*/
std::string toString( int number, bool doMask )
{
	if( doMask == false ) { 
		std::ostringstream s;
		s << number;
		return s.str();
	}
	else
		return ( "*******");
}


/*==========================================================================*
 **
 ** toString 
 **
 **  Description     :
 **      Converts time in time_t to dd/mm/yy hh:mm formatted string 
 **      
 **  Parameters      :
 **      time             	  time_t value to convert
 **      timeString	  	  buffer for the formatted time string
 **      maxTimeStringLength	  size of the buffer 
 **
 **  Returns         :
 **      std::string        formatted  string
 **
 **==========================================================================*/
std::string toString( time_t& time, char * timeString, int maxTimeStringLength )
{
	//        if( strftime(timeString, maxTimeStringLength - 1, "%d/%b/%y %H:%M", localtime( &time ) ) == 0 )
	if( strftime(timeString, maxTimeStringLength - 1, "%d %b %y %H:%M", localtime( &time ) ) == 0 )
	{
		CsErrx("MykiPrinter::toString: Error formatting the time ", errno);
	}
	return timeString;
}


/*==========================================================================*
 **
 ** toTxnTimeString 
 **
 **  Description     :
 **      Converts time in time_t to dd mmm yy hh:mm am / pm formatted string 
 **      
 **  Parameters      :
 **      time             	  time_t value to convert
 **      timeString	  	  buffer for the formatted time string
 **      maxTimeStringLength	  size of the buffer 
 **
 **  Returns         :
 **      std::string        formatted  string
 **
 **==========================================================================*/
std::string toTxnTimeString( time_t& time, char * timeString, int maxTimeStringLength )
{
	if( strftime(timeString, maxTimeStringLength - 1, "%d %b %y %I:%M %p", localtime( &time ) ) == 0 )
	{
		CsErrx("MykiPrinter::toString: Error formatting the time ", errno);

	}
	std::string temp = timeString;
	std::size_t offset;
	// Make sure the am/pm is printed in low case
	if ( ( offset = temp.rfind("AM") ) != std::string::npos || ( offset = temp.rfind("PM") ) != std::string::npos )
	{
		timeString[offset] |= 0x20;
		timeString[offset+1] |= 0x20;
	}
	// Remove leading '0' from the hour
	if ( ( offset = temp.find(":") ) != std::string::npos && ( timeString[offset - 2] == '0' ) )
	{
		strncpy(timeString + offset - 2, timeString + offset - 1, temp.length() - offset + 2 );
	}
	// Do not print the leading '0' for the date 
	return timeString[0] != '0' ? timeString : timeString + 1;
}

/*==========================================================================*
 **
 ** stringWraping
 **
 **  Description     :
 **      format printed string to meed the following format 
 **			(refers to VBDC ticket library section 1.7)
 ** 		1: if last 6 characters in a line(36 char) contain space, bring the 
 ** 			word after space to the next line.
 ** 		2: if last 6 characters in a line(36 char) not contain any space,
 ** 			truncate the word to the next line.
 **      
 **  Parameters      :
 **      time             	  time_t value to convert
 **      timeString	  	  buffer for the formatted time string
 **      maxTimeStringLength	  size of the buffer 
 **
 **  Returns         :
 **      std::string        formatted  string
 **
 **==========================================================================*/

std::string stringWraping(std::string data)
{
	int lineIndex= 0;
	int tmpPosition= 0;

	while(data.substr(lineIndex,MAX_ROW_LENGTH).size() ==(unsigned int) MAX_ROW_LENGTH && data.size() > (unsigned int)MAX_ROW_LENGTH)
	{
		std::string tmp;
		tmp = data.substr(lineIndex,MAX_ROW_LENGTH);
		tmpPosition = tmp.find_last_of(" ");
		if((lineIndex+tmpPosition) >= lineIndex+30 &&(lineIndex+tmpPosition) < lineIndex+35)
		{
			for(int i = 0; i < (35-tmpPosition); i++)
			{
				data.insert(lineIndex+(tmpPosition+1), " ");
			}
		}
		lineIndex += MAX_ROW_LENGTH;
	}
	return data;
}
