/* -*- mode: c; tabs: 4; -*- */
/*=============================================================================
**
**    Vix Technology                   Licensed software
**    (C) 2013                         All rights reserved
**
**=============================================================================
**
**  Project/Product : MBU
**  Filename        : test_BR_LLSC_10_4.c
**  Author(s)       : An Tran
**
**  Description     :
**      Implements BR_LLSC_10_4 business rule unit-tests.
**
**  Function(s)     :
**      test_BR_LLSC_10_4_XXX       [Public]    unit-test functions
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
**    1.00  23.10.13    ANT   Create
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
**  test_BR_LLSC_10_4_001
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

int test_BR_LLSC_10_4_001( MYKI_BR_ContextData_t *pData )
{
    MYKI_TAControl_t       *pMYKI_TAControl         = NULL;
//  MYKI_Directory_t       *pMYKI_Directory         = NULL;
    MYKI_TAProduct_t       *pMYKI_TAProduct         = NULL;
    U8_t                    nDirIndex               = 2;
    int                     PassengerCode_DDA       = 57;
    int                     PassengerCode_Others    = 1;
    int                     nProductId_NHours       = 4;
    int                     Provider_RAIL           = 0;
//  int                     Provider_RAIL_TWO       = 301;
    int                     Provider_TRAM           = 1;

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
            TAppControl.ProductInUse = 0
            TAppControl.PassengerCode = 1 (ie. != DDA passenger type)

        Post-Conditions:
            BYPASSED - passenger code is not DDA
    */
    {
        pMYKI_TAControl->ProductInUse                       = 0;
        pMYKI_TAControl->PassengerCode                      = PassengerCode_Others;
        if ( BR_LLSC_10_4( pData ) != RULE_RESULT_BYPASSED )
        {
            return FALSE;
        }   /* end-of-if */
    }

    /*  Pre-Conditions:
            TAppControl.ProductInUse = 0
            TAppControl.PassengerCode = 1 (ie. = DDA passenger type)

        Post-Conditions:
            BYPASSED - no product in use (ie. no provisional product)
    */
    {
        pMYKI_TAControl->ProductInUse                       = 0;
        pMYKI_TAControl->PassengerCode                      = PassengerCode_DDA;
        if ( BR_LLSC_10_4( pData ) != RULE_RESULT_BYPASSED )
        {
            return FALSE;
        }   /* end-of-if */
    }

    /*  Pre-Conditions:
            TAppControl.ProductInUse = 2
            TAppControl.PassengerCode = 57 (ie. = DDA passenger type)
            TAppProduct.ControlBitmap != provisional

        Post-Conditions:
            BYPASSED - product in use is not provisional product
    */
    {
        pMYKI_TAControl->PassengerCode                      = PassengerCode_DDA;
        pMYKI_TAControl->ProductInUse                       = nDirIndex;
        pMYKI_TAControl->Directory[ nDirIndex ].Status      = TAPP_CONTROL_DIRECTORY_STATUS_ACTIVATED;
        pMYKI_TAControl->Directory[ nDirIndex ].ProductId   = nProductId_NHours;
        pMYKI_TAProduct->ControlBitmap                     &= ~TAPP_TPRODUCT_CONTROL_PROVISIONAL_BITMAP;
        if ( BR_LLSC_10_4( pData ) != RULE_RESULT_BYPASSED )
        {
            return FALSE;
        }   /* end-of-if */
    }

    /*  Pre-Conditions:
            TAppControl.ProductInUse = 2
            TAppControl.PassengerCode = 57 (ie. = DDA passenger type)
            TAppProduct.ControlBitmap = provisional
            TAppProduct.EndDateTime > Dynamic.CurrentDateTime
            TAppProduct.LastUsage.ProviderId != RAIL
            TAppProduct.LastUsage.ProviderId = Static.ServiceProviderId

        Post-Conditions:
            BYPASSED - same provider id
    */
    {
        pMYKI_TAControl->PassengerCode                      = PassengerCode_DDA;
        pMYKI_TAControl->ProductInUse                       = nDirIndex;
        pMYKI_TAControl->Directory[ nDirIndex ].Status      = TAPP_CONTROL_DIRECTORY_STATUS_ACTIVATED;
        pMYKI_TAControl->Directory[ nDirIndex ].ProductId   = nProductId_NHours;
        pMYKI_TAProduct->ControlBitmap                     |= TAPP_TPRODUCT_CONTROL_PROVISIONAL_BITMAP;
        pMYKI_TAProduct->EndDateTime                        = pData->DynamicData.currentDateTime + ( 1 /*HOUR*/ * 60 * 60 );
        pMYKI_TAProduct->LastUsage.ProviderId               = Provider_TRAM;
        pData->StaticData.serviceProviderId                 = pMYKI_TAProduct->LastUsage.ProviderId;
        if ( BR_LLSC_10_4( pData ) != RULE_RESULT_BYPASSED )
        {
            return FALSE;
        }   /* end-of-if */
    }

    /*  Pre-Conditions:
            TAppControl.ProductInUse = 2
            TAppControl.PassengerCode = 57 (ie. = DDA passenger type)
            TAppProduct.ControlBitmap = provisional
            TAppProduct.EndDateTime > Dynamic.CurrentDateTime
            TAppProduct.LastUsage.ProviderId != RAIL
            TAppProduct.LastUsage.ProviderId != Static.ServiceProviderId
            TAppProduct.LastUsage.EntryPointId = Dynamic.EntryPointId

        Post-Conditions:
            BYPASSED - same entry point id
    */
    {
        pMYKI_TAControl->PassengerCode                      = PassengerCode_DDA;
        pMYKI_TAControl->ProductInUse                       = nDirIndex;
        pMYKI_TAControl->Directory[ nDirIndex ].Status      = TAPP_CONTROL_DIRECTORY_STATUS_ACTIVATED;
        pMYKI_TAControl->Directory[ nDirIndex ].ProductId   = nProductId_NHours;
        pMYKI_TAProduct->ControlBitmap                     |= TAPP_TPRODUCT_CONTROL_PROVISIONAL_BITMAP;
        pMYKI_TAProduct->EndDateTime                        = pData->DynamicData.currentDateTime + ( 1 /*HOUR*/ * 60 * 60 );
        pMYKI_TAProduct->LastUsage.ProviderId               = Provider_TRAM;
        pMYKI_TAProduct->LastUsage.Location.EntryPointId    = 11;
        pData->StaticData.serviceProviderId                 = Provider_RAIL;
        pData->DynamicData.entryPointId                     = pMYKI_TAProduct->LastUsage.Location.EntryPointId;
        if ( BR_LLSC_10_4( pData ) != RULE_RESULT_BYPASSED )
        {
            return FALSE;
        }   /* end-of-if */
    }

    /*  Pre-Conditions:
            TAppControl.ProductInUse = 2
            TAppControl.PassengerCode = 57 (ie. = DDA passenger type)
            TAppProduct.ControlBitmap = provisional
            TAppProduct.EndDateTime > Dynamic.CurrentDateTime
            TAppProduct.LastUsage.ProviderId = RAIL
            Static.ServiceProviderId = RAIL

        Post-Conditions:
            BYPASSED - same entry point id
    */
    {
        pMYKI_TAControl->PassengerCode                      = PassengerCode_DDA;
        pMYKI_TAControl->ProductInUse                       = nDirIndex;
        pMYKI_TAControl->Directory[ nDirIndex ].Status      = TAPP_CONTROL_DIRECTORY_STATUS_ACTIVATED;
        pMYKI_TAControl->Directory[ nDirIndex ].ProductId   = nProductId_NHours;
        pMYKI_TAProduct->ControlBitmap                     |= TAPP_TPRODUCT_CONTROL_PROVISIONAL_BITMAP;
        pMYKI_TAProduct->EndDateTime                        = pData->DynamicData.currentDateTime + ( 1 /*HOUR*/ * 60 * 60 );
        pMYKI_TAProduct->LastUsage.ProviderId               = Provider_RAIL;
        pData->StaticData.serviceProviderId                 = Provider_RAIL;
        pData->InternalData.TransportMode                   = TRANSPORT_MODE_RAIL;
        if ( BR_LLSC_10_4( pData ) != RULE_RESULT_BYPASSED )
        {
            return FALSE;
        }   /* end-of-if */
    }

    return TRUE;
}   /* test_BR_LLSC_10_4_001( ) */

