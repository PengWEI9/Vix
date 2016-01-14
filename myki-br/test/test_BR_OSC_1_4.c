/* -*- mode: c; tabs: 4; -*- */
/*=============================================================================
**
**    Vix Technology                   Licensed software
**    (C) 2015                         All rights reserved
**
**=============================================================================
**
**  Project/Product : MBU
**  Filename        : BR_OSC_1_4.c
**  Author(s)       : Xiaogang Qian
**
**  Description     :
**      Implements BR_OSC_1_4 business rule unit-tests.
**
**  Function(s)     :
**      BR_OSC_1_4_XXX     [Public]    unit-test functions
**
**  Information     :
**   Compiler(s)    : ANSI C
**   Target(s)      : Independent
**
**  Subversion      :
**      $Id: BR_OSC_1_4.c 63860 2015-06-02 03:03:04Z atran $
**      $HeadURL: https://auperasvn01.aupera.erggroup.com/svn/DPG_SWBase/myki-br/trunk/test/BR_OSC_1_4.c $
**
**  History         :
**   Vers.  Date        Aut.  Type     Description
**   -----  ----------  ----  -------  ----------------------------------------
**    1.00  28.06.15    XQ    Create
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

#include    <string.h>
#include    <cs.h>
#include    <myki_cardservices.h>
#include    <myki_cdd_enums.h>
#include    <myki_br.h>
#include    <myki_br_rules.h>
#include    <myki_br_context_data.h>

#include    "test_common.h"

/*==========================================================================*
**
**  test_BR_OSC_1_4_001
**
**  Description     :
**      Unit-test EXECUTED conditions.
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

int test_BR_OSC_1_4_001( MYKI_BR_ContextData_t *pData )
{
    MYKI_CAControl_t       *pMYKI_CAControl         = NULL;
    MYKI_OAControl_t       *pMYKI_OAControl         = NULL;
    MYKI_OARoles_t         *pMYKI_OARoles           = NULL;
    MYKI_OAInspectorData_t *pMYKI_OAInspectorData   = NULL;
    int                     Zone                    = 1;
    int                     EntryPointId            = 53;
    int                     StopId                  = 79;
    int                     LineId                  = 1023;
    int                     Direction               = 1;
    Time_t                  BoardingTime            = pData->DynamicData.currentDateTime;
    int                     SamId                   = 87102;
    int                     ServiceProviderId       = 100;
    int                     RoleType_AO             = 1;
    int                     RoleProfile_AO          = 12;
    RuleResult_e            RuleResult              = RULE_RESULT_ERROR;

    if ( pData == NULL ||
         MYKI_CS_OpenCard( MYKI_CS_OPEN_OPERATOR_APP, NULL /*DONTCARE*/     ) != MYKI_CS_OK ||
         MYKI_CS_CAControlGet(                       &pMYKI_CAControl       ) != MYKI_CS_OK ||
         MYKI_CS_OAControlGet(                       &pMYKI_OAControl       ) != MYKI_CS_OK ||
         MYKI_CS_OARolesGet(                         &pMYKI_OARoles         ) != MYKI_CS_OK ||
         MYKI_CS_OAInspectorDataGet(                 &pMYKI_OAInspectorData ) != MYKI_CS_OK )
    {
        return FALSE;
    }

    /*  GIVEN */
    {
        /*  Card image */
        {
            pMYKI_CAControl->Status                                         = CARD_CONTROL_STATUS_ACTIVATED;
            pMYKI_OAControl->Status                                         = OAPP_CONTROL_STATUS_ACTIVATED;
            pMYKI_OAControl->StaffProviderId                                = ServiceProviderId;
            pMYKI_OAControl->RolesCount                                     = 1;
            pMYKI_OARoles->Role[ 0 ].Type                                   = RoleType_AO;
            pMYKI_OARoles->Role[ 0 ].Profile                                = RoleProfile_AO;
        }

        /*  BR context data */
        {
            pData->DynamicData.isAOCard                                     = FALSE;
            pData->DynamicData.currentZone                                  = Zone;
            pData->DynamicData.entryPointId                                 = EntryPointId;
            pData->DynamicData.stopId                                       = StopId;
            pData->DynamicData.lineId                                       = LineId;
            pData->DynamicData.currentTripDirection                         = Direction;
            pData->StaticData.deviceSamId                                   = SamId;
            pData->StaticData.serviceProviderId                             = ServiceProviderId;
            memset( pData->StaticData.staffRoles, 0, sizeof( pData->StaticData.staffRoles ) );
            pData->StaticData.staffRoles[ 0 ].type                          = RoleType_AO;
            pData->StaticData.staffRoles[ 0 ].profile                       = RoleProfile_AO;
            pData->StaticData.staffRoles[ 0 ].operatorType                  = OperatorType_AUTHORISED_OFFICER;
        }
    }

    /*  WHEN */
    {
        /*  Executes business rule */
        RuleResult  = BR_OSC_1_4( pData );
    }

    /*  THEN */
    {
        UT_Start( );

        /*  Make sure business rule is executed */
        UT_Assert( RuleResult == RULE_RESULT_EXECUTED );

        /*  Make sure card image not changed */
        /*         pMYKI_OAInspectorData->DriverId
                   pMYKI_OAInspectorData->ShiftId
                   pMYKI_OAInspectorData->ShiftSeqNo  */
        UT_Assert( pMYKI_OAInspectorData->SamId == SamId );
        UT_Assert( pMYKI_OAInspectorData->ProviderId == ServiceProviderId );
        UT_Assert( pMYKI_OAInspectorData->BoardingLocation.EntryPointId == EntryPointId );
        UT_Assert( pMYKI_OAInspectorData->BoardingLocation.RouteId == LineId );
        UT_Assert( pMYKI_OAInspectorData->BoardingLocation.StopId == StopId );
        UT_Assert( pMYKI_OAInspectorData->BoardingZone == Zone );
        /*         pMYKI_OAInspectorData->TripSeqNo */
        UT_Assert( pMYKI_OAInspectorData->TripDirection == Direction );
        UT_Assert( pMYKI_OAInspectorData->BoardingTime == BoardingTime );
        /*         pMYKI_OAInspectorData->CardCount
                   pMYKI_OAInspectorData->PTFirst
                   pMYKI_OAInspectorData->PTCount
                   pMYKI_OAInspectorData->PTCancelled */

        return  UT_Result( );
    }
}   /*  BR_OSC_1_4_001( ) */

