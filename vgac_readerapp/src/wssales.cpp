/* -*- mode: c; tabs: 4; -*- */
/*=============================================================================
**
**    Vix Technology                   Licensed software
**    (C) 2015                         All rights reserved
**
**=============================================================================
**
**  Project/Product : NGBU
**  Filename        : wssales.cpp
**  Author(s)       : An Tran
**
**  Description     :
*/
/**     @file
**      @brief  Implements Web Services to perform product sales transactions.
*/
/*  Member(s)       :
**      processProductSales     [public]    processes product sales
**      processProductSalesReversal
**                              [public]    processes product sales reversal
**
**  Information     :
**   Compiler(s)    : C++
**   Target(s)      : Independent
**
**  Subversion      :
**      $Id: wssales.cpp 88945 2016-01-12 03:56:05Z atran $
**      $HeadURL: https://auperasvn01.aupera.erggroup.com/svn/DPG_SWBase/vgac_readerapp/trunk/src/wssales.cpp $
**
**  History         :
**   Vers.  Date        Aut.  Type     Description
**   -----  ----------  ----  -------  ----------------------------------------
**    1.00  10.12.15    ANT   Create
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
#include "wssales.h"

/*
 *      Options
 *      -------
 */

#define OPT_ENABLE_ROUNDING             TRUE    /*  TRUE if enable currency rounding */
#define OPT_GST_ROUNDING_PER_PRODUCT    TRUE    /*  TRUE if GST rounding per product sold */

/*
 *      External References
 *      -------------------
 */

    /*  GAC_PROCESSING_THREAD.CPP - TODO: Move to header file */
extern  "C" MYKI_BR_ContextData_t  *GetBrContextDataOut( void );
extern  bool                        preValidateCard( bool &operatorCard, int nCardsDetected );
extern  bool                        ExecuteBusinessSequence( int numberOfCardsPresent, bool operatorCard, int &passengerCode, SequenceFunction_e businessSequence, bool forceProcessBR );

extern  Currency_t                  g_initCurrencyRoundingPortion;

/*
 *      Local Constants and Macros
 *      --------------------------
 */

#define _1CENT                      (100)

/*
 *      Local Prototypes
 *      ----------------
 */

static  Currency_t                  CurrencyRounding( Currency_t amount, Currency_t roundingPortion );
static  Currency_t                  CalculateGST( Currency_t amountPaid, int vatRate );

