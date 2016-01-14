/* -*- mode: c; tabs: 4; -*- */
/*=============================================================================
**
**    Vix Technology                   Licensed software
**    (C) 2013                         All rights reserved
**
**=============================================================================
**
**  Project/Product : MBU
**  Filename        : test_BR_LLSC_10_3.c
**  Author(s)       : An Tran
**
**  Description     :
**      Implements BR_LLSC_10_3 business rule unit-tests.
**
**  Function(s)     :
**      test_BR_LLSC_10_3_XXX       [Public]    unit-test functions
**
**  Information     :
**   Compiler(s)    : ANSI C
**   Target(s)      : Independent
**
**  Subversion      :
**      $HeadURL$
**      $Revision$
**      $LastChangedBy$
**      $LastChangedDate$
**
**  History         :
**   Vers.  Date        Aut.  Type     Description
**   -----  ----------  ----  -------  ----------------------------------------
**    1.00  18.10.13    ANT   Create
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

/*==========================================================================*
**
**  test_BR_LLSC_10_3_001
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

int test_BR_LLSC_10_3_001( MYKI_BR_ContextData_t *pData )
{
    MYKI_TAControl_t       *pMYKI_TAControl         = NULL;
//  MYKI_Directory_t       *pMYKI_Directory         = NULL;
    MYKI_TAProduct_t       *pMYKI_TAProduct         = NULL;
    U8_t                    nDirIndex               = 2;
//  int                     PassengerCode_DDA       = 57;
    int                     PassengerCode_Others    = 1;
    int                     nProductId_NHours       = 4;

    if ( pData == NULL )
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

    /*  Pre-Conditions:
            TAppControl.ProductInUse = 2
            TAppTProduct.ControlBitmap = provisional

        Post-Conditions:
            BYPASSED - provisional product found
    */
    {
        pMYKI_TAControl->ProductInUse                       = nDirIndex;
        pMYKI_TAControl->Directory[ nDirIndex ].Status      = TAPP_CONTROL_DIRECTORY_STATUS_ACTIVATED;
        pMYKI_TAControl->Directory[ nDirIndex ].ProductId   = nProductId_NHours;
        pMYKI_TAProduct->ControlBitmap                     |= TAPP_TPRODUCT_CONTROL_PROVISIONAL_BITMAP;
        if ( BR_LLSC_10_3( pData ) != RULE_RESULT_BYPASSED )
        {
            return FALSE;
        }   /* end-of-if */
    }

    /*  Pre-Conditions:
            TAppControl.ProductInUse = 0
            TAppControl.PassengerCode = 1 (ie. != DDA passenger type)

        Post-Conditions:
            BYPASSED - passenger code is not DDA
    */
    {
        pMYKI_TAControl->ProductInUse                       = 0;
        pMYKI_TAControl->PassengerCode                      = PassengerCode_Others;
        if ( BR_LLSC_10_3( pData ) != RULE_RESULT_BYPASSED )
        {
            return FALSE;
        }   /* end-of-if */
    }

    return TRUE;
}   /* test_BR_LLSC_10_3_001( ) */

/*==========================================================================*
**
**  test_BR_LLSC_10_3_002
**
**  Description     :
**      Unit-test EXECUTED conditions, neither Entry nor Exit, N-HOURS sold.
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

int test_BR_LLSC_10_3_002( MYKI_BR_ContextData_t *pData )
{
    MYKI_TAControl_t       *pMYKI_TAControl         = NULL;
    MYKI_CAControl_t       *pCAControl              = NULL;
    int                     PassengerCode_DDA       = 57;
//  int                     PassengerCode_Others    = 1;
    int                     EntryPointId            = 53;
    int                     LineId                  = 66;
    int                     StopId                  = 79;
    int                     ServiceProviderId       = 100;
    int                     CurrentZone             = 2;

    if ( pData == NULL )
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

    /*  Pre-Conditions:
            TAppControl.ProductInUse = 0
            TAppControl.PassengerCode = 57 (ie. == DDA Passenger type)
            Dynamic.ProvisionalZoneLow = 1
            Dynamic.ProvisionalZoneHigh = 3
            Static.IsEntry = false
            Static.IsExit = false
            Static.ServiceProviderId = 100

        Post-Conditions:
            TAppUsageLog.TxType = 20
    */
    {
        pMYKI_TAControl->NextProductSerialNo                = 1;
        pMYKI_TAControl->Status                             = TAPP_CONTROL_STATUS_ACTIVATED;
        pCAControl->Status                                  = CARD_CONTROL_STATUS_ACTIVATED;
        pMYKI_TAControl->ProductInUse           = 0;
        pMYKI_TAControl->PassengerCode          = PassengerCode_DDA;
        pData->DynamicData.provisionalZoneLow   = 1;
        pData->DynamicData.provisionalZoneHigh  = 3;
        pData->DynamicData.entryPointId         = EntryPointId;
        pData->DynamicData.lineId               = LineId;
        pData->DynamicData.stopId               = StopId;
        pData->DynamicData.currentZone          = CurrentZone;
        pData->StaticData.isEntry               = FALSE;
        pData->StaticData.isExit                = FALSE;
        pData->StaticData.serviceProviderId     = ServiceProviderId;
        if ( BR_LLSC_10_3( pData ) != RULE_RESULT_EXECUTED )
        {
            return FALSE;
        }   /* end-of-if */
    }

    return TRUE;
}   /* test_BR_LLSC_10_3_002( ) */

