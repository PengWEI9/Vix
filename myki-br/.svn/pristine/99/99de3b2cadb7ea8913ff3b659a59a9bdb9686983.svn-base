//=============================================================================
//
//  Functions common to one or more tests.
//
//=============================================================================

#include <cs.h>
#include <myki_cardservices.h>

#include <myki_br.h>        // For testing sequences
#include <myki_br_rules.h>  // For testing individual rules
#include <myki_br_context_data.h>
#include <myki_br_card_constants.h>

#include <myki_cdd_enums.h>

#include "test_common.h"

//-----------------------------------------------------------------------------
//
//  Constants
//
//-----------------------------------------------------------------------------

//=============================================================================
//
//
//
//=============================================================================

void    ShowCardImage( char *filename )
{
    FILE                        *fp;
    int                         i;
    MYKI_CAIssuer_t             *pMYKI_CAIssuer;
    MYKI_CAControl_t            *pMYKI_CAControl;
    MYKI_TAIssuer_t             *pMYKI_TAIssuer;
    MYKI_TAControl_t            *pMYKI_TAControl;
    MYKI_TACapping_t            *pMYKI_TACapping;
    MYKI_TAUsageLogRecordList_t *pMYKI_TAUsageLogRecordList;
    MYKI_TALoadLogRecordList_t  *pMYKI_TALoadLogRecordList;
    MYKI_TAProduct_t            *pMYKI_TAProduct[ DIMOF( pMYKI_TAControl->Directory ) - 1 ];
    MYKI_TAPurseControl_t       *pMYKI_TAPurseControl;
    MYKI_TAPurseBalance_t       *pMYKI_TAPurseBalance;
    MYKI_OAIssuer_t             *pMYKI_OAIssuer;
    MYKI_OAControl_t            *pMYKI_OAControl;
    MYKI_OAPIN_t                *pMYKI_OAPin;

    if ( ! filename )
        return;

    if ( ( fp = fopen( filename, "wt" ) ) == NULL )
        return;

    fprintf( fp, "Card Image\n" );

    if ( MYKI_CS_CAIssuerGet( &pMYKI_CAIssuer ) < 0 )
    {
        CsErrx( "MYKI_CS_CAIssuerGet() failed" );
        return;
    }

    if ( MYKI_CS_CAControlGet( &pMYKI_CAControl ) < 0 )
    {
        CsErrx( "MYKI_CS_CAControlGet() failed" );
        return;
    }

    if ( MYKI_CS_TAIssuerGet( &pMYKI_TAIssuer ) < 0 )
    {
        CsErrx( "MYKI_CS_TAIssuerGet() failed" );
        return;
    }

    if ( MYKI_CS_TAControlGet( &pMYKI_TAControl ) < 0 )
    {
        CsErrx( "MYKI_CS_TAControlGet() failed" );
        return;
    }

    if ( MYKI_CS_TACappingGet( &pMYKI_TACapping ) < 0 )
    {
        CsErrx( "MYKI_CS_TACappingGet() failed" );
        return;
    }

    if ( MYKI_CS_TAUsageLogRecordsGet( 0xff, &pMYKI_TAUsageLogRecordList ) < 0 )       // Number of records
    {
        CsErrx( "MYKI_CS_TAUsageLogRecordsGet() failed" );
        return;
    }

    if ( MYKI_CS_TALoadLogRecordsGet( 0xff, &pMYKI_TALoadLogRecordList ) < 0 )         // Number of records
    {
        CsErrx( "MYKI_CS_TALoadLogRecordsGet() failed" );
        return;
    }

    for ( i = 0; i < DIMOF( pMYKI_TAProduct ); i++ )
    {
        if ( MYKI_CS_TAProductGet( (U8_t)( i ), &pMYKI_TAProduct[ i ] ) < 0 )          // Contract index (0-4)
        {
            CsErrx( "MYKI_CS_TAProductGet( %d ) failed", i );
            return;
        }
    }

    if ( MYKI_CS_TAPurseControlGet( &pMYKI_TAPurseControl ) < 0 )
    {
        CsErrx( "MYKI_CS_TAPurseControlGet() failed" );
        return;
    }

    if ( MYKI_CS_TAPurseBalanceGet( &pMYKI_TAPurseBalance ) < 0 )
    {
        CsErrx( "MYKI_CS_TAPurseBalanceGet() failed" );
        return;
    }

    if ( MYKI_CS_OAIssuerGet( &pMYKI_OAIssuer ) < 0 )
    {
        CsErrx( "MYKI_CS_OAIssuerGet() failed" );
        return;
    }

    if ( MYKI_CS_OAControlGet( &pMYKI_OAControl ) < 0 )
    {
        CsErrx( "MYKI_CS_OAControlGet() failed" );
        return;
    }

    if ( MYKI_CS_OAPINGet( &pMYKI_OAPin ) < 0 )
    {
        CsErrx( "MYKI_CS_OAPINGet() failed" );
        return;
    }

    fprintf( fp, "  CAIssuer\n" );
    fprintf( fp, "    IssuerId                %d\n", pMYKI_CAIssuer->IssuerId             );
    fprintf( fp, "    CSN                     %d\n", pMYKI_CAIssuer->CSN                  );
    fprintf( fp, "    Certificate             %d\n", pMYKI_CAIssuer->Certificate          );
    fprintf( fp, "    Version                 %d\n", pMYKI_CAIssuer->Version              );
    fprintf( fp, "    OperationMode           %d\n", pMYKI_CAIssuer->OperationMode        );
    fprintf( fp, "    CardStockType           %d\n", pMYKI_CAIssuer->CardStockType        );
    fprintf( fp, "    ControlLength           %d\n", pMYKI_CAIssuer->ControlLength        );
    fprintf( fp, "    ControlLocation         %d\n", pMYKI_CAIssuer->ControlLocation      );
    fprintf( fp, "    Keyset                  %d\n", pMYKI_CAIssuer->Keyset               );
    fprintf( fp, "    Reserved                %d\n", pMYKI_CAIssuer->Reserved             );

    fprintf( fp, "  CAControl\n" );
    fprintf( fp, "    ExpiryDate              %d\n", pMYKI_CAControl->ExpiryDate          );
    fprintf( fp, "    BlockingReason          %d\n", pMYKI_CAControl->BlockingReason      );
    fprintf( fp, "    Status                  %d\n", pMYKI_CAControl->Status              );
    fprintf( fp, "    ActionSeqNo             %d\n", pMYKI_CAControl->ActionSeqNo         );
    fprintf( fp, "    IssueCount              %d\n", pMYKI_CAControl->IssueCount          );
    fprintf( fp, "    CurrencyCode            %d\n", pMYKI_CAControl->CurrencyCode        );
    fprintf( fp, "    Deposit                 %d\n", pMYKI_CAControl->Deposit             );
    fprintf( fp, "    NextTxSeqNo             %d\n", pMYKI_CAControl->NextTxSeqNo         );
    fprintf( fp, "    Reserved                %d\n", pMYKI_CAControl->Reserved            );
    fprintf( fp, "    Checksum                %d\n", pMYKI_CAControl->Checksum            );

    fprintf( fp, "  TAIssuer\n" );
    fprintf( fp, "    IssuerId                %d,%d\n", pMYKI_TAIssuer->IssuerId.High8, pMYKI_TAIssuer->IssuerId.Low32 );
    fprintf( fp, "    Certificate             %d\n", pMYKI_TAIssuer->Certificate          );
    fprintf( fp, "    Version                 %d\n", pMYKI_TAIssuer->Version              );
    fprintf( fp, "    OperationMode           %d\n", pMYKI_TAIssuer->OperationMode        );
    fprintf( fp, "    UsageLogLocation        %d\n", pMYKI_TAIssuer->UsageLogLocation     );
    fprintf( fp, "    UsageLogLength          %d\n", pMYKI_TAIssuer->UsageLogLength       );
    fprintf( fp, "    UsageLogCount           %d\n", pMYKI_TAIssuer->UsageLogCount        );
    fprintf( fp, "    LoadLogLocation         %d\n", pMYKI_TAIssuer->LoadLogLocation      );
    fprintf( fp, "    LoadLogLength           %d\n", pMYKI_TAIssuer->LoadLogLength        );
    fprintf( fp, "    LoadLogCount            %d\n", pMYKI_TAIssuer->LoadLogCount         );
    fprintf( fp, "    ControlLocation         %d\n", pMYKI_TAIssuer->ControlLocation      );
    fprintf( fp, "    ControlLength           %d\n", pMYKI_TAIssuer->ControlLength        );
    fprintf( fp, "    ControlDirCount         %d\n", pMYKI_TAIssuer->ControlDirCount      );
    fprintf( fp, "    Keyset                  %d\n", pMYKI_TAIssuer->Keyset               );
    fprintf( fp, "    CappingLocation         %d\n", pMYKI_TAIssuer->CappingLocation      );
    fprintf( fp, "    CappingLength           %d\n", pMYKI_TAIssuer->CappingLength        );

    fprintf( fp, "  TAControl\n" );
    fprintf( fp, "    ExpiryDate              %d\n", pMYKI_TAControl->ExpiryDate          );
    fprintf( fp, "    BlockingReason          %d\n", pMYKI_TAControl->BlockingReason      );
    fprintf( fp, "    Status                  %d\n", pMYKI_TAControl->Status              );
    fprintf( fp, "    PassengerCode           %d\n", pMYKI_TAControl->PassengerCode       );
    fprintf( fp, "    PassengerCodeExpiry     %d\n", pMYKI_TAControl->PassengerCodeExpiry );
    fprintf( fp, "    PassengerControl        %d\n", pMYKI_TAControl->PassengerControl    );
    fprintf( fp, "    ActionSeqNo             %d\n", pMYKI_TAControl->ActionSeqNo         );
    fprintf( fp, "    Language                %d\n", pMYKI_TAControl->Language            );
    fprintf( fp, "    ProductInUse            %d\n", pMYKI_TAControl->ProductInUse        );
    fprintf( fp, "    ProvisionalFare         %d\n", pMYKI_TAControl->ProvisionalFare     );
    fprintf( fp, "    ActiveRecordFlags       %d\n", pMYKI_TAControl->ActiveRecordFlags   );
    fprintf( fp, "    NextTxSeqNo             %d\n", pMYKI_TAControl->NextTxSeqNo         );
    fprintf( fp, "    NextProductSerialNo     %d\n", pMYKI_TAControl->NextProductSerialNo );
    fprintf( fp, "    Checksum                %d\n", pMYKI_TAControl->Checksum            );
    for ( i = 0; i < DIMOF( pMYKI_TAControl->Directory ); i++ )
    {
        fprintf( fp, "    Directory[ %d ]\n", i );
        fprintf( fp, "      IssuerId              %d\n", pMYKI_TAControl->Directory[ i ].IssuerId   );
        fprintf( fp, "      ProductId             %d\n", pMYKI_TAControl->Directory[ i ].ProductId  );
        fprintf( fp, "      SerialNo              %d\n", pMYKI_TAControl->Directory[ i ].SerialNo   );
        fprintf( fp, "      Version               %d\n", pMYKI_TAControl->Directory[ i ].Version    );
        fprintf( fp, "      Status                %d\n", pMYKI_TAControl->Directory[ i ].Status     );
        fprintf( fp, "      Location              %d\n", pMYKI_TAControl->Directory[ i ].Location   );
        fprintf( fp, "      Length                %d\n", pMYKI_TAControl->Directory[ i ].Length     );
    }

    fprintf( fp, "  TACapping\n" );
    fprintf( fp, "     Version                %d\n", pMYKI_TACapping->Version                       );
    fprintf( fp, "     WeeklyMode             %d\n", pMYKI_TACapping->WeeklyMode                    );
    fprintf( fp, "     Daily Expiry           %d\n", pMYKI_TACapping->Daily.Expiry                  );
    fprintf( fp, "     Daily Value            %d\n", pMYKI_TACapping->Daily.Value                   );
    fprintf( fp, "     Daily Zone Low         %d\n", pMYKI_TACapping->Daily.Zone.Low                );
    fprintf( fp, "     Daily Zone High        %d\n", pMYKI_TACapping->Daily.Zone.High               );
    fprintf( fp, "     Weekly Expiry          %d\n", pMYKI_TACapping->Weekly.Expiry                 );
    fprintf( fp, "     Weekly Value           %d\n", pMYKI_TACapping->Weekly.Value                  );
    for ( i = 0; i < DIMOF( pMYKI_TACapping->Weekly.Zone ); i++ )
    {
        fprintf( fp, "     Weekly Zone[ %d ] Low   %d\n", i, pMYKI_TACapping->Weekly.Zone[ i ].Low   );
        fprintf( fp, "     Weekly Zone[ %d ] High  %d\n", i, pMYKI_TACapping->Weekly.Zone[ i ].High  );
    }
    fprintf( fp, "     Reserved0              %d\n", pMYKI_TACapping->Reserved0                     );
    fprintf( fp, "     Reserved1              %d\n", pMYKI_TACapping->Reserved1                     );
    fprintf( fp, "     Checksum               %d\n", pMYKI_TACapping->Checksum                      );

    fprintf( fp, "  TAPurseControl\n" );
    fprintf( fp, "    ValueLocation           %d\n", pMYKI_TAPurseControl->ValueLocation            );
    fprintf( fp, "    ActionSeqNo             %d\n", pMYKI_TAPurseControl->ActionSeqNo              );
    fprintf( fp, "    ControlBitmap           %d\n", pMYKI_TAPurseControl->ControlBitmap            );
    fprintf( fp, "    NextTxSeqNo             %d\n", pMYKI_TAPurseControl->NextTxSeqNo              );
    fprintf( fp, "    AutoThreshold           %d\n", pMYKI_TAPurseControl->AutoThreshold            );
    fprintf( fp, "    AutoValue               %d\n", pMYKI_TAPurseControl->AutoValue                );
    fprintf( fp, "    LastChangeTxSeqNo       %d\n", pMYKI_TAPurseControl->LastChangeTxSeqNo        );
    fprintf( fp, "    LastChangeProviderId    %d\n", pMYKI_TAPurseControl->LastChangeProviderId     );
    fprintf( fp, "    LastChangeDate          %d\n", pMYKI_TAPurseControl->LastChangeDate           );
    fprintf( fp, "    LastChangeNewBalance    %d\n", pMYKI_TAPurseControl->LastChangeNewBalance     );
    fprintf( fp, "    LastChangeCertificate   %d\n", pMYKI_TAPurseControl->LastChangeCertificate    );
    fprintf( fp, "    LastChangeSamId         %d\n", pMYKI_TAPurseControl->LastChangeSamId          );
    fprintf( fp, "    Checksum                %d\n", pMYKI_TAPurseControl->Checksum                 );

    fprintf( fp, "  TAPurseBalance\n" );
    fprintf( fp, "    Balance                 %d\n", pMYKI_TAPurseBalance->Balance                  );

    for ( i = 0; i < DIMOF( pMYKI_TAProduct ); i++ )
    {
        fprintf( fp, "  TAProduct[ %d ]\n", i );
        fprintf( fp, "    ActionSeqNo             %d\n", pMYKI_TAProduct[ i ]->ActionSeqNo      );
        fprintf( fp, "    InstanceCount           %d\n", pMYKI_TAProduct[ i ]->InstanceCount    );
        fprintf( fp, "    ControlBitmap           %d\n", pMYKI_TAProduct[ i ]->ControlBitmap    );
        fprintf( fp, "    NextTxSeqNo             %d\n", pMYKI_TAProduct[ i ]->NextTxSeqNo      );
        fprintf( fp, "    PurchaseValue           %d\n", pMYKI_TAProduct[ i ]->PurchaseValue    );
        fprintf( fp, "    ZoneLow                 %d\n", pMYKI_TAProduct[ i ]->ZoneLow          );
        fprintf( fp, "    ZoneHigh                %d\n", pMYKI_TAProduct[ i ]->ZoneHigh         );
        fprintf( fp, "    StartDateTime           %d\n", pMYKI_TAProduct[ i ]->StartDateTime    );
        fprintf( fp, "    EndDateTime             %d\n", pMYKI_TAProduct[ i ]->EndDateTime      );
        fprintf( fp, "    Checksum                %d\n", pMYKI_TAProduct[ i ]->Checksum         );
        fprintf( fp, "    LU DateTime             %d\n", pMYKI_TAProduct[ i ]->LastUsage.DateTime               );
        fprintf( fp, "    LU ProviderId           %d\n", pMYKI_TAProduct[ i ]->LastUsage.ProviderId             );
        fprintf( fp, "    LU Zone                 %d\n", pMYKI_TAProduct[ i ]->LastUsage.Zone                   );
        fprintf( fp, "    LU EntryPointId         %d\n", pMYKI_TAProduct[ i ]->LastUsage.Location.EntryPointId  );
        fprintf( fp, "    LU RouteId              %d\n", pMYKI_TAProduct[ i ]->LastUsage.Location.RouteId       );
        fprintf( fp, "    LU StopId               %d\n", pMYKI_TAProduct[ i ]->LastUsage.Location.StopId        );
    }

    fprintf( fp, "  OAIssuer\n" );
    fprintf( fp, "    IssuerId                 %d,%d\n", pMYKI_OAIssuer->IssuerId.High8, pMYKI_OAIssuer->IssuerId.Low32 );
    fprintf( fp, "    Certificate              %d\n", pMYKI_OAIssuer->Certificate              );
    fprintf( fp, "    Version                  %d\n", pMYKI_OAIssuer->Version                  );
    fprintf( fp, "    OperationMode            %d\n", pMYKI_OAIssuer->OperationMode            );
    fprintf( fp, "    ControlLocation          %d\n", pMYKI_OAIssuer->ControlLocation          );
    fprintf( fp, "    ControlLength            %d\n", pMYKI_OAIssuer->ControlLength            );
    fprintf( fp, "    PinLocation              %d\n", pMYKI_OAIssuer->PinLocation              );
    fprintf( fp, "    PinLength                %d\n", pMYKI_OAIssuer->PinLength                );
    fprintf( fp, "    ShiftDataControlLocation %d\n", pMYKI_OAIssuer->ShiftDataControlLocation );
    fprintf( fp, "    ShiftDataControlLength   %d\n", pMYKI_OAIssuer->ShiftDataControlLength   );
    fprintf( fp, "    InspectorDataLocation    %d\n", pMYKI_OAIssuer->InspectorDataLocation    );
    fprintf( fp, "    InspectorDataLength      %d\n", pMYKI_OAIssuer->InspectorDataLength      );
    fprintf( fp, "    ShiftLogLocation         %d\n", pMYKI_OAIssuer->ShiftLogLocation         );
    fprintf( fp, "    ShiftLogLength           %d\n", pMYKI_OAIssuer->ShiftLogLength           );
    fprintf( fp, "    Keyset                   %d\n", pMYKI_TAIssuer->Keyset                   );


    fprintf( fp, "  OAControl\n" );
    fprintf( fp, "    ExpiryDate              %d\n", pMYKI_OAControl->ExpiryDate          );
    fprintf( fp, "    BlockingReason          %d\n", pMYKI_OAControl->BlockingReason      );
    fprintf( fp, "    Status                  %d\n", pMYKI_OAControl->Status              );
    fprintf( fp, "    UserControl             %d\n", pMYKI_OAControl->UserControl         );
    fprintf( fp, "    ActionSeqNo             %d\n", pMYKI_OAControl->ActionSeqNo         );
    fprintf( fp, "    InvalidPinCount         %d\n", pMYKI_OAControl->InvalidPinCount     );
    fprintf( fp, "    RolesLocation           %d\n", pMYKI_OAControl->RolesLocation       );
    fprintf( fp, "    RolesLength             %d\n", pMYKI_OAControl->RolesLength         );
    fprintf( fp, "    RolesCount              %d\n", pMYKI_OAControl->RolesCount          );
    fprintf( fp, "    RolesDefault            %d\n", pMYKI_OAControl->RolesDefault        );
    fprintf( fp, "    StaffId                 %.8s\n", pMYKI_OAControl->StaffId             );
    fprintf( fp, "    StaffExpDate            %d\n", pMYKI_OAControl->StaffExpDate        );
    fprintf( fp, "    StaffProviderId         %d\n", pMYKI_OAControl->StaffProviderId     );
    fprintf( fp, "    StaffBaseId             %d\n", pMYKI_OAControl->StaffBaseId         );
    fprintf( fp, "    NextTxSeqNo             %d\n", pMYKI_OAControl->NextTxSeqNo         );
    fprintf( fp, "    Checksum                %d\n", pMYKI_OAControl->Checksum            );

    fclose( fp );
}

