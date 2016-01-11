/*
 * RouteSections.h
 *
 *  Created on: 14/07/2014
 *      Author: Morgan Dell
 */

#ifndef ROUTE_SECTIONS_H_
#define ROUTE_SECTIONS_H_

#include "myki_cd.h"

#ifdef __cplusplus
extern "C" {
#endif

int processRouteSectionsXML();

int getRouteSection(U16_t routeId, U8_t stopId, MYKI_CD_RouteSection_t& routeSection);
int initRouteSectionsLookup();

#ifdef __cplusplus
}
#endif

#endif /* ROUTE_SECTIONS_H_ */
