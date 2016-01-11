/* Copyright (c) 2011. All rights reserved. ERG Transit Systems (HK) Ltd.
**
** CRC.h - CRC 16 Routine
**
** Version Date    Name       Reason
** 1.0     12/12/11  Gerry       Created
**
*/
#pragma once
#include <string.h>
# define  EOS_U8        unsigned char
# define  EOS_U16       unsigned short
# define  EOS_S16       signed short
# define  EOS_U32       unsigned long

#ifdef __cplusplus
extern "C" {
#endif
//These are the interfaces for C
unsigned short calcCRC16(unsigned char *pStart, int Length);
#ifdef __cplusplus
}
#endif

class CRC
{
public:
	CRC(void);
	~CRC(void);
	unsigned short getCRC16(unsigned char *, int Length);
};
