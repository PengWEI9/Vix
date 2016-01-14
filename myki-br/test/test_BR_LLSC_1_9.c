/* -*- mode: c; tabs: 4; -*- */
/*=============================================================================
**
**    Vix Technology                   Licensed software
**    (C) 2013                         All rights reserved
**
**=============================================================================
**
**  Project/Product : MBU
**  Filename        : test_BR_LLSC_1_9.c
**  Author(s)       : An Tran
**
**  Description     :
**      Implements BR_LLSC_1_9 business rule unit-tests.
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
**    1.00  11.10.13    ANT   Create
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
**  test_BR_LLSC_1_9_001
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

int test_BR_LLSC_1_9_001( MYKI_BR_ContextData_t *pData )
{
    MYKI_TACapping_t       *pMYKI_TACapping         = NULL;
    MYKI_TAControl_t       *pMYKI_TAControl         = NULL;
//  MYKI_Directory_t       *pMYKI_Directory         = NULL;
    MYKI_TAProduct_t       *pMYKI_TAProduct         = NULL;
    U8_t                    nDirIndex               = 2;
//  int                     nProductId_Daily        = 5;
    int                     nProductId_Weekly       = 6;
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

    pMYKI_TACapping->Weekly.Zone[ 0 ].Low   = 2;
    pMYKI_TACapping->Weekly.Zone[ 0 ].High  = 2;

    /*  Pre-Conditions:
            Dynamic.ForcedScanOff = false
            Dynamic.CurrentDateTime = weekday

        Post-Conditions:
            BYPASSED
    */
    {
        pData->DynamicData.currentDateTime                  = 0x525739E5;   /* Friday 11/10/2013 10:36:05 */
        pData->DynamicData.isForcedScanOff                  = FALSE;
        if ( BR_LLSC_1_9( pData ) != RULE_RESULT_BYPASSED )
        {
            return FALSE;
        }   /* end-of-if */
    }

    /*  Pre-Conditions:
            Dynamic.ForcedScanOff = false
            Dynamic.CurrentDateTime = weekend
            TACapping.Weekly.Value < Tariff.WeekendCapValue

        Post-Conditions:
            BYPASSED
    */
    {
        pData->DynamicData.currentDateTime                  = 0x524F80B0;   /* Saturday 05/10/2013 13:00:00 */
/*      pData->Tariff.weekendCapValue                       = ( 1108 * 2 ); */
        pMYKI_TACapping->Weekly.Value                       = ( 1108 * 1 );
        if ( BR_LLSC_1_9( pData ) != RULE_RESULT_BYPASSED )
        {
            return FALSE;
        }   /* end-of-if */
    }

    /*  Pre-Conditions:
            Dynamic.ForcedScanOff = false
            Dynamic.CurrentDateTime = weekend
            TACapping.Weekly.Value >= Tariff.WeekendCapValue
            TAppControl.ProductInUse = 2
            TAppControl.Directory[1].ProductId = "WEEKLY"
            TAppTProduct.ZoneLow = TAppCapping.Daily.ZoneLow
            TAppTProduct.ZoneHigh = TAppCapping.Daily.ZoneHigh

        Post-Conditions:
            BYPASSED
    */
    {
        pData->DynamicData.currentDateTime                  = 0x524F80B0;   /* Saturday 05/10/2013 13:00:00 */
/*      pData->Tariff.weekendCapValue                       = ( 1108 * 2 ); */
        pMYKI_TACapping->Weekly.Value                       = ( 1108 * 3 );
        pMYKI_TAControl->ProductInUse                       = nDirIndex;
        pMYKI_TAControl->Directory[ nDirIndex ].Status      = TAPP_CONTROL_DIRECTORY_STATUS_ACTIVATED;
        pMYKI_TAControl->Directory[ nDirIndex ].ProductId   = nProductId_Weekly;
        pMYKI_TAControl->Directory[ nDirIndex ].SerialNo    = nProductSerial;
        pMYKI_TAProduct->ZoneLow                            = pMYKI_TACapping->Weekly.Zone[ 0 ].Low;
        pMYKI_TAProduct->ZoneHigh                           = pMYKI_TACapping->Weekly.Zone[ 0 ].High;
        if ( BR_LLSC_1_9( pData ) != RULE_RESULT_BYPASSED )
        {
            return FALSE;
        }   /* end-of-if */
    }

    return TRUE;
}   /* test_BR_LLSC_1_9_001( ) */

