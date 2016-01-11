/* -*- mode: c; tabs: 4; -*- */
/*=============================================================================
**
**    Vix Technology                   Licensed software
**    (C) 2014                         All rights reserved
**
**=============================================================================
**
**  Project/Product : MVU
**  Filename        : cardfunctions.cpp
**  Author(s)       : An Tran
**
**  Description     :
*/
/**     @file
**      @brief  Defines functions to process card instructions.
**
**      @section    SectionResults  Web Service Results
**      The Web service results returned in ["string_error"] field of JSON
**      response message can have one of the following values,\n
**      - "WS_SUCCESS", successful.
**      - "WS_UNCONFIRMED", card transaction was unconfirmed.
**      - "WS_ERROR", Web service catch-all error.
**      - "WS_ERROR_INVALID_PIN", invalid operator PIN entered.
**      - "WS_ERROR_BLOCKED", operator PIN retries exceeded, card blocked.
**      - "WS_ERROR_NOT_OPERATOR_CARD", transit card presented when operator
**        card was expected.
**      - "WS_ERROR_NOT_TRANSIT_CARD", operator card presented when transit
**        card was expected.
**      - "WS_ERROR_OPEN_CARD", failed opening card.
**      - "WS_ERROR_READ_CARD", failed reading card.
**      - "WS_ERROR_WRITE_CARD", failed updating card.
**      - "WS_ERROR_WRONG_CARD", wrong card presented for reversal transaction.
**
**      @section    SectionReasons  Accepted/Rejected Reasons
**      The card transaction accepted/rejected reasons returned in
**      ["data"]["string_reason"] field of JSON response message can have one
**      of the following values,\n
**      - "REASON_NOT_APPLICABLE", accepted/rejected, no specific reason.
**      - "REASON_CARD_BLOCKED", rejected, card blocked.
**      - "REASON_APPLICATION_BLOCKED", rejected, application blocked.
**      - "REASON_ALREADY_TOUCH_ON", rejected, already touched on.
**      - "REASON_ALREADY_TOUCH_OFF", rejected, already touched off.
**      - "REASON_EXPIRED", rejected, card or application expired.
**      - "REASON_INVALID_ZONE", rejected, invalid zone.
**      - "REASON_MISSING_TOUCH_ON", rejected, missing touch-on.
**      - "REASON_INSUFFICIENT_FUND", rejected, insufficient funds.
**      - "REASON_TOUCH_AGAIN", rejected, touch again.
**      - "REASON_MULTIPLE_CARDS", rejected, multiple cards detected.
**      - "REASON_CARD_NOT_ACTIVATED", rejected, card not activated.
**      - "REASON_CRYPTO_AUTH_FAIL", rejected, card corrupted.
**      - "REASON_UNREADABLE", rejected, card unreadable.
**      - "REASON_INVALID_SERVICE_PROVIDER", rejected, invalid service provider.
**      - "REASON_MAX_PIN_RETRIES_EXCEEDED", rejected, maximum PIN retries exceeded.
**      - "REASON_INVALID_PIN", rejected, invalid PIN.
**      - "REASON_INVALID_AMOUNT", rejected, TPurse load amount below minimum or
**        above maximum allowable.
**      - "REASON_ADDVALUE_DISABLED", rejected, Sale function disabled.
**      - "REASON_EXCEED_MAX_BALANCE", rejected, TPurse exceeds maximum value.
**      - "REASON_REVERSAL_NOT_ALLOWED", rejected, Sale reversal not allowed.
**      - "REASON_PRECOMMIT_CHECK_FAILED", rejected, pre-commit check failed.
**      - "REASON_INVALID_BALANCE", rejected, invalid TPurse balance.
**      - "REASON_CARD_EXPIRED", rejected, (OPERATOR) card expired.
**      - "REASON_OPAP_EXPIRED", rejected, (OPERATOR) application expired.
**      - "REASON_STAFF_EXPIRED", rejected, operator expired.
**      - "REASON_DISABLED_CARD", rejected, operator card disabled.
**      - "REASON_OPAP_DISABLED", rejected, operator application disabled.
**      - "REASON_OPAP_INACTIVE", rejected, operator application inactive.
**      - "REASON_TPURSE_MODIFIED", rejected, TPurse modified.
**
**      @section    SectionBlockingReason   Blocking Reasons
**      The card blocking reasons returned in ["data"]["string_blocking_reason"]
**      field of JSON response message can have one of the following values,\n
**      - "BLOCKING_REASON_UNDEFINED".
**      - "BLOCKING_REASON_LOST_STOLEN".
**      - "BLOCKING_REASON_HOTLISTED_SAM".
**      - "BLOCKING_REASON_BAD_CREDIT".
**      - "BLOCKING_REASON_SURRENDERED".
**      - "BLOCKING_REASON_SUSPENDED".
**      - "BLOCKING_REASON_DAMAGED".
**      - "BLOCKING_REASON_FAULTY".
**      - "BLOCKING_REASON_OTHER".
**      - "BLOCKING_REASON_DISABLED".
**      - "BLOCKING_REASON_RESERVED".
**
*/
/*  Member(s)       :
**
**  Information     :
**   Compiler(s)    : C++
**   Target(s)      : Independent
**
**  Subversion      :
**      $Id: cardfunctions.cpp 87801 2015-12-30 03:21:31Z atran $
**      $HeadURL: https://auperasvn01.aupera.erggroup.com/svn/DPG_SWBase/vgac_readerapp/trunk/src/cardfunctions.cpp $
**
**  History         :
**   Vers.  Date        Aut.  Type     Description
**   -----  ----------  ----  -------  ----------------------------------------
**    1.00  04.09.15    ANT   Create
**    1.01  16.09.15    ANT   Add      Touch on/off "isconcession" flag
**    1.02  30.09.15    ANT   Add      Touch on/off "isforcedscanoff" flag
**    1.03  12.11.15    ANT   Add      NGBU-838: Added returning blocking reason
**                                     NGBU-837: Added multiple cards detection
**    1.04  13.11.15    ANT   Modify   NGBU-771: Removed unnecessary card
**                                     operations
**
**===========================================================================*/

/*
 *      Options
 *      -------
 */

#define OPT_NOT_OPTIMISED               0

/*
 *      Includes
 *      --------
 */

#include <string>
#include <json/json.h>
#include <cs.h>
#include <myki_cardservices.h>
#include <myki_alarm.h>
#include <myki_alarm_names.h>
#include <myki_cdd_enums.h>
#include <myki_cd.h>
#include <myki_br.h>
#include <myki_tmi.h>

#include "app_debug_levels.h"
#include "card_processing_common.h"
#include "card_processing_thread.h"
#include "datastring.h"
#include "gentmi.h"
#include "cardfunctions.h"
#include "wsshift.h"

/*
 *      External References
 *      -------------------
 */

    /*  BR_COMMON.C */
extern  "C" ProductType_e           myki_br_cd_GetProductType( int productId );

    /*  GAC_PROCESSING_THREAD.CPP - TODO: Move to header file */
extern  "C" MYKI_BR_ContextData_t  *GetBrContextDataOut( void );
extern  bool                        preValidateCard( bool &operatorCard, int nCardsDetected );
extern  bool                        ExecuteBusinessSequence( int numberOfCardsPresent, bool operatorCard, int &passengerCode, SequenceFunction_e businessSequence, bool forceProcessBR );

extern  bool                        g_calculatePassExpiry;
extern  U16_t                       g_TPurseLowWarningThreshold;

/*
 *      Local Constants and Macros
 *      --------------------------
 */

/*
 *      Local Prototypes
 *      ----------------
 */

static  Time_t          getProductExpiry( MYKI_BR_ContextData_t *pData );

/*
 *      Global Variables
 *      ----------------
 */

    /*  TEST_ONLY */
OAppShiftDataStatus_t   g_overwriteShiftStatus      = OAPP_SHIFT_DATA_STATUS_UNUSED;
Time_t                  g_overwriteShiftStartTime   = TIME_NOT_SET;
Time_t                  g_overwriteShiftCloseTime   = TIME_NOT_SET;

    /**
     *  @brief  Processes operator PIN validation request.
     *  @param  request Json::Value object containing JSON request message
     *          of the following format,
     *  @code
     *  {
     *      "terminalid":"1234",
     *      "name":"validatepin",
     *      "data":
     *      {
     *          "pin":"01234567",
     *          "oldpin":"76543210",
     *      }
     *  }
     *  @endcode
     *  where,
     *  @li     @c terminalid is the terminal id of the device that
     *          initiated the transaction,
     *  @li     @c name is "validatepin", and
     *  @li     @c data.pin, if name is "valdiatepin", is operator entered PIN.
     *  @li     @c data.oldpin, if name is "changepin", is operator entered old PIN.
     *  @return The string object representing JSON response message of
     *          the following format,
     *  @code
     *  {
     *      "terminalid":"1234",
     *      "name":"validatepin",
     *      "errorcode":0,
     *      "string_error":"WS_SUCCESS",
     *      "data":
     *      {
     *          "string_reason":"REASON_INVALID_PIN",
     *          "string_blocking_reason":"BLOCKING_REASON_OTHER",
     *          "expiry":"24 Dec 15 12:00 pm"
     *      }
     *  }
     *  @endcode
     *  where,
     *  @li     @c terminalid is the terminal id of the device that
     *          initiated the transaction,
     *  @li     @c name is "validatepin" or "changepin", and
     *  @li     @c errorcode is 0 if successful, otherwise failed (DEPRECATED, use string_error).
     *  @li     @c string_error is text representation of PIN validation result. Refer
     *          to @ref SectionResults for more details.
     */
