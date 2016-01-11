#ifndef DC_PRINTERBACKUPTESTCASE_H
#define DC_PRINTERBACKUPTESTCASE_H

#include <TestFramework.h>
#include <string>

class PrinterBackupTestCase : public TestCase
{
    private:
        int m_printerTaskId;

    public:
        PrinterBackupTestCase();
        ~PrinterBackupTestCase();

        TestSuite *suite();

        void copyFiles(const std::string &dirName);

        void setUp(const std::string &inFile, const std::string &outFile);

        void testLoadTPurseLoadMsg();
        void testLoadTPurseReversalMsg();
        void testLoadDriverShiftReportMsg();
        void testLoadProductSalesMsg();

        void testLoadEmpty();

        void testSaveTPurseLoadMsg();
        void testSaveTPurseReversalMsg();
        void testSaveDriverShiftReportMsg();
        void testSaveProductSalesMsg();
};

#endif

