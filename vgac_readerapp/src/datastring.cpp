/* -*- mode: c; tabs: 4; -*- */
/*=============================================================================
**
**    Vix Technology                   Licensed software
**    (C) 2015                         All rights reserved
**
**=============================================================================
**
**  Project/Product : MVU
**  Filename        : datastring.cpp
**  Author(s)       : ?
**
**  Description     :
*/
/**     @file
**      @brief  Defines functions to convert card data to printable text.
*/
/*  Member(s)       :
**
**
**  Information     :
**   Compiler(s)    : C++
**   Target(s)      : Independent
**
**  Subversion      :
**      $Id: datastring.cpp 87801 2015-12-30 03:21:31Z atran $
**      $HeadURL: https://auperasvn01.aupera.erggroup.com/svn/DPG_SWBase/vgac_readerapp/trunk/src/datastring.cpp $
**
**  History         :
**   Vers.  Date        Aut.  Type     Description
**   -----  ----------  ----  -------  ----------------------------------------
**    1.00  31.08.15    ANT   Create
**    1.01  12.11.15    ANT   Modify   NGBU-838: Added getBlockingReasonCode()
**
**===========================================================================*/

/*
 *      Options
 *      -------
 */

/*
 *      Includes
 *      --------
 */

#include <string>
#include <sstream>

#include <json/json.h>

#include <corebasetypes.h>
#include <cs.h>
#include <datec19.h>
#include <datetime.h>
#include <myki_cdd_enums.h>
#include <myki_cardservices.h>
#include <myki_cd.h>

#include "card_processing_thread.h"
#include "datastring.h"

/*
 *      Local Constants and Macros
 *      --------------------------
 */

    /** Card/TApplication/CApplication blocking reasons */
enum
{
    BLOCKING_REASON_UNDEFINED       = (0),
    BLOCKING_REASON_LOST_STOLEN     = (1),
    BLOCKING_REASON_HOSTLISTED_SAM  = (2),
    BLOCKING_REASON_BAD_CREDIT      = (3),
    BLOCKING_REASON_SURRENDERED     = (4),
    BLOCKING_REASON_SUSPENDED       = (5),
    BLOCKING_REASON_DAMAGED         = (6),
    BLOCKING_REASON_FAULTY          = (7),
    BLOCKING_REASON_OTHER           = (8),
    BLOCKING_REASON_RESERVED        = (9),  /*  (9)-(14) */
    BLOCKING_REASON_DISABLED        = (15)
};

/*
 *      Local Prototypes
 *      ----------------
 */

static  void        calcLUHN( char *pData );

/*
 *      Local Variables
 *      ---------------
 */

static  const char     *months[ 12 ]    =
{
    "Jan",
    "Feb",
    "Mar",
    "Apr",
    "May",
    "Jun",
    "Jul",
    "Aug",
    "Sep",
    "Oct",
    "Nov",
    "Dec"
};  /*  months[ ] */

    /**
     *  Render a U8_t into the required string format for GAC output.
     *  @param  value an unsigned 8-bit value to be converted.
     *  @return The string object representing the specified unsigned 8-bit value.
     */
std::string
U8ToString( U8_t value )
{
    char    returnValue[ 16 ]   = {'\0'};

    CsSnprintf( returnValue, sizeof( returnValue ) - 1, "%d", value );
    return  returnValue;
}

    /**
     *  Render a U16_t into the required string format for GAC output.
     *  @param  value an unsigned 16-bit value to be converted.
     *  @return The string object representing the specified unsigned 16-bit value.
     */
std::string
U16ToString( U16_t value )
{
    char    returnValue[ 16 ]   = {'\0'};

    CsSnprintf( returnValue, sizeof( returnValue ) - 1, "%d", value );
    return  returnValue;
}

    /**
     *  Render a signed currency value into the required string format for GAC output.
     *  @param  value a signed 32-bit currency value to be converted.
     *  @return The string object representing the specified signed 32-bit currency value.
     */
std::string
MoneyS32ToString( S32_t value )
{
    char    returnValue[ 64 ]   = {'\0'};

    CsSnprintf( returnValue, sizeof( returnValue ) - 1, "%s$%d.%02d",
            value < 0 ? "-" : "",
            abs( (int)value ) / 100, abs( (int)value ) % 100 );
    return  returnValue;
}

    /**
     *  Render an unsigned currency value into the required string format for GAC output.
     *  @param  value of unsigned 32-bit currency value to be converted.
     *  @return The string object representing the specified unsigned 32-bit currency value.
     */
std::string
MoneyU32ToString( U32_t value )
{
    char    returnValue[ 64 ]   = {'\0'};

    CsSnprintf( returnValue, sizeof( returnValue ) - 1, "$%d.%02d",
            abs( (int)value ) / 100, abs( (int)value ) % 100 );
    return  returnValue;
}

    /**
     *  Render a DateC19 as a string.
     *  @param  date C19 date value to be converted.
     *  @return The string object representing the specified C19 date value.
     */
std::string
DateC19ToString( DateC19_t date )
{
    char    dateString[ 33 ]    = { 0 };
    int     year                = 0,
            month               = 0,
            day                 = 0,
            hour                = 0,
            minute              = 0;

    DateC19ToDateYMDHHMM( date, &year, &month, &day, &hour, &minute );
    if ( year == 0 )
    {
        return  dateString;
    }
    else
    {
        CsSnprintf( dateString, sizeof( dateString ) - 1, "%02d %s %d %02d:%02d %s",
                day, months[ month - 1 ], year,
                ( hour <= 12 ? hour : hour % 12 ), minute, hour >= 12 ? "pm" : "am" );
        return  dateString;
    }
}

    /**
     *  Render a DateC19 as a string taking into account the end of transport time.
     *  @param  date C19 date value to be converted.
     *  @return The string object representing the specified C19 date value.
     */
