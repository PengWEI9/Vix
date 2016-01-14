/* -*- mode: c; tabs: 4; -*- */
/*=============================================================================
**
**    Vix Technology                   Licensed software
**    (C) 2013                         All rights reserved
**
**=============================================================================
**
**  Project/Product : MBU
**  Filename        : test_BR_LLSC_0_1.c
**  Author(s)       : ?
**
**  Description     :
**      Implements BR_LLSC_0_1 business rule unit-tests.
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
**    1.00  dd.mm.yy    ?     Create
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
#include <myki_br.h>
#include <myki_br_rules.h>
#include <myki_br_context_data.h>

#include "test_common.h"

static
RuleResult_e
BR_LLSC_0_1( MYKI_BR_ContextData_t *pData )
{
    RuleResult_e    RuleResult  = BR_LLSC_0_1a( pData );
    if ( RuleResult == RULE_RESULT_EXECUTED )
    {
        RuleResult  = BR_LLSC_0_1b( pData );
    }
    return  RuleResult;
}

/*==========================================================================*
**
**  test_BR_LLSC_0_1_001
**
**  Description     :
**      Unit-test BYPASSED condition.
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

int     test_BR_LLSC_0_1_001( MYKI_BR_ContextData_t *pData )
{
    RuleResult_e                                RuleResult  = RULE_RESULT_ERROR;

    /*  GIVEN */
    {
        /*  Card image */
        {
            pData->DynamicData.isTransit    = FALSE;
        }
    }

    /*  WHEN */
    {
        /*  Execute business rule */
        RuleResult  = BR_LLSC_0_1( pData );
    }

    /*  THEN */
    {
        UT_Start( );

        /*  Make sure business rule is bypassed */
        UT_Assert( RuleResult == RULE_RESULT_BYPASSED );

        return  UT_Result( );
    }
}

/*==========================================================================*
**
**  test_BR_LLSC_0_1_002
**
**  Description     :
**      Unit-test EXECUTED condition.
**      Activated product with
**      TAProduct.StartDateTime > TAProduct.EndDateTime.
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

int     test_BR_LLSC_0_1_002( MYKI_BR_ContextData_t *pData )
{
    CardImage_t                                 CardImage;
    RuleResult_e                                RuleResult      = RULE_RESULT_ERROR;

    /*  GIVEN */
    {
        /*  Card image */
        {
            pData->DynamicData.isTransit                        = TRUE;
            pData->Tariff.TPurseMinBalance                      = 100;
            pData->Tariff.TPurseMaximumBalance                  = 10000;

            CreateCardImage_Empty( &CardImage );

            CardImage.pMYKI_TAControl->Directory[ 0 ].Status    = TAPP_CONTROL_DIRECTORY_STATUS_ACTIVATED;
            CardImage.pMYKI_TAPurseBalance->Balance             = 1000;
            CardImage.pMYKI_TAPurseControl->LastChangeNewBalance= CardImage.pMYKI_TAPurseBalance->Balance;
            CardImage.pMYKI_TAPurseControl->NextTxSeqNo         = CardImage.pMYKI_TAPurseControl->LastChangeTxSeqNo + 1;

            /*  Activated product with TAProduct.StartDateTime > TAProduct.EndDateTime */
            CardImage.pMYKI_TAControl->Directory[ 1 ].SerialNo  = 1;
            CardImage.pMYKI_TAControl->Directory[ 1 ].Status    = TAPP_CONTROL_DIRECTORY_STATUS_ACTIVATED;
            CardImage.pMYKI_TAControl->Directory[ 1 ].IssuerId  = 0;
            CardImage.pMYKI_TAControl->Directory[ 1 ].ProductId = PRODUCT_ID_SINGLE;
            CardImage.pMYKI_TAProduct[ 0 ]->StartDateTime       = pData->DynamicData.currentDateTime;
            CardImage.pMYKI_TAProduct[ 0 ]->EndDateTime         = CardImage.pMYKI_TAProduct[ 0 ]->StartDateTime - 1;
        }
    }

    /*  WHEN */
    {
        /*  Execute business rule */
        RuleResult  = BR_LLSC_0_1( pData );
    }

    /*  THEN */
    {
        UT_Start( );

        /*  Make sure business rule is executed */
        UT_Assert( RuleResult == RULE_RESULT_EXECUTED );

        /*  Make sure card image snapshot created */
        UT_Assert( pData->InternalData.cardSnapshotSequenceNumber == 1 );

        /*  Make sure product invalidated */
        UT_Assert( CardImage.pMYKI_TAControl->Directory[ 1 ].Status == TAPP_CONTROL_DIRECTORY_STATUS_UNUSED );

        return  UT_Result( );
    }
}   /*  test_BR_LLSC_0_1_002( ) */

