#ifndef VCARD_H_
#define VCARD_H_

#include <stdio.h>
#include <stdlib.h>

#include <ct.h>
#include <myki_cardservices.h>
#include <myki_cdd_types.h>

typedef struct MYKI_Card
{
    CT_CardInfo_t      *pCT_CardInfo;
    MYKI_CAIssuer_t    *pCAIssuer; 
    MYKI_CAControl_t   *pCAControl; 
    MYKI_TAIssuer_t    *pTAIssuer; 
    MYKI_TAControl_t   *pTAControl;
    MYKI_TACapping_t   *pTACapping;
    MYKI_TAProduct_t   *pTAProduct[MYKI_MAX_CONTRACTS];
    MYKI_TAPurseControl_t *pTAPurseControl;
    MYKI_TAPurseBalance_t *pTAPurseBalance;
    MYKI_TAUsageLogRecordList_t *pTAUsageLogRecordList;
    MYKI_TALoadLogRecordList_t  *pTALoadLogRecordList;
/*
    MYKI_OAIssuer_t *pOAIssuer;
    MYKI_OAControl_t *pOAControl;
    MYKI_OAInspectorData_t *pOAInspectorData;
    MYKI_OAPIN_t *pOAPIN;
    MYKI_OARoles_t *pOARoles;
    MYKI_OAShiftData_t *pOARoles;
    MYKI_OAShiftDataControl_t *pOAShiftDataControl;
    MYKI_OAShiftLog_t *pOAShiftLog 
*/
} MYKI_Card_t;

#ifdef __cplusplus
extern "C" 
{
#endif

bool card_detect(int argc, const char *argv[], void *data );

bool card_load(int argc, const char *argv[], void *data );

bool card_write(int argc, const char *argv[], void *data );

bool card_image(int argc, const char *argv[], void *data );

bool card_close();
bool card_notify();
 
char* card_enquiry();

    /**
     *      Processes "card unconfirmed" CLI command.
     *      @param  argc number of arguments, ie. 1.
     *      @param  argv argument values, ie. argv[0] = "true" or "false".
     *      @param  data not used.
     *      @return true if successful; false otherwise.
     */
bool card_unconfirmed( int argc, const char *argv[], void *data );

    /**
     *      Processes "card snapshot" CLI command.
     *      @param  argc number of arguments, ie. 1.
     *      @param  argv argument values, ie. argv[0] = "true" or "false".
     *      @param  data not used.
     *      @return true if successful; false otherwise.
     */
bool card_snapshot( int argc, const char *argv[], void *data );

    /**
     *  @brief  Processes 'card validatetransitcard' CLI command.
     *  This function formats and "sends" a JSON 'validatetransitcard' request.
     *  @return true if success; false otherwise.
     */
bool card_validatetransitcard( );

//bool vCardPrint(MYKI_CAIssuer_t *pCAIssuer, MYKI_CAControl_t *pCAControl, MYKI_TAIssuer_t *pTAIssuer, MYKI_TAIssuer_t *pTAIssuer);

#ifdef __cplusplus
}
#endif

#endif /*VCARD_H */
