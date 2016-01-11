/*
 * myki_actionlist.cpp
 *
 *  Created on: 10/10/2012
 *      Author: James Ho
 */

#include "myki_actionlist.h"
#include "Actionlist.h"
#include "DeviceHotlist.h"


// Import actionlist delta SQL
int MYKI_ACTIONLIST_importActionlistSQLDelta(const char* filename)
{
	int result = false;

	if ( filename == NULL )
	{
		CsErrx("MYKI_ACTIONLIST:importActionlistSQLDelta: null filename");
	}
	else
	{
		DbAccess* actionlistDB = getActionlistDBAccess();
		if ( actionlistDB == NULL )
		{
			CsErrx("MYKI_ACTIONLIST:importActionlistSQLDelta: no actionlist DB");
		}
		else
		{
			if ( actionlistDB->executeSqlScript(filename) == SQLITE_OK )
			{
				initActionlistLookupTables();
				result = true;
			}
			else
			{
				CsErrx("MYKI_ACTIONLIST:importActionlistSQLDelta: failed to execute script %s", filename);
			}
		}
	}

	return result;
}

int MYKI_ACTIONLIST_getSequence()
{
    return getActionlistSequence();
}

const char* MYKI_ACTIONLIST_getSequenceInfo()
{
    return getActionlistSequenceInfo();
}

/*==========================================================================*
**
**  MYKI_ACTIONLIST_getDetails
**
**  Description     :
**      Returns actionlist details, ie. name + sequence number + time stamp.
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

const   char   *MYKI_ACTIONLIST_getDetails( void )
{
    return  getActionlistDetails( );
}   /*  MYKI_ACTIONLIST_getDetails( ) */

int MYKI_ACTIONLIST_getMajorVersion()
{
    return 0;
}

int MYKI_ACTIONLIST_getMinorVersion()
{
    return 0;
}

int MYKI_ACTIONLIST_getDeviceHotlistSequence()
{
	return getDeviceHotlistSequence();
}

const char* MYKI_ACTIONLIST_getDeviceHotlistSequenceInfo()
{
	return getDeviceHotlistSequenceInfo();
}

// Import Actionlist XML
int MYKI_ACTIONLIST_processActionlistXML()
{
	return processActionlistXML();
}

int MYKI_ACTIONLIST_processDeviceHotlistXML()
{
	return processDeviceHotlistXML();
}


// Actionlist isCardActionlist
int MYKI_ACTIONLIST_isCardActionlist(const char * cardUID)
{
    return isCardActionlist(cardUID);
}

int MYKI_ACTIONLIST_isDeviceHotlisted(U32_t samId, DateC19_t requestDate)
{
	return isDeviceHotlisted(samId, requestDate);
}

// Card Actionlist Requests
int MYKI_ACTIONLIST_getCardActionlistRequests(const char * cardUID, MYKI_ACTIONLIST_Card_Actionlist_Requests_t * cardActionlistRequests)
{
	return getCardActionlistRequests(cardUID,  * cardActionlistRequests);
}

// TAppUpdateRequestNone
int MYKI_ACTIONLIST_getTAppUpdateRequestNone(const char * cardUID, int actionSeqNo, MYKI_ACTIONLIST_TAppUpdateRequest_t * tAppUpdateRequest)
{
	return getTAppUpdateRequestNone(cardUID, actionSeqNo, * tAppUpdateRequest);
}

// TAppUpdateRequestActivate
int MYKI_ACTIONLIST_getTAppUpdateRequestActivate(const char * cardUID, int actionSeqNo, MYKI_ACTIONLIST_TAppUpdateRequest_t * tAppUpdateRequest)
{
	return getTAppUpdateRequestActivate(cardUID, actionSeqNo, * tAppUpdateRequest);
}

// TAppUpdateRequestBlock
int MYKI_ACTIONLIST_getTAppUpdateRequestBlock(const char * cardUID, int actionSeqNo, MYKI_ACTIONLIST_TAppUpdateRequest_t * tAppUpdateRequest)
{
	return getTAppUpdateRequestBlock(cardUID, actionSeqNo, * tAppUpdateRequest);
}

// TAppUpdateRequestUnblock
int MYKI_ACTIONLIST_getTAppUpdateRequestUnblock(const char * cardUID, int actionSeqNo, MYKI_ACTIONLIST_TAppUpdateRequest_t * tAppUpdateRequest)
{
	return getTAppUpdateRequestUnblock(cardUID, actionSeqNo, * tAppUpdateRequest);
}

