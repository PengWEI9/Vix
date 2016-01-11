/* -*- mode: c; tabs: 4; -*- */
/*=============================================================================
**
**    Vix Technology                   Licensed software
**    (C) 2014                         All rights reserved
**
**=============================================================================
**
**  Project/Product : MBU
**  Filename        : Actionlist.c
**  Author(s)       : James Ho
**
**  Description     :
**
**      +---------------+   +-------------------+
**      | Request       |   | Transaction       |   +-----------------------+
**      |===============|   |===================|   | TransactionType       |
**      | Request_PK*   |<--| FK_Request*       |   |=======================|
**      | card_uid      |   | FK_TransactionTyp |-->| TransactionType_PK*   |
**      | type          |   | action_seq_no     |   | type                  |                     
**      | expiry        |   +-------------------+   | subtype               |                     
**      | action_guid   |                           | field_name            |
**      | action_guid2  |                           | field_value           |
**      | action_guid3  |                           +-----------------------+
**      | action_guid4  |
**      | action_guid5  |
**      | FK_Actionlist |
**      +---------------+
**
**  Member(s)       :
**      doCardQuery             [public]    query database for actionlist records
**      doCardQueryRefreshCache [static]    build cache of actionlist entries for the card
**
**  Information     :
**   Compiler(s)    : C
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
**
**===========================================================================*/

/*
 *      Includes
 *      --------
 */

#include "Actionlist.h"
#include "pugixml.hpp"
#include <ud_api.h>
#include <algorithm>
#include <set>

#include "myki_time_conversion.h"

/*
 *      Local Constants and Macros
 *      --------------------------
 */

#define JULIEN_BASE_FOR_DATEC19_2006        75241


char                                        g_cachedCardUID[ LEN_CardUid_t + 1 ]    = {'\0'};
Rows                                        g_cachedCardActionlistRows;
MYKI_ACTIONLIST_Card_Actionlist_Requests_t  g_cachedCardActionlistRequests          = {{'\0'}};
int                                         g_cachedCardActionlistCapacity          = 0;
int                                         g_cachedActionlistSequence              = -1;
std::string                                 g_cachedActionlistSequenceInfo;
std::string                                 g_cachedActionlistDetails;

// static  int                                 g_curActionListDb                       = ActionListDelta;  /**< Current searching Actionlist database */

int initActionlistLookup()
{
	memset(g_cachedCardUID, 0, sizeof(g_cachedCardUID));
	memset(g_cachedCardActionlistRequests.card_uid, 0, sizeof(g_cachedCardActionlistRequests.card_uid));
	g_cachedActionlistSequence      = -1;
	g_cachedActionlistSequenceInfo  = "";
    g_cachedActionlistDetails       = "";
	return 1;
}


/*
 * Process and imports an actionlist file into the actionlist database.
 */
struct TransactionType
{
	std::string type;
	std::string subtype;
	std::string fieldName;
	std::string fieldValue;
	int primaryKey;
	bool operator < (const TransactionType& other) const
	{
		if ( type < other.type )
			return true;
		if ( other.type < type )
			return false;
		if ( subtype < other.subtype )
			return true;
		if ( other.subtype < subtype )
			return false;
		if ( fieldName < other.fieldName )
			return true;
		if ( other.fieldName < fieldName )
			return false;
		return (fieldValue < other.fieldValue);
	}
};

/*==========================================================================*
**
**  ClearActionlistDatabase
**
**  Description     :
**      Removes all records in actionlist database.
**
**  Parameters      :
**      pDbAccess           [I/O]   Actionlist database object
**
**  Returns         :
**      None
**
**  Notes           :
**
**==========================================================================*/

static  void    ClearActionlistDatabase( DbAccess *pDbAccess )
{
    if ( pDbAccess != NULL )
    {
        std::string s_query = "DELETE FROM 'Actionlist'";
        pDbAccess->executeSqlDelete( s_query );

        s_query = "DELETE FROM 'Request'";
        pDbAccess->executeSqlDelete( s_query );

        s_query = "DELETE FROM 'Transaction'";
        pDbAccess->executeSqlDelete( s_query );

        s_query = "DELETE FROM 'TransactionType'";
        pDbAccess->executeSqlDelete( s_query );
    }   /*  end-of-if */
}   /*  ClearActionlistDatabase( ) */

/*==========================================================================*
**
**  processActionlistXML
**
**  Description     :
**      Imports Actionlist XML to Full or Delta Actionlist database.
**
**  Parameters      :
**      None
**
**  Returns         :
**      MYKI_ACTIONLIST_SUCCESS     successful
**      MYKI_ACTIONLIST_FAILED      failed
**
**  Notes           :
**
**==========================================================================*/

