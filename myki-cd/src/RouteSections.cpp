/*
 * RouteSections.cpp
 *
 *  Created on: 14/07/2014
 *      Author: Morgan Dell
 */

#include "RouteSections.h"
#include "CdCache.h"
#include "DbAccess.h"
#include "ProductConfigurationUtils.h"

CdCache<U32_t, MYKI_CD_RouteSection_t> g_routeSectionsCache("Route sections", 30, CacheMemoryCopy<MYKI_CD_RouteSection_t>);

int initRouteSectionsLookup()
{
	g_routeSectionsCache.clear();
	return 1;
}

int processRouteSectionsXML()
{
	int ret = processXML(
		"processRouteSectionsXML", // description
		"RouteSections.xml",       // xml file
		"RouteSections",           // xml node
		"RouteSections",           // master table
		"RouteSectionRecord");     // record table
	initRouteSectionsLookup();
	return ret;
}

int getRouteSection(U16_t routeId, U8_t stopId, MYKI_CD_RouteSection_t& routeSection)
{
	int returnValue = false;
	bool isNull = false;
	U32_t cacheKey = (U32_t(routeId) << 8) + stopId;

	if ( g_routeSectionsCache.getCacheValue(cacheKey, routeSection, isNull) )
	{
		returnValue = !isNull;
		CsDebug(CD_DEBUG, (CD_DEBUG, "MYKI_CD:getRouteSection: Return Cache RouteSection record found for id %d,%d", routeId, stopId));
	}
	else
	{
		DbAccess* dbAccess = getDBAccess();
        if(dbAccess==NULL)
            return false;        
		Rows rows;

		char s_query[250];
		sprintf(s_query, "SELECT * FROM RouteSectionRecord WHERE route='%u' AND stop='%u' AND %s", (unsigned int)(routeId), (unsigned int)(stopId), getCurrentCDVersionWhere());

		dbAccess->executeSqlQuery(s_query, rows);

		Rows::const_iterator itr1;
		for ( itr1 = rows.begin(); itr1 != rows.end(); ++itr1 )
		{
			const ColumnValue& cv = *itr1;

			routeSection.section = convertStringToU8(cv["section"]);
			stringCopy(routeSection.short_desc, cv["short_desc"], sizeof(routeSection.short_desc));
			stringCopy(routeSection.long_desc, cv["long_desc"], sizeof(routeSection.long_desc));
		}

		if ( rows.empty() )
		{
			CsDebug(CD_DEBUG, (CD_DEBUG, "MYKI_CD:getRouteSection: No RouteSection record found for id %d,%d", routeId, stopId));
			returnValue = false;
		}
		else
		{
			returnValue = true;
		}
		g_routeSectionsCache.addCacheValue(cacheKey, routeSection, !returnValue);
	}

	return returnValue;
}
