#ifndef DC_TXNLISTTESTCASE_H
#define DC_TXNLISTTESTCASE_H

#include <TestFramework.h>
#include <string>

class TxnListTestCase : public TestCase
{
    public:
        TxnListTestCase() : TestCase("TxnListTestCase") {};
        virtual ~TxnListTestCase() {};

        TestSuite *suite();

        void copyFiles(const std::string &dirName);

        void setUp(const std::string inFile, const std::string outFile);

        /**
         * @brief Tests adding Topup cart items.
         */
        void testAddTopupCartItem();

        /**
         * @brief Tests adding Product cart items.
         */
        void testAddProductCartItem();

        /**
         * @brief Tests adding transactions.
         */
        void testAddTransaction();

        /**
         * @brief Tests adding transactions with the same transaction sequence
         * number.
         */
        void testAddTransactionSameSequenceNumber();

        /**
         * @brief Tests clearing the transaction list.
         */
        void testClear();

        /**
         * @brief Tests reversing all cart items from a transaction.
         */
        void testReverseCartItems();


        /**
         * @brief Tests reversing all cart items from a transaction.
         */
        void testReverseAllCartItems();

        /**
         * @brief Tests retrieving a transaction, where the transaction time is
         * equal to the current time.
         */
        void testGetTransactionCurrentTime();

        /**
         * @brief Tests retrieving a transaction, where the transaction time +
         * reversal period is less than the current time.
         */
        void testGetTransactionInvalidTime();

        /**
         * @brief Tests retrieving a transaction, where the transaction time +
         * reversal period is greater than the current time.
         */
        void testGetTransactionValidTime();

        /**
         * @brief Tests loading transaction data from backup JSON file.
         */
        void testLoad();

        /**
         * @brief Tests loading transaction data from backup JSON file, where
         *        the newest file has an invalid CRC.
         */
        void testLoadInvalidCrc();

        /**
         * @brief Tests loading transaction data from a backup JSON file which,
         *        while having a valid CRC, is invalid.
         */
        void testLoadInvalidJson();

        /**
         * @brief Tests saving transaction data to a backup JSON file.
         */
        void testSave();

        /**
         * @brief Tests retrieving a list of transactions in JSON format.
         */
        void testTransactionsToJson();
};

#endif