//=============================================================================
//
//  Create an empty "virgin" card image, with :
//  -   Zero purse balance
//  -   No products (provisional or otherwise)
//  -   No product in use
//
//  This can be called at the start of each unit test to set up a virgin card
//  image which can then be modified as needed. It also provides pointers to
//  each of the card image components (by filling in the passed-in structure)
//  so each unit test does not need to call MYKI_CS_xxxGet().
//
//=============================================================================

int     CreateCardImage_Empty( CardImage_t *pCardImage )
{
    int     i;

    //  Card App Control

    if ( MYKI_CS_CAControlGet( &pCardImage->pMYKI_CAControl ) < 0 )
    {
        CsErrx( "MYKI_CS_CAControlGet() failed" );
        return -1;
    }

    ZERO( *pCardImage->pMYKI_CAControl );

    pCardImage->pMYKI_CAControl->Status = CARD_CONTROL_STATUS_ACTIVATED;

    //  Transit App Control

    if ( MYKI_CS_TAControlGet( &pCardImage->pMYKI_TAControl ) < 0 )
    {
        CsErrx( "MYKI_CS_TAControlGet() failed" );
        return -1;
    }

    ZERO( *pCardImage->pMYKI_TAControl );

    pCardImage->pMYKI_TAControl->Status = TAPP_CONTROL_STATUS_ACTIVATED;
    pCardImage->pMYKI_TAControl->NextProductSerialNo = 1;

    //  Transit App Products

    for ( i = 0; i < DIMOF( pCardImage->pMYKI_TAProduct ); i++ )
    {
        if ( MYKI_CS_TAProductGet( (U8_t)i, &pCardImage->pMYKI_TAProduct[ i ] ) < 0 )                             // Contract index (0-4)
        {
            CsErrx( "MYKI_CS_TAProductGet( %d ) failed", i );
            return -1;
        }

        ZERO( *pCardImage->pMYKI_TAProduct[ i ] );
    }

    //  Transit App Purse Balance

    if ( MYKI_CS_TAPurseBalanceGet( &pCardImage->pMYKI_TAPurseBalance ) < 0 )
    {
        CsErrx( "MYKI_CS_TAPurseBalanceGet() failed" );
        return -1;
    }

    if ( MYKI_CS_TAPurseControlGet( &pCardImage->pMYKI_TAPurseControl ) < 0 )
    {
        CsErrx( "MYKI_CS_TAPurseControlGet() failed" );
        return -1;
    }

    ZERO( *pCardImage->pMYKI_TAPurseBalance );
    ZERO( *pCardImage->pMYKI_TAPurseControl );

    return 0;
}

