/* -*- mode: c; tabs: 4; -*- */
/*=============================================================================
**
**    Vix Technology                   Licensed software
**    (C) 2013                         All rights reserved
**
**=============================================================================
**
**  Project/Product : MBU
**  Filename        : test_BR_LLSC_5_2.c
**  Author(s)       : An Tran
**
**  Description     :
**      Implements BR_LLSC_5_2 business rule unit-tests.
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
**    1.00  28.11.13    ANT   Create
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
**  test_BR_LLSC_5_2_001
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

int test_BR_LLSC_5_2_001( MYKI_BR_ContextData_t *pData )
{
    MYKI_CAControl_t       *pMYKI_CAControl         = NULL;
    MYKI_TAControl_t       *pMYKI_TAControl         = NULL;
    MYKI_TAPurseBalance_t  *pMYKI_TAPurseBalance    = NULL;
    MYKI_TAProduct_t       *pMYKI_TAProduct[ MYKI_DIRECTORY_MAX_LENGTH ]\
                                                    = { NULL };
    U8_t                    nDirIndex               = 3;
    U8_t                    nDirIndexOther          = nDirIndex - 1;
    S32_t                   nTPurseBalance          = 200;
    U8_t                    nProductId_EPass        = 2;

    if ( pData == NULL )
    {
        return FALSE;
    }   /* end-of-if */

    if ( MYKI_CS_CAControlGet( &pMYKI_CAControl ) != MYKI_CS_OK || pMYKI_CAControl == NULL )
    {
        return FALSE;
    }   /* end-of-if */

    if ( MYKI_CS_TAControlGet( &pMYKI_TAControl ) < 0 || pMYKI_TAControl == NULL )
    {
        return FALSE;
    }   /* end-of-if */

    if ( MYKI_CS_TAPurseBalanceGet( &pMYKI_TAPurseBalance ) < 0 || pMYKI_TAPurseBalance == NULL )
    {
        return FALSE;
    }   /* end-of-if */

    if ( MYKI_CS_TAProductGet( (U8_t)( nDirIndex - 1 ), &pMYKI_TAProduct[ nDirIndex ] ) < 0 ||
         pMYKI_TAProduct[ nDirIndex ] == NULL )
    {
        return FALSE;
    }   /* end-of-if */

    if ( MYKI_CS_TAProductGet( (U8_t)( nDirIndexOther - 1 ), &pMYKI_TAProduct[ nDirIndexOther ] ) < 0 ||
         pMYKI_TAProduct[ nDirIndexOther ] == NULL )
    {
        return FALSE;
    }   /* end-of-if */

    pMYKI_TAControl->Status                                     = TAPP_CONTROL_STATUS_ACTIVATED;
    pMYKI_CAControl->Status                                     = CARD_CONTROL_STATUS_ACTIVATED;
    pData->DynamicData.currentInnerZone                         = 2;
    pData->DynamicData.currentZone                              = 4;
    pData->DynamicData.currentOuterZone                         = 6;

    /*  Pre-Conditions:
            TAppTPurseBlance.Balance >= Tariff.TPurseMinBalance

        Post-Conditions:
            BYPASSED - Sufficient funds
    */
    {
        pMYKI_TAPurseBalance->Balance                           = nTPurseBalance;
        pData->Tariff.TPurseMinBalance                          = pMYKI_TAPurseBalance->Balance;
        if ( BR_LLSC_5_2( pData ) != RULE_RESULT_BYPASSED )
        {
            return FALSE;
        }   /* end-of-if */
    }

    /*  Pre-Conditions:
            TAppTPurseBlance.Balance < Tariff.TPurseMinBalance
            TAppControl.Directory[3].Status = "Active"
            TAppControl.Directory[3].ProductId = "e-Pass"
            TAppTProduct.ZoneLow <= Dynamic.CurrentZone.Inner
            TAppTProduct.ZoneHigh => Dynamic.CurrentZone.Inner
            TAppTProduct.EndDateTime > Dynamic.CurrentDateTime
            Tariff.PremiumSurcharge = 0

        Post-Conditions:
            BYPASSED - Active e-Pass covering inner zone and without surcharge
    */
    {
        pMYKI_TAPurseBalance->Balance                           = nTPurseBalance;
        pMYKI_TAControl->Directory[ nDirIndex ].ProductId       = nProductId_EPass;
        pMYKI_TAControl->Directory[ nDirIndex ].Status          = TAPP_CONTROL_DIRECTORY_STATUS_ACTIVATED;
        pMYKI_TAProduct[ nDirIndex ]->ZoneLow                   = pData->DynamicData.currentInnerZone - 1;
        pMYKI_TAProduct[ nDirIndex ]->ZoneHigh                  = pData->DynamicData.currentInnerZone;
        pMYKI_TAProduct[ nDirIndex ]->EndDateTime               = pData->DynamicData.currentDateTime + ( 1 /*HOUR*/ * 60 * 60 );
        pData->Tariff.TPurseMinBalance                          = pMYKI_TAPurseBalance->Balance + 50;
        pData->Tariff.premiumSurcharge                          = 0;
        if ( BR_LLSC_5_2( pData ) != RULE_RESULT_BYPASSED )
        {
            return FALSE;
        }   /* end-of-if */
    }

    /*  Pre-Conditions:
            TAppTPurseBlance.Balance < Tariff.TPurseMinBalance
            TAppControl.Directory[3].Status = "Active"
            TAppControl.Directory[3].ProductId = "e-Pass"
            TAppTProduct.ZoneLow <= Dynamic.CurrentZone.Actual
            TAppTProduct.ZoneHigh => Dynamic.CurrentZone.Actual
            TAppTProduct.EndDateTime > Dynamic.CurrentDateTime
            Tariff.PremiumSurcharge = 0

        Post-Conditions:
            BYPASSED - Active e-Pass covering actual zone and without surcharge
    */
    {
        pMYKI_TAPurseBalance->Balance                           = nTPurseBalance;
        pMYKI_TAControl->Directory[ nDirIndex ].ProductId       = nProductId_EPass;
        pMYKI_TAControl->Directory[ nDirIndex ].Status          = TAPP_CONTROL_DIRECTORY_STATUS_ACTIVATED;
        pMYKI_TAProduct[ nDirIndex ]->ZoneLow                   = pData->DynamicData.currentZone;
        pMYKI_TAProduct[ nDirIndex ]->ZoneHigh                  = pData->DynamicData.currentZone + 1;
        pMYKI_TAProduct[ nDirIndex ]->EndDateTime               = pData->DynamicData.currentDateTime + ( 1 /*HOUR*/ * 60 * 60 );
        pData->Tariff.TPurseMinBalance                          = pMYKI_TAPurseBalance->Balance + 50;
        pData->Tariff.premiumSurcharge                          = 0;
        if ( BR_LLSC_5_2( pData ) != RULE_RESULT_BYPASSED )
        {
            return FALSE;
        }   /* end-of-if */
    }

    /*  Pre-Conditions:
            TAppTPurseBlance.Balance < Tariff.TPurseMinBalance
            TAppControl.Directory[3].Status = "Active"
            TAppControl.Directory[3].ProductId = "e-Pass"
            TAppTProduct.ZoneLow <= Dynamic.CurrentZone.Outer
            TAppTProduct.ZoneHigh => Dynamic.CurrentZone.Outer
            TAppTProduct.EndDateTime > Dynamic.CurrentDateTime
            Tariff.PremiumSurcharge = 0

        Post-Conditions:
            BYPASSED - Active e-Pass covering outer zone and without surcharge
    */
    {
        pMYKI_TAPurseBalance->Balance                           = nTPurseBalance;
        pMYKI_TAControl->Directory[ nDirIndex ].ProductId       = nProductId_EPass;
        pMYKI_TAControl->Directory[ nDirIndex ].Status          = TAPP_CONTROL_DIRECTORY_STATUS_ACTIVATED;
        pMYKI_TAProduct[ nDirIndex ]->ZoneLow                   = pData->DynamicData.currentOuterZone;
        pMYKI_TAProduct[ nDirIndex ]->ZoneHigh                  = pData->DynamicData.currentOuterZone;
        pMYKI_TAProduct[ nDirIndex ]->EndDateTime               = pData->DynamicData.currentDateTime + ( 1 /*HOUR*/ * 60 * 60 );
        pData->Tariff.TPurseMinBalance                          = pMYKI_TAPurseBalance->Balance + 50;
        pData->Tariff.premiumSurcharge                          = 0;
        if ( BR_LLSC_5_2( pData ) != RULE_RESULT_BYPASSED )
        {
            return FALSE;
        }   /* end-of-if */
    }

    /*  Pre-Conditions:
            TAppTPurseBlance.Balance < Tariff.TPurseMinBalance
            TAppControl.Directory[2].Status = "Inactive"
            TAppControl.Directory[2].ProductId = "e-Pass"
            TAppControl.Directory[3].Status = "Active"
            TAppControl.Directory[3].ProductId = "e-Pass"
            TAppTProduct.ZoneLow <= Dynamic.CurrentZone.Actual
            TAppTProduct.ZoneHigh => Dynamic.CurrentZone.Actual
            TAppTProduct.EndDateTime > Dynamic.CurrentDateTime
            TAppTPurseBlance.Balance >= Tariff.PremiumSurcharge

        Post-Conditions:
            BYPASSED - Usable active e-Pass covering actual zone and
            with sufficient funds for surcharge
    */
    {
        pMYKI_TAPurseBalance->Balance                           = nTPurseBalance;
        pMYKI_TAControl->Directory[ nDirIndex ].ProductId       = nProductId_EPass;
        pMYKI_TAControl->Directory[ nDirIndex ].Status          = TAPP_CONTROL_DIRECTORY_STATUS_ACTIVATED;
        pMYKI_TAProduct[ nDirIndex ]->ZoneLow                   = pData->DynamicData.currentZone;
        pMYKI_TAProduct[ nDirIndex ]->ZoneHigh                  = pData->DynamicData.currentZone;
        pMYKI_TAProduct[ nDirIndex ]->EndDateTime               = pData->DynamicData.currentDateTime + ( 1 /*HOUR*/ * 60 * 60 );
        pMYKI_TAControl->Directory[ nDirIndexOther ].ProductId  = nProductId_EPass;
        pMYKI_TAControl->Directory[ nDirIndexOther ].Status     = TAPP_CONTROL_DIRECTORY_STATUS_ISSUED;
        pMYKI_TAProduct[ nDirIndexOther ]->ZoneLow              = pData->DynamicData.currentZone;
        pMYKI_TAProduct[ nDirIndexOther ]->ZoneHigh             = pData->DynamicData.currentZone;
        pMYKI_TAProduct[ nDirIndexOther ]->EndDateTime          = pData->DynamicData.currentDateTime + ( 1 /*HOUR*/ * 60 * 60 );
        pData->Tariff.TPurseMinBalance                          = pMYKI_TAPurseBalance->Balance + 50;
        pData->Tariff.premiumSurcharge                          = pMYKI_TAPurseBalance->Balance - 50;
        if ( BR_LLSC_5_2( pData ) != RULE_RESULT_BYPASSED )
        {
            return FALSE;
        }   /* end-of-if */
    }

    /*  Pre-Conditions:
            TAppTPurseBlance.Balance < Tariff.TPurseMinBalance
            TAppControl.Directory[2].Status = "Inactive"
            TAppControl.Directory[2].ProductId = "e-Pass" (Expired)
            TAppControl.Directory[3].Status = "Inactive"
            TAppControl.Directory[3].ProductId = "e-Pass"
            TAppTProduct.ZoneLow <= Dynamic.CurrentZone.Inner
            TAppTProduct.ZoneHigh => Dynamic.CurrentZone.Inner
            TAppTProduct.EndDateTime > Dynamic.CurrentDateTime
            Tariff.PremiumSurcharge = 0

        Post-Conditions:
            BYPASSED - Usable inactive e-Pass covering inner without surcharge
    */
    {
        pMYKI_TAPurseBalance->Balance                           = nTPurseBalance;
        pMYKI_TAControl->Directory[ nDirIndex ].ProductId       = nProductId_EPass;
        pMYKI_TAControl->Directory[ nDirIndex ].Status          = TAPP_CONTROL_DIRECTORY_STATUS_ISSUED;
        pMYKI_TAProduct[ nDirIndex ]->ZoneLow                   = pData->DynamicData.currentInnerZone - 1;
        pMYKI_TAProduct[ nDirIndex ]->ZoneHigh                  = pData->DynamicData.currentInnerZone;
        pMYKI_TAProduct[ nDirIndex ]->EndDateTime               = pData->DynamicData.currentDateTime + ( 1 /*HOUR*/ * 60 * 60 );
        pMYKI_TAControl->Directory[ nDirIndexOther ].ProductId  = nProductId_EPass;
        pMYKI_TAControl->Directory[ nDirIndexOther ].Status     = TAPP_CONTROL_DIRECTORY_STATUS_ACTIVATED;
        pMYKI_TAProduct[ nDirIndexOther ]->ZoneLow              = pData->DynamicData.currentInnerZone - 1;
        pMYKI_TAProduct[ nDirIndexOther ]->ZoneHigh             = pData->DynamicData.currentInnerZone;
        pMYKI_TAProduct[ nDirIndexOther ]->EndDateTime          = pData->DynamicData.currentDateTime - ( 1 /*HOUR*/ * 60 * 60 );
        pData->Tariff.TPurseMinBalance                          = pMYKI_TAPurseBalance->Balance + 50;
        pData->Tariff.premiumSurcharge                          = 0;
        if ( BR_LLSC_5_2( pData ) != RULE_RESULT_BYPASSED )
        {
            return FALSE;
        }   /* end-of-if */
    }

    return TRUE;
}   /* test_BR_LLSC_5_2_001( ) */

