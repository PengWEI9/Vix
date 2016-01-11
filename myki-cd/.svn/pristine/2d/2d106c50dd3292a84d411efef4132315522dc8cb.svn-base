/* -*- mode: c; tabs: 4; -*- */
/*=============================================================================
**
**    Vix Technology                   Licensed software
**    (C) 2014                         All rights reserved
**
**=============================================================================
**
**  Project/Product : MBU
**  Filename        : myki_cd.cpp
**  Author(s)       : James Ho
**
**  Description     :
**
**  Member(s)       :
**      MYKI_CD_openCDDatabase      [public]    Opens Tariff database
**      MYKI_CD_closeCDDatabase     [public]    Closes Tariff database
**      MYKI_CD_openDeltaActionlistDatabase
**                                  [public]    Opens DELTA Actionlist database
**      MYKI_CD_openFullActionlistDatabase
**                                  [public]    Opens FULL Actionlist database
**      MYKI_CD_closeDeltaActionlistDatabase
**                                  [public]    Closes DELTA Actionlist database
**      MYKI_CD_closeFullActionlistDatabase
**                                  [public]    Closes FULL Actionlist database
**
**  Information     :
**   Compiler(s)    : C++
**   Target(s)      : Independent
**
**  Subversion      :
**      $Id: $
**      $HeadURL: $
**
**  History         :
**   Vers.  Date        Aut.  Type     Description
**   -----  ----------  ----  -------  ----------------------------------------
**    1.00  10.10.12    JH    Create
**    1.01  03.03.15    ANT   Add      APIs to close databases
**
**===========================================================================*/

#include "myki_cd.h"
#include "DeviceParameters.h"
#include "DifferentialPrices.h"
#include "DifferentialPricesProvisional.h"
#include "ProductIssuers.h"
#include "PassengerTypes.h"
#include "Products.h"
#include "FaresStoredValue.h"
#include "FaresProvisional.h"
#include "FaresEpass.h"
#include "HMIMessaging.h"
#include "MasterRoutes.h"
#include "Routes.h"
#include "RouteSections.h"
#include "RouteStops.h"
#include "ServiceProviders.h"
#include "Stations.h"
#include "StationLines.h"
#include "Locations.h"
#include "Zones.h"

#include <ctype.h>
#include <time.h>
#include "datec20.h"

/*
 *      Local Variables
 *      ---------------
 */

static  std::string     g_DeviceType            = "FPDg";
static  U16_t           g_ServiceProviderId     = 0;

int MYKI_CD_openCDDatabase(const char *pFilename)
{
    if(getDBAccess(pFilename)!=NULL)
    {
        return true;
    }

    return false;
}

/*==========================================================================*
**
**  MYKI_CD_closeCDDatabase
**
**  Description     :
**      Closes current Tariff database.
**
**  Parameters      :
**      None
**
**  Returns         :
**      None
**
**  Notes           :
**
**==========================================================================*/

void    MYKI_CD_closeCDDatabase( void )
{
    releaseDBAccess( );
}   /*  MYKI_CD_closeCDDatabase( ) */

/*
int MYKI_CD_openFullActionlistFile(const char *filename)
{
	CsDebug( CD_DEBUG, ( CD_DEBUG, "MYKI_CD:MYKI_CD_openFullActionlistFile: Open full Actionlist DBAccess filename is %s", filename ) );
	return MYKI_CD_openFullActionlistDatabase();
}
*/

/*
int MYKI_CD_openActionlistDatabase()
{
    if(getActionlistDBAccess(false)!=NULL)
    {
		if(getActionlistDBAccess(true)!=NULL)
		{
			 return true;
		}
	}

    return false;
}
*/


/*
int MYKI_CD_openDeltaActionlistFile(const char *filename)
{
	CsDebug( CD_DEBUG, ( CD_DEBUG, "MYKI_CD:MYKI_CD_openDeltaActionlistFile: Open delta Actionlist DBAccess filename is %s", filename ) );
	setDeltaActionListFilename(filename);
	return MYKI_CD_openDeltaActionlistDatabase();
}
*/

int MYKI_CD_openDeltaActionlistDatabase(const char *filename)
{
    if(getActionlistDBAccess(true, filename)!=NULL)
    {
         return true;
    }

    return false;
}
int MYKI_CD_openFullActionlistDatabase(const char *filename)
{
    if(getActionlistDBAccess(false, filename)!=NULL)
    {
         return true;
    }

    return false;
}

/*==========================================================================*
**
**  MYKI_CD_closeDeltaActionlistDatabase
**  MYKI_CD_closeFullActionlistDatabase
**
**  Description     :
**      Closes current DELTA/FULL Actionlist database.
**
**  Parameters      :
**      None
**
**  Returns         :
**      None
**
**  Notes           :
**
**==========================================================================*/

void    MYKI_CD_closeDeltaActionlistDatabase( void )
{
    releaseActionlistDBAccess( true );
}   /*  MYKI_CD_closeDeltaActionlistDatabase( ) */

void    MYKI_CD_closeFullActionlistDatabase( void )
{
    releaseActionlistDBAccess( false );
}   /*  MYKI_CD_closeFullActionlistDatabase( ) */

int MYKI_CD_setDeviceParameters(const char* deviceType, MYKI_CD_Mode_t mode, U8_t zone, U16_t locationId, U8_t passengerType, U16_t serviceProvider, U8_t blockingReason, U8_t paymentMethod)
{
    if ( deviceType      != NULL ) { g_DeviceType         = deviceType;      }
    if ( serviceProvider != 0    ) { g_ServiceProviderId  = serviceProvider; }

	return setDeviceParameters(deviceType, mode, zone, locationId, passengerType, serviceProvider, blockingReason, paymentMethod);
}

int MYKI_CD_useCurrentVersion(Time_t currentTime)
{
    return initCurrentCDVersion(currentTime);
}

Time_t MYKI_CD_getStartDateTime()
{
    return getCurrentCDStartTime();
}

DateC20_t MYKI_CD_getStartDate()
{
    time_t x = (time_t)getCurrentCDStartTime();
    return c20_time2date(&x);
}

int MYKI_CD_validateTariff()
{
    return validateTariff();
}

int MYKI_CD_getMajorVersion()
{
	return getCurrentCDMajorVersion();
}

int MYKI_CD_getMinorVersion()
{
	return getCurrentCDMinorVersion();
}

const char* MYKI_CD_getSource()
{
	return getCurrentCDSource();
}


int MYKI_CD_getBoolean(MYKI_CD_DeviceParameter_t* deviceParameter, U8_t* value)
{
    if(value!=NULL && deviceParameter!=NULL)
    {
        *value = 0;
        if(deviceParameter->value[0]!='\0' && deviceParameter->value[0]!=' ')
        {
            if(toupper(deviceParameter->value[0])=='T' || toupper(deviceParameter->value[0])=='Y' ||  deviceParameter->value[0]=='1')
            {
                *value = 1;
            }
            return true;
        }
        else if(deviceParameter->default_value[0]!='\0' && deviceParameter->default_value[0]!=' ')
        {
            if(toupper(deviceParameter->default_value[0])=='T' || toupper(deviceParameter->default_value[0])=='Y' ||  deviceParameter->default_value[0]=='1')
            {
                *value = 1;
            }        
            return true;
        }
    }
    return false;
}

int MYKI_CD_getS32(MYKI_CD_DeviceParameter_t* deviceParameter, S32_t* value)
{
    if(value!=NULL && deviceParameter!=NULL)
    {
        *value = 0;
        if(deviceParameter->value[0] && deviceParameter->value[0]!=' ')
        {
            if(sscanf(deviceParameter->value,"%d", value)>0)
                return true;
        }
        else if(deviceParameter->default_value[0] && deviceParameter->default_value[0]!=' ')
        {
            if(sscanf(deviceParameter->default_value,"%d", value)>0) 
                return true;
        }
    }
    return false;
}

int MYKI_CD_getU32(MYKI_CD_DeviceParameter_t* deviceParameter, U32_t* value)
{
    if(value!=NULL && deviceParameter!=NULL)
    {
        *value = 0;
        if(deviceParameter->value[0] && deviceParameter->value[0]!=' ')
        {
            if(sscanf(deviceParameter->value,"%u", value)>0)
                return true;
        }
        else if(deviceParameter->default_value[0] && deviceParameter->default_value[0]!=' ')
        {
            if(sscanf(deviceParameter->default_value,"%u", value)>0) 
                return true;
        }
    }
    return false;
}

    /**
     *  Extracts and returns device parameter value as U8_t.
     *  @param  deviceParameter device parameter object.
     *  @param  value returned U8_t value buffer.
     *  @return true if successful; false otherwise.
     */
int
MYKI_CD_getU8( MYKI_CD_DeviceParameter_t* deviceParameter, U8_t* value )
{
    if ( value != NULL && deviceParameter != NULL )
    {
        U32_t   tempValue   = 0;
        *value              = 0;
        if ( deviceParameter->value[ 0 ] && deviceParameter->value[ 0 ] != ' ' )
        {
            if ( sscanf( deviceParameter->value, "%u", &tempValue ) > 0 )
            {
                *value  = (U8_t)tempValue;
                return true;
            }
        }
        else if ( deviceParameter->default_value[ 0 ] && deviceParameter->default_value[ 0 ] != ' ' )
        {
            if ( sscanf( deviceParameter->default_value, "%u", &tempValue ) > 0 )
            {
                *value  = (U8_t)tempValue;
                return true;
            }
        }
    }
    return false;
}


