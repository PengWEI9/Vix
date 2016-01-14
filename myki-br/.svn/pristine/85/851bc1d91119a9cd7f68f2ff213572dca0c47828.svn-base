/* -*- mode: c; tabs: 4; -*- */
/*=============================================================================
**
**    Vix Technology                   Licensed software
**    (C) 2013                         All rights reserved
**
**=============================================================================
**
**  Project/Product : MBU
**  Filename        : BR_LLSC_5_5.c
**  Author(s)       : ?
**
**  ID              : BR_LLSC_5_5 - KA0004 v7.0
**
**  Name            : Scan-On Provisional Product with e-Pass
**
**  Data Fields     :
**
**      1.  TAppControl.ProvisionalFare
**      2.  TAppControl.PassengerCode
**      3.  TAppControl.Directory.Status
**      4.  TAppControl.Directory.ProductId
**      5.  TAppTProduct.ControlBitmap
**      6.  TAppTProduct.PurchaseValue
**      7.  TAppTProduct.EndDateTime
**      8.  TAppTProduct.ZoneLow
**      9.  TAppTProduct.ZoneHigh
**      10. Static.TransportMode
**      11. Dynamic.CurrentZone
**      12. Dynamic.CurrentDateTime
**      13. Dynamic.StopId
**      14. Dynamic.ProvisionalZoneLow
**      15. Dynamic.ProvisionalZoneHigh
**      16. Tariff.AllowEPassOutOfZone
**      17. Tariff.CitySaverZoneFlag
**      18. TappTProduct.StartDateTime
**      19. Static.ServiceProviderID
**      20. Dynamic.EntryPointID
**      21. Dynamic.LineID
**      22. Dynamic.StopID
**
**  Pre-Conditions  :
**
**      1.  The flag is set to allow out-of-zone travel with an e-Pass product.
**      2.  An e-Pass product exists on the smartcard where:
**          a.  The product expiry is greater than or equal to the current
**              date/time (the product has not expired).
**          b.  Either of the following is true
**              i.  There is an e-Pass with product status is Inactive and the
**                  start date time of the inactive e-Pass product is less than
**                  the current date time and there is no other e-Pass with
**                  the status of Active
**              ii. There is an e-Pass product with the product status of Active.
**          c.  The current location; includings its inner zone, outer zone and
**              actual zone is in between the identified e-Pass product’s low
**              zone and high zone.
**
**  Description     :
**
**      1.  Set the provisional fare1 to the correct value for:
**      2.  Usage type (e-Pass).
**          a.  Current zone.
**          b.  Mode of travel.
**          c.  Route/Stop.OR StationID
**      3.  Passenger type (full fare or concession only)
**      4.  If an exception rule for provisional fare exists for the:
**          a.  Passenger type.
**          b.  Provisional low zone and provisional high zone.
**          c.  Current day of the week/date.
**      5.  Then set the provisional fare1 to this exception value.
**      6.  Perform a ProductSale transaction for a provisional product:
**          a.  Set the product ID to n-Hour.
**          b.  If the provisional low zone minus 1 is marked as City Saver
**              set the product low zone to the provisional low zone minus 1,
**              else set the product low zone8 to the provisional low zone.
**          c.  If the provisional high zone is marked as City Saver set the
**              product high zone to the provisional high zone plus 1, else
**              set the product high zone to the provisional high zone.
**          d.  Set the product purchase value to the provisional fare.
**          e.  Set the provisional bit of the product control bitmap to 1.
**      7.  Perform a ProductUpdate/Activate transaction for the provisional product.
**      8.  Perform a ProductUsage/ScanOnEPass transaction for the provisional product.
**      9.  Create usage log
**          a.  Definition:
**              i.  TxType = As per Sequence Type (Entry Exit = 7, Exit Only =18; Entry Only =19, On board= 20),
**              ii. Determine uselog ControlBitmap though look up of the TxUsageType
**              iii.ProviderID = ServiceProviderID
**              iv. TxDateTime = Current Date time
**              v.  Location.EntryPoint = EntryPointID
**              vi. Location.Route = LineID
**              vii.Location.Stop = Stopid
**          b.  Value:  not defined
**          c.  Usage:
**              i.  Zone = Dynamic.Zone
**                  (1) ProductValidationStatus:
**                      (a) Set bit 2 Scan-On true,
**                      (b) Set bit 4 Provisional true
**          d.  Product:
**              i.  ProductIssuerId = As returned from ProductSale transaction at step 6
**              ii. ProductSerialNo = As returned from ProductSale transaction at step 6
**              iii.ProductId = As returned from ProductSale transaction at step 6
**
**  Post-Conditions :
**
**      1.  A provisional fare is stored on the smartcard.
**      2.  A provisional product is created and activated.
**      3.  A provisional product is in a scanned-on state.
**
**  Member(s)       :
**      BR_LLSC_5_5             [public]    business rule
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
**    1.01  26.11.13    ANT   Modify   Added validating e-Pass product against
**                                     current location inner/outer zones
**    1.02  29.11.13    ANT   Modify   Rectified validating start date of
**                                     inactive e-Pass
**
**===========================================================================*/

