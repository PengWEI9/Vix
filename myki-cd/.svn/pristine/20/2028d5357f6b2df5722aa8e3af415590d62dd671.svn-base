/*
 * FaresProvisional.cpp
 *
 *  Created on: 05/10/2012
 *      Author: James Ho
 */

#include "FaresProvisional.h"
#include "CdCache.h"

CdCache<int, MYKI_CD_FaresProvisional_t> g_faresProvisionalCache("Fares provisional", 200, CacheMemoryCopy<MYKI_CD_FaresProvisional_t>);

int initFaresProvisionalLookup()
{
	g_faresProvisionalCache.clear();
	return 1;
}

int processFaresProvisionalXML()
{
	int ret = processXML(
		"processFaresProvisionalXML", // description
		"FaresProvisional.xml",       // xml file
		"FaresProvisional",           // xml node
		"FaresProvisional",           // master table
		"FareProvisionalRecord");     // record table
	initFaresProvisionalLookup();
	return ret;
}

int getFaresProvisional(U8_t zoneLow, U8_t zoneHigh, MYKI_CD_FaresProvisional_t &faresProvisional)
{
    int returnValue=false;
    bool isNull=false;
    
	int id = (zoneHigh << 8) + zoneLow;
	
    if(g_faresProvisionalCache.getCacheValue(id, faresProvisional, isNull))
    {
        returnValue = !isNull;
		CsDebug(CD_DEBUG, (CD_DEBUG, "MYKI_CD:getFaresProvisional: Return Cache Fares Provisional found for zoneLow %d zoneHigh %d.", zoneLow, zoneHigh));
    }
	else
	{		
		DbAccess 	*m_dbAccess;
		Rows rows;

		m_dbAccess =  getDBAccess();
        if(m_dbAccess==NULL)
            return false; 
		char s_query[350];
		sprintf(s_query, "SELECT * FROM FareProvisionalRecord WHERE ((zone_low='%d' AND zone_high='%d') OR (zone_high='%d' AND zone_low='%d')) AND %s",
			int(zoneLow),
			int(zoneHigh),
			int(zoneLow),
			int(zoneHigh),
			getCurrentCDVersionWhere());

		m_dbAccess->executeSqlQuery(s_query, rows);


		Rows::const_iterator itr1;
		for(itr1 = rows.begin(); itr1 != rows.end(); ++itr1){
			const ColumnValue& cv = *itr1;
			faresProvisional.zone_low = convertStringToU8(cv["zone_low"]);
			faresProvisional.zone_high = convertStringToU8(cv["zone_high"]);
			faresProvisional.sv_full = convertStringToU32(cv["sv_full"]);
			faresProvisional.sv_conc = convertStringToU32(cv["sv_conc"]);
			faresProvisional.epass_full = convertStringToU32(cv["epass_full"]);
			faresProvisional.epass_conc = convertStringToU32(cv["epass_conc"]);
		}


		// TEST: Print out results
		if(rows.empty())
		{
			//std::cout << "\nNo Fares Provisional record found for zoneLow(" << (int)zoneLow << ") zoneHigh(" << (int)zoneHigh << ").\n";
			CsDebug(CD_DEBUG, (CD_DEBUG, "MYKI_CD:getFaresProvisional: No Fares Provisional record found for zoneLow(%d) zoneHigh(%d).", zoneLow, zoneHigh));
		}
		else
		{
			returnValue = true;
			//printFaresProvisionalMap(rows);
			//testPrintFaresProvisionalResult(zoneLow, zoneHigh, faresProvisional);
		}
		g_faresProvisionalCache.addCacheValue(id, faresProvisional, !returnValue);
	}
    
    return returnValue;
}

