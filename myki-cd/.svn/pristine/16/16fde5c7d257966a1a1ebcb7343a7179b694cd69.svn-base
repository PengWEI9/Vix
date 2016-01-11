/*
 * Products.cpp
 *
 *  Created on: 04/10/2012
 *      Author: James Ho
 */

#include "Products.h"
#include "pugixml.hpp"
#include "CdCache.h"
#include "datec19.h"
#include "datec20.h"

#include <json/json.h>

#include <string>
#include <set>

CdCache<U16_t, MYKI_CD_Product_t> g_productCache("Product", 100, CacheMemoryCopy<MYKI_CD_Product_t>);

int initProductsLookup()
{
	g_productCache.clear();
	return 1;
}

int processProductsSubtableXML(pugi::xpath_node node, int recordPK, std::vector<int>& subtablePKs)
{
	DbAccess* m_dbAccess = getDBAccess();
    if(m_dbAccess==NULL)
        return 2;      
	int ret = 0;

	// Get the Attribute Element Child belonging to each Record
	bool needPrimaryKeys = false;
	if (subtablePKs.empty())
	{
		needPrimaryKeys = true;
		subtablePKs.push_back(0);
		subtablePKs.push_back(0);
	}
	int& attributePK = subtablePKs[0];
	int& defaultAttributePK = subtablePKs[1];
	if (needPrimaryKeys)
	{
		Rows rows;
		m_dbAccess->executeSqlQuery("SELECT MAX(PrimaryKey) AS PrimaryKey FROM ProductAttribute", rows);
		if (!rows.empty())
			attributePK = convertStringToU32(rows.front()["PrimaryKey"]);
		rows.clear();
		m_dbAccess->executeSqlQuery("SELECT MAX(PrimaryKey) AS PrimaryKey FROM ProductDefaultAttribute", rows);
		if (!rows.empty())
			defaultAttributePK = convertStringToU32(rows.front()["PrimaryKey"]);
	}

	for (pugi::xml_node tool = node.node().first_child(); tool; tool = tool.next_sibling())
	{
		std::string childName(tool.name());

		if (childName.compare("Attribute") == 0)
		{
			std::string s_queryAttribute = "";
			std::string s_queryColumn = "";
			std::string s_queryValue = "";

			attributePK++;
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
			s_queryAttribute = "INSERT INTO ProductAttribute('PrimaryKey',";
			s_queryAttribute += s_queryColumn;
			s_queryAttribute += ",'FK_ProductRecord') VALUES ('";
			s_queryAttribute += static_cast<std::ostringstream*>( &(std::ostringstream() << attributePK) )->str(); // PK for Device Parameter Attribute
			s_queryAttribute += "',";
			s_queryAttribute += s_queryValue;
			s_queryAttribute += ",'";
			s_queryAttribute += static_cast<std::ostringstream*>( &(std::ostringstream() << recordPK) )->str(); // FK to Device Parameter Record
			s_queryAttribute += "'";
			s_queryAttribute += ")";

			// std::cout << "\n" << s_queryDeviceParameterAttribute;
		    if ( m_dbAccess->executeSqlInsert(s_queryAttribute) != 0 )
			{
				CsErrx("Query '%s' failed", s_queryAttribute.c_str());
				ret = 2;
			}
		}
		else if (childName.compare("DefaultAttribute") == 0)
		{
			std::string s_queryDefaultAttribute = "";
			std::string s_queryColumn = "";
			std::string s_queryValue = "";

			defaultAttributePK++;
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
			s_queryDefaultAttribute = "INSERT INTO ProductDefaultAttribute('PrimaryKey',";
			s_queryDefaultAttribute += s_queryColumn;
			s_queryDefaultAttribute += ",'FK_ProductRecord') VALUES ('";
			s_queryDefaultAttribute += static_cast<std::ostringstream*>( &(std::ostringstream() << defaultAttributePK) )->str(); // PK for Device Parameter Attribute
			s_queryDefaultAttribute += "',";
			s_queryDefaultAttribute += s_queryValue;
			s_queryDefaultAttribute += ",'";
			s_queryDefaultAttribute += static_cast<std::ostringstream*>( &(std::ostringstream() << recordPK) )->str(); // FK to Device Parameter Record
			s_queryDefaultAttribute += "'";
			s_queryDefaultAttribute += ")";

			// std::cout << "\n" << s_queryDeviceParameterAttribute;
		    if ( m_dbAccess->executeSqlInsert(s_queryDefaultAttribute) != 0 )
			{
				CsErrx("Query '%s' failed", s_queryDefaultAttribute.c_str());
				ret = 2;
			}
		}
	}

	return ret;
}

