/*
   This file contains all funcations called from Test Harness to read/write data from/to BR
*/

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#include "cs.h"
#include "LDT_cmd.h"
#include "version.h"

extern "C" {
#include "LDT.h"
#include "LDTStructs.h"
}

#include "LDT_cmd.h"
#include "Common.h"

#define UNKNOWN_VALUE 9999
#define UNKNOWN_VALUE_STR "9999"

enum DATA_TYPE {
        U8_T,
        U16_T,
        U32_T,
        DATE_T,
        TIME_T
};

DATA_TYPE TAppLoadLogArguments[] = {
        U8_T,
        U8_T,
        U8_T,
        U16_T,
        U16_T,
        TIME_T,
        U16_T,
        U16_T,
        U8_T,
        U32_T,
        U32_T,
        U8_T,
        U8_T,
        U16_T,
        U8_T
};

DATA_TYPE TAppUsageLogArguments[] = {
        U8_T,
        U8_T,
        U8_T,
        U16_T,
        TIME_T,
        U16_T,
        U16_T,
        U8_T,
        U32_T,
        U32_T,
        U8_T,
        U8_T,
        U8_T,
        U8_T,
        U8_T,
        U16_T
};

DATA_TYPE CappingUpdateArguments[] = {
        U16_T,
        U16_T,
        DATE_T,
        U8_T,
        U8_T,
        U32_T,
        DATE_T,
        U8_T,
        U8_T,
        U32_T
};

DATA_TYPE TAppUpdateArguments[] = {
        U16_T,
        U16_T,
        U8_T,
        DATE_T,
        U8_T,
        DATE_T,
        U8_T,
        U8_T
};

DATA_TYPE TAppUpdateActivateArguments[] = {
        U16_T,
        U8_T,
        U16_T,
        DATE_T,
        U8_T,
        DATE_T,
        U8_T,
        U8_T
};

DATA_TYPE TAppUpdateBlockArguments[] = {
        U16_T,
        U8_T,
        U16_T,
        U16_T
};

DATA_TYPE TAppUpdateUnblockArguments[] = {
        U16_T,
        U16_T,
        U16_T
};

DATA_TYPE TAppUpdateSetProductInUseArguments[] = {
        U8_T
};

DATA_TYPE TPurseUpdateArguments[] = {
        U16_T,
        U16_T,
        U16_T,
        U8_T,
        U32_T,
        U32_T,
        U8_T
};

DATA_TYPE TPurseLoadArguments[] = {
        U32_T,
        U16_T,
        U16_T,
        U16_T,
        U8_T,
        U8_T
};

DATA_TYPE TPurseLoadAutoloadArguments[] = {
        U16_T,
        U16_T,
        U16_T,
        U8_T
};

DATA_TYPE TPurseLoadReverseArguments[] = {
        U16_T,
        U32_T,
        U16_T,
        U16_T,
        U8_T
};

DATA_TYPE TPurseUsageReverseArguments[] = {
        U32_T,
        U16_T,
        U16_T,
        U16_T,
        U8_T
};

DATA_TYPE TPurseUsageTransitArguments[] = {
        U32_T,
        U16_T,
        U16_T,
        U16_T,
        U8_T
};

DATA_TYPE ProductUpdateInvalidateArguments[] = {
        U16_T,
        U16_T,
        U16_T,
        U16_T
};

DATA_TYPE ProductUsageReverseArguments[] = {
        U16_T,
        U16_T,
        U16_T,
        U16_T,
        U8_T
};


DATA_TYPE ProductUpdateArguments[] = {
        U16_T,
        U16_T,
        U16_T,
        U16_T,
        U8_T,
        U8_T,
        U8_T,
        U8_T,
        U8_T,
        U8_T,
        U8_T,
        U8_T
};

DATA_TYPE ProductUpdateActivateArguments[] = {
        U16_T,
        U16_T,
        U16_T,
        U16_T,
        U8_T,
        U16_T,
        TIME_T,
        TIME_T
};

DATA_TYPE ProductUpdateExtendArguments[] = {
        U16_T,
        U16_T,
        U16_T,
        U8_T,
        U8_T,
        U32_T,
        TIME_T,
        TIME_T,
        U16_T,
        U8_T,
        U8_T
};


DATA_TYPE ProductUsageScanOnArguments[] = {
        U16_T,
        U16_T,
        U16_T,
        U16_T,
        U8_T,
        U8_T,
        TIME_T
};

DATA_TYPE ProductUsageScanOffArguments[] = {
        U16_T,
        U16_T,
        U16_T,
        U16_T,
        U8_T,
        U8_T,
        TIME_T,
        U16_T,
        U16_T,
        U16_T,
        U8_T
};

DATA_TYPE CardSaleArguments[] = {
        DATE_T,
        U16_T,
        U16_T,
        U32_T,
        U16_T
};

DATA_TYPE CardSaleReverseArguments[] = {
        U16_T,
        U16_T,
        U16_T
};

DATA_TYPE ProductSaleReverseArguments[] = {
        U16_T,
        U16_T
};

DATA_TYPE ProductSaleUpgradeArguments[] = {
        U16_T,  
        U8_T,
        U32_T,
        U8_T,
        U8_T,
        TIME_T,
        TIME_T,
        U8_T,
        U16_T,
        U16_T,
        U16_T,
        U8_T
};

DATA_TYPE ProductSaleArguments[] = {
        U8_T,  
        U8_T,
        U32_T,
        U8_T,
        U8_T,
        TIME_T,
        TIME_T,
        U16_T,
        U8_T,
        U16_T,
        U16_T,
        U8_T,
        U8_T,
        U8_T,
        U8_T,
        U16_T
};

