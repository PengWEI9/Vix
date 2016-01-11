/*
 * Actionlist.h
 *
 *  Created on: 08/10/2012
 *      Author: James Ho
 */

#ifndef ACTIONLIST_H_
#define ACTIONLIST_H_

#include "DbAccess.h"
#include "myki_actionlist.h"
#include "ProductConfigurationUtils.h"

#ifdef __cplusplus
extern "C" {
#endif

#define MYKI_ACTIONLIST_PRODUCT_SALE "ProductSaleRequest"
#define MYKI_ACTIONLIST_TAPP_UPDATE "TAppUpdateRequest"
#define MYKI_ACTIONLIST_TPURSE_LOAD "TPurseLoadRequest"
#define MYKI_ACTIONLIST_TPURSE_UPDATE "TPurseUpdateRequest"
#define MYKI_ACTIONLIST_PRODUCT_UPDATE "ProductUpdateRequest"
#define MYKI_ACTIONLIST_OAPP_UPDATE "OAppUpdateRequest"

#define MYKI_ACTIONLIST_ACTIVATE "Activate"
#define MYKI_ACTIONLIST_NONE "None"
#define MYKI_ACTIONLIST_DEBIT "Debit"
#define MYKI_ACTIONLIST_UNBLOCK "Unblock"
#define MYKI_ACTIONLIST_BLOCK "Block"

int processActionlistXML();
int initActionlistLookup();
int getActionlistSequence();
const char* getActionlistSequenceInfo();

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
const char *getActionlistDetails( void );

int isCardActionlist(const char * cardUID);

int getCardActionlistRequests(const char * cardUID, MYKI_ACTIONLIST_Card_Actionlist_Requests_t &cardActionlistRequests);


// TAppUpdateRequest
int getTAppUpdateRequestNone(const char * cardUID,  int actionSeqNo, MYKI_ACTIONLIST_TAppUpdateRequest_t &tAppUpdateRequest);
int getTAppUpdateRequestActivate(const char * cardUID, int actionSeqNo, MYKI_ACTIONLIST_TAppUpdateRequest_t &tAppUpdateRequest);
int getTAppUpdateRequestBlock(const char * cardUID, int actionSeqNo, MYKI_ACTIONLIST_TAppUpdateRequest_t &tAppUpdateRequest);
int getTAppUpdateRequestUnblock(const char * cardUID, int actionSeqNo, MYKI_ACTIONLIST_TAppUpdateRequest_t &tAppUpdateRequest);

int getTAppUpdateRequest(const char * cardUID, int actionSeqNo, const char* subtype, MYKI_ACTIONLIST_TAppUpdateRequest_t &tAppUpdateRequest);

// TPurseLoadRequest
int getTPurseLoadRequest(const char * subtype, const char * cardUID, int actionSeqNo, MYKI_ACTIONLIST_TPurseLoadRequest_t &tPurseLoadRequest);

// ProductSaleRequest
int getProductSaleRequestNone(const char * cardUID, int actionSeqNo, MYKI_ACTIONLIST_ProductSaleRequest_t &productSaleRequests);

// TPurseUpdateRequest
int getTPurseUpdateRequest(const char * subtype, const char * cardUID, int actionSeqNo, MYKI_ACTIONLIST_TPurseUpdateRequest_t &purseUpdateRequest);

// ProductUpdateRequest
int getProductUpdateRequest(const char * subtype, const char * cardUID, int actionSeqNo, MYKI_ACTIONLIST_ProductUpdateRequest_t &productUpdateRequest);

// OAppUpdateRequest
int getOAppUpdateRequestBlock(const char * cardUID, int actionSeqNo, MYKI_ACTIONLIST_OAppUpdateRequest_t &oAppUpdateRequest);
int getOAppUpdateRequestUnblock(const char * cardUID, int actionSeqNo, MYKI_ACTIONLIST_OAppUpdateRequest_t &oAppUpdateRequest);

int getOAppUpdateRequest(const char * cardUID, int actionSeqNo, const char* subtype, MYKI_ACTIONLIST_OAppUpdateRequest_t &oAppUpdateRequest);

#ifdef __cplusplus
}
#endif

#endif /* ACTIONLIST_H_ */

