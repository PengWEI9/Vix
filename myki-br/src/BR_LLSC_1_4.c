/**************************************************************************
*   ID :  BR_LLSC_1_4
*
* Manage Product Expiry
*
* Data Fields
*    1. TAppControl.Directory.Status
*    2. TAppTProduct.EndDateTime
*    3. Dynamic.CurrentDateTime
*    4. TAppTProduct.ControlBitmap
*    5. TAppControl.ProductInUse
*
* Pre-Conditions
*    1. For each product check that
*        a. The product expiry(2) less  than or equal to the current date/time(3)
*        b. The product status(1) is Active or Issued.
*        c. The provisional bit on the product control bitmap(4) of the product is not set
*        d. The provisional bit on the product control bitmap(4) of the product is set however this is not the product in use(5).
*        e. The autoload bit in the product control bitmap(4) is set to 0.
*
* Description
*    1. For each product:
*    a. Perform a ProductUpdate/Invalidate transaction.
*
* Post-Conditions
*    1. One or more expired products has been invalidated.
*
* Devices
*    Fare payment devices
*
 ***********************************************************************/

#include <cs.h>                 // For debug logging
#include <myki_cardservices.h>  // Myki Card Services
#include <LDT.h>                // Logical Device Transaction

#include "myki_br_rules.h"      // API for all BR_xxx modules
#include "BR_Common.h"          // Common utilities, macros, etc

//=============================================================================
//
//
//
//=============================================================================

RuleResult_e BR_LLSC_1_4( MYKI_BR_ContextData_t *pData )
{
    int                 preconditionsPassedFlag = FALSE;
    MYKI_TAControl_t    *pMYKI_TAControl;
    MYKI_Directory_t    *pDirectory;
    MYKI_TAProduct_t    *pProduct;
    ProductIterator     iAny;

    CsDbg( BRLL_RULE, "BR_LLSC_1_4 : Start (Manage Product Expiry)" );

    if ( ! pData )
    {
        CsErrx( "BR_LLSC_1_4 : Invalid argument(s)" );
        return RULE_RESULT_ERROR;
    }

    //check Mandatory Data Fields

    //Pre-conditions

    //this check is from reference code..
    if ( ! pData->DynamicData.isTransit )  // Application decides whether the card is a transit card or not based on device mode and card details
    {
        CsDbg( BRLL_RULE, "BR_LLSC_1_4 : Bypass - Not a transit card" );
        return RULE_RESULT_BYPASSED;
    }

    if ( MYKI_CS_TAControlGet( &pMYKI_TAControl ) < 0 )
    {
        CsErrx( "BR_LLSC_1_4 : MYKI_CS_TAControlGet() failed" );
        return RULE_RESULT_ERROR;
    }

    // Run pre-condition checks for all products

    myki_br_InitProductIterator( pMYKI_TAControl, &iAny, PRODUCT_TYPE_UNKNOWN );

    while ( myki_br_ProductIterate( &iAny ) > 0 )
    {
        pDirectory = iAny.pDirectory;
        pProduct   = iAny.pProduct;

        //  a.  The product expiry2 less  than or equal to the current date/time3
        if ( pProduct->EndDateTime > pData->DynamicData.currentDateTime )
        {
            CsDbg( BRLL_CHOICE, "BR_LLSC_1_4 : Product %d not expired (%d > %d): skip ", iAny.index, pProduct->EndDateTime, pData->DynamicData.currentDateTime );
            continue;
        }

        //  b.  The product status1 is Active or Issued.
        if ((pDirectory->Status != TAPP_CONTROL_DIRECTORY_STATUS_ACTIVATED ) && ( pDirectory->Status != TAPP_CONTROL_DIRECTORY_STATUS_ISSUED  ) )
        {
            CsDbg( BRLL_CHOICE, "BR_LLSC_1_4 : Product %d not active or issued : skip ", iAny.index );
            continue;
        }

        //  c.  The provisional bit on the product control bitmap4 of the product is not set
        if ( pProduct->ControlBitmap & TAPP_TPRODUCT_CONTROL_PROVISIONAL_BITMAP )
        {
            //  d.  The provisional bit on the product control bitmap4 of the product is set however this is not the product in use5.
            if ( pMYKI_TAControl->ProductInUse == iAny.index )  // Provisional is set, but this is the product in use, skip
            {
                CsDbg( BRLL_CHOICE, "BR_LLSC_1_4 : Product %d is provisional, but is also product in use : skip ", iAny.index );
                continue;
            }
        }

        //  e.  The autoload bit in the product control bitmap4 is set to 0.
        if ( pProduct->ControlBitmap & TAPP_TPRODUCT_CONTROL_AUTOLOAD_BITMAP )
        {
            CsDbg( BRLL_CHOICE, "BR_LLSC_1_4 : Product %d is autoload : skip ", iAny.index );
            continue;
        }


        //perform update/invalidate transaction
        preconditionsPassedFlag = TRUE;
        CsDbg( BRLL_RULE, "BR_LLSC_1_4 : Invalidate Expired product (index %d, expiry = %d, current = %d)", iAny.index, pProduct->EndDateTime, pData->DynamicData.currentDateTime );

        if ( myki_br_ldt_ProductUpdate_Invalidate( pData, pDirectory ) < 0 )
        {
            CsErrx( "BR_LLSC_1_4 : myki_br_ldt_ProductUpdate_Invalidate() failed" );
            return RULE_RESULT_ERROR;
        }
    }

    //  Pre-conditions (final stage)

    //  Bypass if no products matched the pre-conditions (ie, no products were invalidated)

    if ( ! preconditionsPassedFlag )
    {
         CsDbg( BRLL_RULE, "BR_LLSC_1_4 : Bypass - Pre-conditions not met for any product." );
         return RULE_RESULT_BYPASSED;
    }

    //  Execute if one or more products matched the pre-conditions (the actual work's already been done)

    CsDbg( BRLL_RULE, "BR_LLSC_1_4 : Executed" );
    return RULE_RESULT_EXECUTED;
}