/*
 *      Global Variables
 *      ----------------
 */

    /**
     *  @brief  Processes product sales web service request.
     *  This function shall generate product sales TMI record.
     *  If product sales transaction is paid by TPurse, this function
     *  shall execute business rule to deduct the transaction amount
     *  from the presented patron card. If "TPurse Load" is listed in
     *  the shopping cart, this function shall execute business rule
     *  to add value to the presented patron card.
     *  @param  request Json::Value object containing JSON request message
     *          of the following format,
     *  @code
     *  {
     *      "terminalid":"1234",
     *      "userid":"D101",
     *      "name":"productsales",
     *      "data":
     *      {
     *          "paymenttype":"cash",
     *          "cardnumber":"0123456789ABCDEF",
     *          "cartitems":
     *          [
     *              {
     *                  "quantity":2,
     *                  "GSTApplicable":false,
     *                  "PLU":"4. myki Child",
     *                  "Price":50,
     *                  "id":46,
     *                  "issuer_id":1,
     *                  "long_desc":"GTS Timetable",
     *                  "short_desc":"GTS Timetable",
     *                  "subtype":"None",
     *                  "type":"ThirdParty"
     *              },
     *              {
     *                  "quantity":1,
     *                  "GSTApplicable":false,
     *                  "Price":5000,
     *                  "id":300,
     *                  "issuer_id":1,
     *                  "long_desc":"TPurse Load",
     *                  "short_desc":"TPurse Load",
     *                  "subtype":"None",
     *                  "type":"LLSC"
     *              },
     *              { ... }
     *          ]
     *      }
     *  }
     *  @endcode
     *  where,
     *  @li     @c terminalid is the terminal id of the device that
     *          initiated the transaction,
     *  @li     @c name is "productsales",
     *  @li     @c data.cardnumber is the card serial number used as
     *          payment type or used for TPurse load,
     *  @li     @c data.cartitems is an array of items in shopping cart,
     *  @li     @c data.cartitems[n].quantity is the quantity purchasing,
     *  @li     @c data.cartitems[n] is the product details (as
     *          returned from MYKI_CD_getProducts API).\n
     *          For TPurse Load, product id is 300, product type
     *          is "LLSC" and product price is the TPurse Load amount.
     *  @return The string object representing JSON response message of
     *          the following format,
     *  @code
     *  {
     *    "terminalid":"1234",
     *    "userid":"D101",
     *    "name":"productsale",
     *    "string_error":"WS_SUCCESS",
     *    "string_blocking_reason":"BLOCKING_REASON_LOST_STOLEN",
     *    "expiry":"24 Dec 15 12:00 pm",
     *    "timestamp":"2015-12-16T10:44:55+10:00",
     *    "data":
     *    {
     *      "paymenttype":"cash",
     *      "paymentmedia":
     *      {
     *        "cardnumber":"0123456789ABCDEF",
     *        "remvalue":5200,
     *        "txnseqno":1234
     *      },
     *      "cardnumber":"0123456789ABCDEF",    /- DEPRECATED -/
     *      "remvalue":5200,                    /- DEPRECATED -/
     *      "txnseqno":1234,                    /- DEPRECATED -/
     *      "saleseqno":1212,
     *      "amounttotal":7812,
     *      "amountgst":710,
     *      "amountrounding":-2,
     *      "amountpaid":7810,
     *      "vat_rate":10,
     *      "string_reason":"REASON_NOT_APPLICABLE",
     *      "route_id":1234,
     *      "stop_id":11,
     *      "cartitems":
     *      [
     *        {
     *          "itemnumber":1,
     *          "GSTApplicable":false,
     *          "PLU":"4. myki Child",
     *          "Price":50,
     *          "id":46,
     *          "issuer_id":1,
     *          "long_desc":"GTS Timetable",
     *          "short_desc":"GTS Timetable",
     *          "subtype":"None",
     *          "type":"ThirdParty"
     *        },
     *        {
     *          "itemnumber":2,
     *          "GSTApplicable":false,
     *          "PLU":"4. myki Child",
     *          "Price":50,
     *          "id":46,
     *          "issuer_id":1,
     *          "long_desc":"GTS Timetable",
     *          "short_desc":"GTS Timetable",
     *          "subtype":"None",
     *          "type":"ThirdParty"
     *        },
     *        {
     *          "itemnumber":3,
     *          "GSTApplicable":false,
     *          "Price":5000,
     *          "id":300,
     *          "issuer_id":1,
     *          "long_desc":"TPurse Load",
     *          "short_desc":"TPurse Load",
     *          "subtype":"None",
     *          "type":"LLSC",
     *          "addvaluemedia":
     *          {
     *            "cardnumber":"0123456789ABCDEF",
     *            "remvalue":5200,
     *            "txnseqno":1234
     *          },
     *        },
     *        {
     *          "itemnumber":4,
     *          "GSTApplicable":true,
     *          "PLU":"Surcharge Me",
     *          "Price":50,
     *          "id":47,
     *          "issuer_id":1,
     *          "long_desc":"Surcharge Me Now",
     *          "short_desc":"Surcharge Me Now",
     *          "subtype":"None",
     *          "type":"PremiumSurcharge"
     *        },
     *        { ... }
     *      ],
     *      "surcharge":
     *      {
     *        "amounttotal":1234,
     *        "amountgst":123,
     *        "amountrounding":-4,
     *        "amountpaid":1230,
     *        "cartitems:
     *        [
     *          {
     *            "itemnumber":4,
     *            "GSTApplicable":true,
     *            "PLU":"Surcharge Me",
     *            "Price":50,
     *            "id":47,
     *            "issuer_id":1,
     *            "long_desc":"Surcharge Me Now",
     *            "short_desc":"Surcharge Me Now",
     *            "subtype":"None",
     *            "type":"PremiumSurcharge"
     *          },
     *          { ... }
     *        ]
     *      }
     *    }
     *  }
     *  @endcode
     *  where,
     *  @li     @c terminalid is the terminal id of the device that
     *          initiated the transaction,
     *  @li     @c name is "productsales",
     *  @li     @c data.cardnumber is the card serial number used as
     *          payment type or used for TPurse load,
     *  @li     @c data.timestamp is the transaction unix time since epoch,
     *  @li     @c data.vat_rate is the percentage GST.
     *  @li     @c data.cartitems is an array of items in shopping cart.
     *  @li     @c data.cartitems[n].product.itemnumber is the item number.
     *  @li     @c data.cartitems[n].product is the product details.
     */
