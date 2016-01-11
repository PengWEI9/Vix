/*
 * MasterRoutes.cpp
 *
 *  Created on: 14/07/2014
 *      Author: Morgan Dell
 */

#include "MasterRoutes.h"
#include "pugixml.hpp"
#include "CdCache.h"
#include <sstream>

void MasterRouteFree(MYKI_CD_MasterRoute_t& masterRoute)
{
	delete[] masterRoute.locations;
}
CdCache<U32_t, MYKI_CD_MasterRoute_t> g_masterRouteCache("Master route", 30, CacheMemoryCopy<MYKI_CD_MasterRoute_t>, MasterRouteFree);

int initMasterRoutesLookup()
{
	g_masterRouteCache.clear();
	return 1;
}

int processMasterRoutesSubtableXML(pugi::xpath_node node, int recordPK, std::vector<int>& subtablePKs)
{
	DbAccess* dbAccess = getDBAccess();
	int ret = 0;
    
    if(dbAccess==NULL)
        return 2;  
	// Set up the primary key
	if ( subtablePKs.empty() )
	{
		Rows rows;
		dbAccess->executeSqlQuery("SELECT MAX(PrimaryKey) AS PrimaryKey FROM MasterRouteLocation", rows);
		if ( !rows.empty() )
			subtablePKs.push_back(convertStringToU32(rows.front()["PrimaryKey"]));
		else
			subtablePKs.push_back(0);
	}
	int& locationPK = subtablePKs[0];

	// Get the Attribute Element Child belonging to each Record
	for ( pugi::xml_node tool = node.node().first_child(); tool; tool = tool.next_sibling() )
	{
		std::string childName(tool.name());

		if ( childName == "Location" )
		{
			locationPK++;
			std::stringstream query;
			query
				<< "INSERT INTO MasterRouteLocation("
				<< "'PrimaryKey',"
				<< "'location',"
				<< "'fk_MasterRouteRecord'"
				<< ") VALUES ("
				<< locationPK << ","
				<< sqlify(tool.text().get()) << ","
				<< recordPK << ")";
		    if ( dbAccess->executeSqlInsert(query.str()) != 0 )
			{
				CsErrx("Query '%s' failed", query.str().c_str());
				ret = 2;
			}
		}
	}

	return ret;
}

int processMasterRoutesXML()
{
	int ret = 0;
	const char* subtableDeleteQuery =
		"DELETE FROM MasterRouteLocation WHERE "
		"NOT FK_MasterRouteRecord IN (SELECT PrimaryKey FROM MasterRouteRecord);";
	std::set<std::string> excludedAttributes;
	excludedAttributes.insert("Location");

	ret = processXML(
		"processMasterRoutesXML", // description
		"MasterRoutes.xml",       // xml file
		"MasterRoutes",           // xml node
		"MasterRoutes",           // master table
		"MasterRouteRecord",      // record table
		processMasterRoutesSubtableXML,
		subtableDeleteQuery,
		&excludedAttributes);

	initMasterRoutesLookup();
	return ret;
}

int getMasterRoute(U16_t masterRouteId, U16_t serviceProviderId, MYKI_CD_MasterRoute_t& masterRoute)
{
	int returnValue = false;

	CsDebug(CD_DEBUG, (CD_DEBUG, "MYKI_CD:getMasterRoute %d,%d", masterRouteId, serviceProviderId));

	bool isNull = false;
	U32_t cacheKey = (U32_t(masterRouteId) << 16) + serviceProviderId;
	if ( g_masterRouteCache.getCacheValue(cacheKey, masterRoute, isNull) )
	{
		returnValue = !isNull;
		CsDebug(CD_DEBUG, (CD_DEBUG, "MYKI_CD:getMasterRoute: Return Cache MasterRoute record found for id %d,%d", int(masterRouteId), int(serviceProviderId)));
	}
	else
	{    
        DbAccess* dbAccess =  getDBAccess();
        if(dbAccess==NULL)
            return false;          
		Rows rows;

		char query[300];
		sprintf(query, "SELECT * FROM MasterRouteRecord WHERE id=%d AND service_provider=%d AND %s", int(masterRouteId), int(serviceProviderId), getCurrentCDVersionWhere());

		dbAccess->executeSqlQuery(query, rows);

		U32_t primaryKey;
		Rows::const_iterator itr1;
		for ( itr1 = rows.begin(); itr1 != rows.end(); ++itr1 )
		{
			const ColumnValue& cv = *itr1;
			stringCopy(masterRoute.code, cv["code"], sizeof(masterRoute.code));
			stringCopy(masterRoute.short_desc, cv["short_desc"], sizeof(masterRoute.short_desc));
			stringCopy(masterRoute.long_desc, cv["long_desc"], sizeof(masterRoute.long_desc));
			primaryKey = convertStringToU32(cv["PrimaryKey"]);
        }
		masterRoute.locationCount = 0;
		masterRoute.locations = NULL;

		if ( rows.empty() )
		{
			CsDebug(CD_DEBUG, (CD_DEBUG, "MYKI_CD:getMasterRoute %d,%d - no master route exists", int(masterRouteId), int(serviceProviderId)));
		}
		else
		{
			char subQuery[300];
			sprintf(subQuery, "SELECT * FROM MasterRouteLocation WHERE fk_MasterRouteRecord=%u ORDER BY location", (unsigned int)(primaryKey));
			Rows subRows;
			dbAccess->executeSqlQuery(subQuery, subRows);

			if ( subRows.empty() )
			{
				CsDebug(CD_DEBUG, (CD_DEBUG, "MYKI_CD:getMasterRoute %d,%d - no locations exist", int(masterRouteId), int(serviceProviderId)));
			}
			else
			{
				masterRoute.locationCount = subRows.size();
				masterRoute.locations = new U16_t[masterRoute.locationCount];
				unsigned int i;
				for ( i = 0, itr1 = subRows.begin(); itr1 != subRows.end(); ++itr1, ++i )
				{
					const ColumnValue& cv = *itr1;
					masterRoute.locations[i] = convertStringToU16(cv["location"]);
        		}
			}

			returnValue = true;
		}
		g_masterRouteCache.addCacheValue(cacheKey, masterRoute, !returnValue);
	}

	return returnValue;
}

