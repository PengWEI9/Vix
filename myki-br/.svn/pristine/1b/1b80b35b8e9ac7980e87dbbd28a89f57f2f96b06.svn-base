/* -*- mode: c; tabs: 4; -*- */
/*=============================================================================
**
**    Vix Technology                   Licensed software
**    (C) 2013                         All rights reserved
**
**=============================================================================
**
**  Project/Product : MBU
**  Filename        : BR_LLSC_5_2.c
**  Author(s)       : ?
**
**  ID              : BR_LLSC_5_2 - KA0004 v7.0
**
**  Name            : T-Purse Minimum Balance Stored Value
**
**  Data Fields     :
**
**      1.  TAppControl.Directory.Status
**      2.  TAppTProduct.EndDateTime
**      3.  TAppTProduct.ZoneLow
**      4.  TAppTProduct.ZoneHigh
**      5.  TAppTPurseBalance.Balance
**      6.  Static.TransportMode
**      7.  Dynamic.CurrentZone
**      8.  Dynamic.CurrentDateTime
**      9.  Tariff.TPurseMinBalance
**      10. Tariff.PremiumSurcharge
**
**  Pre-Conditions  :
**
**      1.  The T-Purse balance is less than the minimum balance value for
**          stored value usage, for the current zone and mode of travel.
**      2.  A e-Pass product does not exists on the smartcard where:
**          a.  The product expiry is greater than or equal to the current
**              date/time (the product has not expired).
**          b.  Based upon the current location; considering the inner,
**              outer and actual zones of that location check that between
**              the e-Pass product’s low zone and the product high zone.
**          c.  The product status is Inactive and there is no e-Pass with
**              product status Active, or the e-Pass has product status1 Active.
**      3.  If the premium surcharge field10 is not 0 – i.e there is a
**          surcharge defined then
**          a.  The premium surcharge amount is greater than the current t-purse
**              balance; ie there is insufficient t-purse
**
**  Description     :
**
**      1.  Reject the smartcard.
**
**  Post-Conditions :
**
**      1.  The smartcard is rejected.
**
**  Member(s)       :
**      BR_LLSC_5_2             [public]    business rule
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
**    1.00  dd.mm.yy    ???   Create
**    1.01  27.11.13    ANT   Modify   Rectified pre-conditions.
**
**===========================================================================*/

#include <cs.h>                 // For debug logging
#include <myki_cardservices.h>  // Myki Card Services
#include <LDT.h>                // Logical Device Transaction

#include "myki_br_rules.h"      // API for all BR_xxx modules
#include "BR_Common.h"          // Common utilities, macros, etc