int processActionlistXML( void )
{
	DbAccess               *m_dbAccess  = NULL;

	using namespace pugi;
	pugi::xml_document      doc;
	pugi::xml_parse_result  result      = doc.load_file( "actionlist.xml" );
	int                     ret         = MYKI_ACTIONLIST_SUCCESS;

	if ( !result )
	{
		// Only a warning - alternate version of success
		CsWarnx("processActionlistXML: Could not open 'actionlist.xml', no actionlists updated");
	}
	else
	{
		int                         actionlistPK        = 0;
		int                         requestPK           = 0;
		int                         transactionTypePK   = 0;
		std::set<TransactionType>   transactionTypes;

		// Get the Header
		pugi::xpath_node_set tools = doc.select_nodes("/TransactionRequests");
		for
        (
            pugi::xpath_node_set::const_iterator it = tools.begin();
            it != tools.end();
            ++it
        )
		{
			pugi::xpath_node        node            = *it;
			std::string             s_queryHeader   = "";
			std::string             s_queryColumn   = "";
			std::string             s_queryValue    = "";
			bool                    isDelta         = false;
			std::string             sequence;
			std::string             deltaSequence;
			int                     attributeCount  = 0;

			// Get the attributes belonging to the Header
			for
            (
                pugi::xml_attribute attr = node.node().first_attribute();
                attr;
				attr = attr.next_attribute()
            )
            {
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
				s_queryColumn += str;

				std::string value(attr.value());
				std::transform(str.begin(), str.end(), str.begin(), toupper);
				if ( (str == "ISDELTA") || (str == "IS_DELTA") )
				{
					std::transform(value.begin(), value.end(), value.begin(), toupper);
					if ( (value == "TRUE") || (value == "1") )
					{
						isDelta = true;
						value = "1";
					}
					else
					{
						value = "0";
					}
				}
				if ( str == "SEQUENCE" )
					sequence = value;
				if ( str == "DELTA_SEQUENCE" )
					deltaSequence = value;

				s_queryColumn.append("'");
				s_queryValue += sqlify(value);
				attributeCount++;
			}

            if ( ( m_dbAccess =  getActionlistDBAccess( isDelta ) ) == NULL )
            {
                CsErrx( "processActionlistXML: Could not get %s Actionlist database", isDelta ? "Delta" : "Full" );
                ret = MYKI_ACTIONLIST_FAILED;
            }
            else
            {
                CsVerbose("processActionlistXML: file is %sa delta", (isDelta ? "" : "not "));
                if ( !isDelta )
                {
                    /*  Importing Full Actionlist => Removes ALL Actionlist records */
                    ClearActionlistDatabase( m_dbAccess );
                    ClearActionlistDatabase( getActionlistDBAccess( true ) );
                }
                else
                {

                    /*  Importing Delta Actionlist => Checks if Delta Actionlist has been imported */
                    std::string sequenceTest;
                    std::string query = "SELECT 1 AS Test FROM 'Actionlist' WHERE sequence = '";
                    query += sequence + "' AND delta_sequence = '" + deltaSequence + "'";
                    if ( m_dbAccess->executeSqlQuerySingleItem(query.c_str(), sequenceTest) == SQLITE_OK )
                    {
                        if ( sequenceTest == "1" )
                        {
                            CsWarnx("Already imported delta %s/%s", sequence.c_str(), deltaSequence.c_str());
                            ret = MYKI_ACTIONLIST_FAILED;
                        }
                    }

                }
            }   /*  end-of-if */

			if ( isDelta && (ret == MYKI_ACTIONLIST_SUCCESS) )
			{
				std::string s_primaryKey;
				if ( m_dbAccess->executeSqlQuerySingleItem(
						"SELECT MAX(PrimaryKey) AS PrimaryKey FROM 'Actionlist'",
						s_primaryKey) == SQLITE_OK )
				{
					if ( !s_primaryKey.empty() )
						actionlistPK = convertStringToU32(s_primaryKey);
				}

				if ( m_dbAccess->executeSqlQuerySingleItem(
						"SELECT MAX(Request_PK) AS PrimaryKey FROM 'Request'",
						s_primaryKey) == SQLITE_OK )
				{
					if ( !s_primaryKey.empty() )
						requestPK = convertStringToU32(s_primaryKey);
				}

				if ( m_dbAccess->executeSqlQuerySingleItem(
						"SELECT MAX(TransactionType_PK) AS PrimaryKey FROM 'TransactionType'",
						s_primaryKey) == SQLITE_OK )
				{
					if ( !s_primaryKey.empty() )
						transactionTypePK = convertStringToU32(s_primaryKey);
				}

				Rows rows;
				if ( m_dbAccess->executeSqlQuery("SELECT * from 'TransactionType'", rows) == SQLITE_OK )
				{
					for ( Rows::iterator row = rows.begin(); row != rows.end(); ++row )
					{
						const ColumnValue& cv = *row;
						TransactionType t;
						t.type       = cv["type"];
						t.subtype    = cv["subtype"];
						t.fieldName  = cv["field_name"];
						t.fieldValue = cv["field_value"];
						t.primaryKey = atoi(cv["TransactionType_PK"]);
						transactionTypes.insert(t);
					}
				}
			}

			if ( ret == MYKI_ACTIONLIST_SUCCESS )
			{

				actionlistPK++;
				s_queryHeader += "INSERT INTO Actionlist('PrimaryKey',";
				s_queryHeader += s_queryColumn;
				s_queryHeader += ") VALUES ('";
				s_queryHeader += static_cast<std::ostringstream*>(&(std::ostringstream() << actionlistPK))->str(); // PK for Actionlist
				s_queryHeader += "',";
				s_queryHeader += s_queryValue;
				s_queryHeader += ")";

				CsVerbose(":SQLINSERT: %s;", s_queryHeader.c_str());
			    if ( m_dbAccess->executeSqlInsert(s_queryHeader) != 0 )
				{
					CsErrx("Query '%s' failed", s_queryHeader.c_str());
					ret = MYKI_ACTIONLIST_FAILED;
				}

			}

            /*  NOTE:   There should only be one "TransactionRequests" document root element */
		}

		// Get the Request
		pugi::xpath_node_set requestTools = doc.select_nodes("/TransactionRequests/Request");
		for
        (
            pugi::xpath_node_set::const_iterator it = requestTools.begin();
            ( ret == MYKI_ACTIONLIST_SUCCESS ) && ( it != requestTools.end() );
            ++it
        )
		{
			requestPK++;
		    pugi::xpath_node node = *it;

			std::string s_queryRecord = "";
			std::string s_queryColumn = "";
			std::string s_queryValue = "";
			int attributeCount = 0;

			// Get the attributes belonging to the Record
		    for (pugi::xml_attribute attr = node.node().first_attribute(); attr; attr = attr.next_attribute())
		    {
		    	if(attributeCount > 0)
		    	{
		    		s_queryColumn += ",";
		    		s_queryValue += ",";
		    	}
		    	s_queryColumn.append("'");
		    	s_queryColumn += attr.name();
		    	s_queryColumn.append("'");
				std::string attrName = attr.name();
				if ( attrName == "expiry" )
				{
					struct tm tmBuf;
					memset(&tmBuf, 0, sizeof(tmBuf));
					sscanf(attr.value(), "%d-%d-%dT%d:%d:%d",
						&tmBuf.tm_year,
						&tmBuf.tm_mon,
						&tmBuf.tm_mday,
						&tmBuf.tm_hour,
						&tmBuf.tm_min,
						&tmBuf.tm_sec);
					tmBuf.tm_year -= 1900;
					char buf[64];
					sprintf(buf, "%d", int(mktime(&tmBuf)));
					s_queryValue += buf;
				}
				else if ( attrName == "type" )
				{
					std::string attrValue = attr.value();
					if ( attrValue == "Addition" )
						s_queryValue += static_cast<std::ostringstream*>(&(std::ostringstream() << MYKI_ACTIONLIST_REQUEST_TYPE_ADDITION))->str();
					else
						s_queryValue += static_cast<std::ostringstream*>(&(std::ostringstream() << MYKI_ACTIONLIST_REQUEST_TYPE_UNKNOWN))->str();
				}
				else if ( attrName == "card_uid" )
				{
					U64_t cardUid = 0;
					sscanf(attr.value(), "%llX", &cardUid);
					s_queryValue += static_cast<std::ostringstream*>(&(std::ostringstream() << cardUid))->str();
				}
				else if ( attrName == "action_guid" )
				{
					U64_t guid1 = 0, guid2 = 0, guid3 = 0, guid4 = 0, guid5 = 0;
					sscanf(attr.value(), "%llX-%llX-%llX-%llX-%llX", &guid1, &guid2, &guid3, &guid4, &guid5);
					s_queryColumn += ",action_guid2,action_guid3,action_guid4,action_guid5";
					s_queryValue += static_cast<std::ostringstream*>(&(std::ostringstream() << guid1))->str();
					s_queryValue += ",";
					s_queryValue += static_cast<std::ostringstream*>(&(std::ostringstream() << guid2))->str();
					s_queryValue += ",";
					s_queryValue += static_cast<std::ostringstream*>(&(std::ostringstream() << guid3))->str();
					s_queryValue += ",";
					s_queryValue += static_cast<std::ostringstream*>(&(std::ostringstream() << guid4))->str();
					s_queryValue += ",";
					s_queryValue += static_cast<std::ostringstream*>(&(std::ostringstream() << guid5))->str();
				}
				else
				{
			    	s_queryValue += sqlify(attr.value());
				}
		    	attributeCount++;
		    }
		    s_queryRecord += "INSERT INTO Request(Request_PK,";
		    s_queryRecord += s_queryColumn;
		    s_queryRecord += ",FK_Actionlist)VALUES(";
		    s_queryRecord += static_cast<std::ostringstream*>( &(std::ostringstream() << requestPK) )->str(); // PK for Request
		    s_queryRecord += ",";
		    s_queryRecord += s_queryValue;
		    s_queryRecord += ",";
		    s_queryRecord += static_cast<std::ostringstream*>( &(std::ostringstream() << actionlistPK) )->str(); // FK to Actionlist
		    s_queryRecord += ")";

			CsVerbose(":SQLINSERT: %s;", s_queryRecord.c_str());
		    if ( m_dbAccess->executeSqlInsert(s_queryRecord) != 0 )
			{
				CsErrx("Query '%s' failed", s_queryRecord.c_str());
				ret = MYKI_ACTIONLIST_FAILED;
			}

		    // Get the Transaction
			for
            (
                pugi::xml_node transactionNode = node.node().first_child();
                ( ret == MYKI_ACTIONLIST_SUCCESS ) && transactionNode;
                transactionNode = transactionNode.next_sibling()
            )
			{
				int fkTransactionType;
				int actionSeqNo;
				TransactionType transactionType;

				// Get the attributes belonging to the Transaction
				for (pugi::xml_attribute attr = transactionNode.first_attribute(); attr; attr = attr.next_attribute())
				{
					std::string str(attr.name());
					if (str.compare(0,6,"xmlns:") == 0)
					{
						str.replace(0,6,"");
					}
					else if (str.compare(0,4,"xsi:") == 0)
					{
						str.replace(0,4,"");
					}

					if ( str == "type" )
						transactionType.type = attr.value();
					else if ( str == "subtype" )
						transactionType.subtype = attr.value();
					else if ( str == "action_seq_no" )
						actionSeqNo = atoi(attr.value());
				}

				// Get the Element Child belonging to each Transaction
				for
                (
                    pugi::xml_node transactionChildTool = transactionNode.first_child();
                    ( ret == MYKI_ACTIONLIST_SUCCESS ) && transactionChildTool;
                    transactionChildTool = transactionChildTool.next_sibling()
                )
				{
					transactionType.fieldName = transactionChildTool.name();

					for (pugi::xml_attribute attr = transactionChildTool.first_attribute(); attr; attr = attr.next_attribute())
					{
						transactionType.fieldValue = attr.value();
					}

					std::set<TransactionType>::iterator it = transactionTypes.find(transactionType);
					if ( it == transactionTypes.end() )
					{
						transactionType.primaryKey = ++transactionTypePK;
						fkTransactionType = transactionType.primaryKey;
						transactionTypes.insert(transactionType);
						// SQL INSERT statement into TransactionType table
						char query[512];
						sprintf(
							query,
							"INSERT INTO TransactionType"
							"(TransactionType_PK,type,subtype,field_name,field_value)"
							"VALUES"
							"(%d,'%s','%s','%s','%s')",
							transactionType.primaryKey,
							transactionType.type.c_str(),
							transactionType.subtype.c_str(),
							transactionType.fieldName.c_str(),
							transactionType.fieldValue.c_str());
						CsVerbose(":SQLINSERT: %s;", query);
						if ( m_dbAccess->executeSqlInsert(query) != 0 )
						{
							CsErrx("Query '%s' failed", query);
							ret = MYKI_ACTIONLIST_FAILED;
						}
					}
					else
					{
						fkTransactionType = it->primaryKey;
					}

					// SQL INSERT statement into Transaction table
					char query[512];
					sprintf(
						query,
						"INSERT INTO 'Transaction'"
						"(FK_Request,FK_TransactionType,action_seq_no)"
						"VALUES"
						"(%d,%d,%d)",
						requestPK,
						fkTransactionType,
						actionSeqNo);

					CsVerbose(":SQLINSERT: %s;", query);
		    		if ( m_dbAccess->executeSqlInsert(query) != 0 )
					{
						CsErrx("Query '%s' failed", query);
						ret = MYKI_ACTIONLIST_FAILED;
					}
				}
			}
		}

        /*  And commits changes (if any) */
        if ( m_dbAccess != NULL )
        {
            m_dbAccess->endDbTransaction();
        }
	}

	initActionlistLookup();
	return ret;
}

