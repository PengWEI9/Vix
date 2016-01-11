/*
 * HMIMessaging.cpp
 *
 *  Created on: 17/12/2014
 *      Author: Edward Hutchison
 */

#include "HMIMessaging.h"
#include "CdCache.h"


int processHMIMessagingXML()
{
	int ret = processXML(
		"HMIMessagingXML", // description
		"HMIMessaging.xml",       // xml file
		"HMIMessaging",           // xml node
		"HMIMessaging",           // master table
		"HMIMessagingRecord");     // record table
	return ret;
}


int getHMIMessaging(char* tableType, MYKI_CD_HMIMessaging_t *pHMIMessaging, U16_t arrayLength)
{
    int returnValue=false;
    bool isNull=false;
    int rowNumber =0;

    DbAccess 	*m_dbAccess;
    Rows rows;

    m_dbAccess =  getDBAccess();
    if(m_dbAccess==NULL)
        return 0;  
    
    char s_query[256];
    snprintf(s_query, 255, "SELECT * FROM HMIMessagingRecord where TableType = '%s' AND %s order by CAST(priority as number) asc", tableType, getCurrentCDVersionWhere());

    m_dbAccess->executeSqlQuery(s_query, rows);

    Rows::const_iterator itr1;
    for(itr1 = rows.begin(); itr1 != rows.end() && rowNumber<arrayLength; ++itr1)
    {
        const ColumnValue& cv = *itr1;
        pHMIMessaging[rowNumber].priority = convertStringToU16(cv["Priority"]);
        stringCopy(pHMIMessaging[rowNumber].fieldKey1, cv["TableType"], LEN_15_t);
        stringCopy(pHMIMessaging[rowNumber].fieldKey1, cv["FieldKey1"], LEN_15_t);
        stringCopy(pHMIMessaging[rowNumber].fieldKey2, cv["FieldKey2"], LEN_15_t);
        stringCopy(pHMIMessaging[rowNumber].fieldKey3, cv["FieldKey3"], LEN_15_t);
        stringCopy(pHMIMessaging[rowNumber].fieldKey4, cv["FieldKey4"], LEN_15_t);
        stringCopy(pHMIMessaging[rowNumber].fieldKey5, cv["FieldKey5"], LEN_15_t);
        stringCopy(pHMIMessaging[rowNumber].fieldKey6, cv["FieldKey6"], LEN_15_t);           
        stringCopy(pHMIMessaging[rowNumber].fieldKey7, cv["FieldKey7"], LEN_15_t);
        stringCopy(pHMIMessaging[rowNumber].fieldKey8, cv["FieldKey8"], LEN_15_t);

        pHMIMessaging[rowNumber].effect1 = convertStringToBool(cv["effect1"]);;
         
        stringCopy(pHMIMessaging[rowNumber].textToDisplay, cv["TextToDisplay"], LEN_50_t);
        
        rowNumber++;
    }
    
    return rowNumber;
}
