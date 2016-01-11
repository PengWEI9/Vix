/*
 * DeviceParameters.h
 *
 *  Created on: 01/10/2012
 *      Author: James Ho
 */

#ifndef DEVICEPARAMETERS_H_
#define DEVICEPARAMETERS_H_

#include "DbAccess.h"
#include "myki_cd.h"
#include "ProductConfigurationUtils.h"

#ifdef __cplusplus
extern "C" {
#endif

int setDeviceParameters(
	const char* deviceType, // set to null for unknown
	MYKI_CD_Mode_t mode,
	U8_t zone, // set to 0 for unknown
	U16_t locationId, // set to 0 for unknown
	U8_t passengerType, // set to 0 for unknown
	U16_t serviceProvider, // set to 0 for unknown
	U8_t blockingReason, // set to 0 for unknown
	U8_t paymentMethod // set to 0 for unknown
	);

int initDeviceParametersLookup();
int processDeviceParametersXML();
int getDeviceParameter(const std::string& deviceParameterName, MYKI_CD_DeviceParameter_t &deviceParameter);
int getDeviceParameterBySP(const std::string& deviceParameterName, U16_t serviceProvider, MYKI_CD_DeviceParameter_t &deviceParameter);
int getDeviceParameterBySPandMode(const std::string& deviceParameterName, U16_t serviceProvider, MYKI_CD_Mode_t mode, MYKI_CD_DeviceParameter_t &deviceParameter);

int getPaymentMethod(const char* deviceType, U16_t serviceProvider, const char* paymentMethod, MYKI_CD_DeviceParameter_t &deviceParameter);
int isEarlyBirdStation(U16_t stationId);
int isEarlyBirdDay(int day);
int getEarlyBirdProductId(void);
int getEarlyBirdCutOffTime(void);

#ifdef __cplusplus
}
#endif

#endif /* DEVICEPARAMETERS_H_ */
