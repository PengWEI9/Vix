/*
 * DeviceParameters.cpp
 *
 *  Created on: 01/10/2012
 *      Author: James Ho
 */

#include "DeviceParameters.h"
#include "pugixml.hpp"
#include "CdCache.h"
#include <set>

std::set<U16_t> g_earlyBirdStation;
bool g_earlyBirdStationRetrieved = false;

std::set<int> g_earlyBirdDay;
bool g_earlyBirdDayRetrieved = false;

int g_earlyBirdProduct;
bool g_earlyBirdProductRetrieved = false;

int g_earlyBirdCutOffTime;
bool g_earlyBirdCutOffTimeRetrieved = false;

CdCache<std::string, MYKI_CD_DeviceParameter_t> g_deviceParametersCache(
	"DeviceParameters", 75, CacheMemoryCopy<MYKI_CD_DeviceParameter_t>);

CdCache<std::string, MYKI_CD_DeviceParameter_t> g_paymentMethodsCache(
	"DeviceParameters", 75, CacheMemoryCopy<MYKI_CD_DeviceParameter_t>);

std::string g_currentDeviceParameters;
int currentDeviceParametersInit = setDeviceParameters("FPDg", MYKI_CD_MODE_RAIL, 0, 0, 0, 0, 0, 0); // Default configuration
int setDeviceParameters(const char* deviceType, MYKI_CD_Mode_t mode, U8_t zone, U16_t locationId, U8_t passengerType, U16_t serviceProvider, U8_t blockingReason, U8_t paymentMethod)
{
	std::stringstream result;

	if ( deviceType == 0 )
		result << " AND dpa.device_type IS NULL";
	else
		result << " AND (dpa.device_type IS NULL OR dpa.device_type = '" << deviceType << "')";

	result << " AND dpa.mode IN ('ALL'";
	switch ( mode )
	{
	case MYKI_CD_MODE_BUS: result << ",'BUS'"; break;
	case MYKI_CD_MODE_RAIL: result << ",'RAIL'"; break;
	case MYKI_CD_MODE_TRAM: result << ",'TRAM'"; break;
	default: break;
	}
	result << ")";

#define ADD_ITEM(ITEM, COLUMN) \
	if ( ITEM == 0 ) \
		result << " AND " << COLUMN << " IS NULL"; \
	else \
		result << " AND (" << COLUMN << " IS NULL OR " << COLUMN << "=" << int(ITEM) << ")"
	ADD_ITEM(zone, "dpa.zone");
	ADD_ITEM(locationId, "dpa.location");
	ADD_ITEM(passengerType, "dpa.passenger_type");
	ADD_ITEM(serviceProvider, "dpa.service_provider");
	ADD_ITEM(blockingReason, "dpa.blocking_reason");
	ADD_ITEM(paymentMethod, "dpa.payment_method");
#undef ADD_ITEM

	g_currentDeviceParameters = result.str();
	g_deviceParametersCache.clear();
    g_paymentMethodsCache.clear();
	return 1;
}

int initDeviceParametersLookup()
{
	g_earlyBirdStationRetrieved = false;
	g_earlyBirdDayRetrieved = false;
	g_earlyBirdProductRetrieved = false;
	g_earlyBirdCutOffTimeRetrieved = false;
	g_deviceParametersCache.clear();
    g_paymentMethodsCache.clear();
	return 1;
}

int processDeviceParametersSubtableXML(pugi::xpath_node node, int deviceParameterRecordPK, std::vector<int>& subtablePKs)
{
	DbAccess* m_dbAccess = getDBAccess();
	int ret = 0;
    if(m_dbAccess==NULL)
        return 2; 
    
	// Get the Attribute Element Child belonging to each Device Parameter Record
	bool needPrimaryKeys = false;
	if (subtablePKs.empty())
	{
		needPrimaryKeys = true;
		subtablePKs.push_back(0);
	}
	int& deviceParameterAttributePK = subtablePKs[0];
	if (needPrimaryKeys)
	{
		Rows rows;
		m_dbAccess->executeSqlQuery("SELECT MAX(PrimaryKey) AS PrimaryKey FROM DeviceParameterAttribute", rows);
		if (!rows.empty())
			deviceParameterAttributePK = convertStringToU32(rows.front()["PrimaryKey"]);
	}

	for (pugi::xml_node tool = node.node().first_child(); tool; tool = tool.next_sibling())
	{
		std::string s_queryDeviceParameterAttribute = "";
		std::string s_queryColumn = "";
		std::string s_queryValue = "";

		deviceParameterAttributePK++;
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
		s_queryDeviceParameterAttribute = "INSERT INTO DeviceParameterAttribute('PrimaryKey',";
		s_queryDeviceParameterAttribute += s_queryColumn;
		s_queryDeviceParameterAttribute += ",'FK_DeviceParameterRecord') VALUES ('";
		s_queryDeviceParameterAttribute += static_cast<std::ostringstream*>( &(std::ostringstream() << deviceParameterAttributePK) )->str(); // PK for Device Parameter Attribute
		s_queryDeviceParameterAttribute += "',";
		s_queryDeviceParameterAttribute += s_queryValue;
		s_queryDeviceParameterAttribute += ",'";
		s_queryDeviceParameterAttribute += static_cast<std::ostringstream*>( &(std::ostringstream() << deviceParameterRecordPK) )->str(); // FK to Device Parameter Record
		s_queryDeviceParameterAttribute += "'";
		s_queryDeviceParameterAttribute += ")";

		// std::cout << "\n" << s_queryDeviceParameterAttribute;
		if ( m_dbAccess->executeSqlInsert(s_queryDeviceParameterAttribute) != 0 )
		{
			CsErrx("Query '%s' failed", s_queryDeviceParameterAttribute.c_str());
			ret = 2;
		}
	}

	return ret;
}

