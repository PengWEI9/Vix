#include "persistentuseridtestcase.h"
#include <csf.h>
#include <algorithm>

using namespace std;

static const int DEBUG_LEVEL = 9;

bool argExists(int argc, char **argv, const string &targetArg)
{
    char ** end = argv + argc;
    return (find(argv, end, targetArg) != end);
}

int main(int argc, char *argv[])
{
    CsMsgOptset(CSMSGOP_DEBUG, &DEBUG_LEVEL, (uint32_t)sizeof(int32_t));

    PersistentUserIdTestCase testCase;
    TestRunner runner;

    // TeamCity output
    TestResult testResult;
    JetBrains::TeamcityProgressListener listener;
    JUnitTestResultCollector resultCollector;

    // enable TeamCity output
    if (argExists(argc, argv, "--teamcity"))
    {
        testResult.addListener(&resultCollector);
        testResult.addListener(&listener);
        runner.setTestResult(&testResult);
    }

    TestSuite *suite = testCase.suite();
    runner.addSuite(suite);
    runner.begin();
}