#include <cs.h>                 // For debug logging
#include <myki_cardservices.h>  // Myki Card Services
#include <myki_cdd_enums.h>
#include <LDT.h>                // Logical Device Transaction

#include "myki_br_rules.h"      // API for all BR_xxx modules
#include "BR_Common.h"          // Common utilities, macros, etc

/*==========================================================================*
**
**  BR_LLSC_5_5
**
**  Description     :
**      Implements business rule BR_LLSC_5_5.
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

RuleResult_e BR_LLSC_5_5( MYKI_BR_ContextData_t *pData )
{
    MYKI_TAControl_t    *pMYKI_TAControl;

    MYKI_Directory_t    *pDirActiveEPass    = NULL;     // An active   EPass dir
    MYKI_Directory_t    *pDirInactiveEPass  = NULL;     // An inactive EPass dir
    MYKI_Directory_t    *pDirProvisional    = NULL;
  //U8_t                 active             = 0;
    int                  ret                = LDT_SUCCESS;
    U8_t                 dir;
    int                  zoneLow;
    int                  zoneHigh;
    int                  dirIndexProvisional;

    CsDbg( BRLL_RULE, "BR_LLSC_5_5 : Start (Scan-On Provisional Product with e-Pass)" );

    if ( ! pData )
    {
        CsErrx( "BR_LLSC_5_5 : Invalid argument(s)" );
        return RULE_RESULT_ERROR;
    }

    if (MYKI_CS_TAControlGet(&pMYKI_TAControl))
    {
        CsErrx( "BR_LLSC_5_5 : MYKI_CS_TAControlGet() failed" );
        return RULE_RESULT_ERROR;
    }

    //  Pre-Conditions

    // 1.  The flag is set to allow out-of-zone travel with an e-Pass product(16) .
    if ( ! pData->Tariff.allowEPassOutOfZone )
    {
        CsDbg( BRLL_RULE, "BR_LLSC_5_5 : Bypass - Tariff.allowEPassOutOfZone is false." );
        return RULE_RESULT_BYPASSED;
    }

    // 2.       An e-Pass product exists on the smartcard where:
    // 2.a.     The product expiry(7) is greater than or equal to the current date/time(12)
    //          (the product has not expired).
    // 2.b.     Either of the following is true
    // 2.b.i.   There is an e-Pass with product status(3) is Inactive  and the start date
    //          time(18) of the inactive e-Pass product is less than the current date time(12)
    //          and there is no other e-Pass with the status of Active(3)
    // 2.b.ii.  There is an e-Pass product with the product status(3) of Active.
    // 2.c.     The current location; includings its inner zone, outer zone and actual
    //          zone is in between the identified e-Pass product’s low zone(3) and high zone(4).
    for ( dir = 1; dir < DIMOF( pMYKI_TAControl->Directory ); dir++ ) // T-Purse dir entry does not have a product slot, so there's one less product slots than dir entries
    {
        MYKI_Directory_t    *pDirectory = &pMYKI_TAControl->Directory[ dir ]; // Product slot 0-4 maps to dir entry 1-5
        MYKI_TAProduct_t    *pstProduct;

        if
        (
            ( pDirectory->Status == TAPP_CONTROL_DIRECTORY_STATUS_ACTIVATED || pDirectory->Status == TAPP_CONTROL_DIRECTORY_STATUS_ISSUED) &&
            ( myki_br_cd_GetProductType( pDirectory->ProductId ) == PRODUCT_TYPE_EPASS )
        )
        {
            if ( MYKI_CS_TAProductGet( (U8_t)( dir - 1 ), &pstProduct ) >= 0 )
            {
                // Condition 2.a (removed because expired active and inactive ePasses are both guaranteed to be invalidated
                //              by the time we get to this rule).
//                if (pstProduct->EndDateTime < pData->DynamicData.currentDateTime)
//                {
//                    CsDbg(BRLL_RULE, "BR_LLSC_5_5 : product %d entry %d has expired", dir-1, dir);
//                    continue;
//                }

                // Condition 2.c
                if
                (
                    (
                        pData->DynamicData.currentZone        > pstProduct->ZoneHigh      ||
                        pData->DynamicData.currentZone        < pstProduct->ZoneLow
                    ) &&
                    (
                        pData->DynamicData.currentInnerZone   > pstProduct->ZoneHigh      ||
                        pData->DynamicData.currentInnerZone   < pstProduct->ZoneLow
                    ) &&
                    (
                        pData->DynamicData.currentOuterZone   > pstProduct->ZoneHigh      ||
                        pData->DynamicData.currentOuterZone   < pstProduct->ZoneLow
                    )
                )
                {
                    CsDbg(BRLL_RULE, "BR_LLSC_5_5 : product %d entry %d zone not match current location", dir-1, dir);

                    if (pDirectory->Status == TAPP_CONTROL_DIRECTORY_STATUS_ACTIVATED)
                    {
                        CsDbg( BRLL_RULE, "BR_LLSC_5_5 : BYPASSED : Active ePass not in range");
                        return RULE_RESULT_BYPASSED;
                    }

                    continue;
                }

                // Condition 2.b
                if (pDirectory->Status == TAPP_CONTROL_DIRECTORY_STATUS_ACTIVATED)
                {
                    pDirActiveEPass = pDirectory;
                    break;
                }
                else
                {
                    if (pstProduct->StartDateTime <= pData->DynamicData.currentDateTime)
                    {
                        pDirInactiveEPass = pDirectory;
                        break;
                    }
                }
            }
        }
    }

    // Check we can find a valid ePass during scan-on
    // NOTE: The epass is not actually used for scan on.
    //       The present of an ePass allow the creation of zero fare provision product at scan-on
    //       The zero fare provisional product is used for scan on.

    if ( ! pDirActiveEPass && ! pDirInactiveEPass )
    {
        CsDbg( BRLL_RULE, "BR_LLSC_5_5 : BYPASSED : No ePass in range");
        return RULE_RESULT_BYPASSED;
    }

    // Description

    // 1.  Set the provisional fare1 to the correct value for: epass, currentzone, rail/bus, passenger

    // 2.  Usage type (e-Pass).
    // 2.a.  Current zone(11) .
    //pData->DynamicData.currentZone

    // 2.b.  Mode of travel(10) .
    //pData->StaticData.transportMode

    // 2.c.  Route/Stop(13) .OR StationID
    //pData->DynamicData.stopId


    // 3.  Passenger type(2)  (full fare or concession only)
    //pMYKI_TAControl->PassengerCode

    pMYKI_TAControl->ProvisionalFare = 0;       // need to be getProvisionalEpassFare. They are all zero in the CD anyway

    // 4 ~ 5 are removed

    // 6.   Perform a ProductSale transaction for a provisional product:
    {
        // 6b.
        if ( MYKI_CD_isCitySaverZone( (U8_t)( pData->DynamicData.provisionalZoneLow - 1 ) ) )
            zoneLow  = pData->DynamicData.provisionalZoneLow - 1;
        else
            zoneLow  = pData->DynamicData.provisionalZoneLow;

        // 6c.
        if ( MYKI_CD_isCitySaverZone( (U8_t)( pData->DynamicData.provisionalZoneHigh ) ) )
            zoneHigh  = pData->DynamicData.provisionalZoneHigh + 1;
        else
            zoneHigh  = pData->DynamicData.provisionalZoneHigh;

        // Now perform the ProductSale
        if ( ( dirIndexProvisional = myki_br_ldt_ProductSale( pData, myki_br_cd_GetProductId(PRODUCT_TYPE_NHOUR), zoneLow, zoneHigh, pMYKI_TAControl->ProvisionalFare, TRUE ) ) < 0 )
        {
            CsErrx( "BR_LLSC_5_5 : myki_br_ldt_ProductSale() failed" );
            return RULE_RESULT_ERROR;
        }

        CsDbg
        (
            BRLL_RULE,
            "BR_LLSC_5_5 : myki_br_ldt_ProductSale( id = %d, zl = %d, zh = %d, pv = %d ) returned index %d",
            myki_br_cd_GetProductId( PRODUCT_TYPE_NHOUR ),
            zoneLow,
            zoneHigh,
            pMYKI_TAControl->ProvisionalFare,
            dirIndexProvisional
        );
    }

    pDirProvisional = &pMYKI_TAControl->Directory[ dirIndexProvisional ];

    // 7.   Perform a ProductUpdate/Activate transaction for the provisional product.
    ret = myki_br_ldt_ProductUpdate_Activate(pData, pDirProvisional);

    if (ret != LDT_SUCCESS)
    {
        CsErrx("BR_LLSC_5_5 : myki_br_ldt_ProductUpdate_Activate returned %d", ret);
        return RULE_RESULT_ERROR;
    }

    // 8.   Perform a ProductUsage/ScanOnEPass transaction for the provisional product.
    ret = myki_br_ldt_ProductUsage_ScanOnEPass(pData, pDirProvisional);

    if (ret != LDT_SUCCESS)
    {
        CsErrx("BR_LLSC_5_5 : myki_br_ldt_ProductUsage_ScanOnEPass returned %d", ret);
        return RULE_RESULT_ERROR;
    }

    //  9.  Create usage log
    //      a.  Definition: 
    {
        //      i.  TxType = As per Sequence Type (Entry Exit = 7, Exit Only =18; Entry Only =19, On board= 20), 
        if ( pData->InternalData.UsageLogData.transactionType == MYKI_BR_TRANSACTION_TYPE_NONE )
        {
            pData->InternalData.UsageLogData.transactionType = myki_br_getTransactionType( pData );
        } 
        //      ii. Determine uselog ControlBitmap though look up of the TxUsageType  (Done by framework)
        //      iii.ProviderID = ServiceProviderID(19) (Done by framework)
        //      iv. TxDateTime = Current Date time(12) (Done by framework)
        //      v.  Location.EntryPoint = EntryPointID(20) (Done by framework)
        //      vi. Location.Route= LineID(21) (Done by framework)
        //      vii.Location.Stop= Stopid(22) (Done by framework)
    }

    //      b.  Value: not defined 

    //      c.  Usage
    {
        //      i.  Zone = Dynamic.Zone(11)
        pData->InternalData.UsageLogData.isZoneSet                      = TRUE;
        pData->InternalData.UsageLogData.zone                           = pData->DynamicData.currentZone;

        //      ii. ProductValidationStatus:
        //          (a) Set bit 2 Scan-On true , 
        //          (b) Set bit 4 Provisional true 
        pData->InternalData.UsageLogData.isProductValidationStatusSet   = TRUE;
        pData->InternalData.UsageLogData.productValidationStatus       |= TAPP_USAGE_LOG_PROD_VAL_STATUS_SCAN_ON_BITMAP |
                                                                          TAPP_USAGE_LOG_PROD_VAL_STATUS_PROVISIONAL_BITMAP;
    }

    //      d.  Product: 
    {
        //      i.  ProductIssuerId = As returned from ProductSale transaction at step 6 
        pData->InternalData.UsageLogData.isProductIssuerIdSet           = TRUE;
        pData->InternalData.UsageLogData.productIssuerId                = pDirProvisional->IssuerId;

        //      ii. ProductSerialNo = As returned from ProductSaletransaction at step 6 
        pData->InternalData.UsageLogData.isProductSerialNoSet           = TRUE;
        pData->InternalData.UsageLogData.productSerialNo                = pDirProvisional->SerialNo;

        //      iii.ProductId = As returned from ProductSale transaction at step 6
        pData->InternalData.UsageLogData.isProductIdSet                 = TRUE;
        pData->InternalData.UsageLogData.productId                      = pDirProvisional->ProductId;
    }

    //  NOTE:   TAppUsageLog entry is added by application framework.
    pData->InternalData.IsUsageLogUpdated                               = TRUE;
    CsDbg( BRLL_RULE, "BR_LLSC_5_5 : Updated TAppUsageLog" );

    CsDbg(BRLL_RULE, "BR_LLSC_5_5 : Executed" );
    return RULE_RESULT_EXECUTED;
}

