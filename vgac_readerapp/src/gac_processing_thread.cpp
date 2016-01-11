/***************************************************************************
 * Copyright (c) 2014 Vix Technology. All rights reserved
 *
 * Module name   : gac_processing_thread.cpp
 * Module type   : 
 * Compiler(s)   : ANSI C
 * Environment(s): LINUX
 *
 * Description:
 * Code to control the card library for the GAC Reader 
     
 *
 * Contents:
 *
     
 *

 * Version   Who              Date       Description
   1.0       Morgan Dell      1/10/2013  Created
   1.01      ANT              14/05/2014 a) MBU-1083: Rectified unconfirmed
                                         transaction recovery
                                         b) Added generating card read/write
                                         failure and critical error alarms
                                         c) Added "Debug:CardLib" INI option
   1.02      ANT              23/05/2014 Removed checking for multiple cards at
                                         start of transaction due to reader
                                         stops polling when EMV card is presented
   1.03      ANT              10/06/2014 MBU-1098: Changed nHour period units
   1.04      ANT              27/02/2015 MTU-146: Added GetLocationUpdateTimeout()
   1.05      ANT              01/06/2015 Rectified validating/updating operator PIN
   1.06      ANT              16/07/2015 a) Rectified no time sync detection
                                         b) Enabled reader application to start
                                         without valid Tariff
                                         c) Added generating/clearing invalid
                                         Tariff alarm
   1.07      ANT              14/08/2015 a) Rectified data corruption
                                         b) Added card top-up
   1.08      ANT              20/08/2015 a) Added generating transaction TMI records
   1.09      ANT              26/08/2015 a) Added initialising BR sale parameters
 *
 * $Id:  $
 * $HeadURL: $
 *.........................................................................*/
 
/*
 *      Includes
 *      --------
 */

#include <json/json.h>

#include "card_processing_common.h"
#include "card_processing_thread.h"
#include "apptype.h"
#include "app_debug_levels.h"
#include <message_reader.h>
#include <message_ui.h>

#include <unistd.h>
#include <ipc.h>
#include <ct.h>
#include <ct_viper_mcr_comm.h>
#include <cs.h>
#include <csf.h>
#include <datec19.h>
#include <csmutex.h>
#include <csthread.h>
#include <datetime.h>
#include <myki_cdd_enums.h>
#include <myki_cardservices.h>
#include "myki_alarm.h"
#include "myki_alarm_names.h"
#include <myki_br.h>
#include <myki_fs_serialise.h>
#include <myki_tmi.h>
#include <ud_api.h>
#include <timing_point.h>
#if defined(COBRA) || defined(VIPER)
#include <serpent/drv_touch.h>
#include <serpent/drv_led.h>
#include <serpent/drv_util.h>
#define DRV_FEATURE_FRAM
#include <serpent/drv_fram.h>
#endif



#include "easywsclient.hpp"
#include "datastring.h"
#include "dataxfer.h"
#include "gentmi.h"
#include "cardfunctions.h"
#include "wsshift.h"
#include "wssales.h"
#include "roles.h"
#include "serviceproviders.h"
#include "cardsnapshot.h"

using namespace std;
#include <string>
#include <sstream>
#include <curl/curl.h>

#if     defined( HOST_I386 /* UBUNTU12 */ )
const int IsVirtualReader = 1;
#else
const int IsVirtualReader = 0;
#endif

#include "version.h"

#define POLLING
#define POLLING_DELAY 20
#ifdef DEBUG
    #include <comms_api.h>
#else
  //  #define DEBUG  // HACK: Debug functions keep comms working correctly
    #include <comms_api.h>
    //#undef DEBUG 
#endif
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <sys/stat.h>

#include "json/json.h"

extern "C" 
{
#include <LDT.h>
}

/*
 *      Local Constants and Macros
 *      --------------------------
 */

#define MAX_DUMP_FILENAME                   255

#define DEFAULT_CardRdFailWarn              5           /**< Default number of consecutive card read failures before generating ECardEncoderState warning alarm */
#define DEFAULT_CardRdFailError             10          /**< Default number of consecutive card read failures before generating ECardEncoderState error alarm */
#define DEFAULT_CardWrFailWarn              5           /**< Default number of consecutive card write failures before generating ECardEncoderState warning alarm */
#define DEFAULT_CardWrFailError             10          /**< Default number of consecutive card write failures before generating ECardEncoderState error alarm */
#define DEFAULT_SaveTxnDataFailWarn         1           /**< Default number of consecutive save transaction data failures before generating TransactionLogState warning alarm */
#define DEFAULT_SaveTxnDataFailError        2           /**< Default number of consecutive save transaction data failures before going out-of-service */
#define DEFAULT_MaxPinRetries               3           /**< Default number of consecutive PIN failures before "blocking" operator card */
#define DEFAULT_NoTimeSyncWarn              (1 * 24)    /**< Default number of hours without time synchronisation before generating TimeSynchronisationState warning alarm */
#define DEFAULT_NoTimeSyncError             (7 * 24)    /**< Default number of hours without time synchronisation before generating TimeSynchronisationState error alarm */
#define DEFAULT_NoTimeSyncCheck             (30)        /**< Default time synchronisation check interval (seconds) */
#define DEFAULT_CurrencyRoundingPortion     5           /**< Default currency rounding portion */

#ifdef  COBRA
#define DEFAULT_SAMSlot                     "1"
#else
#define DEFAULT_SAMSlot                     "2"
#endif
    /** Sets card reader failure alarms */
#define SetAlarmCardRdFail( ) \
                {\
                    s_CardRdFailures++;\
                    if ( s_iniCardRdFailWarn > 0 && s_CardRdFailures >= s_iniCardRdFailWarn )\
                        SetAlarm( AlarmMCRWarnRead      );\
                    if ( s_iniCardRdFailError > 0 && s_CardRdFailures >= s_iniCardRdFailError )\
                        SetAlarm( AlarmMCRErrorRead     );\
                }

    /** Clears card read failure alarms */
#define ClearAlarmCardRdFail( ) \
                {\
                    s_CardRdFailures    = 0;\
                    ClearAlarm( AlarmMCRWarnRead    );\
                    ClearAlarm( AlarmMCRErrorRead   );\
                }

    /** Sets card write failure alarms */
#define SetAlarmCardWrFail( ) \
                {\
                    s_CardWrFailures++;\
                    if ( s_iniCardWrFailWarn > 0 && s_CardWrFailures >= s_iniCardWrFailWarn )\
                        SetAlarm( AlarmMCRWarnWrite     );\
                    if ( s_iniCardWrFailError > 0 && s_CardWrFailures >= s_iniCardWrFailError )\
                        SetAlarm( AlarmMCRErrorWrite    );\
                }

    /** Clears card write failure alarms */
#define ClearAlarmCardWrFail( ) \
                {\
                    s_CardWrFailures    = 0;\
                    ClearAlarm( AlarmMCRWarnWrite   );\
                    ClearAlarm( AlarmMCRErrorWrite  );\
                }

    /** Is Driver Console reader application */
#define IS_DRIVER_CONSOLE_READER( )         ( s_isGACReader == false )

/*
 *      Local Function Prototypes
 *      -------------------------
 */

bool            ExecuteBusinessSequence(
                                int                 numberOfCardsPresent,
                                bool                operatorCard,
                                int                &passengerCode,
                                SequenceFunction_e  businessSequence, 
                                bool                forceProcessBR);

/*
 *      Local Variables
 *      ---------------
 */

static MYKI_BR_ContextData_t    g_defaultContextData                    = {{ 0 }};                          /**< Default BR context data */
static MYKI_BR_ContextData_t    brContextData                           = {{ 0 }};                          /**< BR context data */
static bool                     s_cardLibDebug                          = false;                            /**< Enable/Disable CardLibrary debugging */
bool                            g_calculatePassExpiry                   = true;                             /**< TRUE if deriving (and displaying) earliest EPass expiry */
static U8_t                    *g_pKSAM_ID                              = NULL;                             /**< SAM ID */
static int                      s_AcsCompatibilityMode                  = FALSE;                            /**< Enable/Disable ACS compatibility mode */
static char                     s_terminalId[CSFINI_MAXVALUE+1]         = {'\0'};                           /**< Terminal ID */
char                            g_terminalType[ CSFINI_MAXVALUE + 1 ]   = {'\0'};                           /**< Terminal type, eg. "TDC", "FPDm" */
static char                      s_gacAddress[CSFINI_MAXVALUE + 1]       = {'\0'};
static char                      s_alarmAddress[CSFINI_MAXVALUE + 1]     = {'\0'};
static char                      s_gacCardFile[CSFINI_MAXVALUE + 1]      = {'\0'};
static int                       s_dumpCardImage                            = false;
static int                       s_cardRemovalDelay                         = 2000;
static bool                      s_isGACReader                             = true;
static CardProcessingControl_t  g_cardProcessingControl;

int                                 g_inputVoltage                          = 0;
int                                 g_powerState                            = 0;
	static Time_t                   g_lastTimeSyncUTC                       = TIME_NOT_SET;
	static char                     g_lastTimeSyncOffset[ 16 ]              = { 0 };
	static uint32_t                 g_timeSyncCheckTimerID                  = 0;

    static int                      g_iniNoTimeSyncError                    = DEFAULT_NoTimeSyncError;
    static int                      g_iniNoTimeSyncWarn                     = DEFAULT_NoTimeSyncWarn;
    static int                      g_iniNoTimeSyncCheck                    = DEFAULT_NoTimeSyncCheck;
    static char                     g_iniSAMSlot[ CSFINI_MAXVALUE + 1 ] = { 0 };
static CT_CardInfo_t *          s_pCardInfo                             = NULL;
static bool                     s_unconfirmed                           = FALSE;

CsTime_t                 g_startTime;
CsTime_t                 g_lastTransactionTime;
long                     g_totalTransactions                     = 0;
long                     g_totalAcceptedTransactions             = 0;
long                     g_totalRejectedTransactions             = 0;

char                     g_rolesFile[CSFINI_MAXVALUE+1]          = {'\0'};
StaffRole_t              g_staffRoles[ MAX_STAFF_ROLES ]         = { { 0 } };
Json::Value              g_roles;

char                     g_serviceProvidersFile[CSFINI_MAXVALUE+1] = {'\0'};
Json::Value              g_serviceProviders;
char                     g_userIdFile[CSFINI_MAXVALUE+1]           = {'\0'};

    /*  TARIFF parameters */
U16_t                           g_ePassLowWarningThreshold              = 0;
U16_t                           g_TPurseLowWarningThreshold             = 0;

Currency_t                      g_initCurrencyRoundingPortion           = DEFAULT_CurrencyRoundingPortion;  /**< Currency rounding portion */
static int                      s_iniCardRdFailWarn                     = DEFAULT_CardRdFailWarn;           /**< Number of consecutive card read failures before generating ECardEncoderState warning alarm */
static int                      s_iniCardRdFailError                    = DEFAULT_CardRdFailError;          /**< Number of consecutive card read failures before generating ECardEncoderState error alarm */
static int                      s_iniCardWrFailWarn                     = DEFAULT_CardWrFailWarn;           /**< Number of consecutive card write failures before generating ECardEncoderState warning alarm */
static int                      s_iniCardWrFailError                    = DEFAULT_CardWrFailError;          /**< Number of consecutive card write failures before generating ECardEncoderState error alarm */
static int                      s_iniSaveTxnDataFailWarn                = DEFAULT_SaveTxnDataFailWarn;      /**< Number of consecutive save transaction data failures before generating TransactionLogState warning alarm */
static int                      s_iniSaveTxnDataFailError               = DEFAULT_SaveTxnDataFailError;     /**< Number of consecutive save transaction data failures before going out-of-service */
static int                      s_CardRdFailures                        = 0;                                /**< Number of consecutive card read failures */
static int                      s_CardWrFailures                        = 0;                                /**< Number of consecutive card write failures */
static int                      s_SaveTxnDataFailures                   = 0;                                /**< Number of consecutive save transaction data failures */
static int                      s_unconfirmedTxnBRResult                = MYKI_BR_RESULT_ERROR;             /**< Unconfirmed transaction BR result */

#define SECONDS_PER_DAY                     86400
#define SECONDS_PER_HOUR                    3600
#define SECONDS_PER_MINUTE                  60
#define NTPD_DATA                           "/afc/data/ntpd_data"
static int32_t                  s_NTSEpoch                              = 1136073600;

#define MAX_HMI_ARRAY_SIZE      50
static MYKI_CD_HMIMessaging_t   s_HMILoadLogArray[MAX_HMI_ARRAY_SIZE] = { {0} };
static MYKI_CD_HMIMessaging_t   s_HMIUsageLogArray[MAX_HMI_ARRAY_SIZE] = { {0} };

static int    s_HMILoadLogEntries = 0;
static int    s_HMIUsageLogEntries = 0;

static std::string g_json;
extern bool s_webSocketConnChanged;
extern bool s_webSocketConnected;
/*==========================================================================*
**
**  RefreshUIScenario
**
**  Description     :
**      Required by IPC monitor and is not relevant for the GAC Reader
**
**==========================================================================*/
extern "C" void RefreshUIScenario(void) 
{
    // Do nothing
}

/*==========================================================================*
**
**  CancelWaitForGateTimer
**
**  Description     :
**      Required by IPC monitor and is not relevant for the GAC Reader
**
**==========================================================================*/
extern "C" void CancelWaitForGateTimer()
{
    // Do nothing
}

/*==========================================================================*
**
**  GetDumpCardImage
**
**  Description     :
**      Helper function for config option
**
**   Return         :
**      Current value of dump card image.
**==========================================================================*/
extern "C" int GetDumpCardImage()
{
    return s_dumpCardImage;
}


/*==========================================================================*
**
**  GetEcuPresent
**
**  Description     :
**      Helper function
**
**   Return         :
**       False always for GAC reader
**==========================================================================*/
extern "C" int GetEcuPresent()
{
    return false;
}

/*==========================================================================*
**
**  GetCardProcessingThreadContextData
**
**  Description     :
**      Helper function
**
**   Return         :
**       Pointer to g_defaultContextData structure.
**==========================================================================*/
extern "C" MYKI_BR_ContextData_t* GetCardProcessingThreadContextData()
{
    return &g_defaultContextData;
}


    /**
     *  Returns BR context data after having executed business rule.
     *  @return The BR context data object.
     */
extern  "C"
MYKI_BR_ContextData_t *
GetBrContextDataOut( void )
{
    return &brContextData;
}

/*==========================================================================*
**
**  GetCardProcessingControl
**
**  Description     :
**      Helper function
**
**   Return         :
**       Pointer to g_cardProcessingControl structure.
**==========================================================================*/
extern "C" CardProcessingControl_t* GetCardProcessingControl()
{
    return &g_cardProcessingControl;
}
 
/*==========================================================================*
**
**  GetLocationUpdateTimeout
**
**  Description     :
**      Returns LocationUpdateTimeout configuration parameter value.
**
**  Parameters      :
**      None
**
**  Returns         :
**      0                   disabled
**      Else                location update timeout (seconds)
**
**==========================================================================*/
int GetLocationUpdateTimeout( void )
{
    /*  DONOTHING! Not applicable to GAC reader */

    return  0;
}   /*  GetLocationUpdateTimeout( ) */


/*==========================================================================*
**
**  TimeSyncUpdate
**
**  Description     :
**      Updates time synchronisation details.
**
**  Parameters      :
**      pEvent              [I]     NTP callback event
**      pOffset             [I]     
**      pStratum            [I]     Connected stratum, "16" = none
**      pFreqDriftPPM       [I]     Not used
**      pPollInterval       [I]     Not used
**
**  Returns         :
**      None
**
**  Notes           :
**      At regular interval, NTPD executes /afc/bin/ntpd_cb.sh which
**      subsequently executes the readerapp's "ntpcb" command and calls
**      this function to update time synchronisation information.
**
**==========================================================================*/

void    TimeSyncUpdate(
    const char     *pEvent,
    const char     *pOffset,
    const char     *pStratum,
    const char     *pFreqDriftPPM,
    const char     *pPollInterval )
{
    #define NTPCB_STRATUM_NONE      "16"

    (void)pFreqDriftPPM;
    (void)pPollInterval;

    if ( pEvent == NULL || pOffset == NULL || pStratum == NULL )
    {
        return;
    }   /*  end-of-if */

    CsDebug( APP_DEBUG_DETAIL, ( APP_DEBUG_DETAIL, "TimeSyncUpdate : Event(%s) Offset(%s) Stratum(%s)", pEvent, pOffset, pStratum ) );

    if ( strcmp( pEvent, "step" ) == 0 )
    {
        CsWarnx( "TimeSyncUpdate : Significant difference between local/network time" );
    }
    else
    if ( strcmp( pEvent, "stratum"  ) == 0 ||
         strcmp( pEvent, "periodic" ) == 0 )
    {
        if ( strcmp( pStratum, NTPCB_STRATUM_NONE ) == 0 )
        {
            /*  Not yet connected to any NTP server => DONOTHING! */
            CsDebug( APP_DEBUG_DETAIL, ( APP_DEBUG_DETAIL, "TimeSyncUpdate : NTP not connected" ) );
        }
        else
        if ( strcmp( g_lastTimeSyncOffset, pOffset ) == 0 )
        {
            /*  (Assume) Loosing connection with NTP server... */
            CsDebug( APP_DEBUG_DETAIL, ( APP_DEBUG_DETAIL, "TimeSyncUpdate : Loosing NTP connection..." ) );
        }
        else
        {
            CsTime_t    now;
            FILE       *fout    = NULL;

            /*  Local/Network time are still in synchronised => Record current date/time and offset */
            memset(  g_lastTimeSyncOffset, 0,       sizeof( g_lastTimeSyncOffset )     );
            strncpy( g_lastTimeSyncOffset, pOffset, sizeof( g_lastTimeSyncOffset ) - 1 );

            CsTime( &now );
            g_lastTimeSyncUTC   = now.tm_secs;

            fout    = fopen( NTPD_DATA, "w" );
            if ( fout != NULL )
            {
                fprintf( fout, "%d %s", g_lastTimeSyncUTC, g_lastTimeSyncOffset );
                fclose( fout );
            }
            else
            {
                CsWarnx( "TimeSyncUpdate : Failed opening '%s' for writing", NTPD_DATA );
            }   /*  end-of-if */
        }   /*  end-of-if */
    }
    else
    {
        /*  Que!? */
        CsDebug( APP_DEBUG_DETAIL, ( APP_DEBUG_DETAIL, "TimeSyncUpdate : Invalid NTP callback event '%s'", pEvent ) );
    }   /*  end-of-if */
}   /*  TimeSyncUpdate( ) */
 /*==========================================================================*
**
**  TimeSyncCheck
**
**  Description     :
**      Checks for time synchronisation.
**
**  Parameters      :
**      TimerId             [I]     Timer id
**
**  Returns         :
**      None
**
**  Notes           :
**
**==========================================================================*/