int MYKI_CD_getU16(MYKI_CD_DeviceParameter_t* deviceParameter, U16_t* value)
{
    if(value!=NULL && deviceParameter!=NULL)
    {
        unsigned int tempValue = 0;
        *value = 0;
        if(deviceParameter->value[0] && deviceParameter->value[0]!=' ')
        {
            if(sscanf(deviceParameter->value,"%u", &tempValue)>0)
            {
                *value = (U16_t)tempValue;
                return true;
            }
        }
        else if(deviceParameter->default_value[0] && deviceParameter->default_value[0]!=' ')
        {
            if(sscanf(deviceParameter->default_value,"%u", &tempValue)>0)
            {
                *value = (U16_t)tempValue;
                return true;
            }
        }
    }
    return false;
}

int MYKI_CD_getString(MYKI_CD_DeviceParameter_t* deviceParameter, char* value, int len)
{
    if(value!=NULL && deviceParameter!=NULL && len>0)
    {
        if(deviceParameter->value[0]=='\0')
        {
            len = (int)snprintf(value, len, "%s", deviceParameter->default_value);
            return len>=0;            
        }
        else
        {
            len = (int)snprintf(value, len, "%s", deviceParameter->value);
            return len>=0;
        }
    }
    return false;
}


int MYKI_CD_getDeviceParameterStructure(const char * deviceParameterName, MYKI_CD_DeviceParameter_t* deviceParameter)
{
    return getDeviceParameter(deviceParameterName, *deviceParameter);
}

int MYKI_CD_getPaymentMethodDeviceParameterStructure(const char* deviceType, U16_t serviceProvider, const char *paymentMethod, MYKI_CD_DeviceParameter_t *deviceParameter)
{
    return getPaymentMethod(deviceType, serviceProvider, paymentMethod, *deviceParameter);
}

int MYKI_CD_getServiceProviderDeviceParameterStructure(U16_t serviceProviderId, const char *deviceParameterName,  MYKI_CD_DeviceParameter_t *deviceParameter)
{
    return getDeviceParameterBySP(deviceParameterName, serviceProviderId, *deviceParameter);
}

int MYKI_CD_getServiceProviderAndModeDeviceParameterStructure(U16_t serviceProviderId, MYKI_CD_Mode_t mode, const char *deviceParameterName,  MYKI_CD_DeviceParameter_t *deviceParameter)
{
    return getDeviceParameterBySPandMode(deviceParameterName, serviceProviderId, mode, *deviceParameter);
}

/*==========================================================================*
**
**  MYKI_CD_getMinimumBalanceePass
**
**  Description     :
**   The minimum balance required on the smartcard prior to allowing travel when the patron has
**   an active e-Pass product valid for the current zone.
**
**  Parameters      :
**      value           [IO]     pointer to a return value
**
**  Returns         :
**      TRUE                        value successfully retrieved 
**      FALSE                       error
**
**==========================================================================*/
int MYKI_CD_getMinimumBalanceePass(S32_t* value)
{
    MYKI_CD_DeviceParameter_t deviceParameter;
    int returnValue = MYKI_CD_getDeviceParameterStructure("MinimumBalanceePass", &deviceParameter);
    if(returnValue)
    {
        returnValue = MYKI_CD_getS32(&deviceParameter, value);
    }
    return returnValue;
}

/*==========================================================================*
**
**  MYKI_CD_getMinimumBalanceStoredValue
**
**  Description     :
**   The minimum balance required on the smartcard prior to allowing travel when the patron has
**    no active e-Pass product valid for the current zone
**
**  Parameters      :
**      value           [IO]     pointer to a return value
**
**  Returns         :
**      TRUE                        value successfully retrieved 
**      FALSE                       error
**
**==========================================================================*/
int MYKI_CD_getMinimumBalanceStoredValue(S32_t* value)
{
    MYKI_CD_DeviceParameter_t deviceParameter;
    int returnValue = MYKI_CD_getDeviceParameterStructure("MinimumBalanceStoredValue", &deviceParameter);
    if(returnValue)
    {
        returnValue = MYKI_CD_getS32(&deviceParameter, value);
    }
    return returnValue;    
}

/*==========================================================================*
**
**  MYKI_CD_getProvisionalFareMode
**
**  Description     :
**   Configures the way the system manages daily and weekly capping with provisional fares in the
**    event of forced scan-off.
**
**   Possible values are:
**   ‘1’: The provisional fare is charged, and the provisional fare is added to the daily and weekly
**    capping fare totals.
**   ‘2’: The provisional fare is charged, and the standard (actual) fare is added to the daily and
**    weekly capping fare totals (this option will be used for n-Hour capping, separate to the selected
**    option for daily and weekly capping).
**   '3’: The provisional fare is always charged and no fare is added to the daily and weekly capping
**    fare totals. This option does *NOT* check for any other products in card at the time of scan-on
**    to merge and deduce fares.
**
**  Parameters      :
**      value           [IO]     pointer to a return value
**
**  Returns         :
**      TRUE                        value successfully retrieved 
**      FALSE                       error
**
**==========================================================================*/
int MYKI_CD_getProvisionalFareMode(U16_t* value)
{
    MYKI_CD_DeviceParameter_t deviceParameter;
    int returnValue = MYKI_CD_getDeviceParameterStructure("ProvisionalFareMode", &deviceParameter);
    if(returnValue)
    {
        returnValue = MYKI_CD_getU16(&deviceParameter, value);
    }
    return returnValue;         
}

/*==========================================================================*
**
**  MYKI_CD_getEndOfTransportDay
**
**  Description     :
**   This attribute sets the time of day in military format that is considered the end of the NTS
**   transit business day. This affects product expiry where the product is considered valid up until
**   the end of a particular business day.
**   Note: This value is always interpreted as being for the day following the current calendar date.
**   Therefore this value may not be effectively used to interpret the end of the current transport
**   business day as being less than 0000 hours (midnight).
**   For example, a value of “2200” would be interpreted as being 10:00PM of the following day.
**
**  Parameters      :
**      value           [IO]     pointer to a return value
**
**  Returns         :
**      TRUE                        value successfully retrieved 
**      FALSE                       error
**
**==========================================================================*/
int MYKI_CD_getEndOfTransportDay(U16_t* value)
{   
    MYKI_CD_DeviceParameter_t deviceParameter;
    int returnValue = MYKI_CD_getDeviceParameterStructure("EndOfTransportDay", &deviceParameter);

    if(returnValue)
    {
        returnValue = MYKI_CD_getU16(&deviceParameter, value);
        if(returnValue)
            *value = (((*value) / 100) * 60) + ((*value) % 100); // database has e.g. 300 = 3:00am
    }
    return returnValue;        
}

/*==========================================================================*
**
**  MYKI_CD_getBlockingPeriodDSC
**
**  Description     :
**   This defines the number of seconds in which a disposable smartcard presented to any
**    validation device twice will be rejected to prevent accidental re-presentation.
**    (See section 2.11 of NTS0124 – Fare Processing Rules – Part A for details)
**    Note: Where this value is set to zero (0), no blocking period rule applies.
**
**  Parameters      :
**      value           [IO]     pointer to a return value
**
**  Returns         :
**      TRUE                        value successfully retrieved 
**      FALSE                       error
**
**==========================================================================*/
int MYKI_CD_getBlockingPeriodDSC(S32_t *value)
{
    MYKI_CD_DeviceParameter_t deviceParameter;
    int returnValue = MYKI_CD_getDeviceParameterStructure("BlockingPeriodDSC", &deviceParameter);
    if(returnValue)
    {
        returnValue = MYKI_CD_getS32(&deviceParameter, value);
    }
    return returnValue;    
}

/*==========================================================================*
**
**  MYKI_CD_getBlockingPeriodLLSC
**
**  Description     :
**   This defines the number of seconds in which a long-life smartcard presented to any validation
**    device twice will be rejected to prevent accidental re-presentation.
**    (See section 2.11 of NTS0124 – Fare Processing Rules – Part A for details)
**    Note: Where this value is set to zero (0), no blocking period rule applies.
**
**  Parameters      :
**      value           [IO]     pointer to a return value
**
**  Returns         :
**      TRUE                        value successfully retrieved 
**      FALSE                       error
**
**==========================================================================*/
int MYKI_CD_getBlockingPeriodLLSC(S32_t *value)
{
    MYKI_CD_DeviceParameter_t deviceParameter;
    int returnValue = MYKI_CD_getDeviceParameterStructure("BlockingPeriodLLSC", &deviceParameter);
    if(returnValue)
    {
        returnValue = MYKI_CD_getS32(&deviceParameter, value);
    }
    return returnValue;        
}

/*==========================================================================*
**
**  MYKI_CD_getChangeOfMindDSC
**
**  Description     :
**   This defines the number of seconds in which a disposable smartcard presented to any
**    validation device twice will have an existing scan-on cancelled.
**    (See section 2.11 of NTS0124 – Fare Processing Rules – Part A for details)
**    Note: Where this value is set to zero (0), no change of mind period rule applies.
**
**  Parameters      :
**      value           [IO]     pointer to a return value
**
**  Returns         :
**      TRUE                        value successfully retrieved 
**      FALSE                       error
**
**==========================================================================*/
int MYKI_CD_getChangeOfMindDSC(S32_t* value)
{
    MYKI_CD_DeviceParameter_t deviceParameter;
    int returnValue = MYKI_CD_getDeviceParameterStructure("ChangeOfMindDSC", &deviceParameter);
    if(returnValue)
    {
        returnValue = MYKI_CD_getS32(&deviceParameter, value);
    }
    return returnValue;       
}