int processProductsXML()
{
	int ret = 0;

	const char* s_subtableDeleteQuery =
		"DELETE FROM ProductAttribute WHERE "
		"NOT FK_ProductRecord IN (SELECT PrimaryKey FROM ProductRecord); "
		"DELETE FROM ProductDefaultAttribute WHERE "
		"NOT FK_ProductRecord IN (SELECT PrimaryKey FROM ProductRecord);";

	ret = processXML(
		"processProductsXML", // description
		"Products.xml",       // xml file
		"Products",           // xml node
		"Products",           // master table
		"ProductRecord",      // record table
		processProductsSubtableXML,
		s_subtableDeleteQuery);

	initProductsLookup();
	return ret;
}

int getProduct(U8_t id, MYKI_CD_Product_t &product)
{
    int returnValue=false;
	DbAccess 	*m_dbAccess;
	Rows rows;
    
    CsDebug(CD_DEBUG, (CD_DEBUG, "MYKI_CD:getProduct %d", id));

	bool isNull = false;
    if(g_productCache.getCacheValue(id, product, isNull))
    {
        returnValue = !isNull;
		CsDebug(CD_DEBUG, (CD_DEBUG, "MYKI_CD:getProduct: Return Cache Product record found for id %d.", int(id)));
    }
	else
    {    
        m_dbAccess =  getDBAccess();
        if(m_dbAccess==NULL)
            return false;  
        char s_query[200];
        sprintf(s_query, "SELECT * FROM ProductRecord WHERE id='%d' AND %s", int(id), getCurrentCDVersionWhere());

        m_dbAccess->executeSqlQuery(s_query, rows);

        Rows::const_iterator itr1;
        for(itr1 = rows.begin(); itr1 != rows.end(); ++itr1){
            const ColumnValue& cv = *itr1;
            product.issuer_id = convertStringToU8(cv["issuer_id"]);
            product.id = convertStringToU8(cv["id"]);
            stringCopy(product.type, cv["type"], LEN_20_t);
            stringCopy(product.subtype, cv["subtype"], LEN_20_t);
            stringCopy(product.short_desc, cv["short_desc"], LEN_Short_Description_t);
            stringCopy(product.long_desc, cv["long_desc"], LEN_Long_Description_t);
        }



        // TEST: Print out results
        if(rows.empty())
        {
            //std::cout << "\nNo Product record found for id " << (int)id <<".\n";
            CsDebug(CD_DEBUG, (CD_DEBUG, "MYKI_CD:getProduct %d - no product exists", id));
        }
        else
        {
            CsDebug(CD_DEBUG, (CD_DEBUG, "MYKI_CD:getProduct %d - Get Attributes", id));
            // Get the product default attributes             // Get the product attributes
/*            if(getProductDefaultAttribute(id, product) && getProductAttribute(id, product))
            {
                CsDebug(CD_DEBUG, (CD_DEBUG, "MYKI_CD:getProduct %d - Return true", id));
                returnValue = true;
                //printProductMap(rows);
                //testPrintProductResult(id, product);
            }*/
            returnValue = true;
        }
        g_productCache.addCacheValue(id, product, !returnValue);
    }
    
    return returnValue;
}


int getProductType(U8_t id, char* type, int bufferLength)
{
	int returnValue=false;
	MYKI_CD_Product_t product;
	if (getProduct(id, product) == false)
	{
		CsDebug(CD_DEBUG, (CD_DEBUG, "MYKI_CD:getProductType: getProduct failed, returning fail"));
	}
	else
	{
		stringCopy(type, product.subtype, bufferLength);
		returnValue = true;
	}

    return returnValue;
}

int getProductCardType(U8_t id, char* type, int bufferLength)
{
   int returnValue=false;
	MYKI_CD_Product_t product;
	if (getProduct(id, product) == false)
	{
		CsDebug(CD_DEBUG, (CD_DEBUG, "MYKI_CD:getProductCardType: getProduct failed, returning fail"));
	}
	else
	{
		stringCopy(type, product.type, bufferLength);
		returnValue = true;
	}

    return returnValue;
}


