/* -*- mode: c; tabs: 4; -*- */
/*=============================================================================
**
**    Vix Technology                   Licensed software
**    (C) 2013                         All rights reserved
**
**=============================================================================
**
**  Project/Product : MBU
**  Filename        : test_BR_LLSC_4_19.c
**  Author(s)       : David Purdie
**
**  Description     :
**      Implements BR_LLSC_4_19 business rule unit-tests.
**
**  Function(s)     :
**      Template                [Public]    template function
**
**  Information     :
**   Compiler(s)    : ANSI C
**   Target(s)      : Independent
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
#include "BR_Common.h"

#include "test_common.h"

/*
 *      External References
 *      -------------------
 */

/*
 *      Local Constants and Macros
 *      --------------------------
 */

#define HOURS_TO_MINUTES( h )           ( (h) * 60 )

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

static  Time_t  test_BR_LLSC_4_19_RoundToNHourRoundPortion( MYKI_BR_ContextData_t *pData, Time_t DateTime )
{
    Time_t      nHourRoundingPortion    = MINUTES_TO_SECONDS( pData->Tariff.nHourRoundingPortion );

    /*  ALWAYS round up */
    DateTime    = ( ( ( DateTime / nHourRoundingPortion ) + 1 ) * nHourRoundingPortion );
    return DateTime;
}   /* test_BR_LLSC_4_19_RoundToNHourRoundPortion( ) */

/*==========================================================================*
**
**  test_BR_LLSC_4_19_001
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

int test_BR_LLSC_4_19_001( MYKI_BR_ContextData_t *pData )
{
    MYKI_TACapping_t       *pMYKI_TACapping         = NULL;
    MYKI_TAControl_t       *pMYKI_TAControl         = NULL;
    MYKI_TAProduct_t       *pMYKI_TAProduct         = NULL;
    U8_t                    nDirIndex               = 2;

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


    /*
    ** Test with bad data pointer
    */
    CsVerbose( "+test_BR_LLSC_4_19_001 - Null data pointer");
    if ( BR_LLSC_4_19(NULL) != RULE_RESULT_ERROR )
    {
        return FALSE;
    }

    /*
    ** Test with no product in use
    */
    CsVerbose( "+test_BR_LLSC_4_19_001 - No Product in use");
    pMYKI_TAControl->ProductInUse = 0;
    if ( BR_LLSC_4_19(pData) != RULE_RESULT_BYPASSED )
    {
        return FALSE;
    }

    /*
    ** Product is use is not provisional
    */
    CsVerbose( "+test_BR_LLSC_4_19_001 - Product is use is not provisional");
    pMYKI_TAControl->ProductInUse = nDirIndex;
    pMYKI_TAControl->Directory[ nDirIndex ].Status = TAPP_CONTROL_DIRECTORY_STATUS_ACTIVATED;
    if ( MYKI_CS_TAProductGet( (U8_t)( nDirIndex - 1 ), &pMYKI_TAProduct ) < 0 || pMYKI_TAProduct == NULL )
    {
        return FALSE;
    }
    pMYKI_TAProduct->ControlBitmap &= ~TAPP_TPRODUCT_CONTROL_PROVISIONAL_BITMAP;

    if ( BR_LLSC_4_19(pData) != RULE_RESULT_BYPASSED )
    {
        return FALSE;
    }

    /*
    ** The number of additional hours(7) is currently 0 (zero)
    */
    CsVerbose( "+test_BR_LLSC_4_19_001 - The number of additional hours(7) is currently 0 (zero)");
    pMYKI_TAControl->Directory[ nDirIndex ].Status = TAPP_CONTROL_DIRECTORY_STATUS_ACTIVATED;
    pMYKI_TAProduct->ControlBitmap |= TAPP_TPRODUCT_CONTROL_PROVISIONAL_BITMAP;
    pData->DynamicData.additionalMinutesThisTrip = ( 1 /*HOURS*/ * 60 );

    if ( BR_LLSC_4_19(pData) != RULE_RESULT_BYPASSED )
    {
        return FALSE;
    }

    /*
    ** VIX Sanity test - Low zone no higher than high zone
    */
    CsVerbose( "+test_BR_LLSC_4_19_001 - Low Zone, High Zone Sanity");
    pData->DynamicData.additionalMinutesThisTrip = 0;
    pData->DynamicData.currentTripZoneLow = 12;
    pData->DynamicData.currentTripZoneHigh = 7;

    if (BR_LLSC_4_19(pData) != RULE_RESULT_ERROR)
    {
        return FALSE;
    }


    /*
    ** All is good
    */
    return TRUE;
}

