/*============================================================================ 
** Copyright (C) 1998-2013 Vix Technology, All rights reserved
**============================================================================
**
**  Project/Product : 
**  Filename        : DifferentialPricesProvisional.cpp
**  Author(s)       : dpurdie
**  Created         : 27-Nov-13
**
**  Description     : Support for DifferentialPricesProvisional.xml 
**
**
**  Information     :
**   Compiler       : ANSI C++
**   Target         : 
**
***==========================================================================*/


#include "DifferentialPricesProvisional.h"
#include "ProductConfigurationUtils.h"
#include "CdCache.h"

CdCache<std::string, MYKI_CD_DifferentialPricingProvisional_t> g_differentialPricesProvisionalCache(
	"DifferentialPricesProvisional", 300, CacheMemoryCopy<MYKI_CD_DifferentialPricingProvisional_t>);

int initDifferentialPricesProvisionalLookup()
{
	g_differentialPricesProvisionalCache.clear();
	return 1;
}

int processDifferentialPricesProvisionalSubtableXML(pugi::xpath_node node, int recordPK, std::vector<int>& subtablePKs)
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
        dbAccess->executeSqlQuery("SELECT MAX(PrimaryKey) AS PrimaryKey FROM DifferentialPricesProvisionalRoute", rows);
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
            s_queryRoute = "INSERT INTO DifferentialPricesProvisionalRoute('PrimaryKey',";
            s_queryRoute += s_queryColumn;
            s_queryRoute += ",'FK_DifferentialPricesProvisionalRecord') VALUES ('";
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

            s_updateRecord = "UPDATE DifferentialPricesProvisionalRecord ";
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

int processDifferentialPricesProvisionalXML()
{
    DbAccess* dbAccess = getDBAccess();

    int ret = 0;
    if(dbAccess == 0)
    {
        CsErrx("processDifferentialPricesProvisionalXML: Could not get database");
        ret = 1;
    }
    else
    {
        const char* s_subtableDeleteQuery =
            "DELETE FROM DifferentialPricesProvisionalRoute WHERE "
            "NOT FK_DifferentialPricesProvisionalRecord IN (SELECT PrimaryKey FROM DifferentialPricesProvisionalRecord)";

        ret = processXML(
            "processDifferentialPricesProvisionalXML", // description
            "DifferentialPricesProvisional.xml",       // xml file
            "DifferentialPricesProvisional",           // xml node
            "DifferentialPricesProvisional",           // master table
            "DifferentialPricesProvisionalRecord",      // record table
            processDifferentialPricesProvisionalSubtableXML,
            s_subtableDeleteQuery);

        if (ret == 0)
        {
            // Change the format of DateLow and DateHigh in the database
            // from DD/MM to MM-DD - better for comparisons.
            std::string s_query =
                "UPDATE DifferentialPricesProvisionalRecord "
                "SET DateLow = substr(DateLow, 4, 2) || '-' || substr(DateLow, 1, 2) "
                "WHERE DateLow LIKE '__/__'";
            dbAccess->executeSqlUpdate(s_query);

            s_query =
                "UPDATE DifferentialPricesProvisionalRecord "
                "SET DateHigh = substr(DateHigh, 4, 2) || '-' || substr(DateHigh, 1, 2) "
                "WHERE DateHigh LIKE '__/__'";
            dbAccess->executeSqlUpdate(s_query);
        }
    }

	initDifferentialPricesProvisionalLookup();
    return ret;
}

int getDifferentialPricesProvisional(const MYKI_CD_DifferentialPricingRequest_t* input, MYKI_CD_DifferentialPricingProvisional_t& differentialPricing)
{
    int returnValue = false;    
    DbAccess* dbAccess = getDBAccess();
    if ( dbAccess == 0 )
    {
        CsErrx("getDifferentialPricesProvisional: no database");
    }
    else
    {
        Rows rows;
        char query[2048];
        int queryLen = 0;
        queryLen += sprintf(query + queryLen, "SELECT * FROM DifferentialPricesProvisionalRecord");

        queryLen += sprintf(query + queryLen, " WHERE (ZoneLow IS NULL OR ZoneLow <= %d)", int(input->zone_low));
        queryLen += sprintf(query + queryLen, " AND (ZoneHigh IS NULL OR ZoneHigh >= %d)", int(input->zone_high));
        queryLen += sprintf(query + queryLen, " AND (ZoneCountLow IS NULL OR ZoneCountLow <= %d)", int(input->zone_high) - int(input->zone_low) + 1);
        queryLen += sprintf(query + queryLen, " AND (ZoneCountHigh IS NULL OR ZoneCountHigh >= %d)", int(input->zone_high) - int(input->zone_low) + 1);

        if ( !input->passenger_type_null )
            queryLen += sprintf(query + queryLen, " AND (PassengerType IS NULL OR PassengerType=%d)", int(input->passenger_type));

        if ( input->scanon_minutes_since_midnight >= 0 )
        {
            queryLen += sprintf(query + queryLen, " AND (TimeOfDayLow IS NULL OR TimeOfDayLow <= %d)", int(input->scanon_minutes_since_midnight));
            queryLen += sprintf(query + queryLen, " AND (TimeOfDayHigh IS NULL OR TimeOfDayHigh >= %d)", int(input->scanon_minutes_since_midnight));
        }
        if ( input->scanoff_minutes_since_midnight >= 0 )
        {
            queryLen += sprintf(query + queryLen, " AND (TimeOfDayLow IS NULL OR TimeOfDayLow <= %d)", int(input->scanoff_minutes_since_midnight));
            queryLen += sprintf(query + queryLen, " AND (TimeOfDayHigh IS NULL OR TimeOfDayHigh >= %d)", int(input->scanoff_minutes_since_midnight));
        }

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

        queryLen += sprintf(query + queryLen, " AND %s ORDER BY priority ASC", getCurrentCDVersionWhere());

		bool isNull = false;
		bool addToCache = false;
		if ( g_differentialPricesProvisionalCache.getCacheValue(query, differentialPricing, isNull) )
		{
			returnValue = !isNull;
			CsDebug(CD_DEBUG, (CD_DEBUG, "MYKI_CD:getDifferentialPricesProvisional: Return Cache record found"));
		}
		else
		{
            addToCache = true;
            dbAccess->executeSqlQuery(query, rows);

            if(rows.empty())
            {
                CsDebug(CD_DEBUG, (CD_DEBUG, "MYKI_CD:getDifferentialPricesProvisional: No Differential Price record found"));
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
                                << "FROM DifferentialPricesProvisionalRoute "
                                << "WHERE FK_DifferentialPricesProvisionalRecord = '" << cv["PrimaryKey"] << "' "
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
                    differentialPricing.discount = convertStringToU32(cv["Discount"]);
                }
            }
        }
        if ( addToCache )
        {
            g_differentialPricesProvisionalCache.addCacheValue(query, differentialPricing, !returnValue);
        }
    }
    
    return returnValue;
}
