/* -*- mode: c; tabs: 4; -*- */
/*=============================================================================
**
**    Vix Technology                   Licensed software
**    (C) 2015                         All rights reserved
**
**=============================================================================
**
**  Project/Product : MVU
**  Filename        : serviceproviders.cpp
**  Author(s)       : Damian Chiem
**
**  Description     :
*/
/**     @file
**      @brief  Defines function to validate serviceproviders
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

#include <cstdlib>
#include <json/json.h>
#include <cs.h>
#include <csf.h>
#include "serviceproviders.h"
#include "app_debug_levels.h"
#include <fstream>

/*==========================================================================*
**
**  loadServiceProviders
**
**  Description     :
**      Loads the service provider json file 
**
**  Parameters      :
**      None
**
**  Returns         :
**      None
**
**==========================================================================*/
bool loadServiceProviders()
{
    Json::Reader reader;
    bool result = false;
        
    std::ifstream serviceprovidersData(g_serviceProvidersFile, std::ifstream::binary);
    
    if(reader.parse( serviceprovidersData, g_serviceProviders) && g_serviceProviders.size()>0)
    {
        result = true;
    }
    else
    {
        CsErrx("loadServiceProviders failed to load from %s", g_serviceProvidersFile);
    }
    
    return result;
}

/*==========================================================================*
**
**  validServiceProvider
**
**  Description     :
**      Validates service provider id against json serviceProviders file
**
**  Parameters      :
**      None
**
**  Returns         :
**      None
**
**==========================================================================*/
bool validServiceProvider(int serviceProvider)
{
    if(g_serviceProviders.isArray())
    {
        for(Json::Value::ArrayIndex i=0;i<g_serviceProviders.size();i++)
        {
            if(g_serviceProviders[i]==serviceProvider) return true;
        }
    }
    return false;
}