/*==========================================================================*
**
**  test_BR_LLSC_1_9_002
**
**  Description     :
**      Unit-test EXECUTED conditions, ProductInUse N-HOURS upgraded to
**      WEEKLY product.
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

int test_BR_LLSC_1_9_002( MYKI_BR_ContextData_t *pData )
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

    pMYKI_TACapping->Weekly.Zone[ 0 ].Low   = 1;
    pMYKI_TACapping->Weekly.Zone[ 0 ].High  = 3;

    /*  Pre-Conditions:
            Dynamic.ForcedScanOff = false
            Dynamic.CurrentDateTime = weekend
            TACapping.Weekly.Value >= Tariff.WeekendCapValue
            TAppControl.ProductInUse = 2
            TAppControl.Directory[1].ProductId = "N-HOUR"
            TAppTProduct.ZoneLow = TAppCapping.Weekly.ZoneLow
            TAppTProduct.ZoneHigh = TAppCapping.Weekly.ZoneHigh

        Post-Conditions:
            ProductInUse upgraded to WEEKLY product.
            TAppTProduct.PurchaseValue = ( 1108 * 2 )
            TAppTProduct.ZoneLow = 1
            TAppTProduct.ZoneHigh = 3
    */
    {
        pMYKI_TAControl->Status                             = TAPP_CONTROL_STATUS_ACTIVATED;
        pCAControl->Status                                  = CARD_CONTROL_STATUS_ACTIVATED;
        pData->DynamicData.currentDateTime                  = 0x524F80B0;   /* Saturday 05/10/2013 13:00:00 */
        pData->DynamicData.isForcedScanOff                  = FALSE;
/*      pData->Tariff.weekendCapValue                       = ( 1108 * 2 ); */
        pData->Tariff.entitlementProduct                    = 0;
        pData->Tariff.endOfBusinessDay                      = ( 1 /*AM*/ * 60 );
        pMYKI_TACapping->Weekly.Value                       = ( 1108 * 3 );
        pMYKI_TAControl->ProductInUse                       = nDirIndex;
        pMYKI_TAControl->Directory[ nDirIndex ].Status      = TAPP_CONTROL_DIRECTORY_STATUS_ACTIVATED;
        pMYKI_TAControl->Directory[ nDirIndex ].ProductId   = nProductId_NHours;
        pMYKI_TAControl->Directory[ nDirIndex ].SerialNo    = nProductSerial;
        pMYKI_TAProduct->ZoneLow                            = pMYKI_TACapping->Weekly.Zone[ 0 ].Low;
        pMYKI_TAProduct->ZoneHigh                           = pMYKI_TACapping->Weekly.Zone[ 0 ].High;
        pMYKI_TAProduct->ControlBitmap                     &= ~( TAPP_TPRODUCT_CONTROL_BORDER_STATUS_BITMAP );
        pMYKI_TAProduct->EndDateTime                        = pData->DynamicData.currentDateTime;

        if ( BR_LLSC_1_9( pData ) != RULE_RESULT_EXECUTED )
        {
            return FALSE;
        }   /* end-of-if */
    }

    return TRUE;
}   /* test_BR_LLSC_1_9_002( ) */

