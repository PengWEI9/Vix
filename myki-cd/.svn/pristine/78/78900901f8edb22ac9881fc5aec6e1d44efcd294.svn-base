#include "myki_actionlist.h"
#include "myki_cd.h"
#include "ProductConfigurationUtils.h"
#include "cs.h"
#include <string>


int ProcessXML(const std::string& dbFileLocation, const std::string& xmlLocation, const std::string& xmlSource)
{

	int ret = 0;

	if ( !dbFileLocation.empty() )
	{
		ret = chdir(dbFileLocation.c_str());
		if ( ret != 0 )
		{
			CsErr("Could not access db directory '%s'", dbFileLocation.c_str());
		} 
		else 
		{
			std::string fullDBFilename = dbFileLocation + defaultActionlistDBFile;
			std::string deltaDBFilename = dbFileLocation + defaultActionlistDeltaDBFile;
			if (MYKI_CD_openFullActionlistDatabase(fullDBFilename.c_str()) == false) {
				CsErr("Could not access actionlist db file '%s'", fullDBFilename.c_str());
				ret = -1;
			} else {
				if (MYKI_CD_openDeltaActionlistDatabase(deltaDBFilename.c_str()) == false) {
					CsErr("Could not access actionlist db file '%s'", deltaDBFilename.c_str());
					ret = -1;
				}
			}
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
		ret = MYKI_ACTIONLIST_processActionlistXML();
		if ( ret != 0 )
		{
			CsErrx("Could not import actionlist XML file: %d", ret);
		}
		int ret2 = MYKI_ACTIONLIST_processDeviceHotlistXML();
		if ( ret2 != 0 )
		{
			ret = ret2;
			CsErrx("Could not import device hotlist XML file: %d", ret);
		}
	}

/****************************************
	if ( dbAccess != 0 )
	{
		std::string updateQuery1 =
			std::string("UPDATE Actionlist SET source_archive = '") + xmlSource +
			"' WHERE source_archive IS NULL";
		std::string updateQuery2 =
			std::string("UPDATE DeviceHotlist SET source_archive = '") + xmlSource +
			"' WHERE source_archive IS NULL";
		dbAccess->executeSqlUpdate(updateQuery1);
		dbAccess->executeSqlUpdate(updateQuery2);
		dbAccess->executeSqlUpdate("vacuum;");
		dbAccess->closeDatabaseConnection();
	}
********************************************/

	if ( ret == 0 )
		return 0;
	return 2;
}