//=============================================================================
//
//  Create card image 1 - Normal card with multiple products, not scanned on
//
//=============================================================================

void    CardImage1( MYKI_BR_ContextData_t *pData )
{
    MYKI_CAControl_t           *pMYKI_CAControl         = NULL;
    MYKI_TAControl_t           *pMYKI_TAControl         = NULL;
    MYKI_TAProduct_t           *pMYKI_TAProduct[ 5 ]    = { NULL };
    MYKI_TAPurseBalance_t      *pMYKI_TAPurseBalance    = NULL;
    MYKI_TAPurseControl_t      *pMYKI_TAPurseControl    = NULL;
    int                         i;

    if ( MYKI_CS_CAControlGet( &pMYKI_CAControl ) < 0 )
    {
        CsErrx( "MYKI_CS_CAControlGet() failed" );
        return;
    }

    if ( MYKI_CS_TAControlGet( &pMYKI_TAControl ) < 0 )
    {
        CsErrx( "MYKI_CS_TAControlGet() failed" );
        return;
    }

    for ( i = 0; i < DIMOF( pMYKI_TAProduct ); i++ )
    {
        if ( MYKI_CS_TAProductGet( (U8_t)i, &pMYKI_TAProduct[ i ] ) < 0 )                             // Contract index (0-4)
        {
            CsErrx( "MYKI_CS_TAProductGet( %d ) failed", i );
            return;
        }
    }

    if ( MYKI_CS_TAPurseBalanceGet( &pMYKI_TAPurseBalance ) < 0 )
    {
        CsErrx( "MYKI_CS_TAPurseBalanceGet() failed" );
        return;
    }

    if ( MYKI_CS_TAPurseControlGet( &pMYKI_TAPurseControl ) < 0 )
    {
        CsErrx( "MYKI_CS_TAPurseControlGet() failed" );
        return;
    }

    //  Card

    pMYKI_CAControl->Status                       = CARD_CONTROL_STATUS_ACTIVATED;
    pMYKI_CAControl->ExpiryDate                   = pData->DynamicData.currentBusinessDate + 1;

    pMYKI_TAControl->Status                       = TAPP_CONTROL_DIRECTORY_STATUS_ACTIVATED;
    pMYKI_TAControl->ExpiryDate                   = pData->DynamicData.currentBusinessDate + 1;
    pMYKI_TAControl->NextProductSerialNo          = 1;
    pMYKI_TAControl->ProductInUse                 = 0;

    //  Directory Entry 0 - T-Purse

    pMYKI_TAControl->Directory[ 0 ].SerialNo      = 0;
    pMYKI_TAControl->Directory[ 0 ].Status        = TAPP_CONTROL_DIRECTORY_STATUS_ACTIVATED;
    pMYKI_TAControl->Directory[ 0 ].IssuerId      = 0;
    pMYKI_TAControl->Directory[ 0 ].ProductId     = 0;

    pMYKI_TAPurseBalance->Balance                 = 1000;
    pMYKI_TAPurseControl->LastChangeNewBalance    = pMYKI_TAPurseBalance->Balance;
    pMYKI_TAPurseControl->NextTxSeqNo             = 3;
    pMYKI_TAPurseControl->LastChangeTxSeqNo       = pMYKI_TAPurseControl->NextTxSeqNo - 1;

    //  Directory Entry 1 - Product 0

    pMYKI_TAControl->Directory[ 1 ].SerialNo      = 1;
    pMYKI_TAControl->Directory[ 1 ].Status        = TAPP_CONTROL_DIRECTORY_STATUS_ACTIVATED;
    pMYKI_TAControl->Directory[ 1 ].IssuerId      = 0;
    pMYKI_TAControl->Directory[ 1 ].ProductId     = PRODUCT_ID_SINGLE;

    pMYKI_TAProduct[ 0 ]->StartDateTime           = pData->DynamicData.currentDateTime - HOURS_TO_SECONDS( 1 );
    pMYKI_TAProduct[ 0 ]->EndDateTime             = pMYKI_TAProduct[ 0 ]->StartDateTime + HOURS_TO_SECONDS( 24 );

    //  Directory Entry 2 - Product 1

    pMYKI_TAControl->Directory[ 2 ].SerialNo      = 2;
    pMYKI_TAControl->Directory[ 2 ].Status        = TAPP_CONTROL_DIRECTORY_STATUS_ACTIVATED;
    pMYKI_TAControl->Directory[ 2 ].IssuerId      = 0;
    pMYKI_TAControl->Directory[ 2 ].ProductId     = PRODUCT_ID_DAILY;

    pMYKI_TAProduct[ 1 ]->StartDateTime           = pData->DynamicData.currentDateTime - HOURS_TO_SECONDS( 1 );
    pMYKI_TAProduct[ 1 ]->EndDateTime             = pMYKI_TAProduct[ 1 ]->StartDateTime + HOURS_TO_SECONDS( 24 );

    //  Directory Entry 3 - Product 2

    pMYKI_TAControl->Directory[ 3 ].SerialNo      = 3;
    pMYKI_TAControl->Directory[ 3 ].Status        = TAPP_CONTROL_DIRECTORY_STATUS_ACTIVATED;
    pMYKI_TAControl->Directory[ 3 ].IssuerId      = 0;
    pMYKI_TAControl->Directory[ 3 ].ProductId     = PRODUCT_ID_WEEKLY;

    pMYKI_TAProduct[ 2 ]->StartDateTime           = pData->DynamicData.currentDateTime - HOURS_TO_SECONDS( 1 );
    pMYKI_TAProduct[ 2 ]->EndDateTime             = pMYKI_TAProduct[ 2 ]->StartDateTime + DAYS_TO_SECONDS( 8 );

    //  Directory Entry 4 - Product 3

    pMYKI_TAControl->Directory[ 4 ].SerialNo      = 4;
    pMYKI_TAControl->Directory[ 4 ].Status        = TAPP_CONTROL_DIRECTORY_STATUS_UNUSED;
    pMYKI_TAControl->Directory[ 4 ].IssuerId      = 0;
    pMYKI_TAControl->Directory[ 4 ].ProductId     = 0;

    pMYKI_TAProduct[ 3 ]->StartDateTime           = pData->DynamicData.currentDateTime;
    pMYKI_TAProduct[ 3 ]->EndDateTime             = pMYKI_TAProduct[ 3 ]->StartDateTime;

    //  Directory Entry 5 - Product 4

    pMYKI_TAControl->Directory[ 5 ].SerialNo      = 0;
    pMYKI_TAControl->Directory[ 5 ].Status        = TAPP_CONTROL_DIRECTORY_STATUS_UNUSED;
    pMYKI_TAControl->Directory[ 5 ].IssuerId      = 0;
    pMYKI_TAControl->Directory[ 5 ].ProductId     = 0;

    pMYKI_TAProduct[ 4 ]->StartDateTime           = pData->DynamicData.currentDateTime;
    pMYKI_TAProduct[ 4 ]->EndDateTime             = pMYKI_TAProduct[ 4 ]->StartDateTime;
}

