/* -*- mode: c; tabs: 4; -*- */
/*=============================================================================
**
**    Vix Technology                   Licensed software
**    (C) 2013                         All rights reserved
**
**=============================================================================
**
**  Project/Product : MBU
**  Filename        : test_BR_LLSC_10_6.c
**  Author(s)       : An Tran
**
**  Description     :
**      Implements BR_LLSC_10_6 business rule unit-tests.
**
**  Function(s)     :
**      test_BR_LLSC_10_6_XXX       [Public]    unit-test functions
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
**    1.00  25.10.13    ANT   Create
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
**  test_BR_LLSC_10_6_001
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

int test_BR_LLSC_10_6_001( MYKI_BR_ContextData_t *pData )
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
    int                     EntryPointId            = 11;

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
        if ( BR_LLSC_10_6( pData ) != RULE_RESULT_BYPASSED )
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
        pMYKI_TAControl->PassengerCode                      = PassengerCode_DDA;
        pMYKI_TAControl->ProductInUse                       = 0;
        if ( BR_LLSC_10_6( pData ) != RULE_RESULT_BYPASSED )
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
        if ( BR_LLSC_10_6( pData ) != RULE_RESULT_BYPASSED )
        {
            return FALSE;
        }   /* end-of-if */
    }

    /*  Pre-Conditions:
            TAppControl.ProductInUse = 2
            TAppControl.PassengerCode = 57 (ie. = DDA passenger type)
            TAppProduct.ControlBitmap = provisional
            TAppProduct.EndDateTime < Dynamic.CurrentDateTime

        Post-Conditions:
            BYPASSED - provisional product expired
    */
    {
        pMYKI_TAControl->PassengerCode                      = PassengerCode_DDA;
        pMYKI_TAControl->ProductInUse                       = nDirIndex;
        pMYKI_TAControl->Directory[ nDirIndex ].Status      = TAPP_CONTROL_DIRECTORY_STATUS_ACTIVATED;
        pMYKI_TAControl->Directory[ nDirIndex ].ProductId   = nProductId_NHours;
        pMYKI_TAProduct->ControlBitmap                     |= TAPP_TPRODUCT_CONTROL_PROVISIONAL_BITMAP;
        pMYKI_TAProduct->EndDateTime                        = pData->DynamicData.currentDateTime - ( 1 /*HOUR*/ * 60 * 60 );
        if ( BR_LLSC_10_6( pData ) != RULE_RESULT_BYPASSED )
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

        Post-Conditions:
            BYPASSED - not same provider
    */
    {
        pMYKI_TAControl->PassengerCode                      = PassengerCode_DDA;
        pMYKI_TAControl->ProductInUse                       = nDirIndex;
        pMYKI_TAControl->Directory[ nDirIndex ].Status      = TAPP_CONTROL_DIRECTORY_STATUS_ACTIVATED;
        pMYKI_TAControl->Directory[ nDirIndex ].ProductId   = nProductId_NHours;
        pMYKI_TAProduct->ControlBitmap                     |= TAPP_TPRODUCT_CONTROL_PROVISIONAL_BITMAP;
        pMYKI_TAProduct->EndDateTime                        = pData->DynamicData.currentDateTime + ( 1 /*HOUR*/ * 60 * 60 );
        pMYKI_TAProduct->LastUsage.ProviderId               = Provider_TRAM;
        pMYKI_TAProduct->LastUsage.Location.EntryPointId    = EntryPointId;
        pData->StaticData.serviceProviderId                 = Provider_RAIL;
        if ( BR_LLSC_10_6( pData ) != RULE_RESULT_BYPASSED )
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
            TAppProduct.LastUsage.ProviderId == Static.ServiceProviderId
            TAppProduct.LastUsage.EntryPointId != Dynamic.EntryPointId

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
        pMYKI_TAProduct->LastUsage.Location.EntryPointId    = EntryPointId;
        pData->StaticData.serviceProviderId                 = Provider_TRAM;
        pData->DynamicData.entryPointId                     = pMYKI_TAProduct->LastUsage.Location.EntryPointId + 1;
        if ( BR_LLSC_10_6( pData ) != RULE_RESULT_BYPASSED )
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
            Static.ServiceProviderId != RAIL

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
        pMYKI_TAProduct->LastUsage.Location.EntryPointId    = EntryPointId;
        pData->StaticData.serviceProviderId                 = Provider_TRAM;
        pData->InternalData.TransportMode                   = TRANSPORT_MODE_TRAM;
        pData->DynamicData.entryPointId                     = pMYKI_TAProduct->LastUsage.Location.EntryPointId;
        if ( BR_LLSC_10_6( pData ) != RULE_RESULT_BYPASSED )
        {
            return FALSE;
        }   /* end-of-if */
    }

    return TRUE;
}   /* test_BR_LLSC_10_6_001( ) */

