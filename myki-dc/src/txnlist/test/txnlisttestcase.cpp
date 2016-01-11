#include "txnlisttestcase.h"
#include <txnlist/txnlist.h>
#include <myki_cd.h>
#include <backupmanager/exceptions.h>
#include <json/json.h>
#include <corebasetypes.h>
#include <boost/shared_ptr.hpp>
#include <csf.h>
#include <iostream>
#include <stdexcept>

using namespace ReaderCommon;
using namespace std;
using namespace TxnList;

#define PRINT_DEBUG(X) cout << #X ": " << X << std::endl;

#define ADD_TEST(TEST) \
    suite->addTest(new TestCaller<TxnListTestCase>(#TEST, \
                ADDR_OF(TxnListTestCase::TEST), this));

static const time_t DEFAULT_TIME = 1441002600; // Mon Aug 31 16:30:00 EST 2015
static time_t CURRENT_TIME = DEFAULT_TIME;
static const U16_t REVERSAL_PERIOD = 5;

static const string ASSET_DIR = "../assets/";
static const string BACKUP_DIR = "txnbackup/";

/**
 * @brief Deletes all the files from the txnbackup directory.
 */
static void clearFiles()
{
    CsGlob_t glob;

    if (CsGlob(BACKUP_DIR.c_str(), &glob, "*.json") != 0)
    {
        return;
    }

    for (int i = 0; i < glob.gl_argc; ++i)
    {
        string filePath = BACKUP_DIR + string(glob.gl_argv[i]);
        remove(filePath.c_str());
    }

    CsGlobFree(&glob);
}

void TxnListTestCase::copyFiles(const string &dirName)
{
    CsGlob_t glob;

    const string dirPath = ASSET_DIR + dirName;

    Assert(CsGlob(dirPath.c_str(), &glob, "*.json") == 0);

    for (int i = 0; i < glob.gl_argc; ++i)
    {
        string src = dirPath + "/" + glob.gl_argv[i];
        string dest = BACKUP_DIR + glob.gl_argv[i];

        CsFileCopy(src.c_str(), dest.c_str());
    }

    CsGlobFree(&glob);
}

/**
 * Override C's time() function, so it always returns a fixed time. This allows
 * us to easily test the reversal periods.
 */
time_t time(time_t *timer)
{
    return CURRENT_TIME;
}

int MYKI_CD_getReversalPeriod(U16_t *value)
{
    *value = REVERSAL_PERIOD;
    return true;
}

TestSuite *TxnListTestCase::suite()
{
    TestSuite *suite = new TestSuite(name());

    ADD_TEST(testAddTopupCartItem);
    ADD_TEST(testAddProductCartItem);
    ADD_TEST(testAddTransaction);
    ADD_TEST(testAddTransactionSameSequenceNumber);
    ADD_TEST(testClear);
    ADD_TEST(testReverseCartItems);
    ADD_TEST(testReverseAllCartItems);
    ADD_TEST(testGetTransactionCurrentTime);
    ADD_TEST(testGetTransactionInvalidTime);
    ADD_TEST(testGetTransactionValidTime);
    ADD_TEST(testLoad);
    ADD_TEST(testLoadInvalidCrc);
    ADD_TEST(testLoadInvalidJson);
    ADD_TEST(testSave);
    ADD_TEST(testTransactionsToJson);

    return suite;
}

void TxnListTestCase::setUp(const string inFile, const string outFile)
{
    clearFiles();

    // Reset time to default
    CURRENT_TIME = DEFAULT_TIME; 
    CsMkdir("txnbackup", 0755);
}

void TxnListTestCase::testAddTopupCartItem()
{
    TxnManager txnManager;

    Transaction txn(txnManager, CURRENT_TIME, "cash");
    txn.addCartItem(1, CartItemPtr(new TopupCartItem("tpurseload", 3, 300, "TPurse Load", false, 4, "123456789")));
    txn.addCartItem(2, CartItemPtr(new TopupCartItem("tpurseload", 3, 300, "TPurse Load", false, 4, "12345678"))); 
    txn.addCartItem(3, CartItemPtr(new TopupCartItem("tpurseload", 3, 300, "TPurse Load", false, 4, "1234567")));

    Json::Value json = txn.toJson();

    Assert(json["cartitems"].size() == 3);
    Assert(txn.getTimestamp() == CURRENT_TIME);
    Assert(txn.getCartItem(1)->getType() == "tpurseload");
    Assert(txn.getCartItem(2)->getType() == "tpurseload");
    Assert(txn.getCartItem(3)->getType() == "tpurseload");
}

