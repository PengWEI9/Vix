/*
 * StationLines.cpp
 *
 *  Created on: 08/10/2012
 *      Author: James Ho
 */

#include "StationLines.h"
#include "CdCache.h"

typedef struct
{
	MYKI_CD_LinesByStation_t linesByStation;
	MYKI_CD_U16Array_t lines;
} LinesByStationItem;
void LinesByStationFree(LinesByStationItem& item)
{
	free(item.lines.arrayOfU16);
}

typedef struct
{
	MYKI_CD_StationsByLine_t stationsByLine;
	MYKI_CD_U16Array_t stations;
} StationsByLineItem;
void StationsByLineFree(StationsByLineItem& item)
{
	free(item.stations.arrayOfU16);
}

CdCache<U16_t, LinesByStationItem> g_linesByStationCache("Lines by station", 250, CacheMemoryCopy<LinesByStationItem>, LinesByStationFree);
CdCache<U16_t, StationsByLineItem> g_stationsByLineCache("Stations by line", 50, CacheMemoryCopy<StationsByLineItem>, StationsByLineFree);

int initStationLinesLookup()
{
	g_linesByStationCache.clear();
	g_stationsByLineCache.clear();
	return 1;
}

int processStationLinesXML()
{
	int ret = processXML(
		"processStationLinesXML", // description
		"StationLines.xml",       // xml file
		"StationLines",           // xml node
		"StationLines",           // master table
		"StationLineRecord");     // record table
	initStationLinesLookup();
	return ret;
}

int getLinesByStation(U16_t stationId, MYKI_CD_LinesByStation_t &linesByStation, MYKI_CD_U16Array_t &lines)
{
	int returnValue=false;
	bool isNull=false;

	LinesByStationItem cacheItem;
	if (g_linesByStationCache.getCacheValue(stationId, cacheItem, isNull))
	{
		if ( !isNull )
		{
			linesByStation = cacheItem.linesByStation;
			lines = cacheItem.lines;
		}
		returnValue = !isNull;
		CsDebug(CD_DEBUG, (CD_DEBUG, "MYKI_CD:getLinesByStation: Return Cache lines by station record found for id %d.", int(stationId)));
	}
	else
	{
		DbAccess 	*m_dbAccess;

		Rows rows;

		m_dbAccess =  getDBAccess();
        if(m_dbAccess==NULL)
            return false;
		char s_query[256];
		sprintf(s_query, "SELECT line FROM StationLineRecord WHERE station='%d' AND %s ORDER BY line ASC", int(stationId), getCurrentCDVersionWhere());

		m_dbAccess->executeSqlQuery(s_query, rows);

		Rows::const_iterator itr;

		lines.arraySize = rows.size();
		if (lines.arraySize > 0)
			lines.arrayOfU16 = (U16_t*)malloc(int(lines.arraySize) * sizeof(U16_t));
		else
			lines.arrayOfU16 = NULL;
		linesByStation.station = stationId;

		int i = 0;
		for(itr = rows.begin(); itr != rows.end(); ++itr){
			const ColumnValue& cv = *itr;
			lines.arrayOfU16[i]=convertStringToU16(cv["line"]);
			i++;
		}

		// TEST: Print out results
		if(rows.empty())
		{
			//std::cout << "\nNo Station record found for Station id " << stationId <<".\n";
			CsDebug(CD_DEBUG, (CD_DEBUG, "MYKI_CD:getServiceProviders: No Station record found for Station id %d.", stationId));
		}
		else
		{
			returnValue = true;
			//printStationLinesMap(rows);
			//testPrintLinesByStationResult(stationId, linesByStation, lines);
		}
		cacheItem.linesByStation = linesByStation;
		cacheItem.lines = lines;
		g_linesByStationCache.addCacheValue(stationId, cacheItem, !returnValue);
	}

	return returnValue;
}

int  getStationsByLine(U16_t lineId, MYKI_CD_StationsByLine_t &stationsByLine, MYKI_CD_U16Array_t &stations)
{
    int returnValue=false;
    bool isNull=false;

	StationsByLineItem cacheItem;
	if (g_stationsByLineCache.getCacheValue(lineId, cacheItem, isNull))
	{
		if (!isNull)
		{
			stationsByLine = cacheItem.stationsByLine;
			stations = cacheItem.stations;
		}
		returnValue = !isNull;
		CsDebug(CD_DEBUG, (CD_DEBUG, "MYKI_CD:getStationsByLine: Return Cache stations by line record found for id %d.", int(lineId)));
	}
	else
	{
		DbAccess 	*m_dbAccess=NULL;

		Rows rows;

		m_dbAccess =  getDBAccess();
        if(m_dbAccess==NULL)
            return false;
		char s_query[256];
		sprintf(s_query, "SELECT station FROM StationLineRecord WHERE line='%d' AND %s ORDER BY station DESC", int(lineId), getCurrentCDVersionWhere());

		m_dbAccess->executeSqlQuery(s_query, rows);

		Rows::const_iterator itr;

		stations.arraySize = rows.size();
		if (stations.arraySize > 0)
			stations.arrayOfU16 = (U16_t*)malloc(int(stations.arraySize) * sizeof(U16_t));
		else
			stations.arrayOfU16 = NULL;
		stationsByLine.line = lineId;

		int i = 0;
		for(itr = rows.begin(); itr != rows.end(); ++itr){
			const ColumnValue& cv = *itr;
			stations.arrayOfU16[i]=convertStringToU16(cv["station"]);
			i++;
		}

		// TEST: Print out results
		if(rows.empty())
		{
			//std::cout << "\nNo Line record found for Line id " << lineId <<".\n";
			CsDebug(CD_DEBUG, (CD_DEBUG, "MYKI_CD:getStationsByLine: No Line record found for Line id %d.", lineId));
		}
		else
		{
			returnValue=true;
			//printStationLinesMap(rows);
			//testPrintStationsByLineResult(lineId, stationsByLine, stations);
		}
		cacheItem.stationsByLine = stationsByLine;
		cacheItem.stations = stations;
		g_stationsByLineCache.addCacheValue(lineId, cacheItem, !returnValue);
	}

	return returnValue;
}

