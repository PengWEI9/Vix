/* -*- mode: c; tabs: 4; -*- */
/*=============================================================================
**
**    Vix Technology                   Licensed software
**    (C) 2013                         All rights reserved
**
**=============================================================================
**
**  Project/Product : MBU
**  Filename        : test_BR_LLSC_2_7.c
**  Author(s)       : An Tran
**
**  Description     :
**      Implements BR_LLSC_2_7 business rule unit-tests.
**
**  Function(s)     :
**      test_BR_LLSC_2_7_XXX       [Public]    unit-test functions
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
**  test_BR_LLSC_2_7_001
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

int test_BR_LLSC_2_7_001( MYKI_BR_ContextData_t *pData )
{
    MYKI_TAControl_t       *pMYKI_TAControl         = NULL;
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

    /*  Pre-Conditions:
            Actionlist.Type != ACTION_PRODUCT_SALE_NONE

        Post-Conditions:
            BYPASSED - Actionlist.Type != ACTION_PRODUCT_SALE_NONE
    */
    {
        pData->ActionList.type                              = 0;
        pData->ActionList.actionlist.productSale.actionSequenceNo                       = 0;
        if ( BR_LLSC_2_7( pData ) != RULE_RESULT_BYPASSED )
        {
            return FALSE;
        }   /* end-of-if */
    }

    /*  Pre-Conditions:
            Actionlist.Type = ACTION_PRODUCT_SALE_NONE
            Actionlist.ActionSeqNo < 1

        Post-Conditions:
            BYPASSED - Actionlist.ActionSeqNo < 1
    */
    {
        pData->ActionList.type                              = ACTION_PRODUCT_SALE_NONE;
        pData->ActionList.actionlist.productSale.actionSequenceNo                       = 0;
        pMYKI_TAControl->NextProductSerialNo                = nProductSerialNo;
        if ( BR_LLSC_2_7( pData ) != RULE_RESULT_BYPASSED )
        {
            return FALSE;
        }   /* end-of-if */
    }

    /*  Pre-Conditions:
            Actionlist.Type = ACTION_PRODUCT_SALE_NONE
            Actionlist.ActionSeqNo > 15

        Post-Conditions:
            BYPASSED - Actionlist.ActionSeqNo > 15
    */
    {
        pData->ActionList.type                              = ACTION_PRODUCT_SALE_NONE;
        pData->ActionList.actionlist.productSale.actionSequenceNo                       = 16;
        pMYKI_TAControl->NextProductSerialNo                = nProductSerialNo;
        if ( BR_LLSC_2_7( pData ) != RULE_RESULT_BYPASSED )
        {
            return FALSE;
        }   /* end-of-if */
    }

    /*  Pre-Conditions:
            Actionlist.Type = ACTION_PRODUCT_SALE_NONE
            Actionlist.ActionSeqNo = 2 (ie. != 1)
            TAppControl.ActionSeqNo = 15

        Post-Conditions:
            BYPASSED - Wrong Actionlist.ActionSeqNo
    */
    {
        pData->ActionList.type                              = ACTION_PRODUCT_SALE_NONE;
        pData->ActionList.actionlist.productSale.actionSequenceNo                       = 2;
        pMYKI_TAControl->ActionSeqNo                        = 15;
        pMYKI_TAControl->NextProductSerialNo                = nProductSerialNo;
        if ( BR_LLSC_2_7( pData ) != RULE_RESULT_BYPASSED )
        {
            return FALSE;
        }   /* end-of-if */
    }

    /*  Pre-Conditions:
            Actionlist.Type = ACTION_PRODUCT_SALE_NONE
            Actionlist.ActionSeqNo = 14 (!= 13)
            TAppControl.ActionSeqNo = 12

        Post-Conditions:
            BYPASSED - Wrong Actionlist.ActionSeqNo
    */
    {
        pData->ActionList.type                              = ACTION_PRODUCT_SALE_NONE;
        pData->ActionList.actionlist.productSale.actionSequenceNo                       = 14;
        pMYKI_TAControl->ActionSeqNo                        = 12;
        pMYKI_TAControl->NextProductSerialNo                = nProductSerialNo;
        if ( BR_LLSC_2_7( pData ) != RULE_RESULT_BYPASSED )
        {
            return FALSE;
        }   /* end-of-if */
    }

    /*  Pre-Conditions:
            Actionlist.Type = ACTION_PRODUCT_SALE_NONE
            Actionlist.ActionSeqNo = 13
            TAppControl.ActionSeqNo = 12
            no unused product

        Post-Conditions:
            BYPASSED - no unused product
    */
    {
        pData->ActionList.type                              = ACTION_PRODUCT_SALE_NONE;
        pData->ActionList.actionlist.productSale.actionSequenceNo                       = 13;
        for ( nDirIndex = 1; nDirIndex < DIMOF( pMYKI_TAControl->Directory ); nDirIndex++ )
        {
            pMYKI_TAControl->Directory[ nDirIndex ].Status      = TAPP_CONTROL_DIRECTORY_STATUS_ISSUED;
            pMYKI_TAControl->Directory[ nDirIndex ].SerialNo    = nProductSerialNo - nDirIndex;
            pMYKI_TAControl->Directory[ nDirIndex ].ProductId   = nProductId_ePass;
        }   /* end-of-if */
        pMYKI_TAControl->ActionSeqNo                        = 12;
        pMYKI_TAControl->NextProductSerialNo                = nProductSerialNo;
        if ( BR_LLSC_2_7( pData ) != RULE_RESULT_BYPASSED )
        {
            return FALSE;
        }   /* end-of-if */
    }

    /*  Pre-Conditions:
            Actionlist.Type = ACTION_PRODUCT_SALE_NONE
            Actionlist.ActionSeqNo = 13
            TAppControl.ActionSeqNo = 12
            more than one ePass

        Post-Conditions:
            BYPASSED - more than one ePass
    */
    {
        pData->ActionList.type                              = ACTION_PRODUCT_SALE_NONE;
        pData->ActionList.actionlist.productSale.actionSequenceNo                       = 13;
        pMYKI_TAControl->Directory[ 1 ].Status              = TAPP_CONTROL_DIRECTORY_STATUS_ISSUED;
        pMYKI_TAControl->Directory[ 1 ].SerialNo            = nProductSerialNo - 2;
        pMYKI_TAControl->Directory[ 1 ].ProductId           = nProductId_ePass;
        pMYKI_TAControl->Directory[ 2 ].Status              = TAPP_CONTROL_DIRECTORY_STATUS_ISSUED;
        pMYKI_TAControl->Directory[ 2 ].SerialNo            = nProductSerialNo - 1;
        pMYKI_TAControl->Directory[ 2 ].ProductId           = nProductId_ePass;
        pMYKI_TAControl->Directory[ 3 ].Status              = TAPP_CONTROL_DIRECTORY_STATUS_UNUSED;
        pMYKI_TAControl->Directory[ 4 ].Status              = TAPP_CONTROL_DIRECTORY_STATUS_UNUSED;
        pMYKI_TAControl->ActionSeqNo                        = 12;
        pMYKI_TAControl->NextProductSerialNo                = nProductSerialNo;
        if ( BR_LLSC_2_7( pData ) != RULE_RESULT_BYPASSED )
        {
            return FALSE;
        }   /* end-of-if */
    }

    return TRUE;
}   /* test_BR_LLSC_2_7_001( ) */

