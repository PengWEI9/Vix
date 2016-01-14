/* -*- mode: c; tabs: 4; -*- */
/*=============================================================================
**
**    Vix Technology                   Licensed software
**    (C) 2013                         All rights reserved
**
**=============================================================================
**
**  Project/Product : MBU
**  Filename        : test_BR_LLSC_1_5.c
**  Author(s)       : An Tran
**
**  Description     :
**      Implements BR_LLSC_1_5 business rule unit-tests.
**
**  Function(s)     :
**      Template                [Public]    template function
**
**  Information     :
**   Compiler(s)    : ANSI C
**   Target(s)      : Independent
**
**  History         :
**   Vers.  Date        Aut.  Type     Description
**   -----  ----------  ----  -------  ----------------------------------------
**    1.00  03.10.13    ANT   Create
**
**===========================================================================*/

/*
 *      Options
 *      -------
 */

/*
 *      Includes
 *      --------
 */

#include <cs.h>
#include <myki_cardservices.h>
#include <myki_cdd_enums.h>
#include <myki_br.h>
#include <myki_br_rules.h>
#include <myki_br_context_data.h>

#include "test_common.h"

/*
 *      External References
 *      -------------------
 */

/*
 *      Local Constants and Macros
 *      --------------------------
 */

/*
 *      Local Data Types
 *      ----------------
 */

/*
 *      Local Function Prototypes
 *      -------------------------
 */

/*
 *      Local Variables
 *      ---------------
 */

/*
 *      Global Variables
 *      ----------------
 */

/*==========================================================================*
**
**  test_BR_LLSC_1_5_001
**
**  Description     :
**      Unit-test BYPASSED conditions.
**
**  Parameters      :
**      pData           [I/O]   BR context data
**
**  Returns         :
**      TRUE                    test passed
**      FALSE                   test failed
**
**  Notes           :
**
**
**==========================================================================*/