/*==========================================================================*
**
**  test_BR_LLSC_4_19_002
**
**  Description     :
**      Unit-test Execute conditions.
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

int test_BR_LLSC_4_19_002a( MYKI_BR_ContextData_t *pData )
{
    MYKI_TACapping_t       *pMYKI_TACapping             = NULL;
    MYKI_TAControl_t       *pMYKI_TAControl             = NULL;
    MYKI_TAProduct_t       *pMYKI_TAProduct             = NULL;
    MYKI_CAControl_t       *pMYKI_CAControl             = NULL;
    U8_t                    nDirIndex                   = 2;
    time_t                  StartDateTime               = 0;
    int                     nHourExtendPeriodMinutes    = HOURS_TO_MINUTES( 2 );
    int                     nHourPeriodMinutes          = HOURS_TO_MINUTES( 2 );
    RuleResult_e            RuleResult                  = RULE_RESULT_ERROR;

    if ( pData == NULL )
    {
        return FALSE;
    }

    if( MYKI_CS_CAControlGet( &pMYKI_CAControl ) != MYKI_CS_OK )
    {
        return FALSE;
    }

    if ( MYKI_CS_TACappingGet( &pMYKI_TACapping ) < 0 )
    {
        return FALSE;
    }

    if ( MYKI_CS_TAControlGet( &pMYKI_TAControl ) < 0 )
    {
        return FALSE;
    }

    if ( MYKI_CS_TAProductGet( (U8_t)( nDirIndex - 1 ), &pMYKI_TAProduct ) < 0 )
    {
        return FALSE;
    }

    /*  GIVEN */
    {
        /*  Card image - provisional product */
        {
            struct tm       ctm                                 = { 0 };

            /*  StartBusinessDay (03:00) < (TAProduct.StartDateTime + 02:00) < 18:00 (nHourEveningCutOff) */
            ctm.tm_mday                                         = 1;
            ctm.tm_mon                                          = ( 5 - 1 );
            ctm.tm_year                                         = ( 2014 - 1900 );
            ctm.tm_hour                                         = 13;
            ctm.tm_min                                          = 0;
            ctm.tm_sec                                          = 0;
            ctm.tm_isdst                                        = -1;
            StartDateTime                                       = mktime( &ctm );

            pMYKI_CAControl->Status                             = CARD_CONTROL_STATUS_ACTIVATED;

            pMYKI_TAControl->ProductInUse                       = nDirIndex;
            pMYKI_TAControl->Directory[ nDirIndex ].Status      = TAPP_CONTROL_DIRECTORY_STATUS_ACTIVATED;
            pMYKI_TAControl->Directory[ nDirIndex ].SerialNo    = 3;
            pMYKI_TAControl->Status                             = TAPP_CONTROL_STATUS_ACTIVATED;

            pMYKI_TAProduct->ControlBitmap                     |= TAPP_TPRODUCT_CONTROL_PROVISIONAL_BITMAP;
            pMYKI_TAProduct->StartDateTime                      = StartDateTime;
            pMYKI_TAProduct->EndDateTime                        = StartDateTime + MINUTES_TO_SECONDS(nHourPeriodMinutes);
        }

        /*  Tariff */
        {
            MYKI_CD_setCitySaverZone( 3 );
        }

        /*  BR context data */
        {
            pData->DynamicData.additionalMinutesThisTrip        = 0;
            pData->Tariff.nHourExtendThreshold                  = 2;
            pData->Tariff.nHourMaximumDuration                  = 10 /*HOURS*/;
            pData->Tariff.nHourExtendPeriodMinutes              = nHourExtendPeriodMinutes;
            pData->Tariff.nHourPeriodMinutes                    = nHourPeriodMinutes;
            pData->Tariff.nHourRoundingPortion                  = 1;
        }
    }

    /*  WHEN */
    {
        /*  Travelled from zone 2 to 7 */
        pData->DynamicData.currentTripZoneLow                   = 2;
        pData->DynamicData.currentTripZoneHigh                  = 7;

        /*  Executes business rule */
        RuleResult  = BR_LLSC_4_19( pData );
    }

    /*  THEN */
    {
        UT_Start( );

        /*  Make sure business rule is executed */
        UT_Assert( RuleResult == RULE_RESULT_EXECUTED );

        /*  Make sure card image is correct */
        UT_Assert( pMYKI_TAProduct->EndDateTime == (test_BR_LLSC_4_19_RoundToNHourRoundPortion(pData, (StartDateTime + MINUTES_TO_SECONDS(nHourPeriodMinutes))) + (2 * MINUTES_TO_SECONDS(nHourExtendPeriodMinutes))) );

        /*  Make sure BR context data is correct */
        UT_Assert( pData->DynamicData.additionalMinutesThisTrip == ( 2 * HOURS_TO_MINUTES(2) ) );

        return  UT_Result( );
    }
}   /*  test_BR_LLSC_4_19_002a( ) */

