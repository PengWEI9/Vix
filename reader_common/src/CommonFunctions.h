#ifndef COMMON_FUNCTIONS_H_INCLUDED
#define COMMON_FUNCTIONS_H_INCLUDED

#include "CommonConversion.h"

#include "crc32.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <time.h>
#include <assert.h>
#include "cmd.h"

#if defined(MSDOS) || defined(OS2) || defined(WIN32) || defined(__CYGWIN__)
#  include <fcntl.h>
#  include <io.h>
#  define SET_BINARY_MODE(file) setmode(fileno(file), O_BINARY)
#else
#  define SET_BINARY_MODE(file)
#endif

#define CHUNK 16384

class CommonFunctions
{
private:

public:
	CommonFunctions(void);
	~CommonFunctions(void);
	char *getTime();
	int getTimeLen();

    time_t DosTimeToUTime(U16_t DosTime);
    time_t UnixTimeToUTime(U16_t UnixTimeTime);

	int FindIndexOfSubstring(char *strSourceString, char *strSubstringToSearch);
    void SetTime(struct tm settime);
    long FindSizeOfFile(char *FileLocation);
    int ReadFile(char* FileLocation, char *Buffer, long Length);
    int WriteFile(char* FileLocation, char *Buffer, long Length);
    int AddCRC32(char *Buffer, int Length);
    int FlushIO();
};
#endif
