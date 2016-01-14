/* -*- mode: c; tabs: 4; -*- */
/*=============================================================================
**
**    Vix Technology                   Licensed software
**    (C) 2013                         All rights reserved
**
**=============================================================================
**
**  Project/Product : MBU
**  Filename        : test_BR_LLSC_2_5.c
**  Author(s)       : An Tran
**
**  Description     :
**      Implements BR_LLSC_2_5 business rule unit-tests.
**
**  Function(s)     :
**      test_BR_LLSC_2_5_XXX       [Public]    unit-test functions
**
**  Information     :
**   Compiler(s)    : ANSI C
**   Target(s)      : Independent
**
**  Subversion      :
**      $Id: $
**      $HeadURL: $
**
**  History         :
**   Vers.  Date        Aut.  Type     Description
**   -----  ----------  ----  -------  ----------------------------------------
**    1.00  14.11.13    ANT   Create
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
**  test_BR_LLSC_2_5_001
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

int test_BR_LLSC_2_5_001( MYKI_BR_ContextData_t *pData )
{
    MYKI_TAControl_t       *pMYKI_TAControl         = NULL;
    MYKI_TAProduct_t       *pMYKI_TAProduct         = NULL;
    int                     nDirIndex               = 2;
    U8_t                    nProductId_ePass        = 2;
    U16_t                   nProductSerialNo        = 555;

    if ( pData == NULL )
    {
        return FALSE;
    }   /* end-of-if */

    if ( MYKI_CS_TAControlGet( &pMYKI_TAControl ) < 0 || pMYKI_TAControl == NULL )
    {
        return FALSE;
    }   /* end-of-if */

    if ( MYKI_CS_TAProductGet( nDirIndex - 1, &pMYKI_TAProduct ) < 0 || pMYKI_TAProduct == NULL )
    {
        return FALSE;
    }   /* end-of-if */

    /*  Pre-Conditions:
            Actionlist.Type != ACTION_PRODUCT_UPDATE_NONE

        Post-Conditions:
            BYPASSED - Actionlist.Type != ACTION_PRODUCT_UPDATE_NONE
    */
    {
        pData->ActionList.type                              = 0;
        pData->ActionList.actionlist.productUpdate.actionSequenceNo                       = 0;
        pData->ActionList.actionlist.productUpdate.serialNo                   = 0;
        if ( BR_LLSC_2_5( pData ) != RULE_RESULT_BYPASSED )
        {
            return FALSE;
        }   /* end-of-if */
    }

    /*  Pre-Conditions:
            Actionlist.Type = ACTION_PRODUCT_UPDATE_NONE
            Actionlist.ActionSeqNo < 1

        Post-Conditions:
            BYPASSED - Actionlist.ActionSeqNo < 1
    */
    {
        pData->ActionList.type                              = ACTION_PRODUCT_UPDATE_NONE;
        pData->ActionList.actionlist.productUpdate.actionSequenceNo                       = 0;
        pData->ActionList.actionlist.productUpdate.serialNo                   = 0;
        if ( BR_LLSC_2_5( pData ) != RULE_RESULT_BYPASSED )
        {
            return FALSE;
        }   /* end-of-if */
    }

    /*  Pre-Conditions:
            Actionlist.Type = ACTION_PRODUCT_UPDATE_NONE
            Actionlist.ActionSeqNo > 15

        Post-Conditions:
            BYPASSED - Actionlist.ActionSeqNo > 15
    */
    {
        pData->ActionList.type                              = ACTION_PRODUCT_UPDATE_NONE;
        pData->ActionList.actionlist.productUpdate.actionSequenceNo                       = 16;
        pData->ActionList.actionlist.productUpdate.serialNo                   = 0;
        if ( BR_LLSC_2_5( pData ) != RULE_RESULT_BYPASSED )
        {
            return FALSE;
        }   /* end-of-if */
    }

    /*  Pre-Conditions:
            Actionlist.Type = ACTION_PRODUCT_UPDATE_NONE
            Actionlist.ActionSeqNo = 2 (ie. != 1)
            TAppProduct.ActionSeqNo = 15

        Post-Conditions:
            BYPASSED - Wrong Actionlist.ActionSeqNo
    */
    {
        pData->ActionList.type                              = ACTION_PRODUCT_UPDATE_NONE;
        pData->ActionList.actionlist.productUpdate.actionSequenceNo                       = 2;
        pData->ActionList.actionlist.productUpdate.serialNo                   = nProductSerialNo;
        pMYKI_TAControl->Directory[ nDirIndex ].Status      = TAPP_CONTROL_DIRECTORY_STATUS_ACTIVATED;
        pMYKI_TAControl->Directory[ nDirIndex ].SerialNo    = nProductSerialNo;
        pMYKI_TAControl->Directory[ nDirIndex ].ProductId   = nProductId_ePass;
        pMYKI_TAProduct->ActionSeqNo                        = 15;
        if ( BR_LLSC_2_5( pData ) != RULE_RESULT_BYPASSED )
        {
            return FALSE;
        }   /* end-of-if */
    }

    /*  Pre-Conditions:
            Actionlist.Type = ACTION_PRODUCT_UPDATE_NONE
            Actionlist.ActionSeqNo = 14 (!= 13)
            TAppProduct.ActionSeqNo = 12

        Post-Conditions:
            BYPASSED - Wrong Actionlist.ActionSeqNo
    */
    {
        pData->ActionList.type                              = ACTION_PRODUCT_UPDATE_NONE;
        pData->ActionList.actionlist.productUpdate.actionSequenceNo                       = 14;
        pData->ActionList.actionlist.productUpdate.serialNo                   = nProductSerialNo;
        pMYKI_TAControl->Directory[ nDirIndex ].Status      = TAPP_CONTROL_DIRECTORY_STATUS_ACTIVATED;
        pMYKI_TAControl->Directory[ nDirIndex ].SerialNo    = nProductSerialNo;
        pMYKI_TAControl->Directory[ nDirIndex ].ProductId   = nProductId_ePass;
        pMYKI_TAProduct->ActionSeqNo                        = 12;
        if ( BR_LLSC_2_5( pData ) != RULE_RESULT_BYPASSED )
        {
            return FALSE;
        }   /* end-of-if */
    }

    /*  Pre-Conditions:
            Actionlist.Type = ACTION_PRODUCT_UPDATE_NONE
            Actionlist.ActionSeqNo = 13
            TAppProduct.ActionSeqNo = 12
            No product

        Post-Conditions:
            BYPASSED - no product found
    */
    {
        pData->ActionList.type                              = ACTION_PRODUCT_UPDATE_NONE;
        pData->ActionList.actionlist.productUpdate.actionSequenceNo                       = 13;
        pData->ActionList.actionlist.productUpdate.serialNo                   = nProductSerialNo + 1;
        pMYKI_TAControl->Directory[ nDirIndex ].Status      = TAPP_CONTROL_DIRECTORY_STATUS_ACTIVATED;
        pMYKI_TAControl->Directory[ nDirIndex ].SerialNo    = nProductSerialNo;
        pMYKI_TAControl->Directory[ nDirIndex ].ProductId   = nProductId_ePass;
        pMYKI_TAProduct->ActionSeqNo                        = 12;
        if ( BR_LLSC_2_5( pData ) != RULE_RESULT_BYPASSED )
        {
            return FALSE;
        }   /* end-of-if */
    }

    /*  Pre-Conditions:
            Actionlist.Type = ACTION_PRODUCT_UPDATE_NONE
            Actionlist.ActionSeqNo = 13
            TAppProduct.ActionSeqNo = 12
            product blocked

        Post-Conditions:
            BYPASSED - product blocked
    */
    {
        pData->ActionList.type                              = ACTION_PRODUCT_UPDATE_NONE;
        pData->ActionList.actionlist.productUpdate.actionSequenceNo                       = 13;
        pData->ActionList.actionlist.productUpdate.serialNo                   = nProductSerialNo;
        pMYKI_TAControl->Directory[ nDirIndex ].Status      = TAPP_CONTROL_DIRECTORY_STATUS_BLOCKED;
        pMYKI_TAControl->Directory[ nDirIndex ].SerialNo    = nProductSerialNo;
        pMYKI_TAControl->Directory[ nDirIndex ].ProductId   = nProductId_ePass;
        pMYKI_TAProduct->ActionSeqNo                        = 12;
        if ( BR_LLSC_2_5( pData ) != RULE_RESULT_BYPASSED )
        {
            return FALSE;
        }   /* end-of-if */
    }

    return TRUE;
}   /* test_BR_LLSC_2_5_001( ) */

