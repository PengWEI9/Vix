/*
 * myki_actionlist.h
 *
 *  Created on: 10/10/2012
 *      Author: James Ho
 */

#ifndef MYKI_ACTIONLIST_H_
#define MYKI_ACTIONLIST_H_

#include <corebasetypes.h>
#include <LDTStructs.h>

#ifdef __cplusplus
extern "C" {
#endif

#define MYKI_ACTIONLIST_SUCCESS				0
#define MYKI_ACTIONLIST_FAILED				2

#define LEN_CardUid_t 16

typedef enum
{
	MYKI_ACTIONLIST_REQUEST_TYPE_UNKNOWN = 0,
	MYKI_ACTIONLIST_REQUEST_TYPE_ADDITION = 1
} MYKI_ACTIONLIST_RequestType_t;

// Card Actionlist Request
typedef struct
{
	MYKI_ACTIONLIST_RequestType_t requestType;
	U8_t type;
	U8_t subtype;
	U8_t action_seq_no;
	Time_t expiry;
	MYKI_LDT_GUID_t action_guid;
} MYKI_ACTIONLIST_Request_t;

// Card Actionlist Requests
typedef struct
{
	char card_uid[LEN_CardUid_t];
	U8_t numberOfRequests;
	MYKI_ACTIONLIST_Request_t* arrayOfRequest;
} MYKI_ACTIONLIST_Card_Actionlist_Requests_t;

// TAppUpdate Request Structure
typedef TAppUpdate_t MYKI_ACTIONLIST_TAppUpdateRequest_t;

// TPurseLoad Request Structure
typedef TPurseLoad_t MYKI_ACTIONLIST_TPurseLoadRequest_t;

// ProductSale Request Structure
typedef ProductSale_t MYKI_ACTIONLIST_ProductSaleRequest_t;

// TPurseUpdate Request Structure
typedef TPurseUpdate_t MYKI_ACTIONLIST_TPurseUpdateRequest_t;

// ProductUpdate Request Structure
typedef ProductUpdate_t MYKI_ACTIONLIST_ProductUpdateRequest_t;

// OAppUpdate Request Structure
typedef OAppUpdate_t MYKI_ACTIONLIST_OAppUpdateRequest_t;

// Import actionlist delta SQL
int MYKI_ACTIONLIST_importActionlistSQLDelta(const char* filename);

// Import Actionlist XML
int MYKI_ACTIONLIST_processActionlistXML();
int MYKI_ACTIONLIST_processDeviceHotlistXML();

// Return the version info for the in-use actionlist.
int MYKI_ACTIONLIST_getSequence();
const char* MYKI_ACTIONLIST_getSequenceInfo();
int MYKI_ACTIONLIST_getMajorVersion();
int MYKI_ACTIONLIST_getMinorVersion();

    /**
     *      Gets actionlist details. The returned actionlist records
     *      are separated by a new line and have the format of
     *      ssssss nnnnnn yyyy-mm-ddTHH:MM:SS, where ssssss is the
     *      full/delta actionlist name, nnnnnn is the actionlist
     *      sequence number and yyyy-mm-ddTHH:MM:SS is the actionlist
     *      timestamp.
     *
     *      @return printable actionlist details description.
     */
const   char   *MYKI_ACTIONLIST_getDetails( void );

// Return the version info for the in-use device hotlist
int MYKI_ACTIONLIST_getDeviceHotlistSequence();
const char* MYKI_ACTIONLIST_getDeviceHotlistSequenceInfo();

int MYKI_ACTIONLIST_isCardActionlist(const char * cardUID);
int MYKI_ACTIONLIST_getCardActionlistRequests(const char * cardUID, MYKI_ACTIONLIST_Card_Actionlist_Requests_t * cardActionlistRequests);

int MYKI_ACTIONLIST_isDeviceHotlisted(U32_t samId, DateC19_t requestDate);

// TAppUpdateRequest
int MYKI_ACTIONLIST_getTAppUpdateRequestNone(const char * cardUID, int actionSeqNo, MYKI_ACTIONLIST_TAppUpdateRequest_t * tAppUpdateRequest);
int MYKI_ACTIONLIST_getTAppUpdateRequestActivate(const char * cardUID, int actionSeqNo, MYKI_ACTIONLIST_TAppUpdateRequest_t * tAppUpdateRequest);
int MYKI_ACTIONLIST_getTAppUpdateRequestBlock(const char * cardUID, int actionSeqNo, MYKI_ACTIONLIST_TAppUpdateRequest_t * tAppUpdateRequest);
int MYKI_ACTIONLIST_getTAppUpdateRequestUnblock(const char * cardUID, int actionSeqNo, MYKI_ACTIONLIST_TAppUpdateRequest_t * tAppUpdateRequest);

// TPurseLoadRequest
int MYKI_ACTIONLIST_getTPurseLoadRequestNone(const char * cardUID, int actionSeqNo, MYKI_ACTIONLIST_TPurseLoadRequest_t * tPurseLoadRequest);
int MYKI_ACTIONLIST_getTPurseLoadRequestDebit(const char * cardUID, int actionSeqNo, MYKI_ACTIONLIST_TPurseLoadRequest_t * tPurseLoadRequest);

// ProductSaleRequest
int MYKI_ACTIONLIST_getProductSaleRequestNone(const char * cardUID, int actionSeqNo, MYKI_ACTIONLIST_ProductSaleRequest_t * productSaleRequests);

// TPurseUpdateRequest
int MYKI_ACTIONLIST_getTPurseUpdateRequestNone(const char * cardUID, int actionSeqNo, MYKI_ACTIONLIST_TPurseUpdateRequest_t * purseUpdateRequest);
int MYKI_ACTIONLIST_getTPurseUpdateRequestBlock(const char * cardUID, int actionSeqNo, MYKI_ACTIONLIST_TPurseUpdateRequest_t * tPurseUpdateRequest);
int MYKI_ACTIONLIST_getTPurseUpdateRequestUnblock(const char * cardUID, int actionSeqNo, MYKI_ACTIONLIST_TPurseUpdateRequest_t * tPurseUpdateRequest);

// ProductUpdateRequest
int MYKI_ACTIONLIST_getProductUpdateRequestNone(const char * cardUID, int actionSeqNo, MYKI_ACTIONLIST_ProductUpdateRequest_t * productUpdateRequest);
int MYKI_ACTIONLIST_getProductUpdateRequestBlock(const char * cardUID, int actionSeqNo, MYKI_ACTIONLIST_ProductUpdateRequest_t * productUpdateRequest);
int MYKI_ACTIONLIST_getProductUpdateRequestUnblock(const char * cardUID, int actionSeqNo, MYKI_ACTIONLIST_ProductUpdateRequest_t * productUpdateRequest);
		
// OAppUpdateRequest
int MYKI_ACTIONLIST_getOAppUpdateRequestBlock(const char * cardUID, int actionSeqNo, MYKI_ACTIONLIST_OAppUpdateRequest_t * oAppUpdateRequest);
int MYKI_ACTIONLIST_getOAppUpdateRequestUnblock(const char * cardUID, int actionSeqNo, MYKI_ACTIONLIST_OAppUpdateRequest_t * oAppUpdateRequest);

#ifdef __cplusplus
}
#endif

#endif /* MYKI_ACTIONLIST_H_ */