void setOptionDateC19Value(void * option, DateC19_t & value, DateC19_t v )
{
    *((U32_t *)option) = 1;
    *((DateC19_t *)value) = v;
}


void setOptionDateTimeValue(void * option, Time_t & value, time_t v )
{
    *((U32_t *)option) = 1;
    value = v;
}

void setOptionIntValue(void * option, void * value, int v )
{
CsDebug(9,(9, "setOptionIntValue %d", v));
    if (v == UNKNOWN_VALUE)
        *((U32_t *)option) = 0;
    else
    {
        *((U32_t *)option) = 1;
        *((U32_t *)value) = v;
    }
}

bool isValid(const char *a, DATA_TYPE t)
{
    bool ret = true; 
    char * ep;
    U32_t iValue;
    int yyyy, mon, dd, hh, mm, ss;

    if (t == U8_T || t == U16_T || t == U32_T)
    {
        iValue = strtol(a, &ep, 10); 
        if (iValue ==0 && a == ep)
        {
            CsErrx("%s is not an integer\n", a);
            ret = false;
        }
    }
    else if (t == TIME_T)
    {
        if (sscanf(a, "%d:%d:%d:%d:%d:%d", &yyyy, &mon, &dd, &hh, &mm, &ss) != 6)
            ret = false;
    }
    else
    { 
        CsErrx("Wrong data format %d for input %s \n", t, a);
        ret = false;
    }

    return ret;
}
 
/********************** card sale is not part of demo and will be done later
bool ldt_cardSale(int argc, const char * argv[])
{
    CardSale_t request;

    int i;

    LDTTransactionData_t *ldtTransactionData = (LDTTransactionData_t *)data;
    for (i = 0; i < argc; i++) 
    {
        if ( !isValid(argv[i], CardSaleArguments[i - 1]) )
            return false;
    }

    i = 0;

    request.expiryDate = getDateC19( argv[i++] );
    request.serviceProviderId = atoi( argv[i++] );
    setOptionIntValue(&request.isEntryPointIdSet, &request.entryPointId, atoi (argv[i++]));
    setOptionIntValue(&request.isDepositSet, &request.deposit, atoi (argv[i++]));
    setOptionIntValue(&request.isCurrencyCodeSet, &request.currencyCode, atoi (argv[i++]));
}

bool ldt_cardSaleReverse(int argc, const char * argv[])
{
    CardSale_t request;

    int i;

    LDTTransactionData_t *ldtTransactionData = (LDTTransactionData_t *)data;
    for (i = 0; i < argc; i++) 
    {
        if ( !isValid(argv[i], CardSaleReverseArguments[i - 1]) )
            return false;
    }

    i = 0;

    request.serviceProviderId = atoi( argv[i++] );
    if (!strcmp(argv[i], UNKNOWN_VALUE_STR) == 0)
    {
        DateC19_t t = getDateC19(argv[i]);
        setOptionDateC19Value(&request.isExpiryDateSet, request.expiryDate, t);
    }
    i++;
    setOptionIntValue(&request.isEntryPointIdSet, &request.entryPointId, atoi (argv[i++]));
}

*************************************/

bool ldt_productSale(int argc, const char * argv[], void *data)
{
    ProductSale_t request;
    int ret;
    int i;

    CsDebug(9, (9, "ldt_productSale"));

    LDTTransactionData_t *ldtTransactionData = (LDTTransactionData_t *)data;
    for (i = 0; i < argc; i++) 
    {
        if ( !isValid(argv[i], ProductSaleArguments[i]) )
            return false;
    }

    i = 0;
    

    request.productId = atoi (argv[i++]);
    request.issuerId = atoi (argv[i++]);
    request.purchaseValue = atoi (argv[i++]);
    request.zoneLow = atoi (argv[i++]);
    request.zoneHigh = atoi (argv[i++]);
    request.startDateTime = getDateTime (argv[i++]);

    request.endDateTime = getDateTime (argv[i++]);
    request.serviceProviderId = atoi (argv[i++]);
    
    setOptionIntValue(&request.isActionSequenceNoSet, &request.actionSequenceNo, atoi (argv[i++]));
    setOptionIntValue(&request.isEntryPointIdSet, &request.entryPointId, atoi (argv[i++]));
    setOptionIntValue(&request.isRouteIdSet, &request.routeId, atoi (argv[i++]));
    setOptionIntValue(&request.isStopIdSet, &request.stopId, atoi (argv[i++]));
    setOptionIntValue(&request.isAutoloadSet, &request.isAutoload, atoi (argv[i++]));
    setOptionIntValue(&request.isOffpeakSet, &request.isOffpeak, atoi (argv[i++]));
    setOptionIntValue(&request.isProvisionalSet, &request.isProvisional, atoi (argv[i++]));
    setOptionIntValue(&request.isInstanceCountSet, &request.instanceCount, atoi (argv[i++]));


    ret = MYKI_LDT_ProductSale(&request, ldtTransactionData);
    if (ret == LDT_SUCCESS)
    {
        CsDebug(9, (9, "LDTcmd:ldt_productSale: Successfully called MYKI_LDT_ProductSale" ));
        return true;
    }
    else if (ret == LDT_PRECONDITION_NOT_MET)
    {
        CsErrx("LDTcmd:ldt_productSale: Failed to call MYKI_LDT_ProductSale due to precondition not met");
    }
    else
    {
        CsErrx("LDTcmd:ldt_productSale: Failed to call MYKI_LDT_ProductSaleReverse");
    }

    return false;
}