/*==========================================================================*
**
**  test_BR_LLSC_5_2_002
**
**  Description     :
**      Unit-test EXECUTED conditions, no e-Pass product.
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

int test_BR_LLSC_5_2_002( MYKI_BR_ContextData_t *pData )
{
    MYKI_CAControl_t       *pMYKI_CAControl         = NULL;
    MYKI_TAControl_t       *pMYKI_TAControl         = NULL;
    MYKI_TAPurseBalance_t  *pMYKI_TAPurseBalance    = NULL;
    MYKI_TAProduct_t       *pMYKI_TAProduct[ MYKI_DIRECTORY_MAX_LENGTH ]\
                                                    = { NULL };
    U8_t                    nDirIndex               = 3;
    U8_t                    nDirIndexOther          = nDirIndex - 1;
    S32_t                   nTPurseBalance          = 200;

    if ( pData == NULL )
    {
        return FALSE;
    }   /* end-of-if */

    if ( MYKI_CS_CAControlGet( &pMYKI_CAControl ) != MYKI_CS_OK || pMYKI_CAControl == NULL )
    {
        return FALSE;
    }   /* end-of-if */

    if ( MYKI_CS_TAControlGet( &pMYKI_TAControl ) < 0 || pMYKI_TAControl == NULL )
    {
        return FALSE;
    }   /* end-of-if */

    if ( MYKI_CS_TAPurseBalanceGet( &pMYKI_TAPurseBalance ) < 0 || pMYKI_TAPurseBalance == NULL )
    {
        return FALSE;
    }   /* end-of-if */

    if ( MYKI_CS_TAProductGet( (U8_t)( nDirIndex - 1 ), &pMYKI_TAProduct[ nDirIndex ] ) < 0 ||
         pMYKI_TAProduct[ nDirIndex ] == NULL )
    {
        return FALSE;
    }   /* end-of-if */

    if ( MYKI_CS_TAProductGet( (U8_t)( nDirIndexOther - 1 ), &pMYKI_TAProduct[ nDirIndexOther ] ) < 0 ||
         pMYKI_TAProduct[ nDirIndexOther ] == NULL )
    {
        return FALSE;
    }   /* end-of-if */

    pMYKI_TAControl->Status                                     = TAPP_CONTROL_STATUS_ACTIVATED;
    pMYKI_CAControl->Status                                     = CARD_CONTROL_STATUS_ACTIVATED;
    pData->DynamicData.currentInnerZone                         = 2;
    pData->DynamicData.currentZone                              = 4;
    pData->DynamicData.currentOuterZone                         = 6;

    /*  Pre-Conditions:
            TAppTPurseBlance.Balance < Tariff.TPurseMinBalance

        Post-Conditions:
            Executed - insufficient funds
    */
    {
        pMYKI_TAPurseBalance->Balance                           = nTPurseBalance;
        pData->Tariff.TPurseMinBalance                          = pMYKI_TAPurseBalance->Balance + 50;
        if ( BR_LLSC_5_2( pData ) != RULE_RESULT_EXECUTED )
        {
            return FALSE;
        }   /* end-of-if */
    }

    return TRUE;
}   /* test_BR_LLSC_5_2_002( ) */