std::string
DateC19ToStringWithEndOfTransport( DateC19_t date )
{
    char                    dateString[ 33 ]    = {'\0'};
    int                     year                = 0,
                            month               = 0,
                            day                 = 0,
                            hour                = 0,
                            minute              = 0;
    MYKI_BR_ContextData_t  *pData               = NULL;

    DateC19ToDateYMDHHMM( date, &year, &month, &day, &hour, &minute );
    if ( year == 0 )
    {
        return dateString;
    }
    else
    {
        pData   = GetCardProcessingThreadContextData( );

        //  Set the hour and minute to the end of business
        hour    = pData->Tariff.endOfBusinessDay / 60;
        minute  = pData->Tariff.endOfBusinessDay % 60;

        CsSnprintf( dateString, sizeof( dateString ) - 1, "%02d %s %d %02d:%02d %s",
                day, months[ month - 1 ], year,
                ( hour <= 12 ? hour : hour % 12 ), minute, hour >= 12 ? "pm" : "am" );
        return dateString;
    }
}

    /**
     *  Render the myki card number as a string in the required format for the GAC.
     *  @return The string object representing card issuer id and card serial
     *          number or "ERROR" if failed reading from card.
     */
std::string
getMykiCardNumber( )
{
    static MYKI_CAIssuer_t *pMYKI_CAIssuer = NULL;
    std::string             returnValue;
    int                     cardresult      = MYKI_CS_CAIssuerGet( &pMYKI_CAIssuer );

    if ( cardresult >= 0 )
    {
        std::stringstream   iss,
                            CSN;

        iss <<  pMYKI_CAIssuer->IssuerId;
        CSN <<  pMYKI_CAIssuer->CSN;

        returnValue = iss.str() + " " + CSN.str();
        return returnValue;
    }
    else
    {
        return "ERROR";
    }
}

    /**
     *  Returns MYKI card PAN in the format of 'IIIIIINNNNNNNNC', where
     *  IIIIII is the card issuer identification, NNNNNNNN is the card
     *  serial number and C is the LUHN check digit.
     *  @return The string object representing card PAN or "ERROR" if
     *          failed reading from card.
     */
std::string
getMykiPAN( )
{
    static MYKI_CAIssuer_t *pMYKI_CAIssuer = NULL;
    int                     cardresult      = MYKI_CS_CAIssuerGet( &pMYKI_CAIssuer );

    if ( cardresult >= 0 )
    {
        char                pan[ 6 /*ISSUER_ID*/ + 8 /*CSN*/ + 1 /*CHD*/ + 1 ];

        CsSnprintf( pan, sizeof( pan ), "%06d%08d0", pMYKI_CAIssuer->IssuerId, pMYKI_CAIssuer->CSN );
        calcLUHN( pan );
        return pan;
    }
    else
    {
        return "ERROR";
    }
}

    /**
     *  Render the myki card balance as a string in the required format for the GAC.
     *  @note   Negative balances are rendered as -$xx.yy, i.e. no parentheses.
     *  @return The string object representing currency value of the card balance or
     *          "ERROR" if failed reading from card.
     */
std::string
getMykiMoneyBalance( )
{
    static MYKI_TAPurseBalance_t   *pMYKI_TAPurseBalance    = NULL;
    int                             cardresult              = MYKI_CS_TAPurseBalanceGet( &pMYKI_TAPurseBalance );

    if ( cardresult >= 0 )
    {
        return MoneyS32ToString( pMYKI_TAPurseBalance->Balance );
    }
    else
    {
        return "ERROR";
    }
}

    /**
     *  Render the myki auto-load threshold as a string in the required format for the GAC.
     *  @return The string object representing currency value of the card auto-load
     *          threshold or "ERROR" if failed reading from card.
     */
std::string
getAutoloadThreshold( )
{
    static MYKI_TAPurseControl_t   *pMYKI_TAPurseControl    = NULL;
    int                             cardresult              = MYKI_CS_TAPurseControlGet( &pMYKI_TAPurseControl );

    if ( cardresult >= 0 )
    {
        return MoneyU32ToString( pMYKI_TAPurseControl->AutoThreshold );
    }
    else
    {
        return "ERROR";
    }
}

    /**
     *  Render the myki auto-load enabled value as "Yes" or "No".
     *  @return "Yes" if auto-load is enabled, "No" if auto-load is disabled or
     *          "ERROR" if failed reading from card.
     */
std::string
getAutoloadEnabled( )
{
    static MYKI_TAPurseControl_t   *pMYKI_TAPurseControl    = NULL;
    int                             cardresult              = MYKI_CS_TAPurseControlGet( &pMYKI_TAPurseControl );

    if ( cardresult >= 0 )
    {
        return pMYKI_TAPurseControl->AutoThreshold > 0 ? "Yes" : "No";
    }
    else
    {
        return "ERROR";
    }
}

    /**
     *  Render the myki auto-load amount as a string in the required format for the GAC.
     *  @return The string object representing currency value of the card auto-load
     *          amount or "ERROR" if failed reading from card.
     */
std::string
getAutoloadAmount( )
{
    static MYKI_TAPurseControl_t   *pMYKI_TAPurseControl    = NULL;
    int                             cardresult              = MYKI_CS_TAPurseControlGet( &pMYKI_TAPurseControl );

    if ( cardresult >= 0 )
    {
        return MoneyU32ToString( pMYKI_TAPurseControl->AutoValue );
    }
    else
    {
        return "ERROR";
    }
}

    /**
     *  Render the transit provisional fare amount as a string.
     *  @return The string object representing currency value of the transit provisional
     *          fare or "ERROR" if failed reading from card.
     */
std::string
getTransitProvisionalFare( )
{
    static MYKI_TAControl_t        *pMYKI_TAControl     = NULL;
    int                             cardresult          = MYKI_CS_TAControlGet( &pMYKI_TAControl );

    if ( cardresult >= 0 )
    {
        return MoneyU32ToString( pMYKI_TAControl->ProvisionalFare );
    }
    else
    {
        return "ERROR";
    }
}

    /**
     *  Render the card expiry date as a string.
     *  @return The string object representation of the card expiry date and time or
     *          "ERROR" if failed reading from card.
     *  @note   The encoded card expiry date encoded is the last valid day.
     */
std::string
getCardExpiryDate( )
{
    static MYKI_CAControl_t        *pMYKI_CAControl     = NULL;
    int                             cardresult          = MYKI_CS_CAControlGet( &pMYKI_CAControl );

    if ( cardresult >= 0 )
    {
        //  +1 because it should display the end of business day which is the next day early morning.
        return DateC19ToStringWithEndOfTransport( pMYKI_CAControl->ExpiryDate + 1 );
    }
    else
    {
        return "ERROR";
    }
}

    /**
     *  Render the Transit application expiry date as a string.
     *  @return The string object representation of the Transit application expiry
     *          date and time or "ERROR" if failed reading from card.
     *  @note   The encoded Transit application expiry date is the last valid day.
     */
