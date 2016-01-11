#ifndef ZIP_API_H_INCLUDED
#define ZIP_API_H_INCLUDED

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <utime.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "zip.h"


#define WRITEBUFFERSIZE 16384

int OpenZipFile(char *filename);
int AddFileToZip(char *filename, char *data, int len);
void CloseZipFile();

#endif