/*==========================================================================*
**
**  test_BR_LLSC_2_7_002
**
**  Description     :
**      Unit-test EXECUTED conditions, Dynamic.LoadLog.TxType = 0,
**      Dynamic.LoadLog.TxValue = 0.
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

int test_BR_LLSC_2_7_002( MYKI_BR_ContextData_t *pData )
{
    MYKI_TAControl_t       *pMYKI_TAControl         = NULL;
    MYKI_CAControl_t       *pMYKI_CAControl         = NULL;
    U16_t                   nProductSerialNo        = 555;
    U8_t                    nProductId_ePass        = 2;
    Currency_t              nPurchaseValue          = 555;
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

    /*  Pre-Conditions:
            Actionlist.Type = ACTION_PRODUCT_SALE_NONE
            Actionlist.ActionSeqNo = 13
            Actionlist.Value = 555
            TAppControl.ActionSeqNo = 12
            Dynamic.LoadLog.TxType = 0
            Dynamic.LoadLog.TxValue = 0

        Post-Conditions:
            TAppLoadLog.TransactionType = 2
            TAppLoadLog.TransactionValue = 555
    */
    {
        pData->ActionList.type                              = ACTION_PRODUCT_SALE_NONE;
        pData->ActionList.actionlist.productSale.actionSequenceNo                       = 13;
        pData->ActionList.actionlist.productSale.productId                         = nProductId_ePass;
        pData->ActionList.actionlist.productSale.issuerId                   = 9;
        pData->ActionList.actionlist.productSale.zoneLow                    = 1;
        pData->ActionList.actionlist.productSale.zoneHigh                   = 3;
        pData->ActionList.actionlist.productSale.purchaseValue                             = nPurchaseValue;
        pData->ActionList.actionlist.productSale.startDateTime                     = pData->DynamicData.currentDateTime - ( 24 * 60 * 60 );
        pData->ActionList.actionlist.productSale.endDateTime                       = pData->DynamicData.currentDateTime + ( 24 * 60 * 60 );
        pData->ActionList.actionlist.productSale.isAutoload                        = FALSE;
        pData->ActionList.actionlist.productSale.instanceCount                     = 4;
        pMYKI_TAControl->ActionSeqNo                        = 12;
        pMYKI_TAControl->NextProductSerialNo                = nProductSerialNo;
        pMYKI_TAControl->Status                             = TAPP_CONTROL_STATUS_ACTIVATED;
        pMYKI_CAControl->Status                             = CARD_CONTROL_STATUS_ACTIVATED;
        pData->DynamicData.entryPointId                     = EntryPointId;
        pData->DynamicData.lineId                           = LineId;
        pData->DynamicData.stopId                           = StopId;
        pData->DynamicData.loadLogTxValue                   = 0;
        pData->StaticData.serviceProviderId                 = ServiceProviderId;
        pData->InternalData.LoadLogData.transactionType     = MYKI_BR_TRANSACTION_TYPE_NONE;
        pData->InternalData.LoadLogData.transactionValue    = 0;
        if ( BR_LLSC_2_7( pData ) != RULE_RESULT_EXECUTED )
        {
            return FALSE;
        }   /* end-of-if */
    }

    return TRUE;
}   /* test_BR_LLSC_2_7_002( ) */