std::string
processValidatePIN( Json::Value &request )
{
    #define VALIDATE_PIN_SUCCESS                    0
    #define VALIDATE_PIN_ERROR                      1   /*  Error validating PIN - Catch all */
    #define VALIDATE_PIN_ERROR_INVALID              2   /*  Error validating PIN - Invalid PIN */
    #define VALIDATE_PIN_ERROR_BLOCKED              3   /*  Error validating PIN - Too many PIN retries */
    #define VALIDATE_PIN_ERROR_NOT_OPERATOR_CARD    4   /*  Not operator card */
    #define VALIDATE_PIN_ERROR_OPEN_OPERATOR_CARD   5   /*  Failed opening operator card */

    Json::Value             response;
    Json::FastWriter        fw;
    MYKI_BR_ContextData_t  *pDataIn         = GetCardProcessingThreadContextData();
    MYKI_BR_ContextData_t  *pDataOut        = GetBrContextDataOut();
    std::string             pin             = "";
    bool                    isOperatorCard  = false;
    int                     passengerCode   = 0;
    #if     OPT_NOT_OPTIMISED
    int                     oneCardDetected = 1;
    #endif

    CsDebug( APP_DEBUG_FLOW, ( APP_DEBUG_FLOW, "processValidatePIN" ) );

    createBaseResponse( request, response );

    memset( &g_ShiftData, 0, sizeof( g_ShiftData ) );

    if ( request[ "name" ] == "changepin")
    {
        pDataIn->DynamicData.pShiftData    = NULL;
        if ( request[ "data" ][ "oldpin" ].isString() )
        {
            pin = request[ "data" ][ "oldpin" ].asString().substr( 0, 8 ); // Get up to max 8 chars
        }
    }
    else
    {
        /*  Validating login PIN => Retrieves shift data if Driver card */
        pDataIn->DynamicData.pShiftData    = &g_ShiftData;
        if ( request[ "data" ][ "pin" ].isString() )
        {
            pin = request[ "data" ][ "pin" ].asString().substr( 0, 8 ); // Get up to max 8 chars
        }
    }

    memset( pDataIn->DynamicData.operatorPin, 0, sizeof( pDataIn->DynamicData.operatorPin ) );
    for ( unsigned int i = 0; i < pin.size(); i++ )
    {
        // REVIEW: Should alpha numeric pins be rejected?
        // isDigit test?
        pDataIn->DynamicData.operatorPin[ i ]  = pin.at( i );
    }

    /*  Start new card session */
    #if     OPT_NOT_OPTIMISED
    if ( preValidateCard( isOperatorCard, oneCardDetected ) == true )
    #else
    {
        isOperatorCard  = pDataOut->DynamicData.isOperator == FALSE ? false : true;
    }
    #endif
    {
        if ( isOperatorCard == true )
        {
            /*  TEST_ONLY:  Overwrite Driver shift data */
            if ( g_overwriteShiftStatus != OAPP_SHIFT_DATA_STATUS_UNUSED )
            {
                MYKI_CAIssuer_t                *pMYKI_CAIssuer              = NULL;
                MYKI_CAControl_t               *pMYKI_CAControl             = NULL;
                MYKI_OAIssuer_t                *pMYKI_OAIssuer              = NULL;
                MYKI_OAControl_t               *pMYKI_OAControl             = NULL;
                MYKI_OAShiftDataControl_t      *pMYKI_OAShiftDataControl    = NULL;

                if ( MYKI_CS_CAIssuerGet(           &pMYKI_CAIssuer           ) == MYKI_CS_OK &&
                     MYKI_CS_CAControlGet(          &pMYKI_CAControl          ) == MYKI_CS_OK &&
                     MYKI_CS_OAIssuerGet(           &pMYKI_OAIssuer           ) == MYKI_CS_OK &&
                     MYKI_CS_OAControlGet(          &pMYKI_OAControl          ) == MYKI_CS_OK &&
                     MYKI_CS_OAShiftDataControlGet( &pMYKI_OAShiftDataControl ) == MYKI_CS_OK )
                {
                    pMYKI_OAShiftDataControl->Status        = g_overwriteShiftStatus;
                    if ( g_overwriteShiftStartTime != TIME_NOT_SET )
                    {
                        pMYKI_OAShiftDataControl->StartTime = g_overwriteShiftStartTime;
                    }
                    pMYKI_OAShiftDataControl->CloseTime     = g_overwriteShiftCloseTime;
                    CsDbg( APP_DEBUG_FLOW, "processValidatePIN : OAShiftDataControl.Status    << %s",
                            pMYKI_OAShiftDataControl->Status == OAPP_SHIFT_DATA_STATUS_ACTIVATED ? "ACTIVATED" :
                            pMYKI_OAShiftDataControl->Status == OAPP_SHIFT_DATA_STATUS_SUSPENDED ? "SUSPENDED" :
                            pMYKI_OAShiftDataControl->Status == OAPP_SHIFT_DATA_STATUS_CLOSED    ? "CLOSED"    : "UNUSED" );
                    CsDbg( APP_DEBUG_FLOW, "processValidatePIN : OAShiftDataControl.StartTime << %d", pMYKI_OAShiftDataControl->StartTime );
                    CsDbg( APP_DEBUG_FLOW, "processValidatePIN : OAShiftDataControl.CloseTime << %d", pMYKI_OAShiftDataControl->CloseTime );
                }
                else
                {
                    CsErrx( "processValidatePIN : Could not overwrite OAShiftDataControl" );
                }
            }

            /*  Execute business sequence */
            if ( ExecuteBusinessSequence( 1 /*ONE_CARD*/, isOperatorCard, passengerCode, SEQ_FUNCTION_VALIDATE_PIN, false ) == true )
            {
                response[ "data" ][ "string_reason" ]   = \
                                                  getAcceptReasonString( pDataOut->ReturnedData.acceptReason );
                response[ "errorcode"    ]      = VALIDATE_PIN_SUCCESS;
                response[ "string_error" ]      = getResultString( WS_SUCCESS );
            }
            else
            {
                response[ "data" ][ "string_reason" ]   = \
                                                  getRejectReasonString( pDataOut->ReturnedData.rejectReason );
                switch ( pDataOut->ReturnedData.rejectReason )
                {
                case    MYKI_BR_REJECT_REASON_INVALID_PIN:
                    response[ "errorcode"    ]  = VALIDATE_PIN_ERROR_INVALID;
                    response[ "string_error" ]  = getResultString( WS_ERROR_INVALID_PIN );
                    break;

                case    MYKI_BR_REJECT_REASON_MAX_PIN_RETRIES_EXCEEDED:
                    SetAlarm(   AlarmMaxPinRetriesExceeded );
                    ClearAlarm( AlarmMaxPinRetriesExceeded );
                    /*  DROP THROUGH TO NEXT CASE! */

                case    MYKI_BR_REJECT_REASON_CARD_BLOCKED:
                case    MYKI_BR_REJECT_REASON_APPLICATION_BLOCKED:
                    response[ "data" ][ "string_blocking_reason" ]  = \
                                                      getBlockingReasonCode( pDataOut->ReturnedData.blockingReason );
                    response[ "errorcode"       ]   = VALIDATE_PIN_ERROR_BLOCKED;
                    response[ "string_error"    ]   = getResultString( WS_ERROR_BLOCKED );
                    break;

                case    MYKI_BR_REJECT_REASON_EXPIRED:
                    response[ "data" ][ "expiry"    ]   = getExpiry( pDataOut );
                    /*  DROP THROUGH TO NEXT CASE! */

                default:
                    response[ "errorcode"    ]  = VALIDATE_PIN_ERROR;
                    response[ "string_error" ]  = getResultString( WS_ERROR );
                    break;
                }
            }

            /*  TEST_ONLY:  Clears overwritten shift data */
            {
                g_overwriteShiftStatus      = OAPP_SHIFT_DATA_STATUS_UNUSED;
                g_overwriteShiftStartTime   = TIME_NOT_SET;
                g_overwriteShiftCloseTime   = TIME_NOT_SET;
            }
        }
        else
        {
            /*  Not OPERATOR card!? */
            CsErrx( "processValidatePIN : none OPERATOR card detected" );
            response[ "errorcode"    ]  = VALIDATE_PIN_ERROR_NOT_OPERATOR_CARD;
            response[ "string_error" ]  = getResultString( WS_ERROR_NOT_OPERATOR_CARD );
        }

        #if     OPT_NOT_OPTIMISED
        /*  Done with this card session */
        (void)MYKI_CS_CloseCard( );

        /*  And back waiting for card to be removed */
        MYKI_CS_DetectCard( TRUE );
        #endif
    }
    #if     OPT_NOT_OPTIMISED
    else
    {
        /*  Failed (re)opening OPERATOR card */
        CsErrx( "processValidatePIN : failed (re)opening OPERATOR card" );
        response[ "errorcode"    ]  = VALIDATE_PIN_ERROR_OPEN_OPERATOR_CARD;
        response[ "string_error" ]  = getResultString( WS_ERROR_OPEN_CARD );
    }
    #endif

    /*  Clean-up */
    pDataIn->DynamicData.pShiftData    = NULL;
    memset( pDataIn->DynamicData.operatorPin, 0, sizeof( pDataIn->DynamicData.operatorPin ) );

    return fw.write( response );
}

    /**
     *  @brief  Processes operator PIN update request.
     *  @param  request Json::Value object containing JSON request message
     *          of the following format,
     *  @code
     *  {
     *      "terminalid":"1234",
     *      "name":"changepin",
     *      "data":
     *      {
     *          "type":"change",
     *          "oldpin":"01234567",
     *          "newpin":"76543210"
     *      }
     *  }
     *  @endcode
     *  where,
     *  @li     @c terminalid is the terminal id of the device that
     *          initiated the transaction,
     *  @li     @c name is "changepin",
     *  @li     @c data.type is "change" or "set".
     *  @li     @c data.oldpin, if data.type is "change", is operator entered old PIN.
     *  @li     @c data.newpin is operator entered new PIN
     *  @return The string object representing JSON response message of
     *          the following format,
     *  @code
     *  {
     *      "terminalid":"1234",
     *      "name":"changepin",
     *      "errorcode":0,
     *      "string_error":"WS_SUCCESS",
     *      "data":
     *      {
     *          "string_reason":"REASON_INVALID_PIN",
     *          "string_blocking_reason":"BLOCKING_REASON_OTHER",
     *          "expiry":"24 Dec 15 12:00 pm"
     *      }
     *  }
     *  @endcode
     *  where,
     *  @li     @c terminalid is the terminal id of the device that
     *          initiated the transaction,
     *  @li     @c name is "changepin" or "resetpin", and
     *  @li     @c errorcode is 0 if successful, otherwise failed (DEPRECATED, use string_error).
     *  @li     @c string_error is text representation of change PIN result. Refer to
     *          @ref SectionResults for more details.
     */