/*==========================================================================*
**
**  test_BR_LLSC_1_9_003
**
**  Description     :
**      Unit-test EXECUTED conditions, ProductInUse DAILY upgraded to
**      WEEKLY product.
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

int test_BR_LLSC_1_9_003( MYKI_BR_ContextData_t *pData )
{
    MYKI_TACapping_t       *pMYKI_TACapping         = NULL;
    MYKI_TAControl_t       *pMYKI_TAControl         = NULL;
//  MYKI_Directory_t       *pMYKI_Directory         = NULL;
    MYKI_TAProduct_t       *pMYKI_TAProduct         = NULL;
    MYKI_CAControl_t       *pCAControl              = NULL;
    U8_t                    nDirIndex               = 2;
//  int                     nProductId_SingleTrip   = 3;
//  int                     nProductId_NHours       = 4;
    int                     nProductId_Daily        = 5;
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

    pMYKI_TACapping->Weekly.Zone[ 0 ].Low   = 1;
    pMYKI_TACapping->Weekly.Zone[ 0 ].High  = 3;

    /*  Pre-Conditions:
            Dynamic.ForcedScanOff = false
            Dynamic.CurrentDateTime = weekend
            TACapping.Weekly.Value >= Tariff.WeekendCapValue
            TAppControl.ProductInUse = 2
            TAppControl.Directory[1].ProductId = "DAILY"
            TAppTProduct.ZoneLow = TAppCapping.Weekly.ZoneLow
            TAppTProduct.ZoneHigh = TAppCapping.Weekly.ZoneHigh

        Post-Conditions:
            ProductInUse upgraded to WEEKLY product.
            TAppTProduct.PurchaseValue = ( 1108 * 2 )
            TAppTProduct.ZoneLow = 1
            TAppTProduct.ZoneHigh = 3
    */
    {
        pMYKI_TAControl->Status                             = TAPP_CONTROL_STATUS_ACTIVATED;
        pCAControl->Status                                  = CARD_CONTROL_STATUS_ACTIVATED;
        pData->DynamicData.currentDateTime                  = 0x524F80B0;   /* Saturday 05/10/2013 13:00:00 */
        pData->DynamicData.isForcedScanOff                  = FALSE;
/*      pData->Tariff.weekendCapValue                       = ( 1108 * 2 ); */
        pData->Tariff.entitlementProduct                    = 0;
        pData->Tariff.endOfBusinessDay                      = ( 1 /*AM*/ * 60 );
        pMYKI_TACapping->Weekly.Value                       = ( 1108 * 3 );
        pMYKI_TAControl->ProductInUse                       = nDirIndex;
        pMYKI_TAControl->Directory[ nDirIndex ].Status      = TAPP_CONTROL_DIRECTORY_STATUS_ACTIVATED;
        pMYKI_TAControl->Directory[ nDirIndex ].ProductId   = nProductId_Daily;
        pMYKI_TAControl->Directory[ nDirIndex ].SerialNo    = nProductSerial;
        pMYKI_TAProduct->ZoneLow                            = pMYKI_TACapping->Weekly.Zone[ 0 ].Low;
        pMYKI_TAProduct->ZoneHigh                           = pMYKI_TACapping->Weekly.Zone[ 0 ].High;
        pMYKI_TAProduct->ControlBitmap                     &= ~( TAPP_TPRODUCT_CONTROL_BORDER_STATUS_BITMAP );
        pMYKI_TAProduct->EndDateTime                        = pData->DynamicData.currentDateTime;

        if ( BR_LLSC_1_9( pData ) != RULE_RESULT_EXECUTED )
        {
            return FALSE;
        }   /* end-of-if */
    }

    return TRUE;
}   /* test_BR_LLSC_1_9_003( ) */

/*==========================================================================*
**
**  test_BR_LLSC_1_9_004
**
**  Description     :
**      Unit-test EXECUTED conditions, ProductInUse SINGLE TRIP upgraded to
**      WEEKLY product.
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

int test_BR_LLSC_1_9_004( MYKI_BR_ContextData_t *pData )
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

    pData->DynamicData.currentTripZoneLow   = 1;
    pData->DynamicData.currentTripZoneHigh  = 3;
    pMYKI_TACapping->Weekly.Zone[ 0 ].Low   = 2;
    pMYKI_TACapping->Weekly.Zone[ 0 ].High  = 2;

    /*  Pre-Conditions:
            Dynamic.ForcedScanOff = false
            Dynamic.CurrentDateTime = weekend
            TACapping.Weekly.Value >= Tariff.WeekendCapValue
            TAppControl.ProductInUse = 2
            TAppControl.Directory[1].ProductId = "SINGLE TRIP"
            TAppTProduct.ZoneLow = TAppCapping.Weekly.ZoneLow
            TAppTProduct.ZoneHigh = TAppCapping.Weekly.ZoneHigh

        Post-Conditions:
            ProductInUse upgraded to WEEKLY product.
            TAppTProduct.PurchaseValue = ( 1108 * 5 )
            TAppTProduct.ZoneLow = 1
            TAppTProduct.ZoneHigh = 3
    */
    {
        pMYKI_TAControl->Status                             = TAPP_CONTROL_STATUS_ACTIVATED;
        pCAControl->Status                                  = CARD_CONTROL_STATUS_ACTIVATED;
        pData->DynamicData.currentDateTime                  = 0x524F80B0;   /* Saturday 05/10/2013 13:00:00 */
        pData->DynamicData.isForcedScanOff                  = FALSE;
/*      pData->Tariff.weekendCapValue                       = ( 1108 * 2 ); */
        pData->Tariff.entitlementProduct                    = 0;
        pData->Tariff.endOfBusinessDay                      = ( 1 /*AM*/ * 60 );
        pMYKI_TACapping->Weekly.Value                       = ( 1108 * 3 );
        pMYKI_TAControl->ProductInUse                       = nDirIndex;
        pMYKI_TAControl->Directory[ nDirIndex ].Status      = TAPP_CONTROL_DIRECTORY_STATUS_ACTIVATED;
        pMYKI_TAControl->Directory[ nDirIndex ].ProductId   = nProductId_SingleTrip;
        pMYKI_TAControl->Directory[ nDirIndex ].SerialNo    = nProductSerial;
        pMYKI_TAProduct->ZoneLow                            = pData->DynamicData.currentTripZoneLow;
        pMYKI_TAProduct->ZoneHigh                           = pData->DynamicData.currentTripZoneHigh;
        pMYKI_TAProduct->ControlBitmap                     &= ~( TAPP_TPRODUCT_CONTROL_BORDER_STATUS_BITMAP );
        pMYKI_TAProduct->EndDateTime                        = pData->DynamicData.currentDateTime;

        if ( BR_LLSC_1_9( pData ) != RULE_RESULT_EXECUTED )
        {
            return FALSE;
        }   /* end-of-if */
    }

    return TRUE;
}   /* test_BR_LLSC_1_9_004( ) */