int processDeviceParametersXML()
{
	DbAccess* m_dbAccess = getDBAccess();

	int ret = 0;
	if(m_dbAccess == 0)
	{
		CsErrx("processDeviceParametersXML: Could not get database");
		ret = 1;
	}
	else
	{
		const char* s_subtableDeleteQuery =
			"DELETE FROM DeviceParameterAttribute WHERE "
			"NOT FK_DeviceParameterRecord IN (SELECT PrimaryKey FROM DeviceParameterRecord)";

		ret = processXML(
			"processDeviceParametersXML", // description
			"DeviceParameters.xml",       // xml file
			"DeviceParameters",           // xml node
			"DeviceParameters",           // master table
			"DeviceParameterRecord",      // record table
			processDeviceParametersSubtableXML,
			s_subtableDeleteQuery);
	}

	initDeviceParametersLookup();
	return ret;
}

int getDeviceParameter(const std::string& deviceParameterName, MYKI_CD_DeviceParameter_t &deviceParameter)
{
    int returnValue = false;
	DbAccess 	*m_dbAccess;
	Rows rows;

	CsDebug(CD_DEBUG, (CD_DEBUG, "MYKI_CD:getDeviceParameter %s", deviceParameterName.c_str()));

	bool isNull = false;
	if(g_deviceParametersCache.getCacheValue(deviceParameterName, deviceParameter, isNull))
	{
		returnValue = !isNull;
		CsDebug(CD_DEBUG, (CD_DEBUG, "MYKI_CD:getDeviceParameter: Return cache device parameter record found for id %s", deviceParameterName.c_str()));
	}
	else
	{
		m_dbAccess =  getDBAccess();
        if(m_dbAccess==NULL)
            return false; 
    
		std::stringstream s_query;
		s_query
			<< "SELECT"
			<< " dRec.name AS name,"
			<< " dRec.'default' AS 'default',"
			<< " dRec.value AS value,"
			<< " dpa.value AS sub_value "
			<< "FROM"
			<< " DeviceParameterRecord dRec"
			<< " LEFT OUTER JOIN DeviceParameterAttribute dpa "
			<< "ON"
			<< " dRec.PrimaryKey = dpa.fk_DeviceParameterRecord "
			<< g_currentDeviceParameters
			<< " WHERE dRec.name = '" << deviceParameterName << "' AND "
			<< getCurrentCDVersionWhere()
			<< " ORDER BY dpa.mode DESC";
			
		if(m_dbAccess->executeSqlQuery(s_query.str().c_str(), rows)!=SQLITE_OK)
		{
			CsDebug(CD_DEBUG, (CD_DEBUG, "MYKI_CD:getDeviceParameter: No Device Parameters found for %s.", deviceParameterName.c_str()));
		}
		else if(rows.empty())
		{
			CsDebug(CD_DEBUG, (CD_DEBUG, "MYKI_CD:getDeviceParameter: No Device Parameters found for %s.", deviceParameterName.c_str()));
		}
		else
		{
			const ColumnValue& cv = *(rows.begin());
			stringCopy(deviceParameter.name, cv["name"], LEN_Short_Description_t);
			stringCopy(deviceParameter.default_value, cv["default"], LEN_Value_t);
			stringCopy(deviceParameter.value, cv["sub_value"], LEN_Value_t);
			if ( deviceParameter.value[0] == '\0' )
				stringCopy(deviceParameter.value, cv["value"], LEN_Value_t);

			returnValue = true;
		}
		g_deviceParametersCache.addCacheValue(deviceParameterName, deviceParameter, !returnValue);
	}

    return returnValue;
}


