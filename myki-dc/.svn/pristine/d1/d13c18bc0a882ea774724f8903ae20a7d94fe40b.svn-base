/**
 * @file
 * @copyright 2015 Vix Technology. All rights reserved
 * $Author$
 * $Date$
 * $Rev$
 * @brief Defines helper functions for TxnList sub-module.
 */

#include "helpers.h"
#include "cashtransaction.h"
#include "tpursetransaction.h"
#include "topupcartitem.h"
#include "txnmanager.h"
#include <cs.h>
#include <cstring>
#include <sstream>
#include <string>
#include <stdexcept>

using namespace std;
using namespace TxnList;

CartItemPtr TxnList::cartItemFromJson(const Json::Value &json)
{
    CartItemPtr txn;

    bool reversed = (json.isMember("isreversed") && json["isreversed"].asBool());

    if (json["type"] == "LLSC")
    {
        txn.reset(new TopupCartItem(
                    json["type"].asString(),
                    json["Price"].asInt(),
                    json["id"].asInt(),
                    json["short_desc"].asString(),
                    json["GSTApplicable"].asBool(),
                    json["addvaluemedia"]["txnseqno"].asInt(),
                    json["addvaluemedia"]["cardnumber"].asString(),
                    reversed));
    }
    else
    {
        txn.reset(new CartItem(
                    json["type"].asString(),
                    json["Price"].asInt(),
                    json["id"].asInt(),
                    json["short_desc"].asString(),
                    json["GSTApplicable"].asBool(),
                    reversed));
    }

    return txn;
}

TransactionPtr TxnList::transactionFromJson(TxnManager &txnManager,
        const Json::Value &json)
{
    const time_t timestamp = json["saletimestamp"].asInt();
    const string paymentType = json["paymenttype"].asString();

    TransactionPtr txnPtr;

    if (paymentType == "cash")
    {
        txnPtr.reset(new CashTransaction(txnManager, timestamp));
    }
    else if (paymentType == "tpurse")
    {
        txnPtr.reset(new TPurseTransaction(txnManager,
                    timestamp,
                    json["paymentmedia"]["cardnumber"].asString(),
                    json["paymentmedia"]["txnseqno"].asInt()));
    }
    else
    {
        throw runtime_error("Invalid payment type: " + paymentType);
    }

    return txnPtr;
}


Json::Value TxnList::toJson(const TxnMap &txnMap)
{
    Json::Value json(Json::arrayValue);

    for (TxnMap::const_iterator iter = txnMap.begin(); iter != txnMap.end();
            ++iter)
    {
        Json::Value tmpTransaction = iter->second->toJson();
        tmpTransaction["saleseqno"] = iter->first;
        json.append(tmpTransaction);
    }

    return json;
}