/*==========================================================================*
**
**  test_BR_LLSC_5_2_003
**
**  Description     :
**      Unit-test EXECUTED conditions, usable active e-Pass but insufficient
**      funds for surcharge.
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

int test_BR_LLSC_5_2_003( MYKI_BR_ContextData_t *pData )
{
    MYKI_CAControl_t       *pMYKI_CAControl         = NULL;
    MYKI_TAControl_t       *pMYKI_TAControl         = NULL;
    MYKI_TAPurseBalance_t  *pMYKI_TAPurseBalance    = NULL;
    MYKI_TAProduct_t       *pMYKI_TAProduct[ MYKI_DIRECTORY_MAX_LENGTH ]\
                                                    = { NULL };
    U8_t                    nDirIndex               = 3;
    U8_t                    nDirIndexOther          = nDirIndex - 1;
    S32_t                   nTPurseBalance          = 200;
    U8_t                    nProductId_EPass        = 2;

    if ( pData == NULL )
    {
        return FALSE;
    }   /* end-of-if */

    if ( MYKI_CS_CAControlGet( &pMYKI_CAControl ) != MYKI_CS_OK || pMYKI_CAControl == NULL )
    {
        return FALSE;
    }   /* end-of-if */

    if ( MYKI_CS_TAControlGet( &pMYKI_TAControl ) < 0 || pMYKI_TAControl == NULL )
    {
        return FALSE;
    }   /* end-of-if */

    if ( MYKI_CS_TAPurseBalanceGet( &pMYKI_TAPurseBalance ) < 0 || pMYKI_TAPurseBalance == NULL )
    {
        return FALSE;
    }   /* end-of-if */

    if ( MYKI_CS_TAProductGet( (U8_t)( nDirIndex - 1 ), &pMYKI_TAProduct[ nDirIndex ] ) < 0 ||
         pMYKI_TAProduct[ nDirIndex ] == NULL )
    {
        return FALSE;
    }   /* end-of-if */

    if ( MYKI_CS_TAProductGet( (U8_t)( nDirIndexOther - 1 ), &pMYKI_TAProduct[ nDirIndexOther ] ) < 0 ||
         pMYKI_TAProduct[ nDirIndexOther ] == NULL )
    {
        return FALSE;
    }   /* end-of-if */

    pMYKI_TAControl->Status                                     = TAPP_CONTROL_STATUS_ACTIVATED;
    pMYKI_CAControl->Status                                     = CARD_CONTROL_STATUS_ACTIVATED;
    pData->DynamicData.currentInnerZone                         = 2;
    pData->DynamicData.currentZone                              = 4;
    pData->DynamicData.currentOuterZone                         = 6;

    /*  Pre-Conditions:
            TAppTPurseBlance.Balance < Tariff.TPurseMinBalance
            TAppControl.Directory[3].Status = "Active"
            TAppControl.Directory[3].ProductId = "e-Pass"
            TAppTProduct.ZoneLow <= Dynamic.CurrentZone.Inner
            TAppTProduct.ZoneHigh => Dynamic.CurrentZone.Inner
            TAppTProduct.EndDateTime > Dynamic.CurrentDateTime
            TAppTPurseBlance.Balance < Tariff.PremiumSurcharge

        Post-Conditions:
            Executed - usable active e-Pass but insufficient funds for surcharge
    */
    {
        pMYKI_TAPurseBalance->Balance                           = nTPurseBalance;
        pMYKI_TAControl->Directory[ nDirIndex ].ProductId       = nProductId_EPass;
        pMYKI_TAControl->Directory[ nDirIndex ].Status          = TAPP_CONTROL_DIRECTORY_STATUS_ACTIVATED;
        pMYKI_TAProduct[ nDirIndex ]->ZoneLow                   = pData->DynamicData.currentZone;
        pMYKI_TAProduct[ nDirIndex ]->ZoneHigh                  = pData->DynamicData.currentZone + 1;
        pMYKI_TAProduct[ nDirIndex ]->EndDateTime               = pData->DynamicData.currentDateTime + ( 1 /*HOUR*/ * 60 * 60 );
        pData->Tariff.TPurseMinBalance                          = pMYKI_TAPurseBalance->Balance + 50;
        pData->Tariff.premiumSurcharge                          = pMYKI_TAPurseBalance->Balance + 5;
        if ( BR_LLSC_5_2( pData ) != RULE_RESULT_EXECUTED )
        {
            return FALSE;
        }   /* end-of-if */
    }

    return TRUE;
}   /* test_BR_LLSC_5_2_003( ) */