/*==========================================================================*
**
**  test_BR_LLSC_10_6_002
**
**  Description     :
**      Unit-test EXECUTED conditions, scan off product in use.
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

int test_BR_LLSC_10_6_002( MYKI_BR_ContextData_t *pData )
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
            TAppProduct.EndDateTime > Dynamic.CurrentDateTime
            TAppProduct.LastUsage.ProviderId = RAIL
            Static.ServiceProviderId = RAIL

        Post-Conditions:
            EXECUTED - scan off product in use
            TAProduct.EndDateTime unchanged
    */
    {
        pMYKI_TAControl->NextProductSerialNo                = 1;
        pMYKI_TAControl->Status                             = TAPP_CONTROL_STATUS_ACTIVATED;
        pCAControl->Status                                  = CARD_CONTROL_STATUS_ACTIVATED;
        pMYKI_TAControl->PassengerCode                      = PassengerCode_DDA;
        pData->StaticData.serviceProviderId                 = Provider_RAIL;
        pData->StaticData.isEntry                           = FALSE;
        pData->StaticData.isExit                            = TRUE;
        pData->DynamicData.entryPointId                     = EntryPointId;
        pData->DynamicData.lineId                           = LineId;
        pData->DynamicData.stopId                           = StopId;
        pData->DynamicData.currentTripZoneLow               = 1;
        pData->DynamicData.currentTripZoneHigh              = 3;
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
        pMYKI_TAProduct->LastUsage.ProviderId               = Provider_RAIL;
        pMYKI_TAProduct->LastUsage.Location.EntryPointId    = pData->DynamicData.entryPointId + 1;
        pMYKI_TAProduct->LastUsage.DateTime                 = pMYKI_TAProduct->StartDateTime;
        pMYKI_TAProduct->LastUsage.Zone                     = 2;
        pData->InternalData.TransportMode                   = TRANSPORT_MODE_RAIL;
        if ( BR_LLSC_10_6( pData ) != RULE_RESULT_EXECUTED )
        {
            return FALSE;
        }   /* end-of-if */
    }

    return TRUE;
}   /* test_BR_LLSC_10_6_002( ) */