/*==========================================================================*
**
**  getActionlistSequence
**
**  Description     :
**      Gets the last received Actionlist (XML) sequence number.
**
**  Parameters      :
**      None
**
**  Returns         :
**      >0                          last received Actionlist sequence number
**      =0                          none received
**      <0                          failed connecting to Actionlist database
**
**  Notes           :
**
**==========================================================================*/

static  int getActionlistSequence( bool bIsDelta )
{
    DbAccess       *dbAccess            = getActionlistDBAccess( bIsDelta );
    const char     *query               = "SELECT MAX(sequence) AS sequence FROM Actionlist";
    Rows            rows;
    int             nActionlistSequence = -1;

    if(dbAccess!=NULL)
    {
        if ( dbAccess->executeSqlQuery( query, rows ) != SQLITE_OK )
        {
            CsWarnx( "MYKI_ACTIONLIST:getActionlistSequence: select failed" );
        }
        else if ( rows.empty() )
        {
            CsWarnx( "MYKI_ACTIONLIST:getActionlistSequence: no rows returned" );
        }
        else
        {
            nActionlistSequence = atoi( ( *rows.begin() )[ "sequence" ] );
        }
    }
	return nActionlistSequence;
}   /*  getActionlistSequence( ) */

int getActionlistSequence( void )
{
	if ( g_cachedActionlistSequence == -1 )
	{
        g_cachedActionlistSequence  = getActionlistSequence( true );
        if ( g_cachedActionlistSequence <= 0 )
        {
            g_cachedActionlistSequence  = getActionlistSequence( false );
        }
	}
	return g_cachedActionlistSequence;
}

/*==========================================================================*
**
**  getActionlistSequenceInfo
**
**  Description     :
**      Gets the received Actionlist (XML) sequence numbers.
**
**  Parameters      :
**      None
**
**  Returns         :
**      xxx                         printable Actionlist sequence numbers
**
**  Notes           :
**
**==========================================================================*/

static  const char * getActionlistSequenceInfo(
    bool                bIsDelta,
    std::string        &result,
    int                &sequenceLow,
    int                &sequenceHigh )
{
    DbAccess           *dbAccess    = getActionlistDBAccess( bIsDelta );
    const char         *query       = "SELECT sequence FROM Actionlist ORDER BY sequence ASC";
    Rows                rows;
    char                buf[ 32 ];

    if ( dbAccess == NULL )
    {
        CsWarnx( "MYKI_ACTIONLIST:getActionlistSequenceInfo: failed connecting %s Actionlist database", bIsDelta ? "Delta" : "Full" );
    }
    else if ( dbAccess->executeSqlQuery( query, rows ) != SQLITE_OK )
    {
        CsWarnx( "MYKI_ACTIONLIST:getActionlistSequenceInfo: select failed" );
    }
    else if ( rows.empty() )
    {
        CsWarnx( "MYKI_ACTIONLIST:getActionlistSequenceInfo: no rows returned" );
    }
    else
    {
        for ( Rows::const_iterator itr = rows.begin(); itr != rows.end(); ++itr )
        {
            int sequence    = atoi( ( *itr )[ "sequence" ] );
            if ( sequenceLow < 0 )
            {
                sequenceLow     = sequence;
                sequenceHigh    = sequence;
            }
            else if ( ( sequenceHigh + 1 ) == sequence )
            {
                /*  Consecutive Actionlist sequence number */
                sequenceHigh    = sequence;
            }
            else
            {
                sprintf( buf, "%s%d", ( result.empty() ? "" : "," ), sequenceLow );
                result += buf;
                if ( sequenceHigh >= ( sequenceLow + 1 ) )
                {
                    sprintf( buf, "%s%d", ( sequenceHigh == ( sequenceLow + 1 ) ? "," : ".." ), sequenceHigh );
                    result += buf;
                }
                sequenceLow     = sequence;
                sequenceHigh    = sequence;
            }
        }
    }
    return result.c_str();
}   /*  getActionlistSequenceInfo( ) */

const char* getActionlistSequenceInfo( void )
{
	if ( g_cachedActionlistSequenceInfo.empty() )
	{
        std::string     result;
        int             sequenceLow     = -1;
        int             sequenceHigh    = -1;
		char            buf[ 32 ];

        getActionlistSequenceInfo( false,  result, sequenceLow, sequenceHigh );
        getActionlistSequenceInfo( true,   result, sequenceLow, sequenceHigh );

        sprintf( buf, "%s%d", ( result.empty() ? "" : "," ), sequenceLow );
        result += buf;
        if ( sequenceHigh >= ( sequenceLow + 1 ) )
        {
            sprintf( buf, "%s%d", ( sequenceHigh == ( sequenceLow + 1 ) ? "," : ".." ), sequenceHigh );
            result += buf;
        }

        g_cachedActionlistSequenceInfo  = result;
	}
	return g_cachedActionlistSequenceInfo.c_str();
}

/*==========================================================================*
**
**  getActionlistDetails
**
**  Description     :
**      Returns actionlist details, ie. name + sequence number + time stamp.
**
**      ssssss nnnnnn yyyy-mm-ddTHH:MM:SS
**
**      where,
**
**      ssssss              "actionlistsdf" (delta) or "ActionList" (full)
**      nnnnnn              actionlist sequence number
**      yyyy-mm-ddTHH:MM:SS actionlist date/time stamp
**
**  Parameters      :
**      None
**
**  Returns         :
**      xxx                 actionlist details string
**
**  Notes           :
**
**==========================================================================*/

static  void getActionlistDetails( bool bIsDelta )
{
    #define Name_ActionList_Full    "Actionlist"
    #define Name_ActionList_Delta   "actionlistsdf"

    DbAccess           *pDbAccess   = getActionlistDBAccess( bIsDelta );

    if ( pDbAccess != NULL )
    {
        const char     *pName       = ( bIsDelta == true ? Name_ActionList_Delta : Name_ActionList_Full );
        const char     *pQuery      = "SELECT sequence, timestamp FROM Actionlist ORDER BY sequence ASC";
        Rows            rows;

        if ( pDbAccess->executeSqlQuery( pQuery, rows ) == SQLITE_OK && rows.empty() == false )
        {
            for ( Rows::const_iterator itr = rows.begin(); itr != rows.end(); ++itr )
            {
                g_cachedActionlistDetails  += pName;
                g_cachedActionlistDetails  += ' ';
                g_cachedActionlistDetails  += ( *itr )[ "sequence"     ];
                g_cachedActionlistDetails  += ' ';
                g_cachedActionlistDetails  += ( *itr )[ "timestamp"    ];
                g_cachedActionlistDetails  += '\n';
                pName                       = Name_ActionList_Delta;
            }
        }
    }
}   /*  getActionlistDetails( ) */

const char *getActionlistDetails( void )
{
    if ( g_cachedActionlistDetails.empty() )
    {
        getActionlistDetails( false /*FULL*/  );
        getActionlistDetails( true  /*DELTA*/ );
    }
    return  g_cachedActionlistDetails.c_str();
}   /*  getActionlistDetails( ) */

/*==========================================================================*
**
**  appendToGlobalCache
**
**  Description     :
**      appends rows to the global cache of actionlist entries
**
**  Parameters      :
**      r             [I]     list of rows
**
**  Returns         :
**      None
**
**  Notes           :
**
**==========================================================================*/
static void 
appendToGlobalCache (Rows &r)
{
	Rows::const_iterator itr;
	
	for(itr = r.begin(); itr != r.end(); ++itr){
		g_cachedCardActionlistRows.push_back(*itr);
	}
}

/*==========================================================================*
**
**  doCardQueryRefreshCache
**
**  Description     :
**      Queries Full and Delta Actionlist databases and builds the cache for br.
**
**  Parameters      :
**      cardUID             [I]     card UID
**
**  Returns         :
**      None
**
**  Notes           :
**
**==========================================================================*/