/*==========================================================================*
**
**  test_BR_LLSC_10_4_002
**
**  Description     :
**      Unit-test EXECUTED conditions, forced scan off product in use.
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

int test_BR_LLSC_10_4_002( MYKI_BR_ContextData_t *pData )
{
    MYKI_TAControl_t       *pMYKI_TAControl         = NULL;
    MYKI_TAProduct_t       *pMYKI_TAProduct         = NULL;
    MYKI_CAControl_t       *pCAControl              = NULL;
    int                     PassengerCode_DDA       = 57;
//  int                     PassengerCode_Others    = 1;
    int                     Provider_RAIL           = 0;
//  int                     Provider_RAIL_TWO       = 301;
//  int                     Provider_TRAM           = 1;
    U8_t                    nDirIndex               = 2;
    int                     nProductId_NHours       = 4;
    int                     EntryPointId            = 53;
    int                     LineId                  = 66;
    int                     StopId                  = 79;
//  int                     ServiceProviderId       = 100;
//  int                     CurrentZone             = 2;
    int                     ProductIssuerId         = 43;
    int                     ProductSerialNo         = 99;

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
            TAppControl.PassengerCode = 57 (ie. = DDA passenger type)
            TAppProduct.ControlBitmap = provisional
            TAppProduct.EndDateTime < Dynamic.CurrentDateTime

        Post-Conditions:
            EXECUTED - forced scan off product in use
            TAProduct.EndDateTime unchanged
    */
    {
        pMYKI_TAControl->NextProductSerialNo                = 1;
        pMYKI_TAControl->Status                             = TAPP_CONTROL_STATUS_ACTIVATED;
        pCAControl->Status                                  = CARD_CONTROL_STATUS_ACTIVATED;
        pMYKI_TAControl->PassengerCode                      = PassengerCode_DDA;
        pData->StaticData.serviceProviderId                 = Provider_RAIL;
        pData->DynamicData.entryPointId                     = EntryPointId;
        pData->DynamicData.lineId                           = LineId;
        pData->DynamicData.stopId                           = StopId;
        pMYKI_TAControl->ProductInUse                       = nDirIndex;
        pMYKI_TAControl->Directory[ nDirIndex ].Status      = TAPP_CONTROL_DIRECTORY_STATUS_ACTIVATED;
        pMYKI_TAControl->Directory[ nDirIndex ].ProductId   = nProductId_NHours;
        pMYKI_TAControl->Directory[ nDirIndex ].IssuerId    = ProductIssuerId;
        pMYKI_TAControl->Directory[ nDirIndex ].SerialNo    = ProductSerialNo;
        pMYKI_TAProduct->ControlBitmap                     |= TAPP_TPRODUCT_CONTROL_PROVISIONAL_BITMAP;
        pMYKI_TAProduct->StartDateTime                      = pData->DynamicData.currentDateTime - ( 3 /*HOUR*/ * 60 * 60 );
        pMYKI_TAProduct->EndDateTime                        = pData->DynamicData.currentDateTime - ( 1 /*HOUR*/ * 60 * 60 );
        pMYKI_TAProduct->ZoneLow                            = 1;
        pMYKI_TAProduct->ZoneHigh                           = 3;
        if ( BR_LLSC_10_4( pData ) != RULE_RESULT_EXECUTED )
        {
            return FALSE;
        }   /* end-of-if */
    }

    return TRUE;
}   /* test_BR_LLSC_10_4_002( ) */

