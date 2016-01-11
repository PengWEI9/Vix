#include "myki_cd.h"
#include "myki_actionlist.h"
#include "DbAccess.h"
#include "ProductConfigurationUtils.h"
#include <vector>
#include <sys/timeb.h>
#include <stdlib.h>

#define BUFFER_SIZE 256

int main(int argc, char** argv)
{
	//int flag = 1;
	//CsMsgOptset(CSMSGOP_DEBUGENABLED, &flag, sizeof(flag));
	//flag = 9;
	//CsMsgOptset(CSMSGOP_DEBUG, &flag, sizeof(flag));

	if ( MYKI_CD_openCDDatabase(productCfgDBFile.c_str()) == 0 )
	{
		std::cerr << "Could not open database '" << productCfgDBFile << "'" << std::endl;
		return 2;
	}
	if ( MYKI_CD_openFullActionlistDatabase(defaultActionlistDBFile) == 0 )
	{
		std::cerr << "Could not open actionlist database '" << defaultActionlistDBFile << "'" << std::endl;
		return 3;
	}
	if ( !MYKI_CD_validateTariff() )
	{
		std::cerr << "Could not validate database '" << productCfgDBFile << "'" << std::endl;
		return 4;
	}

	int result = MYKI_CD_setDeviceParameters("FPDg", MYKI_CD_MODE_RAIL, 0, 0, 0, 0, 0, 0);
	std::cout << "MYKI_CD_setDeviceParameters returned " << result << std::endl;

	U8_t u8;
	U16_t u16;
	S32_t s32;
    U32_t u32;
    char buffer[BUFFER_SIZE+1];
    U16_t bufferLength = (U16_t) BUFFER_SIZE;
	struct timeb time1, time2, time3;
    
#define RunFunc(FUNC, NAME, VALUE) \
	ftime(&time1); \
	result = (int)FUNC; \
	ftime(&time2); \
	result = (int)FUNC; \
	ftime(&time3); \
	std::cout \
		<< NAME \
		<< " call time 1st:" << (time2.time - time1.time)*1000 + time2.millitm - time1.millitm \
		<< " ms, 2nd:" << (time3.time - time2.time)*1000 + time3.millitm - time2.millitm \
		<< " ms, value " << VALUE << " return code " << result << std::endl;
	/* CD functions */
	
	RunFunc(MYKI_CD_getStartDateTime(), "MYKI_CD_getStartDateTime", result);
	RunFunc(MYKI_CD_getStartDate(), "MYKI_CD_getStartDate", result);
	RunFunc(MYKI_CD_getProductAutoEnabled(&u8), "MYKI_CD_getProductAutoEnabled", (int)u8);
	RunFunc(MYKI_CD_getTPurseAutoEnabled(&u8), "MYKI_CD_getTPurseAutoEnabled", (int)u8);
	RunFunc(MYKI_CD_getMinimumBalanceePass(&s32), "MYKI_CD_getMinimumBalanceePass", s32);
	RunFunc(MYKI_CD_getMinimumBalanceStoredValue(&s32), "MYKI_CD_getMinimumBalanceStoredValue", s32);
	RunFunc(MYKI_CD_getProvisionalFareMode(&u16), "MYKI_CD_getProvisionalFareMode", u16);
	RunFunc(MYKI_CD_getEndOfTransportDay(&u16), "MYKI_CD_getEndOfTransportDay", u16);
	RunFunc(MYKI_CD_getBlockingPeriodDSC(&s32), "MYKI_CD_getBlockingPeriodDSC", s32);
	RunFunc(MYKI_CD_getBlockingPeriodLLSC(&s32), "MYKI_CD_getBlockingPeriodLLSC", s32);
	RunFunc(MYKI_CD_getChangeOfMindDSC(&s32), "MYKI_CD_getChangeOfMindDSC", s32);
	RunFunc(MYKI_CD_getChangeOfMindLLSC(&s32), "MYKI_CD_getChangeOfMindLLSC", s32);
	RunFunc(MYKI_CD_getStationExitFee(&s32), "MYKI_CD_getStationExitFee", s32);
	RunFunc(MYKI_CD_getePassOutOfZone(&u8), "MYKI_CD_getePassOutOfZone", (int)u8);
	RunFunc(MYKI_CD_getnHourPeriodMinutes(&u16), "MYKI_CD_getnHourPeriodMinutes", u16);
	RunFunc(MYKI_CD_getnHourEveningCutoff(&u16), "MYKI_CD_getnHourEveningCutoff", u16);
	RunFunc(MYKI_CD_getnHourExtendPeriodMinutes(&u16), "MYKI_CD_getnHourExtendPeriodMinutes", u16);
	RunFunc(MYKI_CD_getnHourExtendThreshold(&u16), "MYKI_CD_getnHourExtendThreshold", u16);
	RunFunc(MYKI_CD_getnHourMaximumDuration(&u16), "MYKI_CD_getnHourMaximumDuration", u16);
	RunFunc(MYKI_CD_getnHourRoundingPortion(&u16), "MYKI_CD_getnHourRoundingPortion", u16);
	RunFunc(MYKI_CD_getHeadlessModeRoute((U16_t)13, &u16), "MYKI_CD_getHeadlessModeRoute SP:13", u16);
    RunFunc(MYKI_CD_getHeadlessModeRoute((U16_t)18, &u16), "MYKI_CD_getHeadlessModeRoute SP:18", u16);
    RunFunc(MYKI_CD_getHeadlessModeRoute((U16_t)1001, &u16), "MYKI_CD_getHeadlessModeRoute SP:1001", u16);
    RunFunc(MYKI_CD_getHeadlessModeRoute((U16_t)9999, &u16), "MYKI_CD_getHeadlessModeRoute SP:9999", u16);
    RunFunc(MYKI_CD_getMaximumTripTolerance(18,   MYKI_CD_MODE_BUS,&u8), "MYKI_CD_getMaximumTripTolerance",    (int)u8);
    RunFunc(MYKI_CD_getMaximumTripTolerance(9999,   MYKI_CD_MODE_BUS,&u8), "MYKI_CD_getMaximumTripTolerance",  (int)u8);
    RunFunc(MYKI_CD_getMaximumTripTolerance(9999,   MYKI_CD_MODE_RAIL,&u8), "MYKI_CD_getMaximumTripTolerance", (int)u8);
	RunFunc(MYKI_CD_getePassLowWarningThreshold(&u16), "MYKI_CD_getePassLowWarningThreshold", u16);
	RunFunc(MYKI_CD_getTPurseLowWarningThreshold(&u16), "MYKI_CD_getTPurseLowWarningThreshold", u16);
	RunFunc(MYKI_CD_getAddValueEnabled(&u8), "MYKI_CD_getAddValueEnabled", (int)u8);
	RunFunc(MYKI_CD_getePassMaximumDay(&u16), "MYKI_CD_getePassMaximumDay", u16);
	RunFunc(MYKI_CD_getePassMinimumDay(&u16), "MYKI_CD_getePassMinimumDay", u16);
//DEPRECATED RunFunc(MYKI_CD_getePassSelectableDays(&deviceParameter), "MYKI_CD_getePassSelectableDays", deviceParameter.value);
	RunFunc(MYKI_CD_getePassTemporaryDays(&u16), "MYKI_CD_getePassTemporaryDays", u16);
	RunFunc(MYKI_CD_getGSTPercentage(&u8), "MYKI_CD_getGSTPercentage", (int)u8);
	RunFunc(MYKI_CD_getLLSCCancellationFee(&u32), "MYKI_CD_getLLSCCancellationFee", u32);
	RunFunc(MYKI_CD_getLLSCDeposit(&u32), "MYKI_CD_getLLSCDeposit", u32);
	RunFunc(MYKI_CD_getLLSCPersonalisationFee(&u32), "MYKI_CD_getLLSCPersonalisationFee", u32);
	RunFunc(MYKI_CD_getLLSCRegistrationFee(&u32), "MYKI_CD_getLLSCRegistrationFee", u32);
	RunFunc(MYKI_CD_getLLSCReplacementFee(&u32), "MYKI_CD_getLLSCReplacementFee", u32);
	RunFunc(MYKI_CD_getLLSCExpiryWindow(&u16), "MYKI_CD_getLLSCExpiryWindow", u16);
	RunFunc(MYKI_CD_getLLSCSaleFee(&u32), "MYKI_CD_getLLSCSaleFee", u32);
	RunFunc(MYKI_CD_getMaximumAddValue(&u32), "MYKI_CD_getMaximumAddValue", u32);
	RunFunc(MYKI_CD_getMaximumChange(&u32), "MYKI_CD_getMaximumChange", u32);
	RunFunc(MYKI_CD_getMaximumEFTPOSAmount(&u32), "MYKI_CD_getMaximumEFTPOSAmount", u32);
	RunFunc(MYKI_CD_getMaximumTPurseBalance(&s32), "MYKI_CD_getMaximumTPurseBalance", s32);
	RunFunc(MYKI_CD_getMinimumAddValue(&u32), "MYKI_CD_getMinimumAddValue", u32);
	RunFunc(MYKI_CD_getMinimumTPurseAutoloadAmount(&u32), "MYKI_CD_getMinimumTPurseAutoloadAmount", u32);
	RunFunc(MYKI_CD_getMinimumTPurseAutoloadThreshold(&u32), "MYKI_CD_getMinimumTPurseAutoloadThreshold", u32);
	RunFunc(MYKI_CD_getMinimumReceiptAmount(&u32), "MYKI_CD_getMinimumReceiptAmount", u32);
	RunFunc(MYKI_CD_getMinimumRefundBalance(&u32), "MYKI_CD_getMinimumRefundBalance", u32);
    buffer[0] = '\0';
    bufferLength = BUFFER_SIZE;
	RunFunc(MYKI_CD_getPaperTicketMessage(buffer, bufferLength), "MYKI_CD_getPaperTicketMessage", buffer);
    buffer[0] = '\0';
    bufferLength = BUFFER_SIZE;
	RunFunc(MYKI_CD_getPaperTicketName(buffer, bufferLength), "MYKI_CD_getPaperTicketName", buffer);
	RunFunc(MYKI_CD_getRefundCancelEnabled(&u8), "MYKI_CD_getRefundCancelEnabled", (int)u8);
    buffer[0] = '\0';
    bufferLength = BUFFER_SIZE;
	RunFunc(MYKI_CD_getRefundMessage(buffer, bufferLength), "MYKI_CD_getRefundMessage", buffer);
    buffer[0] = '\0';
    bufferLength = 255;//BUFFER_SIZE;
	RunFunc(MYKI_CD_getReversalMessage(buffer, bufferLength), "MYKI_CD_getReversalMessage", buffer);
    buffer[0] = '\0';
    bufferLength = BUFFER_SIZE;
	RunFunc(MYKI_CD_getSalesMessage(buffer, bufferLength), "MYKI_CD_getSalesMessage", buffer);
	RunFunc(MYKI_CD_getRecieptInvoiceMessageThreshold(&u32), "MYKI_CD_getRecieptInvoiceMessageThreshold", u32);
	RunFunc(MYKI_CD_getLLSCConcessionEnabled(&u8), "MYKI_CD_getLLSCConcessionEnabled", (int)u8);
	RunFunc(MYKI_CD_getDSCConcessionEnabled(&u8), "MYKI_CD_getDSCConcessionEnabled", (int)u8);
	RunFunc(MYKI_CD_getUnblockEnabled(&u8), "MYKI_CD_getUnblockEnabled", (int)u8);
	RunFunc(MYKI_CD_getPaymentMethodEnabled("BDC", (U16_t)18, "Cash", &u8), "MYKI_CD_getPaymentMethodEnabled Cash", (int)u8);
    RunFunc(MYKI_CD_getPaymentMethodEnabled("BDC", (U16_t)18, "TPurse", &u8), "MYKI_CD_getPaymentMethodEnabled TPurse", (int)u8);
	RunFunc(MYKI_CD_getReversalPeriod(&u16), "MYKI_CD_getReversalPeriod", u16);
//DEPRECATED	RunFunc(MYKI_CD_getEarlyBirdDayOfWeek(&deviceParameter), "MYKI_CD_getEarlyBirdDayOfWeek", deviceParameter.value);
//DEPRECATED	RunFunc(MYKI_CD_getEarlyBirdStations(&deviceParameter), "MYKI_CD_getEarlyBirdStations", deviceParameter.value);
//DEPRECATED	RunFunc(MYKI_CD_getEarlyBirdProduct(&deviceParameter), "MYKI_CD_getEarlyBirdProduct", deviceParameter.value);

    buffer[0] = '\0';
    bufferLength = BUFFER_SIZE;
	RunFunc(MYKI_CD_getCompanyABN(buffer, bufferLength), "MYKI_CD_getCompanyABN", buffer);
    buffer[0] = '\0';
    bufferLength = BUFFER_SIZE;
	RunFunc(MYKI_CD_getCompanyAddress(buffer, bufferLength), "MYKI_CD_getCompanyAddress", buffer);
    buffer[0] = '\0';
    bufferLength = BUFFER_SIZE;
	RunFunc(MYKI_CD_getCompanyCallCentreName(buffer, bufferLength), "MYKI_CD_getCompanyCallCentreName", buffer);
    buffer[0] = '\0';
    bufferLength = 255;//BUFFER_SIZE;
	RunFunc(MYKI_CD_getCompanyName(buffer, bufferLength), "MYKI_CD_getCompanyName", buffer);
    buffer[0] = '\0';
    bufferLength = 255;//BUFFER_SIZE;
	RunFunc(MYKI_CD_getCompanyNameShort(buffer, bufferLength), "MYKI_CD_getCompanyNameShort", buffer);
    buffer[0] = '\0';
    bufferLength = BUFFER_SIZE;
	RunFunc(MYKI_CD_getCompanyPhone(buffer, bufferLength), "MYKI_CD_getCompanyPhone", buffer);
    buffer[0] = '\0';
    bufferLength = BUFFER_SIZE;
	RunFunc(MYKI_CD_getCompanyWebSite(buffer, bufferLength), "MYKI_CD_getCompanyWebSite", buffer);

	MYKI_CD_DifferentialPricingRequest_t diffInput;
	MYKI_CD_DifferentialPricing_t diffOutput;
	memset(&diffInput, 0, sizeof(diffInput));
	diffInput.zone_low = 1;
	diffInput.zone_high = 3;
	diffInput.rule_type = MYKI_CD_DIFFERENTIAL_PRICING_RULE_TYPE_STANDARD;
	diffInput.passenger_type_null = 1;
	RunFunc(MYKI_CD_getDifferentialPriceStructure(&diffInput, &diffOutput), "MYKI_CD_getDifferentialPriceStructure", diffOutput.id);
	diffInput.zone_high = 2;
	RunFunc(MYKI_CD_getDifferentialPriceStructure(&diffInput, &diffOutput), "MYKI_CD_getDifferentialPriceStructure", diffOutput.id);

	MYKI_CD_DifferentialPricingRequest_t diffInputP;
	MYKI_CD_DifferentialPricingProvisional_t diffOutputP;
	memset(&diffInputP, 0, sizeof(diffInputP));
	diffInputP.zone_low = 1;
	diffInputP.zone_high = 3;
	diffInputP.rule_type = MYKI_CD_DIFFERENTIAL_PRICING_RULE_TYPE_STANDARD;
	diffInputP.passenger_type_null = 1;
	RunFunc(MYKI_CD_getDifferentialPriceProvisionalStructure(&diffInputP, &diffOutputP), "MYKI_CD_getDifferentialPriceProvisionalStructure", diffOutput.id);
    
    // Not currently required
	//MYKI_CD_FaresEpass_t faresEPass;
	//RunFunc(MYKI_CD_getFaresEpassStructure(3, 4, 194, &faresEPass), "MYKI_CD_getFaresEpassStructure", faresEPass.value);

	MYKI_CD_FaresProvisional_t faresProvisional;
	RunFunc(MYKI_CD_getFaresProvisionalStructure(14, 63, &faresProvisional), "MYKI_CD_getFaresProvisionalStructure", faresProvisional.sv_full);

	MYKI_CD_FaresStoredValue_t faresStoredValue;
	RunFunc(MYKI_CD_getFaresStoredValueStructure(11, 65, &faresStoredValue), "MYKI_CD_getFaresStoredValueStructure", faresStoredValue.single_trip);

	MYKI_CD_Locations_t locations1;
	MYKI_CD_Locations_t locations2;
	RunFunc(MYKI_CD_getLocationsStructure(19974, &locations1), "MYKI_CD_getLocationsStructure", locations1.id);
	RunFunc(MYKI_CD_getLocationsStructure(64408, &locations2), "MYKI_CD_getLocationsStructure", locations2.id);

	MYKI_CD_MasterRoute_t masterRoute;
	RunFunc(MYKI_CD_getMasterRouteStructure(1970, 19, &masterRoute), "MYKI_CD_getMasterRouteStructure", masterRoute.locationCount);

	MYKI_CD_PassengerType_t passengerType;
	RunFunc(MYKI_CD_getPassengerTypeStructure(1, &passengerType), "MYKI_CD_getPassengerTypeStructure", passengerType.code);

	MYKI_CD_ProductIssuer_t productIssuer;
	RunFunc(MYKI_CD_getProductIssuerStructure(1, &productIssuer), "MYKI_CD_getProductIssuerStructure", productIssuer.short_desc);

	MYKI_CD_Product_t product;
	RunFunc(MYKI_CD_getProductStructure(3, &product), "MYKI_CD_getProductStructure", product.short_desc);

	char charBuf[100];
	RunFunc(MYKI_CD_getProductType(3, charBuf, sizeof(charBuf)), "MYKI_CD_getProductType", charBuf);
	RunFunc(MYKI_CD_getProductCardType(3, charBuf, sizeof(charBuf)), "MYKI_CD_getProductCardType", charBuf);

	MYKI_CD_RouteInfo_t routeInfo;
	RunFunc(MYKI_CD_getRouteInfoStructure(9057, &routeInfo), "MYKI_CD_getRouteInfo", routeInfo.short_desc);

	MYKI_CD_RouteSection_t routeSection = {0};
	RunFunc(MYKI_CD_getRouteSectionStructure(10, 20, &routeSection), "MYKI_CD_getRouteSection", routeSection.short_desc);

	MYKI_CD_RouteStop_t* pRouteStops = 0;
	RunFunc(MYKI_CD_getRouteStopsStructure(11096, 10, 20, &pRouteStops), "MYKI_CD_getRouteStopsStructure", result);

	MYKI_CD_ServiceProviders_t serviceProviders;
	RunFunc(MYKI_CD_getServiceProvidersStructure(6700, &serviceProviders), "MYKI_CD_getServiceProvidersStructure", serviceProviders.short_desc);
	RunFunc(MYKI_CD_getServiceProviderTransportMode(6700, charBuf, sizeof(charBuf)), "MYKI_CD_getServiceProviderTransportMode", charBuf);

	MYKI_CD_LinesByStation_t linesByStation;
	MYKI_CD_StationsByLine_t stationsByLine;
	MYKI_CD_U16Array_t u16Array;
	RunFunc(MYKI_CD_getLinesByStationStructure(55000, &linesByStation, &u16Array), "MYKI_CD_getLinesByStationStructure", u16Array.arraySize);
	RunFunc(MYKI_CD_getLinesByStationStructure(55001, &linesByStation, &u16Array), "MYKI_CD_getLinesByStationStructure", u16Array.arraySize);
	RunFunc(MYKI_CD_getStationsByLineStructure(10, &stationsByLine, &u16Array), "MYKI_CD_getStationsByLineStructure", u16Array.arraySize);

	MYKI_CD_Stations_t stations;
	RunFunc(MYKI_CD_getStationsStructure(55000, &stations), "MYKI_CD_getStationsStructure", stations.id);
	RunFunc(MYKI_CD_getStationsByLocationIdStructure(64408, &stations), "MYKI_CD_getStationsByLocationIdStructure", stations.id);

	RunFunc(MYKI_CD_isCitySaverZone(1), "MYKI_CD_isCitySaverZone", result);
	RunFunc(MYKI_CD_isEarlyBirdStation(55000), "MYKI_CD_isEarlyBirdStation", result);
	RunFunc(MYKI_CD_isEarlyBirdDay(3), "MYKI_CD_isEarlyBirdDay", result);
	RunFunc(MYKI_CD_getEarlyBirdProductId(), "MYKI_CD_getEarlyBirdProductId", result);
	RunFunc(MYKI_CD_getEarlyBirdCutOffTime(), "MYKI_CD_getEarlyBirdCutOffTime", result);

	MYKI_CD_Locations_t changeOver;
	RunFunc(MYKI_CD_getChangeoverLoc(&locations1, &locations2, &changeOver), "MYKI_CD_getChangeoverLoc", changeOver.short_desc);

	/* Actionlist functions */
	int actionSeqNo = 1;
	char cardUID[LEN_CardUid_t] = {'\0'};
	Rows rows;
	getActionlistDBAccess()->executeSqlQuery("select PRINTF(\"%014X\", MIN(card_uid)) from Request r, 'Transaction' t WHERE t.FK_Request=r.Request_PK AND t.action_seq_no=1", rows);
	for ( Rows::iterator row = rows.begin(); row != rows.end(); row++ )
	{
		if ( row->begin() != row->end() )
			strcpy(cardUID, row->begin()->second);
	}
	long buf[1024];
	RunFunc(MYKI_ACTIONLIST_getSequence(), "MYKI_ACTIONLIST_getSequence", result);
	std::cout << "MYKI_ACTIONLIST_getSequenceInfo returned " << MYKI_ACTIONLIST_getSequenceInfo() << std::endl;
	RunFunc(MYKI_ACTIONLIST_getMajorVersion(), "MYKI_ACTIONLIST_getMajorVersion", result);
	RunFunc(MYKI_ACTIONLIST_getMinorVersion(), "MYKI_ACTIONLIST_getMinorVersion", result);
	RunFunc(MYKI_ACTIONLIST_getDeviceHotlistSequence(), "MYKI_ACTIONLIST_getDeviceHotlistSequence", result);
	std::cout << "MYKI_ACTIONLIST_getDeviceHotlistSequenceInfo returned " << MYKI_ACTIONLIST_getDeviceHotlistSequenceInfo() << std::endl;
	RunFunc(MYKI_ACTIONLIST_isCardActionlist(cardUID), "MYKI_ACTIONLIST_isCardActionlist", result);
	RunFunc(MYKI_ACTIONLIST_getCardActionlistRequests(cardUID, (MYKI_ACTIONLIST_Card_Actionlist_Requests_t*)buf), "MYKI_ACTIONLIST_getCardActionlistRequests", result);
	RunFunc(MYKI_ACTIONLIST_isDeviceHotlisted(1000, DateC19_t(78156)), "MYKI_ACTIONLIST_isDeviceHotlisted", result);
	RunFunc(MYKI_ACTIONLIST_getTAppUpdateRequestNone(cardUID, actionSeqNo, (MYKI_ACTIONLIST_TAppUpdateRequest_t*)buf), "MYKI_ACTIONLIST_getTAppUpdateRequestNone", result);
	RunFunc(MYKI_ACTIONLIST_getTAppUpdateRequestActivate(cardUID, actionSeqNo, (MYKI_ACTIONLIST_TAppUpdateRequest_t*)buf), "MYKI_ACTIONLIST_getTAppUpdateRequestActivate", result);
	RunFunc(MYKI_ACTIONLIST_getTAppUpdateRequestBlock(cardUID, actionSeqNo, (MYKI_ACTIONLIST_TAppUpdateRequest_t*)buf), "MYKI_ACTIONLIST_getTAppUpdateRequestBlock", result);
	RunFunc(MYKI_ACTIONLIST_getTAppUpdateRequestUnblock(cardUID, actionSeqNo, (MYKI_ACTIONLIST_TAppUpdateRequest_t*)buf), "MYKI_ACTIONLIST_getTAppUpdateRequestUnblock", result);
	RunFunc(MYKI_ACTIONLIST_getTPurseLoadRequestNone(cardUID, actionSeqNo, (MYKI_ACTIONLIST_TPurseLoadRequest_t*)buf), "MYKI_ACTIONLIST_getTPurseLoadRequestNone", result);
	RunFunc(MYKI_ACTIONLIST_getTPurseLoadRequestDebit(cardUID, actionSeqNo, (MYKI_ACTIONLIST_TPurseLoadRequest_t*)buf), "MYKI_ACTIONLIST_getTPurseLoadRequestDebit", result);
	RunFunc(MYKI_ACTIONLIST_getProductSaleRequestNone(cardUID, actionSeqNo, (MYKI_ACTIONLIST_ProductSaleRequest_t*)buf), "MYKI_ACTIONLIST_getProductSaleRequestNone", result);
	RunFunc(MYKI_ACTIONLIST_getTPurseUpdateRequestBlock(cardUID, actionSeqNo, (MYKI_ACTIONLIST_TPurseUpdateRequest_t*)buf), "MYKI_ACTIONLIST_getTPurseUpdateRequestBlock", result);
	RunFunc(MYKI_ACTIONLIST_getProductUpdateRequestBlock(cardUID, actionSeqNo, (MYKI_ACTIONLIST_ProductUpdateRequest_t*)buf), "MYKI_ACTIONLIST_getProductUpdateRequestBlock", result);
	RunFunc(MYKI_ACTIONLIST_getProductUpdateRequestUnblock(cardUID, actionSeqNo, (MYKI_ACTIONLIST_ProductUpdateRequest_t*)buf), "MYKI_ACTIONLIST_getProductUpdateRequestUnblock", result);
	RunFunc(MYKI_ACTIONLIST_getTPurseUpdateRequestNone(cardUID, actionSeqNo, (MYKI_ACTIONLIST_TPurseUpdateRequest_t*)buf), "MYKI_ACTIONLIST_getTPurseUpdateRequestNone", result);
	RunFunc(MYKI_ACTIONLIST_getProductUpdateRequestNone(cardUID, actionSeqNo, (MYKI_ACTIONLIST_ProductUpdateRequest_t*)buf), "MYKI_ACTIONLIST_getProductUpdateRequestNone", result);
	RunFunc(MYKI_ACTIONLIST_getOAppUpdateRequestBlock(cardUID, actionSeqNo, (MYKI_ACTIONLIST_OAppUpdateRequest_t*)buf), "MYKI_ACTIONLIST_getOAppUpdateRequestBlock", result);
	RunFunc(MYKI_ACTIONLIST_getOAppUpdateRequestUnblock(cardUID, actionSeqNo, (MYKI_ACTIONLIST_OAppUpdateRequest_t*)buf), "MYKI_ACTIONLIST_getOAppUpdateRequestUnblock", result);

    MYKI_CD_HMIMessaging_t array[50];
	RunFunc(MYKI_CD_getHMIMessaging("LoadLog", array, 50), "MYKI_CD_getHMIMessaging", result);
    for(int i=0;i<result;i++) 
    {
        std::cout << i << "\t" << array[i].priority << "\t" << array[i].textToDisplay << std::endl;
        std::cout << "\tFieldkey1 - TAppLoadLog.TxType       \t" << array[i].fieldKey1<< std::endl;
        std::cout << "\tFieldkey2 - TAppLoadLog.PaymentMethod\t" << array[i].fieldKey2<< std::endl;
        std::cout << "\tEffect1\t"   << array[i].effect1 << std::endl;
        std::cout << "\ttextToDisplay\t" << array[i].textToDisplay<< std::endl;
    }
    std::cout << "--------------------------------------" << std::endl;
	RunFunc(MYKI_CD_getHMIMessaging("UsageLog", array, 50), "MYKI_CD_getHMIMessaging", result);
    for(int i=0;i<result;i++) 
    {
        std::cout << i << "\t" << array[i].priority << "\t" << array[i].textToDisplay << std::endl;
        std::cout << "\tFieldkey1 - TAppUsageLog.TxType                                            \t" << array[i].fieldKey1<< std::endl;
        std::cout << "\tFieldkey2 - TAppUsageLog.ProductValidationStatus bit 0 (Activate e-pass)   \t" << array[i].fieldKey2<< std::endl;
        std::cout << "\tFieldkey3 - TAppUsageLog.ProductValidationStatus bit 1 (Scan-on)           \t" << array[i].fieldKey3<< std::endl;
        std::cout << "\tFieldKey4 – TAppUsageLog.ProductValidationStatus bit 2 (Scan-off)          \t" << array[i].fieldKey4<< std::endl;
        std::cout << "\tFieldKey5 – TAppUsageLog.ProductValidationStatus bit 3 (Forced)            \t" << array[i].fieldKey5<< std::endl;
        std::cout << "\tFieldKey6 – TAppUsageLog.ProductValidationStatus bit 4 (Provisional)       \t" << array[i].fieldKey6<< std::endl;
        std::cout << "\tFieldKey7 – TAppUsageLog.ProductValidationStatus bit 6 (Off peak)          \t" << array[i].fieldKey7<< std::endl;
        std::cout << "\tFieldKey8 – TAppUsageLog.ProductValidationStatus bit 7 (Premium Surcharge) \t" << array[i].fieldKey8<< std::endl;
        std::cout << "\tEffect1\t"   << array[i].effect1 << std::endl;
        std::cout << "\ttextToDisplay\t" << array[i].textToDisplay<< std::endl;
    }
    
    MYKI_CD_RouteInfo_t routes[2000];
	result = RunFunc(MYKI_CD_getRoutesByServiceProvider(1001, routes, 2000), "MYKI_CD_getRoutesByServiceProvider TRAM", result);
    
    for(int i=0;i<result;i++) 
    {
        std::cout << i << "\t" << routes[i].route_id << "\t" << routes[i].master_route << "\t" << routes[i].code << "\t" << routes[i].short_desc << "\t" <<  routes[i].long_desc << std::endl;
    }
    
    std::cout << "--------------------------------------" << std::endl;
	result = RunFunc(MYKI_CD_getRoutesByServiceProvider(18, routes, 2000), "MYKI_CD_getRoutesByServiceProvider BUS", result);
    
    for(int i=0;i<result;i++) 
    {
        std::cout << i << "\t" << routes[i].route_id << "\t" << routes[i].master_route << "\t" << routes[i].code << "\t" << routes[i].short_desc << "\t" <<  routes[i].long_desc << std::endl;
    }    
    
	return 0;
}
