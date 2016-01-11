/*
 * MasterRoutes.h
 *
 *  Created on: 14/07/2014
 *      Author: Morgan Dell
 */

#ifndef MASTER_ROUTES_H_
#define MASTER_ROUTES_H_

#include "DbAccess.h"
#include "myki_cd.h"
#include "ProductConfigurationUtils.h"

#ifdef __cplusplus
extern "C" {
#endif

int processMasterRoutesXML();
int initMasterRoutesLookup();

int getMasterRoute(U16_t masterRouteId, U16_t serviceProviderId, MYKI_CD_MasterRoute_t& masterRoute);

#ifdef __cplusplus
}
#endif

#endif /* MASTER_ROUTES_H_ */