int getDeviceParameterBySP(const std::string& deviceParameterName, U16_t serviceProvider, MYKI_CD_DeviceParameter_t &deviceParameter)
{
    int returnValue = false;
	DbAccess 	*m_dbAccess;
	Rows rows;

	CsDebug(CD_DEBUG, (CD_DEBUG, "MYKI_CD:getDeviceParameterBySP sp=%d %s\n", serviceProvider, deviceParameterName.c_str()));

    // This query is not cached
    
    m_dbAccess =  getDBAccess();
    if(m_dbAccess==NULL)
        return false; 

    std::stringstream s_query;
    
    // REVIEW: This query notably omits the g_currentDeviceParameters values
    
       s_query
        << "SELECT"
        << " dRec.name AS name,"
        << " dRec.'default' AS 'default',"
        << " dRec.value AS value,"
        << " dpa.value AS sub_value "
        << "FROM"
        << " DeviceParameterRecord dRec"
        << " LEFT OUTER JOIN DeviceParameterAttribute dpa "
        << "ON"
        << " dRec.PrimaryKey = dpa.fk_DeviceParameterRecord "
        << " WHERE dRec.name = '" << deviceParameterName << "' AND "
        << " dpa.service_provider = "  << serviceProvider << " AND "
        << getCurrentCDVersionWhere()
        << " ORDER BY dpa.mode DESC";

    if(m_dbAccess->executeSqlQuery(s_query.str().c_str(), rows)!=SQLITE_OK)
    {
        CsDebug(CD_DEBUG, (CD_DEBUG, "MYKI_CD:getDeviceParameterBySP: No Device Parameters found for SP:%d %s.", serviceProvider, deviceParameterName.c_str()));
    }
    else if(rows.empty())
    {
        CsDebug(CD_DEBUG, (CD_DEBUG, "MYKI_CD:getDeviceParameterBySP: No Device Parameters found for SP:%d %s.",  serviceProvider, deviceParameterName.c_str()));
    }
    else
    {
        const ColumnValue& cv = *(rows.begin());
        stringCopy(deviceParameter.name, cv["name"], LEN_Short_Description_t);
        stringCopy(deviceParameter.default_value, cv["default"], LEN_Value_t);
        stringCopy(deviceParameter.value, cv["sub_value"], LEN_Value_t);
        if ( deviceParameter.value[0] == '\0' )
            stringCopy(deviceParameter.value, cv["value"], LEN_Value_t);

        returnValue = true;
    }

    return returnValue;
}


int getDeviceParameterBySPandMode(const std::string& deviceParameterName, U16_t serviceProvider, MYKI_CD_Mode_t mode, MYKI_CD_DeviceParameter_t &deviceParameter)
{
    int returnValue = false;
	DbAccess 	*m_dbAccess;
	Rows rows;

	CsDebug(CD_DEBUG, (CD_DEBUG, "MYKI_CD:getDeviceParameterBySPandMode sp=%d mode=%d %s\n", serviceProvider, mode, deviceParameterName.c_str()));

    // This query is not cached
    
    m_dbAccess =  getDBAccess();
    if(m_dbAccess==NULL)
        return false; 

    std::stringstream s_query;
    
    // REVIEW: This query notably omits the g_currentDeviceParameters values
    
    s_query
        << "SELECT"
        << " dRec.name AS name,"
        << " dRec.'default' AS 'default',"
        << " dRec.value AS value,"
        << " dpa.value AS sub_value, "
	    << " dpa.mode as mode, "
	    << " dpa.service_provider==" << serviceProvider << " as correctsp " 
        << "FROM"
        << " DeviceParameterRecord dRec"
        << " LEFT OUTER JOIN DeviceParameterAttribute dpa "
        << "ON"
        << " dRec.PrimaryKey = dpa.fk_DeviceParameterRecord "
        << " WHERE dRec.name = '" << deviceParameterName << "' AND "
        << " (dpa.service_provider IS NULL OR dpa.service_provider = "  << serviceProvider << " ) AND "
        << " dpa.mode IN (NULL, 'ALL'";

        switch ( mode )
        {
            case MYKI_CD_MODE_BUS: s_query << ",'BUS'"; break;
            case MYKI_CD_MODE_RAIL: s_query << ",'RAIL'"; break;
            case MYKI_CD_MODE_TRAM: s_query << ",'TRAM'"; break;
            default: break;
        }        
            
        s_query << ") AND"
        << getCurrentCDVersionWhere()
        << " ORDER BY correctsp DESC, dpa.mode DESC";
        
    if(m_dbAccess->executeSqlQuery(s_query.str().c_str(), rows)!=SQLITE_OK)
    {
        CsDebug(CD_DEBUG, (CD_DEBUG, "MYKI_CD:getDeviceParameterBySPandMode: No Device Parameters found for SP:%d %d %s.", serviceProvider, mode, deviceParameterName.c_str()));
    }
    else if(rows.empty())
    {
        CsDebug(CD_DEBUG, (CD_DEBUG, "MYKI_CD:getDeviceParameterBySPandMode: No Device Parameters found for SP:%d %d %s.",  serviceProvider, mode, deviceParameterName.c_str()));
    }
    else
    {
        const ColumnValue& cv = *(rows.begin());
        stringCopy(deviceParameter.name, cv["name"], LEN_Short_Description_t);
        stringCopy(deviceParameter.default_value, cv["default"], LEN_Value_t);
        stringCopy(deviceParameter.value, cv["sub_value"], LEN_Value_t);
        if ( deviceParameter.value[0] == '\0' )
            stringCopy(deviceParameter.value, cv["value"], LEN_Value_t);

        returnValue = true;
    }

    return returnValue;
}

