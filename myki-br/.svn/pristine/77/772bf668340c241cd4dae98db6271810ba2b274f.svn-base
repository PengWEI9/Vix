/* -*- mode: c; tabs: 4; -*- */
/*=============================================================================
**
**    Vix Technology                   Licensed software
**    (C) 2015                         All rights reserved
**
**=============================================================================
**
**  Project/Product : MBU
**  Filename        : BR_LLSC_4_20.c
**  Author(s)       : ???
**
**  ID              : BR_LLSC_4_20 - NTS0177 v8.0
**
**  Name            : Determine trip extension
**
**  Data Fields     :
**
**      1. TAppTProduct.EndDateTime
**      2. Dynamic.CurrentDateTime
**      3. TAppControl.ProductInUse
**      4. TAppTProduct.ProductControlBitmap
**      5. Tariff.MaximiumTripTolerance
**      6. Dynamic.EntryPointID
**      7. TAppTproduct.LastUsageEntryPointID
**      8. Static.TransportMode
**      9. Static.ServiceProviderId
**      
**  Pre-Conditions  :
**
**      1. The product in use(3) is set
**      2. The product in use(3) is of type provisional as indicated by the product control bitmap(4)
**      3. The expiry datetime(1) of the product in use(3) is less than the current date time(2) - the product is expired.
**      4. If the mode of transport(8) of the current device is BUS or TRAM and the entypoint Id(6) of the current device is equal to the entry point of the provisional product⁷ or If the mode of transport(7) of the both the provisional product service provider and the current device is RAIL.
**      5. The TripTolerance(5) for this devices configured service provider(9) is not 0 (zero) .
**      6. If the current product expiry(1) plus the TripTolerance(5) (in minutes) is less than the current date time(2) -then the grace period has not expired.
**
**  Description     :
**
**      1.  Perform a productUpdate/Extend on the Product in Use(3)
**          a.Set the provisional expiry date time to the current Date time (2), no further travel is permitted on this product
**
**  Post-Conditions :
**
**      1.  Consideration for Extra Ordinary long trips has been applied
**
**  Devices         :
**
**      Fare payment devices
**
**
**


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
**   Vers.  Date        Aut.      Type     Description
**   -----  ----------  ----     -------  ---------------------------------------- 
**    1.00  18.12.15    MM.NTT   New      Updated to refect changes from NTS0177 7.3 to NTS0177 8.0 
**
**===========================================================================*/

#include <cs.h>                 // For debug logging
#include <myki_cardservices.h>  // Myki Card Services
#include <LDT.h>                // Logical Device Transaction

#include "myki_br_rules.h"      // API for all BR_xxx modules
#include "BR_Common.h"          // Common utilities, macros, etc

/*==========================================================================*
**
**  BR_LLSC_4_20
**
**  Description     :
**      Implements business rule BR_LLSC_4_20.
**
**  Parameters      :
**      pData               [I/O]   BR context data
**
**  Returns         :
**      RULE_RESULT_EXECUTED
**      RULE_RESULT_BYPASSED
**      RULE_RESULT_ERROR
**
**  Notes          :
**
**==========================================================================*/