/*==========================================================================*
**
**  BR_LLSC_5_2
**
**  Description     :
**      Implements business rule BR_LLSC_5_2.
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

RuleResult_e BR_LLSC_5_2( MYKI_BR_ContextData_t *pData )
{
    MYKI_TAPurseBalance_t      *pMYKI_TAPurseBalance    = NULL;
    MYKI_TAControl_t           *pMYKI_TAControl         = NULL;
    MYKI_Directory_t           *pMYKI_Directory         = NULL;
    MYKI_TAProduct_t           *pMYKI_TAProduct         = NULL;
    U8_t                        dir                     = 0;
    int                         nActiveEpass            = 0;
    int                         nUsableActiveEpass      = 0;
    int                         nUsableInactiveEpass    = 0;

    CsDbg( BRLL_RULE, "BR_LLSC_5_2 : Start (T-Purse Minimum Balance Stored Value)" );

    if ( ! pData )
    {
        CsErrx( "BR_LLSC_5_2 : Invalid argument(s)" );
        return RULE_RESULT_ERROR;
    }

    if ( MYKI_CS_TAControlGet( &pMYKI_TAControl ) < 0 )
    {
        CsErrx( "BR_LLSC_5_2 : MYKI_CS_TAControlGet() failed" );
        return RULE_RESULT_ERROR;
    }

    if ( MYKI_CS_TAPurseBalanceGet( &pMYKI_TAPurseBalance ) < 0 )
    {
        CsErrx( "BR_LLSC_5_2 : MYKI_CS_TAPurseBalanceGet() failed" );
        return RULE_RESULT_ERROR;
    }

    /*  PRE-CONDITIONS */
    {
        CsDbg( BRLL_RULE, "BR_LLSC_5_2 : T-Purse balance (%d), minimum balance for a store value usage (%d)",
            pMYKI_TAPurseBalance->Balance, pData->Tariff.TPurseMinBalance );

        /*  1.  The T-Purse balance is less than the minimum balance value for stored
                value usage, for the current zone and mode of travel. */
        if ( pMYKI_TAPurseBalance->Balance >= pData->Tariff.TPurseMinBalance )
        {
            CsDbg( BRLL_RULE, "BR_LLSC_5_2 : Bypass - T-Purse balance (%d) >= minimum balance for a store value usage (%d)",
                pMYKI_TAPurseBalance->Balance, pData->Tariff.TPurseMinBalance );
            return RULE_RESULT_BYPASSED;
        }   /* end-of-if */

        /*  2.  A e-Pass product does not exists on the smartcard where: */
        for ( dir = 1; dir < DIMOF( pMYKI_TAControl->Directory ); dir++ )
        {
            if ( myki_br_GetCardProduct( dir, &pMYKI_Directory, NULL ) < 0 )
            {
                CsErrx( "BR_LLSC_5_2 : myki_br_GetCardProduct(%d) failed", dir );
                return RULE_RESULT_ERROR;
            }   /* end-of-if */

            if ( pMYKI_Directory->Status != TAPP_CONTROL_DIRECTORY_STATUS_ACTIVATED &&
                 pMYKI_Directory->Status != TAPP_CONTROL_DIRECTORY_STATUS_ISSUED    )
            {
                /*  Product is not inactive nor active */
                continue;
            }   /* end-of-if */

            if ( myki_br_cd_GetProductType( pMYKI_Directory->ProductId ) != PRODUCT_TYPE_EPASS )
            {
                /*  Product is not an e-Pass product */
                continue;
            }   /* end-of-if */

            if ( myki_br_GetCardProduct( dir, &pMYKI_Directory, &pMYKI_TAProduct ) < 0 )
            {
                CsErrx( "BR_LLSC_5_2 : myki_br_GetCardProduct(%d) failed", dir );
                return RULE_RESULT_ERROR;
            }   /* end-of-if */

            /*  a.  The product expiry is greater than or equal to the current
                    date/time (the product has not expired). */
            if ( pMYKI_TAProduct->EndDateTime < pData->DynamicData.currentDateTime )
            {
                /*  Product has expired */
                CsDbg( BRLL_RULE, "BR_LLSC_5_2 : Product(%d) expired", dir );
                continue;
            }   /* end-of-if */

            if ( pMYKI_Directory->Status == TAPP_CONTROL_DIRECTORY_STATUS_ACTIVATED )
            {
                /*  One more active e-Pass found */
                nActiveEpass++;
            }   /* end-of-if */

            /*  b.  Based upon the current location; considering the inner,
                    outer and actual zones of that location check that between
                    the e-Pass product’s low zone and the product high zone. */
            if
            (
                ( pData->DynamicData.currentZone        > pMYKI_TAProduct->ZoneHigh ||
                  pData->DynamicData.currentZone        < pMYKI_TAProduct->ZoneLow  )   &&
                ( pData->DynamicData.currentInnerZone   > pMYKI_TAProduct->ZoneHigh ||
                  pData->DynamicData.currentInnerZone   < pMYKI_TAProduct->ZoneLow  )   &&
                ( pData->DynamicData.currentOuterZone   > pMYKI_TAProduct->ZoneHigh ||
                  pData->DynamicData.currentOuterZone   < pMYKI_TAProduct->ZoneLow  )
            )
            {
                /*  e-Pass does not cover current location zones */
                CsDbg( BRLL_RULE, "BR_LLSC_5_2 : Product(%d) zones (%d,%d) not cover current zones (%d,%d,%d)",
                    dir, pMYKI_TAProduct->ZoneLow, pMYKI_TAProduct->ZoneHigh,
                    pData->DynamicData.currentInnerZone, pData->DynamicData.currentZone,  pData->DynamicData.currentOuterZone );
                continue;
            }   /* end-of-if */

            if ( pMYKI_Directory->Status == TAPP_CONTROL_DIRECTORY_STATUS_ACTIVATED )
            {
                /*  One more usable active e-Pass found */
                nUsableActiveEpass++;
                break;  /* out-of-for */
            }
            else
            {
                /*  One more usable inactive e-Pass found */
                nUsableInactiveEpass++;
            }   /* end-of-if */
        }   /* end-of-for */

        do
        {
            /*      c.  The product status is Inactive and there is no e-Pass with
                        product status Active, */
            if ( nUsableActiveEpass == 0 && nUsableInactiveEpass > 0 && nActiveEpass > 0 )
            {
                /*  Can't activate another e-Pass => Reject smart card */
                CsDbg( BRLL_RULE, "BR_LLSC_5_2 : usable inactive e-Pass found but unusable active e-Pass exists" );
                break;  /* out-of-do-while */
            }   /* end-of-if */

            /*          or the e-Pass has product status Active. */
            if ( nUsableActiveEpass == 0 && nUsableInactiveEpass == 0 )
            {
                /*  No usable inactive/active e-Pass => Reject smart card */
                CsDbg( BRLL_RULE, "BR_LLSC_5_2 : no usable inactive/active e-Pass found" );
                break;  /* out-of-do-while */
            }   /* end-of-if */

            /*  3.  If the premium surcharge field is not 0 – i.e there is a surcharge defined then */
            if ( pData->Tariff.premiumSurcharge == 0 )
            {
                /*  Usable active/inactive e-Pass found and no surcharge => OK */
                CsDbg( BRLL_RULE, "BR_LLSC_5_2 : Bypass - no premium surcharge" );
                return RULE_RESULT_BYPASSED;
            }   /* end-of-if */

            /*      a.  The premium surcharge amount is greater than the current t-purse
                        balance; ie there is insufficient t-purse  */
            if ( pMYKI_TAPurseBalance->Balance >= pData->Tariff.premiumSurcharge )
            {
                /*  Usable active/inactive e-Pass found and sufficient funds available for surcharge => OK */
                CsDbg( BRLL_RULE, "BR_LLSC_5_2 : Bypass - T-purse balance (%d) >= Premium Surcharge (%d)",
                    pMYKI_TAPurseBalance->Balance, pData->Tariff.premiumSurcharge );
                return RULE_RESULT_BYPASSED;
            }   /* end-of-if */

            CsDbg( BRLL_RULE, "BR_LLSC_5_2 : T-purse balance (%d) < Premium Surcharge (%d)",
                pMYKI_TAPurseBalance->Balance, pData->Tariff.premiumSurcharge );
        }
        while ( FALSE /*ONCE-ONLY*/ );
    }

    /*  PROCESSING */
    {
        /*  1.  Reject the smartcard. */
        pData->ReturnedData.rejectReason    = MYKI_BR_REJECT_REASON_INSUFFICIENT_FUND;
    }

    CsDbg( BRLL_RULE, "BR_LLSC_5_2 : Executed" );
    return RULE_RESULT_EXECUTED;
}