/*==========================================================================*
**
**  test_BR_LLSC_5_2_004
**
**  Description     :
**      Unit-test EXECUTED conditions, usable inactive e-Pass but there
**      exists an unusable active e-Pass.
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

int test_BR_LLSC_5_2_004( MYKI_BR_ContextData_t *pData )
{
    MYKI_CAControl_t       *pMYKI_CAControl         = NULL;
    MYKI_TAControl_t       *pMYKI_TAControl         = NULL;
    MYKI_TAPurseBalance_t  *pMYKI_TAPurseBalance    = NULL;
    MYKI_TAProduct_t       *pMYKI_TAProduct[ MYKI_DIRECTORY_MAX_LENGTH ]\
                                                    = { NULL };
    U8_t                    nDirIndex               = 3;
    U8_t                    nDirIndexOther          = nDirIndex - 1;
    S32_t                   nTPurseBalance          = 200;
    U8_t                    nProductId_EPass        = 2;

    if ( pData == NULL )
    {
        return FALSE;
    }   /* end-of-if */

    if ( MYKI_CS_CAControlGet( &pMYKI_CAControl ) != MYKI_CS_OK || pMYKI_CAControl == NULL )
    {
        return FALSE;
    }   /* end-of-if */

    if ( MYKI_CS_TAControlGet( &pMYKI_TAControl ) < 0 || pMYKI_TAControl == NULL )
    {
        return FALSE;
    }   /* end-of-if */

    if ( MYKI_CS_TAPurseBalanceGet( &pMYKI_TAPurseBalance ) < 0 || pMYKI_TAPurseBalance == NULL )
    {
        return FALSE;
    }   /* end-of-if */

    if ( MYKI_CS_TAProductGet( (U8_t)( nDirIndex - 1 ), &pMYKI_TAProduct[ nDirIndex ] ) < 0 ||
         pMYKI_TAProduct[ nDirIndex ] == NULL )
    {
        return FALSE;
    }   /* end-of-if */

    if ( MYKI_CS_TAProductGet( (U8_t)( nDirIndexOther - 1 ), &pMYKI_TAProduct[ nDirIndexOther ] ) < 0 ||
         pMYKI_TAProduct[ nDirIndexOther ] == NULL )
    {
        return FALSE;
    }   /* end-of-if */

    pMYKI_TAControl->Status                                     = TAPP_CONTROL_STATUS_ACTIVATED;
    pMYKI_CAControl->Status                                     = CARD_CONTROL_STATUS_ACTIVATED;
    pData->DynamicData.currentInnerZone                         = 2;
    pData->DynamicData.currentZone                              = 4;
    pData->DynamicData.currentOuterZone                         = 6;

    /*  Pre-Conditions:
            TAppTPurseBlance.Balance < Tariff.TPurseMinBalance
            TAppControl.Directory[2].Status = "Active"
            TAppControl.Directory[2].ProductId = "e-Pass" (unusable)
            TAppControl.Directory[3].Status = "Inactive"
            TAppControl.Directory[3].ProductId = "e-Pass"
            TAppTProduct.ZoneLow <= Dynamic.CurrentZone.Inner
            TAppTProduct.ZoneHigh => Dynamic.CurrentZone.Inner
            TAppTProduct.EndDateTime > Dynamic.CurrentDateTime
            Tariff.PremiumSurcharge = 0

        Post-Conditions:
            Executed - usable inactive e-Pass but there exists an unusable active e-Pass
    */
    {
        pMYKI_TAPurseBalance->Balance                           = nTPurseBalance;
        pMYKI_TAControl->Directory[ nDirIndex ].ProductId       = nProductId_EPass;
        pMYKI_TAControl->Directory[ nDirIndex ].Status          = TAPP_CONTROL_DIRECTORY_STATUS_ISSUED;
        pMYKI_TAProduct[ nDirIndex ]->ZoneLow                   = pData->DynamicData.currentZone;
        pMYKI_TAProduct[ nDirIndex ]->ZoneHigh                  = pData->DynamicData.currentZone + 1;
        pMYKI_TAProduct[ nDirIndex ]->EndDateTime               = pData->DynamicData.currentDateTime + ( 1 /*HOUR*/ * 60 * 60 );
        pMYKI_TAControl->Directory[ nDirIndexOther ].ProductId  = nProductId_EPass;
        pMYKI_TAControl->Directory[ nDirIndexOther ].Status     = TAPP_CONTROL_DIRECTORY_STATUS_ACTIVATED;
        pMYKI_TAProduct[ nDirIndexOther ]->ZoneLow              = pData->DynamicData.currentInnerZone - 1;
        pMYKI_TAProduct[ nDirIndexOther ]->ZoneHigh             = pData->DynamicData.currentInnerZone - 1;
        pMYKI_TAProduct[ nDirIndexOther ]->EndDateTime          = pData->DynamicData.currentDateTime + ( 1 /*HOUR*/ * 60 * 60 );
        pData->Tariff.TPurseMinBalance                          = pMYKI_TAPurseBalance->Balance + 50;
        pData->Tariff.premiumSurcharge                          = 0;
        if ( BR_LLSC_5_2( pData ) != RULE_RESULT_EXECUTED )
        {
            return FALSE;
        }   /* end-of-if */
    }

    return TRUE;
}   /* test_BR_LLSC_5_2_004( ) */