int test_BR_LLSC_1_5_001( MYKI_BR_ContextData_t *pData )
{
    MYKI_TACapping_t       *pMYKI_TACapping         = NULL;
    MYKI_TAControl_t       *pMYKI_TAControl         = NULL;
//  MYKI_Directory_t       *pMYKI_Directory         = NULL;
    MYKI_TAProduct_t       *pMYKI_TAProduct         = NULL;
    U8_t                    nDirIndex               = 2;
    int                     nProductId_Daily        = 5;
//  int                     nProductId_Entitlement  = 100;

    if ( pData == NULL )
    {
        return FALSE;
    }   /* end-of-if */

    if ( MYKI_CS_TACappingGet( &pMYKI_TACapping ) < 0 || pMYKI_TACapping == NULL )
    {
        return FALSE;
    }   /* end-of-if */

    if ( MYKI_CS_TAControlGet( &pMYKI_TAControl ) < 0 || pMYKI_TAControl == NULL )
    {
        return FALSE;
    }   /* end-of-if */

    if ( MYKI_CS_TAProductGet( (U8_t)( nDirIndex - 1 ), &pMYKI_TAProduct ) < 0 || pMYKI_TAProduct == NULL )
    {
        return FALSE;
    }   /* end-of-if */

    pData->DynamicData.previousDailyCapZoneLow  = 2;
    pData->DynamicData.previousDailyCapZoneHigh = 2;
    pMYKI_TACapping->Daily.Zone.Low             = 2;
    pMYKI_TACapping->Daily.Zone.High            = 2;

    /*  Pre-Conditions:
            Dynamic.IsForcedScanOff = false
            Dynamic.CurrentTripZoneLow < Dynamic.PreviousDailyCapZoneLow

        Post-Conditions:
            BYPASSED
    */
    {
        pData->DynamicData.isForcedScanOff                  = FALSE;
        pData->DynamicData.currentTripZoneLow               = pData->DynamicData.previousDailyCapZoneLow - 1;
        pData->DynamicData.currentTripZoneHigh              = pData->DynamicData.previousDailyCapZoneHigh;
        if ( BR_LLSC_1_5( pData ) != RULE_RESULT_BYPASSED )
        {
            return FALSE;
        }   /* end-of-if */
    }

    /*  Pre-Conditions:
            Dynamic.IsForcedScanOff = false
            Dynamic.CurrentTripZoneLow > Dynamic.PreviousDailyCapZoneHigh

        Post-Conditions:
            BYPASSED
    */
    {
        pData->DynamicData.isForcedScanOff                  = FALSE;
        pData->DynamicData.currentTripZoneLow               = pData->DynamicData.previousDailyCapZoneHigh + 1;
        pData->DynamicData.currentTripZoneHigh              = pData->DynamicData.previousDailyCapZoneHigh + 1;
        if ( BR_LLSC_1_5( pData ) != RULE_RESULT_BYPASSED )
        {
            return FALSE;
        }   /* end-of-if */
    }

    /*  Pre-Conditions:
            Dynamic.IsForcedScanOff = false
            Dynamic.CurrentTripZoneHigh < Dynamic.PreviousDailyCapZoneLow

        Post-Conditions:
            BYPASSED
    */
    {
        pData->DynamicData.isForcedScanOff                  = FALSE;
        pData->DynamicData.currentTripZoneLow               = pData->DynamicData.previousDailyCapZoneLow;
        pData->DynamicData.currentTripZoneHigh              = pData->DynamicData.previousDailyCapZoneLow - 1;
        if ( BR_LLSC_1_5( pData ) != RULE_RESULT_BYPASSED )
        {
            return FALSE;
        }   /* end-of-if */
    }

    /*  Pre-Conditions:
            Dynamic.IsForcedScanOff = false
            Dynamic.CurrentTripZoneHigh > Dynamic.PreviousDailyCapZoneHigh

        Post-Conditions:
            BYPASSED
    */
    {
        pData->DynamicData.isForcedScanOff                  = FALSE;
        pData->DynamicData.currentTripZoneLow               = pData->DynamicData.previousDailyCapZoneLow;
        pData->DynamicData.currentTripZoneHigh              = pData->DynamicData.previousDailyCapZoneHigh + 1;
        if ( BR_LLSC_1_5( pData ) != RULE_RESULT_BYPASSED )
        {
            return FALSE;
        }   /* end-of-if */
    }

    /*  Pre-Conditions:
            Dynamic.IsForcedScanOff = false
            Dynamic.CurrentTripZoneLow = Dynamic.PreviousDailyCapZoneLow
            Dynamic.CurrentTripZoneHigh = Dynamic.PreviousDailyCapZoneHigh
            TAppCapping.Daily.Value < Tariff.DailyCapValue
            Tariff.EntitlementProduct = 0

        Post-Conditions:
            BYPASSED
    */
    {
/*      pData->Tariff.dailyCapValue                         = 1108; */
        pData->Tariff.entitlementProduct                    = 0;
        pMYKI_TACapping->Daily.Value                        = 1000;
        pData->DynamicData.isForcedScanOff                  = FALSE;
        pData->DynamicData.currentTripZoneLow               = pData->DynamicData.previousDailyCapZoneLow;
        pData->DynamicData.currentTripZoneHigh              = pData->DynamicData.previousDailyCapZoneHigh;

        if ( BR_LLSC_1_5( pData ) != RULE_RESULT_BYPASSED )
        {
            return FALSE;
        }   /* end-of-if */
    }

    /*  Pre-Conditions:
            Dynamic.IsForcedScanOff = false
            Dynamic.CurrentTripZoneLow = Dynamic.PreviousDailyCapZoneLow
            Dynamic.CurrentTripZoneHigh = Dynamic.PreviousDailyCapZoneHigh
            TAppCapping.Daily > Tariff.DailyCapValue
            Tariff.EntitlementProduct = 0
            TAppControl.Directory[1].ProductId = "DAILY"
            TAppTProduct.ZoneLow = TAppCapping.Daily.ZoneLow
            TAppTProduct.ZoneHigh = TAppCapping.Daily.ZoneHigh

        Post-Conditions:
            BYPASSED
    */
    {
/*      pData->Tariff.dailyCapValue                         = 1108; */
        pData->Tariff.entitlementProduct                    = 0;
        pMYKI_TACapping->Daily.Value                        = 1200;
        pMYKI_TAControl->Directory[ nDirIndex ].Status      = TAPP_CONTROL_DIRECTORY_STATUS_ACTIVATED;
        pMYKI_TAControl->Directory[ nDirIndex ].ProductId   = nProductId_Daily;
        pMYKI_TAProduct->ZoneLow                            = pMYKI_TACapping->Daily.Zone.Low;
        pMYKI_TAProduct->ZoneHigh                           = pMYKI_TACapping->Daily.Zone.High;
        pData->DynamicData.isForcedScanOff                  = FALSE;
        pData->DynamicData.currentTripZoneLow               = pData->DynamicData.previousDailyCapZoneLow;
        pData->DynamicData.currentTripZoneHigh              = pData->DynamicData.previousDailyCapZoneHigh;
        if ( BR_LLSC_1_6( pData ) != RULE_RESULT_BYPASSED )
        {
            return FALSE;
        }   /* end-of-if */
    }

    /*  Pre-Conditions:
            Dynamic.IsForcedScanOff = true
            Dynamic.ForceScanDate != Dynamic.CurrentBusinessDate
            Dynamic.CurrentTripZoneLow = Dynamic.PreviousDailyCapZoneLow
            Dynamic.CurrentTripZoneHigh = Dynamic.PreviousDailyCapZoneHigh
            TAppCapping.Daily > Tariff.DailyCapValue
            Tariff.EntitlementProduct = 0

        Post-Conditions:
            BYPASSED
    */
    {
/*      pData->Tariff.dailyCapValue                         = 1108; */
        pData->Tariff.entitlementProduct                    = 0;
        pMYKI_TACapping->Daily.Value                        = 1200;
        pData->DynamicData.isForcedScanOff                  = TRUE;
        pData->DynamicData.forcedScanOffDateTime            = pData->DynamicData.currentBusinessDate - 1;
        pData->DynamicData.currentTripZoneLow               = pData->DynamicData.previousDailyCapZoneLow;
        pData->DynamicData.currentTripZoneHigh              = pData->DynamicData.previousDailyCapZoneHigh;

        if ( BR_LLSC_1_6( pData ) != RULE_RESULT_BYPASSED )
        {
            return FALSE;
        }   /* end-of-if */
    }

    return TRUE;
}   /* test_BR_LLSC_1_5_001( ) */

