/* -*- mode: c; tabs: 4; -*- */
/*=============================================================================
**
**    Vix Technology                   Licensed software
**    (C) 2013                         All rights reserved
**
**=============================================================================
**
**  Project/Product : MBU
**  Filename        : BR_LLSC_2_5.c
**  Author(s)       : ???
**
**  ID              : BR_LLSC_2_5 - NTS0177 v7.2
**
**  Name            : Process Actionlist Enable/Disable Product Autoload
**
**  Data Fields     :
**
**      1.  Actionlist.Type
**      2.  Actionlist.ActionSeqNo
**      3.  Actionlist.ProductSerialNo
**      4.  TAppControl.Directory.Status
**      5.  TAppTProduct.ActionSeqNo
**      6.  TAppTProduct.SerialNo
**
**  Pre-Conditions  :
**
**      1.  An actionlist entry exists for the card UID.
**      2.  The actionlist type is equal to ProductUpdate/None
**          (autoload enable/disable).
**      3.  The actionlist action sequence number is between 1 and 15.
**      4.  If the product action sequence number is equal to 15:
**          a.  The actionlist action sequence number is equal to 1.
**      5.  Else:
**          a.  The actionlist action sequence number is equal to the
**              product action sequence number + 1.
**      6.  A product exists on the smartcard with a product serial number
**          equal to the product serial number in the actionlist. If not,
**          generate a FailureResponse of type ProductNotFound.
**      7.  The product status is not equal to Blocked. If it is blocked,
**          generate a FailureResponse of type ProductBlocked.
**
**  Description     :
**
**      1.  Perform a ProductUpdate/None (autoload enable/disable)
**          transaction for the product with the relevant serial number.
**
**  Post-Conditions :
**
**      1.  Product autoload is enabled/disabled for the specified product
**          on the smartcard.
**
**  Member(s)       :
**      BR_LLSC_2_5             [public]    business rule
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
**    1.01  12.11.13    ANT   Modify   Amended to enable and disable auto-load.
**                                     Rectified checking actionlist sequence
**                                     number against product action sequence
**                                     number.
**    1.02  03.12.13    ANT   Add      Generating FailureResponse messages
**    1.03  04.12.13    ANT   Modify   No code change. Updated comments.
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
**  BR_LLSC_2_5
**
**  Description     :
**      Implements business rule BR_LLSC_2_5.
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

RuleResult_e BR_LLSC_2_5( MYKI_BR_ContextData_t *pData )
{
    MYKI_TAControl_t       *pMYKI_TAControl             = NULL;
    MYKI_Directory_t       *pDirectory                  = NULL;
    MYKI_TAProduct_t       *pMYKI_TAProduct             = NULL;
    CT_CardInfo_t          *pCardInfo                   = NULL;
    int                     errcode                     = 0;
    int                     dirIndex                    = 0;
    U8_t                    searchActionSequenceNumber  = 0;
    U16_t                   searchProductSerialNumber   = 0;
    ProductUpdate_t         foundProductUpdate;

    CsDbg( BRLL_RULE, "BR_LLSC_2_5 : Start (Process Actionlist Enable/Disable Product Autoload)" );

    if ( ! pData )
    {
        CsErrx( "BR_LLSC_2_5 : Invalid argument(s)" );
        return RULE_RESULT_ERROR;
    }

    if ( MYKI_CS_TAControlGet( &pMYKI_TAControl ) < 0 )
    {
        CsErrx( "BR_LLSC_2_5 : MYKI_CS_TAControlGet() failed" );
        return RULE_RESULT_ERROR;
    }

    if ( MYKI_CS_GetCardInfo( &pCardInfo ) < 0 )
    {
        CsErrx( "BR_LLSC_2_5 : MYKI_CS_GetCardInfo() failed" );
        return RULE_RESULT_ERROR;
    }

    memset( &foundProductUpdate, 0, sizeof( ProductUpdate_t ) );

    /*  PRE-CONDITIONS */
    {
        for ( dirIndex = 1; dirIndex < DIMOF( pMYKI_TAControl->Directory ); dirIndex++ )
        {
            if ( myki_br_GetCardProduct( dirIndex, &pDirectory, NULL ) < 0 )
            {
                CsErrx( "BR_LLSC_2_5 : myki_br_GetCardProduct(%d) failed", dirIndex );
                return RULE_RESULT_ERROR;
            }   /*  end-of-if */

            if ( pDirectory->Status == TAPP_CONTROL_DIRECTORY_STATUS_UNUSED )
            {
                continue;
            }   /*  end-of-if */

            if ( myki_br_GetCardProduct( dirIndex, &pDirectory, &pMYKI_TAProduct ) < 0 )
            {
                CsErrx( "BR_LLSC_2_5 : myki_br_GetCardProduct(%d) failed", dirIndex );
                return RULE_RESULT_ERROR;
            }   /*  end-of-if */

            searchActionSequenceNumber  = ( pMYKI_TAProduct->ActionSeqNo == 15 ? 1 : ( pMYKI_TAProduct->ActionSeqNo + 1 ) );
            searchProductSerialNumber   = pDirectory->SerialNo;

            // populate the ActionList structure
            if ( pData->ActionList.type == ACTION_PRODUCT_UPDATE_NONE )
            {
                if ( pData->ActionList.actionlist.productUpdate.actionSequenceNo == searchActionSequenceNumber &&
                     pData->ActionList.actionlist.productUpdate.serialNo         == searchProductSerialNumber  )
                {
                    /*  Found the desired actionlist in cache => process it */
                    CsDbg( BRLL_RULE, "BR_LLSC_2_5 : found desired actionlist in cache" );
                    memcpy( &foundProductUpdate, &pData->ActionList.actionlist.productUpdate, sizeof( ProductUpdate_t ) );
                    break;
                }
                pData->ActionList.type  = ACTION_NULL;
            }   /*  end-of-if */

            if ( ( errcode = myki_br_GetActionlist( pCardInfo->SerialNumber, ACTION_PRODUCT_UPDATE_NONE, searchActionSequenceNumber, searchProductSerialNumber, pData ) ) < 0 )
            {
                CsDbg( BRLL_RULE, "BR_LLSC_2_5 : myki_br_GetActionlist returns error %d", errcode );
                return RULE_RESULT_ERROR;
            }   /*  end-of-if */

            if ( pData->ActionList.type == ACTION_PRODUCT_UPDATE_NONE )
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
        }   /*  end-of-for */

        if ( foundProductUpdate.serialNo != 0 )
        {
            /*  A TAppUpdate/None actionlist found.. */
            pData->ActionList.type  = ACTION_PRODUCT_UPDATE_NONE;
            memcpy( &pData->ActionList.actionlist.productUpdate, &foundProductUpdate, sizeof( ProductUpdate_t ) );
        }
        else
        {
            /*  No TAppUpdate/None actionlist for existing product(s) found,
                check for TAppUpdate/None actionlist of any product for this card */
            if ( ( errcode = myki_br_GetActionlist( pCardInfo->SerialNumber, ACTION_PRODUCT_UPDATE_NONE, 0 /*ANY*/, 0 /*ANY*/, pData ) ) < 0 )
            {
                CsDbg( BRLL_RULE, "BR_LLSC_2_5 : myki_br_GetActionlist returns error %d", errcode );
                return RULE_RESULT_ERROR;
            }   /*  end-of-if */
        }   /*  end-of-if */

        /*  1.  An actionlist entry exists for the card UID.
            2.  The actionlist type is equal to ProductUpdate/None (autoload enable/disable). */
        if ( pData->ActionList.type != ACTION_PRODUCT_UPDATE_NONE )
        {
            CsDbg( BRLL_RULE, "BR_LLSC_2_5 : Bypassed - ActionList.type is not ACTION_PRODUCT_UPDATE_NONE." );
            pData->ReturnedData.bypassCode  = BYPASS_CODE( 2, 5, 2, 0 );
            return RULE_RESULT_BYPASSED;
        }

        /*  3.  The actionlist action sequence number is between 1 and 15. */
        if ( pData->ActionList.actionlist.productUpdate.actionSequenceNo < 1 || pData->ActionList.actionlist.productUpdate.actionSequenceNo > 15 )
        {
            CsDbg( BRLL_RULE, "BR_LLSC_2_5 : Bypassed - ActionList.actionSeqNo (%d) is out of range.",
                pData->ActionList.actionlist.productUpdate.actionSequenceNo );
            pData->ReturnedData.bypassCode  = BYPASS_CODE( 2, 5, 3, 0 );
            return RULE_RESULT_BYPASSED;
        }

        /*  NOTE:   Have to do step 6 before steps 4 and 5! */

        /*  6.  A product exists on the smartcard with a product serial number
                equal to the product serial number in the actionlist. */
        if ( ( dirIndex = myki_br_GetDirWithSerial( pMYKI_TAControl, pData->ActionList.actionlist.productUpdate.serialNo ) ) == 0 )
        {
            /*  If not, generate a FailureResponse of type ProductNotFound. */
            if ( myki_br_ldt_ProductUpdate_None_FailureResponse( pData, MYKI_UD_FAILURE_REASON_PRODUCT_NOT_FOUND ) < 0 )
            {
                CsErrx( "BR_LLSC_2_5 : myki_br_ldt_ProductUpdate_None_FailureResponse() failed" );
                return RULE_RESULT_ERROR;
            }   /* end-of-if */

            CsDbg( BRLL_RULE, "BR_LLSC_2_5 : Bypassed - Cannot find the product with serial number 0X%X",
                pData->ActionList.actionlist.productUpdate.actionSequenceNo );
            pData->ReturnedData.bypassCode  = BYPASS_CODE( 2, 5, 6, 0 );
            return RULE_RESULT_BYPASSED;
        }

        if ( myki_br_GetCardProduct( dirIndex, &pDirectory, &pMYKI_TAProduct ) < 0 )
        {
            CsErrx( "BR_LLSC_2_5 : myki_br_GetCardProduct(%d) failed", dirIndex );
            return RULE_RESULT_ERROR;
        }   /* end-of-if */

        /*  4.  If the product action sequence number is equal to 15:
                a.  The actionlist action sequence number is equal to 1.
            5.  Else:
                a.  The actionlist action sequence number is equal to the product
                    action sequence number + 1. */
        CsDbg( BRLL_RULE, "BR_LLSC_2_5 : pMYKI_TAProduct->ActionSeqNo = %d, pData->ActionList.actionSeqNo = %d",
            pMYKI_TAProduct->ActionSeqNo, pData->ActionList.actionlist.productUpdate.actionSequenceNo );

        if ( pMYKI_TAProduct->ActionSeqNo == 15 )
        {
            if ( pData->ActionList.actionlist.productUpdate.actionSequenceNo != 1 )
            {
                CsDbg( BRLL_RULE, "BR_LLSC_2_5 : Bypassed - pMYKI_TAProduct->ActionSeqNo = 15, ActionList.actionSeqNo != 1");
                pData->ReturnedData.bypassCode  = BYPASS_CODE( 2, 5, 4, 0 );
                return RULE_RESULT_BYPASSED;
            }
        }
        else
        if ( pData->ActionList.actionlist.productUpdate.actionSequenceNo != ( pMYKI_TAProduct->ActionSeqNo + 1 ) )
        {
            CsDbg( BRLL_RULE, "BR_LLSC_2_5 : Bypassed - pMYKI_TAProduct->ActionSeqNo + 1 != ActionList.actionSeqNo");
            pData->ReturnedData.bypassCode  = BYPASS_CODE( 2, 5, 5, 0 );
            return RULE_RESULT_BYPASSED;
        }

        /*  7.  The product status is not equal to Blocked. */
        if ( pDirectory->Status == TAPP_CONTROL_DIRECTORY_STATUS_BLOCKED )
        {
            /*  If it is blocked, generate a FailureResponse of type ProductBlocked. */
            if ( myki_br_ldt_ProductUpdate_None_FailureResponse( pData, MYKI_UD_FAILURE_REASON_PRODUCT_BLOCKED ) < 0 )
            {
                CsErrx( "BR_LLSC_2_5 : myki_br_ldt_ProductUpdate_None_FailureResponse() failed" );
                return RULE_RESULT_ERROR;
            }   /* end-of-if */

            CsDbg( BRLL_RULE, "BR_LLSC_2_5 : Bypassed - Product is already blocked.");
            pData->ReturnedData.bypassCode  = BYPASS_CODE( 2, 5, 7, 0 );
            return RULE_RESULT_BYPASSED;
        }
    }
 
    /*  PROCESSING */
    {
        /*  1.  Perform a ProductUpdate/None (autoload enable/disable) transaction
                    for the product with the relevant serial number. */   
        if ( myki_br_ldt_Actionlist( pData ) < 0 )
        {
            CsErrx( "BR_LLSC_2_5 : myki_br_ldt_Actionlist() failed" );
            return RULE_RESULT_ERROR;
        }

        /*  NOTE:   TAppProduct.ActionSeqNo is updated by LDT */
    }

    /*  POST-CONDITIONS */
    {
        /*  1.  Product autoload is enabled/disabled for the specified product on the smartcard. */
        pData->ReturnedData.actionApplied       = TRUE;
    }

    CsDbg( BRLL_RULE, "BR_LLSC_2_5 : Executed" );
    return RULE_RESULT_EXECUTED;
}