std::string
processProductSales( Json::Value &request )
{
    Json::Value             response;
    Json::FastWriter        fw;
    MYKI_BR_ContextData_t  *pDataIn                 = GetCardProcessingThreadContextData();
    MYKI_BR_ContextData_t  *pDataOut                = GetBrContextDataOut();
    int                     itemNumber              = 0;
    Currency_t              amountTotal             = 0;
    Currency_t              amountGST               = 0;
    Currency_t              amountPaid              = 0;
    Currency_t              amountSurchargeTotal    = 0;
    Currency_t              amountSurchargeGST      = 0;
    Currency_t              amountSurchargePaid     = 0;
    Currency_t              tpurseTxnAmount         = 0;
    int                     itemIndexTPurseLoad    = -1;
    std::string             paymentType             = getJsonString( request[ "data" ], "paymenttype" );
    std::string             cardNumber              = getJsonString( request[ "data" ], "cardnumber"  );
    int                     txnSeqNo                = getJsonInt(    request[ "data" ], "txnseqno"    );
    U8_t                    vatRate                 = 0;
    bool                    canSaleProduct          = false;
    Json::Value             media;

    CsDbg( APP_DEBUG_FLOW, "processProductSales" );

    createBaseResponse( request, response );

    if ( MYKI_CD_getGSTPercentage( &vatRate ) == FALSE )
    {
        CsErrx( "processProductSales : MYKI_CD_getGSTPercentage() failed, set zero vat_rate" );
        vatRate     = 0;
    }

/*  response[ "type" ]                                  = "SET";    */
    response[ "data" ][ "paymenttype" ]                 = request[ "data" ][ "paymenttype" ];
    response[ "data" ][ "route_id"    ]                 = pDataIn->DynamicData.lineId;
    response[ "data" ][ "stop_id"     ]                 = pDataIn->DynamicData.stopId;
    response[ "data" ][ "cartitems"   ]                 = Json::Value( Json::arrayValue );
    response[ "data" ][ "surcharge"   ][ "cartitems" ]  = Json::Value( Json::arrayValue );

    if ( paymentType == "tpurse" )
    {
        if ( request[ "data" ][ "paymentmedia" ].empty() == false )
        {
            media                                       = request[ "data" ][ "paymentmedia" ];
            cardNumber                                  = getJsonString( media, "cardnumber" );
            txnSeqNo                                    = getJsonInt(    media, "txnseqno"   );
        }
        else
        {
            media[ "cardnumber" ]                       = cardNumber;
            media[ "txnseqno"   ]                       = txnSeqNo;
        }
    }

    for ( Json::ValueIterator itr = request[ "data" ][ "cartitems" ].begin();
            itr != request[ "data" ][ "cartitems" ].end();
            itr++ )
    {
        Json::Value        &cartItem        = ( *itr );
        int                 id              = getJsonInt(    cartItem, "id"            );
        std::string         type            = getJsonString( cartItem, "type"          );
        int                 quantity        = getJsonInt(    cartItem, "quantity"      );
        int                 issuerId        = getJsonInt(    cartItem, "issuer_id"     );
        Currency_t          price           = getJsonInt(    cartItem, "Price"         );
        bool                gstApplicable   = getJsonBool(   cartItem, "GSTApplicable" );
        Currency_t          priceGST        = CalculateGST( price, vatRate );
        bool                isSurcharge     = type == "PremiumSurcharge" ? true : false;

        if ( id == ProductId_TPurse )
        {
            /*  Only one TPurse Load is allowed */
            quantity                        = 1;
            itemIndexTPurseLoad             = response[ "data" ][ "cartitems" ].size();
            tpurseTxnAmount                 = price;
            gstApplicable                   = false;
            priceGST                        = 0;
            if ( cartItem[ "addvaluemedia" ].empty() == false )
            {
                media                       = cartItem[ "addvaluemedia" ];
                cardNumber                  = getJsonString( media, "cardnumber" );
                txnSeqNo                    = getJsonInt(    media, "txnseqno"   );
            }
            else
            {
                media[ "cardnumber" ]       = cardNumber;
                media[ "txnseqno"   ]       = txnSeqNo;
            }
        }

        for ( int count = 0; count < quantity; count++ )
        {
            Json::Value     productSold;

            itemNumber++;
            productSold[ "itemnumber"    ]  = itemNumber;
            productSold[ "GSTApplicable" ]  = gstApplicable;
            productSold[ "Price"         ]  = price;
            productSold[ "id"            ]  = cartItem[ "id"            ];
            productSold[ "issuer_id"     ]  = issuerId == 0 ? PRODUCT_OWNER_DEFAULT : issuerId;
            productSold[ "long_desc"     ]  = cartItem[ "long_desc"     ];
            productSold[ "short_desc"    ]  = cartItem[ "short_desc"    ];
            productSold[ "subtype"       ]  = cartItem[ "subtype"       ];
            productSold[ "type"          ]  = type;

            response[ "data" ][ "cartitems" ].append( productSold );
            amountTotal                    += price;
            if ( gstApplicable == true )
            {
                amountGST                  += priceGST;
            }

            if ( isSurcharge == true )
            {
                response[ "data" ][ "surcharge" ][ "cartitems" ].append( productSold );
                amountSurchargeTotal       += price;
                if ( gstApplicable == true )
                {
                    amountSurchargeGST     += priceGST;
                }
            }
        }
    }

    #if     OPT_ENABLE_ROUNDING
    if ( paymentType == "cash" )
    {
        /*  Round amount to be paid to nearest rounding portion */
        amountPaid          = CurrencyRounding( amountTotal,          g_initCurrencyRoundingPortion );
        amountSurchargePaid = CurrencyRounding( amountSurchargeTotal, g_initCurrencyRoundingPortion );
    }
    else
    #endif
    {
        amountPaid          = amountTotal;
        amountSurchargePaid = amountSurchargeTotal;
    }
    #if     !OPT_GST_ROUNDING_PER_PRODUCT
    amountGST               = ( CurrencyRounding( amountGST,          _1CENT ) / _1CENT );
    amountSurchargeGST      = ( CurrencyRounding( amountSurchargeGST, _1CENT ) / _1CENT );
    #endif

    if ( itemNumber > MAX_PRODUCT_SALES_ITEMS )
    {
        CsErrx( "processProductSales : too many products (%d)", itemNumber );
        response[ "string_error" ]  = getResultString( WS_ERROR );
    }
    else
    if ( paymentType == "tpurse" || itemIndexTPurseLoad >= 0 )
    {
        /*  Product sales transaction paid by TPurse or TPurse add value as one of the product sales item */
        SequenceFunction_e  brSequence              = itemIndexTPurseLoad >= 0 ?
                                                            SEQ_FUNCTION_ADD_VALUE :
                                                            SEQ_FUNCTION_DEBIT_TPURSE_VALUE;
        bool                isOperatorCard          = pDataOut->DynamicData.isOperator == FALSE ? false : true;
        int                 passengerCode           = 0;

        if ( isOperatorCard == false )
        {
            std::string  csn    = getMykiPAN( );

            if ( brSequence == SEQ_FUNCTION_ADD_VALUE )
            {
                /*  Sets TPurse load business rule parameters */
                pDataIn->DynamicData.tpurseLoadAmount   = tpurseTxnAmount;
            }
            else
            {
                /*  Sets TPurse debit business rule parameters */
                pDataIn->DynamicData.tPurseDebitAmount  = \
                tpurseTxnAmount                         = amountPaid;
            }

            /*  Execute business sequence */
            if ( ExecuteBusinessSequence( 1 /*ONE_CARD*/, isOperatorCard, passengerCode, brSequence, true ) == true )
            {
                canSaleProduct                                          = true;
                response[ "data" ][ "cardnumber"     ]                  = csn.c_str( );
                response[ "data" ][ "remvalue"       ]                  = getRemainingValue( );
                response[ "data" ][ "txnseqno"       ]                  = pDataOut->ReturnedData.txnSeqNo;  /*  Card (TAPurseControl) transaction sequence number */
                media[ "cardnumber"  ]                                  = csn.c_str( );
                media[ "remvalue"    ]                                  = getRemainingValue( );
                media[ "txnseqno"    ]                                  = pDataOut->ReturnedData.txnSeqNo;
                if ( brSequence == SEQ_FUNCTION_ADD_VALUE )
                {
                    response[ "data" ][ "cartitems" ][ itemIndexTPurseLoad ][ "addvaluemedia" ]    = media;
                }
                else
                {
                    response[ "data" ][ "paymentmedia" ]                = media;
                }
                response[ "data" ][ "txnamount"      ]                  = tpurseTxnAmount;
                response[ "data" ][ "string_reason"  ]                  = getAcceptReasonString( pDataOut->ReturnedData.acceptReason );
                response[ "data" ][ "amounttotal"    ]                  = amountTotal;
                response[ "data" ][ "amountgst"      ]                  = amountGST;
                response[ "data" ][ "amountrounding" ]                  = ( amountPaid - amountTotal );
                response[ "data" ][ "amountpaid"     ]                  = amountPaid;
                response[ "data" ][ "surcharge" ][ "amounttotal"    ]   = amountSurchargeTotal;
                response[ "data" ][ "surcharge" ][ "amountgst"      ]   = amountSurchargeGST;
                response[ "data" ][ "surcharge" ][ "amountrounding" ]   = ( amountSurchargePaid - amountSurchargeTotal );
                response[ "data" ][ "surcharge" ][ "amountpaid"     ]   = amountSurchargePaid;
                response[ "string_error"             ]                  = getResultString( WS_SUCCESS );
            }
            else
            if ( pDataOut->ReturnedData.sequenceResult == SEQ_RESULT_REJECT )
            {
                /*  Transaction rejected by business rule */
                response[ "data" ][ "cardnumber"      ] = csn.c_str( );
                response[ "data" ][ "remvalue"        ] = getRemainingValue( );
                response[ "data" ][ "string_reason"   ] = getRejectReasonString( pDataOut->ReturnedData.rejectReason );
                response[ "string_error"              ] = getResultString( WS_ERROR );
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
                response[ "data" ][ "cardnumber" ]      = csn.c_str( );
                response[ "string_error"         ]      = getResultString( WS_ERROR_READ_CARD );
            }

            /*  Done with transaction */
            pDataIn->DynamicData.tpurseLoadAmount       = 0;
/*          pDataIn->DynamicData.tpurseDebitAmount      = 0;    */
        }
        else
        {
            /*  Not Transit card!? */
            CsErrx( "processProductSales : none TRANSIT card detected" );
            response[ "string_error" ]                  = getResultString( WS_ERROR_NOT_TRANSIT_CARD );
        }
    }
    else
    {
        /*  Product sales transaction does not involve updating patron card */
        canSaleProduct                                          = true;
        memcpy( pDataOut, pDataIn, sizeof( MYKI_BR_ContextData_t ) );
        {
            CsTime_t            nowTime                         = { 0 };
            CsTime( &nowTime );
            pDataOut->DynamicData.currentDateTime               = nowTime.tm_secs;
        }
        response[ "data" ][ "string_reason"  ]                  = getAcceptReasonString( MYKI_BR_ACCEPT_REASON_NOT_APPLICABLE );
        response[ "data" ][ "amounttotal"    ]                  = amountTotal;
        response[ "data" ][ "amountgst"      ]                  = amountGST;
        response[ "data" ][ "amountrounding" ]                  = ( amountPaid - amountTotal );
        response[ "data" ][ "amountpaid"     ]                  = amountPaid;
        response[ "data" ][ "surcharge" ][ "amounttotal"    ]   = amountSurchargeTotal;
        response[ "data" ][ "surcharge" ][ "amountgst"      ]   = amountSurchargeGST;
        response[ "data" ][ "surcharge" ][ "amountrounding" ]   = ( amountSurchargePaid - amountSurchargeTotal );
        response[ "data" ][ "surcharge" ][ "amountpaid"     ]   = amountSurchargePaid;
        response[ "string_error"             ]                  = getResultString( WS_SUCCESS );
    }

    if ( canSaleProduct == true )
    {
        if ( CreateProductSalesTmi( pDataOut, response ) == 0 )
        {
            /*  Successfully created product sales TMI record */
            unsigned int     globalSalesTxnNum              = 0;

            /*  Retrieves last generated (sale) transaction record number */
            if ( TMI_getCounter("salesTransactionCount", &globalSalesTxnNum ) < 0 )
            {
                CsErrx( "processProductSales : TMI_getCounter() failed" );
                globalSalesTxnNum                   = 0;
            }

            response[ "data" ][ "saleseqno" ]                   = globalSalesTxnNum;
            response[ "data" ][ "vat_rate"  ]                   = vatRate;
        }
        else
        {
            /*  Failed creating product sales TMI record */
            response[ "data" ][ "string_reason" ]               = getRejectReasonString( MYKI_BR_REJECT_REASON_NOT_APPLICABLE );
            response[ "string_error" ]                          = getResultString( WS_ERROR );

            /*  TODO:   Generate alarm */
        }
    }

    return fw.write( response );
}

    /**
     *  @brief  Processes product sales web service request.
     *  This function shall generate product sales TMI record.
     *  If product sales transaction is paid by TPurse, this function
     *  shall execute business rule to deduct the transaction amount
     *  from the presented patron card. If "TPurse Load" is listed in
     *  the shopping cart, this function shall execute business rule
     *  to add value to the presented patron card.
     *  @param  request Json::Value object containing JSON request message
     *          of the following format,
     *  @code
     *  {
     *      "terminalid":"1234",
     *      "userid":"D101",
     *      "name":"productsalesreversal",
     *      "data":
     *      {
     *          "paymenttype":"cash",
     *          "cardnumber":"0123456789ABCDEF",
     *          "txnseqno":1234,
     *          "saleseqno":1212,
     *          "saletimestamp":"2015-12-16T10:44:55+10:00",
     *          "cartitems":
     *          [
     *              {
     *                  "itemnumber":1,
     *                  "GSTApplicable":false,
     *                  "PLU":"4. myki Child",
     *                  "Price":50,
     *                  "id":46,
     *                  "issuer_id":1,
     *                  "long_desc":"GTS Timetable",
     *                  "short_desc":"GTS Timetable",
     *                  "subtype":"None",
     *                  "type":"ThirdParty"
     *              },
     *              {
     *                  "itemnumber":2,
     *                  "GSTApplicable":false,
     *                  "Price":5000,
     *                  "id":300,
     *                  "issuer_id":1,
     *                  "long_desc":"TPurse Load",
     *                  "short_desc":"TPurse Load",
     *                  "subtype":"None",
     *                  "type":"LLSC"
     *              },
     *              { ... }
     *          ]
     *      }
     *  }
     *  @endcode
     *  where,
     *  @li     @c terminalid is the terminal id of the device that
     *          initiated the transaction,
     *  @li     @c name is "productsales",
     *  @li     @c data.cardnumber is the card serial number used as
     *          payment type or used for TPurse load,
     *  @li     @c data.cartitems is an array of items in shopping cart,
     *  @li     @c data.cartitems[n].quantity is the quantity purchasing,
     *  @li     @c data.cartitems[n] is the product details (as
     *          returned from MYKI_CD_getProducts API).\n
     *          For TPurse Load, product id is 300, product type
     *          is "LLSC" and product price is the TPurse Load amount.
     *  @return The string object representing JSON response message of
     *          the following format,
     *  @code
     *  {
     *      "terminalid":"1234",
     *      "userid":"D101",
     *      "name":"productsalesreversal",
     *      "string_error":"WS_SUCCESS",
     *      "timestamp":"2015-12-16T10:44:55+10:00",
     *      "data":
     *      {
     *          "paymenttype":"cash",
     *          "cardnumber":"0123456789ABCDEF",
     *          "remvalue":5200,
     *          "txnseqno":1234,
     *          "saleseqno":1212,
     *          "saletimestamp":"2015-12-16T10:44:55+10:00",
     *          "revtxnseqno":4321,
     *          "revsaleseqno":2121,
     *          "string_reason":"REASON_NOT_APPLICABLE",
     *          "string_blocking_reason":"BLOCKING_REASON_LOST_STOLEN",
     *          "expiry":"24 Dec 15 12:00 pm",
     *          "amounttotal":7812,
     *          "amountgst":781,
     *          "amountrounding":-2,
     *          "amountrefunded":7810,
     *          "vat_rate":10,
     *          "route_id":1234,
     *          "stop_id":12,
     *          "cartitems":
     *          [
     *              {
     *                  "itemnumber":1,
     *                  "GSTApplicable":false,
     *                  "PLU":"4. myki Child",
     *                  "Price":50,
     *                  "id":46,
     *                  "issuer_id":1,
     *                  "long_desc":"GTS Timetable",
     *                  "short_desc":"GTS Timetable",
     *                  "subtype":"None",
     *                  "type":"ThirdParty"
     *              },
     *              {
     *                  "itemnumber":2,
     *                  "GSTApplicable":false,
     *                  "PLU":"4. myki Child",
     *                  "Price":50,
     *                  "id":46,
     *                  "issuer_id":1,
     *                  "long_desc":"GTS Timetable",
     *                  "short_desc":"GTS Timetable",
     *                  "subtype":"None",
     *                  "type":"ThirdParty"
     *              },
     *              {
     *                  "itemnumber":3,
     *                  "GSTApplicable":false,
     *                  "Price":5000,
     *                  "id":300,
     *                  "issuer_id":1,
     *                  "long_desc":"TPurse Load",
     *                  "short_desc":"TPurse Load",
     *                  "subtype":"None",
     *                  "type":"LLSC"
     *              },
     *              { ... }
     *          ]
     *      }
     *  }
     *  @endcode
     *  where,
     *  @li     @c terminalid is the terminal id of the device that
     *          initiated the transaction,
     *  @li     @c name is "productsales",
     *  @li     @c data.cardnumber is the card serial number used as
     *          payment type or used for TPurse load,
     *  @li     @c data.timestamp is the transaction unix time since epoch,
     *  @li     @c data.vat_rate is the percentage GST.
     *  @li     @c data.cartitems is an array of items in shopping cart.
     *  @li     @c data.cartitems[n].product.itemnumber is the item number.
     *  @li     @c data.cartitems[n].product is the product details.
     */
