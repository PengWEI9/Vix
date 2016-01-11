/* -*- mode: c; tabs: 4; -*- */
/*=============================================================================
**
**    Vix Technology                   Licensed software
**    (C) 2015                         All rights reserved
**
**=============================================================================
**
**  Project/Product : MVU
**  Filename        : serviceproviders.h
**  Author(s)       : Damian Chiem
**
**  Description     :
*/
/**     @file
**      @brief  Declares function prototypes to validate serviceproviders
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
#if     !defined( __SERVICEPROVIDERS_H_INCLUDED )
#define __SERVICEPROVIDERS_H_INCLUDED            1


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
bool loadServiceProviders();
bool validServiceProvider(int serviceProvider);

/*
 *      Global Variables
 *      ----------------
 */
extern Json::Value              g_serviceProviders;
extern char                     g_serviceProvidersFile[];

#endif /*  !__SERVICEPROVIDERS_H_INCLUDED */