/*==========================================================================*
**
**  test_BR_LLSC_0_1_003
**
**  Description     :
**      Unit-test EXECUTED condition.
**      SINGLE TRIP and DAILY product with
**      TAProduct.EndDateTime > (TAProduct.StartDateTime + 24 Hours).
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

int     test_BR_LLSC_0_1_003( MYKI_BR_ContextData_t *pData )
{
    CardImage_t                                 CardImage;
    RuleResult_e                                RuleResult      = RULE_RESULT_ERROR;

    /*  GIVEN */
    {
        /*  Card image */
        {
            pData->DynamicData.isTransit                        = TRUE;
            pData->Tariff.TPurseMinBalance                      = 100;
            pData->Tariff.TPurseMaximumBalance                  = 10000;

            CreateCardImage_Empty( &CardImage );

            CardImage.pMYKI_TAControl->Directory[ 0 ].Status    = TAPP_CONTROL_DIRECTORY_STATUS_ACTIVATED;
            CardImage.pMYKI_TAPurseBalance->Balance             = 1000;
            CardImage.pMYKI_TAPurseControl->LastChangeNewBalance= CardImage.pMYKI_TAPurseBalance->Balance;
            CardImage.pMYKI_TAPurseControl->NextTxSeqNo         = CardImage.pMYKI_TAPurseControl->LastChangeTxSeqNo + 1;

            /*  DAILY product with TAProduct.EndDateTime > (TAProduct.StartDateTime + 24 Hours) */
            CardImage.pMYKI_TAControl->Directory[ 1 ].SerialNo  = 1;
            CardImage.pMYKI_TAControl->Directory[ 1 ].Status    = TAPP_CONTROL_DIRECTORY_STATUS_ACTIVATED;
            CardImage.pMYKI_TAControl->Directory[ 1 ].IssuerId  = 0;
            CardImage.pMYKI_TAControl->Directory[ 1 ].ProductId = PRODUCT_ID_DAILY;
            CardImage.pMYKI_TAProduct[ 0 ]->StartDateTime       = pData->DynamicData.currentDateTime;
            CardImage.pMYKI_TAProduct[ 0 ]->EndDateTime         = CardImage.pMYKI_TAProduct[ 0 ]->StartDateTime + HOURS_MINUTES_TO_SECONDS( 24, 1 );

            /*  SINGLE TRIP product with TAProduct.EndDateTime > (TAProduct.StartDateTime + 24 Hours) */
            CardImage.pMYKI_TAControl->Directory[ 2 ].SerialNo  = 2;
            CardImage.pMYKI_TAControl->Directory[ 2 ].Status    = TAPP_CONTROL_DIRECTORY_STATUS_ACTIVATED;
            CardImage.pMYKI_TAControl->Directory[ 2 ].IssuerId  = 0;
            CardImage.pMYKI_TAControl->Directory[ 2 ].ProductId = PRODUCT_ID_SINGLE;
            CardImage.pMYKI_TAProduct[ 1 ]->StartDateTime       = pData->DynamicData.currentDateTime;
            CardImage.pMYKI_TAProduct[ 1 ]->EndDateTime         = CardImage.pMYKI_TAProduct[ 1 ]->StartDateTime + HOURS_MINUTES_TO_SECONDS( 24, 1 );
        }
    }

    /*  WHEN */
    {
        /*  Execute business rule */
        RuleResult  = BR_LLSC_0_1( pData );
    }

    /*  THEN */
    {
        UT_Start( );

        /*  Make sure business rule is executed */
        UT_Assert( RuleResult == RULE_RESULT_EXECUTED );

        /*  Make sure card image snapshots created */
        UT_Assert( pData->InternalData.cardSnapshotSequenceNumber == 2 );

        /*  Make sure product invalidated */
        UT_Assert( CardImage.pMYKI_TAControl->Directory[ 1 ].Status == TAPP_CONTROL_DIRECTORY_STATUS_UNUSED );
        UT_Assert( CardImage.pMYKI_TAControl->Directory[ 2 ].Status == TAPP_CONTROL_DIRECTORY_STATUS_UNUSED );

        return  UT_Result( );
    }

    pData->DynamicData.isTransit = TRUE;

    CardImage2( pData );

    if ( BR_LLSC_0_1( pData ) != RULE_RESULT_EXECUTED )
        return FALSE;

    return TRUE;
}   /*  test_BR_LLSC_0_1_003( ) */

/*==========================================================================*
**
**  test_BR_LLSC_0_1_004
**
**  Description     :
**      Unit-test EXECUTED condition.
**      WEEKLY product with
**      TAProduct.EndDateTime > (TAProduct.StartDateTime + 8 Days).
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

int     test_BR_LLSC_0_1_004( MYKI_BR_ContextData_t *pData )
{
    CardImage_t                                 CardImage;
    RuleResult_e                                RuleResult      = RULE_RESULT_ERROR;

    /*  GIVEN */
    {
        /*  Card image */
        {
            pData->DynamicData.isTransit                        = TRUE;
            pData->Tariff.TPurseMinBalance                      = 100;
            pData->Tariff.TPurseMaximumBalance                  = 10000;

            CreateCardImage_Empty( &CardImage );

            CardImage.pMYKI_TAControl->Directory[ 0 ].Status    = TAPP_CONTROL_DIRECTORY_STATUS_ACTIVATED;
            CardImage.pMYKI_TAPurseBalance->Balance             = 1000;
            CardImage.pMYKI_TAPurseControl->LastChangeNewBalance= CardImage.pMYKI_TAPurseBalance->Balance;
            CardImage.pMYKI_TAPurseControl->NextTxSeqNo         = CardImage.pMYKI_TAPurseControl->LastChangeTxSeqNo + 1;

            /*  WEEKLY product with TAProduct.EndDateTime > (TAProduct.StartDateTime + 8 Days) */
            CardImage.pMYKI_TAControl->Directory[ 1 ].SerialNo  = 1;
            CardImage.pMYKI_TAControl->Directory[ 1 ].Status    = TAPP_CONTROL_DIRECTORY_STATUS_ACTIVATED;
            CardImage.pMYKI_TAControl->Directory[ 1 ].IssuerId  = 0;
            CardImage.pMYKI_TAControl->Directory[ 1 ].ProductId = PRODUCT_ID_WEEKLY;
            CardImage.pMYKI_TAProduct[ 0 ]->StartDateTime       = pData->DynamicData.currentDateTime;
            CardImage.pMYKI_TAProduct[ 0 ]->EndDateTime         = CardImage.pMYKI_TAProduct[ 0 ]->StartDateTime + DAYS_HOURS_MINUTES_TO_SECONDS( 8, 0, 1 );
        }
    }

    /*  WHEN */
    {
        /*  Execute business rule */
        RuleResult  = BR_LLSC_0_1( pData );
    }

    /*  THEN */
    {
        UT_Start( );

        /*  Make sure business rule is executed */
        UT_Assert( RuleResult == RULE_RESULT_EXECUTED );

        /*  Make sure card image snapshot created */
        UT_Assert( pData->InternalData.cardSnapshotSequenceNumber == 1 );

        /*  Make sure product invalidated */
        UT_Assert( CardImage.pMYKI_TAControl->Directory[ 1 ].Status == TAPP_CONTROL_DIRECTORY_STATUS_UNUSED );

        return  UT_Result( );
    }
}   /*  test_BR_LLSC_0_1_004( ) */

