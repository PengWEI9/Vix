/**
 * @file
 * @copyright 2015 Vix Technology. All rights reserved
 * $Author: bbrcan $
 * $Date: 2016-01-04 10:57:17 +1100 (Mon, 04 Jan 2016) $
 * $Rev: 88019 $
 * @brief TxnManager class interface.
 */

#ifndef DC_TXNMANAGER_H
#define DC_TXNMANAGER_H

#include <txnlist/txnbackupmanager.h>
#include <corebasetypes.h>
#include <json/json.h>
#include <map>
#include <vector>

namespace TxnList 
{
    /**
     * @brief Manages transaction data, to be stored for future reversal.
     *
     * Contains functions to add/reverse transactions, and load/save to file.
     */
    class TxnManager 
    {
        private:
            TxnMap m_txnMap;
            TxnBackupManager m_backupManager;

            /*! The number of seconds before a transaction becomes expired. */
            Seconds m_reversalPeriod;

            /**
             * @brief Deletes expired transactions.
             */
            void removeExpiredTransactions();

        public:
            /**
             * @brief Constructor
             *
             * @throw runtime_error If loading reversal period configuration
             *        fails.
             */
            TxnManager();

            /**
             * @brief Returns the transaction for given sale sequence number.
             * @param saleSequenceNumber The transaction's global sales sequence
             * number.
             * @return A transaction pointer, or NULL if no transaction found.
             */
            TransactionPtr getTransaction(TxnSequenceNumber saleSequenceNumber);

            /**
             * @brief Adds a transaction.
             *
             * @param saleSequenceNumber The global sales sequence number of the
             * transaction, used as the key.
             * txnPtr The transaction to add, wrapped in a boost::shared_ptr.
             * @throw runtime_error If transaction with same global sequence 
             *                      number already exists.
             */
            void addTransaction(TxnSequenceNumber saleSequenceNumber, 
                    const TransactionPtr &txnPtr);

            /**
             * @brief Removes a transaction.
             *
             * @param txn A reference to the transaction object to remove. Its
             * address is compared against each Transaction.
             */
            void removeTransaction(const Transaction &txn);

            /**
             * @brief Loads backed-up transaction data from file.
             *
             * This function is exception-safe. If there's an error loading from
             * file, the object state is returned to what it originally was.
             */
            void loadFromFile();

            /**
             * @brief Saves transaction data to file.
             */
            void saveToFile();

            /**
             * @brief Clears the transaction data.
             */
            void clear();

            /**
             * @return The number of stored transactions.
             */
            size_t size() const { return m_txnMap.size(); }

            Json::Value toJson();
    };
}

#endif
