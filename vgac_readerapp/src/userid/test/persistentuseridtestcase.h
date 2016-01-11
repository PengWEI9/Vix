#ifndef VGAC_PERSISTENTUSERIDTESTCASE_H
#define VGAC_PERSISTENTUSERIDTESTCASE_H

#include <TestFramework.h>
#include <string>

class PersistentUserIdTestCase : public TestCase
{
    public:
        PersistentUserIdTestCase() : TestCase("PersistentUserIdTestCase") {};
        virtual ~PersistentUserIdTestCase() {};

        void setUp(const std::string inFile, const std::string outFile);
        TestSuite *suite();
        void testReadFile();
        void testSaveFile();
};


#endif
