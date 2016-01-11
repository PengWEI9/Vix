#include "txnlisttestcase.h"
#include <TestFramework.h>
#include <csf.h>
#include <corebasetypes.h>
#include <iostream>
#include <cstring>

static const int DEBUG_LEVEL = 9;

int main(int argc, char *argv[])
{
    CsMsgOptset(CSMSGOP_DEBUG, &DEBUG_LEVEL, (uint32_t)sizeof(int32_t));
    CsfIniOpen("../assets/config.ini", NULL);

    bool teamCityMode = false;

    // enable TeamCity output
    if (argc > 1 && strncmp(argv[1], "--teamcity", strlen(argv[1])) == 0)
    {
        teamCityMode = true;
    }

    TestRunner runner;
    TxnListTestCase testCase;

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

    CsfIniClose();
}
