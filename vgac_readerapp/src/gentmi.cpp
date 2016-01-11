/* -*- mode: c; tabs: 4; -*- */
/*=============================================================================
**
**    Vix Technology                   Licensed software
**    (C) 2015                         All rights reserved
**
**=============================================================================
**
**  Project/Product : NGBU
**  Filename        : gentmi.cpp
**  Author(s)       : An Tran
**
**  Description     :
*/
/**     @file
**      @brief  Defines functions to generate TMI records.
*/
/*  Member(s)       :
**
**  Information     :
**   Compiler(s)    : C++
**   Target(s)      : Independent
**
**  Subversion      :
**      $Id: gentmi.cpp 88757 2016-01-10 23:51:25Z atran $
**      $HeadURL: https://auperasvn01.aupera.erggroup.com/svn/DPG_SWBase/vgac_readerapp/trunk/src/gentmi.cpp $
**
**  History         :
**   Vers.  Date        Aut.  Type     Description
**   -----  ----------  ----  -------  ----------------------------------------
**    1.00  21.09.15    ANT   Create
**    1.01  13.11.15    ANT   Modify   NGBU-854: Rectified remittance totals
**    1.02  11.12.15    ANT   Add      NGBU-952: Added non-transit/surcharge
**                                     product sales
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

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <cs.h>
#include <csf.h>
#include <datetime.h>
#include <myki_cardservices.h>
#include <myki_cd.h>
#include <myki_br.h>
#include <myki_tmi.h>
#include <myki_tmi_avl.h>
#include <myki_tmi_oplog.h>

#include "app_debug_levels.h"
#include "card_processing_common.h"
#include "cardfunctions.h"
#include "datastring.h"
#include "gentmi.h"

/*
 *      Options
 *      -------
 */

#define OPT_EXCLUDE_TPurseDetails       TRUE        /**< TRUE if exclude <TPurseDetails> element */

/*
 *      External References
 *      -------------------
 */

    /*  GAC_PROCESSING_THREAD.CPP */
extern  char            g_terminalType[ CSFINI_MAXVALUE + 1 ];
extern  char            g_userIdFile[ CSFINI_MAXVALUE + 1 ];

/*
 *      Local Constants and Macros
 *      --------------------------
 */

#define DEFAULT_TMI_CONFIRMED           "/afc/data/tmi/confirmed"
#define DEFAULT_TMI_TRANSFER            "/afc/data/tmi/transfer"

/*
 *      Local Prototypes
 *      ----------------
 */

static  char           *GetTransactionLDT( char *pPath, int pathSize, time_t transactionDateTime, bool isUnconfirmed );
static  int             GetTPurseProductOwner( void );
static  std::string     GetDefaultUserId( );
static  int             IncrementCounterBy( const char *pKey, int value );
static  const char     *GetLdtName( char *pBuf, int bufSize, int transactionType );
static  time_t          isoStrToTime( const std::string &timeStr );

/*
 *      Local Variables
 *      ---------------
 */

static  char            g_iniTmiConfirmed[ CSFINI_MAXVALUE + 1 ]    = { 0 };    /**< TMI confirmed path */
static  char            g_iniTmiTransfer [ CSFINI_MAXVALUE + 1 ]    = { 0 };    /**< TMI transfer path */
bool                    g_iniGenerateTmi                            = false;    /**< Enable/Disable TMI generation */
std::string             g_defaultDriverId = "0";

PersistentUserId &getUserId()
{
    static PersistentUserId instance(g_userIdFile);
    return instance;
}

    /**
     *  @brief  Converts a string to enum type TMI_PaymentType_e.
     *  @param  str The string to convert, eg "cash" or "tpurse".
     *  @return valid payment type, or TMI_PAYMENT_TYPE_NONE on error.
     */
static TMI_PaymentType_e strToPaymentType(const std::string &str)
{
    if (str == "cash") return TMI_PAYMENT_TYPE_CASH;
    if (str == "tpurse") return TMI_PAYMENT_TYPE_TPURSE;
    return TMI_PAYMENT_TYPE_NONE;
}

    /**
     *  @brief  Initialises TMI interface.
     *  @return 0 if successful; -1 otherwise.
     */
int
TmiInit( )
{
    #define RWXRWXRWX   0777

    char                iniBuf[ CSFINI_MAXVALUE + 1 ]   = { 0 };
    bool                iniGenerateTmi                  = false;

    /*  Enable/Disable TMI generation */
	if ( CsfIniExpand( "LDT:GenerateTmi", iniBuf, CSFINI_MAXVALUE ) == 0 )
	{
		if ( iniBuf[ 0 ] == 'y' || iniBuf[ 0 ] == 'Y' ||    /*  [Yy]es      */
             iniBuf[ 0 ] == 't' || iniBuf[ 0 ] == 'T' ||    /*  [Tt]rue     */
             iniBuf[ 0 ] == 'e' || iniBuf[ 0 ] == 'E' ||    /*  [Ee]nable   */
             iniBuf[ 0 ] == '1' )
        {
            iniGenerateTmi  = true;
        }
	}

    g_iniGenerateTmi    = false;
    if ( iniGenerateTmi != false )
    {
        if ( CsfIniExpand( "LDT:TmiConfirmed", g_iniTmiConfirmed, sizeof( g_iniTmiConfirmed ) ) != 0 )
        {
            strncpy( g_iniTmiConfirmed, DEFAULT_TMI_CONFIRMED, sizeof( g_iniTmiConfirmed ) );
        }
        if ( CsMkdir( g_iniTmiConfirmed, RWXRWXRWX ) < 0 )
        {
            CsErrx( "TmiInit : CsMkdir('%s') failed", g_iniTmiConfirmed );
            return  -1;
        }

        if ( CsfIniExpand( "LDT:TmiTransfer", g_iniTmiTransfer, sizeof( g_iniTmiTransfer ) ) != 0 )
        {
            strncpy( g_iniTmiTransfer, DEFAULT_TMI_TRANSFER, sizeof( g_iniTmiTransfer ) );
        }
        if ( CsMkdir( g_iniTmiTransfer, RWXRWXRWX ) < 0 )
        {
            CsErrx( "TmiInit : CsMkdir('%s') failed", g_iniTmiTransfer );
            return  -1;
        }

        g_iniGenerateTmi    = true;
	}

    return  0;
}

    /**
     *  @brief  Creates transaction TMI record.
     *  @param  pData BR context data
     *  @param  transactionType transaction type
     *  @param  isUnconfirmed true if card transaction is unconfirmed; false otherwise.
     *  @return 0 success; else failed.
     *  @note   Generating of transaction TMI records is optional.
     */
int
CreateTransactionTmi( MYKI_BR_ContextData_t *pData, int transactionType, bool isUnconfirmed )
{
    int         result  = 0;

    if ( g_iniGenerateTmi != false )
    {
        TMI_Trx_t               TmiTrx                  = { 0 };
        TMI_Ldt_t               TmiLdt                  = { { 0 } };
        time_t                  transactionDateTime     = (time_t)pData->InternalData.TransactionData.currentDateTime;
        char                   *pLdt                    = NULL;
        char                    ldtPath[ CSMAXPATHLEN ] = { 0 };
        int                     bLdtConsumed            = FALSE;
        char                    ldtName[ 64 ]           = { 0 };
        char                    CurrencyCode[ ]         = "AUD";

        /*  Retrieves transaction generated LDT (if any) */
        pLdt    = GetTransactionLDT( ldtPath, sizeof( ldtPath ), transactionDateTime, isUnconfirmed );

        if ( pData->ReturnedData.sequenceResult == SEQ_RESULT_REJECT )
        {
            /*  CSC validation failed */
            if ( transactionType == TransactionType_TPurseTopupPaidCash ||
                 transactionType == TransactionType_TPurseTopupReversalCashRefunded )
            {
                /*  Wraps generated LDT (if any) in <LDTContainer> instead */
                transactionType = TransactionType_CscValidation;
            }
        }

        /*  And generates transaction TMI record */
        switch ( transactionType )
        {
        case    TransactionType_CscValidation:
        case    TransactionType_ValidatePin:
        case    TransactionType_UpdatePin:
        case    TransactionType_ShiftEnd:
            if ( pLdt != NULL )
            {
                /*  Generates transaction TMI record */
                        TmiTrx.timestamp                            = transactionDateTime;
                strcpy( TmiTrx.status,                                "OK" );
                        TmiTrx.complaint                            = (bool)FALSE;
                strcpy( TmiTrx.currency,                              CurrencyCode );
                        TmiTrx.value                                = 0;
                        TmiTrx.serviceProviderId                    = (int)pData->StaticData.serviceProviderId;
                        TmiTrx.entryPointId                         = pData->DynamicData.entryPointId;
                strcpy( TmiLdt.name,                                  GetLdtName( ldtName, sizeof( ldtName ), transactionType ) );
                strcpy( TmiLdt.encoding,                              "UTF8" );
                        TmiLdt.ldt                                  = pLdt;
                result  = TMI_wrapCscValidationLdt( &TmiTrx, &TmiLdt );
                if ( result < 0 )
                {
                    CsErrx( "CreateTransactionTmi : TMI_wrapCscValidationLdt failed (%d)", result );
                    result          = (-1);
                }
                else
                {
                    CsDbg( APP_DEBUG_DETAIL, "CreateTransactionTmi : TMI_wrapCscValidationLdt successful" );
                    bLdtConsumed    = TRUE;
                    result          = 0;
                }
            }
            break;

        case    TransactionType_TPurseTopupPaidCash:
            /*  DONOTHING! Caller will create TMI record */
            result  = 0;
            break;

        case    TransactionType_TPurseTopupReversalCashRefunded:
            /*  DONOTHING! Caller will create TMI record */
            result  = 0;
            break;

        case    TransactionType_ProductSalesPaidTPurse:
            /*  DONOTHING! Caller will create TMI record */
            result  = 0;
            break;

        case    TransactionType_ProductSalesReversalTPurseRefunded:
            /*  DONOTHING! Caller will create TMI record */
            result  = 0;
            break;

        default:
            CsErrx( "CreateTransactionTmi : unknown transaction type (%d)", transactionType );
            result  = (-1);
            break;
        }   /*  end-of-switch */

        if ( pLdt != NULL )
        {
            /*  NOTE:   LDT buffer allocated in GetTransactionLDT() */
            CsFree( pLdt );

            /*  And removes LDT if consumed */
            if ( bLdtConsumed != FALSE )
            {
                unlink( ldtPath );
            }
        }
    }   /*  end-of-if */

    return  result;
}

    /**
     *  @brief  Creates TPurse load transaction TMI record.
     *  @param  pData BR context data
     *  @return 0 success; else failed.
     *  @note   Generating of transaction TMI records is optional.
     */