/*==========================================================================*
**
**  test_BR_OSC_1_4_002a
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

int test_BR_OSC_1_4_002a( MYKI_BR_ContextData_t *pData )
{
    MYKI_CAControl_t       *pMYKI_CAControl         = NULL;
    MYKI_OAControl_t       *pMYKI_OAControl         = NULL;
    MYKI_OARoles_t         *pMYKI_OARoles           = NULL;
    MYKI_OAInspectorData_t *pMYKI_OAInspectorData   = NULL;
    int                     Zone                    = 1;
    int                     EntryPointId            = 53;
    int                     StopId                  = 79;
    int                     LineId                  = 1023;
    int                     Direction               = 1;
/*  Time_t                  BoardingTime            = pData->DynamicData.currentDateTime;   */
    int                     SamId                   = 87102;
    int                     ServiceProviderId       = 100;
    int                     RoleType_AO             = 1;
    int                     RoleProfile_AO          = 12;
    RuleResult_e            RuleResult              = RULE_RESULT_ERROR;

    if ( pData == NULL ||
         MYKI_CS_OpenCard( MYKI_CS_OPEN_OPERATOR_APP, NULL /*DONTCARE*/     ) != MYKI_CS_OK ||
         MYKI_CS_CAControlGet(                       &pMYKI_CAControl       ) != MYKI_CS_OK ||
         MYKI_CS_OAControlGet(                       &pMYKI_OAControl       ) != MYKI_CS_OK ||
         MYKI_CS_OARolesGet(                         &pMYKI_OARoles         ) != MYKI_CS_OK ||
         MYKI_CS_OAInspectorDataGet(                 &pMYKI_OAInspectorData ) != MYKI_CS_OK )
    {
        return FALSE;
    }

    /*  GIVEN */
    {
        /*  Card image */
        {
            pMYKI_CAControl->Status                                         = CARD_CONTROL_STATUS_ACTIVATED;
            pMYKI_OAControl->Status                                         = OAPP_CONTROL_STATUS_ACTIVATED;
            pMYKI_OAControl->StaffProviderId                                = ServiceProviderId;
            pMYKI_OAControl->RolesCount                                     = 1;
            pMYKI_OARoles->Role[ 0 ].Type                                   = RoleType_AO + 1;  /*  != Authorised Officer */
            pMYKI_OARoles->Role[ 0 ].Profile                                = RoleProfile_AO;
        }

        /*  BR context data */
        {
            pData->DynamicData.isAOCard                                     = FALSE;
            pData->DynamicData.currentZone                                  = Zone;
            pData->DynamicData.entryPointId                                 = EntryPointId;
            pData->DynamicData.stopId                                       = StopId;
            pData->DynamicData.lineId                                       = LineId;
            pData->DynamicData.currentTripDirection                         = Direction;
            pData->StaticData.deviceSamId                                   = SamId;
            pData->StaticData.serviceProviderId                             = ServiceProviderId;
            memset( pData->StaticData.staffRoles, 0, sizeof( pData->StaticData.staffRoles ) );
            pData->StaticData.staffRoles[ 0 ].type                          = RoleType_AO;
            pData->StaticData.staffRoles[ 0 ].profile                       = RoleProfile_AO;
            pData->StaticData.staffRoles[ 0 ].operatorType                  = OperatorType_AUTHORISED_OFFICER;
        }
    }

    /*  WHEN */
    {
        /*  Executes business rule */
        RuleResult  = BR_OSC_1_4( pData );
    }

    /*  THEN */
    {
        UT_Start( );

        /*  Make sure business rule is bypassed */
        UT_Assert( RuleResult == RULE_RESULT_BYPASSED );
        UT_Assert( pData->ReturnedData.bypassCode == BYPASS_CODE( 1, 4, 1, 0 ) );

        /*  Make sure card image not changed */
        /*         pMYKI_OAInspectorData->DriverId
                   pMYKI_OAInspectorData->ShiftId
                   pMYKI_OAInspectorData->ShiftSeqNo  */
        UT_Assert( pMYKI_OAInspectorData->SamId == 0 );
        UT_Assert( pMYKI_OAInspectorData->ProviderId == 0 );
        UT_Assert( pMYKI_OAInspectorData->BoardingLocation.EntryPointId == 0 );
        UT_Assert( pMYKI_OAInspectorData->BoardingLocation.RouteId == 0 );
        UT_Assert( pMYKI_OAInspectorData->BoardingLocation.StopId == 0 );
        UT_Assert( pMYKI_OAInspectorData->BoardingZone == 0 );
        /*         pMYKI_OAInspectorData->TripSeqNo */
        UT_Assert( pMYKI_OAInspectorData->TripDirection == 0 );
        UT_Assert( pMYKI_OAInspectorData->BoardingTime == 0 );
        /*         pMYKI_OAInspectorData->CardCount
                   pMYKI_OAInspectorData->PTFirst
                   pMYKI_OAInspectorData->PTCount
                   pMYKI_OAInspectorData->PTCancelled */

        return  UT_Result( );
    }
}   /*  BR_OSC_1_4_002a( ) */

