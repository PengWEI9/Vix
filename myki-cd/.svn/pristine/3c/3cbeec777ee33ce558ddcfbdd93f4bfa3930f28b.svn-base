/* -*- mode: c; tabs: 4; -*- */
/*=============================================================================
**
**    Vix Technology                   Licensed software
**    (C) 2014                         All rights reserved
**
**=============================================================================
**
**  Project/Product : MBU
**  Filename        : ProductConfigurationUtils.c
**  Author(s)       : James Ho
**
**  Description     :
**
**  Member(s)       :
**      getDBAccess             [public]    Gets Tariff database object
**      getActionlistDBAccess   [public]    Gets actionlist database object
**
**  Information     :
**   Compiler(s)    : C++
**   Target(s)      : Independent
**
**  Subversion      :
**      $Id: $
**      $HeadURL: $
**
**  History         :
**   Vers.  Date        Aut.  Type     Description
**   -----  ----------  ----  -------  ----------------------------------------
**    1.00  08.10.12    JH    Create
**    1.01  03.03.15    ANT   Add      Added API to close databases
**
**===========================================================================*/

/*
 *      Includes
 *      --------
 */

#include "ProductConfigurationUtils.h"
#include "Actionlist.h"
#include "DeviceHotlist.h"
#include "DeviceParameters.h"
#include "DifferentialPrices.h"
#include "DifferentialPricesProvisional.h"
#include "FaresEpass.h"
#include "FaresProvisional.h"
#include "FaresStoredValue.h"
#include "Locations.h"
#include "MasterRoutes.h"
#include "PassengerTypes.h"
#include "Products.h"
#include "Routes.h"
#include "RouteSections.h"
#include "RouteStops.h"
#include "ServiceProviders.h"
#include "StationLines.h"
#include "Stations.h"
#include "Zones.h"
#include "pugixml.hpp"
#include <stdio.h>
#include <string>
#include <vector>

/*
 *      Global Variables
 *      ----------------
 */


const std::string       productCfgDBFile        = "productcfg.db";

/*
 *      Local Variables
 *      ---------------
 */

//static std::string      actionlistDBFile        = defaultActionlistDBFile;
//static std::string      actionlistDeltaDBFile   = defaultActionlistDeltaDBFile;
static DbAccess        *g_dbAccess              = NULL;                 /**< Current Tariff database object */


namespace
{
	struct CDVersion_t
	{
		Time_t startTime;
		int majorVersion;
		int minorVersion;
		std::string sourceArchive;
		CDVersion_t() : startTime(0), majorVersion(0), minorVersion(0), sourceArchive("") {}
		CDVersion_t(const CDVersion_t& other) :
			startTime(other.startTime),
			majorVersion(other.majorVersion),
			minorVersion(other.minorVersion),
			sourceArchive(other.sourceArchive)
		{
		}
		const CDVersion_t& operator = (const CDVersion_t& other)
		{
			if ( &other != this )
			{
				startTime = other.startTime;
				majorVersion = other.majorVersion;
				minorVersion = other.minorVersion;
				sourceArchive = other.sourceArchive;
			}
			return *this;
		}
		bool operator == (const CDVersion_t& other) const
		{
			return
				(startTime == other.startTime) &&
				(majorVersion == other.majorVersion) &&
				(minorVersion == other.minorVersion) &&
				(sourceArchive == other.sourceArchive);
		}
	};
	std::vector<CDVersion_t> g_cdVersions;
	CDVersion_t g_currentCDVersion;
	char g_currentCDVersionWhere[100] = {'\0'};
}

std::string sqlify(std::string s)
{
	// If only numbers, then the string is sqlified already
	std::string::size_type pos = s.find_first_not_of("0123456789");
	if ( s.empty() || (pos != std::string::npos) )
	{
		pos = s.find('\'');
		while ( pos != std::string::npos )
		{
			s.insert(pos, "'");
			pos = s.find('\'', pos+2);
		}
		s.insert(0, "'");
		s += "'";
	}
	return s;
}

