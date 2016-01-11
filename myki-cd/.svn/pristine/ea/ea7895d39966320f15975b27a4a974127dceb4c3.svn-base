/*
 * Routes.h
 *
 *  Created on: 14/07/2014
 *      Author: Morgan Dell
 */

#ifndef ROUTES_H_
#define ROUTES_H_

#include "myki_cd.h"

#ifdef __cplusplus
extern "C" {
#endif

int processRoutesXML();

int getRouteInfo(U16_t routeId, MYKI_CD_RouteInfo_t& routeInfo);
int getServiceProviderRoutes(U16_t serviceProviderId, MYKI_CD_RouteInfo_t* pRoutes, U16_t arrayLength);
int initRoutesLookup();

#ifdef __cplusplus
}
#endif

#endif /* ROUTES_H_ */
