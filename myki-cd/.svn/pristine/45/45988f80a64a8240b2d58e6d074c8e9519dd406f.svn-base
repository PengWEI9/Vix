/*
 * PassengerTypes.cpp
 *
 *  Created on: 04/10/2012
 *      Author: James Ho
 */

#include "PassengerTypes.h"
#include "CdCache.h"

CdCache<U8_t, MYKI_CD_PassengerType_t> g_passengerTypeCache("Passenger type", 100, CacheMemoryCopy<MYKI_CD_PassengerType_t>);

int initPassengerTypesLookup()
{
	g_passengerTypeCache.clear();
	return 1;
}

int processPassengerTypesXML()
{
	int ret = processXML(
		"processPassengerTypesXML", // description
		"PassengerTypes.xml",       // xml file
		"PassengerTypes",           // xml node
		"PassengerTypes",           // master table
		"PassengerTypeRecord");     // record table
	initPassengerTypesLookup();
	return ret;
}


int getPassengerType(U8_t id, MYKI_CD_PassengerType_t &passengerType)
{
    int returnValue=false;
    bool isNull=false;
    
	if ( g_passengerTypeCache.getCacheValue(id, passengerType, isNull) )
	{
		returnValue = !isNull;
		CsDebug(CD_DEBUG, (CD_DEBUG, "MYKI_CD:getPassengerType: Return Cache PassengerType record found for id %d", int(id)));
	}
	else
	{
		DbAccess 	*m_dbAccess;

		Rows rows;

		m_dbAccess =  getDBAccess();
        if(m_dbAccess==NULL)
            return false;  
        
		char s_query[200];
		sprintf(s_query, "SELECT * FROM PassengerTypeRecord WHERE id='%d' AND %s", int(id), getCurrentCDVersionWhere());

		m_dbAccess->executeSqlQuery(s_query, rows);

		Rows::const_iterator itr1;
		for(itr1 = rows.begin(); itr1 != rows.end(); ++itr1){
			const ColumnValue& cv = *itr1;
			passengerType.id = convertStringToU8(cv["id"]);
			stringCopy(passengerType.code, cv["code"], LEN_Code_t);
			stringCopy(passengerType.desc, cv["desc"], LEN_Short_Description_t);
			passengerType.percent = convertStringToU8(cv["percent"]);
			stringCopy(passengerType.isdda, cv["isdda"], LEN_6_t);
			passengerType.cardexpirydays = convertStringToU16(cv["cardexpirydays"]);
			passengerType.tappexpirydays = convertStringToU16(cv["tappexpirydays"]);
			passengerType.passengercodedays = convertStringToU16(cv["passengercodedays"]);
		}

		// TEST: Print out results
		if(rows.empty())
		{
			//std::cout << "\nNo Passenger Type record found for id " << (int)id <<".\n";
        	CsDebug(CD_DEBUG, (CD_DEBUG, "MYKI_CD:getPassengerType: No Passenger Type record found for id %d.", id));
		}
		else
		{
        	returnValue = true;
			//printPassengerTypeMap(rows);
			//testPrintPassengerTypeResult(id, passengerType);
		}
		g_passengerTypeCache.addCacheValue(id, passengerType, !returnValue);
	}

    return returnValue;
}