/*==========================================================================*
**
**  MYKI_CD_getChangeOfMindLLSC
**
**  Description     :
**   This defines the number of seconds in which a long life smartcard presented to any validation
**    device twice will have an existing scan-on cancelled.
**    (See section 2.11 of NTS0124 – Fare Processing Rules – Part A for details)
**    Note: Where this value is set to zero (0), no change of mind period rule applies.
**
**  Parameters      :
**      value           [IO]     pointer to a return value
**
**  Returns         :
**      TRUE                        value successfully retrieved 
**      FALSE                       error
**
**==========================================================================*/
int MYKI_CD_getChangeOfMindLLSC(S32_t* value)
{
    MYKI_CD_DeviceParameter_t deviceParameter;
    int returnValue = MYKI_CD_getDeviceParameterStructure("ChangeOfMindLLSC", &deviceParameter);
    if(returnValue)
    {
        returnValue = MYKI_CD_getS32(&deviceParameter, value);
    }
    return returnValue;       
    
}

/*==========================================================================*
**
**  MYKI_CD_getStationExitFee
**
**  Description     :
**   This defines the penalty fee that will be charged when a patron leaves a rail station paid area
**    without having scanned-on.
**    (See section 2.11 of NTS0124 – Fare Processing Rules – Part A for details)
**    Note: Where this value is set to zero (0), no station exit fee applies.
**
**  Parameters      :
**      value           [IO]     pointer to a return value
**
**  Returns         :
**      TRUE                        value successfully retrieved 
**      FALSE                       error
**
**==========================================================================*/
int MYKI_CD_getStationExitFee(S32_t* value)
{
    MYKI_CD_DeviceParameter_t deviceParameter;
    int returnValue = MYKI_CD_getDeviceParameterStructure("StationExitFee", &deviceParameter);
    if(returnValue)
    {
        returnValue = MYKI_CD_getS32(&deviceParameter, value);
    }
    return returnValue;     
}

/*==========================================================================*
**
**  MYKI_CD_getePassOutOfZone
**
**  Description     :
**   This defines whether to allow or disallow out-of-zone travel for e-Pass holders.
**   (See section 2.14.2 of NTS0124 – Fare Processing Rules – Part A for details)
**
**  Parameters      :
**      value           [IO]     pointer to a return value
**
**  Returns         :
**      TRUE                        value successfully retrieved 
**      FALSE                       error
**
**==========================================================================*/
int MYKI_CD_getePassOutOfZone(U8_t *value)
{
    MYKI_CD_DeviceParameter_t deviceParameter;
    int returnValue = MYKI_CD_getDeviceParameterStructure("ePassOutOfZone", &deviceParameter);
    if(returnValue)
    {
        returnValue = MYKI_CD_getBoolean(&deviceParameter, value);
    }
    return returnValue;  
}

/*==========================================================================*
**
**  MYKI_CD_getnHourPeriodMinutes
**
**  Description     :
**   This attribute sets the minimum number of hours an n-Hour product will be valid for.(See
**    NTS0124 Fare Processing Rules 2.4 for details)
**
**  Parameters      :
**      value           [IO]     pointer to the result value
**
**  Returns         :
**      TRUE                        value successfully retrieved 
**      FALSE                       error
**
**==========================================================================*/
int MYKI_CD_getnHourPeriodMinutes(U16_t* value)
{
    MYKI_CD_DeviceParameter_t deviceParameter;
    int returnValue = MYKI_CD_getDeviceParameterStructure("nHourPeriodMinutes", &deviceParameter);
    if(returnValue)
    {
        returnValue = MYKI_CD_getU16(&deviceParameter, value);
    }
    else
    {
        returnValue = MYKI_CD_getDeviceParameterStructure("nHourPeriod", &deviceParameter);
        if(returnValue)
        {
            returnValue = MYKI_CD_getU16(&deviceParameter, value);
            *value *= 60;
        }
    }
    return returnValue;       
}

/*==========================================================================*
**
**  MYKI_CD_getnHourEveningCutoff
**
**  Description     :
**   This attribute sets the time of day in military format for the n-Hour evening cuttoff value. Any n-
**    Hour product issued after this time will expire at the end of the public transport business day as
**    defined by the global TransportBusinessDay device attribute.(See NTS0124 Fare Processing
**    Rules 2.4 for further details on the usage of this field.)
**
**  Parameters      :
**      value           [IO]     pointer to the result value
**
**  Returns         :
**      TRUE                        value successfully retrieved 
**      FALSE                       error
**
**==========================================================================*/
int MYKI_CD_getnHourEveningCutoff(U16_t* value)
{
    MYKI_CD_DeviceParameter_t deviceParameter;
    int returnValue = MYKI_CD_getDeviceParameterStructure("nHourEveningCutoff", &deviceParameter);
    if(returnValue)
    {
        returnValue = MYKI_CD_getU16(&deviceParameter, value);
    }
    return returnValue;       
}

/*==========================================================================*
**
**  MYKI_CD_getnHourExtendPeriodMinutes
**
**  Description     :
**   **   Obsolete
**   This attribute sets the number of hours an n-Hour product will be extended by when the ‘Travel
**   Extension’ business rule applies.(See NTS0124 Fare Processing Rules 2.15.5 for details on the
**   usage of this field.) NB: This field was made obsolete with version 8.1 of the document.
**
**  Parameters      :
**      value           [IO]     pointer to the result value
**
**  Returns         :
**      TRUE                        value successfully retrieved 
**      FALSE                       error
**
**==========================================================================*/
int MYKI_CD_getnHourExtendPeriodMinutes(U16_t* value)
{
    MYKI_CD_DeviceParameter_t deviceParameter;
    int returnValue = MYKI_CD_getDeviceParameterStructure("nHourExtendPeriodMinutes", &deviceParameter);
    if(returnValue)
    {
        returnValue = MYKI_CD_getU16(&deviceParameter, value);
    }
    else
    {
        returnValue = MYKI_CD_getDeviceParameterStructure("nHourExtendPeriod", &deviceParameter);
        if(returnValue)
        {
            returnValue = MYKI_CD_getU16(&deviceParameter, value);
            *value *= 60;
        }
    }
    return returnValue;       
}

/*==========================================================================*
**
**  MYKI_CD_getnHourExtendThreshold
**
**  Description     :
**   This attribute sets the minimum number of zones that must be travelled in a single trip for the
**    ‘Travel Extension’ business rule to apply.(See NTS0124 Fare Processing Rules 2.15.5 for details
**    on the usage of this field.)
**
**  Parameters      :
**      value           [IO]     pointer to the result value
**
**  Returns         :
**      TRUE                        value successfully retrieved 
**      FALSE                       error
**
**==========================================================================*/
int MYKI_CD_getnHourExtendThreshold(U16_t* value)
{
    MYKI_CD_DeviceParameter_t deviceParameter;
    int returnValue = MYKI_CD_getDeviceParameterStructure("nHourExtendThreshold", &deviceParameter);
    if(returnValue)
    {
        returnValue = MYKI_CD_getU16(&deviceParameter, value);
    }
    return returnValue;       

}

/*==========================================================================*
**
**  MYKI_CD_getnHourMaximumDuration
**
**  Description     :
**   Represents the maximum time (hours) that a n-hour product can be extended for.
**    This time does include the n-hour base period.
**
**  Parameters      :
**      value           [IO]     pointer to the result value
**
**  Returns         :
**      TRUE                        value successfully retrieved 
**      FALSE                       error
**
**==========================================================================*/
int MYKI_CD_getnHourMaximumDuration(U16_t* value)
{
    MYKI_CD_DeviceParameter_t deviceParameter;
    int returnValue = MYKI_CD_getDeviceParameterStructure("nHourMaximumDuration", &deviceParameter);
    if(returnValue)
    {
        returnValue = MYKI_CD_getU16(&deviceParameter, value);
    }
    return returnValue;       
}

/*==========================================================================*
**
**  MYKI_CD_getnHourRoundingPortion
**
**  Description     :
**   The n-Hour rounding portion is used in the rounding formula used in the calculation of the n-hour
**    product expiry. It rounds up the current time to the configured unit value. When the value is 60 –
**    the hour is rounded up to the next full hour, when 1 it rounds up to the next full minute.
**    The full implementation of the formula is specified in the NTS Sales rules framework. The standard
**    formula is:
**    Calculated expiry time: (CurrentTime:Hour + nHourRoundingPortion
**    +(Floor(CurrentTime:minute/ nHourRoundingPortion)* nHourRoundingPortion))+ nHourPeriod
**    (hours)
**    See NTS0150 Sales Rules Framework, n-Hour product definition for further information.
**
**  Parameters      :
**      value           [IO]     pointer to the result value
**
**  Returns         :
**      TRUE                        value successfully retrieved 
**      FALSE                       error
**
**==========================================================================*/
int MYKI_CD_getnHourRoundingPortion(U16_t* value)
{
    MYKI_CD_DeviceParameter_t deviceParameter;
    int returnValue = MYKI_CD_getDeviceParameterStructure("nHourRoundingPortion", &deviceParameter);
    if(returnValue)
    {
        returnValue = MYKI_CD_getU16(&deviceParameter, value);
    }
    else
    {
        returnValue = true;
        *value = 60;
    }
    return returnValue;       
}

/*==========================================================================*
**
**  MYKI_CD_getHeadlessModeRoute
**
**  Description     :
**   Where a validation device has been configured to be in the special transition ‘Headless’ mode, this
**    parameter specifies which Route is to be used.
**    NOTE: A headless mode route must be defined for all operators that will use the functionality as
**    routes are bound to particular service providers, therefore the default value cannot be used.
**
**  Parameters      :
**      serviceProvider  [i]     service provider to be used in query
**      value           [IO]     pointer to the result value
**
**  Returns         :
**      TRUE                        value successfully retrieved 
**      FALSE                       error
**
**==========================================================================*/
int MYKI_CD_getHeadlessModeRoute(U16_t serviceProvider, U16_t* value)
{
    MYKI_CD_DeviceParameter_t deviceParameter;
    int returnValue = MYKI_CD_getServiceProviderDeviceParameterStructure(serviceProvider, "HeadlessModeRoute", &deviceParameter);
    if(returnValue)
    {
        returnValue = MYKI_CD_getU16(&deviceParameter, value);
    }
    return returnValue;       
}

