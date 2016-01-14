/* -*- mode: c; tabs: 4; -*- */
/*=============================================================================
**
**    Vix Technology                   Licensed software
**    (C) 2015                         All rights reserved
**
**=============================================================================
**
**  Project/Product : MBU
**  Filename        : BR_LLSC_0_1.cpp
**  Author(s)       : ?
**
**  ID              : BR_LLSC_0_1
**
**  Name            : Card integrity check - NTS0177 v8.1
**
**  Data Fields     :
**
**      1.  Dynamic.CurrentDateTime
**      2.  TApp.TProduct[n].StartTime
**      3.  TApp.Tproduct[n].EndDate
**      4.  TApp.TProduct[n].InstanceCount
**      5.  TAppControl.Directory[n].Status
**      6.  TAppTProduct.Product[n] .ControlBitMap
**      7.  TAppCapping.Daily.Expiry
**      8.  TAppCapping.Daily.Value
**      9.  TAppCapping.Daily.ZoneLow
**      10. TAppCapping.Daily.ZoneHigh
**      11. TAppCapping.Weekly.Expiry
**      12. TAppCapping.Weekly.Value
**      13. TAppCapping.Weekly.ZoneLow
**      14. TAppCapping.Weekly.ZoneHigh
**      15. Tariff.EndOfBusinessDay
**      16. TAppControl.ProductInUse
**      17. TAppControl.Directory[].Status
**      18. Dynamic.IsTransit =true
**
**  Pre-conditions  :
**
**      1.  The card is a transit card i.e Dynamic.IsTransit = true
**
**  Description     :
**
**      1.  For every product that has a status5 of Activated check and process
**          as follows
**          a.  If the start time2 is greater than the expiry time3 then
**              i.  On condition being found  create card snapshot
**              ii. Perform a Product Update/Invalidate On the affected product
**
**          b.  Else if the product is of type Daily or Single Trip and further
**              the difference between the start date2 and  expiry time3 is
**              greater than a 24 hours
**              i.  On condition being found  create card snapshot
**              ii. Perform a Product Update/Invalidate On the affected product
**
**          c.  Else if the product is of type Weekly  and  the difference
**              between the start date2 end expiry time3 is greater than 8 days
**              i.  On condition being found  create card snapshot
**              ii. Perform a Product Update/Invalidate On the affected product
**
**          d.  Else if the product is of type e-Pass and ID = 2 and the
**              instance count4  is not zero then if (either)
**              i.  the start date plus2 the instance count4  plus 1 is less
**                  than  product end date3
**              ii. or the instance count4 greater than 1825 (5 years)
**                  (1) On condition being found  create card snapshot
**
**          e.  Else if the product is of type n-hour and either of the following
**              is true
**              i.  The difference between start date2 and expiry time3 is
**                  greater than 24 hours
**              ii. On condition being found  create card snapshot
**              iii.Perform a Product Update/Invalidate On the affected product
**
**      2.  For every product that has status of Issued5
**          a.  If the the product is not of type e-pass  (i.e only e-pass
**              support issued products)
**              i.  On condition being found  create card snapshot
**          b.  Else if the issued product is of type e-pass and the instance
**              count is not zero  check the
**              i.  the start time2 is greater than  the expiry time3 - or
**              ii. The instance count greater than 1825 (5 years)
**              iii.On condition being found  create card snapshot
**
**      3.  For every product where Product.ControlBitmap6  provisional bit is
**          set, where the product is not of type n-hour
**          i.  On condition being found  create card snapshot
**          ii. Perform a Product Update/Invalidate On the affected product
**
**      4.  For every product where the Product Control bitmap provisional6 bit
**          is set, where they are not the product in use.
**          a.  On condition being found create card snapshot
**          b.  Perform a Product Update/Invalidate
**
**      5.  If the product in use16 refers to a product slot17 that is unused
**          then perform TAppUpdate/SetProductInUse
**
**      6.  If the daily capping counter expiry7 less 2 days is greater than
**          the current business date15:
**          i.  Create a card snapshot
**          ii. Perform a CappingUpdate/None
**              (1) Set the low daily capping zone9 and high daily capping
**                  zone10 to 0.
**              (2) Set the daily capping fares total8 to 0.
**              (3) Set the daily capping counter expiry7 to end of business
**                  day time15 on the following day.
**
**      7.  If the weekly capping counter expiry11 less 8 days is greater
**          than the current business day15:
**          i.  Create a card snapshot
**          ii. Perform a CappingUpdate/None
**          (1) Set the low weekly capping zone13 and high weekly capping
**              zone14 to 0.
**          (2) Set the weekly capping fares total12 to 0.
**          (3) Set the weekly capping counter expiry11 to end of business
**              day time15 of the following Monday.
**
**      8.  If the TApp.TPurse.Balance is less than 2 times the negative
**          (Tariff.MaximumTPurseBalance) OR if TApp.TPurse.Balance is greater
**          2 times the Tariff.MaximumTPurseBalance
**          i.  Create a card snapshot (Reason Code: TPurseBalanceExcess)
**          ii. Reject the card
**
**      9.  If the TApp.TPurse.Balance is not equal to the
**          TApp.TPurseControl.LastChangeNewBlanace or if
**          TApp.TPurseControl.NextTxSeqNo is not equal to
**          TApp.TPurseControl.LastChangeTxSeqNo +1
**          i. Create a card snapshot (Reason Code: TPurseTxSeqNo)
**
**  Post-conditions :
**
**  Member(s)       :
**      BR_LLSC_0_1a            [public]    business rule - part A
**      BR_LLSC_0_1b            [public]    business rule - part B
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
**    1.00  dd.mm.yy    ?     Create
**    1.01  11.09.15    ANT   Add      Updated for NTS0177 v8.1
**    1.02  14.09.15    ANT   Add      Option to bypass business rule in
**                                     TEST mode
**    1.03  29.10.15    ANT   Modify   MBU-1719: Modified pre-condition 5
**                                     to specifically clear product in use.
**
**===========================================================================*/