int
CreateTPurseLoadTmi( MYKI_BR_ContextData_t *pData )
{
    int         result              = 0;

    if ( g_iniGenerateTmi != false )
    {
        TMI_Trx_t               TmiTrx                  = { 0 };
        TMI_Payment_t           TmiCashPayment          = { TMI_PAYMENT_TYPE_NONE };
        TMI_Ldt_t               TmiLdt                  = { { 0 } };
        TMI_SoldProduct_t       TmiSoldProduct          = { 0 };
        TMI_Product_t          &TmiProduct              = TmiSoldProduct.product; 
        TMI_ProductDetails_t   &TmiProductDetails       = TmiProduct.productDetails;
        time_t                  transactionDateTime     = (time_t)pData->DynamicData.currentDateTime;
        char                   *pLdt                    = NULL;
        char                    ldtPath[ CSMAXPATHLEN ] = { 0 };
        bool                    bLdtConsumed            = false;
        char                    ldtName[ 64 ]           = { 0 };
        int                     productOwner            = 0;
        char                    CurrencyCode[ ]         = "AUD";
        U8_t                    vatRate                 = 0;

        /*  Retrieves transaction generated LDT (if any) */
        pLdt    = GetTransactionLDT( ldtPath, sizeof( ldtPath ), transactionDateTime, false );

        if ( pLdt != NULL )
        {
            /*  Retrieves TPURSE product owner */
            productOwner    = GetTPurseProductOwner( );

            #if     __TOBEDELETED__
            if ( MYKI_CD_getGSTPercentage( &vatRate ) == FALSE )
            {
                CsErrx( "CreateTPurseLoadTmi : MYKI_CD_getGSTPercentage() failed, set zero vat_rate" );
                vatRate     = 0;
            }
            #endif

            /*  Generates transaction TMI record */
                    TmiTrx.timestamp                            = transactionDateTime;
            strcpy( TmiTrx.status,                                "OK" );
                    TmiTrx.complaint                            = (bool)FALSE;
            strcpy( TmiTrx.currency,                              CurrencyCode );
                    TmiTrx.value                                = (int)pData->ReturnedData.topupAmount;
                    TmiTrx.serviceProviderId                    = (int)pData->StaticData.serviceProviderId;
                    TmiTrx.entryPointId                         = pData->DynamicData.entryPointId;
                    TmiSoldProduct.itemsToIssue                 = 1;
                    TmiSoldProduct.itemsIssued                  = 1;
                    TmiSoldProduct.basePrice                    = (int)pData->ReturnedData.topupAmount;
                    TmiProduct.productId                        = ProductId_TPurse;
                    TmiProduct.tariffVersion.majorVersion       = MYKI_CD_getMajorVersion( );
                    TmiProduct.tariffVersion.minorVersion       = MYKI_CD_getMinorVersion( );
                    TmiProduct.saleDate                         = transactionDateTime;
            strcpy( TmiProduct.currency,                          CurrencyCode );
                    TmiProduct.vatRate                          = (int)vatRate;
                    TmiProduct.price                            = (int)pData->ReturnedData.topupAmount;

                    TmiProductDetails.productOwner              = productOwner;
            strcpy( TmiProductDetails.fareIndex,                  "flat" );
                    TmiProductDetails.originId                  = pData->DynamicData.stopId;
                    TmiProductDetails.destinationId             = pData->DynamicData.stopId;
                    TmiProductDetails.route                     = pData->DynamicData.lineId;
                    TmiProductDetails.stopId                    = pData->DynamicData.stopId;
                    TmiProductDetails.cartItemNumber            = 1;
                    TmiCashPayment.paymentType                  = TMI_PAYMENT_TYPE_CASH;
            strcpy( TmiCashPayment.currency,                      CurrencyCode );
                    TmiCashPayment.accepted                     = (int)pData->ReturnedData.topupAmount;
            /*      TmiCashPayment.changed                      = 0;
                    TmiCashPayment.refunded                     = 0;
                    TmiCashPayment.invalid                      = 0;    */
            strcpy( TmiLdt.name,                                  GetLdtName( ldtName, sizeof( ldtName ), TransactionType_TPurseTopupPaidCash ) );
            strcpy( TmiLdt.encoding,                              "UTF8" );
                    TmiLdt.ldt                                  = pLdt;

            result  = TMI_createTPurseCashTopUpTrx( &TmiTrx, &TmiSoldProduct, 1, &TmiCashPayment, &TmiLdt );
            if ( result < 0 )
            {
                CsErrx( "CreateTPurseLoadTmi : TMI_createTPurseCashTopUpTrx failed (%d)", result );
                result          = (-1);
            }
            else
            {
                CsDbg( APP_DEBUG_DETAIL, "CreateTPurseLoadTmi : TMI_createTPurseCashTopUpTrx successful" );
                bLdtConsumed    = true;
                result          = 0;

                /*  Updates totals */
                {
                    int tripGrossCashAmount         = IncrementCounterBy( "tripGrossCashAmount",        (unsigned int)pData->ReturnedData.topupAmount );
                    int tripCashCount               = IncrementCounterBy( "tripCashCount",              1                                             );
                    int tripGrossSundryTickets      = IncrementCounterBy( "tripGrossSundryTickets",     1                                             );
                    int portionGrossCashAmount      = IncrementCounterBy( "portionGrossCashAmount",     (unsigned int)pData->ReturnedData.topupAmount );
                    int portionCashCount            = IncrementCounterBy( "portionCashCount",           1                                             );
                    int portionGrossSundryTickets   = IncrementCounterBy( "portionGrossSundryTickets",  1                                             );
                    int shiftGrossCashAmount        = IncrementCounterBy( "shiftGrossCashAmount",       (unsigned int)pData->ReturnedData.topupAmount );
                    int shiftCashCount              = IncrementCounterBy( "shiftCashCount",             1                                             );
                    int shiftGrossSundryTickets     = IncrementCounterBy( "shiftGrossSundryTickets",    1                                             );
                    int periodGrossCashAmount       = IncrementCounterBy( "periodGrossCashAmount",      (unsigned int)pData->ReturnedData.topupAmount );

                    CsDbg( APP_DEBUG_DETAIL, "CreateTPurseLoadTmi : Gross (%d:%d:%d)/(%d:%d:%d)/(%d:%d:%d)/(%d)",
                            tripCashCount,      tripGrossCashAmount,    tripGrossSundryTickets,
                            portionCashCount,   portionGrossCashAmount, portionGrossSundryTickets,
                            shiftCashCount,     shiftGrossCashAmount,   shiftGrossSundryTickets,
                            periodGrossCashAmount );
                }

                /*  Persists ALL counters */
                if ( ( result = TMI_saveCounters( ) ) < 0 )
                {
                    CsErrx( "CreateTPurseLoadTmi : TMI_saveCounters() failed (%d)", result );
                }
            }
        }
        else
        {
            CsErrx( "CreateTPurseLoadTmi : no LDT generated for TPurse load" );
        }   /*  end-of-if */

        if ( pLdt != NULL )
        {
            /*  NOTE:   LDT buffer allocated in GetTransactionLDT() */
            CsFree( pLdt );

            /*  And removes LDT if consumed */
            if ( bLdtConsumed == true )
            {
                unlink( ldtPath );
            }
        }
    }
    return  result;
}

    /**
     *  @brief  Creates TPurse load reversal TMI record.
     *  @param  pData BR context data
     *  @return 0 success; else failed.
     *  @note   Generating of transaction TMI records is optional.
     */
