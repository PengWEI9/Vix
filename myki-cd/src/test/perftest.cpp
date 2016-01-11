#include "myki_cd.h"
#include "myki_actionlist.h"
#include "DbAccess.h"
#include "ProductConfigurationUtils.h"
#include <vector>
#include <sys/timeb.h>
#include <stdlib.h>

template <typename T> T convert(const char* s)
{
	return atoi(s);
}
template <> const char* convert(const char* s)
{
	return s;
}

template <typename ID_t, typename CD_t> int testTable(DbAccess* dbAccess, unsigned int maxCount, const char* desc, const char* idsQuery, int (*lookup)(ID_t, CD_t*))
{
	std::vector<ID_t> ids;
	Rows rows;
	dbAccess->executeSqlQuery(idsQuery, rows);

	for ( Rows::iterator row = rows.begin(); row != rows.end(); row++ )
	{
		if ( row->begin() != row->end() )
			ids.push_back(convert<ID_t>(row->begin()->second));
	}
	if ( ids.empty() )
	{
		ids.push_back(convert<ID_t>("0"));
	}

	unsigned int idCount = ids.size();
	CD_t mykiCd;

	double proportion = 0.005;
	std::vector<ID_t> selection;
	while ( selection.size() < maxCount )
	{
		ID_t x = ids[rand() % idCount];
		for ( unsigned int i = 0; i < (unsigned int)(proportion*maxCount) + 1; i++ )
		{
			selection.push_back(x);
		}
		proportion *= 0.995;
	}
	unsigned int selectionCount = selection.size();

	CsVerbose("Start %s", desc);
	struct timeb startTime;
	ftime(&startTime);
	unsigned int counter;
	for ( counter = 0; counter < maxCount; counter++ )
	{
		int index = (rand() % selectionCount);
		memset(&mykiCd, 0, sizeof(mykiCd));
		if ( lookup(selection[index], &mykiCd) == 0 )
		{
			CsErrx("%s: Lookup for '%u' failed", desc, (unsigned int)(selection[index]));
			return 3;
		}
	}
	struct timeb endTime;
	ftime(&endTime);
	CsVerbose("End %s", desc);

	int totalTime =
		(int(endTime.time) - int(startTime.time)) * 1000 +
		(int(endTime.millitm) - int(startTime.millitm));
	CsVerbose("Total time %s   %d ms", desc, totalTime);
	if ( counter != 0 )
		CsVerbose("Average time %s %0.3f ms", desc, double(totalTime)/double(counter));

	return 0;
}

int CitySaverLookup(U8_t zone, int* result)
{
	*result = MYKI_CD_isCitySaverZone(zone);
	return 1;
}

int DeviceHotlistLookup(U32_t samId, int* result)
{
	*result = MYKI_ACTIONLIST_isDeviceHotlisted(samId, 1420084800); // 2015-01-01 12:00:00 WST
	return 1;
}

int ActionlistLookup(const char* cardUID, MYKI_ACTIONLIST_TAppUpdateRequest_t* result)
{
	MYKI_ACTIONLIST_getTAppUpdateRequestBlock(cardUID, 1, result);
	return 1;
}

int LinesByStationLookup(U16_t stationId, MYKI_CD_LinesByStation_t* result)
{
	MYKI_CD_U16Array_t linesArray;
	linesArray.arraySize = 12345;
	linesArray.arrayOfU16 = (U16_t*)0;
	int ret = MYKI_CD_getLinesByStationStructure(stationId, result, &linesArray);
	if ( ret )
	{
		int blah = 0;
		for ( int i = 0; i < linesArray.arraySize; i++ )
			blah += linesArray.arrayOfU16[i];
	}
	return ret;
}

int StationsByLineLookup(U16_t lineId, MYKI_CD_StationsByLine_t* result)
{
	MYKI_CD_U16Array_t stationsArray;
	stationsArray.arraySize = 12345;
	stationsArray.arrayOfU16 = (U16_t*)0;
	int ret = MYKI_CD_getStationsByLineStructure(lineId, result, &stationsArray);
	if ( ret )
	{
		int blah = 0;
		for ( int i = 0; i < stationsArray.arraySize; i++ )
			blah += stationsArray.arrayOfU16[i];
	}
	return ret;
}

int main(int argc, char** argv)
{
	int maxCount = 100;
	if ( argc > 1 )
		maxCount = atoi(argv[1]);
	if ( maxCount <= 0 )
	{
		std::cerr << "Bad max count" << std::endl;
		return 1;
	}

	srand(time(0));
	if ( MYKI_CD_openCDDatabase(productCfgDBFile.c_str()) == 0 )
	{
		std::cerr << "Could not open database '" << productCfgDBFile << "'" << std::endl;
		return 2;
	}
	if ( MYKI_CD_openFullActionlistDatabase(defaultActionlistDBFile) == 0 )
	{
		std::cerr << "Could not open actionlist database '" << defaultActionlistDBFile << "'" << std::endl;
		return 3;
	}


	int ret = testTable<U16_t, MYKI_CD_Locations_t>(
		getDBAccess(),
		maxCount,
		"Locations",
		"select id from LocationRecord;",
		MYKI_CD_getLocationsStructure);
	if ( ret != 0 )
		return ret;
	ret = testTable<U8_t, MYKI_CD_PassengerType_t>(
		getDBAccess(),
		maxCount,
		"Passenger types",
		"select id from PassengerTypeRecord;",
		MYKI_CD_getPassengerTypeStructure);
	if ( ret != 0 )
		return ret;
	ret = testTable<U8_t, int>(
		getDBAccess(),
		maxCount,
		"City saver",
		"select physical from ZoneRecord;",
		CitySaverLookup);
	if ( ret != 0 )
		return ret;
	ret = testTable<U16_t, MYKI_CD_LinesByStation_t>(
		getDBAccess(),
		maxCount,
		"Lines by station",
		"select distinct station from StationLineRecord;",
		LinesByStationLookup);
	if ( ret != 0 )
		return ret;
	ret = testTable<U16_t, MYKI_CD_StationsByLine_t>(
		getDBAccess(),
		maxCount,
		"Stations by line",
		"select distinct line from StationLineRecord;",
		StationsByLineLookup);
	if ( ret != 0 )
		return ret;

	ret = testTable<U32_t, int>(
		getActionlistDBAccess(),
		maxCount,
		"Device hotlist",
		"select SamId from DeviceHotlistEntry;",
		DeviceHotlistLookup);
	if ( ret != 0 )
		return ret;
	ret = testTable<const char*, MYKI_ACTIONLIST_Card_Actionlist_Requests_t>(
		getActionlistDBAccess(),
		maxCount,
		"Card actionlist requests",
		"select PRINTF(\"%014X\", MAX(card_uid)) from Request r, 'Transaction' t WHERE t.FK_Request=r.Request_PK AND t.action_seq_no=1;",
		MYKI_ACTIONLIST_getCardActionlistRequests);
	if ( ret != 0 )
		return ret;
	ret = testTable<const char*, MYKI_ACTIONLIST_TAppUpdateRequest_t>(
		getActionlistDBAccess(),
		maxCount,
		"Application block actionlist",
		"select PRINTF(\"%014X\", MIN(card_uid)) from Request r, 'Transaction' t WHERE t.FK_Request=r.Request_PK AND t.action_seq_no=1;",
		ActionlistLookup);
	if ( ret != 0 )
		return ret;

	CsVerbose("CD start date = %d, start time = %d", int(MYKI_CD_getStartDate()), int(MYKI_CD_getStartDateTime()));
	return 0;
}