bool ldt_productSaleReverse(int argc, const char * argv[],  void *data)
{
    ProductSale_t request;
    int i;
    bool ret;

    CsDebug(9, (9, "ldt_productSaleReverse"));

    LDTTransactionData_t *ldtTransactionData = (LDTTransactionData_t *)data;
    for (i = 0; i < argc; i++) 
    {
        if ( !isValid(argv[i], ProductSaleReverseArguments[i - 1]) )
            return false;
    }

    i = 0;
    
    request.serialNo = atoi (argv[i++]);
    request.serviceProviderId = atoi (argv[i++]);

    ret = MYKI_LDT_ProductSaleReverse(&request, ldtTransactionData);
    if (ret == LDT_SUCCESS)
    {
        CsDebug(9, (9, "LDTcmd:ldt_productSaleReverse: Successfully called MYKI_LDT_ProductSaleReverse" ));
        return true;
    }
    else if (ret == LDT_PRECONDITION_NOT_MET)
    {
        CsErrx("LDTcmd:ldt_productSaleReverse: Failed to call MYKI_LDT_ProductSaleReverse due to precondition not met");
    }
    else
    {
        CsErrx("LDTcmd:ldt_productSaleReverse: Failed to call MYKI_LDT_ProductSaleReverse");
    }

    return false;
}

bool ldt_productSaleUpgrade(int argc, const char * argv[], void *data)
{
    ProductSale_t request;
    int i;

    LDTTransactionData_t *ldtTransactionData = (LDTTransactionData_t *)data;
    for (i = 0; i < argc; i++) 
    {
        if ( !isValid(argv[i], ProductSaleUpgradeArguments[i - 1]) )
            return false;
    }

    i = 0;
    
    request.serialNo = atoi (argv[i++]);
    request.productId = atoi (argv[i++]);
    request.purchaseValue = atoi (argv[i++]);
    request.zoneLow = atoi (argv[i++]);
    request.zoneHigh = atoi (argv[i++]);

    if (!strcmp(argv[i], UNKNOWN_VALUE_STR) == 0)
    {
        time_t t = getDateTime(argv[i]);
        setOptionDateTimeValue(&request.isStartDateTimeSet, request.startDateTime, t);
    }
    i++;
    if (!strcmp(argv[i], UNKNOWN_VALUE_STR) == 0)
    {
        time_t t = getDateTime(argv[i]);
        setOptionDateTimeValue(&request.isEndDateTimeSet, request.endDateTime, t);
    }
    i++;

    setOptionIntValue(&request.isProvisionalSet, &request.isProvisional, atoi (argv[i++]));

    request.serviceProviderId = atoi (argv[i++]);

    setOptionIntValue(&request.isEntryPointIdSet, &request.entryPointId, atoi (argv[i++]));
    setOptionIntValue(&request.isRouteIdSet, &request.routeId, atoi (argv[i++]));
    setOptionIntValue(&request.isStopIdSet, &request.stopId, atoi (argv[i++]));
    
    if (MYKI_LDT_ProductSaleUpgrade(&request, ldtTransactionData) > 0);
    {
        return true;
    }

    return false;
}

bool ldt_productUsageScanOn(int argc, const char * argv[], void * data)
{
    ProductUsage_t request;
    int i;

    LDTTransactionData_t *ldtTransactionData = (LDTTransactionData_t *)data;
    for (i = 0; i < argc; i++) 
    {
        if ( !isValid(argv[i], ProductUsageScanOnArguments[i - 1]) )
            return false;
    }

    i = 0;
    
    request.serialNo = atoi (argv[i++]);
    request.serviceProviderId = atoi (argv[i++]);
    request.entryPointId = atoi (argv[i++]);
    request.routeId = atoi (argv[i++]);
    request.stopId = atoi (argv[i++]);
    request.zone = atoi (argv[i++]);
    request.dateTime = getDateTime (argv[i++]);

    if (MYKI_LDT_ProductUsageScanOn(&request, ldtTransactionData) > 0);
    {
        return true;
    }

    return false;
}

bool ldt_productUsageReverse(int argc, const char * argv[], void * data)
{
    ProductUsage_t request;
    int i;

    LDTTransactionData_t *ldtTransactionData = (LDTTransactionData_t *)data;
    for (i = 0; i < argc; i++) 
    {
        if ( !isValid(argv[i], ProductUsageReverseArguments[i - 1]) )
            return false;
    }

    i = 0;
    
    request.serialNo = atoi (argv[i++]);
    request.serviceProviderId = atoi (argv[i++]);
    request.entryPointId = atoi (argv[i++]);
    request.routeId = atoi (argv[i++]);
    request.stopId = atoi (argv[i++]);

    if (MYKI_LDT_ProductUsageReverse(&request, ldtTransactionData) > 0);
    {
        return true;
    }

    return false;
}

bool ldt_productUsageScanOnEpass(int argc, const char * argv[], void * data)
{
    ProductUsage_t request;
    int i;

    LDTTransactionData_t *ldtTransactionData = (LDTTransactionData_t *)data;
    for (i = 0; i < argc; i++) 
    {
        if ( !isValid(argv[i], ProductUsageScanOnArguments[i - 1]) )
            return false;
    }

    i = 0;
    
    request.serialNo = atoi (argv[i++]);
    request.serviceProviderId = atoi (argv[i++]);
    request.entryPointId = atoi (argv[i++]);
    request.routeId = atoi (argv[i++]);
    request.stopId = atoi (argv[i++]);
    request.zone = atoi (argv[i++]);
    request.dateTime = getDateTime (argv[i++]);

    if (MYKI_LDT_ProductUsageScanOnEpass(&request, ldtTransactionData) > 0);
    {
        return true;
    }

    return false;
}

