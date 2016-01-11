/*
 * FaresStoredValue.cpp
 *
 *  Created on: 04/10/2012
 *      Author: James Ho
 */

#include "FaresStoredValue.h"
#include "CdCache.h"

CdCache<int, MYKI_CD_FaresStoredValue_t> g_fareStoredValueCache("Fares stored value", 200, CacheMemoryCopy<MYKI_CD_FaresStoredValue_t>);

int initFaresStoredValueLookup()
{
	g_fareStoredValueCache.clear();
	return 1;
}

int processFaresStoredValueXML()
{
	int ret = processXML(
		"processFaresStoredValueXML", // description
		"FaresStoredValue.xml",       // xml file
		"FaresStoredValue",           // xml node
		"FaresStoredValue",           // master table
		"FareStoredValueRecord");     // record table
	initFaresStoredValueLookup();
	return ret;
}

int getFaresStoredValue(U8_t zoneLow, U8_t zoneHigh, MYKI_CD_FaresStoredValue_t &faresStoredValue)
{
    int returnValue=false;
    bool isNull=false;
	
	int id = (zoneHigh << 8) + zoneLow;
	
    if(g_fareStoredValueCache.getCacheValue(id, faresStoredValue, isNull))
    {
        returnValue = !isNull;
		CsDebug(CD_DEBUG, (CD_DEBUG, "MYKI_CD:getFaresStoredValue: Return Cache Fares Stored Value found for zoneLow %d zoneHigh %d.", zoneLow, zoneHigh));
    }
	else
	{	
	    DbAccess 	*m_dbAccess;
		Rows rows;
	
		m_dbAccess =  getDBAccess();
        if(m_dbAccess==NULL)
            return false;  
        
		char s_query[350];
		sprintf(s_query, "SELECT * FROM FareStoredValueRecord WHERE ((zone_low='%d' AND zone_high='%d') OR (zone_high='%d' AND zone_low='%d')) AND %s",
			int(zoneLow),
			int(zoneHigh),
			int(zoneLow),
			int(zoneHigh),
			getCurrentCDVersionWhere());

		m_dbAccess->executeSqlQuery(s_query, rows);

		Rows::const_iterator itr1;
		for(itr1 = rows.begin(); itr1 != rows.end(); ++itr1){
			const ColumnValue& cv = *itr1;
			faresStoredValue.zone_low = convertStringToU8(cv["zone_low"]);
			faresStoredValue.zone_high = convertStringToU8(cv["zone_high"]);
			faresStoredValue.single_trip = convertStringToU32(cv["single_trip"]);
			faresStoredValue.nhour = convertStringToU32(cv["nhour"]);
			faresStoredValue.daily = convertStringToU32(cv["daily"]);
			faresStoredValue.weekly = convertStringToU32(cv["weekly"]);
			faresStoredValue.weekend = convertStringToU32(cv["weekend"]);
		}

		// TEST: Print out results
		if(rows.empty())
		{
			//std::cout << "\nNo Fares Stored Value record found for zoneLow(" << (int)zoneLow << ") zoneHigh(" << (int)zoneHigh << ").\n";
			CsDebug(CD_DEBUG, (CD_DEBUG, "MYKI_CD:getFaresStoredValue: No Fares Stored Value record found for zoneLow(%d) zoneHigh(%d).", zoneLow, zoneHigh));
		}
		else
		{
			returnValue = true;
			//printFaresStoredValueMap(rows);
			//testPrintFaresStoredValueResult(zoneLow, zoneHigh, faresStoredValue);
		}
		g_fareStoredValueCache.addCacheValue(id, faresStoredValue, !returnValue);
    }
	
    return returnValue;
}