int
CreateTPurseLoadReversalTmi( MYKI_BR_ContextData_t *pData )
{
    int         result              = 0;

    if ( g_iniGenerateTmi != false )
    {
        TMI_Trx_t               TmiTrx                  = { 0 };
        TMI_Payment_t           TmiCashPayment          = { TMI_PAYMENT_TYPE_NONE };
        TMI_Ldt_t               TmiLdt                  = { { 0 } };
        TMI_RefundedProduct_t   TmiRefundedProduct      = { { 0 } };
        TMI_Product_t          &TmiProduct              = TmiRefundedProduct.product;
        TMI_ProductDetails_t   *pTmiProductDetails      = &TmiProduct.productDetails;
        time_t                  transactionDateTime     = (time_t)pData->DynamicData.currentDateTime;
        char                   *pLdt                    = NULL;
        char                    ldtPath[ CSMAXPATHLEN ] = { 0 };
        bool                    bLdtConsumed            = false;
        char                    ldtName[ 64 ]           = { 0 };
        int                     productOwner            = 0;
        char                    CurrencyCode[ ]         = "AUD";
        U8_t                    vatRate                 = 0;

        /*  Retrieves transaction generated LDT (if any) */
        pLdt    = GetTransactionLDT( ldtPath, sizeof( ldtPath ), transactionDateTime, false );

        if ( pLdt != NULL )
        {
            /*  Retrieves TPURSE product owner */
            productOwner    = GetTPurseProductOwner( );

            #if     __TOBEDELETED__
            if ( MYKI_CD_getGSTPercentage( &vatRate ) == FALSE )
            {
                CsErrx( "CreateTPurseLoadReversalTmi : MYKI_CD_getGSTPercentage() failed, set zero vat_rate" );
                vatRate     = 0;
            }
            #endif

            /*  And generates transaction TMI record */
                    TmiTrx.timestamp                            = transactionDateTime;
            strcpy( TmiTrx.status,                                "OK" );
                    TmiTrx.complaint                            = false;
            strcpy( TmiTrx.currency,                              CurrencyCode );
                    TmiTrx.value                                = (int)pData->ReturnedData.topupAmount;
                    TmiTrx.serviceProviderId                    = (int)pData->StaticData.serviceProviderId;
                    TmiTrx.entryPointId                         = pData->DynamicData.entryPointId;
            strcpy( TmiRefundedProduct.refundReason,              "TPurse" );
                    TmiRefundedProduct.itemsToRefund            = 1;
                    TmiRefundedProduct.itemsRefunded            = 1;
                    TmiRefundedProduct.refundedAmount           = (int)pData->ReturnedData.topupAmount;
            strcpy( TmiRefundedProduct.currency,                  CurrencyCode );
                    TmiProduct.productId                        = ProductId_TPurse;
                    TmiProduct.tariffVersion.majorVersion       = MYKI_CD_getMajorVersion( );
                    TmiProduct.tariffVersion.minorVersion       = MYKI_CD_getMinorVersion( );
                    TmiProduct.saleDate                         = transactionDateTime;
            strcpy( TmiProduct.currency,                          CurrencyCode );
                    TmiProduct.vatRate                          = (int)vatRate;
                    TmiProduct.price                            = (int)pData->ReturnedData.topupAmount;
                    TmiRefundedProduct.product                  = TmiProduct;
                    pTmiProductDetails->productOwner            = productOwner;
            strcpy( pTmiProductDetails->fareIndex,                "flat" );
                    pTmiProductDetails->originId                = pData->DynamicData.stopId;
                    pTmiProductDetails->destinationId           = pData->DynamicData.stopId;
                    pTmiProductDetails->route                   = pData->DynamicData.lineId;
                    pTmiProductDetails->stopId                  = pData->DynamicData.stopId;
                    pTmiProductDetails->cartItemNumber          = 1;
                    TmiCashPayment.paymentType                  = TMI_PAYMENT_TYPE_CASH;
            strcpy( TmiCashPayment.currency,                      CurrencyCode );
            /*      TmiCashPayment.accepted                     = 0;
                    TmiCashPayment.changed                      = 0;    */
                    TmiCashPayment.refunded                     = (int)pData->ReturnedData.topupAmount;
            /*      TmiCashPayment.invalid                      = 0;    */
            strcpy( TmiLdt.name,                                  GetLdtName( ldtName, sizeof( ldtName ), TransactionType_TPurseTopupReversalCashRefunded ) );
            strcpy( TmiLdt.encoding,                              "UTF8" );
                    TmiLdt.ldt                                  = pLdt;
            result  = TMI_createTPurseTopupCancelledTrx( &TmiTrx, &TmiRefundedProduct, 1, &TmiCashPayment, &TmiLdt );
            if ( result < 0 )
            {
                CsErrx( "CreateTPurseLoadReversalTmi : TMI_createTPurseTopupCancelledTrx failed (%d)", result );
                result          = (-1);
            }
            else
            {
                CsDbg( APP_DEBUG_DETAIL, "CreateTPurseLoadReversalTmi : TMI_createTPurseTopupCancelledTrx successful" );
                bLdtConsumed    = TRUE;
                result          = 0;

                /*  Updates totals */
                {
                    int tripAnnulledCashAmount          = IncrementCounterBy( "tripAnnulledCashAmount",         (unsigned int)pData->ReturnedData.topupAmount );
                    int tripAnnulledCashCount           = IncrementCounterBy( "tripAnnulledCashCount",          1                                             );
                    int tripAnnulledSundryTickets       = IncrementCounterBy( "tripAnnulledSundryTickets",      1                                             );
                    int portionAnnulledCashAmount       = IncrementCounterBy( "portionAnnulledCashAmount",      (unsigned int)pData->ReturnedData.topupAmount );
                    int portionAnnulledCashCount        = IncrementCounterBy( "portionAnnulledCashCount",       1                                             );
                    int portionAnnulledSundryTickets    = IncrementCounterBy( "portionAnnulledSundryTickets",   1                                             );
                    int shiftAnnulledCashAmount         = IncrementCounterBy( "shiftAnnulledCashAmount",        (unsigned int)pData->ReturnedData.topupAmount );
                    int shiftAnnulledCashCount          = IncrementCounterBy( "shiftAnnulledCashCount",         1                                             );
                    int shiftAnnulledSundryTickets      = IncrementCounterBy( "shiftAnnulledSundryTickets",     1                                             );
                    int periodAnnulledCashAmount        = IncrementCounterBy( "periodAnnulledCashAmount",       (unsigned int)pData->ReturnedData.topupAmount );

                    CsDbg( APP_DEBUG_DETAIL, "CreateTPurseLoadReversalTmi : Annulled (%d:%d:%d)/(%d:%d:%d)/(%d:%d:%d)/(%d)",
                            tripAnnulledCashCount,      tripAnnulledCashAmount,     tripAnnulledSundryTickets,
                            portionAnnulledCashCount,   portionAnnulledCashAmount,  portionAnnulledSundryTickets,
                            shiftAnnulledCashCount,     shiftAnnulledCashAmount,    shiftAnnulledSundryTickets,
                            periodAnnulledCashAmount );
                }

                /*  Persists ALL counters */
                if ( ( result = TMI_saveCounters( ) ) < 0 )
                {
                    CsErrx( "CreateTPurseLoadReversalTmi : TMI_saveCounters() failed (%d)", result );
                }
            }
        }

        if ( pLdt != NULL )
        {
            /*  NOTE:   LDT buffer allocated in GetTransactionLDT() */
            CsFree( pLdt );

            /*  And removes LDT if consumed */
            if ( bLdtConsumed == true )
            {
                unlink( ldtPath );
            }
        }
    }
    return  result;
}

    /**
     *  @brief  Creates passenger count TMI record.
     *  @param  pData BR context data
     *  @param  isConcession true if concession passenger type;
     *          false if full fare passenger type.
     *  @return 0 if successful; else failed.
     */
int
CreatePassengerCountTmi( MYKI_BR_ContextData_t *pData, bool isConcession )
{
    #define PASSENGER_COUNT_PRODUCT_ID      0

    int         result              = 0;

    if ( g_iniGenerateTmi != false )
    {
        TMI_Trx_t               TmiTrx                      = { 0 };
        TMI_SoldProduct_t       TmiSoldProduct              = { 0 };
        TMI_Product_t           &TmiProduct                 = TmiSoldProduct.product; 
        TMI_ProductDetails_t   *pTmiProductDetails          = &TmiProduct.productDetails;
        TMI_Payment_t           TmiCashPayment              = { TMI_PAYMENT_TYPE_NONE };
        time_t                  transactionDateTime         = (time_t)pData->DynamicData.currentDateTime;
        char                    CurrencyCode[ ]             = "AUD";
        U8_t                    vatRate                     = 0;

        #if     __TOBEDELETED__
        if ( MYKI_CD_getGSTPercentage( &vatRate ) == FALSE )
        {
            CsErrx( "CreatePassengerCountTmi : MYKI_CD_getGSTPercentage() failed, set zero vat_rate" );
            vatRate     = 0;
        }
        #endif

                TmiTrx.timestamp                            = transactionDateTime;
        strcpy( TmiTrx.status,                                "OK" );
                TmiTrx.complaint                            = false;
        strcpy( TmiTrx.currency,                              CurrencyCode );
                TmiTrx.value                                = 0;
                TmiTrx.serviceProviderId                    = (int)pData->StaticData.serviceProviderId;
                TmiTrx.entryPointId                         = pData->DynamicData.entryPointId;
                TmiSoldProduct.itemsToIssue                 = 1;
                TmiSoldProduct.itemsIssued                  = 1;
                TmiSoldProduct.basePrice                    = 0;
                TmiProduct.productId                        = PASSENGER_COUNT_PRODUCT_ID;
                TmiProduct.tariffVersion.majorVersion       = MYKI_CD_getMajorVersion( );
                TmiProduct.tariffVersion.minorVersion       = MYKI_CD_getMinorVersion( );
                TmiProduct.saleDate                         = transactionDateTime;
        strcpy( TmiProduct.currency,                          CurrencyCode );
                TmiProduct.vatRate                          = (int)vatRate;
                TmiProduct.price                            = 0;
                pTmiProductDetails->productOwner            = PRODUCT_OWNER_DEFAULT;
        strcpy( pTmiProductDetails->fareIndex,                isConcession == true ? "Concession" : "Full Fare" );
                pTmiProductDetails->originId                = pData->DynamicData.stopId;
                pTmiProductDetails->destinationId           = pData->DynamicData.stopId;
                pTmiProductDetails->route                   = pData->DynamicData.lineId;
                pTmiProductDetails->stopId                  = pData->DynamicData.stopId;
                pTmiProductDetails->cartItemNumber          = 1;
        strcpy( pTmiProductDetails->description,              isConcession == true ? "Concession Pass" : "Full Fare Pass" );
                TmiCashPayment.paymentType                  = TMI_PAYMENT_TYPE_CASH;
        strcpy( TmiCashPayment.currency,                      CurrencyCode );
                TmiCashPayment.accepted                     = 0;
        /*      TmiCashPayment.changed                      = 0;
                TmiCashPayment.refunded                     = 0;
                TmiCashPayment.invalid                      = 0;    */

        result  = TMI_createPassengerCountTrx( &TmiTrx, &TmiSoldProduct, &TmiCashPayment );
        if ( result < 0 )
        {
            CsErrx( "CreatePassengerCountTmi : TMI_createPassengerCountTrx failed (%d)", result );
            result  = (-1);
        }
        else
        {
            CsDbg( APP_DEBUG_DETAIL, "CreatePassengerCountTmi : TMI_createPassengerCountTrx successful" );

            /*  Update totals */
            if ( isConcession == false )
            {
                int tripFullPassCount           = IncrementCounterBy( "tripFullPassCount",    1 );
                int portionFullPassCount        = IncrementCounterBy( "portionFullPassCount", 1 );
                int shiftFullPassCount          = IncrementCounterBy( "shiftFullPassCount",   1 );

                CsDbg( APP_DEBUG_DETAIL, "CreatePassengerCountTmi : FullPassCount=%d/%d/%d",
                        tripFullPassCount, portionFullPassCount, shiftFullPassCount );
            }
            else
            {
                int tripConcessionPassCount     = IncrementCounterBy( "tripConcessionPassCount",    1 );
                int portionConcessionPassCount  = IncrementCounterBy( "portionConcessionPassCount", 1 );
                int shiftConcessionPassCount    = IncrementCounterBy( "shiftConcessionPassCount",   1 );

                CsDbg( APP_DEBUG_DETAIL, "CreatePassengerCountTmi : ConcessionPassCount=%d/%d/%d",
                    tripConcessionPassCount, portionConcessionPassCount, shiftConcessionPassCount );
            }

            /*  Persists ALL counters */
            if ( ( result = TMI_saveCounters( ) ) < 0 )
            {
                CsErrx( "CreatePassengerCountTmi : TMI_saveCounters() failed (%d)", result );
            }

            result  = 0;
        }
    }

    return  result;
}

    /**
     *  @brief  Creates Shift and ShiftOpen TMI records.
     *  @param  pData BR context data.
     *  @return 0 if successful; else failed.
     */