static  void    TimeSyncCheck( uint32_t TimerId )
{
    IPC_IntPayload_t   m = { {IPC_TIMEOUT, APP_TASK }, TIME_SYNC_TIMEOUT};
    IpcSend(IpcGetID( APP_TASK ), &m, sizeof m);
}   /*  TimeSyncCheck( ) */

/*==========================================================================*
**
**  DumpTimingPoints
**
**  Description     :
**      Write timing information for the transaction to the debug log 
**
**  Parameters      :
**      None
**
**  Returns         :
**      None
**
**==========================================================================*/
static void DumpTimingPoints( )
{
   char seps[] = " ,";
   char* token = 0;
   char* strtokSave = 0;
   unsigned long t;
   int txnStart = 0;
   int txnFinish = 0;
   int transapStart = 0;
   int transapFinish = 0;
   int brStart = 0;
   int brFinish = 0;
   int ctStart = 0;
   int ctFinish = 0;
   int cardCommandStart = 0;
   int cardCommandFinish = 0;
   int cryptoStart = 0;
   int cryptoFinish = 0;
   int txnTotalTime = 0;
   int transApTotalTime = 0;
   int brTotalTime = 0;
   int ctTotalTime = 0;
   int cardCommandTotalTime = 0;
   int cryptoTotalTime = 0;

   char Str[256];
   char tpStr[256];
   char elapsedStr[256];
   char* pStr = &Str[0];

   while (DAF_TimingPointGetNextRecord(&pStr) == 0)
   {
      int length = strlen(pStr);
      pStr[length-1] = 0;
      strcpy(tpStr, pStr);

      // Establish string and get the first token:
      token = strtok_r(pStr, seps, &strtokSave);
      token = strtok_r(NULL, seps, &strtokSave);

      sscanf(token, "%lu", &t);//Use sscanf to parse unsigned long

      token = strtok_r(NULL, seps, &strtokSave);
      token = strtok_r(NULL, seps, &strtokSave);

      if (memcmp(token, "APPLICATION_TRANSACTION_TOTAL", 29) == 0 )
      {
         if (txnStart)
            txnFinish = t;
         else
            txnStart = t;
      }
      else if (memcmp(token, "DAF_BR_EXECUTION", 16) == 0 )
      {
         if (brStart)
            brFinish = t;
         else
            brStart = t;
      }
      else if (memcmp(token, "DAF_TRANSAP", 11) == 0 && memcmp(token, "DAF_TRANSAP_DESFIRE", 19) != 0 )
      {
         if (transapStart)
            transapFinish = t;
         else
            transapStart = t;
      }
      else if (memcmp(token, "DAF_CT", 6) == 0)
      {
         if (ctStart)
            ctFinish = t;
         else
            ctStart = t;
      }
      else if (memcmp(token, "DAF_TRANSAP_DESFIRE", 19) == 0)
      {
         if (cardCommandStart)
            cardCommandFinish = t;
         else
            cardCommandStart = t;
      }
      else if (memcmp(token, "DAF_CRYP", 8) == 0 || memcmp(token, "CRYPTO_DESFIRE", 14) == 0 )
      {
         if (cryptoStart)
            cryptoFinish = t;
         else
            cryptoStart = t;
      }

      elapsedStr[0] = 0;

      if (transapFinish)
      {
         sprintf(elapsedStr, "Elapsed Time: %dms", transapFinish - transapStart);
         transApTotalTime += transapFinish - transapStart;
         transapStart  = 0;
         transapFinish = 0;
      }
      else if (ctFinish)
      {
         sprintf(elapsedStr," Elapsed Time: %dms", ctFinish - ctStart);
         ctTotalTime += ctFinish - ctStart;
         ctStart  = 0;
         ctFinish = 0;
      }
      else if (cardCommandFinish)
      {
         sprintf(elapsedStr, "Elapsed Time: %dms", cardCommandFinish - cardCommandStart);
         cardCommandTotalTime += cardCommandFinish - cardCommandStart;
         cardCommandStart  = 0;
         cardCommandFinish = 0;
      }
      else if (cryptoFinish)
      {
         sprintf(elapsedStr, "Elapsed Time: %dms", cryptoFinish - cryptoStart);
         cryptoTotalTime += cryptoFinish - cryptoStart;
         cryptoStart  = 0;
         cryptoFinish = 0;
      }

      CsDebug(APP_DEBUG_STATE, (APP_DEBUG_STATE, "%s\t\t\t\t%s\n", tpStr, elapsedStr));
   }

   txnTotalTime = txnFinish - txnStart;
   brTotalTime  = brFinish  - brStart;
   CsDebug(APP_DEBUG_FLOW, (APP_DEBUG_FLOW, "Txn Total Elapsed Time: %dms\n", txnTotalTime));
   CsDebug(APP_DEBUG_FLOW, (APP_DEBUG_FLOW, "Component times BR %dms, TransAp %dms, Card Command %dms, Crypto %dms, CT %dms",
      brTotalTime,
      transApTotalTime,
      cardCommandTotalTime,
      cryptoTotalTime,
      ctTotalTime));
}

/*==========================================================================*
**
** SetKSamId
**
** Description :
** Sets virtual reader SAM ID.
**
** Parameters :
** pKSAM_ID [I] new KSAM ID
**
** Returns :
** None
**
** Note:
** This function is used to temporary set the SAMID to value used
** for device hotlisting automatic testing.
**
**==========================================================================*/
void SetKSamId( U32_t KSAM_ID )
{
    if ( IsVirtualReader != FALSE && g_pKSAM_ID != NULL )
    {
        CsDebug( APP_DEBUG_FLOW, ( APP_DEBUG_FLOW, "CardProcessingThread:SetKSamId(%08X)", KSAM_ID ) );
        g_pKSAM_ID[ 0 ] = (U8_t)( ( KSAM_ID >> 24 ) & 0xff );
        g_pKSAM_ID[ 1 ] = (U8_t)( ( KSAM_ID >> 16 ) & 0xff );
        g_pKSAM_ID[ 2 ] = (U8_t)( ( KSAM_ID >>  8 ) & 0xff );
        g_pKSAM_ID[ 3 ] = (U8_t)(   KSAM_ID         & 0xff );
    }
}   /* SetKSamId( ) */

/*==========================================================================*
**
**  InitialiseDatabases
**
**  Description     :
**      Open database from file location.  This function must succeed on startup
**
**   Return         :
**       True : database successfully opened
**==========================================================================*/
extern "C" int InitialiseDatabases()
{
    char iniBuf[128];
    bool result             = false;
    bool alIsInit           = false;
    bool tariffIsInit       = false;
    bool userProfilesIsInit = false;
    if ( CsfIniExpand("MykiApp General:CDDatabase", iniBuf, sizeof(iniBuf)) == 0 )
    {
        CsDebug(APP_DEBUG_FLOW, (APP_DEBUG_FLOW, "InitialiseDatabases open %s", iniBuf));
        tariffIsInit = MYKI_CD_openCDDatabase(iniBuf);
    }
    else
    {
        CsDebug(APP_DEBUG_FLOW, (APP_DEBUG_FLOW, "InitialiseDatabases open default"));
        tariffIsInit = MYKI_CD_openCDDatabase(NULL);
        
    }

    if (tariffIsInit==true)
    {
        if ( MYKI_CD_getStartDateTime() == 0 )
        {
            CsErrx("InitialiseDatabases - no active CD version");
            /*  NOTE:   Allows reader application to start-up without valid Tariff
            result = false;
            */
        }
    }
    
    if ( CsfIniExpand("MykiApp General:ActionlistDatabase", iniBuf, sizeof(iniBuf)) == 0 )
    {
        alIsInit = MYKI_CD_openFullActionlistDatabase(iniBuf);
    } 
    else 
    {
        alIsInit = MYKI_CD_openFullActionlistDatabase(defaultActionlistDBFile);
    }

    if(alIsInit==true)
    {
        if ( CsfIniExpand("MykiApp General:DeltaActionlistDatabase", iniBuf, sizeof(iniBuf)) == 0 )
        {
          alIsInit = MYKI_CD_openDeltaActionlistDatabase(iniBuf);
        } 
        else 
        {
          alIsInit = MYKI_CD_openDeltaActionlistDatabase(defaultActionlistDeltaDBFile);
        }
    }
//  Absence of a delta al db should not stop us from progressing, however errors in opening
//   it (given it is found) might be logged as an ERROR by the called function
//    result = true;
    
    
    if ( CsfIniExpand("MykiApp General:RolesJson", g_rolesFile, sizeof(g_rolesFile)) != 0 )
    {
        CsWarnx("Roles file location not present in configuration files");
        g_rolesFile[0] = 0;
    } 
    
    userProfilesIsInit = loadRoles();

    if ( CsfIniExpand("MykiApp General:ServiceProvidersJson", g_serviceProvidersFile, sizeof(g_serviceProvidersFile)) != 0 )
    {
        CsWarnx("Service Providers File location not present in configuration files");
        g_serviceProvidersFile[0] = 0;
    }

    userProfilesIsInit = loadServiceProviders();
    
    if ( alIsInit==false )
    {
      /* MTU-876 no action list will not stop us from going to service, as it is not critical for going to service */
    }

    if ( tariffIsInit==false )
    {
        SetAlarm( AlarmOutOfServiceDatabaseError );        /* MBU-1560 failure to initialize database or user profiles */
        SetAlarm( AlarmVALErrorTariffInit );               /* Failure to load valid tariff on start up */
        SetAlarm( AlarmOutOfServiceTariffError);           /* Failure to init tariff (missing or inconsistent data) */
        CsErrx("InitialiseDatabases failed");
    } else
    {
        ClearAlarm( AlarmOutOfServiceDatabaseError );
        ClearAlarm( AlarmVALErrorTariffInit ); 
        ClearAlarm( AlarmOutOfServiceTariffError);    
    }

    if ( userProfilesIsInit==false)
    {
        CsErrx("Initialise roles and serviceprovider files failed");
        SetAlarm( AlarmOutOfServiceUserProfiles );
    } else
    {
        ClearAlarm( AlarmOutOfServiceUserProfiles );
    }
    
    if ( userProfilesIsInit==true && tariffIsInit==true )
    {
        result = true;
    }
    
    return result;
}
    
/*==========================================================================*
**
**  GetDeviceId
**
**  Description     :
**      Initialise BR Context from CD database
**
**   Return         :
**       int representing the ESN (Electronic Serial Number) of the device
**==========================================================================*/

static int GetDeviceId()
{
#if defined(COBRA) || defined(VIPER)
    int r = DrvFramInit();
    if ( r < 0 )
    {
        CsWarnx("Error initialising FRAM - will not get device ID");
    }
    else
    {
        FrmManufactStruc mInfo;
        r = DrvFramReadManfact(&mInfo);
        if ( r < 0 )
        {
            CsWarnx("Error reading FRAM - will not get device ID");
        }
        else
        {
            return mInfo.id.esn;
        }
    }
#endif
    return 0;
}

/*==========================================================================*
**
**  InitialiseBRContextFromCD
**
**  Description     :
**      Initialise BR Context from CD database
**
** 
**==========================================================================*/
void InitialiseBRContextFromCD( MYKI_BR_ContextData_t* pContextData )
{
    U32_t               u32Value            = 0;
    char                sTransportMode[ 64 ];
    const char         *pDeviceType         = NULL;
    MYKI_CD_Mode_t      cdTransportMode     = MYKI_CD_MODE_UNKNOWN;

    memset( sTransportMode, 0, sizeof( sTransportMode ) );
    if ( MYKI_CD_getServiceProviderTransportMode(
            pContextData->StaticData.serviceProviderId, sTransportMode, sizeof( sTransportMode ) ) == FALSE )
    {
        CsErrx( "InitialiseBRContextFromCD : MYKI_CD_getServiceProviderTransportMode(%d) failed",
            pContextData->StaticData.serviceProviderId );
        pContextData->InternalData.TransportMode    = TRANSPORT_MODE_UNKNOWN;
    }
    else
    {
        /*  Determines transport mode */
        if      ( strcmp( sTransportMode, "RAIL" ) == 0 )   cdTransportMode = MYKI_CD_MODE_RAIL;
        else if ( strcmp( sTransportMode, "BUS"  ) == 0 )   cdTransportMode = MYKI_CD_MODE_BUS;
        else if ( strcmp( sTransportMode, "TRAM" ) == 0 )   cdTransportMode = MYKI_CD_MODE_TRAM;

        /*  Determines device type */
        if ( cdTransportMode != MYKI_CD_MODE_UNKNOWN )
        {
            switch ( cdTransportMode )
            {
            case MYKI_CD_MODE_BUS:
                pContextData->InternalData.TransportMode    = TRANSPORT_MODE_BUS;
                break;

            case MYKI_CD_MODE_TRAM:
                pContextData->InternalData.TransportMode    = TRANSPORT_MODE_TRAM;
                break;

            case MYKI_CD_MODE_RAIL:
                pContextData->InternalData.TransportMode    = TRANSPORT_MODE_RAIL;
                break;

            default:
                pContextData->InternalData.TransportMode    = TRANSPORT_MODE_UNKNOWN;
                break;
            }

            if ( strlen( g_terminalType ) > 0 )
            {
                pDeviceType = g_terminalType;
            }
            else
            if ( pContextData->StaticData.isEntry == FALSE &&
                 pContextData->StaticData.isExit  == FALSE )
            {
                /*  HHD On-board validation - UNSUPPORTED! */
                CsErrx( "InitialiseBRContextFromCD : Unsupported On-board Validation for '%s'", sTransportMode );
            }
            else
            if ( pContextData->StaticData.isEntry != FALSE &&
                 pContextData->StaticData.isExit  != FALSE )
            {
                /*  Entry/Exit */
                switch ( cdTransportMode )
                {
                case MYKI_CD_MODE_BUS:
                case MYKI_CD_MODE_TRAM:
                    pDeviceType = "FPDm";
                    break;

                case MYKI_CD_MODE_RAIL:
                    pDeviceType = "FPDs";
                    break;

                default:
                    CsErrx( "InitialiseBRContextFromCD : Unexpected Entry/Exit for '%s'", sTransportMode );
                    break;
                }   /*  end-of-switch */
            }
            else
            {
                /*  Entry or Exit Only */
                switch ( cdTransportMode )
                {
                case MYKI_CD_MODE_BUS:
                case MYKI_CD_MODE_TRAM:
                    pDeviceType = "FPDm";
                    break;

                case MYKI_CD_MODE_RAIL:
                    pDeviceType = "FPDg";
                    break;

                default:
                    CsErrx( "InitialiseBRContextFromCD : Unexpected %s Only for '%s'",
                            ( pContextData->StaticData.isEntry != FALSE ? "Entry" : "Exit" ), sTransportMode );
                    break;
                }   /*  end-of-switch */
            }   /*  end-of-if */

            if ( pDeviceType != NULL )
            {
                /*  NOTE:   TDCSecondary and GAC terminal types are not defined in Tariff. */
                if ( strcmp( pDeviceType, "TDCSecondary" ) == 0 ) { pDeviceType = "TDC";  }
                if ( strcmp( pDeviceType, "GAC" )          == 0 ) { pDeviceType = "FPDm"; }

                CsDebug( APP_DEBUG_DETAIL, ( APP_DEBUG_DETAIL, "InitialiseBRContextFromCD : MYKI_CD_setDeviceParameters(%s, %d)", pDeviceType, cdTransportMode ) );
                MYKI_CD_setDeviceParameters( pDeviceType, cdTransportMode, 0, 0, 0, 0, 0, 0 );
            }   /*  end-of-if */
        }
        else
        {
            pContextData->InternalData.TransportMode    = TRANSPORT_MODE_UNKNOWN;
        }   /*  end-of-if */
    }   /*  end-of-if */

    MYKI_CD_getMinimumBalanceePass(&pContextData->Tariff.TPurseMinBalanceEPass);
    MYKI_CD_getMinimumBalanceStoredValue(&pContextData->Tariff.TPurseMinBalance);
    MYKI_CD_getePassLowWarningThreshold( &g_ePassLowWarningThreshold );
    MYKI_CD_getTPurseLowWarningThreshold( &g_TPurseLowWarningThreshold );
    MYKI_CD_getEndOfTransportDay(&pContextData->Tariff.endOfBusinessDay);
    MYKI_CD_getBlockingPeriodLLSC(&pContextData->Tariff.blockingPeriod);
    MYKI_CD_getChangeOfMindLLSC(&pContextData->Tariff.changeOfMindPeriod);
    MYKI_CD_getProvisionalFareMode(&pContextData->Tariff.provisionalCappingOption); // Provisional Fare Mode = Provisional Capping Option   
    MYKI_CD_getePassOutOfZone(&pContextData->Tariff.allowEPassOutOfZone);  
    MYKI_CD_getStationExitFee(&pContextData->Tariff.stationExitFee);
    MYKI_CD_getnHourPeriodMinutes(&pContextData->Tariff.nHourPeriodMinutes);
    MYKI_CD_getnHourRoundingPortion(&pContextData->Tariff.nHourRoundingPortion);
    MYKI_CD_getnHourExtendPeriodMinutes(&pContextData->Tariff.nHourExtendPeriodMinutes);
    MYKI_CD_getnHourExtendThreshold(&pContextData->Tariff.nHourExtendThreshold);
    MYKI_CD_getnHourMaximumDuration(&pContextData->Tariff.nHourMaximumDuration);
    MYKI_CD_getMaximumTPurseBalance(&pContextData->Tariff.TPurseMaximumBalance);
    MYKI_CD_getHeadlessModeRoute( pContextData->StaticData.serviceProviderId, &pContextData->Tariff.headlessRouteId );
    MYKI_CD_getMaximumTripTolerance(pContextData->StaticData.serviceProviderId, cdTransportMode, &pContextData->Tariff.maximumTripTolerance);  

    MYKI_CD_getAddValueEnabled( &pContextData->Tariff.addValueEnabled );
    MYKI_CD_getMinimumAddValue( &u32Value );
    pContextData->Tariff.minimumAddValue    = (Currency_t)u32Value;
    MYKI_CD_getMaximumAddValue( &u32Value );
    pContextData->Tariff.maximumAddValue    = (Currency_t)u32Value;
    
    s_HMILoadLogEntries  = MYKI_CD_getHMIMessaging((char*)"LoadLog", s_HMILoadLogArray, MAX_HMI_ARRAY_SIZE);
    s_HMIUsageLogEntries = MYKI_CD_getHMIMessaging((char*)"UsageLog", s_HMIUsageLogArray, MAX_HMI_ARRAY_SIZE);
}   /*  InitialiseBRContextFromCD( ) */