std::string
getTransitExpiryDate( )
{
    static MYKI_TAControl_t        *pMYKI_TAControl     = NULL;
    int                             cardresult          = MYKI_CS_TAControlGet( &pMYKI_TAControl );

    if ( cardresult >= 0 )
    {
        return DateC19ToStringWithEndOfTransport( pMYKI_TAControl->ExpiryDate + 1 );
    }
    else
    {
        return "ERROR";
    }
}

    /**
     *  Render the transit passenger code expiry date as a string.
     *  @return The string object representation of the passenger code expiry date
     *          and time or "ERROR" if failed reading from card.
     *  @note   The encoded passenger code expiry date is the last valid day.
     */
std::string
getTransitPassengerCodeExpiryDate( )
{
    static MYKI_TAControl_t        *pMYKI_TAControl     = NULL;
    int                             cardresult          = MYKI_CS_TAControlGet( &pMYKI_TAControl );

    if ( cardresult >= 0 )
    {
        return DateC19ToStringWithEndOfTransport( pMYKI_TAControl->PassengerCodeExpiry + 1 );
    }
    else
    {
        return "ERROR";
    }
}

    /**
     *  Render the card issuer id as a string.
     *  @return The string object representation of the card issuer id or "ERROR"
     *          if failed reading from card.
     *  @deprecated
     */
std::string
getIssuerId( )
{
    static MYKI_CAIssuer_t         *pMYKI_CAIssuer      = NULL;
    int                             cardresult          = MYKI_CS_CAIssuerGet( &pMYKI_CAIssuer );

    if ( cardresult >= 0 )
    {
        std::stringstream           value;

        if ( pMYKI_CAIssuer->IssuerId == 308425 )
            value << "PTV";
        else
            value << pMYKI_CAIssuer->IssuerId;

        return value.str();
    }
    else
    {
        return "ERROR";
    }
}

    /**
     *  Render the card application status as a string.
     *  @return The string object representing the card application status or
     *          "ERROR" if failed reading from card.
     */
std::string
getCardApplicationStatus( )
{
    static MYKI_CAControl_t    *pMYKI_CAControl     = NULL;
    int                         cardresult          = MYKI_CS_CAControlGet( &pMYKI_CAControl );
    std::string                 returnValue;

    if ( cardresult >= 0 )
    {
        switch ( pMYKI_CAControl->Status )
        {
        case CARD_CONTROL_STATUS_RESERVED:
            returnValue = "Reserved";
            break;

        case CARD_CONTROL_STATUS_INITIALISED:
            returnValue = "Initialised";
            break;

        case CARD_CONTROL_STATUS_ISSUED:
            returnValue = "Issued";
            break;

        case CARD_CONTROL_STATUS_ACTIVATED:
            returnValue = "Activated";
            break;

        case CARD_CONTROL_STATUS_BLOCKED:
            returnValue = "Blocked";
            break;

        case CARD_CONTROL_STATUS_DISABLED:
            returnValue = "Disabled";
            break;

        default:
            returnValue = "Unknown";
            break;
        }
        return returnValue.c_str();
    }
    else
    {
        return "ERROR";
    }
}

    /**
     *  Render the Blocking reason as a string.
     *  @param  blockingReason blocking reason code read from card.
     *  @return The string object representation of the specified blocking reason.
     */
std::string
getBlockingReason( U8_t blockingReason )
{
    std::string                 returnValue;

    switch ( blockingReason )
    {
    case BLOCKING_REASON_UNDEFINED:
        returnValue = "Undefined";
        break;
    case BLOCKING_REASON_LOST_STOLEN:
        returnValue = "Lost / Stolen";
        break;
    case BLOCKING_REASON_HOSTLISTED_SAM:
        returnValue = "hotlisted SAM";
        break;
    case BLOCKING_REASON_BAD_CREDIT:
        returnValue = "Bad credit";
        break;
    case BLOCKING_REASON_SURRENDERED:
        returnValue = "Surrendered";
        break;
    case BLOCKING_REASON_SUSPENDED:
        returnValue = "Suspended";
        break;
    case BLOCKING_REASON_DAMAGED:
        returnValue = "Damaged";
        break;
    case BLOCKING_REASON_FAULTY:
        returnValue = "Faulty";
        break;
    case BLOCKING_REASON_OTHER:
        returnValue = "Other";
        break;
    case BLOCKING_REASON_DISABLED:
        returnValue = "Disabled";
        break;
    default:
        returnValue = "Reserved";
        break;
    }
    return returnValue.c_str();
}

    /**
     *  Render the Blocking reason code as a string.
     *  @param  blockingReason blocking reason code read from card.
     *  @return The string object representation of the specified blocking reason code.
     */
std::string
getBlockingReasonCode( U8_t blockingReason )
{
    std::string                 returnValue;

    switch ( blockingReason )
    {
    case    BLOCKING_REASON_UNDEFINED:      returnValue = "BLOCKING_REASON_UNDEFINED";      break;
    case    BLOCKING_REASON_LOST_STOLEN:    returnValue = "BLOCKING_REASON_LOST_STOLEN";    break;
    case    BLOCKING_REASON_HOSTLISTED_SAM: returnValue = "BLOCKING_REASON_HOTLISTED_SAM";  break;
    case    BLOCKING_REASON_BAD_CREDIT:     returnValue = "BLOCKING_REASON_BAD_CREDIT";     break;
    case    BLOCKING_REASON_SURRENDERED:    returnValue = "BLOCKING_REASON_SURRENDERED";    break;
    case    BLOCKING_REASON_SUSPENDED:      returnValue = "BLOCKING_REASON_SUSPENDED";      break;
    case    BLOCKING_REASON_DAMAGED:        returnValue = "BLOCKING_REASON_DAMAGED";        break;
    case    BLOCKING_REASON_FAULTY:         returnValue = "BLOCKING_REASON_FAULTY";         break;
    case    BLOCKING_REASON_OTHER:          returnValue = "BLOCKING_REASON_OTHER";          break;
    case    BLOCKING_REASON_DISABLED:       returnValue = "BLOCKING_REASON_DISABLED";       break;
    default:                                returnValue = "BLOCKING_REASON_RESERVED";       break;
    }

    return returnValue.c_str();
}

    /**
     *  Render the card application blocking reason as a string.
     *  @return The string object representation of the card application blocking reason
     *          or "ERROR" if failed reading from card.
     */