/*==========================================================================*
**
**  test_BR_LLSC_10_3_003
**
**  Description     :
**      Unit-test EXECUTED conditions, Exit only, N-HOURS sold.
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

int test_BR_LLSC_10_3_003( MYKI_BR_ContextData_t *pData )
{
    MYKI_TAControl_t       *pMYKI_TAControl         = NULL;
    MYKI_TAProduct_t       *pMYKI_TAProduct         = NULL;
    MYKI_CAControl_t       *pCAControl              = NULL;
    int                     PassengerCode_DDA       = 57;
//  int                     PassengerCode_Others    = 1;
    U8_t                    nDirIndex               = 2;
    int                     nProductId_NHours       = 4;
    int                     EntryPointId            = 53;
    int                     LineId                  = 66;
    int                     StopId                  = 79;
    int                     ServiceProviderId       = 100;
    int                     CurrentZone             = 2;

    if ( pData == NULL )
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

    /*  Pre-Conditions:
            TAppControl.ProductInUse = 2
            TAppControl.PassengerCode = 57 (ie. == DDA Passenger type)
            Dynamic.ProvisionalZoneLow = 1
            Dynamic.ProvisionalZoneHigh = 3
            TAppTProduct.ControlBitmap != provisional
            Static.IsEntry = false
            Static.IsExit = true
            Static.ServiceProviderId = 100

        Post-Conditions:
            TAppUsageLog.TxType = 18
    */
    {
        pMYKI_TAControl->NextProductSerialNo                = 1;
        pMYKI_TAControl->Status                             = TAPP_CONTROL_STATUS_ACTIVATED;
        pCAControl->Status                                  = CARD_CONTROL_STATUS_ACTIVATED;
        pMYKI_TAControl->ProductInUse                       = nDirIndex;
        pMYKI_TAControl->Directory[ nDirIndex ].Status      = TAPP_CONTROL_DIRECTORY_STATUS_ACTIVATED;
        pMYKI_TAControl->Directory[ nDirIndex ].ProductId   = nProductId_NHours;
        pMYKI_TAProduct->ControlBitmap                     &= ~TAPP_TPRODUCT_CONTROL_PROVISIONAL_BITMAP;
        pMYKI_TAControl->PassengerCode                      = PassengerCode_DDA;
        pData->DynamicData.provisionalZoneLow               = 1;
        pData->DynamicData.provisionalZoneHigh              = 3;
        pData->DynamicData.entryPointId                     = EntryPointId;
        pData->DynamicData.lineId                           = LineId;
        pData->DynamicData.stopId                           = StopId;
        pData->DynamicData.currentZone                      = CurrentZone;
        pData->StaticData.isEntry                           = FALSE;
        pData->StaticData.isExit                            = TRUE;
        pData->StaticData.serviceProviderId                 = ServiceProviderId;
        if ( BR_LLSC_10_3( pData ) != RULE_RESULT_EXECUTED )
        {
            return FALSE;
        }   /* end-of-if */
    }

    return TRUE;
}   /* test_BR_LLSC_10_3_003( ) */