int
CreateShiftOpenTmi( MYKI_BR_ContextData_t *pData )
{
    int         result              = 0;

    if ( g_iniGenerateTmi != false )
    {
        TMI_Trx_t               TmiTrx                      = { 0 };
        TMI_Shift_t             TmiShift                    = { 0 };
        CsTime_t                now                         = { 0 };
        std::string             driverId                    = getUserId().get();
        Time_t                  shiftOpenDateTime           = TIME_NOT_SET;
        unsigned int            shiftReference              = 0;
        const char             *pKey                        = NULL;

        if ( driverId == g_defaultDriverId )
        {
            driverId = GetDefaultUserId();
        }

        CsTime( &now );

        /*  Saves shift start date/time */
        shiftOpenDateTime                       = (Time_t)now.tm_secs;
        if ( ( result = TMI_setCounter( ( pKey = "shiftOpenDateTime" ), (unsigned int)shiftOpenDateTime ) ) < 0 )
        {
            CsErrx( "CreateShiftOpenTmi : TMI_setCounter('%s') failed (%d)", pKey, result );
        }

        TmiShift.timestamp = (time_t)now.tm_secs;
        TmiShift.shiftOpenTimestamp = (time_t)shiftOpenDateTime;
        strncpy( TmiShift.userId, driverId.c_str(), sizeof( TmiShift.userId ) );
        TmiShift.serviceProviderId = pData->StaticData.serviceProviderId;

        TmiTrx.timestamp = now.tm_secs;
        strncpy(TmiTrx.status, "OK", TMI_STATUS_LEN);
        TmiTrx.complaint = false;
        strncpy(TmiTrx.currency, "AUD", TMI_CURRENCY_LEN);
        TmiTrx.value = 0;
        TmiTrx.serviceProviderId = pData->StaticData.serviceProviderId;
        TmiTrx.entryPointId = pData->DynamicData.entryPointId;

        if ( ( result = TMI_incrementCounter( ( pKey = "shiftReference" ), &shiftReference ) ) < 0 )
        {
            CsErrx( "CreateShiftOpenTmi : TMI_incrementCounter('%s') failed (%d)", pKey, result );
        }
        else if ( ( result = TMI_clearCounter( pKey = "shiftRecordCount" ) ) < 0 )
        {
            CsErrx( "CreateShiftOpenTmi : TMI_clearCounter('%s') failed (%d)", pKey, result );
        }

        if ( ( result = TMI_createShiftTrx( &TmiShift ) ) < 0 )
        {
            CsErrx( "CreateShiftOpenTmi : TMI_createShiftTrx failed (%d)", result );
        }
        else if ( ( result = TMI_createShiftOpenTrx( &TmiShift ) ) < 0 )
        {
            CsErrx( "CreateShiftOpenTmi : TMI_createShiftOpenTrx failed (%d)", result );
        }
        else if ( ( result = TMI_createTrayNumberTrx( &TmiTrx ) ) < 0 )
        {
            CsErrx( "CreateShiftOpenTmi: TMI_createTrayNumberTrx failed (%d)", result);
        }
        else
        {
            CsDbg( APP_DEBUG_DETAIL, "CreateShiftOpenTmi : TMI_createShiftOpenTrx successful" );
            result  = 0;
        }
    }
    return  result;
}

    /**
     *  @brief  Creates ShiftClose TMI record.
     *  @param  pData BR context data.
     *  @return 0 if successful; else failed.
     */
int
CreateShiftCloseTmi( MYKI_BR_ContextData_t *pData )
{
    int         result              = 0;

    if ( g_iniGenerateTmi != false )
    {
        TMI_Shift_t             TmiShift                    = { 0 };
        TMI_Payment_t           TmiCashPayment              = { TMI_PAYMENT_TYPE_NONE };
        TMI_Payment_t           TmiTPursePayment            = { TMI_PAYMENT_TYPE_NONE };
        std::string             driverId                    = getUserId().get();
        Time_t                  shiftOpenDateTime           = (Time_t)GetCounter( "shiftOpenDateTime"         );
        int                     portionGrossCashAmount      =    (int)GetCounter( "portionGrossCashAmount"    );
        int                     portionAnnulledCashAmount   =    (int)GetCounter( "portionAnnulledCashAmount" );
        int                     portionOverpaidCashAmount   =    (int)GetCounter( "portionOverpaidCashAmount" );
        unsigned int            portionNetCashAmount        = 0;
        CsTime_t                now                         = { 0 };
        char                    CurrencyCode[ ]             = "AUD";
        int                     result                      = 0;
        const char             *pKey                        = NULL;

        if ( shiftOpenDateTime == TIME_NOT_SET )
        {
            /*  No currently active shift */
            CsWarnx( "CreateShiftCloseTmi : shift open date/time not set" );
            return  0;
        }

        CsTime( &now );

        if ( driverId == g_defaultDriverId )
        {
            driverId = GetDefaultUserId();
        }
        else
        {
            TMI_Remittance_t    TmiRemittance   = { 0 };

            if ( portionGrossCashAmount < portionAnnulledCashAmount )
            {
                /*  Corruption!? */
                CsErrx( "CreateShiftCloseTmi : Shift cash gross (%d) less than annul (%d)",
                        portionGrossCashAmount, portionAnnulledCashAmount );
            }
            else
            {
                portionNetCashAmount    = ( portionGrossCashAmount - portionAnnulledCashAmount );
            }

                    TmiRemittance.timestamp             = now.tm_secs;
           strncpy( TmiRemittance.userId,                 driverId.c_str(), sizeof( TmiRemittance.userId ) );
            strcpy( TmiRemittance.operation,              "-" );
                    TmiRemittance.paymentType           = TMI_PAYMENT_TYPE_CASH;
            strcpy( TmiRemittance.currency,               "AUD" );
                    TmiRemittance.amount                = portionNetCashAmount;
                    TmiRemittance.serviceProviderId     = pData->StaticData.serviceProviderId;

            result  = TMI_createRemittanceTrx( &TmiRemittance );
            if ( result < 0 )
            {
                CsErrx( "CreateShiftCloseTmi : TMI_createRemittanceTrx failed (%d)", result );
                result  = (-1);
            }
        }

                TmiShift.timestamp                  = now.tm_secs;
                TmiShift.shiftOpenTimestamp         = (time_t)( shiftOpenDateTime != TIME_NOT_SET ? shiftOpenDateTime : now.tm_secs );
       strncpy( TmiShift.userId,                      driverId.c_str(), sizeof( TmiShift.userId ) );
                TmiShift.serviceProviderId          = pData->StaticData.serviceProviderId;
                TmiCashPayment.paymentType          = TMI_PAYMENT_TYPE_CASH;
        strcpy( TmiCashPayment.currency,              CurrencyCode );
                TmiCashPayment.accepted             = portionGrossCashAmount;
                TmiCashPayment.overpaid             = portionOverpaidCashAmount;
        /*      TmiCashPayment.changed              */
                TmiCashPayment.refunded             = portionAnnulledCashAmount;
        /*      TmiCashPayment.invalid              */
                TmiTPursePayment.paymentType        = TMI_PAYMENT_TYPE_TPURSE;
        strcpy( TmiTPursePayment.currency,            CurrencyCode );
                TmiTPursePayment.accepted           = GetCounter( "portionGrossOtherAmount" );
        /*      TmiTPursePayment.overpaid
                TmiTPursePayment.changed            */
                TmiTPursePayment.refunded           = GetCounter( "portionAnnulledOtherAmount" );
        /*      TmiTPursePayment.invalid            */

        result  = TMI_createShiftCloseTrx( &TmiShift, &TmiCashPayment, &TmiTPursePayment );
        if ( result < 0 )
        {
            CsErrx( "CreateShiftCloseTmi : TMI_createShiftCloseTrx failed (%d)", result );
            result  = (-1);
        }
        else
        {
            CsDbg( APP_DEBUG_DETAIL, "CreateShiftCloseTmi : TMI_createShiftCloseTrx successful" );
            if
            (
                ( result = TMI_clearCounter( pKey = "shiftRecordCount"  ) ) < 0 ||
                ( result = TMI_clearCounter( pKey = "shiftOpenDateTime" ) ) < 0
            )
            {
                CsErrx( "CreateShiftCloseTmi : TMI_clearCounter('%s') failed (%d)", pKey, result );
            }
            result  = 0;
        }
    }
    return  result;
}

    /** 
     *  @brief  Creates enter/depart stop event log TMI record.
     *  @param  pData business rule context data.
     *  @param  evLogType event log type.
     *  @param  routeId route id.
     *  @param  stopId stop id.
     *  @param  gpsAvailable true if GPS coordinates are valid; false otherwise.
     *  @param  latitude GPS latitude.
     *  @param  longitude GPS longitude.
     *  @param  noStopDuration no stop detected duration (seconds), only
     *          applicable for EvLogType_NoStopDetected event type.
     */
int
CreateEvStop(
    MYKI_BR_ContextData_t  *pData,
    int                     evLogType,
    int                     routeId,
    int                     stopId,
    const char             *pStopName,
    bool                    gpsAvailable,
    double                  latitude,
    double                  longitude,
    int                     noStopDuration )
{
    int                     result              = 0;

    if ( g_iniGenerateTmi != false )
    {
        TMI_Trx_t               TmiTrx          = { 0 };
        TMI_NoStopDetection_t   TmiNoStop       = { { 0 } };
        TMI_Stop_t             *TmiStop         = &TmiNoStop.stop;
        CsTime_t                csTime          = { 0 };

        CsTime( &csTime );

                TmiTrx.timestamp                = (time_t)csTime.tm_secs;
        strcpy( TmiTrx.status,                    "OK" );
                TmiTrx.complaint                = false;
        strcpy( TmiTrx.currency,                  "AUD" );
                TmiTrx.value                    = 0;
                TmiTrx.serviceProviderId        = pData->StaticData.serviceProviderId;
                TmiTrx.entryPointId             = pData->DynamicData.entryPointId;
                TmiStop->route                  = routeId;
                TmiStop->stopId                 = stopId;
                TmiStop->latitude               = latitude;
                TmiStop->longitude              = longitude;
                TmiStop->showCoordinates        = gpsAvailable;
       strncpy( TmiStop->stopName,                pStopName, sizeof( TmiStop->stopName ) );

        switch ( evLogType )
        {
        case    EvLogType_EnterStop:            result  = TMI_createEnterStopTrx(       &TmiTrx, TmiStop    );  break;
        case    EvLogType_DepartFirstStop:      result  = TMI_createDepartFirstStopTrx( &TmiTrx, TmiStop    );  break;
        case    EvLogType_EnterLastStop:        result  = TMI_createEnterLastStopTrx(   &TmiTrx, TmiStop    );  break;
        case    EvLogType_DepartStop:           result  = TMI_createDepartStopTrx(      &TmiTrx, TmiStop    );  break;
        case    EvLogType_ManualStop:           result  = TMI_createManualStopTrx(      &TmiTrx, TmiStop    );  break;
        case    EvLogType_Periodic:             result  = TMI_createPeriodicStopTrx(    &TmiTrx, TmiStop    );  break;
        case    EvLogType_NoStopDetected:       TmiNoStop.actualDurationExceeded    = noStopDuration;
                                                result = TMI_createNoStopDetectionTrx(  &TmiTrx, &TmiNoStop );  break;
        default:
            CsErrx( "CreateEvStop : unexpected event type (%d)", evLogType );
            result  = -1;
            break;
        }
    }
    return  result;
}

    /**
     *  @brief  Creates trip/route selection event log TMI record.
     *  @param  pData business rule context data.
     *  @param  tripId trip id.
     *  @param  tripStartTime trip start time (seconds since midnight)
     *  @param  routeId route id.
     *  @param  firstStopId first stop id.
     *  @param  lastStopId last stop id.
     *  @return 0 if successful; -1 otherwise.
     */
