/*============================================================================
**
**    Vix Technology       Licensed software
**    (C) 2014             All rights reserved
**
**============================================================================
**
**  Project/Product : Device Application Framework
**  Filename        : myki_time_conversion.h
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

#ifndef _MYKI_TIME_CONVERSION_H_
#define _MYKI_TIME_CONVERSION_H_

#include <corebasetypes.h>

#define TIME_T_BASE_FOR_2006           1136034000 /* Number of time_t seconds at 1/1/2006 */

/*
** NOTE: Myki system is using technically incorrect 1136034000 (31/12/2005 13h00m00s UCT, 31/12/2005 21h00m00s WAST, 01/01/2006 00h00m00s EST)
** instead of 1136073600 (01/01/2006 00h00m00s UCT, 01/01/2006 08h00m00s WAST, 01/01/2006 11h00m00s EST)
*/

#ifdef __cplusplus
    extern "C" {
#endif

void DateMykiU24ToTimeT(const U32_t DateMykiU24, Time_t *pTimeT);
void TimeTToDateMykiU24(const Time_t TimeT, U32_t *pDateMykiU24);

#ifdef __cplusplus
    }
#endif
#endif