int test_BR_OSC_1_4_002b( MYKI_BR_ContextData_t *pData )
{
    MYKI_CAControl_t       *pMYKI_CAControl         = NULL;
    MYKI_OAControl_t       *pMYKI_OAControl         = NULL;
    MYKI_OARoles_t         *pMYKI_OARoles           = NULL;
    MYKI_OAInspectorData_t *pMYKI_OAInspectorData   = NULL;
    int                     Zone                    = 1;
    int                     EntryPointId            = 53;
    int                     StopId                  = 79;
    int                     LineId                  = 1023;
    int                     Direction               = 1;
/*  Time_t                  BoardingTime            = pData->DynamicData.currentDateTime;   */
    int                     SamId                   = 87102;
    int                     ServiceProviderId       = 100;
    int                     RoleType_AO             = 1;
    int                     RoleProfile_AO          = 12;
    RuleResult_e            RuleResult              = RULE_RESULT_ERROR;

    if ( pData == NULL ||
         MYKI_CS_OpenCard( MYKI_CS_OPEN_OPERATOR_APP, NULL /*DONTCARE*/     ) != MYKI_CS_OK ||
         MYKI_CS_CAControlGet(                       &pMYKI_CAControl       ) != MYKI_CS_OK ||
         MYKI_CS_OAControlGet(                       &pMYKI_OAControl       ) != MYKI_CS_OK ||
         MYKI_CS_OARolesGet(                         &pMYKI_OARoles         ) != MYKI_CS_OK ||
         MYKI_CS_OAInspectorDataGet(                 &pMYKI_OAInspectorData ) != MYKI_CS_OK )
    {
        return FALSE;
    }

    /*  GIVEN */
    {
        /*  Card image */
        {
            pMYKI_CAControl->Status                                         = CARD_CONTROL_STATUS_ACTIVATED;
            pMYKI_OAControl->Status                                         = OAPP_CONTROL_STATUS_ACTIVATED;
            pMYKI_OAControl->StaffProviderId                                = ServiceProviderId;
            pMYKI_OAControl->RolesCount                                     = 1;
            pMYKI_OARoles->Role[ 0 ].Type                                   = RoleType_AO;
            pMYKI_OARoles->Role[ 0 ].Profile                                = RoleProfile_AO + 1;   /*  != Authorised Officer */
        }

        /*  BR context data */
        {
            pData->DynamicData.isAOCard                                     = FALSE;
            pData->DynamicData.currentZone                                  = Zone;
            pData->DynamicData.entryPointId                                 = EntryPointId;
            pData->DynamicData.stopId                                       = StopId;
            pData->DynamicData.lineId                                       = LineId;
            pData->DynamicData.currentTripDirection                         = Direction;
            pData->StaticData.deviceSamId                                   = SamId;
            pData->StaticData.serviceProviderId                             = ServiceProviderId;
            memset( pData->StaticData.staffRoles, 0, sizeof( pData->StaticData.staffRoles ) );
            pData->StaticData.staffRoles[ 0 ].type                          = RoleType_AO;
            pData->StaticData.staffRoles[ 0 ].profile                       = RoleProfile_AO;
            pData->StaticData.staffRoles[ 0 ].operatorType                  = OperatorType_AUTHORISED_OFFICER;
        }
    }

    /*  WHEN */
    {
        /*  Executes business rule */
        RuleResult  = BR_OSC_1_4( pData );
    }

    /*  THEN */
    {
        UT_Start( );

        /*  Make sure business rule is bypassed */
        UT_Assert( RuleResult == RULE_RESULT_BYPASSED );
        UT_Assert( pData->ReturnedData.bypassCode == BYPASS_CODE( 1, 4, 1, 0 ) );

        /*  Make sure card image not changed */
        /*         pMYKI_OAInspectorData->DriverId
                   pMYKI_OAInspectorData->ShiftId
                   pMYKI_OAInspectorData->ShiftSeqNo  */
        UT_Assert( pMYKI_OAInspectorData->SamId == 0 );
        UT_Assert( pMYKI_OAInspectorData->ProviderId == 0 );
        UT_Assert( pMYKI_OAInspectorData->BoardingLocation.EntryPointId == 0 );
        UT_Assert( pMYKI_OAInspectorData->BoardingLocation.RouteId == 0 );
        UT_Assert( pMYKI_OAInspectorData->BoardingLocation.StopId == 0 );
        UT_Assert( pMYKI_OAInspectorData->BoardingZone == 0 );
        /*         pMYKI_OAInspectorData->TripSeqNo */
        UT_Assert( pMYKI_OAInspectorData->TripDirection == 0 );
        UT_Assert( pMYKI_OAInspectorData->BoardingTime == 0 );
        /*         pMYKI_OAInspectorData->CardCount
                   pMYKI_OAInspectorData->PTFirst
                   pMYKI_OAInspectorData->PTCount
                   pMYKI_OAInspectorData->PTCancelled */

        return  UT_Result( );
    }
}   /*  BR_OSC_1_4_002b( ) */

