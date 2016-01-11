/**
 * @file
 * @copyright 2015 Vix Technology. All rights reserved
 * $Author$
 * $Date$
 * $Rev$
 * @brief TPurseTransaction class interface.
 */

#include "tpursetransaction.h"
#include <json/json.h>

using namespace std;
using namespace TxnList;

Json::Value TPurseTransaction::toJson() const
{
    Json::Value json = Transaction::toJson();

    json["paymentmedia"]["cardnumber"] = getCardNumber();
    json["paymentmedia"]["txnseqno"] = getCardTxnSequenceNumber();

    return json;
}
