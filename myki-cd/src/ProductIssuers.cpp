/*
 * ProductIssuers.cpp
 *
 *  Created on: 04/10/2012
 *      Author: James Ho
 */

#include "ProductIssuers.h"

int processProductIssuersXML()
{
	int ret = processXML(
		"processProductIssuersXML", // description
		"ProductIssuers.xml",       // xml file
		"ProductIssuers",           // xml node
		"ProductIssuers",           // master table
		"ProductIssuerRecord");     // record table
	return ret;
}


int getProductIssuer(U8_t id, MYKI_CD_ProductIssuer_t &productIssuer)
{
    int returnValue=false;
	DbAccess 	*m_dbAccess;
	Rows rows;

	m_dbAccess =  getDBAccess();

	char s_query[200];
	sprintf(s_query, "SELECT * FROM ProductIssuerRecord WHERE id='%d' AND %s", int(id), getCurrentCDVersionWhere());

	m_dbAccess->executeSqlQuery(s_query, rows);


	Rows::const_iterator itr1;
	for(itr1 = rows.begin(); itr1 != rows.end(); ++itr1){
		const ColumnValue& cv = *itr1;
		productIssuer.id = convertStringToU8(cv["id"]);
		stringCopy(productIssuer.short_desc, cv["short_desc"], LEN_Short_Description_t);
		stringCopy(productIssuer.long_desc, cv["long_desc"], LEN_Long_Description_t);
	}

	// TEST: Print out results
	if(rows.empty())
	{
		//std::cout << "\nNo Product Issuer record found for id " << (int)id <<".\n";
        CsDebug(CD_DEBUG, (CD_DEBUG, "MYKI_CD:getProductIssuer: No Product Issuer record found for id %d.", id));
	}
	else
	{
        returnValue=true;
		//printProductIssuerMap(rows);
		//testPrintProductIssuerResult(id, productIssuer);
	}
    return returnValue;
}