/*==========================================================================*
**
**  test_BR_LLSC_0_1_005
**
**  Description     :
**      Unit-test EXECUTED condition.
**      EPASS product with
**      TDirectory.ProductId = 2
**      TAProduct.InstanceCount != 0
**      TAProduct.InstanceCount > 1825 OR
**      TAProduct.EndDateTime > (TAProduct.StartDateTime + TAProduct.InstanceCount + 1 Day)
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

int     test_BR_LLSC_0_1_005( MYKI_BR_ContextData_t *pData )
{
    CardImage_t                                 CardImage;
    RuleResult_e                                RuleResult      = RULE_RESULT_ERROR;

    /*  GIVEN */
    {
        /*  Card image */
        {
            pData->DynamicData.isTransit                        = TRUE;
            pData->Tariff.TPurseMinBalance                      = 100;
            pData->Tariff.TPurseMaximumBalance                  = 10000;

            CreateCardImage_Empty( &CardImage );

            CardImage.pMYKI_TAControl->Directory[ 0 ].Status    = TAPP_CONTROL_DIRECTORY_STATUS_ACTIVATED;
            CardImage.pMYKI_TAPurseBalance->Balance             = 1000;
            CardImage.pMYKI_TAPurseControl->LastChangeNewBalance= CardImage.pMYKI_TAPurseBalance->Balance;
            CardImage.pMYKI_TAPurseControl->NextTxSeqNo         = CardImage.pMYKI_TAPurseControl->LastChangeTxSeqNo + 1;

            /*  EPASS product with TAProduct.EndDateTime > (TAProduct.StartDateTime + TAProduct.InstanceCount + 1 Day) */
            CardImage.pMYKI_TAControl->Directory[ 1 ].SerialNo  = 1;
            CardImage.pMYKI_TAControl->Directory[ 1 ].Status    = TAPP_CONTROL_DIRECTORY_STATUS_ACTIVATED;
            CardImage.pMYKI_TAControl->Directory[ 1 ].IssuerId  = 0;
            CardImage.pMYKI_TAControl->Directory[ 1 ].ProductId = PRODUCT_ID_EPASS;
            CardImage.pMYKI_TAProduct[ 0 ]->InstanceCount       = 30 /*DAYS*/;
            CardImage.pMYKI_TAProduct[ 0 ]->StartDateTime       = pData->DynamicData.currentDateTime;
            CardImage.pMYKI_TAProduct[ 0 ]->EndDateTime         = CardImage.pMYKI_TAProduct[ 0 ]->StartDateTime + DAYS_TO_SECONDS( 30 + 1 + 1 );

            /*  EPASS product with TAProduct.InstanceCount > 1825 */
            CardImage.pMYKI_TAControl->Directory[ 2 ].SerialNo  = 2;
            CardImage.pMYKI_TAControl->Directory[ 2 ].Status    = TAPP_CONTROL_DIRECTORY_STATUS_ACTIVATED;
            CardImage.pMYKI_TAControl->Directory[ 2 ].IssuerId  = 0;
            CardImage.pMYKI_TAControl->Directory[ 2 ].ProductId = PRODUCT_ID_EPASS;
            CardImage.pMYKI_TAProduct[ 1 ]->InstanceCount       = ( 1825 + 1 ) /*DAYS*/;
            CardImage.pMYKI_TAProduct[ 1 ]->StartDateTime       = pData->DynamicData.currentDateTime;
            CardImage.pMYKI_TAProduct[ 1 ]->EndDateTime         = CardImage.pMYKI_TAProduct[ 1 ]->StartDateTime + DAYS_TO_SECONDS( 30 );
        }
    }

    /*  WHEN */
    {
        /*  Execute business rule */
        RuleResult  = BR_LLSC_0_1( pData );
    }

    /*  THEN */
    {
        UT_Start( );

        /*  Make sure business rule is executed */
        UT_Assert( RuleResult == RULE_RESULT_EXECUTED );

        /*  Make sure card image snapshots created */
        UT_Assert( pData->InternalData.cardSnapshotSequenceNumber == 2 );

        /*  Make sure product unchanged */
        UT_Assert( CardImage.pMYKI_TAControl->Directory[ 1 ].Status == TAPP_CONTROL_DIRECTORY_STATUS_ACTIVATED );
        UT_Assert( CardImage.pMYKI_TAControl->Directory[ 2 ].Status == TAPP_CONTROL_DIRECTORY_STATUS_ACTIVATED );

        return  UT_Result( );
    }
}   /*  test_BR_LLSC_0_1_005( ) */