std::string
processChangePIN( Json::Value &request )
{
    #define UPDATE_PIN_SUCCESS                      0
    #define UPDATE_PIN_ERROR                        1   /*  Error updating PIN - Catch all */
    #define UPDATE_PIN_ERROR_NOT_OPERATOR_CARD      4   /*  Not operator card */
    #define UPDATE_PIN_ERROR_OPEN_OPERATOR_CARD     5   /*  Failed opening operator card */

    Json::Value             response;
    Json::FastWriter        fw;
    MYKI_BR_ContextData_t  *pDataIn         = GetCardProcessingThreadContextData();
    MYKI_BR_ContextData_t  *pDataOut        = GetBrContextDataOut();
    std::string             pin             = "";
    bool                    isOperatorCard  = false;
    int                     passengerCode   = 0;
    #if     OPT_NOT_OPTIMISED
    int                     oneCardDetected = 1;
    #endif

    CsDebug( APP_DEBUG_FLOW, ( APP_DEBUG_FLOW, "processChangePIN" ) );

    createBaseResponse( request, response );

    if ( request[ "data" ][ "type" ] == "change" )
    {
        std::string validateResponse = processValidatePIN(request);

        Json::Reader reader;
        if ( reader.parse( validateResponse.c_str(), response ) && response.size() > 0 )
        {
            if ( response[ "errorcode" ] != UPDATE_PIN_SUCCESS )
                return fw.write( response );
        }
    }

    if ( request[ "data" ][ "newpin" ].isString() )
    {
        pin = request[ "data" ][ "newpin" ].asString().substr( 0, 8 ); // Get up to max 8 chars
    }

    memset( pDataIn->DynamicData.operatorPin, 0, sizeof( pDataIn->DynamicData.operatorPin ) );
    for ( unsigned int i = 0; i < pin.size(); i++ )
    {
        // REVIEW: Should alpha numeric pins be rejected?
        // isDigit test?
        pDataIn->DynamicData.operatorPin[ i ]  = pin.at( i );
    }

    /*  Start new card session */
    #if     OPT_NOT_OPTIMISED
    if ( preValidateCard( isOperatorCard, oneCardDetected ) == true )
    #else
    {
        isOperatorCard  = pDataOut->DynamicData.isOperator == FALSE ? false : true;
    }
    #endif
    {
        if ( isOperatorCard == true )
        {
            /*  Execute business sequence */
            if ( ExecuteBusinessSequence( 1 /*ONE_CARD*/, isOperatorCard, passengerCode, SEQ_FUNCTION_UPDATE_PIN, false ) == true )
            {
                response[ "data" ][ "string_reason" ]   = \
                                              getAcceptReasonString( pDataOut->ReturnedData.acceptReason );
                response[ "errorcode"    ]  = UPDATE_PIN_SUCCESS;
                response[ "string_error" ]  = getResultString( WS_SUCCESS );
            }
            else
            {
                response[ "data" ][ "string_reason" ]   = \
                                              getRejectReasonString( pDataOut->ReturnedData.rejectReason );
                response[ "errorcode"    ]  = UPDATE_PIN_ERROR;
                response[ "string_error" ]  = getResultString( WS_ERROR );
            }
        }
        else
        {
            /*  Not OPERATOR card!? */
            CsErrx( "processChangePIN : none OPERATOR card detected" );
            response[ "errorcode"    ]  = UPDATE_PIN_ERROR_NOT_OPERATOR_CARD;
            response[ "string_error" ]  = getResultString( WS_ERROR_NOT_OPERATOR_CARD );
        }

        /*  Done with this card session */
        #if     OPT_NOT_OPTIMISED
        (void)MYKI_CS_CloseCard( );

        /*  And back waiting for card to be removed */
        MYKI_CS_DetectCard( TRUE );
        #endif
    }
    #if     OPT_NOT_OPTIMISED
    else
    {
        /*  Failed (re)opening OPERATOR card */
        CsErrx( "processChangePIN : failed (re)opening OPERATOR card" );
        response[ "errorcode"    ]  = UPDATE_PIN_ERROR_OPEN_OPERATOR_CARD;
        response[ "string_error" ]  = getResultString( WS_ERROR_OPEN_CARD );
    }
    #endif

    return fw.write( response );
}

    /**
     *  @brief  Processes TPurse load request.
     *  @param  request Json::Value object containing JSON request message
     *          of the following format,
     *  @code
     *  {
     *      "terminalid":"1234",
     *      "name":"tpurseload",
     *      "data":
     *      {
     *          "cardnumber":"0123456789ABCDEF",
     *          "amount":1500
     *      }
     *  }
     *  @endcode
     *  where,
     *  @li     @c terminalid is the terminal id of the device that
     *          initiated the transaction,
     *  @li     @c name is "tpurseload",
     *  @li     @c data.cardnumber is the card serial number,
     *  @li     @c data.amount is the TPurse value to be reversed,
     *  @return The string object representing JSON response message of
     *          the following format,
     *  @code
     *  {
     *      "terminalid":"1234",
     *      "name":"tpurseload",
     *      "errorcode":0,
     *      "string_error":"WS_SUCCESS"
     *      "data":
     *      {
     *          "cardnumber":"0123456789ABCDEF",
     *          "addedvalue":1500,
     *          "remvalue":5200,
     *          "txnseqno":1234,
     *          "saleseqno":1212,
     *          "paymenttype":"cash",
     *          "reason":0,
     *          "string_reason":"REASON_NOT_APPLICABLE",
     *          "string_blocking_reason":"BLOCKING_REASON_LOST_STOLEN",
     *          "expiry":"24 Dec 15 12:00 pm"
     *      }
     *  }
     *  @endcode
     *  where,
     *  @li     @c terminalid is the terminal id of the device that
     *          initiated the transaction,
     *  @li     @c name is "tpurseload",
     *  @li     @c errorcode is 0 if successful, otherwise failed (DEPRECATED,
     *          use string_error).
     *  @li     @c string_error is text representation of TPurse load result.
     *          Refer to @ref SectionResults for more details.
     *  @li     @c data.cardnumber is the card serial number,
     *  @li     @c data.addedvalue is the TPurse value added,
     *  @li     @c data.remvalue is the TPurse remaining value after
     *          transaction,
     *  @li     @c data.txnseqno is the transaction sequence number
     *          returned from card,
     *  @li     @c saleseqno is the Sale transaction sequence number
     *          (printed on receipt),
     *  @li     @c paymenttype is the transaction payment type, and
     *  @li     @c data.reason is the accepted/rejected reason code (refer to
     *          myki_br_context_data.h for more details - DEPRECATED,
     *          use data.string_reason).
     *  @li     @c data.string_reason is the text representation of the
     *          accepted/rejected reason. Refer to @ref SectionReasons for
     *          more details.
     */
