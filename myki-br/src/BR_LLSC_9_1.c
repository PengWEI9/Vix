/**************************************************************************
*   ID :  BR_LLSC_9_1
*
*    Card product space optimisation
*    Reduces products on the card where an
*    existing product is full covered by another
*    product
*
*
*
*    Pre-Conditions
*       None
*    Description
*    1. For every product that is type Weekly
*    a. For every zone covered by the weekly product
*    i.If a Daily product exists such that it is completely covered by the
*       Weekly product (ie Weekly product low zone is less than or equal to
*       the low zone of the Daily and the Weekly product high zone is greater
*       than or equal to the daily product high zone) then perform
*    (1)    ProductUpdate/Invalidate transaction on the Daily product
*    (2)    If daily product was the product in use then set the weekly as the product in use.
*    ii.    If a n-Hour product exists where the following is all true:
*    (1)    product control bitmap indiciates that this is not a boarder product, and
*    (2)    it is completely covered by the Weekly product (ie Weekly product low zone is less than or equal to the low zone of the n-Hour and the Weekly product high zone is greater than or equal to the n-Hour high zone) , and
*    (3)    the expiry date of the n-hour is less than or equal to the expiry of the weekly product then perform:
*    iii.   Then perform  a ProductUpdate/Invalidate transaction on the Daily product
*    (1)    If the n-Hour product was the product in use then set the weekly as the product in use.
*    2. For every product that is of type Daily
*    a. If an n-Hour product exists where the following is all true:
*    i.  product control bitmap indiciates that this is not a boarder product, and
*    ii.    it is completely covered by the Daily product (ie Daily product low zone is less than or equal to the low zone of the n-Hour and the Daily product high zone is greater than or equal to the n-hour high zone) , and
*    iii.    the expiry of the n-hour is less than expiry or equal to date of the daily product
*    b.  then perform:
*    i. A ProductUpdate/Invalidate on the n-hour
*    ii.    If the n-Hour product was the product in use then set the Daily as the product in use.
*    3. For every product that is of type Single Trip
*    a. if a single trip product exists such that it is completely covered by either a daily, weekly or by an Activated e-Pass then
*    i. A ProductUpdate/Invalidate on the single trip product
*    ii.    If the single trip product  was the product in use then set the covering product in use
*    Post conditions
*
*    Devices
*    Fare payment devices
*
**************************************************************************/

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

/*----------------------------------------------------------------------------
** FUNCTION           : BR_LLSC_9_1_ProductUpdate_and_Invalidate
**
** DESCRIPTION        : Do a ProductUpdate/Invalidate on a product
**                      If the product was the product in use then set the covering product in use
**
**                      NOTE: This routine will report errors so that the user does not need to.
**
**
** INPUTS             : pData           - MYKI_BR_ContextData_t. Rule context data
**                      pInvalidate     - Product to Update/Invalidate
**                      pCovered        - Covering Product
**
** RETURNS            : 0   - all is well
**                      -1  - Error detected and reported via CsErrx
**
----------------------------------------------------------------------------*/

static int BR_LLSC_9_1_ProductUpdate_and_Invalidate(MYKI_BR_ContextData_t *pData, ProductIterator *pInvalidate, ProductIterator *pCovered)
{
    CsDbg( BRLL_RULE, "Invalidate product at index %d", pInvalidate->index );

    //CsVerbose( "BR_LLSC_9_1_ProductUpdate_and_Invalidate. I:%d, C:%d",pInvalidate->index, pCovered->index);
    if (myki_br_ldt_ProductUpdate_Invalidate(pData, pInvalidate->pDirectory) < 0)
    {
        CsErrx("BR_LLSC_9_1 : myki_br_ldt_ProductUpdate_Invalidate() failed");
        return -1;
    }

    // Was it the product in use
    // If so then set the covering product
    if ( pInvalidate->index == pInvalidate->pMYKI_TAControl->ProductInUse )
    {
        if (myki_br_ldt_AppUpdate_SetProductInUse(pData, pCovered->pDirectory) < 0 )
        {

            CsErrx( "BR_LLSC_9_1 : myki_br_ldt_AppUpdate_SetProductInUse() failed" );
            return -1;
        }
    }

    //CsDbg(BRLL_RULE, "BR_LLSC_9_1 : ProductUpdate_and_Invalidate() Success");
    return  0;
}

/*----------------------------------------------------------------------------
** FUNCTION           : BR_LLSC_9_1
**
** DESCRIPTION        :
**
**
** INPUTS             : pData       - MYKI_BR_ContextData_t. Rule context data
**
** RETURNS            :
**
----------------------------------------------------------------------------*/

