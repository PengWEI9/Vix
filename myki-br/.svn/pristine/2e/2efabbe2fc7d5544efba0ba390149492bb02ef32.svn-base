//=============================================================================
//
//  Stubbed replacements for myki_cardservices.
//
//=============================================================================

//-----------------------------------------------------------------------------
//  Includes
//-----------------------------------------------------------------------------

#include <stdio.h>
#include <myki_cardservices.h>
#include <myki_fs_serialise.h>
#include <cs.h>

//-----------------------------------------------------------------------------
//  Function-Local variables
//-----------------------------------------------------------------------------

static  MYKI_CAIssuer_t                 MYKI_CAIssuer;
static  MYKI_CAControl_t                MYKI_CAControl;
static  MYKI_TAIssuer_t                 MYKI_TAIssuer;
static  MYKI_TAControl_t                MYKI_TAControl;
static  MYKI_TACapping_t                MYKI_TACapping;
static  MYKI_TAUsageLogRecordList_t     MYKI_TAUsageLogRecordList;
static  MYKI_TALoadLogRecordList_t      MYKI_TALoadLogRecordList;
static  MYKI_TAProduct_t                MYKI_TAProduct[ MYKI_MAX_CONTRACTS ];
static  MYKI_TAPurseControl_t           MYKI_TAPurseControl;
static  MYKI_TAPurseBalance_t           MYKI_TAPurseBalance;
static  MYKI_OAIssuer_t                 MYKI_OAIssuer;
static  MYKI_OAControl_t                MYKI_OAControl;
static  MYKI_OAPIN_t                    MYKI_OAPin;
static  MYKI_OARoles_t                  MYKI_OARoles;
static  MYKI_OAInspectorData_t          MYKI_OAInspectorData;
static  MYKI_OAShiftDataControl_t       MYKI_OAShiftDataControl;
static  MYKI_OAShiftDataRecordList_t    MYKI_OAShiftDataRecordList;
static  MYKI_OAShiftLogRecordList_t     MYKI_OAShiftLogRecordList;

static  CT_CardInfo_t                   CT_CardInfo;

static  int     TA_Control_Read;
static  int     OA_Control_Read;
static  int     UsageLogsWritten;
static  int     LoadLogsWritten;

//=============================================================================
//
//  All of the following functions simply set the passed pointer to point to
//  the relevant in-memory structure. The data referenced by this pointer
//  can be modified at will. The ...Set() functions do not need to
//  be called, but are provided to complete the API.
//
//=============================================================================

int MYKI_CS_OpenCard(MYKI_CS_OpenCardMode_e mode, U8_t *pAppList)
{
    memset( &MYKI_CAIssuer              , 0, sizeof( MYKI_CAIssuer              ) );
    memset( &MYKI_CAControl             , 0, sizeof( MYKI_CAControl             ) );
    memset( &MYKI_TAIssuer              , 0, sizeof( MYKI_TAIssuer              ) );
    memset( &MYKI_TAControl             , 0, sizeof( MYKI_TAControl             ) );
    memset( &MYKI_TACapping             , 0, sizeof( MYKI_TACapping             ) );
    memset( &MYKI_TAUsageLogRecordList  , 0, sizeof( MYKI_TAUsageLogRecordList  ) );
    memset( &MYKI_TALoadLogRecordList   , 0, sizeof( MYKI_TALoadLogRecordList   ) );
    memset( &MYKI_TAProduct             , 0, sizeof( MYKI_TAProduct             ) );
    memset( &MYKI_TAPurseControl        , 0, sizeof( MYKI_TAPurseControl        ) );
    memset( &MYKI_TAPurseBalance        , 0, sizeof( MYKI_TAPurseBalance        ) );
    memset( &MYKI_OAIssuer              , 0, sizeof( MYKI_OAIssuer              ) );
    memset( &MYKI_OAControl             , 0, sizeof( MYKI_OAControl             ) );
    memset( &MYKI_OAPin                 , 0, sizeof( MYKI_OAPin                 ) );
    memset( &MYKI_OARoles               , 0, sizeof( MYKI_OARoles               ) );
    memset( &MYKI_OAInspectorData       , 0, sizeof( MYKI_OAInspectorData       ) );
    memset( &MYKI_OAShiftDataControl    , 0, sizeof( MYKI_OAShiftDataControl    ) );
    memset( &MYKI_OAShiftDataRecordList , 0, sizeof( MYKI_OAShiftDataRecordList ) );
    memset( &MYKI_OAShiftLogRecordList  , 0, sizeof( MYKI_OAShiftLogRecordList  ) );

    TA_Control_Read = FALSE;
    UsageLogsWritten = 0;
    LoadLogsWritten = 0;

    return MYKI_CS_OK;
}

int MYKI_CS_GetCardInfo(CT_CardInfo_t **pCT_CardInfo)
{
    *pCT_CardInfo = &CT_CardInfo;
    return MYKI_CS_OK;
}

int MYKI_CS_TAPurseBalanceGet(MYKI_TAPurseBalance_t **pMYKI_TAPurseBalance)
{
    *pMYKI_TAPurseBalance = &MYKI_TAPurseBalance;
    return MYKI_CS_OK;
}

