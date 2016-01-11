/*
 * Stations.h
 *
 *  Created on: 08/10/2012
 *      Author: James Ho
 */

#ifndef STATIONS_H_
#define STATIONS_H_

#include "DbAccess.h"
#include "myki_cd.h"
#include "ProductConfigurationUtils.h"

#ifdef __cplusplus
extern "C" {
#endif

int processStationsXML();
int initStationsLookup();

int getStations(U16_t id, MYKI_CD_Stations_t &stations);
int getStationsByLocationId(U16_t locationId, MYKI_CD_Stations_t &stations);

#ifdef __cplusplus
}
#endif

#endif /* STATIONS_H_ */
