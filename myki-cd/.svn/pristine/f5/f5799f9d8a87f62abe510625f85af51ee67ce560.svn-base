/*
 * ProductConfigurationStructs.h
 *
 *  Created on: 08/10/2012
 *      Author: James Ho
 */

#ifndef PRODUCTCONFIGURATIONSTRUCTS_H_
#define PRODUCTCONFIGURATIONSTRUCTS_H_

#include "corebasetypes.h"
#include "coretypes.h"
#include "myki_cd.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct
{
	U16_t id;
	U8_t zone;
	U8_t inner_zone;
	U8_t outer_zone;
	char short_desc[LEN_Short_Description_t];
	char long_desc[LEN_Long_Description_t];
	// gps points should be float. Pass as char for now as it's not being used.
	char gps_lat[LEN_Gps_t];
	char gps_long[LEN_Gps_t];
	char gps_lat_tolerance[LEN_Gps_t];
	char gps_long_tolerance[LEN_Gps_t];
}  Locations_t;

typedef struct
{
	U16_t id;
	U16_t location;
	U8_t provisional_zone_low;
	U8_t provisional_zone_high;
} Stations_t;

typedef struct
{
	U16_t station;
//	std::vector<U16_t> line;
} LinesByStation_t;

typedef struct
{
	U16_t line;
//	std::vector<U16_t> station;
} StationsByLine_t;

typedef struct
{
	U16_t arraySize;
	U16_t* arrayOfU16;
} U16Array_t;

typedef struct
{
	U8_t zone_low;
	U8_t zone_high;
	U16_t days;
	U32_t value;
} FaresEpass_t;

typedef struct
{
	U16_t id;
	char short_desc[LEN_Short_Description_t];
	char long_desc[LEN_Long_Description_t];
	char mode[LEN_Mode_t];
} ServiceProviders_t;

typedef struct
{
	U8_t zone_low;
	U8_t zone_high;
	U32_t sv_full;
	U32_t sv_conc;
	U32_t epass_full;
	U32_t epass_conc;
} FaresProvisional_t;

typedef struct
{
	U8_t zone_low;
	U8_t zone_high;
	U32_t single_trip;
	U32_t nhour;
	U32_t daily;
	U32_t weekly;
	U32_t weekend;
} FaresStoredValue_t;

//struct ProductAttribute_t
//{
//	std::string name;
//	std::string value;
//};
//
//struct ProductDefaultAttribute_t
//{
//	std::string name;
//	std::string value;
//};

typedef struct
{
	U8_t issuer_id;
	U8_t id;
	char type[LEN_20_t];
	char subtype[LEN_20_t];
	char short_desc[LEN_Short_Description_t];
	char long_desc[LEN_Long_Description_t];
//	std::vector<ProductDefaultAttribute_t> productDefaultAttribute;
//	std::vector<ProductAttribute_t> productAttribute;
} Product_t;

typedef struct
{
	U8_t id;
	char code[LEN_Code_t];
	char desc[LEN_Short_Description_t];
	U8_t percent;
	char isdda[LEN_6_t];
	U16_t cardexpirydays;
	U16_t tappexpirydays;
	U16_t passengercodedays;
} PassengerType_t;

typedef struct
{
	U8_t id;
	char short_desc[LEN_Short_Description_t];
	char long_desc[LEN_Long_Description_t];
} ProductIssuer_t;

//typedef struct
//{
//	std::string zone;
//	std::string mode;
//	std::string value;
//	std::string device_type;
//	std::string location;
//	std::string service_provider;
//	std::string passenger_type;
//	std::string blocking_reason;
//	std::string payment_method;
//} DeviceParameterAttribute_t;

typedef struct
{
	char name[LEN_Short_Description_t];
	char default_value[LEN_20_t];
	char value[LEN_20_t];
//	std::vector<DeviceParameterAttribute_t> deviceParameterAttribute;
} DeviceParameter_t;

#ifdef __cplusplus
}
#endif

#endif /* PRODUCTCONFIGURATIONSTRUCTS_H_ */
