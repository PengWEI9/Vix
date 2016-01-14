/**************************************************************************
*   ID :  BR_LLSC_10_5
*    Travel Access Pass Scan-On Extend Provisional Product
*    1.  TAppControl.ProductInUse
*    2.  TAppControl.ProvisionalFare
*    3.  TAppControl.PassengerCode
*    4.  TAppTProduct.ControlBitmap
*    5.  TAppTProduct.ZoneLow
*    6.  TAppTProduct.ZoneHigh
*    7.  TAppTProduct.LastUsageZone
*    8.  Dynamic.CurrentZone
*    9.  Dynamic.CurrentDateTime
*    10.  Dynamic.ProvisionalZoneLow
*    11.  Dynamic.ProvisionalZoneHigh
*    12.  Tariff.CitySaverZoneFlag
*    13.  Dynamic.usageLog
*
*    Pre-Conditions
*    1.  The product in use field(1) is not 0.
*    2.  The provisional bit of the product control bitmap(4) of the product in use(1) is 1.
*
*    Description
*    1.  Perform a Product Update/Extend transaction for the provisional product:
*    a.  Set the provisional fare(2) to 0
*    b.  Set the product low zone(5) to the zone of origin(10) or declared destination zone, whichever is lower. If the resulting product low zone(5) minus 1 is
*        marked as City Saver(12), decrement the resulting product low zone(5) by 1.
*    c.  Set the product high zone(6) to the zone of origin(10) or declared destination zone, whichever is higher. If the resulting product high zone(6) is marked as
*        City Saver(12), increment the resulting product high zone(6) by 1.
*    d.  Set the purchase value to 0
*
*    2.  UsageLog
*    a.  Definition:
*    i.  TxType = Type On board= 20,
*    ii.  ProviderID =  current ServiceProviderID(15)
*    iii.  TxDateTime = Current date time(10)
*    iv.  Location.EntryPoint = EntryPointID(11)
*    v.  Location.Route=  LineID(12)
*    vi.  Location.Stop= Stopid(13)
*
*    b.  Value:
*    i.  TxValue = 0
*    ii.  NewTPurseBalance = current tpurse balance(19)
*    c.  Usage
*    i.  Zone = Current zone(16)
*    ii.  ProvisionalValidationStatus No Change
*    d.  Product:
*    i.  ProductIssuerId - as  used in step 1
*    ii.  ProductId - as used in step 1
*    iii.  ProductSerialNo - as used in step 1
*
*    Post-Conditions
*    1.  The provisional product is amended to include the declared zone range of travel.
*
*      Devices
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

RuleResult_e BR_LLSC_10_5( MYKI_BR_ContextData_t *pData )
{
    MYKI_TAControl_t    *pMYKI_TAControl;
    MYKI_TAProduct_t    *pProduct;
    MYKI_Directory_t    *pDirectory;

    CsDbg( BRLL_RULE, "BR_LLSC_10_5 : Start (Travel Access Pass Scan-On Extend Provisional Product)" );

    if ( ! pData )
    {
        CsErrx( "BR_LLSC_10_5 : Invalid argument(s)" );
        return RULE_RESULT_ERROR;
    }

    if ( MYKI_CS_TAControlGet( &pMYKI_TAControl ) < 0 )
    {
        CsErrx( "BR_LLSC_10_5 : MYKI_CS_TAControlGet() failed" );
        return RULE_RESULT_ERROR;
    }

    //  1. The product in use field(1) is not 0 (a product is in a scanned-on state).

    if ( pMYKI_TAControl->ProductInUse == 0 )
    {
        CsDbg( BRLL_RULE, "BR_LLSC_10_5 : Bypass - No products in use" );
        return RULE_RESULT_BYPASSED;
    }

    //  Get a pointer to the product in use

    if ( myki_br_GetCardProduct( pMYKI_TAControl->ProductInUse, &pDirectory, &pProduct ) < 0 )
    {
        CsErrx( "BR_LLSC_10_5 : myki_br_GetCardProduct( %d ) failed", pMYKI_TAControl->ProductInUse );
        return RULE_RESULT_ERROR;
    }

    //  2. The provisional bit on the product control bitmap(2) of the product in use(1) is set to 1.

    if ( ! ( pProduct->ControlBitmap & TAPP_TPRODUCT_CONTROL_PROVISIONAL_BITMAP ) )
    {
        CsDbg( BRLL_RULE, "BR_LLSC_10_5 : Bypass - Product Provisional Bit not set" );
        return RULE_RESULT_BYPASSED;
    }

    //
    //  The body of this BR has not been implemented
    //  The rule does not appear to be used in any sequence
    //
    CsErrx( "BR_LLSC_10_5 : Not implemented. Not used in any sequence" );
    return RULE_RESULT_ERROR;
}