std::string
getCardApplicationBlockingReason( )
{
    static MYKI_CAControl_t    *pMYKI_CAControl     = NULL;
    int                         cardresult          = MYKI_CS_CAControlGet( &pMYKI_CAControl );

    if ( cardresult >= 0 )
    {
        if ( pMYKI_CAControl->Status == CARD_CONTROL_STATUS_BLOCKED )
            return getBlockingReason( pMYKI_CAControl->BlockingReason );
        else
            return "";
    }
    else
    {
        return "ERROR";
    }
}

    /**
     *  Render the Transit application status as a string.
     *  @return The string object representation of the Transit application status.
     */
std::string
getTransitApplicationStatus( )
{
    static MYKI_TAControl_t    *pMYKI_TAControl     = NULL;
    int                         cardresult          = MYKI_CS_TAControlGet( &pMYKI_TAControl );
    std::string                 returnValue;

    if ( cardresult >= 0 )
    {
        switch ( pMYKI_TAControl->Status )
        {
        case TAPP_CONTROL_STATUS_RESERVED:
            returnValue = "Reserved";
            break;
        case TAPP_CONTROL_STATUS_INITIALISED:
            returnValue = "Initialised";
            break;
        case TAPP_CONTROL_STATUS_ISSUED:
            returnValue = "Issued";
            break;
        case TAPP_CONTROL_STATUS_ACTIVATED:
            returnValue = "Activated";
            break;
        case TAPP_CONTROL_STATUS_BLOCKED:
            returnValue = "Blocked";
            break;
        case TAPP_CONTROL_STATUS_DISABLED:
            returnValue = "Disabled";
            break;
        default:
            returnValue = "Unknown";
            break;
        }
        return returnValue.c_str();
    }
    else
    {
        return "ERROR";
    }
}

    /**
     *  Render the Transit application registration type as a string.
     *  @return The string object representation of the Transit application
     *          registration type or "ERROR" if failed reading card.
     */
std::string
getTransitApplicationRegistrationType( )
{
    enum
    {
        PASSENGER_CONTROL_REGISTERED_BIT = (0x01),      // TODO Update CDD Enums.h
        PASSENGER_CONTROL_PERSONALISED_BIT = (0x02),
        PASSENGER_CONTROL_DDA_BIT = (0x04),
        PASSENGER_CONTROL_RESERVED_BIT = (0x08),

    };

    static MYKI_TAControl_t    *pMYKI_TAControl     = NULL;

    if ( MYKI_CS_TAControlGet( &pMYKI_TAControl ) >= 0 )
    {
        if ( pMYKI_TAControl->PassengerControl & PASSENGER_CONTROL_DDA_BIT )
        {
            if ( pMYKI_TAControl->PassengerControl & PASSENGER_CONTROL_PERSONALISED_BIT )
                return "DDA - Personalised";
            else if ( pMYKI_TAControl->PassengerControl & PASSENGER_CONTROL_REGISTERED_BIT )
                return "DDA - Registered";
            else
                return "DDA";
        }
        else
        {
            if ( pMYKI_TAControl->PassengerControl & PASSENGER_CONTROL_PERSONALISED_BIT )
                return "Personalised";
            else if ( pMYKI_TAControl->PassengerControl & PASSENGER_CONTROL_REGISTERED_BIT )
                return "Registered";
        }
    }
    else
    {
        return "ERROR";
    }
    return "Unregistered";
}

    /**
     *  Render the Transit application blocking reason as a string
     *  @return The string object representing Transit application blocking reason or
     *          "ERROR" if failed reading from card.
     */
std::string
getTransitApplicationBlockingReason( )
{
    static MYKI_TAControl_t    *pMYKI_TAControl     = NULL;
    int                         cardresult          = MYKI_CS_TAControlGet( &pMYKI_TAControl );

    if ( cardresult >= 0 )
    {
        if ( pMYKI_TAControl->Status == TAPP_CONTROL_STATUS_BLOCKED )
            return getBlockingReason( pMYKI_TAControl->BlockingReason );
        else
            return "";
    }
    else
    {
        return "ERROR";
    }
}

    /**
     *  Render the card serial number as a formatted string.
     *  @return The string object representation of the card (hardware) serial number.
     *  @deprecated
     */
std::string
getSerialNumber( )
{
    char                cardUID[ 32 ]   = {'\0'};
    CT_CardInfo_t      *s_pCardInfo     = NULL;

    if ( MYKI_CS_GetCardInfo( &s_pCardInfo ) != MYKI_CS_OK )
    {
        s_pCardInfo = NULL;
    }

    if ( s_pCardInfo != NULL )
    {
        char           *t   = cardUID;

        for ( int i = 0; i < s_pCardInfo->SerialLength; i++ )
        {
            t  += sprintf( t, "%02X", s_pCardInfo->SerialNumber[ i ] );
        }
        return  cardUID;
    }
    return "ERROR";
}

    /**
     *  Render the presence of a surcharge as "Yes"/"No".
     *  @return "Yes" if a surcharge, "No" if no surcharge or "ERROR" if
     *          failed reading from card.
     */
std::string
getSurcharge( )
{
    static MYKI_TAControl_t    *pMYKI_TAControl     = NULL;
    static MYKI_TAProduct_t    *pMYKI_TAProduct     = NULL;
    int                         cardresult          = MYKI_CS_TAControlGet( &pMYKI_TAControl );

    if ( cardresult >= 0 )
    {
        for ( unsigned int i = 1; i < DIMOF( pMYKI_TAControl->Directory ); i++ )
        {
            MYKI_Directory_t   *pDirectory          = NULL;

            if ( myki_gac_GetCardProduct( i, &pDirectory, &pMYKI_TAProduct ) < 0 )
            {
                //  Do nothing - continue
            }
            else
            {
                if ( ( pDirectory->Status == TAPP_CONTROL_DIRECTORY_STATUS_ACTIVATED ) &&
                     ( pMYKI_TAProduct->ControlBitmap & TAPP_TPRODUCT_CONTROL_PREMIUM_BITMAP ) )
                    return "Yes";
            }
        }
    }
    else
    {
        return "ERROR";
    }
    return "No";
}

    /**
     *  Render the zone that the card is touched on for.
     *  @return Touched on is a dynamic “on/off” (In touched on state “Zone” is displayed. Touched off = “-“).
     */