static  void doCardQueryRefreshCache( const char *cardUID )
{
    DbAccess       *pDbAccess       = getActionlistDBAccess( false );

	g_cachedCardActionlistRows.clear();
	Rows rows;
	U64_t       uid     = 0;
	char        s_query[ 768 ];

	sscanf( cardUID, "%llX", &uid );
	sprintf( s_query,
		"SELECT PRINTF(\"%%014X\", r.card_uid) AS card_uid,"
		" r.type AS requestType, r.expiry AS expiry,"
		" PRINTF(\"%%08X-%%04X-%%04X-%%04X-%%012X\", r.action_guid,r.action_guid2,r.action_guid3,r.action_guid4,r.action_guid5) AS action_guid,"
		" tt.type AS type, tt.subtype AS subtype, t.action_seq_no, tt.field_name, tt.field_value "
		"FROM Request r, 'Transaction' t, TransactionType tt "
		"WHERE r.card_uid=%lld AND t.FK_Request=r.Request_PK AND tt.TransactionType_PK=t.FK_TransactionType "
		"ORDER BY r.card_uid, t.action_seq_no, r.type, r.expiry, r.action_guid, tt.type, tt.subtype",
	uid );

    if ( pDbAccess != NULL )
    {
        if ( pDbAccess->executeSqlQuery( s_query, rows ) != SQLITE_OK )
        {
            CsWarnx( "MYKI_ACTIONLIST:doCardQueryRefreshCache: query <%s> from full failed for card %s", s_query, cardUID );
        } else {
			CsDebug( CD_DEBUG, (CD_DEBUG, "MYKI_ACTIONLIST:doCardQueryRefreshCache : %ld entries found for Card UID %s in FULL AL", rows.size(), cardUID));
			appendToGlobalCache(rows);
		}
    }   /*  end-of-if */
	// Load the Delta (may not be any delta)
    pDbAccess       = getActionlistDBAccess( true );
    if ( pDbAccess != NULL )
    {
		rows.clear();
        if ( pDbAccess->executeSqlQuery( s_query, rows ) != SQLITE_OK )
        {
            CsWarnx( "MYKI_ACTIONLIST:doCardQueryRefreshCache: query <%s>  from Delta failed for card %s", s_query, cardUID );
			appendToGlobalCache(rows);
        } else {
			CsDebug( CD_DEBUG, (CD_DEBUG, "MYKI_ACTIONLIST:doCardQueryRefreshCache : %ld entries found for Card UID %s in Delta AL", rows.size(), cardUID));
			appendToGlobalCache(rows);
		}
    }   /*  end-of-if */
	CsDebug( CD_DEBUG, ( CD_DEBUG, "MYKI_ACTIONLIST:doCardQueryRefreshCache:  Card UID in cache for %s has %ld entries", cardUID, g_cachedCardActionlistRows.size() ) );
}   /*  doCardQueryRefreshCache( ) */


void doCardQuery( const char *cardUID )
{
	if ( strncmp( cardUID, g_cachedCardUID, sizeof( g_cachedCardUID ) ) == 0 )
	{
		CsDebug( CD_DEBUG, ( CD_DEBUG, "MYKI_ACTIONLIST:doCardQuery: Same Card UID in cache for %s", cardUID ) );
	}
	else	
	{
        /*  Retrieves Full and Delta Actionlist records */
        doCardQueryRefreshCache( cardUID );

        /*  And caches them */
		stringCopy( g_cachedCardUID, cardUID, sizeof( g_cachedCardUID ) );
	}
}

/*==========================================================================*
**
**  firstMatchingCacheRow
**
**  Description     :
**      Searches for first matched Actionlist record.
**
**  Parameters      :
**      type                [I]     Actionlist type
**      subtype             [I]     Actionlist sub-type
**      actionSeqNo         [I]     Actionlist sequence number
**
**  Returns         :
**      xxx                         row iterator
**
**  Notes           :
**
**==========================================================================*/

Rows::const_iterator    firstMatchingCacheRow(
    const char         *type,
    const char         *subtype,
    int                 actionSeqNo )
{
	char                s_actionSeqNo[ 32 ];

	memset(  s_actionSeqNo, 0,    sizeof( s_actionSeqNo ) );
	sprintf( s_actionSeqNo, "%d", actionSeqNo             );

	for
	(
		Rows::const_iterator itr = g_cachedCardActionlistRows.begin();
		itr != g_cachedCardActionlistRows.end();
		++itr
	)
	{
		if (
			(strcmp((*itr)["action_seq_no"], s_actionSeqNo) == 0) &&
			(strcmp((*itr)["subtype"], subtype) == 0) &&
			(strcmp((*itr)["type"], type) == 0)
		   )
		{
			return itr;
		}
	}

	return g_cachedCardActionlistRows.end();
}

/*==========================================================================*
**
**  nextMatchingCacheRow
**
**  Description     :
**      Searches for next matched Actionlist record.
**
**  Parameters      :
**      lastMatch           [I]     last matched Actionlist record
**
**  Returns         :
**      xxx                         row iterator
**
**  Notes           :
**      
**
**==========================================================================*/

Rows::const_iterator    nextMatchingCacheRow( Rows::const_iterator lastMatch )
{
	Rows::const_iterator    itr = lastMatch;

	++itr;
	if ( itr == g_cachedCardActionlistRows.end() )
    {
		return itr;
    }   /*  end-of-if */

	if (
		(strcmp((*itr)["action_seq_no"], (*lastMatch)["action_seq_no"]) == 0) &&
		(strcmp((*itr)["subtype"], (*lastMatch)["subtype"]) == 0) &&
		(strcmp((*itr)["type"], (*lastMatch)["type"]) == 0)
	   )
	{
		return itr;
	}
	return g_cachedCardActionlistRows.end();
}

/*==========================================================================*
**
**  isEndCacheRow
**
**  Description     :
**      Tests for end of matching Actionlist records.
**
**  Parameters      :
**      next                [I]     next Actionlist record
**
**  Returns         :
**      true                        end of matching Actionlist records
**      false                       otherwise
**
**  Notes           :
**
**==========================================================================*/

bool    isEndCacheRow( Rows::const_iterator next )
{
    return next == g_cachedCardActionlistRows.end() ? true : false;
}   /*  isEndCacheRow( ) */


/*  TO BE CONTINUE! */



/*
 * Checks if the Card is in the actionlist database.
 *
 * Input:
 *		const char * cardUID
 *
 * Return:
 * 		true or false
 */
int isCardActionlist(const char * cardUID)
{
	int returnValue=false;

	doCardQuery(cardUID);
	if(!g_cachedCardActionlistRows.empty())
	{
		returnValue = true;
	}

	return returnValue;
}

/*
 * Get the list of Card Actionlist Requests for a given Card UID.
 */
