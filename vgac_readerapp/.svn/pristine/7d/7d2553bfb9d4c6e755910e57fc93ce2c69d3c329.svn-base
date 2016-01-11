#include <stdio.h>
#include <string.h>
#include "datec19.h"
#include "Common.h"

#include "cs.h"

time_t getDateTime(const char * d)
{
    time_t now;
    struct tm timeinfo;

    time(&now);
    localtime_r( &now, &timeinfo );

    if ( strlen(d) == 19 )
    {
        int year;
        int month;
        char separator;
        sscanf(d, "%d%c%d%c%d%c%d%c%d%c%d", &year, &separator, &month, &separator, &timeinfo.tm_mday, &separator, &timeinfo.tm_hour, &separator, &timeinfo.tm_min, &separator, &timeinfo.tm_sec);
        timeinfo.tm_year = year - 1900;
        timeinfo.tm_mon = month - 1;
		timeinfo.tm_isdst = -1;
    }
    else
    {
        sscanf(d, "%d:%d:%d", &timeinfo.tm_hour, &timeinfo.tm_min, &timeinfo.tm_sec);
		timeinfo.tm_isdst = -1;
    }

    return (  mktime(&timeinfo) );

}

DateC19_t getDateC19(const char * d)
{
    struct tm timeinfo;

    sscanf(d, "%d:%d:%d:%d:%d:%d", &timeinfo.tm_year, &timeinfo.tm_mon, &timeinfo.tm_mday, &timeinfo.tm_hour, &timeinfo.tm_min, &timeinfo.tm_sec);

    return (mkdate(&timeinfo));
}