/*==========================================================================*
**
**  test_BR_LLSC_0_1_006
**
**  Description     :
**      Unit-test EXECUTED condition.
**      N-HOUR product with
**      TAProduct.EndDateTime > (TAProduct.StartDateTime + 1 Day).
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

int     test_BR_LLSC_0_1_006( MYKI_BR_ContextData_t *pData )
{
    CardImage_t                                 CardImage;
    RuleResult_e                                RuleResult      = RULE_RESULT_ERROR;

    /*  GIVEN */
    {
        /*  Card image */
        {
            pData->DynamicData.isTransit                        = TRUE;
            pData->Tariff.TPurseMinBalance                      = 100;
            pData->Tariff.TPurseMaximumBalance                  = 10000;

            CreateCardImage_Empty( &CardImage );

            CardImage.pMYKI_TAControl->Directory[ 0 ].Status    = TAPP_CONTROL_DIRECTORY_STATUS_ACTIVATED;
            CardImage.pMYKI_TAPurseBalance->Balance             = 1000;
            CardImage.pMYKI_TAPurseControl->LastChangeNewBalance= CardImage.pMYKI_TAPurseBalance->Balance;
            CardImage.pMYKI_TAPurseControl->NextTxSeqNo         = CardImage.pMYKI_TAPurseControl->LastChangeTxSeqNo + 1;

            /*  N-HOUR product with TAProduct.EndDateTime > (TAProduct.StartDateTime + 1 Day) */
            CardImage.pMYKI_TAControl->Directory[ 1 ].SerialNo  = 1;
            CardImage.pMYKI_TAControl->Directory[ 1 ].Status    = TAPP_CONTROL_DIRECTORY_STATUS_ACTIVATED;
            CardImage.pMYKI_TAControl->Directory[ 1 ].IssuerId  = 0;
            CardImage.pMYKI_TAControl->Directory[ 1 ].ProductId = PRODUCT_ID_NHOUR;
            CardImage.pMYKI_TAProduct[ 0 ]->StartDateTime       = pData->DynamicData.currentDateTime;
            CardImage.pMYKI_TAProduct[ 0 ]->EndDateTime         = CardImage.pMYKI_TAProduct[ 0 ]->StartDateTime + HOURS_MINUTES_TO_SECONDS( 24, 1 );
        }
    }

    /*  WHEN */
    {
        /*  Execute business rule */
        RuleResult  = BR_LLSC_0_1( pData );
    }

    /*  THEN */
    {
        UT_Start( );

        /*  Make sure business rule is executed */
        UT_Assert( RuleResult == RULE_RESULT_EXECUTED );

        /*  Make sure card image snapshot created */
        UT_Assert( pData->InternalData.cardSnapshotSequenceNumber == 1 );

        /*  Make sure product invalidated */
        UT_Assert( CardImage.pMYKI_TAControl->Directory[ 1 ].Status == TAPP_CONTROL_DIRECTORY_STATUS_UNUSED );

        return  UT_Result( );
    }
}   /*  test_BR_LLSC_0_1_006( ) */

/*==========================================================================*
**
**  test_BR_LLSC_0_1_007
**
**  Description     :
**      Unit-test EXECUTED condition.
**      Issued product of type other than EPASS.
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

int     test_BR_LLSC_0_1_007( MYKI_BR_ContextData_t *pData )
{
    CardImage_t                                 CardImage;
    RuleResult_e                                RuleResult      = RULE_RESULT_ERROR;

    /*  GIVEN */
    {
        /*  Card image */
        {
            pData->DynamicData.isTransit                        = TRUE;
            pData->Tariff.TPurseMinBalance                      = 100;
            pData->Tariff.TPurseMaximumBalance                  = 10000;

            CreateCardImage_Empty( &CardImage );

            CardImage.pMYKI_TAControl->Directory[ 0 ].Status    = TAPP_CONTROL_DIRECTORY_STATUS_ACTIVATED;
            CardImage.pMYKI_TAPurseBalance->Balance             = 1000;
            CardImage.pMYKI_TAPurseControl->LastChangeNewBalance= CardImage.pMYKI_TAPurseBalance->Balance;
            CardImage.pMYKI_TAPurseControl->NextTxSeqNo         = CardImage.pMYKI_TAPurseControl->LastChangeTxSeqNo + 1;

            /*  Issued product of type other than EPASS */
            CardImage.pMYKI_TAControl->Directory[ 1 ].SerialNo  = 1;
            CardImage.pMYKI_TAControl->Directory[ 1 ].Status    = TAPP_CONTROL_DIRECTORY_STATUS_ISSUED;
            CardImage.pMYKI_TAControl->Directory[ 1 ].IssuerId  = 0;
            CardImage.pMYKI_TAControl->Directory[ 1 ].ProductId = PRODUCT_ID_NHOUR;
            CardImage.pMYKI_TAProduct[ 0 ]->StartDateTime       = pData->DynamicData.currentDateTime;
            CardImage.pMYKI_TAProduct[ 0 ]->EndDateTime         = CardImage.pMYKI_TAProduct[ 0 ]->StartDateTime + HOURS_TO_SECONDS( 2 );

            /*  Issued EPASS - OK */
            CardImage.pMYKI_TAControl->Directory[ 2 ].SerialNo  = 2;
            CardImage.pMYKI_TAControl->Directory[ 2 ].Status    = TAPP_CONTROL_DIRECTORY_STATUS_ISSUED;
            CardImage.pMYKI_TAControl->Directory[ 2 ].IssuerId  = 0;
            CardImage.pMYKI_TAControl->Directory[ 2 ].ProductId = PRODUCT_ID_EPASS;
            CardImage.pMYKI_TAProduct[ 1 ]->InstanceCount       = 1 /*DAY*/;
            CardImage.pMYKI_TAProduct[ 1 ]->StartDateTime       = pData->DynamicData.currentDateTime;
            CardImage.pMYKI_TAProduct[ 1 ]->EndDateTime         = CardImage.pMYKI_TAProduct[ 1 ]->StartDateTime + DAYS_TO_SECONDS( 365 );
        }
    }

    /*  WHEN */
    {
        /*  Execute business rule */
        RuleResult  = BR_LLSC_0_1( pData );
    }

    /*  THEN */
    {
        UT_Start( );

        /*  Make sure business rule is executed */
        UT_Assert( RuleResult == RULE_RESULT_EXECUTED );

        /*  Make sure card image snapshot created */
        UT_Assert( pData->InternalData.cardSnapshotSequenceNumber == 1 );

        /*  Make sure product unchanged */
        UT_Assert( CardImage.pMYKI_TAControl->Directory[ 1 ].Status == TAPP_CONTROL_DIRECTORY_STATUS_ISSUED );
        UT_Assert( CardImage.pMYKI_TAControl->Directory[ 2 ].Status == TAPP_CONTROL_DIRECTORY_STATUS_ISSUED );

        return  UT_Result( );
    }
}   /*  test_BR_LLSC_0_1_007( ) */