int getPaymentMethod(const char* deviceType, U16_t serviceProvider, const char* paymentMethod, MYKI_CD_DeviceParameter_t &deviceParameter)
{
    int returnValue = false;
	DbAccess 	*m_dbAccess;
	Rows rows;

	CsDebug(CD_DEBUG, (CD_DEBUG, "MYKI_CD:getPaymentMethod %s", paymentMethod));

	bool isNull = false;
	if(g_paymentMethodsCache.getCacheValue(paymentMethod, deviceParameter, isNull))
	{
		returnValue = !isNull;
		CsDebug(CD_DEBUG, (CD_DEBUG, "MYKI_CD:getPaymentMethod: Return cache device parameter record found for id %s", paymentMethod));
	}
	else
	{
		m_dbAccess =  getDBAccess();
        if(m_dbAccess==NULL)
            return false; 

        /** Uses the passed in values instead of g_currentDeviceParameters */
        std::stringstream deviceParams;
        if (deviceType == 0)
            deviceParams << " AND dpa.device_type IS NULL";
        else
            deviceParams << " AND (dpa.device_type IS NULL OR dpa.device_type = '" << deviceType << "')";
        if (serviceProvider == 0)
            deviceParams << " AND dpa.service_provider IS NULL";
        else
            deviceParams << "AND (dpa.service_provider IS NULL OR  dpa.service_provider IN ('ALL','" << serviceProvider << "'))";
        std::string currentDeviceParams = deviceParams.str();
    
		std::stringstream s_query;
		s_query
			<< "SELECT"
			<< " dRec.name AS name,"
			<< " dRec.'default' AS 'default',"
			<< " dRec.value AS value,"
			<< " dpa.value AS sub_value "
			<< "FROM"
			<< " DeviceParameterRecord dRec"
			<< " LEFT OUTER JOIN DeviceParameterAttribute dpa "
			<< "ON"
			<< " dRec.PrimaryKey = dpa.fk_DeviceParameterRecord "
			<< currentDeviceParams
			<< " WHERE dRec.name = 'PaymentMethodEnabled' AND "
            << "dpa.payment_method = '" << paymentMethod << "' AND "
			<< getCurrentCDVersionWhere()
			<< " ORDER BY dpa.mode DESC";
			
		if(m_dbAccess->executeSqlQuery(s_query.str().c_str(), rows)!=SQLITE_OK)
		{
			CsDebug(CD_DEBUG, (CD_DEBUG, "MYKI_CD:getPaymentMethod: No Device Parameters found for %s.", paymentMethod));
		}
		else if(rows.empty())
		{
			CsDebug(CD_DEBUG, (CD_DEBUG, "MYKI_CD:getPaymentMethod: No Device Parameters found for %s.", paymentMethod));
		}
		else
		{
			const ColumnValue& cv = *(rows.begin());
			stringCopy(deviceParameter.name, cv["name"], LEN_Short_Description_t);
			stringCopy(deviceParameter.default_value, cv["default"], LEN_Value_t);
			stringCopy(deviceParameter.value, cv["sub_value"], LEN_Value_t);
			if ( deviceParameter.value[0] == '\0' )
				stringCopy(deviceParameter.value, cv["value"], LEN_Value_t);

			returnValue = true;
		}
		g_paymentMethodsCache.addCacheValue(paymentMethod, deviceParameter, !returnValue);
	}

    return returnValue;
}