/*==========================================================================*
**
**  test_BR_LLSC_10_6_003
**
**  Description     :
**      Unit-test EXECUTED conditions, activate and scan off epass.
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

int test_BR_LLSC_10_6_003( MYKI_BR_ContextData_t *pData )
{
    MYKI_TAControl_t       *pMYKI_TAControl         = NULL;
    MYKI_TAProduct_t       *pMYKI_TAProduct         = NULL;
    MYKI_TAProduct_t       *pMYKI_TAProduct_EPass   = NULL;
    MYKI_CAControl_t       *pCAControl              = NULL;
    int                     PassengerCode_DDA       = 57;
//  int                     PassengerCode_Others    = 1;
//  int                     Provider_RAIL           = 0;
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
            Static.ServiceProviderId = TRAM

        Post-Conditions:
            EXECUTED - activate E-Pass
    */
    {
        pMYKI_TAControl->NextProductSerialNo                = 1;
        pMYKI_TAControl->Status                             = TAPP_CONTROL_STATUS_ACTIVATED;
        pCAControl->Status                                  = CARD_CONTROL_STATUS_ACTIVATED;
        pMYKI_TAControl->PassengerCode                              = PassengerCode_DDA;
        pData->StaticData.serviceProviderId                         = Provider_TRAM;
        pData->StaticData.isEntry                                   = TRUE;
        pData->StaticData.isExit                                    = FALSE;
        pData->DynamicData.entryPointId                             = EntryPointId;
        pData->DynamicData.lineId                                   = LineId;
        pData->DynamicData.stopId                                   = StopId;
        pData->DynamicData.currentTripZoneLow                       = 2;
        pData->DynamicData.currentTripZoneHigh                      = 2;
        pData->InternalData.TransportMode                           = TRANSPORT_MODE_TRAM;
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
            pMYKI_TAProduct->LastUsage.Location.EntryPointId        = pData->DynamicData.entryPointId;
            pMYKI_TAProduct->LastUsage.DateTime                     = pMYKI_TAProduct->StartDateTime;
            pMYKI_TAProduct->LastUsage.Zone                         = 3;
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
        if ( BR_LLSC_10_6( pData ) != RULE_RESULT_EXECUTED )
        {
            return FALSE;
        }   /* end-of-if */
    }

    return TRUE;
}   /* test_BR_LLSC_10_6_003( ) */

/*==========================================================================*
**
**  test_BR_LLSC_10_6_004
**
**  Description     :
**      Unit-test EXECUTED conditions, scan off epass.
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

int test_BR_LLSC_10_6_004( MYKI_BR_ContextData_t *pData )
{
    MYKI_TAControl_t       *pMYKI_TAControl         = NULL;
    MYKI_TAProduct_t       *pMYKI_TAProduct         = NULL;
    MYKI_TAProduct_t       *pMYKI_TAProduct_EPass   = NULL;
    MYKI_CAControl_t       *pCAControl              = NULL;
    int                     PassengerCode_DDA       = 57;
//  int                     PassengerCode_Others    = 1;
    int                     Provider_RAIL           = 0;
//  int                     Provider_RAIL_TWO       = 301;
//  int                     Provider_TRAM           = 1;
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
            TAppProduct[2].LastUsage.ProviderId = RAIL
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
        pData->StaticData.isEntry                                   = TRUE;
        pData->StaticData.isExit                                    = TRUE;
        pData->DynamicData.entryPointId                             = EntryPointId;
        pData->DynamicData.lineId                                   = LineId;
        pData->DynamicData.stopId                                   = StopId;
        pData->DynamicData.currentTripZoneLow                       = 2;
        pData->DynamicData.currentTripZoneHigh                      = 2;
        pData->InternalData.TransportMode                           = TRANSPORT_MODE_RAIL;
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
            pMYKI_TAProduct->LastUsage.ProviderId                   = Provider_RAIL;
            pMYKI_TAProduct->LastUsage.Location.EntryPointId        = pData->DynamicData.entryPointId + 1;
            pMYKI_TAProduct->LastUsage.Zone                         = 1;
        }
        {   /* Active E-Pass */
            pMYKI_TAControl->Directory[ nDirIndexEPass ].Status     = TAPP_CONTROL_DIRECTORY_STATUS_ACTIVATED;
            pMYKI_TAControl->Directory[ nDirIndexEPass ].ProductId  = nProductId_EPass;
            pMYKI_TAControl->Directory[ nDirIndexEPass ].IssuerId   = ProductIssuerId + 1;
            pMYKI_TAControl->Directory[ nDirIndexEPass ].SerialNo   = ProductSerialNo + 1;
        }
        if ( BR_LLSC_10_6( pData ) != RULE_RESULT_EXECUTED )
        {
            return FALSE;
        }   /* end-of-if */
    }

    return TRUE;
}   /* test_BR_LLSC_10_6_004( ) */
