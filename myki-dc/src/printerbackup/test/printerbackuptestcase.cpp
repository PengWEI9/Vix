#include "printerbackuptestcase.h"
#include <printerbackup/printerbackupmanager.h>
#include <corebasetypes.h>
#include <csf.h>
#include <ipc.h>
#include <json/json.h>
#include <cstdlib>
#include <iostream>
#include <cstring>
#include <fstream>
#include <stdexcept>
#include <boost/bind.hpp>
#include <boost/lexical_cast.hpp>

using namespace boost;
using namespace std;
using namespace PrinterBackup;

#define PRINT_DEBUG(X) std::cout << #X ": " << X << std::endl;

#define ADD_TEST(TEST) \
    suite->addTest(new TestCaller<PrinterBackupTestCase>(#TEST, \
                ADDR_OF(PrinterBackupTestCase::TEST), this));

static const string BACKUP_DIR = "printerbackups/";

template <typename T>
int receiveIpcMessage(int printerTaskId, T &msg)
{
    char buf[BUFSIZ] = {0};
    size_t bytesRead = 0;

    int ret = IpcReceive(printerTaskId, &buf, sizeof(msg), &bytesRead);
    memcpy(&msg, buf, sizeof(msg));
    return ret;
}

/**
 * @return A list of all the files in the test output directory.
 */
static vector<string> testOutputFiles()
{
    CsGlob_t glob = {};
    vector<string> vec;

    if (CsGlob(BACKUP_DIR.c_str(), &glob, "*.dat") == 0)
    {
        for (int i = 0; i < glob.gl_argc; ++i)
        {
            vec.push_back(BACKUP_DIR + string(glob.gl_argv[i]));
        }
    }

    CsGlobFree(&glob);
    return vec;
}

/**
 * @brief Deletes all the files from the printerbackups directory.
 */
static void clearFiles()
{
    vector<string> vec = testOutputFiles();

    for_each(vec.begin(), vec.end(), bind(::remove, bind(&string::c_str, _1)));
}

/**
 * @brief Copies a sample file from the assets directory to the printerbackups/
 *        directory.
 * @param fileName The name of the file to copy - NOT the path.
 */
static void copySample(const string &fileName)
{
    CsMkdir(BACKUP_DIR.c_str(), 0755);

    string source = "../assets/" + fileName;
    string dest = BACKUP_DIR + fileName;
    CsFileCopy(source.c_str(), dest.c_str());
}

static IPC_Printer_TpurseLoad_t sampleTPurseLoadMsg()
{
    IPC_Printer_TpurseLoad_t printerMsg = {};

    printerMsg.hdr.type = IPC_PRINTER_TPURSELOAD;
    printerMsg.hdr.source = DC_PRINTER_TASK;

    IPC_Company_Details_t &companyDetails = printerMsg.companyData;

    strncpy(companyDetails.companyPhone, "123456", IPC_STRING_MAX_LENGTH);
	strncpy(companyDetails.companyName, "CompanyName", IPC_STRING_MAX_LENGTH);
	strncpy(companyDetails.companyABN, "123456", IPC_STRING_MAX_LENGTH);
	strncpy(companyDetails.companyWebSite, "www.company.com", IPC_STRING_MAX_LENGTH);

    IPC_Common_Details_t &commonDetails = printerMsg.commonData;

	strncpy(commonDetails.busID, "8008", IPC_STRING_MAX_LENGTH);
    commonDetails.terminalID = 99999;
    commonDetails.shiftNumber = 101;
    commonDetails.transactionNumber = 5;

	strncpy(commonDetails.mykiReference, "123456", IPC_STRING_MAX_LENGTH);
    strncpy(commonDetails.location, "Bundoora", IPC_STRING_MAX_LENGTH);
    strncpy(commonDetails.salesMessage, "Good on ya!", IPC_STRING_MAX_LENGTH);

    printerMsg.receiptInvoiceThreshold = 1000;
    printerMsg.amountTopUp = 20;
    printerMsg.amountNewBalance = 100;
    printerMsg.amountTotal = 20;

    return printerMsg;
}