/*==========================================================================*
**
**  MYKI_CD_getePassLowWarningThreshold
**
**  Description     :
**   This attribute defines when a validation device should start indicating to the patron that the ePass
**    is due to expiry soon. The value indicates the number of days that the ePass expiry must be less
**    than in order for the “low-warning” alert to be shown to the patron.
**    Note: This affects every ePass product, regardless of the original number of days it was purchased
**    for.
**
**  Parameters      :
**      value           [IO]     pointer to the result value
**
**  Returns         :
**      TRUE                        value successfully retrieved 
**      FALSE                       error
**
**==========================================================================*/
int MYKI_CD_getePassLowWarningThreshold(U16_t* value)
{
    MYKI_CD_DeviceParameter_t deviceParameter;
    int returnValue = MYKI_CD_getDeviceParameterStructure("ePassLowWarningThreshold", &deviceParameter);
    if(returnValue)
    {
        returnValue = MYKI_CD_getU16(&deviceParameter, value);
    }    
    return returnValue;      
}

/*==========================================================================*
**
**  MYKI_CD_getTPurseLowWarningThreshold
**
**  Description     :
**   This attribute defines when a validation device should start indicating to the patron that the TPurse
**    balance is getting low. The value indicates the number of cents that the T-Purse balance
**    must be less than in order for the “low-warning” alert to be shown to the patron.
**
**  Parameters      :
**      value           [IO]     pointer to the result value
**
**  Returns         :
**      TRUE                        value successfully retrieved 
**      FALSE                       error
**
**==========================================================================*/
int MYKI_CD_getTPurseLowWarningThreshold(U16_t* value)
{
    MYKI_CD_DeviceParameter_t deviceParameter;
    int returnValue = MYKI_CD_getDeviceParameterStructure("TPurseLowWarningThreshold", &deviceParameter);
    if(returnValue)
    {
        returnValue = MYKI_CD_getU16(&deviceParameter, value);
    }    
    return returnValue;      
}

/*==========================================================================*
**
**  MYKI_CD_getAddValueEnabled
**
**  Description     :
**   Defines whether a specific sales channel may provide add value services
**
**  Parameters      :
**      value           [IO]     pointer to the result value
**
**  Returns         :
**      TRUE                        value successfully retrieved 
**      FALSE                       error
**
**==========================================================================*/
int MYKI_CD_getAddValueEnabled(U8_t* value)
{
    MYKI_CD_DeviceParameter_t deviceParameter;
    int returnValue = MYKI_CD_getDeviceParameterStructure("AddValueEnabled", &deviceParameter);
    if(returnValue)
    {
        returnValue = MYKI_CD_getBoolean(&deviceParameter, value);
    }
    return returnValue;
}

/*==========================================================================*
**
**  MYKI_CD_getePassMaximumDay
**
**  Description     :
**   When freely selecting a variable number of days for the purchase of an e-Pass product, this is the
**    maximum number of days selectable.
**
**  Parameters      :
**      value           [IO]     pointer to the result value
**
**  Returns         :
**      TRUE                        value successfully retrieved 
**      FALSE                       error
**
**==========================================================================*/
int MYKI_CD_getePassMaximumDay(U16_t* value)
{
    MYKI_CD_DeviceParameter_t deviceParameter;
    int returnValue = MYKI_CD_getDeviceParameterStructure("ePassMaximumDay", &deviceParameter);
    if(returnValue)
    {
        returnValue = MYKI_CD_getU16(&deviceParameter, value);
    }
    return returnValue;           
}

/*==========================================================================*
**
**  MYKI_CD_getePassMinimumDay
**
**  Description     :
**   When freely selecting a variable number of days for the purchase of an e-Pass product, this is the
**    minimum number of days selectable.
**
**  Parameters      :
**      value           [IO]     pointer to the result value
**
**  Returns         :
**      TRUE                        value successfully retrieved 
**      FALSE                       error
**
**==========================================================================*/
int MYKI_CD_getePassMinimumDay(U16_t* value)
{
   MYKI_CD_DeviceParameter_t deviceParameter;
    int returnValue = MYKI_CD_getDeviceParameterStructure("ePassMinimumDay", &deviceParameter);
    if(returnValue)
    {
        returnValue = MYKI_CD_getU16(&deviceParameter, value);
    }
    return returnValue;       
}

/*==========================================================================*
**
**  MYKI_CD_getePassSelectableDays
**
**  Description     :
**   An array of preset number of days for the purchase of an e-Pass product.
**    This is represented as a comma-delimited list of numeric values.
**    Note: This is intended for quick-selection of e-Pass days and does not reflect the only possible day
**    ranges available for e-Pass sales.
**
**  Parameters      :
**      value           [IO]     pointer to the result value
**
**  Returns         :
**      TRUE                        value successfully retrieved 
**      FALSE                       error
**
**==========================================================================*/
int MYKI_CD_getePassSelectableDays(U16_t *array, U8_t * len)
{
    // REVIEW: Is this API required
    return FALSE;
}

/*==========================================================================*
**
**  MYKI_CD_getePassTemporaryDays
**
**  Description     :
**   The number of days for which to issue a temporary e-Pass product on disposable, for patrons
**    surrendering their long-life smartcard for replacement.
**
**  Parameters      :
**      value           [IO]     pointer to the result value
**
**  Returns         :
**      TRUE                        value successfully retrieved 
**      FALSE                       error
**
**==========================================================================*/
int MYKI_CD_getePassTemporaryDays(U16_t* value)
{
    MYKI_CD_DeviceParameter_t deviceParameter;
    int returnValue = MYKI_CD_getDeviceParameterStructure("ePassTemporaryDays", &deviceParameter);
    if(returnValue)
    {
        returnValue = MYKI_CD_getU16(&deviceParameter, value);
    }
    return returnValue;     
}

/*==========================================================================*
**
**  MYKI_CD_getGSTPercentage
**
**  Description     :
**   The GST percentage value for all products.
**
**  Parameters      :
**      value           [IO]     pointer to the result value
**
**  Returns         :
**      TRUE                        value successfully retrieved 
**      FALSE                       error
**
**==========================================================================*/
int MYKI_CD_getGSTPercentage(U8_t* value)
{
    MYKI_CD_DeviceParameter_t deviceParameter;
    int returnValue = MYKI_CD_getDeviceParameterStructure("GSTPercentage", &deviceParameter);
    if(returnValue)
    {
        returnValue = MYKI_CD_getU8(&deviceParameter, value);
    }
    return returnValue;         
}

/*==========================================================================*
**
**  MYKI_CD_getLLSCCancellationFee
**
**  Description     :
**   The fee for cancellation of a long-life smartcard.
**
**  Parameters      :
**      value           [IO]     pointer to the result value
**
**  Returns         :
**      TRUE                        value successfully retrieved 
**      FALSE                       error
**
**==========================================================================*/
int MYKI_CD_getLLSCCancellationFee(U32_t* value)
{
    MYKI_CD_DeviceParameter_t deviceParameter;
    int returnValue = MYKI_CD_getDeviceParameterStructure("LLSCCancellationFee", &deviceParameter);
    if(returnValue)
    {
        returnValue = MYKI_CD_getU32(&deviceParameter, value);
    }
    return returnValue;         
}

/*==========================================================================*
**
**  MYKI_CD_getLLSCDeposit
**
**  Description     :
**   The deposit required for the purchase of a long-life smartcard.
**
**  Parameters      :
**      value           [IO]     pointer to the result value
**
**  Returns         :
**      TRUE                        value successfully retrieved 
**      FALSE                       error
**
**==========================================================================*/
int MYKI_CD_getLLSCDeposit(U32_t* value)
{
    MYKI_CD_DeviceParameter_t deviceParameter;
    int returnValue = MYKI_CD_getDeviceParameterStructure("LLSCDeposit", &deviceParameter);
    if(returnValue)
    {
        returnValue = MYKI_CD_getU32(&deviceParameter, value);
    }
    return returnValue;     
}

/*==========================================================================*
**
**  MYKI_CD_getLLSCPersonalisationFee
**
**  Description     :
**   The fee for personalization of a long-life smartcard.
**
**  Parameters      :
**      value           [IO]     pointer to the result value
**
**  Returns         :
**      TRUE                        value successfully retrieved 
**      FALSE                       error
**
**==========================================================================*/
int MYKI_CD_getLLSCPersonalisationFee(U32_t* value)
{
    MYKI_CD_DeviceParameter_t deviceParameter;
    int returnValue = MYKI_CD_getDeviceParameterStructure("LLSCPersonalisationFee", &deviceParameter);
    if(returnValue)
    {
        returnValue = MYKI_CD_getU32(&deviceParameter, value);
    }
    return returnValue;     
}

/*==========================================================================*
**
**  MYKI_CD_getLLSCRegistrationFee
**
**  Description     :
**   The fee for registration of a long-life smartcard.
**
**  Parameters      :
**      value           [IO]     pointer to the result value
**
**  Returns         :
**      TRUE                        value successfully retrieved 
**      FALSE                       error
**
**==========================================================================*/
int MYKI_CD_getLLSCRegistrationFee(U32_t* value)
{
    MYKI_CD_DeviceParameter_t deviceParameter;
    int returnValue = MYKI_CD_getDeviceParameterStructure("LLSCRegistrationFee", &deviceParameter);
    if(returnValue)
    {
        returnValue = MYKI_CD_getU32(&deviceParameter, value);
    }
    return returnValue;     
}