/*==========================================================================*
**
**  test_BR_LLSC_10_4_003
**
**  Description     :
**      Unit-test EXECUTED conditions, forced scan off product in use.
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

int test_BR_LLSC_10_4_003( MYKI_BR_ContextData_t *pData )
{
    MYKI_TAControl_t       *pMYKI_TAControl         = NULL;
    MYKI_TAProduct_t       *pMYKI_TAProduct         = NULL;
    MYKI_CAControl_t       *pCAControl              = NULL;
    int                     PassengerCode_DDA       = 57;
//  int                     PassengerCode_Others    = 1;
    int                     Provider_RAIL           = 0;
//  int                     Provider_RAIL_TWO       = 301;
    int                     Provider_TRAM           = 1;
    U8_t                    nDirIndex               = 2;
    int                     nProductId_NHours       = 4;
    int                     EntryPointId            = 53;
    int                     LineId                  = 66;
    int                     StopId                  = 79;
//  int                     ServiceProviderId       = 100;
//  int                     CurrentZone             = 2;
    int                     ProductIssuerId         = 43;
    int                     ProductSerialNo         = 99;

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
            TAppControl.PassengerCode = 57 (ie. = DDA passenger type)
            TAppProduct.ControlBitmap = provisional
            TAppProduct.EndDateTime > Dynamic.CurrentDateTime
            TAppProduct.LastUsage.ProviderId = TRAM
            Static.ServiceProviderId = RAIL

        Post-Conditions:
            EXECUTED - forced scan off product in use
            TAProduct.EndDateTime = Dynamic.CurrentDateTime
    */
    {
        pMYKI_TAControl->NextProductSerialNo                = 1;
        pMYKI_TAControl->Status                             = TAPP_CONTROL_STATUS_ACTIVATED;
        pCAControl->Status                                  = CARD_CONTROL_STATUS_ACTIVATED;
        pMYKI_TAControl->PassengerCode                      = PassengerCode_DDA;
        pData->StaticData.serviceProviderId                 = Provider_RAIL;
        pData->DynamicData.entryPointId                     = EntryPointId;
        pData->DynamicData.lineId                           = LineId;
        pData->DynamicData.stopId                           = StopId;
        pMYKI_TAControl->ProductInUse                       = nDirIndex;
        pMYKI_TAControl->Directory[ nDirIndex ].Status      = TAPP_CONTROL_DIRECTORY_STATUS_ACTIVATED;
        pMYKI_TAControl->Directory[ nDirIndex ].ProductId   = nProductId_NHours;
        pMYKI_TAControl->Directory[ nDirIndex ].IssuerId    = ProductIssuerId;
        pMYKI_TAControl->Directory[ nDirIndex ].SerialNo    = ProductSerialNo;
        pMYKI_TAProduct->ControlBitmap                     |= TAPP_TPRODUCT_CONTROL_PROVISIONAL_BITMAP;
        pMYKI_TAProduct->StartDateTime                      = pData->DynamicData.currentDateTime - ( 1 /*HOUR*/ * 60 * 60 );
        pMYKI_TAProduct->EndDateTime                        = pData->DynamicData.currentDateTime + ( 1 /*HOUR*/ * 60 * 60 );
        pMYKI_TAProduct->ZoneLow                            = 1;
        pMYKI_TAProduct->ZoneHigh                           = 3;
        pMYKI_TAProduct->LastUsage.ProviderId               = Provider_TRAM;
        pMYKI_TAProduct->LastUsage.Location.EntryPointId    = pData->DynamicData.entryPointId + 1;
        pMYKI_TAProduct->LastUsage.DateTime                 = pMYKI_TAProduct->StartDateTime;
        if ( BR_LLSC_10_4( pData ) != RULE_RESULT_EXECUTED )
        {
            return FALSE;
        }   /* end-of-if */
    }

    return TRUE;
}   /* test_BR_LLSC_10_4_003( ) */

