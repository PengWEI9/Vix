/*
 * StationLines.h
 *
 *  Created on: 08/10/2012
 *      Author: James Ho
 */

#ifndef STATIONLINES_H_
#define STATIONLINES_H_

#include "DbAccess.h"
#include "myki_cd.h"
#include "ProductConfigurationUtils.h"

#ifdef __cplusplus
extern "C" {
#endif

int processStationLinesXML();
int initStationLinesLookup();

int getLinesByStation(U16_t stationId, MYKI_CD_LinesByStation_t &linesByStation, MYKI_CD_U16Array_t&);
int getStationsByLine(U16_t lineId, MYKI_CD_StationsByLine_t &stationsByLine, MYKI_CD_U16Array_t&);

#ifdef __cplusplus
}
#endif

#endif /* STATIONLINES_H_ */