int getProductDefaultAttribute(U8_t id, MYKI_CD_Product_t &product)
{
    int returnValue=false;
    
    CsDebug(CD_DEBUG, (CD_DEBUG, "MYKI_CD:getProductDefaultAttribute %d", id));
    
	DbAccess 	*m_dbAccess;
	Rows rows;

	m_dbAccess =  getDBAccess();
    if(m_dbAccess==NULL)
        return false;  
	char s_query[400];
	sprintf(s_query,
		"SELECT ProductDefaultAttribute.* FROM "
		"ProductDefaultAttribute "
		"INNER JOIN ProductRecord "
		"on ProductRecord.PrimaryKey = ProductDefaultAttribute.FK_ProductRecord "
		"WHERE ProductRecord.id='%d' "
		"AND ProductRecord.fk_major_version=%d AND ProductRecord.fk_minor_version=%d",
		int(id), getCurrentCDMajorVersion(), getCurrentCDMinorVersion());

	returnValue = m_dbAccess->executeSqlQuery(s_query, rows)==SQLITE_OK;


//	Rows::const_iterator itr2;
//	product.productDefaultAttribute.resize(rows.size());
//	int i = 0;
//	for(itr2 = rows.begin(); itr2 != rows.end(); ++itr2){
//		MYKI_CD_ProductDefaultAttribute_t productDefaultAttribute;
//		const ColumnValue& cv = *itr2;
//		productDefaultAttribute.name = cv["name"];
//		productDefaultAttribute.value = cv["value"];
//
//		product.productDefaultAttribute[i] = productDefaultAttribute;
//		i++;
//	}

	// printProductMap(rows);
    
    CsDebug(CD_DEBUG, (CD_DEBUG, "MYKI_CD:getProductDefaultAttribute return %d", returnValue));
    return returnValue;    
}

int getProductAttribute(U8_t id, MYKI_CD_Product_t &product)
{
    int returnValue=false;  
	DbAccess 	*m_dbAccess;
	Rows rows;

    CsDebug(CD_DEBUG, (CD_DEBUG, "MYKI_CD:getProductAttribute %d", id));
    
	m_dbAccess =  getDBAccess();
    if(m_dbAccess==NULL)
        return false;    

	char s_query[400];
	sprintf(s_query,
		"SELECT ProductAttribute.* FROM "
		"ProductAttribute "
		"INNER JOIN ProductRecord "
		"on ProductRecord.PrimaryKey = ProductAttribute.FK_ProductRecord "
		"WHERE ProductRecord.id='%d' "
		"AND ProductRecord.fk_major_version=%d AND ProductRecord.fk_minor_version=%d",
		int(id), getCurrentCDMajorVersion(), getCurrentCDMinorVersion());

	returnValue = m_dbAccess->executeSqlQuery(s_query, rows)==SQLITE_OK;

//	Rows::const_iterator itr2;
//	product.productAttribute.resize(rows.size());
//	int i = 0;
//	for(itr2 = rows.begin(); itr2 != rows.end(); ++itr2){
//		MYKI_CD_ProductAttribute_t productAttribute;
//		const ColumnValue& cv = *itr2;
//		productAttribute.name = cv["name"];
//		productAttribute.value = cv["value"];
//
//		product.productAttribute[i] = productAttribute;
//		i++;
//	}

	// printProductMap(rows);
    
    
    CsDebug(CD_DEBUG, (CD_DEBUG, "MYKI_CD:getProductAttribute return %d", returnValue));
    return returnValue;
}

    /**
     *  @brief  Checks product attribute.
     *  @param  isIncluded true if included; false otherwise.
     *  @param  valueToCheck attribute value to check against.
     *  @param  value resulted attribute value from query.
     */
static
void
checkProductIncluded( bool &isIncluded, U32_t valueToCheck, std::string &value )
{
    if ( value == "ALL" )
    {
        isIncluded  = true;
    }
    else
    if ( value == "NONE" )
    {
        isIncluded  = false;
    }
    else
    if ( convertStringToU32( value ) == valueToCheck )
    {
        isIncluded  = isIncluded == false ? true : false;
    }
}

    /**
     *  @brief  Checks product attribute.
     *  @param  isIncluded true if included; false otherwise.
     *  @param  valueToCheck attribute value to check against.
     *  @param  value resulted attribute value from query.
     */
