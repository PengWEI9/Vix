/* -*- mode: c; tabs: 4; -*- */
/*=============================================================================
**
**    Vix Technology                   Licensed software
**    (C) 2015                         All rights reserved
**
**=============================================================================
**
**  Project/Product : MVU
**  Filename        : roles.h
**  Author(s)       : Damian Chiem
**
**  Description     :
*/
/**     @file
**      @brief  Declares function prototypes to process staff roles
*/
/*  Member(s)       :
**
**  Information     :
**   Compiler(s)    : C++
**   Target(s)      : Independent
**
**  Subversion      :
**      $Id: 
**      $HeadURL: 
**
**  History         :
**   Vers.  Date        Aut.  Type     Description
**   -----  ----------  ----  -------  ----------------------------------------
**    1.00  23.10.15    DAC   Create
**
**===========================================================================*/

#if     !defined( __ROLES_H_INCLUDED )
#define __ROLES_H_INCLUDED            1

/*
 *      Includes
 *      --------
 */

#include <string>
#include <myki_br.h>
#include <json/json.h>

/*
 *      Prototypes
 *      ----------
 */
bool loadRoles();

/*
 *      Global Variables
 *      ----------------
 */

extern  StaffRole_t          g_staffRoles[];
extern  Json::Value          g_roles;
extern	char				 g_rolesFile[];

#endif  /*  !__ROLES_H_INCLUDED */