/*==========================================================================*
**
**  test_BR_LLSC_1_9_005
**
**  Description     :
**      Unit-test EXECUTED conditions, no ProductInUse
**      WEEKLY product sold.
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

int test_BR_LLSC_1_9_005( MYKI_BR_ContextData_t *pData )
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

    pMYKI_TACapping->Weekly.Zone[ 0 ].Low   = 1;
    pMYKI_TACapping->Weekly.Zone[ 0 ].High  = 3;

    /*  Pre-Conditions:
            Dynamic.ForcedScanOff = false
            Dynamic.CurrentDateTime = weekend
            TACapping.Weekly.Value >= Tariff.WeekendCapValue
            TAppControl.ProductInUse = 0

        Post-Conditions:
            WEEKLY product sold.
            TAppTProduct.PurchaseValue = ( 1108 * 3 )
            TAppTProduct.ZoneLow = 1
            TAppTProduct.ZoneHigh = 3
    */
    {
        pMYKI_TAControl->NextProductSerialNo                = 1;
        pMYKI_TAControl->Status                             = TAPP_CONTROL_STATUS_ACTIVATED;
        pCAControl->Status                                  = CARD_CONTROL_STATUS_ACTIVATED;
        pData->DynamicData.currentDateTime                  = 0x524F80B0;   /* Saturday 05/10/2013 13:00:00 */
        pData->DynamicData.isForcedScanOff                  = FALSE;
/*      pData->Tariff.weekendCapValue                       = ( 1108 * 2 ); */
        pData->Tariff.entitlementProduct                    = 0;
        pData->Tariff.endOfBusinessDay                      = ( 1 /*AM*/ * 60 );
        pMYKI_TACapping->Weekly.Value                       = ( 1108 * 3 );
        pMYKI_TAControl->ProductInUse                       = 0;

        if ( BR_LLSC_1_9( pData ) != RULE_RESULT_EXECUTED )
        {
            return FALSE;
        }   /* end-of-if */
    }

    return TRUE;
}   /* test_BR_LLSC_1_9_005( ) */