static
void
checkProductIncluded( bool &isIncluded, const char *valueToCheck, std::string &value )
{
    if ( value == "ALL" )
    {
        isIncluded  = true;
    }
    else
    if ( value == "NONE" )
    {
        isIncluded  = false;
    }
    else
    if ( valueToCheck != NULL && value == valueToCheck )
    {
        isIncluded  = isIncluded == false ? true : false;
    }
}

    /**
     *  @brief  Checks product start date attribute.
     *  @param  isIncluded true if included; false otherwise.
     *  @param  valueToCheck attribute value to check against.
     *  @param  value resulted attribute value from query.
     */
static
void
checkProductIncludedStartDate( bool &isIncluded, U32_t valueToCheck, std::string &value )
{
    if ( value == "ALL" )
    {
        isIncluded  = true;
    }
    else
    if ( value == "NONE" )
    {
        isIncluded  = false;
    }
    else
    {
        struct tm       ctm     = { 0 };
        DateC19_t       date    = 0;

        sscanf( value.c_str(), "%d-%d-%d", &ctm.tm_year, &ctm.tm_mon, &ctm.tm_mday );
        ctm.tm_year    -= 1900;
        ctm.tm_mon     -= 1;
        date            = mkdate( &ctm );

        if ( date <= valueToCheck )
        {
            isIncluded  = isIncluded == false ? true : false;
        }
    }
}

    /**
     *  @brief  Checks product end date attribute.
     *  @param  isIncluded true if included; false otherwise.
     *  @param  valueToCheck attribute value to check against.
     *  @param  value resulted attribute value from query.
     */
static
void
checkProductIncludedEndDate( bool &isIncluded, U32_t valueToCheck, std::string &value )
{
    if ( value == "ALL" )
    {
        isIncluded  = true;
    }
    else
    if ( value == "NONE" )
    {
        isIncluded  = false;
    }
    else
    {
        struct tm       ctm     = { 0 };
        DateC19_t       date    = 0;

        sscanf( value.c_str(), "%d-%d-%d", &ctm.tm_year, &ctm.tm_mon, &ctm.tm_mday );
        ctm.tm_year    -= 1900;
        ctm.tm_mon     -= 1;
        date            = mkdate( &ctm );

        if ( date >= valueToCheck )
        {
            isIncluded  = isIncluded == false ? true : false;
        }
    }
}

    /**
     *  @brief  Checks product time range attribute.
     *  @param  isIncluded true if included; false otherwise.
     *  @param  valueToCheck attribute value to check against.
     *  @param  value resulted attribute value from query.
     */
static
void
checkProductIncludedTime( bool &isIncluded, U32_t valueToCheck, std::string &value )
{
    if ( value == "ALL" )
    {
        isIncluded  = true;
    }
    else
    if ( value == "NONE" )
    {
        isIncluded  = false;
    }
    else
    {
        U32_t   fromHHMM    = 0;
        U32_t   toHHMM      = 0;

        sscanf( value.c_str(), "%d|%d", &fromHHMM, &toHHMM );
        if ( valueToCheck >= fromHHMM && valueToCheck <= toHHMM )
        {
            isIncluded  = isIncluded == false ? true : false;
        }
    }
}

    /**
     *  @brief  Retrieves saleable product details given search
     *          criteria.
     *  This (C++) function returns saleable product details that
     *  satisfy the specified search criteria.
     *  @note   Products with product price of zero are excluded
     *          from the saleable product list.
     *  @param  products returned Json:Value object containing
     *          saleable product details. The returned JSON object
     *          has the following format,
     *  @code
     *  {
     *    "products":
     *    [
     *      {
     *        "GSTApplicable":false,
     *        "PLU":"4. myki Child",
     *        "Price":50,
     *        "id":46,
     *        "issuer_id":1,
     *        "long_desc":"GTS Timetable",
     *        "short_desc":"GTS Timetable",
     *        "subtype":"None",
     *        "type":"ThirdParty"
     *      },
     *      { ... },
     *      null
     *    ]
     *  }
     *  @endcode
     *  where,
     *  @li     @c id is the product id (1-255).
     *  @li     @c issuer_id is the product issuer/owner id.
     *  @li     @c type is the product type, eg. "ThirdParty", "PremiumSurcharge".
     *  @li     @c subtype is the product sub-type, eg. "None".
     *  @li     @c long_desc is the product long description.
     *  @li     @c short_desc is the product short description.
     *  @li     @c Price is the product price in lowest denomination (cents).
     *  @li     @c GSTApplicable is true if GTS is applicable, false otherwise.
     *  @li     @c PLU is (optional) 'Price Lookup Unit' uniquely identifies product.
     *  @param  zone zone to validate against sales zones.
     *  @param  productType product type to search for (eg. "ThirdParty").
     *  @param  time time to validate against.
     *  @param  serviceProviderId service provider id.
     *  @param  deviceType device type (eg. "BDC").
     *  @return true if successful; false otherwise.
     */