/*==========================================================================*
**
**  test_BR_LLSC_2_5_002
**
**  Description     :
**      Unit-test EXECUTED conditions, disable auto-load.
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

int test_BR_LLSC_2_5_002( MYKI_BR_ContextData_t *pData )
{
    MYKI_TAControl_t       *pMYKI_TAControl         = NULL;
    MYKI_TAProduct_t       *pMYKI_TAProduct         = NULL;
    MYKI_CAControl_t       *pMYKI_CAControl         = NULL;
    int                     nDirIndex               = 2;
    U8_t                    nProductId_ePass        = 2;
    U16_t                   nProductSerialNo        = 555;
    int                     EntryPointId            = 53;
    int                     LineId                  = 66;
    int                     StopId                  = 79;
    int                     ServiceProviderId       = 100;

    if ( pData == NULL )
    {
        return FALSE;
    }   /* end-of-if */

    if ( MYKI_CS_TAControlGet( &pMYKI_TAControl ) < 0 || pMYKI_TAControl == NULL )
    {
        return FALSE;
    }   /* end-of-if */

    if ( MYKI_CS_CAControlGet( &pMYKI_CAControl ) != MYKI_CS_OK )
    {
        return FALSE;
    }

    if ( MYKI_CS_TAProductGet( nDirIndex - 1, &pMYKI_TAProduct ) < 0 || pMYKI_TAProduct == NULL )
    {
        return FALSE;
    }   /* end-of-if */

    /*  Pre-Conditions:
            Actionlist.Type = ACTION_PRODUCT_UPDATE_NONE
            Actionlist.ActionSeqNo = 13
            Actionlist.serialNo = 555
            Actionlist.isAutoload = false
            TAppProduct.ActionSeqNo = 12
            Product active

        Post-Conditions:
            Auto-load disabled
    */
    {
        pData->ActionList.type                              = ACTION_PRODUCT_UPDATE_NONE;
        pData->ActionList.actionlist.productUpdate.actionSequenceNo                       = 13;
        pData->ActionList.actionlist.productUpdate.serialNo                   = nProductSerialNo;
        pData->ActionList.actionlist.productUpdate.isAutoload                        = FALSE;
        pMYKI_TAProduct->ActionSeqNo                        = 12;
        pMYKI_TAProduct->ControlBitmap                     |= (TAPP_TPRODUCT_CONTROL_AUTOLOAD_BITMAP);
        pMYKI_TAControl->Directory[ nDirIndex ].Status      = TAPP_CONTROL_DIRECTORY_STATUS_ACTIVATED;
        pMYKI_TAControl->Directory[ nDirIndex ].SerialNo    = nProductSerialNo;
        pMYKI_TAControl->Directory[ nDirIndex ].ProductId   = nProductId_ePass;
        pMYKI_TAControl->Status                             = TAPP_CONTROL_STATUS_ACTIVATED;
        pMYKI_CAControl->Status                             = CARD_CONTROL_STATUS_ACTIVATED;
        pData->DynamicData.entryPointId                     = EntryPointId;
        pData->DynamicData.lineId                           = LineId;
        pData->DynamicData.stopId                           = StopId;
        pData->StaticData.serviceProviderId                 = ServiceProviderId;
        if ( BR_LLSC_2_5( pData ) != RULE_RESULT_EXECUTED )
        {
            return FALSE;
        }   /* end-of-if */
    }

    return TRUE;
}   /* test_BR_LLSC_2_5_002( ) */

