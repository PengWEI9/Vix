/*
 * Products.h
 *
 *  Created on: 04/10/2012
 *      Author: James Ho
 */

#ifndef PRODUCTS_H_
#define PRODUCTS_H_

#include "DbAccess.h"
#include "myki_cd.h"
#include "ProductConfigurationUtils.h"

#ifdef __cplusplus
#include <json/json.h>
#endif

int processProductsXML();
int initProductsLookup();

int getProduct(U8_t id, MYKI_CD_Product_t &product);
int getProductType(U8_t id, char* type, int bufferLength);
int getProductCardType(U8_t id, char* type, int bufferLength);
int getProductDefaultAttribute(U8_t id, MYKI_CD_Product_t &product);
int getProductAttribute(U8_t id, MYKI_CD_Product_t &product);

int
getProducts(
    Json::Value    &products,
    int             zone,
    const char     *productType,
    Time_t          time,
    int             serviceProviderId,
    const char     *deviceType );

#endif /* PRODUCTS_H_ */
