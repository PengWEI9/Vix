/* -*- mode: c; tabs: 4; -*- */
/*=============================================================================
**
**    Vix Technology                   Licensed software
**    (C) 2015                         All rights reserved
**
**=============================================================================
**
**  Project/Product : NGBU
**  Filename        : utcmd.cpp
**  Author(s)       : An Tran
**
**  Description     :
*/
/**     @file
**      @brief  Implements CLI commands for unit-tests.
*/
/*  Member(s)       :
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
**    1.00  30.12.15    ANT   Create
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

#include <json/json.h>

#include <cs.h>
#include <csf.h>
#include <csfcmd.h>

#include "card_processing_thread.h"
#include "datastring.h"
#include "dataxfer.h"
#include "utcmd.h"

/*
 *      Options
 *      -------
 */

#define UT_ProductSalesReversal         TRUE

/*
 *      Local Constants and Macros
 *      --------------------------
 */

#define SayOK( pEnv )                   CsfCmdPrintf( pEnv, "OK\n" )

/*
 *      Local Variables
 *      ---------------
 */

/*
 *      Global Variables
 *      ----------------
 */


#if     UT_ProductSalesReversal
static  const char     *Desc_salereversal       = "Send Product Sales Reversal Request";
static  const char     *Help_salereversal[ ]    =
{
    "\1"    "Synposis:",
    "\2"        "salereversal [items-to-refund [tpurse-option]]",
                "",
    "\1"    "Parameters:",
    "\2"        "items-to-refund    number of items to refund",
                "tpurse-option      [Tt]purse as payment type or [Ll]oad value"
                "",
    "\1"    "Description:",
    "\2"        "Send product sales reversal request.",
                "",
    NULL
};

    /**
     *  @brief  Processes 'salereversal' command.
     *  @param  argc number of command line arguments.
     *  @param  argv command line argument values.
     *  @return 0 if successful; -1 if failed.
     */