int getCardActionlistRequests(const char * cardUID, MYKI_ACTIONLIST_Card_Actionlist_Requests_t &cardActionlistRequests)
{
	int returnValue=false;

	if(strncmp(cardUID, g_cachedCardActionlistRequests.card_uid, sizeof(g_cachedCardActionlistRequests.card_uid)) == 0)
	{
		//std::cout << "\nMYKI_ACTIONLIST: getCardActionlistRequests: Same Card UID in cache. Return cache";
		CsDebug(CD_DEBUG, (CD_DEBUG, "MYKI_ACTIONLIST: getCardActionlistRequests: Same Card UID in cache. Return cache for cardUID %s.", cardUID));
		memcpy(&cardActionlistRequests, &g_cachedCardActionlistRequests, sizeof(cardActionlistRequests));
		return true;
	}

	strncpy(g_cachedCardActionlistRequests.card_uid, cardUID, sizeof(g_cachedCardActionlistRequests.card_uid));

	doCardQuery(cardUID);
	Rows& rows = g_cachedCardActionlistRows;

	if ( g_cachedCardActionlistCapacity == 0 )
	{
		g_cachedCardActionlistCapacity = 10;
		g_cachedCardActionlistRequests.arrayOfRequest = reinterpret_cast<MYKI_ACTIONLIST_Request_t*>(malloc(g_cachedCardActionlistCapacity * sizeof(MYKI_ACTIONLIST_Request_t)));
		memset(g_cachedCardActionlistRequests.arrayOfRequest, 0, g_cachedCardActionlistCapacity * sizeof(MYKI_ACTIONLIST_Request_t));
	}

	Rows::const_iterator itr;
	int i = 0;
	for(itr = rows.begin(); itr != rows.end(); ++itr){
		if(i+1 >= g_cachedCardActionlistCapacity)
		{
			g_cachedCardActionlistRequests.arrayOfRequest = reinterpret_cast<MYKI_ACTIONLIST_Request_t*>(
				realloc(g_cachedCardActionlistRequests.arrayOfRequest, 2 * g_cachedCardActionlistCapacity * sizeof(MYKI_ACTIONLIST_Request_t)));
			g_cachedCardActionlistCapacity *= 2;
		}
		memset(g_cachedCardActionlistRequests.arrayOfRequest + i, 0, sizeof(MYKI_ACTIONLIST_Request_t));

		const ColumnValue& cv = *itr;
		g_cachedCardActionlistRequests.arrayOfRequest[i].requestType = (MYKI_ACTIONLIST_RequestType_t)(convertStringToU32(cv["requestType"]));
		g_cachedCardActionlistRequests.arrayOfRequest[i].action_seq_no = convertStringToU8(cv["action_seq_no"]);
		g_cachedCardActionlistRequests.arrayOfRequest[i].expiry = (Time_t)(convertStringToU32(cv["expiry"]));
		stringCopy(g_cachedCardActionlistRequests.arrayOfRequest[i].action_guid, cv["action_guid"], LEN_MYKI_LDT_GUID);


		std::string typeString(cv["type"]);
		std::string subtypeString(cv["subtype"]);

		// Set to unknown value if the type or subtype is not specified
		g_cachedCardActionlistRequests.arrayOfRequest[i].type = (U8_t)-1;
		g_cachedCardActionlistRequests.arrayOfRequest[i].subtype = (U8_t)-1;

		// Product Sale
		if(typeString.compare(MYKI_ACTIONLIST_PRODUCT_SALE) == 0)
		{
			g_cachedCardActionlistRequests.arrayOfRequest[i].type = MYKI_UD_TYPE_PRODUCT_SALE;

			// Product Sale None
			if(subtypeString.compare(MYKI_ACTIONLIST_NONE) == 0)
			{
				g_cachedCardActionlistRequests.arrayOfRequest[i].subtype = MYKI_UD_PRODUCT_SALE_SUB_TYPE_NONE;
			}
		}
		// TApp Update
		else if(typeString.compare(MYKI_ACTIONLIST_TAPP_UPDATE) == 0)
		{
			g_cachedCardActionlistRequests.arrayOfRequest[i].type = MYKI_UD_TYPE_TAPP_UPDATE;

			// TApp Update None
			if(subtypeString.compare(MYKI_ACTIONLIST_NONE) == 0)
			{
				g_cachedCardActionlistRequests.arrayOfRequest[i].subtype = MYKI_UD_TAPP_UPDATE_SUB_TYPE_NONE;
			}
			// TApp Update Activate
			else if(subtypeString.compare(MYKI_ACTIONLIST_ACTIVATE) == 0)
			{
				g_cachedCardActionlistRequests.arrayOfRequest[i].subtype = MYKI_UD_TAPP_UPDATE_SUB_TYPE_ACTIVATE;
			}
			// TApp Update Block
			else if(subtypeString.compare(MYKI_ACTIONLIST_BLOCK) == 0)
			{
				g_cachedCardActionlistRequests.arrayOfRequest[i].subtype = MYKI_UD_TAPP_UPDATE_SUB_TYPE_BLOCK;
			}
			// TApp Update Unblock
			else if(subtypeString.compare(MYKI_ACTIONLIST_UNBLOCK) == 0)
			{
				g_cachedCardActionlistRequests.arrayOfRequest[i].subtype = MYKI_UD_TAPP_UPDATE_SUB_TYPE_UNBLOCK;
			}
		}
                // OApp Update
                else if(typeString.compare(MYKI_ACTIONLIST_OAPP_UPDATE) == 0)
                {
                        g_cachedCardActionlistRequests.arrayOfRequest[i].type = MYKI_UD_TYPE_OAPP_UPDATE;
 
                        // OApp Update Block
                        if(subtypeString.compare(MYKI_ACTIONLIST_BLOCK) == 0)
                        {
                                g_cachedCardActionlistRequests.arrayOfRequest[i].subtype = MYKI_UD_OAPP_UPDATE_SUB_TYPE_BLOCK;
                        }
                        // OApp Update Unblock
                        else if(subtypeString.compare(MYKI_ACTIONLIST_UNBLOCK) == 0)
                        {
                                g_cachedCardActionlistRequests.arrayOfRequest[i].subtype = MYKI_UD_OAPP_UPDATE_SUB_TYPE_UNBLOCK;
                        }
                }
		// TPurse Load
		else if(typeString.compare(MYKI_ACTIONLIST_TPURSE_LOAD) == 0)
		{
			g_cachedCardActionlistRequests.arrayOfRequest[i].type = MYKI_UD_TYPE_TPURSE_LOAD;

			// TPurse Load None
			if(subtypeString.compare(MYKI_ACTIONLIST_NONE) == 0)
			{
				g_cachedCardActionlistRequests.arrayOfRequest[i].subtype = MYKI_UD_TPURSE_LOAD_SUB_TYPE_NONE;
			}
			// TPurse Load Debit
			else if(subtypeString.compare(MYKI_ACTIONLIST_DEBIT) == 0)
			{
				g_cachedCardActionlistRequests.arrayOfRequest[i].subtype = MYKI_UD_TPURSE_LOAD_SUB_TYPE_DEBIT;
			}
		}
		// TPurse Update
		else if(typeString.compare(MYKI_ACTIONLIST_TPURSE_UPDATE) == 0)
		{
			g_cachedCardActionlistRequests.arrayOfRequest[i].type = MYKI_UD_TYPE_TPURSE_UPDATE;

			// TPurse Update None
			if(subtypeString.compare(MYKI_ACTIONLIST_NONE) == 0)
			{
				g_cachedCardActionlistRequests.arrayOfRequest[i].subtype = MYKI_UD_TPURSE_UPDATE_SUB_TYPE_NONE;
			}
			// TPurse Update Block
			else if(subtypeString.compare(MYKI_ACTIONLIST_BLOCK) == 0)
			{
				g_cachedCardActionlistRequests.arrayOfRequest[i].subtype = MYKI_UD_TPURSE_UPDATE_SUB_TYPE_BLOCK;
			}
		}
		else if(typeString.compare(MYKI_ACTIONLIST_PRODUCT_UPDATE) == 0)
		{
			g_cachedCardActionlistRequests.arrayOfRequest[i].type = MYKI_UD_TYPE_PRODUCT_UPDATE;

			// TPurse Update None
			if(subtypeString.compare(MYKI_ACTIONLIST_NONE) == 0)
			{
				g_cachedCardActionlistRequests.arrayOfRequest[i].subtype = MYKI_UD_PRODUCT_UPDATE_SUB_TYPE_NONE;
			}
			// TPurse Update Block
			else if(subtypeString.compare(MYKI_ACTIONLIST_BLOCK) == 0)
			{
				g_cachedCardActionlistRequests.arrayOfRequest[i].subtype = MYKI_UD_PRODUCT_UPDATE_SUB_TYPE_BLOCK;
			}
		}
		// Only increment count if this is different to the last
		if((i == 0) || (0 != memcmp(g_cachedCardActionlistRequests.arrayOfRequest + i, g_cachedCardActionlistRequests.arrayOfRequest + (i-1), sizeof(MYKI_ACTIONLIST_Request_t))))
			i++;
	}

	g_cachedCardActionlistRequests.numberOfRequests = i;
	memcpy(&cardActionlistRequests, &g_cachedCardActionlistRequests, sizeof(cardActionlistRequests));

	if(i == 0)
	{
        CsDebug(CD_DEBUG, (CD_DEBUG, "MYKI_ACTIONLIST:getCardActionlistRequests: No Actionlist found for Card UID %s.", cardUID));
	}
	else
	{
        returnValue=true;
	}

	return returnValue;
}

// TAppUpdateRequest
int getTAppUpdateRequestNone(const char * cardUID, int actionSeqNo, MYKI_ACTIONLIST_TAppUpdateRequest_t &tAppUpdateRequest)
{
	return getTAppUpdateRequest(cardUID, actionSeqNo, MYKI_ACTIONLIST_NONE, tAppUpdateRequest);
}

int getTAppUpdateRequestActivate(const char * cardUID, int actionSeqNo, MYKI_ACTIONLIST_TAppUpdateRequest_t &tAppUpdateRequest)
{
	return getTAppUpdateRequest(cardUID, actionSeqNo, MYKI_ACTIONLIST_ACTIVATE, tAppUpdateRequest);
}

int getTAppUpdateRequestBlock(const char * cardUID, int actionSeqNo, MYKI_ACTIONLIST_TAppUpdateRequest_t &tAppUpdateRequest)
{
	return getTAppUpdateRequest(cardUID, actionSeqNo, MYKI_ACTIONLIST_BLOCK, tAppUpdateRequest);
}

int getTAppUpdateRequestUnblock(const char * cardUID, int actionSeqNo, MYKI_ACTIONLIST_TAppUpdateRequest_t &tAppUpdateRequest)
{
	return getTAppUpdateRequest(cardUID, actionSeqNo, MYKI_ACTIONLIST_UNBLOCK, tAppUpdateRequest);
}