bool ldt_productUsageScanOnStoredValue(int argc, const char * argv[], void * data)
{
    ProductUsage_t request;
    int i;

    LDTTransactionData_t *ldtTransactionData = (LDTTransactionData_t *)data;
    for (i = 0; i < argc; i++) 
    {
        if ( !isValid(argv[i], ProductUsageScanOnArguments[i - 1]) )
            return false;
    }

    i = 0;
    
    request.serialNo = atoi (argv[i++]);
    request.serviceProviderId = atoi (argv[i++]);
    request.entryPointId = atoi (argv[i++]);
    request.routeId = atoi (argv[i++]);
    request.stopId = atoi (argv[i++]);
    request.zone = atoi (argv[i++]);
    request.dateTime = getDateTime (argv[i++]);

    setOptionIntValue(&request.isOriginatingServiceProviderIdSet, &request.originatingServiceProviderId, atoi (argv[i++]));
    setOptionIntValue(&request.isOriginatingEntryPointIdSet, &request.originatingEntryPointId, atoi (argv[i++]));
    setOptionIntValue(&request.isOriginatingRouteIdSet, &request.originatingRouteId, atoi (argv[i++]));
    setOptionIntValue(&request.isOriginatingStopIdSet, &request.originatingStopId, atoi (argv[i++]));

    if (MYKI_LDT_ProductUsageScanOnStoredValue(&request, ldtTransactionData) > 0);
    {
        return true;
    }

    return false;
}

bool ldt_productUsageScanOff(int argc, const char * argv[], void *data)
{
    ProductUsage_t request;
    int i;

    LDTTransactionData_t *ldtTransactionData = (LDTTransactionData_t *)data;
    for (i = 0; i < argc; i++) 
    {
        if ( !isValid(argv[i], ProductUsageScanOffArguments[i - 1]) )
            return false;
    }

    i = 0;
    
    request.serialNo = atoi (argv[i++]);
    request.serviceProviderId = atoi (argv[i++]);
    request.entryPointId = atoi (argv[i++]);
    request.routeId = atoi (argv[i++]);
    request.stopId = atoi (argv[i++]);
    request.zone = atoi (argv[i++]);
    request.dateTime = getDateTime (argv[i++]);

    setOptionIntValue(&request.isOriginatingServiceProviderIdSet, &request.originatingServiceProviderId, atoi (argv[i++]));
    setOptionIntValue(&request.isOriginatingEntryPointIdSet, &request.originatingEntryPointId, atoi (argv[i++]));
    setOptionIntValue(&request.isOriginatingRouteIdSet, &request.originatingRouteId, atoi (argv[i++]));
    setOptionIntValue(&request.isOriginatingStopIdSet, &request.originatingStopId, atoi (argv[i++]));

    if (MYKI_LDT_ProductUsageScanOff(&request, ldtTransactionData) > 0);
    {
        return true;
    }

    return false;
}

bool ldt_productUsageForceScanOff(int argc, const char * argv[], void * data)
{
    ProductUsage_t request;
    int i;

    LDTTransactionData_t *ldtTransactionData = (LDTTransactionData_t *)data;
    for (i = 0; i < argc; i++) 
    {
        if ( !isValid(argv[i], ProductUsageScanOffArguments[i - 1]) )
            return false;
    }

    i = 0;
    
    request.serialNo = atoi (argv[i++]);
    request.serviceProviderId = atoi (argv[i++]);
    request.entryPointId = atoi (argv[i++]);
    request.routeId = atoi (argv[i++]);
    request.stopId = atoi (argv[i++]);
    request.zone = atoi (argv[i++]);
    request.dateTime = getDateTime (argv[i++]);

    setOptionIntValue(&request.isOriginatingServiceProviderIdSet, &request.originatingServiceProviderId, atoi (argv[i++]));
    setOptionIntValue(&request.isOriginatingEntryPointIdSet, &request.originatingEntryPointId, atoi (argv[i++]));
    setOptionIntValue(&request.isOriginatingRouteIdSet, &request.originatingRouteId, atoi (argv[i++]));
    setOptionIntValue(&request.isOriginatingStopIdSet, &request.originatingStopId, atoi (argv[i++]));

    if (MYKI_LDT_ProductUsageForceScanOff(&request, ldtTransactionData) > 0);
    {
        return true;
    }

    return false;
}

bool ldt_productUpdate(int argc, const char * argv[], void * data)
{
    ProductUpdate_t request;
    int i;

    LDTTransactionData_t *ldtTransactionData = (LDTTransactionData_t *)data;
    for (i = 0; i < argc; i++) 
    {
        if ( !isValid(argv[i], ProductUpdateArguments[i - 1]) )
            return false;
    }

    i = 0;
    
    request.serialNo = atoi (argv[i++]);
    request.serviceProviderId = atoi (argv[i++]);

    setOptionIntValue(&request.isEntryPointIdSet, &request.entryPointId, atoi (argv[i++]));
    setOptionIntValue(&request.isActionSequenceNoSet, &request.actionSequenceNo, atoi (argv[i++]));
    setOptionIntValue(&request.isAutoloadSet, &request.isAutoload, atoi (argv[i++]));
    setOptionIntValue(&request.isOffpeakSet, &request.isOffpeak, atoi (argv[i++]));
    setOptionIntValue(&request.isPremiumSet, &request.isPremium, atoi (argv[i++]));
    setOptionIntValue(&request.isProvisionalSet, &request.isProvisional, atoi (argv[i++]));
    setOptionIntValue(&request.isTripDirectionStatusSet, &request.isTripDirectionStatus, atoi (argv[i++]));
    setOptionIntValue(&request.isTripDirectionValueSet, &request.isTripDirectionValue, atoi (argv[i++]));
    setOptionIntValue(&request.isBorderStatusSet, &request.isBorderStatus, atoi (argv[i++]));
    setOptionIntValue(&request.isBorderSideSet, &request.isBorderSide, atoi (argv[i++]));

    if (MYKI_LDT_ProductUpdate(&request, ldtTransactionData) > 0);
    {
        return true;
    }

    return false;
}