/*==========================================================================*
**
**  test_BR_LLSC_1_9_006
**
**  Description     :
**      Unit-test EXECUTED conditions, ProductInUse N-HOURS not same zone(s)
**      WEEKLY product sold.
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

int test_BR_LLSC_1_9_006( MYKI_BR_ContextData_t *pData )
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

    pMYKI_TACapping->Weekly.Zone[ 0 ].Low   = 1;
    pMYKI_TACapping->Weekly.Zone[ 0 ].High  = 3;

    /*  Pre-Conditions:
            Dynamic.ForcedScanOff = false
            Dynamic.CurrentDateTime = weekday
            TACapping.Weekly.Value >= Tariff.WeekendCapValue
            TAppControl.ProductInUse = 2
            TAppControl.Directory[1].ProductId = "DAILY"
            TAppTProduct.ZoneLow = TAppCapping.Weekly.ZoneLow
            TAppTProduct.ZoneHigh != TAppCapping.Weekly.ZoneHigh

        Post-Conditions:
            WEEKLY product sold.
            TAppTProduct.PurchaseValue = ( 1108 * 2 )
            TAppTProduct.ZoneLow = 1
            TAppTProduct.ZoneHigh = 3
    */
    {
        pMYKI_TAControl->NextProductSerialNo                = 1;
        pMYKI_TAControl->Status                             = TAPP_CONTROL_STATUS_ACTIVATED;
        pCAControl->Status                                  = CARD_CONTROL_STATUS_ACTIVATED;
        pData->DynamicData.currentDateTime                  = 0x524F80B0;   /* Saturday 05/10/2013 13:00:00 */
        pData->DynamicData.isForcedScanOff                  = FALSE;
/*      pData->Tariff.weekendCapValue                       = ( 1108 * 2 ); */
        pData->Tariff.entitlementProduct                    = 0;
        pData->Tariff.endOfBusinessDay                      = ( 1 /*AM*/ * 60 );
        pMYKI_TACapping->Weekly.Value                       = ( 1108 * 3 );
        pMYKI_TAControl->ProductInUse                       = nDirIndex;
        pMYKI_TAControl->Directory[ nDirIndex ].Status      = TAPP_CONTROL_DIRECTORY_STATUS_ACTIVATED;
        pMYKI_TAControl->Directory[ nDirIndex ].ProductId   = nProductId_NHours;
        pMYKI_TAControl->Directory[ nDirIndex ].SerialNo    = nProductSerial;
        pMYKI_TAProduct->ZoneLow                            = pMYKI_TACapping->Weekly.Zone[ 0 ].Low;
        pMYKI_TAProduct->ZoneHigh                           = pMYKI_TACapping->Weekly.Zone[ 0 ].High - 1;
        pMYKI_TAProduct->ControlBitmap                     &= ~( TAPP_TPRODUCT_CONTROL_BORDER_STATUS_BITMAP );
        pMYKI_TAProduct->EndDateTime                        = pData->DynamicData.currentDateTime;

        if ( BR_LLSC_1_9( pData ) != RULE_RESULT_EXECUTED )
        {
            return FALSE;
        }   /* end-of-if */
    }

    return TRUE;
}   /* test_BR_LLSC_1_9_006( ) */

/*==========================================================================*
**
**  test_BR_LLSC_1_9_007
**
**  Description     :
**      Unit-test EXECUTED conditions, ProductInUse SINGLE TRIP not within
**      zone(s), WEEKLY product sold.
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

int test_BR_LLSC_1_9_007( MYKI_BR_ContextData_t *pData )
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

    pData->DynamicData.currentTripZoneLow   = 1;
    pData->DynamicData.currentTripZoneHigh  = 3;
    pMYKI_TACapping->Weekly.Zone[ 0 ].Low   = 2;
    pMYKI_TACapping->Weekly.Zone[ 0 ].High  = 2;

    /*  Pre-Conditions:
            Dynamic.ForcedScanOff = false
            Dynamic.CurrentDateTime = weekday
            TACapping.Weekly.Value >= Tariff.WeekendCapValue
            TAppControl.ProductInUse = 2
            TAppControl.Directory[1].ProductId = "SINGLE TRIP"
            TAppTProduct.ZoneLow = Dynamic.CurrentTripZoneLow
            TAppTProduct.ZoneHigh > Dynamic.CurrentTripZoneHigh

        Post-Conditions:
            WEEKLY product sold.
            TAppTProduct.PurchaseValue = ( 1108 * 2 )
            TAppTProduct.ZoneLow = 1
            TAppTProduct.ZoneHigh = 3
    */
    {
        pMYKI_TAControl->NextProductSerialNo                = 1;
        pMYKI_TAControl->Status                             = TAPP_CONTROL_STATUS_ACTIVATED;
        pCAControl->Status                                  = CARD_CONTROL_STATUS_ACTIVATED;
        pData->DynamicData.currentDateTime                  = 0x524F80B0;   /* Saturday 05/10/2013 13:00:00 */
        pData->DynamicData.isForcedScanOff                  = FALSE;
/*      pData->Tariff.weekendCapValue                       = ( 1108 * 2 ); */
        pData->Tariff.entitlementProduct                    = 0;
        pData->Tariff.endOfBusinessDay                      = ( 1 /*AM*/ * 60 );
        pMYKI_TACapping->Weekly.Value                       = ( 1108 * 3 );
        pMYKI_TAControl->ProductInUse                       = nDirIndex;
        pMYKI_TAControl->Directory[ nDirIndex ].Status      = TAPP_CONTROL_DIRECTORY_STATUS_ACTIVATED;
        pMYKI_TAControl->Directory[ nDirIndex ].ProductId   = nProductId_SingleTrip;
        pMYKI_TAControl->Directory[ nDirIndex ].SerialNo    = nProductSerial;
        pMYKI_TAProduct->ZoneLow                            = pData->DynamicData.currentTripZoneLow;
        pMYKI_TAProduct->ZoneHigh                           = pData->DynamicData.currentTripZoneHigh + 1;
        pMYKI_TAProduct->ControlBitmap                     &= ~( TAPP_TPRODUCT_CONTROL_BORDER_STATUS_BITMAP );
        pMYKI_TAProduct->EndDateTime                        = pData->DynamicData.currentDateTime;

        if ( BR_LLSC_1_9( pData ) != RULE_RESULT_EXECUTED )
        {
            return FALSE;
        }   /* end-of-if */
    }

    return TRUE;
}   /* test_BR_LLSC_1_9_007( ) */