U32_t convertStringToU32(const std::string& string)
{
	return U32_t(atoll(string.c_str()));
}
U32_t convertStringToU32(const char* string)
{
	return U32_t(atoll(string));
}

U16_t convertStringToU16(const std::string& string)
{
	return U16_t(atoi(string.c_str()));
}
U16_t convertStringToU16(const char* string)
{
	return U16_t(atoi(string));
}

U8_t convertStringToU8(const std::string& string)
{
	return U8_t(atoi(string.c_str()));
}
U8_t convertStringToU8(const char* string)
{
	return U8_t(atoi(string));
}

bool convertStringToBool(const std::string& string)
{
    return convertStringToBool(string.c_str());
}

bool convertStringToBool(const char* string)
{
    if(string[0]!='\0')
    {
        char c = string[0];
        return c=='y' || c=='Y' || c=='t' || c=='T' || c=='1';
    }
    return false;
}

void stringCopy(char* target, const std::string& source, unsigned int targetLen)
{
         // alloc with room for null termination - required by xdr_string
         strncpy(target, source.c_str(), targetLen);
         target[targetLen-1] = '\0';
}
void stringCopy(char* target, const char* source, unsigned int targetLen)
{
         // alloc with room for null termination - required by xdr_string
         strncpy(target, source, targetLen);
         target[targetLen-1] = '\0';
}
void stringMemCopy(void* target, const char* source, unsigned int targetLen)
{
	strncpy(reinterpret_cast<char*>(target), source, targetLen);
}

/*==========================================================================*
**
**  getDBAccess
**
**  Description     :
**      Gets Tariff database object.
**
**  Parameters      :
**		filename	:		[I]		db filename to open
**
**  Returns         :
**      !NULL                       Tariff database object
**      NULL                        failed connecting Tariff database
**
**  Notes           :
**
**==========================================================================*/

DbAccess* getDBAccess(const char* filename) 
{
    static std::string s_dbFile = productCfgDBFile;
	if ( filename == NULL )
		filename = s_dbFile.c_str();

	if ( (g_dbAccess == NULL) || (s_dbFile != filename) )
	{
		DbAccess* newDbAccess = new DbAccess();
		try
		{
			newDbAccess->createDatabaseConnection(filename);
		}
		catch (...)
		{
			CsErrx("Could not load CD file '%s'", filename);
			delete newDbAccess;
			newDbAccess = NULL;
		}

		if ( newDbAccess != NULL )
		{
			std::vector<CDVersion_t> newCDVersions;
			Rows rows;
			if ( SQLITE_OK != newDbAccess->executeSqlQuery(
					"SELECT source_archive, major_version, minor_version, "
					"strftime('%s', start_date) AS start_date FROM CDVersion "
					"ORDER BY start_date DESC, major_version DESC, minor_version DESC",
					rows) )
			{
				CsErrx("Failed to get CD version information from '%s'", filename);
				delete newDbAccess;
				newDbAccess = NULL;
			}
			else
			{
				for ( Rows::iterator row = rows.begin(); row != rows.end(); row++ )
				{
					const ColumnValue& cv = *row;
					CDVersion_t cdVersion;
					cdVersion.sourceArchive = cv["source_archive"];
					cdVersion.startTime = Time_t(convertStringToU32(cv["start_date"]));
					cdVersion.majorVersion = convertStringToU32(cv["major_version"]);
					cdVersion.minorVersion = convertStringToU32(cv["minor_version"]);
					newCDVersions.push_back(cdVersion);
				}

				if ( g_dbAccess != NULL )
				{
					g_dbAccess->closeDatabaseConnection();
					delete g_dbAccess;
				}
				g_dbAccess = newDbAccess;
				g_cdVersions = newCDVersions;
				s_dbFile = filename;
				g_currentCDVersion.sourceArchive = "";
				g_currentCDVersion.startTime = 0;
				g_currentCDVersion.majorVersion = 0;
				g_currentCDVersion.minorVersion = 0;
				initCurrentCDVersion(time(0));
			}
		}

		return newDbAccess;
	}

    return  g_dbAccess;
}   /*  getDBAccess( ) */

