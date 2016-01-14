/* -*- mode: c; tabs: 4; -*- */
/*=============================================================================
**
**    Vix Technology                   Licensed software
**    (C) 2013                         All rights reserved
**
**=============================================================================
**
**  Project/Product : MBU
**  Filename        : test_BR_LLSC_1_6.c
**  Author(s)       : An Tran
**
**  Description     :
**      Implements BR_LLSC_1_6 business rule unit-tests.
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
**    1.00  20.09.13    ANT   Create
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
**  test_BR_LLSC_1_6_001
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

int test_BR_LLSC_1_6_001( MYKI_BR_ContextData_t *pData )
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

    pMYKI_TACapping->Daily.Zone.Low     = 2;
    pMYKI_TACapping->Daily.Zone.High    = 2;

    /*  Pre-Conditions:
            TAppCapping.Daily.Value < Tariff.DailyCapValue
            Tariff.EntitlementProduct = 0 (ie. does not exist)

        Post-Conditions:
            BYPASSED
    */
    {
/*      pData->Tariff.dailyCapValue                         = 1108; */
        pData->Tariff.entitlementProduct                    = 0;
        pMYKI_TACapping->Daily.Value                        = 350;
        if ( BR_LLSC_1_6( pData ) != RULE_RESULT_BYPASSED )
        {
            return FALSE;
        }   /* end-of-if */
    }

    /*  Pre-Conditions:
            TAppCapping.Daily.Value < Tariff.DailyCapValue
            Tariff.EntitlementProduct = 100 (exists)
            TAppCapping.Daily.Value < Tariff.EntitlementValue

        Post-Conditions:
            BYPASSED
    */
    #if     __TODO__
    {
/*      pData->Tariff.dailyCapValue                         = 1108; */
        pData->Tariff.entitlementProduct                    = nProductId_Entitlement;
        pData->Tariff.entitlementValue                      = 680;
        pMYKI_TACapping->Daily.Value                        = 350;
        if ( BR_LLSC_1_6( pData ) != RULE_RESULT_BYPASSED )
        {
            return FALSE;
        }   /* end-of-if */
    }
    #endif  /* __TODO__ */

    /*  Pre-Conditions:
            TAppCapping.Daily > Tariff.DailyCapValue
            Tariff.EntitlementProduct = 0 (ie. does not exist)
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
        if ( BR_LLSC_1_6( pData ) != RULE_RESULT_BYPASSED )
        {
            return FALSE;
        }   /* end-of-if */
    }

    /*  Pre-Conditions:
            TAppCapping.Daily > Tariff.DailyCapValue
            Tariff.EntitlementProduct = 0 (ie. does not exist)
            TAppControl.Directory[1].ProductId = "DAILY"
            TAppTProduct.ZoneLow < TAppCapping.Daily.ZoneLow
            TAppTProduct.ZoneHigh > TAppCapping.Daily.ZoneHigh

        Post-Conditions:
            BYPASSED
    */
    {
/*      pData->Tariff.dailyCapValue                         = 1108; */
        pData->Tariff.entitlementProduct                    = 0;
        pMYKI_TACapping->Daily.Value                        = 1200;
        pMYKI_TAControl->Directory[ nDirIndex ].Status      = TAPP_CONTROL_DIRECTORY_STATUS_ACTIVATED;
        pMYKI_TAControl->Directory[ nDirIndex ].ProductId   = nProductId_Daily;
        pMYKI_TAProduct->ZoneLow                            = pMYKI_TACapping->Daily.Zone.Low - 1;
        pMYKI_TAProduct->ZoneHigh                           = pMYKI_TACapping->Daily.Zone.High + 1;
        if ( BR_LLSC_1_6( pData ) != RULE_RESULT_BYPASSED )
        {
            return FALSE;
        }   /* end-of-if */
    }

    /*  Pre-Conditions:
            TAppCapping.Daily > Tariff.DailyCapValue
            Tariff.EntitlementProduct = 0 (ie. does not exist)
            TAppControl.Directory[1].ProductId = "ENTITLEMENT"
            TAppTProduct.ZoneLow = TAppCapping.Daily.ZoneLow
            TAppTProduct.ZoneHigh = TAppCapping.Daily.ZoneHigh

        Post-Conditions:
            BYPASSED
    */
    #if     __TODO__
    {
/*      pData->Tariff.dailyCapValue                         = 1108; */
        pData->Tariff.entitlementProduct                    = nProductId_Entitlement;
        pData->Tariff.entitlementValue                      = 220;
        pMYKI_TACapping->Daily.Value                        = 350;
        pMYKI_TAControl->Directory[ nDirIndex ].Status      = TAPP_CONTROL_DIRECTORY_STATUS_ACTIVATED;
        pMYKI_TAControl->Directory[ nDirIndex ].ProductId   = nProductId_Entitlement;
        pMYKI_TAProduct->ZoneLow                            = pMYKI_TACapping->Daily.Zone.Low;
        pMYKI_TAProduct->ZoneHigh                           = pMYKI_TACapping->Daily.Zone.High;
        if ( BR_LLSC_1_6( pData ) != RULE_RESULT_BYPASSED )
        {
            return FALSE;
        }   /* end-of-if */
    }
    #endif  /* __TODO__ */

    /*  Pre-Conditions:
            TAppCapping.Daily > Tariff.DailyCapValue
            Tariff.EntitlementProduct = 100
            TAppControl.Directory[1].ProductId = "ENTITLEMENT"
            TAppTProduct.ZoneLow = TAppCapping.Daily.ZoneLow
            TAppTProduct.ZoneHigh = TAppCapping.Daily.ZoneHigh

        Post-Conditions:
            BYPASSED
    */
    #if     __TODO__
    {
        pData->Tariff.dailyCapValue                         = 240;
        pData->Tariff.entitlementProduct                    = nProductId_Entitlement;
        pData->Tariff.entitlementValue                      = 220;
        pMYKI_TACapping->Daily.Value                        = 350;
        pMYKI_TAControl->Directory[ nDirIndex ].Status      = TAPP_CONTROL_DIRECTORY_STATUS_ACTIVATED;
        pMYKI_TAControl->Directory[ nDirIndex ].ProductId   = nProductId_Entitlement;
        pMYKI_TAProduct->ZoneLow                            = pMYKI_TACapping->Daily.Zone.Low - 1;
        pMYKI_TAProduct->ZoneHigh                           = pMYKI_TACapping->Daily.Zone.High + 1;
        if ( BR_LLSC_1_6( pData ) != RULE_RESULT_BYPASSED )
        {
            return FALSE;
        }   /* end-of-if */
    }
    #endif  /* __TODO__ */

    return TRUE;
}   /* test_BR_LLSC_1_6_001( ) */

