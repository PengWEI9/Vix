/*============================================================================
**
**    Vix Technology       Licensed software
**    (C) 2014             All rights reserved
**
**============================================================================
**
**  Project/Product : Device Application Framework
**  Filename        : myki_time_conversion.c
**  Author(s)       : various
**
**  Description     :
**                    special 'myki' time conversion
**
**  Compiler options:  
**
**  Information     :
**   Compiler       : C
**   Target         : Portable
**
**  History         :
**   Date         Author   Description
**   -------------------------------------------------------------------------
**   12-02-2014   cdl      Publish for reuse in in 'xml2ini'
**==========================================================================*/

#include "myki_time_conversion.h"   /* include first to check interface completeness */

#include <time.h>

/* NTT Data - see below */
static long TimeTToDate24(time_t utc);
static time_t Date24ToTimeT(long numMins);

/*===========================================================================
Interface as extracted from myki_cardservices.c
**=========================================================================*/

void DateMykiU24ToTimeT(const U32_t DateMykiU24, Time_t *pTimeT)
{
   // Convert from DateMykiU24 (minutes from 01/01/2006: 0=01/01/2006) object to Time_t...
   *pTimeT = 0;

   if (DateMykiU24 > 0)
   {
      *pTimeT = Date24ToTimeT(DateMykiU24);
   }
}

void TimeTToDateMykiU24(const Time_t TimeT, U32_t *pDateMykiU24)
{
   // Convert from Time_t to DateMykiU24 (minutes from 01/01/2006: 0=01/01/2006) ...
   *pDateMykiU24 = 0;

   if (TimeT > TIME_T_BASE_FOR_2006)
   {
      *pDateMykiU24 = TimeTToDate24(TimeT);
   }
}

/*===========================================================================
** Date and Time conversion methods provided by NTT Data
**=========================================================================*/

#define MYKI_EPOCH_UTC_ELAPSED_SECONDS 1136034000
#if (MYKI_EPOCH_UTC_ELAPSED_SECONDS != TIME_T_BASE_FOR_2006)
#error with epoch
#endif

#if defined(WIN32)
#define localtime_r(a_tt, a_tm) \
    do { \
        struct tm *pTM = localtime(a_tt); \
        if (pTM) \
            *a_tm = *pTM; \
        else \
            memset(a_tm, 0, sizeof(struct tm)); \
    } while (0)
#endif

static long TimeTToDate24(time_t utc)
{
   time_t t;
   time_t t1;
   time_t t2;
   struct tm localReference; 
   struct tm vixtime;
   double diff;
   
   t = (time_t)MYKI_EPOCH_UTC_ELAPSED_SECONDS;
   
   localtime_r(&t, &localReference);
   localtime_r(&utc, &vixtime);
 
   t1 = mktime(&localReference);

   if(vixtime.tm_isdst == 0)
   {
      vixtime.tm_min -= 60;
   }

   t2 = mktime(&vixtime);

   diff = difftime(t2, t1);

   return (long)(diff/60);
}

static time_t Date24ToTimeT(long numMins)
{
   struct tm localReference;
   time_t t = (time_t)MYKI_EPOCH_UTC_ELAPSED_SECONDS;

   localtime_r(&t, &localReference);
   
   localReference.tm_min += numMins;
 
   t = mktime(&localReference);

   if(localReference.tm_isdst == 0)
   {
      localReference.tm_min += 60;
   }
   t = mktime(&localReference);

   return t;
}