/*==========================================================================*
**
**  test_BR_LLSC_10_4_004
**
**  Description     :
**      Unit-test EXECUTED conditions, activate and force scan off epass.
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

int test_BR_LLSC_10_4_004( MYKI_BR_ContextData_t *pData )
{
    MYKI_TAControl_t       *pMYKI_TAControl         = NULL;
    MYKI_TAProduct_t       *pMYKI_TAProduct         = NULL;
    MYKI_TAProduct_t       *pMYKI_TAProduct_EPass   = NULL;
    MYKI_CAControl_t       *pCAControl              = NULL;
    int                     PassengerCode_DDA       = 57;
//  int                     PassengerCode_Others    = 1;
    int                     Provider_RAIL           = 0;
//  int                     Provider_RAIL_TWO       = 301;
    int                     Provider_TRAM           = 1;
    U8_t                    nDirIndex               = 2;
    U8_t                    nDirIndexEPass          = 3;
    int                     nProductId_EPass        = 2;
    int                     nProductId_NHours       = 4;
    int                     EntryPointId            = 53;
    int                     LineId                  = 66;
    int                     StopId                  = 79;
//  int                     ServiceProviderId       = 100;
//  int                     CurrentZone             = 2;
    int                     ProductIssuerId         = 43;
    int                     ProductSerialNo         = 99;

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

    if ( MYKI_CS_TAProductGet( (U8_t)( nDirIndexEPass - 1 ), &pMYKI_TAProduct_EPass ) < 0 || pMYKI_TAProduct_EPass == NULL )
    {
        return FALSE;
    }   /* end-of-if */

    if ( MYKI_CS_CAControlGet(&pCAControl) != MYKI_CS_OK )
    {
        return FALSE;
    }

    /*  Pre-Conditions:
            TAppControl.ProductInUse = 2
            TAppControl.PassengerCode = 57 (ie. = DDA passenger type)
            TAppProduct[2].ControlBitmap = provisional
            TAppProduct[2].EndDateTime > Dynamic.CurrentDateTime
            TAppProduct[2].LastUsage.ProviderId = TRAM
            EPass inactive
            Static.ServiceProviderId = RAIL

        Post-Conditions:
            EXECUTED - activate E-Pass
    */
    {
        pMYKI_TAControl->NextProductSerialNo                        = 1;
        pMYKI_TAControl->Status                                     = TAPP_CONTROL_STATUS_ACTIVATED;
        pCAControl->Status                                          = CARD_CONTROL_STATUS_ACTIVATED;
        pMYKI_TAControl->PassengerCode                              = PassengerCode_DDA;
        pData->StaticData.serviceProviderId                         = Provider_RAIL;
        pData->DynamicData.entryPointId                             = EntryPointId;
        pData->DynamicData.lineId                                   = LineId;
        pData->DynamicData.stopId                                   = StopId;
        {   /* Product in use, ie. provisional product */
            pMYKI_TAControl->ProductInUse                           = nDirIndex;
            pMYKI_TAControl->Directory[ nDirIndex ].Status          = TAPP_CONTROL_DIRECTORY_STATUS_ACTIVATED;
            pMYKI_TAControl->Directory[ nDirIndex ].ProductId       = nProductId_NHours;
            pMYKI_TAControl->Directory[ nDirIndex ].IssuerId        = ProductIssuerId;
            pMYKI_TAControl->Directory[ nDirIndex ].SerialNo        = ProductSerialNo;
            pMYKI_TAProduct->ControlBitmap                         |= TAPP_TPRODUCT_CONTROL_PROVISIONAL_BITMAP;
            pMYKI_TAProduct->StartDateTime                          = pData->DynamicData.currentDateTime - ( 1 /*HOUR*/ * 60 * 60 );
            pMYKI_TAProduct->EndDateTime                            = pData->DynamicData.currentDateTime + ( 1 /*HOUR*/ * 60 * 60 );
            pMYKI_TAProduct->ZoneLow                                = 1;
            pMYKI_TAProduct->ZoneHigh                               = 3;
            pMYKI_TAProduct->LastUsage.ProviderId                   = Provider_TRAM;
            pMYKI_TAProduct->LastUsage.Location.EntryPointId        = pData->DynamicData.entryPointId + 1;
            pMYKI_TAProduct->LastUsage.DateTime                     = pMYKI_TAProduct->StartDateTime;
        }
        {   /* Inactive E-Pass */
            pMYKI_TAControl->Directory[ nDirIndexEPass ].Status     = TAPP_CONTROL_DIRECTORY_STATUS_ISSUED;
            pMYKI_TAControl->Directory[ nDirIndexEPass ].ProductId  = nProductId_EPass;
            pMYKI_TAControl->Directory[ nDirIndexEPass ].IssuerId   = ProductIssuerId + 1;
            pMYKI_TAControl->Directory[ nDirIndexEPass ].SerialNo   = ProductSerialNo + 1;
            pMYKI_TAProduct_EPass->ZoneLow                          = 1;
            pMYKI_TAProduct_EPass->ZoneHigh                         = 3;
            pMYKI_TAProduct_EPass->InstanceCount                    = 30 /*DAYS*/;
        }
        if ( BR_LLSC_10_4( pData ) != RULE_RESULT_EXECUTED )
        {
            return FALSE;
        }   /* end-of-if */
    }

    return TRUE;
}   /* test_BR_LLSC_10_4_004( ) */

