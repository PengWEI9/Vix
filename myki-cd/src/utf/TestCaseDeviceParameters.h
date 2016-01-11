/* -*- mode: c; tabs: 4; -*- */
/*=============================================================================
**
**    Vix Technology                   Licensed software
**    (C) 2014                         All rights reserved
**
**=============================================================================
**
**  Project/Product : NGBU
**  Filename        : TestCaseDeviceParameters.h
**  Author(s)       : Meera Tom
**
**  Description     :
**      Defines unit-test class to retrieve products.
**
**  Member(s)       :
**      CTemplate               [Public]    Constructor
**      ~CTemplate              [Public]    Destructor
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

#if     !defined( __TESTDEVICEPARAMETERS_H_INCLUDED )
#define __TESTDEVICEPARAMETERS_H_INCLUDED

/*
 *      Options
 *      -------
 */

/*
 *      Includes
 *      --------
 */

#include    <string>
#include    <TestFramework.h>

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

    /**
     *  @brief  Products unit-test class.
     */
class   TestCaseDeviceParameters : public TestCase
{
public:

    TestCaseDeviceParameters( const std::string name, const std::string dbPath );
    virtual ~TestCaseDeviceParameters();

    virtual void    setUp( const std::string INFile, const std::string OUTFile );
    virtual void    tearDown( const std::string OUTFile );

    virtual void    setUpSuite( const std::string inFile );
    virtual void    tearDownSuite( );

    TestSuite      *suite();

    void            testGetDeviceParameters_PaymentMethod_001();
    void            testGetDeviceParameters_PaymentMethod_002();

private:

    bool            m_dbOpened;
    std::string     m_dbPath;

};

#endif  /*  !__TESTDEVICEPARAMETERS_H_INCLUDED */
