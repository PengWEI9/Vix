/*
 * ProductIssuers.h
 *
 *  Created on: 04/10/2012
 *      Author: James Ho
 */

#ifndef PRODUCTISSUERS_H_
#define PRODUCTISSUERS_H_

#include "DbAccess.h"
#include "myki_cd.h"
#include "ProductConfigurationUtils.h"

#ifdef __cplusplus
extern "C" {
#endif

int processProductIssuersXML();

int getProductIssuer(U8_t id, MYKI_CD_ProductIssuer_t &productIssuer);

#ifdef __cplusplus
}
#endif

#endif /* PRODUCTISSUERS_H_ */