/*==========================================================================*
**
**  test_BR_LLSC_1_5_002
**
**  Description     :
**      Unit-test EXECUTED conditions, ProductInUse N-HOURS upgraded to
**      DAILY product.
**
**  Parameters      :
**      pData           [I/O]   BR context data
**
**  Returns         :
**      TRUE                    test passed
**      FALSE                   test failed
**
**  Notes           :
**
**
**==========================================================================*/

int test_BR_LLSC_1_5_002( MYKI_BR_ContextData_t *pData )
{
    MYKI_TACapping_t       *pMYKI_TACapping         = NULL;
    MYKI_TAControl_t       *pMYKI_TAControl         = NULL;
//  MYKI_Directory_t       *pMYKI_Directory         = NULL;
    MYKI_TAProduct_t       *pMYKI_TAProduct         = NULL;
    MYKI_CAControl_t       *pCAControl              = NULL;
    U8_t                    nDirIndex               = 2;
//  int                     nProductId_SingleTrip   = 3;
    int                     nProductId_NHours       = 4;
//  int                     nProductId_Daily        = 5;
//  int                     nProductId_Entitlement  = 100;
    int                     nProductSerial          = 99;

    if ( pData == NULL )
    {
        return FALSE;
    }   /* end-of-if */

    if ( MYKI_CS_TACappingGet( &pMYKI_TACapping ) < 0 || pMYKI_TACapping == NULL )
    {
        return FALSE;
    }   /* end-of-if */

    if ( MYKI_CS_TAControlGet( &pMYKI_TAControl ) < 0 || pMYKI_TAControl == NULL )
    {
        return FALSE;
    }   /* end-of-if */

    if ( MYKI_CS_TAProductGet( (U8_t)( nDirIndex - 1 ), &pMYKI_TAProduct ) < 0 || pMYKI_TAProduct == NULL )
    {
        return FALSE;
    }   /* end-of-if */

    if ( MYKI_CS_CAControlGet(&pCAControl) != MYKI_CS_OK )
    {
        return FALSE;
    }

    pData->DynamicData.previousDailyCapZoneLow  = 1;
    pData->DynamicData.previousDailyCapZoneHigh = 3;
    pMYKI_TACapping->Daily.Zone.Low             = 1;
    pMYKI_TACapping->Daily.Zone.High            = 3;

    /*  Pre-Conditions:
            TAppCapping.Daily > Tariff.DailyCapValue
            Tariff.EntitlementProduct = 0 (ie. does not exist)
            TAppControl.ProductInUse = 2
            TAppControl.Directory[1].ProductId = "N-HOURS"
            TAppTProduct.ZoneLow = Dynamic.CurrentTripZoneLow
            TAppTProduct.ZoneHigh = Dynamic.CurrentTripZoneHigh
            TAppTProduct.ControlBitmap.BorderActive = 0
            TAppTProduct.EndDateTime <= business day for common date (Dynamic.CurrentDateTime)
            Dynamic.IsForcedScanOff = 0

        Post-Conditions:
            ProductInUse upgraded to DAILY product.
            TAppTProduct.PurchaseValue = 1108
            TAppTProduct.ZoneLow = 1
            TAppTProduct.ZoneHigh = 3
    */
    {
        pMYKI_TAControl->Status                             = TAPP_CONTROL_STATUS_ACTIVATED;
        pCAControl->Status                                  = CARD_CONTROL_STATUS_ACTIVATED;
        pData->DynamicData.isForcedScanOff                  = FALSE;
/*      pData->Tariff.dailyCapValue                         = 1108; */
        pData->Tariff.entitlementProduct                    = 0;
        pData->Tariff.endOfBusinessDay                      = ( 1 /*AM*/ * 60 );
        pMYKI_TACapping->Daily.Value                        = 1200;
        pData->DynamicData.currentTripZoneLow               = pData->DynamicData.previousDailyCapZoneLow;
        pData->DynamicData.currentTripZoneHigh              = pData->DynamicData.previousDailyCapZoneHigh;
        pMYKI_TAControl->ProductInUse                       = nDirIndex;
        pMYKI_TAControl->Directory[ nDirIndex ].Status      = TAPP_CONTROL_DIRECTORY_STATUS_ACTIVATED;
        pMYKI_TAControl->Directory[ nDirIndex ].ProductId   = nProductId_NHours;
        pMYKI_TAControl->Directory[ nDirIndex ].SerialNo    = nProductSerial;
        pMYKI_TAProduct->ZoneLow                            = pData->DynamicData.currentTripZoneLow;
        pMYKI_TAProduct->ZoneHigh                           = pData->DynamicData.currentTripZoneHigh;
        pMYKI_TAProduct->ControlBitmap                     &= ~( TAPP_TPRODUCT_CONTROL_BORDER_STATUS_BITMAP );
        pMYKI_TAProduct->EndDateTime                        = pData->DynamicData.currentDateTime;

        if ( BR_LLSC_1_5( pData ) != RULE_RESULT_EXECUTED )
        {
            return FALSE;
        }   /* end-of-if */
    }

    return TRUE;
}   /* test_BR_LLSC_1_5_002( ) */