#include <cs.h>                 // For debug logging
#include <myki_cardservices.h>  // Myki Card Services
#include <LDT.h>                // Logical Device Transaction

#include "myki_br_rules.h"      // API for all BR_xxx modules
#include "BR_Common.h"          // Common utilities, macros, etc

/*==========================================================================*
**
**  BR_LLSC_0_1a
**
**  Description     :
**      BR_LLSC_0_1a business rule, step 1 and 7 of BR_LLSC_0_1.
**
**  Parameters      :
**      pData               [I/O]   BR context data
**
**  Returns         :
**      RULE_RESULT_EXECUTED
**      RULE_RESULT_BYPASSED
**      RULE_RESULT_ERROR
**
**  Returns         :
**      None
**
**  Notes           :
**
**
**==========================================================================*/

RuleResult_e BR_LLSC_0_1a( MYKI_BR_ContextData_t *pData )
{
    int                     i                       = 0;
    MYKI_CAIssuer_t        *pMYKI_CAIssuer          = NULL;
    MYKI_TAControl_t       *pMYKI_TAControl         = NULL;
    MYKI_TACapping_t       *pMYKI_TACapping         = NULL;
    MYKI_TAProduct_t       *pMYKI_TAProduct         = NULL;
    MYKI_Directory_t       *pDirectory              = NULL;
    int                     hasDailyCappingReset    = FALSE;
    int                     hasWeeklyCappingReset   = FALSE;
    int                     nResult                 = 0;

    CsDbg( BRLL_RULE, "BR_LLSC_0_1a : Start (Card Integrity Check - Part A)" );

    if ( ! pData )
    {
        CsErrx( "BR_LLSC_0_1a : Invalid argument(s)" );
        return RULE_RESULT_ERROR;
    }

    //
    //  Check Pre-conditions
    //

    //  1. The card is a transit card i.e Dynamic.IsTransit = true

    if ( ! pData->DynamicData.isTransit )  // Application decides whether the card is a transit card or not based on device mode and card details
    {
        CsDbg( BRLL_RULE, "BR_LLSC_0_1a : Pre-condition BR_LLSC_0_1.1 not met." );
        CsDbg( BRLL_RULE, "BR_LLSC_0_1a : Bypass - ! pData->DynamicData.isTransit" );
        pData->ReturnedData.bypassCode  = BYPASS_CODE( 0, 1, 1, 0 );
        return RULE_RESULT_BYPASSED;
    }

    if ( ( nResult = MYKI_CS_CAIssuerGet( &pMYKI_CAIssuer ) ) < 0 )
    {
        CsErrx( "BR_LLSC_0_1a : MYKI_CS_CAIssuerGet() failed (%d)", nResult );
        return RULE_RESULT_ERROR;
    }

    if
    (
        ( pMYKI_CAIssuer->OperationMode & OPERATION_MODE_TEST ) != 0 &&
        pData->TestData.condition == TEST_CONDITION_BYPASS_BR_LLSC_0_1
    )
    {
        //  Bypass BR_LLSC_0_1 (card integrity check) to simulate BR_LLSC_99_1
        //  (pre-commit check) conditions
        //
        CsWarnx( "BR_LLSC_0_1a : Bypass - TEST mode, bypass card integrity check" );
        pData->ReturnedData.bypassCode  = BYPASS_CODE( 0, 1, 2, 0 );
        return RULE_RESULT_BYPASSED;
    }

    //
    //  Execute Rule
    //

    if ( ( nResult = MYKI_CS_TAControlGet( &pMYKI_TAControl ) ) < 0 )
    {
        CsErrx( "BR_LLSC_0_1a : MYKI_CS_TAControlGet() failed(%d)", nResult );
        return RULE_RESULT_ERROR;
    }

    for ( i = 1; i < DIMOF( pMYKI_TAControl->Directory ); i++ )     // Check all dir entries that map to products (ie, not the purse (0) entry)
    {
        ProductType_e       productType;

        if ( ( nResult = myki_br_GetCardProduct( i, &pDirectory, NULL ) ) < 0 )
        {
            CsErrx( "BR_LLSC_0_1a : myki_br_GetCardProduct(%d) failed(%d)", i, nResult );
            return RULE_RESULT_ERROR;
        }

        //  1. For every product that has a status of Activated

        if ( pDirectory->Status == TAPP_CONTROL_DIRECTORY_STATUS_ACTIVATED )
        {
            CsDbg( BRLL_RULE, "BR_LLSC_0_1a : Product %d is activated", i );

            if ( ( nResult = myki_br_GetCardProduct( i, &pDirectory, &pMYKI_TAProduct ) ) < 0 )
            {
                CsErrx( "BR_LLSC_0_1a : Failed to get product %d details(%d)", i, nResult );
                return RULE_RESULT_ERROR;
            }

            productType = myki_br_cd_GetProductType( pDirectory->ProductId );

            //  a. If the start time2 is greater than the expiry time3 then
            //  i. On condition being found  create card snapshot
            //  ii.    Perform a Product Update/Invalidate On the affected product

            if ( pMYKI_TAProduct->StartDateTime > pMYKI_TAProduct->EndDateTime )
            {
                CsWarnx( "BR_LLSC_0_1a : Product %d : StartDateTime(%d) > EndDateTime(%d)",
                        i, pMYKI_TAProduct->StartDateTime, pMYKI_TAProduct->EndDateTime );

                myki_br_CreateCardSnapshot( pData );

                if ( ( nResult = myki_br_ldt_ProductUpdate_Invalidate( pData, pDirectory ) ) < 0 )
                {
                    CsErrx( "BR_LLSC_0_1a : myki_br_ldt_ProductUpdate_Invalidate() failed(%d)", nResult );
                    return RULE_RESULT_ERROR;
                }
                continue;   /* for-loop */
            }

            //  b. Else if the product is of type Daily or Single Trip and further the difference between the start date2 and  expiry time3 is  greater than a 24 hours
            //  i. On condition being found  create card snapshot
            //  ii.    Perform a Product Update/Invalidate On the affected product

            else if
            (
                ( productType == PRODUCT_TYPE_DAILY || productType == PRODUCT_TYPE_SINGLE ) &&
                ( pMYKI_TAProduct->EndDateTime > ( pMYKI_TAProduct->StartDateTime + HOURS_TO_SECONDS( 24 ) ) )
            )
            {
                CsWarnx( "BR_LLSC_0_1a : Product %d : %s and EndDateTime(%d) > StartDateTime(%d) + 24 hours",
                        i, myki_br_GetProductTypeName( productType ), pMYKI_TAProduct->EndDateTime, pMYKI_TAProduct->StartDateTime );

                myki_br_CreateCardSnapshot( pData );

                if ( ( nResult = myki_br_ldt_ProductUpdate_Invalidate( pData, pDirectory ) ) < 0 )
                {
                    CsErrx( "BR_LLSC_0_1a : myki_br_ldt_ProductUpdate_Invalidate() failed(%d)", nResult );
                    return RULE_RESULT_ERROR;
                }
                continue;   /* for-loop */
            }

            //  c. Else if the product is of type Weekly  and  the difference between the start date2 end expiry time3 is greater than 8 days
            //  i. On condition being found  create card snapshot
            //  ii.    Perform a Product Update/Invalidate On the affected product

            else if
            (
                ( productType == PRODUCT_TYPE_WEEKLY ) &&
                ( pMYKI_TAProduct->EndDateTime > ( pMYKI_TAProduct->StartDateTime + DAYS_TO_SECONDS( 8 ) ) )
            )
            {
                CsWarnx( "BR_LLSC_0_1a : Product %d : Weekly and EndDateTime(%d) > StartDateTime(%d) + 8 days",
                        i, pMYKI_TAProduct->EndDateTime, pMYKI_TAProduct->StartDateTime );

                myki_br_CreateCardSnapshot( pData );

                if ( ( nResult = myki_br_ldt_ProductUpdate_Invalidate( pData, pDirectory ) ) < 0 )
                {
                    CsErrx( "BR_LLSC_0_1a : myki_br_ldt_ProductUpdate_Invalidate() failed(%d)", nResult );
                    return RULE_RESULT_ERROR;
                }
                continue;   /* for-loop */
            }

            //  d. Else if the product is of type e-Pass and ID = 2 and the instance count4  is not zero then if (either)
            //  i. the start date plus2 the instance count4  plus 1 is less  than  product end date3
            //  ii.    or the instance count4 greater than 1825 (5 years)
            //  (1)    On condition being found  create card snapshot

            else if
            (
                ( productType == PRODUCT_TYPE_EPASS ) &&
                ( pDirectory->ProductId == 2 ) &&                       // KWS: Need a named constant for this
                ( pMYKI_TAProduct->InstanceCount != 0 ) &&
                (
                    ( pMYKI_TAProduct->EndDateTime > ( pMYKI_TAProduct->StartDateTime + DAYS_TO_SECONDS( pMYKI_TAProduct->InstanceCount + 1 ) ) ) ||
                    ( pMYKI_TAProduct->InstanceCount > 1825 )           // KWS: Need a named constant for this
                )
            )
            {
                CsWarnx( "BR_LLSC_0_1a : Product %d : ePass and InstanceCount(%d) / StartDateTime(%d) / EndDateTime(%d) invalid",
                        i, pMYKI_TAProduct->InstanceCount, pMYKI_TAProduct->StartDateTime, pMYKI_TAProduct->EndDateTime );

                myki_br_CreateCardSnapshot( pData );
            }

            //  e. Else if the product is of type n-hour and either of the following is true
            //  i. The difference between start date2 and expiry time3 is greater than 24 hours
            //  ii.    On condition being found  create card snapshot
            //  iii.   Perform a Product Update/Invalidate On the affected product

            else if
            (
                ( productType == PRODUCT_TYPE_NHOUR ) &&
                ( pMYKI_TAProduct->EndDateTime > ( pMYKI_TAProduct->StartDateTime + HOURS_TO_SECONDS( 24 ) ) )
            )
            {
                CsWarnx( "BR_LLSC_0_1a : Product %d : nHour and EndDateTime(%d) > StartDateTime(%d) + 24 hours",
                        i, pMYKI_TAProduct->EndDateTime, pMYKI_TAProduct->StartDateTime );

                myki_br_CreateCardSnapshot( pData );

                if ( ( nResult = myki_br_ldt_ProductUpdate_Invalidate( pData, pDirectory ) ) < 0 )
                {
                    CsErrx( "BR_LLSC_0_1a : myki_br_ldt_ProductUpdate_Invalidate() failed(%d)", nResult );
                    return RULE_RESULT_ERROR;
                }
                continue;   /* for-loop */
            }
        }

        //  2. For every product that has a status of Issued

        else if ( pDirectory->Status == TAPP_CONTROL_DIRECTORY_STATUS_ISSUED )
        {
            CsDbg( BRLL_RULE, "BR_LLSC_0_1a : Product %d is issued", i );

            if ( ( nResult = myki_br_GetCardProduct( i, &pDirectory, &pMYKI_TAProduct ) ) < 0 )
            {
                CsErrx( "BR_LLSC_0_1a : Failed(%d) to get product %d details", nResult, i );
                return RULE_RESULT_ERROR;
            }

            productType = myki_br_cd_GetProductType( pDirectory->ProductId );

            //  a. If the the product is not of type e-pass  (i.e only e-pass support issued products)
            //  i. On condition being found  create card snapshot

            if ( productType != PRODUCT_TYPE_EPASS )
            {
                CsWarnx( "BR_LLSC_0_1a : Product %d : Invalid ISSUED %s(%d) product",
                        i, myki_br_GetProductTypeName( productType ), productType );

                myki_br_CreateCardSnapshot( pData );
            }

            //  b. Else if the issued product is of type e-pass and the instance count is not zero  check the
            //  i.  the start time2 is greater than  the expiry time3 - or
            //  ii.    The instance count greater than 1825 (5 years)
            //  iii.   On condition being found  create card snapshot

            else if
            (
                ( productType == PRODUCT_TYPE_EPASS ) &&
                ( pMYKI_TAProduct->InstanceCount != 0 ) &&
                (
                    ( pMYKI_TAProduct->StartDateTime > pMYKI_TAProduct->EndDateTime ) ||
                    ( pMYKI_TAProduct->InstanceCount > 1825 )           // KWS: Need a named constant for this
                )
            )
            {
                CsWarnx( "BR_LLSC_0_1a : Product %d : ePass and InstanceCount(%d) / StartDateTime(%d) / EndDateTime(%d) invalid",
                        i, pMYKI_TAProduct->InstanceCount, pMYKI_TAProduct->StartDateTime, pMYKI_TAProduct->EndDateTime );

                myki_br_CreateCardSnapshot( pData );
            }
        }

        //  Not strictly part of BR, but print it for info

        else
        {
            CsDbg( BRLL_RULE, "BR_LLSC_0_1a : Product %d is neither issued nor activated (status = %d)", i, pDirectory->Status );
            continue;   // No further processing of this product, go on to next product
        }

        //  3. For every product where Product.ControlBitmap6  provisional bit is set, where the product is not of type n-hour
        //  i. On condition being found  create card snapshot
        //  ii.    Perform a Product Update/Invalidate On the affected product
        //  Assumes product is either Issued or Activated, or else this would be meaningless.

        if
        (
            ( pMYKI_TAProduct->ControlBitmap & TAPP_TPRODUCT_CONTROL_PROVISIONAL_BITMAP ) &&
            ( productType != PRODUCT_TYPE_NHOUR )
        )
        {
            CsWarnx( "BR_LLSC_0_1a : Product %d : PROVISIONAL set on %s(%d) product",
                    i, myki_br_GetProductTypeName( productType ), productType );

            myki_br_CreateCardSnapshot( pData );

            if ( ( nResult = myki_br_ldt_ProductUpdate_Invalidate( pData, pDirectory ) ) < 0 )
            {
                CsErrx( "BR_LLSC_0_1a : myki_br_ldt_ProductUpdate_Invalidate() failed(%d)", nResult );
                return RULE_RESULT_ERROR;
            }
            continue;   /* for-loop */
        }

        //  4. For every product where the Product Control bitmap provisional6 bit is set, where they are not the product in use.
        //  a. On condition being found create card snapshot
        //  b. Perform a Product Update/Invalidate
        //  Assumes product is either Issued or Activated, or else this would be meaningless.

        if
        (
            ( pMYKI_TAProduct->ControlBitmap & TAPP_TPRODUCT_CONTROL_PROVISIONAL_BITMAP ) &&
            ( i != pMYKI_TAControl->ProductInUse )
        )
        {
            CsWarnx( "BR_LLSC_0_1a : Product %d : PROVISIONAL product but not in use(%d)",
                    i, pMYKI_TAControl->ProductInUse );

            myki_br_CreateCardSnapshot( pData );

            if ( ( nResult = myki_br_ldt_ProductUpdate_Invalidate( pData, pDirectory ) ) < 0 )
            {
                CsErrx( "BR_LLSC_0_1a : myki_br_ldt_ProductUpdate_Invalidate() failed(%d)", nResult );
                return RULE_RESULT_ERROR;
            }
        }
    }

    //  5. If the product in use16 refers to a product slot17 that is unused then perform TAppUpdate/SetProductInUse
    //      Implies we should only perform this check if there is a product in use (ie, ProductInUse > 0)

    if ( pMYKI_TAControl->ProductInUse > 0 )
    {
        if ( myki_br_GetCardProduct( pMYKI_TAControl->ProductInUse, &pDirectory, NULL ) < 0 )
        {
            CsErrx( "BR_LLSC_0_1a : Failed to get product %d details", pMYKI_TAControl->ProductInUse );
            return RULE_RESULT_ERROR;
        }

        if ( pDirectory->Status == TAPP_CONTROL_DIRECTORY_STATUS_UNUSED )
        {
            CsWarnx( "BR_LLSC_0_1a : In Use Product (%d) not in use", pMYKI_TAControl->ProductInUse );

            //  Clear product in use
            if ( ( nResult = myki_br_ldt_AppUpdate_SetProductInUse( pData, NULL ) ) < 0 )
            {
                CsErrx( "BR_LLSC_0_1a : myki_br_ldt_AppUpdate_SetProductInUse() failed(%d)", nResult );
                return RULE_RESULT_ERROR;
            }
        }
    }

    //  6.  If the daily capping counter expiry7 less 2 days is greater than than the current business date15
    //  i. Create a card snapshot
    //  ii.    Perform a CappingUpdate/None
    //  (1)    Set the low daily capping zone9 and high daily capping zone10 to 0.
    //  (2)    Set the daily capping fares total8 to 0.
    //  (3)    Set the daily capping counter expiry7 to end of business day time15 on the following day.

    if ( ( nResult = MYKI_CS_TACappingGet( &pMYKI_TACapping ) ) < 0 )
    {
        CsErrx( "BR_LLSC_0_1a : MYKI_CS_TACappingGet() failed(%d)", nResult );
        return RULE_RESULT_ERROR;
    }

    if ( pMYKI_TACapping->Daily.Expiry > ( pData->DynamicData.currentBusinessDate + 2 ) )
    {
        CsWarnx( "BR_LLSC_0_1a : Daily capping counter expiry(%d) > current(%d) + 2",
                pMYKI_TACapping->Daily.Expiry, pData->DynamicData.currentBusinessDate );

        myki_br_CreateCardSnapshot( pData );
        hasDailyCappingReset    = TRUE;
    }

    //  7. If the weekly capping counter expiry11 less 8 days is greater than the current business day15:
    //  i. Create a card snapshot
    //  ii.    Perform a CappingUpdate/None
    //  (1)    Set the low weekly capping zone13 and high weekly capping zone14 to 0.
    //  (2)    Set the weekly capping fares total12 to 0.
    //  (3)    Set the weekly capping counter expiry11 to end of business day time15 of the following Monday.

    if ( pMYKI_TACapping->Weekly.Expiry > ( pData->DynamicData.currentBusinessDate + 8 ) )
    {
        CsWarnx( "BR_LLSC_0_1a : Weekly capping counter expiry(%d) > current(%d) + 8",
                pMYKI_TACapping->Weekly.Expiry, pData->DynamicData.currentBusinessDate );

        myki_br_CreateCardSnapshot( pData );
        hasWeeklyCappingReset   = TRUE;
    }

    if ( hasDailyCappingReset == TRUE || hasWeeklyCappingReset == TRUE )
    {
        if ( ( nResult = myki_br_ldt_PerformCappingReset( pData, hasDailyCappingReset, hasWeeklyCappingReset ) ) < 0 )
        {
            CsErrx( "BR_LLSC_0_1a : myki_br_ldt_PerformCappingReset() failed(%d)", nResult );
            return RULE_RESULT_ERROR;
        }
    }

    CsDbg( BRLL_RULE, "BR_LLSC_0_1a : Executed" );
    return RULE_RESULT_EXECUTED;
}

