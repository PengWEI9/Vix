/* -*- mode: c; tabs: 4; -*- */
/*=============================================================================
**
**    Vix Technology                   Licensed software
**    (C) 2015                         All rights reserved
**
**=============================================================================
**
**  Project/Product : MVU
**  Filename        : roles.cpp
**  Author(s)       : Damian Chiem
**
**  Description     :
*/
/**     @file
**      @brief  Defines function to process staff roles
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

/*
 *      Includes
 *      --------
 */

#include <cstdlib>
#include <json/json.h>
#include <cs.h>
#include <csf.h>
#include "roles.h"
#include "app_debug_levels.h"
#include <fstream>

/*
 *      Local Prototypes
 *      ----------------
 */

bool isRole(std::string rolename, int type, int profile);
bool isAttendantRole(int type, int profile);
bool isTechnicianRole(int type, int profile);
bool isDriverRole(int type, int profile);
bool isAuthorisedOfficerRole(int type, int profile);


/*==========================================================================*
**
**  loadRoles
**
**  Description     :
**      Loads the roles json file 
**
**  Parameters      :
**      None
**
**  Returns         :
**      None
**
**==========================================================================*/
bool loadRoles()
{
    Json::Reader reader;
    bool result = false;
        
    std::ifstream rolesData(g_rolesFile, std::ifstream::binary);
    
    if(reader.parse( rolesData, g_roles) && g_roles.size()>0)
    {
        if(g_roles.isArray())
        {
            for(Json::Value::ArrayIndex i=0;i<g_roles.size();i++) 
            {
                g_staffRoles[i].operatorType = OperatorType_UNKNOWN;
               
                if( g_roles[i]["profile"].isInt() && g_roles[i]["type"].isInt() ) 
                {
                    g_staffRoles[i].type = g_roles[i]["type"].asInt(); 
                    g_staffRoles[i].profile = g_roles[i]["profile"].asInt();   
                }
                // for each 'key' property in the array, ie: profile, type, permissions
                for( unsigned int j=0 ; j<g_roles[i].getMemberNames().size() ; j++ )
                {
                   std::string memberName = g_roles[i].getMemberNames()[j];
                   if ( memberName.compare("technician") == 0 )
                   {
                       g_staffRoles[i].operatorType = OperatorType_TECHNICIAN;
                   }
                   else if ( memberName.compare("driver") == 0 )
                   {
                       g_staffRoles[i].operatorType = OperatorType_DRIVER;
                   }
                   else if ( memberName.compare("supervisor") == 0 )
                   {
                       g_staffRoles[i].operatorType = OperatorType_SUPERVISOR;
                   }
                   else if ( memberName.compare("authorised-officer") == 0 )
                   {
                       g_staffRoles[i].operatorType = OperatorType_AUTHORISED_OFFICER;
                   }
                   else if ( memberName.compare("attendant") == 0 )
                   {
                       g_staffRoles[i].operatorType = OperatorType_ATTENDANT;
                   }
                }

                CsDebug(APP_DEBUG_FLOW, (APP_DEBUG_FLOW, "GacProcessingThread: loadRoles found operator (%d) in staff['%u']", g_staffRoles[i].operatorType, i));
            }
        }      
        result = true;
    }
    else
    {
        CsErrx("loadRoles failed to load from %s", g_rolesFile);
    }
    
    return result;
}


/*==========================================================================*
**
**  isRole
**
**  Description     :
**      Validates provided role against json roles file
**
**  Parameters      :
**      None
**
**  Returns         :
**      None
**
**==========================================================================*/
bool isRole(std::string rolename, int type, int profile)
{
    if(g_roles.isArray())
    {
        for(Json::Value::ArrayIndex i=0;i<g_roles.size();i++)
        {
            if(g_roles[i]["profile"].isInt() && g_roles[i]["type"].isInt() && g_roles[i][rolename].isInt()) 
            {
                if(g_roles[i]["profile"].asInt()==profile && g_roles[i]["type"]==type && g_roles[i][rolename].asInt()==1) 
                {
                    CsDebug(APP_DEBUG_FLOW, (APP_DEBUG_FLOW, "GacProcessingThread: isRole  rolename %s type %d profile %d found", rolename.c_str(), type, profile));
                    return true;
                 }  
            }             
        }
    }
    return false;
}

/*==========================================================================*
**
**  isAttendantRole
**
**  Description     :
**      Helper function
**
**  Parameters      :
**      None
**
**  Returns         :
**      None
**
**==========================================================================*/
bool isAttendantRole(int type, int profile)
{
    return isRole("attendant", type, profile);
}

/*==========================================================================*
**
**  isTechnicianRole
**
**  Description     :
**      Helper function
**
**  Parameters      :
**      None
**
**  Returns         :
**      None
**
**==========================================================================*/
bool isTechnicianRole(int type, int profile)
{
    return isRole("technician", type, profile);
}

/*==========================================================================*
**
**  isDriverRole
**
**  Description     :
**      Helper function
**
**  Parameters      :
**      None
**
**  Returns         :
**      None
**
**==========================================================================*/
bool isDriverRole(int type, int profile)
{
    return isRole("driver", type, profile);
}

/*==========================================================================*
**
**  isAuthorisedOfficerRole
**
**  Description     :
**      Helper function
**
**  Parameters      :
**      None
**
**  Returns         :
**      None
**
**==========================================================================*/
bool isAuthorisedOfficerRole(int type, int profile)
{
    return isRole("authorised-officer", type, profile);
}