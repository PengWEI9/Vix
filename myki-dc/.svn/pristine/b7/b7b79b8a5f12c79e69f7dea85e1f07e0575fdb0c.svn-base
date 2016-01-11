/**
 * @file
 * @copyright 2015 Vix Technology. All rights reserved
 * $Author$
 * $Date$
 * $Rev$
 * @brief Contains implementation of TopupCartItem class.
 */

#include "topupcartitem.h"
#include <cstring>

using namespace std;
using namespace TxnList;

Json::Value TopupCartItem::toJson() const
{
    Json::Value json = CartItem::toJson();

    // add TopupCartItem-specific fields
    json["addvaluemedia"]["cardnumber"] = getCardSerialNumber();
    json["addvaluemedia"]["txnseqno"] = getCardTxnSequenceNumber();

    /**
     * duplicate fields, for compatability with code expecting older tpurseload
     * field names.
     */
    json["txntype"] = getType();
    json["amount"] = getPrice();

    return json;
}