RuleResult_e BR_LLSC_9_1(MYKI_BR_ContextData_t *pData)
{
    MYKI_TAControl_t            *pMYKI_TAControl;
    ProductIterator             iWeek;
    ProductIterator             iDay;
    ProductIterator             iHour;
    ProductIterator             iSingle;

    CsDbg(BRLL_RULE, "BR_LLSC_9_1 : Start (Card product space optimisation)");

    if (!pData)
    {
        CsErrx("BR_LLSC_9_1 : Invalid argument(s)");
        return RULE_RESULT_ERROR;
    }

    //  1.  For every product that is type Weekly

    if (MYKI_CS_TAControlGet(&pMYKI_TAControl) < 0)
    {
        CsErrx("BR_LLSC_9_1 : MYKI_CS_TAControlGet() failed");
        return RULE_RESULT_ERROR;
    }

    //  a.  For every zone covered by the weekly product
    myki_br_InitProductIterator(pMYKI_TAControl, &iWeek, PRODUCT_TYPE_WEEKLY);
    while (myki_br_ProductIterate(&iWeek) > 0)
    {
        //  i.  If a Daily product exists ...
        ProductIterator iDay;
        CsDbg(BRLL_RULE, "BR_LLSC_9_1 : Scanning for Daily products covered by weekly");
        myki_br_InitProductIterator(pMYKI_TAControl, &iDay, PRODUCT_TYPE_DAILY);
        while (myki_br_ProductIterate(&iDay) > 0)
        {
            //  i.  If a Daily product exists such that it is completely covered by the
            //     Weekly product (ie Weekly product low zone is less than or equal to
            //     the low zone of the Daily and the Weekly product high zone is greater
            //     than or equal to the daily product high zone)

            //CsVerbose( "BR_LLSC_9_1 : Daily Covered:%d ?<=? %d and %d ?>=? %d",iWeek.pProduct->ZoneLow, iDay.pProduct->ZoneLow , iWeek.pProduct->ZoneHigh, iDay.pProduct->ZoneHigh);
            if (   (iWeek.pProduct->ZoneLow  <= iDay.pProduct->ZoneLow)
                && (iWeek.pProduct->ZoneHigh >= iDay.pProduct->ZoneHigh))
            {
                //     then perform
                //  (1) a ProductUpdate/Invalidate transaction on the Daily product
                //  (2) If daily product was the product in use then set the weekly as the product in use.

                CsDbg(BRLL_RULE, "BR_LLSC_9_1 : Daily Covered:%d <= %d and %d >= %d",iWeek.pProduct->ZoneLow, iDay.pProduct->ZoneLow , iWeek.pProduct->ZoneHigh, iDay.pProduct->ZoneHigh);
                if (BR_LLSC_9_1_ProductUpdate_and_Invalidate(pData, &iDay, &iWeek) < 0)
                {
                    // Note: Error reported by BR_LLSC_9_1_ProductUpdate_and_Invalidate
                    return RULE_RESULT_ERROR;
                }
            }
        }
        if (iDay.error)
        {
            return RULE_RESULT_ERROR;
        }


        //  ii. If a n-Hour product exists where the following is all true:
        CsDbg(BRLL_RULE, "BR_LLSC_9_1 : Scanning for nHour products covered by weekly");
        myki_br_InitProductIterator(pMYKI_TAControl, &iHour, PRODUCT_TYPE_NHOUR);
        while (myki_br_ProductIterate(&iHour) > 0)
        {
            //  (1) product control bitmap indiciates that this is not a boarder product, and
            //  (2) it is completely covered by the Weekly product (ie Weekly product low zone is less than
            //      or equal to the low zone of the n-Hour and the Weekly product high zone is greater than
            //      or equal to the n-Hour high zone) , and
            //  (3) the expiry date of the n-hour is less than or equal to the expiry of the weekly product then perform:
            if ( ! (iHour.pProduct->ControlBitmap & TAPP_TPRODUCT_CONTROL_BORDER_STATUS_BITMAP)
                && (iWeek.pProduct->ZoneLow  <= iHour.pProduct->ZoneLow)
                && (iWeek.pProduct->ZoneHigh >= iHour.pProduct->ZoneHigh)
                && (iHour.pProduct->EndDateTime <= iWeek.pProduct->EndDateTime) )
            {
                //  Then perform
                // a ProductUpdate/Invalidate transaction on the Daily product (sic)
                //  (1) If the n-Hour product was the product in use then set the weekly as the product in use.
                CsDbg(BRLL_RULE, "BR_LLSC_9_1 : Hourly Covered by weekly product:%d <= %d and %d >= %d",iWeek.pProduct->ZoneLow, iHour.pProduct->ZoneLow , iWeek.pProduct->ZoneHigh, iHour.pProduct->ZoneHigh);
                CsDbg(BRLL_RULE, "BR_LLSC_9_1 : Control Bitmap: %x", iHour.pProduct->ControlBitmap);
                CsDbg(BRLL_RULE, "BR_LLSC_9_1 : EndDateTime: %d <= %d", iHour.pProduct->EndDateTime, iWeek.pProduct->EndDateTime);
                if (BR_LLSC_9_1_ProductUpdate_and_Invalidate(pData, &iHour, &iWeek) < 0)
                {
                    // Note: Error reported by BR_LLSC_9_1_ProductUpdate_and_Invalidate
                    return RULE_RESULT_ERROR;
                }
            }
        }
        if (iHour.error)
        {
            return RULE_RESULT_ERROR;
        }

    }
    if (iWeek.error)
    {
        return RULE_RESULT_ERROR;
    }

    //  2.  For every product that is of type Daily

    myki_br_InitProductIterator(pMYKI_TAControl, &iDay, PRODUCT_TYPE_DAILY);
    while (myki_br_ProductIterate(&iDay) > 0)
    {
        //  a.  If an n-Hour product exists where the following is all true:

        ProductIterator iHour;
        CsDbg(BRLL_RULE, "BR_LLSC_9_1 : Scanning for nHour products covered by daily");
        myki_br_InitProductIterator(pMYKI_TAControl, &iHour, PRODUCT_TYPE_NHOUR);
        while (myki_br_ProductIterate(&iHour) > 0)
        {
            //  i.   product control bitmap indiciates that this is not a border product, and
            //  ii.  it is completely covered by the Daily product (ie Daily product low zone is less
            //       than or equal to the low zone of the n-Hour and the Daily product high zone
            //       is greater than or equal to the n-hour high zone) , and
            //  iii. the expiry of the n-hour is less than expiry or equal to date of the daily product

            if ( ! (iHour.pProduct->ControlBitmap & TAPP_TPRODUCT_CONTROL_BORDER_STATUS_BITMAP)
                && (iDay.pProduct->ZoneLow <= iHour.pProduct->ZoneLow)
                && (iDay.pProduct->ZoneHigh >= iHour.pProduct->ZoneHigh)
                && (iHour.pProduct->EndDateTime <= iDay.pProduct->EndDateTime))
            {
                //  b.   then perform:
                //  i.  A ProductUpdate/Invalidate on the n-hour
                //  ii. If the n-Hour product was the product in use then set the Daily as the product in use.
                CsDbg(BRLL_RULE, "BR_LLSC_9_1 : Hourly Covered by Daily product");
                if (BR_LLSC_9_1_ProductUpdate_and_Invalidate(pData, &iHour, &iDay) < 0)
                {
                    // Note: Error reported by BR_LLSC_9_1_ProductUpdate_and_Invalidate
                    return RULE_RESULT_ERROR;
                }
            }
        }
        if (iHour.error)
        {
            return RULE_RESULT_ERROR;
        }

    }
    if (iDay.error)
    {
        return RULE_RESULT_ERROR;
    }


    //  3.  For every product that is of type Single Trip

    myki_br_InitProductIterator(pMYKI_TAControl, &iSingle, PRODUCT_TYPE_SINGLE);
    while (myki_br_ProductIterate(&iSingle) > 0)
    {
        //  a.  if a single trip product exists such that it is completely covered by either a
        //      daily, weekly or by an Activated e-Pass then

        ProductIterator iAll;
        myki_br_InitProductIterator(pMYKI_TAControl, &iAll, PRODUCT_TYPE_UNKNOWN);
        while (myki_br_ProductIterate(&iAll) > 0)
        {
            //CsDbg(BRLL_RULE, "BR_LLSC_9_1: Product: %d, Status: %d", iAll.currentProduct, iAll.pDirectory->Status );
            if (     iAll.currentProduct == PRODUCT_TYPE_DAILY
                ||   iAll.currentProduct == PRODUCT_TYPE_WEEKLY
                || ( iAll.currentProduct == PRODUCT_TYPE_EPASS && iAll.pDirectory->Status == TAPP_CONTROL_STATUS_ACTIVATED ))
            {

                // is completely covered
                if (   (iAll.pProduct->ZoneLow  <= iSingle.pProduct->ZoneLow)
                    && (iAll.pProduct->ZoneHigh >= iSingle.pProduct->ZoneHigh))
                {
                    //  then
                    //  i.  A ProductUpdate/Invalidate on the single trip product
                    //  ii. If the single trip product  was the product in use then set the covering product in use

                    CsDbg(BRLL_RULE, "BR_LLSC_9_1 : Single Covered by Other product");
                    if (BR_LLSC_9_1_ProductUpdate_and_Invalidate(pData, &iSingle, &iAll) < 0)
                    {
                        // Note: Error reported by BR_LLSC_9_1_ProductUpdate_and_Invalidate
                        return RULE_RESULT_ERROR;
                    }
                }
            }
        }
        if (iAll.error)
        {
            return RULE_RESULT_ERROR;
        }

    }
    if (iSingle.error)
    {
        return RULE_RESULT_ERROR;
    }

    CsDbg(BRLL_RULE, "BR_LLSC_9_1 : Executed");
    return RULE_RESULT_EXECUTED;
}