/*==========================================================================*
**
**  releaseDBAccess
**
**  Description     :
**      Destroys current Tariff database object (if any).
**
**  Parameters      :
**		None
**
**  Returns         :
**      None
**
**  Notes           :
**
**==========================================================================*/

void    releaseDBAccess( void )
{
    if ( g_dbAccess != NULL )
    {
        CsDebug( CD_DEBUG, ( CD_DEBUG, "MYKI_CD:releaseDBAccess: closes current Tariff database" ) );
        g_dbAccess->closeDatabaseConnection( );
        delete  g_dbAccess;
        g_dbAccess  = NULL;
    }
}   /*  releaseDBAccess( ) */

int initLookupTables()
{
	initDeviceParametersLookup();
	initDifferentialPricesLookup();
	initDifferentialPricesProvisionalLookup();
	initFaresEpassLookup();
	initFaresProvisionalLookup();
	initFaresStoredValueLookup();
	initLocationLookup();
	initMasterRoutesLookup();
	initPassengerTypesLookup();
	initProductsLookup();
	initRoutesLookup();
	initRouteSectionsLookup();
	initRouteStopsLookup();
	initServiceProvidersLookup();
	initStationLinesLookup();
	initStationsLookup();
	initZonesLookup();
	return 1;
}


/*==========================================================================*
**
**  validateTariff
**
**  Description     :
**      Performs basic tests on database to ensure some key tables are populated.
**
**  Parameters      :
**		None
**
**  Returns         :
**      Boolean
**
**  Notes           :
**
**==========================================================================*/

int validateTariff()
{
    bool returnValue = true;
    int i=0;
    
	DbAccess 	*m_dbAccess;
	Rows rows;

	m_dbAccess =  getDBAccess();
    
	if(m_dbAccess == 0)
	{
		CsErrx("validateTariff - Could not get database");
		return false;
	}    
    
    static const char* tables [] =
    { 
        "DeviceParameterRecord",
        "DeviceParameterAttribute",
        "LocationRecord",
        "ProductRecord",
        NULL
    };

    while(tables[i]!=NULL && returnValue)
    {
        char s_query[200];
        sprintf(s_query, "SELECT Count(*) as total FROM %s", tables[i]);

        m_dbAccess->executeSqlQuery(s_query, rows);

        if(rows.empty())
        {
            CsErrx("validateTariff - Table %s not present", tables[i]);
            returnValue = false;
        }
        else 
        {
            const ColumnValue& cv = *(rows.begin());
            if(convertStringToU32(cv["total"])==0)
            {
                CsErrx("validateTariff - Table %s is empty", tables[i]);
                returnValue =false;
            }
        }
        i++;
    }
    
    return returnValue;
}

int getCurrentCDMajorVersion()
{
	return g_currentCDVersion.majorVersion;
}
int getCurrentCDMinorVersion()
{
	return g_currentCDVersion.minorVersion;
}
const char* getCurrentCDSource()
{
	return g_currentCDVersion.sourceArchive.c_str();
}
Time_t getCurrentCDStartTime()
{
	return g_currentCDVersion.startTime;
}
const char* getCurrentCDVersionWhere()
{
	return g_currentCDVersionWhere;
}

int initCurrentCDVersion(Time_t currentTime)
{
	unsigned int count = g_cdVersions.size();
	for ( unsigned int i = 0; i < count; i++ )
	{
		const CDVersion_t& cdVersion = g_cdVersions[i];
		if ( currentTime >= cdVersion.startTime )
		{
			if ( !(g_currentCDVersion == cdVersion) )
			{
				g_currentCDVersion = cdVersion;
				sprintf(g_currentCDVersionWhere, " (fk_major_version=%d AND fk_minor_version=%d) ", cdVersion.majorVersion, cdVersion.minorVersion);
				initLookupTables();
			}
			return 1;
		}
	}
	static CDVersion_t emptyCDVersion;
	g_currentCDVersion = emptyCDVersion;
	sprintf(g_currentCDVersionWhere, " (0 = 1) ");
	return 0;
}

