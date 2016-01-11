/*
 * ServiceProviders.h
 *
 *  Created on: 08/10/2012
 *      Author: James Ho
 */

#ifndef SERVICEPROVIDERS_H_
#define SERVICEPROVIDERS_H_

#include "DbAccess.h"
#include "myki_cd.h"
#include "ProductConfigurationUtils.h"

#ifdef __cplusplus
extern "C" {
#endif

int processServiceProvidersXML();
int initServiceProvidersLookup();

int getServiceProviders(U16_t id, MYKI_CD_ServiceProviders_t &serviceProviders);
int getServiceProviderTransportMode(U16_t id, char* mode, int bufferLength);

#ifdef __cplusplus
}
#endif

#endif /* SERVICEPROVIDERS_H_ */