bool ldt_productUpdateActivate(int argc, const char * argv[], void * data)
{
    ProductUpdate_t request;
    int i;

    LDTTransactionData_t *ldtTransactionData = (LDTTransactionData_t *)data;
    for (i = 0; i < argc; i++) 
    {
        if ( !isValid(argv[i], ProductUpdateActivateArguments[i - 1]) )
            return false;
    }

    i = 0;
    
    request.serialNo = atoi (argv[i++]);
    request.serviceProviderId = atoi (argv[i++]);
    request.entryPointId = atoi (argv[i++]);
    request.routeId = atoi (argv[i++]);
    request.stopId = atoi (argv[i++]);

    setOptionIntValue(&request.isInstanceCountSet, &request.instanceCount, atoi (argv[i++]));
    if (!strcmp(argv[i], UNKNOWN_VALUE_STR) == 0)
    {
        time_t t = getDateTime(argv[i]);
        setOptionDateTimeValue(&request.isStartDateTimeSet, request.startDateTime, t);
    }
    i++;
    if (!strcmp(argv[i], UNKNOWN_VALUE_STR) == 0)
    {
        time_t t = getDateTime(argv[i]);
        setOptionDateTimeValue(&request.isEndDateTimeSet, request.endDateTime, t);
    }
    i++;

    if (MYKI_LDT_ProductUpdateActivate(&request, ldtTransactionData) > 0);
    {
        return true;
    }

    return false;
}

bool ldt_productUpdateInvalidate(int argc, const char * argv[], void * data)
{
    ProductUpdate_t request;
    int i;

    LDTTransactionData_t *ldtTransactionData = (LDTTransactionData_t *)data;
    for (i = 0; i < argc; i++) 
    {
        if ( !isValid(argv[i], ProductUpdateInvalidateArguments[i - 1]) )
            return false;
    }

    i = 0;
    
    request.serialNo = atoi (argv[i++]);
    request.serviceProviderId = atoi (argv[i++]);
    setOptionIntValue(&request.isEntryPointIdSet, &request.entryPointId, atoi (argv[i++]));
    setOptionIntValue(&request.isActionSequenceNoSet, &request.actionSequenceNo, atoi (argv[i++]));

    if (MYKI_LDT_ProductUpdateInvalidate(&request, ldtTransactionData) > 0);
    {
        return true;
    }

    return false;
}

bool ldt_productUpdateExtend(int argc, const char * argv[], void * data)
{
    ProductUpdate_t request;
    int i;

    LDTTransactionData_t *ldtTransactionData = (LDTTransactionData_t *)data;
    for (i = 0; i < argc; i++) 
    {
        if ( !isValid(argv[i], ProductUpdateExtendArguments[i - 1]) )
            return false;
    }

    i = 0;
    
    request.serialNo = atoi (argv[i++]);
    request.serviceProviderId = atoi (argv[i++]);
    setOptionIntValue(&request.isEntryPointIdSet, &request.entryPointId, atoi (argv[i++]));
    setOptionIntValue(&request.isZoneLowSet, &request.zoneLow, atoi (argv[i++]));
    setOptionIntValue(&request.isZoneHighSet, &request.zoneHigh, atoi (argv[i++]));
    setOptionIntValue(&request.isPurchaseValueSet, &request.purchaseValue, atoi (argv[i++]));
    if (!strcmp(argv[i], UNKNOWN_VALUE_STR) == 0)
    {
        time_t t = getDateTime(argv[i]);
        setOptionDateTimeValue(&request.isStartDateTimeSet, request.startDateTime, t);
    }
    i++;
    if (!strcmp(argv[i], UNKNOWN_VALUE_STR) == 0)
    {
        time_t t = getDateTime(argv[i]);
        setOptionDateTimeValue(&request.isEndDateTimeSet, request.endDateTime, t);
    }
    i++;
    setOptionIntValue(&request.isInstanceCountSet, &request.instanceCount, atoi (argv[i++]));
    setOptionIntValue(&request.isPremiumSet, &request.isPremium, atoi (argv[i++]));
    setOptionIntValue(&request.isClearBorderStatusSet, &request.clearBorderStatus, atoi (argv[i++]));


    if (MYKI_LDT_ProductUpdateExtend(&request, ldtTransactionData) > 0);
    {
        return true;
    }

    return false;
}