int getTAppUpdateRequest(const char * cardUID, int actionSeqNo, const char* subtype, MYKI_ACTIONLIST_TAppUpdateRequest_t &tAppUpdateRequest)
{
	int returnValue=false;
	memset(&tAppUpdateRequest, 0, sizeof(tAppUpdateRequest));

	doCardQuery(cardUID);
	Rows::const_iterator itr;

	bool firstRow = true;
	for(
		itr = firstMatchingCacheRow(MYKI_ACTIONLIST_TAPP_UPDATE, subtype, actionSeqNo);
		itr != g_cachedCardActionlistRows.end();
		itr = nextMatchingCacheRow(itr)
	   )
	{
		returnValue = true;
		const ColumnValue& cv = *itr;

		if ( firstRow )
		{
			const char* guid = cv["action_guid"];
			if ( (guid != 0) && (guid[0] != '\0') )
			{
				tAppUpdateRequest.isActionGuidSet = TRUE;
				stringCopy(tAppUpdateRequest.actionGuid, guid, LEN_MYKI_LDT_GUID);
			}
			const char* seqNo = cv["action_seq_no"];
			if ( (seqNo != 0) && (seqNo[0] != '\0') )
			{
				tAppUpdateRequest.isActionSequenceNoSet = TRUE;
				tAppUpdateRequest.actionSequenceNo = convertStringToU8(seqNo);
			}
			firstRow = false;
		}

		const char* fieldName = cv["field_name"];
		const char* fieldValue = cv["field_value"];

		if(strcmp(fieldName, "ServiceProviderId") == 0)
		{
			tAppUpdateRequest.serviceProviderId = convertStringToU16(fieldValue);
		}
		else if(strcmp(fieldName, "EntryPointId") == 0)
		{
			tAppUpdateRequest.entryPointId = convertStringToU16(fieldValue);
			tAppUpdateRequest.isEntryPointIdSet = TRUE;
		}
		else if(strcmp(fieldName, "ExpiryDate") == 0)
		{
			tAppUpdateRequest.expiryDate = convertStringToU16(fieldValue);
			if(tAppUpdateRequest.expiryDate>0)
				tAppUpdateRequest.expiryDate += JULIEN_BASE_FOR_DATEC19_2006;
			tAppUpdateRequest.isExpiryDateSet = TRUE;
		}
		else if(strcmp(fieldName, "PassengerCode") == 0)
		{
			tAppUpdateRequest.passengerCode = convertStringToU8(fieldValue);
			tAppUpdateRequest.isPassengerCodeSet = TRUE;
		}
		else if(strcmp(fieldName, "PassengerCodeExpiry") == 0)
		{
			tAppUpdateRequest.passengerCodeExpiry = convertStringToU16(fieldValue);
			if(tAppUpdateRequest.passengerCodeExpiry>0)
				tAppUpdateRequest.passengerCodeExpiry += JULIEN_BASE_FOR_DATEC19_2006;			
			tAppUpdateRequest.isPassengerCodeExpirySet = TRUE;
		}
		else if(strcmp(fieldName, "PassengerControl") == 0)
		{
			tAppUpdateRequest.passengerControl = convertStringToU8(fieldValue);
			tAppUpdateRequest.isPassengerControlSet = TRUE;
		}
		else if(strcmp(fieldName, "Language") == 0)
		{
			tAppUpdateRequest.language = convertStringToU8(fieldValue);
			tAppUpdateRequest.isLanguageSet = TRUE;
		}
		else if(strcmp(fieldName, "BlockingReason") == 0)
		{
			tAppUpdateRequest.blockingReason = convertStringToU8(fieldValue);
		}
		else if(strcmp(fieldName, "SerialNo") == 0)
		{
			tAppUpdateRequest.serialNo = convertStringToU16(fieldValue);
			tAppUpdateRequest.isSerialNoSet = TRUE;
		}
	}

	if(!returnValue)
	{
        CsDebug(CD_DEBUG, (CD_DEBUG, "MYKI_ACTIONLIST:getTAppUpdateRequest: No TAppUpdateRequest subtype %s found for Card UID %s", subtype, cardUID));
	}

	return returnValue;
}



// TPurseLoadRequest
int getTPurseLoadRequest(const char * subtype, const char * cardUID, int actionSeqNo, MYKI_ACTIONLIST_TPurseLoadRequest_t &tPurseLoadRequest)
{
	int returnValue=false;
	memset(&tPurseLoadRequest, 0, sizeof(tPurseLoadRequest));

	doCardQuery(cardUID);
	Rows::const_iterator itr;

	bool firstRow = true;
	for(
		itr = firstMatchingCacheRow(MYKI_ACTIONLIST_TPURSE_LOAD, subtype, actionSeqNo);
		itr != g_cachedCardActionlistRows.end();
		itr = nextMatchingCacheRow(itr)
	   )
	{
		returnValue = true;
		const ColumnValue& cv = *itr;

		if ( firstRow )
		{
			const char* guid = cv["action_guid"];
			if ( (guid != 0) && (guid[0] != '\0') )
			{
				tPurseLoadRequest.isActionGuidSet = TRUE;
				stringCopy(tPurseLoadRequest.actionGuid, guid, LEN_MYKI_LDT_GUID);
			}
			const char* seqNo = cv["action_seq_no"];
			if ( (seqNo != 0) && (seqNo[0] != '\0') )
			{
				tPurseLoadRequest.isActionSequenceNoSet = TRUE;
				tPurseLoadRequest.actionSequenceNo = convertStringToU8(seqNo);
			}
			firstRow = false;
		}

		const char* fieldName = cv["field_name"];
		const char* fieldValue = cv["field_value"];

		if(strcmp(fieldName, "Value") == 0)
		{
			tPurseLoadRequest.value = convertStringToU32(fieldValue);
		}
		else if(strcmp(fieldName, "ServiceProviderId") == 0)
		{
			tPurseLoadRequest.serviceProviderId = convertStringToU16(fieldValue);
		}
		else if(strcmp(fieldName, "EntryPointId") == 0)
		{
			tPurseLoadRequest.entryPointId = convertStringToU16(fieldValue);
			tPurseLoadRequest.isEntryPointIdSet = TRUE;
		}
		else if(strcmp(fieldName, "RouteId") == 0)
		{
			tPurseLoadRequest.routeId = convertStringToU16(fieldValue);
			tPurseLoadRequest.isRouteIdSet = TRUE;
		}
		else if(strcmp(fieldName, "StopId") == 0)
		{
			tPurseLoadRequest.stopId = convertStringToU8(fieldValue);
			tPurseLoadRequest.isStopIdSet = TRUE;
		}
	}

	if(!returnValue)
	{
        CsDebug(CD_DEBUG, (CD_DEBUG, "MYKI_ACTIONLIST:getTPurseLoadRequest: No TPurseLoadRequest subtype %s found for Card UID %s", subtype, cardUID));
	}

	return returnValue;
}


// ProductSaleRequest
int getProductSaleRequestNone(const char * cardUID, int actionSeqNo, MYKI_ACTIONLIST_ProductSaleRequest_t &productSaleRequests)
{
	int returnValue=false;
	memset(&productSaleRequests, 0, sizeof(productSaleRequests));

	doCardQuery(cardUID);
	Rows::const_iterator itr;

	bool firstRow = true;
	for(
		itr = firstMatchingCacheRow(MYKI_ACTIONLIST_PRODUCT_SALE, MYKI_ACTIONLIST_NONE, actionSeqNo);
		itr != g_cachedCardActionlistRows.end();
		itr = nextMatchingCacheRow(itr)
	   )
	{
		returnValue = true;
		const ColumnValue& cv = *itr;

		if ( firstRow )
		{
			const char* guid = cv["action_guid"];
			if ( (guid != 0) && (guid[0] != '\0') )
			{
				productSaleRequests.isActionGuidSet = TRUE;
				stringCopy(productSaleRequests.actionGuid, guid, LEN_MYKI_LDT_GUID);
			}
			const char* seqNo = cv["action_seq_no"];
			if ( (seqNo != 0) && (seqNo[0] != '\0') )
			{
				productSaleRequests.isActionSequenceNoSet = TRUE;
				productSaleRequests.actionSequenceNo = convertStringToU8(seqNo);
			}
			firstRow = false;
		}

		const char* fieldName = cv["field_name"];
		const char* fieldValue = cv["field_value"];

                CsDebug(CD_DEBUG, (CD_DEBUG, "MYKI_ACTIONLIST:getProductSaleRequestNone: field name = %s value = %s", fieldName, fieldValue));

		if(strcmp(fieldName, "ProductId") == 0)
		{
			productSaleRequests.productId = convertStringToU8(fieldValue);
		}
		else if(strcmp(fieldName, "IssuerId") == 0)
		{
			productSaleRequests.issuerId = convertStringToU8(fieldValue);
		}
		else if(strcmp(fieldName, "PurchaseValue") == 0)
		{
			productSaleRequests.purchaseValue = convertStringToU32(fieldValue);
		}
		else if(strcmp(fieldName, "ZoneLow") == 0)
		{
			productSaleRequests.zoneLow = convertStringToU8(fieldValue);
		}
		else if(strcmp(fieldName, "ZoneHigh") == 0)
		{
			productSaleRequests.zoneHigh = convertStringToU8(fieldValue);
		}
		else if(strcmp(fieldName, "StartDateTime") == 0)
		{
			U32_t startDateTime = convertStringToU32(fieldValue);

			DateMykiU24ToTimeT(startDateTime, &productSaleRequests.startDateTime);
			productSaleRequests.isStartDateTimeSet = TRUE;
		}
		else if(strcmp(fieldName, "EndDateTime") == 0)
		{
			U32_t endDateTime = convertStringToU32(fieldValue);

			DateMykiU24ToTimeT(endDateTime, &productSaleRequests.endDateTime);
			productSaleRequests.isEndDateTimeSet = TRUE;
		}
		else if(strcmp(fieldName, "ServiceProviderId") == 0)
		{
			productSaleRequests.serviceProviderId = convertStringToU16(fieldValue);
		}
		else if(strcmp(fieldName, "EntryPointId") == 0)
		{
			productSaleRequests.entryPointId = convertStringToU16(fieldValue);
			productSaleRequests.isEntryPointIdSet = TRUE;
		}
		else if(strcmp(fieldName, "RouteId") == 0)
		{
			productSaleRequests.routeId = convertStringToU16(fieldValue);
			productSaleRequests.isRouteIdSet = TRUE;
		}
		else if(strcmp(fieldName, "StopId") == 0)
		{
			productSaleRequests.stopId = convertStringToU8(fieldValue);
			productSaleRequests.isStopIdSet = TRUE;
		}
		else if(strcmp(fieldName, "IsAutoload") == 0)
		{
			if(strcmp(fieldValue, "true") == 0)
			{
				productSaleRequests.isAutoload = 1;
			}
			else if(strcmp(fieldValue, "false") == 0)
			{
				productSaleRequests.isAutoload = 0;
			}
			productSaleRequests.isAutoloadSet = TRUE;
		}
		else if(strcmp(fieldName, "IsOffpeak") == 0)
		{
			if(strcmp(fieldValue, "true") == 0)
			{
				productSaleRequests.isOffpeak = 1;
			}
			else if(strcmp(fieldValue, "false") == 0)
			{
				productSaleRequests.isOffpeak = 0;
			}
			productSaleRequests.isOffpeakSet = TRUE;
		}
		else if(strcmp(fieldName, "IsProvisional") == 0)
		{
			if(strcmp(fieldValue, "true") == 0)
			{
				productSaleRequests.isProvisional = 1;
			}
			else if(strcmp(fieldValue, "false") == 0)
			{
				productSaleRequests.isProvisional = 0;
			}
			productSaleRequests.isProvisionalSet = TRUE;
		}
		else if(strcmp(fieldName, "InstanceCount") == 0)
		{
			productSaleRequests.instanceCount = convertStringToU16(fieldValue);
			productSaleRequests.isInstanceCountSet = TRUE;
		}
		else if(strcmp(fieldName, "SerialNo") == 0)
		{
			productSaleRequests.serialNo = convertStringToU16(fieldValue);
		}
		else if(strcmp(fieldName, "SurchargeProductId") == 0)
		{
			productSaleRequests.surchargeProductId = convertStringToU8(fieldValue);
		}
		else if(strcmp(fieldName, "targetProductSerialNumber") == 0)
		{
			productSaleRequests.targetProductSerialNumber = convertStringToU8(fieldValue);
		}
	}

	if(!returnValue)
	{
        CsDebug(CD_DEBUG, (CD_DEBUG, "MYKI_ACTIONLIST:getProductSaleRequestNone: No ProductSaleRequest subtype %s found for Card UID %s", MYKI_ACTIONLIST_NONE, cardUID));
	}

	return returnValue;
}