static  DbAccess *g_actionlistDBAccess[ 2 ]   = { NULL, NULL };
static std::string   s_dbFile[ 2 ] = { defaultActionlistDBFile, defaultActionlistDeltaDBFile };

/*==========================================================================*
**
**  getActionlistDBAccess
**
**  Description     :
**      Gets full or delta actionlist database object.
**
**  Parameters      :
**      bDelta              [I]     true = Delta; false = Full
**		filename	:		[I]		db filename to open
**
**  Returns         :
**      !NULL                       actionlist database object
**      NULL                        failed connecting actionlist database
**
**  Notes           :
**
**==========================================================================*/

DbAccess   *getActionlistDBAccess( bool bDelta, const char *filename)
{
    int index = ( bDelta == true ? I_DELTA_ACTIONLIST : I_FULL_ACTIONLIST );

	std::string fileToOpen, currentFile;
	currentFile = s_dbFile[index];
	if (filename != NULL) {
		fileToOpen = filename;
	}  else {
		fileToOpen = currentFile;
	}
	CsDebug( CD_DEBUG, ( CD_DEBUG, "MYKI_CD:getActionlistDBAccess: Create %s Actionlist DBAccess filename is %s, index is %d, existing filename is %s", bDelta == true ? "Delta" : "Full", filename == NULL ? filename : "NULL", index, s_dbFile[index].c_str() ) );

    
    if ( g_actionlistDBAccess[index] == NULL || fileToOpen != currentFile)
    {
		CsDebug( CD_DEBUG, ( CD_DEBUG, "MYKI_CD:getActionlistDBAccess: Create %s Actionlist DBAccess", bDelta == true ? "Delta" : "Full" ) );

        DbAccess   *newDbAccess = new DbAccess();
        if ( newDbAccess != NULL )
        {
            try
            {
                newDbAccess->createDatabaseConnection(fileToOpen);
            }
            catch ( ... )
            {
                CsErrx( "MYKI_CD:getActionlistDBAccess: Could not load actionlist DB '%s'", fileToOpen.c_str() );
                delete  newDbAccess;
                newDbAccess = NULL;
            }
        }
        else
        {
            CsErrx( "MYKI_CD:getActionlistDBAccess: Failed instantiated Actionlist DBAccess object" );
        }   /*  end-of-if */

        if ( newDbAccess != NULL )
        {
            if ( g_actionlistDBAccess[ index ] != NULL )
            {
                g_actionlistDBAccess[ index ]->closeDatabaseConnection();
                delete  g_actionlistDBAccess[ index ];
                g_actionlistDBAccess[ index ]   = NULL;
            }
            g_actionlistDBAccess[ index ]       = newDbAccess;
            s_dbFile[ index ]                   = fileToOpen;

            initActionlistLookupTables();
        }
    }

    return g_actionlistDBAccess[ index ];
}   /*  getActionlistDBAccess( ) */

/*==========================================================================*
**
**  releaseActionlistDBAccess
**
**  Description     :
**      Destroys full or delta actionlist database object.
**
**  Parameters      :
**      bDelta              [I]     true = Delta; false = Full
**
**  Returns         :
**      None
**
**  Notes           :
**
**==========================================================================*/

void    releaseActionlistDBAccess( bool bDelta )
{
    int index   = ( bDelta == true ? I_DELTA_ACTIONLIST : I_FULL_ACTIONLIST );

    if ( g_actionlistDBAccess[ index ] != NULL )
    {
        CsDebug( CD_DEBUG, ( CD_DEBUG, "MYKI_CD:releaseActionlistDBAccess: closes current %s actionlist database",
                bDelta == true ? "Delta" : "Full" ) );
        g_actionlistDBAccess[ index ]->closeDatabaseConnection( );
        delete  g_actionlistDBAccess[ index ];
        g_actionlistDBAccess[ index ]   = NULL;

        initActionlistLookupTables();
    }
}   /*  releaseActionlistDBAccess( ) */

const std::string
getDeltaActionListFilename (void)
{
	return s_dbFile[I_DELTA_ACTIONLIST];
}

