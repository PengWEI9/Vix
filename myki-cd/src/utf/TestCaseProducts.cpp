/* -*- mode: c; tabs: 4; -*- */
/*=============================================================================
**
**    Vix Technology                   Licensed software
**    (C) 2014                         All rights reserved
**
**=============================================================================
**
**  Project/Product : NGBU
**  Filename        : TestProducts.cpp
**  Author(s)       : An Tran
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
**      $Id: TestCaseProducts.cpp 86281 2015-12-10 19:02:06Z atran $
**      $HeadURL: https://auperasvn01.aupera.erggroup.com/svn/DPG_SWBase/myki-cd/trunk/src/utf/TestCaseProducts.cpp $
**
**  History         :
**   Vers.  Date        Aut.  Type     Description
**   -----  ----------  ----  -------  ----------------------------------------
**    1.00  08.12.15    ANT   Create
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

#include    "TestCaseProducts.h"

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

TestCaseProducts::TestCaseProducts( const std::string name, const std::string dbPath ) :
    TestCase( name ),
    m_dbOpened( false ),
    m_dbPath( dbPath )
{
}

TestCaseProducts::~TestCaseProducts()
{
}

void
TestCaseProducts::setUp( const std::string INFile, const std::string OUTFile )
{
}

void
TestCaseProducts::tearDown( const std::string OUTFile )
{
}

void
TestCaseProducts::setUpSuite( const std::string inFile )
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
        CsErrx( "TestCaseProducts::setUpSuite : failed opening '%s' Tariff", m_dbPath.c_str() );
    }
    else
    {
        m_dbOpened  = true;
    }
}

void
TestCaseProducts::tearDownSuite( )
{
    if ( m_dbOpened == true )
    {
        MYKI_CD_closeCDDatabase( );
        m_dbOpened  = false;
    }
}

TestSuite
*TestCaseProducts::suite()
{
    #undef  REGISTER_TEST
    #define REGISTER_TEST( suite, klass, method )\
                {\
                    ( suite )->addTest(\
                        new TestCaller<klass>( #method, &klass::method, this )\
                    );\
                }

    TestSuite  *suite   = new TestSuite( "TestCaseProducts" );

    if ( suite != NULL )
    {
        REGISTER_TEST( suite, TestCaseProducts, testGetProducts_NonTransit_001  );
        REGISTER_TEST( suite, TestCaseProducts, testGetProducts_Surcharge_001   );
    }

    return  suite;
}

    /**
     *  @brief  Successfully retrieves non-transit product unit-test.
     */
void
TestCaseProducts::testGetProducts_NonTransit_001()
{
    Json::Value         products;
    Json::FastWriter    fw;
    std::string         jsonProducts;
    int                 zone                = 1;
    const char         *productType         = "ThirdParty";
    Time_t              time                = TIME_NOT_SET;
    int                 serviceProviderId   = 18;
    const char         *deviceType          = "BDC";
    int                 productIndex        = 0;

    Assert( m_dbOpened == true );
    Assert( MYKI_CD_getProducts( products, zone, productType, time, serviceProviderId, deviceType ) == true );

    while ( products[ "products" ][ productIndex ].empty() == false )
    {
        CsVerbose( "TestCaseProducts::testGetProducts_NonTransit_001 : Product[%d]", productIndex );
        CsVerbose( "TestCaseProducts::testGetProducts_NonTransit_001 :   id         = %d", products[ "products" ][ productIndex ][ "id"         ].asInt()     );
        CsVerbose( "TestCaseProducts::testGetProducts_NonTransit_001 :   issuer_id  = %d", products[ "products" ][ productIndex ][ "issuer_id"  ].asInt()     );
        CsVerbose( "TestCaseProducts::testGetProducts_NonTransit_001 :   type       = %s", products[ "products" ][ productIndex ][ "type"       ].asCString() );
        CsVerbose( "TestCaseProducts::testGetProducts_NonTransit_001 :   subtype    = %s", products[ "products" ][ productIndex ][ "subtype"    ].asCString() );
        CsVerbose( "TestCaseProducts::testGetProducts_NonTransit_001 :   short_desc = %s", products[ "products" ][ productIndex ][ "short_desc" ].asCString() );
        CsVerbose( "TestCaseProducts::testGetProducts_NonTransit_001 :   long_desc  = %s", products[ "products" ][ productIndex ][ "long_desc"  ].asCString() );
        CsVerbose( "TestCaseProducts::testGetProducts_NonTransit_001 :   Price      = %d", products[ "products" ][ productIndex ][ "Price"      ].asInt()     );

        productIndex++;
    }
    if ( productIndex == 0 )
    {
        CsVerbose( "TestCaseProducts::testGetProducts_NonTransit_001 : No product found" );
    }
    jsonProducts    = fw.write( products );
    CsVerbose( "TestCaseProducts::testGetProducts_NonTransit_001 : %s", jsonProducts.c_str() );
}

    /**
     *  @brief  Successfully retrieves surcharge product unit-test.
     */
void
TestCaseProducts::testGetProducts_Surcharge_001()
{
    Json::Value         products;
    Json::FastWriter    fw;
    std::string         jsonProducts;
    int                 zone                = 1;
    const char         *productType         = "PremiumSurcharge";
    Time_t              time                = TIME_NOT_SET;
    int                 serviceProviderId   = 160;
    const char         *deviceType          = "BDC";
    int                 productIndex        = 0;

    Assert( m_dbOpened == true );
    Assert( MYKI_CD_getProducts( products, zone, productType, time, serviceProviderId, deviceType ) == true );

    while ( products[ "products" ][ productIndex ].empty() == false )
    {
        CsVerbose( "TestCaseProducts::testGetProducts_Surcharge_001 : Product[%d]", productIndex );
        CsVerbose( "TestCaseProducts::testGetProducts_Surcharge_001 :   id         = %d", products[ "products" ][ productIndex ][ "id"         ].asInt()     );
        CsVerbose( "TestCaseProducts::testGetProducts_Surcharge_001 :   issuer_id  = %d", products[ "products" ][ productIndex ][ "issuer_id"  ].asInt()     );
        CsVerbose( "TestCaseProducts::testGetProducts_Surcharge_001 :   type       = %s", products[ "products" ][ productIndex ][ "type"       ].asCString() );
        CsVerbose( "TestCaseProducts::testGetProducts_Surcharge_001 :   subtype    = %s", products[ "products" ][ productIndex ][ "subtype"    ].asCString() );
        CsVerbose( "TestCaseProducts::testGetProducts_Surcharge_001 :   short_desc = %s", products[ "products" ][ productIndex ][ "short_desc" ].asCString() );
        CsVerbose( "TestCaseProducts::testGetProducts_Surcharge_001 :   long_desc  = %s", products[ "products" ][ productIndex ][ "long_desc"  ].asCString() );
        CsVerbose( "TestCaseProducts::testGetProducts_Surcharge_001 :   Price      = %d", products[ "products" ][ productIndex ][ "Price"      ].asInt()     );

        productIndex++;
    }
    if ( productIndex == 0 )
    {
        CsVerbose( "TestCaseProducts::testGetProducts_Surcharge_001 : No product found" );
    }
    jsonProducts    = fw.write( products );
    CsVerbose( "TestCaseProducts::testGetProducts_Surcharge_001 : %s", jsonProducts.c_str() );
}
