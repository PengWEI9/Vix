/*
 * Actionlist.cpp
 *
 *  Created on: 08/10/2012
 *      Author: James Ho
 */

#include "DeviceHotlist.h"
#include "pugixml.hpp"
#include "DbAccess.h"
#include "myki_actionlist.h"
#include "ProductConfigurationUtils.h"
#include "datec19.h"
#include <algorithm>
#include <sstream>
#include <map>

std::map<U32_t, DateC19_t> g_deviceHotlistCache;
bool g_deviceHotlistCacheRetrieved = false;
int g_cachedDeviceHotlistSequence = -1;
std::string g_cachedDeviceHotlistSequenceInfo;

int initDeviceHotlistLookup()
{
	g_deviceHotlistCache.clear();
	g_deviceHotlistCacheRetrieved = false;
	g_cachedDeviceHotlistSequence = -1;
	g_cachedDeviceHotlistSequenceInfo = "";
	return 1;
}

/*
 * Process and imports an device hotlist file into the actionlist database.
 */
int processDeviceHotlistXML()
{
	DbAccess* dbAccess = getActionlistDBAccess();

	pugi::xml_document doc;
	pugi::xml_parse_result result = doc.load_file("DeviceHotlist.xml");

	int ret = MYKI_ACTIONLIST_SUCCESS;
	if ( dbAccess == 0 )
	{
		CsErrx("processDeviceHotlistXML: Could not get database");
		ret = MYKI_ACTIONLIST_FAILED;
	}
	else if ( !result )
	{
		// Only a warning - alternate version of success
		CsWarnx("processDeviceHotlistXML: Could not open 'DeviceHotlist.xml', no device hotlists updated");
	}
	else
	{
		int deviceHotlistPK = 0;
		int deviceHotlistEntryPK = 0;

		// Get the Header
		pugi::xpath_node_set tools = doc.select_nodes("/ns0:HotlistDevice");
		pugi::xpath_node_set::const_iterator it;
		for ( it = tools.begin(); it != tools.end(); ++it )
		{
			pugi::xpath_node node = *it;
			std::ostringstream s_queryHeader;
			std::ostringstream s_queryColumn;
			std::ostringstream s_queryValue;
			bool isDelta = false;

			// Get the attributes belonging to the Header
			for ( pugi::xml_attribute attr = node.node().first_attribute(); attr; attr = attr.next_attribute() )
			{
				if ( attr != node.node().first_attribute() )
				{
					s_queryColumn << ",";
					s_queryValue << ",";
				}

				std::string str(attr.name());
				if ( str.compare(0,6,"xmlns:") == 0 )
				{
					str.replace(0,6,"");
				}
				else if ( str.compare(0,4,"xsi:") == 0 )
				{
					str.replace(0,4,"");
				}
				s_queryColumn << "'" << str << "'";

				std::string value(attr.value());
				std::transform(str.begin(), str.end(), str.begin(), toupper);
				if ( (str == "ISDELTA") || (str == "IS_DELTA") )
				{
					std::transform(value.begin(), value.end(), value.begin(), toupper);
					if ( (value == "TRUE") || (value == "1") )
					{
						isDelta = true;
						value = "1";
					}
					else
					{
						value = "0";
					}
				}
				s_queryValue << sqlify(value);
			}

			CsVerbose("processDeviceHotlistXML: file is %sa delta", (isDelta ? "" : "not "));
			if ( !isDelta )
			{
				std::string s_query = "DELETE FROM 'DeviceHotlist'";
				dbAccess->executeSqlDelete(s_query);

				s_query = "DELETE FROM 'DeviceHotlistEntry'";
				dbAccess->executeSqlDelete(s_query);
			}
			else
			{
				std::string s_primaryKey;
				if ( dbAccess->executeSqlQuerySingleItem(
						"SELECT MAX(PrimaryKey) AS PrimaryKey FROM 'DeviceHotlist'",
						s_primaryKey) == SQLITE_OK )
				{
					if ( !s_primaryKey.empty() )
						deviceHotlistPK = convertStringToU32(s_primaryKey);
				}

				if ( dbAccess->executeSqlQuerySingleItem(
						"SELECT MAX(PrimaryKey) AS PrimaryKey FROM 'DeviceHotlistEntry'",
						s_primaryKey) == SQLITE_OK )
				{
					if ( !s_primaryKey.empty() )
						deviceHotlistEntryPK = convertStringToU32(s_primaryKey);
				}
			}

			deviceHotlistPK++;
			s_queryHeader
				<< "INSERT INTO DeviceHotlist('PrimaryKey',"
				<< s_queryColumn.str()
				<< ") VALUES ('" << deviceHotlistPK << "',"
				<< s_queryValue.str()
				<< ")";

			CsVerbose(":SQLINSERT: %s;", s_queryHeader.str().c_str());
		    if ( dbAccess->executeSqlInsert(s_queryHeader.str()) != SQLITE_OK )
			{
				CsErrx("Query '%s' failed", s_queryHeader.str().c_str());
				ret = MYKI_ACTIONLIST_FAILED;
			}
		}

		// Get the device list
		pugi::xpath_node_set requestTools = doc.select_nodes("/ns0:HotlistDevice/Device");
		for ( it = requestTools.begin(); it != requestTools.end(); ++it )
		{
			deviceHotlistEntryPK++;
		    pugi::xpath_node node = *it;

			std::ostringstream s_queryRecord;
			std::ostringstream s_queryColumn;
			std::ostringstream s_queryValue;

			// Get the attributes belonging to the Record
		    for ( pugi::xml_attribute attr = node.node().first_attribute(); attr; attr = attr.next_attribute() )
		    {
		    	if ( attr != node.node().first_attribute() )
		    	{
		    		s_queryColumn << ",";
		    		s_queryValue << ",";
		    	}
				std::string name = attr.name();
				std::string value = attr.value();
				if ( name == "samid" )
				{
					U32_t samId = 0;
					sscanf(value.c_str(), "%u", &samId);
					samId =
						((samId & 0xFF000000) >> 24) +
						((samId & 0x00FF0000) >> 8) +
						((samId & 0x0000FF00) << 8) +
						((samId & 0x000000FF) << 24);
					s_queryValue << samId;
				}
				else
				{
					s_queryValue << sqlify(attr.value());
				}
				s_queryColumn << "'" << name << "'";
		    }
			s_queryRecord
				<< "INSERT INTO DeviceHotlistEntry('PrimaryKey',"
				<< s_queryColumn.str()
				<< ",'FK_DeviceHotlist') VALUES ('" << deviceHotlistEntryPK << "',"
				<< s_queryValue.str()
				<< ",'" << deviceHotlistPK << "')";

			CsVerbose(":SQLINSERT: %s;", s_queryRecord.str().c_str());
		    if ( dbAccess->executeSqlInsert(s_queryRecord.str()) != SQLITE_OK )
			{
				CsErrx("Query '%s' failed", s_queryRecord.str().c_str());
				ret = MYKI_ACTIONLIST_FAILED;
			}
		}

		dbAccess->endDbTransaction();
	}

	initDeviceHotlistLookup();
	return ret;
}