int test_BR_LLSC_4_19_002b( MYKI_BR_ContextData_t *pData )
{
    MYKI_TACapping_t       *pMYKI_TACapping             = NULL;
    MYKI_TAControl_t       *pMYKI_TAControl             = NULL;
    MYKI_TAProduct_t       *pMYKI_TAProduct             = NULL;
    MYKI_CAControl_t       *pMYKI_CAControl             = NULL;
    U8_t                    nDirIndex                   = 2;
    time_t                  StartDateTime               = 0;
    int                     nHourExtendPeriodMinutes    = HOURS_TO_MINUTES( 2 );
    int                     nHourPeriodMinutes          = HOURS_TO_MINUTES( 2 );
    RuleResult_e            RuleResult                  = RULE_RESULT_ERROR;

    if ( pData == NULL )
    {
        return FALSE;
    }

    if( MYKI_CS_CAControlGet( &pMYKI_CAControl ) != MYKI_CS_OK )
    {
        return FALSE;
    }

    if ( MYKI_CS_TACappingGet( &pMYKI_TACapping ) < 0 )
    {
        return FALSE;
    }

    if ( MYKI_CS_TAControlGet( &pMYKI_TAControl ) < 0 )
    {
        return FALSE;
    }

    if ( MYKI_CS_TAProductGet( (U8_t)( nDirIndex - 1 ), &pMYKI_TAProduct ) < 0 )
    {
        return FALSE;
    }

    /*  GIVEN */
    {
        /*  Card image - provisional product */
        {
            struct tm       ctm                                 = { 0 };

            /*  StartBusinessDay (03:00) < (TAProduct.StartDateTime + 02:00) < 18:00 (nHourEveningCutOff) */
            ctm.tm_mday                                         = 1;
            ctm.tm_mon                                          = ( 5 - 1 );
            ctm.tm_year                                         = ( 2014 - 1900 );
            ctm.tm_hour                                         = 13;
            ctm.tm_min                                          = 0;
            ctm.tm_sec                                          = 0;
            ctm.tm_isdst                                        = -1;
            StartDateTime                                       = mktime( &ctm );

            pMYKI_CAControl->Status                             = CARD_CONTROL_STATUS_ACTIVATED;

            pMYKI_TAControl->ProductInUse                       = nDirIndex;
            pMYKI_TAControl->Directory[ nDirIndex ].Status      = TAPP_CONTROL_DIRECTORY_STATUS_ACTIVATED;
            pMYKI_TAControl->Directory[ nDirIndex ].SerialNo    = 3;
            pMYKI_TAControl->Status                             = TAPP_CONTROL_STATUS_ACTIVATED;

            pMYKI_TAProduct->ControlBitmap                     |= TAPP_TPRODUCT_CONTROL_PROVISIONAL_BITMAP;
            pMYKI_TAProduct->StartDateTime                      = StartDateTime;
            pMYKI_TAProduct->EndDateTime                        = StartDateTime + MINUTES_TO_SECONDS(nHourPeriodMinutes);
        }

        /*  Tariff */
        {
            MYKI_CD_setCitySaverZone( 3 );
        }

        /*  BR context data */
        {
            pData->DynamicData.additionalMinutesThisTrip        = 0;
            pData->Tariff.nHourExtendThreshold                  = 0;
            pData->Tariff.nHourMaximumDuration                  = 10 /*HOURS*/;
            pData->Tariff.nHourExtendPeriodMinutes              = nHourExtendPeriodMinutes;
            pData->Tariff.nHourPeriodMinutes                    = nHourPeriodMinutes;
            pData->Tariff.nHourRoundingPortion                  = 1;
        }
    }

    /*  WHEN */
    {
        /*  Travelled from zone 2 to 7 */
        pData->DynamicData.currentTripZoneLow                   = 2;
        pData->DynamicData.currentTripZoneHigh                  = 7;

        /*  Executes business rule */
        RuleResult  = BR_LLSC_4_19( pData );
    }

    /*  THEN */
    {
        UT_Start( );

        /*  Make sure business rule is executed */
        UT_Assert( RuleResult == RULE_RESULT_EXECUTED );

        /*  Make sure card image is correct */
        UT_Assert( pMYKI_TAProduct->EndDateTime == (StartDateTime + MINUTES_TO_SECONDS(nHourPeriodMinutes)) );

        /*  Make sure BR context data is correct */
        UT_Assert( pData->DynamicData.additionalMinutesThisTrip == 0 );

        return  UT_Result( );
    }
}   /*  test_BR_LLSC_4_19_002b( ) */