int isEarlyBirdStation(U16_t stationId)
{
	if ( !g_earlyBirdStationRetrieved )
	{
		DbAccess 	*m_dbAccess;
		Rows rows;

		m_dbAccess =  getDBAccess();
        if(m_dbAccess==NULL)
            return false; 

		char s_query[350];
		sprintf(s_query,
			"SELECT DeviceParameterAttribute.value FROM "
			"DeviceParameterAttribute "
			"INNER JOIN DeviceParameterRecord "
			"on DeviceParameterRecord.PrimaryKey = DeviceParameterAttribute.FK_DeviceParameterRecord "
			"WHERE DeviceParameterRecord.name = 'EarlyBirdStations' "
			"AND %s", getCurrentCDVersionWhere());
		
		m_dbAccess->executeSqlQuery(s_query, rows);
		
		for ( Rows::iterator row = rows.begin(); row != rows.end(); row++ )
		{
			g_earlyBirdStation.insert(U16_t(atoi(row->begin()->second)));
		}

		g_earlyBirdStationRetrieved = true;
	}

	if ( g_earlyBirdStation.find(stationId) != g_earlyBirdStation.end() )
	{
		return true;
	}
    
	return false;
}

int isEarlyBirdDay(int day)
{
	if ( !g_earlyBirdDayRetrieved )
	{
		DbAccess 	*m_dbAccess;
		Rows rows;

		m_dbAccess =  getDBAccess();
        if(m_dbAccess==NULL)
            return false; 

		char s_query[200];
		sprintf(s_query,
			"SELECT DeviceParameterRecord.value FROM "
			"DeviceParameterRecord "
			"WHERE DeviceParameterRecord.name = 'EarlyBirdDayOfWeek' "
			"AND %s", getCurrentCDVersionWhere());
		
		m_dbAccess->executeSqlQuery(s_query, rows);
		
		for ( Rows::iterator row = rows.begin(); row != rows.end(); row++ )
		{
		    std::string value;
			std::string day;
			
			value=row->begin()->second;
						
			while (value.find(",", 0) != std::string::npos)
			{
				size_t pos = value.find(",", 0);
				day = value.substr(0, pos);
				g_earlyBirdDay.insert(int(atoi(day.c_str())));
				
				value.erase(0, pos + 1);   
			}
			g_earlyBirdDay.insert(int(atoi(value.c_str())));
		}

		g_earlyBirdDayRetrieved = true;
	}


	if ( g_earlyBirdDay.find(day) != g_earlyBirdDay.end() )
	{
		return true;
	}

	return false;
}

int getEarlyBirdProductId(void)
{
	if ( !g_earlyBirdProductRetrieved )
	{
		DbAccess 	*m_dbAccess;
		Rows rows;

		m_dbAccess =  getDBAccess();
        if(m_dbAccess==NULL)
            return 0; 

		char s_query[200];
		sprintf(s_query,
			"SELECT DeviceParameterRecord.value FROM "
			"DeviceParameterRecord "
			"WHERE DeviceParameterRecord.name = 'EarlyBirdProduct' "
			"AND %s", getCurrentCDVersionWhere());
		
		m_dbAccess->executeSqlQuery(s_query, rows);
		
		for ( Rows::iterator row = rows.begin(); row != rows.end(); row++ )
		{
			g_earlyBirdProduct = int(atoi(row->begin()->second));
		}

		g_earlyBirdProductRetrieved = true;
	}

	return g_earlyBirdProduct;
}

int getEarlyBirdCutOffTime(void)
{
	if ( !g_earlyBirdCutOffTimeRetrieved )
	{
		DbAccess 	*m_dbAccess;
		Rows rows;

		m_dbAccess =  getDBAccess();
        if(m_dbAccess==NULL)
            return false; 
    
		char s_query[200];
		sprintf(s_query,
			"SELECT DeviceParameterRecord.value FROM "
			"DeviceParameterRecord "
			"WHERE DeviceParameterRecord.name = 'EarlyBirdCutoffTime' "
			"AND %s", getCurrentCDVersionWhere());
		
		m_dbAccess->executeSqlQuery(s_query, rows);
		
		for ( Rows::iterator row = rows.begin(); row != rows.end(); row++ )
		{
			g_earlyBirdCutOffTime = int(atoi(row->begin()->second));
		}
		
		g_earlyBirdCutOffTime = (((g_earlyBirdCutOffTime) / 100) * 60) + ((g_earlyBirdCutOffTime) % 100); // returns minutes past midnight
        
		g_earlyBirdCutOffTimeRetrieved = true;
	}

	return g_earlyBirdCutOffTime;
}

