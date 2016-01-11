/* Copyright (c) 2011. All rights reserved. ERG Transit Systems (HK) Ltd.
**
** CommonFunctions.cpp - provide basic functions for the retail reader
**
** Version Date    Name       Reason
** 1.0     12/12/11  Gerry       Created
**
*/
#include "CommonFunctions.h"
#include <cs.h>
#include <csthread.h>
#include <errno.h>
#include <time.h>
#include <sys/time.h>
#include <sys/ioctl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <linux/rtc.h>
#include <linux/ioctl.h>

CommonFunctions::CommonFunctions()
{	}

CommonFunctions::~CommonFunctions()
{	}

void CommonFunctions::SetTime(struct tm settime)
{
    struct timeval systime;

    CsDebug(1,(1,"sec - %d", settime.tm_sec));
    CsDebug(1,(1,"min - %d", settime.tm_min));
    CsDebug(1,(1,"hr - %d", settime.tm_hour));
    CsDebug(1,(1,"day - %d", settime.tm_mday));
    CsDebug(1,(1,"mo - %d", settime.tm_mon));
    CsDebug(1,(1,"yr - %d", settime.tm_year));

    CsDebug(1,(1,"mktime - %d", mktime(&settime)));

    settime.tm_mon--;
    settime.tm_year -= 1900;
    systime.tv_sec = mktime(&settime);
    systime.tv_usec = 0;

    settimeofday(&systime, NULL);
    int fd;
    if((fd = open("/dev/rtc", O_RDWR))<0)
    {
        CsDebug(1,(1,"RTC Set Failed"));
    }
    else
    {
        CsDebug(1,(1,"Setting RTC"));
        ioctl(fd,RTC_SET_TIME,&settime);
        close(fd);
    }

}

char *CommonFunctions::getTime()
{
  time_t rawtime;
  struct tm * timeinfo;
  time ( &rawtime );
  timeinfo = localtime ( &rawtime );
  return asctime (timeinfo);
}
int CommonFunctions::getTimeLen()
{
	return strlen(getTime());
}

int CommonFunctions::FindIndexOfSubstring(char *strSourceString, char *strSubstringToSearch)
{
	char TempSource[350];
	memset(TempSource, 0, sizeof(TempSource));
	strncpy(TempSource, strSourceString, strlen(strSourceString));

	char *Temp;
	Temp = strstr(TempSource,strSubstringToSearch);
	for(int i = 0; i < (int)strlen(strSourceString); i++)
	{
		if(Temp == &TempSource[i])
			return i;
	}
	return -1;
}

long CommonFunctions::FindSizeOfFile(char * FileLocation)
{
    FILE * pFile;
    long lSize;
//  size_t result;
    pFile = fopen(FileLocation,"rb");
    if (pFile == NULL)
    {
        CsDebug(1,(1,"Failed To Read - %s", FileLocation));
        return -1;
    }
    fseek (pFile, 0, SEEK_END);
    lSize = ftell(pFile);
    rewind(pFile);
    fclose(pFile);
    return lSize;
}

int CommonFunctions::ReadFile(char *FileLocation, char *Buffer, long Length)
{
    FILE * pFile;
    pFile = fopen(FileLocation,"rb");
    if (pFile == NULL)
    {
        CsDebug(1,(1,"File Record Error"));
        return -1;
    }
    int result = fread(Buffer,1,Length,pFile);
    fclose(pFile);
    return result;
}

int CommonFunctions::WriteFile(char *FileLocation, char *Buffer, long Length)
{
    FILE * pFile;
    pFile = fopen(FileLocation, "wb");
    if (pFile == NULL)
    {
        CsDebug(1,(1,"File Record Error"));
        return -1;
    }
    int result = fwrite(Buffer, Length, 1, pFile);
    fclose(pFile);
    return result;
}

int CommonFunctions::AddCRC32(char *Buffer, int Length)
{
    CommonConversion cc;
    unsigned long crc32 = getCRC32((unsigned char *)Buffer,Length);
    CsDebug(1,(1,"CRC Calculated - %x", crc32));
    memcpy(&Buffer[Length], cc.ConvertToCharArray((int)crc32),4);
    return 0;
}

int CommonFunctions::FlushIO()
{
    CsDebug(1,(1,"Flush IO Start"));
    char FlushCommand[40];
    memset(FlushCommand,0x00,sizeof(FlushCommand));
	strcat(FlushCommand,"/bin/sync");
	write_cmd(0, FlushCommand, strlen(FlushCommand));
	CsDebug(1,(1,"Flush IO End"));
    return 0;
}


time_t CommonFunctions::DosTimeToUTime(U16_t DosTime)
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

    CsDebug(1,(1,"y - %d, m - %d, d - %d", timeinfo->tm_year, timeinfo->tm_mon, timeinfo->tm_mday));

    return mktime(timeinfo);
}

time_t CommonFunctions::UnixTimeToUTime(U16_t UnixTimeTime)
{
    int year,month,day;

    year = UnixTimeTime>>9;
    month = (UnixTimeTime>>5) & 0x0F;
    day = UnixTimeTime & 0x1F;

    year += 1970;

    struct tm *timeinfo;

    time_t dtNow = time(NULL);

    time ( &dtNow );
    timeinfo = localtime ( &dtNow );

    timeinfo->tm_year = year - 1900;
    timeinfo->tm_mon = month - 1;
    timeinfo->tm_mday = day - 1;
    timeinfo->tm_hour = 0;
    timeinfo->tm_min = 0;
    timeinfo->tm_sec = 0;

    return mktime(timeinfo);
}

