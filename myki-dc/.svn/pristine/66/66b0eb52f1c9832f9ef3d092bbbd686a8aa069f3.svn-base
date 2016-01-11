/**
 * @file
 * @copyright 2015 Vix Technology. All rights reserved
 * $Author$
 * $Date$
 * $Rev$
 * @brief Contains functions for handling productsales messages.
 */

#ifndef MYKI_DC_PRODUCTSALES_H
#define MYKI_DC_PRODUCTSALES_H

#include <json/json.h>

/**
 * @brief Handles the "productsales" message from the browser, and passes it 
 * onto readerapp.
 *
 * @see http://cew.vix.local:8090/display/VM/Non-Transit+Product+Sale+and+Surcharge
 *
 * @param request A JSON object of the following format:
 * @code
 * {
 *     "terminalid":"1234",
 *     "userid":"D101",
 *     "name":"productsales",
 *     "data":
 *     {
 *         "paymenttype":"cash",
 *         "cardnumber":"0123456789ABCDEF",
 *         "cartitems":
 *             [
 *             {
 *                 "product":
 *                 {
 *                     "quantity":2,
 *                     "GSTApplicable":false,
 *                     "PLU":"4. myki Child",
 *                     "Price":50,
 *                     "id":46,
 *                     "long_desc":"GTS Timetable",
 *                     "short_desc":"GTS Timetable",
 *                     "subtype":"None",
 *                     "type":"ThirdParty"
 *                 },
 *             },
 *             {
 *                 "product":
 *                 {
 *                     "quantity":1,
 *                     "GSTApplicable":false,
 *                     "Price":5000,
 *                     "id":300,
 *                     "long_desc":"TPurse Load",
 *                     "short_desc":"TPurse Load",
 *                     "subtype":"None",
 *                     "type":"LLSC"
 *                 },
 *             },
 *             { ... }
 *         ]
 *     }
 * }
 * @endcode
 * @return JSON as a string, in the following format:
 * @code
 * {  
 *     "data":null,
 *     "errorcode":0,
 *     "name":"productsales",
 *     "terminalid":"90400642",
 *     "timestamp":"2015-12-11T16:45:21+10:00",
 *     "type":"GET",
 *     "userid":""
 * }
 * @endcode
 */
std::string process_GET_productsales(const Json::Value &request);

/**
 * @brief Handles the "productsales" message from readerapp.
 *
 * Adds the transaction to the persistent transaction list (via 
 * TxnList::TxnManager), then sends a message to the browser.
 *
 * @param
 * @code
 * {
 *     "terminalid":"1234",
 *     "userid":"D101",
 *     "name":"productsale",
 *     "string_error":"WS_SUCCESS",
 *     "string_blocking_reason":"BLOCKING_REASON_LOST_STOLEN",
 *     "expiry":"24 Dec 15 12:00 pm",
 *     "timestamp":1449710654,
 *     "data":
 *     {
 *         "paymenttype":"cash",
 *         "cardnumber":"0123456789ABCDEF",
 *         "remvalue":5200,
 *         "txnseqno":1234,
 *         "saleseqno":1212,
 *         "vat_rate":10,
 *         "string_reason":"REASON_NOT_APPLICABLE",
 *         "cartitems": 
 *         [
 *             {
 *                 "itemnumber":1,
 *                 "GSTApplicable":false,
 *                 "PLU":"4. myki Child",
 *                 "Price":50,
 *                 "id":46,
 *                 "long_desc":"GTS Timetable",
 *                 "short_desc":"GTS Timetable",
 *                 "subtype":"None",
 *                 "type":"ThirdParty"
 *             },
 *             {
 *                 "itemnumber":2,
 *                 "GSTApplicable":false,
 *                 "PLU":"4. myki Child",
 *                 "Price":50,
 *                 "id":46,
 *                 "long_desc":"GTS Timetable",
 *                 "short_desc":"GTS Timetable",
 *                 "subtype":"None",
 *                 "type":"ThirdParty"
 *             },
 *             {
 *                 "itemnumber":3,
 *                 "GSTApplicable":false,
 *                 "Price":5000,
 *                 "id":300,
 *                 "long_desc":"TPurse Load",
 *                 "short_desc":"TPurse Load",
 *                 "subtype":"None",
 *                 "type":"LLSC"
 *             }
 *         ]
 *     }
 * }
 * @endcode
 * @return An empty string.
 */
std::string process_SET_productsales(const Json::Value &request);

#endif