std::string
getTouchedOn( )
{
    static MYKI_TAControl_t    *pMYKI_TAControl = NULL;
    static MYKI_TAProduct_t    *pMYKI_TAProduct = NULL;
    MYKI_Directory_t           *pDirectory      = NULL;
    int                         cardresult      = MYKI_CS_TAControlGet( &pMYKI_TAControl );

    if ( cardresult >= 0 )
    {
        if ( pMYKI_TAControl->ProductInUse > 0 )
        {
            if ( myki_gac_GetCardProduct( pMYKI_TAControl->ProductInUse, &pDirectory, &pMYKI_TAProduct ) < 0  || pMYKI_TAProduct == NULL )
            {
                return "ERROR";
            }
            else
            {
                std::stringstream value;

                int physicalZone =  (int)LogicalZoneToPhysicalZone(pMYKI_TAProduct->LastUsage.Zone);
                value << (int)physicalZone;
                std::string returnValue = "Zone " + value.str();
                return returnValue;
            }
        }
    }
    else
    {
        return "ERROR";
    }

    return "-";
}

    /**
     *  Render the purchase value of the provisional product.
     *  @return The string object representation of the purchase value of the provisional product.
     */
std::string
getOutstandingDefaultFare( )
{
    static MYKI_TAControl_t    *pMYKI_TAControl = NULL;
    static MYKI_TAProduct_t    *pMYKI_TAProduct = NULL;

    if ( MYKI_CS_TAControlGet( &pMYKI_TAControl ) >= 0 )
    {
        if ( pMYKI_TAControl != NULL && pMYKI_TAControl->ProductInUse > 0 )
        {
            MYKI_Directory_t   *pDirectory      = NULL;

            if ( myki_gac_GetCardProduct( pMYKI_TAControl->ProductInUse, &pDirectory, &pMYKI_TAProduct ) < 0 )
            {
                return "ERROR";
            }
            else
            {
                if ( pMYKI_TAProduct != NULL && ( pMYKI_TAProduct->ControlBitmap & TAPP_TPRODUCT_CONTROL_PROVISIONAL_BITMAP ) )
                {
                    if ( pMYKI_TAProduct->PurchaseValue == 0 )
                        return "";
                    else
                        return MoneyU32ToString( pMYKI_TAProduct->PurchaseValue );
                }
            }
        }
    }
    else
    {
        return "ERROR";
    }

    return "";
}

    /**
     *  Render the Transit passenger code as a formatted string.
     *  @return The string object representation of the Transit passenger type code
     *          or "ERROR" if failed reading from card.
     */
std::string
getTransitPassengerCode( )
{
    static MYKI_TAControl_t    *pMYKI_TAControl     = NULL;
    int                         cardresult          = MYKI_CS_TAControlGet( &pMYKI_TAControl );

    if ( cardresult >= 0 )
    {
        MYKI_CD_PassengerType_t passengerType;

        if ( MYKI_CD_getPassengerTypeStructure( pMYKI_TAControl->PassengerCode, &passengerType ) )
        {
            return passengerType.desc;
        }
        else
        {
            char                codeString[ 32 ]    = {'\0'};

            CsSnprintf( codeString, sizeof( codeString ) - 1, "%d", pMYKI_TAControl->PassengerCode );
            return codeString;
        }
    }
    else
    {
        return "ERROR";
    }
}

    /**
     *  Render the Transit application version as a formatted string.
     *  @return The string object representation of the Transit application version.
     */
std::string
getTransitVersion( )
{
    static MYKI_TAIssuer_t     *pMYKI_TAIssuer      = NULL;
    int                         cardresult          = MYKI_CS_TAIssuerGet( &pMYKI_TAIssuer );

    if ( cardresult >= 0 )
    {
        char                    versionString[ 32 ] = {'\0'};

        CsSnprintf( versionString, sizeof( versionString ) - 1, "%d", pMYKI_TAIssuer->Version );
        return versionString;
    }
    else
    {
        return "ERROR";
    }
}

    /**
     *  Render the Daily capping expiry as a string.
     *  @return The string object representation of the Daily capping expiry.
     */
std::string
getDailyCappingExpiry( )
{
    static MYKI_TACapping_t    *pMYKI_TACapping     = NULL;
    int                         cardresult          = MYKI_CS_TACappingGet( &pMYKI_TACapping );

    if ( cardresult >= 0 )
    {
        return DateC19ToStringWithEndOfTransport( pMYKI_TACapping->Daily.Expiry );
    }
    else
    {
        return "ERROR";
    }
}

    /**
     *  Render the Daily capping zones as a string.
     *  @return The string object representing Daily capping zones.
     */
std::string
getDailyCappingZones( )
{
    static MYKI_TACapping_t    *pMYKI_TACapping     = NULL;
    int                         cardresult          = MYKI_CS_TACappingGet( &pMYKI_TACapping );

    if ( cardresult >= 0 )
    {
        char                    returnValue[ 64 ]   = {'\0'};

        if ( pMYKI_TACapping->Daily.Zone.Low > 0 && pMYKI_TACapping->Daily.Zone.High > 0 )
        {
            CsSnprintf( returnValue, sizeof( returnValue ) - 1, "%d - %d",
                    (int)LogicalZoneToPhysicalZone( pMYKI_TACapping->Daily.Zone.Low ),
                    (int)LogicalZoneToPhysicalZone( pMYKI_TACapping->Daily.Zone.High ) );
        }
        return returnValue;
    }
    else
    {
        return "ERROR";
    }
}

    /**
     *  Render the Daily capping value as a string.
     *  @return The string object representation of the Daily capping value.
     */
std::string
getDailyCappingValue( )
{
    static MYKI_TACapping_t    *pMYKI_TACapping     = NULL;
    int                         cardresult          = MYKI_CS_TACappingGet( &pMYKI_TACapping );

    if ( cardresult >= 0 )
    {
        return MoneyU32ToString( pMYKI_TACapping->Daily.Value );
    }
    else
    {
        return "ERROR";
    }
}

    /**
     *  Render the Weekly capping expiry as a string.
     *  @return The string object representation of the Weekly capping expiry.
     */
