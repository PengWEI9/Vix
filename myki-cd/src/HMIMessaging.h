/*
 * HMIMessaging.h
 *
 *  Created on: 08/10/2012
 *      Author: James Ho
 */

#ifndef HMIMessaging_H_
#define HMIMessaging_H_

#include "DbAccess.h"
#include "myki_cd.h"
#include "ProductConfigurationUtils.h"

#ifdef __cplusplus
extern "C" {
#endif

int processHMIMessagingXML();
int initHMIMessagingLookup();

int getHMIMessaging(char* tableType, MYKI_CD_HMIMessaging_t *HMIMessaging, U16_t arrayLength);

#ifdef __cplusplus
}
#endif

#endif /* HMIMessaging_H_ */
