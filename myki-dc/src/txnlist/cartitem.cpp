/**
 * @file
 * @copyright 2015 Vix Technology. All rights reserved
 * $Author$
 * $Date$
 * $Rev$
 * @brief Contains implementation of Txn class.
 */

#include "cartitem.h"
#include <cstring>

using namespace std;
using namespace TxnList;

Json::Value CartItem::toJson() const
{
    Json::Value json;

    json["Price"] = getPrice();
    json["type"] = getType();
    json["isreversed"] = isReversed();
    json["id"] = getProductId();
    json["short_desc"] = getShortDescription();
    json["GSTApplicable"] = isGstApplicable();

    return json;
}