int
CreateTripRouteSelection(
    MYKI_BR_ContextData_t  *pData,
    int                     evLogType,
    int                     shiftId,
    int                     tripId,
    int                     tripStartTime,
    int                     routeId,
    int                     firstStopId,
    int                     lastStopId )
{
    int                         result                          = 0;

    if ( g_iniGenerateTmi != false )
    {
        TMI_Trx_t               TmiTrx                          = { 0 };
        TMI_Selection_t         TmiSelection                    = { 0 };
        CsTime_t                csTime                          = { 0 };
        std::string             driverId                        = getUserId().get();

        if ( driverId == g_defaultDriverId )
        {
            driverId = GetDefaultUserId();
        }

        CsTime( &csTime );

                TmiTrx.timestamp                = (time_t)csTime.tm_secs;
        strcpy( TmiTrx.status,                    "OK" );
                TmiTrx.complaint                = false;
        strcpy( TmiTrx.currency,                  "AUD" );
                TmiTrx.value                    = 0;
                TmiTrx.serviceProviderId        = pData->StaticData.serviceProviderId;
                TmiTrx.entryPointId             = pData->DynamicData.entryPointId;
                TmiSelection.routeCode          = routeId;
                TmiSelection.firstStopId        = firstStopId;
                TmiSelection.endStopId          = lastStopId;
        strcpy( TmiSelection.staffId,             driverId.c_str() );
                TmiSelection.startTimeMins      = tripStartTime;
                TmiSelection.tripNumber         = tripId;
                TmiSelection.shiftId            = shiftId;
                TmiSelection.activeEnd          = TMI_ACTIVE_END_PRIMARY;   /*  TODO: Determine active end for TDC */
        switch ( evLogType )
        {
        case    EvLogType_TripSelection:
            result  = TMI_createTripSelectionTrx( &TmiTrx, &TmiSelection );
            break;

        case    EvLogType_RouteSelection:
            result  = TMI_createRouteSelectionTrx( &TmiTrx, &TmiSelection );
            break;

        case    EvLogType_HeadlessOperation:
            TmiSelection.activeEnd              = TMI_ACTIVE_END_NONE;
            result  = TMI_createHeadlessOperationTrx( &TmiTrx, &TmiSelection );
            break;

        default:
            CsErrx( "CreateTripRouteSelection : unexpected event type (%d)", evLogType );
            result  = -1;
            break;
        }
    }
    return  result;
}

    /**
     *  @brief  Batches generated TMI records (if any), conditionally performs
     *          operational period commit and generates operational period log.
     *  @param  pData business rule context data.
     *  @return 0 if successful; otherwise failed.
     */
int
CreateOpLog( MYKI_BR_ContextData_t *pData )
{
    if (!g_iniGenerateTmi)
    {
        CsDbg(APP_DEBUG_DETAIL, "createOpLog: g_iniGenerateTmi = false, so returning immediately.");
        return 0;
    }

    TMI_OpLog_t opLog = {};

    strncpy(opLog.userId, getUserId().get().c_str(), TMI_USERID_LEN);
    strncpy(opLog.currency, "AUD", TMI_CURRENCY_LEN);

    //opLog.timestamp = pData->DynamicData.currentDateTime;

    //if (opLog.timestamp == 0)
    {
        opLog.timestamp = time(NULL);
    }

    int result = 0;

    if ((result = TMI_createOpLog(&opLog)) < 0)
    {
        CsErrx("CreateOpLog: TMI_createOpLog() failed (%d)", result);
        return -1;
    }

    return 0;
}

    /**
     *  @brief  Checks and (if required) performs operational period commit.
     *  @return 0 if successful; otherwise failed.
     */
int
CheckAndCommitPeriod( MYKI_BR_ContextData_t *pData )
{
    int                     result          = 0;
    int                     isWithinPeriod  = TRUE;
    std::string             currDriverId    = getUserId().get();

    if ( currDriverId != g_defaultDriverId )
    {
        /*  Driver currently logged in => can not perform operational period commit */
        CsDbg( APP_DEBUG_DETAIL, "CheckAndCommitPeriod : Driver (%s) logged in", currDriverId.c_str() );
    }
    else
    if ( ( result = TMI_isWithinPeriod( &isWithinPeriod, (time_t)0 /* NOW */ ) ) != 0 )
    {
        CsErrx( "CheckAndCommitPeriod : TMI_isWithinPeriod() failed (%d)", result );
    }
    else
    if ( isWithinPeriod != FALSE )
    {
        /*  Still within the current operational period */
        CsDbg( APP_DEBUG_DETAIL, "CheckAndCommitPeriod : still within current operational period" );
    }
    else
    {
        /*  New operational period started, closes headless shift */
        CreateShiftCloseTmi( pData );

        /*  Performs operational period commit */
        CreateOpLog( pData );

        /*  And restarts headless shift */
        CreateShiftOpenTmi( pData );
    }

    return  0;
}

    /**
     *  @brief  Creates product sales TMI record.
     *  @param  pData business rule context data.
     *  @param  productSaleDetails product sales details.
     *  @return 0 if successful; otherwise failed.
     */