int MYKI_CS_CAIssuerGet(MYKI_CAIssuer_t **pMYKI_CAIssuer)
{
    *pMYKI_CAIssuer = &MYKI_CAIssuer;
    return MYKI_CS_OK;
}

int MYKI_CS_CAControlGet(MYKI_CAControl_t **pMYKI_CAControl)
{
    *pMYKI_CAControl = &MYKI_CAControl;
    return MYKI_CS_OK;
}

int MYKI_CS_TAIssuerGet(MYKI_TAIssuer_t **pMYKI_TAIssuer)
{
    *pMYKI_TAIssuer = &MYKI_TAIssuer;
    return MYKI_CS_OK;
}

int MYKI_CS_TAControlGet(MYKI_TAControl_t **pMYKI_TAControl)
{
    CsVerbose( "MYKI_CS_TAControlGet()\n" );

    *pMYKI_TAControl = &MYKI_TAControl;
    TA_Control_Read = TRUE;
    return MYKI_CS_OK;
}

int MYKI_CS_TACappingGet(MYKI_TACapping_t **pMYKI_TACapping)
{
    *pMYKI_TACapping = &MYKI_TACapping;
    return MYKI_CS_OK;
}

int MYKI_CS_TAUsageLogRecordsGet(U8_t numberOfRecords, MYKI_TAUsageLogRecordList_t **pMYKI_TAUsageLogRecordList)
{
    *pMYKI_TAUsageLogRecordList = &MYKI_TAUsageLogRecordList;
    return MYKI_CS_OK;
}

int MYKI_CS_TALoadLogRecordsGet(U8_t numberOfRecords, MYKI_TALoadLogRecordList_t **pMYKI_TALoadLogRecordList)
{
    *pMYKI_TALoadLogRecordList = &MYKI_TALoadLogRecordList;
    return MYKI_CS_OK;
}

int MYKI_CS_TAProductGet(U8_t contractIndex, MYKI_TAProduct_t **pMYKI_TAProduct)
{
    *pMYKI_TAProduct = &MYKI_TAProduct[ contractIndex ];

    if ( ! TA_Control_Read )
    {
        CsErrx( "MYKI_CS_TAProductGet() TA Control not yet read\n" );
        return -1;
    }

    return MYKI_CS_OK;
}

int MYKI_CS_TAPurseControlGet(MYKI_TAPurseControl_t **pMYKI_TAPurseControl)
{
    *pMYKI_TAPurseControl = &MYKI_TAPurseControl;
    return MYKI_CS_OK;
}

int MYKI_CS_TAPurseBalanceSet(MYKI_TAPurseBalance_t **pMYKI_TAPurseBalance)
{
    return MYKI_CS_OK;
}

int MYKI_CS_CAIssuerSet()
{
    return MYKI_CS_OK;
}

int MYKI_CS_CAControlSet(MYKI_CAControl_t **pMYKI_CAControl)
{
    return MYKI_CS_OK;
}

int MYKI_CS_TAIssuerSet(MYKI_TAIssuer_t **pMYKI_TAIssuer)
{
    return MYKI_CS_OK;
}

int MYKI_CS_TAControlSet(MYKI_TAControl_t **pMYKI_TAControl)
{
    CsVerbose( "MYKI_CS_TAControlSet()\n" );
    return MYKI_CS_OK;
}

int MYKI_CS_TACappingSet(MYKI_TACapping_t **pMYKI_TACapping)
{
    return MYKI_CS_OK;
}

int MYKI_CS_TAUsageLogRecordCreate(MYKI_TAUsageLogRecord_t *pMYKI_TAUsageLogRecord)
{
    if ( UsageLogsWritten > 0 )
    {
        CsErrx( "MYKI_CS_TAUsageLogRecordCreate() Too many usage logs written\n" );
        return -1;
    }

    UsageLogsWritten++;
    return MYKI_CS_OK;
}

int MYKI_CS_TALoadLogRecordCreate(MYKI_TALoadLogRecord_t *pMYKI_TALoadLogRecord)
{
    if ( LoadLogsWritten > 0 )
    {
        CsErrx( "MYKI_CS_TALoadLogRecordCreate() Too many load logs written\n" );
        return -1;
    }

    LoadLogsWritten++;
    return MYKI_CS_OK;
}

int MYKI_CS_TAProductSet(U8_t contractIndex)
{
    return MYKI_CS_OK;
}

int MYKI_CS_TAPurseControlSet(MYKI_TAPurseControl_t **pMYKI_TAPurseControl)
{
    return MYKI_CS_OK;
}

int MYKI_CS_Commit(void)
{
    TA_Control_Read = FALSE;
    UsageLogsWritten = 0;
    LoadLogsWritten = 0;

    return MYKI_CS_OK;
}

int MYKI_CS_TAProductCreate(const MYKI_TAProduct_t *pMYKI_TAProduct, const MYKI_Directory_t *pMYKI_Directory)
{
    int     dir;

    for ( dir = 1; dir < 6; dir++ )
    {
        if ( MYKI_TAControl.Directory[ dir ].Status == 0 )
        {
            MYKI_TAControl.Directory[ dir ] = *pMYKI_Directory;
            MYKI_TAProduct[ dir - 1 ] = *pMYKI_TAProduct;
            break;
        }
    }

    return MYKI_CS_OK;
}