/*==========================================================================*
**
**  test_BR_LLSC_10_3_004
**
**  Description     :
**      Unit-test EXECUTED conditions, Entry only, N-HOURS sold.
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

int test_BR_LLSC_10_3_004( MYKI_BR_ContextData_t *pData )
{
    MYKI_TAControl_t       *pMYKI_TAControl         = NULL;
    MYKI_CAControl_t       *pCAControl              = NULL;
    int                     PassengerCode_DDA       = 57;
//  int                     PassengerCode_Others    = 1;
    int                     EntryPointId            = 53;
    int                     LineId                  = 66;
    int                     StopId                  = 79;
    int                     ServiceProviderId       = 100;
    int                     CurrentZone             = 2;

    if ( pData == NULL )
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

    /*  Pre-Conditions:
            TAppControl.ProductInUse = 0
            TAppControl.PassengerCode = 57 (ie. == DDA Passenger type)
            Dynamic.ProvisionalZoneLow = 1
            Dynamic.ProvisionalZoneHigh = 3
            Static.IsEntry = true
            Static.IsExit = false
            Static.ServiceProviderId = 100

        Post-Conditions:
            TAppUsageLog.TxType = 19
    */
    {
        pMYKI_TAControl->NextProductSerialNo    = 1;
        pMYKI_TAControl->Status                 = TAPP_CONTROL_STATUS_ACTIVATED;
        pCAControl->Status                      = CARD_CONTROL_STATUS_ACTIVATED;
        pMYKI_TAControl->ProductInUse           = 0;
        pMYKI_TAControl->PassengerCode          = PassengerCode_DDA;
        pData->DynamicData.provisionalZoneLow   = 1;
        pData->DynamicData.provisionalZoneHigh  = 3;
        pData->DynamicData.entryPointId         = EntryPointId;
        pData->DynamicData.lineId               = LineId;
        pData->DynamicData.stopId               = StopId;
        pData->DynamicData.currentZone          = CurrentZone;
        pData->StaticData.isEntry               = TRUE;
        pData->StaticData.isExit                = FALSE;
        pData->StaticData.serviceProviderId     = ServiceProviderId;
        if ( BR_LLSC_10_3( pData ) != RULE_RESULT_EXECUTED )
        {
            return FALSE;
        }   /* end-of-if */
    }

    return TRUE;
}   /* test_BR_LLSC_10_3_004( ) */

/*==========================================================================*
**
**  test_BR_LLSC_10_3_005
**
**  Description     :
**      Unit-test EXECUTED conditions, Entry and Exit, N-HOURS sold.
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

int test_BR_LLSC_10_3_005( MYKI_BR_ContextData_t *pData )
{
    MYKI_TAControl_t       *pMYKI_TAControl         = NULL;
    MYKI_CAControl_t       *pCAControl              = NULL;
    int                     PassengerCode_DDA       = 57;
//  int                     PassengerCode_Others    = 1;
    int                     EntryPointId            = 53;
    int                     LineId                  = 66;
    int                     StopId                  = 79;
    int                     ServiceProviderId       = 100;
    int                     CurrentZone             = 2;

    if ( pData == NULL )
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

    /*  Pre-Conditions:
            TAppControl.ProductInUse = 0
            TAppControl.PassengerCode = 57 (ie. == DDA Passenger type)
            Dynamic.ProvisionalZoneLow = 1
            Dynamic.ProvisionalZoneHigh = 3
            Static.IsEntry = true
            Static.IsExit = true
            Static.ServiceProviderId = 100

        Post-Conditions:
            TAppUsageLog.TxType = 8
    */
    {
        pMYKI_TAControl->NextProductSerialNo    = 1;
        pMYKI_TAControl->Status                 = TAPP_CONTROL_STATUS_ACTIVATED;
        pCAControl->Status                      = CARD_CONTROL_STATUS_ACTIVATED;
        pMYKI_TAControl->ProductInUse           = 0;
        pMYKI_TAControl->PassengerCode          = PassengerCode_DDA;
        pData->DynamicData.provisionalZoneLow   = 1;
        pData->DynamicData.provisionalZoneHigh  = 3;
        pData->DynamicData.entryPointId         = EntryPointId;
        pData->DynamicData.lineId               = LineId;
        pData->DynamicData.stopId               = StopId;
        pData->DynamicData.currentZone          = CurrentZone;
        pData->StaticData.isEntry               = TRUE;
        pData->StaticData.isExit                = TRUE;
        pData->StaticData.serviceProviderId     = ServiceProviderId;
        if ( BR_LLSC_10_3( pData ) != RULE_RESULT_EXECUTED )
        {
            return FALSE;
        }   /* end-of-if */
    }

    return TRUE;
}   /* test_BR_LLSC_10_3_005( ) */
