/*
 * DifferentialPrices.cpp
 *
 *  Created on: 08/10/2013
 *      Author: Morgan Dell
 */

#include "DifferentialPrices.h"
#include "ProductConfigurationUtils.h"
#include "CdCache.h"

CdCache<std::string, MYKI_CD_DifferentialPricing_t> g_differentialPricesCache(
	"DifferentialPrices", 300, CacheMemoryCopy<MYKI_CD_DifferentialPricing_t>);

int initDifferentialPricesLookup()
{
	g_differentialPricesCache.clear();
	return 1;
}

int processDifferentialPricesSubtableXML(pugi::xpath_node node, int recordPK, std::vector<int>& subtablePKs)
{
	DbAccess* dbAccess = getDBAccess();
	int ret = 0;
    if(dbAccess==NULL)
        return 2; 
	// Get the Attribute Element Child belonging to each Device Parameter Record
	bool needPrimaryKeys = false;
	if (subtablePKs.empty())
	{
		needPrimaryKeys = true;
		subtablePKs.push_back(0);
	}
	int& routePK = subtablePKs[0];
	if (needPrimaryKeys)
	{
		Rows rows;
		dbAccess->executeSqlQuery("SELECT MAX(PrimaryKey) AS PrimaryKey FROM DifferentialPriceRoute", rows);
		if (!rows.empty())
			routePK = convertStringToU32(rows.front()["PrimaryKey"]);
	}

	for (pugi::xml_node tool = node.node().first_child(); tool; tool = tool.next_sibling())
	{
		std::string s_queryRoute = "";
		std::string s_queryColumn = "";
		std::string s_queryValue = "";
		std::string s_updateRecord;

		int attributeCount = 0;
		for (pugi::xml_attribute attr = tool.first_attribute(); attr; attr = attr.next_attribute())
		{
			if(attributeCount > 0)
			{
				s_queryColumn += ",";
				s_queryValue += ",";
			}
			s_queryColumn.append("'");
			s_queryColumn += attr.name();
			s_queryColumn.append("'");
			s_queryValue += sqlify(attr.value());
			attributeCount++;
		}
		if (attributeCount > 0)
		{
			routePK++;
			s_queryRoute = "INSERT INTO DifferentialPriceRoute('PrimaryKey',";
			s_queryRoute += s_queryColumn;
			s_queryRoute += ",'FK_DifferentialPriceRecord') VALUES ('";
			s_queryRoute += static_cast<std::ostringstream*>( &(std::ostringstream() << routePK) )->str(); // PK for Differential Price Route
			s_queryRoute += "',";
			s_queryRoute += s_queryValue;
			s_queryRoute += ",'";
			s_queryRoute += static_cast<std::ostringstream*>( &(std::ostringstream() << recordPK) )->str(); // FK to Differential Price Record
			s_queryRoute += "'";
			s_queryRoute += ")";

			//std::cout << "\n\n" << s_queryRoute;
			if ( dbAccess->executeSqlInsert(s_queryRoute) != 0 )
			{
				CsErrx("Query '%s' failed", s_queryRoute.c_str());
				ret = 2;
			}

			s_updateRecord = "UPDATE DifferentialPriceRecord ";
			s_updateRecord += "SET HasRoutes = 1 ";
			s_updateRecord += "WHERE PrimaryKey = '";
			s_updateRecord += static_cast<std::ostringstream*>( &(std::ostringstream() << recordPK) )->str();
			s_updateRecord += "'";
			if ( dbAccess->executeSqlUpdate(s_updateRecord) != 0 )
			{
				CsErrx("Query '%s' failed", s_updateRecord.c_str());
				ret = 2;
			}
		}
	}

	return ret;
}

int processDifferentialPricesXML()
{
	DbAccess* dbAccess = getDBAccess();

	int ret = 0;
	if(dbAccess == 0)
	{
		CsErrx("processDeviceParametersXML: Could not get database");
		ret = 1;
	}
	else
	{
		const char* s_subtableDeleteQuery =
			"DELETE FROM DifferentialPriceRoute WHERE "
			"NOT FK_DifferentialPriceRecord IN (SELECT PrimaryKey FROM DifferentialPriceRecord)";

		ret = processXML(
			"processDifferentialPricesXML", // description
			"DifferentialPrices.xml",       // xml file
			"DifferentialPrices",           // xml node
			"DifferentialPrices",           // master table
			"DifferentialPriceRecord",      // record table
			processDifferentialPricesSubtableXML,
			s_subtableDeleteQuery);

		if (ret == 0)
		{
			// Change the format of DateLow and DateHigh in the database
			// from DD/MM to MM-DD - better for comparisons.
			std::string s_query =
				"UPDATE DifferentialPriceRecord "
				"SET DateLow = substr(DateLow, 4, 2) || '-' || substr(DateLow, 1, 2) "
				"WHERE DateLow LIKE '__/__'";
			dbAccess->executeSqlUpdate(s_query);

			s_query =
				"UPDATE DifferentialPriceRecord "
				"SET DateHigh = substr(DateHigh, 4, 2) || '-' || substr(DateHigh, 1, 2) "
				"WHERE DateHigh LIKE '__/__'";
			dbAccess->executeSqlUpdate(s_query);
		}
	}

	initDifferentialPricesLookup();
	return ret;
}

