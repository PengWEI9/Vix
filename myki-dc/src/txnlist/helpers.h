/**
 * @file
 * @copyright 2015 Vix Technology. All rights reserved
 * $Author: bbrcan $
 * $Date: 2015-12-07 14:47:33 +1100 (Mon, 07 Dec 2015) $
 * $Rev: 85568 $
 * @brief Declares helper functions for TxnList sub-module.
 */

#ifndef DC_TXNLIST_HELPER_H
#define DC_TXNLIST_HELPER_H

#include "types.h"
#include <ctime>
#include <json/json.h>
#include <string>

namespace TxnList {

    // forward declarations
    class TxnManager;
    
    /**
     * @brief Converts a JSON object into a CartItem object.
     * @param json JSON object to convert.
     * @return Shared pointer to a CartItem object.
     */
    CartItemPtr cartItemFromJson(const Json::Value &json);

    /**
     * @brief Converts a JSON object into a Transaction object.
     * @param txnManager
     * @param json JSON object to convert.
     * return Shared pointer to a Transaction object.
     */
    TransactionPtr transactionFromJson(TxnManager &txnManager,
            const Json::Value &json);

    /**
     * @brief Converts a transaction map into JSON.
     * @param txnMap The map to convert.
     * @return A JSON object.
     */
    Json::Value toJson(const TxnMap &txnMap);

    /**
     * @brief Erases all elements satisfying predicate pred from the given Map.
     * @param map The map to erase from.
     * @param pred Unary function used to determine whether an element is to be
     * removed.
     */
    template <class Map, class UnaryPredicate>
    void map_erase_if(Map &m, const UnaryPredicate &pred)
    {
        for (typename Map::iterator iter = m.begin();  iter != m.end();)
        {
            if (pred(*iter)) m.erase(iter);
            ++iter;
        }
    }
}

#endif
