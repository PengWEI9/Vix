/* Copyright (c) 2011. All rights reserved. ERG Transit Systems (HK) Ltd.
**
** CommonConverssion.cpp - provide function for basic conversion
**
** Version Date    Name       Reason
** 1.0     12/12/11  Gerry       Created
**
*/
#include "CommonConversion.h"
#include <math.h>

// FIXME - we should be using the standard ntohl and hotnl functions -
// these are portable and expand correctly do or not do something depending
// on the compiler being used

CommonConversion::CommonConversion(void)
{
}


CommonConversion::~CommonConversion(void)
{
}

char* CommonConversion::ConvertToCharArray(unsigned short Input)
{
	// FIXME now fixed to be static, but not thread safe
	static char r[2];
	r[0] = Input;
	r[1] = Input>>8;
	return r;
}

char* CommonConversion::ConvertToCharArray(unsigned int Input)
{
	// FIXME now fixed to be static, but not thread safe
	static char r[4];
	r[0] = Input;
	r[1] = Input>>8;
	r[2] = Input>>16;
	r[3] = Input>>24;
	return r;
}

char* CommonConversion::ConvertToCharArray(U64_t Input)
{
    static char r[8];
	r[0] = Input;
	r[1] = Input>>8;
	r[2] = Input>>16;
	r[3] = Input>>24;
	r[4] = Input>>32;
	r[5] = Input>>40;
	r[6] = Input>>48;
	r[7] = Input>>56;
	return r;
}

char* CommonConversion::ConvertToCharArray(int Input)
{
	// FIXME now fixed to be static, but not thread safe
	static char r[4];
	r[0] = Input;
	r[1] = Input>>8;
	r[2] = Input>>16;
	r[3] = Input>>24;
	return r;
}

int CommonConversion::ConvertToInt(char FirstChar)
{
	return FirstChar;
}

int CommonConversion::ConvertToInt(char FirstChar, char SecondChar)
{
	return FirstChar + SecondChar * 256;
}

int CommonConversion::ConvertToInt(char FirstChar, char SecondChar, char ThirdChar ,char FourthChar)
{
	int temp = 0;
	temp = FirstChar;
	temp += SecondChar * 256;
	temp += ThirdChar * 256 * 256;
	temp += FourthChar * 256 * 256 * 256;
	return temp;
}

unsigned int CommonConversion::ConvertToUInt(char FirstChar, char SecondChar, char ThirdChar, char FourthChar)
{
	unsigned int temp = 0;
	temp = FirstChar;
	temp += SecondChar * 256;
	temp += ThirdChar * 256 * 256;
	temp += FourthChar * 256 * 256 * 256;
	return temp;
}

unsigned int CommonConversion::ConvertToUInt(unsigned char FirstChar, unsigned char SecondChar, unsigned char ThirdChar, unsigned char FourthChar)
{
	unsigned int temp = 0;
	temp = FirstChar;
	temp += SecondChar * 256;
	temp += ThirdChar * 256 * 256;
	temp += FourthChar * 256 * 256 * 256;
	return temp;
}
unsigned int CommonConversion::ConvertToUInt(unsigned char *Value)
{
    return Value[0] + (Value[1] * 256) + (Value[2] * 256 * 256) + (Value[3] * 256 * 256 * 256);
}

unsigned int CommonConversion::ConvertToUInt(char *Value)
{
    return ConvertToUInt((unsigned char *) Value);
}


U64_t CommonConversion::ConvertToU64(unsigned char *Value)
{
    U64_t temp[8],t,v,x[8];
    for(int i = 0; i < 8; i++)
    {
        x[i] = Value[i];
    }
    t = (U64_t)256;
    v = 0;
    temp[0] = x[0];
    temp[1] = x[1] * t;
    temp[2] = x[2] * pow(t,2);
    temp[3] = x[3] * pow(t,3);
    temp[4] = x[4] * pow(t,4);
    temp[5] = x[5] * pow(t,5);
    temp[6] = x[6] * pow(t,6);
    temp[7] = x[7] * pow(t,7);

    for(int i = 0; i < 8; i++)
    {
        v += temp[i];
    }
    return v;
}
U64_t CommonConversion::ConvertToU64(char *Value)
{
    return ConvertToU64((unsigned char *) Value);
}

unsigned short CommonConversion::ConvertToUShort(char FirstChar, char SecondChar)
{
	return FirstChar + SecondChar * 256 ;
}

unsigned short CommonConversion::ConvertToUShort(unsigned char FirstChar,unsigned char SecondChar)
{
	return FirstChar + SecondChar  * 256 ;
}

unsigned short CommonConversion::ConvertToUShort(unsigned char *Value)
{
    return Value[0] + Value[1] * 256;
}
unsigned short CommonConversion::ConvertToUShort(char* Value)
{
    return ConvertToUShort((unsigned char *) Value);
}
#if 0
void CommonConversion::Convert(unsigned long val, unsigned char *output)
{
	if (output)
		memcpy(output, &temp, sizeof(temp));
}

void CommonConversion::Convert(unsigned short val, unsigned char *output)
{
	if (output)
		memcpy(output, &val, sizeof(val));
}

void CommonConversion::Convert(unsigned char val,  unsigned char *output)
{
	if (output)
		*output = val;
}

unsigned long CommonConversion::Convert(unsigned char *input)
{
	unsigned long temp = 0;

	if (input)
	{
		memcpy(&temp, input, sizeof(temp));
	}

	return temp;
}

unsigned short CommonConversion::Convert(unsigned char *input)
{
	unsigned short temp = 0;

	if (input)
	{
		memcpy(&temp, input, sizeof(temp));
	}

	return temp;
}

unsigned char CommonConversion::Convert(unsigned char *input)
{
	if (input)
		return *input;

	return 0;
}


time_t CommonConversion::DosTimeToUTime(U16_t DosTime)
{
    int year,month,day;

    year = DosTime>>9;
    month = (DosTime>>5) & 0x0F;
    day = DosTime & 0x1F;

    year += 1980;

    struct tm *timeinfo;

    time_t dtNow = time(NULL);

    time ( &dtNow );
    timeinfo = localtime ( &dtNow );

    timeinfo->tm_year = year - 1900;
    timeinfo->tm_mon = month - 1;
    timeinfo->tm_mday = day;
    timeinfo->tm_hour = 0;
    timeinfo->tm_min = 0;
    timeinfo->tm_sec = 0;

    return mktime(timeinfo);
}

time_t CommonConversion::UnixTimeToUTime(U16_t UnixTimeTime)
{
    int year,month,day;

    year = DosTime>>9;
    month = (DosTime>>5) & 0x0F;
    day = DosTime & 0x1F;

    year += 1970;

    struct tm *timeinfo;

    time_t dtNow = time(NULL);

    time ( &dtNow );
    timeinfo = localtime ( &dtNow );

    timeinfo->tm_year = year - 1900;
    timeinfo->tm_mon = month - 1;
    timeinfo->tm_mday = day;
    timeinfo->tm_hour = 0;
    timeinfo->tm_min = 0;
    timeinfo->tm_sec = 0;

    return mktime(timeinfo);
}




#endif


/* end of file */