int test_BR_LLSC_4_19_002c( MYKI_BR_ContextData_t *pData )
{
    MYKI_TACapping_t       *pMYKI_TACapping             = NULL;
    MYKI_TAControl_t       *pMYKI_TAControl             = NULL;
    MYKI_TAProduct_t       *pMYKI_TAProduct             = NULL;
    MYKI_CAControl_t       *pMYKI_CAControl             = NULL;
    U8_t                    nDirIndex                   = 2;
    time_t                  StartDateTime               = 0;
    int                     nHourExtendPeriodMinutes    = HOURS_TO_MINUTES( 2 );
    int                     nHourPeriodMinutes          = HOURS_TO_MINUTES( 2 );
    RuleResult_e            RuleResult                  = RULE_RESULT_ERROR;

    if ( pData == NULL )
    {
        return FALSE;
    }

    if( MYKI_CS_CAControlGet( &pMYKI_CAControl ) != MYKI_CS_OK )
    {
        return FALSE;
    }

    if ( MYKI_CS_TACappingGet( &pMYKI_TACapping ) < 0 )
    {
        return FALSE;
    }

    if ( MYKI_CS_TAControlGet( &pMYKI_TAControl ) < 0 )
    {
        return FALSE;
    }

    if ( MYKI_CS_TAProductGet( (U8_t)( nDirIndex - 1 ), &pMYKI_TAProduct ) < 0 )
    {
        return FALSE;
    }

    /*  GIVEN */
    {
        /*  Card image - provisional product */
        {
            struct tm       ctm                                 = { 0 };

            /*  StartBusinessDay (03:00) < (TAProduct.StartDateTime + 02:00) < nHourEveningCutOff (18:00) */
            ctm.tm_mday                                         = 1;
            ctm.tm_mon                                          = ( 5 - 1 );
            ctm.tm_year                                         = ( 2014 - 1900 );
            ctm.tm_hour                                         = 13;
            ctm.tm_min                                          = 0;
            ctm.tm_sec                                          = 0;
            ctm.tm_isdst                                        = -1;
            StartDateTime                                       = mktime( &ctm );

            pMYKI_CAControl->Status                             = CARD_CONTROL_STATUS_ACTIVATED;

            pMYKI_TAControl->ProductInUse                       = nDirIndex;
            pMYKI_TAControl->Directory[ nDirIndex ].Status      = TAPP_CONTROL_DIRECTORY_STATUS_ACTIVATED;
            pMYKI_TAControl->Directory[ nDirIndex ].SerialNo    = 3;
            pMYKI_TAControl->Status                             = TAPP_CONTROL_STATUS_ACTIVATED;

            pMYKI_TAProduct->ControlBitmap                     |= TAPP_TPRODUCT_CONTROL_PROVISIONAL_BITMAP;
            pMYKI_TAProduct->StartDateTime                      = StartDateTime;
            pMYKI_TAProduct->EndDateTime                        = StartDateTime + MINUTES_TO_SECONDS(nHourPeriodMinutes);
        }

        /*  Tariff */
        {
            MYKI_CD_setCitySaverZone( 3 );
        }

        /*  BR context data */
        {
            pData->DynamicData.additionalMinutesThisTrip        = 0;
            pData->Tariff.nHourExtendThreshold                  = 2;
            pData->Tariff.nHourMaximumDuration                  = 3 /* HOUR */;
            pData->Tariff.nHourExtendPeriodMinutes              = nHourExtendPeriodMinutes;
            pData->Tariff.nHourPeriodMinutes                    = nHourPeriodMinutes;
            pData->Tariff.nHourRoundingPortion                  = 1;
        }
    }

    /*  WHEN */
    {
        /*  Travelled from zone 2 to 7 */
        pData->DynamicData.currentTripZoneLow                   = 2;
        pData->DynamicData.currentTripZoneHigh                  = 7;

        /*  Executes business rule */
        RuleResult  = BR_LLSC_4_19( pData );
    }

    /*  THEN */
    {
        UT_Start( );

        /*  Make sure business rule is executed */
        UT_Assert( RuleResult == RULE_RESULT_EXECUTED );

        /*  Make sure card image is correct */
        UT_Assert( pMYKI_TAProduct->EndDateTime == (test_BR_LLSC_4_19_RoundToNHourRoundPortion(pData, (StartDateTime + MINUTES_TO_SECONDS(nHourPeriodMinutes))) + HOURS_TO_SECONDS(1)) );

        /*  Make sure BR context data is correct */
        UT_Assert( pData->DynamicData.additionalMinutesThisTrip == HOURS_TO_MINUTES(1) );

        return  UT_Result( );
    }
}   /*  test_BR_LLSC_4_19_002c( ) */