/*==========================================================================*
**
**  test_BR_LLSC_1_5_003
**
**  Description     :
**      Unit-test EXECUTED conditions, ProductInUse SINGLE TRIP upgraded to
**      DAILY product.
**
**  Parameters      :
**      pData           [I/O]   BR context data
**
**  Returns         :
**      TRUE                    test passed
**      FALSE                   test failed
**
**  Notes           :
**
**
**==========================================================================*/

int test_BR_LLSC_1_5_003( MYKI_BR_ContextData_t *pData )
{
    MYKI_TACapping_t       *pMYKI_TACapping         = NULL;
    MYKI_TAControl_t       *pMYKI_TAControl         = NULL;
//  MYKI_Directory_t       *pMYKI_Directory         = NULL;
    MYKI_TAProduct_t       *pMYKI_TAProduct         = NULL;
    MYKI_CAControl_t       *pCAControl              = NULL;
    U8_t                    nDirIndex               = 2;
    int                     nProductId_SingleTrip   = 3;
//  int                     nProductId_NHours       = 4;
//  int                     nProductId_Daily        = 5;
//  int                     nProductId_Entitlement  = 100;
    int                     nProductSerial          = 99;

    if ( pData == NULL )
    {
        return FALSE;
    }   /* end-of-if */

    if ( MYKI_CS_TACappingGet( &pMYKI_TACapping ) < 0 || pMYKI_TACapping == NULL )
    {
        return FALSE;
    }   /* end-of-if */

    if ( MYKI_CS_TAControlGet( &pMYKI_TAControl ) < 0 || pMYKI_TAControl == NULL )
    {
        return FALSE;
    }   /* end-of-if */

    if ( MYKI_CS_TAProductGet( (U8_t)( nDirIndex - 1 ), &pMYKI_TAProduct ) < 0 || pMYKI_TAProduct == NULL )
    {
        return FALSE;
    }   /* end-of-if */

    if ( MYKI_CS_CAControlGet(&pCAControl) != MYKI_CS_OK )
    {
        return FALSE;
    }

    pData->DynamicData.previousDailyCapZoneLow  = 1;
    pData->DynamicData.previousDailyCapZoneHigh = 3;
    pMYKI_TACapping->Daily.Zone.Low             = 1;
    pMYKI_TACapping->Daily.Zone.High            = 3;

    /*  Pre-Conditions:
            TAppCapping.Daily > Tariff.DailyCapValue
            Tariff.EntitlementProduct = 0 (ie. does not exist)
            TAppControl.ProductInUse = 2
            TAppControl.Directory[1].ProductId = "SINGLE TRIP"
            TAppTProduct.ZoneLow = Dynamic.CurrentTripZoneLow
            TAppTProduct.ZoneHigh = Dynamic.CurrentTripZoneHigh
            Dynamic.IsForcedScanOff = 0

        Post-Conditions:
            ProductInUse upgraded to DAILY product.
            TAppTProduct.PurchaseValue = 1108
            TAppTProduct.ZoneLow = 1
            TAppTProduct.ZoneHigh = 3
    */
    {
        pMYKI_TAControl->Status                             = TAPP_CONTROL_STATUS_ACTIVATED;
        pCAControl->Status                                  = CARD_CONTROL_STATUS_ACTIVATED;
        pData->DynamicData.isForcedScanOff                  = FALSE;
/*      pData->Tariff.dailyCapValue                         = 1108; */
        pData->Tariff.entitlementProduct                    = 0;
        pData->Tariff.endOfBusinessDay                      = ( 1 /*AM*/ * 60 );
        pMYKI_TACapping->Daily.Value                        = 1200;
        pData->DynamicData.currentTripZoneLow               = pData->DynamicData.previousDailyCapZoneLow;
        pData->DynamicData.currentTripZoneHigh              = pData->DynamicData.previousDailyCapZoneHigh;
        pMYKI_TAControl->ProductInUse                       = nDirIndex;
        pMYKI_TAControl->Directory[ nDirIndex ].Status      = TAPP_CONTROL_DIRECTORY_STATUS_ACTIVATED;
        pMYKI_TAControl->Directory[ nDirIndex ].ProductId   = nProductId_SingleTrip;
        pMYKI_TAControl->Directory[ nDirIndex ].SerialNo    = nProductSerial;
        pMYKI_TAProduct->ZoneLow                            = pData->DynamicData.currentTripZoneLow;
        pMYKI_TAProduct->ZoneHigh                           = pData->DynamicData.currentTripZoneHigh;
        pMYKI_TAProduct->ControlBitmap                     &= ~( TAPP_TPRODUCT_CONTROL_BORDER_STATUS_BITMAP );
        pMYKI_TAProduct->EndDateTime                        = pData->DynamicData.currentDateTime;

        if ( BR_LLSC_1_5( pData ) != RULE_RESULT_EXECUTED )
        {
            return FALSE;
        }   /* end-of-if */
    }

    return TRUE;
}   /* test_BR_LLSC_1_5_003( ) */