/*==========================================================================*
**
**  test_BR_LLSC_0_1_008
**
**  Description     :
**      Unit-test EXECUTED condition.
**      Issued EPASS product with
**      TAProduct.InstanceCount > 1825 OR
**      TAProduct.EndDateTime < TAProduct.StartDateTime
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

int     test_BR_LLSC_0_1_008( MYKI_BR_ContextData_t *pData )
{
    CardImage_t                                 CardImage;
    RuleResult_e                                RuleResult      = RULE_RESULT_ERROR;

    /*  GIVEN */
    {
        /*  Card image */
        {
            pData->DynamicData.isTransit                        = TRUE;
            pData->Tariff.TPurseMinBalance                      = 100;
            pData->Tariff.TPurseMaximumBalance                  = 10000;

            CreateCardImage_Empty( &CardImage );

            CardImage.pMYKI_TAControl->Directory[ 0 ].Status    = TAPP_CONTROL_DIRECTORY_STATUS_ACTIVATED;
            CardImage.pMYKI_TAPurseBalance->Balance             = 1000;
            CardImage.pMYKI_TAPurseControl->LastChangeNewBalance= CardImage.pMYKI_TAPurseBalance->Balance;
            CardImage.pMYKI_TAPurseControl->NextTxSeqNo         = CardImage.pMYKI_TAPurseControl->LastChangeTxSeqNo + 1;

            /*  Issued EPASS product with TAProduct.EndDateTime < TAProduct.StartDateTime */
            CardImage.pMYKI_TAControl->Directory[ 1 ].SerialNo  = 1;
            CardImage.pMYKI_TAControl->Directory[ 1 ].Status    = TAPP_CONTROL_DIRECTORY_STATUS_ISSUED;
            CardImage.pMYKI_TAControl->Directory[ 1 ].IssuerId  = 0;
            CardImage.pMYKI_TAControl->Directory[ 1 ].ProductId = PRODUCT_ID_EPASS;
            CardImage.pMYKI_TAProduct[ 0 ]->InstanceCount       = 1 /*DAY*/;
            CardImage.pMYKI_TAProduct[ 0 ]->StartDateTime       = pData->DynamicData.currentDateTime;
            CardImage.pMYKI_TAProduct[ 0 ]->EndDateTime         = CardImage.pMYKI_TAProduct[ 0 ]->StartDateTime - 1;

            /*  Issued EPASS product with TAProduct.InstanceCount > 1825 */
            CardImage.pMYKI_TAControl->Directory[ 2 ].SerialNo  = 2;
            CardImage.pMYKI_TAControl->Directory[ 2 ].Status    = TAPP_CONTROL_DIRECTORY_STATUS_ISSUED;
            CardImage.pMYKI_TAControl->Directory[ 2 ].IssuerId  = 0;
            CardImage.pMYKI_TAControl->Directory[ 2 ].ProductId = PRODUCT_ID_EPASS;
            CardImage.pMYKI_TAProduct[ 1 ]->InstanceCount       = ( 1825 + 1 ) /*DAYS*/;
            CardImage.pMYKI_TAProduct[ 1 ]->StartDateTime       = pData->DynamicData.currentDateTime;
            CardImage.pMYKI_TAProduct[ 1 ]->EndDateTime         = CardImage.pMYKI_TAProduct[ 1 ]->StartDateTime + DAYS_TO_SECONDS( 365 );
        }
    }

    /*  WHEN */
    {
        /*  Execute business rule */
        RuleResult  = BR_LLSC_0_1( pData );
    }

    /*  THEN */
    {
        UT_Start( );

        /*  Make sure business rule is executed */
        UT_Assert( RuleResult == RULE_RESULT_EXECUTED );

        /*  Make sure card image snapshot created */
        UT_Assert( pData->InternalData.cardSnapshotSequenceNumber == 2 );

        /*  Make sure product unchanged */
        UT_Assert( CardImage.pMYKI_TAControl->Directory[ 1 ].Status == TAPP_CONTROL_DIRECTORY_STATUS_ISSUED );
        UT_Assert( CardImage.pMYKI_TAControl->Directory[ 2 ].Status == TAPP_CONTROL_DIRECTORY_STATUS_ISSUED );

        return  UT_Result( );
    }
}   /*  test_BR_LLSC_0_1_008( ) */