//=============================================================================
//
//  Create card image 2 - Normal card with multiple products with correctable errors
//
//=============================================================================

void    CardImage2( MYKI_BR_ContextData_t *pData )
{
    MYKI_CAControl_t           *pMYKI_CAControl         = NULL;
    MYKI_TAControl_t           *pMYKI_TAControl         = NULL;
    MYKI_TAProduct_t           *pMYKI_TAProduct[ 5 ]    = { NULL };
    MYKI_TAPurseBalance_t      *pMYKI_TAPurseBalance    = NULL;
    MYKI_TAPurseControl_t      *pMYKI_TAPurseControl    = NULL;
    int                         i;

    if ( MYKI_CS_CAControlGet( &pMYKI_CAControl ) < 0 )
    {
        CsErrx( "MYKI_CS_CAControlGet() failed" );
        return;
    }

    if ( MYKI_CS_TAControlGet( &pMYKI_TAControl ) < 0 )
    {
        CsErrx( "MYKI_CS_TAControlGet() failed" );
        return;
    }

    for ( i = 0; i < DIMOF( pMYKI_TAProduct ); i++ )
    {
        if ( MYKI_CS_TAProductGet( (U8_t)i, &pMYKI_TAProduct[ i ] ) < 0 )                             // Contract index (0-4)
        {
            CsErrx( "MYKI_CS_TAProductGet( %d ) failed", i );
            return;
        }
    }

    if ( MYKI_CS_TAPurseBalanceGet( &pMYKI_TAPurseBalance ) < 0 )
    {
        CsErrx( "MYKI_CS_TAPurseBalanceGet() failed" );
        return;
    }

    if ( MYKI_CS_TAPurseControlGet( &pMYKI_TAPurseControl ) < 0 )
    {
        CsErrx( "MYKI_CS_TAPurseControlGet() failed" );
        return;
    }

    //  Card

    pMYKI_CAControl->Status                       = CARD_CONTROL_STATUS_ACTIVATED;
    pMYKI_CAControl->ExpiryDate                   = pData->DynamicData.currentBusinessDate + 1;

    pMYKI_TAControl->Status                       = TAPP_CONTROL_DIRECTORY_STATUS_ACTIVATED;
    pMYKI_TAControl->ExpiryDate                   = pData->DynamicData.currentBusinessDate + 1;
    pMYKI_TAControl->NextProductSerialNo          = 1;
    pMYKI_TAControl->ProductInUse                 = 5;

    //  Directory Entry 0 - T-Purse

    pMYKI_TAControl->Directory[ 0 ].SerialNo      = 0;
    pMYKI_TAControl->Directory[ 0 ].Status        = TAPP_CONTROL_DIRECTORY_STATUS_ACTIVATED;
    pMYKI_TAControl->Directory[ 0 ].IssuerId      = 0;
    pMYKI_TAControl->Directory[ 0 ].ProductId     = 0;

    pMYKI_TAPurseBalance->Balance                 = 1000;
    pMYKI_TAPurseControl->LastChangeNewBalance    = pMYKI_TAPurseBalance->Balance;
    pMYKI_TAPurseControl->NextTxSeqNo             = 3;
    pMYKI_TAPurseControl->LastChangeTxSeqNo       = pMYKI_TAPurseControl->NextTxSeqNo - 1;

    //  Directory Entry 1 - Product 0 : Start time > Expiry time

    pMYKI_TAControl->Directory[ 1 ].SerialNo      = 1;
    pMYKI_TAControl->Directory[ 1 ].Status        = TAPP_CONTROL_DIRECTORY_STATUS_ACTIVATED;
    pMYKI_TAControl->Directory[ 1 ].IssuerId      = 0;
    pMYKI_TAControl->Directory[ 1 ].ProductId     = PRODUCT_ID_SINGLE;

    pMYKI_TAProduct[ 0 ]->StartDateTime           = pData->DynamicData.currentDateTime - HOURS_TO_SECONDS( 1 );
    pMYKI_TAProduct[ 0 ]->EndDateTime             = pMYKI_TAProduct[ 0 ]->StartDateTime - 1;

    //  Directory Entry 2 - Product 1 : Single, Expiry time - Start time > 24 hours

    pMYKI_TAControl->Directory[ 2 ].SerialNo      = 2;
    pMYKI_TAControl->Directory[ 2 ].Status        = TAPP_CONTROL_DIRECTORY_STATUS_ACTIVATED;
    pMYKI_TAControl->Directory[ 2 ].IssuerId      = 0;
    pMYKI_TAControl->Directory[ 2 ].ProductId     = PRODUCT_ID_SINGLE;

    pMYKI_TAProduct[ 1 ]->StartDateTime           = pData->DynamicData.currentDateTime - HOURS_TO_SECONDS( 1 );
    pMYKI_TAProduct[ 1 ]->EndDateTime             = pMYKI_TAProduct[ 1 ]->StartDateTime + HOURS_TO_SECONDS( 24 ) + 1;

    //  Directory Entry 3 - Product 2 : Daily, Expiry time - Start time > 24 hours

    pMYKI_TAControl->Directory[ 3 ].SerialNo      = 3;
    pMYKI_TAControl->Directory[ 3 ].Status        = TAPP_CONTROL_DIRECTORY_STATUS_ACTIVATED;
    pMYKI_TAControl->Directory[ 3 ].IssuerId      = 0;
    pMYKI_TAControl->Directory[ 3 ].ProductId     = PRODUCT_ID_DAILY;

    pMYKI_TAProduct[ 2 ]->StartDateTime           = pData->DynamicData.currentDateTime - HOURS_TO_SECONDS( 1 );
    pMYKI_TAProduct[ 2 ]->EndDateTime             = pMYKI_TAProduct[ 2 ]->StartDateTime + HOURS_TO_SECONDS( 24 ) + 1;

    //  Directory Entry 4 - Product 3 : Weekly, Expiry time - Start time > 8 days

    pMYKI_TAControl->Directory[ 4 ].SerialNo      = 4;
    pMYKI_TAControl->Directory[ 4 ].Status        = TAPP_CONTROL_DIRECTORY_STATUS_ACTIVATED;
    pMYKI_TAControl->Directory[ 4 ].IssuerId      = 0;
    pMYKI_TAControl->Directory[ 4 ].ProductId     = PRODUCT_ID_WEEKLY;

    pMYKI_TAProduct[ 3 ]->StartDateTime           = pData->DynamicData.currentDateTime - HOURS_TO_SECONDS( 1 );
    pMYKI_TAProduct[ 3 ]->EndDateTime             = pMYKI_TAProduct[ 3 ]->StartDateTime + DAYS_TO_SECONDS( 8 ) + 1;

    //  Directory Entry 5 - Product 4 : Unused

    pMYKI_TAControl->Directory[ 5 ].SerialNo      = 0;
    pMYKI_TAControl->Directory[ 5 ].Status        = TAPP_CONTROL_DIRECTORY_STATUS_UNUSED;
    pMYKI_TAControl->Directory[ 5 ].IssuerId      = 0;
    pMYKI_TAControl->Directory[ 5 ].ProductId     = 0;

    pMYKI_TAProduct[ 4 ]->StartDateTime           = pData->DynamicData.currentDateTime;
    pMYKI_TAProduct[ 4 ]->EndDateTime             = pMYKI_TAProduct[ 4 ]->StartDateTime;
}