/*==========================================================================*
**
**  test_BR_LLSC_10_4_005
**
**  Description     :
**      Unit-test EXECUTED conditions, force scan off epass.
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

int test_BR_LLSC_10_4_005( MYKI_BR_ContextData_t *pData )
{
    MYKI_TAControl_t       *pMYKI_TAControl         = NULL;
    MYKI_TAProduct_t       *pMYKI_TAProduct         = NULL;
    MYKI_TAProduct_t       *pMYKI_TAProduct_EPass   = NULL;
    MYKI_CAControl_t       *pCAControl              = NULL;
    int                     PassengerCode_DDA       = 57;
//  int                     PassengerCode_Others    = 1;
    int                     Provider_RAIL           = 0;
//  int                     Provider_RAIL_TWO       = 301;
    int                     Provider_TRAM           = 1;
    U8_t                    nDirIndex               = 2;
    U8_t                    nDirIndexEPass          = 3;
    int                     nProductId_EPass        = 2;
    int                     nProductId_NHours       = 4;
    int                     EntryPointId            = 53;
    int                     LineId                  = 66;
    int                     StopId                  = 79;
//  int                     ServiceProviderId       = 100;
//  int                     CurrentZone             = 2;
    int                     ProductIssuerId         = 43;
    int                     ProductSerialNo         = 99;

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

    if ( MYKI_CS_TAProductGet( (U8_t)( nDirIndexEPass - 1 ), &pMYKI_TAProduct_EPass ) < 0 || pMYKI_TAProduct_EPass == NULL )
    {
        return FALSE;
    }   /* end-of-if */

    if ( MYKI_CS_CAControlGet(&pCAControl) != MYKI_CS_OK )
    {
        return FALSE;
    }

    /*  Pre-Conditions:
            TAppControl.ProductInUse = 2
            TAppControl.PassengerCode = 57 (ie. = DDA passenger type)
            TAppProduct[2].ControlBitmap = provisional
            TAppProduct[2].EndDateTime > Dynamic.CurrentDateTime
            TAppProduct[2].LastUsage.ProviderId = TRAM
            EPass inactive
            Static.ServiceProviderId = RAIL

        Post-Conditions:
            EXECUTED - activate E-Pass
    */
    {
        pMYKI_TAControl->NextProductSerialNo                = 1;
        pMYKI_TAControl->Status                             = TAPP_CONTROL_STATUS_ACTIVATED;
        pCAControl->Status                                  = CARD_CONTROL_STATUS_ACTIVATED;
        pMYKI_TAControl->PassengerCode                              = PassengerCode_DDA;
        pData->StaticData.serviceProviderId                         = Provider_RAIL;
        pData->DynamicData.entryPointId                             = EntryPointId;
        pData->DynamicData.lineId                                   = LineId;
        pData->DynamicData.stopId                                   = StopId;
        {   /* Product in use, ie. provisional product */
            pMYKI_TAControl->ProductInUse                           = nDirIndex;
            pMYKI_TAControl->Directory[ nDirIndex ].Status          = TAPP_CONTROL_DIRECTORY_STATUS_ACTIVATED;
            pMYKI_TAControl->Directory[ nDirIndex ].ProductId       = nProductId_NHours;
            pMYKI_TAControl->Directory[ nDirIndex ].IssuerId        = ProductIssuerId;
            pMYKI_TAControl->Directory[ nDirIndex ].SerialNo        = ProductSerialNo;
            pMYKI_TAProduct->ControlBitmap                         |= TAPP_TPRODUCT_CONTROL_PROVISIONAL_BITMAP;
            pMYKI_TAProduct->StartDateTime                          = pData->DynamicData.currentDateTime - ( 1 /*HOUR*/ * 60 * 60 );
            pMYKI_TAProduct->EndDateTime                            = pData->DynamicData.currentDateTime + ( 1 /*HOUR*/ * 60 * 60 );
            pMYKI_TAProduct->ZoneLow                                = 1;
            pMYKI_TAProduct->ZoneHigh                               = 3;
            pMYKI_TAProduct->LastUsage.ProviderId                   = Provider_TRAM;
            pMYKI_TAProduct->LastUsage.Location.EntryPointId        = pData->DynamicData.entryPointId + 1;
        }
        {   /* Active E-Pass */
            pMYKI_TAControl->Directory[ nDirIndexEPass ].Status     = TAPP_CONTROL_DIRECTORY_STATUS_ACTIVATED;
            pMYKI_TAControl->Directory[ nDirIndexEPass ].ProductId  = nProductId_EPass;
            pMYKI_TAControl->Directory[ nDirIndexEPass ].IssuerId   = ProductIssuerId + 1;
            pMYKI_TAControl->Directory[ nDirIndexEPass ].SerialNo   = ProductSerialNo + 1;
        }
        if ( BR_LLSC_10_4( pData ) != RULE_RESULT_EXECUTED )
        {
            return FALSE;
        }   /* end-of-if */
    }

    return TRUE;
}   /* test_BR_LLSC_10_4_005( ) */