bool ldt_TPurseLoad(int argc, const char * argv[], void * data)
{
    TPurseLoad_t request;
    int i;
    CsDebug(9, (9, " inside ldt_TPurseLoad(%d,?,?)",argc ));
    //MYKI_BR_ContextData_t *pContextData = GetCardProcessingThreadContextData();

    LDTTransactionData_t *ldtTransactionData = (LDTTransactionData_t *)data;
    for (i = 0; i < argc; i++) 
    {
    CsDebug(9, (9, "checking argv[%d]",i));
        if ( !isValid(argv[i], TPurseLoadArguments[i ]) )
            return false;
    }
    CsDebug(9, (9, "arg types validated"));

    i = 0;
    
    request.value = atoi (argv[i++]);
    request.serviceProviderId = atoi (argv[i++]);
    setOptionIntValue(&request.isEntryPointIdSet, &request.entryPointId, atoi (argv[i++]));
    setOptionIntValue(&request.isRouteIdSet, &request.routeId, atoi (argv[i++]));
    setOptionIntValue(&request.isStopIdSet, &request.stopId, atoi (argv[i++]));
//xxxxx    setOptionIntValue(&request.isActionSequenceNoSet, &request.actionSequenceNo, atoi (argv[i++]));

    CsDebug(9, (9, "about to call MYKI_LDT_TPurseLoad"));
    if (MYKI_LDT_TPurseLoad(&request, ldtTransactionData) > 0);
    {
        return true;
    }

    return false;
}

bool ldt_TPurseLoadAutoload(int argc, const char * argv[], void * data)
{
    TPurseLoad_t request;
    int i;

    LDTTransactionData_t *ldtTransactionData = (LDTTransactionData_t *)data;
    for (i = 0; i < argc; i++) 
    {
       if ( !isValid(argv[i], TPurseLoadAutoloadArguments[i - 1]) )
            return false;
    }

    i = 0;
    
    request.serviceProviderId = atoi (argv[i++]);
    setOptionIntValue(&request.isEntryPointIdSet, &request.entryPointId, atoi (argv[i++]));
    setOptionIntValue(&request.isRouteIdSet, &request.routeId, atoi (argv[i++]));
    setOptionIntValue(&request.isStopIdSet, &request.stopId, atoi (argv[i++]));

    if (MYKI_LDT_TPurseLoadAutoload(&request, ldtTransactionData) > 0);
    {
        return true;
    }

    return false;
}

bool ldt_TPurseLoadReverse(int argc, const char * argv[], void * data)
{
    TPurseLoad_t request;
    int i;

    LDTTransactionData_t *ldtTransactionData = (LDTTransactionData_t *)data;
    for (i = 0; i < argc; i++) 
    {
        if ( !isValid(argv[i], TPurseLoadReverseArguments[i - 1]) )
            return false;
    }

    i = 0;
    
    request.serviceProviderId = atoi (argv[i++]);
    request.value = atoi (argv[i++]);
    setOptionIntValue(&request.isEntryPointIdSet, &request.entryPointId, atoi (argv[i++]));
    setOptionIntValue(&request.isRouteIdSet, &request.routeId, atoi (argv[i++]));
    setOptionIntValue(&request.isStopIdSet, &request.stopId, atoi (argv[i++]));

    if (MYKI_LDT_TPurseLoadReverse(&request, ldtTransactionData) > 0);
    {
        return true;
    }

    return false;
}

bool ldt_TPurseUsageTransit(int argc, const char * argv[], void * data)
{
    TPurseUsage_t request;
    int i;

    LDTTransactionData_t *ldtTransactionData = (LDTTransactionData_t *)data;
    for (i = 0; i < argc; i++) 
    {
        if ( !isValid(argv[i], TPurseUsageTransitArguments[i - 1]) )
            return false;
    }

    i = 0;
    
    request.value = atoi (argv[i++]);
    request.serviceProviderId = atoi (argv[i++]);
    setOptionIntValue(&request.isEntryPointIdSet, &request.entryPointId, atoi (argv[i++]));
    setOptionIntValue(&request.isRouteIdSet, &request.routeId, atoi (argv[i++]));
    setOptionIntValue(&request.isStopIdSet, &request.stopId, atoi (argv[i++]));

    if (MYKI_LDT_TPurseUsageTransit(&request, ldtTransactionData) > 0);
    {
        return true;
    }

    return false;
}

bool ldt_TPurseUsageReverse(int argc, const char * argv[], void * data)
{
    TPurseUsage_t request;
    int i;

    LDTTransactionData_t *ldtTransactionData = (LDTTransactionData_t *)data;
    for (i = 0; i < argc; i++) 
    {
        if ( !isValid(argv[i], TPurseUsageReverseArguments[i - 1]) )
            return false;
    }

    i = 0;
    
    request.value = atoi (argv[i++]);
    request.serviceProviderId = atoi (argv[i++]);
    setOptionIntValue(&request.isEntryPointIdSet, &request.entryPointId, atoi (argv[i++]));
    setOptionIntValue(&request.isRouteIdSet, &request.routeId, atoi (argv[i++]));
    setOptionIntValue(&request.isStopIdSet, &request.stopId, atoi (argv[i++]));

    if (MYKI_LDT_TPurseUsageReverse(&request, ldtTransactionData) > 0);
    {
        return true;
    }

    return false;
}

