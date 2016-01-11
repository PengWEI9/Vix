/*
 * Routes.cpp
 *
 *  Created on: 14/07/2014
 *      Author: Morgan Dell
 */

#include "Routes.h"
#include "CdCache.h"
#include "DbAccess.h"
#include "ProductConfigurationUtils.h"

CdCache<U16_t, MYKI_CD_RouteInfo_t> g_routesCache("Routes", 30, CacheMemoryCopy<MYKI_CD_RouteInfo_t>);

int initRoutesLookup()
{
	g_routesCache.clear();
	return 1;
}

int processRoutesXML()
{
	int ret = processXML(
		"processRoutesXML", // description
		"Routes.xml",       // xml file
		"Routes",           // xml node
		"Routes",           // master table
		"RouteRecord");     // record table
	initRoutesLookup();
	return ret;
}

int getRouteInfo(U16_t routeId, MYKI_CD_RouteInfo_t& routeInfo)
{
	int returnValue = false;
	bool isNull = false;

	if ( g_routesCache.getCacheValue(routeId, routeInfo, isNull) )
	{
		returnValue = !isNull;
		CsDebug(CD_DEBUG, (CD_DEBUG, "MYKI_CD:getRouteInfo: Return Cache Route record found for id %d", routeId));
	}
	else
	{
		DbAccess* dbAccess = getDBAccess();
        if(dbAccess==NULL)
            return false;        
		Rows rows;

		char s_query[200];
		sprintf(s_query, "SELECT * FROM RouteRecord WHERE id='%u' AND %s", (unsigned int)(routeId), getCurrentCDVersionWhere());

		dbAccess->executeSqlQuery(s_query, rows);

		Rows::const_iterator itr1;
		for ( itr1 = rows.begin(); itr1 != rows.end(); ++itr1 )
		{
			const ColumnValue& cv = *itr1;

			routeInfo.master_route = convertStringToU16(cv["master_route"]);
            stringCopy(routeInfo.code, cv["code"], sizeof(routeInfo.code));
            stringCopy(routeInfo.short_desc, cv["short_desc"], sizeof(routeInfo.short_desc));
            routeInfo.route_id = convertStringToU16(cv["id"]);
			stringCopy(routeInfo.short_desc, cv["short_desc"], sizeof(routeInfo.short_desc));
			stringCopy(routeInfo.long_desc, cv["long_desc"], sizeof(routeInfo.long_desc));
		}

		if ( rows.empty() )
		{
			CsDebug(CD_DEBUG, (CD_DEBUG, "MYKI_CD:getRouteInfo: No Route record found for id %d", routeId));
			returnValue = false;
		}
		else
		{
			returnValue = true;
		}
		g_routesCache.addCacheValue(routeId, routeInfo, !returnValue);
	}

	return returnValue;
}


int getServiceProviderRoutes(U16_t serviceProviderId, MYKI_CD_RouteInfo_t* pRoutes, U16_t arrayLength)
{
	CsDebug(CD_DEBUG, (CD_DEBUG, "MYKI_CD:getMasterRoute %d", serviceProviderId));

	bool isNull = false;
    DbAccess* dbAccess =  getDBAccess();
    if(dbAccess==NULL)
        return 0;    
    Rows rows;

    char query[512];
    
    snprintf(query, 512, "SELECT r.id, r.master_route, m.short_desc as code, r.short_desc as short_desc, m.long_desc as long_desc from routeRecord r, masterRouteRecord m where m.service_provider=%u and r.master_route=m.id and r.fk_major_version=%d AND r.fk_minor_version=%d and m.fk_major_version=%d AND m.fk_minor_version=%d",
        serviceProviderId, getCurrentCDMajorVersion(), getCurrentCDMinorVersion(), getCurrentCDMajorVersion(), getCurrentCDMinorVersion());

    dbAccess->executeSqlQuery(query, rows);

    U32_t primaryKey;
    Rows::const_iterator itr1;
    int i =0;
    for ( itr1 = rows.begin(); itr1 != rows.end() && i<arrayLength; ++itr1 )
    {
        const ColumnValue& cv = *itr1;
        pRoutes[i].route_id     = convertStringToU16(cv["id"]);
        pRoutes[i].master_route = convertStringToU16(cv["master_route"]);
        stringCopy(pRoutes[i].code,          cv["code"],   sizeof(pRoutes[i].short_desc));
        stringCopy(pRoutes[i].short_desc,    cv["short_desc"],   sizeof(pRoutes[i].short_desc));
        stringCopy(pRoutes[i].long_desc,     cv["long_desc"],    sizeof(pRoutes[i].long_desc));
   
        i++;
    }
    
	return i;
}

