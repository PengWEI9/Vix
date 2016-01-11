/*
 * RouteStops.cpp
 *
 *  Created on: 26/06/2014
 *      Author: Morgan Dell
 */

#include "RouteStops.h"
#include "CdCache.h"
#include "DbAccess.h"
#include "ProductConfigurationUtils.h"

typedef struct
{
	MYKI_CD_RouteStop_t* routeStops;
	int routeStopsLength;
} RouteStopsItem;
void RouteStopsFree(RouteStopsItem& item)
{
	free(item.routeStops);
}
CdCache<U16_t, RouteStopsItem> g_routeStopsCache("Route stops", 100, CacheMemoryCopy<RouteStopsItem>, RouteStopsFree);


int initRouteStopsLookup()
{
	g_routeStopsCache.clear();
    return 1;
}

int processRouteStopsXML()
{
	int ret = processXML(
		"processRouteStopsXML", // description
		"RouteStops.xml",       // xml file
		"RouteStops",           // xml node
		"RouteStops",           // master table
		"RouteStopRecord");     // record table
	initRouteStopsLookup();
	return ret;
}

int getRouteStops(U16_t routeId, U8_t fromStopId, U8_t toStopId, MYKI_CD_RouteStop_t** pRouteStops)
{
	int returnValue = 0;
	bool isNull = false;

	*pRouteStops = NULL;
	if ( toStopId < fromStopId )
	{
		U8_t temp  = toStopId;
		toStopId   = fromStopId;
		fromStopId = temp;
	}

	// For this query, first get an array with all the stops belonging to the route
	// and return in the pointer only a subset of that array.  Doing it like this
	// means the cache is far more effective so the function runs quicker.
	RouteStopsItem cacheItem;
	if ( g_routeStopsCache.getCacheValue(routeId, cacheItem, isNull))
	{
		if ( !isNull )
		{
			if ( (cacheItem.routeStops != NULL) && (cacheItem.routeStopsLength > 0) )
			{
				// Now we have all the stops, return only the ones the caller wants
				// In almost all cases, the array will be monotonically increasing
				// from stop 1 to N so check for that as it is fast. If not, look
				// through the list.
				if (
					cacheItem.routeStops[cacheItem.routeStopsLength-1].stopId ==
					cacheItem.routeStops[0].stopId + cacheItem.routeStopsLength - 1
				   )
				{
					int startIndex = int(fromStopId) - int(cacheItem.routeStops[0].stopId);
					returnValue = int(toStopId) - int(fromStopId) + 1;
					if ( startIndex < 0 )
					{
						returnValue += startIndex;
						startIndex = 0;
					}
					if ( startIndex + returnValue > cacheItem.routeStopsLength )
					{
						returnValue = cacheItem.routeStopsLength - startIndex;
					}
					if ( returnValue > 0 )
					{
						*pRouteStops = cacheItem.routeStops + startIndex;
					}
					else
					{
						returnValue = 0;
					}
				}
				else
				{
					// This could be done more efficiently with a binary search, but that
					// makes the code hard to read and this should never be called as
					// the data always has stops 1 .. N
					for ( int i = 0; i < cacheItem.routeStopsLength; ++i )
					{
						U8_t thisStopId = cacheItem.routeStops[i].stopId;
						// Can do this because the stops are ordered by id
						if ( (fromStopId <= thisStopId) && (thisStopId <= toStopId) )
						{
							if ( *pRouteStops == NULL )
							{
								*pRouteStops = cacheItem.routeStops + i;
								returnValue  = 1;
							}
							else
							{
								++returnValue;
							}
						}
						else if ( thisStopId > toStopId )
						{
							break;
						}
					}
				}
			} // cacheItem.routeStops != NULL
		} // !isNull
		CsDebug(CD_DEBUG, (CD_DEBUG, "MYKI_CD:getRouteStops: Return Cache RouteStops record found for route %u, stops %u..%u", routeId, fromStopId, toStopId));
	}
	else
	{
		DbAccess* dbAccess = getDBAccess();
        if(dbAccess==NULL)
            return 0;        
		Rows rows;

		char s_query[300];
		sprintf(
			s_query, "SELECT * FROM RouteStopRecord WHERE route=%u AND %s ORDER BY id ASC",
			(unsigned int)(routeId),
			getCurrentCDVersionWhere());

		dbAccess->executeSqlQuery(s_query, rows);

		if ( rows.empty() )
		{
			cacheItem.routeStops = NULL;
			cacheItem.routeStopsLength = 0;
		}
		else
		{
			cacheItem.routeStopsLength = rows.size();
			cacheItem.routeStops = reinterpret_cast<MYKI_CD_RouteStop_t*>(malloc(sizeof(MYKI_CD_RouteStop_t) * cacheItem.routeStopsLength));
			unsigned int index = 0;
			Rows::const_iterator row;
			for ( row = rows.begin(); row != rows.end(); ++row, ++index )
			{
				const ColumnValue& cv = *row;

				U8_t thisStopId = convertStringToU8(cv["id"]);
				cacheItem.routeStops[index].stopId = thisStopId;
				cacheItem.routeStops[index].locationId = convertStringToU16(cv["location"]);
				cacheItem.routeStops[index].provisionalZoneLow = convertStringToU8(cv["provisional_zone_low"]);
				cacheItem.routeStops[index].provisionalZoneHigh = convertStringToU8(cv["provisional_zone_high"]);
				cacheItem.routeStops[index].gpsBearing = convertStringToU8(cv["gps_bearing"]);

				// Can do this because the stops are ordered by id
				if ( (fromStopId <= thisStopId) && (thisStopId <= toStopId) )
				{
					if ( *pRouteStops == NULL )
					{
						*pRouteStops = cacheItem.routeStops + index;
						returnValue  = 1;
					}
					else
					{
						++returnValue;
					}
				}
			} // for loop over rows
		} // !rows.empty()

		g_routeStopsCache.addCacheValue(routeId, cacheItem, false);
    }

    return returnValue;
}
