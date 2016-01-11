#include "filebackupmanagertestcase.h"
#include <backupmanager/filebackupmanager.h>
#include <csf.h>
#include <fstream>

using namespace std;
using namespace ReaderCommon;

#define ADD_TEST(TEST) \
    suite->addTest(new TestCaller<FileBackupManagerTestCase>(#TEST, \
                ADDR_OF(FileBackupManagerTestCase::TEST), this));

static bool isFileValid(const string &path, const string &expected)
{
    ifstream stream(path.c_str());

    if (stream.fail())
    {
        cout << "isFileValid:: Stream failed!" << endl;
        return false;
    }

    string actual((istreambuf_iterator<char>(stream)), 
            istreambuf_iterator<char>());

    cout << "isFileValid actual (" << path << "):" << endl;
    cout << actual << endl;
    cout << "isFileValid expected:" << endl;
    cout << expected << endl;

    return (actual == expected);
}

TestSuite *FileBackupManagerTestCase::suite()
{
    TestSuite *suite = new TestSuite(name());

    ADD_TEST(testSave);
    ADD_TEST(testLoad);
    ADD_TEST(testLoadInvalidCrcSingle);
    ADD_TEST(testLoadInvalidCrcMultiple);

    return suite;
}

void FileBackupManagerTestCase::clearFiles()
{
    CsGlob_t glob;
    string globStr = "*" + m_suffix;

    if (CsGlob(m_backupDir.c_str(), &glob, globStr.c_str()) != 0)
    {
        return;
    }

    for (int i = 0; i < glob.gl_argc; ++i)
    {
        string filePath = m_backupDir + string(glob.gl_argv[i]);
        remove(filePath.c_str());
    }

    CsGlobFree(&glob);
}

void FileBackupManagerTestCase::setUp(const string inFile, const string outFile)
{
}

void FileBackupManagerTestCase::tearDown(const std::string inFile)
{
    clearFiles();
}

void FileBackupManagerTestCase::testSave()
{
    cout << "testSave begin" << endl;

    const string testString = "Hello, this is a test.";
    FileBackupManager backupManager(m_backupDir, m_prefix, m_suffix);
    backupManager.save(testString, 1441002600);
    Assert(isFileValid("backupfiles/test_backup_20150831143000_2571328179.txt", 
                testString));

    cout << "testSave end" << endl;
}

void FileBackupManagerTestCase::testLoad()
{
    cout << "testLoad begin" << endl;
    FileBackupManager backupManager("../assets/sample_ok", m_prefix, m_suffix);
    string data;

    try {
        data = backupManager.load();
    }
    catch (exception &e)
    {
        cout << "Exception caught: " << e.what() << endl;
        throw;
    }

    Assert(data == "Hello, this is a test2.");
    cout << "testLoad end" << endl;
}

void FileBackupManagerTestCase::testLoadInvalidCrcSingle()
{
    cout << "testLoadInvalidCrcSingle begin" << endl;

    FileBackupManager backupManager("../assets/sample_invalidcrcsingle", 
            m_prefix, m_suffix);

    string data;

    try {
        data = backupManager.load();
    }
    catch (exception &e)
    {
        cout << "Exception caught but not rethrown: " << e.what() << endl;
    }

    Assert(data == "Hello, this is a test.");

    cout << "testLoadInvalidCrcSingle end" << endl;
}

void FileBackupManagerTestCase::testLoadInvalidCrcMultiple()
{
    cout << "testLoadInvalidCrcMultiple begin" << endl;

    FileBackupManager backupManager("../assets/sample_invalidcrcmultiple", 
            m_prefix, m_suffix);
    string data;

    try {
        data = backupManager.load();
    }
    catch (exception &e)
    {
        cout << "Exception caught but not rethrown: " << e.what() << endl;
        throw;
    }

    Assert(data == "");

    cout << "testLoadInvalidCrcMultiple end" << endl;
}


#undef ADD_TEST
