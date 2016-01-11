/* -*- mode: c; tabs: 4; -*- */
/*=============================================================================
**
**    Vix Technology                   Licensed software
**    (C) 2015                         All rights reserved
**
**=============================================================================
**
**  Project/Product : NGBU
**  Filename        : wssales.cpp
**  Author(s)       : An Tran
**
**  Description     :
*/
/**     @file
**      @brief  Declares Web Services to perform product sales transactions.
*/
/*  Member(s)       :
**
**  Information     :
**   Compiler(s)    : C++
**   Target(s)      : Independent
**
**  Subversion      :
**      $Id: wssales.h 87801 2015-12-30 03:21:31Z atran $
**      $HeadURL: https://auperasvn01.aupera.erggroup.com/svn/DPG_SWBase/vgac_readerapp/trunk/src/wssales.h $
**
**  History         :
**   Vers.  Date        Aut.  Type     Description
**   -----  ----------  ----  -------  ----------------------------------------
**    1.00  10.12.15    ANT   Create
**
**===========================================================================*/

#if     !defined( __WSSALES_H_INCLUDED )
#define __WSSALES_H_INCLUDED            1

/*
 *      Options
 *      -------
 */

/*
 *      Includes
 *      --------
 */

#include <string>
#include <json/json.h>

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

std::string processProductSales( Json::Value &request );
std::string processProductSalesReversal( Json::Value &request );

#endif  /*  !__WSSALES_H_INCLUDED */