int getDifferentialPrice(const MYKI_CD_DifferentialPricingRequest_t* input, MYKI_CD_DifferentialPricing_t& differentialPricing)
{
	int returnValue = false;
	DbAccess* dbAccess = getDBAccess();
	if ( dbAccess == 0 )
	{
		CsErrx("getDifferentialPrice: no database");
	}
	else
	{
		Rows rows;
		char query[2048];
		int queryLen = 0;
		queryLen += sprintf(query + queryLen, "SELECT * FROM DifferentialPriceRecord");

		queryLen += sprintf(query + queryLen, " WHERE (ZoneLow IS NULL OR ZoneLow <= %d)", int(input->zone_low));
		queryLen += sprintf(query + queryLen, " AND (ZoneHigh IS NULL OR ZoneHigh >= %d)", int(input->zone_high));
		queryLen += sprintf(query + queryLen, " AND (ZoneCountLow IS NULL OR ZoneCountLow <= %d)", int(input->zone_high) - int(input->zone_low) + 1);
		queryLen += sprintf(query + queryLen, " AND (ZoneCountHigh IS NULL OR ZoneCountHigh >= %d)", int(input->zone_high) - int(input->zone_low) + 1);

		if ( input->rule_type == MYKI_CD_DIFFERENTIAL_PRICING_RULE_TYPE_STANDARD )
			queryLen += sprintf(query + queryLen, " AND (RuleType IS NULL OR RuleType='STANDARD')");
		else if ( input->rule_type == MYKI_CD_DIFFERENTIAL_PRICING_RULE_TYPE_OFFPEAK )
			queryLen += sprintf(query + queryLen, " AND (RuleType IS NULL OR RuleType='OFFPEAK')");
        else
        {
            CsErrx("getDifferentialPrice: no database");
            return false;
        }

		if ( !input->passenger_type_null )
			queryLen += sprintf(query + queryLen, " AND (PassengerType IS NULL OR PassengerType=%d)", int(input->passenger_type));

        queryLen += sprintf(query + queryLen, " AND (" );
        queryLen += sprintf(query + queryLen, "    (" );
        queryLen += sprintf(query + queryLen, "           (TimeOfDayLow IS NULL OR TimeOfDayLow <= %d)", int(input->scanon_minutes_since_midnight));
        queryLen += sprintf(query + queryLen, "       AND (TimeOfDayHigh IS NULL OR TimeOfDayHigh >= %d)", int(input->scanon_minutes_since_midnight));
        queryLen += sprintf(query + queryLen, "       AND (TimeOfDayComparison IS NULL OR TimeOfDayComparison <> 'SCANOFF')");
        queryLen += sprintf(query + queryLen, "    ) OR (");
        queryLen += sprintf(query + queryLen, "           (TimeOfDayLow IS NULL OR TimeOfDayLow <= %d)", int(input->scanoff_minutes_since_midnight));
        queryLen += sprintf(query + queryLen, "       AND (TimeOfDayHigh IS NULL OR TimeOfDayHigh >= %d)", int(input->scanoff_minutes_since_midnight));
        queryLen += sprintf(query + queryLen, "       AND (TimeOfDayComparison IS NULL OR TimeOfDayComparison <> 'SCANON')");
        queryLen += sprintf(query + queryLen, "    )");
        queryLen += sprintf(query + queryLen, " )");
/*
		if ( input->scanon_minutes_since_midnight >= 0 )
		{
			queryLen += sprintf(query + queryLen, " AND (TimeOfDayLow IS NULL OR TimeOfDayLow <= %d)", int(input->scanon_minutes_since_midnight));
			queryLen += sprintf(query + queryLen, " AND (TimeOfDayHigh IS NULL OR TimeOfDayHigh >= %d)", int(input->scanon_minutes_since_midnight));
			queryLen += sprintf(query + queryLen, " AND (TimeOfDayComparison IS NULL OR TimeOfDayComparison <> 'SCANOFF')");
		}
		if ( input->scanoff_minutes_since_midnight >= 0 )
		{
			queryLen += sprintf(query + queryLen, " AND (TimeOfDayLow IS NULL OR TimeOfDayLow <= %d)", int(input->scanoff_minutes_since_midnight));
			queryLen += sprintf(query + queryLen, " AND (TimeOfDayHigh IS NULL OR TimeOfDayHigh >= %d)", int(input->scanoff_minutes_since_midnight));
			queryLen += sprintf(query + queryLen, " AND (TimeOfDayComparison IS NULL OR TimeOfDayComparison <> 'SCANON')");
		}
*/
		if ( input->day_of_week != 0 )
		{
			queryLen += sprintf(query + queryLen, " AND (DayOfWeekLow IS NULL OR DayOfWeekLow <= %d)", int(input->day_of_week));
			queryLen += sprintf(query + queryLen, " AND (DayOfWeekHigh IS NULL OR DayOfWeekHigh >= %d)", int(input->day_of_week));
		}

		if ( (input->day_of_month != 0) && (input->month != 0) )
		{
			queryLen += sprintf(query + queryLen, " AND (DateLow IS NULL OR '%02d-%02d' >= DateLow)", int(input->month), int(input->day_of_month));
			queryLen += sprintf(query + queryLen, " AND (DateHigh IS NULL OR '%02d-%02d' <= DateHigh)", int(input->month), int(input->day_of_month));
		}

		if ( input->trip_direction == MYKI_CD_TRIP_DIRECTION_INBOUND )
			queryLen += sprintf(query + queryLen, " AND (TripDirection IS NULL OR TripDirection = 'INBOUND')");
		else if ( input->trip_direction == MYKI_CD_TRIP_DIRECTION_OUTBOUND )
			queryLen += sprintf(query + queryLen, " AND (TripDirection IS NULL OR TripDirection = 'OUTBOUND')");

		queryLen += sprintf(query + queryLen, " AND %s ORDER BY priority ASC", getCurrentCDVersionWhere());

		bool isNull = false;
		bool addToCache = false;
		if ( g_differentialPricesCache.getCacheValue(query, differentialPricing, isNull) )
		{
			returnValue = !isNull;
            CsDebug(CD_DEBUG, (CD_DEBUG, "MYKI_CD:getDifferentialPrice: Return Cache record found"));
		}
		else
		{
			addToCache = true;
			dbAccess->executeSqlQuery(query, rows);

			if(rows.empty())
			{
				CsDebug(CD_DEBUG, (CD_DEBUG, "MYKI_CD:getDifferentialPrice: No Differential Price record found"));
			}
			else
			{
				for ( Rows::iterator it = rows.begin(); (returnValue == false) && (it != rows.end()); it++ )
				{
					const ColumnValue& cv = *it;
					if ( strcmp(cv["HasRoutes"], "1") == 0 )
					{
						addToCache = false;
						if ( input->route_count <= 0 )
						{
							continue;
						}
						else
						{
							Rows rows2;
							std::ostringstream s_query2;
							s_query2
								<< "SELECT count(1) AS count "
								<< "FROM DifferentialPriceRoute "
								<< "WHERE FK_DifferentialPriceRecord = '" << cv["PrimaryKey"] << "' "
								<< "AND service_provider || ',' || route IN (";
							for ( unsigned int i = 0; i < input->route_count; i++ )
							{
								if ( i > 0 )
									s_query2 << ", ";
								s_query2 << "'" << input->routes[i].service_provider_id << "," << input->routes[i].route_id << "'";
							}
							s_query2 << ")";
							dbAccess->executeSqlQuery(s_query2.str().c_str(), rows2);
							if ( (rows2.size() != 1) || (convertStringToU32(rows2.front()["count"]) != input->route_count) )
								continue;
						}
					}

					returnValue = true;
					differentialPricing.id = convertStringToU32(cv["id"]);
					stringCopy(differentialPricing.short_desc, cv["description"], sizeof(differentialPricing.short_desc));
					if (strcmp(cv["DiscountType"], "STATIC") == 0)
						differentialPricing.discount_type = MYKI_CD_DISCOUNT_TYPE_STATIC;
					else
						differentialPricing.discount_type = MYKI_CD_DISCOUNT_TYPE_PERCENT;
					differentialPricing.base_discount = convertStringToU32(cv["BaseDiscount"]);
					differentialPricing.applied_discount = convertStringToU32(cv["AppliedDiscount"]);
					differentialPricing.entitlement_issuer = convertStringToU8(cv["EntitlementIssuer"]);
					differentialPricing.entitlement_product = convertStringToU8(cv["EntitlementProduct"]);
					differentialPricing.entitlement_zone_low = convertStringToU8(cv["EntitlementZoneLow"]);
					differentialPricing.entitlement_zone_high = convertStringToU8(cv["EntitlementZoneHigh"]);
				}
			}
		}
        if ( addToCache )
        {
            g_differentialPricesCache.addCacheValue(query, differentialPricing, !returnValue);
        }
	}

    return returnValue;
}