int
CreateProductSalesTmi( MYKI_BR_ContextData_t *pData, Json::Value &productSaleDetails )
{
    int                         result                  = 0;

    if ( g_iniGenerateTmi != false )
    {
        TMI_Trx_t               TmiTrx                  = { 0 };
        TMI_Payment_t           TmiPayment              = { TMI_PAYMENT_TYPE_NONE };
        TMI_TPurseDetails_t     TmiTPurseDetails        = { { 0 } };
        TMI_TPurseDetails_t    *pTmiTPurseDetails       = NULL;
        TMI_Ldt_t               TmiLdt                  = { { 0 } };
        TMI_Ldt_t              *pTmiLdt                 = NULL;
        int                     itemsToIssue            = productSaleDetails[ "data" ][ "cartitems" ].size();
        int                     itemsIssued             = 0;
        time_t                  transactionDateTime     = (time_t)pData->DynamicData.currentDateTime;
/*      Currency_t              amountTotal             = getJsonInt( productSaleDetails[ "data" ], "amounttotal"    ); */
        Currency_t              amountRounding          = getJsonInt( productSaleDetails[ "data" ], "amountrounding" );
        Currency_t              amountPaid              = getJsonInt( productSaleDetails[ "data" ], "amountpaid"     );
        char                    CurrencyCode[ ]         = "AUD";
        int                     countNonTransit         = 0;
        int                     countSurcharge          = 0;
        int                     countTpurseLoad         = 0;
        std::string             paymentTypeStr          = getJsonString( productSaleDetails[ "data" ], "paymenttype" );
        TMI_PaymentType_e       paymentType             = strToPaymentType( paymentTypeStr );
        char                   *pLdt                    = NULL;
        char                    ldtPath[ CSMAXPATHLEN ] = { 0 };
        bool                    bLdtConsumed            = false;
        char                    ldtName[ 64 ]           = { 0 };

        if ( paymentType == TMI_PAYMENT_TYPE_NONE )
        {
            CsErrx( "CreateProductSalesTmi : invalid '%s' payment type", paymentTypeStr.c_str() );
            result  = -1;
        }
        else
        if ( itemsToIssue == 0 )
        {
            CsErrx( "CreateProductSalesTmi : no product to issue" );
            result  = -2;
        }
        else
        if ( itemsToIssue > MAX_PRODUCT_SALES_ITEMS )
        {
            CsErrx( "CreateProductSalesTmi : too many products(%d)", itemsToIssue );
            result  = -3;
        }
        else
        {
            TMI_SoldProduct_t       TmiSoldProducts[ itemsToIssue ];
            U8_t                    vatRate             = 0;

            /*  Retrieves transaction generated LDT (if any) */
            pLdt    = GetTransactionLDT( ldtPath, sizeof( ldtPath ), transactionDateTime, false );

            if ( MYKI_CD_getGSTPercentage( &vatRate ) == FALSE )
            {
                CsErrx( "CreateProductSalesTmi : MYKI_CD_getGSTPercentage() failed, set zero vat_rate" );
                vatRate     = 0;
            }

            for ( Json::ValueIterator itr = productSaleDetails[ "data" ][ "cartitems" ].begin();
                    itr != productSaleDetails[ "data" ][ "cartitems" ].end();
                    itr++ )
            {
                Json::Value            &cartItem                = ( *itr );
                TMI_SoldProduct_t      *pTmiSoldProduct         = &TmiSoldProducts[ itemsIssued++ ];
                TMI_Product_t          *pTmiProduct             = &pTmiSoldProduct->product;
                TMI_ProductDetails_t   *pTmiProductDetails      = &pTmiProduct->productDetails;
                std::string             productType             = getJsonString( cartItem, "type"       );
                std::string             productShortDesc        = getJsonString( cartItem, "short_desc" );
                bool                    isTPurseLoad            = productType == "LLSC" ? true : false;
                Currency_t              price                   = getJsonInt(    cartItem, "Price"      );

                memset( pTmiSoldProduct, 0, sizeof( TMI_SoldProduct_t ) );
                        pTmiSoldProduct->itemsToIssue           = 1;    /*  itemsToIssue; */
                        pTmiSoldProduct->itemsIssued            = 1;    /*  getJsonInt(  cartItem, "itemnumber"    ); */
                        pTmiSoldProduct->basePrice              = price;
                        pTmiProduct->productId                  = getJsonInt(  cartItem, "id"            );
                        pTmiProduct->tariffVersion.majorVersion = MYKI_CD_getMajorVersion( );
                        pTmiProduct->tariffVersion.minorVersion = MYKI_CD_getMinorVersion( );
                        pTmiProduct->saleDate                   = pData->DynamicData.currentDateTime;
                strcpy( pTmiProduct->currency,                    CurrencyCode );
                        pTmiProduct->vatRate                    = getJsonBool( cartItem, "GSTApplicable" ) == true ? vatRate : 0;
                        pTmiProduct->price                      = price;
                        pTmiProductDetails->productOwner        = getJsonInt(  cartItem, "issuer_id"     );
               strncpy( pTmiProductDetails->fareIndex,            isTPurseLoad == true ? "flat" : productShortDesc.c_str(), BUFSIZ );
                        pTmiProductDetails->originId            = pData->DynamicData.stopId;
                        pTmiProductDetails->destinationId       = pData->DynamicData.stopId;
                        pTmiProductDetails->route               = pData->DynamicData.lineId;
                        pTmiProductDetails->stopId              = pData->DynamicData.stopId;
                        pTmiProductDetails->cartItemNumber      = getJsonInt(  cartItem, "itemnumber"    );
               strncpy( pTmiProductDetails->description,          productShortDesc.c_str(), BUFSIZ );
                if ( pLdt != NULL )
                {
                    int transactionType                         = paymentType == TMI_PAYMENT_TYPE_CASH ?
                                                                        TransactionType_TPurseTopupPaidCash :
                                                                        TransactionType_ProductSalesPaidTPurse;
                    pTmiLdt                                     = &TmiLdt;
                    strcpy( TmiLdt.name,                          GetLdtName( ldtName, sizeof( ldtName ), transactionType ) );
                    strcpy( TmiLdt.encoding,                      "UTF8" );
                            TmiLdt.ldt                          = pLdt;
                }

                if ( itemsIssued > itemsToIssue ||
                     pTmiProductDetails->cartItemNumber > itemsToIssue )
                {
                    CsErrx( "CreateProductSalesTmi : itemsIssued(%d/%d) > itemsToIssue(%d)",
                            itemsIssued, pTmiProductDetails->cartItemNumber, itemsToIssue );
                    result  = -4;
                    break;
                }
                if      ( productType == "ThirdParty"       ) { countNonTransit++;  }
                else if ( productType == "PremiumSurcharge" ) { countSurcharge++;   }
                else if ( productType == "LLSC"             ) { countTpurseLoad++;  }
                else
                {
                    CsErrx( "CreateProductSalesTmi : unsupported '%s' product type", productType.c_str() );
                    result  = -5;
                }
            }

            if ( result == 0 )
            {
                        TmiTrx.timestamp                            = transactionDateTime;
                strcpy( TmiTrx.status,                                "OK" );
                        TmiTrx.complaint                            = false;
                strcpy( TmiTrx.currency,                              CurrencyCode );
                        TmiTrx.value                                = amountPaid;
                        TmiTrx.serviceProviderId                    = (int)pData->StaticData.serviceProviderId;
                        TmiTrx.entryPointId                         = pData->DynamicData.entryPointId;
                        TmiPayment.paymentType                      = paymentType;
                strcpy( TmiPayment.currency,                          CurrencyCode );
                        TmiPayment.accepted                         = (int)amountPaid;
                        TmiPayment.overpaid                         = (int)amountRounding;
                /*      TmiPayment.changed                          = 0;
                        TmiPayment.refunded                         = 0;
                        TmiPayment.invalid                          = 0;    */
                if ( paymentType == TMI_PAYMENT_TYPE_TPURSE )
                {
                    std::string         mykiPAN                     = getMykiPAN( );
                    std::string         cardIssuerId                = mykiPAN.substr( 0, 6 );
                    std::string         cardSerialNumber            = mykiPAN.substr( 6, 8 );
                    std::string         mediaId                     = getSerialNumber( );
                    TMI_Application_t  *pTmiApplication             = &TmiTPurseDetails.application;

                    pTmiTPurseDetails                               = &TmiTPurseDetails;
                   strncpy( pTmiTPurseDetails->mediaId,               mediaId.c_str(), TMI_MEDIA_ID_LEN );
                    strcpy( pTmiTPurseDetails->mediaType,             "LLSC" );
                   strncpy( pTmiTPurseDetails->cardIssuerId,          cardIssuerId.c_str(), TMI_CARD_ISSUER_ID_LEN );
                   strncpy( pTmiTPurseDetails->cardSerialNumber,      cardSerialNumber.c_str(), TMI_CARD_CSN_LEN );
                    strcpy( pTmiApplication->applicationId,           "transit" );
                    strcpy( pTmiApplication->currency,                CurrencyCode );
                            pTmiApplication->remainingStoredValue   = getJsonInt( productSaleDetails[ "data" ], "remvalue" );

                    #if     OPT_EXCLUDE_TPurseDetails
                    {
                        /*  NOTE:   Inclusion of <TPurseDetails> element causing ARCOS to fail importing
                                    transactions paid by TPurse */
                        CsDbg( APP_DEBUG_STATE, "CreateProductSalesTmi : Excluded <TPurseDetails> element" );
                        pTmiTPurseDetails                           = NULL;
                    }
                    #endif
                }
                if ( ( result = TMI_createNonTransitPurchaseTrx( &TmiTrx,
                                    &TmiSoldProducts[ 0 ], (size_t)itemsToIssue, &TmiPayment, pTmiLdt, pTmiTPurseDetails ) ) < 0 )
                {
                    CsErrx( "CreateProductSalesTmi : TMI_createNonTransitPurchaseTrx() failed (%d)", result );
                    result  = -6;
                }
                else
                {
                    CsErrx( "CreateProductSalesTmi : TMI_createNonTransitPurchaseTrx() successful" );
                    bLdtConsumed    = true;
                    result          = 0;

                    /*  Updates totals */
                    {
                        int     totalCount                  = ( countNonTransit + countSurcharge + countTpurseLoad );
                        int     tripGrossSundryTickets      = IncrementCounterBy( "tripGrossSundryTickets",     totalCount      );
                        int     portionGrossSundryTickets   = IncrementCounterBy( "portionGrossSundryTickets",  totalCount      );
                        int     shiftGrossSundryTickets     = IncrementCounterBy( "shiftGrossSundryTickets",    totalCount      );
                        if ( paymentType == TMI_PAYMENT_TYPE_CASH )
                        {
                            int tripGrossCashAmount         = IncrementCounterBy( "tripGrossCashAmount",        amountPaid      );
                            int tripOverpaidCashAmount      = IncrementCounterBy( "tripOverpaidCashAmount",     amountRounding  );
                            int tripCashCount               = IncrementCounterBy( "tripCashCount",              totalCount      );
                            int portionGrossCashAmount      = IncrementCounterBy( "portionGrossCashAmount",     amountPaid      );
                            int portionOverpaidCashAmount   = IncrementCounterBy( "portionOverpaidCashAmount",  amountRounding  );
                            int portionCashCount            = IncrementCounterBy( "portionCashCount",           totalCount      );
                            int shiftGrossCashAmount        = IncrementCounterBy( "shiftGrossCashAmount",       amountPaid      );
                            int shiftOverpaidCashAmount     = IncrementCounterBy( "shiftOverpaidCashAmount",    amountRounding  );
                            int shiftCashCount              = IncrementCounterBy( "shiftCashCount",             totalCount      );
                            int periodGrossCashAmount       = IncrementCounterBy( "periodGrossCashAmount",      amountPaid      );

                            CsDbg( APP_DEBUG_DETAIL, "CreateProductSalesTmi : Cash (%d:%d:%d:%d)/(%d:%d:%d:%d)/(%d:%d:%d:%d)/(%d)",
                                    tripCashCount,      tripGrossCashAmount,        tripOverpaidCashAmount,     tripGrossSundryTickets,
                                    portionCashCount,   portionGrossCashAmount,     portionOverpaidCashAmount,  portionGrossSundryTickets,
                                    shiftCashCount,     shiftGrossCashAmount,       shiftOverpaidCashAmount,    shiftGrossSundryTickets,
                                    periodGrossCashAmount );
                        }
                        else
                        {
                            int tripGrossOtherAmount        = IncrementCounterBy( "tripGrossOtherAmount",       amountPaid      );
                            int tripOtherCount              = IncrementCounterBy( "tripOtherCount",             totalCount      );
                            int portionGrossOtherAmount     = IncrementCounterBy( "portionGrossOtherAmount",    amountPaid      );
                            int portionOtherCount           = IncrementCounterBy( "portionOtherCount",          totalCount      );
                            int shiftGrossOtherAmount       = IncrementCounterBy( "shiftGrossOtherAmount",      amountPaid      );
                            int shiftOtherCount             = IncrementCounterBy( "shiftOtherCount",            totalCount      );

                            CsDbg( APP_DEBUG_DETAIL, "CreateProductSalesTmi : Other (%d:%d:%d)/(%d:%d:%d)/(%d:%d:%d)",
                                    tripOtherCount,     tripGrossOtherAmount,       tripGrossSundryTickets,
                                    portionOtherCount,  portionGrossOtherAmount,    portionGrossSundryTickets,
                                    shiftOtherCount,    shiftGrossOtherAmount,      shiftGrossSundryTickets );
                        }

                        /*  Persists ALL counters */
                        if ( ( result = TMI_saveCounters( ) ) < 0 )
                        {
                            CsErrx( "CreateProductSalesTmi : TMI_saveCounters() failed (%d)", result );
                        }
                    }
                }
            }

            if ( pLdt != NULL )
            {
                /*  NOTE:   LDT buffer allocated in GetTransactionLDT() */
                CsFree( pLdt );

                /*  And removes LDT if consumed */
                if ( bLdtConsumed == true )
                {
                    unlink( ldtPath );
                }
            }
        }
    }
    return  result;
}   /*  CreateProductSalesTmi( ) */

    /**
     *  @brief  Creates product sales reversal TMI record.
     *  @param  pData business rule context data.
     *  @param  productSaleReversalDetails product sales reversal details.
     *  @return 0 if successful; otherwise failed.
     */
