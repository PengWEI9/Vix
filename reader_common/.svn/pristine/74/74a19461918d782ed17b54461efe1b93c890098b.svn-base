#include "bbrambackupmanagertestcase.h"
#include <backupmanager/bbrambackupmanager.h>
#include <csf.h>
#include <serpent/drv_bbsram.h>
#include <fstream>
#include <cstring>
#include <algorithm>

using namespace std;
using namespace ReaderCommon;

#define PRINT_DEBUG(X) cout << #X << ": " << X << endl;

#define ADD_TEST(TEST) \
    suite->addTest(new TestCaller<BBRamBackupManagerTestCase>(#TEST, \
                ADDR_OF(BBRamBackupManagerTestCase::TEST), this));


static const char * PARTITION_NAME = "test_partition";
static const size_t PARTITION_SIZE = BUFSIZ;

#ifndef BBRAM_ENABLED
/**
 * This allows use to test the functionality on non-compatible environments.
 *
 * Ideally, we'd have a better method of doing this, such as stubbing
 * lower-level functions. 
 *
 * This will at least allow us to check that our class behaves correctly.
 */
int DrvBbSramOpenByName(DrvBbSramPartition *part, const char *name)
{
    CsDbg(1, "Using custom DrvBbSramOpenByName");
    static void * partitionAddress = NULL;

    if (strcmp(name, PARTITION_NAME) != 0)
    {
        CsDbg(1, "DrvBbSramOpenByName: names don't match. PARTITION_NAME = %s,"
                " given name = %s", PARTITION_NAME, name);
        return -1;
    }
    
    if ((partitionAddress == NULL)
            && ((partitionAddress = calloc(1, PARTITION_SIZE)) == NULL))
    {
            return -1;
    }

    part->memory = partitionAddress;
    part->size = PARTITION_SIZE;

    return 0;
}

void DrvBbSramClose(DrvBbSramPartition *part)
{
    // do nothing
    return;
}

#endif

TestSuite *BBRamBackupManagerTestCase::suite()
{
    TestSuite *suite = new TestSuite(name());

    ADD_TEST(testSave);
    ADD_TEST(testLoad);
    ADD_TEST(testInvalidPartitionName);
    ADD_TEST(testSaveInsufficientMemory);

    return suite;
}

void BBRamBackupManagerTestCase::testSave()
{
    cout << "testSave" << endl;

    BBRamBackupManager backupManager(m_partitionName);

    try {
        backupManager.save("Hello, this is a test.");
    }
    catch (exception &e)
    {
        cout << "Unexpected exception: " << e.what() << endl;
        throw;
    }
}

void BBRamBackupManagerTestCase::testLoad()
{
    cout << "testLoad" << endl;

    string testStr = "Hello, this is a test2.";

    string result;

    try {
        BBRamBackupManager backupManager(m_partitionName);
        backupManager.save(testStr);
        result = backupManager.load();
    }
    catch (exception &e)
    {
        cout << "Unexpected exception: " << e.what() << endl;
        throw;
    }

    Assert(result == testStr);
}

void BBRamBackupManagerTestCase::testInvalidPartitionName()
{
    cout << "testInvalidPartitionName" << endl;

    try {
        BBRamBackupManager("invalid");
    }
    catch (BBRamBackupInitException &e)
    {
        cout << "Expected exception: " << e.what() << endl;
        return;
    }
    catch (exception &e)
    {
        cout << "Unexpected exception: " << e.what() << endl;
        throw;
    }

    Assert(false && "No exception thrown.");
}

void BBRamBackupManagerTestCase::testSaveInsufficientMemory()
{
    cout << "testSaveInsufficientMemory" << endl;

    BBRamBackupManager backupManager(m_partitionName);

    const size_t size = backupManager.partitionSize() + 1;

    char * bigString = new char[size];
    fill_n(bigString, size, 'a');

    try {
        backupManager.save(bigString, size);
    }
    catch (BBRamBackupSizeException &e)
    {
        cout << "Expected exception: " << e.what() << endl;
        return;
    }
    catch (exception &e)
    {
        cout << "Unexpected exception: " << e.what() << endl;
        throw;
    }

    Assert(false && "No exception thrown.");
    delete[] bigString;
}

#undef ADD_TEST
#undef PRINT_DEBUG
