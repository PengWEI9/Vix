/* -*- mode: c; tabs: 4; -*- */
/*=============================================================================
**
**    Vix Technology                   Licensed software
**    (C) 2015                         All rights reserved
**
**=============================================================================
**
**  Project/Product : NGBU
**  Filename        : gentmi.h
**  Author(s)       : An Tran
**
**  Description     :
*/
/**     @file
**      @brief  Declares function prototypes to generate TMI records.
*/
/*  Member(s)       :
**
**  Information     :
**   Compiler(s)    : C++
**   Target(s)      : Independent
**
**  Subversion      :
**      $Id: gentmi.h 88278 2016-01-06 01:05:34Z atran $
**      $HeadURL: https://auperasvn01.aupera.erggroup.com/svn/DPG_SWBase/vgac_readerapp/trunk/src/gentmi.h $
**
**  History         :
**   Vers.  Date        Aut.  Type     Description
**   -----  ----------  ----  -------  ----------------------------------------
**    1.00  21.09.15    ANT   Create
**
**===========================================================================*/

#if     !defined( __GENTMI_H_INCLUDED )
#define __GENTMI_H_INCLUDED             1

/*
 *      Options
 *      -------
 */

/*
 *      Includes
 *      --------
 */

#include "userid/persistentuserid.h"
#include <myki_br.h>
#include <string>

/*
 *      Constants and Macros
 *      --------------------
 */

    /*  Transaction types */
#define TransactionType_CscValidation                       SEQ_FUNCTION_DEFAULT                        /**< Card validation */
#define TransactionType_ValidatePin                         SEQ_FUNCTION_VALIDATE_PIN                   /**< Operator PIN validation */
#define TransactionType_UpdatePin                           SEQ_FUNCTION_UPDATE_PIN                     /**< Operator PIN update */
#define TransactionType_ShiftEnd                            SEQ_FUNCTION_UPDATE_SHIFT_DATA              /**< Shift end */
#define TransactionType_TPurseTopupPaidCash                 SEQ_FUNCTION_ADD_VALUE                      /**< TPurse load paid by cash */
#define TransactionType_TPurseTopupReversalCashRefunded     SEQ_FUNCTION_ADD_VALUE_REVERSAL             /**< TPurse load reversal cash refunded */
#define TransactionType_ProductSalesPaidTPurse              SEQ_FUNCTION_DEBIT_TPURSE_VALUE             /**< Product sales paid by TPurse */
#define TransactionType_ProductSalesReversalTPurseRefunded  SEQ_FUNCTION_DEBIT_TPURSE_VALUE_REVERSAL    /**< Product sales reversal TPurse refunded */

#define ProductId_TPurse                                300

#define PRODUCT_OWNER_DEFAULT                           1       /**< Default product owner id */
#define MAX_PRODUCT_SALES_ITEMS                         100     /**< Maximum products that can be sold in one transaction */

    /** Event log types */
enum
{
        EvLogType_Unknown   = (0),
        EvLogType_EnterStop,
        EvLogType_DepartFirstStop,
        EvLogType_EnterLastStop,
        EvLogType_DepartStop,
        EvLogType_ManualStop,
        EvLogType_Periodic,
        EvLogType_NoStopDetected,
        EvLogType_DriverChangeEnd,
        EvLogType_DriverBreakStart,
        EvLogType_DriverBreakEnd,
        EvLogType_TripSelection,
        EvLogType_RouteSelection,
        EvLogType_HeadlessOperation
};

/*
 *      External References
 *      -------------------
 */
extern   		bool            g_iniGenerateTmi;
extern          std::string     g_defaultDriverId;
/*
 *      Prototypes
 *      ----------
 */

PersistentUserId &getUserId();
int             TmiInit( );
int             CreateTransactionTmi( MYKI_BR_ContextData_t *pData, int transactionType, bool isUnconfirmed );
int             CreateTPurseLoadTmi( MYKI_BR_ContextData_t *pData );
int             CreateTPurseLoadReversalTmi( MYKI_BR_ContextData_t *pData );
int             CreatePassengerCountTmi( MYKI_BR_ContextData_t *pData, bool isConcession );
int             CreateShiftOpenTmi( MYKI_BR_ContextData_t *pData );
int             CreateShiftCloseTmi( MYKI_BR_ContextData_t *pData );
int             CreateEvStop( MYKI_BR_ContextData_t *pData, int evLogType,
                              int routeId, int stopId, const char *pStopName, bool gpsAvailable, double latitude, double longitude, int noStopDuration );
int             CreateTripRouteSelection( MYKI_BR_ContextData_t *pData, int evLogType,
                                          int shiftId, int tripId, int tripStartTime, int routeId, int firstStopId, int lastStopId );
int             CreateOpLog( MYKI_BR_ContextData_t *pData );
int             CheckAndCommitPeriod( MYKI_BR_ContextData_t *pData );
int             CreateProductSalesTmi( MYKI_BR_ContextData_t *pData, Json::Value &productSaleDetails );
int             CreateProductSalesReversalTmi( MYKI_BR_ContextData_t *pData, Json::Value &productSaleReversalDetails );

unsigned int    GetCounter( const char *pKey );

#endif  /*  !__GENTMI_H_INCLUDED */