/*==========================================================================*
**
**  test_BR_LLSC_1_6_002
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

int test_BR_LLSC_1_6_002( MYKI_BR_ContextData_t *pData )
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

    pMYKI_TACapping->Daily.Zone.Low     = 1;
    pMYKI_TACapping->Daily.Zone.High    = 3;

    /*  Pre-Conditions:
            TAppCapping.Daily > Tariff.DailyCapValue
            Tariff.EntitlementProduct = 0 (ie. does not exist)
            TAppControl.ProductInUse = 2
            TAppControl.Directory[1].ProductId = "N-HOURS"
            TAppTProduct.ZoneLow > TAppCapping.Daily.ZoneLow
            TAppTProduct.ZoneHigh < TAppCapping.Daily.ZoneHigh
            TAppTProduct.ControlBitmap.BorderActive = 0
            TAppTProduct.EndDateTime <= business day for common date (Dynamic.CurrentDateTime)
            Dynamic.IsForcedScanOff = 0

        Post-Conditions:
            ProductInUse upgraded to DAILY product.
            TAppTProduct.PurchaseValue = 240
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
        pMYKI_TAControl->ProductInUse                       = nDirIndex;
        pMYKI_TAControl->Directory[ nDirIndex ].Status      = TAPP_CONTROL_DIRECTORY_STATUS_ACTIVATED;
        pMYKI_TAControl->Directory[ nDirIndex ].ProductId   = nProductId_NHours;
        pMYKI_TAControl->Directory[ nDirIndex ].SerialNo    = nProductSerial;
        pMYKI_TAProduct->ZoneLow                            = pMYKI_TACapping->Daily.Zone.Low + 1;
        pMYKI_TAProduct->ZoneHigh                           = pMYKI_TACapping->Daily.Zone.High - 1;
        pMYKI_TAProduct->ControlBitmap                     &= ~( TAPP_TPRODUCT_CONTROL_BORDER_STATUS_BITMAP );
        pMYKI_TAProduct->EndDateTime                        = pData->DynamicData.currentDateTime;

        if ( BR_LLSC_1_6( pData ) != RULE_RESULT_EXECUTED )
        {
            return FALSE;
        }   /* end-of-if */
    }

    return TRUE;
}   /* test_BR_LLSC_1_6_002( ) */