/*==========================================================================*
**
**  BR_LLSC_0_1b
**
**  Description     :
**      BR_LLSC_0_1b business rule, step 8 and 9 of BR_LLSC_0_1.
**
**  Parameters      :
**      pData               [I/O]   BR context data
**
**  Returns         :
**      RULE_RESULT_EXECUTED
**      RULE_RESULT_BYPASSED
**      RULE_RESULT_ERROR
**
**  Returns         :
**      None
**
**  Notes           :
**
**
**==========================================================================*/

RuleResult_e BR_LLSC_0_1b( MYKI_BR_ContextData_t *pData )
{
    MYKI_TAControl_t       *pMYKI_TAControl         = NULL;
    MYKI_TAPurseBalance_t  *pMYKI_TAPurseBalance    = NULL;
    MYKI_TAPurseControl_t  *pMYKI_TAPurseControl    = NULL;
    int                     nResult                 = 0;
    Currency_t              minTPurseBalance        = 0;
    Currency_t              maxTPurseBalance        = 0;

    CsDbg( BRLL_RULE, "BR_LLSC_0_1b : Start (Card Integrity Check - Part B)" );

    if ( pData == NULL )
    {
        CsErrx( "BR_LLSC_0_1b : Invalid argument(s)" );
        return RULE_RESULT_ERROR;
    }

    if ( ( nResult = MYKI_CS_TAControlGet( &pMYKI_TAControl ) ) < 0 )
    {
        CsErrx( "BR_LLSC_0_1b : MYKI_CS_TAControlGet() failed(%d)", nResult );
        return RULE_RESULT_ERROR;
    }

    if ( ( nResult = MYKI_CS_TAPurseBalanceGet( &pMYKI_TAPurseBalance ) ) < 0 )
    {
        CsErrx( "BR_LLSC_0_1b : MYKI_CS_TAPurseBalanceGet() failed (%d)", nResult );
        return RULE_RESULT_ERROR;
    }

    if ( ( nResult = MYKI_CS_TAPurseControlGet( &pMYKI_TAPurseControl ) ) < 0 )
    {
        CsErrx( "BR_LLSC_0_1b : MYKI_CS_TAPurseControlGet() failed (%d)", nResult );
        return RULE_RESULT_ERROR;
    }

    /*  PRE-CONDITIONS */
    {
        /*  8.  If the TApp.TPurse.Balance is less than 2 times the negative
                (Tariff.MaximumTPurseBalance) OR if TApp.TPurse.Balance is greater
                2 times the Tariff.MaximumTPurseBalance
                i.  Create a card snapshot (Reason Code: TPurseBalanceExcess)
                ii. Reject the card */
        minTPurseBalance    = - ( pData->Tariff.TPurseMaximumBalance * 2 );
        if ( pMYKI_TAPurseBalance->Balance < minTPurseBalance )
        {
            CsErrx( "BR_LLSC_0_1b : Bypassed - TAPurseBalance(%d) < Minimum (%d)",
                    pMYKI_TAPurseBalance->Balance, minTPurseBalance );
            myki_br_CreateCardSnapshot( pData );
            pData->ReturnedData.rejectReason    = MYKI_BR_REJECT_REASON_INVALID_BALANCE;
            pData->ReturnedData.bypassCode      = BYPASS_CODE( 0, 1, 8, 1 );
            return  RULE_RESULT_BYPASSED;
        }
        maxTPurseBalance    = ( pData->Tariff.TPurseMaximumBalance * 2 );
        if ( pMYKI_TAPurseBalance->Balance > maxTPurseBalance )
        {
            CsErrx( "BR_LLSC_0_1b : Bypassed - TAPurseBalance(%d) > Maximum (%d)",
                    pMYKI_TAPurseBalance->Balance, maxTPurseBalance );
            myki_br_CreateCardSnapshot( pData );
            pData->ReturnedData.rejectReason    = MYKI_BR_REJECT_REASON_INVALID_BALANCE;
            pData->ReturnedData.bypassCode      = BYPASS_CODE( 0, 1, 8, 2 );
            return  RULE_RESULT_BYPASSED;
        }
    }
  
    /*  PROCESSING */
    {
        /*  9.  If the TApp.TPurse.Balance is not equal to the
                TApp.TPurseControl.LastChangeNewBlanace or if
                TApp.TPurseControl.NextTxSeqNo is not equal to
                TApp.TPurseControl.LastChangeTxSeqNo +1
                i. Create a card snapshot (Reason Code: TPurseTxSeqNo) */
        if ( pMYKI_TAPurseBalance->Balance != pMYKI_TAPurseControl->LastChangeNewBalance )
        {
            CsWarnx( "BR_LLSC_0_1b : TAPurseBalance(%d) != TAPurseControl.LastChangeNewBalance(%d)",
                    pMYKI_TAPurseBalance->Balance, pMYKI_TAPurseControl->LastChangeNewBalance );
            myki_br_CreateCardSnapshot( pData );
        }
        else
        if ( pMYKI_TAPurseControl->NextTxSeqNo != (U16_t)( pMYKI_TAPurseControl->LastChangeTxSeqNo + 1 ) )
        {
            CsWarnx( "BR_LLSC_0_1b : TAPurseControl.NextTxSeqNo(%d) != TAPurseControl.LastChangeTxSeqNo(%d) + 1",
                    pMYKI_TAPurseControl->NextTxSeqNo, (U16_t)( pMYKI_TAPurseControl->LastChangeTxSeqNo + 1 ) );
            myki_br_CreateCardSnapshot( pData );
        }
    }

    CsDbg( BRLL_RULE, "BR_LLSC_0_1b : Executed" );
    return  RULE_RESULT_EXECUTED;
}