static IPC_Reversal_TpurseLoad_t sampleTPurseReversalMsg()
{
    IPC_Reversal_TpurseLoad_t printerMsg = {};

    printerMsg.hdr.type = IPC_PRINTER_TPURSELOAD_REV;
    printerMsg.hdr.source = DC_PRINTER_TASK;

    IPC_Company_Details_t &companyDetails = printerMsg.companyData;

    strncpy(companyDetails.companyPhone, "123456", IPC_STRING_MAX_LENGTH);
	strncpy(companyDetails.companyName, "CompanyName", IPC_STRING_MAX_LENGTH);
	strncpy(companyDetails.companyABN, "123456", IPC_STRING_MAX_LENGTH);
	strncpy(companyDetails.companyWebSite, "www.company.com", IPC_STRING_MAX_LENGTH);

    IPC_Common_Details_t &commonDetails = printerMsg.commonData;

	strncpy(commonDetails.busID, "8008", IPC_STRING_MAX_LENGTH);
    commonDetails.terminalID = 99999;
    commonDetails.shiftNumber = 101;
    commonDetails.transactionNumber = 5;

    printerMsg.amountTopUp = 20;
    printerMsg.amountNewBalance = 100;
    printerMsg.amountTotal = 20;
    printerMsg.origTransactionNumber = 1;
    printerMsg.origDate = 1444963726;

    return printerMsg;
}

static IPC_DriverShiftReport_t sampleDriverShiftReportMsg()
{
    IPC_DriverShiftReport_t printerMsg = {};

    printerMsg.hdr.type = IPC_PRINTER_DRIVER_SHIFT_REP;
    printerMsg.hdr.source = DC_PRINTER_TASK;

    printerMsg.reportType = REPORT_END_OF_SHIFT;

    strncpy(printerMsg.shiftDetails.operatorID, "abc", IPC_STRING_MAX_LENGTH);
    printerMsg.shiftDetails.shiftNumber = 51;
    strncpy(printerMsg.shiftDetails.busID, "12345", IPC_STRING_MAX_LENGTH);
    printerMsg.shiftDetails.shiftStartDate = 1444963026;
    printerMsg.shiftDetails.shiftEndDate = 1444963726;

    IPC_ShiftCashStatus_t &shiftCash = printerMsg.shiftDetails.shiftCash;
    
    shiftCash.cashStatus = CASH_BROUGHT_FORWARD;
    shiftCash.cashAmount = 321;

    printerMsg.cashTotals.grossAmount = 123;
    printerMsg.cashTotals.annulledAmount = 321;
    printerMsg.cashTotals.netAmount = 123;
    printerMsg.otherTotals.grossAmount = 321;
    printerMsg.otherTotals.annulledAmount = 123;
    printerMsg.otherTotals.netAmount = 321;
    printerMsg.paperTickets.grossTickets = 123;
    printerMsg.paperTickets.annulledTickets = 321;
    printerMsg.paperTickets.netTickets = 123;
    printerMsg.sundryTickets.grossTickets = 321;
    printerMsg.sundryTickets.annulledTickets = 321;
    printerMsg.sundryTickets.netTickets = 123;
    printerMsg.fullPassCount = 321;
    printerMsg.concessionPassCount = 123;
    printerMsg.annulledTickets = 321;
    printerMsg.netCash = 123;

    return printerMsg;
}

static std::string sampleJson()
{
    Json::Value sample;

    sample["item1"] = 1;
    sample["item2"] = 2;
    sample["sub"]["item1"] = 3;
    sample["sub"]["item2"] = 4;

    return boost::lexical_cast<string>(sample);
}

PrinterBackupTestCase::PrinterBackupTestCase() : 
    TestCase("PrinterBackupTestCase") 
{
    if (IpcInit() != 0)
    {
        throw runtime_error("IpcInit() failed: try running as root.");
    }

    m_printerTaskId = IpcGetID(DC_PRINTER_TASK);
}