int test_BR_OSC_1_4_002c( MYKI_BR_ContextData_t *pData )
{
    MYKI_CAControl_t       *pMYKI_CAControl         = NULL;
    MYKI_OAControl_t       *pMYKI_OAControl         = NULL;
    MYKI_OARoles_t         *pMYKI_OARoles           = NULL;
    MYKI_OAInspectorData_t *pMYKI_OAInspectorData   = NULL;
    int                     Zone                    = 1;
    int                     EntryPointId            = 53;
    int                     StopId                  = 79;
    int                     LineId                  = 1023;
    int                     Direction               = 1;
/*  Time_t                  BoardingTime            = pData->DynamicData.currentDateTime;   */
    int                     SamId                   = 87102;
    int                     ServiceProviderId       = 100;
    int                     RoleType_AO             = 1;
    int                     RoleProfile_AO          = 12;
    RuleResult_e            RuleResult              = RULE_RESULT_ERROR;

    if ( pData == NULL ||
         MYKI_CS_OpenCard( MYKI_CS_OPEN_OPERATOR_APP, NULL /*DONTCARE*/     ) != MYKI_CS_OK ||
         MYKI_CS_CAControlGet(                       &pMYKI_CAControl       ) != MYKI_CS_OK ||
         MYKI_CS_OAControlGet(                       &pMYKI_OAControl       ) != MYKI_CS_OK ||
         MYKI_CS_OARolesGet(                         &pMYKI_OARoles         ) != MYKI_CS_OK ||
         MYKI_CS_OAInspectorDataGet(                 &pMYKI_OAInspectorData ) != MYKI_CS_OK )
    {
        return FALSE;
    }

    /*  GIVEN */
    {
        /*  Card image */
        {
            pMYKI_CAControl->Status                                         = CARD_CONTROL_STATUS_ACTIVATED;
            pMYKI_OAControl->Status                                         = OAPP_CONTROL_STATUS_ACTIVATED;
            pMYKI_OAControl->StaffProviderId                                = ServiceProviderId + 1;    /*  != Device Service Provider */
            pMYKI_OAControl->RolesCount                                     = 1;
            pMYKI_OARoles->Role[ 0 ].Type                                   = RoleType_AO;
            pMYKI_OARoles->Role[ 0 ].Profile                                = RoleProfile_AO;
        }

        /*  BR context data */
        {
            pData->DynamicData.isAOCard                                     = FALSE;
            pData->DynamicData.currentZone                                  = Zone;
            pData->DynamicData.entryPointId                                 = EntryPointId;
            pData->DynamicData.stopId                                       = StopId;
            pData->DynamicData.lineId                                       = LineId;
            pData->DynamicData.currentTripDirection                         = Direction;
            pData->StaticData.deviceSamId                                   = SamId;
            pData->StaticData.serviceProviderId                             = ServiceProviderId;
            memset( pData->StaticData.staffRoles, 0, sizeof( pData->StaticData.staffRoles ) );
            pData->StaticData.staffRoles[ 0 ].type                          = RoleType_AO;
            pData->StaticData.staffRoles[ 0 ].profile                       = RoleProfile_AO;
            pData->StaticData.staffRoles[ 0 ].operatorType                  = OperatorType_AUTHORISED_OFFICER;
        }
    }

    /*  WHEN */
    {
        /*  Executes business rule */
        RuleResult  = BR_OSC_1_4( pData );
    }

    /*  THEN */
    {
        UT_Start( );

        /*  Make sure business rule is bypassed */
        UT_Assert( RuleResult == RULE_RESULT_BYPASSED );
        UT_Assert( pData->ReturnedData.bypassCode == BYPASS_CODE( 1, 4, 2, 0 ) );

        /*  Make sure card image not changed */
        /*         pMYKI_OAInspectorData->DriverId
                   pMYKI_OAInspectorData->ShiftId
                   pMYKI_OAInspectorData->ShiftSeqNo  */
        UT_Assert( pMYKI_OAInspectorData->SamId == 0 );
        UT_Assert( pMYKI_OAInspectorData->ProviderId == 0 );
        UT_Assert( pMYKI_OAInspectorData->BoardingLocation.EntryPointId == 0 );
        UT_Assert( pMYKI_OAInspectorData->BoardingLocation.RouteId == 0 );
        UT_Assert( pMYKI_OAInspectorData->BoardingLocation.StopId == 0 );
        UT_Assert( pMYKI_OAInspectorData->BoardingZone == 0 );
        /*         pMYKI_OAInspectorData->TripSeqNo */
        UT_Assert( pMYKI_OAInspectorData->TripDirection == 0 );
        UT_Assert( pMYKI_OAInspectorData->BoardingTime == 0 );
        /*         pMYKI_OAInspectorData->CardCount
                   pMYKI_OAInspectorData->PTFirst
                   pMYKI_OAInspectorData->PTCount
                   pMYKI_OAInspectorData->PTCancelled */

        return  UT_Result( );
    }
}   /*  BR_OSC_1_4_002c( ) */