/*==========================================================================*
**
**  test_BR_LLSC_1_6_003
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

int test_BR_LLSC_1_6_003( MYKI_BR_ContextData_t *pData )
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

    pMYKI_TACapping->Daily.Zone.Low     = 1;
    pMYKI_TACapping->Daily.Zone.High    = 3;

    /*  Pre-Conditions:
            TAppCapping.Daily > Tariff.DailyCapValue
            Tariff.EntitlementProduct = 0 (ie. does not exist)
            TAppControl.ProductInUse = 2
            TAppControl.Directory[1].ProductId = "SINGLE TRIP"
            TAppTProduct.ZoneLow > TAppCapping.Daily.ZoneLow
            TAppTProduct.ZoneHigh < TAppCapping.Daily.ZoneHigh
            TAppTProduct.EndDateTime <= business day for common date (Dynamic.CurrentDateTime)
            Dynamic.IsForcedScanOff = 0

        Post-Conditions:
            ProductInUse upgraded to DAILY product.
            TAppTProduct.PurchaseValue = 240
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
        pMYKI_TAControl->ProductInUse                       = nDirIndex;
        pMYKI_TAControl->Directory[ nDirIndex ].Status      = TAPP_CONTROL_DIRECTORY_STATUS_ACTIVATED;
        pMYKI_TAControl->Directory[ nDirIndex ].ProductId   = nProductId_SingleTrip;
        pMYKI_TAControl->Directory[ nDirIndex ].SerialNo    = nProductSerial;
        pMYKI_TAProduct->ZoneLow                            = pMYKI_TACapping->Daily.Zone.Low + 1;
        pMYKI_TAProduct->ZoneHigh                           = pMYKI_TACapping->Daily.Zone.High - 1;
        pMYKI_TAProduct->EndDateTime                        = pData->DynamicData.currentDateTime;

        if ( BR_LLSC_1_6( pData ) != RULE_RESULT_EXECUTED )
        {
            return FALSE;
        }   /* end-of-if */
    }

    return TRUE;
}   /* test_BR_LLSC_1_6_003( ) */

/*==========================================================================*
**
**  test_BR_LLSC_1_6_004
**
**  Description     :
**      Unit-test EXECUTED conditions, ProductInUse N-HOURS upgraded to
**      ENTITLEMENT product.
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

int test_BR_LLSC_1_6_004( MYKI_BR_ContextData_t *pData )
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

    pMYKI_TACapping->Daily.Zone.Low     = 1;
    pMYKI_TACapping->Daily.Zone.High    = 3;

    /*  Pre-Conditions:
            TAppCapping.Daily > Tariff.DailyCapValue
            Tariff.ApplicableFare = 300
            Tariff.EntitlementProduct = 100
            Tariff.EntitlementZoneLow = 2
            Tariff.EntitlementZoneHigh = 3
            TAppControl.ProductInUse = 2
            TAppControl.Directory[1].ProductId = "N-HOURS"
            TAppTProduct.ZoneLow > TAppCapping.Daily.ZoneLow
            TAppTProduct.ZoneHigh < TAppCapping.Daily.ZoneHigh
            TAppTProduct.EndDateTime <= business day for common date (Dynamic.CurrentDateTime)
            Dynamic.IsForcedScanOff = 0

        Post-Conditions:
            ProductInUse upgraded to ENTITLEMENT product.
            TAppTProduct.PurchaseValue = 220
            TAppTProduct.ZoneLow = 1
            TAppTProduct.ZoneHigh = 3
    */
    #if     __TODO__
    {
        pMYKI_TAControl->Status                             = TAPP_CONTROL_STATUS_ACTIVATED;
        pCAControl->Status                                  = CARD_CONTROL_STATUS_ACTIVATED;
        pData->DynamicData.isForcedScanOff                  = FALSE;
/*      pData->Tariff.dailyCapValue                         = 1108; */
        pData->Tariff.baseFare /* ApplicableFare!? */       = 255;
        pData->Tariff.entitlementProduct                    = nProductId_Entitlement;
        pData->Tariff.entitlementValue                      = 220;
        pData->Tariff.entitlementZoneLow                    = 2;
        pData->Tariff.entitlementZoneHigh                   = 3;
        pData->Tariff.endOfBusinessDay                      = ( 1 /*AM*/ * 60 );
        pMYKI_TACapping->Daily.Value                        = 350;
        pMYKI_TAControl->ProductInUse                       = nDirIndex;
        pMYKI_TAControl->Directory[ nDirIndex ].Status      = TAPP_CONTROL_DIRECTORY_STATUS_ACTIVATED;
        pMYKI_TAControl->Directory[ nDirIndex ].ProductId   = nProductId_NHours;
        pMYKI_TAControl->Directory[ nDirIndex ].SerialNo    = nProductSerial;
        pMYKI_TAProduct->ZoneLow                            = pMYKI_TACapping->Daily.Zone.Low + 1;
        pMYKI_TAProduct->ZoneHigh                           = pMYKI_TACapping->Daily.Zone.High - 1;
        pMYKI_TAProduct->EndDateTime                        = pData->DynamicData.currentDateTime;
        pMYKI_TAProduct->ControlBitmap                     &= ~( TAPP_TPRODUCT_CONTROL_BORDER_STATUS_BITMAP );

        if ( BR_LLSC_1_6( pData ) != RULE_RESULT_EXECUTED )
        {
            return FALSE;
        }   /* end-of-if */
    }
    #endif  /* __TODO__ */

    return TRUE;
}   /* test_BR_LLSC_1_6_004( ) */