/*==========================================================================*
**
**  test_BR_LLSC_1_5_004
**
**  Description     :
**      Unit-test EXECUTED conditions, no ProductInUse,
**      DAILY product sold.
**
**  Parameters      :
**      pData           [I/O]   BR context data
**
**  Returns         :
**      TRUE                    test passed
**      FALSE                   test failed
**
**  Notes           :
**
**
**==========================================================================*/

int test_BR_LLSC_1_5_004( MYKI_BR_ContextData_t *pData )
{
    MYKI_TACapping_t       *pMYKI_TACapping         = NULL;
    MYKI_TAControl_t       *pMYKI_TAControl         = NULL;
//  MYKI_Directory_t       *pMYKI_Directory         = NULL;
    MYKI_TAProduct_t       *pMYKI_TAProduct         = NULL;
    MYKI_CAControl_t       *pCAControl              = NULL;
    U8_t                    nDirIndex               = 2;
//  int                     nProductId_SingleTrip   = 3;
//  int                     nProductId_NHours       = 4;
//  int                     nProductId_Daily        = 5;
//  int                     nProductId_Entitlement  = 100;
//  int                     nProductSerial          = 99;

    if ( pData == NULL )
    {
        return FALSE;
    }   /* end-of-if */

    if ( MYKI_CS_TACappingGet( &pMYKI_TACapping ) < 0 || pMYKI_TACapping == NULL )
    {
        return FALSE;
    }   /* end-of-if */

    if ( MYKI_CS_TAControlGet( &pMYKI_TAControl ) < 0 || pMYKI_TAControl == NULL )
    {
        return FALSE;
    }   /* end-of-if */

    if ( MYKI_CS_TAProductGet( (U8_t)( nDirIndex - 1 ), &pMYKI_TAProduct ) < 0 || pMYKI_TAProduct == NULL )
    {
        return FALSE;
    }   /* end-of-if */

    if ( MYKI_CS_CAControlGet(&pCAControl) != MYKI_CS_OK )
    {
        return FALSE;
    }

    pData->DynamicData.previousDailyCapZoneLow  = 1;
    pData->DynamicData.previousDailyCapZoneHigh = 3;
    pMYKI_TACapping->Daily.Zone.Low             = 1;
    pMYKI_TACapping->Daily.Zone.High            = 3;

    /*  Pre-Conditions:
            TAppCapping.Daily > Tariff.DailyCapValue
            Tariff.EntitlementProduct = 0 (ie. does not exist)
            TAppControl.ProductInUse = 0
            Dynamic.IsForcedScanOff = 0

        Post-Conditions:
            ProductInUse upgraded to DAILY product.
            TAppTProduct.PurchaseValue = 1108
            TAppTProduct.ZoneLow = 1
            TAppTProduct.ZoneHigh = 3
    */
    {
        pMYKI_TAControl->NextProductSerialNo                = 1;
        pMYKI_TAControl->Status                             = TAPP_CONTROL_STATUS_ACTIVATED;
        pCAControl->Status                                  = CARD_CONTROL_STATUS_ACTIVATED;
        pData->DynamicData.isForcedScanOff                  = FALSE;
/*      pData->Tariff.dailyCapValue                         = 1108; */
        pData->Tariff.entitlementProduct                    = 0;
        pData->Tariff.endOfBusinessDay                      = ( 1 /*AM*/ * 60 );
        pMYKI_TACapping->Daily.Value                        = 1200;
        pData->DynamicData.currentTripZoneLow               = pData->DynamicData.previousDailyCapZoneLow;
        pData->DynamicData.currentTripZoneHigh              = pData->DynamicData.previousDailyCapZoneHigh;
        pMYKI_TAControl->ProductInUse                       = 0;

        if ( BR_LLSC_1_5( pData ) != RULE_RESULT_EXECUTED )
        {
            return FALSE;
        }   /* end-of-if */
    }

    return TRUE;
}   /* test_BR_LLSC_1_5_004( ) */

