/*
 * ServiceProviders.cpp
 *
 *  Created on: 08/10/2012
 *      Author: James Ho
 */

#include "ServiceProviders.h"
#include "CdCache.h"

CdCache<U16_t, MYKI_CD_ServiceProviders_t> g_serviceProvidersCache("Service providers", 100, CacheMemoryCopy<MYKI_CD_ServiceProviders_t>);
CdCache<U16_t, std::string> g_serviceProviderModeCache("Service provider mode", 100, CacheAssignCopy<std::string>);

int initServiceProvidersLookup()
{
	g_serviceProvidersCache.clear();
	g_serviceProviderModeCache.clear();
	return 1;
}

int processServiceProvidersXML()
{
	int ret = processXML(
		"processServiceProvidersXML", // description
		"ServiceProviders.xml",       // xml file
		"ServiceProviders",           // xml node
		"ServiceProviders",           // master table
		"ServiceProviderRecord");     // record table
	initServiceProvidersLookup();
	return ret;
}


int getServiceProviders(U16_t id, MYKI_CD_ServiceProviders_t &serviceProviders)
{
    int returnValue=false;
    bool isNull=false;

	if (g_serviceProvidersCache.getCacheValue(id, serviceProviders, isNull))
	{
		returnValue = !isNull;
		CsDebug(CD_DEBUG, (CD_DEBUG, "MYKI_CD:getServiceProviders: Return Cache Service Providers record found for id %d.", int(id)));
	}
	else
	{
		DbAccess 	*m_dbAccess;

		Rows rows;

		m_dbAccess =  getDBAccess();
        if(m_dbAccess==NULL)
            return false;
		char s_query[200];
		sprintf(s_query, "SELECT * FROM ServiceProviderRecord where id='%d' AND %s", int(id), getCurrentCDVersionWhere());

		m_dbAccess->executeSqlQuery(s_query, rows);


		Rows::const_iterator itr1;
		for(itr1 = rows.begin(); itr1 != rows.end(); ++itr1){
			const ColumnValue& cv = *itr1;
			serviceProviders.id = convertStringToU16(cv["id"]);
			stringCopy(serviceProviders.short_desc, cv["short_desc"], LEN_Short_Description_t);
			stringCopy(serviceProviders.long_desc, cv["long_desc"], LEN_Long_Description_t);
			stringCopy(serviceProviders.mode, cv["mode"], LEN_Mode_t);
		}

		// TEST: Print out results
		if(rows.empty())
		{
			//std::cout << "\nNo Service Provider record found for id " << id <<".\n";
        	CsDebug(CD_DEBUG, (CD_DEBUG, "MYKI_CD:getServiceProviders: No Service Provider record found for id %d.", id));
		}
		else
		{
        	returnValue = true;
			//printServiceProvidersMap(rows);
			//testPrintServiceProvidersResult(id, serviceProviders);
		}
		g_serviceProvidersCache.addCacheValue(id, serviceProviders, !returnValue);
	}
    
    return returnValue;
}

int getServiceProviderTransportMode(U16_t id, char* mode, int bufferLength)
{
   	if(mode==NULL)
       	return false;

    int returnValue=false;
    bool isNull=false;

	std::string outData;
	if ( g_serviceProviderModeCache.getCacheValue(id, outData, isNull) )
	{
		returnValue = !isNull && !outData.empty();
		CsDebug(CD_DEBUG, (CD_DEBUG, "MYKI_CD:getServiceProviderTransportMode: Return Cache Service Provider Mode record found for id %d.", int(id)));
		stringCopy(mode, outData, bufferLength);
	}
	else
	{
		DbAccess 	*m_dbAccess;

		CsDebug(CD_DEBUG, (CD_DEBUG, "MYKI_CD:getServiceProviderTransportMode %d", id));

		m_dbAccess =  getDBAccess();
        if(m_dbAccess==NULL)
            return false;
		char s_query[128];
		sprintf(s_query, "SELECT mode FROM ServiceProviderRecord where id='%d' AND %s", int(id), getCurrentCDVersionWhere());

		m_dbAccess->executeSqlQuerySingleItem(s_query, outData);

		if(outData.empty())
		{
			CsDebug(CD_DEBUG, (CD_DEBUG, "MYKI_CD:getServiceProviderTransportMode %d - no product exists", id));
		}
		else
		{
			CsDebug(CD_DEBUG, (CD_DEBUG, "MYKI_CD:getServiceProviderTransportMode %d - %s", id, outData.c_str()));
			stringCopy(mode, outData, bufferLength);

			returnValue = true;
		}
		g_serviceProviderModeCache.addCacheValue(id, outData, !returnValue);
	}

    return returnValue;
}