/*==========================================================================*
**
**  test_BR_LLSC_1_6_005
**
**  Description     :
**      Unit-test EXECUTED conditions, ProductInUse SINGLE TRIP upgraded to
**      ENTITLEMENT product.
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

int test_BR_LLSC_1_6_005( MYKI_BR_ContextData_t *pData )
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

    pMYKI_TACapping->Daily.Zone.Low     = 1;
    pMYKI_TACapping->Daily.Zone.High    = 3;

    /*  Pre-Conditions:
            TAppCapping.Daily > Tariff.DailyCapValue
            Tariff.ApplicableFare = 300
            Tariff.EntitlementProduct = 100
            Tariff.EntitlementZoneLow = 2
            Tariff.EntitlementZoneHigh = 3
            TAppControl.ProductInUse = 2
            TAppControl.Directory[1].ProductId = "SINGLE TRIP"
            TAppTProduct.ZoneLow > TAppCapping.Daily.ZoneLow
            TAppTProduct.ZoneHigh < TAppCapping.Daily.ZoneHigh
            TAppTProduct.EndDateTime <= business day for common date (Dynamic.CurrentDateTime)
            Dynamic.IsForcedScanOff = 0

        Post-Conditions:
            ProductInUse upgraded to ENTITLEMENT product.
            TAppTProduct.PurchaseValue = 220
            TAppTProduct.ZoneLow = 1
            TAppTProduct.ZoneHigh = 3
    */
    #if     __TODO__
    {
        pMYKI_TAControl->Status                             = TAPP_CONTROL_STATUS_ACTIVATED;
        pCAControl->Status                                  = CARD_CONTROL_STATUS_ACTIVATED;
        pData->DynamicData.isForcedScanOff                  = FALSE;
/*      pData->Tariff.dailyCapValue                         = 1108; */
        pData->Tariff.baseFare /* ApplicableFare!? */       = 255;
        pData->Tariff.entitlementProduct                    = nProductId_Entitlement;
        pData->Tariff.entitlementValue                      = 220;
        pData->Tariff.entitlementZoneLow                    = 2;
        pData->Tariff.entitlementZoneHigh                   = 3;
        pData->Tariff.endOfBusinessDay                      = ( 1 /*AM*/ * 60 );
        pMYKI_TACapping->Daily.Value                        = 350;
        pMYKI_TAControl->ProductInUse                       = nDirIndex;
        pMYKI_TAControl->Directory[ nDirIndex ].Status      = TAPP_CONTROL_DIRECTORY_STATUS_ACTIVATED;
        pMYKI_TAControl->Directory[ nDirIndex ].ProductId   = nProductId_SingleTrip;
        pMYKI_TAControl->Directory[ nDirIndex ].SerialNo    = nProductSerial;
        pMYKI_TAProduct->ZoneLow                            = pMYKI_TACapping->Daily.Zone.Low + 1;
        pMYKI_TAProduct->ZoneHigh                           = pMYKI_TACapping->Daily.Zone.High - 1;
        pMYKI_TAProduct->EndDateTime                        = pData->DynamicData.currentDateTime;

        if ( BR_LLSC_1_6( pData ) != RULE_RESULT_EXECUTED )
        {
            return FALSE;
        }   /* end-of-if */
    }
    #endif  /* __TODO__ */

    return TRUE;
}   /* test_BR_LLSC_1_6_005( ) */

