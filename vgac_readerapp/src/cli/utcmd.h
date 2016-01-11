/* -*- mode: c; tabs: 4; -*- */
/*=============================================================================
**
**    Vix Technology                   Licensed software
**    (C) 2015                         All rights reserved
**
**=============================================================================
**
**  Project/Product : NGBU
**  Filename        : utcmd.cpp
**  Author(s)       : An Tran
**
**  Description     :
*/
/**     @file
**      @brief  Declares CLI commands for unit-tests.
*/
/*  Member(s)       :
**
**  Information     :
**   Compiler(s)    : C++
**   Target(s)      : Independent
**
**  Subversion      :
**      $Id: $
**      $HeadURL: $
**
**  History         :
**   Vers.  Date        Aut.  Type     Description
**   -----  ----------  ----  -------  ----------------------------------------
**    1.00  30.12.15    ANT   Create
**
**===========================================================================*/

#if     !defined( __UTCMD_H_INCLUDED )
#define __UTCMD_H_INCLUDED              1

/*
 *      Includes
 *      --------
 */

#include <json/json.h>

int AddUnitTestCommands( void );

#endif  /*  !__UTCMD_H_INCLUDED */