void TxnListTestCase::testAddProductCartItem()
{
    TxnManager txnManager;

    Transaction txn(txnManager, CURRENT_TIME, "cash");
    txn.addCartItem(1, CartItemPtr(new CartItem("ThirdParty", 
                        50, 46, "GTS Timetable short", true)));

    Json::Value json = txn.toJson();

    Assert(json["cartitems"].size() == 1);
    Assert(txn.getCartItem(1)->getType() == "ThirdParty");

    //ProductCartItem &productCartItem = dynamic_cast<ProductCartItem&>(*txn.getCartItem(1));
    CartItemPtr cartItemPtr = txn.getCartItem(1);

    Assert(cartItemPtr->getProductId() == 46);
    Assert(cartItemPtr->getShortDescription() == "GTS Timetable short");
}

void TxnListTestCase::testAddTransaction()
{
    TxnManager txnManager;

    TransactionPtr txnPtr(new CashTransaction(txnManager, CURRENT_TIME));
    txnManager.addTransaction(1, txnPtr);
    txnPtr->addCartItem(1, CartItemPtr(new TopupCartItem("tpurseload", 3, 300, "TPurse Load", false, 4, "123456789")));
    txnPtr->addCartItem(2, CartItemPtr(new TopupCartItem("tpurseload", 3, 300, "TPurse Load", false, 4, "12345678"))); 
    txnPtr->addCartItem(3, CartItemPtr(new TopupCartItem("tpurseload", 3, 300, "TPurse Load", false, 4, "1234567")));

    Json::Value json = txnManager.toJson();

    Assert(json.size() == 1);
    Assert(json[0u]["cartitems"].size() == 3);
    Assert(txnManager.getTransaction(1) != NULL);
    Assert(txnManager.getTransaction(1)->getTimestamp() == CURRENT_TIME);
}

void TxnListTestCase::testAddTransactionSameSequenceNumber()
{
    TxnManager txnManager;

    try {
        txnManager.addTransaction(1, TransactionPtr(new CashTransaction(txnManager, CURRENT_TIME)));
        txnManager.addTransaction(1, TransactionPtr(new CashTransaction(txnManager, CURRENT_TIME)));
        Assert(false && "Exception expected but not thrown!");
    }
    catch (runtime_error &e) 
    {
        cout << "Exception expected: " << e.what() << endl;
    }
}

void TxnListTestCase::testClear()
{   
    TxnManager txnManager;

    txnManager.addTransaction(1, TransactionPtr(new CashTransaction(txnManager, CURRENT_TIME)));
    txnManager.addTransaction(2, TransactionPtr(new CashTransaction(txnManager, CURRENT_TIME)));

    Assert(txnManager.size() == 2);
    txnManager.clear();
    Assert(txnManager.size() == 0);
}

void TxnListTestCase::testReverseCartItems()
{
    TxnManager txnManager;

    TransactionPtr txnPtr(new CashTransaction(txnManager, CURRENT_TIME));
    txnManager.addTransaction(1, txnPtr);

    Assert(txnManager.size() == 1);
    txnPtr->addCartItem(1, CartItemPtr(new TopupCartItem("tpurseload", 3, 300, "TPurse Load", false, 4, "123456789")));
    txnPtr->addCartItem(2, CartItemPtr(new TopupCartItem("tpurseload", 3, 300, "TPurse Load", false, 4, "12345678"))); 
    txnPtr->addCartItem(3, CartItemPtr(new TopupCartItem("tpurseload", 3, 300, "TPurse Load", false, 4, "1234567")));

    Assert(txnPtr->size() == 3);
    Assert(txnPtr->areAllItemsReversed() == false);

    // reversals should succeed
    txnPtr->reverseCartItem(1);
    txnPtr->reverseCartItem(2);

    // transaction should NOT have been removed from TxnManager
    Assert(txnPtr->areAllItemsReversed() == false);
    Assert(txnManager.size() == 1);
}

