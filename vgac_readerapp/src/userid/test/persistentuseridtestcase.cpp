#include "persistentuseridtestcase.h"
#include "../persistentuserid.h"
#include <csf.h>
#include <string>
#include <iostream>
#include <fstream>

using namespace std;

#define ADD_TEST(TEST) \
    suite->addTest(new TestCaller<PersistentUserIdTestCase>(#TEST, \
                &PersistentUserIdTestCase::TEST, this));

#define PRINT_DEBUG(X) std::cout << #X ": " << X << std::endl;

const string originalValue = "VIX123";

TestSuite *PersistentUserIdTestCase::suite()
{
    TestSuite *suite = new TestSuite(name());

    ADD_TEST(testReadFile);
    ADD_TEST(testSaveFile);

    return suite;
}

void PersistentUserIdTestCase::setUp(const string inFile, const string outFile)
{
    CsFileCopy("../assets/userid", "userid");
}

void PersistentUserIdTestCase::testReadFile()
{
    cout << "testReadFile" << endl;

    PersistentUserId userId("userid");

    PRINT_DEBUG(userId.get());
    Assert(userId.get() == originalValue);
}

void PersistentUserIdTestCase::testSaveFile()
{
    cout << "testSaveFile" << endl;

    string newValue = "VIX321";

    PersistentUserId userId("userid");
    Assert(userId.get() == originalValue);

    userId = newValue;
    PRINT_DEBUG(userId.get());
    Assert(userId.get() == newValue);

    // Verify string was saved
    ifstream stream("userid");
    string tmp;
    stream >> tmp;
    PRINT_DEBUG(tmp);
    Assert(tmp == newValue);
}

#undef ADD_TEST
#undef PRINT_DEBUG
