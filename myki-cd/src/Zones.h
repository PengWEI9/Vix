/*
 * Stations.h
 *
 *  Created on: 08/10/2012
 *      Author: James Ho
 */

#ifndef ZONES_H_
#define ZONES_H_

#include "DbAccess.h"
#include "myki_cd.h"
#include "ProductConfigurationUtils.h"

#ifdef __cplusplus
extern "C" {
#endif

int processZonesXML();
int initZonesLookup();
int isCitySaverZone(U8_t zoneId);

#ifdef __cplusplus
}
#endif

#endif /* ZONES_H_ */