//=============================================================================
//
//  Create card image 3 - Normal card with multiple products with correctable errors
//
//=============================================================================

void    CardImage3( MYKI_BR_ContextData_t *pData )
{
    MYKI_CAControl_t           *pMYKI_CAControl         = NULL;
    MYKI_TAControl_t           *pMYKI_TAControl         = NULL;
    MYKI_TAProduct_t           *pMYKI_TAProduct[ 5 ]    = { NULL };
    MYKI_TAPurseBalance_t      *pMYKI_TAPurseBalance    = NULL;
    MYKI_TAPurseControl_t      *pMYKI_TAPurseControl    = NULL;
    int                         i;

    if ( MYKI_CS_CAControlGet( &pMYKI_CAControl ) < 0 )
    {
        CsErrx( "MYKI_CS_CAControlGet() failed" );
        return;
    }

    if ( MYKI_CS_TAControlGet( &pMYKI_TAControl ) < 0 )
    {
        CsErrx( "MYKI_CS_TAControlGet() failed" );
        return;
    }

    for ( i = 0; i < DIMOF( pMYKI_TAProduct ); i++ )
    {
        if ( MYKI_CS_TAProductGet( (U8_t)i, &pMYKI_TAProduct[ i ] ) < 0 )                             // Contract index (0-4)
        {
            CsErrx( "MYKI_CS_TAProductGet( %d ) failed", i );
            return;
        }
    }

    if ( MYKI_CS_TAPurseBalanceGet( &pMYKI_TAPurseBalance ) < 0 )
    {
        CsErrx( "MYKI_CS_TAPurseBalanceGet() failed" );
        return;
    }

    if ( MYKI_CS_TAPurseControlGet( &pMYKI_TAPurseControl ) < 0 )
    {
        CsErrx( "MYKI_CS_TAPurseControlGet() failed" );
        return;
    }

    //  Card

    pMYKI_CAControl->Status                       = CARD_CONTROL_STATUS_ACTIVATED;
    pMYKI_CAControl->ExpiryDate                   = pData->DynamicData.currentBusinessDate + 1;

    pMYKI_TAControl->Status                       = TAPP_CONTROL_DIRECTORY_STATUS_ACTIVATED;
    pMYKI_TAControl->ExpiryDate                   = pData->DynamicData.currentBusinessDate + 1;
    pMYKI_TAControl->NextProductSerialNo          = 1;
    pMYKI_TAControl->ProductInUse                 = 5;

    //  Directory Entry 0 - T-Purse

    pMYKI_TAControl->Directory[ 0 ].SerialNo      = 0;
    pMYKI_TAControl->Directory[ 0 ].Status        = TAPP_CONTROL_DIRECTORY_STATUS_ACTIVATED;
    pMYKI_TAControl->Directory[ 0 ].IssuerId      = 0;
    pMYKI_TAControl->Directory[ 0 ].ProductId     = 0;

    pMYKI_TAPurseBalance->Balance                 = 1000;
    pMYKI_TAPurseControl->LastChangeNewBalance    = pMYKI_TAPurseBalance->Balance;
    pMYKI_TAPurseControl->NextTxSeqNo             = 3;
    pMYKI_TAPurseControl->LastChangeTxSeqNo       = pMYKI_TAPurseControl->NextTxSeqNo - 1;

    //  Directory Entry 1 - Product 0 : E-Pass, ID 2, Instance Count not zero, Start + Instance + 1 > End

    pMYKI_TAControl->Directory[ 1 ].SerialNo      = 1;
    pMYKI_TAControl->Directory[ 1 ].Status        = TAPP_CONTROL_DIRECTORY_STATUS_ACTIVATED;
    pMYKI_TAControl->Directory[ 1 ].IssuerId      = 0;
    pMYKI_TAControl->Directory[ 1 ].ProductId     = PRODUCT_ID_EPASS;

    pMYKI_TAProduct[ 0 ]->InstanceCount           = 2;
    pMYKI_TAProduct[ 0 ]->StartDateTime           = pData->DynamicData.currentDateTime - HOURS_TO_SECONDS( 1 );
    pMYKI_TAProduct[ 0 ]->EndDateTime             = pMYKI_TAProduct[ 0 ]->StartDateTime + DAYS_TO_SECONDS( pMYKI_TAProduct[ 0 ]->InstanceCount + 1 ) + 1;

    //  Directory Entry 2 - Product 1 : E-Pass, ID 2, Instance Count not zero, Instance Count > 1825

    pMYKI_TAControl->Directory[ 2 ].SerialNo      = 2;
    pMYKI_TAControl->Directory[ 2 ].Status        = TAPP_CONTROL_DIRECTORY_STATUS_ACTIVATED;
    pMYKI_TAControl->Directory[ 2 ].IssuerId      = 0;
    pMYKI_TAControl->Directory[ 2 ].ProductId     = PRODUCT_ID_EPASS;

    pMYKI_TAProduct[ 1 ]->InstanceCount           = 1826;
    pMYKI_TAProduct[ 1 ]->StartDateTime           = pData->DynamicData.currentDateTime - HOURS_TO_SECONDS( 1 );
    pMYKI_TAProduct[ 1 ]->EndDateTime             = pMYKI_TAProduct[ 1 ]->StartDateTime + DAYS_TO_SECONDS( pMYKI_TAProduct[ 1 ]->InstanceCount + 1 );

    //  Directory Entry 3 - Product 2 : N-Hour, Expiry time - Start time > 24 hours

    pMYKI_TAControl->Directory[ 3 ].SerialNo      = 3;
    pMYKI_TAControl->Directory[ 3 ].Status        = TAPP_CONTROL_DIRECTORY_STATUS_ACTIVATED;
    pMYKI_TAControl->Directory[ 3 ].IssuerId      = 0;
    pMYKI_TAControl->Directory[ 3 ].ProductId     = PRODUCT_ID_NHOUR;

    pMYKI_TAProduct[ 2 ]->StartDateTime           = pData->DynamicData.currentDateTime - HOURS_TO_SECONDS( 1 );
    pMYKI_TAProduct[ 2 ]->EndDateTime             = pMYKI_TAProduct[ 2 ]->StartDateTime + HOURS_TO_SECONDS( 24 ) + 1;

    //  Directory Entry 4 - Product 3 : E-Pass, ID 2, Instance Count not zero, Start, End, Count all ok (just)

    pMYKI_TAControl->Directory[ 4 ].SerialNo      = 4;
    pMYKI_TAControl->Directory[ 4 ].Status        = TAPP_CONTROL_DIRECTORY_STATUS_ACTIVATED;
    pMYKI_TAControl->Directory[ 4 ].IssuerId      = 0;
    pMYKI_TAControl->Directory[ 4 ].ProductId     = PRODUCT_ID_EPASS;

    pMYKI_TAProduct[ 3 ]->InstanceCount           = 1825;
    pMYKI_TAProduct[ 3 ]->StartDateTime           = pData->DynamicData.currentDateTime - HOURS_TO_SECONDS( 1 );
    pMYKI_TAProduct[ 3 ]->EndDateTime             = pMYKI_TAProduct[ 3 ]->StartDateTime + DAYS_TO_SECONDS( pMYKI_TAProduct[ 3 ]->InstanceCount + 1 );

    //  Directory Entry 5 - Product 4 : Not in use

    pMYKI_TAControl->Directory[ 5 ].SerialNo      = 0;
    pMYKI_TAControl->Directory[ 5 ].Status        = TAPP_CONTROL_DIRECTORY_STATUS_UNUSED;
    pMYKI_TAControl->Directory[ 5 ].IssuerId      = 0;
    pMYKI_TAControl->Directory[ 5 ].ProductId     = 0;

    pMYKI_TAProduct[ 4 ]->StartDateTime           = pData->DynamicData.currentDateTime;
    pMYKI_TAProduct[ 4 ]->EndDateTime             = pMYKI_TAProduct[ 4 ]->StartDateTime;
}

