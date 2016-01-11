/**
 * @file
 * @copyright 2015 Vix Technology. All rights reserved
 * $Author$
 * $Date$
 * $Rev$
 * @brief Contains functions for handling productsalesreversal messages.
 */

#include "productsalesreversal.h"
#include "../utils.h"
#include "../dc.h"
#include <txnlist/txnlist.h>
#include <printerbackup/printerbackupmanager.h>
#include <boost/lexical_cast.hpp>
#include <string>

using boost::lexical_cast;

namespace {

    /**
     * @brief Creates a product sales reversal printer message, and sends it to
     * the printer process.
     * @param request JSON object from readerapp.
     */
    void sendProductSalesReversalPrinterMsg(const Json::Value &request)
    {
        Json::Value printerMsg = createBasePrinterMsg(request);

        printerMsg["data"]["ReversalMessage"] = g_reversalMessage;

        if (sendJsonIpcMessage(printerTaskId, IPC_PRINTER_JSON, 
                    printerMsg) != 0)
        {
            CsErrx("sendProductSalesReversalPrinterMsg:"
                    " sendJsonIpcMessage failed");
        }

        printerBackupManager().save(lexical_cast<string>(printerMsg));
    }
}

string process_GET_productsalesreversal(const Json::Value &request)
{
    CsDbg(1, "process_GET_productsalesreversal: %s", request["terminalid"].asCString());

    const int saleSeqNo = request["data"]["saleseqno"].asInt();
    const Json::Value &items = request["data"]["cartitems"];

    // Ensure transaction exists

    TxnList::TransactionPtr txnPtr = txnManager().getTransaction(saleSeqNo);

    if (txnPtr == NULL)
    {
        CsErrx("process_GET_productsalesreversal: Transaction %d not found", 
                saleSeqNo);
        return lexical_cast<string>(createErrorResponse(request));
    }

    // Ensure cart items exist 

    for (Json::ValueIterator iter = items.begin(); iter != items.end(); ++iter)
    {
        const int itemNumber = (*iter)["itemnumber"].asInt();

        if (txnPtr->getCartItem(itemNumber) == NULL)
        {
            CsErrx("process_GET_productsalesreversal: Cart item %d for"
                    " transaction %d not found", saleSeqNo, itemNumber);
            return lexical_cast<string>(createErrorResponse(request));
        }
    }

    Json::Value response = createBaseResponse(request);
    Json::Value readerMessage = response;

    readerMessage["data"] = request["data"];
    readerMessage["type"] = "set";

    response["processing"] = true;

    Json::FastWriter fw;

    sendReaderMessage(fw.write(readerMessage));
    return fw.write(response);
}

string process_SET_productsalesreversal(const Json::Value &request)
{
    CsDbg(1, "process_SET_productsalesreversal: %s", 
            request["terminalid"].asCString());
    Json::FastWriter fw;
    Json::Value browserMessage = request;

    if (request["string_error"] != "WS_SUCCESS")
    {
        sendDCMessage(lexical_cast<string>(request));
        return "";
    }

    int saleSeqNo = request["data"]["saleseqno"].asInt();
    const Json::Value &items = request["data"]["cartitems"];

    TxnList::TransactionPtr txnPtr = txnManager().getTransaction(saleSeqNo);

    if (txnPtr == NULL)
    {
        CsErrx("process_SET_productsalesreversal: transaction %d not found", 
                saleSeqNo);
        sendDCMessage(lexical_cast<string>(createErrorResponse(request)));
        return "";
    }

    for (Json::ValueIterator iter = items.begin(); iter != items.end(); ++iter)
    {
        const int itemNumber = (*iter)["itemnumber"].asInt();

        try {
            txnPtr->reverseCartItem(itemNumber);
            txnManager().saveToFile();
        }
        catch (exception &e)
        {
            CsErrx("process_SET_productsalesreversal: cart item %d for"
                    " transaction %d not found", saleSeqNo, itemNumber);
            sendDCMessage(lexical_cast<string>(createErrorResponse(request)));
            return "";
        }
    }
    
    sendDCMessage(fw.write(browserMessage));
    
    Json::Value response = createBaseResponse(request);
    sendProductSalesReversalPrinterMsg(request);
    return "";
}