/*==========================================================================*
**
**  MYKI_CD_getLLSCReplacementFee
**
**  Description     :
**   The fee for replacement of a long-life smartcard.
**   Note: This parameter is valid up until and including PR7. For PR8 and beyond a new replacement
**   structure will be in place. See Replacement [sic]
**
**  Parameters      :
**      value           [IO]     pointer to the result value
**
**  Returns         :
**      TRUE                        value successfully retrieved 
**      FALSE                       error
**
**==========================================================================*/
int MYKI_CD_getLLSCReplacementFee(U32_t* value)
{
    MYKI_CD_DeviceParameter_t deviceParameter;
    int returnValue = MYKI_CD_getDeviceParameterStructure("LLSCReplacementFee", &deviceParameter);
    if(returnValue)
    {
        returnValue = MYKI_CD_getU32(&deviceParameter, value);
    }
    return returnValue;    
}

/*==========================================================================*
**
**  MYKI_CD_getLLSCExpiryWindow
**
**  Description     :
**   The number of Days a LLSC replacement can be executed under the category of Expired; prior to
**    the expiry of the LLSC Card or Transit application
**   Note: 0 – indicates replacement is not permitted prior to card or transit application expiry.
**
**  Parameters      :
**      value           [IO]     pointer to the result value
**
**  Returns         :
**      TRUE                        value successfully retrieved 
**      FALSE                       error
**
**==========================================================================*/
int MYKI_CD_getLLSCExpiryWindow(U16_t* value)
{
    MYKI_CD_DeviceParameter_t deviceParameter;
    int returnValue = MYKI_CD_getDeviceParameterStructure("LLSCExpiryWindow", &deviceParameter);
    if(returnValue)
    {
        returnValue = MYKI_CD_getU16(&deviceParameter, value);
    }
    return returnValue;    
}

/*==========================================================================*
**
**  MYKI_CD_getLLSCSaleFee
**
**  Description     :
**   The sales fee for the purchase of a long-life smartcard.
**
**  Parameters      :
**      value           [IO]     pointer to the result value
**
**  Returns         :
**      TRUE                        value successfully retrieved 
**      FALSE                       error
**
**==========================================================================*/
int MYKI_CD_getLLSCSaleFee(U32_t* value)
{
    MYKI_CD_DeviceParameter_t deviceParameter;
    int returnValue = MYKI_CD_getDeviceParameterStructure("LLSCSaleFee", &deviceParameter);
    if(returnValue)
    {
        returnValue = MYKI_CD_getU32(&deviceParameter, value);
    }
    return returnValue;        
}

/*==========================================================================*
**
**  MYKI_CD_getMaximumAddValue
**
**  Description     :
**   The maximum amount that may be loaded onto the T-Purse in a single transaction.
**
**  Parameters      :
**      value           [IO]     pointer to the result value
**
**  Returns         :
**      TRUE                        value successfully retrieved 
**      FALSE                       error
**
**==========================================================================*/
int MYKI_CD_getMaximumAddValue(U32_t* value)
{
    MYKI_CD_DeviceParameter_t deviceParameter;
    int returnValue = MYKI_CD_getDeviceParameterStructure("MaximumAddValue", &deviceParameter);
    if(returnValue)
    {
        returnValue = MYKI_CD_getU32(&deviceParameter, value);
    }
    return returnValue;        
}

/*==========================================================================*
**
**  MYKI_CD_getMaximumChange
**
**  Description     :
**   The maximum change to be provided at sales devices. This may only be modified in 10 cent
**    increments.
**
**  Parameters      :
**      value           [IO]     pointer to the result value
**
**  Returns         :
**      TRUE                        value successfully retrieved 
**      FALSE                       error
**
**==========================================================================*/
int MYKI_CD_getMaximumChange(U32_t* value)
{
    MYKI_CD_DeviceParameter_t deviceParameter;
    int returnValue = MYKI_CD_getDeviceParameterStructure("MaximumChange", &deviceParameter);
    if(returnValue)
    {
        returnValue = MYKI_CD_getU32(&deviceParameter, value);
    }
    return returnValue;        
}

/*==========================================================================*
**
**  MYKI_CD_getMaximumEFTPOSAmount
**
**  Description     :
**   The maximum transaction amount for EFTPOS payments.
**
**  Parameters      :
**      value           [IO]     pointer to the result value
**
**  Returns         :
**      TRUE                        value successfully retrieved 
**      FALSE                       error
**
**==========================================================================*/
int MYKI_CD_getMaximumEFTPOSAmount(U32_t* value)
{
    MYKI_CD_DeviceParameter_t deviceParameter;
    int returnValue = MYKI_CD_getDeviceParameterStructure("MaximumEFTPOSAmount", &deviceParameter);
    if(returnValue)
    {
        returnValue = MYKI_CD_getU32(&deviceParameter, value);
    }
    return returnValue;        
}

/*==========================================================================*
**
**  MYKI_CD_getMaximumTPurseBalance
**
**  Description     :
**   The maximum allowable T-Purse balance. This cannot exceed the technical limit as defined by in
**    NTS-D-0085 Card Data Definition.
**
**  Parameters      :
**      value           [IO]     pointer to the result value
**
**  Returns         :
**      TRUE                        value successfully retrieved 
**      FALSE                       error
**
**==========================================================================*/
int MYKI_CD_getMaximumTPurseBalance(S32_t* value)
{
    MYKI_CD_DeviceParameter_t deviceParameter;
    int returnValue = MYKI_CD_getDeviceParameterStructure("MaximumTPurseBalance", &deviceParameter);
    if(returnValue)
    {
        returnValue = MYKI_CD_getS32(&deviceParameter, value);
    }
    return returnValue;       
}


/*==========================================================================*
**
**  MYKI_CD_getMaximumTripTolerance
**
**  Description     :
**   Returns the maximum allowable Trip Tolerance. 
**
**  Parameters      :
**      serviceProvider [I]      ID of the serviceprovider of the device
**      value           [IO]     pointer to the result value
**
**  Returns         :
**      TRUE                        value successfully retrieved 
**      FALSE                       error
**
**==========================================================================*/
int MYKI_CD_getMaximumTripTolerance(U16_t serviceProvider, MYKI_CD_Mode_t mode, U8_t* value)
{
    MYKI_CD_DeviceParameter_t deviceParameter;
    int returnValue = MYKI_CD_getServiceProviderAndModeDeviceParameterStructure(serviceProvider, mode, "MaximumTripTolerance",  &deviceParameter);
    if(returnValue)
    {
        returnValue = MYKI_CD_getU8(&deviceParameter, value);
    }
    return returnValue;       
}


/*==========================================================================*
**
**  MYKI_CD_getMinimumAddValue
**
**  Description     :
**   The minimum amount that may be loaded onto the T-Purse in a single transaction.
**
**  Parameters      :
**      value           [IO]     pointer to the result value
**
**  Returns         :
**      TRUE                        value successfully retrieved 
**      FALSE                       error
**
**==========================================================================*/
int MYKI_CD_getMinimumAddValue(U32_t* value)
{
    MYKI_CD_DeviceParameter_t deviceParameter;
    int returnValue = MYKI_CD_getDeviceParameterStructure("MinimumAddValue", &deviceParameter);
    if(returnValue)
    {
        returnValue = MYKI_CD_getU32(&deviceParameter, value);
    }
    return returnValue;       
}

/*==========================================================================*
**
**  MYKI_CD_getMinimumTPurseAutoloadAmount
**
**  Description     :
**   The minimum amount that may be configured for the on-card t-purse autoload feature, specifically
**    for the value to be added when the autoload is executed.
**    NOTE: This value need only be checked when setting up the autoload feature on the card (i.e. at
**    the web portal before generating the action list). Devices that execute the autoload functionality on
**    the card do not need to check this as well.
**
**  Parameters      :
**      value           [IO]     pointer to the result value
**
**  Returns         :
**      TRUE                        value successfully retrieved 
**      FALSE                       error
**
**==========================================================================*/
int MYKI_CD_getMinimumTPurseAutoloadAmount(U32_t* value)
{
    MYKI_CD_DeviceParameter_t deviceParameter;
    int returnValue = MYKI_CD_getDeviceParameterStructure("MinimumTPurseAutoloadAmount", &deviceParameter);
    if(returnValue)
    {
        returnValue = MYKI_CD_getU32(&deviceParameter, value);
    }
    return returnValue;        
}

/*==========================================================================*
**
**  MYKI_CD_getMinimumTPurseAutoloadThreshold
**
**  Description     :
**   The minimum amount that may be configured for the on-card t-purse autoload feature, specifically
**    for the threshold that must be reached before the autoload is executed.
**    NOTE: This value need only be checked when setting up the autoload feature on the card (i.e. at
**    the web portal before generating the action list). Devices that execute the autoload functionality on
**    the card do not need to check this as well.
**
**  Parameters      :
**      value           [IO]     pointer to the result value
**
**  Returns         :
**      TRUE                        value successfully retrieved 
**      FALSE                       error
**
**==========================================================================*/
int MYKI_CD_getMinimumTPurseAutoloadThreshold(U32_t* value)
{
    MYKI_CD_DeviceParameter_t deviceParameter;
    int returnValue = MYKI_CD_getDeviceParameterStructure("MinimumTPurseAutoloadThreshold", &deviceParameter);
    if(returnValue)
    {
        returnValue = MYKI_CD_getU32(&deviceParameter, value);
    }
    return returnValue;        
}

