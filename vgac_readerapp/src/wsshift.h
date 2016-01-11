/* -*- mode: c; tabs: 4; -*- */
/*=============================================================================
**
**    Vix Technology                   Licensed software
**    (C) 2015                         All rights reserved
**
**=============================================================================
**
**  Project/Product : MVU
**  Filename        : wsshift.h
**  Author(s)       : An Tran
**
**  Description     :
*/
/**     @file
**      @brief  Declares function prototypes to process start/end shift.
*/
/*  Member(s)       :
**
**  Information     :
**   Compiler(s)    : C++
**   Target(s)      : Independent
**
**  Subversion      :
**      $Id: wsshift.h 84103 2015-11-26 19:40:46Z atran $
**      $HeadURL: https://auperasvn01.aupera.erggroup.com/svn/DPG_SWBase/vgac_readerapp/trunk/src/wsshift.h $
**
**  History         :
**   Vers.  Date        Aut.  Type     Description
**   -----  ----------  ----  -------  ----------------------------------------
**    1.00  07.10.15    ANT   Create
**
**===========================================================================*/

#if     !defined( __WSSHIFT_H_INCLUDED )
#define __WSSHIFT_H_INCLUDED            1

/*
 *      Includes
 *      --------
 */

#include <string>
#include <json/json.h>

/*
 *      Prototypes
 *      ----------
 */

std::string processLogon( Json::Value &request );
std::string processTableCard( Json::Value &request );
std::string processTrip( Json::Value &request );
std::string processShiftEnd( Json::Value &request );
std::string processLogOff( Json::Value &request );
std::string processPeriodCommitCheck( Json::Value &request );
std::string processGetDriverTotals( Json::Value &request );

/*
 *      Global Variables
 *      ----------------
 */

extern  MYKI_BR_ShiftData_t         g_ShiftData;
extern  unsigned int                g_Headless_Shift_Id;

#endif  /*  !__WSSHIFT_H_INCLUDED */
