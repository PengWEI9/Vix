/* Copyright (c) 2011. All rights reserved. ERG Transit Systems (HK) Ltd.
**
** CommonConverssion.h - provide function for basic conversion
**
** Version Date    Name       Reason
** 1.0     12/12/11  Gerry       Created
**
*/
#ifndef COMMON_CONVERSION_H_INCLUDED
#define COMMON_CONVERSION_H_INCLUDED

#include <stdio.h>
#include "corebasetypes.h"
#include "time.h"
//#include "carddata.h"

class CommonConversion
{
public:
	CommonConversion(void);
	~CommonConversion(void);
	char* ConvertToCharArray(unsigned short Input);
	char* ConvertToCharArray(U64_t Input);
	char* ConvertToCharArray(unsigned int Input);
	char* ConvertToCharArray(int Input);

//	void Convert(unsigned long val,  unsigned char *output);
//	void Convert(unsigned short val, unsigned char *output);
//	void Convert(unsigned char val,  unsigned char *output);
//	unsigned long Convert(unsigned char *input);
//	unsigned short Convert(unsigned char *input);
//	unsigned char Convert(unsigned char *input);



	int ConvertToInt(char FirstChar);
	int ConvertToInt(char FirstChar, char SecondChar);
	int ConvertToInt(char FirstChar, char SecondChar, char ThirdChar, char FourthChar);

	unsigned int ConvertToUInt(char FirstChar, char SecondChar, char ThirdChar, char FourthChar);
	unsigned int ConvertToUInt(unsigned char FirstChar, unsigned char SecondChar, unsigned char ThirdChar, unsigned char FourthChar);
    unsigned int ConvertToUInt(char* Value);
    unsigned int ConvertToUInt(unsigned char *Value);

	unsigned short ConvertToUShort(char FirstChar, char SecondChar);
	unsigned short ConvertToUShort(unsigned char FirstChar, unsigned char SecondChar);
	unsigned short ConvertToUShort(char* Value);
	unsigned short ConvertToUShort(unsigned char *Value);

	U64_t ConvertToU64(char *Value);
	U64_t ConvertToU64(unsigned char *Value);

	time_t DosTimeToUTime(U16_t DosTime);
	time_t UnixTimeToUTime(U16_t UnixTime);

};

#endif
