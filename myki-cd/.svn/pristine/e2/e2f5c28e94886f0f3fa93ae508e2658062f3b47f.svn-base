/*
 * Locations.cpp
 *
 *  Created on: 08/10/2012
 *      Author: James Ho
 */

#include "Locations.h"
#include "CdCache.h"

CdCache<U16_t, MYKI_CD_Locations_t> g_locationCache("Location", 1000, CacheMemoryCopy<MYKI_CD_Locations_t>);


int initLocationLookup()
{
	g_locationCache.clear();
    return 1;
}

int processLocationsXML()
{
	int ret = processXML(
		"processLocationsXML", // description
		"Locations.xml",       // xml file
		"Locations",           // xml node
		"Locations",           // master table
		"LocationRecord");     // record table
	initLocationLookup();
	return ret;
}

int getLocations(U16_t id, MYKI_CD_Locations_t &locations)
{
    
    int returnValue=false;
    bool isNull=false;
    

    if(g_locationCache.getCacheValue(id, locations, isNull))
    {
        returnValue = !isNull;
		CsDebug(CD_DEBUG, (CD_DEBUG, "MYKI_CD:getLocations: Return Cache Location record found for id %d.", id));
    }
    else
    {
        DbAccess 	*m_dbAccess = getDBAccess();
        if(m_dbAccess==NULL)
            return false;          
        Rows rows;

        char s_query[200];
        sprintf(s_query, "SELECT * FROM LocationRecord WHERE id='%u' AND %s", (unsigned int)(id), getCurrentCDVersionWhere());

        m_dbAccess->executeSqlQuery(s_query, rows);

        Rows::const_iterator itr1;
        for(itr1 = rows.begin(); itr1 != rows.end(); ++itr1){
            const ColumnValue& cv = *itr1;

            locations.id = convertStringToU16(cv["id"]);
            locations.zone = convertStringToU8(cv["zone"]);
            locations.inner_zone = convertStringToU8(cv["inner_zone"]);
            locations.outer_zone = convertStringToU8(cv["outer_zone"]);
            stringCopy(locations.short_desc, cv["short_desc"], LEN_Short_Description_t);
            stringCopy(locations.long_desc, cv["long_desc"], LEN_Long_Description_t);

            // Pass gps fields as char for now as it's not being used. Should be float values.
            stringCopy(locations.gps_lat, cv["gps_lat"], LEN_Gps_t);
            stringCopy(locations.gps_long, cv["gps_long"], LEN_Gps_t);
            stringCopy(locations.gps_lat_tolerance, cv["gps_lat_tolerance"], LEN_Gps_t);
            stringCopy(locations.gps_long_tolerance, cv["gps_long_tolerance"], LEN_Gps_t);
        }

        // TEST: Print out results
        if(rows.empty())
        {
            //std::cout << "\nNo Location record found for id " << id <<".\n";
               CsDebug(CD_DEBUG, (CD_DEBUG, "MYKI_CD:getLocations: No Location record found for id %d.", id));
            returnValue =false;
        }
        else
        {
            returnValue =true;
        }
        g_locationCache.addCacheValue(id, locations, !returnValue);
    }

    return returnValue;
}

