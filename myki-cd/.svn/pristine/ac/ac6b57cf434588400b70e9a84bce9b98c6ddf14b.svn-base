/*
 * FaresEpass.h
 *
 *  Created on: 08/10/2012
 *      Author: James Ho
 */

#ifndef FARESEPASS_H_
#define FARESEPASS_H_

#include "DbAccess.h"
#include "myki_cd.h"
#include "ProductConfigurationUtils.h"

#ifdef __cplusplus
extern "C" {
#endif

int processFaresEpassXML();

int initFaresEpassLookup();
int getFaresEpass(U8_t zoneLow, U8_t zoneHigh, U16_t days, MYKI_CD_FaresEpass_t &faresEpass);

#ifdef __cplusplus
}
#endif

#endif /* FARESEPASS_H_ */