/*==========================================================================*
**
**  MYKI_CD_getMinimumReceiptAmount
**
**  Description     :
**   A paper receipt must be printed for the patron for all transactions over this amount.
**    Note: The special value zero (0) means that receipts will never be printed without being manually
**    requested.
**
**  Parameters      :
**      value           [IO]     pointer to the result value
**
**  Returns         :
**      TRUE                        value successfully retrieved 
**      FALSE                       error
**
**==========================================================================*/
int MYKI_CD_getMinimumReceiptAmount(U32_t* value)
{
    MYKI_CD_DeviceParameter_t deviceParameter;
    int returnValue = MYKI_CD_getDeviceParameterStructure("MinimumReceiptAmount", &deviceParameter);
    if(returnValue)
    {
        returnValue = MYKI_CD_getU32(&deviceParameter, value);
    }
    return returnValue;      
}

/*==========================================================================*
**
**  MYKI_CD_getMinimumRefundBalance
**
**  Description     :
**   The minimum T-Purse balance to permit cash refund of T-Purse funds during card cancellation at
**    supporting sales channels.
**
**  Parameters      :
**      value           [IO]     pointer to the result value
**
**  Returns         :
**      TRUE                        value successfully retrieved 
**      FALSE                       error
**
**==========================================================================*/
int MYKI_CD_getMinimumRefundBalance(U32_t* value)
{
    MYKI_CD_DeviceParameter_t deviceParameter;
    int returnValue = MYKI_CD_getDeviceParameterStructure("MinimumRefundBalance", &deviceParameter);
    if(returnValue)
    {
        returnValue = MYKI_CD_getU32(&deviceParameter, value);
    }
    return returnValue;      
}

/*==========================================================================*
**
**  MYKI_CD_getPaperTicketMessage
**
**  Description     :
**   A customer or service message printed on paper tickets.
**
**  Parameters      :
**      value           [IO]     pointer to the result value
**
**  Returns         :
**      TRUE                        value successfully retrieved 
**      FALSE                       error
**
**==========================================================================*/
int MYKI_CD_getPaperTicketMessage(char* value, U16_t len)
{
    MYKI_CD_DeviceParameter_t deviceParameter;
    int returnValue = MYKI_CD_getDeviceParameterStructure("PaperTicketMessage", &deviceParameter);
    if(returnValue)
    {
        returnValue = MYKI_CD_getString(&deviceParameter, value, len);
    }
    return returnValue;      
}

/*==========================================================================*
**
**  MYKI_CD_getPaperTicketName
**
**  Description     :
**   A readable name for paper tickets.
**
**  Parameters      :
**      value           [IO]     pointer to the result value
**
**  Returns         :
**      TRUE                        value successfully retrieved 
**      FALSE                       error
**
**==========================================================================*/
int MYKI_CD_getPaperTicketName(char* value, U16_t len)
{
    MYKI_CD_DeviceParameter_t deviceParameter;
    int returnValue = MYKI_CD_getDeviceParameterStructure("PaperTicketName", &deviceParameter);
    if(returnValue)
    {
        returnValue = MYKI_CD_getString(&deviceParameter, value, len);
    }
    return returnValue;          
}

/*==========================================================================*
**
**  MYKI_CD_getProductAutoEnabled
**
**  Description     :
**   Defines whether a specific sales channel may process card-based product autoload.
**
**  Parameters      :
**      value           [IO]     pointer to the result value
**
**  Returns         :
**      TRUE                        value successfully retrieved 
**      FALSE                       error
**
**==========================================================================*/
int MYKI_CD_getProductAutoEnabled(U8_t *value)
{
    MYKI_CD_DeviceParameter_t deviceParameter;
    int returnValue = MYKI_CD_getDeviceParameterStructure("ProductAutoEnabled", &deviceParameter);
    if(returnValue)
    {
        returnValue = MYKI_CD_getBoolean(&deviceParameter, value);
    }
    return returnValue;
}

/*==========================================================================*
**
**  MYKI_CD_getRefundCancelEnabled
**
**  Description     :
**   Defines whether a specific sales channel may provide an instant cash refund when a patron
**    returns a long-life smartcard for cancellation
**
**  Parameters      :
**      value           [IO]     pointer to the result value
**
**  Returns         :
**      TRUE                        value successfully retrieved 
**      FALSE                       error
**
**==========================================================================*/
int MYKI_CD_getRefundCancelEnabled(U8_t *value)
{
    MYKI_CD_DeviceParameter_t deviceParameter;
    int returnValue = MYKI_CD_getDeviceParameterStructure("RefundCancelEnabled", &deviceParameter);
    if(returnValue)
    {
        returnValue = MYKI_CD_getBoolean(&deviceParameter, value);
    }
    return returnValue;
}

/*==========================================================================*
**
**  MYKI_CD_getRefundMessage
**
**  Description     :
**   A customer or service message printed on refund receipts
**
**  Parameters      :
**      value           [IO]     pointer to the result value
**
**  Returns         :
**      TRUE                        value successfully retrieved 
**      FALSE                       error
**
**==========================================================================*/
int MYKI_CD_getRefundMessage(char* value, U16_t len)
{
    MYKI_CD_DeviceParameter_t deviceParameter;
    int returnValue = MYKI_CD_getDeviceParameterStructure("RefundMessage", &deviceParameter);
    if(returnValue)
    {
        returnValue = MYKI_CD_getString(&deviceParameter, value, len);
    }
    return returnValue;     
}

/*==========================================================================*
**
**  MYKI_CD_getReversalMessage
**
**  Description     :
**   A customer or service message printed on reversal receipts.
**
**  Parameters      :
**      value           [IO]     pointer to the result value
**
**  Returns         :
**      TRUE                        value successfully retrieved 
**      FALSE                       error
**
**==========================================================================*/
int MYKI_CD_getReversalMessage(char* value, U16_t len)
{
    MYKI_CD_DeviceParameter_t deviceParameter;
    int returnValue = MYKI_CD_getDeviceParameterStructure("ReversalMessage", &deviceParameter);
    if(returnValue)
    {
        returnValue = MYKI_CD_getString(&deviceParameter, value, len);
    }
    return returnValue;     
}

/*==========================================================================*
**
**  MYKI_CD_getSalesMessage
**
**  Description     :
**   A customer or service message printed on sales receipts.
**
**  Parameters      :
**      value           [IO]     pointer to the result value
**
**  Returns         :
**      TRUE                        value successfully retrieved 
**      FALSE                       error
**
**==========================================================================*/
int MYKI_CD_getSalesMessage(char* value, U16_t len)
{
    MYKI_CD_DeviceParameter_t deviceParameter;
    int returnValue = MYKI_CD_getDeviceParameterStructure("SalesMessage", &deviceParameter);
    if(returnValue)
    {
        returnValue = MYKI_CD_getString(&deviceParameter, value, len);
    }
    return returnValue; 
}


/*==========================================================================*
**
**  MYKI_CD_getCompanyABN
**
**  Description     :
**   A customer or service message printed on sales receipts.
**
**  Parameters      :
**      value           [IO]     pointer to the result value
**
**  Returns         :
**      TRUE                        value successfully retrieved 
**      FALSE                       error
**
**==========================================================================*/
int MYKI_CD_getCompanyABN(char* value, U16_t len)
{
    MYKI_CD_DeviceParameter_t deviceParameter;
    int returnValue = MYKI_CD_getDeviceParameterStructure("CompanyABN", &deviceParameter);
    if(returnValue)
    {
        returnValue = MYKI_CD_getString(&deviceParameter, value, len);
    }
    return returnValue; 
}

/*==========================================================================*
**
**  MYKI_CD_getCompanyAddress
**
**  Description     :
**   Configurable company address
**
**  Parameters      :
**      value           [IO]     pointer to the result value
**
**  Returns         :
**      TRUE                        value successfully retrieved 
**      FALSE                       error
**
**==========================================================================*/
int MYKI_CD_getCompanyAddress(char* value, U16_t len)
{
    MYKI_CD_DeviceParameter_t deviceParameter;
    int returnValue = MYKI_CD_getDeviceParameterStructure("CompanyAddress", &deviceParameter);
    if(returnValue)
    {
        returnValue = MYKI_CD_getString(&deviceParameter, value, len);
    }
    return returnValue; 
}

/*==========================================================================*
**
**  MYKI_CD_getCompanyCallCentreName
**
**  Description     :
**   Configurable company call centre name
**
**  Parameters      :
**      value           [IO]     pointer to the result value
**
**  Returns         :
**      TRUE                        value successfully retrieved 
**      FALSE                       error
**
**==========================================================================*/
int MYKI_CD_getCompanyCallCentreName(char* value, U16_t len)
{
    MYKI_CD_DeviceParameter_t deviceParameter;
    int returnValue = MYKI_CD_getDeviceParameterStructure("CompanyCallCentreName", &deviceParameter);
    if(returnValue)
    {
        returnValue = MYKI_CD_getString(&deviceParameter, value, len);
    }
    return returnValue; 
}

/*==========================================================================*
**
**  MYKI_CD_getCompanyName
**
**  Description     :
**   Configurable company name
**
**  Parameters      :
**      value           [IO]     pointer to the result value
**
**  Returns         :
**      TRUE                        value successfully retrieved 
**      FALSE                       error
**
**==========================================================================*/
int MYKI_CD_getCompanyName(char* value, U16_t len)
{
    MYKI_CD_DeviceParameter_t deviceParameter;
    int returnValue = MYKI_CD_getDeviceParameterStructure("CompanyName", &deviceParameter);
    if(returnValue)
    {
        returnValue = MYKI_CD_getString(&deviceParameter, value, len);
    }
    return returnValue; 
}