static
int
Cmd_salereversal( CmdEnv_t *pEnv, int argc, const char *argv[] )
{
    #define SALEREVERSAL_COMMAND        0
    #define SALEREVERSAL_ITEMSTOREFUND  1
    #define SALEREVERSAL_TPURSEOPTION   2
    #define _1MINUTE                    60

    MYKI_BR_ContextData_t  *pData                   = GetCardProcessingThreadContextData();
    const char             *pErrTooManyArguments    = "%s: too many argument(s)\n";
    const char             *pErrInvalidArguments    = "%s: invalid argument(s)\n";
    const char             *pCommand                = argv[ SALEREVERSAL_COMMAND ];
    Json::Value             request;
    std::string             requestJson;
    Json::FastWriter        fw;
    Json::Value             cartItems               = Json::Value( Json::arrayValue );
    char                    TerminalId[ 32 ]        = { 0 };
    int                     itemsToRefund           = 1;
    int                     itemsRefunded           = 0;
    CsTime_t                now                     = { 0 };
    char                    timeStr[ 64 ]           = { 0 };
    std::string             paymentType             = "cash";
    bool                    addValue                = false;

    switch ( argc )
    {
    case    ( SALEREVERSAL_TPURSEOPTION + 1 ):
        switch ( argv[ SALEREVERSAL_TPURSEOPTION ][ 0 ] )
        {
        case    't':    case    'T':    /*  [Tt]purse   */
        case    'p':    case    'P':    /*  [Pp]ayment  */  paymentType = "tpurse"; break;
        case    'l':    case    'L':    /*  [Ll]oad     */  addValue    = true;     break;
        default:
            CsfCmdPrintf( pEnv, pErrInvalidArguments, pCommand );
            return  1;
        }
        /*  DROP THROUGH TO NEXT CASE! */

    case    ( SALEREVERSAL_ITEMSTOREFUND + 1 ):
        itemsToRefund       = atoi( argv[ SALEREVERSAL_ITEMSTOREFUND ] );
        if ( itemsToRefund <= 0 )
        {
            CsfCmdPrintf( pEnv, pErrInvalidArguments, pCommand );
            return  1;
        }
        /*  DROP THROUGH TO NEXT CASE! */

    case    ( SALEREVERSAL_COMMAND + 1 ):
        break;

    default:
        CsfCmdPrintf( pEnv, pErrTooManyArguments, pCommand );
        return  1;
    }

    CsSnprintf( TerminalId, sizeof( TerminalId ), "%d", pData->StaticData.deviceId );

    CsTime( &now );
    CsStrTimeISO( &now, sizeof( timeStr ) - 1, timeStr );

    request[ "terminalid"   ]   = TerminalId;
    request[ "userid"       ]   = "DRIVER1",
    request[ "name"         ]   = "productsalesreversal";
    request[ "timestamp"    ]   = timeStr;
    request[ "type"         ]   = "set";

    while ( itemsRefunded < itemsToRefund )
    {
        Json::Value         cartItem1;
        cartItem1[ "GSTApplicable"  ]   = true;
        cartItem1[ "id"             ]   = 46;
        cartItem1[ "issuer_id"      ]   = 1;
        cartItem1[ "itemnumber"     ]   = ( itemsRefunded + 1 );
        cartItem1[ "long_desc"      ]   = "GTS Timetable";
        cartItem1[ "PLU"            ]   = "GTS Timetable";
        cartItem1[ "Price"          ]   = 50;
        cartItem1[ "short_desc"     ]   = "GTS Timetable";
        cartItem1[ "subtype"        ]   = "None";
        cartItem1[ "type"           ]   = "ThirdParty";
        cartItems.append( cartItem1 );
        itemsRefunded++;
    }
    if ( addValue == true )
    {
        Json::Value         cartItem2;
        Json::Value         media2;
        cartItem2[ "GSTApplicable"  ]   = true;
        cartItem2[ "id"             ]   = 300;
        cartItem2[ "issuer_id"      ]   = 1;
        cartItem2[ "itemnumber"     ]   = ( itemsRefunded + 1 );
        cartItem2[ "long_desc"      ]   = "Add Value";
        cartItem2[ "PLU"            ]   = "Add Value";
        cartItem2[ "Price"          ]   = 10;
        cartItem2[ "short_desc"     ]   = "Add Value";
        cartItem2[ "subtype"        ]   = "None";
        cartItem2[ "type"           ]   = "LLSC";
        media2   [ "cardnumber"     ]   = "308425030313041";
        media2   [ "txnseqno"       ]   = 20;
        cartItem2[ "addvaluemedia"  ]   = media2;
        cartItems.append( cartItem2 );
        itemsRefunded++;
    }

    now.tm_secs    -= _1MINUTE;
    CsStrTimeISO( &now, sizeof( timeStr ) - 1, timeStr );

    request[ "data" ]                       = Json::Value();
    request[ "data" ][ "paymenttype"    ]   = paymentType;
    request[ "data" ][ "saleseqno"      ]   = 65;
    request[ "data" ][ "saletimestamp"  ]   = timeStr;
    request[ "data" ][ "cartitems"      ]   = cartItems;
    request[ "data" ][ "route_id"       ]   = 2015;
    request[ "data" ][ "stop_id"        ]   = 12;
    if ( paymentType == "tpurse" )
    {
        Json::Value         media1;
        media1 [ "cardnumber" ]             = "308425030313041";
        media1 [ "txnseqno"   ]             = 20;
        request[ "data" ][ "paymentmedia" ] = media1;
    }

    requestJson = fw.write( request );
    ungetMessage( requestJson );
    SayOK( pEnv );

    return  0;
}
#endif

    /**
     *  @brief  Adds unit-test CLI commands.
     *  @return 0 if successful; otherwise failed.
     */
int
AddUnitTestCommands( void )
{
    #define CMD_DECLARATION( c )    { #c, Desc_##c, Cmd_##c, Help_##c },

    static  CmdToken_t  Commands[ ]     =
    {
        #if     UT_ProductSalesReversal
        CMD_DECLARATION( salereversal )
        #endif
        { NULL }
    };
    static  int         SizeOfCommands  = ( sizeof( Commands ) / sizeof( CmdToken_t ) ) - 1;

    for ( int i = 0; i < SizeOfCommands; i++ )
    {
        CsfCmdRegister( &Commands[ i ] );
    }

    return  0;
}