/*==========================================================================*
**
**  test_BR_LLSC_1_9_008
**
**  Description     :
**      Unit-test EXECUTED conditions, ProductInUse DAILY valid
**      zone(s), WEEKLY product sold.
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

int test_BR_LLSC_1_9_008( MYKI_BR_ContextData_t *pData )
{
    MYKI_TACapping_t       *pMYKI_TACapping         = NULL;
    MYKI_TAControl_t       *pMYKI_TAControl         = NULL;
//  MYKI_Directory_t       *pMYKI_Directory         = NULL;
    MYKI_TAProduct_t       *pMYKI_TAProduct         = NULL;
    MYKI_CAControl_t       *pCAControl              = NULL;
    U8_t                    nDirIndex               = 2;
//  int                     nProductId_SingleTrip   = 3;
//  int                     nProductId_NHours       = 4;
    int                     nProductId_Daily        = 5;
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

    pMYKI_TACapping->Weekly.Zone[ 0 ].Low   = 1;
    pMYKI_TACapping->Weekly.Zone[ 0 ].High  = 3;

    /*  Pre-Conditions:
            Dynamic.ForcedScanOff = false
            Dynamic.CurrentDateTime = weekday
            TACapping.Weekly.Value >= Tariff.WeekendCapValue
            TAppControl.ProductInUse = 2
            TAppControl.Directory[1].ProductId = "DAILY"
            TAppTProduct.ZoneLow = TAppCapping.Weekly.ZoneLow
            TAppTProduct.ZoneHigh = TAppCapping.Weekly.ZoneHigh

        Post-Conditions:
            WEEKLY product sold.
            TAppTProduct.PurchaseValue = ( 1108 * 2 )
            TAppTProduct.ZoneLow = 1
            TAppTProduct.ZoneHigh = 3
    */
    {
        pMYKI_TAControl->NextProductSerialNo                = 1;
        pMYKI_TAControl->Status                             = TAPP_CONTROL_STATUS_ACTIVATED;
        pCAControl->Status                                  = CARD_CONTROL_STATUS_ACTIVATED;
        pData->DynamicData.currentDateTime                  = 0x524F80B0;   /* Saturday 05/10/2013 13:00:00 */
        pData->DynamicData.isForcedScanOff                  = FALSE;
/*      pData->Tariff.weekendCapValue                       = ( 1108 * 2 ); */
        pData->Tariff.entitlementProduct                    = 0;
        pData->Tariff.endOfBusinessDay                      = ( 1 /*AM*/ * 60 );
        pMYKI_TACapping->Weekly.Value                       = ( 1108 * 3 );
        pMYKI_TAControl->ProductInUse                       = nDirIndex;
        pMYKI_TAControl->Directory[ nDirIndex ].Status      = TAPP_CONTROL_DIRECTORY_STATUS_ACTIVATED;
        pMYKI_TAControl->Directory[ nDirIndex ].ProductId   = nProductId_Daily;
        pMYKI_TAControl->Directory[ nDirIndex ].SerialNo    = nProductSerial;
        pMYKI_TAProduct->ZoneLow                            = pMYKI_TACapping->Weekly.Zone[ 0 ].Low;
        pMYKI_TAProduct->ZoneHigh                           = pMYKI_TACapping->Weekly.Zone[ 0 ].High;
        pMYKI_TAProduct->ControlBitmap                     &= ~( TAPP_TPRODUCT_CONTROL_BORDER_STATUS_BITMAP );
        pMYKI_TAProduct->EndDateTime                        = pData->DynamicData.currentDateTime + ( 24 * 60 * 60 );

        if ( BR_LLSC_1_9( pData ) != RULE_RESULT_EXECUTED )
        {
            return FALSE;
        }   /* end-of-if */
    }

    return TRUE;
}   /* test_BR_LLSC_1_9_008( ) */