/*==========================================================================*
**
**  test_BR_LLSC_2_7_002
**
**  Description     :
**      Unit-test EXECUTED conditions, Dynamic.LoadLog.TxType = 0,
**      Dynamic.LoadLog.TxValue = 0.
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

int test_BR_LLSC_2_7_003( MYKI_BR_ContextData_t *pData )
{
    MYKI_TAControl_t       *pMYKI_TAControl         = NULL;
    MYKI_CAControl_t       *pMYKI_CAControl         = NULL;
    U8_t                    nProductId_ePass        = 2;
    U16_t                   nProductSerialNo        = 555;
    Currency_t              nPurchaseValue          = 555;
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

    /*  Pre-Conditions:
            Actionlist.Type = ACTION_PRODUCT_SALE_NONE
            Actionlist.ActionSeqNo = 13
            Actionlist.Value = 555
            TAppControl.ActionSeqNo = 12
            Dynamic.LoadLog.TxType = 1
            Dynamic.LoadLog.TxValue = 45

        Post-Conditions:
            TAppLoadLog.TransactionType = 21
            TAppLoadLog.TransactionValue = 600
    */
    {
        pData->ActionList.type                              = ACTION_PRODUCT_SALE_NONE;
        pData->ActionList.actionlist.productSale.actionSequenceNo                       = 13;
        pData->ActionList.actionlist.productSale.productId                         = nProductId_ePass;
        pData->ActionList.actionlist.productSale.issuerId                   = 9;
        pData->ActionList.actionlist.productSale.zoneLow                    = 1;
        pData->ActionList.actionlist.productSale.zoneHigh                   = 3;
        pData->ActionList.actionlist.productSale.purchaseValue                             = nPurchaseValue;
        pData->ActionList.actionlist.productSale.startDateTime                     = pData->DynamicData.currentDateTime - ( 24 * 60 * 60 );
        pData->ActionList.actionlist.productSale.endDateTime                       = pData->DynamicData.currentDateTime + ( 24 * 60 * 60 );
        pData->ActionList.actionlist.productSale.isAutoload                        = TRUE;
        pData->ActionList.actionlist.productSale.instanceCount                     = 4;
        pMYKI_TAControl->ActionSeqNo                        = 12;
        pMYKI_TAControl->NextProductSerialNo                = nProductSerialNo;
        pMYKI_TAControl->Status                             = TAPP_CONTROL_STATUS_ACTIVATED;
        pMYKI_CAControl->Status                             = CARD_CONTROL_STATUS_ACTIVATED;
        pData->DynamicData.entryPointId                     = EntryPointId;
        pData->DynamicData.lineId                           = LineId;
        pData->DynamicData.stopId                           = StopId;
        pData->DynamicData.loadLogTxValue                   = 45;
        pData->StaticData.serviceProviderId                 = ServiceProviderId;
        pData->InternalData.LoadLogData.transactionType     = MYKI_BR_TRANSACTION_TYPE_TPURSE_LOAD_VALUE;
        pData->InternalData.LoadLogData.transactionValue    = 45;
        if ( BR_LLSC_2_7( pData ) != RULE_RESULT_EXECUTED )
        {
            return FALSE;
        }   /* end-of-if */
    }

    return TRUE;
}   /* test_BR_LLSC_2_7_003( ) */

