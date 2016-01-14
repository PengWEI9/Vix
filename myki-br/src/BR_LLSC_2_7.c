/* -*- mode: c; tabs: 4; -*- */
/*=============================================================================
**
**    Vix Technology                   Licensed software
**    (C) 2013                         All rights reserved
**
**=============================================================================
**
**  Project/Product : MBU
**  Filename        : BR_LLSC_2_7.c
**  Author(s)       : ???
**
**  ID              : BR_LLSC_2_7 - NTS0177 v7.2
**
**  Name            : Process Actionlist Product Sale
**
**  Data Fields     :
**
**      1.  Tariff.CitySaverZoneFlag
**      2.  Actionlist.Type
**      3.  Actionlist.ActionSeqNo
**      4.  Actionlist.ProductZoneLow
**      5.  Actionlist.ProductZoneHigh
**      6.  TAppControl.ActionSeqNo
**      7.  TAppControl.Directory.Status
**      8.  Dynamic.CurrentDateTime
**      9.  Dynamic.EntryPointID
**      10. Dynamic.LineID
**      11. Dynamic.StopID
**      12. Actionlist.ProductPurchaseValue
**      13. ActionList.ServiceProvider
**      14. Actionlist.IssuerID
**      15. ActionList.ProductID
**      16. TAppTProduct.NextTxSeqNo
**      17. Dynamic.LoadLog
**      18. Dynamic.LoadLogTxValue
**
**  Pre-Conditions  :
**
**      1.  An actionlist entry exists for the card UID.
**      2.  The actionlist type is equal to TPurseLoad/None.
**      3.  The actionlist action sequence number is between 1 and 15.
**      4.  If the transit application action sequence number is equal to 15:
**          a.  The actionlist action sequence number is equal to 1.
**      5.  Else:
**          a.  The actionlist action sequence number is equal to the
**              transit application action sequence number + 1.
**      6.  At least one product directory status is set to Unused. If not,
**          generate a FailureResponse of type ProductLimitReached.
**      7.  A maximum of one e-Pass product already exists on the card.
**          If not, generate a FailureResponse of type ProductLimitReached.
**      8.  If the City Saver flag on product low zone - 1 is set, decrement
**          the product low zone4 by 1. If the City Saver flag on the product
**          high zone is set, increment the product high zone by 1.
**
**  Description     :
**
**      1.  Perform a ProductSale/None transaction.
**      2.  Modify load log
**          a.  Definition:
**              i.  TAppLoadLog.ControlBitmap: Product
**              ii. If LoadTxType is not set then
**                  (1) Set LoadTxtype to Load Product(2)
**                  (2) Else, Set LoadTxType to Multiple Actionlists (21)
**              iii.Determine loadlog ControlBitmap though look up of the TxLoadType
**              iv. TxSeqNo = as returned from product Sale none transaction
**              v.  ServiceProviderID =  Actionlist ServiceProviderID13
**              vi. Set TxDateTime as current Date time
**              vii.Set location object to the current device location
**                  (i.e Entry Point, Route, Stop ID)
**          b.  Value:
**              i.  Dyanamic.LoadLogTxValue = Dynamic.LoadLogTxValue + Action List value
**              ii. If Dynamic.LoadLogTxValue is greater than 0 then set
**                  LoadLog.TxValue = Dynamic.LoadLogTxValue Else set LoadLog.TxValue = 0
**              iii.TAppLoadLog.NewTPurseBalance = Current balanace
**              iv. PaymentMethod = Ad-hoc autoload (6)
**          c.  Product:
**              i.  ProductIssuerId  = ActionLIst.IssuerID
**              ii. ProductSerialNo =  As returned from ProductSale/none transaction
**                  at step 1
**              iii.ProductId = ActionList.ProductID
**
**  Post-Conditions :
**
**      1.  A new inactive e-Pass product is created on the smartcard.
**
**  Member(s)       :
**      BR_LLSC_2_7             [public]    business rule
**
**  Information     :
**   Compiler(s)    : C
**   Target(s)      : Independent
**
**  Subversion      :
**      $Id: $
**      $HeadURL: $
**
**  History         :
**   Vers.  Date        Aut.  Type     Description
**   -----  ----------  ----  -------  ----------------------------------------
**    1.00  ??.??.??    ???   Create
**    1.01  15.11.13    ANT   Modify   Removed updating
**                                     TAppControl.ActionSeqNo as done by LDT
**                            Add      Added passing actionlist data to LDT
**    1.02  03.12.13    ANT   Add      Generating FailureResponse messages
**    1.03  04.12.13    ANT   Modify   Rectified TAppLoadLog generation
**    1.04  29.10.14    ANT   Modify   Changed myki_br_GetActionlist() signature.
**
**===========================================================================*/

