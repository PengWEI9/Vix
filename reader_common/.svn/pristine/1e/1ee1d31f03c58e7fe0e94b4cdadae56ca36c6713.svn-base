#ifndef READER_COMMON_FILEBACKUPMANAGERTESTCASE_H
#define READER_COMMON_FILEBACKUPMANAGERTESTCASE_H

#include <TestFramework.h>
#include <string>

class FileBackupManagerTestCase : public TestCase
{
    private:
        std::string m_backupDir;
        std::string m_prefix;
        std::string m_suffix;

       /**
        * @brief Deletes all the files from the txnbackup directory.
        */
       void clearFiles();

    public: 
        FileBackupManagerTestCase() : 
            TestCase("FileBackupManagerTestCase"),
            m_backupDir("backupfiles/"),
            m_prefix("test_backup"),
            m_suffix(".txt") {};

        TestSuite *suite();
        //void setUpSuite(const std::string INFile);
        void setUp(const std::string inFile, const std::string outFile);
        void tearDown(const std::string inFile);

        void testSave();
        void testLoad();
        void testLoadInvalidCrcSingle();
        void testLoadInvalidCrcMultiple();
};

#endif