// TPurseUpdateRequest
int getTPurseUpdateRequest(const char * subtype, const char * cardUID, int actionSeqNo, MYKI_ACTIONLIST_TPurseUpdateRequest_t &purseUpdateRequest)
{
	int returnValue=false;
	memset(&purseUpdateRequest, 0, sizeof(purseUpdateRequest));

	doCardQuery(cardUID);
	Rows::const_iterator itr;

	bool firstRow = true;
	for(
		itr = firstMatchingCacheRow(MYKI_ACTIONLIST_TPURSE_UPDATE, subtype, actionSeqNo);
		itr != g_cachedCardActionlistRows.end();
		itr = nextMatchingCacheRow(itr)
	   )
	{
		returnValue = true;
		const ColumnValue& cv = *itr;

		if ( firstRow )
		{
			const char* guid = cv["action_guid"];
			if ( (guid != 0) && (guid[0] != '\0') )
			{
				purseUpdateRequest.isActionGuidSet = TRUE;
				stringCopy(purseUpdateRequest.actionGuid, guid, LEN_MYKI_LDT_GUID);
			}
			const char* seqNo = cv["action_seq_no"];
			if ( (seqNo != 0) && (seqNo[0] != '\0') )
			{
				purseUpdateRequest.isActionSequenceNoSet = TRUE;
				purseUpdateRequest.actionSequenceNo = convertStringToU8(seqNo);
			}
			firstRow = false;
		}

		const char* fieldName = cv["field_name"];
		const char* fieldValue = cv["field_value"];


		if(strcmp(fieldName, "ServiceProviderId") == 0)
		{
			purseUpdateRequest.serviceProviderId = convertStringToU16(fieldValue);
		}
		else if(strcmp(fieldName, "EntryPointId") == 0)
		{
			purseUpdateRequest.entryPointId = convertStringToU16(fieldValue);
			purseUpdateRequest.isEntryPointIdSet = TRUE;
		}
		else if(strcmp(fieldName, "IsAutoload") == 0)
		{
			if(strcmp(fieldValue, "true") == 0)
			{
				purseUpdateRequest.isAutoload = 1;
			}
			else if(strcmp(fieldValue, "false") == 0)
			{
				purseUpdateRequest.isAutoload = 0;
			}
			purseUpdateRequest.isAutoloadSet = TRUE;
		}
		else if(strcmp(fieldName, "IsAutoload") == 0)
		{
			if(strcmp(fieldValue, "true") == 0)
			{
				purseUpdateRequest.isAutoload = 1;
			}
			else if(strcmp(fieldValue, "false") == 0)
			{
				purseUpdateRequest.isAutoload = 0;
			}
			purseUpdateRequest.isAutoloadSet = TRUE;
		}
		if(strcmp(fieldName, "AutoThreshold") == 0)
		{
			purseUpdateRequest.autoThreshold = convertStringToU32(fieldValue);
			purseUpdateRequest.isAutoThresholdSet = TRUE;
		}
		else if(strcmp(fieldName, "AutoValue") == 0)
		{
			purseUpdateRequest.autoValue = convertStringToU32(fieldValue);
			purseUpdateRequest.isAutoValueSet = TRUE;
		}
		else if(strcmp(fieldName, "IsTransitOnly") == 0)
		{
			if(strcmp(fieldValue, "true") == 0)
			{
				purseUpdateRequest.isTransitOnly = 1;
			}
			else if(strcmp(fieldValue, "false") == 0)
			{
				purseUpdateRequest.isTransitOnly = 0;
			}
			purseUpdateRequest.isTransitOnlySet = TRUE;
		}
		else if(strcmp(fieldName, "BlockingReason") == 0)
		{
			purseUpdateRequest.blockingReason = convertStringToU8(fieldValue);
		}
	}

	if(!returnValue)
	{
        CsDebug(CD_DEBUG, (CD_DEBUG, "MYKI_ACTIONLIST:getTPurseUpdateRequest: No TPurseUpdateRequest subtype %s found for Card UID %s", subtype, cardUID));
	}

	return returnValue;
}



