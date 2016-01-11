#include "filebackupmanagertestcase.h"
#include "bbrambackupmanagertestcase.h"
#include <TestFramework.h>
#include <csf.h>
#include <corebasetypes.h>
#include <cstdlib>
#include <cstring>
#include <algorithm>

using namespace std;

static const int DEBUG_LEVEL = 9;

static bool argExists(int argc, char **argv, const string &targetArg)
{
    char ** end = argv + argc;
    return (std::find(argv, end, targetArg) != end);
}

template <typename T>
static void runTestCase(T &testCase, bool teamCityMode)
{
    cout << testCase.name() << endl;

    TestRunner runner;

    // TeamCity output
    TestResult testResult;
    JetBrains::TeamcityProgressListener listener;
    JUnitTestResultCollector resultCollector;

    if (teamCityMode)
    {
        testResult.addListener(&resultCollector);
        testResult.addListener(&listener);
        runner.setTestResult(&testResult);
    }

    TestSuite *suite = testCase.suite();
    runner.addSuite(suite);
    runner.begin();
}

int main(int argc, char** argv)
{
    CsMsgOptset(CSMSGOP_DEBUG, &DEBUG_LEVEL, (uint32_t)sizeof(int32_t));

    // set date to Melbourne, so times in files match expected
    setenv("TZ", "Australia/Perth", 1);
    tzset();

    FileBackupManagerTestCase fileTestCase;
    BBRamBackupManagerTestCase bbramTestCase;

    bool teamCityMode = argExists(argc, argv, "--teamcity");

    runTestCase(fileTestCase, teamCityMode);
    runTestCase(bbramTestCase, teamCityMode);
}