std::string
processTPurseLoad( Json::Value &request )
{
    #define TPURSELOAD_SUCCESS                      0
    #define TPURSELOAD_ERROR                        1   /*  Error - Catch all */
    #define TPURSELOAD_NOT_TRANSIT_CARD             2   /*  Error - Catch all */
    #define TPURSELOAD_ERROR_OPEN_CARD              3   // REVIEW Should have generic error list
    // TODO Add extra error codes to handle failures in MYKI_LDT_TPurseLoad and MYKI_LDT_TPurseLoadReverse

    Json::Value             response;
    Json::FastWriter        fw;
    MYKI_BR_ContextData_t  *pDataIn         = GetCardProcessingThreadContextData();
    MYKI_BR_ContextData_t  *pDataOut        = GetBrContextDataOut();
    std::string             pin             = "";
    bool                    isOperatorCard  = false;
    int                     passengerCode   = 0;
    #if     OPT_NOT_OPTIMISED
    int                     oneCardDetected = 1;
    #endif
    U32_t                   amount          = (U32_t)getJsonInt( request[ "data" ], "amount" );

    CsDbg( APP_DEBUG_FLOW, "processTPurseLoad : amount=%d", amount );

    createBaseResponse( request, response );

    /*  Start new card session */
    #if     OPT_NOT_OPTIMISED
    if ( preValidateCard( isOperatorCard, oneCardDetected ) == true )
    #else
    {
        isOperatorCard  = pDataOut->DynamicData.isOperator == FALSE ? false : true;
    }
    #endif
    {
        if ( isOperatorCard == false )
        {
            std::string  csn    = getMykiPAN( );

            /*  Sets TPurse load business rule parameters */
            pDataIn->DynamicData.tpurseLoadAmount       = (Currency_t)amount;

            /*  Execute business sequence */
            if ( ExecuteBusinessSequence( 1 /*ONE_CARD*/, isOperatorCard, passengerCode, SEQ_FUNCTION_ADD_VALUE, true ) == true )
            {
                if ( CreateTPurseLoadTmi( pDataOut ) == 0 )
                {
                    unsigned int     globalSalesTxnNum      = 0;

                    /*  Retrieves last generated (sale) transaction record number */
                    if ( TMI_getCounter("salesTransactionCount", &globalSalesTxnNum ) < 0 )
                    {
                        CsErrx( "processTPurseLoad : TMI_getCounter() failed" );
                        globalSalesTxnNum                   = 0;
                    }

                    response[ "data" ][ "cardnumber"    ]   = csn.c_str( );
                    response[ "data" ][ "addedvalue"    ]   = pDataOut->ReturnedData.topupAmount;
                    response[ "data" ][ "remvalue"      ]   = pDataOut->ReturnedData.remainingBalance;
                    response[ "data" ][ "txnamount"     ]   = pDataOut->ReturnedData.topupAmount;
                    response[ "data" ][ "txnseqno"      ]   = pDataOut->ReturnedData.txnSeqNo;              /*  Card (TAPurseControl) transaction sequence number */
                    response[ "data" ][ "saleseqno"     ]   = globalSalesTxnNum;
                    response[ "data" ][ "paymenttype"   ]   = "cash";
                    response[ "data" ][ "reason"        ]   = pDataOut->ReturnedData.acceptReason;
                    response[ "data" ][ "string_reason" ]   = getAcceptReasonString( pDataOut->ReturnedData.acceptReason );
                    response[ "errorcode"               ]   = TPURSELOAD_SUCCESS;
                    response[ "string_error"            ]   = getResultString( WS_SUCCESS );
                }
                else
                {
                    /*  Failed generating TPurse load TMI record */
                    response[ "data" ][ "cardnumber"    ]   = csn.c_str( );
                    response[ "data" ][ "remvalue"      ]   = getRemainingValue( );
                    response[ "errorcode"               ]   = TPURSELOAD_ERROR;
                    response[ "data" ][ "string_reason" ]   = getRejectReasonString( MYKI_BR_REJECT_REASON_NOT_APPLICABLE );
                    response[ "string_error"            ]   = getResultString( WS_ERROR );

                    /*  TODO:   Generate alarm */
                }
            }
            else
            if ( pDataOut->ReturnedData.sequenceResult == SEQ_RESULT_REJECT )
            {
                /*  Transaction rejected by business rule */
                response[ "data" ][ "cardnumber"    ]   = csn.c_str( );
                response[ "data" ][ "remvalue"      ]   = getRemainingValue( );
                response[ "data" ][ "reason"        ]   = pDataOut->ReturnedData.rejectReason;
                response[ "data" ][ "string_reason" ]   = getRejectReasonString( pDataOut->ReturnedData.rejectReason );
                response[ "errorcode"               ]   = TPURSELOAD_ERROR;
                response[ "string_error"            ]   = getResultString( WS_ERROR );
                switch ( pDataOut->ReturnedData.rejectReason )
                {
                case    MYKI_BR_REJECT_REASON_CARD_BLOCKED:
                case    MYKI_BR_REJECT_REASON_APPLICATION_BLOCKED:
                    response[ "data" ][ "string_blocking_reason" ]  = \
                                                          getBlockingReasonCode( pDataOut->ReturnedData.blockingReason );
                    break;

                case    MYKI_BR_REJECT_REASON_EXPIRED:
                    response[ "data" ][ "expiry"    ]   = getExpiry( pDataOut );
                    break;

                default:
                    /*  Others! DONOTHING! */
                    break;
                }
            }
            else
            {
                /*  Failed read/write card */
                response[ "data" ][ "cardnumber"    ]   = csn.c_str( );
                response[ "errorcode"               ]   = TPURSELOAD_ERROR_OPEN_CARD;
                response[ "string_error"            ]   = getResultString( WS_ERROR_READ_CARD );
            }
        }
        else
        {
            /*  Not Transit card!? */
            CsErrx( "processTPurseLoad : none TRANSIT card detected" );
            response[ "errorcode"                   ]   = TPURSELOAD_NOT_TRANSIT_CARD;
            response[ "string_error"                ]   = getResultString( WS_ERROR_NOT_TRANSIT_CARD );
        }

        #if     OPT_NOT_OPTIMISED
        /*  Done with this card session */
        (void)MYKI_CS_CloseCard( );

        /*  And back waiting for card to be removed */
        MYKI_CS_DetectCard( TRUE );
        #endif
    }
    #if     OPT_NOT_OPTIMISED
    else
    if ( oneCardDetected > 1 )
    {
        /*  Multiple cards detected */
        response[ "data" ][ "string_reason" ]   = getRejectReasonString( MYKI_BR_REJECT_REASON_MULTIPLE_CARDS );
        response[ "errorcode"       ]           = TPURSELOAD_ERROR;
        response[ "string_error"    ]           = getResultString( WS_ERROR );
    }
    else
    {
        /*  Failed (re)opening TRANSIT card */
        CsErrx( "processTPurseLoad : failed (re)opening TRANSI card" );
        response[ "errorcode"       ]   = TPURSELOAD_ERROR_OPEN_CARD;
        response[ "string_error"    ]   = getResultString( WS_ERROR_OPEN_CARD );
    }
    #endif

    return fw.write( response );
}

    /**
     *  @brief  Processes TPurse load reversal request.
     *  @param  request Json::Value object containing JSON request message
     *          of the following format,
     *  @code
     *  {
     *      "terminalid":"1234",
     *      "name":"tpurseloadreversal",
     *      "data":
     *      {
     *          "cardnumber":"0123456789ABCDEF",
     *          "amount":1500,
     *          "txnseqno":1234,
     *          "saleseqno":1212,
     *          "paymenttype":"cash"
     *      }
     *  }
     *  @endcode
     *  where,
     *  @li     @c terminalid is the terminal id of the device that
     *          initiated the transaction,
     *  @li     @c name is "tpurseloadreversal",
     *  @li     @c data.cardnumber is the card serial number,
     *  @li     @c data.amount is the TPurse value to be reversed,
     *  @li     @c data.txnseqno is the original transaction sequence
     *          number returned from card,
     *  @li     @c saleseqno is the original Sale transaction sequence
     *          number (printed on receipt), and
     *  @li     @c paymenttype is the transaction payment type.
     *  @return The string object representing JSON response message of
     *          the following format,
     *  @code
     *  {
     *      "terminalid":"1234",
     *      "name":"tpurseloadreversal",
     *      "errorcode":0,
     *      "string_error":"SUCCESS",
     *      "data":
     *      {
     *          "cardnumber":"0123456789ABCDEF",
     *          "deductedvalue":1500,
     *          "remvalue":5200,
     *          "txnseqno":1234,
     *          "saleseqno":1212,
     *          "revtxnseqno":4321,
     *          "revsaleseqno":2121,
     *          "paymenttype":"cash",
     *          "reason":0,
     *          "string_reason":"REASON_NOT_APPLICABLE",
     *          "string_blocking_reason":"BLOCKING_REASON_LOST_STOLEN",
     *          "expiry":"24 Dec 15 12:00 pm"
     *      }
     *  }
     *  @endcode
     *  where,
     *  @li     @c terminalid is the terminal id of the device that
     *          initiated the transaction,
     *  @li     @c name is "tpurseloadreversal",
     *  @li     @c errorcode is 0 if successful, otherwise failed (DEPRECATED,
     *          use string_error).
     *  @li     @c string_error is text representation of TPurse load
     *          reversal result. Refer to @ref SectionResults for more details.
     *  @li     @c data.cardnumber is the card serial number,
     *  @li     @c data.deductedvalue is the deducted TPurse value,
     *  @li     @c data.remvalue is the TPurse remaining value after
     *          transaction,
     *  @li     @c data.txnseqno is the original transaction sequence
     *          number returned from card,
     *  @li     @c saleseqno is the original Sale transaction sequence
     *          number,
     *  @li     @c revtxnseqno is the reversal transaction sequence
     *          number returned from the card,
     *  @li     @c revsaleseqno is the Sale Reversal transaction
     *          sequence number (printed on receipt),
     *  @li     @c paymenttype is the transaction payment type, and
     *  @li     @c data.reason is the accepted/rejected reason code (refer to
     *          myki_br_context_data.h for more details - DEPRECATED, use
     *          data.string_reason).
     *  @li     @c data.string_reason is the text representation of the
     *          accepted/rejected reason. Refer to @ref SectionReasons for
     *          more details.
     */
