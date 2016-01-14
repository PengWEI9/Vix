/* -*- mode: c; tabs: 4; -*- */
/*=============================================================================
**
**    Vix Technology                   Licensed software
**    (C) 2013                         All rights reserved
**
**=============================================================================
**
**  Project/Product : MBU
**  Filename        : BR_LLSC_4_13.c
**  Author(s)       : ???
**
**  ID              : BR_LLSC_4_13 - NTS0177 v7.2
**
**  Name            : Blocking Period Exit
**
**  Data Fields     :
**
**      1.  TAppControl.ProductInUse
**      2.  TAppTProduct.ControlBitmap
**      3.  TAppTProduct.LastUsageEntryPointId
**      4.  TAppTProduct.LastUsageDateTime
**      5.  Dynamic.EntryPointId
**      6.  Dynamic.CurrentDateTime
**      7.  Tariff.BlockingPeriod
**      8.  TAppTProduct.Last
**      9.  Static.ModeOfTransport
**
**  Pre-Conditions  :
**
**      1.  The product in use field(1) is 0 (there is no product scanned on)
**      2.  For each product on the card with a status of Activated and where
**          the provisional bit on the product control bitmap(2) is not set;
**          check that following are all true:
**          a.  The mode of transport of the product in use(1) is the same
**              mode of transport(9) of the current device
**          b.  The Last Product last usage date time(4) plus the blocking
**              period(7) is greater than or equal to the current date/time(6)
**          c.  If The mode of transport of the device(9) is equal to Rail then
**              the scan-on station ID(3) is equal to the current station ID(5)
**              else the scan-on entry point Id, Route, Stop are equal to the
**              entry point ID, Route and stop of the current device.
**
**  Description     :
**
**      1.  Reject the smart card
**
** Post-Conditions  :
**
**      1.  The smart card is rejected.
**
** Devices          :
**
**      Fare payment devices
**
**  Member(s)       :
**      BR_LLSC_4_13            [public]    business rule
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
**    1.01  21.10.14    ANT   Modify   Rectified reject code, ie. "Already
**                                     Touch Off"
**
**===========================================================================*/

#include <cs.h>                 // For debug logging
#include <myki_cardservices.h>  // Myki Card Services
#include <LDT.h>                // Logical Device Transaction

#include "myki_br_rules.h"      // API for all BR_xxx modules
#include "BR_Common.h"          // Common utilities, macros, etc

/*==========================================================================*
**
**  BR_LLSC_4_13
**
**  Description     :
**      Implements business rule BR_LLSC_4_13.
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

RuleResult_e BR_LLSC_4_13( MYKI_BR_ContextData_t *pData )
{
    MYKI_TAControl_t   *pMYKI_TAControl = NULL;
    MYKI_Directory_t   *pDirectory      = NULL;
    MYKI_TAProduct_t   *pProduct        = NULL;
    ProductIterator     iAny;

    CsDbg(BRLL_RULE,"BR_LLSC_4_13 : Start (Blocking Period Exit)");

    if ( ! pData )
    {
        CsErrx( "BR_LLSC_4_13 : Invalid argument(s)" );
        return RULE_RESULT_ERROR;
    }

    if(MYKI_CS_TAControlGet(&pMYKI_TAControl))
    {
        CsErrx( "BR_LLSC_4_13 : MYKI_CS_TAControlGet() failed" );
        return RULE_RESULT_ERROR;
    }


    //Pre-Condition:1
    if ( pMYKI_TAControl->ProductInUse != 0 )
    {
        CsDbg( BRLL_RULE, "BR_LLSC_4_13 : Bypass - Product %d is in use", pMYKI_TAControl->ProductInUse );
        return RULE_RESULT_BYPASSED;
    }

    // Run pre-condition checks for all products

    myki_br_InitProductIterator( pMYKI_TAControl, &iAny, PRODUCT_TYPE_UNKNOWN );

    while ( myki_br_ProductIterate( &iAny ) > 0 )
    {
        pDirectory = iAny.pDirectory;
        pProduct   = iAny.pProduct;

        //  Pre-Condition 2 : For each product on the card that is active and non-provisional
        if ((pDirectory->Status != TAPP_CONTROL_DIRECTORY_STATUS_ACTIVATED ) || ((pProduct->ControlBitmap & TAPP_TPRODUCT_CONTROL_PROVISIONAL_BITMAP) != 0) )
        {
            CsDbg(BRLL_CHOICE,"BR_LLSC_4_13 : Product %d is not active or Provisional Bit is set : skip ", iAny.index );
            continue;
        }

        //Precondition 2.a (not required - this check for same mode is included in myki_br_CurrentLocationIsScanOnLocation() below)

        //Precondition 2.b
        if (pProduct->LastUsage.DateTime + pData->Tariff.blockingPeriod  < pData->DynamicData.currentDateTime )
        {
            CsDbg(BRLL_RULE,"BR_LLSC_4_13 : Product %d: Blocking Period expired : skip", iAny.index );
            continue;
        }

        //Pre-Condition 2.c
        if ( ! myki_br_CurrentLocationIsScanOnLocation( pData, pProduct ) )
        {
            CsDbg(BRLL_RULE,"BR_LLSC_4_13 : Product %d: Scan off was NOT at current Location : skip", iAny.index );
            continue;
        }

        pData->ReturnedData.rejectReason = MYKI_BR_REJECT_REASON_ALREADY_TOUCH_OFF;
        CsDbg( BRLL_RULE, "BR_LLSC_4_13 : Executed - Product %d is activated, not provisional, and was scanned off at current location within blocking period", iAny.index );
        return RULE_RESULT_EXECUTED;
    }

    CsDbg( BRLL_RULE, "BR_LLSC_4_13 : Bypass - Preconditions not met for any product" );
    return RULE_RESULT_BYPASSED;
}   /*  BR_LLSC_4_13( ) */
