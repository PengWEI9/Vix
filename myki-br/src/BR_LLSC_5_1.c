/**************************************************************************
*   ID :  BR_LLSC_5_1
*    T-Purse Minimum Balance e-Pass
*    1.  TAppControl.Directory.Status
*    2.  TAppTProduct.EndDateTime
*    3.  TAppTProduct.ZoneLow
*    4.  TAppTProduct.ZoneHigh
*    5.  TAppTPurseBalance.Balance
*    6.  Static.TransportMode
*    7.  Dynamic.CurrentZone
*    8.  Dynamic.CurrentDateTime
*    9.  Tariff.TPurseMinBalanceEPass
*    10.  Tariff.PremiumSurcharge
*
*   Pre-Conditions
*    1.  An e-Pass product exists on the smartcard where:
*         a.  The product expiry(2)  is greater than or equal to the current date/time(8)  (the product has not expired).
*         b.  Based upon the current location; considering the inner, outer and actual zones verify that the location is between the e-Pass product's low zone(3) 
*             and the product high zone(4) .
*         c.  The product status(1)  is Inactive and there ares no e-Pass with product status(1)  Active, or the e-Pass has product status(1)  Active.
*    2.  The T-Purse balance(5)  is less than the minimum balance value for e-Pass usage(9) , for the current zone(7)  and mode of travel(6) .
*    3.  If the premium surcharge field(10)  is  not 0 - i.e there is a surcharge defined then
*         a.  The premium surcharge amount is greater  than the current t-purse balance(5) ; ie there is insuffent t-purse balance
* 
*   Description
*    1.  Reject the smartcard.
* 
*   Post-Conditions
*    1.  The smartcard is rejected.
* 
*   Devices
*    Fare payment devices
*
*
*    Business Context
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

RuleResult_e BR_LLSC_5_1( MYKI_BR_ContextData_t *pData )
{
    MYKI_TAControl_t            *pMYKI_TAControl;
    MYKI_TAPurseBalance_t       *pMYKI_TAPurseBalance;
    U8_t                         dir;
    U8_t                         active = 0;
    U8_t                         productsFound = 0;

    CsDbg( BRLL_RULE, "BR_LLSC_5_1 : Start (T-Purse Minimum Balance e-Pass)" );

    if ( ! pData )
    {
        CsErrx( "BR_LLSC_5_1 : Invalid argument(s)" );
        return RULE_RESULT_ERROR;
    }

    if ( MYKI_CS_TAControlGet( &pMYKI_TAControl ) < 0 )
    {
        CsErrx( "BR_LLSC_5_1 : MYKI_CS_TAControlGet() failed" );
        return RULE_RESULT_ERROR;
    }

    if ( MYKI_CS_TAPurseBalanceGet( &pMYKI_TAPurseBalance ) < 0 )
    {
        CsErrx( "BR_LLSC_5_1 : MYKI_CS_TAPurseBalanceGet() failed" );
        return RULE_RESULT_ERROR;
    }

    //    1.  An e-Pass product exists on the smartcard where:
    //    a.  The product expiry(2)  is greater than or equal to the current date/time(8)  (the product has not expired).
    //    b.  Based upon the current location; considering the inner, outer and actual zones
    //         verify that the location is between the e-Pass product's low zone(3) 
    //         and the product high zone(4) .
    //    c.  The product status(1)  is Inactive and there ares no e-Pass with product status(1)  Active,
    //         or the e-Pass has product status(1)  Active.

    for ( dir = 1; dir < DIMOF( pMYKI_TAControl->Directory ); dir++ ) // T-Purse dir entry does not have a product slot, so there's one less product slots than dir entries
    {
        MYKI_Directory_t    *pDirectory = &pMYKI_TAControl->Directory[ dir ]; // Product slot 0-4 maps to dir entry 1-5
        MYKI_TAProduct_t    *pProduct;

        if( ( pDirectory->Status == TAPP_CONTROL_DIRECTORY_STATUS_ACTIVATED || pDirectory->Status == TAPP_CONTROL_DIRECTORY_STATUS_ISSUED) &&
            ( myki_br_cd_GetProductType( pDirectory->ProductId ) == PRODUCT_TYPE_EPASS ) )
        {    
            if (MYKI_CS_TAProductGet( (U8_t)( dir - 1 ), &pProduct ) >= 0)
            {
                if (pProduct->EndDateTime < pData->DynamicData.currentDateTime)
                {
                    continue;
                }

                // Condition 1.b not met
                if (pData->DynamicData.currentZone > pProduct->ZoneHigh ||
                    pData->DynamicData.currentZone < pProduct->ZoneLow )
                {
                    continue;
                }

                // Condition 1.c  equals to "all products are inactive or only one active"
                if (pDirectory->Status == TAPP_CONTROL_DIRECTORY_STATUS_ACTIVATED)
                {
                    active++;
                }        
                productsFound++;
            }
        }
    }

    // 1.c  not met
    if ( active > 1 )
    {
        pData->ReturnedData.rejectReason = MYKI_BR_REJECT_REASON_NOT_APPLICABLE;
        CsDbg(BRLL_RULE, "BR_LLSC_5_1 : Executed: Too many active products found.");
        return RULE_RESULT_EXECUTED;
    }
    
    if(productsFound==0)
    {
        CsDbg(BRLL_RULE, "BR_LLSC_5_1 : Bypass: No e-Pass product exists on the smartcard");
        return RULE_RESULT_BYPASSED;
    }
   
    //    2.  The T-Purse balance(5)  is less than the minimum balance value for e-Pass usage(9) ,
    //         for the current zone(7)  and mode of travel(6) .
    if (pMYKI_TAPurseBalance->Balance < pData->Tariff.TPurseMinBalanceEPass)
    {
        pData->ReturnedData.rejectReason = MYKI_BR_REJECT_REASON_INSUFFICIENT_FUND;
        CsDbg(BRLL_RULE, "BR_LLSC_5_1 : Executed: T-Purse balance >= e-Pass minimum balance %d < %d", pMYKI_TAPurseBalance->Balance, pData->Tariff.TPurseMinBalanceEPass);
        return RULE_RESULT_EXECUTED;
    }

    //    3.  If the premium surcharge field(10)  is  not 0 - i.e there is a surcharge defined then
    //    a.  The premium surcharge amount is greater than the current t-purse balance(5) ; ie there is insufficient t-purse balance
    if (pData->Tariff.premiumSurcharge == 0)
    {
        CsDbg(BRLL_RULE, "BR_LLSC_5_1 : Bypass: premiumSurcharge is 0");
        return RULE_RESULT_BYPASSED;
    }

    // 3.a
    if (pData->Tariff.premiumSurcharge > pMYKI_TAPurseBalance->Balance)
    {
        pData->ReturnedData.rejectReason = MYKI_BR_REJECT_REASON_INSUFFICIENT_FUND;
        CsDbg(BRLL_RULE, "BR_LLSC_5_1 : Executed: premiumSurcharge balance <= T-purse balance %d>%d", pData->Tariff.premiumSurcharge, pMYKI_TAPurseBalance->Balance);
        return RULE_RESULT_EXECUTED;
    }

    CsDbg(BRLL_RULE, "BR_LLSC_5_1 : Bypass: premiumSurcharge is less than current balance");
    return RULE_RESULT_BYPASSED;
}

