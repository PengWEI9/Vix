#ifndef READER_COMMON_BBRAMBACKUPMANAGERTESTCASE_H
#define READER_COMMON_BBRAMBACKUPMANAGERTESTCASE_H

#include <TestFramework.h>
#include <string>

class BBRamBackupManagerTestCase : public TestCase
{
    private:
        std::string m_partitionName;
    
    public: 
        BBRamBackupManagerTestCase() : 
            TestCase("BBRamBackupManagerTestCase"),
            m_partitionName("test_partition") {};

        TestSuite *suite();
        //void setUpSuite(const std::string INBBRam);
        void setUp(const std::string inFile, const std::string outFile) {};
        void tearDown(const std::string inFile) {};

        void testSave();
        void testLoad();

        void testInvalidPartitionName();
        void testSaveInsufficientMemory();
};

#endif
