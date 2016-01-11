/*
 * PassengerTypes.h
 *
 *  Created on: 04/10/2012
 *      Author: James Ho
 */

#ifndef PASSENGERTYPES_H_
#define PASSENGERTYPES_H_

#include "DbAccess.h"
#include "myki_cd.h"
#include "ProductConfigurationUtils.h"

#ifdef __cplusplus
extern "C" {
#endif

int processPassengerTypesXML();
int initPassengerTypesLookup();

int getPassengerType(U8_t id, MYKI_CD_PassengerType_t &passengerType);

#ifdef __cplusplus
}
#endif

#endif /* PASSENGERTYPES_H_ */