PrinterBackupTestCase::~PrinterBackupTestCase()
{
    IpcClose();
}

TestSuite *PrinterBackupTestCase::suite()
{
    TestSuite *suite = new TestSuite(name());

    ADD_TEST(testLoadTPurseLoadMsg);
    ADD_TEST(testLoadTPurseReversalMsg);
    ADD_TEST(testLoadDriverShiftReportMsg);
    ADD_TEST(testLoadProductSalesMsg);
    ADD_TEST(testLoadEmpty);

    ADD_TEST(testSaveTPurseLoadMsg);
    ADD_TEST(testSaveTPurseReversalMsg);
    ADD_TEST(testSaveDriverShiftReportMsg);
    ADD_TEST(testSaveProductSalesMsg);

    return suite;
}

void PrinterBackupTestCase::setUp(const string &inFile, const string &outFile)
{
    IpcFlush(m_printerTaskId);
    clearFiles();
}

void PrinterBackupTestCase::testLoadTPurseLoadMsg()
{
    copySample("printermsg_20151016091821_1802924344.dat");

    PRINT_DEBUG(m_printerTaskId);

    PrinterBackupManager backupManager(m_printerTaskId);
    backupManager.reprint();

    // ensure message is in queue
    Assert(IpcReceiveQueueSize(m_printerTaskId) == 1);

    IPC_Printer_TpurseLoad_t msg = {};
    Assert(receiveIpcMessage(m_printerTaskId, msg) == 0);

    IPC_Printer_TpurseLoad_t sample = sampleTPurseLoadMsg();
    Assert(memcmp(&msg, &sample, sizeof(msg)) == 0);
}

void PrinterBackupTestCase::testLoadTPurseReversalMsg()
{
    copySample("printermsg_20151016091821_2070948708.dat");

    PrinterBackupManager backupManager(m_printerTaskId);
    backupManager.reprint();

    // ensure message is in queue
    Assert(IpcReceiveQueueSize(m_printerTaskId) == 1);

    IPC_Reversal_TpurseLoad_t msg = {};
    Assert(receiveIpcMessage(m_printerTaskId, msg) == 0);

    IPC_Reversal_TpurseLoad_t sample = sampleTPurseReversalMsg();
    Assert(memcmp(&msg, &sample, sizeof(msg)) == 0);
}

void PrinterBackupTestCase::testLoadDriverShiftReportMsg()
{
    copySample("printermsg_20151016091821_1834340016.dat");

    PrinterBackupManager backupManager(m_printerTaskId);
    backupManager.reprint();

    // ensure message is in queue
    Assert(IpcReceiveQueueSize(m_printerTaskId) == 1);

    IPC_DriverShiftReport_t msg = {};
    Assert(receiveIpcMessage(m_printerTaskId, msg) == 0);

    IPC_DriverShiftReport_t sample = sampleDriverShiftReportMsg();
    Assert(memcmp(&msg, &sample, sizeof(msg)) == 0);
}

void PrinterBackupTestCase::testLoadProductSalesMsg()
{
    copySample("printermsg_20151016091821_2422974048.dat");

    PrinterBackupManager backupManager(m_printerTaskId);
    backupManager.reprint();

    // ensure message is in queue
    Assert(IpcReceiveQueueSize(m_printerTaskId) == 1);

    string rawData;
    char buf[BUFSIZ] = {0};
    IPC_PayloadMulti_t *tmp = NULL;

    do {
        size_t bytesRead = 0;

        // clear buf, just in case
        memset(buf, 0, BUFSIZ);

        // receive message
        Assert(IpcReceive(m_printerTaskId, &buf, BUFSIZ, &bytesRead) == 0);
        Assert(bytesRead > sizeof(IPC_PayloadMulti_t));
        tmp = reinterpret_cast<IPC_PayloadMulti_t*>(buf);

        // concatenate message data together
        rawData += tmp->data;
    }
    while (tmp->blockSequenceNumber < tmp->numberOfBlocks);

    PRINT_DEBUG(rawData);
    PRINT_DEBUG(sampleJson());

    Assert(rawData == sampleJson());
}

