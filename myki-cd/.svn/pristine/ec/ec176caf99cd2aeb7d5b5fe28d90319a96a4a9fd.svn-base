/* -*- mode: c; tabs: 4; -*- */
/*=============================================================================
**
**    Vix Technology                   Licensed software
**    (C) 2014                         All rights reserved
**
**=============================================================================
**
**  Project/Product : NGBU
**  Filename        : TestDeviceParamters.cpp
**  Author(s)       : Meera Tom
**
**  Description     :
**      Implements unit-tests retrieving products.
**
**  Member(s)       :
**
**  Information     :
**   Compiler(s)    : C++
**   Target(s)      : Independent
**
**  Subversion      :
**
**  History         :
**   Vers.  Date        Aut.  Type     Description
**   -----  ----------  ----  -------  ----------------------------------------
**
**===========================================================================*/

/*
 *      Options
 *      -------
 */

/*
 *      Includes
 *      --------
 */

#include    <string>
#include    <cs.h>
#include    <csf.h>
#include    <TestFramework.h>
#include    <myki_cd.h>

#include    "TestCaseDeviceParameters.h"

/*
 *      Local Constants and Macros
 *      --------------------------
 */

/*
 *      Local Variables
 *      ---------------
 */

/*
 *      Global Variables
 *      ----------------
 */

TestCaseDeviceParameters::TestCaseDeviceParameters( const std::string name, const std::string dbPath ) :
    TestCase( name ),
    m_dbOpened( false ),
    m_dbPath( dbPath )
{
}

TestCaseDeviceParameters::~TestCaseDeviceParameters()
{
}

void
TestCaseDeviceParameters::setUp( const std::string INFile, const std::string OUTFile )
{
}

void
TestCaseDeviceParameters::tearDown( const std::string OUTFile )
{
}

void
TestCaseDeviceParameters::setUpSuite( const std::string inFile )
{
    /*  Default configuration parameter values */
    static  const CsfSysOption_t    defaultSysOptions[ ]    =
    {
        /*  ic_name                         ic_value                            ic_flags    */
        {   "Config:File",                  "unit-test.ini",                    0           },

        {   "Debug:DebugEnabled",           "Y",                                0           },
        {   "Debug:Debug",                  "9",                                0           },
        {   "Debug:Warning",                "Y",                                0           },
        {   "Debug:Verbose",                "Y",                                0           },
        {   "Debug:Console",                "Y",                                0           },
        {   "Debug:LocalLogFile",           "/afc/log/unit-test.log",           0           },
        {   "Debug:LocalLogMB",             "1",                                0           },
        {   "Debug:LocalLogCount",          "3",                                0           },
    };  /*  defaultSysOptions[ ] */
    static  const int               defaultSysOptionsSize   = sizeof( defaultSysOptions ) / sizeof( CsfSysOption_t );

    CsfIniInit( defaultSysOptionsSize, defaultSysOptions );

    if ( MYKI_CD_openCDDatabase( m_dbPath.c_str() ) == false )
    {
        /*  Failed opening Tariff database */
        m_dbOpened  = false;
        CsErrx( "TestCaseDeviceParameters::setUpSuite : failed opening '%s' Tariff", m_dbPath.c_str() );
    }
    else
    {
        m_dbOpened  = true;
    }
}

void
TestCaseDeviceParameters::tearDownSuite( )
{
    if ( m_dbOpened == true )
    {
        MYKI_CD_closeCDDatabase( );
        m_dbOpened  = false;
    }
}

TestSuite
*TestCaseDeviceParameters::suite()
{
    #undef  REGISTER_TEST
    #define REGISTER_TEST( suite, klass, method )\
                {\
                    ( suite )->addTest(\
                        new TestCaller<klass>( #method, &klass::method, this )\
                    );\
                }

    TestSuite  *suite   = new TestSuite( "TestCaseDeviceParameters" );

    if ( suite != NULL )
    {
        REGISTER_TEST( suite, TestCaseDeviceParameters, testGetDeviceParameters_PaymentMethod_001 );
        REGISTER_TEST( suite, TestCaseDeviceParameters, testGetDeviceParameters_PaymentMethod_002 );
    }

    return  suite;
}

    /**
     *  @brief  Successfully check whether Payment type cash is enabled unit-test.
     */
void
TestCaseDeviceParameters::testGetDeviceParameters_PaymentMethod_001()
{
    const char         *deviceType          = "BDC";
    int                 serviceProviderId   = 160;
    const char         *paymentType         = "Cash";
    U8_t                result              = 0;

    Assert( m_dbOpened == true );
    Assert( MYKI_CD_getPaymentMethodEnabled( deviceType, serviceProviderId, paymentType, &result ) == true );

    CsVerbose( "TestCaseDeviceParameters::testGetProducts_PaymentMethod_001 : Payment type = %s, Value = %d", paymentType, result);
}

    /**
     *  @brief  Successfully check whether Payment type Tpurse is enabled unit-test.
     */
void
TestCaseDeviceParameters::testGetDeviceParameters_PaymentMethod_002()
{
    const char         *deviceType          = "BDC";
    int                 serviceProviderId   = 160;
    const char         *paymentType         = "TPurse";
    U8_t                result              = 0;

    Assert( m_dbOpened == true );
    Assert( MYKI_CD_getPaymentMethodEnabled( deviceType, serviceProviderId, paymentType, &result ) == true );

    CsVerbose( "TestCaseDeviceParameters::testGetProducts_PaymentMethod_002 : Payment type = %s, Value = %d", paymentType, result);
}