int test_BR_LLSC_4_19_002d( MYKI_BR_ContextData_t *pData )
{
    MYKI_TACapping_t       *pMYKI_TACapping             = NULL;
    MYKI_TAControl_t       *pMYKI_TAControl             = NULL;
    MYKI_TAProduct_t       *pMYKI_TAProduct             = NULL;
    MYKI_CAControl_t       *pMYKI_CAControl             = NULL;
    U8_t                    nDirIndex                   = 2;
    time_t                  StartDateTime               = 0;
    time_t                  StartOfBusinessDay          = 0;
    int                     nHourExtendPeriodMinutes    = 15;
    int                     nHourPeriodMinutes          = HOURS_TO_MINUTES( 2 );
    RuleResult_e            RuleResult                  = RULE_RESULT_ERROR;

    if ( pData == NULL )
    {
        return FALSE;
    }

    if( MYKI_CS_CAControlGet( &pMYKI_CAControl ) != MYKI_CS_OK )
    {
        return FALSE;
    }

    if ( MYKI_CS_TACappingGet( &pMYKI_TACapping ) < 0 )
    {
        return FALSE;
    }

    if ( MYKI_CS_TAControlGet( &pMYKI_TAControl ) < 0 )
    {
        return FALSE;
    }

    if ( MYKI_CS_TAProductGet( (U8_t)( nDirIndex - 1 ), &pMYKI_TAProduct ) < 0 )
    {
        return FALSE;
    }

    /*  GIVEN */
    {
        /*  Card image - provisional product */
        {
            struct tm       ctm                                 = { 0 };

            ctm.tm_mday                                         = 1;
            ctm.tm_mon                                          = ( 5 - 1 );
            ctm.tm_year                                         = ( 2014 - 1900 );
            ctm.tm_hour                                         = 3;
            ctm.tm_min                                          = 0;
            ctm.tm_sec                                          = 0;
            ctm.tm_isdst                                        = -1;
            StartOfBusinessDay                                  = mktime( &ctm );

            /*  StartBusinessDay (03:00) > (TAProduct.StartDateTime + 02:00) */
            ctm.tm_mday                                         = 1;
            ctm.tm_mon                                          = ( 5 - 1 );
            ctm.tm_year                                         = ( 2014 - 1900 );
            ctm.tm_hour                                         = 0;
            ctm.tm_min                                          = 5;
            ctm.tm_sec                                          = 0;
            ctm.tm_isdst                                        = -1;
            StartDateTime                                       = mktime( &ctm );

            pMYKI_CAControl->Status                             = CARD_CONTROL_STATUS_ACTIVATED;

            pMYKI_TAControl->ProductInUse                       = nDirIndex;
            pMYKI_TAControl->Directory[ nDirIndex ].Status      = TAPP_CONTROL_DIRECTORY_STATUS_ACTIVATED;
            pMYKI_TAControl->Directory[ nDirIndex ].SerialNo    = 3;
            pMYKI_TAControl->Status                             = TAPP_CONTROL_STATUS_ACTIVATED;

            pMYKI_TAProduct->ControlBitmap                     |= TAPP_TPRODUCT_CONTROL_PROVISIONAL_BITMAP;
            pMYKI_TAProduct->StartDateTime                      = StartDateTime;
            pMYKI_TAProduct->EndDateTime                        = StartDateTime + MINUTES_TO_SECONDS(nHourPeriodMinutes);
        }

        /*  Tariff */
        {
            MYKI_CD_setCitySaverZone( 3 );
        }

        /*  BR context data */
        {
            pData->DynamicData.additionalMinutesThisTrip        = 0;
            pData->Tariff.nHourExtendThreshold                  = 2;
            pData->Tariff.nHourMaximumDuration                  = 3 /* HOUR */;
            pData->Tariff.nHourExtendPeriodMinutes              = nHourExtendPeriodMinutes;
            pData->Tariff.nHourPeriodMinutes                    = nHourPeriodMinutes;
            pData->Tariff.nHourRoundingPortion                  = 1;
        }
    }

    /*  WHEN */
    {
        /*  Travelled from zone 2 to 7 */
        pData->DynamicData.currentTripZoneLow                   = 2;
        pData->DynamicData.currentTripZoneHigh                  = 7;

        /*  Executes business rule */
        RuleResult  = BR_LLSC_4_19( pData );
    }

    /*  THEN */
    {
        UT_Start( );

        /*  Make sure business rule is executed */
        UT_Assert( RuleResult == RULE_RESULT_EXECUTED );

        /*  Make sure card image is correct */
        UT_Assert( pMYKI_TAProduct->EndDateTime == StartOfBusinessDay );

        /*  Make sure BR context data is correct */
        UT_Assert( pData->DynamicData.additionalMinutesThisTrip == ( 2 * nHourExtendPeriodMinutes ) );

        return  UT_Result( );
    }
}   /*  test_BR_LLSC_4_19_002d( ) */