/*==========================================================================*
**
**  test_BR_LLSC_2_7_004
**
**  Description     :
**      Unit-test EXECUTED conditions, Dynamic.LoadLog.TxType = 0,
**      Dynamic.LoadLog.TxValue = 0.
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

int test_BR_LLSC_2_7_004( MYKI_BR_ContextData_t *pData )
{
    MYKI_TAControl_t       *pMYKI_TAControl         = NULL;
    MYKI_CAControl_t       *pMYKI_CAControl         = NULL;
    U8_t                    nProductId_ePass        = 2;
    U16_t                   nProductSerialNo        = 555;
    Currency_t              nPurchaseValue          = 555;
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

    /*  Pre-Conditions:
            Actionlist.Type = ACTION_PRODUCT_SALE_NONE
            Actionlist.ActionSeqNo = 13
            Actionlist.Value = 555
            TAppControl.ActionSeqNo = 12
            Dynamic.LoadLog.TxType = 1
            Dynamic.LoadLog.TxValue = 45

        Post-Conditions:
            TAppLoadLog.TransactionType = 21
            TAppLoadLog.TransactionValue = 600
    */
    {
        pData->DynamicData.currentDateTime                          = pData->DynamicData.currentDateTime - ( pData->DynamicData.currentDateTime % ( 24 * 60 * 60 ) );
        pData->ActionList.type                                      = ACTION_PRODUCT_SALE_NONE;
        pData->ActionList.actionlist.productSale.actionSequenceNo   = 13;
        pData->ActionList.actionlist.productSale.productId          = nProductId_ePass;
        pData->ActionList.actionlist.productSale.issuerId           = 9;
        pData->ActionList.actionlist.productSale.zoneLow            = 1;
        pData->ActionList.actionlist.productSale.zoneHigh           = 3;
        pData->ActionList.actionlist.productSale.purchaseValue      = nPurchaseValue;
        pData->ActionList.actionlist.productSale.startDateTime      = pData->DynamicData.currentDateTime;
        pData->ActionList.actionlist.productSale.isStartDateTimeSet = TRUE;
        pData->ActionList.actionlist.productSale.endDateTime        = pData->DynamicData.currentDateTime + ( 365 * 24 * 60 * 60 );
        pData->ActionList.actionlist.productSale.isEndDateTimeSet   = TRUE;
        pData->ActionList.actionlist.productSale.isAutoload         = TRUE;
        pData->ActionList.actionlist.productSale.instanceCount      = 4;
        pMYKI_TAControl->ActionSeqNo                                = 12;
        pMYKI_TAControl->NextProductSerialNo                        = nProductSerialNo;
        pMYKI_TAControl->Status                                     = TAPP_CONTROL_STATUS_ACTIVATED;
        pMYKI_CAControl->Status                                     = CARD_CONTROL_STATUS_ACTIVATED;
        pData->DynamicData.entryPointId                             = EntryPointId;
        pData->DynamicData.lineId                                   = LineId;
        pData->DynamicData.stopId                                   = StopId;
        pData->DynamicData.loadLogTxValue                           = 45;
        pData->StaticData.serviceProviderId                         = ServiceProviderId;
        pData->InternalData.LoadLogData.transactionType             = 0;
        pData->InternalData.LoadLogData.transactionValue            = 0;
        if ( BR_LLSC_2_7( pData ) != RULE_RESULT_EXECUTED )
        {
            return FALSE;
        }   /* end-of-if */
    }

    return TRUE;
}   /* test_BR_LLSC_2_7_004( ) */