/*==========================================================================*
**
**  test_BR_LLSC_0_1_009
**
**  Description     :
**      Unit-test EXECUTED condition.
**      Product of type other than N-HOUR has Provisional bit set OR
**      Not in used provisional N-HOUR product.
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

int     test_BR_LLSC_0_1_009( MYKI_BR_ContextData_t *pData )
{
    CardImage_t                                 CardImage;
    RuleResult_e                                RuleResult      = RULE_RESULT_ERROR;

    /*  GIVEN */
    {
        /*  Card image */
        {
            pData->DynamicData.isTransit                        = TRUE;
            pData->Tariff.TPurseMinBalance                      = 100;
            pData->Tariff.TPurseMaximumBalance                  = 10000;

            CreateCardImage_Empty( &CardImage );

            CardImage.pMYKI_TAControl->Directory[ 0 ].Status    = TAPP_CONTROL_DIRECTORY_STATUS_ACTIVATED;
            CardImage.pMYKI_TAPurseBalance->Balance             = 1000;
            CardImage.pMYKI_TAPurseControl->LastChangeNewBalance= CardImage.pMYKI_TAPurseBalance->Balance;
            CardImage.pMYKI_TAPurseControl->NextTxSeqNo         = CardImage.pMYKI_TAPurseControl->LastChangeTxSeqNo + 1;

            /*  Product of type other than N-HOUR has Provisional bit set */
            CardImage.pMYKI_TAControl->Directory[ 1 ].SerialNo  = 1;
            CardImage.pMYKI_TAControl->Directory[ 1 ].Status    = TAPP_CONTROL_DIRECTORY_STATUS_ACTIVATED;
            CardImage.pMYKI_TAControl->Directory[ 1 ].IssuerId  = 0;
            CardImage.pMYKI_TAControl->Directory[ 1 ].ProductId = PRODUCT_ID_DAILY;
            CardImage.pMYKI_TAProduct[ 0 ]->ControlBitmap      |= TAPP_TPRODUCT_CONTROL_PROVISIONAL_BITMAP;
            CardImage.pMYKI_TAProduct[ 0 ]->StartDateTime       = pData->DynamicData.currentDateTime;
            CardImage.pMYKI_TAProduct[ 0 ]->EndDateTime         = CardImage.pMYKI_TAProduct[ 0 ]->StartDateTime + DAYS_TO_SECONDS( 1 );

            /*  In used provisional N-HOUR product - OK */
            CardImage.pMYKI_TAControl->ProductInUse             = 2;
            CardImage.pMYKI_TAControl->Directory[ 2 ].SerialNo  = 2;
            CardImage.pMYKI_TAControl->Directory[ 2 ].Status    = TAPP_CONTROL_DIRECTORY_STATUS_ACTIVATED;
            CardImage.pMYKI_TAControl->Directory[ 2 ].IssuerId  = 0;
            CardImage.pMYKI_TAControl->Directory[ 2 ].ProductId = PRODUCT_ID_NHOUR;
            CardImage.pMYKI_TAProduct[ 1 ]->ControlBitmap      |= TAPP_TPRODUCT_CONTROL_PROVISIONAL_BITMAP;
            CardImage.pMYKI_TAProduct[ 1 ]->StartDateTime       = pData->DynamicData.currentDateTime;
            CardImage.pMYKI_TAProduct[ 1 ]->EndDateTime         = CardImage.pMYKI_TAProduct[ 1 ]->StartDateTime + HOURS_TO_SECONDS( 3 );

            /*  Not in used provisional N-HOUR product */
            CardImage.pMYKI_TAControl->Directory[ 3 ].SerialNo  = 3;
            CardImage.pMYKI_TAControl->Directory[ 3 ].Status    = TAPP_CONTROL_DIRECTORY_STATUS_ACTIVATED;
            CardImage.pMYKI_TAControl->Directory[ 3 ].IssuerId  = 0;
            CardImage.pMYKI_TAControl->Directory[ 3 ].ProductId = PRODUCT_ID_NHOUR;
            CardImage.pMYKI_TAProduct[ 2 ]->ControlBitmap      |= TAPP_TPRODUCT_CONTROL_PROVISIONAL_BITMAP;
            CardImage.pMYKI_TAProduct[ 2 ]->StartDateTime       = pData->DynamicData.currentDateTime;
            CardImage.pMYKI_TAProduct[ 2 ]->EndDateTime         = CardImage.pMYKI_TAProduct[ 2 ]->StartDateTime + HOURS_TO_SECONDS( 3 );
        }
    }

    /*  WHEN */
    {
        /*  Execute business rule */
        RuleResult  = BR_LLSC_0_1( pData );
    }

    /*  THEN */
    {
        UT_Start( );

        /*  Make sure business rule is executed */
        UT_Assert( RuleResult == RULE_RESULT_EXECUTED );

        /*  Make sure card image snapshot created */
        UT_Assert( pData->InternalData.cardSnapshotSequenceNumber == 2 );

        /*  Make sure product invalidated */
        UT_Assert( CardImage.pMYKI_TAControl->Directory[ 1 ].Status == TAPP_CONTROL_DIRECTORY_STATUS_UNUSED );
        UT_Assert( CardImage.pMYKI_TAControl->Directory[ 2 ].Status == TAPP_CONTROL_DIRECTORY_STATUS_ACTIVATED );
        UT_Assert( CardImage.pMYKI_TAControl->Directory[ 3 ].Status == TAPP_CONTROL_DIRECTORY_STATUS_UNUSED );

        return  UT_Result( );
    }
}   /*  test_BR_LLSC_0_1_009( ) */