/*==========================================================================*
**
**  test_BR_LLSC_1_6_006
**
**  Description     :
**      Unit-test EXECUTED conditions, no ProductInUse, DAILY product sold.
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

int test_BR_LLSC_1_6_006( MYKI_BR_ContextData_t *pData )
{
    MYKI_TACapping_t       *pMYKI_TACapping         = NULL;
    MYKI_TAControl_t       *pMYKI_TAControl         = NULL;
    MYKI_CAControl_t       *pCAControl              = NULL;
//  MYKI_Directory_t       *pMYKI_Directory         = NULL;
//  MYKI_TAProduct_t       *pMYKI_TAProduct         = NULL;
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

    if ( MYKI_CS_CAControlGet(&pCAControl) != MYKI_CS_OK )
    {
        return FALSE;
    }

    pMYKI_TACapping->Daily.Zone.Low     = 1;
    pMYKI_TACapping->Daily.Zone.High    = 3;

    /*  Pre-Conditions:
            TAppCapping.Daily > Tariff.DailyCapValue
            Tariff.EntitlementProduct = 0
            TAppControl.ProductInUse = 0
            Dynamic.IsForcedScanOff = 0

        Post-Conditions:
            DAILY product sold.
            TAppTProduct.PurchaseValue = 240
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
        pMYKI_TAControl->ProductInUse                       = 0;

        if ( BR_LLSC_1_6( pData ) != RULE_RESULT_EXECUTED )
        {
            return FALSE;
        }   /* end-of-if */
    }

    return TRUE;
}   /* test_BR_LLSC_1_6_006( ) */

/*==========================================================================*
**
**  test_BR_LLSC_1_6_007
**
**  Description     :
**      Unit-test EXECUTED conditions, ProductInUse N-HOURS is crossed
**      border product, DAILY produt sold.
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

int test_BR_LLSC_1_6_007( MYKI_BR_ContextData_t *pData )
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

    pMYKI_TACapping->Daily.Zone.Low     = 1;
    pMYKI_TACapping->Daily.Zone.High    = 3;

    /*  Pre-Conditions:
            TAppCapping.Daily > Tariff.DailyCapValue
            Tariff.EntitlementProduct = 0
            TAppControl.ProductInUse = 2
            TAppControl.Directory[1].ProductId = "N-HOURS"
            TAppTProduct.ZoneLow > TAppCapping.Daily.ZoneLow
            TAppTProduct.ZoneHigh < TAppCapping.Daily.ZoneHigh
            TAppTProduct.EndDateTime <= business day for common date (Dynamic.CurrentDateTime)
            TAppTProduct.ControlBitmap with border status set
            Dynamic.IsForcedScanOff = 0

        Post-Conditions:
            DAILY product sold.
            TAppTProduct.PurchaseValue = 220
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
        pMYKI_TAControl->ProductInUse                       = nDirIndex;
        pMYKI_TAControl->Directory[ nDirIndex ].Status      = TAPP_CONTROL_DIRECTORY_STATUS_ACTIVATED;
        pMYKI_TAControl->Directory[ nDirIndex ].ProductId   = nProductId_NHours;
        pMYKI_TAControl->Directory[ nDirIndex ].SerialNo    = nProductSerial;
        pMYKI_TAProduct->ZoneLow                            = pMYKI_TACapping->Daily.Zone.Low + 1;
        pMYKI_TAProduct->ZoneHigh                           = pMYKI_TACapping->Daily.Zone.High - 1;
        pMYKI_TAProduct->EndDateTime                        = pData->DynamicData.currentDateTime;
        pMYKI_TAProduct->ControlBitmap                     |= ( TAPP_TPRODUCT_CONTROL_BORDER_STATUS_BITMAP );

        if ( BR_LLSC_1_6( pData ) != RULE_RESULT_EXECUTED )
        {
            return FALSE;
        }   /* end-of-if */
    }

    return TRUE;
}   /* test_BR_LLSC_1_6_007( ) */

