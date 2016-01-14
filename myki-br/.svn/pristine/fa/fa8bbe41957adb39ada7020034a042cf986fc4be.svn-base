/**************************************************************************
*   ID :  BR_LLSC_5_3
*    Duplicate Scan-On
*
* Data Fields
*    1. TAppControl.ProductInUse
*    2. TAppTProduct.LastUsageLineId
*    3. TAppTProduct.LastUsageProviderId
*    4. Static.TransportMode
*    5. Dynamic.LineId
*    6. Dynamic.EntryPointID
*    7. TAppTproduct.LastUsageEntryPointID
*    8. Dynamic.EntryPointID
*
* Pre-Conditions
*    1.  The product in use field(1) is not 0 (a product is in a scanned-on state).
*    2.  The mode of transport as determined by the scan-on service provider(3) of the product in use(1) is equal to the mode of transport of the device(4).
*    3. If either of the following is true:
*        a.  If the mode of transport of the device(4) is not equal to Rail and the scan-on route/line(2)     is equal to the route/line of the device(5).
*        b.  If the mode of transport of the device(4) is     equal to Rail and the scan on entry point ID(7) is equal to the entry point Id of the device(8).
*
*   Description
*    1. Reject the smartcard.
*
*   Post-Conditions
*    1. The smartcard is rejected.
*
*   Devices
*    Entry gates
*
 ***********************************************************************/

#include <cs.h>                 // For debug logging
#include <myki_cardservices.h>  // Myki Card Services
#include <LDT.h>                // Logical Device Transaction
#include "BR_Common.h"

#include "myki_br_rules.h"      // API for all BR_xxx modules
#include "BR_Common.h"          // Common utilities, macros, etc

//=============================================================================
//
//
//
//=============================================================================

RuleResult_e BR_LLSC_5_3( MYKI_BR_ContextData_t *pData )
{
    MYKI_TAControl_t  *pMYKI_TAControl;
    MYKI_TAProduct_t  *pMYKI_TAProduct;

    CsDbg( BRLL_RULE,"BR_LLSC_5_3 : Start (Duplicate Scan-On)" );

    if ( ! pData )
    {
        CsErrx( "BR_LLSC_5_3 : Invalid argument(s)" );
        return RULE_RESULT_ERROR;
    }

    if ( MYKI_CS_TAControlGet( &pMYKI_TAControl ) < 0 )
    {
        CsErrx( "BR_LLSC_5_3 : MYKI_CS_TAControlGet() failed" );
        return RULE_RESULT_ERROR;
    }

    // 1.  The product in use field(1)  is not 0 (a product is in a scanned-on state).
    if (pMYKI_TAControl->ProductInUse == 0)
    {
        CsDbg(BRLL_RULE, "BR_LLSC_5_3 : No Product in Use");
        pData->ReturnedData.bypassCode  = BYPASS_CODE( 5, 3, 1, 0 );
        return RULE_RESULT_BYPASSED;
    }

    //  Get a pointer to the product in use

    if ( MYKI_CS_TAProductGet( (U8_t)( pMYKI_TAControl->ProductInUse - 1 ), &pMYKI_TAProduct ) < 0 )
    {
        CsErrx( "BR_LLSC_5_3 : MYKI_CS_TAProductGet( %d ) failed", pMYKI_TAControl->ProductInUse );
        return RULE_RESULT_ERROR;
    }


    //2.  The mode of transport as determined by the scan-on service provider(3) 
    //     of the product in use(1)  is equal to the mode of transport of the device(4)  ,
    if ( pData->InternalData.TransportMode != myki_br_cd_GetTransportModeForProvider( pMYKI_TAProduct->LastUsage.ProviderId ) )
    {
        CsDbg( BRLL_RULE, "BR_LLSC_5_3 : Bypass: transportMode != LastUsage.ProviderId");
        pData->ReturnedData.bypassCode  = BYPASS_CODE( 5, 3, 2, 0 );
        return RULE_RESULT_BYPASSED;
    }


    //3.  If either of the following is true:
    //    a.  If the mode of transport of the device(4)  is not equal to Rail and the
    //         scan-on route/line(2)  is equal to the route/line of the device(5) .
    //    b.  If the mode of transport of the device(4)  is equal to Rail and the
    //         scan on entry point ID(7)  is equal to the entry point Id of the device(8) 
    //
    //  Modified slightly for consistency with other rules, by calling the common function.
    //  Difference is that the common function also checks entryPointId for non-RAIL devices.

    if ( ! myki_br_CurrentLocationIsScanOnLocationOrRoute( pData, pMYKI_TAProduct, FALSE ) )
    {
        CsDbg( BRLL_RULE, "BR_LLSC_5_3 : Bypass - Scan-on location != Current location" );
        pData->ReturnedData.bypassCode  = BYPASS_CODE( 5, 3, 3, 0 );
        return RULE_RESULT_BYPASSED;
    }

    // Reject the smart card
    pData->ReturnedData.rejectReason = MYKI_BR_REJECT_REASON_ALREADY_TOUCH_ON;
    CsDbg( BRLL_RULE, "BR_LLSC_5_3 : Executed");
    return RULE_RESULT_EXECUTED;
}