/*==========================================================================*
**
**  test_BR_LLSC_0_1_010
**
**  Description     :
**      Unit-test EXECUTED condition.
**      TPurse balance < Minimum.
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

int     test_BR_LLSC_0_1_010( MYKI_BR_ContextData_t *pData )
{
    CardImage_t                                 CardImage;
    RuleResult_e                                RuleResult      = RULE_RESULT_ERROR;

    /*  GIVEN */
    {
        /*  Card image */
        {
            pData->DynamicData.isTransit                        = TRUE;
            pData->Tariff.TPurseMinBalance                      = 100;
            pData->Tariff.TPurseMaximumBalance                  = 10000;

            CreateCardImage_Empty( &CardImage );

            CardImage.pMYKI_TAControl->Directory[ 0 ].Status    = TAPP_CONTROL_DIRECTORY_STATUS_ACTIVATED;
            CardImage.pMYKI_TAPurseBalance->Balance             = - ( pData->Tariff.TPurseMaximumBalance * 2 ) - 1;
            CardImage.pMYKI_TAPurseControl->LastChangeNewBalance= CardImage.pMYKI_TAPurseBalance->Balance;
            CardImage.pMYKI_TAPurseControl->NextTxSeqNo         = CardImage.pMYKI_TAPurseControl->LastChangeTxSeqNo + 1;
        }
    }

    /*  WHEN */
    {
        /*  Execute business rule */
        RuleResult  = BR_LLSC_0_1( pData );
    }

    /*  THEN */
    {
        UT_Start( );

        /*  Make sure business rule is bypassed */
        UT_Assert( RuleResult == RULE_RESULT_BYPASSED );

        /*  Make sure card image snapshot created */
        UT_Assert( pData->InternalData.cardSnapshotSequenceNumber == 1 );

        /*  Make sure returned data is correct */
        UT_Assert( pData->ReturnedData.rejectReason == MYKI_BR_REJECT_REASON_INVALID_BALANCE );
        UT_Assert( pData->ReturnedData.bypassCode == BYPASS_CODE( 0, 1, 8, 1 ) );

        return  UT_Result( );
    }
}   /*  test_BR_LLSC_0_1_010( ) */

/*==========================================================================*
**
**  test_BR_LLSC_0_1_011
**
**  Description     :
**      Unit-test EXECUTED condition.
**      TPurse balance > Maximum.
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

int     test_BR_LLSC_0_1_011( MYKI_BR_ContextData_t *pData )
{
    CardImage_t                                 CardImage;
    RuleResult_e                                RuleResult      = RULE_RESULT_ERROR;

    /*  GIVEN */
    {
        /*  Card image */
        {
            pData->DynamicData.isTransit                        = TRUE;
            pData->Tariff.TPurseMinBalance                      = 100;
            pData->Tariff.TPurseMaximumBalance                  = 10000;

            CreateCardImage_Empty( &CardImage );

            CardImage.pMYKI_TAControl->Directory[ 0 ].Status    = TAPP_CONTROL_DIRECTORY_STATUS_ACTIVATED;
            CardImage.pMYKI_TAPurseBalance->Balance             = ( pData->Tariff.TPurseMaximumBalance * 2 ) + 1;
            CardImage.pMYKI_TAPurseControl->LastChangeNewBalance= CardImage.pMYKI_TAPurseBalance->Balance;
            CardImage.pMYKI_TAPurseControl->NextTxSeqNo         = CardImage.pMYKI_TAPurseControl->LastChangeTxSeqNo + 1;
        }
    }

    /*  WHEN */
    {
        /*  Execute business rule */
        RuleResult  = BR_LLSC_0_1( pData );
    }

    /*  THEN */
    {
        UT_Start( );

        /*  Make sure business rule is bypassed */
        UT_Assert( RuleResult == RULE_RESULT_BYPASSED );

        /*  Make sure card image snapshot created */
        UT_Assert( pData->InternalData.cardSnapshotSequenceNumber == 1 );

        /*  Make sure returned data is correct */
        UT_Assert( pData->ReturnedData.rejectReason == MYKI_BR_REJECT_REASON_INVALID_BALANCE );
        UT_Assert( pData->ReturnedData.bypassCode == BYPASS_CODE( 0, 1, 8, 2 ) );

        return  UT_Result( );
    }
}   /*  test_BR_LLSC_0_1_011( ) */

/*==========================================================================*
**
**  test_BR_LLSC_0_1_012
**
**  Description     :
**      Unit-test EXECUTED condition.
**      TPurse balance != LastChangeNewBalance.
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

int     test_BR_LLSC_0_1_012( MYKI_BR_ContextData_t *pData )
{
    CardImage_t                                 CardImage;
    RuleResult_e                                RuleResult      = RULE_RESULT_ERROR;

    /*  GIVEN */
    {
        /*  Card image */
        {
            pData->DynamicData.isTransit                        = TRUE;
            pData->Tariff.TPurseMinBalance                      = 100;
            pData->Tariff.TPurseMaximumBalance                  = 10000;

            CreateCardImage_Empty( &CardImage );

            CardImage.pMYKI_TAControl->Directory[ 0 ].Status    = TAPP_CONTROL_DIRECTORY_STATUS_ACTIVATED;
            CardImage.pMYKI_TAPurseBalance->Balance             = 1000;
            CardImage.pMYKI_TAPurseControl->LastChangeNewBalance= CardImage.pMYKI_TAPurseBalance->Balance + 1;
            CardImage.pMYKI_TAPurseControl->NextTxSeqNo         = CardImage.pMYKI_TAPurseControl->LastChangeTxSeqNo + 1;
        }
    }

    /*  WHEN */
    {
        /*  Execute business rule */
        RuleResult  = BR_LLSC_0_1( pData );
    }

    /*  THEN */
    {
        UT_Start( );

        /*  Make sure business rule is executed */
        UT_Assert( RuleResult == RULE_RESULT_EXECUTED );

        /*  Make sure card image snapshot created */
        UT_Assert( pData->InternalData.cardSnapshotSequenceNumber == 1 );

        return  UT_Result( );
    }
}   /*  test_BR_LLSC_0_1_012( ) */