std::string
getWeeklyCappingExpiry( )
{
    static MYKI_TACapping_t    *pMYKI_TACapping     = NULL;
    int                         cardresult          = MYKI_CS_TACappingGet( &pMYKI_TACapping );

    if ( cardresult >= 0 )
    {
        return DateC19ToStringWithEndOfTransport( pMYKI_TACapping->Weekly.Expiry );
    }
    else
    {
        return "ERROR";
    }
}

    /**
     *  Render the Weekly capping zones as a string.
     *  @param  index Weekly capping index [0..4].
     *  @return The string object representing Weekly capping zones.
     */
std::string
getWeeklyCappingZones( int index /* = 0 */ )
{
    static MYKI_TACapping_t    *pMYKI_TACapping     = NULL;
    int                         cardresult          = MYKI_CS_TACappingGet( &pMYKI_TACapping );

    if ( cardresult >= 0 )
    {
        char                    returnValue[ 64 ]   = {'\0'};

        if ( index < MYKI_WEEKLY_MAX_ZONEDAYS &&
             pMYKI_TACapping->Weekly.Zone[ index ].Low  != 0 &&
             pMYKI_TACapping->Weekly.Zone[ index ].High !=0)
        {
            CsSnprintf( returnValue, sizeof( returnValue ) - 1, "%d - %d",
                (int)LogicalZoneToPhysicalZone( pMYKI_TACapping->Weekly.Zone[ index ].Low ),
                (int)LogicalZoneToPhysicalZone( pMYKI_TACapping->Weekly.Zone[ index ].High ) );
        }
        return returnValue;
    }
    else
    {
        return "ERROR";
    }
}

    /**
     *  Render the Weekly capping value as a string.
     *  @return The string object representation of the Weekly capping value.
     */
std::string
getWeeklyCappingValue( )
{
    static MYKI_TACapping_t    *pMYKI_TACapping     = NULL;
    int                         cardresult          = MYKI_CS_TACappingGet( &pMYKI_TACapping );

    if ( cardresult >= 0 )
    {
        return MoneyU32ToString( pMYKI_TACapping->Weekly.Value );
    }
    else
    {
        return "ERROR";
    }
}

    /**
     *  Render number of seconds since epoch into local date and time string.
     *  @param  time    number of seconds since epoch.
     *  @return The string object representation of local date and time or
     *          "ERROR" if specified time is invalid.
     */
std::string
TimeToString( Time_t time )
{
    struct tm                  *pTm                 = localtime( (time_t*)&time );

    if ( pTm == NULL )
    {
        return "ERROR";
    }
    else
    {
        char                    dateString[ 32 ]    = {'\0'};

        CsSnprintf( dateString, sizeof( dateString ) - 1, "%02d %s %d %02d:%02d:%02d",
                pTm->tm_mday, months[ pTm->tm_mon ], pTm->tm_year + 1900,
                pTm->tm_hour, pTm->tm_min, pTm->tm_sec );

        return dateString;

    }
}

    /**
     *  Helper function to return the location string.
     *  @param  Location location object to be converted.
     *  @return The string object representing the location object.
     *  @note   Currently only renders the entry point id, retrieving the location
     *          string from cd is functional but not required by the client.
     */
std::string
getLocation( MYKI_Location_t &Location )
{
    #if 0
    MYKI_CD_Locations_t         cdLocation;
    int                         res = MYKI_CD_getLocationsStructure( Location.EntryPointId, &cdLocation );

    if ( res )
    {
        char                    returnValue[ 128 ]  = {'\0'};

        CsSnprintf( returnValue, sizeof( returnValue ) - 1, "%s (%d)",
                cdLocation.long_desc,Location.EntryPointId );
        return returnValue;
    }
    else
    #endif
    {
        char                    returnValue[ 32 ]   = {'\0'};

        CsSnprintf( returnValue, sizeof( returnValue ) - 1, "%d", Location.EntryPointId );
        return returnValue;
    }
}

    /**
     *  Helper function to return strings describing the product type.
     *  @param  productId Id used to perform lookup with product configuration db.
     *  @param  instanceCount Number of remaining instances of product.
     *  @return The string object representation of the product type.
     */
std::string
getProductType( U16_t productId, U16_t instanceCount )
{
    std::string                 returnValue         = "";
    MYKI_CD_Product_t           product;

    if ( ! MYKI_CD_getProductStructure( (U8_t)productId, &product ) )
    {
        CsErrx( "MYKI_CD_getProductStructure(%d) failed", productId );
        return "unknown";
    }

    returnValue = product.short_desc;

    if ( instanceCount > 0 )
    {
        char                    instanceString[ 32 ]    = {'\0'};

        CsSnprintf( instanceString, sizeof( instanceString ) - 1, "{%d}", instanceCount );

        returnValue    += instanceString;
    }

    return returnValue;
}

    /**
     *  Helper function to return strings describing the provider mode (e.g. Rail).
     *  @param  providerId service provider id.
     *  @return The string object representation of transport mode.
     */
std::string
getProviderMode( U16_t providerId )
{
    MYKI_CD_ServiceProviders_t  serviceProviders;

    if ( ! MYKI_CD_getServiceProvidersStructure( providerId, &serviceProviders ) )
        return "Unknown";

    return serviceProviders.mode;
}

    /**
     *  Helper function to return strings describing the provider name (e.g. Metro).
     *  @param  providerId service provider id.
     *  @return The string object representation of the service provider name.
     */
std::string
getProviderName( U16_t providerId )
{
    MYKI_CD_ServiceProviders_t  serviceProviders;

    if ( ! MYKI_CD_getServiceProvidersStructure( providerId, &serviceProviders ) )
        return "Unknown";

    return serviceProviders.long_desc;
}

    /**
     *  Helper function to return strings describing the product status.
     *  @param  status product status.
     *  @return The string representation of the product status.
     */
std::string
getProductStatus( U8_t status )
{
    switch ( status )
    {
        case TAPP_CONTROL_DIRECTORY_STATUS_UNUSED     : return "Unused";
        case TAPP_CONTROL_DIRECTORY_STATUS_INITIALISED: return "Initialised";
        case TAPP_CONTROL_DIRECTORY_STATUS_ISSUED     : return "Issued";
        case TAPP_CONTROL_DIRECTORY_STATUS_ACTIVATED  : return "Activated";
        case TAPP_CONTROL_DIRECTORY_STATUS_BLOCKED    : return "Blocked";
        default: return "Unknown";
    }
}

    /**
     *  Helper function to return strings related to the payment method.
     *  @param  PaymentMethod   payment method.
     *  @return The string object representation of the payment method.
     */