std::string
processProductSalesReversal( Json::Value &request )
{
    Json::Value             response;
    Json::FastWriter        fw;
    MYKI_BR_ContextData_t  *pDataIn                 = GetCardProcessingThreadContextData();
    MYKI_BR_ContextData_t  *pDataOut                = GetBrContextDataOut();

    Currency_t              amountTotal             = 0;
    Currency_t              amountGST               = 0;
    Currency_t              amountRefunded          = 0;
    Currency_t              amountSurchargeTotal    = 0;
    Currency_t              amountSurchargeGST      = 0;
    Currency_t              amountSurchargeRefunded = 0;
    Currency_t              tpurseTxnAmount         = 0;
    int                     itemIndexTPurseLoad     = -1;
    std::string             paymentType             = getJsonString( request[ "data" ], "paymenttype" );
    std::string             cardNumber              = getJsonString( request[ "data" ], "cardnumber"  );
    int                     txnSeqNo                = getJsonInt(    request[ "data" ], "txnseqno"    );
    int                     routeId                 = getJsonInt(    request[ "data" ], "route_id"    );
    int                     stopId                  = getJsonInt(    request[ "data" ], "stop_id"     );
    U8_t                    vatRate                 = 0;
    bool                    canRefundProduct        = false;
    Json::Value             media;

    CsDbg( APP_DEBUG_FLOW, "processProductSalesReversal" );

    createBaseResponse( request, response );

    if ( MYKI_CD_getGSTPercentage( &vatRate ) == FALSE )
    {
        CsErrx( "processProductSalesReversal : MYKI_CD_getGSTPercentage() failed, set zero vat_rate" );
        vatRate     = 0;
    }

/*  response[ "type" ]                                  = "SET";    */
    response[ "data" ][ "paymenttype" ]                 = request[ "data" ][ "paymenttype" ];
    response[ "data" ][ "route_id"    ]                 = routeId == 0 ? pDataIn->DynamicData.lineId : routeId;
    response[ "data" ][ "stop_id"     ]                 = stopId  == 0 ? pDataIn->DynamicData.stopId : stopId;
    response[ "data" ][ "cartitems"   ]                 = Json::Value( Json::arrayValue );
    #if     __IF_NEEDED__
    response[ "data" ][ "surcharge"   ][ "cartitems" ]  = Json::Value( Json::arrayValue );
    #endif

    if ( paymentType == "tpurse" )
    {
        if ( request[ "data" ][ "paymentmedia" ].empty() == false )
        {
            media                                       = request[ "data" ][ "paymentmedia" ];
            cardNumber                                  = getJsonString( media, "cardnumber" );
            txnSeqNo                                    = getJsonInt(    media, "txnseqno"   );
        }
        else
        {
            /*  DEPRECATED! */
            media[ "cardnumber" ]                       = cardNumber;
            media[ "txnseqno"   ]                       = txnSeqNo;
        }
    }
    response[ "data" ][ "saleseqno"     ]               = getJsonInt(    request[ "data" ], "saleseqno"     );
    response[ "data" ][ "saletimestamp" ]               = getJsonString( request[ "data" ], "saletimestamp" );

    for ( Json::ValueIterator itr = request[ "data" ][ "cartitems" ].begin();
            itr != request[ "data" ][ "cartitems" ].end();
            itr++ )
    {
        Json::Value        &cartItem        = ( *itr );
        int                 itemNumber      = getJsonInt(    cartItem, "itemnumber"    );
        int                 id              = getJsonInt(    cartItem, "id"            );
        std::string         type            = getJsonString( cartItem, "type"          );
        int                 issuerId        = getJsonInt(    cartItem, "issuer_id"     );
        Currency_t          price           = getJsonInt(    cartItem, "Price"         );
        bool                gstApplicable   = getJsonBool(   cartItem, "GSTApplicable" );
        Currency_t          priceGST        = CalculateGST( price, vatRate );
        bool                isSurcharge     = type == "PremiumSurcharge" ? true : false;
        Json::Value         productRefunded;

        if ( id == ProductId_TPurse )
        {
            /*  Only one TPurse Load is allowed */
            itemIndexTPurseLoad             = response[ "data" ][ "cartitems" ].size();
            tpurseTxnAmount                 = price;
            gstApplicable                   = false;
            priceGST                        = 0;
            if ( cartItem[ "addvaluemedia" ].empty() == false )
            {
                media                       = cartItem[ "addvaluemedia" ];
                cardNumber                  = getJsonString( media, "cardnumber" );
                txnSeqNo                    = getJsonInt(    media, "txnseqno"   );
            }
            else
            {
                /*  DEPRECATED! */
                media[ "cardnumber" ]       = cardNumber;
                media[ "txnseqno"   ]       = txnSeqNo;
            }
        }

        productRefunded[ "itemnumber"    ]  = itemNumber;
        productRefunded[ "GSTApplicable" ]  = gstApplicable;
        productRefunded[ "Price"         ]  = price;
        productRefunded[ "id"            ]  = cartItem[ "id"            ];
        productRefunded[ "issuer_id"     ]  = issuerId == 0 ? PRODUCT_OWNER_DEFAULT : issuerId;
        productRefunded[ "long_desc"     ]  = cartItem[ "long_desc"     ];
        productRefunded[ "short_desc"    ]  = cartItem[ "short_desc"    ];
        productRefunded[ "subtype"       ]  = cartItem[ "subtype"       ];
        productRefunded[ "type"          ]  = type;

        response[ "data" ][ "cartitems" ].append( productRefunded );
        amountTotal                        += price;
        if ( gstApplicable == true )
        {
            amountGST                      += priceGST;
        }

        #if     __IF_NEEDED__
        if ( isSurcharge == true )
        {
            response[ "data" ][ "surcharge" ][ "cartitems" ].append( productSold );
            amountSurchargeTotal           += price;
            if ( gstApplicable == true )
            {
                amountSurchargeGST         += priceGST;
            }
        }
        #endif
    }

    #if     OPT_ENABLE_ROUNDING
    if ( paymentType == "cash" )
    {
        /*  Round amount to be paid to nearest rounding portion */
        amountRefunded          = CurrencyRounding( amountTotal,          g_initCurrencyRoundingPortion );
        amountSurchargeRefunded = CurrencyRounding( amountSurchargeTotal, g_initCurrencyRoundingPortion );
    }
    else
    #endif
    {
        amountRefunded          = amountTotal;
        amountSurchargeRefunded = amountSurchargeTotal;
    }
    #if     !OPT_GST_ROUNDING_PER_PRODUCT
    amountGST                   = ( CurrencyRounding( amountGST,          _1CENT ) / _1CENT );
    amountSurchargeGST          = ( CurrencyRounding( amountSurchargeGST, _1CENT ) / _1CENT );
    #endif

    if ( paymentType == "tpurse" || itemIndexTPurseLoad >= 0 )
    {
        /*  Product sales transaction paid by TPurse or TPurse add value as one of the product sales item */
        SequenceFunction_e  brSequence              = itemIndexTPurseLoad >= 0 ?
                                                            SEQ_FUNCTION_ADD_VALUE_REVERSAL :
                                                            SEQ_FUNCTION_DEBIT_TPURSE_VALUE_REVERSAL;
        bool                isOperatorCard          = pDataOut->DynamicData.isOperator == FALSE ? false : true;
        int                 passengerCode           = 0;

        if ( isOperatorCard == false )
        {
            std::string  csn    = getMykiPAN( );

            if ( cardNumber == csn )
            {
                if ( brSequence == SEQ_FUNCTION_ADD_VALUE_REVERSAL )
                {
                    /*  Sets TPurse load reversal business rule parameters */
                    pDataIn->DynamicData.tpurseLoadAmount   = tpurseTxnAmount;
                }
                else
                {
                    /*  Sets TPurse debit reversal business rule parameters */
                    pDataIn->DynamicData.tPurseDebitAmount  = \
                    tpurseTxnAmount                         = amountRefunded;
                }
                pDataIn->DynamicData.reverseTxSeqNo         = txnSeqNo;

                /*  Execute business sequence */
                if ( ExecuteBusinessSequence( 1 /*ONE_CARD*/, isOperatorCard, passengerCode, brSequence, true ) == true )
                {
                    canRefundProduct                                        = true;

                    response[ "data" ][ "cardnumber"     ]                  = csn.c_str( );
                    response[ "data" ][ "remvalue"       ]                  = getRemainingValue( );
                    response[ "data" ][ "txnseqno"       ]                  = txnSeqNo;
                    response[ "data" ][ "revtxnseqno"    ]                  = pDataOut->ReturnedData.txnSeqNo;
                    media[ "cardnumber"  ]                                  = csn.c_str( );
                    media[ "remvalue"    ]                                  = getRemainingValue( );
                    media[ "txnseqno"    ]                                  = txnSeqNo;
                    media[ "revtxnseqno" ]                                  = pDataOut->ReturnedData.txnSeqNo;
                    if ( brSequence == SEQ_FUNCTION_ADD_VALUE_REVERSAL )
                    {
                        response[ "data" ][ "cartitems" ][ itemIndexTPurseLoad ][ "addvaluemedia" ]    = media;
                    }
                    else
                    {
                        response[ "data" ][ "paymentmedia" ]                = media;
                    }
                    response[ "data" ][ "txnamount"      ]                  = tpurseTxnAmount;
                    response[ "data" ][ "string_reason"  ]                  = getAcceptReasonString( pDataOut->ReturnedData.acceptReason );
                    response[ "data" ][ "amounttotal"    ]                  = amountTotal;
                    response[ "data" ][ "amountgst"      ]                  = amountGST;
                    response[ "data" ][ "amountrounding" ]                  = ( amountRefunded - amountTotal );
                    response[ "data" ][ "amountrefunded" ]                  = amountRefunded;
                    #if     __IF_NEEDED__
                    response[ "data" ][ "surcharge" ][ "amounttotal"    ]   = amountSurchargeTotal;
                    response[ "data" ][ "surcharge" ][ "amountgst"      ]   = amountSurchargeGST;
                    response[ "data" ][ "surcharge" ][ "amountrounding" ]   = ( amountSurchargeRefunded - amountSurchargeTotal );
                    response[ "data" ][ "surcharge" ][ "amountrefunded" ]   = amountSurchargeRefunded;
                    #endif
                    response[ "string_error" ]                              = getResultString( WS_SUCCESS );
                }
                else
                if ( pDataOut->ReturnedData.sequenceResult == SEQ_RESULT_REJECT )
                {
                    /*  Transaction rejected by business rule */
                    response[ "data" ][ "cardnumber" ]                      = csn.c_str( );
                    response[ "data" ][ "remvalue"   ]                      = getRemainingValue( );
                    media[ "cardnumber" ]                                   = csn.c_str( );
                    media[ "remvalue"   ]                                   = getRemainingValue( );
                    if ( brSequence == SEQ_FUNCTION_ADD_VALUE_REVERSAL )
                    {
                        response[ "data" ][ "cartitems" ][ itemIndexTPurseLoad ]    = media;
                    }
                    else
                    {
                        response[ "data" ][ "paymentmedia" ]                = media;
                    }
                    response[ "data" ][ "string_reason" ]                   = getRejectReasonString( pDataOut->ReturnedData.rejectReason );
                    response[ "string_error" ]                              = getResultString( WS_ERROR );
                    switch ( pDataOut->ReturnedData.rejectReason )
                    {
                    case    MYKI_BR_REJECT_REASON_CARD_BLOCKED:
                    case    MYKI_BR_REJECT_REASON_APPLICATION_BLOCKED:
                        response[ "data" ][ "string_blocking_reason" ]      = getBlockingReasonCode( pDataOut->ReturnedData.blockingReason );
                        break;

                    case    MYKI_BR_REJECT_REASON_EXPIRED:
                        response[ "data" ][ "expiry" ]                      = getExpiry( pDataOut );
                        break;

                    default:
                        /*  Others! DONOTHING! */
                        break;
                    }
                }
                else
                {
                    /*  Failed read/write card */
                    response[ "string_error" ]  = getResultString( WS_ERROR_READ_CARD );
                }

                /*  Done with transaction */
                pDataIn->DynamicData.tpurseLoadAmount       = 0;
                pDataIn->DynamicData.tPurseDebitAmount      = 0;
            }
            else
            {
                CsDbg( APP_DEBUG_STATE, "processProductSalesReversal : wrong card (%s/%s)",
                        cardNumber.c_str(), csn.c_str() );
                response[ "data" ][ "cardnumber" ]  = cardNumber;
                response[ "string_error" ]          = getResultString( WS_ERROR_WRONG_CARD );
            }
        }
        else
        {
            /*  Not Transit card!? */
            CsErrx( "processProductSalesReversal : none TRANSIT card detected" );
            response[ "data" ][ "cardnumber" ]  = cardNumber;
            response[ "string_error" ]          = getResultString( WS_ERROR_NOT_TRANSIT_CARD );
        }
    }
    else
    {
        /*  Product sales transaction does not involve updating patron card */
        canRefundProduct                                        = true;
        memcpy( pDataOut, pDataIn, sizeof( MYKI_BR_ContextData_t ) );
        {
            CsTime_t            nowTime                         = { 0 };
            CsTime( &nowTime );
            pDataOut->DynamicData.currentDateTime               = nowTime.tm_secs;
        }
        response[ "data" ][ "string_reason"  ]                  = getAcceptReasonString( MYKI_BR_ACCEPT_REASON_NOT_APPLICABLE );
        response[ "data" ][ "amounttotal"    ]                  = amountTotal;
        response[ "data" ][ "amountgst"      ]                  = amountGST;
        response[ "data" ][ "amountrounding" ]                  = ( amountRefunded - amountTotal );
        response[ "data" ][ "amountrefunded" ]                  = amountRefunded;
        #if     __IF_NEEDED__
        response[ "data" ][ "surcharge" ][ "amounttotal"    ]   = amountSurchargeTotal;
        response[ "data" ][ "surcharge" ][ "amountgst"      ]   = amountSurchargeGST;
        response[ "data" ][ "surcharge" ][ "amountrounding" ]   = ( amountSurchargeRefunded - amountSurchargeTotal );
        response[ "data" ][ "surcharge" ][ "amountrefunded" ]   = amountSurchargeRefunded;
        #endif
        response[ "string_error"             ]                  = getResultString( WS_SUCCESS );
    }

    if ( canRefundProduct == true )
    {
        if ( CreateProductSalesReversalTmi( pDataOut, response ) == 0 )
        {
            /*  Successfully created product sales TMI record */
            unsigned int     globalSalesTxnNum                  = 0;

            /*  Retrieves last generated (sale) transaction record number */
            if ( TMI_getCounter( "salesTransactionCount", &globalSalesTxnNum ) < 0 )
            {
                CsErrx( "processProductSalesReversal : TMI_getCounter() failed" );
                globalSalesTxnNum                               = 0;
            }

            response[ "data" ][ "revsaleseqno"  ]               = globalSalesTxnNum;
            response[ "data" ][ "vat_rate"      ]               = vatRate;
        }
        else
        {
            /*  Failed creating product sales TMI record */
            response[ "data" ][ "string_reason" ]               = getRejectReasonString( MYKI_BR_REJECT_REASON_NOT_APPLICABLE );
            response[ "string_error" ]                          = getResultString( WS_ERROR );

            /*  TODO:   Generate alarm */
        }
    }

    return fw.write( response );
}

    /**
     *  @brief  Rounding the amount to currency portion.
     *  @param  amount amount to be rounded.
     *  @return Rounded amount.
     */
static
Currency_t
CurrencyRounding( Currency_t amount, Currency_t roundingPortion )
{
    if ( roundingPortion > 0 )
    {
        Currency_t  leftOver    = ( amount % roundingPortion );

        amount -= leftOver;
        if ( ( leftOver * 2 ) >= roundingPortion )
        {
            /*  Round up to next currency portion */
            amount += roundingPortion;
        }
    }
    return  amount;
}

    /**
     *  @brief  Calculates GST given amount including GST.
     *  @param  amount amount including GST.
     *  @param  vatRate percentage GST.
     *  @return Calculated GST amount rounded to nearest cent.
     */
static
Currency_t
CalculateGST( Currency_t amount, int vatRate )
{
    Currency_t      amountGST   = ( ( amount * _1CENT ) * vatRate ) / ( 100 + vatRate );

    #if     OPT_GST_ROUNDING_PER_PRODUCT
    amountGST   = ( CurrencyRounding( amountGST, _1CENT ) / _1CENT );
    #endif

    return  amountGST;
}