void PrinterBackupTestCase::testLoadEmpty()
{
    PrinterBackupManager backupManager(m_printerTaskId);
    backupManager.reprint();

    // ensure no message was sent
    Assert(IpcReceiveQueueSize(m_printerTaskId) == 0);
}

void PrinterBackupTestCase::testSaveTPurseLoadMsg()
{
    IPC_Printer_TpurseLoad_t sampleMsg = sampleTPurseLoadMsg();

    PrinterBackupManager backupManager(m_printerTaskId);
    backupManager.save(sampleMsg, 1444947501);

    // Manually load the file we just saved, and compare its contents

    ifstream stream(testOutputFiles()[0].c_str());
    Assert(stream.good());

    int msgType = 0;
    IPC_Printer_TpurseLoad_t msg = {};

    stream.read(reinterpret_cast<char*>(&msgType), sizeof(msgType));
    stream.read(reinterpret_cast<char*>(&msg), sizeof(msg));

    Assert(msgType == IPC_PRINTER_TPURSELOAD);
    Assert(memcmp(&msg, &sampleMsg, sizeof(msg)) == 0);
}

void PrinterBackupTestCase::testSaveTPurseReversalMsg()
{
    // Save message

    IPC_Reversal_TpurseLoad_t sampleMsg = sampleTPurseReversalMsg();

    PrinterBackupManager backupManager(m_printerTaskId);
    backupManager.save(sampleMsg, 1444947501);

    // Manually load the file we just saved, and compare its contents
    ifstream stream(testOutputFiles()[0].c_str());
    Assert(stream.good());

    int msgType = 0;
    IPC_Reversal_TpurseLoad_t msg = {};

    stream.read(reinterpret_cast<char*>(&msgType), sizeof(msgType));
    stream.read(reinterpret_cast<char*>(&msg), sizeof(msg));

    Assert(msgType == IPC_PRINTER_TPURSELOAD_REV);
    Assert(memcmp(&msg, &sampleMsg, sizeof(msg)) == 0);
}

void PrinterBackupTestCase::testSaveDriverShiftReportMsg()
{
    // Save message

    IPC_DriverShiftReport_t sampleMsg = sampleDriverShiftReportMsg();

    PrinterBackupManager backupManager(m_printerTaskId);
    backupManager.save(sampleMsg, 1444947501);

    // Manually load the file we just saved, and compare its contents

    ifstream stream(testOutputFiles()[0].c_str());
    Assert(stream.good());

    int msgType = 0;
    IPC_DriverShiftReport_t msg = {};

    stream.read(reinterpret_cast<char*>(&msgType), sizeof(msgType));
    stream.read(reinterpret_cast<char*>(&msg), sizeof(msg));

    Assert(msgType == IPC_PRINTER_DRIVER_SHIFT_REP);
    Assert(memcmp(&msg, &sampleMsg, sizeof(msg)) == 0);
}

void PrinterBackupTestCase::testSaveProductSalesMsg()
{
    PrinterBackupManager backupManager(m_printerTaskId);
    backupManager.save(sampleJson(), 1444947501);
    
    // Manually load the file we just saved, and compare its contents

    ifstream stream(testOutputFiles()[0].c_str());
    Assert(stream.good());

    Json::Reader reader;
    Json::Value json;
    reader.parse(stream, json);

    Assert(json["item1"].asInt() == 1);
    Assert(json["item2"].asInt() == 2);
    Assert(json["sub"]["item1"].asInt() == 3);
    Assert(json["sub"]["item2"].asInt() == 4);
}

#undef PRINT_DEBUG
#undef ADD_TEST