/*==========================================================================*
**
**  test_BR_LLSC_1_5_005
**
**  Description     :
**      Unit-test EXECUTED conditions, ProductInUse not same zone
**      DAILY product sold.
**
**  Parameters      :
**      pData           [I/O]   BR context data
**
**  Returns         :
**      TRUE                    test passed
**      FALSE                   test failed
**
**  Notes           :
**
**
**==========================================================================*/

int test_BR_LLSC_1_5_005( MYKI_BR_ContextData_t *pData )
{
    MYKI_TACapping_t       *pMYKI_TACapping         = NULL;
    MYKI_TAControl_t       *pMYKI_TAControl         = NULL;
//  MYKI_Directory_t       *pMYKI_Directory         = NULL;
    MYKI_TAProduct_t       *pMYKI_TAProduct         = NULL;
    MYKI_CAControl_t       *pCAControl              = NULL;
    U8_t                    nDirIndex               = 2;
//  int                     nProductId_SingleTrip   = 3;
    int                     nProductId_NHours       = 4;
//  int                     nProductId_Daily        = 5;
//  int                     nProductId_Entitlement  = 100;
    int                     nProductSerial          = 99;

    if ( pData == NULL )
    {
        return FALSE;
    }   /* end-of-if */

    if ( MYKI_CS_TACappingGet( &pMYKI_TACapping ) < 0 || pMYKI_TACapping == NULL )
    {
        return FALSE;
    }   /* end-of-if */

    if ( MYKI_CS_TAControlGet( &pMYKI_TAControl ) < 0 || pMYKI_TAControl == NULL )
    {
        return FALSE;
    }   /* end-of-if */

    if ( MYKI_CS_TAProductGet( (U8_t)( nDirIndex - 1 ), &pMYKI_TAProduct ) < 0 || pMYKI_TAProduct == NULL )
    {
        return FALSE;
    }   /* end-of-if */

    if ( MYKI_CS_CAControlGet(&pCAControl) != MYKI_CS_OK )
    {
        return FALSE;
    }

    pData->DynamicData.previousDailyCapZoneLow  = 1;
    pData->DynamicData.previousDailyCapZoneHigh = 3;
    pMYKI_TACapping->Daily.Zone.Low             = 1;
    pMYKI_TACapping->Daily.Zone.High            = 3;

    /*  Pre-Conditions:
            TAppCapping.Daily > Tariff.DailyCapValue
            Tariff.EntitlementProduct = 0 (ie. does not exist)
            TAppControl.ProductInUse = 0
            Dynamic.IsForcedScanOff = 0

        Post-Conditions:
            ProductInUse upgraded to DAILY product.
            TAppTProduct.PurchaseValue = 1108
            TAppTProduct.ZoneLow = 1
            TAppTProduct.ZoneHigh = 3
    */
    {
        pMYKI_TAControl->NextProductSerialNo                = 1;
        pMYKI_TAControl->Status                             = TAPP_CONTROL_STATUS_ACTIVATED;
        pCAControl->Status                                  = CARD_CONTROL_STATUS_ACTIVATED;
        pData->DynamicData.isForcedScanOff                  = FALSE;
/*      pData->Tariff.dailyCapValue                         = 1108; */
        pData->Tariff.entitlementProduct                    = 0;
        pData->Tariff.endOfBusinessDay                      = ( 1 /*AM*/ * 60 );
        pMYKI_TACapping->Daily.Value                        = 1200;
        pData->DynamicData.currentTripZoneLow               = pData->DynamicData.previousDailyCapZoneLow;
        pData->DynamicData.currentTripZoneHigh              = pData->DynamicData.previousDailyCapZoneHigh;
        pMYKI_TAControl->ProductInUse                       = nDirIndex;
        pMYKI_TAControl->Directory[ nDirIndex ].Status      = TAPP_CONTROL_DIRECTORY_STATUS_ACTIVATED;
        pMYKI_TAControl->Directory[ nDirIndex ].ProductId   = nProductId_NHours;
        pMYKI_TAControl->Directory[ nDirIndex ].SerialNo    = nProductSerial;
        pMYKI_TAProduct->ZoneLow                            = pData->DynamicData.currentTripZoneLow - 1;
        pMYKI_TAProduct->ZoneHigh                           = pData->DynamicData.currentTripZoneHigh;
        pMYKI_TAProduct->ControlBitmap                     &= ~( TAPP_TPRODUCT_CONTROL_BORDER_STATUS_BITMAP );
        pMYKI_TAProduct->EndDateTime                        = pData->DynamicData.currentDateTime;

        if ( BR_LLSC_1_5( pData ) != RULE_RESULT_EXECUTED )
        {
            return FALSE;
        }   /* end-of-if */
    }

    return TRUE;
}   /* test_BR_LLSC_1_5_005( ) */