std::string
getPaymentMethod( U8_t PaymentMethod )
{
    // TODO Remove and reference directly from card services library.
    enum
    {
        TAPP_USAGE_LOG_PAYMENT_METHOD_UNDEFINED = (0),      /**< Undefined */
        TAPP_USAGE_LOG_PAYMENT_METHOD_TPURSE,               /**< T-Purse */
        TAPP_USAGE_LOG_PAYMENT_METHOD_CASH,                 /**< Cash */
        TAPP_USAGE_LOG_PAYMENT_METHOD_EFT_REFUNDABLE,       /**< EFT (Refundable) */
        TAPP_USAGE_LOG_PAYMENT_METHOD_EFT_NONREFUNDABLE,    /**< EFT (Non-refundable) */
        TAPP_USAGE_LOG_PAYMENT_METHOD_RECURRING_AUTOLOAD,   /**< Recurring autoload */
        TAPP_USAGE_LOG_PAYMENT_METHOD_ADHOC_AUTOLOAD,       /**< Ad-hoc autoload */
        TAPP_USAGE_LOG_PAYMENT_METHOD_CHEQUE,               /**< Cheque */
        TAPP_USAGE_LOG_PAYMENT_METHOD_VOUCHER,              /**< Voucher */
        TAPP_USAGE_LOG_PAYMENT_METHOD_TRANSFER,             /**< Transfer */

    };

    switch ( PaymentMethod )
    {
    case TAPP_USAGE_LOG_PAYMENT_METHOD_UNDEFINED:            return "Undefined";
    case TAPP_USAGE_LOG_PAYMENT_METHOD_TPURSE:               return "myki money";
    case TAPP_USAGE_LOG_PAYMENT_METHOD_CASH:                 return "Cash";
    case TAPP_USAGE_LOG_PAYMENT_METHOD_EFT_REFUNDABLE:       return "EFT (Refundable)";
    case TAPP_USAGE_LOG_PAYMENT_METHOD_EFT_NONREFUNDABLE:    return "EFT (Non-refundable)";
    case TAPP_USAGE_LOG_PAYMENT_METHOD_RECURRING_AUTOLOAD:   return "Recurring Autoload";
    case TAPP_USAGE_LOG_PAYMENT_METHOD_ADHOC_AUTOLOAD:       return "Ad-hoc autoload";
    case TAPP_USAGE_LOG_PAYMENT_METHOD_CHEQUE:               return "Cheque";
    case TAPP_USAGE_LOG_PAYMENT_METHOD_VOUCHER:              return "Voucher";
    case TAPP_USAGE_LOG_PAYMENT_METHOD_TRANSFER:             return "Transfer";
    default: return "Unknown";
    }
}

    /**
     *  Helper function to return either "-" or "" depending on txtype for a usage log.
     *  @param  txType  transaction type.
     *  @return "-" if transaction type is a DEBIT transaction and
     *          "" if transaction type is a CREDIT transaction.
     */
std::string
getTxTypeSigning( U8_t txType )
{
    switch ( txType )
    {
        case MYKI_BR_TRANSACTION_TYPE_ENTRY_EXIT                 : return "";
        case MYKI_BR_TRANSACTION_TYPE_APPLICATION_BLOCK          : return "-";
        case MYKI_BR_TRANSACTION_TYPE_PRODUCT_BLOCK              : return "-";
        case MYKI_BR_TRANSACTION_TYPE_PRODUCT_UNBLOCK            : return "-";
        case MYKI_BR_TRANSACTION_TYPE_CHANGE_OF_MIND             : return "";
        case MYKI_BR_TRANSACTION_TYPE_STATION_EXIT_FEE           : return "";
        case MYKI_BR_TRANSACTION_TYPE_EXIT_ONLY                  : return "";
        case MYKI_BR_TRANSACTION_TYPE_ENTRY_ONLY                 : return "";
        case MYKI_BR_TRANSACTION_TYPE_ON_BOARD                   : return "";
        case MYKI_BR_TRANSACTION_TYPE_WRONG_SIDE_OF_GATE         : return "";
        case MYKI_BR_TRANSACTION_TYPE_TPURSE_LOAD_VALUE          : return "";
        case MYKI_BR_TRANSACTION_TYPE_LOAD_PRODUCT               : return "";
        case MYKI_BR_TRANSACTION_TYPE_MULTIPLE_ACTION_LIST       : return "";
        case MYKI_BR_TRANSACTION_TYPE_PRODUCT_AUTOLOAD_DISABLE   : return "";
        case MYKI_BR_TRANSACTION_TYPE_AUTO_LOAD_ENABLE           : return "";
        case MYKI_BR_TRANSACTION_TYPE_AUTO_LOAD_DISABLE          : return "";
        case MYKI_BR_TRANSACTION_TYPE_TPURSE_AUTO_LOAD_VALUE     : return "";
        case MYKI_BR_TRANSACTION_TYPE_TPURSE_AL_LOAD_VALUE       : return "-"; // Actually a reverse
        case MYKI_BR_TRANSACTION_TYPE_NONE                       : return "";
        case MYKI_BR_TRANSACTION_TYPE_PURSE_USAGE_MANUAL         : return "";
        case MYKI_BR_TRANSACTION_TYPE_APPLICATION_UNBLOCK        : return "-";
        case MYKI_BR_TRANSACTION_TYPE_TRANSIT_TPURSE_USAGE       : return "-";
        case MYKI_BR_TRANSACTION_TYPE_NON_TRANSIT_TPURSE_USAGE   : return "-";
        case MYKI_BR_TRANSACTION_TYPE_LOAD_PRODUCT_RENEWAL       : return "";
        case MYKI_BR_TRANSACTION_TYPE_UNKNOWN                    : return "";
        case MYKI_BR_TRANSACTION_TYPE_REFUND_VALUE               : return "";
        case MYKI_BR_TRANSACTION_TYPE_REFUND_PRODUCT             : return "";
        case MYKI_BR_TRANSACTION_TYPE_REVERSE_VALUE              : return "-";
        case MYKI_BR_TRANSACTION_TYPE_REVERSE_PRODUCT            : return "-";
        case MYKI_BR_TRANSACTION_TYPE_PRODUCT_AUTOLOAD_ENABLE    : return "";
        case MYKI_BR_TRANSACTION_TYPE_ACTIVATE_TRANSIT_APPLICATION : return "";
        case MYKI_BR_TRANSACTION_TYPE_CUSTOMER_CATEGORY_UPDATED  : return "";
        case MYKI_BR_TRANSACTION_TYPE_MULTIPLE_LOAD_ACTIONS      : return "";
        case MYKI_BR_TRANSACTION_TYPE_ON_BOARD_REVALIDATION      : return "";

        default: return "";
    }
}

    /**
     *  Retrieves directory entry and product given directory index.
     *  @param  dirIndex    directory entry index (>=1).
     *  @param  pDirectory  returned directory entry object address buffer.
     *  @param  pProduct    returned product object address buffer.
     */
