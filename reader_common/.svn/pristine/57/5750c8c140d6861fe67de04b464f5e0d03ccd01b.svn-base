/* Copyright (c) 2011. All rights reserved. ERG Transit Systems (HK) Ltd.
**
** zip_api.cpp - interface for accessing zip functions
**
** Version Date    Name       Reason
** 1.0     12/12/11  Gerry       Created
**
*/

#include "zip_api.h"

static zipFile zf;
static char buf[WRITEBUFFERSIZE];

int OpenZipFile(char *filename)
{
    if ((zf = zipOpen64(filename, 0)) == NULL)
    {
        printf("error opening %s\n", filename);
        return -1;
    }
    return 0;
}

int AddFileToZip(char *filename, char *data, int len)
{
    zip_fileinfo zi;
    struct tm *filedate;
    time_t tm_t = 0;
    int err = ZIP_OK;

    if (zf == 0)
    {
        printf("zf == 0, operator error!\n");
        return -1;
    }

    memset(&zi, 0, sizeof(zi));

    time(&tm_t);
    filedate = localtime(&tm_t);
    zi.tmz_date.tm_sec  = filedate->tm_sec;
    zi.tmz_date.tm_min  = filedate->tm_min;
    zi.tmz_date.tm_hour = filedate->tm_hour;
    zi.tmz_date.tm_mday = filedate->tm_mday;
    zi.tmz_date.tm_mon  = filedate->tm_mon ;
    zi.tmz_date.tm_year = filedate->tm_year;

    err = zipOpenNewFileInZip3_64(zf,
                     filename,
                     &zi,
                     NULL, 0, NULL, 0, NULL,
                     Z_DEFLATED,
                     Z_DEFAULT_COMPRESSION,
                     0,
                     -MAX_WBITS,
                     DEF_MEM_LEVEL,
                     Z_DEFAULT_STRATEGY,
                     NULL, 0, 0);

    if (err != ZIP_OK)
    {
        printf("error opening %s in zipfile\n", filename);
    }
    else
    {
        err = zipWriteInFileInZip (zf, data, len);
        if (err < 0)
        {
            printf("error in writing %s in the zipfile\n", filename);
        }
        else
        {
            err = zipCloseFileInZip(zf);
            if (err != ZIP_OK)
                printf("error in closing %s in the zipfile\n", filename);
        }
    }

    return (err == ZIP_OK) ? 0 : -1;
}


void CloseZipFile()
{
    if (zipClose(zf, NULL) != ZIP_OK)
        printf("error closing zip");
    zf = 0;
}

/* end of file */