/*==========================================================================*
**
**  MYKI_CD_getCompanyNameShort
**
**  Description     :
**   Get abbreviated version of company name.
**
**  Parameters      :
**      value           [IO]     pointer to the result value
**
**  Returns         :
**      TRUE                        value successfully retrieved 
**      FALSE                       error
**
**==========================================================================*/
int MYKI_CD_getCompanyNameShort(char* value, U16_t len)
{
    MYKI_CD_DeviceParameter_t deviceParameter;
    int returnValue = MYKI_CD_getDeviceParameterStructure("CompanyNameShort", &deviceParameter);
    if(returnValue)
    {
        returnValue = MYKI_CD_getString(&deviceParameter, value, len);
    }
    return returnValue; 
}

/*==========================================================================*
**
**  MYKI_CD_getCompanyPhone
**
**  Description     :
**   Get helpdesk phone number for company
**
**  Parameters      :
**      value           [IO]     pointer to the result value
**
**  Returns         :
**      TRUE                        value successfully retrieved 
**      FALSE                       error
**
**==========================================================================*/
int MYKI_CD_getCompanyPhone(char* value, U16_t len)
{
    MYKI_CD_DeviceParameter_t deviceParameter;
    int returnValue = MYKI_CD_getDeviceParameterStructure("CompanyPhone", &deviceParameter);
    if(returnValue)
    {
        returnValue = MYKI_CD_getString(&deviceParameter, value, len);
    }
    return returnValue; 
}

/*==========================================================================*
**
**  MYKI_CD_getCompanyWebSite
**
**  Description     :
**   Get company website URL.
**
**  Parameters      :
**      value           [IO]     pointer to the result value
**
**  Returns         :
**      TRUE                        value successfully retrieved 
**      FALSE                       error
**
**==========================================================================*/
int MYKI_CD_getCompanyWebSite(char* value, U16_t len)
{
    MYKI_CD_DeviceParameter_t deviceParameter;
    int returnValue = MYKI_CD_getDeviceParameterStructure("CompanyWebSite", &deviceParameter);
    if(returnValue)
    {
        returnValue = MYKI_CD_getString(&deviceParameter, value, len);
    }
    return returnValue; 
}

/*==========================================================================*
**
**  MYKI_CD_getRecieptInvoiceMessageThreshold
**
**  Description     :
**   Transactions over this threshold amount will trigger the printed message regarding contacting the
**    Call Centre for a Tax Invoice. For consistency this parameter uses currency units as defined in
**    section 2.1.2
**    Note the message is not controlled by product configuration.
**    The message is current set to: " For a tax invoice of $1000 or more that includes personal and/or
**    business details please retain this receipt and call 13 6954 (13 myki)."
**
**  Parameters      :
**      value           [IO]     pointer to the result value
**
**  Returns         :
**      TRUE                        value successfully retrieved 
**      FALSE                       error
**
**==========================================================================*/
int MYKI_CD_getRecieptInvoiceMessageThreshold(U32_t *value)
{
    MYKI_CD_DeviceParameter_t deviceParameter;
    int returnValue = MYKI_CD_getDeviceParameterStructure("RecieptInvoiceMessageThreshold", &deviceParameter);
    if(returnValue)
    {
        returnValue = MYKI_CD_getU32(&deviceParameter, value);
    }
    return returnValue;      
}

/*==========================================================================*
**
**  MYKI_CD_getTPurseAutoEnabled
**
**  Description     :
**   Defines whether a specific sales channel may process card-based T-Purse autoload
**
**  Parameters      :
**      value           [IO]     pointer to the result value
**
**  Returns         :
**      TRUE                        value successfully retrieved 
**      FALSE                       error
**
**==========================================================================*/
int MYKI_CD_getTPurseAutoEnabled(U8_t *value)
{
    MYKI_CD_DeviceParameter_t deviceParameter;
    int returnValue = MYKI_CD_getDeviceParameterStructure("TPurseAutoEnabled", &deviceParameter);
    if(returnValue)
    {
        returnValue = MYKI_CD_getBoolean(&deviceParameter, value);
    }
    return returnValue;
}

/*==========================================================================*
**
**  MYKI_CD_getLLSCConcessionEnabled
**
**  Description     :
**   Defines whether a specific sales channel may sell Long Life products at concession fares.
**
**  Parameters      :
**      value           [IO]     pointer to the result value
**
**  Returns         :
**      TRUE                        value successfully retrieved 
**      FALSE                       error
**
**==========================================================================*/
int MYKI_CD_getLLSCConcessionEnabled(U8_t *value)
{
    //TODO: Review if more parameters are required (location)
    MYKI_CD_DeviceParameter_t deviceParameter;
    int returnValue = MYKI_CD_getDeviceParameterStructure("LLSCConcessionEnabled", &deviceParameter);
    if(returnValue)
    {
        returnValue = MYKI_CD_getBoolean(&deviceParameter, value);
    }
    return returnValue;
}

/*==========================================================================*
**
**  MYKI_CD_getDSCConcessionEnabled
**
**  Description     :
**   Defines whether a specific sales channel may sell disposable products at concession fares.
**
**  Parameters      :
**      value           [IO]     pointer to the result value
**
**  Returns         :
**      TRUE                        value successfully retrieved 
**      FALSE                       error
**
**==========================================================================*/
int MYKI_CD_getDSCConcessionEnabled(U8_t *value)
{
    MYKI_CD_DeviceParameter_t deviceParameter;
    int returnValue = MYKI_CD_getDeviceParameterStructure("DSCConcessionEnabled", &deviceParameter);
    if(returnValue)
    {
        returnValue = MYKI_CD_getBoolean(&deviceParameter, value);
    }
    return returnValue;
}

/*==========================================================================*
**
**  MYKI_CD_getUnblockEnabled
**
**  Description     :
**   Defines whether a device may unblock a smart-card and for which blocking reasons.
**   Note that this parameter does not affect the action list processing of type Unblock
**
**  Parameters      :
**      value           [IO]     pointer to the result value
**
**  Returns         :
**      TRUE                        value successfully retrieved 
**      FALSE                       error
**
**==========================================================================*/
int MYKI_CD_getUnblockEnabled(U8_t *value)
{
    MYKI_CD_DeviceParameter_t deviceParameter;
    int returnValue = MYKI_CD_getDeviceParameterStructure("UnblockEnabled", &deviceParameter);
    if(returnValue)
    {
        returnValue = MYKI_CD_getBoolean(&deviceParameter, value);
    }
    return returnValue;
}

/*==========================================================================*
**
**  MYKI_CD_getPaymentMethodEnabled
**
**  Description     :
**   Defines whether a device may utilise a particular payment method.
**
**  Parameters      :
**      value           [IO]     pointer to the result value
**
**  Returns         :
**      TRUE                        value successfully retrieved 
**      FALSE                       error
**
**==========================================================================*/
int MYKI_CD_getPaymentMethodEnabled(const char* deviceType, U16_t serviceProvider, const char* paymentMethod, U8_t *value ) 
{
    MYKI_CD_DeviceParameter_t deviceParameter;
    int returnValue = MYKI_CD_getPaymentMethodDeviceParameterStructure(deviceType, serviceProvider, paymentMethod, &deviceParameter);
    if(returnValue)
    {
        returnValue = MYKI_CD_getBoolean(&deviceParameter, value);
    }
    return returnValue;
}

/*==========================================================================*
**
**  MYKI_CD_getReversalPeriod
**
**  Description     :
**   Defines the amount of time (in minutes) before a sales transaction may no longer be reversed.
**
**  Parameters      :
**      value           [IO]     pointer to the result value
**
**  Returns         :
**      TRUE                        value successfully retrieved 
**      FALSE                       error
**
**==========================================================================*/
int MYKI_CD_getReversalPeriod(U16_t *value)
{
    MYKI_CD_DeviceParameter_t deviceParameter;
    int returnValue = MYKI_CD_getDeviceParameterStructure("ReversalPeriod", &deviceParameter);
    if(returnValue)
    {
        returnValue = MYKI_CD_getU16(&deviceParameter, value);
    }
    return returnValue;      
}

// Differential Prices
int MYKI_CD_getDifferentialPriceStructure(const MYKI_CD_DifferentialPricingRequest_t* input, MYKI_CD_DifferentialPricing_t* differentialPricing)
{
    return getDifferentialPrice(input, *differentialPricing);
}

// Differential Prices Provisional
int MYKI_CD_getDifferentialPriceProvisionalStructure(const MYKI_CD_DifferentialPricingRequest_t* input, MYKI_CD_DifferentialPricingProvisional_t* differentialPricing)
{
    return getDifferentialPricesProvisional(input, *differentialPricing);
}

// Fares Epass
int MYKI_CD_getFaresEpassStructure(U8_t zoneLow, U8_t zoneHigh, U16_t days, MYKI_CD_FaresEpass_t * faresEpass)
{
    return getFaresEpass(zoneLow, zoneHigh, days, *faresEpass);
}

// Fares Provisional
int MYKI_CD_getFaresProvisionalStructure(U8_t zoneLow, U8_t zoneHigh,   MYKI_CD_FaresProvisional_t *faresProvisional)
{
    return getFaresProvisional(zoneLow, zoneHigh, *faresProvisional);
}

// Fares Stored Value
int MYKI_CD_getFaresStoredValueStructure(U8_t zoneLow, U8_t zoneHigh, MYKI_CD_FaresStoredValue_t* faresStoredValue)
{
    return getFaresStoredValue(zoneLow, zoneHigh, *faresStoredValue);   
}

// HMI Messaging returns number of rows
int MYKI_CD_getHMIMessaging(char* tableType, MYKI_CD_HMIMessaging_t* array, U16_t arrayLength)
{
    return getHMIMessaging(tableType, array, arrayLength);
}
// Locations
int MYKI_CD_getLocationsStructure(U16_t id, MYKI_CD_Locations_t *locations)
{
    return getLocations(id, *locations);

}