int getDeviceHotlistSequence()
{
	if ( g_cachedDeviceHotlistSequence == -1 )
	{
		DbAccess* dbAccess =  getActionlistDBAccess();

		const char* query = "SELECT MAX(SequenceNumber) AS sequence FROM DeviceHotlist";
		Rows rows;
		if ( dbAccess->executeSqlQuery(query, rows) != SQLITE_OK )
		{
			CsWarnx("MYKI_ACTIONLIST:getDeviceHotlistSequence: select failed");
		}
		else if ( rows.empty() )
		{
			CsWarnx("MYKI_ACTIONLIST:getDeviceHotlistSequence: no rows returned");
		}
		else
		{
			g_cachedDeviceHotlistSequence = atoi((*rows.begin())["sequence"]);
		}
	}
	return g_cachedDeviceHotlistSequence;
}

const char* getDeviceHotlistSequenceInfo()
{
	if ( g_cachedDeviceHotlistSequenceInfo.empty() )
	{
		DbAccess* dbAccess =  getActionlistDBAccess();

		const char* query = "SELECT SequenceNumber FROM DeviceHotlist ORDER BY SequenceNumber ASC";
		Rows rows;
		if ( dbAccess->executeSqlQuery(query, rows) != SQLITE_OK )
		{
			CsWarnx("MYKI_ACTIONLIST:getDeviceHotlistSequenceInfo: select failed");
		}
		else if ( rows.empty() )
		{
			CsWarnx("MYKI_ACTIONLIST:getDeviceHotlistSequenceInfo: no rows returned");
		}
		else
		{
			std::string result;
			int sequenceLow = -1;
			int sequenceHigh = -1;
			bool first = true;
			for ( Rows::const_iterator itr = rows.begin(); itr != rows.end(); ++itr )
			{
				int sequence = atoi((*itr)["SequenceNumber"]);
				if ( first )
				{
					first = false;
					sequenceLow = sequence;
					sequenceHigh = sequence;
				}
				else if ( sequenceHigh + 1 == sequence )
				{
					sequenceHigh = sequence;
				}
				else
				{
					char buf[32];
					sprintf(buf, "%s%d", (result.empty() ? "" : ","), sequenceLow);
					result += buf;
					if ( sequenceHigh >= sequenceLow + 1 )
					{
						sprintf(buf, "%s%d", (sequenceHigh == sequenceLow + 1 ? "," : ".."), sequenceHigh);
						result += buf;
					}
					sequenceLow = sequence;
					sequenceHigh = sequence;
				}
			}

			char buf[32];
			sprintf(buf, "%s%d", (result.empty() ? "" : ","), sequenceLow);
			result += buf;
			if ( sequenceHigh >= sequenceLow + 1 )
			{
				sprintf(buf, "%s%d", (sequenceHigh == sequenceLow + 1 ? "," : ".."), sequenceHigh);
				result += buf;
			}

			g_cachedDeviceHotlistSequenceInfo = result;
		}
	}
	return g_cachedDeviceHotlistSequenceInfo.c_str();
}

