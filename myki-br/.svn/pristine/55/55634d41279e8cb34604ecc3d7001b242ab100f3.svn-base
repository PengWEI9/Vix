/**************************************************************************
*   ID :  BR_LLSC_5_4
*    Scan-On e-Pass
*    1.  TAppControl.Directory.Status
*    2.  TAppTProduct.EndDateTime
*    3.  TAppTProduct.ZoneLow
*    4.  TAppTProduct.ZoneHigh
*    5.  Dynamic.CurrentZone
*    6.  Dynamic.CurrentDateTime
*    7.  Tariff.AllowEPassOutOfZone
*
*   Pre-Conditions
*    1.  The flag is set to disallow out-of-zone travel with an e-Pass product(7) .
*    2.  An e-Pass product exists on the smartcard where:
*         a.  The product expiry(2)  is greater than or equal to the current date/time(6)  (the product has not expired).
*         b.  The current zone(5)  is within the product low zone(3)  and the product high zone(4)  of the e-Pass product.
*    3.  Either of the following is true:
*         i.  There is an e-Pass product with status(1)  is Inactive and the start date(8)  of the inactive e-Pass product is less than the current date time(6)  and
*             there is no other e-Pass product with product status(1)  Active,
*         ii. There is an e-Pass product with the product status(1)  Active.
*
*   Description
*    1.  With the e-Pass product:
*         a.  If the product status(1)  of the e-Pass product is set to Inactive, perform a ProductUpdate/Activate transaction for the e-Pass product.
*         b.  Perform a ProductUsage/ScanOnEPass transaction for the e-Pass product.
*    2.  Update usage log
*         a.  Definition:
*              i.  TxType = As per Sequence Type (Entry Exit = 7, Exit Only =18; Entry Only =19, On board= 20),
*         b.  Value:  not defined
*         c.  Usage:
*              i.  Zone = Dynamic.Zone(11)
*              ii. ProductValidationStatus:
*                   (1)   Scan-On,
*                   (2)   Provisional,
*                   (3)  If the e-Pass was activated at 1.a Perform also set Activated
*         d.  Product:
*              i.   ProductIssuerId  = As returned from ProductUsage/ScanOnEpass transaction at step 1b
*              ii.  ProductSerialNo =  As returned from ProductUsage/ScanOnEpass transaction at step 1b
*              iii. ProductId = As returned from ProductUsage/ScanOnEpass transaction at step 1b
*
*   Post-Conditions
*    1.  If inactive, an e-Pass product is activated.
*    a.  An e-Pass product is in a scanned-on state.
*
*   Devices
*    Fare payment devices
*
 ***********************************************************************/

#include <cs.h>                 // For debug logging
#include <myki_cardservices.h>  // Myki Card Services
#include <myki_cdd_enums.h>
#include <LDT.h>                // Logical Device Transaction

#include "myki_br_rules.h"      // API for all BR_xxx modules
#include "BR_Common.h"          // Common utilities, macros, etc

//=============================================================================
//
//
//
//=============================================================================