// Master Routes
int MYKI_CD_getMasterRouteStructure(U16_t masterRouteId, U16_t serviceProviderId, MYKI_CD_MasterRoute_t* pMasterRoute)
{
	return getMasterRoute(masterRouteId, serviceProviderId, *pMasterRoute);
}

int MYKI_CD_getRoutesByServiceProvider(U16_t serviceProviderId, MYKI_CD_RouteInfo_t* pRoutes, U16_t arrayLength)
{
	return getServiceProviderRoutes(serviceProviderId, pRoutes, arrayLength);
}


// Passenger Type
int MYKI_CD_getPassengerTypeStructure(U8_t id, MYKI_CD_PassengerType_t *passengerType)
{
    return getPassengerType(id, *passengerType);
}

// Product Issuer
int MYKI_CD_getProductIssuerStructure(U8_t id, MYKI_CD_ProductIssuer_t *productIssuer)
{
    return getProductIssuer(id, *productIssuer);
}

// Product
int MYKI_CD_getProductStructure(U8_t id, MYKI_CD_Product_t *product)
{
    return getProduct(id, *product);
}

int MYKI_CD_getProductType(U8_t id, char* type, int bufferLength)
{
    return getProductType(id, type, bufferLength);
}

int MYKI_CD_getProductCardType(U8_t id, char* type, int bufferLength)
{
    return getProductCardType(id, type, bufferLength);
}

// Routes
int MYKI_CD_getRouteInfoStructure(U16_t routeId, MYKI_CD_RouteInfo_t* pRouteInfo)
{
	return getRouteInfo(routeId, *pRouteInfo);
}

// Route Sections
int MYKI_CD_getRouteSectionStructure(U16_t routeId, U8_t stopId, MYKI_CD_RouteSection_t* pRouteSection)
{
	return getRouteSection(routeId, stopId, *pRouteSection);
}

// Route stops
int MYKI_CD_getRouteStopsStructure(U16_t routeId, U8_t fromStopId, U8_t toStopId, MYKI_CD_RouteStop_t** pRouteStops)
{
	return getRouteStops(routeId, fromStopId, toStopId, pRouteStops);
}

// Service Providers
int MYKI_CD_getServiceProvidersStructure(U16_t id, MYKI_CD_ServiceProviders_t *serviceProviders)
{
    return getServiceProviders(id, *serviceProviders);
}

int MYKI_CD_getServiceProviderTransportMode(U16_t id, char* mode, int bufferLength)
{
    return getServiceProviderTransportMode(id, mode, bufferLength);
}

// Station Lines
int MYKI_CD_getLinesByStationStructure(U16_t stationId, MYKI_CD_LinesByStation_t *linesByStation, MYKI_CD_U16Array_t* array)
{
    return getLinesByStation(stationId, *linesByStation, *array);
}

int MYKI_CD_getStationsByLineStructure(U16_t lineId, MYKI_CD_StationsByLine_t *stationsByLine, MYKI_CD_U16Array_t *array)
{
    return getStationsByLine(lineId, *stationsByLine, *array);
}

// Stations
int MYKI_CD_getStationsStructure(U16_t id, MYKI_CD_Stations_t* stations)
{
    return getStations(id, *stations);
}

int MYKI_CD_getStationsByLocationIdStructure(U16_t locationId, MYKI_CD_Stations_t* stations)
{
    return getStationsByLocationId(locationId, *stations);
}

// Zones
int MYKI_CD_isCitySaverZone(U8_t zoneId)
{
    return isCitySaverZone(zoneId);
}

// Early Bird information accessor functions
// These functions return indications that there is no Early Bird Information
// as we have no knowledge of the format of the provided data.
//
int MYKI_CD_isEarlyBirdStation(U16_t stationId)
{
	return isEarlyBirdStation(stationId);
}

int MYKI_CD_isEarlyBirdDay(int day)
{
    return isEarlyBirdDay(day);
}

int MYKI_CD_getEarlyBirdProductId(void)
{
    return getEarlyBirdProductId();
}

// Returned as minutes past midnight (e.g. 0705 = 435 minutes)
int MYKI_CD_getEarlyBirdCutOffTime(void)
{
	return getEarlyBirdCutOffTime();
}

int MYKI_CD_getChangeoverLoc(MYKI_CD_Locations_t * pLocScanOn, MYKI_CD_Locations_t * pLocScanOff, MYKI_CD_Locations_t * pLocChangeover)
{
    int result = false;
    MYKI_CD_Stations_t scanOnStation;
    MYKI_CD_Stations_t scanOffStation;

    MYKI_CD_LinesByStation_t linesByScanOnStation;
    MYKI_CD_LinesByStation_t linesByScanOffStation;
    MYKI_CD_U16Array_t linesScanOn;
    MYKI_CD_U16Array_t linesScanOff;
    MYKI_CD_U16Array_t stationsScanOn;
    MYKI_CD_U16Array_t stationsScanOff;
    memset(&linesScanOn, 0, sizeof(linesScanOn));
    memset(&linesScanOff, 0, sizeof(linesScanOff));
    memset(&stationsScanOn, 0, sizeof(stationsScanOn));
    memset(&stationsScanOff, 0, sizeof(stationsScanOff));

    int i, j, k, l;
    U16_t changeOverStationId = 0;

    // Get the Scan On and Scan Off Station Record from the Location ID.
    if(getStationsByLocationId(pLocScanOn->id, scanOnStation) && getStationsByLocationId(pLocScanOff->id, scanOffStation))
    {
        // Get the Lines that Scan On and Scan Off staions are a part of.
        if(getLinesByStation(scanOnStation.id, linesByScanOnStation, linesScanOn) && getLinesByStation(scanOffStation.id, linesByScanOffStation, linesScanOff))
        {
            // Iterate the Lines that the Scan On station belongs to.
            for(i=0; i<linesScanOn.arraySize; i++)
            {
                if(changeOverStationId>0) break;
                MYKI_CD_StationsByLine_t stationsByLine;

                // Get the Staions on current Scan On Line
                if(getStationsByLine(linesScanOn.arrayOfU16[i], stationsByLine, stationsScanOn))
                {
                    //Iterate through the staions on the current Scan On Line
                    for(j=0; j<stationsScanOn.arraySize; j++)
                    {
                        if(changeOverStationId>0) break;
                        // Iterate the Lines that the Scan Off station belongs to.
                        for(k=0; k<linesScanOff.arraySize; k++)
                        {
                            if(changeOverStationId>0) break;

                            // Get the Staions on current Scan Off Line
                            if(getStationsByLine(linesScanOff.arrayOfU16[k], stationsByLine, stationsScanOff))
                            {
                                //Iterate through the staions on the current Scan Off Line
                                for(l=0; l<stationsScanOff.arraySize; l++)
                                {
                                    // Compare the two stations.
                                    if(stationsScanOn.arrayOfU16[j] == stationsScanOff.arrayOfU16[l])
                                    {
                                        changeOverStationId = stationsScanOn.arrayOfU16[j];
                                        result = true;
                                        break;
                                    }
                                }

                            }
                        }

                    }
                }
            }
        }
    }

    if(result)
    {
        // Get the Change Over Station
        MYKI_CD_Stations_t changeOverStation;
        if(getStations(changeOverStationId, changeOverStation))
        {
            result = getLocations(changeOverStation.location, *pLocChangeover);
        }
    }

    return result;
}


// Import Product Configuration XML
int MYKI_CD_processProductConfigurationXML( int railTariff )
{
    int ret = 0;
    ret = processDeviceParametersXML();
    if ( ret == 0 )
        ret = processDifferentialPricesXML();
    if ( ret == 0 )
        ret = processDifferentialPricesProvisionalXML(); 
#ifdef PROCESS_FARESEPASS_XML
    if ( ret == 0 )
        ret = processFaresEpassXML();  // FaresEPass table is not required.
#endif
    if ( ret == 0 )
        ret = processFaresProvisionalXML();
    if ( ret == 0 )
        ret = processFaresStoredValueXML();
    if ( ret == 0 )
        ret = processHMIMessagingXML();
    if ( ret == 0 )
        ret = processLocationsXML();
    if ( ret == 0 && railTariff == FALSE )
        ret = processMasterRoutesXML();
    if ( ret == 0 )
        ret = processPassengerTypesXML();
    if ( ret == 0 )
        ret = processProductIssuersXML();
    if ( ret == 0 )
        ret = processProductsXML();
    if ( ret == 0 && railTariff == FALSE )
        ret = processRoutesXML();
    if ( ret == 0 && railTariff == FALSE )
        ret = processRouteSectionsXML();
    if ( ret == 0 && railTariff == FALSE )
        ret = processRouteStopsXML();
    if ( ret == 0 )
        ret = processServiceProvidersXML();
    if ( ret == 0 )
        ret = processStationsXML();
    if ( ret == 0 )
        ret = processStationLinesXML();
    if ( ret == 0 )
        ret = processZonesXML();

    return ret;
}

int
MYKI_CD_getProducts(
    Json::Value    &products,
    int             zone,
    const char     *productType,
    Time_t          time /* TIME_NOT_SET */,
    int             serviceProviderId /* = 0 */,
    const char     *deviceType /* = NULL */ )
{
    if ( deviceType == NULL )
    {
        deviceType  = g_DeviceType.c_str();
    }

    if ( serviceProviderId == 0 )
    {
        serviceProviderId   = g_ServiceProviderId;
    }

    if ( time == TIME_NOT_SET )
    {
        CsTime_t    now;
        CsTime( &now );
        time    = now.tm_secs;
    }

    return  getProducts( products, zone, productType, time, serviceProviderId, deviceType );
}
