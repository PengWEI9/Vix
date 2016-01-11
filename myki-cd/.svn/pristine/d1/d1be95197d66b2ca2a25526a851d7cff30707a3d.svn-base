#include "myki_cd.h"
#include "ProductConfigurationUtils.h"
#include "cs.h"
#include <string>

/*
 *      External References
 *      -------------------
 */

    /*  XMLUpdateMain.cpp */
extern  bool    railTariff;

int ProcessXML(const std::string& dbFileLocation, const std::string& xmlLocation, const std::string& xmlSource)
{
	int ret = 0;
	DbAccess *dbAccess = 0;
	if ( ! dbFileLocation.empty() ) 
	{
		std::string fullDbFilename = dbFileLocation + productCfgDBFile;
		dbAccess = getDBAccess(fullDbFilename.c_str());
		if ( dbAccess == 0 )
		{
			CsErrx("Failed to open CD database '%s'", fullDbFilename.empty() ? "(default)" : fullDbFilename.c_str());
			ret = 1;
		}
	}

	// Process XML
	if ( (ret == 0) && !xmlLocation.empty() )
	{
		ret = chdir(xmlLocation.c_str());
		if ( ret != 0 )
		{
			CsErr("Could not access XML directory '%s'", xmlLocation.c_str());
		}
	}

	if ( ret == 0 )
	{
		ret = MYKI_CD_processProductConfigurationXML( railTariff );
		if ( ret != 0 )
		{
			CsErrx("Could not import XML files: %d", ret);
		}
	}

	if ( dbAccess != 0 )
	{
		std::string updateQuery1 =
			"INSERT INTO CDVersion (major_version, minor_version, start_date) "
			"SELECT major_version, minor_version, start_date "
			"FROM Products "
			"WHERE major_version||':'||minor_version||':'||start_date NOT IN "
			"(SELECT major_version||':'||minor_version||':'||start_date FROM CDVersion)";
		std::string updateQuery2 =
			std::string("UPDATE CDVersion SET source_archive = '") + xmlSource +
			"' WHERE source_archive IS NULL";
		dbAccess->executeSqlUpdate(updateQuery1); // this is an update query not an insert as it may do 0 rows.
		dbAccess->executeSqlUpdate(updateQuery2);
		dbAccess->executeSqlUpdate("vacuum;");
		dbAccess->closeDatabaseConnection();
	}

	if ( ret == 0 )
		return 0;
	return 2;
}
