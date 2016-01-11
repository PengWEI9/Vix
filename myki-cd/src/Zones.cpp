/*
 * Stations.cpp
 *
 *  Created on: 08/10/2012
 *      Author: James Ho
 */

#include "Zones.h"
#include <set>

std::set<U8_t> g_citySaverZones;
bool g_citySaverZonesRetrieved;

int initZonesLookup()
{
	g_citySaverZones.clear();
	g_citySaverZonesRetrieved = false;
	return 1;
}

int processZonesXML()
{
	int ret = processXML(
		"processZonesXML", // description
		"Zones.xml",       // xml file
		"Zones",           // xml node
		"Zones",           // master table
		"ZoneRecord");     // record table
	initZonesLookup();
	return ret;
}

int isCitySaverZone(U8_t zoneId)
{
	if ( !g_citySaverZonesRetrieved )
	{
		DbAccess 	*m_dbAccess;
		Rows rows;

		m_dbAccess =  getDBAccess();
        
        if(m_dbAccess==NULL)
            return false;

		char s_query[200];
		sprintf(s_query, "SELECT logical FROM ZoneRecord where is_city_saver='true' AND %s", getCurrentCDVersionWhere());
		m_dbAccess->executeSqlQuery(s_query, rows);

		for ( Rows::iterator row = rows.begin(); row != rows.end(); row++ )
		{
			g_citySaverZones.insert(U8_t(atoi(row->begin()->second)));
		}

		g_citySaverZonesRetrieved = true;
	}

	if ( g_citySaverZones.find(zoneId) != g_citySaverZones.end() )
	{
		return true;
	}
    
	return false;
}