// TPurseLoadRequestNone
int MYKI_ACTIONLIST_getTPurseLoadRequestNone(const char * cardUID, int actionSeqNo, MYKI_ACTIONLIST_TPurseLoadRequest_t * tPurseLoadRequest)
{
	return getTPurseLoadRequest(MYKI_ACTIONLIST_NONE, cardUID, actionSeqNo, * tPurseLoadRequest);
}

// TPurseLoadRequestDebit
int MYKI_ACTIONLIST_getTPurseLoadRequestDebit(const char * cardUID, int actionSeqNo, MYKI_ACTIONLIST_TPurseLoadRequest_t * tPurseLoadRequest)
{
	return getTPurseLoadRequest(MYKI_ACTIONLIST_DEBIT, cardUID, actionSeqNo, * tPurseLoadRequest);
}

// ProductSaleRequestNone
int MYKI_ACTIONLIST_getProductSaleRequestNone(const char * cardUID, int actionSeqNo, MYKI_ACTIONLIST_ProductSaleRequest_t * productSaleRequests)
{
	return getProductSaleRequestNone(cardUID, actionSeqNo, * productSaleRequests);
}

// TPurseUpdateRequestNone
int MYKI_ACTIONLIST_getTPurseUpdateRequestNone(const char * cardUID, int actionSeqNo, MYKI_ACTIONLIST_TPurseUpdateRequest_t * purseUpdateRequest)
{
	return getTPurseUpdateRequest(MYKI_ACTIONLIST_NONE, cardUID, actionSeqNo, * purseUpdateRequest);
}

// TPurseUpdateRequestBlock
int MYKI_ACTIONLIST_getTPurseUpdateRequestBlock(const char * cardUID, int actionSeqNo, MYKI_ACTIONLIST_TPurseUpdateRequest_t * tPurseUpdateRequest)
{
	return getTPurseUpdateRequest(MYKI_ACTIONLIST_BLOCK, cardUID, actionSeqNo, * tPurseUpdateRequest);
}

// TPurseUpdateRequestUnblock
int MYKI_ACTIONLIST_getTPurseUpdateRequestUnblock(const char * cardUID, int actionSeqNo, MYKI_ACTIONLIST_TPurseUpdateRequest_t * tPurseUpdateRequest)
{
	return getTPurseUpdateRequest(MYKI_ACTIONLIST_UNBLOCK, cardUID, actionSeqNo, * tPurseUpdateRequest);
}


// ProductUpdateRequestNone
int MYKI_ACTIONLIST_getProductUpdateRequestNone(const char * cardUID, int actionSeqNo, MYKI_ACTIONLIST_ProductUpdateRequest_t * productUpdateRequest)
{
	return getProductUpdateRequest(MYKI_ACTIONLIST_NONE, cardUID, actionSeqNo, * productUpdateRequest);
}

// ProductUpdateRequestBlock
int MYKI_ACTIONLIST_getProductUpdateRequestBlock(const char * cardUID, int actionSeqNo, MYKI_ACTIONLIST_ProductUpdateRequest_t * productUpdateRequest)
{
	return getProductUpdateRequest(MYKI_ACTIONLIST_BLOCK, cardUID, actionSeqNo, * productUpdateRequest);
}

// ProductUpdateRequestUnblock
int MYKI_ACTIONLIST_getProductUpdateRequestUnblock(const char * cardUID, int actionSeqNo, MYKI_ACTIONLIST_ProductUpdateRequest_t * productUpdateRequest)
{
	return getProductUpdateRequest(MYKI_ACTIONLIST_UNBLOCK, cardUID, actionSeqNo, * productUpdateRequest);
}

// OAppUpdateRequestBlock
int MYKI_ACTIONLIST_getOAppUpdateRequestBlock(const char * cardUID, int actionSeqNo, MYKI_ACTIONLIST_OAppUpdateRequest_t * oAppUpdateRequest)
{
	return getOAppUpdateRequestBlock(cardUID, actionSeqNo, * oAppUpdateRequest);
}

// OAppUpdateRequestUnblock
int MYKI_ACTIONLIST_getOAppUpdateRequestUnblock(const char * cardUID, int actionSeqNo, MYKI_ACTIONLIST_OAppUpdateRequest_t * oAppUpdateRequest)
{
	return getOAppUpdateRequestUnblock(cardUID, actionSeqNo, * oAppUpdateRequest);
}