RuleResult_e BR_LLSC_5_4( MYKI_BR_ContextData_t *pData )
{
    MYKI_TAControl_t    *pMYKI_TAControl;

    MYKI_Directory_t    *pDirInUse = NULL;
    MYKI_Directory_t    *pDirActive = NULL;
    MYKI_Directory_t    *pDirInactive = NULL;
    MYKI_TAProduct_t    *pPdtInactive = NULL;
    U8_t                 active = 0;
    U8_t                 dir;
    int                  isProductActivated = FALSE;

    CsDbg(BRLL_RULE,"BR_LLSC_5_4 - Start Scan-On e_Pass");

    if ( ! pData )
    {
        CsErrx( "BR_LLSC_5_4 : Invalid argument(s)" );
        return RULE_RESULT_ERROR;
    }

    //-------------------------------------------------------------------------
    //
    //  Pre-conditions
    //
    //-------------------------------------------------------------------------

    // 1.  The flag is set to disallow out-of-zone travel with an e-Pass product(7) .

    if ( pData->Tariff.allowEPassOutOfZone )
    {
        CsDbg( BRLL_RULE, "BR_LLSC_5_4 : Bypass - pTarrif->allowEPassOutOfZone is TRUE");
        return RULE_RESULT_BYPASSED;
    }

    // 2.  An e-Pass product exists on the smartcard where:
    // 2.a.  The product expiry(2)  is greater than or equal to the current date/time(6)  (the product has not expired).
    // 2.b.  The current zone(5)  is within the product low zone(3)  and the product high zone(4)  of the e-Pass product.

    // 3.  Either of the following is true:
    // 3.i.  There is an e-Pass product with status(1)  is Inactive and the start date(8)  of the inactive e-Pass product is less than the current date time(6)  and there is no other e-Pass product with product status(1)  Active,
    // 3.ii. There is an e-Pass product with the product status(1)  Active.

    if ( MYKI_CS_TAControlGet( &pMYKI_TAControl ) )
    {
        CsErrx( "BR_LLSC_5_4: MYKI_CS_TAControlGet() failed" );
        return RULE_RESULT_ERROR;
    }

    for ( dir = 1; dir < DIMOF( pMYKI_TAControl->Directory ); dir++ ) // T-Purse dir entry does not have a product slot, so there's one less product slots than dir entries
    {
        MYKI_Directory_t    *pDirectory = &pMYKI_TAControl->Directory[ dir ]; // Product slot 0-4 maps to dir entry 1-5
        MYKI_TAProduct_t    *pstProduct;

        CsDbg(BRLL_RULE, "BR_LLSC_5_4: Checking product %d (dir entry %d)", dir-1, dir);

        if (pDirectory->Status != TAPP_CONTROL_DIRECTORY_STATUS_UNUSED &&
            myki_br_cd_GetProductType( pDirectory->ProductId ) == PRODUCT_TYPE_EPASS)
        {
            if (MYKI_CS_TAProductGet( (U8_t)( dir - 1 ), &pstProduct ) >= 0)
            {
                // Condition 2.a
                if (pstProduct->EndDateTime < pData->DynamicData.currentDateTime)
                {
                    CsDbg(BRLL_RULE, "BR_LLSC_5_4: product %d (dir entry %d) has expired : skip", dir-1, dir);
                    continue;
                }

                CsDbg(BRLL_RULE, "BR_LLSC_5_4: Checking product %d (dir entry %d) - not expired", dir-1, dir);

                // Condition 2.b
                if (pData->DynamicData.currentZone > pstProduct->ZoneHigh ||
                    pData->DynamicData.currentZone < pstProduct->ZoneLow )
                {
                    CsDbg(BRLL_RULE, "BR_LLSC_5_4 : product %d (dir entry %d) zone not match current location : skip", dir-1, dir);
                    continue;
                }

                // Condition 3
                if (pDirectory->Status == TAPP_CONTROL_DIRECTORY_STATUS_ACTIVATED)
                {
                    active++;
                    pDirActive = pDirectory;
                }
                else if (pDirectory->Status == TAPP_CONTROL_DIRECTORY_STATUS_ISSUED)
                {
                    if (pstProduct->StartDateTime < pData->DynamicData.currentDateTime)
                    {
                        pDirInactive = pDirectory;
                        pPdtInactive = pstProduct;
                    }
                }
            }
        }
    }

    //  These "pre-conditions" are not in NTS0177, but are implied :
    //      There is a single (but not more than one) active product, or
    //      there is no active product, but there is at least one suitable inactive product

    if ( active > 1 )
    {
        CsDbg( BRLL_RULE, "BR_LLSC_5_4 : Bypass - %d active products found. Can not scan on.");
        return RULE_RESULT_BYPASSED;
    }

    if ( active == 0 && pPdtInactive == NULL )
    {
        CsDbg( BRLL_RULE, "BR_LLSC_5_4 : Bypass - No active product and no suitable inactive products found. Can not scan on.");
        return RULE_RESULT_BYPASSED;
    }

    //-------------------------------------------------------------------------
    //
    //  Actions
    //
    //-------------------------------------------------------------------------

    if ( active == 1 )
    {
        pDirInUse = pDirActive;
        CsDbg( BRLL_RULE, "BR_LLSC_5_4 : Selected active product id %d for scan on.", pDirInUse->ProductId );
    }
    else    // No active products, previous pre-condition ensures that there is at least one suitable inactive product at this point
    {
        pDirInUse = pDirInactive;
        CsDbg( BRLL_RULE, "BR_LLSC_5_4 : Selected inactive product id %d for scan on.", pDirInUse->ProductId );
    }

    // With the e-Pass product:
    // 1.a.  If the product status(1)  of the e-Pass product is set to Inactive,
    //      perform a ProductUpdate/Activate transaction for the e-Pass product.

    if ( pDirInUse->Status != TAPP_CONTROL_DIRECTORY_STATUS_ACTIVATED )
    {
        if ( myki_br_ldt_ProductUpdate_Activate( pData, pDirInUse ) < 0 )
        {
            CsErrx( "BR_LLSC_5_4: myki_br_ldt_ProductUpdate_Activate() failed" );
            return RULE_RESULT_ERROR;
        }
        isProductActivated = TRUE;
    }

    // 1.b.  Perform a ProductUsage/ScanOnEPass transaction for the e-Pass product.

    if ( myki_br_ldt_ProductUsage_ScanOnEPass( pData, pDirInUse ) < 0 )
    {
        CsErrx( "BR_LLSC_5_4: myki_br_ldt_ProductUsage_ScanOnEPass() failed" );
        return RULE_RESULT_ERROR;
    }

    //  2.  Update usage log
    //      a.  Definition: 
    {
        //      i.  TxType = As per Sequence Type (Entry Exit = 7, Exit Only =18; Entry Only =19, On board= 20), 
        if ( pData->InternalData.UsageLogData.transactionType == MYKI_BR_TRANSACTION_TYPE_NONE )
        {
            pData->InternalData.UsageLogData.transactionType = myki_br_getTransactionType( pData );
        } 
        //      ii. Determine uselog ControlBitmap though look up of the TxUsageType (Done by framework)
        //      iii.Set TxDateTime as current Date time(6) (Done by framework)
    }

    //      b.  Value: not defined 

    //      c.  Usage: 
    {
        //      i.  Zone = Dynamic.Zone(11)
        pData->InternalData.UsageLogData.isZoneSet                      = TRUE;
        pData->InternalData.UsageLogData.zone                           = pData->DynamicData.currentZone;

        //      ii. ProductValidationStatus: 
        //          (1) Scan-On, 
        //          (2) Provisional, 
        //          (3) If the e-Pass was activated at 1.a Perform alsoset Activated 
        pData->InternalData.UsageLogData.isProductValidationStatusSet   = TRUE;
        pData->InternalData.UsageLogData.productValidationStatus       |= TAPP_USAGE_LOG_PROD_VAL_STATUS_SCAN_ON_BITMAP |
                                                                          TAPP_USAGE_LOG_PROD_VAL_STATUS_PROVISIONAL_BITMAP;
        if ( isProductActivated != FALSE )
        {
            pData->InternalData.UsageLogData.productValidationStatus   |= TAPP_USAGE_LOG_PROD_VAL_STATUS_ACTIVATE_BITMAP;
        }
    }

    //      d.  Product: 
    {
        //      i.  ProductIssuerId = As returned from ProductUsage/ScanOnEpass transaction at step 1b 
        pData->InternalData.UsageLogData.isProductIssuerIdSet           = TRUE;
        pData->InternalData.UsageLogData.productIssuerId                = pDirInUse->IssuerId;

        //      ii. ProductSerialNo = As returned from ProductUsage/ScanOnEpass transaction at step 1b 
        pData->InternalData.UsageLogData.isProductSerialNoSet           = TRUE;
        pData->InternalData.UsageLogData.productSerialNo                = pDirInUse->SerialNo;

        //      iii.ProductId = As returned from ProductUsage/ScanOnEpass transaction at step 1b
        pData->InternalData.UsageLogData.isProductIdSet                 = TRUE;
        pData->InternalData.UsageLogData.productId                      = pDirInUse->ProductId;
    }

    //  NOTE:   TAppUsageLog entry is added by application framework.
    pData->InternalData.IsUsageLogUpdated                               = TRUE;
    CsDbg( BRLL_RULE, "BR_LLSC_5_4 : Updated TAppUsageLog" );

    CsDbg( BRLL_RULE, "BR_LLSC_5_4 : Executed" );
    return RULE_RESULT_EXECUTED;
}

