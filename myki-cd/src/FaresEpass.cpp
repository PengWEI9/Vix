/*
 * FaresEpass.cpp
 *
 *  Created on: 08/10/2012
 *      Author: James Ho
 */

#include "FaresEpass.h"
#include "CdCache.h"

CdCache<U32_t, MYKI_CD_FaresEpass_t> g_faresEpassCache("FaresEpass", 20, CacheMemoryCopy<MYKI_CD_FaresEpass_t>);

int initFaresEpassLookup()
{
	g_faresEpassCache.clear();
    return 1;
}

int processFaresEpassXML()
{
	int ret = processXML(
		"processFaresEpassXML", // description
		"FaresePass.xml",       // xml file
		"FaresePass",           // xml node
		"FaresePass",           // master table
		"FareePassRecord");     // record table
	initFaresEpassLookup();
	return ret;
}


int getFaresEpass(U8_t zoneLow, U8_t zoneHigh, U16_t days, MYKI_CD_FaresEpass_t &faresEpass)
{
    int returnValue=false;
	bool isNull=false;
    
	CsDebug(CD_DEBUG, (CD_DEBUG, "MYKI_CD:getFaresEpass %d %d %d", int(zoneLow), int(zoneHigh), int(days)));
    
	U32_t cacheKey = (U32_t(zoneLow) << 24) + (U32_t(zoneHigh) << 16) + days;
	if(g_faresEpassCache.getCacheValue(cacheKey, faresEpass, isNull))
	{
		returnValue = !isNull;
		CsDebug(CD_DEBUG, (CD_DEBUG, "MYKI_CD:getFaresEpass: Return cache FaresEpass record found for %d %d %d", int(zoneLow), int(zoneHigh), int(days)));
	}
	else
	{
		DbAccess 	*m_dbAccess;

		Rows rows;

		m_dbAccess =  getDBAccess();
        if(m_dbAccess==NULL)
            return false; 
		char s_query[350];
		sprintf(s_query, "SELECT * FROM FareePassRecord where days='%d' AND ((zone_low='%d' AND zone_high='%d') OR (zone_high='%d' AND zone_low='%d')) AND %s",
			int(days),
			int(zoneLow),
			int(zoneHigh),
			int(zoneLow),
			int(zoneHigh),
			getCurrentCDVersionWhere());

		m_dbAccess->executeSqlQuery(s_query, rows);


		Rows::const_iterator itr1;
		for(itr1 = rows.begin(); itr1 != rows.end(); ++itr1){
			const ColumnValue& cv = *itr1;
			faresEpass.zone_low = convertStringToU8(cv["zone_low"]);
			faresEpass.zone_high = convertStringToU8(cv["zone_high"]);
			faresEpass.days = convertStringToU16(cv["days"]);
			faresEpass.value = convertStringToU32(cv["value"]);
		}

		if(rows.empty())
		{
	        CsDebug(CD_DEBUG, (CD_DEBUG, "MYKI_CD:getFaresEpass: No Fares E-Pass record found for zoneLow(%d) zoneHigh(%d) days(%d).", zoneLow, zoneHigh, days));
		}
		else
		{
	        returnValue = true;
	        CsDebug(CD_DEBUG, (CD_DEBUG, "MYKI_CD:getFaresEpass return result"));
		}
		g_faresEpassCache.addCacheValue(cacheKey, faresEpass, !returnValue);
	}

    return returnValue;
}