std::string
processTPurseLoadReversal( Json::Value &request )
{
    #define TPURSELOAD_REVERSAL_SUCCESS             0
    #define TPURSELOAD_REVERSAL_ERROR               1   /**< Error - Catch all */
    #define TPURSELOAD_REVERSAL_NOT_TRANSIT_CARD    2   /**< Error - Not a Transit card */
    #define TPURSELOAD_REVERSAL_ERROR_OPEN_CARD     3   /**< Error - Read/write card */
    #define TPURSELOAD_REVERSAL_ERROR_WRONG_CARD    4   /**< Error - Wrong card */

    Json::Value             response;
    Json::FastWriter        fw;
    MYKI_BR_ContextData_t  *pDataIn         = GetCardProcessingThreadContextData();
    MYKI_BR_ContextData_t  *pDataOut        = GetBrContextDataOut();
    std::string             pin             = "";
    bool                    isOperatorCard  = false;
    int                     passengerCode   = 0;
    #if     OPT_NOT_OPTIMISED
    int                     oneCardDetected = 1;
    #endif
    std::string             cardNumber      =        getJsonString( request[ "data" ],  "cardnumber"    );
    U32_t                   amount          = (U32_t)getJsonInt(    request[ "data" ],  "amount"        );
    int                     txSeqNo         =        getJsonInt(    request[ "data" ],  "txnseqno"      );
    int                     saleSeqNo       =        getJsonInt(    request[ "data" ],  "saleseqno"     );
    std::string             paymentType     =        getJsonString( request[ "data" ],  "paymenttype"   );

    CsDbg( APP_DEBUG_FLOW, "processTPurseLoadReversal : amount=%d, txSeqNo=%d", amount, txSeqNo );

    createBaseResponse( request, response );

    /*  Start new card session */
    #if     OPT_NOT_OPTIMISED
    if ( preValidateCard( isOperatorCard, oneCardDetected ) == true )
    #else
    {
        isOperatorCard  = pDataOut->DynamicData.isOperator == FALSE ? false : true;
    }
    #endif
    {
        if ( isOperatorCard == false )
        {
            std::string     csn             = getMykiPAN( );

            if ( cardNumber == csn )
            {
                /*  Sets TPurse load reversal business rule parameters */
                pDataIn->DynamicData.tpurseLoadAmount   = (Currency_t)amount;
                pDataIn->DynamicData.reverseTxSeqNo     = txSeqNo;

                /*  Execute business sequence */
                if ( ExecuteBusinessSequence( 1 /*ONE_CARD*/, isOperatorCard, passengerCode, SEQ_FUNCTION_ADD_VALUE_REVERSAL, true ) == true )
                {
                    if ( CreateTPurseLoadReversalTmi( pDataOut ) == 0 )
                    {
                        unsigned     globalSalesTxnNum               = 0;

                        /*  Retrieves last generated (sale) transaction record number */
                        if ( TMI_getCounter("salesTransactionCount", &globalSalesTxnNum ) < 0 )
                        {
                            CsErrx( "processTPurseLoad : TMI_getCounter() failed" );
                            globalSalesTxnNum                   = 0;
                        }

                        response[ "data" ][ "cardnumber"    ]   = csn.c_str( );
                        response[ "data" ][ "deductedvalue" ]   = pDataOut->ReturnedData.topupAmount;
                        response[ "data" ][ "remvalue"      ]   = pDataOut->ReturnedData.remainingBalance;
                        response[ "data" ][ "txnseqno"      ]   = txSeqNo;
                        response[ "data" ][ "saleseqno"     ]   = saleSeqNo;
                        response[ "data" ][ "revtxnseqno"   ]   = pDataOut->ReturnedData.txnSeqNo;              /*  Card (TAPurseControl) transaction sequence number */
                        response[ "data" ][ "revsaleseqno"  ]   = globalSalesTxnNum;
                        response[ "data" ][ "paymenttype"   ]   = "cash";
                        response[ "data" ][ "reason"        ]   = pDataOut->ReturnedData.acceptReason;
                        response[ "data" ][ "string_reason" ]   = getAcceptReasonString( pDataOut->ReturnedData.acceptReason );
                        response[ "errorcode"               ]   = TPURSELOAD_REVERSAL_SUCCESS;
                        response[ "string_error"            ]   = getResultString( WS_SUCCESS );
                    }
                    else
                    {
                        /*  Failed generating TPurse load TMI record */
                        response[ "data" ][ "cardnumber"    ]   = csn.c_str( );
                        response[ "data" ][ "remvalue"      ]   = getRemainingValue( );
                        response[ "errorcode"               ]   = TPURSELOAD_REVERSAL_ERROR;
                        response[ "data" ][ "string_reason" ]   = getRejectReasonString( MYKI_BR_REJECT_REASON_NOT_APPLICABLE );
                        response[ "string_error"            ]   = getResultString( WS_ERROR );

                        /*  TODO:   Generate alarm */
                    }
                }
                else
                if ( pDataOut->ReturnedData.sequenceResult == SEQ_RESULT_REJECT )
                {
                    /*  Transaction rejected by business rule */
                    response[ "data" ][ "cardnumber"    ]   = csn.c_str( );
                    response[ "data" ][ "remvalue"      ]   = getRemainingValue( );
                    response[ "data" ][ "reason"        ]   = pDataOut->ReturnedData.rejectReason;
                    response[ "data" ][ "string_reason" ]   = getRejectReasonString( pDataOut->ReturnedData.rejectReason );
                    response[ "errorcode"               ]   = TPURSELOAD_REVERSAL_ERROR;
                    response[ "string_error"            ]   = getResultString( WS_ERROR );
                    switch ( pDataOut->ReturnedData.rejectReason )
                    {
                    case    MYKI_BR_REJECT_REASON_CARD_BLOCKED:
                    case    MYKI_BR_REJECT_REASON_APPLICATION_BLOCKED:
                        response[ "data" ][ "string_blocking_reason" ]  = \
                                                              getBlockingReasonCode( pDataOut->ReturnedData.blockingReason );
                        break;

                    case    MYKI_BR_REJECT_REASON_EXPIRED:
                        response[ "data" ][ "expiry"    ]   = getExpiry( pDataOut );
                        break;

                    default:
                        /*  Others! DONOTHING! */
                        break;
                    }
                }
                else
                {
                    /*  Failed read/write card */
                    response[ "data" ][ "cardnumber"    ]   = csn.c_str( );
                    response[ "errorcode"               ]   = TPURSELOAD_REVERSAL_ERROR_OPEN_CARD;
                    response[ "string_error"            ]   = getResultString( WS_ERROR_READ_CARD );
                }
            }
            else
            {
                CsDbg( APP_DEBUG_STATE, "processTPurseLoadReversal : wrong card (%s/%s)",
                        cardNumber.c_str(), csn.c_str() );
                response[ "data" ][ "cardnumber"        ]   = cardNumber;
                response[ "errorcode"                   ]   = TPURSELOAD_REVERSAL_ERROR;
                response[ "string_error"                ]   = getResultString( WS_ERROR_WRONG_CARD );
            }
        }
        else
        {
            /*  Not Transit card!? */
            CsErrx( "processTPurseLoadReversal : none TRANSIT card detected" );
            response[ "data" ][ "cardnumber"    ]   = cardNumber;
            response[ "errorcode"               ]   = TPURSELOAD_REVERSAL_NOT_TRANSIT_CARD;
            response[ "string_error"            ]   = getResultString( WS_ERROR_NOT_TRANSIT_CARD );
        }

        #if     OPT_NOT_OPTIMISED
        /*  Done with this card session */
        (void)MYKI_CS_CloseCard( );

        /*  And back waiting for card to be removed */
        MYKI_CS_DetectCard( TRUE );
        #endif
    }
    #if     OPT_NOT_OPTIMISED
    else
    if ( oneCardDetected > 1 )
    {
        /*  Multiple cards detected */
        response[ "data" ][ "string_reason" ]   = getRejectReasonString( MYKI_BR_REJECT_REASON_MULTIPLE_CARDS );
        response[ "errorcode"       ]           = TPURSELOAD_REVERSAL_ERROR;
        response[ "string_error"    ]           = getResultString( WS_ERROR );
    }
    else
    {
        /*  Failed (re)opening TRANSIT card */
        CsErrx( "processTPurseLoadReversal : failed (re)opening TRANSIT card" );
        response[ "errorcode"           ]   = TPURSELOAD_REVERSAL_ERROR_OPEN_CARD;
        response[ "string_error"        ]   = getResultString( WS_ERROR_OPEN_CARD );
    }
    #endif

    return fw.write( response );
}

    /**
     *  @brief  Processes Transit card touch-on/touch-off.
     *  @param  request Json::Value object containing JSON request message
     *          of the following format,
     *  @code
     *  {
     *      "terminalid":"1234",
     *      "name":"validatetransitcard",
     *  }
     *  @endcode
     *  where,
     *  @li     @c terminalid is the terminal id of the device that
     *          initiated the transaction, and
     *  @li     @c name is "validatetransitcard".
     *  @return The string object representing JSON response message of
     *          the following format,
     *  @code
     *  {
     *      "terminalid":"1234",
     *      "name":"validatetransitcard",
     *      "errorcode":0,
     *      "string_error":"SUCCESS",
     *      "data":
     *      {
     *          "cardnumber":"0123456789ABCDEF",
     *          "remvalue":1500,
     *          "istouchon":1,
     *          "istopupreversal":0,
     *          "isactionlisted":0,
     *          "ischangeofmind":1,
     *          "ismoney":1,
     *          "islowbalance":0,
     *          "isconcession":0,
     *          "isforcedscanoff":0,
     *          "fare":375,
     *          "expiry":"24 Dec 15 12:00 pm"
     *          "reason":0,
     *          "string_reason":"REASON_NOT_APPLICABLE"
     *          "string_blocking_reason":"BLOCKING_REASON_LOST_STOLEN"
     *      }
     *  }
     *  @endcode
     *  where,
     *  @li     @c terminalid is the terminal id of the device that
     *          initiated the transaction,
     *  @li     @c name is "validatetransitcard",
     *  @li     @c errorcode is 0 if successful, otherwise failed (DEPRECATED -
     *          use string_error).
     *  @li     @c string_error is text representation of TPurse load
     *          reversal result. Refer to @ref SectionResults for more details.
     *  @li     @c data.cardnumber is the card serial number,
     *  @li     @c data.remvalue is the TPurse remaining value after
     *          transaction,
     *  @li     @c data.istouchon is 1 if touch-on; 0 if touch-off,
     *  @li     @c data.istopupreversal is 1 if TPurseLoad/Debit performed; otherwise 0,
     *  @li     @c data.isactionlisted is 1 if actionlist performed; otherwise 0,
     *  @li     @c data.ischangeofmind is 1 if change-of-mind; otherwise 0,
     *  @li     @c data.ismoney is 1 if TPurse deducted; otherwise 0, and
     *  @li     @c data.islowbalance is 1 if TPurse balance is below threshold; otherwise 0.
     *  @li     @c data.isconcession is 1 if concession fare; otherwise 0.
     *  @li     @c data.isforcedscanoff is 1 if forced scan-off performed; otherwise 0.
     *  @li     @c data.fare is the deducted value.
     *  @li     @c data.expiry is card or product expiry.
     *  @li     @c data.reason is the accepted/rejected reason code (refer to
     *          myki_br_context_data.h for more details - DEPRECATED, use data.string_reason).
     *  @li     @c data.string_reason is the text representation of the
     *          accepted/rejected reason. Refer to @ref SectionReasons for
     *          more details.
     */
