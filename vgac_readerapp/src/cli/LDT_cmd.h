#ifndef _LDT_CMD_H
#define _LDT_CMD_H 

#include "LDTStructs.h"

/*
 * Product Sale 
 *
 * Returns:
 *    True on success and False fro error return
bool ldt_productSale(const char * productId, const char * issuerId, const char * value, const char * zoneLow, const char * zoneHigh, const char * startDateTime, const char * endDateTime, const char * serviceProiderId, const char * actionSeqNo, const char * entryPointId, const char * routeId, const char * stopId, const char * isAutoload, const char * isOffpeak, const char * isProvional, const char * instanceCount);
*/


//bool ldt_productSale(int argc, const char * argv[], void *ldtTransactionData);
//bool ldt_productSaleReverse(int argc, const char * argv[], void *ldtTransactionData);
bool ldt_productSale(int argc, const char * argv[], void *ldtTransactionData);
bool ldt_productSaleReverse(int argc, const char * argv[], void *ldtTransactionData);

bool ldt_productSaleUpgrade(int argc, const char * argv[], void *ldtTransactionData);
bool ldt_productUsageScanOn(int argc, const char * argv[], void *ldtTransactionData);
bool ldt_productUsageScanOnEpass(int argc, const char * argv[], void *ldtTransactionData);
bool ldt_productUsageScanOnEpass(int argc, const char * argv[], void *ldtTransactionData);
bool ldt_productUsageScanOnStoredValue(int argc, const char * argv[], void *ldtTransactionData);
bool ldt_productUsageScanOff(int argc, const char * argv[], void *ldtTransactionData);
bool ldt_productUsageForceScanOff(int argc, const char * argv[], void *ldtTransactionData);
bool ldt_productUpdate(int argc, const char * argv[], void *ldtTransactionData);
bool ldt_productUpdateActivate(int argc, const char * argv[], void *ldtTransactionData);
bool ldt_productUpdateInvalidate(int argc, const char * argv[], void *ldtTransactionData);
bool ldt_productUpdateExtend(int argc, const char * argv[], void *ldtTransactionData);

bool ldt_TPurseLoad(int argc, const char * argv[], void *ldtTransactionData);
bool ldt_TPurseLoadAutoload(int argc, const char * argv[], void *ldtTransactionData);
bool ldt_TPurseLoadReverse(int argc, const char * argv[], void *ldtTransactionData);
bool ldt_TPurseUsageTransit(int argc, const char * argv[], void *ldtTransactionData);
bool ldt_TPurseUsageReverse(int argc, const char * argv[], void *ldtTransactionData);
bool ldt_TPurseUpdate(int argc, const char * argv[], void *ldtTransactionData);

bool ldt_TAppUpdate(int argc, const char * argv[], void *ldtTransactionData);
bool ldt_TAppUpdateActivate(int argc, const char * argv[], void *ldtTransactionData);
bool ldt_TAppUpdateBlock(int argc, const char * argv[], void *ldtTransactionData);
bool ldt_TAppUpdateUnblock(int argc, const char * argv[], void *ldtTransactionData);
bool ldt_TAppUpdateSetProductInUse(int argc, const char * argv[], void *ldtTransactionData);

bool ldt_CappingUpdate(int argc, const char * argv[], void *ldtTransactionData);

bool ldt_TAppLoadLog(int argc, const char * argv[], void *ldtTransactionData);
bool ldt_TAppUsageLog(int argc, const char * argv[], void *ldtTransactionData);


#endif /* _LDT_CMD_H */