/*==========================================================================*
**
**  test_BR_LLSC_0_1_013
**
**  Description     :
**      Unit-test EXECUTED condition.
**      NextTxSeqNo != LastChangeTxSeqNo + 1.
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

int     test_BR_LLSC_0_1_013( MYKI_BR_ContextData_t *pData )
{
    CardImage_t                                 CardImage;
    RuleResult_e                                RuleResult      = RULE_RESULT_ERROR;

    /*  GIVEN */
    {
        /*  Card image */
        {
            pData->DynamicData.isTransit                        = TRUE;
            pData->Tariff.TPurseMinBalance                      = 100;
            pData->Tariff.TPurseMaximumBalance                  = 10000;

            CreateCardImage_Empty( &CardImage );

            CardImage.pMYKI_TAControl->Directory[ 0 ].Status    = TAPP_CONTROL_DIRECTORY_STATUS_ACTIVATED;
            CardImage.pMYKI_TAPurseBalance->Balance             = 1000;
            CardImage.pMYKI_TAPurseControl->LastChangeNewBalance= CardImage.pMYKI_TAPurseBalance->Balance;
            CardImage.pMYKI_TAPurseControl->NextTxSeqNo         = CardImage.pMYKI_TAPurseControl->LastChangeTxSeqNo + 1 + 1;
        }
    }

    /*  WHEN */
    {
        /*  Execute business rule */
        RuleResult  = BR_LLSC_0_1( pData );
    }

    /*  THEN */
    {
        UT_Start( );

        /*  Make sure business rule is executed */
        UT_Assert( RuleResult == RULE_RESULT_EXECUTED );

        /*  Make sure card image snapshot created */
        UT_Assert( pData->InternalData.cardSnapshotSequenceNumber == 1 );

        return  UT_Result( );
    }
}   /*  test_BR_LLSC_0_1_013( ) */

/*==========================================================================*
**
**  test_BR_LLSC_0_1_014
**
**  Description     :
**      Unit-test EXECUTED condition.
**      The product in use refers to a product slot that is unused.
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

int     test_BR_LLSC_0_1_014( MYKI_BR_ContextData_t *pData )
{
    CardImage_t                                 CardImage;
    RuleResult_e                                RuleResult      = RULE_RESULT_ERROR;

    /*  GIVEN */
    {
        /*  Card image */
        {
            pData->DynamicData.isTransit                        = TRUE;
            pData->Tariff.TPurseMinBalance                      = 100;
            pData->Tariff.TPurseMaximumBalance                  = 10000;

            CreateCardImage_Empty( &CardImage );

            CardImage.pMYKI_TAControl->Directory[ 0 ].Status    = TAPP_CONTROL_DIRECTORY_STATUS_ACTIVATED;
            CardImage.pMYKI_TAPurseBalance->Balance             = 1000;
            CardImage.pMYKI_TAPurseControl->LastChangeNewBalance= CardImage.pMYKI_TAPurseBalance->Balance;
            CardImage.pMYKI_TAPurseControl->NextTxSeqNo         = CardImage.pMYKI_TAPurseControl->LastChangeTxSeqNo + 1;

            /*  In used provisional N-HOUR product - OK */
            CardImage.pMYKI_TAControl->ProductInUse             = 2;
            CardImage.pMYKI_TAControl->Directory[ 2 ].SerialNo  = 2;
            CardImage.pMYKI_TAControl->Directory[ 2 ].Status    = TAPP_CONTROL_DIRECTORY_STATUS_UNUSED;
            CardImage.pMYKI_TAControl->Directory[ 2 ].IssuerId  = 1;
            CardImage.pMYKI_TAControl->Directory[ 2 ].ProductId = PRODUCT_ID_NHOUR;
            CardImage.pMYKI_TAProduct[ 1 ]->ControlBitmap       = 0;
            CardImage.pMYKI_TAProduct[ 1 ]->StartDateTime       = pData->DynamicData.currentDateTime;
            CardImage.pMYKI_TAProduct[ 1 ]->EndDateTime         = CardImage.pMYKI_TAProduct[ 1 ]->StartDateTime + HOURS_TO_SECONDS( 3 );
        }
    }

    /*  WHEN */
    {
        /*  Execute business rule */
        RuleResult  = BR_LLSC_0_1( pData );
    }

    /*  THEN */
    {
        UT_Start( );

        /*  Make sure business rule is executed */
        UT_Assert( RuleResult == RULE_RESULT_EXECUTED );

        /*  Make sure no card image snapshot created */
        UT_Assert( pData->InternalData.cardSnapshotSequenceNumber == 0 );

        /*  Make sure product in use is clear */
        UT_Assert( CardImage.pMYKI_TAControl->ProductInUse == 0 );

        return  UT_Result( );
    }
}   /*  test_BR_LLSC_0_1_014( ) */
