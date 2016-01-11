/**
 * @file
 * @copyright 2015 Vix Technology. All rights reserved
 * $Author$
 * $Date$
 * $Rev$
 * @brief TxnBackupManager class implementation.
 */

#include "txnbackupmanager.h"
#include "cashtransaction.h"
#include "tpursetransaction.h"
#include "txnmanager.h"
#include "helpers.h"
#include "transaction.h"
#include "cartitem.h"
#include <csf.h>
#include <stdexcept>

using namespace std;
using namespace TxnList;

/**
 * @brief Retrieves a value from the loaded ini configuration data.
 * @param key The key to lookup, eg "General:TransactionBackupPath".
 * @throw runtime_error If the key cannot be found.
 * @return Ini value.
 */
static string lookupIniValue(const string &key)
{
    char buffer[CSFINI_MAXVALUE];
    
    if (CsfIniExpand(key.c_str(), buffer, CSFINI_MAXVALUE) != 0)
    {
        throw runtime_error("Unable to retrieve " + key + " from ini files.");
    }

    return string(buffer);
}

TxnBackupManager::TxnBackupManager() :
    m_fileBackupManager(lookupIniValue("General:TransactionBackupPath"), 
            "txn_backup", ".json")
{
}

void TxnBackupManager::save(const TxnMap &txnMap)
{
    // write to file
    m_fileBackupManager.save(toJson(txnMap).toStyledString());
}

void TxnBackupManager::load(TxnManager &txnManager)
{
    const string data = m_fileBackupManager.load();

    // read file into json object
    Json::Value json;
    Json::Reader reader;

    // ensure valid JSON
    if (!data.empty() && !reader.parse(data, json))
    {
        throw runtime_error("Error parsing JSON");
    }

    txnManager.clear();

    // populate txnManager
    for (Json::Value::iterator transactionIter = json.begin(); 
            transactionIter != json.end(); ++transactionIter)
    {
        TransactionPtr txnPtr = transactionFromJson(txnManager, 
                *transactionIter);

        // using txnManager.addTransaction ensures JSON transactions are checked
        txnManager.addTransaction((*transactionIter)["saleseqno"].asInt(),
                txnPtr);
        
        const Json::Value &cartItemsJson = (*transactionIter)["cartitems"];

        for (Json::Value::iterator itemIter = cartItemsJson.begin();
                itemIter != cartItemsJson.end(); ++itemIter)
        {
            CartItemNumber itemNumber = (*itemIter)["itemnumber"].asInt();
            txnPtr->addCartItem(itemNumber, cartItemFromJson(*itemIter));
        }
    }
}