//=============================================================================
//
//  Create card image 4 - Normal card with no products and no purse
//
//=============================================================================

void    CardImage4( MYKI_BR_ContextData_t *pData )
{
    MYKI_CAControl_t            *pMYKI_CAControl;
    MYKI_TAControl_t            *pMYKI_TAControl;

    if ( MYKI_CS_CAControlGet( &pMYKI_CAControl ) < 0 )
    {
        CsErrx( "MYKI_CS_CAControlGet() failed" );
        return;
    }

    if ( MYKI_CS_TAControlGet( &pMYKI_TAControl ) < 0 )
    {
        CsErrx( "MYKI_CS_TAControlGet() failed" );
        return;
    }

    //  Card

    pMYKI_CAControl->Status                       = CARD_CONTROL_STATUS_ACTIVATED;
    pMYKI_CAControl->ExpiryDate                   = pData->DynamicData.currentBusinessDate + 1;

    pMYKI_TAControl->Status                       = TAPP_CONTROL_DIRECTORY_STATUS_ACTIVATED;
    pMYKI_TAControl->ExpiryDate                   = pData->DynamicData.currentBusinessDate + 1;
    pMYKI_TAControl->NextProductSerialNo          = 1;
    pMYKI_TAControl->ProductInUse                 = 0;

    //  Directory Entry 0 - T-Purse

    pMYKI_TAControl->Directory[ 0 ].SerialNo      = 0;
    pMYKI_TAControl->Directory[ 0 ].Status        = TAPP_CONTROL_DIRECTORY_STATUS_UNUSED;
    pMYKI_TAControl->Directory[ 0 ].IssuerId      = 0;
    pMYKI_TAControl->Directory[ 0 ].ProductId     = 0;
}

//=============================================================================
//
//  Create card image 5 - Normal card with no products with Purse Balance
//
//=============================================================================

void    CardImage5( MYKI_BR_ContextData_t *pData )
{
    MYKI_CAControl_t           *pMYKI_CAControl         = NULL;
    MYKI_TAControl_t           *pMYKI_TAControl         = NULL;
    MYKI_TAPurseBalance_t      *pMYKI_TAPurseBalance    = NULL;
    MYKI_TAPurseControl_t      *pMYKI_TAPurseControl    = NULL;

    if ( MYKI_CS_CAControlGet( &pMYKI_CAControl ) < 0 )
    {
        CsErrx( "MYKI_CS_CAControlGet() failed" );
        return;
    }

    if ( MYKI_CS_TAControlGet( &pMYKI_TAControl ) < 0 )
    {
        CsErrx( "MYKI_CS_TAControlGet() failed" );
        return;
    }

    if ( MYKI_CS_TAPurseBalanceGet( &pMYKI_TAPurseBalance ) < 0 )
    {
        CsErrx( "MYKI_CS_TAPurseBalanceGet() failed" );
        return;
    }

    if ( MYKI_CS_TAPurseControlGet( &pMYKI_TAPurseControl ) < 0 )
    {
        CsErrx( "MYKI_CS_TAPurseControlGet() failed" );
        return;
    }

    //  Card

    pMYKI_CAControl->Status                       = CARD_CONTROL_STATUS_ACTIVATED;
    pMYKI_CAControl->ExpiryDate                   = pData->DynamicData.currentBusinessDate + 1;

    pMYKI_TAControl->Status                       = TAPP_CONTROL_DIRECTORY_STATUS_ACTIVATED;
    pMYKI_TAControl->ExpiryDate                   = pData->DynamicData.currentBusinessDate + 1;
    pMYKI_TAControl->NextProductSerialNo          = 1;
    pMYKI_TAControl->ProductInUse                 = 0;

    //  Directory Entry 0 - T-Purse

    pMYKI_TAControl->Directory[ 0 ].SerialNo      = 0;
    pMYKI_TAControl->Directory[ 0 ].Status        = TAPP_CONTROL_DIRECTORY_STATUS_ACTIVATED;
    pMYKI_TAControl->Directory[ 0 ].IssuerId      = 0;
    pMYKI_TAControl->Directory[ 0 ].ProductId     = 0;

    pMYKI_TAPurseBalance->Balance                 = 1000;
    pMYKI_TAPurseControl->LastChangeNewBalance    = pMYKI_TAPurseBalance->Balance;
    pMYKI_TAPurseControl->NextTxSeqNo             = 3;
    pMYKI_TAPurseControl->LastChangeTxSeqNo       = pMYKI_TAPurseControl->NextTxSeqNo - 1;
}

//=============================================================================
//
//  Create card image 6 - Normal card with multiple products, scanned on
//
//=============================================================================

