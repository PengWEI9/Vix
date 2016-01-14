/* -*- mode: c; tabs: 4; -*- */
/*=============================================================================
**
**    Vix Technology                   Licensed software
**    (C) 2013                         All rights reserved
**
**=============================================================================
**
**  Project/Product : MBU
**  Filename        : BR_LLSC_7_2.c
**  Author(s)       : ?
**
**  ID              : BR_LLSC_7_2 - NTS0177 v7.2
**
**  Name            : Prepare for Asymmetric Forced scan off
**
**  Data Fields     :
**
**      1.  TAppControl.ProductInUse
**      2.  TAppTProduct.ControlBitmap
**      3.  TAppTProduct.ZoneLow
**      4.  TAppTProduct.ZoneHigh
**      5.  Dynamic.CurrentTripZoneLow
**      6.  Dynamic.CurrentTripZoneHigh
**      7.  Tariff.CitySaverZoneFlag
**      8.  TAppTProudct.EndDateTime
**      9.  Dynamic.ProposedMergeZoneLow
**      10. Dynamic.ProposedMergeZoneHigh
**      11. Dynamic.ProposedMerge.Fare
**      12. Dynamic.ProposedMerge.Direction
**      13. TAppControl.Directory.Status
**      14. Dynamic.CurrentTripDirection
**      15. Dynamic.IsForcedScanOff
**      16. Dynamic.ForcedScanOffDate
**      17. TAppTProduct.StartDateTime
**      18. Dynamic.CurrentZone
**      19. Dynamic.CurrentDateTime
**      20. Static.ServiceProviderID
**      21. Dynamic.EntryPointID
**      22. Dynamic.LineID
**      23. Dynamic.StopID
**
**
**  Pre-Conditions  :
**      None
**
**  Description     :
**      1.  Set IsForcedScanOff(15) = True - ie this is a force scan off sequence
**      2.  Set ForcedScanOffDate(16) to the scan on date(17) of the provisional
**          product.
**      3.  Set the current trip low zone(5) to the provisional low zone(3).
**      4.  Set the current trip high zone(6) to the provisional high zone(4).
**      5.  If the product control bit map(2) on the provisional product
**          indicates that this is a border product then perform ProductUpdate/None
**          to clear the board status bit
**
**  Post-Conditions :
**      None
**
**  Devices         :
**      Fare payment devices.
** 
**  Member(s)       :
**      BR_LLSC_7_2             [public]    business rule
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
**    1.00  ??.??.13    ANT   Create
**    1.01  31.01.14    ANT   Modify   Rectified incorrect setting of
**                                     Dynamic.ForcedScanOffDateTime and
**                                     Dynamic.CurrentTripZoneLog/High
**
**===========================================================================*/

#include <cs.h>                 // For debug logging
#include <myki_cardservices.h>  // Myki Card Services
#include <LDT.h>                // Logical Device Transaction

#include "myki_br_rules.h"      // API for all BR_xxx modules
#include "BR_Common.h"          // Common utilities, macros, etc

/*==========================================================================*
**
**  BR_LLSC_7_2
**
**  Description     :
**      Implements business rule BR_LLSC_7_2.
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

RuleResult_e BR_LLSC_7_2( MYKI_BR_ContextData_t *pData )
{
    MYKI_TAControl_t            *pMYKI_TAControl    = NULL;
    MYKI_TAProduct_t            *pMYKI_TAProduct    = NULL;
    MYKI_Directory_t            *pDirectory         = NULL;

    CsDbg( BRLL_RULE, "BR_LLSC_7_2 : Start (Prepare for Asymmetric Forced scan off)" );

    if ( ! pData )
    {
        CsErrx( "BR_LLSC_7_2 : Invalid argument(s)" );
        return RULE_RESULT_ERROR;
    }

    if ( MYKI_CS_TAControlGet( &pMYKI_TAControl ) < 0 )
    {
        CsErrx( "BR_LLSC_7_2 : MYKI_CS_TAControlGet() failed" );
        return RULE_RESULT_ERROR;
    }

    /*  PROCESSING */
    {
        if ( myki_br_GetCardProduct( pMYKI_TAControl->ProductInUse, &pDirectory, &pMYKI_TAProduct ) < 0 )
        {
            CsErrx( "BR_LLSC_7_2 : myki_br_GetCardProduct(%d) failed", pMYKI_TAControl->ProductInUse );
            return RULE_RESULT_ERROR;
        }

        /*  1.  Set IsForcedScanOff(15) = True - ie this is a force scan off sequence */
        pData->DynamicData.isForcedScanOff          = TRUE;

        /*  2.  Set ForcedScanOffDate(16) to the scan on date(17) of the provisional product. */
        pData->DynamicData.forcedScanOffDateTime    = pMYKI_TAProduct->StartDateTime;

        CsDbg( BRLL_RULE, "BR_LLSC_7_2 : Setting Dynamic.CurrenTripZoneLow/High(%d,%d) to (%d,%d)",
                pData->DynamicData.currentTripZoneLow, pData->DynamicData.currentTripZoneHigh,
                pMYKI_TAProduct->ZoneLow, pMYKI_TAProduct->ZoneHigh );

        /*  3.  Set the current trip low zone(5) to the provisional low zone(3). */
        pData->DynamicData.currentTripZoneLow       = pMYKI_TAProduct->ZoneLow;

        /*  4.  Set the current trip high zone(6) to the provisional high zone(4). */
        pData->DynamicData.currentTripZoneHigh      = pMYKI_TAProduct->ZoneHigh;

        /*  5.  If the product control bit map(2) on the provisional product indicates
                that this is a border product then */
        if ( pMYKI_TAProduct->ControlBitmap & TAPP_TPRODUCT_CONTROL_BORDER_STATUS_BITMAP )
        {
            /*  perform ProductUpdate/None to clear the board status bit */
            if ( myki_br_ldt_ProductUpdate_ClearBorderStatus( pData, pDirectory ) < 0 )
            {
                CsErrx( "BR_LLSC_7_2 : myki_br_ldt_ProductUpdate_ClearBorderStatus() failed" );
                return RULE_RESULT_ERROR;
            }
        }
    }

    CsDbg( BRLL_RULE, "BR_LLSC_7_2 : Executed" );
    return RULE_RESULT_EXECUTED;
}   /* BR_LLSC_7_2( ) */


