/*
 * DifferentialPrices.h
 *
 *  Created on: 08/10/2013
 *      Author: Morgan Dell
 */

#ifndef DIFFERENTIALPRICES_H_
#define DIFFERENTIALPRICES_H_

#include "DbAccess.h"
#include "myki_cd.h"

#ifdef __cplusplus
extern "C" {
#endif

int initDifferentialPricesLookup();
int processDifferentialPricesXML();
int getDifferentialPrice(const MYKI_CD_DifferentialPricingRequest_t* input, MYKI_CD_DifferentialPricing_t& differentialPricing);

#ifdef __cplusplus
}
#endif

#endif /* DIFFERENTIALPRICES_H_ */