void    CardImage6( MYKI_BR_ContextData_t *pData )
{
    MYKI_CAControl_t           *pMYKI_CAControl         = NULL;
    MYKI_TAControl_t           *pMYKI_TAControl         = NULL;
    MYKI_TAProduct_t           *pMYKI_TAProduct[ 5 ]    = { NULL };
    MYKI_TAPurseBalance_t      *pMYKI_TAPurseBalance    = NULL;
    MYKI_TAPurseControl_t      *pMYKI_TAPurseControl    = NULL;
    int                         i;

    if ( MYKI_CS_CAControlGet( &pMYKI_CAControl ) < 0 )
    {
        CsErrx( "MYKI_CS_CAControlGet() failed" );
        return;
    }

    if ( MYKI_CS_TAControlGet( &pMYKI_TAControl ) < 0 )
    {
        CsErrx( "MYKI_CS_TAControlGet() failed" );
        return;
    }

    for ( i = 0; i < DIMOF( pMYKI_TAProduct ); i++ )
    {
        if ( MYKI_CS_TAProductGet( (U8_t)i, &pMYKI_TAProduct[ i ] ) < 0 )                             // Contract index (0-4)
        {
            CsErrx( "MYKI_CS_TAProductGet( %d ) failed", i );
            return;
        }
    }

    if ( MYKI_CS_TAPurseBalanceGet( &pMYKI_TAPurseBalance ) < 0 )
    {
        CsErrx( "MYKI_CS_TAPurseBalanceGet() failed" );
        return;
    }

    if ( MYKI_CS_TAPurseControlGet( &pMYKI_TAPurseControl ) < 0 )
    {
        CsErrx( "MYKI_CS_TAPurseControlGet() failed" );
        return;
    }

    //  Card

    pMYKI_CAControl->Status                       = CARD_CONTROL_STATUS_ACTIVATED;
    pMYKI_CAControl->ExpiryDate                   = pData->DynamicData.currentBusinessDate + 1;

    pMYKI_TAControl->Status                       = TAPP_CONTROL_DIRECTORY_STATUS_ACTIVATED;
    pMYKI_TAControl->ExpiryDate                   = pData->DynamicData.currentBusinessDate + 1;
    pMYKI_TAControl->NextProductSerialNo          = 1;
    pMYKI_TAControl->ProductInUse                 = 1;

    //  Directory Entry 0 - T-Purse

    pMYKI_TAControl->Directory[ 0 ].SerialNo      = 0;
    pMYKI_TAControl->Directory[ 0 ].Status        = TAPP_CONTROL_DIRECTORY_STATUS_ACTIVATED;
    pMYKI_TAControl->Directory[ 0 ].IssuerId      = 0;
    pMYKI_TAControl->Directory[ 0 ].ProductId     = 0;

    pMYKI_TAPurseBalance->Balance                 = 1000;
    pMYKI_TAPurseControl->LastChangeNewBalance    = pMYKI_TAPurseBalance->Balance;
    pMYKI_TAPurseControl->NextTxSeqNo             = 3;
    pMYKI_TAPurseControl->LastChangeTxSeqNo       = pMYKI_TAPurseControl->NextTxSeqNo - 1;

    //  Directory Entry 1 - Product 0

    pMYKI_TAControl->Directory[ 1 ].SerialNo      = 1;
    pMYKI_TAControl->Directory[ 1 ].Status        = TAPP_CONTROL_DIRECTORY_STATUS_ACTIVATED;
    pMYKI_TAControl->Directory[ 1 ].IssuerId      = 0;
    pMYKI_TAControl->Directory[ 1 ].ProductId     = PRODUCT_ID_SINGLE;

    pMYKI_TAProduct[ 0 ]->StartDateTime           = pData->DynamicData.currentDateTime - HOURS_TO_SECONDS( 1 );
    pMYKI_TAProduct[ 0 ]->EndDateTime             = pMYKI_TAProduct[ 0 ]->StartDateTime + HOURS_TO_SECONDS( 24 );
    pMYKI_TAProduct[ 0 ]->LastUsage.Location.EntryPointId  = 10500;

    //  Directory Entry 2 - Product 1

    pMYKI_TAControl->Directory[ 2 ].SerialNo      = 2;
    pMYKI_TAControl->Directory[ 2 ].Status        = TAPP_CONTROL_DIRECTORY_STATUS_ACTIVATED;
    pMYKI_TAControl->Directory[ 2 ].IssuerId      = 0;
    pMYKI_TAControl->Directory[ 2 ].ProductId     = PRODUCT_ID_DAILY;

    pMYKI_TAProduct[ 1 ]->StartDateTime           = pData->DynamicData.currentDateTime - HOURS_TO_SECONDS( 1 );
    pMYKI_TAProduct[ 1 ]->EndDateTime             = pMYKI_TAProduct[ 1 ]->StartDateTime + HOURS_TO_SECONDS( 24 );

    //  Directory Entry 3 - Product 2

    pMYKI_TAControl->Directory[ 3 ].SerialNo      = 3;
    pMYKI_TAControl->Directory[ 3 ].Status        = TAPP_CONTROL_DIRECTORY_STATUS_ACTIVATED;
    pMYKI_TAControl->Directory[ 3 ].IssuerId      = 0;
    pMYKI_TAControl->Directory[ 3 ].ProductId     = PRODUCT_ID_WEEKLY;

    pMYKI_TAProduct[ 2 ]->StartDateTime           = pData->DynamicData.currentDateTime - HOURS_TO_SECONDS( 1 );
    pMYKI_TAProduct[ 2 ]->EndDateTime             = pMYKI_TAProduct[ 2 ]->StartDateTime + DAYS_TO_SECONDS( 8 );

    //  Directory Entry 4 - Product 3

    pMYKI_TAControl->Directory[ 4 ].SerialNo      = 4;
    pMYKI_TAControl->Directory[ 4 ].Status        = TAPP_CONTROL_DIRECTORY_STATUS_UNUSED;
    pMYKI_TAControl->Directory[ 4 ].IssuerId      = 0;
    pMYKI_TAControl->Directory[ 4 ].ProductId     = 0;

    pMYKI_TAProduct[ 3 ]->StartDateTime           = pData->DynamicData.currentDateTime;
    pMYKI_TAProduct[ 3 ]->EndDateTime             = pMYKI_TAProduct[ 3 ]->StartDateTime;

    //  Directory Entry 5 - Product 4

    pMYKI_TAControl->Directory[ 5 ].SerialNo      = 0;
    pMYKI_TAControl->Directory[ 5 ].Status        = TAPP_CONTROL_DIRECTORY_STATUS_UNUSED;
    pMYKI_TAControl->Directory[ 5 ].IssuerId      = 0;
    pMYKI_TAControl->Directory[ 5 ].ProductId     = 0;

    pMYKI_TAProduct[ 4 ]->StartDateTime           = pData->DynamicData.currentDateTime;
    pMYKI_TAProduct[ 4 ]->EndDateTime             = pMYKI_TAProduct[ 4 ]->StartDateTime;
}

//=============================================================================
//
//  Create card image 7 - Normal card, not scanned on, no products, $5.00 balance
//
//=============================================================================

