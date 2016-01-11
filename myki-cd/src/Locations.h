/*
 * Locations.h
 *
 *  Created on: 08/10/2012
 *      Author: James Ho
 */

#ifndef LOCATIONS_H_
#define LOCATIONS_H_

#include "DbAccess.h"
#include "myki_cd.h"
#include "ProductConfigurationUtils.h"


#ifdef __cplusplus
extern "C" {
#endif

int processLocationsXML();

int getLocations(U16_t id, MYKI_CD_Locations_t &locations);
int initLocationLookup();

#ifdef __cplusplus
}
#endif

#endif /* LOCATIONS_H_ */