std::string
processValidateTransitCard( Json::Value &request )
{
    #define TRANSIT_VALIDATE_SUCCESS            0
    #define TRANSIT_VALIDATE_ERROR              1   /*  Error - Catch all */
    #define TRANSIT_VALIDATE_NOT_TRANSIT_CARD   2   /*  Error - Not a Transit card */
    #define TRANSIT_VALIDATE_ERROR_OPEN_CARD    3
    #define NO_DISCOUNT                         0

    Json::Value             response;
    Json::FastWriter        fw;
//  MYKI_BR_ContextData_t  *pDataIn         = GetCardProcessingThreadContextData();
    MYKI_BR_ContextData_t  *pDataOut        = GetBrContextDataOut();
    std::string             pin             = "";
    bool                    isOperatorCard  = false;
    int                     passengerCode   = 0;
    #if     OPT_NOT_OPTIMISED
    int                     oneCardDetected = 1;
    #endif

    CsDbg( APP_DEBUG_FLOW, "processValidateTransitCard" );

    createBaseResponse( request, response );

    /*  Start new card session */
    #if     OPT_NOT_OPTIMISED
    if ( preValidateCard( isOperatorCard, oneCardDetected ) == true )
    #else
    {
        isOperatorCard  = pDataOut->DynamicData.isOperator == FALSE ? false : true;
    }
    #endif
    {
        std::string         csn             = getMykiPAN( );

        if ( isOperatorCard == false )
        {
            /*  Execute business sequence */
            if ( ExecuteBusinessSequence( 1 /*ONE_CARD*/, isOperatorCard, passengerCode, SEQ_FUNCTION_DEFAULT, true ) == true )
            {
                MYKI_CD_PassengerType_t MYKI_CD_PassengerType   = { 0 };
                bool                    isTouchOn               = pDataOut->ReturnedData.sequenceResult == SEQ_RESULT_SCAN_ON            ? true : false;
                bool                    isTopupReversal         = pDataOut->ReturnedData.topupAmount < 0                                 ? true : false;
                bool                    isActionlisted          = pDataOut->ReturnedData.actionApplied                                   ? true : false;
                bool                    isChangeOfMind          = pDataOut->ReturnedData.acceptReason == MYKI_BR_ACCEPT_CHANGE_OF_MIND   ? true : false;
                bool                    isMoney                 = pDataOut->ReturnedData.applicableFare != 0                             ? true : false;
                bool                    isLowBalance            = pDataOut->ReturnedData.remainingBalance <= g_TPurseLowWarningThreshold ? true : false;
                bool                    isConcession            = false;
                bool                    isForcedScanOff         = pDataOut->DynamicData.isForcedScanOff != 0                             ? true : false;

                if ( MYKI_CD_getPassengerTypeStructure( pDataOut->ReturnedData.passengerId, &MYKI_CD_PassengerType ) != FALSE &&
                     MYKI_CD_PassengerType.percent != NO_DISCOUNT )
                {
                    /*  ASSUMED! Concession fare */
                    isConcession                            = true;
                }

                response[ "data" ][ "cardnumber"        ]   = csn.c_str( );
                response[ "data" ][ "remvalue"          ]   = getRemainingValue( );
                response[ "data" ][ "istouchon"         ]   = isTouchOn       ? 1 : 0;
                response[ "data" ][ "istopupreversal"   ]   = isTopupReversal ? 1 : 0;
                response[ "data" ][ "isactionlisted"    ]   = isActionlisted  ? 1 : 0;
                response[ "data" ][ "ischangeofmind"    ]   = isChangeOfMind  ? 1 : 0;
                response[ "data" ][ "ismoney"           ]   = isMoney         ? 1 : 0;
                response[ "data" ][ "islowbalance"      ]   = isLowBalance    ? 1 : 0;
                response[ "data" ][ "isconcession"      ]   = isConcession    ? 1 : 0;
                response[ "data" ][ "fare"              ]   = pDataOut->ReturnedData.applicableFare;
                response[ "data" ][ "isforcedscanoff"   ]   = isForcedScanOff ? 1 : 0;
                response[ "data" ][ "expiry"            ]   = getExpiry( pDataOut );
                response[ "data" ][ "reason"            ]   = pDataOut->ReturnedData.acceptReason;
                response[ "data" ][ "string_reason"     ]   = getAcceptReasonString( pDataOut->ReturnedData.acceptReason );
                response[ "errorcode"                   ]   = TRANSIT_VALIDATE_SUCCESS;
                response[ "string_error"                ]   = getResultString( WS_SUCCESS );
            }
            else
            if ( pDataOut->ReturnedData.sequenceResult == SEQ_RESULT_REJECT )
            {
                bool                    isTopupReversal         = pDataOut->ReturnedData.topupAmount < 0                                 ? true : false;
                bool                    isActionlisted          = pDataOut->ReturnedData.actionApplied                                   ? true : false;

                /*  Transaction rejected by business rule */
                response[ "data" ][ "cardnumber"        ]   = csn.c_str( );
                response[ "data" ][ "remvalue"          ]   = getRemainingValue( );
                response[ "data" ][ "istopupreversal"   ]   = isTopupReversal ? 1 : 0;
                response[ "data" ][ "isactionlisted"    ]   = isActionlisted  ? 1 : 0;
                response[ "data" ][ "expiry"            ]   = getExpiry( pDataOut );
                response[ "data" ][ "reason"            ]   = pDataOut->ReturnedData.rejectReason;
                response[ "data" ][ "string_reason"     ]   = getRejectReasonString( pDataOut->ReturnedData.rejectReason );
                response[ "errorcode"                   ]   = TRANSIT_VALIDATE_ERROR;
                response[ "string_error"                ]   = getResultString( WS_ERROR );
                switch ( pDataOut->ReturnedData.rejectReason )
                {
                case    MYKI_BR_REJECT_REASON_CARD_BLOCKED:
                case    MYKI_BR_REJECT_REASON_APPLICATION_BLOCKED:
                    response[ "data" ][ "string_blocking_reason" ]  = \
                                                              getBlockingReasonCode( pDataOut->ReturnedData.blockingReason );
                    break;

                case    MYKI_BR_REJECT_REASON_EXPIRED:
                    response[ "data" ][ "expiry"        ]   = getExpiry( pDataOut );
                    break;

                default:
                    /*  Others! DONOTHING! */
                    break;
                }
            }
            else
            {
                /*  Failed reading/writing card */
                response[ "data" ][ "cardnumber"        ]   = csn.c_str( );
                response[ "errorcode"                   ]   = TRANSIT_VALIDATE_ERROR_OPEN_CARD;
                response[ "string_error"                ]   = getResultString( WS_ERROR_READ_CARD );
            }
        }
        else
        {
            /*  Not Transit card!? */
            CsErrx( "processValidateTransitCard : none TRANSIT card detected" );
            response[ "errorcode"       ]   = TRANSIT_VALIDATE_NOT_TRANSIT_CARD;
            response[ "string_error"    ]   = getResultString( WS_ERROR_NOT_TRANSIT_CARD );
        }

        #if     OPT_NOT_OPTIMISED
        /*  Done with this card session */
        (void)MYKI_CS_CloseCard( );

        /*  And back waiting for card to be removed */
        MYKI_CS_DetectCard( TRUE );
        #endif
    }
    #if     OPT_NOT_OPTIMISED
    else
    if ( oneCardDetected > 1 )
    {
        /*  Multiple cards detected */
        response[ "data" ][ "string_reason" ]   = getRejectReasonString( MYKI_BR_REJECT_REASON_MULTIPLE_CARDS );
        response[ "errorcode"       ]           = TRANSIT_VALIDATE_ERROR;
        response[ "string_error"    ]           = getResultString( WS_ERROR );
    }
    else
    {
        /*  Failed (re)opening TRANSIT card */
        CsErrx( "processValidateTransitCard : failed (re)opening TRANSIT card" );
        response[ "errorcode"       ]   = TRANSIT_VALIDATE_ERROR_OPEN_CARD;
        response[ "string_error"    ]   = getResultString( WS_ERROR_OPEN_CARD );
    }
    #endif

    return fw.write( response );
}

    /**
     *  @param  request Json::Value object containing JSON request message
     *          of the following format,
     *  @code
     *  {
     *      "terminalid":"1234",
     *      "name":"tpurseload",
     *      ...
     *  }
     *  where,
     *  @li     @c terminalid is the terminal id of the device that
     *          initiated the transaction, and
     *  @li     @c name is the request name.
     *  @return The string object representing JSON response message of
     *          the following format,
     *  @code
     *  {
     *      "terminalid":"1234",
     *      "name":"tpurseload",
     *      "string_error":"WS_ERROR_READ_CARD",
     *      ...
     *  }
     *  @endcode
     *  where,
     *  @li     @c string_error is "WS_ERROR_READ_CARD".
     */
