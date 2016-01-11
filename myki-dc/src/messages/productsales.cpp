/**
 * @file
 * @copyright 2015 Vix Technology. All rights reserved
 * $Author$
 * $Date$
 * $Rev$
 * @brief Contains functions for handling productsales messages.
 */

#include "productsales.h"

#include "../utils.h"
#include "../dc.h"
#include <csf.h>
#include <txnlist/helpers.h>
#include <printerbackup/printerbackupmanager.h>
#include <txnlist/txnlist.h>
#include <boost/lexical_cast.hpp>
#include <boost/bind.hpp>
#include <boost/lambda/lambda.hpp>
#include <string>

using boost::lexical_cast;

namespace {

    /**
     * @brief Creates a product sales printer message, and sends it to the
     * printer process.
     * @param request JSON object from readerapp.
     * @param printTaxInvoice Whether or not to print the tax invoice.
     */
    void sendProductSalesPrinterMsg(const Json::Value &request, 
            bool printTaxInvoice)
    {
        Json::Value printerMsg = createBasePrinterMsg(request);
        printerMsg["data"]["PrintTaxInvoice"] = printTaxInvoice; 
        printerMsg["data"]["SalesMessage"] = g_salesMessage;

        if (sendJsonIpcMessage(printerTaskId, IPC_PRINTER_JSON, 
                    printerMsg) != 0)
        {
            CsErrx("sendProductSalesPrinterMsg: sendJsonIpcMessage failed");
        }
        
        /* Force PrintTaxInvoice flag to have invoice printed in case of ReprintReceipt button press */
        printerMsg["data"]["PrintTaxInvoice"] = true;

        printerBackupManager().save(lexical_cast<string>(printerMsg));
    }

    /**
     * @brief Converts a JSON object into a Transaction.
     * @param json 
     * @return Shared pointer to Transaction object.
     */
    TxnList::TransactionPtr transactionFromJson(const Json::Value &request)
    {
        Json::Value data = request["data"];
        data["timestamp"] = static_cast<int>(isoStrToTime(request["timestamp"].asString()));
        return TxnList::transactionFromJson(txnManager(), data);
    }
}

std::string process_GET_productsales(const Json::Value &request)
{
    CsDbg(1, "process_GET_productsales: %s", request["terminalid"].asCString());

    Json::Value response = createBaseResponse(request); 
    Json::Value message = createBaseResponse(request);

    Json::FastWriter fw;

    message["data"] = request["data"];
    message["type"] = "set";

    response["processing"] = true;

    sendReaderMessage(fw.write(message));
    return fw.write(response);
}

std::string process_SET_productsales(const Json::Value &request)
{
    CsDbg(1, "process_SET_productsales: %s", request["terminalid"].asCString());

    // browser message
    Json::FastWriter fw;
    Json::Value browserMessage = request;

    if (request["string_error"] != "WS_SUCCESS")
    {
        CsErrx("process_SET_productsales: %s", request["string_error"].asCString());
        sendDCMessage(fw.write(browserMessage));
        return "";
    }

    TxnList::TransactionPtr txnPtr(transactionFromJson(request));
    txnManager().addTransaction(request["data"]["saleseqno"].asInt(), txnPtr);

    const Json::Value &items = request["data"]["cartitems"];
    const unsigned int total = request["data"]["amounttotal"].asUInt();

    // whether or not tpurseload included in transaction
    bool includesTPurseLoad = false;

    for (Json::ValueIterator iter = items.begin(); iter != items.end(); ++iter)
    {
        const int itemNumber = (*iter)["itemnumber"].asInt();

        if ((*iter)["type"].asString() == "LLSC") includesTPurseLoad = true;

        txnPtr->addCartItem(itemNumber, TxnList::cartItemFromJson(*iter));
    }

    txnManager().saveToFile();
    
    /*
     *  Requirement:
     *  1/ If AutoPrintReceipt=N and Total sales amount is less than MinimumReceiptAmount, 
     *     tax invoice shouldn't be printed
     *  2/ If AutoPrintReceipt=N and Total sales amount is more than MinimumReceiptAmount, 
     *     tax invoice should be printed
     *  3/ If AutoPrintReceipt=Y, irrespective of sales amount, tax invoice should be printed
     *  4/ If sale includes add value, irrespective of AutoPrintReceipt and sale amount, 
     *     tax invoice is always printed
     */
    bool printTaxInvoice = (includesTPurseLoad 
            || g_autoPrintReceipt
            || total > g_minimumReceiptAmount);
    
    CsDbg(4, "printTaxInvoice: total <%d> g_autoPrintReceipt<%s> includesTPurseLoad<%s>", 
             total, (g_autoPrintReceipt ? "TRUE" : "FALSE"), (includesTPurseLoad ? "TRUE" : "FALSE"));
    
    sendDCMessage(fw.write(browserMessage));    
    sendProductSalesPrinterMsg(request, printTaxInvoice);    

    return "";
}