/*==========================================================================*
**
**  test_BR_LLSC_2_5_003
**
**  Description     :
**      Unit-test EXECUTED conditions, enable auto-load.
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

int test_BR_LLSC_2_5_003( MYKI_BR_ContextData_t *pData )
{
    MYKI_TAControl_t       *pMYKI_TAControl         = NULL;
    MYKI_TAProduct_t       *pMYKI_TAProduct         = NULL;
    MYKI_CAControl_t       *pMYKI_CAControl         = NULL;
    int                     nDirIndex               = 2;
    U8_t                    nProductId_ePass        = 2;
    U16_t                   nProductSerialNo        = 555;
    int                     EntryPointId            = 53;
    int                     LineId                  = 66;
    int                     StopId                  = 79;
    int                     ServiceProviderId       = 100;

    if ( pData == NULL )
    {
        return FALSE;
    }   /* end-of-if */

    if ( MYKI_CS_TAControlGet( &pMYKI_TAControl ) < 0 || pMYKI_TAControl == NULL )
    {
        return FALSE;
    }   /* end-of-if */

    if ( MYKI_CS_CAControlGet( &pMYKI_CAControl ) != MYKI_CS_OK )
    {
        return FALSE;
    }

    if ( MYKI_CS_TAProductGet( nDirIndex - 1, &pMYKI_TAProduct ) < 0 || pMYKI_TAProduct == NULL )
    {
        return FALSE;
    }   /* end-of-if */

    /*  Pre-Conditions:
            Actionlist.Type = ACTION_PRODUCT_UPDATE_NONE
            Actionlist.ActionSeqNo = 13
            Actionlist.serialNo = 555
            Actionlist.isAutoload = true
            TAppProduct.ActionSeqNo = 12
            Product active

        Post-Conditions:
            Auto-load enabled
    */
    {
        pData->ActionList.type                              = ACTION_PRODUCT_UPDATE_NONE;
        pData->ActionList.actionlist.productUpdate.actionSequenceNo                       = 13;
        pData->ActionList.actionlist.productUpdate.serialNo                   = nProductSerialNo;
        pData->ActionList.actionlist.productUpdate.isAutoload                        = TRUE;
        pMYKI_TAProduct->ActionSeqNo                        = 12;
        pMYKI_TAProduct->ControlBitmap                     &= ~(TAPP_TPRODUCT_CONTROL_AUTOLOAD_BITMAP);
        pMYKI_TAControl->Directory[ nDirIndex ].Status      = TAPP_CONTROL_DIRECTORY_STATUS_ACTIVATED;
        pMYKI_TAControl->Directory[ nDirIndex ].SerialNo    = nProductSerialNo;
        pMYKI_TAControl->Directory[ nDirIndex ].ProductId   = nProductId_ePass;
        pMYKI_TAControl->Status                             = TAPP_CONTROL_STATUS_ACTIVATED;
        pMYKI_CAControl->Status                             = CARD_CONTROL_STATUS_ACTIVATED;
        pData->DynamicData.entryPointId                     = EntryPointId;
        pData->DynamicData.lineId                           = LineId;
        pData->DynamicData.stopId                           = StopId;
        pData->StaticData.serviceProviderId                 = ServiceProviderId;
        if ( BR_LLSC_2_5( pData ) != RULE_RESULT_EXECUTED )
        {
            return FALSE;
        }   /* end-of-if */
    }

    return TRUE;
}   /* test_BR_LLSC_2_5_003( ) */