int
CreateProductSalesReversalTmi( MYKI_BR_ContextData_t *pData, Json::Value &productSaleReversalDetails )
{
    int                         result                  = 0;

    if ( g_iniGenerateTmi != false )
    {
        TMI_Trx_t               TmiTrx                  = { 0 };
        TMI_Payment_t           TmiPayment              = { TMI_PAYMENT_TYPE_NONE };
        TMI_TPurseDetails_t     TmiTPurseDetails        = { { 0 } };
        TMI_TPurseDetails_t    *pTmiTPurseDetails       = NULL;
        TMI_Ldt_t               TmiLdt                  = { { 0 } };
        TMI_Ldt_t              *pTmiLdt                 = NULL;
        int                     itemsToRefund           = productSaleReversalDetails[ "data" ][ "cartitems" ].size();
        int                     itemsRefunded           = 0;
        time_t                  transactionDateTime     = (time_t)pData->DynamicData.currentDateTime;
/*      Currency_t              amountTotal             = getJsonInt( productSaleReversalDetails[ "data" ], "amounttotal"    ); */
        Currency_t              amountRounding          = getJsonInt( productSaleReversalDetails[ "data" ], "amountrounding" );
        Currency_t              amountOverpaid          = 0 - amountRounding;
        Currency_t              amountRefunded          = getJsonInt( productSaleReversalDetails[ "data" ], "amountrefunded" );
        char                    CurrencyCode[ ]         = "AUD";
        int                     countNonTransit         = 0;
        int                     countSurcharge          = 0;
        int                     countTpurseLoad         = 0;
        std::string             paymentTypeStr          = getJsonString( productSaleReversalDetails[ "data" ], "paymenttype" );
        TMI_PaymentType_e       paymentType             = strToPaymentType( paymentTypeStr );
        std::string             saleDateStr             = getJsonString( productSaleReversalDetails[ "data" ], "saletimestamp" );
        time_t                  saleDate                = isoStrToTime( saleDateStr );
        int                     routeId                 = getJsonInt( productSaleReversalDetails[ "data" ], "route_id" );
        int                     stopId                  = getJsonInt( productSaleReversalDetails[ "data" ], "stop_id"  );
        char                   *pLdt                    = NULL;
        char                    ldtPath[ CSMAXPATHLEN ] = { 0 };
        bool                    bLdtConsumed            = false;
        char                    ldtName[ 64 ]           = { 0 };

        if ( paymentType == TMI_PAYMENT_TYPE_NONE )
        {
            CsErrx( "CreateProductSalesReversalTmi : invalid '%s' payment type", paymentTypeStr.c_str() );
            result  = -1;
        }
        else
        if ( itemsToRefund == 0 )
        {
            CsErrx( "CreateProductSalesReversalTmi : no product to issue" );
            result  = -2;
        }
        else
        if ( itemsToRefund > MAX_PRODUCT_SALES_ITEMS )
        {
            CsErrx( "CreateProductSalesReversalTmi : too many products(%d)", itemsToRefund );
            result  = -3;
        }
        else
        {
            TMI_RefundedProduct_t   TmiRefundedProducts[ itemsToRefund ];
            U8_t                    vatRate             = 0;

            if ( routeId == 0 ) routeId = pData->DynamicData.lineId;
            if ( stopId  == 0 ) stopId  = pData->DynamicData.stopId;

            /*  Retrieves transaction generated LDT (if any) */
            pLdt    = GetTransactionLDT( ldtPath, sizeof( ldtPath ), transactionDateTime, false );

            if ( MYKI_CD_getGSTPercentage( &vatRate ) == FALSE )
            {
                CsErrx( "CreateProductSalesReversalTmi : MYKI_CD_getGSTPercentage() failed, set zero vat_rate" );
                vatRate     = 0;
            }

            for ( Json::ValueIterator itr = productSaleReversalDetails[ "data" ][ "cartitems" ].begin();
                    itr != productSaleReversalDetails[ "data" ][ "cartitems" ].end();
                    itr++ )
            {
                Json::Value            &cartItem                = ( *itr );
                TMI_RefundedProduct_t  *pTmiRefundedProduct     = &TmiRefundedProducts[ itemsRefunded++ ];
                TMI_Product_t          *pTmiProduct             = &pTmiRefundedProduct->product;
                TMI_ProductDetails_t   *pTmiProductDetails      = &pTmiProduct->productDetails;
                std::string             productType             = getJsonString( cartItem, "type"       );
                std::string             productShortDesc        = getJsonString( cartItem, "short_desc" );
                bool                    isTPurseLoad            = productType == "LLSC" ? true : false;
                Currency_t              price                   = getJsonInt( cartItem, "Price" );

                memset( pTmiRefundedProduct, 0, sizeof( TMI_RefundedProduct_t ) );
                strcpy( pTmiRefundedProduct->refundReason,        isTPurseLoad == true ? "TPurse" : "NonTransit" );
                        pTmiRefundedProduct->itemsToRefund      = 1;    /*  itemsToRefund; */
                        pTmiRefundedProduct->itemsRefunded      = 1;    /*  getJsonInt(  cartItem, "itemnumber"    ); */
                        pTmiRefundedProduct->refundedAmount     = price;
                strcpy( pTmiRefundedProduct->currency,            CurrencyCode );
                        pTmiProduct->productId                  = getJsonInt( cartItem, "id" );
                        pTmiProduct->tariffVersion.majorVersion = MYKI_CD_getMajorVersion( );
                        pTmiProduct->tariffVersion.minorVersion = MYKI_CD_getMinorVersion( );
                        pTmiProduct->saleDate                   = saleDate != 0 ? saleDate : pData->DynamicData.currentDateTime;
                strcpy( pTmiProduct->currency,                    CurrencyCode );
                        pTmiProduct->vatRate                    = getJsonBool( cartItem, "GSTApplicable" ) == true ? vatRate : 0;
                        pTmiProduct->price                      = price;
                        pTmiProductDetails->productOwner        = getJsonInt( cartItem, "issuer_id"  );
               strncpy( pTmiProductDetails->fareIndex,            isTPurseLoad == true ? "flat" : productShortDesc.c_str(), BUFSIZ );
                        pTmiProductDetails->originId            = stopId;
                        pTmiProductDetails->destinationId       = stopId;
                        pTmiProductDetails->route               = routeId;
                        pTmiProductDetails->stopId              = stopId;
                        pTmiProductDetails->cartItemNumber      = getJsonInt( cartItem, "itemnumber" );
               strncpy( pTmiProductDetails->description,          productShortDesc.c_str(), BUFSIZ );
                if ( pLdt != NULL )
                {
                    int transactionType                         = paymentType == TMI_PAYMENT_TYPE_CASH ?
                                                                        TransactionType_TPurseTopupReversalCashRefunded :
                                                                        TransactionType_ProductSalesPaidTPurse; /*  TODO! */
                    pTmiLdt                                     = &TmiLdt;
                    strcpy( TmiLdt.name,                          GetLdtName( ldtName, sizeof( ldtName ), transactionType ) );
                    strcpy( TmiLdt.encoding,                      "UTF8" );
                            TmiLdt.ldt                          = pLdt;
                }

                if      ( productType == "ThirdParty"       ) { countNonTransit++;  }
                else if ( productType == "PremiumSurcharge" ) { countSurcharge++;   }
                else if ( productType == "LLSC"             ) { countTpurseLoad++;  }
                else
                {
                    CsErrx( "CreateProductSalesReversalTmi : unsupported '%s' product type", productType.c_str() );
                    result  = -5;
                }
            }

            if ( result == 0 )
            {
                        TmiTrx.timestamp                            = transactionDateTime;
                strcpy( TmiTrx.status,                                "OK" );
                        TmiTrx.complaint                            = false;
                strcpy( TmiTrx.currency,                              CurrencyCode );
                        TmiTrx.value                                = (int)amountRefunded;
                        TmiTrx.serviceProviderId                    = (int)pData->StaticData.serviceProviderId;
                        TmiTrx.entryPointId                         = pData->DynamicData.entryPointId;
                        TmiPayment.paymentType                      = paymentType;
                strcpy( TmiPayment.currency,                          CurrencyCode );
                /*      TmiPayment.accepted                         = 0;    */
                        TmiPayment.overpaid                         = (int)amountOverpaid;
                /*      TmiPayment.changed                          = 0;    */
                        TmiPayment.refunded                         = (int)amountRefunded;
                /*      TmiPayment.invalid                          = 0;    */
                if ( paymentType == TMI_PAYMENT_TYPE_TPURSE )
                {
                    Json::Value         paymentMedia                = productSaleReversalDetails[ "data" ][ "paymentmedia" ];
                    std::string         mykiPAN                     = getJsonString( paymentMedia, "cardnumber" );
                    int                 remainingValue              = getJsonInt(    paymentMedia, "remvalue"   );
                    std::string         cardIssuerId                = mykiPAN.substr( 0, 6 );
                    std::string         cardSerialNumber            = mykiPAN.substr( 6, 8 );
                    std::string         mediaId                     = getSerialNumber( );
                    TMI_Application_t  *pTmiApplication             = &TmiTPurseDetails.application;

                    pTmiTPurseDetails                               = &TmiTPurseDetails;
                   strncpy( pTmiTPurseDetails->mediaId,               mediaId.c_str(), TMI_MEDIA_ID_LEN );
                    strcpy( pTmiTPurseDetails->mediaType,             "LLSC" );
                   strncpy( pTmiTPurseDetails->cardIssuerId,          cardIssuerId.c_str(), TMI_CARD_ISSUER_ID_LEN );
                   strncpy( pTmiTPurseDetails->cardSerialNumber,      cardSerialNumber.c_str(), TMI_CARD_CSN_LEN );
                    strcpy( pTmiApplication->applicationId,           "transit" );
                    strcpy( pTmiApplication->currency,                CurrencyCode );
                            pTmiApplication->remainingStoredValue   = remainingValue;

                    #if     OPT_EXCLUDE_TPurseDetails
                    {
                        /*  NOTE:   Inclusion of <TPurseDetails> element causing ARCOS to fail importing
                                    transactions paid by TPurse */
                        CsDbg( APP_DEBUG_STATE, "CreateProductSalesReversalTmi : Excluded <TPurseDetails> element" );
                        pTmiTPurseDetails                           = NULL;
                    }
                    #endif
                }
                if ( ( result = TMI_createNonTransitReversalTrx( &TmiTrx,
                                    &TmiRefundedProducts[ 0 ], (size_t)itemsToRefund, &TmiPayment, pTmiLdt, pTmiTPurseDetails ) ) < 0 )
                {
                    CsErrx( "CreateProductSalesReversalTmi : TMI_createNonTransitReversalTrx() failed (%d)", result );
                    result  = -6;
                }
                else
                {
                    CsErrx( "CreateProductSalesReversalTmi : TMI_createNonTransitReversalTrx() successful" );
                    bLdtConsumed    = true;
                    result          = 0;

                    /*  Updates totals */
                    {
                        int     totalCount                      = ( countNonTransit + countSurcharge + countTpurseLoad );
                        int     tripAnnulledSundryTickets       = IncrementCounterBy( "tripAnnulledSundryTickets",      totalCount      );
                        int     portionAnnulledSundryTickets    = IncrementCounterBy( "portionAnnulledSundryTickets",   totalCount      );
                        int     shiftAnnulledSundryTickets      = IncrementCounterBy( "shiftAnnulledSundryTickets",     totalCount      );
                        if ( paymentType == TMI_PAYMENT_TYPE_CASH )
                        {
                            int tripAnnulledCashAmount          = IncrementCounterBy( "tripAnnulledCashAmount",         amountRefunded  );
                            int tripOverpaidCashAmount          = IncrementCounterBy( "tripOverpaidCashAmount",         amountOverpaid  );
                            int tripAnnulledCashCount           = IncrementCounterBy( "tripAnnulledCashCount",          totalCount      );
                            int portionAnnulledCashAmount       = IncrementCounterBy( "portionAnnulledCashAmount",      amountRefunded  );
                            int portionOverpaidCashAmount       = IncrementCounterBy( "portionOverpaidCashAmount",      amountOverpaid  );
                            int portionAnnulledCashCount        = IncrementCounterBy( "portionAnnulledCashCount",       totalCount      );
                            int shiftAnnulledCashAmount         = IncrementCounterBy( "shiftAnnulledCashAmount",        amountRefunded  );
                            int shiftOverpaidCashAmount         = IncrementCounterBy( "shiftOverpaidCashAmount",        amountOverpaid  );
                            int shiftAnnulledCashCount          = IncrementCounterBy( "shiftAnnulledCashCount",         totalCount      );
                            int periodAnnulledCashAmount        = IncrementCounterBy( "periodAnnulledCashAmount",       amountRefunded  );

                            CsDbg( APP_DEBUG_DETAIL, "CreateProductSalesReversalTmi : Cash (%d:%d:%d:%d)/(%d:%d:%d:%d)/(%d:%d:%d:%d)/(%d)",
                                    tripAnnulledCashCount,      tripAnnulledCashAmount,     tripOverpaidCashAmount,     tripAnnulledSundryTickets,
                                    portionAnnulledCashCount,   portionAnnulledCashAmount,  portionOverpaidCashAmount,  portionAnnulledSundryTickets,
                                    shiftAnnulledCashCount,     shiftAnnulledCashAmount,    shiftOverpaidCashAmount,    shiftAnnulledSundryTickets,
                                    periodAnnulledCashAmount );
                        }
                        else
                        {
                            int tripAnnulledOtherAmount         = IncrementCounterBy( "tripAnnulledOtherAmount",        amountRefunded  );
                            int tripAnnulledOtherCount          = IncrementCounterBy( "tripAnnulledOtherCount",         totalCount      );
                            int portionAnnulledOtherAmount      = IncrementCounterBy( "portionAnnulledOtherAmount",     amountRefunded  );
                            int portionAnnulledOtherCount       = IncrementCounterBy( "portionAnnulledOtherCount",      totalCount      );
                            int shiftAnnulledOtherAmount        = IncrementCounterBy( "shiftAnnulledOtherAmount",       amountRefunded  );
                            int shiftAnnulledOtherCount         = IncrementCounterBy( "shiftAnnulledOtherCount",        totalCount      );

                            CsDbg( APP_DEBUG_DETAIL, "CreateProductSalesReversalTmi : Other (%d:%d:%d)/(%d:%d:%d)/(%d:%d:%d)",
                                    tripAnnulledOtherCount,     tripAnnulledOtherAmount,    tripAnnulledSundryTickets,
                                    portionAnnulledOtherCount,  portionAnnulledOtherAmount, portionAnnulledSundryTickets,
                                    shiftAnnulledOtherCount,    shiftAnnulledOtherAmount,   shiftAnnulledSundryTickets );
                        }

                        /*  Persists ALL counters */
                        if ( ( result = TMI_saveCounters( ) ) < 0 )
                        {
                            CsErrx( "CreateProductSalesReversalTmi : TMI_saveCounters() failed (%d)", result );
                        }
                    }
                }
            }

            if ( pLdt != NULL )
            {
                /*  NOTE:   LDT buffer allocated in GetTransactionLDT() */
                CsFree( pLdt );

                /*  And removes LDT if consumed */
                if ( bLdtConsumed == true )
                {
                    unlink( ldtPath );
                }
            }
        }
    }
    return  result;
}   /*  CreateProductSalesReversalTmi( ) */

    /**
     *  @brief  Retrieves (confirmed/unconfirmed) transaction LDT.
     *  @param  pPath returned LDT XML path buffer.
     *  @param  pathSize LDT XML path buffer size.
     *  @param  transactionDateTime transaction date/time.
     *  @param  isUnconfirmed true if card transaction is unconfirmed; false otherwise.
     *  @return NULL if no transaction LDT found; else transaction LDT.
     *  @note   TODO - There is a (small) chance that transferud.sh moves the
     *          transaction LDT to /afc/data/ldt/transfer folder before it is
     *          read by this folder, hence causing transaction TMI generation
     *          to fail.
     */