int test_BR_LLSC_4_19_002e( MYKI_BR_ContextData_t *pData )
{
    MYKI_TACapping_t       *pMYKI_TACapping             = NULL;
    MYKI_TAControl_t       *pMYKI_TAControl             = NULL;
    MYKI_TAProduct_t       *pMYKI_TAProduct             = NULL;
    MYKI_CAControl_t       *pMYKI_CAControl             = NULL;
    U8_t                    nDirIndex                   = 2;
    time_t                  StartDateTime               = 0;
    time_t                  StartOfBusinessDay          = 0;
    int                     nHourExtendPeriodMinutes    = 15;
    int                     nHourPeriodMinutes          = HOURS_TO_MINUTES( 2 );
    RuleResult_e            RuleResult                  = RULE_RESULT_ERROR;

    if ( pData == NULL )
    {
        return FALSE;
    }

    if( MYKI_CS_CAControlGet( &pMYKI_CAControl ) != MYKI_CS_OK )
    {
        return FALSE;
    }

    if ( MYKI_CS_TACappingGet( &pMYKI_TACapping ) < 0 )
    {
        return FALSE;
    }

    if ( MYKI_CS_TAControlGet( &pMYKI_TAControl ) < 0 )
    {
        return FALSE;
    }

    if ( MYKI_CS_TAProductGet( (U8_t)( nDirIndex - 1 ), &pMYKI_TAProduct ) < 0 )
    {
        return FALSE;
    }

    /*  GIVEN */
    {
        /*  Card image - provisional product */
        {
            struct tm       ctm                                 = { 0 };

            ctm.tm_mday                                         = 2;
            ctm.tm_mon                                          = ( 5 - 1 );
            ctm.tm_year                                         = ( 2014 - 1900 );
            ctm.tm_hour                                         = 3;
            ctm.tm_min                                          = 0;
            ctm.tm_sec                                          = 0;
            ctm.tm_isdst                                        = -1;
            StartOfBusinessDay                                  = mktime( &ctm );

            /*  (TAProduct.StartDateTime + 02:00) > nHourEveningCutOff (18:00) */
            ctm.tm_mday                                         = 1;
            ctm.tm_mon                                          = ( 5 - 1 );
            ctm.tm_year                                         = ( 2014 - 1900 );
            ctm.tm_hour                                         = 18;
            ctm.tm_min                                          = 30;
            ctm.tm_sec                                          = 0;
            ctm.tm_isdst                                        = -1;
            StartDateTime                                       = mktime( &ctm );

            pMYKI_CAControl->Status                             = CARD_CONTROL_STATUS_ACTIVATED;

            pMYKI_TAControl->ProductInUse                       = nDirIndex;
            pMYKI_TAControl->Directory[ nDirIndex ].Status      = TAPP_CONTROL_DIRECTORY_STATUS_ACTIVATED;
            pMYKI_TAControl->Directory[ nDirIndex ].SerialNo    = 3;
            pMYKI_TAControl->Status                             = TAPP_CONTROL_STATUS_ACTIVATED;

            pMYKI_TAProduct->ControlBitmap                     |= TAPP_TPRODUCT_CONTROL_PROVISIONAL_BITMAP;
            pMYKI_TAProduct->StartDateTime                      = StartDateTime;
            pMYKI_TAProduct->EndDateTime                        = StartDateTime + MINUTES_TO_SECONDS(nHourPeriodMinutes);
        }

        /*  Tariff */
        {
            MYKI_CD_setCitySaverZone( 3 );
        }

        /*  BR context data */
        {
            pData->DynamicData.additionalMinutesThisTrip        = 0;
            pData->Tariff.nHourExtendThreshold                  = 2;
            pData->Tariff.nHourMaximumDuration                  = 3 /* HOUR */;
            pData->Tariff.nHourExtendPeriodMinutes              = nHourExtendPeriodMinutes;
            pData->Tariff.nHourPeriodMinutes                    = nHourPeriodMinutes;
            pData->Tariff.nHourRoundingPortion                  = 1;
        }
    }

    /*  WHEN */
    {
        /*  Travelled from zone 2 to 7 */
        pData->DynamicData.currentTripZoneLow                   = 2;
        pData->DynamicData.currentTripZoneHigh                  = 7;

        /*  Executes business rule */
        RuleResult  = BR_LLSC_4_19( pData );
    }

    /*  THEN */
    {
        UT_Start( );

        /*  Make sure business rule is executed */
        UT_Assert( RuleResult == RULE_RESULT_EXECUTED );

        /*  Make sure card image is correct */
        UT_Assert( pMYKI_TAProduct->EndDateTime == StartOfBusinessDay );

        /*  Make sure BR context data is correct */
        UT_Assert( pData->DynamicData.additionalMinutesThisTrip == ( 2 * nHourExtendPeriodMinutes ) );

        return  UT_Result( );
    }
}   /*  test_BR_LLSC_4_19_002e( ) */
