/* Copyright (c) 2011. All rights reserved. ERG Transit Systems (HK) Ltd.
**
** inifile.h - ini file access functions
**
** Version Date    Name       Reason
** 1.0     16/8/11 JohnD       Created
**
*/


#ifndef __inifile_h__
#define __inifile_h__

#define INI_STR         1
#define INI_INT         2
#define INI_CHAR_INT    3
#define INI_CHAR        4
#define INI_LONG        5

void IniFileInit();
int IniFileGetData(const char *Tag, const char *Section, const char *Option, void *pData, int bType);

#endif

/* end of file */