bool ldt_TPurseUpdate(int argc, const char * argv[], void * data)
{
    TPurseUpdate_t request;
    int i;

    LDTTransactionData_t *ldtTransactionData = (LDTTransactionData_t *)data;
    for (i = 0; i < argc; i++) 
    {
        if ( !isValid(argv[i], TPurseUpdateArguments[i - 1]) )
            return false;
    }

    i = 0;
    
    request.serviceProviderId = atoi (argv[i++]);
    setOptionIntValue(&request.isEntryPointIdSet, &request.entryPointId, atoi (argv[i++]));
    setOptionIntValue(&request.isActionSequenceNoSet, &request.actionSequenceNo, atoi (argv[i++]));
    setOptionIntValue(&request.isAutoloadSet, &request.isAutoload, atoi (argv[i++]));
    setOptionIntValue(&request.isAutoThresholdSet, &request.autoThreshold, atoi (argv[i++]));
    setOptionIntValue(&request.isAutoValueSet, &request.autoValue, atoi (argv[i++]));
    setOptionIntValue(&request.isTransitOnlySet, &request.isTransitOnly, atoi (argv[i++]));

    if (MYKI_LDT_TPurseUpdate(&request, ldtTransactionData) > 0);
    {
        return true;
    }

    return false;
}

bool ldt_TAppUpdate(int argc, const char * argv[], void * data)
{
    TAppUpdate_t request;
    time_t t;
    int i;

    LDTTransactionData_t *ldtTransactionData = (LDTTransactionData_t *)data;
    for (i = 0; i < argc; i++) 
    {
        if ( !isValid(argv[i], TAppUpdateArguments[i - 1]) )
            return false;
    }

    i = 0;
    
    request.serviceProviderId = atoi (argv[i++]);
    setOptionIntValue(&request.isEntryPointIdSet, &request.entryPointId, atoi (argv[i++]));
    setOptionIntValue(&request.isActionSequenceNoSet, &request.actionSequenceNo, atoi (argv[i++]));
    t = getDateTime(argv[i++]);
    setOptionDateC19Value(&request.isExpiryDateSet, request.expiryDate, t);
    setOptionIntValue(&request.isPassengerCodeSet, &request.passengerCode, atoi (argv[i++]));
    t = getDateTime(argv[i++]);
    setOptionDateC19Value(&request.isPassengerCodeExpirySet, request.passengerCodeExpiry, t);
    setOptionIntValue(&request.isPassengerControlSet, &request.passengerControl,  atoi (argv[i++]));
    setOptionIntValue(&request.isLanguageSet, &request.language, atoi (argv[i++]));

    if (MYKI_LDT_TAppUpdate(&request, ldtTransactionData) > 0);
    {
        return true;
    }

    return false;
}

bool ldt_TAppUpdateActivate(int argc, const char * argv[], void * data)
{
    TAppUpdate_t request;
    time_t t;
    int i;

    LDTTransactionData_t *ldtTransactionData = (LDTTransactionData_t *)data;
    for (i = 0; i < argc; i++) 
    {
        if ( !isValid(argv[i], TAppUpdateActivateArguments[i - 1]) )
            return false;
    }

    i = 0;
    
    request.serviceProviderId = atoi (argv[i++]);
    setOptionIntValue(&request.isActionSequenceNoSet, &request.actionSequenceNo, atoi (argv[i++]));
    setOptionIntValue(&request.isEntryPointIdSet, &request.entryPointId, atoi (argv[i++]));
    t = getDateTime(argv[i++]);
    setOptionDateC19Value(&request.isExpiryDateSet, request.expiryDate, t);
    setOptionIntValue(&request.isPassengerCodeSet, &request.passengerCode, atoi (argv[i++]));
    setOptionDateC19Value(&request.isPassengerCodeExpirySet, request.passengerCodeExpiry, t);
    setOptionIntValue(&request.isPassengerControlSet, &request.passengerControl, atoi (argv[i++]));
    setOptionIntValue(&request.isLanguageSet, &request.language, atoi (argv[i++]));

    if (MYKI_LDT_TAppUpdateActivate(&request, ldtTransactionData) > 0);
    {
        return true;
    }

    return false;
}

bool ldt_TAppUpdateBlock(int argc, const char * argv[], void * data)
{
    TAppUpdate_t request;
    int i;

    LDTTransactionData_t *ldtTransactionData = (LDTTransactionData_t *)data;
    for (i = 0; i < argc; i++) 
    {
        if ( !isValid(argv[i], TAppUpdateBlockArguments[i - 1]) )
            return false;
    }

    i = 0;
    
    request.serviceProviderId = atoi (argv[i++]);
    request.blockingReason = atoi (argv[i++]);
    setOptionIntValue(&request.isEntryPointIdSet, &request.entryPointId, atoi (argv[i++]));
    setOptionIntValue(&request.isActionSequenceNoSet, &request.actionSequenceNo, atoi (argv[i++]));

    if (MYKI_LDT_TAppUpdateBlock(&request, ldtTransactionData) > 0);
    {
        return true;
    }
}

bool ldt_TAppUpdateUnblock(int argc, const char * argv[], void * data)
{
    TAppUpdate_t request;
    int i;

    LDTTransactionData_t *ldtTransactionData = (LDTTransactionData_t *)data;
    for (i = 0; i < argc; i++) 
    {
        if ( !isValid(argv[i], TAppUpdateUnblockArguments[i - 1]) )
            return false;
    }

    i = 0;
    
    request.serviceProviderId = atoi (argv[i++]);
    request.blockingReason = atoi (argv[i++]);
    setOptionIntValue(&request.isEntryPointIdSet, &request.entryPointId, atoi (argv[i++]));

    if (MYKI_LDT_TAppUpdateUnblock(&request, ldtTransactionData) > 0);
    {
        return true;
    }
}

