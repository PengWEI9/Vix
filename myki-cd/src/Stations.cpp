/*
 * Stations.cpp
 *
 *  Created on: 08/10/2012
 *      Author: James Ho
 */

#include "Stations.h"
#include "CdCache.h"

CdCache<U16_t, MYKI_CD_Stations_t> g_stationCache("Stations", 200, CacheMemoryCopy<MYKI_CD_Stations_t>);
CdCache<U16_t, MYKI_CD_Stations_t> g_stationsByLocationCache("Stations by location", 100, CacheMemoryCopy<MYKI_CD_Stations_t>);

int initStationsLookup()
{
	g_stationCache.clear();
	g_stationsByLocationCache.clear();
	return 1;
}

int processStationsXML()
{
	int ret = processXML(
		"processStationsXML", // description
		"Stations.xml",       // xml file
		"Stations",           // xml node
		"Stations",           // master table
		"StationRecord");     // record table
	initStationsLookup();
	return ret;
}

int getStations(U16_t id, MYKI_CD_Stations_t &stations)
{
    int returnValue=false;
    bool isNull=false;
	
	if(g_stationCache.getCacheValue(id, stations, isNull))
    {
        returnValue = !isNull;
		CsDebug(CD_DEBUG, (CD_DEBUG, "MYKI_CD:getStations: Return Cache Station found for id %d.", id));
    }
	else
	{	
		DbAccess 	*m_dbAccess;

		Rows rows;

		m_dbAccess =  getDBAccess();
        if(m_dbAccess==NULL)
            return false;
		char s_query[200];
		sprintf(s_query, "SELECT * FROM StationRecord where id='%d' AND %s", int(id), getCurrentCDVersionWhere());

		m_dbAccess->executeSqlQuery(s_query, rows);

		Rows::const_iterator itr1;
		for(itr1 = rows.begin(); itr1 != rows.end(); ++itr1){
			const ColumnValue& cv = *itr1;
			stations.id = convertStringToU16(cv["id"]);
			stations.location = convertStringToU16(cv["location"]);
			stations.provisional_zone_low = convertStringToU8(cv["provisional_zone_low"]);
			stations.provisional_zone_high = convertStringToU8(cv["provisional_zone_high"]);
		}

		// TEST: Print out results
		if(rows.empty())
		{
			CsDebug(CD_DEBUG, (CD_DEBUG, "MYKI_CD:getStations: No Station record found for id %d.", id));
		}
		else
		{
			returnValue = true;
			//printStationsMap(rows);
			//testPrintStationsResult(id, stations);
		}
		g_stationCache.addCacheValue(id, stations, !returnValue);
	}
    
    return returnValue;
}

int getStationsByLocationId(U16_t locationId, MYKI_CD_Stations_t &stations)
{
    int returnValue=false;
    bool isNull=false;
	
	if(g_stationsByLocationCache.getCacheValue(locationId, stations, isNull))
    {
        returnValue = !isNull;
		CsDebug(CD_DEBUG, (CD_DEBUG, "MYKI_CD:getStations: Return Cache Station found for locationId %d.", locationId));
    }
	else
	{		
		DbAccess 	*m_dbAccess;

		Rows rows;

		m_dbAccess =  getDBAccess();
        if(m_dbAccess==NULL)
            return false;
        
		char s_query[200];
		sprintf(s_query, "SELECT * FROM StationRecord where location='%d' AND %s", int(locationId), getCurrentCDVersionWhere());

		m_dbAccess->executeSqlQuery(s_query, rows);

		Rows::const_iterator itr1;
		for(itr1 = rows.begin(); itr1 != rows.end(); ++itr1){
			const ColumnValue& cv = *itr1;
			stations.id = convertStringToU16(cv["id"]);
			stations.location = convertStringToU16(cv["location"]);
			stations.provisional_zone_low = convertStringToU8(cv["provisional_zone_low"]);
			stations.provisional_zone_high = convertStringToU8(cv["provisional_zone_high"]);
		}

		// TEST: Print out results
		if(rows.empty())
		{
			//std::cout << "\nMYKI_CD:getStationsFromLocationId :No Station record found for id " << locationId <<".\n";
			CsDebug(CD_DEBUG, (CD_DEBUG, "MYKI_CD:getStationsFromLocationId: No Station record found for Location id %d.", locationId ));
		}
		else
		{
			returnValue = true;
			//printStationsMap(rows);
			//testPrintStationsResult(locationId, stations);
		}
		g_stationsByLocationCache.addCacheValue(locationId, stations, !returnValue);
	}

    return returnValue;
}