RuleResult_e BR_LLSC_4_20( MYKI_BR_ContextData_t *pData )
{
    MYKI_TAControl_t    *pMYKI_TAControl;
    MYKI_Directory_t    *pDirectory;
    MYKI_TAProduct_t    *pProduct;

    CsDbg( BRLL_RULE, "BR_LLSC_4_20 : Start (Consideration of extra-ordinary trip tolerance)" );

    if ( ! pData )
    {
        CsErrx( "BR_LLSC_4_20 : Invalid argument(s)" );
        return RULE_RESULT_ERROR;
    }

    if ( MYKI_CS_TAControlGet( &pMYKI_TAControl ) )
    {
        CsErrx( "BR_LLSC_4_20 : MYKI_CS_TAControlGet() failed" );
        return RULE_RESULT_ERROR;
    }

    //--------------------------------------------------------------------------
    //  Pre-Conditions
    //  1 1. The product in use(3) is set
    if ( pMYKI_TAControl->ProductInUse == 0 )
    {
        CsDbg( BRLL_RULE, "BR_LLSC_4_20 : Bypass - No product in use" );
        return RULE_RESULT_BYPASSED;
    }

    //  2. The product in use(3) is of type provisional as indicated by the product control bitmap(4)


    if ( myki_br_GetCardProduct( pMYKI_TAControl->ProductInUse, &pDirectory, &pProduct ) < 0 )
    {
        CsErrx( "BR_LLSC_4_20 : myki_br_GetCardProduct( %d ) failed", pMYKI_TAControl->ProductInUse );
        return RULE_RESULT_ERROR;
    }

    if ( !(pProduct->ControlBitmap & TAPP_TPRODUCT_CONTROL_PROVISIONAL_BITMAP ))
    {
        CsDbg( BRLL_RULE, "BR_LLSC_4_20 : Bypass - Product Provisional Bit not set" );
        return RULE_RESULT_BYPASSED;
    }

    //  3. The expiry datetime(1) of the product in use(3) is less than the current date time(2) - the product is expired.
  
    if ( pProduct->EndDateTime >= pData->DynamicData.currentDateTime )
    {
        CsDbg( BRLL_RULE, "BR_LLSC_4_20 : Bypass - Expiry date/time (%d) >= Current date/time (%d)", pProduct->EndDateTime, pData->DynamicData.currentDateTime);
        return RULE_RESULT_BYPASSED;
    }

    //  4. If the mode of transport(8) of the current device is BUS or TRAM 
    //       and the entypoint Id(6) of the current device is equal to the entry point of the provisional product⁷ 
    //     or If the mode of transport(7) of the both the provisional product service provider and the current device is RAIL.
  
    if ( 
        (
         (pData->InternalData.TransportMode == TRANSPORT_MODE_BUS || pData->InternalData.TransportMode == TRANSPORT_MODE_TRAM) &&
         (pProduct->LastUsage.Location.EntryPointId == pData->DynamicData.entryPointId ) &&
         (pProduct->LastUsage.ProviderId == pData->StaticData.serviceProviderId )
        ) ||
       (pData->InternalData.TransportMode == TRANSPORT_MODE_RAIL && (myki_br_cd_GetTransportModeForProvider( pProduct->LastUsage.ProviderId ) == TRANSPORT_MODE_RAIL ))
       )
    {
        CsDbg( BRLL_RULE, "BR_LLSC_4_20 : Entry and Exit both not Rail or Mode Tram/Bus on Different Vehicle Service Provider");
        return RULE_RESULT_BYPASSED;
    }

    //  5. The TripTolerance(5) for this device is configured service provider(9) is not 0 (zero) .
  
    if (pData->Tariff.maximumTripTolerance == 0 )
    {
        CsDbg( BRLL_RULE, "BR_LLSC_4_20 : Bypass - Maximum Trip Tolerance ==0 (%d) ", pData->Tariff.maximumTripTolerance);
        return RULE_RESULT_BYPASSED;
    }

    //  6. If the current product expiry(1) plus the TripTolerance(5) (in minutes) is less than the current date time(2) -then the grace period has not expired.

    if ( pProduct->LastUsage.DateTime + pData->Tariff.maximumTripTolerance < pData->DynamicData.currentDateTime )
    {
        CsDbg( BRLL_RULE, "BR_LLSC_4_20 : Bypass - Scan-on date/time (%d) + Change of mind period (%d) < Current date/time (%d)", pProduct->EndDateTime, pData->DynamicData.currentDateTime);
        return RULE_RESULT_BYPASSED;
    }
                        

    //--------------------------------------------------------------------------
    // Actions
    //
        // MM Note: (TODO) Check this is the correct way to request a ProductUpdate - Copied of br_LLSC_4_19
        //  Make changes in a copy of the product
        //
        MYKI_TAProduct_t  newProduct = *pProduct;

        //  1.  Perform a ProductUpdate/Extend on the provisional product
        //  a.  Set the provisional expiry to the current date time

        newProduct.EndDateTime = pData->DynamicData.currentDateTime;

            if ( myki_br_ldt_ProductUpdate_Extend( pData, pDirectory, pProduct, &newProduct, FALSE, FALSE, FALSE ) < 0 )
            {
                CsErrx( "BR_LLSC_4_20 : myki_br_ldt_ProductUpdate_Extend() failed" );
                return RULE_RESULT_ERROR;
            }

        CsDbg( BRLL_RULE, "BR_LLSC_4_20 : Provisional Expiry:%d", pData->DynamicData.currentDateTime);


     return RULE_RESULT_EXECUTED;
}