bool ldt_TAppUpdateSetProductInUse(int argc, const char * argv[], void * data)
{
    TAppUpdate_t request;
    int i;

    LDTTransactionData_t *ldtTransactionData = (LDTTransactionData_t *)data;
    for (i = 0; i < argc; i++) 
    {
        if ( !isValid(argv[i], TAppUpdateSetProductInUseArguments[i - 1]) )
            return false;
    }

    i = 0;
    
    setOptionIntValue(&request.isSerialNoSet, &request.serialNo, atoi (argv[i++]));

    if (MYKI_LDT_TAppUpdateSetProductInUse(&request, ldtTransactionData) > 0);
    {
        return true;
    }
}

bool ldt_CappingUpdate(int argc, const char * argv[], void * data)
{
    CappingUpdate_t request;
    time_t t;
    int i;

    LDTTransactionData_t *ldtTransactionData = (LDTTransactionData_t *)data;
    for (i = 0; i < argc; i++) 
    {
        if ( !isValid(argv[i], CappingUpdateArguments[i - 1]) )
            return false;
    }

    i = 0;
    
    request.serviceProviderId = atoi (argv[i++]);
    setOptionIntValue(&request.isEntryPointIdSet, &request.entryPointId, atoi (argv[i++]));
    t = getDateTime(argv[i++]);
    setOptionDateC19Value(&request.isDailyExpirySet, request.dailyExpiry, t);
    setOptionIntValue(&request.isDailyZoneLowSet, &request.dailyZoneLow, atoi (argv[i++]));
    setOptionIntValue(&request.isDailyZoneHighSet, &request.dailyZoneHigh, atoi (argv[i++]));
    setOptionIntValue(&request.isDailyValueSet, &request.dailyValue, atoi (argv[i++]));
    setOptionIntValue(&request.isWeeklyExpirySet, &request.weeklyExpiry, atoi (argv[i++]));
    setOptionIntValue(&request.isWeeklyZoneHighSet, &request.weeklyZoneHigh, atoi (argv[i++]));
    setOptionIntValue(&request.isWeeklyValueSet, &request.weeklyValue, atoi (argv[i++]));

    if (MYKI_LDT_CappingUpdate(&request, ldtTransactionData) > 0);
    {
        return true;
    }
}

bool ldt_TAppLoadLog(int argc, const char * argv[], void * data)
{
    TAppLoadLog_t request;
    int i;

    //LDTTransactionData_t *ldtTransactionData = (LDTTransactionData_t *)data;
    for (i = 0; i < argc; i++) 
    {
        if ( !isValid(argv[i], TAppLoadLogArguments[i - 1]) )
            return false;
    }

    i = 0;
    
    request.version = atoi (argv[i++]);
    request.controlBitmap = atoi (argv[i++]);
    request.transactionType = atoi (argv[i++]);
    request.transactionSequenceNumber = atoi (argv[i++]);
    request.providerId = atoi (argv[i++]);
    request.transactionDateTime = getDateTime (argv[i++]);
    request.providerId = atoi (argv[i++]);
    request.entryPointId = atoi (argv[i++]);
    request.routeId = atoi (argv[i++]);
    request.stopId = atoi (argv[i++]);
    setOptionIntValue(&request.isTransactionValueSet, &request.transactionValue, atoi (argv[i++]));
    setOptionIntValue(&request.isNewTPurseBalanceSet, &request.newTPurseBalance, atoi (argv[i++]));
    setOptionIntValue(&request.isPaymentMethodSet, &request.paymentMethod, atoi (argv[i++]));
    setOptionIntValue(&request.isProductIssuerIdSet, &request.productIssuerId, atoi (argv[i++]));
    setOptionIntValue(&request.isProductSerialNoSet, &request.productSerialNo, atoi (argv[i++]));
    setOptionIntValue(&request.isProductIdSet, &request.productId, atoi (argv[i++]));

    if (MYKI_LDT_TAppLoadLog(&request) > 0);
    {
        return true;
    }
}

bool ldt_TAppUsageLog(int argc, const char * argv[], void * data)
{
    TAppUsageLog_t request;
    int i;

    //LDTTransactionData_t *ldtTransactionData = (LDTTransactionData_t *)data;
    for (i = 0; i < argc; i++) 
    {
        if ( !isValid(argv[i], TAppUsageLogArguments[i - 1]) )
            return false;
    }

    i = 0;
    
    request.version = atoi (argv[i++]);
    request.controlBitmap = atoi (argv[i++]);
    request.transactionType = atoi (argv[i++]);
    request.providerId = atoi (argv[i++]);
    request.transactionDateTime = getDateTime (argv[i++]);
    request.entryPointId = atoi (argv[i++]);
    request.routeId = atoi (argv[i++]);
    request.stopId = atoi (argv[i++]);
    setOptionIntValue(&request.isTransactionValueSet, &request.transactionValue, atoi (argv[i++]));
    setOptionIntValue(&request.isNewTPurseBalanceSet, &request.newTPurseBalance, atoi (argv[i++]));
    setOptionIntValue(&request.isPaymentMethodSet, &request.paymentMethod, atoi (argv[i++]));
    setOptionIntValue(&request.isZoneSet, &request.zone, atoi (argv[i++]));
    setOptionIntValue(&request.isProductValidationStatusSet, &request.productValidationStatus, atoi (argv[i++]));
    setOptionIntValue(&request.isProductIssuerIdSet, &request.productIssuerId, atoi (argv[i++]));
    setOptionIntValue(&request.isProductIdSet, &request.productId, atoi (argv[i++]));
    setOptionIntValue(&request.isProductSerialNoSet, &request.productSerialNo, atoi (argv[i++]));

    if (MYKI_LDT_TAppUsageLog(&request) > 0);
    {
        return true;
    }
}



