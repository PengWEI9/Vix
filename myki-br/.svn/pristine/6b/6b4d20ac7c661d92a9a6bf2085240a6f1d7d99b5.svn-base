/* -*- mode: c; tabs: 4; -*- */
/*=============================================================================
**
**    Vix Technology                   Licensed software
**    (C) 2014                         All rights reserved
**
**=============================================================================
**
**  Project/Product : MBU
**  Filename        : BR_LLSC_4_7.c
**  Author(s)       : ?
**
**  ID              : BR_LLSC_4_7 - NTS0177 v7.2
**
**  Name            : Missing Scan-Off Provisional product expired
**
**  Data Fields     :
**
**      1.  TAppControl.ProductInUse
**      2.  TAppTProduct.LastUsageDateTime
**      3.  Dynamic.FareRouteID
**
**  Pre-Conditions  :
**
**      1.  The product in use field1 is not 0 (a product is in a scanned-on
**          state).
**      2.  The Product control bitmap of the product in use shows a provisional
**          product
**      3.  The expiry date on the product is less than current date time4 (ie
**          the provisional product has expired).
**
**  Description     :
**
**      1.  Set the FareRouteID3 to none - no route will be considered for
**          pricing
**      2.  Perform a Forced Scan-Off.
**
**  Post-Conditions :
**
**      1.  A forced scan-off is performed.
**
**  Devices         :
**
**      Fare payment devices
**
**  Member(s)       :
**      BR_LLSC_4_7             [public]    business rule
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
**    1.01  16.09.14    ANT   Modify   MBU-1062: Set reject reason code for
**                                     ExitOnly mode.
**
**===========================================================================*/

#include <cs.h>                 // For debug logging
#include <myki_cardservices.h>  // Myki Card Services
#include <LDT.h>                // Logical Device Transaction

#include "myki_br_rules.h"      // API for all BR_xxx modules
#include "BR_Common.h"          // Common utilities, macros, etc

/*==========================================================================*
**
**  BR_LLSC_4_7
**
**  Description     :
**      Implements business rule BR_LLSC_4_7.
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

RuleResult_e BR_LLSC_4_7( MYKI_BR_ContextData_t *pData )
{
    MYKI_TAControl_t    *pMYKI_TAControl;
    MYKI_Directory_t    *pDirectory;
    MYKI_TAProduct_t    *pProduct;

    CsDbg( BRLL_RULE, "BR_LLSC_4_7 : Start (Missing Scan-Off Provisional product expired)" );

    if ( ! pData )
    {
        CsErrx( "BR_LLSC_4_7 : Invalid argument(s)" );
        return RULE_RESULT_ERROR;
    }

    if ( MYKI_CS_TAControlGet( &pMYKI_TAControl ) < 0 )
    {
        CsErrx( "BR_LLSC_4_7 : MYKI_CS_TAControlGet() failed" );
        return RULE_RESULT_ERROR;
    }

    //  1. The product in use field1 is not 0 (a product is in a scanned-on state).

    if ( pMYKI_TAControl->ProductInUse == 0 )
    {
        CsDbg( BRLL_RULE, "BR_LLSC_4_7 : Bypass - No products in use" );
        return RULE_RESULT_BYPASSED;
    }

    //  Get a pointer to the product in use

    if ( myki_br_GetCardProduct( pMYKI_TAControl->ProductInUse, &pDirectory, &pProduct ) < 0 )
    {
        CsErrx( "BR_LLSC_4_7 : myki_br_GetCardProduct( %d ) failed", pMYKI_TAControl->ProductInUse );
        return RULE_RESULT_ERROR;
    }

    //  2. The provisional bit on the product control bitmap2 of the product in use1 is set to 1.

    if ( ! ( pProduct->ControlBitmap & TAPP_TPRODUCT_CONTROL_PROVISIONAL_BITMAP ) )
    {
        CsDbg( BRLL_RULE, "BR_LLSC_4_7 : Bypass - Product Provisional Bit not set" );
        return RULE_RESULT_BYPASSED;
    }

    //  3.  The expiry date on the product is less than current date time4 (ie the provisional product has expired).

    CsDbg( BRLL_RULE, "BR_LLSC_4_7 : Expiry date/time = %d, Current date/time = %d", pProduct->EndDateTime, pData->DynamicData.currentDateTime );

    if ( pProduct->EndDateTime >= pData->DynamicData.currentDateTime )
    {
        CsDbg( BRLL_RULE, "BR_LLSC_4_7 : Bypass - Expiry date/time (%d) >= Current date/time (%d)", pProduct->EndDateTime, pData->DynamicData.currentDateTime );
        return RULE_RESULT_BYPASSED;
    }

    /*  PROCESSING */
    {
        //  1.  Set the FareRouteID3 to none - no route will be considered for pricing

        pData->DynamicData.fareRouteIdIsValid = FALSE;      // 1. Set the FareRouteID7 to none - no route will be considered for pricing

        //  2.  Perform a Forced Scan-Off (in subsequent business rules)

        if ( pData->StaticData.isEntry == FALSE &&
             pData->StaticData.isExit  != FALSE )
        {
            /*  Sect 2.2.2 NTS0177 v7.2: For Exit-Only mode, card will be rejected.
                Hence, set the reject code. */
            pData->ReturnedData.rejectReason    = MYKI_BR_REJECT_REASON_MISSING_TOUCH_ON;
        }   /*  end-of-if */
    }

    CsDbg( BRLL_RULE, "BR_LLSC_4_7 : Executed" );
    return RULE_RESULT_EXECUTED;
}