/*==========================================================================*
**
**  test_BR_LLSC_1_5_006
**
**  Description     :
**      Unit-test EXECUTED conditions, ProductInUse crossed border,
**      DAILY product sold.
**
**  Parameters      :
**      pData           [I/O]   BR context data
**
**  Returns         :
**      TRUE                    test passed
**      FALSE                   test failed
**
**  Notes           :
**
**
**==========================================================================*/

int test_BR_LLSC_1_5_006( MYKI_BR_ContextData_t *pData )
{
    MYKI_TACapping_t       *pMYKI_TACapping         = NULL;
    MYKI_TAControl_t       *pMYKI_TAControl         = NULL;
//  MYKI_Directory_t       *pMYKI_Directory         = NULL;
    MYKI_TAProduct_t       *pMYKI_TAProduct         = NULL;
    MYKI_CAControl_t       *pCAControl              = NULL;
    U8_t                    nDirIndex               = 2;
//  int                     nProductId_SingleTrip   = 3;
    int                     nProductId_NHours       = 4;
//  int                     nProductId_Daily        = 5;
//  int                     nProductId_Entitlement  = 100;
    int                     nProductSerial          = 99;

    if ( pData == NULL )
    {
        return FALSE;
    }   /* end-of-if */

    if ( MYKI_CS_TACappingGet( &pMYKI_TACapping ) < 0 || pMYKI_TACapping == NULL )
    {
        return FALSE;
    }   /* end-of-if */

    if ( MYKI_CS_TAControlGet( &pMYKI_TAControl ) < 0 || pMYKI_TAControl == NULL )
    {
        return FALSE;
    }   /* end-of-if */

    if ( MYKI_CS_TAProductGet( (U8_t)( nDirIndex - 1 ), &pMYKI_TAProduct ) < 0 || pMYKI_TAProduct == NULL )
    {
        return FALSE;
    }   /* end-of-if */

    if ( MYKI_CS_CAControlGet(&pCAControl) != MYKI_CS_OK )
    {
        return FALSE;
    }

    pData->DynamicData.previousDailyCapZoneLow  = 1;
    pData->DynamicData.previousDailyCapZoneHigh = 3;
    pMYKI_TACapping->Daily.Zone.Low             = 1;
    pMYKI_TACapping->Daily.Zone.High            = 3;

    /*  Pre-Conditions:
            TAppCapping.Daily > Tariff.DailyCapValue
            Tariff.EntitlementProduct = 0 (ie. does not exist)
            TAppControl.ProductInUse = 0
            Dynamic.IsForcedScanOff = 0

        Post-Conditions:
            ProductInUse upgraded to DAILY product.
            TAppTProduct.PurchaseValue = 1108
            TAppTProduct.ZoneLow = 1
            TAppTProduct.ZoneHigh = 3
    */
    {
        pMYKI_TAControl->NextProductSerialNo                = 1;
        pMYKI_TAControl->Status                             = TAPP_CONTROL_STATUS_ACTIVATED;
        pCAControl->Status                                  = CARD_CONTROL_STATUS_ACTIVATED;
        pData->DynamicData.isForcedScanOff                  = FALSE;
/*      pData->Tariff.dailyCapValue                         = 1108; */
        pData->Tariff.entitlementProduct                    = 0;
        pData->Tariff.endOfBusinessDay                      = ( 1 /*AM*/ * 60 );
        pMYKI_TACapping->Daily.Value                        = 1200;
        pData->DynamicData.currentTripZoneLow               = pData->DynamicData.previousDailyCapZoneLow;
        pData->DynamicData.currentTripZoneHigh              = pData->DynamicData.previousDailyCapZoneHigh;
        pMYKI_TAControl->ProductInUse                       = nDirIndex;
        pMYKI_TAControl->Directory[ nDirIndex ].Status      = TAPP_CONTROL_DIRECTORY_STATUS_ACTIVATED;
        pMYKI_TAControl->Directory[ nDirIndex ].ProductId   = nProductId_NHours;
        pMYKI_TAControl->Directory[ nDirIndex ].SerialNo    = nProductSerial;
        pMYKI_TAProduct->ZoneLow                            = pData->DynamicData.currentTripZoneLow;
        pMYKI_TAProduct->ZoneHigh                           = pData->DynamicData.currentTripZoneHigh;
        pMYKI_TAProduct->ControlBitmap                     |= ( TAPP_TPRODUCT_CONTROL_BORDER_STATUS_BITMAP );
        pMYKI_TAProduct->EndDateTime                        = pData->DynamicData.currentDateTime;

        if ( BR_LLSC_1_5( pData ) != RULE_RESULT_EXECUTED )
        {
            return FALSE;
        }   /* end-of-if */
    }

    return TRUE;
}   /* test_BR_LLSC_1_5_006( ) */