void TxnListTestCase::testReverseAllCartItems()
{
    TxnManager txnManager;

    TransactionPtr txnPtr(new CashTransaction(txnManager, CURRENT_TIME));
    txnManager.addTransaction(1, txnPtr);

    Assert(txnManager.size() == 1);
    txnPtr->addCartItem(1, CartItemPtr(new TopupCartItem("tpurseload", 3, 300, "TPurse Load", false, 4, "123456789")));
    txnPtr->addCartItem(2, CartItemPtr(new TopupCartItem("tpurseload", 3, 300, "TPurse Load", false, 4, "12345678"))); 
    txnPtr->addCartItem(3, CartItemPtr(new TopupCartItem("tpurseload", 3, 300, "TPurse Load", false, 4, "1234567")));

    Assert(txnPtr->size() == 3);
    Assert(txnPtr->areAllItemsReversed() == false);

    // reversals should succeed
    txnPtr->reverseCartItem(1);
    txnPtr->reverseCartItem(2);
    txnPtr->reverseCartItem(3);

    // transaction should have been removed from TxnManager
    Assert(txnPtr->areAllItemsReversed() == true);
    Assert(txnManager.size() == 0);
}

void TxnListTestCase::testGetTransactionCurrentTime()
{
    TxnManager txnManager;

    TransactionPtr txnPtr(new CashTransaction(txnManager, CURRENT_TIME));
    txnManager.addTransaction(1, txnPtr);
    txnPtr->addCartItem(1, CartItemPtr(new TopupCartItem("tpurseload", 3, 300, "TPurse Load", false, 4, "123456789")));

    // validate list contains txn
    Assert(txnManager.size() == 1);

    // transaction should exist
    Assert(txnManager.getTransaction(1) == txnPtr);
}

void TxnListTestCase::testGetTransactionInvalidTime()
{
    TxnManager txnManager;

    TransactionPtr txnPtr(new CashTransaction(txnManager, CURRENT_TIME));
    txnManager.addTransaction(1, txnPtr);

    txnPtr->addCartItem(1, CartItemPtr(new TopupCartItem("tpurseload", 3, 300, "TPurse Load", false, 4, "123456789")));

    // validate list contains txn
    Assert(txnManager.size() == 1);

    // push current time past valid reversal period
    CURRENT_TIME += ((REVERSAL_PERIOD*60) + 100);

    // transaction expired, therefore should be deleted
    Assert(txnManager.getTransaction(1) == NULL);
    Assert(txnManager.size() == 0);
}

void TxnListTestCase::testGetTransactionValidTime()
{
    TxnManager txnManager;

    TransactionPtr txnPtr(new CashTransaction(txnManager, CURRENT_TIME));
    txnManager.addTransaction(1, txnPtr);

    txnPtr->addCartItem(1, CartItemPtr(new TopupCartItem("tpurseload", 3, 300, "TPurse Load", false, 4, "123456789")));

    // validate list contains txn
    Assert(txnManager.size() == 1);

    // push current time WITHIN valid reversal period
    CURRENT_TIME += ((REVERSAL_PERIOD*60) - 1);

    // transaction should exist
    Assert(txnManager.getTransaction(1) == txnPtr);
}

void TxnListTestCase::testLoad()
{
    copyFiles("txnbackup_ok");

    TxnManager txnManager;
    txnManager.loadFromFile();

    PRINT_DEBUG(txnManager.toJson());
    Assert(txnManager.size() == 2);
    TransactionPtr txnPtr = txnManager.getTransaction(1);
    Assert(txnPtr != NULL);
    Assert(txnPtr->getTimestamp() == 1441002600);
    PRINT_DEBUG(txnPtr->getPaymentType());
    Assert(txnPtr->getPaymentType() == "cash");
    Assert(txnPtr->getCartItem(1)->getType() == "LLSC");
    Assert(txnPtr->getCartItem(2)->getType() == "LLSC");

    txnPtr = txnManager.getTransaction(2);
    Assert(txnPtr->getTimestamp() == 1441002601);
    Assert(txnPtr->getCartItem(1)->getType() == "ThirdParty");
}

void TxnListTestCase::testLoadInvalidCrc()
{
    copyFiles("txnbackup_invalidcrc");

    TxnManager txnManager;
    txnManager.loadFromFile();

    Assert(txnManager.size() == 1);
    txnManager.toJson();
    TransactionPtr txnPtr = txnManager.getTransaction(1);

    Assert(txnPtr != NULL);
    Assert(txnPtr->getTimestamp() == 1441002400);
    Assert(txnPtr->getPaymentType() == "cash");
    Assert(txnPtr->getCartItem(1)->getType() == "ThirdParty");
    Assert(txnPtr->getCartItem(2)->getType() == "LLSC");
}

void TxnListTestCase::testLoadInvalidJson()
{
    copyFiles("txnbackup_invalidjson");

    TxnManager txnManager;

    try {
        txnManager.loadFromFile();
        Assert(false && "Exception expected but not thrown!");
    }
    catch (runtime_error &e) {}
}

