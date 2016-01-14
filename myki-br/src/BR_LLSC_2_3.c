/* -*- mode: c; tabs: 4; -*- */
/*=============================================================================
**
**    Vix Technology                   Licensed software
**    (C) 2013                         All rights reserved
**
**=============================================================================
**
**  Project/Product : MBU
**  Filename        : BR_LLSC_2_3.c
**  Author(s)       : ???
**
**  ID              : BR_LLSC_2_3 - NTS0177 v7.2
**
**  Name            : Process Hotlist Block Product
**
**  Data Fields     :
**
**      1.  Actionlist.Type
**      2.  Actionlist.ActionSeqNo
**      3.  Actionlist.ProductSerialNo
**      4.  TAppControl.Directory.Status
**      5.  TAppTProduct.ActionSeqNo
**      6.  TAppTProduct.SerialNo
**      7.  Dynamic.usageLog
**      8.  Static.ServiceProvider
**      9.  Dynamic.CurrentDateTime
**
**  Pre-Conditions  :
**
**      1.  An actionlist entry exists for the card UID.
**      2.  The actionlist type is equal to ProductUpdate/Block.
**      3.  The actionlist action sequence number is between 1 and 15.
**      4.  If the product action sequence number is equal to 15:
**          a.  The actionlist action sequence number is equal to 1.
**      5.  Else:
**          a.  The actionlist action sequence number is equal to the
**              product action sequence number + 1.
**      6.  A product exists on the smartcard with a product serial number
**          equal to the product serial number in the actionlist.
**          If not, generate a FailureResponse of type ProductNotFound.
**      7.  The product status is not equal to Blocked. If it is blocked,
**          generate a FailureResponse of type ProductBlocked.
**
**  Description     :
**
**      1.  Perform a ProductUpdate/Block transaction for the product
**          with the relevant serial number.
**      2.  Modify usage log
**          a.  Definition:
**              i.  Set UsageTxType (11)
**              ii. Determine uselog ControlBitmap though look up of the
**                  TxUsageType
**              iii.ServiceProvider = ServiceProvider
**              iv. TxDateTime – Current DateTime
**          b.  Product:
**              i.  ProductIssuerId = IssuerID
**              ii. ProductSerialNo = As returned from ProductUpdate/Block
**                  transaction at step 1
**              iii.ProductId = ActionList.ProductID
**
**  Post-Conditions :
**
**      1.  The specified product is blocked.
**
**  Member(s)       :
**      BR_LLSC_2_3             [public]    business rule
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
**    1.01  03.12.13    ANT   Add      Generating FailureResponse messages
**    1.02  04.12.13    ANT   Modify   No code change. Updated comments.
**    1.03  29.10.14    ANT   Modify   Changed myki_br_GetActionlist() signature.
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
**  BR_LLSC_2_3
**
**  Description     :
**      Implements business rule BR_LLSC_2_3.
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

RuleResult_e BR_LLSC_2_3( MYKI_BR_ContextData_t *pData )
{
    MYKI_TAControl_t       *pMYKI_TAControl             = NULL;
    MYKI_Directory_t       *pDirectory                  = NULL;
    MYKI_TAProduct_t       *pMYKI_TAProduct             = NULL;
    CT_CardInfo_t          *pCardInfo                   = NULL;
    int                     errcode                     = 1;
    int                     dirIndex                    = 0;
    U8_t                    searchActionSequenceNumber  = 0;
    U16_t                   searchProductSerialNumber   = 0;
    ProductUpdate_t         foundProductUpdate;

    CsDbg( BRLL_RULE, "BR_LLSC_2_3 : Start (Process Hotlist Block Product)" );

    if ( ! pData )
    {
        CsErrx( "BR_LLSC_2_3 : Invalid argument(s)" );
        return RULE_RESULT_ERROR;
    }

    if ( MYKI_CS_TAControlGet( &pMYKI_TAControl ) < 0 )
    {
        CsErrx( "BR_LLSC_2_3 : MYKI_CS_TAControlGet() failed" );
        return RULE_RESULT_ERROR;
    }

    if ( MYKI_CS_GetCardInfo( &pCardInfo ) < 0 )
    {
        CsErrx( "BR_LLSC_2_3 : MYKI_CS_GetCardInfo() failed" );
        return RULE_RESULT_ERROR;
    }

    memset( &foundProductUpdate, 0, sizeof( ProductUpdate_t ) );

    /*  PRE-CONDITIONS */
    {
        for ( dirIndex = 1; dirIndex < DIMOF( pMYKI_TAControl->Directory ); dirIndex++ )
        {
            if ( myki_br_GetCardProduct( dirIndex, &pDirectory, NULL ) < 0 )
            {
                CsErrx( "BR_LLSC_2_3 : myki_br_GetCardProduct(%d) failed", dirIndex );
                return RULE_RESULT_ERROR;
            }   /*  end-of-if */

            if ( pDirectory->Status == TAPP_CONTROL_DIRECTORY_STATUS_UNUSED )
            {
                continue;
            }   /*  end-of-if */

            if ( myki_br_GetCardProduct( dirIndex, &pDirectory, &pMYKI_TAProduct ) < 0 )
            {
                CsErrx( "BR_LLSC_2_3 : myki_br_GetCardProduct(%d) failed", dirIndex );
                return RULE_RESULT_ERROR;
            }   /*  end-of-if */

            searchActionSequenceNumber  = ( pMYKI_TAProduct->ActionSeqNo == 15 ? 1 : ( pMYKI_TAProduct->ActionSeqNo + 1 ) );
            searchProductSerialNumber   = pDirectory->SerialNo;

            // populate the ActionList structure
            if ( pData->ActionList.type == ACTION_PRODUCT_UPDATE_BLOCK )
            {
                if ( pData->ActionList.actionlist.productUpdate.actionSequenceNo == searchActionSequenceNumber &&
                     pData->ActionList.actionlist.productUpdate.serialNo         == searchProductSerialNumber  )
                {
                    /*  Found the desired actionlist in cache => process it */
                    CsDbg( BRLL_RULE, "BR_LLSC_2_3 : found desired actionlist in cache" );
                    memcpy( &foundProductUpdate, &pData->ActionList.actionlist.productUpdate, sizeof( ProductUpdate_t ) );
                    break;
                }
                pData->ActionList.type  = ACTION_NULL;
            }   /*  end-of-if */

            if ( pData->ActionList.type != ACTION_PRODUCT_UPDATE_BLOCK )
            {
                if ( ( errcode = myki_br_GetActionlist( pCardInfo->SerialNumber, ACTION_PRODUCT_UPDATE_BLOCK, searchActionSequenceNumber, searchProductSerialNumber, pData ) ) < 0 )
                {
                    CsDbg( BRLL_RULE, "BR_LLSC_2_3 : myki_br_GetActionlist returns error %d", errcode );
                    return RULE_RESULT_ERROR;
                }   /*  end-of-if */

                if ( pData->ActionList.type == ACTION_PRODUCT_UPDATE_BLOCK )
                {
                    memcpy( &foundProductUpdate, &pData->ActionList.actionlist.productUpdate, sizeof( ProductUpdate_t ) );
                    if ( errcode > 0 )
                    {
                        /*  Found the desired actionlist => process it */
                        break;
                    }   /*  end-of-if */

                    /*  Searches again for another product */
                    pData->ActionList.type  = ACTION_NULL;
                }   /*  end-of-if */
            }   /*  end-of-if */
        }   /*  end-of-for */

        if ( foundProductUpdate.serialNo != 0 )
        {
            /*  A TAppUpdate/Block actionlist for existing product found.. */
            pData->ActionList.type  = ACTION_PRODUCT_UPDATE_BLOCK;
            memcpy( &pData->ActionList.actionlist.productUpdate, &foundProductUpdate, sizeof( ProductUpdate_t ) );
        }
        else
        {
            /*  No TAppUpdate/Block actionlist for existing product(s) found,
                check for TAppUpdate/Block actionlist of any product for this card */
            if ( ( errcode = myki_br_GetActionlist( pCardInfo->SerialNumber, ACTION_PRODUCT_UPDATE_BLOCK, 0 /*ANY*/, 0 /*ANY*/, pData ) ) < 0 )
            {
                CsDbg( BRLL_RULE, "BR_LLSC_2_3 : myki_br_GetActionlist returns error %d", errcode );
                return RULE_RESULT_ERROR;
            }   /*  end-of-if */
        }   /*  end-of-if */

        /*  1.  An actionlist entry exists for the card UID.
            2.  The actionlist type is equal to ProductUpdate/Block. */
        if ( pData->ActionList.type != ACTION_PRODUCT_UPDATE_BLOCK )
        {
            CsDbg( BRLL_RULE, "BR_LLSC_2_3 : Bypassed - ActionList.type is not ACTION_PRODUCT_UPDATE_BLOCK." );
            return RULE_RESULT_BYPASSED;
        }

        /*  3.  The actionlist action sequence number is between 1 and 15. */
        if ( pData->ActionList.actionlist.productUpdate.actionSequenceNo < 1 || pData->ActionList.actionlist.productUpdate.actionSequenceNo > 15)
        {
            CsDbg( BRLL_RULE, "BR_LLSC_2_3 : Bypassed - ActionList.actionSeqNo (%d) is out of range.", pData->ActionList.actionlist.productUpdate.actionSequenceNo );
            return RULE_RESULT_BYPASSED;
        }

        /*  NOTE:   Must do step 6 before steps 4 and 5 */
        /*  6.  A product exists on the smartcard with a product serial number equal
                to the product serial number in the actionlist. */
        if ( ( dirIndex = myki_br_GetDirWithSerial( pMYKI_TAControl, pData->ActionList.actionlist.productUpdate.serialNo ) ) == 0 )
        {
            /*  If not, generate a FailureResponse of type ProductNotFound. */
            if ( myki_br_ldt_ProductUpdate_Block_FailureResponse( pData, MYKI_UD_FAILURE_REASON_PRODUCT_NOT_FOUND ) < 0 )
            {
                CsErrx( "BR_LLSC_2_3 : myki_br_ldt_ProductUpdate_Block_FailureResponse() failed" );
                return RULE_RESULT_ERROR;
            }   /* end-of-if */

            CsDbg( BRLL_RULE, "BR_LLSC_2_3 : Bypassed - Cannot find the product with serial number %d",
                pData->ActionList.actionlist.productUpdate.serialNo );
            return RULE_RESULT_BYPASSED;
        }

        if ( myki_br_GetCardProduct( dirIndex, &pDirectory, &pMYKI_TAProduct ) < 0 )
        {
            CsErrx( "BR_LLSC_2_3 : myki_br_GetCardProduct(%d) failed", dirIndex );
            return RULE_RESULT_ERROR;
        }   /* end-of-for */

        /*  4.  If the product action sequence number is equal to 15:
                a.  The actionlist action sequence number is equal to 1.
            5.	Else:
                a.  The actionlist action sequence number is equal to the product
                    action sequence number + 1. */
        CsDbg( BRLL_RULE, "BR_LLSC_2_3 : pMYKI_TAProduct->ActionSeqNo = %d, pData->ActionList.actionSeqNo = %d",
            pMYKI_TAProduct->ActionSeqNo, pData->ActionList.actionlist.productUpdate.actionSequenceNo );

        if ( pMYKI_TAProduct->ActionSeqNo == 15 )
        {
            if ( pData->ActionList.actionlist.productUpdate.actionSequenceNo != 1 )
            {
                CsDbg( BRLL_RULE, "BR_LLSC_2_3 : Bypassed - pMYKI_TAProduct->ActionSeqNo = 15, ActionList.actionSeqNo != 1");
                return RULE_RESULT_BYPASSED;
            }
        }
        else
        if ( pData->ActionList.actionlist.productUpdate.actionSequenceNo != ( pMYKI_TAProduct->ActionSeqNo + 1 ) )
        {
            CsDbg( BRLL_RULE, "BR_LLSC_2_3 : Bypassed - pMYKI_TAProduct->ActionSeqNo + 1 != ActionList.productUpdate.actionSeqNo");
            return RULE_RESULT_BYPASSED;
        }

        /*  7.  The product status is not equal to Blocked. */
        if ( pDirectory->Status == TAPP_CONTROL_DIRECTORY_STATUS_BLOCKED )
        {
            /*  If it is blocked, generate a FailureResponse of type ProductBlocked. */
            if ( myki_br_ldt_ProductUpdate_Block_FailureResponse( pData, MYKI_UD_FAILURE_REASON_PRODUCT_BLOCKED ) < 0 )
            {
                CsErrx( "BR_LLSC_2_3 : myki_br_ldt_ProductUpdate_Block_FailureResponse() failed" );
                return RULE_RESULT_ERROR;
            }   /* end-of-if */

            CsDbg( BRLL_RULE, "BR_LLSC_2_3 : Bypassed - Product is already blocked.");
            return RULE_RESULT_BYPASSED;
        }
    }

    /*  PROCESSING */
    {
        /*  1.  Perform a ProductUpdate/Block transaction for the product with the relevant serial number. */
        if ( myki_br_ldt_Actionlist( pData ) < 0 )
        {
            CsErrx( "BR_LLSC_2_3 : myki_br_ldt_Actionlist(%d) failed", dirIndex );
            return RULE_RESULT_ERROR;
        }   /* end-of-if */

        /*  NOTE:   TAppProduct.ActionSeqNo is updated by LDT */

        /*  2.  Modify usage log
                a.  Definition: */
        {
            /*      i.  Set  UsageTxType(11) */
            pData->InternalData.UsageLogData.transactionType        = MYKI_BR_TRANSACTION_TYPE_PRODUCT_BLOCK;
            /*      ii. Determine uselog ControlBitmap though look up of the TxUsageType (Done by framework)
                    iii.ServiceProvider = ServiceProvider (Done by framework)
                    iv. TxDateTime = Current DateTime (Done by framework) */
        }

        /*      b.  Product: */
        {
            /*      i.  ProductIssuerId  = IssuerID */
            pData->InternalData.UsageLogData.isProductIssuerIdSet   = TRUE;
            pData->InternalData.UsageLogData.productIssuerId        = pDirectory->IssuerId;

            /*      ii. ProductSerialNo =  As returned from ProductUpdate/Block transaction at step 1 */
            pData->InternalData.UsageLogData.isProductSerialNoSet   = TRUE;
            pData->InternalData.UsageLogData.productSerialNo        = pDirectory->SerialNo;

            /*      iii.ProductId = ActionList.ProductID */
            pData->InternalData.UsageLogData.isProductIdSet         = TRUE;
            pData->InternalData.UsageLogData.productId              = pDirectory->ProductId;
        }

        /*  NOTE:   TAppUsageLog entry is added by application framework. */
        pData->InternalData.IsUsageLogUpdated           = TRUE;
        CsDbg( BRLL_RULE, "BR_LLSC_2_3 : Updated TAppUsageLog" );
    }

    /*  POST-CONDITIONS */
    {
        /*  1.  The specified product is blocked. */
        pData->ReturnedData.actionApplied       = TRUE;
    }

    CsDbg( BRLL_RULE, "BR_LLSC_2_3 : Executed" );
    return RULE_RESULT_EXECUTED;
}