void    CardImage7( MYKI_BR_ContextData_t *pData )
{
    MYKI_CAControl_t           *pMYKI_CAControl         = NULL;
    MYKI_TAControl_t           *pMYKI_TAControl         = NULL;
    MYKI_TAProduct_t           *pMYKI_TAProduct[ 5 ]    = { NULL };
    MYKI_TAPurseBalance_t      *pMYKI_TAPurseBalance    = NULL;
    MYKI_TAPurseControl_t      *pMYKI_TAPurseControl    = NULL;
    int                         i;

    if ( MYKI_CS_CAControlGet( &pMYKI_CAControl ) < 0 )
    {
        CsErrx( "MYKI_CS_CAControlGet() failed" );
        return;
    }

    if ( MYKI_CS_TAControlGet( &pMYKI_TAControl ) < 0 )
    {
        CsErrx( "MYKI_CS_TAControlGet() failed" );
        return;
    }

    for ( i = 0; i < DIMOF( pMYKI_TAProduct ); i++ )
    {
        if ( MYKI_CS_TAProductGet( (U8_t)i, &pMYKI_TAProduct[ i ] ) < 0 )                             // Contract index (0-4)
        {
            CsErrx( "MYKI_CS_TAProductGet( %d ) failed", i );
            return;
        }
    }

    if ( MYKI_CS_TAPurseBalanceGet( &pMYKI_TAPurseBalance ) < 0 )
    {
        CsErrx( "MYKI_CS_TAPurseBalanceGet() failed" );
        return;
    }

    if ( MYKI_CS_TAPurseControlGet( &pMYKI_TAPurseControl ) < 0 )
    {
        CsErrx( "MYKI_CS_TAPurseControlGet() failed" );
        return;
    }

    //  Card

    pMYKI_CAControl->Status                       = CARD_CONTROL_STATUS_ACTIVATED;
    pMYKI_CAControl->ExpiryDate                   = pData->DynamicData.currentBusinessDate + 1;

    pMYKI_TAControl->Status                       = TAPP_CONTROL_DIRECTORY_STATUS_ACTIVATED;
    pMYKI_TAControl->ExpiryDate                   = pData->DynamicData.currentBusinessDate + 1;
    pMYKI_TAControl->NextProductSerialNo          = 1;
    pMYKI_TAControl->ProductInUse                 = 0;

    //  Directory Entry 0 - T-Purse

    pMYKI_TAControl->Directory[ 0 ].SerialNo      = 0;
    pMYKI_TAControl->Directory[ 0 ].Status        = TAPP_CONTROL_DIRECTORY_STATUS_ACTIVATED;
    pMYKI_TAControl->Directory[ 0 ].IssuerId      = 0;
    pMYKI_TAControl->Directory[ 0 ].ProductId     = 0;

    pMYKI_TAPurseBalance->Balance                 = 500;
    pMYKI_TAPurseControl->LastChangeNewBalance    = pMYKI_TAPurseBalance->Balance;
    pMYKI_TAPurseControl->NextTxSeqNo             = 3;
    pMYKI_TAPurseControl->LastChangeTxSeqNo       = pMYKI_TAPurseControl->NextTxSeqNo - 1;
}

//=============================================================================
//
//  Create card image 8 - Normal card, not scanned on, no products, $2.00 balance
//
//=============================================================================

void    CardImage8( MYKI_BR_ContextData_t *pData )
{
    MYKI_CAControl_t           *pMYKI_CAControl         = NULL;
    MYKI_TAControl_t           *pMYKI_TAControl         = NULL;
    MYKI_TAProduct_t           *pMYKI_TAProduct[ 5 ]    = { NULL };
    MYKI_TAPurseBalance_t      *pMYKI_TAPurseBalance    = NULL;
    MYKI_TAPurseControl_t      *pMYKI_TAPurseControl    = NULL;
    int                         i;

    if ( MYKI_CS_CAControlGet( &pMYKI_CAControl ) < 0 )
    {
        CsErrx( "MYKI_CS_CAControlGet() failed" );
        return;
    }

    if ( MYKI_CS_TAControlGet( &pMYKI_TAControl ) < 0 )
    {
        CsErrx( "MYKI_CS_TAControlGet() failed" );
        return;
    }

    for ( i = 0; i < DIMOF( pMYKI_TAProduct ); i++ )
    {
        if ( MYKI_CS_TAProductGet( (U8_t)i, &pMYKI_TAProduct[ i ] ) < 0 )                             // Contract index (0-4)
        {
            CsErrx( "MYKI_CS_TAProductGet( %d ) failed", i );
            return;
        }
    }

    if ( MYKI_CS_TAPurseBalanceGet( &pMYKI_TAPurseBalance ) < 0 )
    {
        CsErrx( "MYKI_CS_TAPurseBalanceGet() failed" );
        return;
    }

    if ( MYKI_CS_TAPurseControlGet( &pMYKI_TAPurseControl ) < 0 )
    {
        CsErrx( "MYKI_CS_TAPurseControlGet() failed" );
        return;
    }

    //  Card

    pMYKI_CAControl->Status                       = CARD_CONTROL_STATUS_ACTIVATED;
    pMYKI_CAControl->ExpiryDate                   = pData->DynamicData.currentBusinessDate + 1;

    pMYKI_TAControl->Status                       = TAPP_CONTROL_DIRECTORY_STATUS_ACTIVATED;
    pMYKI_TAControl->ExpiryDate                   = pData->DynamicData.currentBusinessDate + 1;
    pMYKI_TAControl->NextProductSerialNo          = 1;
    pMYKI_TAControl->ProductInUse                 = 0;

    //  Directory Entry 0 - T-Purse

    pMYKI_TAControl->Directory[ 0 ].SerialNo      = 0;
    pMYKI_TAControl->Directory[ 0 ].Status        = TAPP_CONTROL_DIRECTORY_STATUS_ACTIVATED;
    pMYKI_TAControl->Directory[ 0 ].IssuerId      = 0;
    pMYKI_TAControl->Directory[ 0 ].ProductId     = 0;

    pMYKI_TAPurseBalance->Balance                 = 500;
    pMYKI_TAPurseControl->LastChangeNewBalance    = pMYKI_TAPurseBalance->Balance;
    pMYKI_TAPurseControl->NextTxSeqNo             = 3;
    pMYKI_TAPurseControl->LastChangeTxSeqNo       = pMYKI_TAPurseControl->NextTxSeqNo - 1;
}

//=============================================================================
//
//  Create card image 9 - Normal card, not scanned on, no products, -$0.26 balance
//
//=============================================================================

void    CardImage9( MYKI_BR_ContextData_t *pData )
{
    MYKI_CAControl_t           *pMYKI_CAControl         = NULL;
    MYKI_TAControl_t           *pMYKI_TAControl         = NULL;
    MYKI_TAProduct_t           *pMYKI_TAProduct[ 5 ]    = { NULL };
    MYKI_TAPurseBalance_t      *pMYKI_TAPurseBalance    = NULL;
    MYKI_TAPurseControl_t      *pMYKI_TAPurseControl    = NULL;
    int                         i;

    if ( MYKI_CS_CAControlGet( &pMYKI_CAControl ) < 0 )
    {
        CsErrx( "MYKI_CS_CAControlGet() failed" );
        return;
    }

    if ( MYKI_CS_TAControlGet( &pMYKI_TAControl ) < 0 )
    {
        CsErrx( "MYKI_CS_TAControlGet() failed" );
        return;
    }

    for ( i = 0; i < DIMOF( pMYKI_TAProduct ); i++ )
    {
        if ( MYKI_CS_TAProductGet( (U8_t)i, &pMYKI_TAProduct[ i ] ) < 0 )                             // Contract index (0-4)
        {
            CsErrx( "MYKI_CS_TAProductGet( %d ) failed", i );
            return;
        }
    }

    if ( MYKI_CS_TAPurseBalanceGet( &pMYKI_TAPurseBalance ) < 0 )
    {
        CsErrx( "MYKI_CS_TAPurseBalanceGet() failed" );
        return;
    }

    if ( MYKI_CS_TAPurseControlGet( &pMYKI_TAPurseControl ) < 0 )
    {
        CsErrx( "MYKI_CS_TAPurseControlGet() failed" );
        return;
    }

    //  Card

    pMYKI_CAControl->Status                       = CARD_CONTROL_STATUS_ACTIVATED;
    pMYKI_CAControl->ExpiryDate                   = pData->DynamicData.currentBusinessDate + 1;

    pMYKI_TAControl->Status                       = TAPP_CONTROL_DIRECTORY_STATUS_ACTIVATED;
    pMYKI_TAControl->ExpiryDate                   = pData->DynamicData.currentBusinessDate + 1;
    pMYKI_TAControl->NextProductSerialNo          = 1;
    pMYKI_TAControl->ProductInUse                 = 0;

    //  Directory Entry 0 - T-Purse

    pMYKI_TAControl->Directory[ 0 ].SerialNo      = 0;
    pMYKI_TAControl->Directory[ 0 ].Status        = TAPP_CONTROL_DIRECTORY_STATUS_ACTIVATED;
    pMYKI_TAControl->Directory[ 0 ].IssuerId      = 0;
    pMYKI_TAControl->Directory[ 0 ].ProductId     = 0;

    pMYKI_TAPurseBalance->Balance                 = -26;
    pMYKI_TAPurseControl->LastChangeNewBalance    = pMYKI_TAPurseBalance->Balance;
    pMYKI_TAPurseControl->NextTxSeqNo             = 3;
    pMYKI_TAPurseControl->LastChangeTxSeqNo       = pMYKI_TAPurseControl->NextTxSeqNo - 1;
}