#include <cs.h>                 // For debug logging
#include <myki_cardservices.h>  // Myki Card Services
#include <LDT.h>                // Logical Device Transaction
#include <myki_actionlist.h>    // Actionlist
#include "myki_br_rules.h"      // API for all BR_xxx modules
#include "BR_Common.h"          // Common utilities, macros, etc

/*==========================================================================*
**
**  BR_LLSC_2_7
**
**  Description     :
**      Implements business rule BR_LLSC_2_7.
**
**  Parameters      :
**      pData               [I/O]   BR context data
**
**  Returns         :
**      RULE_RESULT_EXECUTED
**      RULE_RESULT_BYPASSED
**      RULE_RESULT_ERROR
**
**  Notes           :
**
**==========================================================================*/

RuleResult_e BR_LLSC_2_7( MYKI_BR_ContextData_t *pData )
{
    MYKI_TAControl_t            *pMYKI_TAControl        = NULL;
    MYKI_TAPurseBalance_t       *pMYKI_TAPurseBalance   = NULL;
    MYKI_TAPurseControl_t       *pMYKI_TAPurseControl   = NULL;
    MYKI_TAProduct_t            *pMYKI_TAProduct        = NULL;
    MYKI_Directory_t            *pMYKI_Directory        = NULL;
    CT_CardInfo_t               *pCardInfo              = NULL;
    U8_t                         unUsedCount            = 0;
    U8_t                         ePassCount             = 0;
    int                          dir                    = 0;
    int                          errcode                = 0;
    TAppLoadLog_t               *pAppLoadLog            = NULL;


    CsDbg( BRLL_RULE, "BR_LLSC_2_7 : Start (Process Actionlist Product Sale)" );

    if ( ! pData )
    {
        CsErrx( "BR_LLSC_2_7 : Invalid argument(s)" );
        return RULE_RESULT_ERROR;
    }

    if ( MYKI_CS_TAControlGet( &pMYKI_TAControl ) < 0 )
    {
        CsErrx( "BR_LLSC_2_7 : MYKI_CS_TAControlGet() failed" );
        return RULE_RESULT_ERROR;
    }

    if ( MYKI_CS_TAPurseBalanceGet( &pMYKI_TAPurseBalance ) < 0 )
    {
        CsErrx( "BR_LLSC_2_7 : MYKI_CS_TAPurseBalanceGet() failed" );
        return RULE_RESULT_ERROR;
    }

    if ( MYKI_CS_GetCardInfo( &pCardInfo ) < 0 )
    {
        CsErrx( "BR_LLSC_2_7 : MYKI_CS_GetCardInfo() failed" );
        return RULE_RESULT_ERROR;
    }

    if ( MYKI_CS_TAPurseControlGet( &pMYKI_TAPurseControl ) < 0 )
    {
        CsErrx( "BR_LLSC_2_7 : MYKI_CS_TAPurseControlGet() failed" );
        return RULE_RESULT_ERROR;
    }

    /*  PRE-CONDITIONS */
    {
        U8_t    searchActionSequenceNumber  = ( pMYKI_TAControl->ActionSeqNo == 15 ? 1 : ( pMYKI_TAControl->ActionSeqNo + 1 ) );

        // populate the ActionList structure
        if ( pData->ActionList.type == ACTION_PRODUCT_SALE_NONE )
        {
            if ( pData->ActionList.actionlist.productSale.actionSequenceNo == searchActionSequenceNumber )
            {
                CsDbg( BRLL_RULE, "BR_LLSC_2_7 : found desired actionlist in cache" );
            }
            else
            {
                pData->ActionList.type  = ACTION_NULL;
            }
        }

        if ( pData->ActionList.type != ACTION_PRODUCT_SALE_NONE )
        {
            if ( ( errcode = myki_br_GetActionlist( pCardInfo->SerialNumber, ACTION_PRODUCT_SALE_NONE, searchActionSequenceNumber, 0 /*DONTCARE*/, pData ) ) < 0 )
            {
                CsDbg( BRLL_RULE, "BR_LLSC_2_7 : myki_br_GetActionlist returns error %d", errcode);
                return RULE_RESULT_ERROR;
            }
        }

        /*  1.  An actionlist entry exists for the card UID.
            2.  The actionlist type is equal to TPurseLoad/None. */
        if ( pData->ActionList.type != ACTION_PRODUCT_SALE_NONE )
        {
            CsDbg( BRLL_RULE, "BR_LLSC_2_7 : Bypassed - ActionList.type is not ACTION_PRODUCT_SALE_NONE." );
            pData->ReturnedData.bypassCode  = BYPASS_CODE( 2, 7, 2, 0 );
            return RULE_RESULT_BYPASSED;
        }

        /*  3.  The actionlist action sequence number is between 1 and 15. */
        if ( pData->ActionList.actionlist.productSale.actionSequenceNo < 1 || pData->ActionList.actionlist.productSale.actionSequenceNo > 15 )
        {
            CsDbg( BRLL_RULE, "BR_LLSC_2_7 : Bypassed - ActionList.actionSeqNo (%d) is out of range.",
                pData->ActionList.actionlist.productSale.actionSequenceNo );
            pData->ReturnedData.bypassCode  = BYPASS_CODE( 2, 7, 3, 0 );
            return RULE_RESULT_BYPASSED;
        }

        /*  4.  If the transit application action sequence number is equal to 15:
                a.  The actionlist action sequence number is equal to 1.
            5.  Else:
                a.  The actionlist action sequence number is equal to the transit application
                    action sequence number + 1. */
        CsDbg( BRLL_RULE, "BR_LLSC_2_7 : pMYKI_TAControl->ActionSeqNo = %d, pData->ActionList.actionSeqNo = %d",
            pMYKI_TAControl->ActionSeqNo, pData->ActionList.actionlist.productSale.actionSequenceNo );
        if ( pMYKI_TAControl->ActionSeqNo == 15 )
        {
            if ( pData->ActionList.actionlist.productSale.actionSequenceNo != 1 )
            {
                CsDbg( BRLL_RULE, "BR_LLSC_2_7 : Bypassed - pMYKI_TAControl->ActionSeqNo = 15, ActionList.actionSeqNo != 1");
                pData->ReturnedData.bypassCode  = BYPASS_CODE( 2, 7, 4, 0 );
                return RULE_RESULT_BYPASSED;
            }
        }
        else
        if ( pData->ActionList.actionlist.productSale.actionSequenceNo != ( pMYKI_TAControl->ActionSeqNo + 1 ) )
        {
            CsDbg( BRLL_RULE, "BR_LLSC_2_7 : Bypassed - pMYKI_TAControl->ActionSeqNo + 1 != ActionList.actionSeqNo");
            pData->ReturnedData.bypassCode  = BYPASS_CODE( 2, 7, 5, 0 );
            return RULE_RESULT_BYPASSED;
        }

        for ( dir = 1; dir < DIMOF( pMYKI_TAControl->Directory ); dir++ ) // T-Purse dir entry does not have a product slot, so there's one less product slots than dir entries
        {
            MYKI_Directory_t    *pDirectory = NULL;

            pDirectory = &pMYKI_TAControl->Directory[ dir ]; // Product slot 0-4 maps to dir entry 1-5

            if ( pDirectory->Status == TAPP_CONTROL_DIRECTORY_STATUS_UNUSED )
            {
                ++unUsedCount;
            }
            else
            if ( myki_br_cd_GetProductType( pDirectory->ProductId ) == PRODUCT_TYPE_EPASS )
            {
                ++ePassCount;
            }
        }

        /*  6.  At least one product directory status is set to Unused. */
        if ( unUsedCount == 0 )
        {
            /*  If not, generate a FailureResponse of type ProductLimitReached. */
            if ( myki_br_ldt_ProductSale_None_FailureResponse( pData, MYKI_UD_FAILURE_REASON_PRODUCT_LIMIT_REACHED ) < 0 )
            {
                CsErrx( "BR_LLSC_2_7 : myki_br_ldt_ProductSale_None_FailureResponse() failed" );
                return RULE_RESULT_ERROR;
            }   /* end-of-if */

            CsDbg( BRLL_RULE, "BR_LLSC_2_7 : Bypassed - at least one unused product is required." );
            pData->ReturnedData.bypassCode  = BYPASS_CODE( 2, 7, 6, 0 );
            return RULE_RESULT_BYPASSED;
        }

        /*  7.  A maximum of one e-Pass product already exists on the card. */
        if ( ePassCount > 1 )
        {
            /*  If not, generate a FailureResponse of type ProductLimitReached. */
            if ( myki_br_ldt_ProductSale_None_FailureResponse( pData, MYKI_UD_FAILURE_REASON_PRODUCT_LIMIT_REACHED ) < 0 )
            {
                CsErrx( "BR_LLSC_2_7 : myki_br_ldt_ProductSale_None_FailureResponse() failed" );
                return RULE_RESULT_ERROR;
            }   /* end-of-if */

            CsDbg( BRLL_RULE, "BR_LLSC_2_7 : Bypassed - a maximum of one e-Pass product may exist.");
            pData->ReturnedData.bypassCode  = BYPASS_CODE( 2, 7, 7, 0 );
            return RULE_RESULT_BYPASSED;
        }

        /*  8.  If the City Saver flag on product low zone - 1 is set, decrement the product low zone by 1.
                If the City Saver flag on the product high zone is set, increment the product high zone by 1. */
        ADJUST_FOR_CITYSAVER( pData->ActionList.actionlist.productSale.zoneLow, pData->ActionList.actionlist.productSale.zoneHigh );
    }

    /*  PROCESSING */
    {
        /*  1.  Perform a ProductSale/None transaction. */
        if ( ( dir = myki_br_ldt_ProductSaleEx( pData, &pData->ActionList.actionlist.productSale ) ) < 0 )
        {
            CsErrx( "BR_LLSC_2_7 : myki_br_ldt_ProductSaleEx() failed" );
            return RULE_RESULT_ERROR;
        }

        /*  NOTE:   TAppControl.ActionSeqNo is updated by LDT */

        if ( myki_br_GetCardProduct( dir, &pMYKI_Directory, &pMYKI_TAProduct ) < 0 )
        {
            CsErrx( "BR_LLSC_2_7 : myki_br_GetCardProduct(%d) failed", dir );
            return RULE_RESULT_ERROR;
        }   /* end-of-if */

        /*  2.  Modify load log */
        pAppLoadLog                             = &pData->InternalData.LoadLogData;

        /*      a.  Definition: */
        {
            /*      i.  TAppLoadLog.ControlBitmap: Product (Done by framework) */
            /*      ii. If LoadTxType is not set then */
            if ( pAppLoadLog->transactionType == MYKI_BR_TRANSACTION_TYPE_NONE )
            {
                /*      (1) Set LoadTxtype to Load Product(2) */
                pAppLoadLog->transactionType        = MYKI_BR_TRANSACTION_TYPE_LOAD_PRODUCT;
            }
            else
            {               
                /*      (2) Else, Set LoadTxType to Multiple Actionlists (21) */
                pAppLoadLog->transactionType        = MYKI_BR_TRANSACTION_TYPE_MULTIPLE_ACTION_LIST;
            }   /* end-of-if */

            /*      iii.Determine loadlog ControlBitmap th(r)ough look up of the TxLoadType (Done by framework) */
            /*      iv. TxSeqNo = as returned from product Sale none transaction */

            pAppLoadLog->transactionSequenceNumber  = pMYKI_TAProduct->NextTxSeqNo - 1;

            /*      v.  ServiceProviderID =  Actionlist ServiceProviderID (Done by framework)
                    vi. Set TxDateTime as current Date time (Done by framework)
                    vii.Set location object to the current device location
                        (i.e Entry Point, Route, Stop ID) (Done by framework) */
        }

        /*      b.  Value: */
        {
            /*      i.  Dyanamic.LoadLogTxValue = Dynamic.LoadLogTxValue + Action List value */
            pData->DynamicData.loadLogTxValue      += pData->ActionList.actionlist.productSale.purchaseValue;

            /*      ii. If Dynamic.LoadLogTxValue is greater than 0 */
            if ( pData->DynamicData.loadLogTxValue > 0 )
            {
                /*      then set LoadLog.TxValue = Dynamic.LoadLogTxValue */
                pAppLoadLog->transactionValue       = (U32_t)pData->DynamicData.loadLogTxValue;
            }
            else
            {
                /*      Else set LoadLog.TxValue = 0 */
                pAppLoadLog->transactionValue       = (U32_t)0;
            }   /* end-of-if */
            pAppLoadLog->isTransactionValueSet      = TRUE;

            /*      iii.TAppLoadLog.NewTPurseBalance = Current balanace */
            pAppLoadLog->isNewTPurseBalanceSet      = TRUE;
            pAppLoadLog->newTPurseBalance           = pMYKI_TAPurseBalance->Balance;

            /*      iv. PaymentMethod = Ad-hoc autoload (6) */
            pAppLoadLog->isPaymentMethodSet         = TRUE;
            pAppLoadLog->paymentMethod              = TAPP_USAGE_LOG_PAYMENT_METHOD_ADHOC_AUTOLOAD;
        }

        /*      c.  Product: */
        {
            /*      i.  ProductIssuerId  = ActionLIst.IssuerID */
            pAppLoadLog->isProductIssuerIdSet       = TRUE;
            pAppLoadLog->productIssuerId            = pMYKI_Directory->IssuerId;

            /*      ii. ProductSerialNo = As returned from ProductSale/none transaction at step 1 */
            pAppLoadLog->isProductSerialNoSet       = TRUE;
            pAppLoadLog->productSerialNo            = pMYKI_Directory->SerialNo;

            /*      iii.ProductId = ActionList.ProductID */
            pAppLoadLog->isProductIdSet             = TRUE;
            pAppLoadLog->productId                  = pMYKI_Directory->ProductId;
        }

        /*  NOTE:   TAppLoadLog entry is added by application framework. */
        pData->InternalData.IsLoadLogUpdated        = TRUE;
        CsDbg( BRLL_RULE, "BR_LLSC_2_7 : Updated TAppLoadLog" );
    }

    /*  POST-CONDITIONS */
    {
        /*  1.  A new inactive e-Pass product is created on the smartcard. */
        pData->ReturnedData.actionApplied       = TRUE;
    }

    CsDbg( BRLL_RULE, "BR_LLSC_2_7 : Executed" );
    return RULE_RESULT_EXECUTED;
}