// ProductUpdateRequest
int getProductUpdateRequest(const char * subtype, const char * cardUID, int actionSeqNo, MYKI_ACTIONLIST_ProductUpdateRequest_t &productUpdateRequest)
{
	int returnValue=false;
	memset(&productUpdateRequest, 0, sizeof(productUpdateRequest));

	doCardQuery(cardUID);
	Rows::const_iterator itr;

	bool firstRow = true;
	for(
		itr = firstMatchingCacheRow(MYKI_ACTIONLIST_PRODUCT_UPDATE, subtype, actionSeqNo);
		itr != g_cachedCardActionlistRows.end();
		itr = nextMatchingCacheRow(itr)
	   )
	{
		returnValue = true;
		const ColumnValue& cv = *itr;

		if ( firstRow )
		{
			const char* guid = cv["action_guid"];
			if ( (guid != 0) && (guid[0] != '\0') )
			{
				productUpdateRequest.isActionGuidSet = TRUE;
				stringCopy(productUpdateRequest.actionGuid, guid, LEN_MYKI_LDT_GUID);
			}
			const char* seqNo = cv["action_seq_no"];
			if ( (seqNo != 0) && (seqNo[0] != '\0') )
			{
				productUpdateRequest.isActionSequenceNoSet = TRUE;
				productUpdateRequest.actionSequenceNo = convertStringToU8(seqNo);
			}
			firstRow = false;
		}

		const char* fieldName = cv["field_name"];
		const char* fieldValue = cv["field_value"];

		if(strcmp(fieldName, "SerialNo") == 0)
		{
			productUpdateRequest.serialNo = convertStringToU16(fieldValue);
		}
		else if(strcmp(fieldName, "ServiceProviderId") == 0)
		{
			productUpdateRequest.serviceProviderId = convertStringToU16(fieldValue);
		}
		else if(strcmp(fieldName, "EntryPointId") == 0)
		{
			productUpdateRequest.entryPointId = convertStringToU16(fieldValue);
			productUpdateRequest.isEntryPointIdSet = TRUE;
		}
		else if(strcmp(fieldName, "IsAutoload") == 0)
		{
			if(strcmp(fieldValue, "true") == 0)
			{
				productUpdateRequest.isAutoload = 1;
			}
			else if(strcmp(fieldValue, "false") == 0)
			{
				productUpdateRequest.isAutoload = 0;
			}
			productUpdateRequest.isAutoloadSet = TRUE;
		}
		else if(strcmp(fieldName, "IsOffpeak") == 0)
		{
			if(strcmp(fieldValue, "true") == 0)
			{
				productUpdateRequest.isOffpeak = 1;
			}
			else if(strcmp(fieldValue, "false") == 0)
			{
				productUpdateRequest.isOffpeak = 0;
			}
			productUpdateRequest.isOffpeakSet = TRUE;
		}
		else if(strcmp(fieldName, "IsPremium") == 0)
		{
			if(strcmp(fieldValue, "true") == 0)
			{
				productUpdateRequest.isPremium = 1;
			}
			else if(strcmp(fieldValue, "false") == 0)
			{
				productUpdateRequest.isPremium = 0;
			}
			productUpdateRequest.isPremiumSet = TRUE;
		}
		else if(strcmp(fieldName, "IsProvisional") == 0)
		{
			if(strcmp(fieldValue, "true") == 0)
			{
				productUpdateRequest.isProvisional = 1;
			}
			else if(strcmp(fieldValue, "false") == 0)
			{
				productUpdateRequest.isProvisional = 0;
			}
			productUpdateRequest.isProvisionalSet = TRUE;
		}
		else if(strcmp(fieldName, "IsTripDirectionStatus") == 0)
		{
			if(strcmp(fieldValue, "true") == 0)
			{
				productUpdateRequest.isTripDirectionStatus = 1;
			}
			else if(strcmp(fieldValue, "false") == 0)
			{
				productUpdateRequest.isTripDirectionStatus = 0;
			}
			productUpdateRequest.isTripDirectionStatusSet = TRUE;
		}
		else if(strcmp(fieldName, "IsTripDirectionValue") == 0)
		{
			if(strcmp(fieldValue, "true") == 0)
			{
				productUpdateRequest.isTripDirectionValue = 1;
			}
			else if(strcmp(fieldValue, "false") == 0)
			{
				productUpdateRequest.isTripDirectionValue = 0;
			}
			productUpdateRequest.isTripDirectionValueSet = TRUE;
		}
		else if(strcmp(fieldName, "IsBorderStatus") == 0)
		{
			if(strcmp(fieldValue, "true") == 0)
			{
				productUpdateRequest.isBorderStatus = 1;
			}
			else if(strcmp(fieldValue, "false") == 0)
			{
				productUpdateRequest.isBorderStatus = 0;
			}
			productUpdateRequest.isBorderStatusSet = TRUE;
		}
		else if(strcmp(fieldName, "IsBorderSide") == 0)
		{
			if(strcmp(fieldValue, "true") == 0)
			{
				productUpdateRequest.isBorderSide = 1;
			}
			else if(strcmp(fieldValue, "false") == 0)
			{
				productUpdateRequest.isBorderSide = 0;
			}
			productUpdateRequest.isBorderSideSet = TRUE;
		}
		else if(strcmp(fieldName, "RouteId") == 0)
		{
			productUpdateRequest.routeId = convertStringToU16(fieldValue);
		}
		else if(strcmp(fieldName, "StopId") == 0)
		{
			productUpdateRequest.stopId = convertStringToU8(fieldValue);
		}
		else if(strcmp(fieldName, "InstanceCount") == 0)
		{
			productUpdateRequest.instanceCount = convertStringToU8(fieldValue);
			productUpdateRequest.isInstanceCountSet = TRUE;
		}
		else if(strcmp(fieldName, "StartDateTime") == 0)
		{
			U32_t startDateTime = convertStringToU32(fieldValue);

			DateMykiU24ToTimeT(startDateTime, &productUpdateRequest.startDateTime);
			productUpdateRequest.isStartDateTimeSet = TRUE;
		}
		else if(strcmp(fieldName, "EndDateTime") == 0)
		{
			U32_t endDateTime = convertStringToU32(fieldValue);

			DateMykiU24ToTimeT(endDateTime, &productUpdateRequest.endDateTime);
			productUpdateRequest.isEndDateTimeSet = TRUE;
		}
		else if(strcmp(fieldName, "ZoneLow") == 0)
		{
			productUpdateRequest.zoneLow = convertStringToU8(fieldValue);
			productUpdateRequest.isZoneLowSet = TRUE;
		}
		else if(strcmp(fieldName, "ZoneHigh") == 0)
		{
			productUpdateRequest.zoneHigh = convertStringToU8(fieldValue);
			productUpdateRequest.isZoneHighSet = TRUE;
		}
		else if(strcmp(fieldName, "PurchaseValue") == 0)
		{
			productUpdateRequest.purchaseValue = convertStringToU32(fieldValue);
			productUpdateRequest.isPurchaseValueSet = TRUE;
		}
		else if(strcmp(fieldName, "ClearBorderStatus") == 0)
		{
			if(strcmp(fieldValue, "true") == 0)
			{
				productUpdateRequest.clearBorderStatus = 1;
			}
			else if(strcmp(fieldValue, "false") == 0)
			{
				productUpdateRequest.clearBorderStatus = 0;
			}
			productUpdateRequest.isClearBorderStatusSet = TRUE;
		}
	}

	if(!returnValue)
	{
        CsDebug(CD_DEBUG, (CD_DEBUG, "MYKI_ACTIONLIST:getProductUpdateRequest: No ProductUpdateRequest subtype %s found for Card UID %s", subtype, cardUID));
	}

	return returnValue;
}



// OAppUpdateRequest
int getOAppUpdateRequestBlock(const char * cardUID, int actionSeqNo, MYKI_ACTIONLIST_OAppUpdateRequest_t &oAppUpdateRequest)
{
	return getOAppUpdateRequest(cardUID, actionSeqNo, MYKI_ACTIONLIST_BLOCK, oAppUpdateRequest);
}

int getOAppUpdateRequestUnblock(const char * cardUID, int actionSeqNo, MYKI_ACTIONLIST_OAppUpdateRequest_t &oAppUpdateRequest)
{
	return getOAppUpdateRequest(cardUID, actionSeqNo, MYKI_ACTIONLIST_UNBLOCK, oAppUpdateRequest);
}

int getOAppUpdateRequest(const char * cardUID, int actionSeqNo, const char* subtype, MYKI_ACTIONLIST_OAppUpdateRequest_t &oAppUpdateRequest)
{
	int returnValue=false;
	memset(&oAppUpdateRequest, 0, sizeof(oAppUpdateRequest));

	doCardQuery(cardUID);
	Rows::const_iterator itr;

	bool firstRow = true;
	for(
		itr = firstMatchingCacheRow(MYKI_ACTIONLIST_OAPP_UPDATE, subtype, actionSeqNo);
		itr != g_cachedCardActionlistRows.end();
		itr = nextMatchingCacheRow(itr)
	   )
	{
		returnValue = true;
		const ColumnValue& cv = *itr;

		if ( firstRow )
		{
			const char* guid = cv["action_guid"];
			if ( (guid != 0) && (guid[0] != '\0') )
			{
				oAppUpdateRequest.isActionGuidSet = TRUE;
				stringCopy(oAppUpdateRequest.actionGuid, guid, LEN_MYKI_LDT_GUID);
			}
			const char* seqNo = cv["action_seq_no"];
			if ( (seqNo != 0) && (seqNo[0] != '\0') )
			{
				oAppUpdateRequest.isActionSequenceNoSet = TRUE;
				oAppUpdateRequest.actionSequenceNo = convertStringToU8(seqNo);
			}
			firstRow = false;
		}

		const char* fieldName = cv["field_name"];
		const char* fieldValue = cv["field_value"];

		if(strcmp(fieldName, "ServiceProviderId") == 0)
		{
			oAppUpdateRequest.serviceProviderId = convertStringToU16(fieldValue);
		}
		else if(strcmp(fieldName, "ExpiryDate") == 0)
		{
			oAppUpdateRequest.expiryDate = convertStringToU16(fieldValue);
			if(oAppUpdateRequest.expiryDate>0) // Check may not be required if 1800-01-01 is seen in LDTs
				oAppUpdateRequest.expiryDate += JULIEN_BASE_FOR_DATEC19_2006;			
			oAppUpdateRequest.isExpiryDateSet = TRUE;
		}
		else if(strcmp(fieldName, "StaffId") == 0)
		{
			stringMemCopy(oAppUpdateRequest.staffId, fieldValue, sizeof(oAppUpdateRequest.staffId));
			oAppUpdateRequest.isStaffIdSet = TRUE;
		}
		else if(strcmp(fieldName, "StaffExpDate") == 0)
		{
			oAppUpdateRequest.staffExpDate = convertStringToU16(fieldValue);
			if(oAppUpdateRequest.staffExpDate>0)
				oAppUpdateRequest.staffExpDate += JULIEN_BASE_FOR_DATEC19_2006;				
			oAppUpdateRequest.isStaffExpDateSet = TRUE;
		}
		else if(strcmp(fieldName, "StaffProviderId") == 0)
		{
			oAppUpdateRequest.staffProviderId = convertStringToU16(fieldValue);
			oAppUpdateRequest.isStaffProviderIdSet = TRUE;
		}
		else if(strcmp(fieldName, "StaffBaseId") == 0)
		{
			oAppUpdateRequest.staffBaseId = convertStringToU8(fieldValue);
			oAppUpdateRequest.isStaffBaseIdSet = TRUE;
		}
		else if(strcmp(fieldName, "BlockingReason") == 0)
		{
			oAppUpdateRequest.blockingReason = convertStringToU8(fieldValue);
		}
		else if(strcmp(fieldName, "Pin") == 0)
		{
			stringMemCopy(oAppUpdateRequest.pin, fieldValue, sizeof(oAppUpdateRequest.pin));
			oAppUpdateRequest.isPinSet = TRUE;
		}
	}

	if(!returnValue)
	{
        CsDebug(CD_DEBUG, (CD_DEBUG, "MYKI_ACTIONLIST:getOAppUpdateRequest: No OAppUpdateRequest subtype %s found for Card UID %s", subtype, cardUID));
	}

	return returnValue;
}