void TxnListTestCase::testSave()
{
    TxnManager txnManager;

    TransactionPtr txnPtr(new CashTransaction(txnManager, CURRENT_TIME));
    txnManager.addTransaction(1, txnPtr);
    txnPtr->addCartItem(1, CartItemPtr(new TopupCartItem("tpurseload", 3, 300, "TPurse Load", false, 4, "123456789")));
    txnPtr->addCartItem(2, CartItemPtr(new TopupCartItem("tpurseload", 3, 300, "TPurse Load", false, 4, "12345678"))); 
    txnPtr->addCartItem(3, CartItemPtr(new TopupCartItem("tpurseload", 3, 300, "TPurse Load", false, 4, "1234567")));

    CURRENT_TIME += 1;

    txnPtr.reset(new CashTransaction(txnManager, CURRENT_TIME));
    txnManager.addTransaction(2, txnPtr);
    txnPtr->addCartItem(1, CartItemPtr(new CartItem("ThirdParty", 50, 46, "GTS Timetable short", true)));

    txnManager.saveToFile();

    // make sure our transactions are definitely there!
    Assert(txnManager.size() == 2);

    // constructor automatically loads from file
    TxnManager newTxnManager;
    newTxnManager.loadFromFile();

    Assert(newTxnManager.size() == 2);

    TransactionPtr newTxnPtr = newTxnManager.getTransaction(1);
    Assert(newTxnPtr->getTimestamp() == CURRENT_TIME - 1);
    Assert(newTxnPtr->getPaymentType() == "cash");
    Assert(newTxnPtr->getCartItem(1)->getType() == "tpurseload");
    Assert(newTxnPtr->getCartItem(2)->getType() == "tpurseload");
    Assert(newTxnPtr->getCartItem(3)->getType() == "tpurseload");

    newTxnPtr = newTxnManager.getTransaction(2);
    Assert(newTxnPtr->getTimestamp() == CURRENT_TIME);
    Assert(newTxnPtr->getPaymentType() == "cash");
    Assert(newTxnPtr->getCartItem(1)->getType() == "ThirdParty");
}

void TxnListTestCase::testTransactionsToJson()
{
    TxnManager txnManager;

    TransactionPtr txnPtr(new CashTransaction(txnManager, CURRENT_TIME));
    txnManager.addTransaction(1, txnPtr);
    txnPtr->addCartItem(1, CartItemPtr(new TopupCartItem("tpurseload", 3, 300, "TPurse Load", false, 4, "123456789")));
    txnPtr->addCartItem(2, CartItemPtr(new TopupCartItem("tpurseload", 3, 300, "TPurse Load", false, 4, "12345678"))); 
    txnPtr->addCartItem(3, CartItemPtr(new TopupCartItem("tpurseload", 3, 300, "TPurse Load", false, 4, "1234567")));

    CURRENT_TIME += 1;

    txnPtr.reset(new CashTransaction(txnManager, CURRENT_TIME));
    txnManager.addTransaction(2, txnPtr);

    txnPtr->addCartItem(1, CartItemPtr(new CartItem("ThirdParty", 50, 46, "GTS Timetable short", true)));

    Json::Value json = txnManager.toJson();

    Assert(json[0u]["saleseqno"] == 1);
    Assert(json[0u]["saletimestamp"] == static_cast<int>(CURRENT_TIME - 1));
    Assert(json[0u]["paymenttype"] == "cash"); 

    Assert(json[0u]["cartitems"][0u]["txntype"] == "tpurseload");
    Assert(json[0u]["cartitems"][1u]["txntype"] == "tpurseload");
    Assert(json[0u]["cartitems"][2u]["txntype"] == "tpurseload");

    Assert(json[1u]["saleseqno"] == 2);
    Assert(json[1u]["saletimestamp"] == static_cast<int>(CURRENT_TIME));
    Assert(json[1u]["paymenttype"] == "cash"); 
    Assert(json[1u]["cartitems"][0u]["type"] == "ThirdParty");
    Assert(json[1u]["cartitems"][0u]["GSTApplicable"] == true);
    Assert(json[1u]["cartitems"][0u]["id"] == 46);
    Assert(json[1u]["cartitems"][0u]["itemnumber"] == 1);
    Assert(json[1u]["cartitems"][0u]["isreversed"] == false);
    Assert(json[1u]["cartitems"][0u]["short_desc"] == "GTS Timetable short");
}

#undef PRINT_DEBUG
#undef ADD_TEST