/*==========================================================================*
**
**  test_BR_LLSC_1_5_007
**
**  Description     :
**      Unit-test EXECUTED conditions, ProductInUse still valid,
**      DAILY product sold.
**
**  Parameters      :
**      pData           [I/O]   BR context data
**
**  Returns         :
**      TRUE                    test passed
**      FALSE                   test failed
**
**  Notes           :
**
**
**==========================================================================*/

int test_BR_LLSC_1_5_007( MYKI_BR_ContextData_t *pData )
{
    MYKI_TACapping_t       *pMYKI_TACapping         = NULL;
    MYKI_TAControl_t       *pMYKI_TAControl         = NULL;
//  MYKI_Directory_t       *pMYKI_Directory         = NULL;
    MYKI_TAProduct_t       *pMYKI_TAProduct         = NULL;
    MYKI_CAControl_t       *pCAControl              = NULL;
    U8_t                    nDirIndex               = 2;
//  int                     nProductId_SingleTrip   = 3;
    int                     nProductId_NHours       = 4;
//  int                     nProductId_Daily        = 5;
//  int                     nProductId_Entitlement  = 100;
    int                     nProductSerial          = 99;

    if ( pData == NULL )
    {
        return FALSE;
    }   /* end-of-if */

    if ( MYKI_CS_TACappingGet( &pMYKI_TACapping ) < 0 || pMYKI_TACapping == NULL )
    {
        return FALSE;
    }   /* end-of-if */

    if ( MYKI_CS_TAControlGet( &pMYKI_TAControl ) < 0 || pMYKI_TAControl == NULL )
    {
        return FALSE;
    }   /* end-of-if */

    if ( MYKI_CS_TAProductGet( (U8_t)( nDirIndex - 1 ), &pMYKI_TAProduct ) < 0 || pMYKI_TAProduct == NULL )
    {
        return FALSE;
    }   /* end-of-if */

    if ( MYKI_CS_CAControlGet(&pCAControl) != MYKI_CS_OK )
    {
        return FALSE;
    }

    pData->DynamicData.previousDailyCapZoneLow  = 1;
    pData->DynamicData.previousDailyCapZoneHigh = 3;
    pMYKI_TACapping->Daily.Zone.Low             = 1;
    pMYKI_TACapping->Daily.Zone.High            = 3;

    /*  Pre-Conditions:
            TAppCapping.Daily > Tariff.DailyCapValue
            Tariff.EntitlementProduct = 0 (ie. does not exist)
            TAppControl.ProductInUse = 0
            Dynamic.IsForcedScanOff = 0

        Post-Conditions:
            ProductInUse upgraded to DAILY product.
            TAppTProduct.PurchaseValue = 1108
            TAppTProduct.ZoneLow = 1
            TAppTProduct.ZoneHigh = 3
    */
    {
        pMYKI_TAControl->NextProductSerialNo                = 1;
        pMYKI_TAControl->Status                             = TAPP_CONTROL_STATUS_ACTIVATED;
        pCAControl->Status                                  = CARD_CONTROL_STATUS_ACTIVATED;
        pData->DynamicData.isForcedScanOff                  = FALSE;
/*      pData->Tariff.dailyCapValue                         = 1108; */
        pData->Tariff.entitlementProduct                    = 0;
        pData->Tariff.endOfBusinessDay                      = ( 1 /*AM*/ * 60 );
        pMYKI_TACapping->Daily.Value                        = 1200;
        pData->DynamicData.currentTripZoneLow               = pData->DynamicData.previousDailyCapZoneLow;
        pData->DynamicData.currentTripZoneHigh              = pData->DynamicData.previousDailyCapZoneHigh;
        pMYKI_TAControl->ProductInUse                       = nDirIndex;
        pMYKI_TAControl->Directory[ nDirIndex ].Status      = TAPP_CONTROL_DIRECTORY_STATUS_ACTIVATED;
        pMYKI_TAControl->Directory[ nDirIndex ].ProductId   = nProductId_NHours;
        pMYKI_TAControl->Directory[ nDirIndex ].SerialNo    = nProductSerial;
        pMYKI_TAProduct->ZoneLow                            = pData->DynamicData.currentTripZoneLow;
        pMYKI_TAProduct->ZoneHigh                           = pData->DynamicData.currentTripZoneHigh;
        pMYKI_TAProduct->ControlBitmap                     &= ~( TAPP_TPRODUCT_CONTROL_BORDER_STATUS_BITMAP );
        pMYKI_TAProduct->EndDateTime                        = pData->DynamicData.currentDateTime + ( 24 * 60 * 60 );

        if ( BR_LLSC_1_5( pData ) != RULE_RESULT_EXECUTED )
        {
            return FALSE;
        }   /* end-of-if */
    }

    return TRUE;
}   /* test_BR_LLSC_1_5_007( ) */