int MYKI_CS_TAProductDelete(U8_t contractIndex)
{
    memset( &MYKI_TAControl.Directory[ contractIndex + 1 ], 0, sizeof( MYKI_TAControl.Directory[ contractIndex ] ) );
    memset( &MYKI_TAProduct[ contractIndex ], 0, sizeof( MYKI_TAProduct[ contractIndex - 1 ] ) );

    return MYKI_CS_OK;
}

int MYKI_CS_PurseDebit(U32_t Value)
{
    return MYKI_CS_OK;
}

int MYKI_CS_PurseCredit(U32_t Value)
{
    return MYKI_CS_OK;
}

int MYKI_CS_OAIssuerGet(MYKI_OAIssuer_t **pMYKI_OAIssuer)
{
    *pMYKI_OAIssuer = &MYKI_OAIssuer;
    return MYKI_CS_OK;
}

int MYKI_CS_OAControlGet(MYKI_OAControl_t **pMYKI_OAControl)
{
    CsVerbose( "MYKI_CS_OAControlGet()\n" );

    *pMYKI_OAControl = &MYKI_OAControl;
    OA_Control_Read = TRUE;
    return MYKI_CS_OK;
}

int MYKI_CS_OAControlSet(MYKI_OAControl_t **pMYKI_OAControl)
{
    CsVerbose( "MYKI_CS_OAControlSet()\n" );
    return MYKI_CS_OK;
}

int MYKI_CS_OAPINGet(MYKI_OAPIN_t **pMYKI_OAPin)
{
    *pMYKI_OAPin = &MYKI_OAPin;
    return MYKI_CS_OK;
}

int MYKI_CS_OARolesGet(MYKI_OARoles_t **pMYKI_OARoles)
{
    *pMYKI_OARoles = &MYKI_OARoles;
    return MYKI_CS_OK;
}

int MYKI_CS_OAInspectorDataGet(MYKI_OAInspectorData_t **pMYKI_OAInspectorData)
{
    *pMYKI_OAInspectorData = &MYKI_OAInspectorData;
    return MYKI_CS_OK;
}

int MYKI_CS_OAInspectorDataSet()
{
    CsVerbose( "MYKI_CS_OAInspectorDataSet()\n" );
    return MYKI_CS_OK;
}

int MYKI_CS_OAPINSet()
{
    CsVerbose( "MYKI_CS_OAPINSet()\n" );
    return MYKI_CS_OK;
}

int MYKI_CS_OAShiftDataControlGet(MYKI_OAShiftDataControl_t **pMYKI_OAShiftDataControl)
{
    *pMYKI_OAShiftDataControl   = &MYKI_OAShiftDataControl;
    return  MYKI_CS_OK;
}

int MYKI_CS_OAShiftDataControlSet()
{
    return  MYKI_CS_OK;
}

int MYKI_CS_OAShiftDataRecordsGet(U8_t numberOfRecords, MYKI_OAShiftDataRecordList_t **pMYKI_OAShiftDataRecordList)
{
    *pMYKI_OAShiftDataRecordList    = &MYKI_OAShiftDataRecordList;
    if ( numberOfRecords < MYKI_OAShiftDataRecordList.NumberOfRecords )
    {
        MYKI_OAShiftDataRecordList.NumberOfRecords  = numberOfRecords;
    }
    return  MYKI_CS_OK;
}

int MYKI_CS_OAShiftDataRecordCreate(int recordIndex, MYKI_OAShiftDataRecord_t *pMYKI_OAShiftDataRecord)
{
    if ( recordIndex < MYKI_MAX_SHIFTDATA_RECORDS )
    {
        memcpy( &MYKI_OAShiftDataRecordList.MYKI_ShiftDataRecord[ recordIndex ],
                pMYKI_OAShiftDataRecord,
                sizeof( MYKI_OAShiftDataRecord_t ) );
        return  MYKI_CS_OK;
    }
    return  MYKI_CS_ERR_NO_OBJECT;
}

int MYKI_CS_OAShiftLogRecordsGet(U8_t numberOfRecords, MYKI_OAShiftLogRecordList_t **pMYKI_OAShiftLogRecordList)
{
    *pMYKI_OAShiftLogRecordList = &MYKI_OAShiftLogRecordList;
    if ( numberOfRecords < MYKI_OAShiftLogRecordList.NumberOfRecords )
    {
        MYKI_OAShiftLogRecordList.NumberOfRecords   = numberOfRecords;
    }
    return  MYKI_CS_OK;
}

int MYKI_CS_OAShiftLogRecordCreate(MYKI_OAShiftLogRecord_t *pMYKI_OAShiftLogRecord)
{
    return  MYKI_CS_OK;
}

int MYKI_FS_SerialiseToFile( FILE *pFileOut )
{
    CsVerbose( "MYKI_FS_SerialiseToFile()" );
    return 0;
}