std::string
processCardRemoved( Json::Value &request )
{
    Json::Value             response;
    Json::FastWriter        fw;

    CsDbg( APP_DEBUG_FLOW, "processCardRemoved" );

    createBaseResponse( request, response );

    response[ "string_error"    ]   = getResultString( WS_ERROR_READ_CARD );

    return  fw.write( response );
}

    /**
     *  @brief  Processes "full" and "concession" passenger count.
     *  @param  request Json::Value object containing JSON request message
     *          of the following format,
     *  @code
     *  {
     *      "terminalid":"1234",
     *      "name":"countpassenger",
     *      "data":
     *      {
     *          "count":"full"
     *      }
     *  }
     *  @endcode
     *  where,
     *  @li     @c terminalid is the terminal id of the device that
     *          initiated the transaction,
     *  @li     @c name is "countpassenger", and
     *  @li     @c data.count is "full" or "concession".
     *  @return The string object representing JSON response message of
     *          the following format,
     *  @code
     *  {
     *      "terminalid":"1234",
     *      "name":"countpassenger",
     *      "string_error":"SUCCESS",
     *      "data":
     *      {
     *          "count":"full"
     *      }
     *  }
     *  @endcode
     *  where,
     *  @li     @c string_error is text representation of passenger count
     *          result. Refer to @ref SectionResults for more details.
     *  @li     @c data.count is "full" or "concession".
     */
std::string
processPassengerCount( Json::Value &request )
{
    Json::Value             response;
    Json::FastWriter        fw;
    std::string             count           = getJsonString( request[ "data" ], "count" );
    bool                    isFull          = count == "full"       ? true : false;
    bool                    isConcession    = count == "concession" ? true : false;
    MYKI_BR_ContextData_t  *pDataIn         = GetCardProcessingThreadContextData();
    MYKI_BR_ContextData_t  *pDataOut        = GetBrContextDataOut();

    CsDbg( APP_DEBUG_FLOW, "processPassengerCount" );

    createBaseResponse( request, response );

    if ( pDataIn != NULL && pDataOut != NULL )
    {
        if ( isFull != false || isConcession != false )
        {
            memcpy( pDataOut, pDataIn, sizeof( MYKI_BR_ContextData_t ) );
            if ( pDataOut->DynamicData.currentDateTime == 0 )
            {
                CsTime_t    now;

                CsTime( &now );
                pDataOut->DynamicData.currentDateTime                   = now.tm_secs;
                pDataOut->InternalData.TransactionData.currentDateTime  = now.tm_secs;
            }

            if ( CreatePassengerCountTmi( pDataOut, isConcession ) == 0 )
            {
                response[ "string_error"    ]   = getResultString( WS_SUCCESS );
            }
            else
            {
                response[ "string_error"    ]   = getResultString( WS_ERROR );
            }
        }
        else
        {
            CsErrx( "processPassengerCount : invalid passenger count type '%s'", count.c_str() );
            response[ "string_error"        ]   = getResultString( WS_ERROR );
        }
    }
    else
    {
        CsErrx( "processPassengerCount : NULL BR context data" );
        response[ "string_error"            ]   = getResultString( WS_ERROR );
    }

    return fw.write( response );
}

    /**
     *  Retrieves integer value from JSON value.
     *  @param  request JSON values.
     *  @param  pName JSON field name.
     *  @return Integer value of the specified JSON field.
     */
int
getJsonInt( Json::Value &request, const char *pName )
{
    int             iValue  = 0;

    if ( pName != NULL && request.empty() == false && request[ pName ].empty() == false )
    {
        if ( request[ pName ].isInt() )
            iValue  = request[ pName ].asInt();
        else
        if ( request[ pName ].isString() )
        {
            std::string sValue  = getJsonString( request, pName );
            iValue              = atoi( sValue.c_str() );
        }
    }
    return  iValue;
}

    /**
     *  Retrieves string value from JSON value.
     *  @param  request JSON values.
     *  @param  pName JSON field name.
     *  @return String value of the specified JSON field.
     */
std::string
getJsonString( Json::Value &request, const char *pName )
{
    std::string         sValue;

    if ( pName != NULL && request.empty() == false && request[ pName ].empty() == false )
    {
        if ( request[ pName ].isString() )
        {
            sValue   = request[ pName ].asString();
        }
        else
        if ( request[ pName ].isInt() )
        {
            int     iValue  = request[ pName ].asInt();
            char    buf[ 32 ];

            CsSnprintf( buf, sizeof( buf ), "%d", iValue );
            sValue  = buf;
        }
    }
    return  sValue;
}

    /**
     *  Retrieves boolean value from JSON value.
     *  @param  value JSON values.
     *  @param  pName JSON field name.
     *  @return Boolean value of the specified JSON field.
     */
bool
getJsonBool( Json::Value &value, const char *pName )
{
    bool                bValue  = false;

    if ( pName != NULL && value.empty() == false && value[ pName ].empty() == false )
    {
        if ( value[ pName ].isBool() )
        {
            bValue  = value[ pName ].asBool();
        }
        else
        if ( value[ pName ].isInt() )
        {
            bValue  = getJsonInt( value, pName ) != 0 ? true : false;
        }
    }
    return  bValue;
}

    /**
     *  Initialises JSON response message.
     *  @param  request JSON request message object.
     *  @param  response JSON response message object.
     */
void
createBaseResponse( Json::Value &request, Json::Value &response )
{
    CsTime_t    t;
    char        timeBuf[ 64 ]   = { 0 };

    CsTime( &t );
    CsStrTimeISO( &t, sizeof( timeBuf ) - 1, timeBuf );

    response[ "terminalid"  ]   = request[ "terminalid" ];
    response[ "userid"      ]   = request[ "userid"     ];
    response[ "name"        ]   = request[ "name"       ];
    response[ "timestamp"   ]   = timeBuf;
    response[ "type"        ]   = request[ "type"       ];
    response[ "data"        ]   = Json::Value();
}

    /**
     *  Determines product expiry date.
     *  @param  pData BR context data.
     *  @param  Product expiry date or TIME_NOT_SET;
     */