int
myki_gac_GetCardProduct( unsigned int dirIndex, MYKI_Directory_t **pDirectory, MYKI_TAProduct_t **pProduct )
{
    MYKI_TAControl_t   *pMYKI_TAControl = NULL;

    if ( pDirectory != NULL )
        *pDirectory = NULL;

    if ( pProduct != NULL )
        *pProduct   = NULL;

    if ( dirIndex < 1 || dirIndex > DIMOF( pMYKI_TAControl->Directory ) - 1 )
    {
        CsErrx( "myki_gac_GetCardProduct() Directory index %d out of range (must be between 1 and %d)", dirIndex, DIMOF( pMYKI_TAControl->Directory ) - 1 );
        return -1;
    }

    if ( ! pDirectory )
    {
        CsErrx( "myki_gac_GetCardProduct() Directory buffer not specified" );
        return -1;
    }

    if ( MYKI_CS_TAControlGet( &pMYKI_TAControl ) < 0 )
    {
        CsErrx( "myki_gac_GetCardProduct() MYKI_CS_TAControlGet() failed" );
        return -1;
    }

    *pDirectory = &pMYKI_TAControl->Directory[ dirIndex ];

    if ( pProduct )
    {
        if ( (*pDirectory)->Status == TAPP_CONTROL_DIRECTORY_STATUS_UNUSED )
        {
            CsErrx( "myki_br_GetCardProduct() Product %d is unused", dirIndex );
            return -1;
        }

        if ( MYKI_CS_TAProductGet( (U8_t)( dirIndex - 1 ), pProduct ) < 0 )
        {
            CsErrx( "myki_br_GetCardProduct() MYKI_CS_TAProductGet( %d ) failed", dirIndex - 1 );
            return -1;
        }
    }

    return 0;
}

    /**
     *  Convert a Logical Zone into a Physical Zone.
     *  @param  value   logical zone number.
     *  @return Physical zone number.
     */
U8_t
LogicalZoneToPhysicalZone( U8_t value )
{
    if ( value > 1 )
        value--;

    return value;
}

    /**
     *  Converts a DateC19 into year, month, day, hour and minute variables.
     *  @param  dateC19 value to be converted.
     *  @param  pYear returned year buffer.
     *  @param  pMonth returned month buffer [1..12].
     *  @param  pMday returned day of month buffer [1..31].
     *  @param  pHour returned hour buffer [0..23].
     *  @param  pMinute returned minute buffer [0..59].
     */
void
DateC19ToDateYMDHHMM( DateC19_t dateC19, int *pYear, int *pMonth, int *pMday, int *pHour, int *pMinute )
{
    struct tm       stm;

    //  Convert DateC19_t to year(YYYY), month, mday
    if ( dateC19 == 0 )
    {
        if ( pYear   != NULL ) { *pYear     = 0; }
        if ( pMonth  != NULL ) { *pMonth    = 0; }
        if ( pMday   != NULL ) { *pMday     = 0; }
        if ( pHour   != NULL ) { *pHour     = 0; }
        if ( pMinute != NULL ) { *pMinute   = 0; }
    }
    else
    {
        DT_StructTmFromDateC19( dateC19, &stm );

        if ( pYear   != NULL ) { *pYear     = stm.tm_year + 1900; }
        if ( pMonth  != NULL ) { *pMonth    = stm.tm_mon  + 1;    }
        if ( pMday   != NULL ) { *pMday     = stm.tm_mday;        }
        if ( pHour   != NULL ) { *pHour     = stm.tm_hour;        }
        if ( pMinute != NULL ) { *pMinute   = stm.tm_min;         }
    }
}

    /**
     *  Converts CsTime_t value to ISO time string, eg. 2015-08-21T12:23:31+10:00.
     *  @param  pTm CsTime_t object to be converted.
     *  @param  len string buffer size.
     *  @param  pBuffer string buffer address.
     *  @return <0 if failed, otherwise number of string length.
     */
int32_t
CsStrTimeISO( const CsTime_t *pTm, int32_t len, char *pBuffer )
{
    struct tm   tmBuf;
    int         ret;
    time_t      tempSecs;

    if ( pBuffer == NULL )
    {
        return (-1);
    }

    int         tz  =  - ( pTm->tm_tzone / 60 );    // timezone offset appears to be inverted.

    tempSecs        = pTm->tm_secs;
    (void) localtime_r( &tempSecs, &tmBuf );

    ret = CsSnprintf( pBuffer, len, "%04d-%02d-%02dT%02d:%02d:%02d%s%02d:%02d",
            tmBuf.tm_year + 1900, tmBuf.tm_mon + 1, tmBuf.tm_mday,
            tmBuf.tm_hour, tmBuf.tm_min, tmBuf.tm_sec,
            tz > 0 ? "+" : "-",
            abs( ( tz - ( tz % 60 ) ) / 60 ),
            tz % 60 );

    return ret;
}

    /**
     *  Calculates LUHN check digit on data buffer.
     *  @param  pData data buffer including '0' check digit, updated
     *          with LUHN check digit on returning from function.
     */
static
void
calcLUHN( char *pData )
{
    int             len     = strlen( pData );
    int             weight  = 0;
    int             luhn    = 0;

    if ( ( len & 1 ) != 0 )
    {
        /*  ODD number of digits (including check digit) */
        weight  = 1;
    }
    else
    {
        /*  EVEN number of digits (including check digit) */
        weight  = 2;
    }
    for ( int index = 0; index < len; index++ )
    {
        int value   = pData[ index ] - '0';
        value      *= weight;
        luhn       += ( value / 10 ) + ( value % 10 );
        weight      = ( weight == 1 ) ? 2 : 1;
    }
    if ( luhn != 0 )
    {
        luhn    = ( 10 - ( luhn % 10 ) ) % 10;
    }
    pData[ len - 1 ]    = luhn + '0';
}