static
char
*GetTransactionLDT( char *pPath, int pathSize, time_t transactionDateTime, bool isUnconfirmed )
{
    #define BLOCK_SIZE                          0x10
    #define LDT_PATH_CONFIRMED                  "/afc/data/ldt/confirmed"
    #define LDT_PATH_PENDING                    "/afc/data/ldt/pending"
    #define LDT_FILENAME_PATTERN                "ldt_?????_??????????_??????????_%04d%02d%02d_%02d%02d%02d*"

    const char         *pLdtPath                = isUnconfirmed == false ? LDT_PATH_CONFIRMED : LDT_PATH_PENDING;
    char               *pLdt                    = NULL;

    if ( pPath != NULL && pathSize > 0 && transactionDateTime > 0 )
    {
        CsGlob_t        csGlob                  = { 0 };
        struct tm       ctm                     = { 0 };
        char            pattern[ CSMAXPATHLEN ] = { 0 };
        FILE           *fin                     = NULL;
        int             ldtSize                 = 0;
        int             ldtBufSize              = 0;

        memset( pPath,   0, pathSize );
        memset( pattern, 0, sizeof( pattern ) );
        localtime_r( &transactionDateTime, &ctm );
        CsSnprintf( pattern, ( sizeof( pattern ) - 1 ), LDT_FILENAME_PATTERN,
                ( ctm.tm_year + 1900 ), ( ctm.tm_mon + 1 ), ctm.tm_mday,
                ctm.tm_hour, ctm.tm_min, ctm.tm_sec );
        if ( CsGlob( pLdtPath, &csGlob, pattern ) == 0 )
        {
            if ( csGlob.gl_argc == 0 )
            {
                CsDbg( APP_DEBUG_DETAIL, "GetTransactionLDT : no LDT generated for transaction" );
                pPath   = NULL;
            }
            else
            {
                if ( csGlob.gl_argc > 1 )
                {
                    CsWarnx( "GetTransactionLDT : found %d LDTs, used last generated", csGlob.gl_argc );
                }

                CsGlobSort( &csGlob );
                strncpy( pPath, pLdtPath, ( pathSize - 1 ) );
                CsFilenCat( pPath, ( pathSize - 1 ), csGlob.gl_argv[ csGlob.gl_argc - 1 ] );
                CsDbg( APP_DEBUG_DETAIL, "GetTransactionLDT : found '%s'", pPath );

                if ( ( fin = fopen( pPath, "r" ) ) != NULL )
                {
                    /*  Gets LDT size */
                    fseek( fin, 0, SEEK_END );
                    ldtSize = ftell( fin );
                    fseek( fin, 0, SEEK_SET );

                    ldtBufSize  = ( ( ldtSize / BLOCK_SIZE ) + 1 ) * BLOCK_SIZE;
                    pLdt    = (char *)CsMalloc( ldtBufSize );
                    if ( pLdt == NULL )
                    {
                        CsErrx( "GetTransactionLDT : failed allocating LDT buffer" );
                    }
                    else
                    {
                        memset( pLdt, 0, ldtBufSize );
                        if ( fread( pLdt, ldtSize, 1, fin ) != 1 )
                        {
                            CsErrx( "GetTransactionLDT : failed reading LDT from '%s'", pPath );
                            CsFree( pLdt );
                            pLdt    = NULL;
                        }
                        else
                        {
                            CsDbg( APP_DEBUG_DETAIL, "GetTransactionLDT : read %d bytes from '%s'", ldtSize, pPath );
                        }
                    }   /*  end-of-if */
                    fclose( fin );
                }
                else
                {
                    CsErrx( "GetTransactionLDT : failed opening '%s'", pPath );
                }   /*  end-of-if */
            }   /*  end-of-if */
        }
        CsGlobFree( &csGlob );
    }   /*  end-of-if */
    return  pLdt;
}   /*  GetTransactionLDT( ) */

    /**
     *  Returns TPurse product owner.
     *  @return TPurse product owner.
     */
static
int
GetTPurseProductOwner( void )
{
    int                 result              = 0;
    int                 productOwner        = 0;
    MYKI_TAControl_t   *pMYKI_TAControl     = NULL;

    if ( ( result = MYKI_CS_TAControlGet( &pMYKI_TAControl ) ) != MYKI_CS_OK || pMYKI_TAControl == NULL )
    {
        CsWarnx( "GetTPurseProductOwner : MYKI_CS_TAControlGet() failed (%d)", result );
    }
    return  productOwner != 0 ? productOwner : PRODUCT_OWNER_DEFAULT;
}

    /**
     *  Returns default user id.
     *  @param  returned default user id buffer.
     *  @param  buffer size.
     */
static
std::string
GetDefaultUserId()
{
    if
    (
        ( g_terminalType[ 0 ] == 'T' || g_terminalType[ 0 ] == 'B' ) &&
          g_terminalType[ 1 ] == 'D' &&
          g_terminalType[ 2 ] == 'C'
    )
    {
        /*  TDC/TDCSecondary/BDC */
        return "VixDC";
    }
    else
    if
    (
        g_terminalType[ 0 ] == 'F' &&
        g_terminalType[ 1 ] == 'P' &&
        g_terminalType[ 2 ] == 'D'
    )
    {
        /*  FPDg/FPDm/FPDs  */
        return "VixFPD";
    }
    else
    {
        /*  GAC */
        return "VixGate";
    }
}

    /**
     *  Returns counter value.
     *  @param  pKey counter name.
     *  @return counter value or 0.
     */
unsigned int
GetCounter( const char *pKey )
{
    unsigned int    value   = 0;
    int             result  = TMI_getCounter( pKey, &value );

    if ( result < 0 )
    {
        CsErrx( "GetCounter : TMI_getCounter('%s') failed (%d)", pKey, result );
        return  0;
    }
    return  value;
}

    /**
     *  Increments/Decrements counter.
     *  @param  pKey counter name.
     *  @param  value value to be incremented/decremented by.
     *  @return 0 success; -1 otherwise.
     */
static
int
IncrementCounterBy( const char *pKey, int value )
{
    unsigned int    counterValue    = 0;
    int             result          = 0;

    if ( value > 0 )
    {
        /*  Increment */
        result          = TMI_incrementCounterBy( pKey, &counterValue, value );
        if ( result < 0 )
        {
            CsErrx( "IncrementCounterBy : TMI_incrementCounterBy('%s',%d) failed (%d)", pKey, value, result );
            return  -1;
        }
    }
    else
    {
        /*  Decrement.
            Note:   (a) Underflow is intentional.
                    (b) Only work if counter range is [0..FFFFFFFF] */
        counterValue    = GetCounter( pKey );
        counterValue   += value;
        result          = TMI_setCounter( pKey, counterValue );
        if ( result < 0 )
        {
            CsErrx( "IncrementCounterBy : TMI_setCounter('%s',%d) failed (%d)", pKey, value, result );
            return  -1;
        }
    }
    return  (int)counterValue;
}

    /**
     *  @brief  Formats and returns LDT container name.
     *  @param  pBuf returned name buffer.
     *  @param  bufSize returned name buffer size.
     *  @param  transactionType transaction type.
     *  @return LDT container name or "(null)".
     */
static
const char
*GetLdtName( char *pBuf, int bufSize, int transactionType )
{
    if ( pBuf != NULL && bufSize > 0 )
    {
        const char         *pName                   = NULL;
        unsigned int        operationalRecordCount  = GetCounter( "operationalRecordCount" );

        switch ( transactionType )
        {
        case    TransactionType_CscValidation:                      pName   = "SoSo";                       break;
        case    TransactionType_ValidatePin:                        pName   = "ValidatePin";                break;
        case    TransactionType_UpdatePin:                          pName   = "UpdatePin";                  break;
        case    TransactionType_ShiftEnd:                           pName   = "UpdateOperator";             break;
        case    TransactionType_TPurseTopupPaidCash:                pName   = "AddValueTransaction";        break;
        case    TransactionType_TPurseTopupReversalCashRefunded:    pName   = "TPurseReversalTransaction";  break;
        case    TransactionType_ProductSalesPaidTPurse:             pName   = "NonTransitTransaction";      break;
/*      case    TransactionType_ProductSalesReversalRefundedTPurse: pName   = "NonTransitReversal";         break;  */
        default:                                                    pName   = "SoSo";                       break;
        }
        CsSnprintf( pBuf, bufSize, "%s%d", pName, ( operationalRecordCount + 1 ) );
        return  pBuf;
    }
    return  "(null)";
}

    /**
     *  @brief  Converts ISO local time string to time_t.
     *  @param  timeStr ISO time string.
     *  @return Converted time.
     */
static
time_t
isoStrToTime( const std::string &timeStr )
{
    struct tm t;

    if (sscanf(timeStr.c_str(), "%d-%d-%dT%d:%d:%d+%*d:00",
            &(t.tm_year),
            &(t.tm_mon),
            &(t.tm_mday),
            &(t.tm_hour),
            &(t.tm_min),
            &(t.tm_sec)) != 6)
    {
        return 0;
    }

    // adjust values to match 'struct tm' spec
    t.tm_year -= 1900;
    t.tm_mon -= 1;
    t.tm_isdst = -1;

    return mktime(&t);
}