static
Time_t
getProductExpiry( MYKI_BR_ContextData_t *pData )
{
    Time_t                  productExpiry   = TIME_NOT_SET;

    if ( g_calculatePassExpiry )
    {
        MYKI_TAControl_t   *pMYKI_TAControl = NULL;
        U8_t                dirIndex        = 0;
        int                 nResult         = 0;

        CsVerbose( "Calculate Pass Expiry" );

        if ( ( nResult = MYKI_CS_TAControlGet( &pMYKI_TAControl ) ) < 0 )
        {
            CsErrx( "getProductExpiry : MYKI_CS_TAControlGet() failed (%d)", nResult );
            return  0;
        }

        if ( pMYKI_TAControl == NULL )
        {
            CsErrx( "getProductExpiry : MYKI_CS_TAControlGet() returned NULL" );
            return  0;
        }

        for ( dirIndex = 1; dirIndex < DIMOF( pMYKI_TAControl->Directory ); dirIndex++ )
        {
            MYKI_Directory_t   *pDirectory      = &pMYKI_TAControl->Directory[ dirIndex ];
            MYKI_TAProduct_t   *pMYKI_TAProduct = NULL;

            if
            (
                (
                    pDirectory != NULL &&
                    (
                        pDirectory->Status == TAPP_CONTROL_DIRECTORY_STATUS_ACTIVATED ||
                        pDirectory->Status == TAPP_CONTROL_DIRECTORY_STATUS_ISSUED
                    )
                ) &&
                ( myki_br_cd_GetProductType( pDirectory->ProductId ) == PRODUCT_TYPE_EPASS )
            )
            {
                if ( pDirectory->Status == TAPP_CONTROL_DIRECTORY_STATUS_ACTIVATED )
                {
                    if ( ( nResult = MYKI_CS_TAProductGet( (U8_t)( dirIndex - 1 ), &pMYKI_TAProduct ) ) < 0 )
                    {
                        CsErrx( "getProductExpiry : MYKI_CS_TAProductGet(%d) failed (%d)", ( dirIndex - 1 ), nResult );
                    }
                    else
                    if ( pMYKI_TAProduct == NULL )
                    {
                        CsErrx( "getProductExpiry : MYKI_CS_TAProductGet(%d) returned NULL", ( dirIndex - 1 ) );
                    }
                    else
                    {
                        CsDbg( APP_DEBUG_STATE, "getProductExpiry : got active pass @ directory index %d with expiry %u", dirIndex, pMYKI_TAProduct->EndDateTime );
                        if
                        (
                            /*  Product not expired */
                            ( pMYKI_TAProduct->EndDateTime > pData->DynamicData.currentDateTime ) &&
                            /*  Product expiry is earlier */
                            (
                                productExpiry == TIME_NOT_SET ||
                                productExpiry > pMYKI_TAProduct->EndDateTime
                            )
                        )
                        {
                            CsDbg( APP_DEBUG_STATE, "getProductExpiry : product expiry set to %d", pMYKI_TAProduct->EndDateTime );
                            productExpiry   = pMYKI_TAProduct->EndDateTime;
                        }
                    }
                }
            }
        }
    }
    else
    {
        MYKI_CD_Product_t   MYKI_CD_Product = { 0 };

        if
        (
            pData->ReturnedData.productId != (-1) &&
            MYKI_CD_getProductStructure( pData->ReturnedData.productId, &MYKI_CD_Product )
        )
        {
            if ( strcmp( MYKI_CD_Product.type, "LLSC" ) == 0 && pData->ReturnedData.isProvisional == 0 )
            {
                /*  Not a provisional product */
                productExpiry   = pData->ReturnedData.expiryDateTime;
            }
        }
    }
    return  productExpiry;
}

    /**
     *  Returns expiry date string resulted from running business rule.
     *  @param  pData BR context data.
     *  @return The string object representing card/product expiry date.
     */
std::string
getExpiry( MYKI_BR_ContextData_t *pData )
{
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

    int             year                = 0,
                    month               = 0,
                    day                 = 0,
                    hour                = 0,
                    minute              = 0;
    char            expiryString[ 128 ] = { '\0' };

    if ( pData->ReturnedData.rejectReason == MYKI_BR_REJECT_REASON_EXPIRED )
    {
        CsDbg( APP_DEBUG_STATE, "getExpiry : card expired date time expiryDateTime %d",
                pData->ReturnedData.cardExpiryDate );

        //  Add 1 to the expiry date as the expiry is always at midnight or later (assumption: no negative endOfBusinessDay)
        DateC19ToDateYMDHHMM( pData->ReturnedData.cardExpiryDate + 1, &year, &month, &day, &hour, &minute );

        //  Set the hour and minute to the end of business
        hour    = pData->Tariff.endOfBusinessDay / 60;
        minute  = pData->Tariff.endOfBusinessDay % 60;
    }
    else
    {
        Time_t      productExpiry       = getProductExpiry( pData );

        if ( productExpiry != TIME_NOT_SET )
        {
            struct tm   ctm             = { 0 };

            localtime_r( (time_t *)&productExpiry, &ctm );
            year    = ctm.tm_year + 1900;
            month   = ctm.tm_mon  + 1;
            day     = ctm.tm_mday;
            hour    = ctm.tm_hour;
            minute  = ctm.tm_min;
        }
    }

    if ( year > 2000 )
    {
        CsSnprintf( expiryString, sizeof( expiryString ) - 1, "%02d %s %d %02d:%02d %s",
                day, months[ month - 1 ], year - 2000,
                hour <= 12 ? hour : hour % 12, minute, hour >= 12 ? "pm" : "am" );
    }

    return  expiryString;
}

    /**
     *  Returns text representation of the specified web service
     *  result code.
     *  @param  WebServiceResult web service result code.
     *  @return Text representation of the web service result code.
     */
std::string
getResultString( WebServiceResult_e WebServiceResult )
{
    #define WEBSERVICE_RESULT_DECLARATION( a )  #a
    static  const char     *errorStrings[ ] =
    {
        WEBSERVICE_RESULTS,
        NULL
    };
    #undef  WEBSERVICE_RESULT_DECLARATION

    if ( WebServiceResult >= WEBSERVICE_RESULT_MAX )
    {
        CsErrx( "getResultString : web service result code (%d) out-of-bound", (int)WebServiceResult );
        WebServiceResult    = WS_ERROR;
    }
    return  errorStrings[ WebServiceResult ];
}

    /**
     *  Returns text representation of the card transaction reject reason.
     *  @param  reason card transaction reject reason code.
     *  @return text representation of the card transaction reject reason.
     */
std::string
getRejectReasonString( MYKI_BR_RejectReason_t reason )
{
    #define REASON_DECLARATION( a ) #a
    static  const char     *rejectReasonStrings[ ] =
    {
        MYKI_BR_REJECT_REASONS,
        NULL
    };
    #undef  REASON_DECLARATION

    if ( reason >= MYKI_BR_REJECT_REASON_MAX )
    {
        CsErrx( "getRejectReasonString : reject reason (%d) out-of-bound", (int)reason );
        reason  = MYKI_BR_REJECT_REASON_NOT_APPLICABLE;
    }

    /*  Returns reason code string without preceding "MYKI_BR_REJECT_" */
    return  &rejectReasonStrings[ reason ][ 15 /* STRLEN( "MYKI_BR_REJECT_" ) */ ];
}

    /**
     *  Returns text representation of the card transaction accept reason.
     *  @param  reason card transaction accept reason code.
     *  @return text representation of the card transaction accept reason.
     */
std::string
getAcceptReasonString( MYKI_BR_AcceptReason_t reason )
{
    #define REASON_DECLARATION( a ) #a
    static  const char     *acceptReasonStrings[ ] =
    {
        MYKI_BR_ACCEPT_REASONS,
        NULL
    };
    #undef  REASON_DECLARATION

    if ( reason >= MYKI_BR_ACCEPT_REASON_MAX )
    {
        CsErrx( "getAcceptReasonString : accept reason (%d) out-of-bound", (int)reason );
        reason  = MYKI_BR_ACCEPT_REASON_NOT_APPLICABLE;
    }

    /*  Returns reason code string without preceding "MYKI_BR_ACCEPT_" */
    return  &acceptReasonStrings[ reason ][ 15 /* STRLEN( "MYKI_BR_ACCEPT_" ) */ ];
}

    /**
     *  Retrieves TPurse balance.
     *  @return TPurse balance.
     */
int
getRemainingValue( void )
{
    MYKI_TAPurseBalance_t  *pMYKI_TAPurseBalance    = NULL;
    int                     result                  = 0;
    
    if ( ( result = MYKI_CS_TAPurseBalanceGet( &pMYKI_TAPurseBalance ) ) < 0 )
    {
        CsErrx( "getRemainingValue : MYKI_CS_TAPurseBalanceGet() failed (%d)", result );
        return  0;
    }
    return  pMYKI_TAPurseBalance->Balance;
}