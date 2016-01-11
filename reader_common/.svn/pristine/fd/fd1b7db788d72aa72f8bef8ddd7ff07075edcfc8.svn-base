/* Copyright (c) 2011. All rights reserved. ERG Transit Systems (HK) Ltd.
**
** inifile.c - ini file access functions
**
** Version Date    Name       Reason
** 1.0     16/8/11 JohnD       Created
**
*/


#include <stdlib.h>             /*  atoi, atol */
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <cs.h>
#include <csf.h>                /*  CsfIniInit, CsfIniSearch */
#include <csmutex.h>
#include <inifile.h>

#define TAG_SIZE 50
#define LINE_BUFFER_SIZE 300
#define CONFIG_FILE "retail.ini"

#define INIT_SEMAPHORE(pSemaphore) \
        CsAssert(CsMutexInit(pSemaphore, CSSYNC_THREAD)== 0, "Init semaphore -1ed")
#define RESERVE_SEMAPHORE(pSemaphore) \
        CsAssert(CsMutexLock(pSemaphore) == 0, "lock semaphore -1ed")
#define RELEASE_SEMAPHORE(pSemaphore) \
        CsAssert(CsMutexUnlock(pSemaphore) == 0, "release semaphore -1ed")

static CsMutex_t IniLock;
static char      CurrentTag[TAG_SIZE];

/*---------------------------------------------------------------------------
Synopsis:
    int IniInit()

Purpose:
    Intialises config module.

Parameters:
    None

Returns:
    None


---------------------------------------------------------------------------*/
void IniFileInit()
{
    INIT_SEMAPHORE( &IniLock );
    CsfIniInit( 0, NULL );
    memset( CurrentTag, 0, TAG_SIZE );
}


/*---------------------------------------------------------------------------
Synopsis:
    int IniGetData(const char *Tag, const char *Section, const char *Option,
                  void *pData, int bSize)

Purpose:
    Get a config data from ini file.

Parameters:
    const char *Tag         Tag name
    const char *Section     Section name
    const char *Option      Option name
    void       *pData       Pointer of data buffer
    int         bSize       Buffer size

Returns:
    int ReturnStatus        0 = Config data found
                            non-zero = not found


---------------------------------------------------------------------------*/
int IniFileGetData(const char *Tag, const char *Section, const char *Option,
              void *pData, int bType)
{
    int ReturnStatus = 0;
    static char TmpStr[LINE_BUFFER_SIZE], LineBuffer[LINE_BUFFER_SIZE];

    CsDebug(9, (9, "CD::CD_GetIniData: %s %s %s", Tag, Section, Option));

    RESERVE_SEMAPHORE( &IniLock );

    /* don't have to open the tag again if it was previously opened */
    if (strcmp(CurrentTag, Tag) != 0)
    {
        ReturnStatus = -1;
        CsfIniInit( 0, NULL );
        strcpy(TmpStr, CONFIG_FILE);
        CsWarn("CD_GetIniData: Open tag '%s', file (%s)", Tag, TmpStr);
    }

    if (ReturnStatus != 0)
    {
        CsWarn("CD_GetIniData: Open tag '%s' -1ed, file (%s)", Tag, TmpStr);
        RELEASE_SEMAPHORE( &IniLock );
        return (-1);
    }

    /* update the tag */
    strcpy( CurrentTag, Tag );
    sprintf(TmpStr, "%s:%s", Section, Option);
    ReturnStatus = CsfIniSearch( (const char *)TmpStr, LineBuffer, LINE_BUFFER_SIZE );

    if (ReturnStatus == 0)
    {
       switch (bType)
       {
          case INI_STR:
              strcpy((char *)pData, LineBuffer);
              CsDebug(9, (9, "CD::CD_GetIniData: Found %s", (char *)pData));
              break;
          case INI_INT:
              *( (int *)pData ) = atoi(LineBuffer);
              CsDebug(9, (9, "CD::CD_GetIniData: Found %i", *(int *)pData));
              break;
          case INI_CHAR_INT:
              *( (char *)pData ) = atoi(LineBuffer);
              CsDebug(9, (9, "CD::CD_GetIniData: Found %i", *(char *)pData));
              break;
          case INI_CHAR:
              memcpy((char *)pData, LineBuffer, 1);
              CsDebug(9, (9, "CD::CD_GetIniData: Found %c", *(char *)pData));
              break;
          case INI_LONG:
              *( (unsigned long *)pData ) = atol(LineBuffer);
              CsDebug(9, (9, "CD::CD_GetIniData: Found %li", *(unsigned long *)pData));
              break;
          default:
              /* do nothing */
              break;
        }
    }

    RELEASE_SEMAPHORE( &IniLock );

    return( ReturnStatus );
}