/*==========================================================================*
**
**  test_BR_LLSC_1_6_008
**
**  Description     :
**      Unit-test EXECUTED conditions, ProductInUse N-HOURS is still
**      valid, DAILY produt sold.
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

int test_BR_LLSC_1_6_008( MYKI_BR_ContextData_t *pData )
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

    pMYKI_TACapping->Daily.Zone.Low     = 1;
    pMYKI_TACapping->Daily.Zone.High    = 3;

    /*  Pre-Conditions:
            TAppCapping.Daily > Tariff.DailyCapValue
            Tariff.EntitlementProduct = 0
            TAppControl.ProductInUse = 2
            TAppControl.Directory[1].ProductId = "N-HOURS"
            TAppTProduct.ZoneLow > TAppCapping.Daily.ZoneLow
            TAppTProduct.ZoneHigh < TAppCapping.Daily.ZoneHigh
            TAppTProduct.EndDateTime > business day for common date (Dynamic.CurrentDateTime)
            Dynamic.IsForcedScanOff = 0

        Post-Conditions:
            DAILY product sold
            TAppTProduct.PurchaseValue = 220
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
        pMYKI_TAControl->ProductInUse                       = nDirIndex;
        pMYKI_TAControl->Directory[ nDirIndex ].Status      = TAPP_CONTROL_DIRECTORY_STATUS_ACTIVATED;
        pMYKI_TAControl->Directory[ nDirIndex ].ProductId   = nProductId_NHours;
        pMYKI_TAControl->Directory[ nDirIndex ].SerialNo    = nProductSerial;
        pMYKI_TAProduct->ZoneLow                            = pMYKI_TACapping->Daily.Zone.Low + 1;
        pMYKI_TAProduct->ZoneHigh                           = pMYKI_TACapping->Daily.Zone.High - 1;
        pMYKI_TAProduct->EndDateTime                        = pData->DynamicData.currentDateTime + ( 24 * 60 * 60 );
        pMYKI_TAProduct->ControlBitmap                     &= ~( TAPP_TPRODUCT_CONTROL_BORDER_STATUS_BITMAP );

        if ( BR_LLSC_1_6( pData ) != RULE_RESULT_EXECUTED )
        {
            return FALSE;
        }   /* end-of-if */
    }

    return TRUE;
}   /* test_BR_LLSC_1_6_008( ) */

/*==========================================================================*
**
**  test_BR_LLSC_1_6_009
**
**  Description     :
**      Unit-test EXECUTED conditions, no ProductInUse,
**      ENTITLEMENT product sold.
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

int test_BR_LLSC_1_6_009( MYKI_BR_ContextData_t *pData )
{
    MYKI_TACapping_t       *pMYKI_TACapping         = NULL;
    MYKI_TAControl_t       *pMYKI_TAControl         = NULL;
//  MYKI_Directory_t       *pMYKI_Directory         = NULL;
//  MYKI_TAProduct_t       *pMYKI_TAProduct         = NULL;
    MYKI_CAControl_t       *pCAControl              = NULL;
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

    if ( MYKI_CS_CAControlGet(&pCAControl) != MYKI_CS_OK )
    {
        return FALSE;
    }

    pMYKI_TACapping->Daily.Zone.Low     = 1;
    pMYKI_TACapping->Daily.Zone.High    = 3;

    /*  Pre-Conditions:
            TAppCapping.Daily > Tariff.DailyCapValue
            Tariff.EntitlementProduct = 0
            TAppControl.ProductInUse = 0
            Dynamic.IsForcedScanOff = 0

        Post-Conditions:
            DAILY product sold.
            TAppTProduct.PurchaseValue = 240
            TAppTProduct.ZoneLow = 1
            TAppTProduct.ZoneHigh = 3
    */
    #if     __TODO__
    {
        pMYKI_TAControl->Status                             = TAPP_CONTROL_STATUS_ACTIVATED;
        pCAControl->Status                                  = CARD_CONTROL_STATUS_ACTIVATED;
        pData->DynamicData.isForcedScanOff                  = FALSE;
        pData->Tariff.dailyCapValue                         = 240;
        pData->Tariff.baseFare /* ApplicableFare!? */       = 255;
        pData->Tariff.entitlementProduct                    = nProductId_Entitlement;
        pData->Tariff.entitlementValue                      = 220;
        pData->Tariff.entitlementZoneLow                    = 2;
        pData->Tariff.entitlementZoneHigh                   = 3;
        pData->Tariff.endOfBusinessDay                      = ( 1 /*AM*/ * 60 );
        pMYKI_TACapping->Daily.Value                        = 350;
        pMYKI_TAControl->ProductInUse                       = 0;

        if ( BR_LLSC_1_6( pData ) != RULE_RESULT_EXECUTED )
        {
            return FALSE;
        }   /* end-of-if */
    }
    #endif  /* __TODO__ */

    return TRUE;
}   /* test_BR_LLSC_1_6_009( ) */