int
getProducts(
    Json::Value    &products,
    int             zone,
    const char     *productType,
    Time_t          time,
    int             serviceProviderId,
    const char     *deviceType )
{
    #define MYKI_CD_DOW_SUNDAY      7

    int             returnValue     = false;
    DateC19_t       date            = 0;
    int             dayOfWeek       = 0;
    Time_t          timeHHMM        = 2400;
    struct tm       localTm         = { 0 };

    CsDbg( CD_DEBUG, "MYKI_CD:getProducts" );

    if ( time == TIME_NOT_SET )
    {
        CsErrx( "MYKI_CD:getProducts : time not set" );
        return  false;
    }

    /*  Creates an empty "products" array */
    products[ "products" ]  = Json::Value( Json::arrayValue );

    localtime_r( (time_t*)&time, &localTm );
    date        = mkdate( &localTm );
    dayOfWeek   = ( localTm.tm_wday == 0 /*SUNDAY*/ ? MYKI_CD_DOW_SUNDAY : localTm.tm_wday );
    timeHHMM    = ( localTm.tm_hour * 100 ) + localTm.tm_min;

    try
    {
        std::stringstream   query1;
        std::string         query;
        const char         *pQuery          = NULL;
        Rows                productRecords;
        DbAccess           *m_dbAccess      = getDBAccess();

        if ( m_dbAccess == NULL )
        {
            /*  Failed opening Tariff database */
            throw   __LINE__;
        }

        /*  Retrieves all products matching given product type */
        query1  << "SELECT  * "
                   "FROM    ProductRecord "
                   "WHERE   ProductRecord.fk_major_version= " << getCurrentCDMajorVersion() << " "
                       "AND ProductRecord.fk_minor_version= " << getCurrentCDMinorVersion() << " ";
        if ( productType != NULL )
        {
            query1  << "AND ProductRecord.type LIKE '" << productType << "' ";
        }
        query   = query1.str();
        pQuery  = query.c_str();
        if ( m_dbAccess->executeSqlQuery( pQuery, productRecords ) != SQLITE_OK )
        {
            /*  Failed executing query */
            throw   __LINE__;
        }

        for ( Rows::const_iterator itrProduct = productRecords.begin();
                itrProduct != productRecords.end();
                ++itrProduct )
        {
            const ColumnValue  &productRecord           = *itrProduct;
            std::string         productPK               = productRecord[ "PrimaryKey" ];
            std::string         productType             = productRecord[ "type"       ];
            std::stringstream   query2;
            Rows                attrRecords;
            bool                allowSalesChannel       = false;
            bool                allowServiceProvider    = false;
            bool                allowStartDate          = productType == "ThirdParty" ? true : false;
            bool                allowEndDate            = productType == "ThirdParty" ? true : false;
            bool                allowDayOfWeek          = false;
            bool                allowTime               = false;
            bool                allowSaleZone           = false;
            std::string         PLU;
            U32_t               productPrice            = 0;
            bool                GSTApplicable           = false;

            /*  Retrieves all default and specific attribute name/value pairs */
            query2  << "SELECT      1 AS is_default, name, value "
                       "FROM        ProductDefaultAttribute "
                       "WHERE       ProductDefaultAttribute.FK_ProductRecord=" << productPK << " "
                       "UNION "
                       "SELECT      0 AS is_default, name, value "
                       "FROM        ProductAttribute "
                       "WHERE       ProductAttribute.FK_ProductRecord=" << productPK << " "
                       "ORDER   BY  name, is_default DESC ";
            query   = query2.str();
            pQuery  = query.c_str();
            if ( m_dbAccess->executeSqlQuery( pQuery, attrRecords ) != SQLITE_OK )
            {
                /*  Failed executing query */
                throw   __LINE__;
            }

            /*  Performs validity check on saleable product */
            for ( Rows::const_iterator itrAttr = attrRecords.begin();
                    itrAttr != attrRecords.end();
                    ++itrAttr )
            {
                const ColumnValue  &attrRecord  = *itrAttr;
                std::string         attrName    = attrRecord[ "name"  ];
                std::string         attrValue   = attrRecord[ "value" ];
                const char         *pAttrName   = attrName.c_str();
                const char         *pAttrValue  = attrValue.c_str();

                if      ( attrName == "SalesChannel"    )   { checkProductIncluded(          allowSalesChannel,    deviceType,        attrValue ); }
                else if ( attrName == "ServiceProvider" )   { checkProductIncluded(          allowServiceProvider, serviceProviderId, attrValue ); }
                else if ( attrName == "SalesZone"       )   { checkProductIncluded(          allowSaleZone,        zone,              attrValue ); }
                else if ( attrName == "StartDate"       )   { checkProductIncludedStartDate( allowStartDate,       date,              attrValue ); }
                else if ( attrName == "EndDate"         )   { checkProductIncludedEndDate(   allowEndDate,         date,              attrValue ); }
                else if ( attrName == "DayOfWeek"       )   { checkProductIncluded(          allowDayOfWeek,       dayOfWeek,         attrValue ); }
                else if ( attrName == "TimeOfDay"       )   { checkProductIncludedTime(      allowTime,            timeHHMM,          attrValue ); }
                else if ( attrName == "PLU"             )   { PLU           =                                                         attrValue  ; }
                else if ( attrName == "Price"           )   { productPrice  = convertStringToU32(                                     attrValue ); }
                else if ( attrName == "GSTApplicable"   )   { GSTApplicable = convertStringToBool(                                    attrValue ); }
            }

            if      ( allowSalesChannel    == false ) { CsDbg( CD_DEBUG, "MYKI_CD:getProducts : excluded device type"       ); }
            else if ( allowServiceProvider == false ) { CsDbg( CD_DEBUG, "MYKI_CD:getProducts : excluded service provider"  ); }
            else if ( allowSaleZone        == false ) { CsDbg( CD_DEBUG, "MYKI_CD:getProducts : excluded sales zone"        ); }
            else if ( allowStartDate       == false ) { CsDbg( CD_DEBUG, "MYKI_CD:getProducts : not yet available"          ); }
            else if ( allowEndDate         == false ) { CsDbg( CD_DEBUG, "MYKI_CD:getProducts : no longer available"        ); }
            else if ( allowDayOfWeek       == false ) { CsDbg( CD_DEBUG, "MYKI_CD:getProducts : not available on this day"  ); }
            else if ( allowTime            == false ) { CsDbg( CD_DEBUG, "MYKI_CD:getProducts : not available at this time" ); }
            else if ( productPrice         == 0     ) { CsDbg( CD_DEBUG, "MYKI_CD:getProducts : zero product price"         ); }
            else
            {
                Json::Value     productValue;
                std::string     productId           = productRecord[ "id" ];
                std::string     productShortDesc    = productRecord[ "short_desc" ];

                CsDbg( CD_DEBUG, "MYKI_CD:getProducts : Add '%d-%s'",
                        productId.c_str(),
                        productShortDesc.c_str() );

                /*  Creates saleable product object */
                productValue[ "id"            ] = convertStringToU32( productRecord[ "id"        ] );
                productValue[ "issuer_id"     ] = convertStringToU32( productRecord[ "issuer_id" ] );
                productValue[ "type"          ] = productRecord[ "type"       ];
                productValue[ "subtype"       ] = productRecord[ "subtype"    ];
                productValue[ "short_desc"    ] = productRecord[ "short_desc" ];
                productValue[ "long_desc"     ] = productRecord[ "long_desc"  ];
                if ( PLU.empty() == false )
                {
                    productValue[ "PLU"       ] = PLU;
                }
                productValue[ "Price"         ] = productPrice;
                productValue[ "GSTApplicable" ] = GSTApplicable;

                /*  And adds to saleable product list */
                products[ "products" ].append( productValue );
            }
        }
        returnValue = true;
    }
    catch ( int e )
    {
        /*  Failed accessing Tariff database */
        returnValue = false;
    }

    CsDbg( CD_DEBUG, "MYKI_CD:getProducts returns %s", returnValue == false ? "FALSE" : "TRUE" );
    return  returnValue;
}