/*
 * Checks if the device is in the hotlist database.
 */
int isDeviceHotlisted(U32_t samId, DateC19_t requestDate)
{
	int returnValue = 0;
	if ( !g_deviceHotlistCacheRetrieved )
	{
		DbAccess* dbAccess = getActionlistDBAccess();

		if ( dbAccess == 0 )
		{
			CsErrx("MYKI_CD:isDeviceHotlisted - could not get database");
		}
		else
		{
			Rows rows;
			dbAccess->executeSqlQuery("SELECT SamId, MIN(effective_date) AS effective_date FROM DeviceHotlistEntry GROUP BY SamId", rows);
			for ( Rows::const_iterator row = rows.begin(); row != rows.end(); row++ )
			{
				const ColumnValue& cv = *row;
				U32_t dbSamId = convertStringToU32(cv["SamId"]);
				struct tm tmBuf;
				memset(&tmBuf, 0, sizeof(tmBuf));
				sscanf(cv["effective_date"], "%d-%d-%d", &tmBuf.tm_year, &tmBuf.tm_mon, &tmBuf.tm_mday);
				tmBuf.tm_year -= 1800;
				tmBuf.tm_mon -= 1;
				g_deviceHotlistCache[dbSamId] = mkdate(&tmBuf);
			}
			g_deviceHotlistCacheRetrieved = true;
		}
	}

	std::map<U32_t, DateC19_t>::const_iterator it = g_deviceHotlistCache.find(samId);
	if ( (it != g_deviceHotlistCache.end()) && (it->second <= requestDate) )
	{
		returnValue = 1;
	}

	return returnValue;
}