/*==========================================================================*
**
**  InitialiseCardProcessing
**
**  Description     :
**      Loads data from ini file and configures BR structures
**
** 
**==========================================================================*/
extern "C" void InitialiseCardProcessing()
{
    CsTime( &g_startTime );
    
    MYKI_BR_ContextData_t* pContextData = GetCardProcessingThreadContextData();
    memset(pContextData, 0, sizeof(*pContextData));
    char iniBuf[CSFINI_MAXVALUE + 1];
    
     CsDebug(APP_DEBUG_FLOW, (APP_DEBUG_FLOW, "Device ID %d", GetDeviceId())); 
     
     
     CsErrx("InitialiseDatabases failed");
  
	s_cardLibDebug = false;
	if ( CsfIniExpand("MykiApp Debug:CardLib", iniBuf, CSFINI_MAXVALUE) == 0 )
	{
		if ( (iniBuf[0] == 'y') || (iniBuf[0] == 'Y') )
			s_cardLibDebug = true;
	}

    if ( CsfIniExpand("General:TerminalId", s_terminalId, sizeof(s_terminalId)) != 0 )
    {
        CsWarnx("Terminal ID not present in configuration files");
        s_terminalId[0] = 0;
    }

    if ( CsfIniExpand( "General:TerminalType", g_terminalType, sizeof( g_terminalType ) ) != 0 )
    {
        CsWarnx( "TerminalType is not present in configuration files" );
        g_terminalType[ 0 ] = 0;
    }

    if ( CsfIniExpand("MykiApp General:CalculatePassExpiry", iniBuf, CSFINI_MAXVALUE) == 0 )
    {
        if ( (iniBuf[0] == 'y') || (iniBuf[0] == 'Y')  || (iniBuf[0] == '1'))
            g_calculatePassExpiry = true;
        else if ( (iniBuf[0] == 'n') || (iniBuf[0] == 'N')  || (iniBuf[0] == '0') )
            g_calculatePassExpiry = false;
    }

    s_AcsCompatibilityMode = FALSE;
    if ( CsfIniExpand("MykiApp General:AcsCompatibilityMode", iniBuf, CSFINI_MAXVALUE) == 0 )
    {
        if ( (iniBuf[0] == 'y') || (iniBuf[0] == 'Y') )
            s_AcsCompatibilityMode = TRUE;
    }

    s_gacAddress[0] = '\0';
    if ( CsfIniExpand("MykiApp General:GacAddress", s_gacAddress, sizeof(s_gacAddress)) == 0 )
    {
        CsDebug(APP_DEBUG_FLOW, (APP_DEBUG_FLOW, "GacAddress %s", s_gacAddress));
    }
    
    if ( s_gacAddress[ 0 ] == 'w' && s_gacAddress[ 1 ] == 's' )
    {
        s_isGACReader   = false;    // Address starts with ws (WebSocket) or wss (Secured WebSocket)
    }
    CsDebug( APP_DEBUG_FLOW, ( APP_DEBUG_FLOW, "GacAddress %s, %s reader functionality",
            s_gacAddress, s_isGACReader == true ? "GAC" : "DC" ) );
    
    s_alarmAddress[0] = '\0';
    if ( CsfIniExpand("MykiApp General:AlarmAddress", s_alarmAddress, sizeof(s_alarmAddress)) == 0 )
    {
        CsDebug(APP_DEBUG_FLOW, (APP_DEBUG_FLOW, "AlarmAddress %s", s_alarmAddress));
    }    
    
    s_gacCardFile[0] = '\0';
    if ( CsfIniExpand("MykiApp General:CardFile", s_gacCardFile, sizeof(s_gacCardFile)) == 0 )
    {
        CsDebug(APP_DEBUG_FLOW, (APP_DEBUG_FLOW, "CardFile %s", s_gacCardFile));
    }    

    if (CsfIniExpand("MykiApp General:UserIdFile", g_userIdFile, CSFINI_MAXVALUE) == 0)
    {
        CsDebug(APP_DEBUG_FLOW, (APP_DEBUG_FLOW, "User ID File: %s", g_userIdFile));
    }
  
#define SetNumericConfig(CFG, VALUE, DEFAULT) \
do { \
    if ( CsfIniExpand(CFG, iniBuf, sizeof(iniBuf)) == 0 ) \
    { \
        VALUE = atoi(iniBuf); \
    } \
    else \
    { \
        VALUE = DEFAULT; \
        CsWarnx("Could not read %s, setting \"%s\" to the default value of %d", CFG, #VALUE, DEFAULT); \
    } \
} while (0)
    SetNumericConfig( "MykiApp General:EntryPointId",               pContextData->DynamicData.entryPointId,         0                               );
    SetNumericConfig( "MykiApp General:LineId",                     pContextData->DynamicData.lineId,               0                               );
    SetNumericConfig( "MykiApp General:StopId",                     pContextData->DynamicData.stopId,               0                               );
    SetNumericConfig( "MykiApp General:AcquirerId",                 pContextData->StaticData.deviceAquirerId,       0                               );
    SetNumericConfig( "MykiApp General:ServiceProviderId",          pContextData->StaticData.serviceProviderId,     0                               );
    SetNumericConfig( "MykiApp General:ProductAutoloadEnabled",     pContextData->StaticData.productAutoEnabled,    0                               );
    SetNumericConfig( "MykiApp General:PurseAutoloadEnabled",       pContextData->StaticData.purseAutoEnabled,      0                               );
    SetNumericConfig( "MykiApp General:IsEntry",                    pContextData->StaticData.isEntry,               1                               );
    SetNumericConfig( "MykiApp General:IsExit",                     pContextData->StaticData.isExit,                0                               );
    SetNumericConfig( "MykiApp General:MaxPinRetries",              pContextData->StaticData.maxPinRetries,         DEFAULT_MaxPinRetries           );
    SetNumericConfig( "MykiApp General:CardRdFailWarn",             s_iniCardRdFailWarn,                            DEFAULT_CardRdFailWarn          );
    SetNumericConfig( "MykiApp General:CardRdFailError",            s_iniCardRdFailError,                           DEFAULT_CardRdFailError         );
    SetNumericConfig( "MykiApp General:CardWrFailWarn",             s_iniCardWrFailWarn,                            DEFAULT_CardWrFailWarn          );
    SetNumericConfig( "MykiApp General:CardWrFailError",            s_iniCardWrFailError,                           DEFAULT_CardWrFailError         );
    SetNumericConfig( "MykiApp General:SaveTxnDataFailWarn",        s_iniSaveTxnDataFailWarn,                       DEFAULT_SaveTxnDataFailWarn     );
    SetNumericConfig( "MykiApp General:SaveTxnDataFailError",       s_iniSaveTxnDataFailError,                      DEFAULT_SaveTxnDataFailError    );
    SetNumericConfig( "MykiApp General:NoTimeSyncWarn",             g_iniNoTimeSyncWarn,                            DEFAULT_NoTimeSyncWarn          );
    SetNumericConfig( "MykiApp General:NoTimeSyncError",            g_iniNoTimeSyncError,                           DEFAULT_NoTimeSyncError         );
    SetNumericConfig( "MykiApp General:NoTimeSyncCheck",            g_iniNoTimeSyncCheck,                           DEFAULT_NoTimeSyncCheck         );
    SetNumericConfig( "TPE:NumberOfHoursBetweenShifts", 	    pContextData->StaticData.numberOfHoursBetweenShifts,
                                                                                                                    0                               );
    SetNumericConfig( "TPE:MaxShiftDownTime",           	        pContextData->StaticData.maxShiftDownTime,	    0                               );
    SetNumericConfig( "TPE:HeadlessShiftID", 		                g_Headless_Shift_Id,				            0                               );
    SetNumericConfig( "TPE:CurrencyRoundingPortion",                g_initCurrencyRoundingPortion,                  DEFAULT_CurrencyRoundingPortion );
#undef SetNumericConfig

    CsVerbose("Terminal ID is %s", s_terminalId);
    pContextData->StaticData.deviceId = atoi(s_terminalId);

    if(MYKI_BR_Initialise(pContextData->DynamicData.entryPointId)==-1)
    {
        CsErrx("Failed to initialise BR for entry point %d", pContextData->DynamicData.entryPointId);

        // TODO Disable reader and raise alarm.
    }
    
    if ( CsfIniExpand( "MykiApp General:SAMSlot", g_iniSAMSlot, CSFINI_MAXVALUE ) != 0 )
    {
        strcpy( g_iniSAMSlot, DEFAULT_SAMSlot );
    }

    /*  Initialises TMI interface */
    TmiInit( );

    InitialiseBRContextFromCD(pContextData);

    pContextData->DynamicData.routeChangeover = ROUTE_CHANGEOVER_NONE;    
}


/*==========================================================================*
**
**  TerminateCardProcessing
**
**  Description     :
**      Ends card processing as part of the shutdown sequence
**
** 
**==========================================================================*/
extern "C" void TerminateCardProcessing()
{
    CardProcessingControl_t* pControl = GetCardProcessingControl();
    CsMutexLock(&pControl->lock);
    pControl->terminate = true;
    CsCondBcast(&(pControl->changed));
    CsMutexUnlock(&pControl->lock);

    IPC_NoPayload_t message;
    message.hdr.type = IPC_HEARTBEAT_REQUEST;
    message.hdr.source = APP_TASK;
    IpcSend(IpcGetID(APP_TASK), &message, sizeof(message));   
    
}

/*==========================================================================*
**
**  sendReaderState
**
**  Description     :
**      Send the reader state to DC
**
**==========================================================================*/
void sendReaderState(int outOfOrder)
{
    Json::Value             message;
    Json::FastWriter        fw;
    std::string             msg = "";

    message["name"]       = "readerstate";
    message["event"]      = "changeOutOfOrderState";
    message["type"]       = "SET";
    message["outOfOrder"] = outOfOrder; 
    message["terminalid"] = s_terminalId;

    msg = fw.write(message);
    putMessage(msg);
}

/*==========================================================================*
**
**  OnIdle
**
**  Description     :
**      Do when card processing thread is in idle state
**
**==========================================================================*/
void OnIdle()
{
    int outOfOrder = 0;
    MYKI_ALARM_AlarmListPair_t alarms[256];
    U32_t size = 256;
    U32_t i = 0;
    int ret = MYKI_ALARM_listSimpleAlarms(alarms, &size);
    if ( ret == 0 )
    {
        for(i=0;i<size;i++)
        {
            switch ( alarms[i].unitType )
            {
                case MYKI_ALARM_OPERATIONAL_STATE:
                    if(alarms[i].alarmId==100 || alarms[i].alarmId==140 || alarms[i].alarmId==200)
                    {
                        outOfOrder = 1;
                    }
                    break;
                case MYKI_ALARM_STORAGE_MEDIUM_STATE:
                    if(alarms[i].alarmId==20)
                    {
                        outOfOrder = 1;
                    }
                    break;
                case MYKI_ALARM_TIME_SYNCHRONIZATION_STATE:
                    if(alarms[i].alarmId==20)
                    {
                        outOfOrder = 1;
                    }
                    break;
                case MYKI_ALARM_TRANSACTION_LOG_STATE:
                    if(alarms[i].alarmId==20)
                    {
                        outOfOrder = 1;
                    }
                    break;
                default:
                    break;
            }
        }
    }
    if (g_cardProcessingControl.outOfOrder != outOfOrder || s_webSocketConnChanged)
    {
        if (s_webSocketConnected)
        {
            /*  Just connected/reconnected to DC */
            MYKI_BR_ContextData_t  *pData   = GetCardProcessingThreadContextData( );

            s_webSocketConnChanged = false;
            g_cardProcessingControl.outOfOrder = outOfOrder;
            sendReaderState(g_cardProcessingControl.outOfOrder);

            /*  Checks and commits period if required */
            CheckAndCommitPeriod( pData );
        }
    }

    CsTime_t now;
    CsTime(&now);
    if ( now.tm_secs < s_NTSEpoch )
    {
        CsErrx( "OnIdle : Current time of device is less than NTS EPOCH. Time = %d. Raising OperationalState-150", now.tm_secs );
        SetAlarm( AlarmOutOfServiceTimeSyncError );
    }
    else
    {
        ClearAlarm( AlarmOutOfServiceTimeSyncError );
    }
}

/*==========================================================================*
**
**  _SetCardProcessingState
**
**  Description     :
**      Set the card processing state 
**
**  Parameters      :    
**      newState [i] : new card processing state
**      locked   [i] : if not locked then use mutex on this call (HACK: to prevent deadlock)
**      reason   [i] : sub type of card processing state.
** 
**==========================================================================*/
extern "C" void _SetCardProcessingState(CardProcessingState_e newState, int locked, int reason)
{
    if ( !locked )
        CsMutexLock(&g_cardProcessingControl.lock);

    if ( g_cardProcessingControl.state != newState || reason!=g_cardProcessingControl.reason  )
    {
        CsDebug(1, (1, "_SetCardProcessingState reason %d", g_cardProcessingControl.reason));
        g_cardProcessingControl.state = newState;
        g_cardProcessingControl.reason = reason;
        CsCondBcast(&g_cardProcessingControl.changed);

        /*  Clears alarms (if required) */
        switch ( g_cardProcessingControl.state )
        {
        case    CARD_PROCESSING_STATE_INITIALISING:
            /*  Card processor is starting up. Unconditionally clears ALL alarms */
            /*  +   OperationalState alarms */
            ClearAlarm( AlarmOutOfServiceError              );
            ClearAlarm( AlarmOutOfServiceMaintenance        );
            ClearAlarm( AlarmOutOfServiceManual             );
            ClearAlarm( AlarmOutOfServiceRestarted          );
            ClearAlarm( AlarmOutOfServiceTimeSyncError      );
            ClearAlarm( AlarmOutOfServiceTariffError        );
            ClearAlarm( AlarmOutOfServiceSaveTxnDataError   );
            ClearAlarm( AlarmOutOfServiceSamKeyError        );
            /*  +   ECardEncoderState alarms */
            ClearAlarm( AlarmMCRWarn                        );
            ClearAlarm( AlarmMCRError                       );
            /*  +   ValidatorSystemState alarms */
            ClearAlarm( AlarmVALWarn                        );
            ClearAlarm( AlarmVALError                       );
            /*  +   TransactionLogState alarms */
            ClearAlarm( AlarmSaveTxnDataWarn                );
            ClearAlarm( AlarmSaveTxnDataError               );
            break;

        default:
            break;
        }   /*  end-of-switch */

        /*  And raises alarms (if required) */
        switch ( g_cardProcessingControl.state )
        {
        case    CARD_PROCESSING_STATE_WAITING_FOR_ENABLE:
            /*  Card processor is being disabled, ie. going out-of-service */
            /*  NOTE:   Only dealt with critical errors at this stage! */
            switch ( g_cardProcessingControl.reason )
            {
            case    CP_DISABLE_REASON_READER_ERROR:
/*              SetAlarm( AlarmOutOfServiceError            );  */
                SetAlarm( AlarmMCRError                     );
                SetAlarm( AlarmVALError                     );
                break;

            case    CP_DISABLE_REASON_SAM_ERROR:
                SetAlarm( AlarmOutOfServiceSamKeyError      );  
                SetAlarm( AlarmMCRError                     );
                SetAlarm( AlarmVALError                     );
                break;

            case    CP_DISABLE_REASON_SAVE_TXTDATA_ERROR:
                SetAlarm( AlarmOutOfServiceSaveTxnDataError );  
                SetAlarm( AlarmVALError                     );
                break;
            }   /*  end-of-switch */
            break;

        default:
            break;
        }   /*  end-of-switch */
    }

    if ( !locked )
        CsMutexUnlock(&g_cardProcessingControl.lock);
}

/*==========================================================================*
**
**  CommPrintf
**
**  Description     :
**      Debug callback function for card stack
**
**  Parameters      :    
**      level [i] : log level
**      message [i] : message to send to cs log
**==========================================================================*/
void CommPrintf(int level, const char* message)
{
    if ( s_cardLibDebug && (message != 0) )
    {
        char buf[256];
        char* translatedMessage = buf;
        unsigned int length = strlen(message);
        if ( length >= sizeof(buf) )
            translatedMessage = new char[length + 1];
        strcpy(translatedMessage, message);

        while ( (length > 0) && (isspace(translatedMessage[length-1])) )
            translatedMessage[--length] = '\0';

        switch ( level )
        {
        case COMM_DEBUG_LEVEL_ERROR:
            CsWarnx("%s", translatedMessage);
            break;
        case COMM_DEBUG_LEVEL_INFO:
            CsDebug(APP_DEBUG_DETAIL, (APP_DEBUG_DETAIL, "%s", translatedMessage));
            break;
        case COMM_DEBUG_LEVEL_DATA:
            CsDebug(APP_DEBUG_STATE, (APP_DEBUG_STATE, "%s", translatedMessage));
            break;
        default:
            CsDebug(APP_DEBUG_DETAIL, (APP_DEBUG_DETAIL, "%s", translatedMessage));
        }

        if ( translatedMessage != buf )
            delete[] translatedMessage;
    }
}

 /*==========================================================================*
**
**  CardStackPrintf
**
**  Description     :
**      Debug function for card stack
**
**  Parameters      :    
**      message [i] : string to output to cs log
**==========================================================================*/
void CardStackPrintf(const char* message)
{
    if ( s_cardLibDebug )
        CommPrintf(COMM_DEBUG_LEVEL_DATA, message);
}

 /*==========================================================================*
**
**  preValidateCard
**
**  Description     :
**      Attempts to open the card and find either an operator application
**      or a transit application
**
**  Parameters      :    
**      operatorCard [o] : set to true if detected card is an operator card
**  Returns         :
**     True  : current card can be opened and is a transit card or an operator card.
**
**  Notes           :
**      Card read failure alarms are cleared when card is validated.
**
**==========================================================================*/
bool preValidateCard(
    bool&               operatorCard,
    int                 nCardsDetected )
{
    bool            result      = false;
    int             csResult    = MYKI_CS_OK;

    operatorCard                = false;
    CsDebug( APP_DEBUG_FLOW, ( APP_DEBUG_FLOW, "CardProcessingThread: preValidateCard getting card info" ) );

    int res = MYKI_CS_GetCardInfo( &s_pCardInfo );

    if ( res == 0 )
    {
        if ( s_unconfirmed != FALSE )
        {
            if ( nCardsDetected > 1 )
            {
                brContextData.ReturnedData.rejectReason = MYKI_BR_REJECT_REASON_MULTIPLE_CARDS;
                return false;
            }   /*  end-of-if */

            /*  Attempts recovering from unconfirmed transaction */
            csResult    = MYKI_CS_ReOpenCard( );
            switch ( csResult )
            {
            case    MYKI_CS_OK:
                /*  Same card represented => OK to continue with card transaction */
                CsDebug( APP_DEBUG_FLOW, ( APP_DEBUG_FLOW, "CardProcessingThread: preValidateCard got same card" ) );
                operatorCard    = brContextData.DynamicData.isOperator;
                result          = true;
                break;

            case    MYKI_CS_ERR_DIFFERENT_CARD:
                /*  Different card presented - DROP THROUGH TO NEXT CASE! */
            case    MYKI_CS_ERR_NOT_SUPPORTED:
                /*  No pending ambiguous commit!? */
            default:
                /*  Que!? */
                CsDebug( APP_DEBUG_FLOW,
                    ( APP_DEBUG_FLOW, "CardProcessingThread: preValidateCard reopening card failed (%d)", csResult ) );

                /*  Saves unconfirmed LDTs */
                MYKI_LDT_Unconfirmed( &brContextData.InternalData.TransactionData );

                /*  Ends card session of unconfirmed transaction */
                MYKI_CS_CloseCard( );

                /*  And done with unconfirmed transaction! */
                s_unconfirmed   = false;
                break;
            }   /*  end-of-switch */
        }   /*  end-of-if */

        if ( result == false )
        {
            CsDebug( APP_DEBUG_FLOW, ( APP_DEBUG_FLOW, "CardProcessingThread: preValidateCard opening card" ) );
            U8_t    pAppList        = 0;
            int     handle          = MYKI_CS_OpenCard( MYKI_CS_OPEN_LIST_APPS, &pAppList );
            if ( handle >= 0 )
            {
                CsDebug( APP_DEBUG_DETAIL, ( APP_DEBUG_DETAIL, "CardProcessingThread: preValidateCard card handle=%d", handle ) );
                if ( pAppList & MYKI_CS_APP_BITMAP_OPERATOR )   // isoperator
                {
                    CsDebug( APP_DEBUG_STATE, ( APP_DEBUG_STATE, "CardProcessingThread: preValidateCard got operator card" ) );
                    operatorCard    = true;
                    result          = true;
                }
                else if ( pAppList & MYKI_CS_APP_BITMAP_TRANSIT )
                {
                    CsDebug( APP_DEBUG_STATE, ( APP_DEBUG_STATE, "CardProcessingThread: preValidateCard got transit card" ) );
                    result          = true;
                }
                else
                {
                    CsDebug( APP_DEBUG_STATE, ( APP_DEBUG_STATE, "CardProcessingThread: preValidateCard not a MYKI card" ) );
                }
            }
            else
            if ( MYKI_CS_DetectMultipleCardsPresent() > 1 )
            {
                brContextData.ReturnedData.rejectReason = MYKI_BR_REJECT_REASON_MULTIPLE_CARDS;
                return false;
            }
            else
            {
                CsWarnx( "CardProcessingThread: preValidateCard opening card failed: %d", handle );
            }
        }   /*  end-of-if */
    }
    else
    {
        CsWarnx("CardProcessingThread - get card info failed: %d", res);
    }

    if ( result == false )
    {
        /*  Failed reading card */
        SetAlarmCardRdFail( );
    }
    else
    {
        /*  Successfully read card => Will clear alarms when card is successfully validated */
        /*  DONOTHING! */
    }   /*  end-of-if */

    return result;
}

/*==========================================================================*
**
**  CardDetectWrapper
**
**  Description     :
**      Silences the debug output of curl
**
**  Parameters      :    
**
**  Returns         :
**     1 : Single card present
**     >1: Multiple cards present
**     0 : No cards present
**     <0: Device comms error
**==========================================================================*/
int CardDetectWrapper()
{
    int result = MYKI_CS_DetectCard(0); // Zero means do not poll twice with delay.
    if ( result < 0 )
    {
        CsDebug(APP_DEBUG_FLOW, (APP_DEBUG_FLOW, "CardProcessingThread: CardDetectWrapper: calling MYKI_CS_CloseCard (%d)", result));
/*      MYKI_CS_CloseCard(); */
        if ( result != CT_ERR_TIMEOUT )
            result = MYKI_CS_DetectCard(0);
    }

    return result;
}

static void ExecuteCardFunction(CardProcessingControl_t* args)
{
    // Run an arbitrary function, if requested
    if ( args->cardFunction != 0 )
    {
        CsDebug(APP_DEBUG_STATE, (APP_DEBUG_STATE, "ExecuteCardFunction: function at 0x%x", int(args->cardFunction)));
        int detectResult = 0;
        if ( args->cardFunctionSkipCardDetect == 0 )
        {
            while ( !args->terminate && ((detectResult = CardDetectWrapper()) <= 0) )
            {
                CsSleep(0, POLLING_DELAY);
            }
        }
        args->cardFunction(args->cardFunctionArgument);
        args->cardFunction = 0;
        args->cardFunctionArgument = 0;
        if ( args->cardFunctionSkipCardDetect == 0 )
        {
            while ( !args->terminate && ((detectResult = MYKI_CS_IsCardPresent()) > 0) )
            {
                CsSleep(0, POLLING_DELAY);
            }
        }
        args->cardFunctionSkipCardDetect = 0;
    }
}

/*==========================================================================*
**
**  compareFieldKey
**
**  Description     :
**      Helper function to compare field keys
**
**  Parameters      :
**      U8_t fieldKey    [I]     Transaction Type enumeration
**      char *cdFieldKey    [I]     Value from database either numeric, empty-string or /All
**
**  Returns         :
**      Boolean representing comparison value                           
**
**==========================================================================*/
bool compareFieldKey(U8_t fieldKey, char* cdFieldKey)
{
    U8_t value = -99;
    
    if(cdFieldKey[0]=='\0' || strcasecmp(cdFieldKey, "/Any")==0)
        return true;
        
    value = strtol(cdFieldKey, NULL, 10); 
    
    return fieldKey==value;    
}


/*==========================================================================*
**
**  getLoadLogTxType
**
**  Description     :
**      Helper function to return string describing the transaction type
**
**  Parameters      :
**      U8_t TxType    [I]     Transaction Type enumeration
**      U8_t PaymentMethod    [I]     Payment Method value from card
**      bool displayValue    [O]     Determines if the value of the transaction should be displayed on the GUI
**
**  Returns         :
**      String description (in english)                            
**
**  Notes           :
**      TODO move enumeration lookups from BR into myki-br library
**
**==========================================================================*/
std::string getLoadLogTxType(U8_t TxType, U8_t PaymentMethod, bool &displayValue)
{   
    U8_t FieldKey1 = TxType; 
    U8_t FieldKey2 = PaymentMethod; 

    if(s_HMILoadLogEntries>0)
    {
        for(int i =0; i<s_HMILoadLogEntries; i++)
        {
            if(
              compareFieldKey(FieldKey1, s_HMILoadLogArray[i].fieldKey1) &&
              compareFieldKey(FieldKey2, s_HMILoadLogArray[i].fieldKey2))
              {
                    displayValue = s_HMILoadLogArray[i].effect1;
                    return s_HMILoadLogArray[i].textToDisplay;
              }
        }
    }
    return "undefined";
}

/*==========================================================================*
**
**  getUsageLogTxType
**
**  Description     :
**      Helper function to return string describing the transaction type
**
**  Parameters      :
**      U8_t TxType    [I]     Transaction Type enumeration
**      U8_t PaymentMethod    [I]     Payment Method enumeration
**      bool displayValue    [O]     Determines if the value of the transaction should be displayed on the GUI
**
**  Returns         :
**      String description (in english)                            
**
**  Notes           :
**      TODO move enumeration lookups from BR into myki-br library
**
**==========================================================================*/
std::string getUsageLogTxType(U8_t TxType, U8_t FieldKey, bool &displayValue)
{

    enum
    {
        TAPP_USAGE_LOG_PAYMENT_METHOD_UNDEFINED = (0),      //< Undefined 
        TAPP_USAGE_LOG_PAYMENT_METHOD_TPURSE,               //< T-Purse 
        TAPP_USAGE_LOG_PAYMENT_METHOD_CASH,                 //< Cash 
        TAPP_USAGE_LOG_PAYMENT_METHOD_EFT_REFUNDABLE,       //< EFT (Refundable) 
        TAPP_USAGE_LOG_PAYMENT_METHOD_EFT_NONREFUNDABLE,    //< EFT (Non-refundable) 
        TAPP_USAGE_LOG_PAYMENT_METHOD_RECURRING_AUTOLOAD,   //< Recurring autoload 
        TAPP_USAGE_LOG_PAYMENT_METHOD_ADHOC_AUTOLOAD,       //< Ad-hoc autoload 
        TAPP_USAGE_LOG_PAYMENT_METHOD_CHEQUE,               //< Cheque 
        TAPP_USAGE_LOG_PAYMENT_METHOD_VOUCHER,              //< Voucher 
        TAPP_USAGE_LOG_PAYMENT_METHOD_TRANSFER,             //< Transfer 

    };
    
    U8_t FieldKey1 = TxType; 
    U8_t FieldKey2 = ((TAPP_USAGE_LOG_PROD_VAL_STATUS_ACTIVATE_BITMAP&FieldKey)!=0)    ?-1:0; 
    U8_t FieldKey3 = ((TAPP_USAGE_LOG_PROD_VAL_STATUS_SCAN_ON_BITMAP&FieldKey)!=0)     ?-1:0;
    U8_t FieldKey4 = ((TAPP_USAGE_LOG_PROD_VAL_STATUS_SCAN_OFF_BITMAP&FieldKey)!=0)    ?-1:0;
    U8_t FieldKey5 = ((TAPP_USAGE_LOG_PROD_VAL_STATUS_FORCED_BITMAP&FieldKey)!=0)      ?-1:0;
    U8_t FieldKey6 = ((TAPP_USAGE_LOG_PROD_VAL_STATUS_PROVISIONAL_BITMAP&FieldKey)!=0) ?-1:0;
    U8_t FieldKey7 = ((TAPP_USAGE_LOG_PROD_VAL_STATUS_OFFPEAK_BITMAP&FieldKey)!=0)     ?-1:0;
    U8_t FieldKey8 = ((TAPP_USAGE_LOG_PROD_VAL_STATUS_PREMIUM_BITMAP&FieldKey)!=0)     ?-1:0;
 
    if(s_HMIUsageLogEntries>0)
    {
        for(int i =0; i<s_HMIUsageLogEntries; i++)
        {
            if(
              compareFieldKey(FieldKey1, s_HMIUsageLogArray[i].fieldKey1) &&
              compareFieldKey(FieldKey2, s_HMIUsageLogArray[i].fieldKey2) &&
              compareFieldKey(FieldKey3, s_HMIUsageLogArray[i].fieldKey3) &&
              compareFieldKey(FieldKey4, s_HMIUsageLogArray[i].fieldKey4) &&
              compareFieldKey(FieldKey5, s_HMIUsageLogArray[i].fieldKey5) &&
              compareFieldKey(FieldKey6, s_HMIUsageLogArray[i].fieldKey6) &&
              compareFieldKey(FieldKey7, s_HMIUsageLogArray[i].fieldKey7) &&
              compareFieldKey(FieldKey8, s_HMIUsageLogArray[i].fieldKey8))
              {
                    displayValue = s_HMIUsageLogArray[i].effect1;
                    return s_HMIUsageLogArray[i].textToDisplay;
              }
        }
    }
    return "Undefined";
}

/*==========================================================================*
**
**  createKeyValuePair
**
**  Description     :
**      Helper function to create a json key value pair for use by the gac
**
**  Parameters      :
**      key    [I]     key
**      value  [I]     value
**
**  Returns         :
**      Json::Value                            
**
**  Notes           :
**
**
**==========================================================================*/
Json::Value createKeyValuePair(std::string key, std::string value)
{
    Json::Value returnValue;
    
    returnValue["key"] = key;
    returnValue["value"] = value;
    return returnValue;
}


/*==========================================================================*
**
**  validateCard
**
**  Description     :
**      Validates the presented card.
**
**  Parameters      :
**      numberOfCardsPresent    [I]     number of cards detected
**      operatorCard            [I]     true if operator; false otherwise
**      passengerCode           [O]     passenger code
**
**  Returns         :
**      true                            success
**      false                           failed
**
**  Notes           :
**
**
**==========================================================================*/

static char expiryString[33] = {'\0'};

static  bool    validateCard(
    int                     numberOfCardsPresent,
    bool                    operatorCard,
    int                    &passengerCode )
{
    return  ExecuteBusinessSequence( numberOfCardsPresent, operatorCard, passengerCode, SEQ_FUNCTION_DEFAULT, false );
}   /*  validateCard( ) */

bool            ExecuteBusinessSequence(
    int                     numberOfCardsPresent,
    bool                    operatorCard,
    int                    &passengerCode,
    SequenceFunction_e      businessSequence,
    bool                    forceProcessBR
    )
{
    static  const char             *months[ 12 ]            =
    {
        "Jan",  "Feb",  "Mar",  "Apr",  "May",  "Jun",
        "Jul",  "Aug",  "Sep",  "Oct",  "Nov",  "Dec"
    };  /*  months[ ] */

    bool                            result          = false;
    int                             brResult        = MYKI_BR_RESULT_ERROR;

    CsTime_t validateTime;
    CsTime( &validateTime );

    if ( !s_unconfirmed )
    {
        /*  PROCESSING NEW CARD TRANSACTION */
        memcpy( &brContextData, &g_defaultContextData, sizeof( brContextData ) );

        for (int i = 0; i < MAX_STAFF_ROLES ; i++)
        {
            brContextData.StaticData.staffRoles[i]          = g_staffRoles[i];
        }

        brContextData.StaticData.AcsCompatibilityMode       = s_AcsCompatibilityMode;
        brContextData.DynamicData.numberOfCardsPresented    = numberOfCardsPresent;
        brContextData.DynamicData.isOperator                = ( operatorCard ? TRUE : FALSE );
        brContextData.DynamicData.isTransit                 = ( operatorCard ? FALSE : TRUE );

        if ( brContextData.DynamicData.currentDateTime == 0 )
        {
            brContextData.DynamicData.currentDateTime       = validateTime.tm_secs;
        }
        time_t  nowTimeT                                    = (time_t)brContextData.DynamicData.currentDateTime;

        /* Determines the current business date */
        {
            struct tm  nowTm;
		    localtime_r( &nowTimeT, &nowTm );

            /* The start day is 1799 (DateC19_t) instead of 1899 (UTC) */
            Time_t  secondsSinceMidnight                    = ( (Time_t)nowTm.tm_hour * 60 * 60 ) +
                                                              ( (Time_t)nowTm.tm_min       * 60 ) +
                                                                (Time_t)nowTm.tm_sec;
            nowTm.tm_year                                   += ( 1900 - DATEC19_EPOCH );
            brContextData.DynamicData.currentBusinessDate   = mkdate( &nowTm );
            if ( secondsSinceMidnight < ( (Time_t)brContextData.Tariff.endOfBusinessDay * 60 ) )
            {
                /* Business day is the day before */
                brContextData.DynamicData.currentBusinessDate--;
            }   /* end-of-if */

        }

        CsDebug( APP_DEBUG_STATE, ( APP_DEBUG_STATE, "ExecuteBusinessSequence : brContextData.StaticData.AcsCompatibilityMode %s", brContextData.StaticData.AcsCompatibilityMode == TRUE ? "TRUE" : "FALSE" ) );
        CsDebug( APP_DEBUG_STATE, ( APP_DEBUG_STATE, "ExecuteBusinessSequence : brContextData.DynamicData.currentDateTime     %d", brContextData.DynamicData.currentDateTime                                ) );
        CsDebug( APP_DEBUG_STATE, ( APP_DEBUG_STATE, "ExecuteBusinessSequence : brContextData.DynamicData.currentBusinessDate %d", brContextData.DynamicData.currentBusinessDate                            ) );

        if(operatorCard || forceProcessBR) // Do not process BR on transit cards on GAC/DC Reader
        {
            StartCardSnapshot( &brContextData, NULL );

            /*  EXECUTE BUSINESS RULES */
            CsDebug( APP_DEBUG_FLOW,  ( APP_DEBUG_FLOW,  "ExecuteBusinessSequence : Calling MYKI_BR_Execute (Transit %d Operator %d)", brContextData.DynamicData.isTransit, brContextData.DynamicData.isOperator ) );
            DAF_TIMING_POINT_CAPTURE( DAF_BR_EXECUTION_START );
            {
                brResult    = MYKI_BR_ExecuteSequence( &brContextData, businessSequence );
            }
            DAF_TIMING_POINT_CAPTURE( DAF_BR_EXECUTION_FINISH );
            CsDebug( APP_DEBUG_FLOW,  ( APP_DEBUG_FLOW, "ExecuteBusinessSequence : MYKI_BR_Execute returned %d", brResult ) );

            /*  And clears test data for next card presentation */
            memset( &g_defaultContextData.TestData, 0, sizeof( MYKI_BR_TestData_t ) );
        }
        else
        {
           brResult = MYKI_BR_RESULT_SUCCESS;
        }            
    }
    else
    {
        /*  RECOVERING FROM UNCONFIRMED TRANSACTION */
        CsDebug( APP_DEBUG_FLOW,  ( APP_DEBUG_FLOW, "ExecuteBusinessSequence : Recovering from unconfirmed transaction" ) );
        brResult        = s_unconfirmedTxnBRResult;
        operatorCard    = brContextData.DynamicData.isOperator;
    }   /*  end-of-if */


    /*  FORMAT PRODUCT EXPIRY STRING */
    if (brContextData.ReturnedData.expiryDateTime>0)
    {
        time_t      expiryDateTime  = brContextData.ReturnedData.expiryDateTime;
        struct tm   expiryDate;

        CsDebug( APP_DEBUG_STATE, ( APP_DEBUG_STATE, "ExecuteBusinessSequence : brContextData.ReturnedData.expiryDateTime %d", brContextData.ReturnedData.expiryDateTime ) );
        localtime_r( &expiryDateTime, &expiryDate );
        snprintf( expiryString, sizeof( expiryString ), "%02d %s %02d %02d:%02d %s",
                expiryDate.tm_mday,
                months[ expiryDate.tm_mon ],
                expiryDate.tm_year % 100,
                expiryDate.tm_hour < 13 ? ( expiryDate.tm_hour ) : ( expiryDate.tm_hour % 12 ),
                expiryDate.tm_min,
                expiryDate.tm_hour < 12 ? "am" : "pm" );
    }

    int ldtResult = 0;

    if ( brResult == MYKI_BR_RESULT_ERROR )
    {
	CsDebug( APP_DEBUG_FLOW,  ( APP_DEBUG_FLOW, "ExecuteBusinessSequence : MYKI_BR_RESULT_ERROR" ) );
        SetAlarmCardRdFail( );

        MYKI_LDT_Rollback(&brContextData.InternalData.TransactionData);
        CommitCardSnapshots( CARD_SNAPSHOT_ERROR );
    }
    else
    {
        ClearAlarmCardRdFail( );
        if ( IS_DRIVER_CONSOLE_READER() )
        {
            /*  And commits/re-commits changes to card */
            if ( s_unconfirmed != false )
            {
                brContextData.InternalData.TransactionData.isReOpenCard = true;
            }   /*  end-of-if */
            ldtResult   = MYKI_LDT_Commit( &brContextData.InternalData.TransactionData );

            if ( ldtResult == LDT_SUCCESS                   ||
                 ldtResult == LDT_NO_COMMIT                 ||
                 ldtResult == LDT_UD_ERROR_SAVE_TRANSACTION )
            {
                if ( operatorCard || forceProcessBR )
                {
                    EndCardSnapshot( &brContextData );
                }
            }

            if ( ldtResult == LDT_UD_MULTIPLE_CARDS )
            {
                CsDebug( APP_DEBUG_STATE, ( APP_DEBUG_STATE, "ExecuteBusinessSequence : ldtResult = LDT_UD_MULTIPLE_CARDS" ) );

                /*  Multiple cards detected while committing card */
                brResult                                = MYKI_BR_RESULT_ERROR;
                brContextData.ReturnedData.rejectReason = MYKI_BR_REJECT_REASON_MULTIPLE_CARDS;

                /*  NOTE:   There seems to be issue detecting multiple cards when there is
                            one or more EMV cards in reader field. It is safer to abort unconfirmed
                            transaction recovery to force calling MYKI_CS_CloseCard() later and
                            restart card polling */
                if ( s_unconfirmed != false )
                {
                    /*  Saves transaction as "unconfirmed" */
                    MYKI_LDT_Unconfirmed( &brContextData.InternalData.TransactionData );

                    /*  And ends unconfirmed transaction recovery */
                    s_unconfirmed   = false;
                }
                else
                {
                    CommitCardSnapshots( CARD_SNAPSHOT_REMOVE );
                }   /*  end-of-if */
            }
            else
            if ( ldtResult != LDT_SUCCESS && ldtResult != LDT_NO_COMMIT)
            {
                s_unconfirmedTxnBRResult    = brResult;
                brResult                    = MYKI_BR_RESULT_ERROR;

                CsDebug( APP_DEBUG_STATE, ( APP_DEBUG_STATE, "ExecuteBusinessSequence : ldtResult = %d", ldtResult ) );

                if ( ldtResult == LDT_UD_UNCONFIRMED )
                {
                    CsWarnx( "ExecuteBusinessSequence : Unconfirmed card commit" );
                    s_unconfirmed   = true;

                    /*  Failed committing card */
                    SetAlarmCardWrFail( );
                }
                else
                {
                    s_unconfirmed   = false;

                    if ( ldtResult == LDT_UD_ERROR_SAVE_TRANSACTION )
                    {
                        /*  Failed saving (confirmed/unconfirmed) transaction data */
                        CsErrx( "ExecuteBusinessSequence : Failed saving transaction data" );

                        CommitCardSnapshots( CARD_SNAPSHOT_CONFIRMED );

                        s_SaveTxnDataFailures++;
                        if ( s_iniSaveTxnDataFailWarn > 0 && s_SaveTxnDataFailures >= s_iniSaveTxnDataFailWarn )
                        {
                            SetAlarm( AlarmSaveTxnDataWarn );
                        }   /*  end-of-if */
                        if ( s_iniSaveTxnDataFailError > 0 && s_SaveTxnDataFailures >= s_iniSaveTxnDataFailError )
                        {
                            SetAlarm( AlarmSaveTxnDataError );
                            /*  Too many consecutive save transaction data failures => go out-of-service */
                            SetCardProcessingState(
                                    CARD_PROCESSING_STATE_WAITING_FOR_ENABLE,
                                    FALSE /* RESOURCE_NOT_LOCKED */,
                                    CP_DISABLE_REASON_SAVE_TXTDATA_ERROR );
                        }   /*  end-of-if */
                    }
                    else
                    if ( ldtResult == LDT_UD_FAILED )
                    {
                        /*  Failed committing card */
                        SetAlarmCardWrFail( );
                        CommitCardSnapshots( CARD_SNAPSHOT_REMOVE );
                    }   /*  end-of-if */
                }   /*  end-of-if */

                brContextData.ReturnedData.rejectReason = MYKI_BR_REJECT_REASON_TOUCH_AGAIN;
            }
            else
            {
                /*  Successfully card commit */
                s_unconfirmed               = false;
                s_unconfirmedTxnBRResult    = MYKI_BR_RESULT_ERROR;
                CommitCardSnapshots( CARD_SNAPSHOT_CONFIRMED );
                ClearAlarmCardWrFail( );

                /*  And generates transaction TMI */
                if ( CreateTransactionTmi( &brContextData, businessSequence /* TODO */, false /*CONFIRMED*/ ) < 0 )
                {
                    CsErrx( "ExecuteBusinessSequence : failed generating transaction TMI record" );
                    ldtResult   = LDT_UD_ERROR_SAVE_TRANSACTION;

                    s_SaveTxnDataFailures++;
                    if ( s_iniSaveTxnDataFailWarn > 0 && s_SaveTxnDataFailures >= s_iniSaveTxnDataFailWarn )
                    {
                        SetAlarm( AlarmSaveTxnDataWarn );
                    }
                    if ( s_iniSaveTxnDataFailError > 0 && s_SaveTxnDataFailures >= s_iniSaveTxnDataFailError )
                    {
                        SetAlarm( AlarmSaveTxnDataError );
                        /*  Too many consecutive save transaction data failures => go out-of-service */
                        SetCardProcessingState(
                                CARD_PROCESSING_STATE_WAITING_FOR_ENABLE,
                                FALSE /* RESOURCE_NOT_LOCKED */,
                                CP_DISABLE_REASON_SAVE_TXTDATA_ERROR );
                    }
                }   /*  end-of-if */
            }   /*  end-of-if */
            if ( ldtResult != LDT_UD_ERROR_SAVE_TRANSACTION && ldtResult != LDT_NO_COMMIT)
            {
                /*  Clears failed saving transaction data alarm (if any) */
                s_SaveTxnDataFailures     = 0;
                ClearAlarm( AlarmSaveTxnDataError );
                ClearAlarm( AlarmSaveTxnDataWarn  );
                ClearAlarm( AlarmOutOfServiceSaveTxnDataError );  
            }   /*  end-of-if */
        }
	}

    if ( brResult == MYKI_BR_RESULT_SUCCESS )
    {
        result = true;

        passengerCode = brContextData.ReturnedData.passengerId;
    }
    else if ( numberOfCardsPresent > 1 )
    {
       result = false; 
    }

    return result;
}

/*==========================================================================*
**
**  eventCardPresented
**
**  Description     :
**      Create json image of card and send within a card presented message 
**      via curl to the GAC
**
**  Parameters      :
**      operatorCard    [I]     Flag indicating if this is an operator card
**      cardnumber      [I]     String containing the card number
**      validCard       [I]     If an operator card this flag will show that 
**                                BR executed successfully and the card is 
**                                valid for use.
**
**  Returns         :
**      String true on success                            
**
**  Notes           :
**
**
**==========================================================================*/

bool    eventCardPresented( bool operatorCard, const char* cardnumber, bool validCard )
{
    bool                returnValue         = false;
    int                 res                 = -1;
    int                 cardresult          = 0;
    MYKI_OAPIN_t       *pMYKI_OAPIN         = NULL;
    MYKI_OARoles_t     *pMYKI_OARoles       = NULL;
    MYKI_OAControl_t   *pMYKI_OAControl     = NULL;
    MYKI_CAIssuer_t    *pMYKI_CAIssuer      = NULL;
    MYKI_CAControl_t   *pMYKI_CAControl     = NULL;
    Json::Value         event;
	
    CsDbg( APP_DEBUG_FLOW, "eventCardPresented : is operator = %s", operatorCard ? "true" : "false" );

    if ( s_unconfirmed != false || IS_DRIVER_CONSOLE_READER() )
    {
        res = 0;
    }
    else
    {
        FILE *pFileOut = fopen(s_gacCardFile, "wt");
        if(pFileOut)
        {
            res = MYKI_FS_SerialiseToFile(pFileOut);  // Ensures that all sections of the card are in the cache and read in the correct order (and valid to an extent)
            fclose(pFileOut);
        }   
    }

    if ( res < 0 )
    {
        CsErrx( "MYKI_FS_SerialiseToFile failed. Error: %d", res );
    }
    else
    {
        event["name"]               = "cardevent";
        event["event"]              = "cardpresented";
        event["terminalid"]         = s_terminalId;
        event["data"]["type"]       = std::string(operatorCard==true?"operator":"transit");
        
        event["data"]["cardnumber"] = cardnumber;
        
        // Card serialised and in cache
        if ( operatorCard )
        {
            event["data"]["valid"]  = validCard;
            event["data"]["expiry"] = expiryString;
           
            if ( s_unconfirmed == false )
            {
                if ( ( cardresult = MYKI_CS_OAControlGet( &pMYKI_OAControl ) ) != MYKI_CS_OK || pMYKI_OAControl == NULL )
                {
                    CsErrx( "eventCardPresented : MYKI_CS_OAControlGet() failed (%d)", cardresult );
                    return  false;
                }

                event["data"]["serviceprovider"]    = U16ToString( pMYKI_OAControl->StaffProviderId );
                event["data"]["staffid"]            = std::string( reinterpret_cast<const char*>( pMYKI_OAControl->StaffId ) );
                
                CsDbg( APP_DEBUG_FLOW, "operator card staffexpiry check pMYKI_OAControl->StaffExpDate %d brContextData.DynamicData.currentBusinessDate %d",
                        pMYKI_OAControl->StaffExpDate, brContextData.DynamicData.currentBusinessDate );
				
				if ( ( cardresult = MYKI_CS_CAControlGet( &pMYKI_CAControl ) ) != MYKI_CS_OK || pMYKI_CAControl == NULL )
                {
                    CsErrx( "eventCardPresented : MYKI_CS_CAControlGet() failed (%d)", cardresult );
                    return  false;
                }

				/* Card Related Reject Reasons Take priority */
				if ( brContextData.ReturnedData.rejectReason == MYKI_BR_REJECT_REASON_EXPIRED)
				{
					if (pMYKI_CAControl->ExpiryDate < brContextData.DynamicData.currentBusinessDate)
					{
						brContextData.ReturnedData.rejectReason = MYKI_BR_REJECT_REASON_CARD_EXPIRED;
					}
					else if (pMYKI_OAControl->ExpiryDate < brContextData.DynamicData.currentBusinessDate)
				    {
					    brContextData.ReturnedData.rejectReason = MYKI_BR_REJECT_REASON_OPAP_EXPIRED;
				    }	
				}
				else if (pMYKI_CAControl->Status == CARD_CONTROL_STATUS_DISABLED) 
				{
					brContextData.ReturnedData.rejectReason = MYKI_BR_REJECT_REASON_DISABLED_CARD;
				}
				else if (pMYKI_CAControl->Status == CARD_CONTROL_STATUS_ISSUED || pMYKI_CAControl->Status == CARD_CONTROL_STATUS_INITIALISED ) 
				{
					brContextData.ReturnedData.rejectReason = MYKI_BR_REJECT_REASON_CARD_NOT_ACTIVATED;
				}
				
				/* App Related Reject Reasons Second Priority */
				else if (pMYKI_OAControl->Status == OAPP_CONTROL_STATUS_DISABLED )
				{
					brContextData.ReturnedData.rejectReason = MYKI_BR_REJECT_REASON_OPAP_DISABLED;
				}
				else if (pMYKI_OAControl->Status == OAPP_CONTROL_STATUS_ISSUED || pMYKI_OAControl->Status == OAPP_CONTROL_STATUS_INITIALISED ) 
				{
                    brContextData.ReturnedData.rejectReason = MYKI_BR_REJECT_REASON_OPAP_INACTIVE;
				}
				else if (pMYKI_OAControl->ExpiryDate < brContextData.DynamicData.currentBusinessDate)
				{
					brContextData.ReturnedData.rejectReason = MYKI_BR_REJECT_REASON_OPAP_EXPIRED;
				}	
				else
				{
					CsDebug(APP_DEBUG_FLOW, (APP_DEBUG_FLOW, "operator card staffexpiry check 2 pMYKI_OAControl->StaffExpDate %d brContextData.DynamicData.currentBusinessDate %d",pMYKI_OAControl->StaffExpDate,brContextData.DynamicData.currentBusinessDate));					
					if (pMYKI_OAControl->StaffExpDate < brContextData.DynamicData.currentBusinessDate)
					{
						brContextData.ReturnedData.rejectReason = MYKI_BR_REJECT_REASON_STAFF_EXPIRED;
						CsDebug(APP_DEBUG_FLOW, (APP_DEBUG_FLOW, "operator MYKI_BR_REJECT_REASON_STAFF_EXPIRED"));
					}
				}
				CsDebug(APP_DEBUG_FLOW, (APP_DEBUG_FLOW, "brContextData.ReturnedData.rejectReason %d",brContextData.ReturnedData.rejectReason));
				if ( validCard == true && brContextData.ReturnedData.rejectReason == 0 )
                {
					CsDebug(APP_DEBUG_FLOW, (APP_DEBUG_FLOW, "setting acceptReason"));
                    event["data"]["reason"] = brContextData.ReturnedData.acceptReason;
                }
                else
                {
					CsDebug(APP_DEBUG_FLOW, (APP_DEBUG_FLOW, "setting rejectReason"));
                    event["data"]["reason"] = brContextData.ReturnedData.rejectReason;
                }

                std::string csn = getMykiPAN( );
                event["data"]["CSN"] = csn.c_str( );

                if ( ( cardresult = MYKI_CS_OAPINGet( &pMYKI_OAPIN ) ) != MYKI_CS_OK || pMYKI_OAPIN == NULL )
                {
                    CsErrx( "eventCardPresented : MYKI_CS_OAPINGet() failed (%d)", cardresult );
                    return  false;
                }
                event["data"]["pin"] = std::string((char*)pMYKI_OAPIN->Pin);

                CsDbg( APP_DEBUG_FLOW, "eventCardPresented : operator card get roles" );
                if ( ( cardresult = MYKI_CS_OARolesGet( &pMYKI_OARoles ) ) != MYKI_CS_OK || pMYKI_OARoles == NULL )
                {
                    CsErrx( "eventCardPresented : MYKI_CS_OARolesGet() failed (%d)", cardresult );
                    return  false;
                }

                int roleIndex = 0;
                event["data"]["roles"] = Json::Value(Json::arrayValue);
                
                CsDebug(APP_DEBUG_FLOW, (APP_DEBUG_FLOW, "operator card got roles"));
                for(int role=0;role<MYKI_MAX_ROLES;role++)
                {
                    CsDebug(APP_DEBUG_FLOW, (APP_DEBUG_FLOW, "operator card type %d profile %d", pMYKI_OARoles->Role[role].Type, pMYKI_OARoles->Role[role].Profile));

                    if(pMYKI_OARoles->Role[role].Type!=0 && pMYKI_OARoles->Role[role].Profile!=0)
                    {                    
                        std::stringstream strType, strProfile;
                        strType    << (int)pMYKI_OARoles->Role[role].Type ;
                        strProfile << (int)pMYKI_OARoles->Role[role].Profile ;
                        
                        
                        event["data"]["roles"][roleIndex]["type"] = strType.str();
                        event["data"]["roles"][roleIndex]["profile"] = strProfile.str();
                        roleIndex++;
                    }
                }
            }
        }
        else
        if ( IS_DRIVER_CONSOLE_READER() )
        {
            CsDbg( APP_DEBUG_DETAIL, "eventCardPresented : skip sending card details" );
        }
        else // Process a transit card
        {            
            event["data"]["data"] = Json::Value(Json::arrayValue);
            Json::Value card,capping, transit, money,activeProduct, products, payments, usage, section;
            int sectionId = 0;
            Json::FastWriter fw; 
            
            // CARD ///////////////////////////////////////////////////////////
            card["id"]              = ++sectionId;
            card["label"]           = "Card";
            card["title"]           = "View Smartcard Contents";
            card["sectionData"]     = Json::Value(Json::arrayValue);
                    
            section = Json::Value();            
            section["section"] = Json::Value(Json::arrayValue);  // This should be simpler but it was easiest way to get ember.js to show the values without sorting them
            section["section"].append(createKeyValuePair("myki card number",     getMykiCardNumber())); // TODO Hex card number may need to be converted.
            section["section"].append(createKeyValuePair("myki money balance",     getMykiMoneyBalance()));
                
            section["section"].append(createKeyValuePair("Touched On",             getTouchedOn()));
            {
                std::string defaultFare = getOutstandingDefaultFare();
                if(defaultFare!="")section["section"].append(createKeyValuePair("Outstanding default fare",             defaultFare));
            }
            section["section"].append(createKeyValuePair("Surcharge",             getSurcharge()));
            section["section"].append(createKeyValuePair("Fare type",             getTransitPassengerCode()));
            section["section"].append(createKeyValuePair("Concession Expiry",     getTransitPassengerCodeExpiryDate()));
            section["section"].append(createKeyValuePair("Card Expiry",             getCardExpiryDate()));
            section["section"].append(createKeyValuePair("Card Status",             getCardApplicationStatus()));
            {
                std::string reason = getCardApplicationBlockingReason();
                if(reason!="") section["section"].append(createKeyValuePair("Blocking Reason",     reason));
            }    
            section["section"].append(createKeyValuePair("Transit App Expiry",     getTransitExpiryDate()));
            section["section"].append(createKeyValuePair("Transit App Status",     getTransitApplicationStatus()));            
            {
                std::string reason = getTransitApplicationBlockingReason();
                if(reason!="") section["section"].append(createKeyValuePair("Blocking Reason",     reason));            
            }    
            section["section"].append(createKeyValuePair("Registration Type",     getTransitApplicationRegistrationType()));            
            card["sectionData"].append(section);

            card["message"]["error"]    = "";
            card["message"]["text"]     = "";            
            event["data"]["data"].append(card);
            

            printf("card\n%s\n", fw.write(card).c_str());            

            // CAPPING ////////////////////////////////////////////////////////
            capping["id"]              = ++sectionId;
            capping["label"]           = "Capping";
            capping["title"]           = "Capping";
            capping["sectionData"]     = Json::Value(Json::arrayValue);
            
            
            // Daily
            section = Json::Value();
            section["section"] = Json::Value(Json::arrayValue);
            section["section"].append(createKeyValuePair("Capping",                "Daily"));
            section["section"].append(createKeyValuePair("Expiry",                 getDailyCappingExpiry()));
            section["section"].append(createKeyValuePair("Value",                  getDailyCappingValue()));
            {
                std::string zones = getDailyCappingZones();
                if(zones!="") section["section"].append(createKeyValuePair("Zone/s",                 zones));
            }
            capping["sectionData"].append(section);
            
            // Weekly
            section = Json::Value();
            section["section"] = Json::Value(Json::arrayValue);
            section["section"].append(createKeyValuePair("Capping",     "Weekly"));
            section["section"].append(createKeyValuePair("Expiry",                 getWeeklyCappingExpiry()));
            section["section"].append(createKeyValuePair("Value",                  getWeeklyCappingValue()));
            for(int i=0;i<5;i++)
            {
                std::string zones = getWeeklyCappingZones(i);
                if(zones!="") section["section"].append(createKeyValuePair("Zone/s", zones));
            }             
            capping["sectionData"].append(section);            
            
            section["message"]["error"]    = "";
            section["message"]["text"]     = "";
            event["data"]["data"].append(capping);

            // TRANSIT ////////////////////////////////////////////////////////
            transit["id"]              = ++sectionId;
            transit["label"]           = "Transit application";
            transit["title"]           = "Transit application";
            transit["sectionData"]     = Json::Value(Json::arrayValue);
            section = Json::Value();
            section["section"] = Json::Value(Json::arrayValue);
            
            section["section"].append(createKeyValuePair("Transit App expiry",                  getTransitExpiryDate()));
            section["section"].append(createKeyValuePair("Transit App status",                  getTransitApplicationStatus()));
            {
                std::string reason = getTransitApplicationBlockingReason();
                if(reason!="") section["section"].append(createKeyValuePair("Blocking Reason",     reason));            
            }  
            section["section"].append(createKeyValuePair("Fare type",                           getTransitPassengerCode()));
            section["section"].append(createKeyValuePair("Concession Expiry",                  getTransitPassengerCodeExpiryDate()));
            transit["sectionData"].append(section);

            section["message"]["error"]    = "";
            section["message"]["text"]     = "";
            event["data"]["data"].append(transit);

            
            // MYKI MONEY ////////////////////////////////////////////////////////
            money["id"]              = ++sectionId;
            money["label"]           = "myki money";
            money["title"]           = "myki money";
            money["sectionData"]     = Json::Value(Json::arrayValue);
            section = Json::Value();
            section["section"] = Json::Value(Json::arrayValue);
            
            section["section"].append(createKeyValuePair("myki money balance",      getMykiMoneyBalance()));
            section["section"].append(createKeyValuePair("Auto top up enabled",   getAutoloadEnabled()));
            section["section"].append(createKeyValuePair("Auto top up threshold", getAutoloadThreshold()));
            section["section"].append(createKeyValuePair("Auto top up amount",    getAutoloadAmount()));
            money["sectionData"][0u] = section;

            section["message"]["error"]    = "";
            section["message"]["text"]     = "";
            event["data"]["data"].append(money);            
                
            // PRODUCTS ////////////////////////////////////////////////////////
            products["id"]              = ++sectionId;
            products["label"]           = "Products";
            products["title"]           = "Products";
            products["sectionData"]     = Json::Value(Json::arrayValue);
            section = Json::Value();
            section["section"] = Json::Value(Json::arrayValue);

            {
                static MYKI_TAProduct_t *pMYKI_TAProduct;
                static MYKI_TAControl_t *pMYKI_TAControl;
                if(MYKI_CS_TAControlGet(&pMYKI_TAControl)>=0)
                {
                    for (int contractIndex=0; contractIndex<MYKI_MAX_CONTRACTS; contractIndex++)
                    {
                        // Only read and output existing products...
                        if (pMYKI_TAControl->Directory[contractIndex+1].Status != 0)
                        {
                            if (MYKI_CS_TAProductGet(contractIndex, &pMYKI_TAProduct)>=0) 
                            {
                                section = Json::Value();
                                section["section"] = Json::Value(Json::arrayValue);                            
                                section["section"].append(createKeyValuePair("Product Type",         getProductType(pMYKI_TAControl->Directory[contractIndex+1].ProductId, pMYKI_TAProduct->InstanceCount)));
                                section["section"].append(createKeyValuePair("Status",                 getProductStatus(pMYKI_TAControl->Directory[contractIndex+1].Status) ));
                                section["section"].append(createKeyValuePair("Start date",              TimeToString(pMYKI_TAProduct->StartDateTime)  ));
                                section["section"].append(createKeyValuePair("Expiry date",            TimeToString(pMYKI_TAProduct->EndDateTime)));    
                                section["section"].append(createKeyValuePair("Zone/s",                U8ToString(LogicalZoneToPhysicalZone(pMYKI_TAProduct->ZoneLow)) + " - " + U8ToString(LogicalZoneToPhysicalZone(pMYKI_TAProduct->ZoneHigh))));                    
                                section["section"].append(createKeyValuePair("Zone overlap",          ((pMYKI_TAProduct->ControlBitmap|BORDER_STATUS_INBOUND) || (pMYKI_TAProduct->ControlBitmap|BORDER_STATUS_OUTBOUND))?"true":"false" ));        
                                
                                section["section"].append(createKeyValuePair("Default Fare",             std::string((pMYKI_TAProduct->ControlBitmap&TAPP_TPRODUCT_CONTROL_PROVISIONAL_BITMAP) ? "Yes":"No")));
                                section["section"].append(createKeyValuePair("Off Peak",               std::string((pMYKI_TAProduct->ControlBitmap&TAPP_TPRODUCT_CONTROL_OFF_PEAK_BITMAP)      ? "Yes":"No")));
                                section["section"].append(createKeyValuePair("Surcharge",               std::string((pMYKI_TAProduct->ControlBitmap&TAPP_TPRODUCT_CONTROL_PREMIUM_BITMAP)      ? "Yes":"No")));


                                if(pMYKI_TAProduct->LastUsage.DateTime>0 && pMYKI_TAProduct->LastUsage.ProviderId>0)
                                {
                                    section["section"].append(createKeyValuePair("Last touched on/off",    TimeToString(pMYKI_TAProduct->LastUsage.DateTime)     ));
                                    section["section"].append(createKeyValuePair("Last usage mode",      getProviderMode(pMYKI_TAProduct->LastUsage.ProviderId)    ));
                                    section["section"].append(createKeyValuePair("Last usage provider",  getProviderName(pMYKI_TAProduct->LastUsage.ProviderId)      ));    
                                    section["section"].append(createKeyValuePair("Last usage zone",      "Zone " +  U8ToString(LogicalZoneToPhysicalZone(pMYKI_TAProduct->LastUsage.Zone))    ));                    
                                }

                                products["sectionData"].append(section);
                                
                            }
                        }
                    }
                }
            }            

            section["message"]["error"]    = "";
            section["message"]["text"]     = "";
            event["data"]["data"].append(products);        


            // ACTIVE PRODUCT /////////////////////////////////////////////////
            activeProduct["id"]              = ++sectionId;
            activeProduct["label"]           = "Product in use";
            activeProduct["title"]           = "Product in use";
            activeProduct["sectionData"]     = Json::Value(Json::arrayValue);
            
            {
                static MYKI_TAProduct_t *pMYKI_TAProduct;
                static MYKI_TAControl_t *pMYKI_TAControl;
                MYKI_Directory_t *pDirectory = NULL;
                if(MYKI_CS_TAControlGet(&pMYKI_TAControl)>=0)
                {
                    if (pMYKI_TAControl->ProductInUse > 0)
                    {
                        if ( myki_gac_GetCardProduct( pMYKI_TAControl->ProductInUse, &pDirectory, &pMYKI_TAProduct ) >=0  && pMYKI_TAProduct!=NULL)
                        {
                            section = Json::Value();
                            section["section"] = Json::Value(Json::arrayValue);                                            
                            section["section"].append(createKeyValuePair("Product Type",         getProductType(pMYKI_TAControl->Directory[pMYKI_TAControl->ProductInUse].ProductId, pMYKI_TAProduct->InstanceCount)));
                            section["section"].append(createKeyValuePair("Status",                 getProductStatus(pMYKI_TAControl->Directory[pMYKI_TAControl->ProductInUse].Status) ));
                            section["section"].append(createKeyValuePair("Start date",              TimeToString(pMYKI_TAProduct->StartDateTime)  ));
                            section["section"].append(createKeyValuePair("Expiry date",            TimeToString(pMYKI_TAProduct->EndDateTime)));    
                            section["section"].append(createKeyValuePair("Zone/s",                U8ToString(LogicalZoneToPhysicalZone(pMYKI_TAProduct->ZoneLow)) + " - " + U8ToString(LogicalZoneToPhysicalZone(pMYKI_TAProduct->ZoneHigh))));                    
                            section["section"].append(createKeyValuePair("Zone overlap",          ((pMYKI_TAProduct->ControlBitmap|BORDER_STATUS_INBOUND) || (pMYKI_TAProduct->ControlBitmap|BORDER_STATUS_OUTBOUND))?"true":"false" ));        
                            
                            section["section"].append(createKeyValuePair("Default Fare",           std::string((pMYKI_TAProduct->ControlBitmap&TAPP_TPRODUCT_CONTROL_PROVISIONAL_BITMAP) ? "Yes":"No")));
                            section["section"].append(createKeyValuePair("Off Peak",               std::string((pMYKI_TAProduct->ControlBitmap&TAPP_TPRODUCT_CONTROL_OFF_PEAK_BITMAP)    ? "Yes":"No") ));
                            section["section"].append(createKeyValuePair("Surcharge",              std::string((pMYKI_TAProduct->ControlBitmap&TAPP_TPRODUCT_CONTROL_PREMIUM_BITMAP)     ? "Yes":"No") ));
                            
                            if(pMYKI_TAProduct->LastUsage.DateTime>0 && pMYKI_TAProduct->LastUsage.ProviderId>0)
                            {
                                section["section"].append(createKeyValuePair("Last touched on/off",    TimeToString(pMYKI_TAProduct->LastUsage.DateTime)     ));
                                section["section"].append(createKeyValuePair("Last usage mode",      getProviderMode(pMYKI_TAProduct->LastUsage.ProviderId)    ));
                                section["section"].append(createKeyValuePair("Last usage provider",  getProviderName(pMYKI_TAProduct->LastUsage.ProviderId)      ));    
                                section["section"].append(createKeyValuePair("Last usage zone",       "Zone " + U8ToString(LogicalZoneToPhysicalZone(pMYKI_TAProduct->LastUsage.Zone))    ));                    
                            }              

                            activeProduct["sectionData"].append(section);
                        }
                    }
                }    
            }
            section["message"]["error"]    = "";
            section["message"]["text"]     = "";
            event["data"]["data"].append(activeProduct);                

            
            // PAYMENTS /////////////////////////////////////////////////
            payments["id"]              = ++sectionId;
            payments["label"]           = "Payment History";
            payments["title"]           = "Payment History";
            payments["sectionData"]     = Json::Value(Json::arrayValue);
            {
                static MYKI_TALoadLogRecordList_t    *pMYKI_TALoadLogRecordList;
                int recordCount = 0xFF;
                int res = MYKI_CS_TALoadLogRecordsGet(recordCount, &pMYKI_TALoadLogRecordList);
                if(res>=0)
                {
                    for(int recIx=0;recIx<pMYKI_TALoadLogRecordList->NumberOfRecords;  recIx++)
                    {                
                        section = Json::Value();
                        section["section"] = Json::Value(Json::arrayValue);      
                        bool displayValue = false;
                        section["section"].append(createKeyValuePair("Date",             TimeToString(pMYKI_TALoadLogRecordList->MYKI_TALoadLogRecord[recIx].TxDateTime)));
                        section["section"].append(createKeyValuePair("Description",      getLoadLogTxType(pMYKI_TALoadLogRecordList->MYKI_TALoadLogRecord[recIx].TxType, pMYKI_TALoadLogRecordList->MYKI_TALoadLogRecord[recIx].PaymentMethod, displayValue)));
                        
                        if(displayValue)
                        {
                            section["section"].append(createKeyValuePair("Value",            getTxTypeSigning(pMYKI_TALoadLogRecordList->MYKI_TALoadLogRecord[recIx].TxType) +  MoneyU32ToString(pMYKI_TALoadLogRecordList->MYKI_TALoadLogRecord[recIx].TxValue)));
                        }
                        section["section"].append(createKeyValuePair("Payment Method",   getPaymentMethod(pMYKI_TALoadLogRecordList->MYKI_TALoadLogRecord[recIx].PaymentMethod)));
                        
                        section["section"].append(createKeyValuePair("New myki money balance",           MoneyS32ToString(pMYKI_TALoadLogRecordList->MYKI_TALoadLogRecord[recIx].NewTPurseBalance)));                        
                        payments["sectionData"].append(section); 
                    }
                }
            }


            section["message"]["error"]    = "";
            section["message"]["text"]     = "";
            event["data"]["data"].append(payments);

            // USAGE /////////////////////////////////////////////////
            usage["id"]              = ++sectionId;
            usage["label"]           = "Travel history";
            usage["title"]           = "Travel history";
            usage["sectionData"]     = Json::Value(Json::arrayValue);
            {
                static MYKI_TAUsageLogRecordList_t    *pMYKI_TAUsageLogRecordList;
                int recordCount = 0xFF;
                int res = MYKI_CS_TAUsageLogRecordsGet(recordCount, &pMYKI_TAUsageLogRecordList);
                if(res>=0)
                {
                    for(int recIx=0; recIx<pMYKI_TAUsageLogRecordList->NumberOfRecords;  recIx++)
                    {
                        section = Json::Value();
                        section["section"] = Json::Value(Json::arrayValue);                            
                        bool displayValue = false; // Updated in getUsageLogTxType
                        section["section"].append(createKeyValuePair("Date",      TimeToString(pMYKI_TAUsageLogRecordList->MYKI_TAUsageLogRecord[recIx].TxDateTime)));
                        section["section"].append(createKeyValuePair("Description",      getUsageLogTxType(pMYKI_TAUsageLogRecordList->MYKI_TAUsageLogRecord[recIx].TxType, pMYKI_TAUsageLogRecordList->MYKI_TAUsageLogRecord[recIx].ProductValidationStatus, displayValue)));
                        section["section"].append(createKeyValuePair("Zone",      U8ToString(LogicalZoneToPhysicalZone(pMYKI_TAUsageLogRecordList->MYKI_TAUsageLogRecord[recIx].Zone))));
                        if(pMYKI_TAUsageLogRecordList->MYKI_TAUsageLogRecord[recIx].TxValue!=0 && displayValue)
                        {
                            section["section"].append(createKeyValuePair("New myki money balance",    MoneyS32ToString(pMYKI_TAUsageLogRecordList->MYKI_TAUsageLogRecord[recIx].NewTPurseBalance)));                        
                            section["section"].append(createKeyValuePair("Fares deducted for last trip",    getTxTypeSigning(pMYKI_TAUsageLogRecordList->MYKI_TAUsageLogRecord[recIx].TxType) +  MoneyU32ToString(pMYKI_TAUsageLogRecordList->MYKI_TAUsageLogRecord[recIx].TxValue))); 
                            
                            if(pMYKI_TAUsageLogRecordList->MYKI_TAUsageLogRecord[recIx].ProductSerialNo>0)
                            {
                                static MYKI_TAProduct_t *pMYKI_TAProduct;
                                static MYKI_TAControl_t *pMYKI_TAControl;
                                if(MYKI_CS_TAControlGet(&pMYKI_TAControl)>=0)
                                {
                                    
                                    for (int contractIndex=0; contractIndex<MYKI_MAX_CONTRACTS; contractIndex++)
                                    {
                                        if (pMYKI_TAControl->Directory[contractIndex+1].SerialNo==pMYKI_TAUsageLogRecordList->MYKI_TAUsageLogRecord[recIx].ProductSerialNo)
                                        {
                                            if (MYKI_CS_TAProductGet(contractIndex, &pMYKI_TAProduct)>=0) 
                                            {
                                                section["section"].append(createKeyValuePair("Value",    MoneyU32ToString(pMYKI_TAProduct->PurchaseValue)));                        
                                            }
                                        }
                                    }
                                }
                            }
                        }
                        usage["sectionData"].append(section);
                    }
                }
            }            

            section["message"]["error"]    = "";
            section["message"]["text"]     = "";
            event["data"]["data"].append(usage);        
        }
        
        {
            Json::FastWriter fw; 
             event["type"] = "PUT";
             g_json = fw.write(event);

            // Printf is useful here for debugging as CsDebug has a size limit on output.
         //   printf("card read\n%s\n", g_json.c_str());

            returnValue = true;
            uploadData(g_json.c_str(), g_json.size());

        }
    }
    
    return returnValue;
}

       
/*==========================================================================*
**
**  eventBadCardPresented
**
**  Description     :
**      Send a card removed message via curl to the GAC
**
**  Parameters      :
**      operatorCard    [I]     Flag indicating if this is an operator card
**      cardnumber      [I]     String containing the card number
**
**  Returns         :
**      String true on success                            
**
**  Notes           :
**
**
**==========================================================================*/

void eventBadCardPresented( bool operatorCard, const char* cardnumber )
{
    Json::Value         event;
    Json::FastWriter    fw; 
	MYKI_OAControl_t   *pMYKI_OAControl = NULL;
	MYKI_CAControl_t   *pMYKI_CAControl = NULL;
	int                 cardresult      = 0;

    CsDbg( APP_DEBUG_FLOW, "eventBadCardPresented : %s card", operatorCard ? "OPERATOR" : "TRANSIT" );

    event[ "name"                   ]   = "cardevent";
    event[ "terminalid"             ]   = s_terminalId;
    event[ "type"                   ]   = "PUT";
    event[ "event"                  ]   = "badcardpresented";
    event[ "data" ][ "type"         ]   = std::string( operatorCard == true ? "operator" : "transit" );
    event[ "data" ][ "cardnumber"   ]   = std::string( cardnumber );

    if ( brContextData.ReturnedData.rejectReason == MYKI_BR_REJECT_REASON_MULTIPLE_CARDS )
    {
        event[ "data" ][ "reason"           ]   = MYKI_BR_REJECT_REASON_MULTIPLE_CARDS;
        event[ "data" ][ "string_reason"    ]   = getRejectReasonString( MYKI_BR_REJECT_REASON_MULTIPLE_CARDS );
    }
    else
	if ( ( cardresult = MYKI_CS_CAControlGet( &pMYKI_CAControl ) ) == MYKI_CS_OK )
	{
		if ( pMYKI_CAControl->Status == CARD_CONTROL_STATUS_ISSUED ||
             pMYKI_CAControl->Status == CARD_CONTROL_STATUS_INITIALISED ) 
		{
			CsDbg( APP_DEBUG_FLOW, "MYKI_BR_REJECT_REASON_CARD_NOT_ACTIVATED" );
			event[ "data" ][ "reason"           ]   = MYKI_BR_REJECT_REASON_CARD_NOT_ACTIVATED;
            event[ "data" ][ "string_reason"    ]   = getRejectReasonString( MYKI_BR_REJECT_REASON_CARD_NOT_ACTIVATED );
		}
        else
        if ( ( cardresult = MYKI_CS_OAControlGet( &pMYKI_OAControl ) ) == MYKI_CS_OK )
        {
            if ( cardresult == TA_DESFIRE_ERR_NO_APP )
            {
                CsDbg( APP_DEBUG_FLOW, "MYKI_BR_REJECT_REASON_UNREADABLE" );
                event[ "data" ][ "reason"           ]   = MYKI_BR_REJECT_REASON_UNREADABLE;
                event[ "data" ][ "string_reason"    ]   = getRejectReasonString( MYKI_BR_REJECT_REASON_UNREADABLE );
            }
            else
            if ( cardresult == CRYPTO_AUTH_FAIL_ERR )
            {
                CsDbg( APP_DEBUG_FLOW, "MYKI_BR_REJECT_REASON_CRYPTO_AUTH_FAIL" );
                event[ "data" ][ "reason"           ]   = MYKI_BR_REJECT_REASON_CRYPTO_AUTH_FAIL;
                event[ "data" ][ "string_reason"    ]   = getRejectReasonString( MYKI_BR_REJECT_REASON_CRYPTO_AUTH_FAIL );
            }	
        }
	}
    std::string jsondata    = fw.write( event );
    uploadData( jsondata.c_str(), jsondata.size() );
}
     
/*==========================================================================*
**
**  eventCardRemoved
**
**  Description     :
**      Send a card removed message via curl to the GAC
**
**  Parameters      :
**      operatorCard    [I]     Flag indicating if this is an operator card
**      cardnumber      [I]     String containing the card number
**
**  Returns         :
**      String true on success                            
**
**  Notes           :
**
**
**==========================================================================*/
void eventCardRemoved(bool operatorCard, const char* cardnumber, bool goodcard)
{
    CsDebug(APP_DEBUG_FLOW, (APP_DEBUG_FLOW, "eventCardRemoved %s", operatorCard?"true":"false"));
    Json::Value event;
    Json::FastWriter fw; 
    event["name"]       = "cardevent";
    event["terminalid"] = s_terminalId;
    event["type"]       = "PUT";
    event["event"]      = "cardremoved";
    
    event["data"]["type"]       = std::string(operatorCard==true?"operator":"transit");
    event["data"]["cardnumber"] = std::string(cardnumber);
    
    std::string jsondata  = fw.write(event);
    uploadData(jsondata.c_str(), jsondata.size());    
        
    remove(s_gacCardFile);        
}


/*==========================================================================*
**
**  checkCDVersionUpdate
**
**  Description     :
**      Tests to see if the cd version is changed, raises an alarm if 
**       version is 0.0
**
**  Parameters      :
**      
**
**  Returns         :
**      true : cd version changed
**      false: cd version the same
**
**  Notes           :
**
**
**==========================================================================*/
bool checkCDVersionUpdate()
{
    bool            returnValue     = false;
    static int      cdMajorVersion  = (-1);
    static int      cdMinorVersion  = (-1);
    
    if ( (cdMajorVersion != MYKI_CD_getMajorVersion()) || (cdMinorVersion != MYKI_CD_getMinorVersion()) )
    {
        cdMajorVersion = MYKI_CD_getMajorVersion();
        cdMinorVersion = MYKI_CD_getMinorVersion();
        returnValue = true;
        
        /*  Performs Tariff sanity check */
        if ( (cdMajorVersion == 0 && cdMinorVersion == 0) || MYKI_CD_validateTariff()==false)
        {
            /*  Raises invalid Tariff alarms */
            SetAlarm( AlarmOutOfServiceTariffError      );
            SetAlarm( AlarmVALErrorTariff               );
            SetAlarm( AlarmOutOfServiceDatabaseError ); // MBU-1560 Raise OpState-10 alarm if failed to load DB or roles file on update.
        }
        else
        {
            /*  Clears invalid Tariff alarms */
            ClearAlarm( AlarmOutOfServiceTariffError    );
            ClearAlarm( AlarmVALErrorTariff             );
            ClearAlarm( AlarmOutOfServiceDatabaseError );
        }   /*  end-of-if */        
    }
    
    return returnValue;
}

/*==========================================================================*
**
**  processCardInstruction
**
**  Description     :
**      Test for new instruction and execute it on the card
**
**  Parameters      :
**      pCardProcessingControl  [I]     card processing control
**      canProcCard             [I/O]   card process card
**
**  Returns         :
**      None
**
**  Notes           :
**      Card related function can only be executed once.
**
**==========================================================================*/

void    processCardInstruction(
    CardProcessingControl_t    *pCardProcessingControl,
    bool                       &canProcCard )
{
    Json::Value     request;
    Json::Reader    reader;
    std::string     msg             = getMessage( );
    std::string     response        = "";

    /*  NOTE:   Card function can only be executed while (good) card is
                still within reader field. */
    if ( pCardProcessingControl->state != CARD_PROCESSING_STATE_WAITING_FOR_GOOD_CARD_REMOVAL )
    {
        canProcCard = false;
    }

    if ( msg.empty() == false )
    {
        CsDebug( APP_DEBUG_FLOW, ( APP_DEBUG_FLOW, "processCardInstruction : got message %s", msg.c_str() ) );

        /*  Parses JSON request message */
        if ( reader.parse( msg.c_str(), request ) && request.size() > 0 ) // Must check size otherwise library can segfault
        {
            std::string name        = getJsonString( request, "name"       );
            std::string terminalId  = getJsonString( request, "terminalid" );
            bool        isForMe     = terminalId.compare( s_terminalId ) == 0 ? true : false;

            CsDebug( APP_DEBUG_FLOW, ( APP_DEBUG_FLOW, "processCardInstruction : parsed message" ) );
            CsDebug( APP_DEBUG_FLOW, ( APP_DEBUG_FLOW, "processCardInstruction : process '%s'", name.c_str() ) );

            /*  Performs requested card function */
            if      ( canProcCard == true && isForMe == true && name.compare( "changepin"            ) == 0 ) { response = processChangePIN(            request ); }
            else if ( canProcCard == true && isForMe == true && name.compare( "validatepin"          ) == 0 ) { response = processValidatePIN(          request ); }
            else if ( canProcCard == true && isForMe == true && name.compare( "tpurseload"           ) == 0 ) { response = processTPurseLoad(           request ); canProcCard = false; }
            else if ( canProcCard == true && isForMe == true && name.compare( "tpurseloadreversal"   ) == 0 ) { response = processTPurseLoadReversal(   request ); canProcCard = false; }
            else if ( canProcCard == true && isForMe == true && name.compare( "validatetransitcard"  ) == 0 ) { response = processValidateTransitCard(  request ); canProcCard = false; }
            else if (                        isForMe == true && name.compare( "countpassenger"       ) == 0 ) { response = processPassengerCount(       request ); }
            else if (                                           name.compare( "logon"                ) == 0 ) { response = processLogon(                request ); }
            else if (                                           name.compare( "tablecard"            ) == 0 ) { response = processTableCard(            request ); }
            else if (                                           name.compare( "trip"                 ) == 0 ) { response = processTrip(                 request ); }
            else if (                                           name.compare( "shiftend"             ) == 0 ) { response = processShiftEnd(             request ); }
            else if (                                           name.compare( "logoff"               ) == 0 ) { response = processLogOff(               request ); }
            else if (                        isForMe == true && name.compare( "getdrivertotals"      ) == 0 ) { response = processGetDriverTotals(      request ); }
            else if (                                           name.compare( "periodcommitcheck"    ) == 0 ) { response = processPeriodCommitCheck(    request ); }
            else if (                                           name.compare( "productsales"         ) == 0 ) { response = processProductSales(         request ); canProcCard = false; }
            else if (                                           name.compare( "productsalesreversal" ) == 0 ) { response = processProductSalesReversal( request ); canProcCard = false; }
            else
            {
                CsErrx( "processCardInstruction : discarded '%s'", name.c_str() );
                if
                (
                    canProcCard == false && isForMe == true &&
                    (
                        name.compare( "tpurseload"          ) == 0 ||
                        name.compare( "tpurseloadreversal"  ) == 0
                    /*  name.compare( "validatetransitcard" ) == 0 */
                    )
                )
                {
                    /*  Lets originator know card command has been discarded */
                    response    = processCardRemoved( request );
                }
            }
            
            /*  And posts JSON response message */
            if ( response.empty() == false )
            {
                putMessage( response );
            }
        }
    }
}

bool dirExists(const char* dir)
{
    struct stat results;
    return ((stat(dir, &results) == 0) && (S_ISDIR(results.st_mode)));
}

/*==========================================================================*
**
**  exec
**
**  Description     :
**      Execute system command
**
**  Parameters      :
**      system command  
**
**  Returns         :
**      system output                                
**
**  Notes           :
**      
**
**==========================================================================*/
std::string exec(const char* cmd)
{
    FILE* pipe = popen(cmd, "r");
    if (!pipe) return "";
    char buffer[128];
    std::string result = "";
    while (!feof(pipe)) {
        if (fgets(buffer, 128, pipe) != NULL)
            result += buffer;
    }
    pclose(pipe);
    return result;
}

/*==========================================================================*
**
**  CardProcessingThread
**
**  Description     :
**      Main processing thread
**
**  Parameters      :
**      arg    [I]     
**
**  Returns         :
**                                 
**
**  Notes           :
**
**
**==========================================================================*/
extern "C" void* CardProcessingThread(void* arg)
{
    CardProcessingControl_t    *args        = reinterpret_cast<CardProcessingControl_t*>(arg);
    bool                        canProcCard = false;

   /* Initialise the card stack */
#ifdef DEBUG
    CommSetDebugFn(CommPrintf);
#endif

    CT_Config_t ctConfig;
    memset(&ctConfig, 0, sizeof(ctConfig));
    ctConfig.MultipleReadersDetectConfig = CT_MULTIPLE_READERS_FIRST_DETECTION_ONLY;
    ctConfig.eventHandlerTaskPriority    = 0;
    ctConfig.pollTaskPriority            = 0;
    ctConfig.CT_DetectCardPingConfig     = CT_DETECTCARD_PING_AFTER_NOTIFY_DETECTED;

    CT_DeviceParameters_t ctDeviceParameters;
    memset(&ctDeviceParameters, 0, sizeof(ctDeviceParameters));
    HCM_DeviceParameters_t hcmDeviceParameters = {0};

    ctDeviceParameters.MCR_MIFARE.isMCRLocal          = TRUE;
    hcmDeviceParameters.pRequestedHCMTargetDeviceName = g_iniSAMSlot;
    hcmDeviceParameters.UseHCMSerialLink              = 1;

    MYKI_CS_Init_Params_t initParams             = {0};
    initParams.pCT_Config                        = NULL;
    initParams.pCT_DeviceParameters              = &ctDeviceParameters;
    initParams.pHCM_DeviceParameters             = &hcmDeviceParameters;
    initParams.CT_CardHandle                     = 0;

    initParams.pHostMCRSerialComPortBaudRateList = NULL;
    static char cardReaderDeviceDescriptor[]     = "vipermcr1";

    initParams.pCardReaderDeviceDescriptor       = cardReaderDeviceDescriptor;
    initParams.pMYKI_CS_DiagTracePrintFn         = CardStackPrintf;/*IN: Pointer to MYKI_CS diagnostic trace print function*/
    initParams.MYKI_CS_DiagTraceLevel            = (s_cardLibDebug ? MYKI_CS_DIAG_TRACE_ALL : MYKI_CS_DIAG_TRACE_DISABLED);/*IN: MYKI_CS diagnostic trace level*/
    initParams.pCT_CommsTracePrintFn             = CardStackPrintf;/*IN: Pointer to CT comms trace print function*/

    int ctRet = CT_MCR_RegisterDeviceDriver();
    if ( ctRet < 0 )
    {
        CsErrx("CardProcessingThread - CT_MCR_RegisterDeviceDriver returned %d", ctRet);
    }
    else if ( (ctRet = CT_Init(&ctConfig)) < 0 )
    {
        CsErrx("CardProcessingThread - CT_Init returned %d", ctRet);
    }
    else if ( (ctRet = CT_ViperMCR_RegisterLocalReaderCommsDriver()) < 0 )
    {
        CsErrx("CardProcessingThread - CT_ViperMCR_RegisterLocalReaderCommsDriver returned %d", ctRet);
    }
    else if ( (ctRet = CT_OpenDevice(initParams.pCardReaderDeviceDescriptor, CT_DEV_MCR, initParams.pCT_DeviceParameters, NULL)) < 0 )
    {
        // CT Open Device can fail if the process is killed and restarted quickly by process manager
        CsErrx("CardProcessingThread - CT_OpenDevice returned %d. retry in 3 seconds", ctRet);
        CsSleep(3, 0);
        if ( (ctRet = CT_OpenDevice(initParams.pCardReaderDeviceDescriptor, CT_DEV_MCR, initParams.pCT_DeviceParameters, NULL)) < 0 )
        {
            CsErrx("CardProcessingThread - CT_OpenDevice returned %d", ctRet);
        }
    }
    
    if(ctRet>=0)
    {
        initParams.CT_CardHandle = ctRet;
    }

    /*Set initial OutOfOrderState*/
    g_cardProcessingControl.outOfOrder = 0;
    if(!dirExists(LDT_UD_PENDING_PATH) || !dirExists(LDT_UD_CONFIRMED_PATH))
    {
        CsErrx("Missing directory '%s' and/or '%s' setting intial outOfOrder state.");
        g_cardProcessingControl.outOfOrder = 1;
    }

    if ( DAF_FAILED( ctRet ) )
    {
        CsErrx("Disable reader due to DAF failure %d", ctRet);
        SetCardProcessingState(
                CARD_PROCESSING_STATE_WAITING_FOR_ENABLE,
                FALSE /* NOT_LOCKED */,
                CP_DISABLE_REASON_READER_ERROR );
    }
    else
    {
        int ret = MYKI_CS_Initialise(&initParams);

        if ( IsVirtualReader != FALSE )
        {
            /*  NOTE:   In virtual reader, SAM initialisation always fails with COMM_ERR error! */
            if ( ret == COMM_ERR )
            {
                CsDebug( APP_DEBUG_FLOW, ( APP_DEBUG_FLOW, "CardProcessingThread: ignored SAM initialisation failure" ) );
                ret = MYKI_CS_OK;
            }
            /*  Saves SAMID buffer address for later used to load virtual card image */
            g_pKSAM_ID  = &initParams.KSAM_ID_Report.ESN[ 0 ];
        }

        if ( DAF_FAILED( ret ) )
        {
            switch ( ret )
            {
            case    COMM_ERR:
            case    MYKI_CS_ERR_OUT_OF_MEMORY:
            case    MYKI_CS_ERR_KSAM_INVALID_NO_KEYS_FOUND:
                SetCardProcessingState(
                        CARD_PROCESSING_STATE_WAITING_FOR_ENABLE,
                        FALSE /* NOT_LOCKED */,
                        CP_DISABLE_REASON_SAM_ERROR );
                break;

            default:
                /*  Others! */
                SetCardProcessingState(
                        CARD_PROCESSING_STATE_WAITING_FOR_ENABLE,
                        FALSE /* NOT_LOCKED */,
                        CP_DISABLE_REASON_READER_ERROR );
                break;
            }   /* end-of-switch */

            CsErrx("CardProcessingThread: card stack initialise failed, returned %d", ret);
            CsErrx("CardProcessingThread: last APDU status=0x%02x%02x", initParams.APDUResponseStatus.SW1, initParams.APDUResponseStatus.SW2);
        }
        else
        {
            CsDebug(APP_DEBUG_FLOW, (APP_DEBUG_FLOW, "CardProcessingThread: card stack successfully initialised"));

            // Get the SAM ID
            MYKI_BR_ContextData_t* pContextData = GetCardProcessingThreadContextData();
            
            pContextData->StaticData.deviceSamId    = ( (U32_t)initParams.KSAM_ID_Report.ESN[ 0 ] << 24 ) |
                                                      ( (U32_t)initParams.KSAM_ID_Report.ESN[ 1 ] << 16 ) |
                                                      ( (U32_t)initParams.KSAM_ID_Report.ESN[ 2 ] <<  8 ) |
                                                        (U32_t)initParams.KSAM_ID_Report.ESN[ 3 ];

            CsDebug(APP_DEBUG_FLOW, (APP_DEBUG_FLOW, "CardProcessingThread: samId = 0x%08X", pContextData->StaticData.deviceSamId));
        }

        if ( s_cardLibDebug )
            (void)MYKI_CS_Enable_CT_CommsTrace();
        else
            MYKI_CS_Disable_CT_CommsTrace();
    }   /* end-of-if */    
    
    int detectResult = 0;

    if ( initDataTransfer( s_alarmAddress, s_gacAddress, args ) != 0 )
    {
        /*  TODO:   Terminate reader application? */
    }
    initCardSnapShot( );

    DAF_TimingPoint_EnableCapture(1);
    
    checkCDVersionUpdate();

    TimeSyncCheck( 0 /*INITIAL-CALL*/ );
    CloseAndOpenShift( );
    
    /* Card processing loop */
    while ( !args->terminate )
    {
        if ( IsVirtualReader != FALSE )
        {
            /*  CARD_PROCESSING_STATE_WAITING_FOR_ENABLE - Waiting for card reader to be enabled */
            CsMutexLock( &args->lock );
            {
                if ( ! args->cardDetectEnabled &&
                     ! args->operatorCardDetectEnabled )
                {
                    CsDebug( APP_DEBUG_STATE,
                           ( APP_DEBUG_STATE, "CardProcessingThread: waiting to be enabled, cardDetectEnabled=%d, operatorCardDetectEnabled=%d",
                                (int)args->cardDetectEnabled,
                                (int)args->operatorCardDetectEnabled ) );
                    SetCardProcessingState( CARD_PROCESSING_STATE_WAITING_FOR_ENABLE, TRUE, g_cardProcessingControl.reason );
                    while
                    (
                        ! args->terminate &&
                        ! args->cardDetectEnabled &&
                        ! args->operatorCardDetectEnabled
                    )
                    {
                        if ( args->cardFunction == NULL )
                        {
                            CsTime_t    waitTm  = { 0 };

                            /*  Processes IDLE state */
                            OnIdle( );

                            /*  And waits (with time out) for change of state */
                            CsTime( &waitTm );
                            waitTm.tm_secs     += 1 /*SEC*/;
                            CsCondWaittm( &args->changed, &args->lock, &waitTm );
                        }
                        ExecuteCardFunction( args );
                    }
                }
            }
            CsMutexUnlock( &args->lock );
        }

        /* Wait for a card to be presented */
        CsMutexLock(&args->lock);
        if ( args->state != CARD_PROCESSING_STATE_WAITING_FOR_CARD )
        {
            CsDebug(APP_DEBUG_STATE, (APP_DEBUG_STATE, "CardProcessingThread: waiting for a card"));
            SetCardProcessingState(CARD_PROCESSING_STATE_WAITING_FOR_CARD, TRUE, 0);
        }

        while
            (
                !args->terminate &&
                (args->state == CARD_PROCESSING_STATE_WAITING_FOR_CARD) &&
                ((detectResult = (detectResult <= 0 ? CardDetectWrapper() : detectResult)) <= 0)
            )
        {
            {
                // block here until the callback thread wakes us up / time to poll
                if ( args->cardFunction == NULL )
                {
                    if ( IS_DRIVER_CONSOLE_READER() )
                        OnIdle( );
                    // Update the CD set in use
                    if ( g_defaultContextData.DynamicData.currentDateTime == 0 )
                    {
                        CsTime_t now;
                        CsTime(&now);
                        MYKI_CD_useCurrentVersion(now.tm_secs);
                    }
                    else
                    {
                        MYKI_CD_useCurrentVersion(g_defaultContextData.DynamicData.currentDateTime);
                    }
                    
                    if(checkCDVersionUpdate())
                    {
                        InitialiseBRContextFromCD(&g_defaultContextData);
                    }
                    
                    CsMutexUnlock(&args->lock);
                    CsSleep(0, POLLING_DELAY);
                    CsMutexLock(&args->lock);

                    if ( IS_DRIVER_CONSOLE_READER() )
                    {
                        canProcCard = false;
                        processCardInstruction( args, canProcCard );
                    }
                }               
            }
            ExecuteCardFunction( args );
        }

        CsMutexUnlock(&args->lock);

        if ( IsVirtualReader != FALSE )
        {
            if ( args->state == CARD_PROCESSING_STATE_WAITING_FOR_ENABLE )
            {
                CsDebug( APP_DEBUG_FLOW,
                       ( APP_DEBUG_FLOW, "CardProcessingThread: no longer ready, skip card processing, jump to waiting for enable" ) );
                detectResult    = 0;
                continue;
            }
        }

        /* Card should have been presented now */
        if ( detectResult <= 0 )
            detectResult = CardDetectWrapper();
        if ( detectResult > 0 )
        {
            DAF_TIMING_POINT_CAPTURE (APPLICATION_TRANSACTION_TOTAL_START);
            CsDebug(APP_DEBUG_FLOW, (APP_DEBUG_FLOW, "CardProcessingThread: processing - detected %d card%s", detectResult, (detectResult > 1 ? "s" : "")));
            CsTime_t processingStartTime, processingEndTime;
            CsTime(&processingStartTime);
            
            bool operatorCard      = false;
            bool prevalidateResult = false;

            char cardUID[32];
            cardUID[0] = '\0';
            prevalidateResult = preValidateCard( operatorCard, detectResult );
            if(s_pCardInfo!=NULL)
            {
                char* t = cardUID;
                for(int i=0; i < s_pCardInfo->SerialLength ;i++)
                {
                    t += sprintf(t,"%02X", s_pCardInfo->SerialNumber[i] );
                }
            }
            if(prevalidateResult==false)
            {
                eventBadCardPresented(operatorCard, cardUID);
                SetCardProcessingState(CARD_PROCESSING_STATE_WAITING_FOR_BAD_CARD_REMOVAL, FALSE, 0);
            }
            else
            {
                bool validCard=false;
                
                CsDebug(APP_DEBUG_FLOW, (APP_DEBUG_FLOW, "CardProcessingThread clear instruction queue"));

                CsTime( &g_lastTransactionTime );
                g_totalTransactions++;
                
                int passengerCode = 0;
                if ( validateCard( detectResult, operatorCard, passengerCode ) )
                {
                    validCard = true;
                }
                
                // Send notification and wait for card removal
                if(eventCardPresented(operatorCard, cardUID, validCard))
                {
                    g_totalAcceptedTransactions++;
                    SetCardProcessingState(CARD_PROCESSING_STATE_WAITING_FOR_GOOD_CARD_REMOVAL, FALSE, 0);
                }
                else
                {
                    g_totalRejectedTransactions++;                    
                    eventBadCardPresented(operatorCard, cardUID);
                    SetCardProcessingState(CARD_PROCESSING_STATE_WAITING_FOR_BAD_CARD_REMOVAL, FALSE, 0);
                }
            }
            CsTime(&processingEndTime);
            CsTime_t processingElapsedTime = CsTimeDiff(&processingEndTime, &processingStartTime);
            CsVerbose("CardProcessingThread: processing complete duration %dms", (processingElapsedTime.tm_secs * 1000) + processingElapsedTime.tm_msecs);
            DAF_TIMING_POINT_CAPTURE (APPLICATION_TRANSACTION_TOTAL_FINISH);
            DumpTimingPoints();

            // Wait for the card to go away and report when it has gone
            if ( (args->state == CARD_PROCESSING_STATE_WAITING_FOR_GOOD_CARD_REMOVAL) ||
                 (args->state == CARD_PROCESSING_STATE_WAITING_FOR_SILENT_CARD_REMOVAL) ||
                 (args->state == CARD_PROCESSING_STATE_WAITING_FOR_BAD_CARD_REMOVAL) )
            {
                CsDebug(APP_DEBUG_FLOW, (APP_DEBUG_FLOW, "CardProcessingThread: check if card has been removed"));

                /*  WORKAROUND: It seems the reader stops polling after calling MYKI_CS_DetectMultipleCards()
                                [in the MYKI_CS_Commit()] when there is an EMV card in the reader field.
                                The only known solution is calling MYKI_CS_CloseCard() to force restarting poll.
                                !! MYKI_CS_CloseCard() should have been called after card has been removed !! */
                if ( s_unconfirmed == false &&
                     args->state   != CARD_PROCESSING_STATE_WAITING_FOR_GOOD_CARD_REMOVAL )
                {
                    (void)MYKI_CS_CloseCard( );

                    /*  NOTE:   The first call to MYKI_CS_DetectCard(FALSE) after calling MYKI_CS_CloseCard()
                                only enables card polling. Need to force waiting for ATR. */
                    MYKI_CS_DetectCard( TRUE );
                }   /*  end-of-if */

                CsTime_t startTime;
                CsTime( &startTime );

                CsMutexLock( &args->lock );
                {
                    /*  NOTE:   Only one card function can be executed per card presentation. */
                    canProcCard = ( args->state == CARD_PROCESSING_STATE_WAITING_FOR_GOOD_CARD_REMOVAL ? true : false );
                    while
                    (
                        ! args->terminate &&
                        ( ( detectResult = CardDetectWrapper() ) > 0 )
                    )
                    {
                        if ( args->cardFunction == NULL )
                        {
                            if ( IS_DRIVER_CONSOLE_READER() )
                            {
                                processCardInstruction( args, canProcCard );
                                OnIdle( );
                            } 
                            CsMutexUnlock( &args->lock );
                            CsSleep( 0, POLLING_DELAY * 5 );
                            CsMutexLock( &args->lock );
                        }
                        ExecuteCardFunction( args );
                    }
                }
                CsMutexUnlock( &args->lock );

                if ( s_unconfirmed == false &&
                     args->state   == CARD_PROCESSING_STATE_WAITING_FOR_GOOD_CARD_REMOVAL )
                {
                    (void)MYKI_CS_CloseCard( );

                    /*  NOTE:   The first call to MYKI_CS_DetectCard(FALSE) after calling MYKI_CS_CloseCard()
                                only enables card polling. Need to force waiting for ATR. */
                    MYKI_CS_DetectCard( TRUE );
                }   /*  end-of-if */

                CsMutexLock(&args->lock);
                eventCardRemoved(operatorCard, cardUID, args->state == CARD_PROCESSING_STATE_WAITING_FOR_GOOD_CARD_REMOVAL);
                
                SetCardProcessingState(CARD_PROCESSING_STATE_TRANSACTION_COMPLETE, TRUE, 0);
                int millisecs = 0;
                while
                    (
                        (detectResult <= 0) &&
                        (args->cardFunction == 0) &&
                        (millisecs < s_cardRemovalDelay) &&
                        (args->state == CARD_PROCESSING_STATE_TRANSACTION_COMPLETE)
                    )
                {

                    CsTime_t endTime;
                    CsTime(&endTime);
                    CsTime_t diffTime = CsTimeDiff(&endTime, &startTime);
                    millisecs = diffTime.tm_secs * 1000 + diffTime.tm_msecs;

                } 

                CsMutexUnlock(&args->lock);
            }
        }
    }


    /* Clean up */
    int ret = MYKI_CS_Shutdown();
    if ( ret != 0 )
        CsWarnx("MYKI_CS_Shutdown returned %d", ret);

    cleanupDataTransfer( );

    CsDebug(APP_DEBUG_FLOW, (APP_DEBUG_FLOW, "CardProcessingThread: ended"));   

    exit(1);
    return 0;
}

/*==========================================================================*
**
**  getAlarmIPCId
**
**  Description     :
**      Returns the IPC task ID for this application.
**
**  Parameters      :    
**
**  Returns         :
**                                 
**
**  Notes           :
**
**
**==========================================================================*/
TaskID_t getAlarmIPCId()
{
    return APP_TASK;
}


/*==========================================================================*
**
**  sendAlarm
**
**  Description     :
**      Uses uploadAlarm to send the alarm via curl to the gac
**
**  Parameters      :    
**
**  Returns         :
**                                 
**
**==========================================================================*/
void sendAlarm( IPC_Generic_t *p )
{
    if ( p != NULL )
    {
        uploadAlarm( p->data, p->len );
    }
}

/*==========================================================================*
**
**  processTimeSyncTimeout
**
**  Description     :
**      Process Time Sync Timeout (Called from IPC Monitor)
**
**  Parameters      :
**      None
**
**  Returns         :
**      None
**
**==========================================================================*/
void processTimeSyncTimeout()
{
    CsDebug(APP_DEBUG_FLOW, (APP_DEBUG_FLOW, "processTimeSyncTimeout"));
    if ( IsVirtualReader == FALSE && g_iniNoTimeSyncCheck > 0 )
    {
        CsTime_t            now;

        if ( g_timeSyncCheckTimerID==0 /* Initial Call */ )
        {
            /*  Creates and starts periodic timer */
            TimerSpec_t v         = { {0}, {0} };
            v.it_value.tm_secs    = g_iniNoTimeSyncCheck;
            v.it_interval.tm_secs = g_iniNoTimeSyncCheck;
            CsfTimerCreate2( TimeSyncCheck, "Time Synchronisation Check", &g_timeSyncCheckTimerID );
            CsfTimerSet( g_timeSyncCheckTimerID, TMR_RELTIME, &v );

        }
        
        {
            FILE           *fin             = NULL;
            int             retries         = 3;

            while ( ( fin = fopen( NTPD_DATA, "r" ) ) == NULL && retries-- > 0 )
            {
                CsSleep( 0 /*SECS*/, 20 /*MSECS*/ );
            }   /*  end-of-while */

            if ( fin == NULL )
            {
                /*  NTPD has not synchronised with any NTP server => Out-of-Service */
                CsDebug( APP_DEBUG_DETAIL, ( APP_DEBUG_DETAIL, "TimeSyncCheck : Not yet connected to any NTP server" ) );
                if ( g_iniNoTimeSyncError > 0 )
                {
                    CsErrx("processTimeSyncTimeout : Disable reader due to no connection to NTP Server");
                    
                    SetAlarm( AlarmTimeSyncWarn );
                    SetAlarm( AlarmTimeSyncError );
                }   /*  end-of-if */
                return;
            }   /*  end-of-if */

            fscanf( fin, "%d %s", &g_lastTimeSyncUTC, g_lastTimeSyncOffset );
            fclose( fin );
        }

        CsTime( &now );
        CsDebug( APP_DEBUG_DETAIL, ( APP_DEBUG_DETAIL, "processTimeSyncTimeout : Now (%d) LastTimeSync UTC(%d) Offset(%s)", now.tm_secs, g_lastTimeSyncUTC, g_lastTimeSyncOffset ) );

        
        if ( now.tm_secs < g_lastTimeSyncUTC ) 
        {
            CsDebug(APP_DEBUG_DETAIL, (APP_DEBUG_DETAIL, "processTimeSyncTimeout set warning - Possible System Clock Reset"));
            /*  System clock has been reset ! */
            SetAlarm( AlarmTimeSyncWarn );
            ClearAlarm( AlarmTimeSyncError );
        }
        else if( g_iniNoTimeSyncError > 0 && now.tm_secs >= ( g_lastTimeSyncUTC + ( g_iniNoTimeSyncError * SECONDS_PER_HOUR ) ) )
        {
            CsErrx("processTimeSyncTimeout : Disable reader time sync is out of date now %u sync %u threshold %u", 
                    now.tm_secs, g_lastTimeSyncUTC, ( g_lastTimeSyncUTC + ( g_iniNoTimeSyncError * SECONDS_PER_HOUR )));
            SetAlarm( AlarmTimeSyncWarn );
            SetAlarm( AlarmTimeSyncError );
        }
        else if(g_iniNoTimeSyncWarn > 0 && now.tm_secs >= ( g_lastTimeSyncUTC + ( g_iniNoTimeSyncWarn * SECONDS_PER_HOUR ) ) )
        {
            /*  System clock has not been synchronised with network time for sometime */
            SetAlarm( AlarmTimeSyncWarn );
            ClearAlarm( AlarmTimeSyncError );
        }
        else
        {
            ClearAlarm( AlarmTimeSyncWarn  );
            ClearAlarm( AlarmTimeSyncError );
        }   /*  end-of-if */

    }   /*  end-of-if */
}