const std::string 
getFullActionListFilename (void)
{
	return s_dbFile[I_FULL_ACTIONLIST];
}


int initActionlistLookupTables()
{
	initDeviceHotlistLookup();
	initActionlistLookup();
	return 1;
}

int processXML(const char* description, const char* xmlFile, const char* xmlNode, const char* masterTable, const char* recordTable, int (*subtableFunction)(pugi::xpath_node node, int recordPK, std::vector<int>& subtablePKs) /*=0*/, const char* subtableDeleteQuery /*=0*/, std::set<std::string>* excludedAttributes /*=0*/)
{
	DbAccess 	*m_dbAccess;

	m_dbAccess =  getDBAccess();

	pugi::xml_document doc;
	pugi::xml_parse_result result = doc.load_file(xmlFile);

	int ret = 0;
	if(m_dbAccess == 0)
	{
		CsErrx("%s: Could not get database", description);
		ret = 1;
	}
	else if(!result)
	{
		CsErrx("%s: Could not open '%s'", description, xmlFile);
		ret = 1;
	}
	else
	{
		std::string headerMajorVersion;
		std::string headerMinorVersion;
		int recordPK = 0;
		std::vector<int> subtablePKs;

		// Get the Header
		std::string s_xmlNode = std::string("/") + xmlNode;
		pugi::xpath_node_set tools = doc.select_nodes(s_xmlNode.c_str());
		pugi::xpath_node_set::const_iterator it;
		for (it = tools.begin(); it != tools.end(); ++it)
		{
			pugi::xpath_node node = *it;
			std::string s_queryHeader = "";
			std::string s_queryColumn = "";
			std::string s_queryValue = "";
			int attributeCount = 0;
			// Get the attributes belonging to the Header
			for (pugi::xml_attribute attr = node.node().first_attribute(); attr;
					attr = attr.next_attribute()) {
				if (attributeCount > 0) {
					s_queryColumn += ",";
					s_queryValue += ",";
				}
				s_queryColumn.append("'");

				std::string str(attr.name());
				if (str.compare(0,6,"xmlns:") == 0)
				{
					str.replace(0,6,"");
				}
				else if (str.compare(0,4,"xsi:") == 0)
				{
					str.replace(0,4,"");
				}
				else if (str == "major_version")
				{
					headerMajorVersion = attr.value();
				}
				else if (str == "minor_version")
				{
					headerMinorVersion = attr.value();
				}
				s_queryColumn += str;

				s_queryColumn.append("'");
				s_queryValue += sqlify(attr.value());
				attributeCount++;
			}

			char s_deleteQuery[512];
			sprintf(
				s_deleteQuery,
				"DELETE FROM %s WHERE major_version = '%s' AND minor_version = '%s'",
				masterTable,
				headerMajorVersion.c_str(),
				headerMinorVersion.c_str());
			m_dbAccess->executeSqlDelete(s_deleteQuery);
			if ( getCurrentCDStartTime() != 0 )
			{
				sprintf(
					s_deleteQuery,
					"DELETE FROM %s WHERE major_version||'.'||minor_version IN "
					"(SELECT major_version||'.'||minor_version FROM %s "
					" WHERE CAST(strftime('%%s', start_date) AS NUMBER) < %d)",
					masterTable,
					masterTable,
					int(getCurrentCDStartTime()));
				m_dbAccess->executeSqlDelete(s_deleteQuery);
			}
			sprintf(
				s_deleteQuery,
				"DELETE FROM %s WHERE NOT fk_major_version||'.'||fk_minor_version IN "
				"(SELECT major_version||'.'||minor_version FROM %s)",
				recordTable,
				masterTable);
			m_dbAccess->executeSqlDelete(s_deleteQuery);
			if ( subtableDeleteQuery != 0 )
				m_dbAccess->executeSqlDelete(subtableDeleteQuery);

			s_queryHeader += "INSERT INTO ";
			s_queryHeader += masterTable;
			s_queryHeader += "(";
			s_queryHeader += s_queryColumn;
			s_queryHeader += ") VALUES (";
			s_queryHeader += s_queryValue;
			s_queryHeader += ")";

			std::cout << "\n\n" << s_queryHeader;
			if ( m_dbAccess->executeSqlInsert(s_queryHeader) != 0 )
			{
				CsErrx("Query '%s' failed", s_queryHeader.c_str());
				ret = 2;
			}
		}

		// Get the primary key from the record table
		std::string s_recordKeyQuery = "SELECT MAX(PrimaryKey) AS PrimaryKey FROM ";
		s_recordKeyQuery += recordTable;
		Rows recordKeyRows;
		m_dbAccess->executeSqlQuery(s_recordKeyQuery.c_str(), recordKeyRows);
		if ( !recordKeyRows.empty() )
		{
			recordPK = convertStringToU32(recordKeyRows.front()["PrimaryKey"]);
		}

		// Get the Records
		std::string s_xmlSubNode = s_xmlNode + "/Record";
		pugi::xpath_node_set recordNodes = doc.select_nodes(s_xmlSubNode.c_str());
		if (recordNodes.empty() && (*(s_xmlNode.rbegin()) == 's'))
		{
			// no sub-nodes - try with the same name but without the 's' plural.
			// DeviceParameters.xml is like this
			s_xmlSubNode = s_xmlNode + s_xmlNode;
			s_xmlSubNode.erase(s_xmlSubNode.length()-1);
			recordNodes = doc.select_nodes(s_xmlSubNode.c_str());
		}
		for (it = recordNodes.begin(); it != recordNodes.end(); ++it)
		{
			recordPK++;
			pugi::xpath_node node = *it;

			std::string s_queryRecord = "";
			std::string s_queryColumn = "";
			std::string s_queryValue = "";
			int attributeCount = 0;
			// Get the attributes belonging to the Record
			for (pugi::xml_attribute attr = node.node().first_attribute(); attr; attr = attr.next_attribute())
			{
				if((excludedAttributes == 0) || (excludedAttributes->find(attr.name()) == excludedAttributes->end()))
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
			}
			// Get the optional attributes belong to the record
			for (pugi::xml_node optional = node.node().first_child(); optional; optional = optional.next_sibling())
			{
				if((optional.first_child().value() != 0) && (strlen(optional.first_child().value()) > 0))
				{
					if((excludedAttributes == 0) || (excludedAttributes->find(optional.name()) == excludedAttributes->end()))
					{
						if(attributeCount > 0)
						{
							s_queryColumn += ",";
							s_queryValue += ",";
						}
						s_queryColumn.append("'");
						s_queryColumn += optional.name();
						s_queryColumn.append("'");
						s_queryValue += sqlify(optional.first_child().value());
						attributeCount++;
					}
				}
			}
			s_queryRecord += "INSERT INTO ";
			s_queryRecord += recordTable;
			s_queryRecord += "('PrimaryKey',";
			s_queryRecord += s_queryColumn;
			s_queryRecord += ",'FK_major_version','FK_minor_version') VALUES ('";
			s_queryRecord += static_cast<std::ostringstream*>( &(std::ostringstream() << recordPK) )->str(); // PK for Record
			s_queryRecord += "',";
			s_queryRecord += s_queryValue;
			s_queryRecord += ",'";
			s_queryRecord += headerMajorVersion;
			s_queryRecord += "','";
			s_queryRecord += headerMinorVersion;
			s_queryRecord += "'";
			s_queryRecord += ")";

			//std::cout << "\n\n" << s_queryRecord;
			if ( m_dbAccess->executeSqlInsert(s_queryRecord) != 0 )
			{
				CsErrx("Query '%s' failed", s_queryRecord.c_str());
				ret = 2;
			}

			if ( subtableFunction != 0 )
			{
				int tmpRet = subtableFunction(node, recordPK, subtablePKs);
				if ( tmpRet != 0 )
				{
					CsErrx("Subtable function for ID '%d' failed", recordPK);
					ret = tmpRet;
				}
			}
		}

		m_dbAccess->endDbTransaction();
	}

	return ret;
}
