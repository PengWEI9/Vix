//=============================================================================
//
//  Functionality common to the BR_xxx individual business rule modules.
//
//  A lot of this code is wrapper functions for external code - specifically
//  the LDT and CD packages.
//
//=============================================================================

#include "BR_Common.h"

#include <datec19.h>
#include <cs.h>                             // cs       - Core Services
#include <myki_cardservices.h>              // Myki_CS  - Card Services
#include <myki_fs_serialise.h>              // Myki_FS  - Card serialisation
#include <LDT.h>                            // Myki_LDT - Logical Device Transaction
#include <myki_cd.h>                        // Myki_CD  - Configuration Data
#include <myki_actionlist.h>                // Myki_CD  - Configuration Data
/*
 *      Local Function Prototypes
 *      -------------------------
 */

static  DateC19_t   myki_br_NextMonday( MYKI_BR_ContextData_t *pData );
static  int         ProductUpdateInitialise( MYKI_BR_ContextData_t *pData, MYKI_Directory_t *pDirectory, ProductUpdate_t *pRequest );
static  void        DebugShowLdtProductUpdateRequest( int dbglvl, ProductUpdate_t *request );
static  Time_t      myki_br_NHourEndDateTime( MYKI_BR_ContextData_t *pData );

//=============================================================================
//
//  Return the product type (an internal enumeration) for a given product ID
//
//  Calls MYKI_CD to get the product "subtype" from CD as a string, then maps
//  known strings to product types.
//
//=============================================================================

ProductType_e       myki_br_cd_GetProductType( int productId )
{
    int         i;
    char        subtype[ 64 ];          // This should be a constant defined in the MYKI_CD API

    static  struct
    {
        char            *subtypeDesc;
        ProductType_e   type;

    }   productList[] =
    {
        { "Daily"       , PRODUCT_TYPE_DAILY    },
        { "nHour"       , PRODUCT_TYPE_NHOUR    },
        { "SingleTrip"  , PRODUCT_TYPE_SINGLE   },
        { "Weekly"      , PRODUCT_TYPE_WEEKLY   },
        { "ePass"       , PRODUCT_TYPE_EPASS    },
        { "FixedePass"  , PRODUCT_TYPE_EPASS    },  //  Assume fixedEpass is a special type of ePass
        { "None"        , PRODUCT_TYPE_UNKNOWN  }   //  Check
    };

    if ( ! MYKI_CD_getProductType( (U8_t)productId, subtype, sizeof( subtype ) ) )
    {
        CsErrx( "myki_br_cd_GetProductType( %d ) MYKI_CD_getProductType() failed", productId );
        return PRODUCT_TYPE_UNKNOWN;
    }

    for ( i = 0; i < DIMOF( productList ); i++ )
    {
        if ( ! strcmp( productList[ i ].subtypeDesc, subtype ) )
        {
            CsDbg( BRLL_RULE, "myki_br_cd_GetProductType( %d ) Product subtype \"%s\" is Product Type %d", productId, subtype, productList[ i ].type );
            return productList[ i ].type;
        }
    }

    CsErrx( "myki_br_cd_GetProductType( %d ) Product subtype \"%s\" not known", productId, subtype );
    return PRODUCT_TYPE_UNKNOWN;
}

/*==========================================================================*
**
**  myki_br_cd_IsFixedePass
**
**  Description     :
**      Determines if specified product (ID) is a fixed ePass.
**
**  Parameters      :
**      productId           [I]     product ID
**
**  Returns         :
**      TRUE                        is fixed ePass
**      FALSE                       otherwise
**
**  Notes           :
**
**==========================================================================*/

int myki_br_cd_IsFixedePass( int productId )
{
    char        subtype[ 64 ];

    if ( MYKI_CD_getProductType( (U8_t)productId, subtype, sizeof( subtype ) ) == FALSE )
    {
        CsErrx( "myki_br_cd_IsFixedePass(%d) MYKI_CD_getProductType() failed", productId );
        return FALSE;
    }   /* end-of-if */
    return strcmp( subtype, "FixedePass" ) != 0 ? FALSE : TRUE;
}   /* myki_br_cd_IsFixedePass( ) */

//=============================================================================
//
//  Return the product id from the BR internal product type enumeration
//
//  TODO: Calls MYKI_CD to get the list of the product id associated with
//        a subtype
//
//=============================================================================

int  myki_br_cd_GetProductId( ProductType_e productType )
{
    int         productId;

    switch (productType)
    {
        case PRODUCT_TYPE_EPASS:
            productId = 2;
            break;
        case PRODUCT_TYPE_SINGLE:
            productId = 3;
            break;
        case PRODUCT_TYPE_NHOUR:
            productId = 4;
            break;
        case PRODUCT_TYPE_DAILY:
            productId = 5;
            break;
        case PRODUCT_TYPE_WEEKLY:
            productId = 6;
            break;
        default:
            CsErrx( "myki_br_cd_GetProductId: Unknown product type ( %d )", productType );
            productId = 0;
            break;
    }

    return productId;
}

/*----------------------------------------------------------------------------
** FUNCTION           : myki_br_GetProductDuration
**
** DESCRIPTION        : Return the product type duration (or period) in seconds
**
**
** INPUTS             : pData               - Context Data
**                      productType         - Product Type
**
** TODO: This should be getting the product duration from CD.
**
** RETURNS            : the product type duration (or period) in seconds
**
----------------------------------------------------------------------------*/

int  myki_br_GetProductDuration  ( MYKI_BR_ContextData_t *pData, ProductType_e productType )
{
    int         productDuration = 0;

    switch (productType)
    {
        case PRODUCT_TYPE_NHOUR:
            if ( pData->Tariff.nHourPeriodMinutes > 0)
            {
                productDuration = MINUTES_TO_SECONDS( pData->Tariff.nHourPeriodMinutes );
            }
            else
            {
                productDuration = HOURS_TO_SECONDS( 2 );
            }
            break;
        case PRODUCT_TYPE_DAILY:
            productDuration = DAYS_TO_SECONDS( 1 );
            break;
        case PRODUCT_TYPE_WEEKLY:
            productDuration = DAYS_TO_SECONDS( 7 );
            break;
        case PRODUCT_TYPE_EPASS: // TODO Remove hack and use data fom actionlist to specify length. KWS: From where???
            productDuration = DAYS_TO_SECONDS( 30 );
            break;
        default:
            CsErrx( "myki_br_GetProductDuration: Unknown product type ( %d ), returning a duration of 0", productType );
            productDuration = 0;
            break;
    }

    return productDuration;
}

/*==========================================================================*
**
**  myki_br_GetEpassDuration
**
**  Description     :
**      Determines ePass duration.
**
**  Parameters      :
**      pDirectory          [I]     product directory entry
**      pProduct            [I]     product object
**
**  Returns         :
**      TIME_NOT_SET                failed
**      Else                        product duration in seconds
**
**  Notes           :
**
**
**==========================================================================*/

int myki_br_GetEpassDuration(
    MYKI_Directory_t       *pDirectory,
    MYKI_TAProduct_t       *pProduct )
{
    Time_t                  ProductDuration = TIME_NOT_SET;

    if ( pDirectory == NULL || pProduct == NULL )
    {
        return TIME_NOT_SET;
    }   /* end-of-if */

    if ( myki_br_cd_GetProductType( pDirectory->ProductId ) != PRODUCT_TYPE_EPASS )
    {
        CsErrx( "myki_br_GetEpassDuration() Product is not ePass" );
        return TIME_NOT_SET;
    }   /* end-of-if */

    /*  *** UNDOCUMENTED FEATURE!
        TAppProduct.InstanceCount contains the duration of ePass (in days) */
    if ( pProduct->InstanceCount == 0 )
    {
        /* Determines ePass duration from current product start and end date/time*/
        if ( pProduct->StartDateTime > pProduct->EndDateTime )
        {
            CsErrx( "myki_br_GetEpassDuration() StartDateTime(%d) > EndDateTime(%d)",
                    pProduct->StartDateTime, pProduct->EndDateTime );
            return TIME_NOT_SET;
        }   /* end-of-if */

        ProductDuration = pProduct->EndDateTime - pProduct->StartDateTime;
    }
    else
    {
        /* Determines ePass duration from product InstanceCount */
        ProductDuration = (Time_t)DAYS_TO_SECONDS( pProduct->InstanceCount );
    }   /* end-of-if */

    return ProductDuration;
}   /* myki_br_GetEpassDuration( ) */

//=============================================================================
//
//  Return the name of the specified product type - used for debug logs
//
//=============================================================================

char    *myki_br_GetProductTypeName( ProductType_e productType )
{
    switch ( productType )
    {
        case PRODUCT_TYPE_NHOUR :   return "nHour";
        case PRODUCT_TYPE_DAILY :   return "Daily";
        case PRODUCT_TYPE_WEEKLY:   return "Weekly";
        case PRODUCT_TYPE_EPASS :   return "ePass";
        default                 :   return "Unknown";
    }
}

//=============================================================================
//
//  Return the Card serial number formatted as a text string
//
//=============================================================================

int                 myki_br_GetCardSerialNumberString  ( unsigned char* cardUIDUnformatted, char* pSerialNumberBuffer, int length )
{
    if ( cardUIDUnformatted == NULL )
    {
        CsErrx( "myki_br_GetCardSerialNumberString unsigned char* cardUIDUnformatted==NULL" );
        return -1;
    }
    if ( pSerialNumberBuffer == NULL )
    {
        CsErrx( "myki_br_GetCardSerialNumberString pSerialNumberBuffer==NULL" );
        return -1;
    }

    if ( length < 15 )  // Length of formatted string (7 x 2 characters) + NUL
    {
        CsErrx( "myki_br_GetCardSerialNumberString length<15" );
        return -1;
    }

    sprintf
    (
        pSerialNumberBuffer,
        "%02X%02X%02X%02X%02X%02X%02X",
        cardUIDUnformatted[0],
        cardUIDUnformatted[1],
        cardUIDUnformatted[2],
        cardUIDUnformatted[3],
        cardUIDUnformatted[4],
        cardUIDUnformatted[5],
        cardUIDUnformatted[6]
    );

    return 0;
}

//=============================================================================
//
//  Populate the data fields of pData->Actionlist.
//
//  Input:
//      cardUID                     [I]     search card UID
//      searchType                  [I]     search actionlist type
//      searchActionSequenceNumber  [I]     search action sequence number,
//                                          0=any
//      searchProductSerialNumber   [I]     search product serial number,
//                                          0=don't care
//
//  Output:
//      MYKI_BR_ContextData_t *pData->Actionlist,
//      pData->Actionlist is populated on success. If the desired actionlist
//      is not found, pData->Actionlist is populated with the last found
//      actionlist record of the searching type.
//
//  Returns:
//      <0                      failed retrieving actionlist records
//      =0                      desired actionlist not found
//      >0                      desired actionlist found
//
//=============================================================================

int  myki_br_GetActionlist(
    unsigned char          *cardUIDUnformatted,
    ActionlistType_e        searchType,
    U8_t                    searchActionSequenceNumber,
    U16_t                   searchProductSerialNumber,
    MYKI_BR_ContextData_t  *pData )
{
    MYKI_ACTIONLIST_Card_Actionlist_Requests_t  cardActionlistRequests;
    MYKI_ACTIONLIST_Request_t                  *p_Request;
    char                                        cardUID[ 32 ];
    int                                         i;
    int                                         bFound  = FALSE;

    if ( !cardUIDUnformatted || !pData )
    {
        CsErrx( "myki_br_GetActionlist : Invalid argument" );
        return -1;
    }

    memset( &pData->ActionList, 0, sizeof( pData->ActionList ) );

    if ( myki_br_GetCardSerialNumberString( cardUIDUnformatted, cardUID, 32 ) < 0 )
    {
        CsErrx( "myki_br_GetActionlist : myki_br_GetCardSerialNumberString returns error" );
        return -1;
    }

    if ( ! MYKI_ACTIONLIST_getCardActionlistRequests( cardUID, &cardActionlistRequests ) )
    {
        CsDbg( BRLL_RULE, "myki_br_GetActionlist : MYKI_ACTIONLIST_getCardActionlistRequests returns error" );
        pData->ActionList.type = ACTION_NULL;
        return 0;
    }

    pData->ActionList.type  = ACTION_NULL;

    for ( i = 0, p_Request = cardActionlistRequests.arrayOfRequest;
          i < cardActionlistRequests.numberOfRequests; i++, p_Request++ )
    {
        bFound  = FALSE;
        if ( p_Request->type == MYKI_UD_TYPE_TAPP_UPDATE )
        {
            switch ( searchType )
            {
            case    ACTION_TAPP_UPDATE_BLOCK:
                if ( ( bFound = MYKI_ACTIONLIST_getTAppUpdateRequestBlock( cardUID, p_Request->action_seq_no, &pData->ActionList.actionlist.tAppUpdate ) ) == FALSE )
                {
                    CsErrx( "myki_br_GetActionlist : MYKI_ACTIONLIST_getTAppUpdateRequestBlock(%d) AL not found", p_Request->action_seq_no );
                }
                break;

            case    ACTION_TAPP_UPDATE_UNBLOCK:
                if ( ( bFound = MYKI_ACTIONLIST_getTAppUpdateRequestUnblock( cardUID, p_Request->action_seq_no, &pData->ActionList.actionlist.tAppUpdate ) ) == FALSE )
                {
                    CsErrx( "myki_br_GetActionlist : MYKI_ACTIONLIST_getTAppUpdateRequestUnblock(%d) AL not found", p_Request->action_seq_no );
                }
                break;

            case    ACTION_TAPP_UPDATE_NONE:
                if ( ( bFound = MYKI_ACTIONLIST_getTAppUpdateRequestNone( cardUID, p_Request->action_seq_no, &pData->ActionList.actionlist.tAppUpdate ) ) == FALSE )
                {
                    CsErrx( "myki_br_GetActionlist : MYKI_ACTIONLIST_getTAppUpdateRequestNone(%d) AL not found", p_Request->action_seq_no );
                }
                break;

            case    ACTION_TAPP_UPDATE_ACTIVATE:
                if ( ( bFound = MYKI_ACTIONLIST_getTAppUpdateRequestActivate( cardUID, p_Request->action_seq_no, &pData->ActionList.actionlist.tAppUpdate ) ) == FALSE )
                {
                    CsErrx( "myki_br_GetActionlist : MYKI_ACTIONLIST_getTAppUpdateRequestActivate(%d) AL not found", p_Request->action_seq_no );
                }
                break;

            default:
                break;
            }   /*  end-of-switch */
        }
        else if ( p_Request->type == MYKI_UD_TYPE_PRODUCT_UPDATE )
        {
            switch ( searchType )
            {
            case    ACTION_PRODUCT_UPDATE_BLOCK:
                if ( ( bFound = MYKI_ACTIONLIST_getProductUpdateRequestBlock( cardUID, p_Request->action_seq_no, &pData->ActionList.actionlist.productUpdate ) ) == FALSE )
                {
                    CsErrx( "myki_br_GetActionlist : MYKI_ACTIONLIST_getTPurseUpdateRequestBlock(%d) AL not found", p_Request->action_seq_no );
                }
                break;

            case    ACTION_PRODUCT_UPDATE_UNBLOCK:
                if ( ( bFound = MYKI_ACTIONLIST_getProductUpdateRequestUnblock( cardUID, p_Request->action_seq_no, &pData->ActionList.actionlist.productUpdate ) ) == FALSE )
                {
                    CsErrx( "myki_br_GetActionlist : MYKI_ACTIONLIST_getTPurseUpdateRequestBlock(%d) AL not found", p_Request->action_seq_no );
                }
                break;

            case    ACTION_PRODUCT_UPDATE_NONE:
                if ( ( bFound = MYKI_ACTIONLIST_getProductUpdateRequestNone( cardUID, p_Request->action_seq_no, &pData->ActionList.actionlist.productUpdate ) ) == FALSE )
                {
                    CsErrx( "myki_br_GetActionlist : MYKI_ACTIONLIST_getTPurseUpdateRequestBlock(%d) AL not found", p_Request->action_seq_no );
                }
                break;

            default:
                break;
            }   /*  end-of-switch */
        }
        else if ( p_Request->type == MYKI_UD_TYPE_TPURSE_UPDATE  )
        {
            switch ( searchType )
            {
            case    ACTION_TPURSE_UPDATE_BLOCK:
                if ( ( bFound = MYKI_ACTIONLIST_getTPurseUpdateRequestBlock( cardUID, p_Request->action_seq_no, &pData->ActionList.actionlist.tPurseUpdate ) ) == FALSE )
                {
                    CsErrx( "myki_br_GetActionlist : MYKI_ACTIONLIST_getTPurseUpdateRequestBlock(%d) AL not found", p_Request->action_seq_no );
                }
                break;

            case    ACTION_TPURSE_UPDATE_UNBLOCK:
                if ( ( bFound = MYKI_ACTIONLIST_getTPurseUpdateRequestUnblock( cardUID, p_Request->action_seq_no, &pData->ActionList.actionlist.tPurseUpdate ) ) == FALSE )
                {
                    CsErrx( "myki_br_GetActionlist : MYKI_ACTIONLIST_getTPurseUpdateRequestUnblock(%d) AL not found", p_Request->action_seq_no );
                }
                break;

            case    ACTION_TPURSE_UPDATE_NONE:
                if ( ( bFound = MYKI_ACTIONLIST_getTPurseUpdateRequestNone( cardUID, p_Request->action_seq_no, &pData->ActionList.actionlist.tPurseUpdate ) ) == FALSE )
                {
                    CsErrx( "myki_br_GetActionlist : MYKI_ACTIONLIST_getTPurseUpdateRequestNone(%d) AL not found", p_Request->action_seq_no );
                }
                break;

            default:
                break;
            }   /*  end-of-switch */
		}
        else if ( p_Request->type == MYKI_UD_TYPE_TPURSE_LOAD )
        {
            switch ( searchType )
            {
            case    ACTION_TPURSE_LOAD_DEBIT:
                if ( ( bFound = MYKI_ACTIONLIST_getTPurseLoadRequestDebit( cardUID, p_Request->action_seq_no, &pData->ActionList.actionlist.tPurseLoad ) ) == FALSE )
                {
                    CsErrx( "myki_br_GetActionlist : MYKI_ACTIONLIST_getTPurseLoadRequestDebit(%d) AL not found", p_Request->action_seq_no );
                }
                break;

            case    ACTION_TPURSE_LOAD_NONE:
                if ( ( bFound = MYKI_ACTIONLIST_getTPurseLoadRequestNone( cardUID, p_Request->action_seq_no, &pData->ActionList.actionlist.tPurseLoad ) ) == FALSE )
                {
                    CsErrx( "myki_br_GetActionlist : MYKI_ACTIONLIST_getTPurseLoadRequestNone(%d) AL not found", p_Request->action_seq_no );
                }
                break;

            default:
                break;
            }   /*  end-of-switch */
        }
        else if ( p_Request->type == MYKI_UD_TYPE_PRODUCT_SALE )
        {
            switch ( searchType )
            {
            case    ACTION_PRODUCT_SALE_NONE:
                if ( ( bFound = MYKI_ACTIONLIST_getProductSaleRequestNone( cardUID, p_Request->action_seq_no, &pData->ActionList.actionlist.productSale ) ) == FALSE )
                {
                    CsErrx( "myki_br_GetActionlist : MYKI_ACTIONLIST_getProductSaleRequestNone(%d) AL not found", p_Request->action_seq_no );
                }
                break;

            default:
                break;
            }   /*  end-of-switch */
        }
        else if ( p_Request->type == MYKI_UD_TYPE_OAPP_UPDATE )
        {
            switch ( searchType )
            {
            case    ACTION_OAPP_UPDATE_BLOCK:
                if ( ( bFound = MYKI_ACTIONLIST_getOAppUpdateRequestBlock( cardUID, p_Request->action_seq_no, &pData->ActionList.actionlist.OAppUpdate ) ) == FALSE )
                {
                    CsErrx( "myki_br_GetActionlist : MYKI_ACTIONLIST_getOAppUpdateRequestBlock(%d)", p_Request->action_seq_no );
                }
                break;

            case    ACTION_OAPP_UPDATE_UNBLOCK:
                if ( ( bFound = MYKI_ACTIONLIST_getOAppUpdateRequestUnblock( cardUID, p_Request->action_seq_no, &pData->ActionList.actionlist.OAppUpdate ) ) == FALSE )
                {
                    CsErrx( "myki_br_GetActionlist : MYKI_ACTIONLIST_getOAppUpdateRequestUnblock(%d) AL not found", p_Request->action_seq_no );
                }
                break;

            default:
                break;
            }   /*  end-of-switch */
        }   /*  end-of-if */

        if ( bFound != FALSE )
        {
            pData->ActionList.type  = searchType;
            if
            (
                (
                    searchActionSequenceNumber == 0 ||
                    searchActionSequenceNumber == p_Request->action_seq_no
                )
                &&
                (
                    searchProductSerialNumber == 0 ||
                    searchProductSerialNumber == pData->ActionList.actionlist.productUpdate.serialNo
                )
            )
            {
                /*  Found the desired actionlist record! */
                return 1;
            }   /*  end-of-if */
        }   /*  end-of-if */
    }   /*  end-of-for */

    CsDbg( BRLL_RULE, "myki_br_GetActionlist : desired action list not found." );

    return 0;
}   /*  myki_br_GetActionlist( ) */

//=============================================================================
//
//  Return the product card type (an internal enumeration) for a given product ID
//
//  Calls MYKI_CD to get the product "type" from CD as a string, then maps
//  known strings to product card types.
//
//=============================================================================

ProductCardType_e   myki_br_cd_GetProductCardType( int productId )
{
    int         i;
    char        cardtype[ 64 ];         // This should be a constant defined in the MYKI_CD API

    static  struct
    {
        char            *typeDesc;
        ProductType_e   type;

    }   productList[] =
    {
        { "DSC"         , PRODUCT_CARD_TYPE_DSC      },
        { "LLSC"        , PRODUCT_CARD_TYPE_LLSC     },
        { "ThirdParty"  , PRODUCT_CARD_TYPE_UNKNOWN  }   //  Check
    };

    if ( ! MYKI_CD_getProductCardType( (U8_t)productId, cardtype, sizeof( cardtype ) ) )
    {
        CsErrx( "myki_br_cd_GetProductCardType( %d ) MYKI_CD_getProductCardType() failed", productId );
        return PRODUCT_CARD_TYPE_UNKNOWN;
    }

    for ( i = 0; i < DIMOF( productList ); i++ )
    {
        if ( ! strcmp( productList[ i ].typeDesc, cardtype ) )
        {
            CsDbg( BRLL_RULE, "myki_br_cd_GetProductCardType( %d ) Product type \"%s\" is Product Card Type %d", productId, cardtype, productList[ i ].type );
            return productList[ i ].type;
        }
    }
    CsErrx( "myki_br_cd_GetProductCardType( %d ) Product type \"%s\" not known", productId, cardtype );
    return PRODUCT_CARD_TYPE_UNKNOWN;
}

/*==========================================================================*
**
**  myki_br_GetTransportModeString
**
**  Description     :
**      Returns text representation of specified transport mode.
**
**  Parameters      :
**      transportMode       [I]     transport mode
**
**  Returns         :
**      XXX                         text representation of transport mode
**
**  Notes           :
**
**==========================================================================*/

const char*         myki_br_GetTransportModeString( TransportMode_e transportMode )
{
    static  const char* TransportModeString[ ] =
    {
        "UNKNOWN",          /*  TRANSPORT_MODE_UNKNOWN */
        "BUS",              /*  TRANSPORT_MODE_BUS */
        "RAIL",             /*  TRANSPORT_MODE_RAIL */
        "TRAM",             /*  TRANSPORT_MODE_TRAM */
    };  /*  TransportModeString[] */

    if ( transportMode < TRANSPORT_MODE_UNKNOWN || transportMode > TRANSPORT_MODE_TRAM )
    {
        transportMode   = TRANSPORT_MODE_UNKNOWN;
    }

    return  TransportModeString[ transportMode ];
}   /*  myki_br_GetTransportModeString( ) */

//=============================================================================
//
//  Return the provider transport mode (an internal enumeration) for a given provider ID
//
//  Calls MYKI_CD to get the provider "mode" from CD as a string, then maps
//  known strings to provider transport modes.
//
//=============================================================================

TransportMode_e     myki_br_cd_GetTransportModeForProvider( int providerId )
{
    int         i;
    char        transportMode[ 64 ];    // This should be a constant defined in the MYKI_CD API

    static  struct
    {
        char            *modeDesc;
        TransportMode_e mode;

    }   providerList[] =
    {
        { "RAIL"    , TRANSPORT_MODE_RAIL },
        { "BUS"     , TRANSPORT_MODE_BUS  },
        { "TRAM"    , TRANSPORT_MODE_TRAM }
    };

    if ( ! MYKI_CD_getServiceProviderTransportMode( (U16_t)providerId, transportMode, sizeof( transportMode ) ) )
    {
        CsErrx( "myki_br_cd_GetTransportModeForProvider() MYKI_CD_getServiceProviderTransportMode() failed" );
        return TRANSPORT_MODE_UNKNOWN;
    }

    for ( i = 0; i < DIMOF( providerList ); i++ )
    {
        if ( ! strcmp( providerList[ i ].modeDesc, transportMode ) )
        {
            CsDbg( BRLL_RULE, "myki_br_cd_GetTransportModeForProvider() Provider mode \"%s\" is Transport Mode %d", providerList[ i ].modeDesc, providerList[ i ].mode );
            return providerList[ i ].mode;
        }
    }

    CsErrx( "myki_br_cd_GetProductType() Provider mode \"%s\" not known", transportMode);
    return TRANSPORT_MODE_UNKNOWN;
}

//=============================================================================
//
//  Get current transaction type determined by StaticData.isEntry and StaticData.isExit
//
//  Returns
//      TxType_e
//
//=============================================================================

MYKI_BR_LogTransactionType_t  myki_br_getTransactionType( MYKI_BR_ContextData_t *pData )
{
    static const MYKI_BR_LogTransactionType_t   LogTransactionTypes[ 4 ]   =
    {
        MYKI_BR_TRANSACTION_TYPE_ON_BOARD,          /* Entry = FALSE, Exit = FALSE */
        MYKI_BR_TRANSACTION_TYPE_EXIT_ONLY,         /* Entry = FALSE, Exit = TRUE  */
        MYKI_BR_TRANSACTION_TYPE_ENTRY_ONLY,        /* Entry = TRUE,  Exit = FALSE */
        MYKI_BR_TRANSACTION_TYPE_ENTRY_EXIT         /* Entry = TRUE,  Exit = TRUE  */
    };  /* LogTransactionTypes[ ] */

    return
        pData == NULL ?
            MYKI_BR_TRANSACTION_TYPE_UNKNOWN :
            LogTransactionTypes
            [
                ( pData->StaticData.isEntry == FALSE ? 0 : 2 ) +
                ( pData->StaticData.isExit  == FALSE ? 0 : 1 )
            ];
}

//=============================================================================
//
//  Return the "trip direction" as determined by the TripDir Status and
//  TripDir Value bits in the Product Control Bitmap for the given product.
//
//  Arguments
//      MYKI_TAProduct_t    *pProduct
//
//  Returns
//      The trip direction of the given product.
//
//  Status      Value       Result
//  0           0           Unknown
//  0           1           Disabled
//  1           0           Inbound
//  1           1           Outbound
//
//  While normally all functions that can return an error should have their
//  return value checked for an error value by the caller, in this case as
//  long as the caller passes a non-NULL pointer, then the caller does not
//  really need to check for an error return.
//
//=============================================================================

TripDirection_t myki_br_getProductTripDirection( MYKI_TAProduct_t *pProduct )
{
    if ( ! pProduct )
    {
        CsErrx( "myki_br_getProductTripDirection() : NULL pointer" );
        return TRIP_DIRECTION_ERROR;
    }

    if ( pProduct->ControlBitmap & TAPP_TPRODUCT_CONTROL_TRIPDIR_STATUS_BITMAP )
    {
        if ( pProduct->ControlBitmap & TAPP_TPRODUCT_CONTROL_TRIPDIR_VALUE_BITMAP )
            return TRIP_DIRECTION_OUTBOUND;
        else
            return TRIP_DIRECTION_INBOUND;
    }
    else
    {
        if ( pProduct->ControlBitmap & TAPP_TPRODUCT_CONTROL_TRIPDIR_VALUE_BITMAP )
            return TRIP_DIRECTION_DISABLED;
        else
            return TRIP_DIRECTION_UNKNOWN;
    }

    CsErrx( "myki_br_getProductTripDirection() : Should not have got here" );
    return TRIP_DIRECTION_ERROR;
}

//=============================================================================
//
//
//
//=============================================================================

int     myki_br_setProductTripDirection( MYKI_TAProduct_t *pProduct, TripDirection_t direction )
{
    return myki_br_SetBitmapDirection( &pProduct->ControlBitmap, direction );
}

//=============================================================================
//
//
//
//=============================================================================

int     myki_br_SetBitmapDirection( U8_t *pBitmap, TripDirection_t direction )
{
    *pBitmap &= ~( TAPP_TPRODUCT_CONTROL_TRIPDIR_STATUS_BITMAP | TAPP_TPRODUCT_CONTROL_TRIPDIR_VALUE_BITMAP );

    switch ( direction )
    {
        case TRIP_DIRECTION_OUTBOUND :
            *pBitmap |= TAPP_TPRODUCT_CONTROL_TRIPDIR_STATUS_BITMAP | TAPP_TPRODUCT_CONTROL_TRIPDIR_VALUE_BITMAP;
            break;
        case TRIP_DIRECTION_INBOUND :
            *pBitmap |= TAPP_TPRODUCT_CONTROL_TRIPDIR_STATUS_BITMAP;
            break;
        case TRIP_DIRECTION_DISABLED :
            *pBitmap |= TAPP_TPRODUCT_CONTROL_TRIPDIR_VALUE_BITMAP;
            break;
        case TRIP_DIRECTION_UNKNOWN :
        default :
            //  Both bits clear
            break;
    }

    return 0;
}

//=============================================================================
//
//
//
//=============================================================================

int     myki_br_SetBitmapBorderStatus( U8_t *pBitmap, BorderStatus_t borderStatus )
{
    switch ( borderStatus )
    {
        case BORDER_STATUS_OUTBOUND :
            *pBitmap |= TAPP_TPRODUCT_CONTROL_BORDER_STATUS_BITMAP | TAPP_TPRODUCT_CONTROL_BORDER_SIDE_BITMAP;
            break;
        case BORDER_STATUS_INBOUND :
            *pBitmap |= TAPP_TPRODUCT_CONTROL_BORDER_STATUS_BITMAP;
            *pBitmap &= ~TAPP_TPRODUCT_CONTROL_BORDER_SIDE_BITMAP;
            break;
        case BORDER_STATUS_NONE :
            *pBitmap &= ~TAPP_TPRODUCT_CONTROL_BORDER_STATUS_BITMAP;
            break;
        case BORDER_STATUS_UNKNOWN :
        default :
            //  Both bits clear
            *pBitmap &= ~( TAPP_TPRODUCT_CONTROL_BORDER_STATUS_BITMAP | TAPP_TPRODUCT_CONTROL_BORDER_SIDE_BITMAP );
            break;
    }

    return 0;
}

/*==========================================================================*
**
**  myki_br_setProductProvisionalStatus
**
**  Description     :
**      Sets/Clears TAppProduct.ControlBitmap.Provisional bit.
**
**  Parameters      :
**      pProduct            [I/O]   (copy of) Product object
**      status              [I]     TRUE if set; FALSE if clear
**
**  Returns         :
**      0                           success
**
**  Notes           :
**
**==========================================================================*/

int     myki_br_setProductProvisionalStatus( MYKI_TAProduct_t *pProduct, int status )
{
    if ( status )
    {
        pProduct->ControlBitmap |= TAPP_TPRODUCT_CONTROL_PROVISIONAL_BITMAP;
    }
    else
    {
        pProduct->ControlBitmap &= ~TAPP_TPRODUCT_CONTROL_PROVISIONAL_BITMAP;
    }

    return 0;
}

//=============================================================================
//
//
//
//=============================================================================

int     myki_br_setProductBorderStatus( MYKI_TAProduct_t *pProduct, int status )
{
    if ( status )
    {
        pProduct->ControlBitmap |= TAPP_TPRODUCT_CONTROL_BORDER_STATUS_BITMAP;
    }
    else
    {
        pProduct->ControlBitmap &= ~TAPP_TPRODUCT_CONTROL_BORDER_STATUS_BITMAP;
    }

    return 0;
}

//=============================================================================
//
//
//
//=============================================================================

int     myki_br_setProductOffPeakStatus( MYKI_TAProduct_t *pProduct, int status )
{
    if ( status )
    {
        pProduct->ControlBitmap |= TAPP_TPRODUCT_CONTROL_OFF_PEAK_BITMAP;
    }
    else
    {
        pProduct->ControlBitmap &= ~TAPP_TPRODUCT_CONTROL_OFF_PEAK_BITMAP;
    }

    return 0;
}

//=============================================================================
//
//
//
//=============================================================================

int     myki_br_ClearProposedMergeTripDirection( MYKI_BR_ContextData_t *pData )
{
    pData->InternalData.ProposedMerge.TripDirection = TRIP_DIRECTION_UNKNOWN;

    return pData->InternalData.ProposedMerge.TripDirection;
}

//=============================================================================
//
//  This function updates the proposed merge trip direction based on its
//  current value and the direction from a new product being merged.
//
//  The algorithm is somewhat complicated to describe, and the simplest way to describe it
//  is not the simplest way to implement it, so below is :
//  1. A truth table showing the result from each combination of current and new values.
//  2. Pseudo-code for the actual code.
//
//  Current     New         Result
//  --------    --------    --------
//  Disabled    Unknown     Disabled        Either one is disabled, result is disabled
//  Unknown     Disabled    Disabled        Either one is disabled, result is disabled
//  Disabled    Disabled    Disabled        Either one is disabled, result is disabled
//  Inbound     Disabled    Disabled        Either one is disabled, result is disabled
//  Outbound    Disabled    Disabled        Either one is disabled, result is disabled
//  Disabled    Inbound     Disabled        Either one is disabled, result is disabled
//  Disabled    Outbound    Disabled        Either one is disabled, result is disabled
//  Inbound     Outbound    Disabled        Both known and different, result is disabled
//  Outbound    Inbound     Disabled        Both known and different, result is disabled
//  Inbound     Inbound     Inbound         Both known and same, result is the same
//  Outbound    Outbound    Outbound        Both known and same, result is the same
//  Unknown     Inbound     Inbound         One known, other unknown, result is the known one
//  Unknown     Outbound    Outbound        One known, other unknown, result is the known one
//  Inbound     Unknown     Inbound         One known, other unknown, result is the known one
//  Outbound    Unknown     Outbound        One known, other unknown, result is the known one
//  Unknown     Unknown     Unknown         Both unknown, result is unknown
//
//  If new is not unknown
//      If current is unknown
//          Result = new            Current is unknown and new is known, switch to new
//      Else if current <> new
//          Result = Disabled       Both different and neither is unknown, set to Disabled
//      Else
//          Result = current        Both the same (and neither is unknown), leave current unchanged
//  Else
//      Result = current            New is unknown, leave current unchanged
//
//=============================================================================

int     myki_br_UpdateProposedMergeTripDirection( MYKI_BR_ContextData_t *pData, TripDirection_t newDirection )
{
    static const char  *StrTripDirection[ ] =
    {
        "UNKNOWN",
        "INBOUND",
        "OUTBOUND",
        "DISABLED",
        "ERROR"
    };

    CsDbg( BRLL_RULE, "myki_br_UpdateProposedMergeTripDirection : IN  ProposedMergedTripDirection = %s", StrTripDirection[ pData->InternalData.ProposedMerge.TripDirection ] );
    CsDbg( BRLL_RULE, "myki_br_UpdateProposedMergeTripDirection : IN  NewTripDirection            = %s", StrTripDirection[ newDirection ] );

    if ( newDirection != TRIP_DIRECTION_UNKNOWN )
    {
        if ( pData->InternalData.ProposedMerge.TripDirection == TRIP_DIRECTION_UNKNOWN )
        {
            pData->InternalData.ProposedMerge.TripDirection = newDirection;
        }
        else if ( pData->InternalData.ProposedMerge.TripDirection != newDirection )
        {
            pData->InternalData.ProposedMerge.TripDirection = TRIP_DIRECTION_DISABLED;
        }
    }

    CsDbg( BRLL_RULE, "myki_br_UpdateProposedMergeTripDirection : OUT ProposedMergedTripDirection = %s", StrTripDirection[ pData->InternalData.ProposedMerge.TripDirection ] );

    return pData->InternalData.ProposedMerge.TripDirection;
}

//=============================================================================
//
//  Returns TRUE if the two zone ranges are contiguous, or FALSE otherwise.
//
//  Assumes that in each zone range, the low zone is less than or equal to the
//  high zone.
//
//  Note that this function defines "contiguous" to include zone ranges which are
//  adjacent as well as truly overlapping.
//
//  For instance, 2-3,3-4 will return TRUE, and 2-3,4-5 will also return TRUE.
//
//=============================================================================

int     myki_br_ZonesOverlapOrAdjacent( int zone1Low, int zone1High, int zone2Low, int zone2High )
{
    if ( zone1High < zone2Low - 1 )
        return FALSE;

    if ( zone1Low > zone2High + 1 )
        return FALSE;

    return TRUE;
}

//=============================================================================
//
//  Returns TRUE if the two zone ranges overlap, or FALSE otherwise.
//
//  Assumes that in each zone range, the low zone is less than or equal to the
//  high zone.
//
//  Note that this function does not return TRUE for zones which are adjacent,
//  only for zones which truly overlap.
//
//  For instance, 2-3,3-4 will return TRUE, while 2-3,4-5 will return FALSE.
//
//=============================================================================

int     myki_br_ZonesOverlap( int zone1Low, int zone1High, int zone2Low, int zone2High )
{
    if ( zone1High < zone2Low )
        return FALSE;

    if ( zone1Low > zone2High )
        return FALSE;

    return TRUE;
}

//=============================================================================
//
//  Return the directory index of the oldest N-Hour product which is entirely
//  within the specified zone range.
//
//  Input:
//      pMYKI_TAControl     Pointer to card TAControl data
//      zoneLow             Low zone of the zone range to check
//      zoneHigh            High zone of the zone range to check
//
//  Output:
//     NONE
//
//  Returns:
//      Return the directory index of the oldest N-Hour product.
//      Return zero if no N-Hour product found.
//      Return (-1) on error.
//
//=============================================================================

int     myki_br_GetOldestNHourDirContained( MYKI_TAControl_t *pMYKI_TAControl, int zoneLow, int zoneHigh )
{
    Time_t  oldestTime = 0x7FFFFFFF;    // This should be improved - it assumes Time_t is an S32. There should be a TIME_T_MAX or similar in basetypes.
    int     oldestDir  = 0;             // Real products are dir indexes 1-5, so use 0 to mean "no nHour found"
    ProductIterator             iProduct;

    CsDbg( BRLL_RULE, "myki_br_GetOldestNHourDirContained : Get oldest nHour between zones %d - %d", zoneLow, zoneHigh );

    myki_br_InitProductIterator( pMYKI_TAControl, &iProduct, PRODUCT_TYPE_NHOUR );
    while ( myki_br_ProductIterate( &iProduct ) > 0 )
    {
        if ( iProduct.pProduct->ZoneLow < zoneLow || iProduct.pProduct->ZoneHigh > zoneHigh )
        {
            CsDbg( BRLL_FIELD, "myki_br_GetOldestNHourDirContained : Product %d not contained, ignore", iProduct.index );
            continue;
        }

        if ( iProduct.pProduct->EndDateTime >= oldestTime )
        {
            CsDbg( BRLL_FIELD, "myki_br_GetOldestNHourDirContained : Product %d (expiry %d) is not older than current oldest (expiry %d), ignore", iProduct.index, iProduct.pProduct->EndDateTime, oldestTime );
            continue;
        }

        CsDbg( BRLL_FIELD, "myki_br_GetOldestNHourDirContained : Current candidate is product %d (expiry %d)", iProduct.index, iProduct.pProduct->EndDateTime );
        oldestDir = iProduct.index;
        oldestTime = iProduct.pProduct->EndDateTime;
    }

    if ( iProduct.error )
    {
        CsErrx( "myki_br_GetOldestNHourDirContained : Iteration Failure, returning FALSE" );
        return FALSE;
    }

    CsDbg( BRLL_FIELD, "myki_br_GetOldestNHourDirContained : Oldest product is %d (expiry %d)", oldestDir, oldestTime );
    return oldestDir;
}

//=============================================================================
//
//  Return the directory index of the oldest N-Hour product which overlaps
//  the specified zone range.
//
//  Input:
//      pMYKI_TAControl     Pointer to card TAControl data
//      zoneLow             Low zone of the zone range to check
//      zoneHigh            High zone of the zone range to check
//
//  Output:
//     NONE
//
//  Returns:
//      Return the directory index of the oldest N-Hour product.
//      Return zero if no N-Hour product found.
//      Return (-1) on error.
//
//=============================================================================

int     myki_br_GetOldestNHourDirOverlap( MYKI_TAControl_t *pMYKI_TAControl, int zoneLow, int zoneHigh )
{
    Time_t  oldestTime = 0x7FFFFFFF;    // This should be improved - it assumes Time_t is an S32. There should be a TIME_T_MAX or similar in basetypes.
    int     oldestDir  = 0;             // Real products are dir indexes 1-5, so use 0 to mean "no nHour found"
    ProductIterator             iProduct;

    CsDbg( BRLL_RULE, "myki_br_GetOldestNHourDirOverlap : Get oldest nHour overlapping zones %d - %d", zoneLow, zoneHigh );

    myki_br_InitProductIterator( pMYKI_TAControl, &iProduct, PRODUCT_TYPE_NHOUR );
    while ( myki_br_ProductIterate( &iProduct ) > 0 )
    {
        if ( ! myki_br_ZonesOverlap( iProduct.pProduct->ZoneLow, iProduct.pProduct->ZoneHigh, zoneLow, zoneHigh ) )
        {
            CsDbg( BRLL_FIELD, "myki_br_GetOldestNHourDirOverlap : Product %d does not overlap, ignore", iProduct.index );
            continue;
        }

        if ( iProduct.pProduct->EndDateTime >= oldestTime )
        {
            CsDbg( BRLL_FIELD, "myki_br_GetOldestNHourDirOverlap : Product %d (expiry %d) is not older than current oldest (expiry %d), ignore", iProduct.index, iProduct.pProduct->EndDateTime, oldestTime );
            continue;
        }

        CsDbg( BRLL_FIELD, "myki_br_GetOldestNHourDirOverlap : Current candidate is product %d (expiry %d)", iProduct.index, iProduct.pProduct->EndDateTime );
        oldestDir = iProduct.index;
        oldestTime = iProduct.pProduct->EndDateTime;
    }

    if ( iProduct.error )
    {
        CsErrx( "myki_br_GetOldestNHourDirOverlap : Iteration Failure, returning FALSE" );
        return FALSE;
    }

    CsDbg( BRLL_FIELD, "myki_br_GetOldestNHourDirOverlap : Oldest product is %d (expiry %d)", oldestDir, oldestTime );
    return oldestDir;
}

//=============================================================================
//
//  Return the directory index of the oldest N-Hour product, ignoring zone
//  range entirely.
//
//  Input:
//      pMYKI_TAControl     Pointer to card TAControl data
//      zoneLow             Low zone of the zone range to check
//      zoneHigh            High zone of the zone range to check
//
//  Output:
//     NONE
//
//  Returns:
//      Return the directory index of the oldest N-Hour product.
//      Return zero if no N-Hour product found.
//      Return (-1) on error.
//
//=============================================================================

int     myki_br_GetOldestNHourDir( MYKI_TAControl_t *pMYKI_TAControl )
{
    Time_t  oldestTime = 0x7FFFFFFF;    // This should be improved - it assumes Time_t is an S32. There should be a TIME_T_MAX or similar in basetypes.
    int     oldestDir  = 0;             // Real products are dir indexes 1-5, so use 0 to mean "no nHour found"
    ProductIterator             iProduct;

    CsDbg( BRLL_RULE, "myki_br_GetOldestNHourDir : Get oldest nHour product (any zone range)" );

    myki_br_InitProductIterator( pMYKI_TAControl, &iProduct, PRODUCT_TYPE_NHOUR );
    while ( myki_br_ProductIterate( &iProduct ) > 0 )
    {
        if ( iProduct.pProduct->EndDateTime >= oldestTime )
        {
            CsDbg( BRLL_FIELD, "myki_br_GetOldestNHourDir : Product %d (expiry %d) is not older than current oldest (expiry %d), ignore", iProduct.index, iProduct.pProduct->EndDateTime, oldestTime );
            continue;
        }

        CsDbg( BRLL_FIELD, "myki_br_GetOldestNHourDir : Current candidate is product %d (expiry %d)", iProduct.index, iProduct.pProduct->EndDateTime );
        oldestDir = iProduct.index;
        oldestTime = iProduct.pProduct->EndDateTime;
    }

    if ( iProduct.error )
    {
        CsErrx( "myki_br_GetOldestNHourDir : Iteration Failure, returning FALSE" );
        return FALSE;
    }

    CsDbg( BRLL_FIELD, "myki_br_GetOldestNHourDir : Oldest product is %d (expiry %d)", oldestDir, oldestTime );
    return oldestDir;
}

//=============================================================================
//
//  Return the directory index of the product that SN equasl to the input productSerialNo
//
//
//  Input:
//      MYKI_TAProduct_t    *pProduct
//      U32_t                       productSerialNo
//
//  Output:
//     NONE
//
//  Returns:
//      Return the directory index
//      Return zero if not find
//
//=============================================================================

U8_t myki_br_GetDirWithSerial( MYKI_TAControl_t    *pMYKI_TAControl, U32_t productSerialNo)
{
    MYKI_Directory_t    *pDirectory = NULL;
    U8_t                 dir;

    if ( ! pMYKI_TAControl )
    {
        CsErrx( "myki_br_GetDirWithSerial() : pMYKI_TAControl is null." );
        return 0;
    }

    for (dir = 1; dir < DIMOF( pMYKI_TAControl->Directory ); dir++ ) // T-Purse dir entry does not have a product slot, so there's one less product slots than dir entries
    {

        pDirectory = &pMYKI_TAControl->Directory[ dir ]; // Product slot 0-4 maps to dir entry 1-5

        if ( pDirectory->SerialNo == productSerialNo )
            return dir;

    }

    return 0; // not found.
}


//=============================================================================
//
//  Get the product directory information, plus the product file information
//
//=============================================================================

int     myki_br_GetCardProduct( int dirIndex, MYKI_Directory_t **pDirectory, MYKI_TAProduct_t **pProduct )
{
    MYKI_TAControl_t            *pMYKI_TAControl;

    if ( pDirectory )
        *pDirectory = NULL;

    if ( pProduct )
        *pProduct = NULL;

    if ( dirIndex < 1 || dirIndex > DIMOF( pMYKI_TAControl->Directory ) - 1 )
    {
        CsErrx( "myki_br_GetCardProduct() Directory index %d out of range (must be between 1 and %d)", dirIndex, DIMOF( pMYKI_TAControl->Directory ) - 1 );
        return -1;
    }

    if ( ! pDirectory )
    {
        CsErrx( "myki_br_GetCardProduct() Directory buffer not specified" );
        return -1;
    }

    if ( MYKI_CS_TAControlGet( &pMYKI_TAControl ) < 0 )
    {
        CsErrx( "myki_br_GetCardProduct() MYKI_CS_TAControlGet() failed" );
        return -1;
    }

    *pDirectory = &pMYKI_TAControl->Directory[ dirIndex ];

    if ( pProduct )
    {
        if ( (*pDirectory)->Status == TAPP_CONTROL_DIRECTORY_STATUS_UNUSED )
        {
            CsErrx( "myki_br_GetCardProduct() Product %d is unused", dirIndex );
            return -1;
        }

        if ( MYKI_CS_TAProductGet( (U8_t)( dirIndex - 1 ), pProduct ) < 0 )
        {
            CsErrx( "myki_br_GetCardProduct() MYKI_CS_TAProductGet( %d ) failed", dirIndex - 1 );
            return -1;
        }
    }

    return 0;
}

/*----------------------------------------------------------------------------
** FUNCTION           : myki_br_ExtendProduct
**
** DESCRIPTION        : Extend a product as per BR_LLSC_6_14 and BR_LLSC_6_15
**                      Use with care. Its desinged to be used by BR_LLSC_6_14 and BR_LLSC_6_15
**
** INPUTS             : pData,              - Context
**                      pDirectory          - Ref to product directoru entry
**                      pProduct            - Ref to product
**                      zoneLow
**                      zoneHigh
**                      purchaseValue
**                      additionalMinutes   - If non-zero will update EndDateTime
**                      bitmap              - For direction and border status
**
** RETURNS            : 0  - Ok
**                      -1 - Not OK
**
----------------------------------------------------------------------------*/

int                 myki_br_ExtendProduct
(
    MYKI_BR_ContextData_t   *pData,
    MYKI_Directory_t        *pDirectory,
    MYKI_TAProduct_t        *pProduct,
    int                     zoneLow,
    int                     zoneHigh,
    Currency_t              purchaseValue,
    int                     additionalMinutes,
    U8_t                    bitmap,
    int                     alwaysCreateProductUpdateNone
)
{
    ProductUpdate_t         request;

    ProductUpdateInitialise( pData, pDirectory, &request );

    ADJUST_FOR_CITYSAVER( zoneLow, zoneHigh );

    request.isPurchaseValueSet      = TRUE;
    request.purchaseValue           = purchaseValue;
    request.isZoneLowSet            = TRUE;
    request.zoneLow                 = zoneLow;
    request.isZoneHighSet           = TRUE;
    request.zoneHigh                = zoneHigh;

    if ( additionalMinutes != 0 )
    {
        request.endDateTime         = myki_br_NHourEndDateTimeEx
                (
                    pData,
                    pProduct->StartDateTime,
                    myki_br_GetProductDuration( pData, PRODUCT_TYPE_NHOUR ),
                    MINUTES_TO_SECONDS( additionalMinutes )
                );
        request.isEndDateTimeSet    = TRUE;
    }

    DebugShowLdtProductUpdateRequest( BRLL_FIELD, &request );

#ifdef LDT_SUPPORTED

    if ( MYKI_LDT_ProductUpdateExtend( &request, &pData->InternalData.TransactionData ) != LDT_SUCCESS )
    {
        CsErrx( "myki_br_ExtendProduct() MYKI_LDT_ProductUpdateExtend() failed" );
        return -1;
    }

#endif

    //
    //  Now do a simple product update
    //
    MYKI_TAProduct_t    UpdatedProduct;

    //  First, create a copy of the product so we can modify it

    UpdatedProduct = *pProduct;
    UpdatedProduct.ControlBitmap = bitmap;

    //  Perform a product update/none only if there's a change in the product.
    if ( alwaysCreateProductUpdateNone )
    {
        if ( myki_br_ldt_ProductUpdate( pData, pDirectory, pProduct, &UpdatedProduct ) < 0 )
        {
            CsErrx( "myki_br_ExtendProduct : myki_br_ldt_ProductUpdate() failed" );
            return -1;
        }
    }
    else
    {
        if ( myki_br_ldt_ProductUpdate_IfChanged( pData, pDirectory, pProduct, &UpdatedProduct ) < 0 )
        {
            CsErrx( "myki_br_ExtendProduct : myki_br_ldt_ProductUpdate_IfChanged() failed" );
            return -1;
        }
    }


    return 0;
}

//=============================================================================
//
//  Create a preferential zone map.
//
//  This is an array containing an element for each possible zone in the system.
//  Each element contains the highest-priority activated non-provisional product
//  on the card that covers that zone. The lowest and highest zone covered by the
//  map are also recorded.
//
//  Notes :
//  -   It's possible for there to be gaps in the map between the lowest and highest covered zone.
//  -   The map *does not* include that have been issued but not activated.
//  -   The map *does not* include provisional products (other than the product in use).
//  -   The map *does* include expired products, as
//      (a) Products which were expired at scan-on will have been deleted at that point.
//      (b) Products which were valid at scan-on but are now expired are considered
//          still valid for the purposes of rules which use this zone map, and are
//          then deleted later (during scan-off possibly, but certainly at the next scan-on).
//
//=============================================================================

//  List of product types in order of priority from lowest to highest - higher priority products overwrite lower priority products in the zone map.
//  Used by both myki_br_CreateZoneMap() and myki_br_CalculateCombinedProductValue().

static  int         productPriorityList[] =
{
    PRODUCT_TYPE_EPASS,
    PRODUCT_TYPE_WEEKLY,
    PRODUCT_TYPE_DAILY,
    PRODUCT_TYPE_NHOUR
};

int     myki_br_CreateZoneMap( MYKI_BR_ContextData_t *pData, int zoneRangeLow, int zoneRangeHigh )
{
    MYKI_TAControl_t    *pMYKI_TAControl;
    int                 dir;
    int                 zone;
    ProductType_e       productType;
    int                 productPriority;
    int                 i;
    int                 dirIndex;
    int                 zoneLow;
    int                 zoneHigh;

    if ( MYKI_CS_TAControlGet( &pMYKI_TAControl ) < 0 )
    {
        CsErrx( "myki_br_CreateZoneMap : MYKI_CS_TAControlGet() failed" );
        return -1;
    }

    //
    //  Clear the zone map
    //

    for ( zone = 0; zone < DIMOF( pData->InternalData.ZoneMap.zoneList ); zone++ )
    {
        pData->InternalData.ZoneMap.zoneList[ zone ].priority = -1;  // Invalid priority
    }
    pData->InternalData.ZoneMap.zoneLow  = ZONE_MAP_MAX + 1;     // Greater than the maximum valid zone value
    pData->InternalData.ZoneMap.zoneHigh = ZONE_MAP_MIN - 1;     // Less than the minimum valid zone value

    //
    //  For each active, non-provisional product on the card (except the purse), update the zone map
    //

    for ( dir = 1; dir < DIMOF( pMYKI_TAControl->Directory ); dir++ )           // For each real product slot (ie, excluding purse)
    {
        MYKI_Directory_t    *pDirectory = NULL;
        MYKI_TAProduct_t    *pProduct = NULL;

        //  Just get the dir entry first

        if ( myki_br_GetCardProduct( dir, &pDirectory, NULL ) < 0 )
        {
            CsErrx( "myki_br_CreateZoneMap : myki_br_GetCardProduct() failed" );
            return -1;
        }

        //  Exclude products that are not active

        if ( pDirectory->Status != TAPP_CONTROL_DIRECTORY_STATUS_ACTIVATED )
        {
            CsDbg( BRLL_FIELD, "myki_br_CreateZoneMap : Index %d, product not activated, ignore", dir );
            continue;
        }

        //  Now get the product as well

        if ( myki_br_GetCardProduct( dir, &pDirectory, &pProduct ) < 0 )
        {
            CsErrx( "myki_br_CreateZoneMap : myki_br_GetCardProduct() failed" );
            return -1;
        }

        //  Exclude provisional products, other than the product in use

        if ( pProduct->ControlBitmap & TAPP_TPRODUCT_CONTROL_PROVISIONAL_BITMAP )
        {
//            if ( dir != pMYKI_TAControl->ProductInUse )
            {
                CsDbg( BRLL_FIELD, "myki_br_CreateZoneMap : Index %d, product is provisional, ignore", dir );
//                CsDbg( BRLL_FIELD, "myki_br_CreateZoneMap : Index %d, product is provisional and not in use, ignore", dir );
                continue;
            }
        }

        // Exclude products not in the zone range if it is defined - as used by BR_LLSC_6_14
        if ( zoneRangeLow > 0 && zoneRangeHigh > 0 )
        {
            if
            (
                pProduct->ZoneLow > zoneRangeHigh
                || pProduct->ZoneHigh < zoneRangeLow
            )
            {
                CsDbg( BRLL_FIELD, "myki_br_CreateZoneMap : Index %d, product zone not in range, ignore", dir );
                continue;
            }
        }

        //  Get the product type and find the priority for this product

        productType     = myki_br_cd_GetProductType( pDirectory->ProductId );
        productPriority = -1;           // Value if not found (-1)

        for ( i = 0; i < DIMOF( productPriorityList ); i++ )
        {
            if ( productPriorityList[ i ] == productType )
            {
                productPriority = i;    // Value if found (0-n)
                break;                  // Break out of this inner loop (not parent product search loop)
            }
        }

        //  Show this product's details for debugging

        CsDbg
        (
            BRLL_FIELD,
            "myki_br_CreateZoneMap : Index %d, Id = %d, Type = %s (%d), Priority = %d, Low = %d, High = %d\n",
            dir,
            pDirectory->ProductId,
            myki_br_GetProductTypeName( productType ),
            productType,
            productPriority,
            pProduct->ZoneLow,
            pProduct->ZoneHigh
        );

        if ( pProduct->ZoneLow < ZONE_MAP_MIN || pProduct->ZoneHigh > ZONE_MAP_MAX )
        {
            CsErrx( "myki_br_CreateZoneMap : Product %d has invalid zone range (%d - %d) should be within (%d - %d)", dir, pProduct->ZoneLow, pProduct->ZoneHigh, ZONE_MAP_MIN, ZONE_MAP_MAX );
            return -1;
        }

        //  For each zone this product covers, if the product's priority is higher than
        //  whatever product is already covering that zone, then update the zone to this
        //  product. Also update the low and high zones of the overall map.

        //  Consider changing zoneList structure to contain pointers to the product and directory rather
        //  than copies of individual fields from those pointers.

        for ( zone = pProduct->ZoneLow; zone <= pProduct->ZoneHigh; zone++ )
        {
            if ( productPriority > pData->InternalData.ZoneMap.zoneList[ zone ].priority )
            {
                pData->InternalData.ZoneMap.zoneList[ zone ].priority        = productPriority;
                pData->InternalData.ZoneMap.zoneList[ zone ].productType     = productType;
                pData->InternalData.ZoneMap.zoneList[ zone ].dirIndex        = dir;
                pData->InternalData.ZoneMap.zoneList[ zone ].purchaseValue   = pProduct->PurchaseValue;
                pData->InternalData.ZoneMap.zoneList[ zone ].isOffPeak       = pProduct->ControlBitmap & TAPP_TPRODUCT_CONTROL_OFF_PEAK_BITMAP;

                if ( zone < pData->InternalData.ZoneMap.zoneLow )
                    pData->InternalData.ZoneMap.zoneLow = zone;

                if ( zone > pData->InternalData.ZoneMap.zoneHigh )
                    pData->InternalData.ZoneMap.zoneHigh = zone;
            }
        }
    }

    //
    //  Create the zone product list - a list of each product in the zone map and which zone(s) it covers, in order of the zone map
    //

    dirIndex = -1;
    zoneLow  = -1;
    zoneHigh = -1;
    pData->InternalData.ZoneMap.productCount = 0;

    //  For each zone between the map's low zone and high zone (+1 so we process the final block in the last loop)
    for ( zone = pData->InternalData.ZoneMap.zoneLow; zone <= pData->InternalData.ZoneMap.zoneHigh + 1; zone++ )
    {
        //  If this is the last loop (ie one past the high zone) or if this zone has a new product
        if ( zone == pData->InternalData.ZoneMap.zoneHigh + 1 || pData->InternalData.ZoneMap.zoneList[ zone ].dirIndex != dirIndex )
        {
            if ( dirIndex >= 0 )        // Ignore the "last block" if this is the first block or the last block had no product allocated
            {
                zoneHigh = zone - 1;

                if ( pData->InternalData.ZoneMap.productCount >= DIMOF( pData->InternalData.ZoneMap.productList ) )
                {
                    CsErrx( "Internal error : more than %d product ranges on the card!", DIMOF( pData->InternalData.ZoneMap.productList ) );
                    return -1;
                }

                if ( pData->InternalData.ZoneMap.zoneList[ zoneLow ].productType != PRODUCT_TYPE_UNKNOWN )  // Only add ranges covered by a real product
                {
                    //  Consider changing productList structure to contain pointers to the product and directory rather
                    //  than copies of individual fields from those pointers.

                    pData->InternalData.ZoneMap.productList[ pData->InternalData.ZoneMap.productCount ].zoneLow         = zoneLow;
                    pData->InternalData.ZoneMap.productList[ pData->InternalData.ZoneMap.productCount ].zoneHigh        = zoneHigh;
                    pData->InternalData.ZoneMap.productList[ pData->InternalData.ZoneMap.productCount ].productType     = pData->InternalData.ZoneMap.zoneList[ zoneLow ].productType;
                    pData->InternalData.ZoneMap.productList[ pData->InternalData.ZoneMap.productCount ].dirIndex        = pData->InternalData.ZoneMap.zoneList[ zoneLow ].dirIndex;
                    pData->InternalData.ZoneMap.productList[ pData->InternalData.ZoneMap.productCount ].purchaseValue   = pData->InternalData.ZoneMap.zoneList[ zoneLow ].purchaseValue;
                    pData->InternalData.ZoneMap.productList[ pData->InternalData.ZoneMap.productCount ].isOffPeak       = pData->InternalData.ZoneMap.zoneList[ zoneLow ].isOffPeak;
                    pData->InternalData.ZoneMap.productCount++;
                }
            }

            dirIndex = pData->InternalData.ZoneMap.zoneList[ zone ].dirIndex;
            zoneLow  = zone;
        }
    }

    //
    //  Show the zone map and product list for debugging
    //

    CsDbg( BRLL_FIELD, "Zone Map (%d - %d) :\n", pData->InternalData.ZoneMap.zoneLow, pData->InternalData.ZoneMap.zoneHigh );
    for ( zone = pData->InternalData.ZoneMap.zoneLow; zone <= pData->InternalData.ZoneMap.zoneHigh; zone++ )
    {
        if ( pData->InternalData.ZoneMap.zoneList[ zone ].productType != PRODUCT_TYPE_UNKNOWN )
        {
            CsDbg
            (
                BRLL_FIELD,
                "  Zone %3d = %-7s at dir index %d : OffPeak = %s",
                zone,
                myki_br_GetProductTypeName( pData->InternalData.ZoneMap.zoneList[ zone ].productType ),
                pData->InternalData.ZoneMap.zoneList[ zone ].dirIndex,
                TF( pData->InternalData.ZoneMap.zoneList[ zone ].isOffPeak )
            );
        }
        else
        {
            CsDbg( BRLL_FIELD, "  Zone %3d = not covered\n", zone );
        }
    }

    CsDbg( BRLL_FIELD, "Product Map :\n" );
    for ( i = 0; i < pData->InternalData.ZoneMap.productCount; i++ )
    {
        CsDbg
        (
            BRLL_FIELD,
            "  Dir index %d = %-7s, zones %d - %d\n",
            i,
            myki_br_GetProductTypeName( pData->InternalData.ZoneMap.productList[ i ].productType ),
            pData->InternalData.ZoneMap.productList[ i ].zoneLow,
            pData->InternalData.ZoneMap.productList[ i ].zoneHigh
        );
    }

    return 0;
}

//=============================================================================
//
//
//
//=============================================================================

static  int     ZoneRangeProduct( ProductType_e productType )
{
    return
    (
        productType == PRODUCT_TYPE_EPASS  ||
        productType == PRODUCT_TYPE_WEEKLY ||
        productType == PRODUCT_TYPE_DAILY
    );
}

//=============================================================================
//
//  Calculate the combined product value of the zone map.
//  Must call myki_br_CreateZoneMap prior to this
//
//  There are two values to calculate :
//  -   Peak        Peak purchase value of every product
//  -   Off-Peak    Off-Peak purchase value of every product, which is the same as the Peak value for
//                  e-Pass, Daily and Weekly products, but may be discounted for nHour and Single products.
//
//  Algorithm :
//      -   For each contiguous block of ePass, Daily or Weekly products, use
//          the equivalent single-trip fare from CD as the fare for that zone range.
//      -   For each nHour product, use the product's purchase value as the value for that zone range.
//      -   The Peak fare is the sum of the full fares for each zone range.
//      -   The Off-Peak fare is the sum of the full fares for ePass, Daily and Weekly products, and
//          the discounted fare for nHour products.
//
//=============================================================================

int  myki_br_CalculateCombinedProductValue( MYKI_BR_ContextData_t *pData, int passengerCode, Currency_t *pPeak, Currency_t *pOffPeak )
{
    MYKI_TAControl_t   *pMYKI_TAControl = NULL;
    MYKI_TAProduct_t   *pProductInUse   = NULL;
    MYKI_Directory_t   *pDirectoryInUse = NULL;
    int                 zoneLow;
    int                 zoneHigh;
    Currency_t          fare;
    int                 zoneRangeProduct = FALSE;
    int                 zone;
    int                 i;

    if ( ! pPeak || ! pOffPeak )
    {
        CsErrx( "myki_br_CalculateCombinedProductValue : Invalid parameters" );
        return -1;
    }

    if ( MYKI_CS_TAControlGet( &pMYKI_TAControl ) < 0 )
    {
        CsErrx( "myki_br_CalculateCombinedProductValue : MYKI_CS_TAControlGet() failed" );
        return -1;
    }

    if ( pMYKI_TAControl->ProductInUse == 0 )
    {
        CsErrx( "myki_br_CalculateCombinedProductValue : No product in use" );
        return -1;
    }

    if ( myki_br_GetCardProduct( pMYKI_TAControl->ProductInUse, &pDirectoryInUse, &pProductInUse ) < 0 )
    {
        CsErrx( "myki_br_CalculateCombinedProductValue : myki_br_GetCardProduct(%d) failed", pMYKI_TAControl->ProductInUse );
        return -1;
    }

    *pPeak = 0;
    *pOffPeak = 0;

    //   6.  For each contiguous block of zones that resolves it self as type e-Pass, weekly or daily,
    //   a.  determine the fare as:
    //   i.  The zone range determined as determined by the zonal map
    //   ii.  Passenger type22
    //   iii.  Determined fare route20
    //   iv.  The current date time21
    //   b.  Accumulate this value in CombinedProductValue10

    for ( zone = pData->InternalData.ZoneMap.zoneLow; zone <= pData->InternalData.ZoneMap.zoneHigh + 1; zone++ )
    {
        if ( zone > pData->InternalData.ZoneMap.zoneHigh || ! ZoneRangeProduct( pData->InternalData.ZoneMap.zoneList[ zone ].productType ) )
        {
            if ( zoneRangeProduct )
            {
                //  End of a contiguous block of ePass / Weekly / Daily products
                zoneRangeProduct = FALSE;
                zoneHigh = zone - 1;

                // Fare is the peak fare for a single trip over this zone range for the current passenger type, route and date/time.
                if
                (
                    myki_br_getFareStoredValueEx
                    (
                        &fare,
						pData,
						zoneLow,
                        zoneHigh,
                        passengerCode,
                        pData->DynamicData.currentTripDirection,
                        pData->DynamicData.fareRouteIdIsValid,
                        pData->DynamicData.fareRouteId,
						pProductInUse->StartDateTime,
						pData->DynamicData.currentDateTime
                    ) < 0
                )
                {
                    CsErrx( "myki_br_CalculateCombinedProductValue : myki_br_getFareStoredValueEx() failed" );
                    return -1;
                }

                *pPeak += fare;
            }
        }
        else
        {
            if ( ! zoneRangeProduct )
            {
                //  Start of a new contiguous block of ePass / Weekly / Daily products
                zoneRangeProduct = TRUE;
                zoneLow = zone;
            }
        }
    }

    //  7.  Set the Combined off peak product valueA to the value of the combined product value10

    *pOffPeak = *pPeak;

    //   8.  For each contiguous block of products that contains 1 or more N-hour or Single Trip products; accumulate the product's purchase value into the CombinedProductvalue10
    //   9.  If the OffPeakDiscountRate34 is not zero then
    //   a.  For each contiguous bock of product that contains 1 or more n-hour or Single Trip products
    //   i.  If the product control bitmap indicates on the product indicates the current trip is an off-peak; Accumulate the combined off peak product purchase valueA as product value9 multiplied as 1 less OffPeakDiscountRate34
    //   ii.  Else increment the combined off peak productA value by the product value9.
    //   10. Else set the Combined off peak product valueA to the value of the combined product value10

    for ( i = 0; i < pData->InternalData.ZoneMap.productCount; i++ )
    {
        if ( ! ZoneRangeProduct( pData->InternalData.ZoneMap.productList[ i ].productType ) )
        {
            *pPeak += pData->InternalData.ZoneMap.productList[ i ].purchaseValue;

            if ( pData->DynamicData.offPeakDiscountRate > 0 && pData->InternalData.ZoneMap.productList[ i ].isOffPeak )
            {
                *pOffPeak += CalculateDiscountedFare( pData->InternalData.ZoneMap.productList[ i ].purchaseValue, pData->DynamicData.offPeakDiscountRate );
            }
            else
            {
                *pOffPeak += pData->InternalData.ZoneMap.productList[ i ].purchaseValue;
            }
        }
    }

    return 0;
}

//=============================================================================
//
//  Check whether an at least one active product covers each of the zones
//  in the current trip zone range.
//
//  Zone Map must have been created by calling myki_br_CreateZoneMap() before
//  this function is called.
//
//=============================================================================

int     myki_br_IsFullCoverage( MYKI_BR_ContextData_t *pData, int zoneLow, int zoneHigh )
{
    int     zone;

    CsDbg( BRLL_FIELD, "Check zone range %d to %d for full coverage\n", zoneLow, zoneHigh );

    //  Check for full coverage of the current trip
    //  Full coverage means that at least one active product covers each of the zones on the current trip

    for ( zone = zoneLow; zone <= zoneHigh; zone++ )
    {
        if ( pData->InternalData.ZoneMap.zoneList[ zone ].priority < 0 ) // Reject zones not covered plus zones where covering product is unknown type
        {
            return FALSE;
        }
    }

    return TRUE;
}

//=============================================================================
//
//  Show the details of the ProductUsage_t request structure
//
//=============================================================================

static  void    DebugShowLdtProductUsageRequest( int dbglvl, ProductUsage_t *request )
{
    CsDbg( dbglvl, "Product Usage:" );

    CsDbg( dbglvl, "  serialNo          : %d", request->serialNo );
    CsDbg( dbglvl, "  serviceProviderId : %d", request->serviceProviderId );
    CsDbg( dbglvl, "  entryPointId      : %d", request->entryPointId );
    CsDbg( dbglvl, "  routeId           : %d", request->routeId );
    CsDbg( dbglvl, "  stopId            : %d", request->stopId );
    CsDbg( dbglvl, "  zone              : %d", request->zone );
    CsDbg( dbglvl, "  dateTime          : %d", request->dateTime );
    CsDbg( dbglvl, "  Originating Information");
    CsDbg( dbglvl, "    isOriginatingServiceProviderIdSet : %d", request->isOriginatingServiceProviderIdSet );
    CsDbg( dbglvl, "      originatingServiceProviderId    : %d", request->originatingServiceProviderId );
    CsDbg( dbglvl, "    isOriginatingEntryPointIdSet      : %d", request->isOriginatingEntryPointIdSet );
    CsDbg( dbglvl, "      originatingEntryPointId         : %d", request->originatingEntryPointId );
    CsDbg( dbglvl, "    isOriginatingRouteIdSet           : %d", request->isOriginatingRouteIdSet );
    CsDbg( dbglvl, "      originatingRouteId              : %d", request->originatingRouteId );
    CsDbg( dbglvl, "    isOriginatingStopIdSet            : %d", request->isOriginatingStopIdSet );
    CsDbg( dbglvl, "      originatingStopId               : %d", request->originatingStopId );
}

//=============================================================================
//
//  Show the details of the ProductUpdate_t request structure
//
//=============================================================================

static  void    DebugShowLdtProductUpdateRequest( int dbglvl, ProductUpdate_t *request )
{
    CsDbg( dbglvl, "Product Update:" );
                                                CsDbg( dbglvl, "  serialNo                  = %d", request->serialNo );
                                                CsDbg( dbglvl, "  serviceProviderId         = %d", request->serviceProviderId );
    if ( request->isEntryPointIdSet         )   CsDbg( dbglvl, "  entryPointId              = %d", request->entryPointId );
    if ( request->isActionSequenceNoSet     )   CsDbg( dbglvl, "  actionSequenceNo          = %d", request->actionSequenceNo );
    if ( request->isAutoloadSet             )   CsDbg( dbglvl, "  isAutoload                = %d", request->isAutoload );
    if ( request->isOffpeakSet              )   CsDbg( dbglvl, "  isOffpeak                 = %d", request->isOffpeak );
    if ( request->isPremiumSet              )   CsDbg( dbglvl, "  isPremium                 = %d", request->isPremium );
    if ( request->isProvisionalSet          )   CsDbg( dbglvl, "  isProvisional             = %d", request->isProvisional );
    if ( request->isTripDirectionStatusSet  )   CsDbg( dbglvl, "  isTripDirectionStatus     = %d", request->isTripDirectionStatus );
    if ( request->isTripDirectionValueSet   )   CsDbg( dbglvl, "  isTripDirectionValue      = %d", request->isTripDirectionValue );
    if ( request->isBorderStatusSet         )   CsDbg( dbglvl, "  isBorderStatus            = %d", request->isBorderStatus );
    if ( request->isBorderSideSet           )   CsDbg( dbglvl, "  isBorderSide              = %d", request->isBorderSide );
                                                CsDbg( dbglvl, "  routeId                   = %d", request->routeId );
                                                CsDbg( dbglvl, "  stopId                    = %d", request->stopId );
    if ( request->isInstanceCountSet        )   CsDbg( dbglvl, "  instanceCount             = %d", request->instanceCount );
    if ( request->isStartDateTimeSet        )   CsDbg( dbglvl, "  startDateTime             = %d", request->startDateTime );
    if ( request->isEndDateTimeSet          )   CsDbg( dbglvl, "  endDateTime               = %d", request->endDateTime );
    if ( request->isZoneLowSet              )   CsDbg( dbglvl, "  zoneLow                   = %d", request->zoneLow );
    if ( request->isZoneHighSet             )   CsDbg( dbglvl, "  zoneHigh                  = %d", request->zoneHigh );
    if ( request->isPurchaseValueSet        )   CsDbg( dbglvl, "  purchaseValue             = %d", request->purchaseValue );
    if ( request->isClearBorderStatusSet    )   CsDbg( dbglvl, "  clearBorderStatus         = %d", request->clearBorderStatus );
    if ( request->isBorderStatusSet         )   CsDbg( dbglvl, "  BorderStatus              = %d", request->isBorderStatus );
}

/*==========================================================================*
**
**  DebugShowLdtProductSaleRequest
**
**  Description     :
**      Shows the details of the ProductSale_t request structure.
**
**  Parameters      :
**      dbglvl              [I]     Debug level
**      request             [I]     ProductSale_t request
**
**  Returns         :
**      None
**
**  Notes           :
**
**==========================================================================*/

static  void    DebugShowLdtProductSaleRequest( int dbglvl, const ProductSale_t *request )
{
    CsDbg(     dbglvl, "Product Sale:" );
    CsDbg(     dbglvl, "  productId                 = %d", request->productId                   );
    CsDbg(     dbglvl, "  issuerId                  = %d", request->issuerId                    );
    CsDbg(     dbglvl, "  purchaseValue             = %d", request->purchaseValue               );
    CsDbg(     dbglvl, "  zoneLow                   = %d", request->zoneLow                     );
    CsDbg(     dbglvl, "  zoneHigh                  = %d", request->zoneHigh                    );
    if ( request->isStartDateTimeSet != FALSE )
        CsDbg( dbglvl, "  startDateTime             = %d", request->startDateTime               );
    if ( request->isEndDateTimeSet != FALSE )
        CsDbg( dbglvl, "  endDateTime               = %d", request->endDateTime                 );
    CsDbg(     dbglvl, "  serviceProviderId         = %d", request->serviceProviderId           );
    if ( request->isActionSequenceNoSet != FALSE )
        CsDbg( dbglvl, "  actionSequenceNo          = %d", request->actionSequenceNo            );
    if ( request->isEntryPointIdSet != FALSE )
        CsDbg( dbglvl, "  entryPointId              = %d", request->entryPointId                );
    if ( request->isRouteIdSet != FALSE )
        CsDbg( dbglvl, "  routeId                   = %d", request->routeId                     );
    if ( request->isStopIdSet != FALSE )
        CsDbg( dbglvl, "  stopId                    = %d", request->stopId                      );
    if ( request->isAutoloadSet != FALSE )
        CsDbg( dbglvl, "  isAutoload                = %d", request->isAutoload                  );
    if ( request->isOffpeakSet != FALSE )
        CsDbg( dbglvl, "  isOffpeak                 = %d", request->isOffpeak                   );
    if ( request->isProvisionalSet != FALSE )
        CsDbg( dbglvl, "  isProvisional             = %d", request->isProvisional               );
    if ( request->isInstanceCountSet != FALSE )
        CsDbg( dbglvl, "  instanceCount             = %d", request->instanceCount               );
    CsDbg(     dbglvl, "  serialNo                  = %d", request->serialNo                    );
    CsDbg(     dbglvl, "  surchargeProductId        = %d", request->surchargeProductId          );
    CsDbg(     dbglvl, "  targetProductSerialNumber = %d", request->targetProductSerialNumber   );
}

/*==========================================================================*
**
**  DebugShowLdtAppUpdateRequest
**
**  Description     :
**      Shows the details of the TAppUpdate_t request structure.
**
**  Parameters      :
**      dbglvl              [I]     Debug level
**      request             [I]     TAppUpdate_t request
**
**  Returns         :
**      None
**
**  Notes           :
**
**==========================================================================*/

static  void    DebugShowLdtAppUpdateRequest( int dbglvl, const TAppUpdate_t *request )
{
    CsDbg( dbglvl, "App Update:" );
                                                    CsDbg( dbglvl, "  serviceProviderId     = %d", request->serviceProviderId );
    if ( request->isEntryPointIdSet             )   CsDbg( dbglvl, "  entryPointId          = %d", request->entryPointId );
    if ( request->isActionSequenceNoSet         )   CsDbg( dbglvl, "  actionSequenceNo      = %d", request->actionSequenceNo );
    if ( request->isExpiryDateSet               )   CsDbg( dbglvl, "  expiryDate            = %d", request->expiryDate );
    if ( request->isPassengerCodeSet            )   CsDbg( dbglvl, "  passengerCode         = %d", request->passengerCode );
    if ( request->isPassengerCodeExpirySet      )   CsDbg( dbglvl, "  passengerCodeExpiry   = %d", request->passengerCodeExpiry );
    if ( request->isPassengerControlSet         )   CsDbg( dbglvl, "  passengerControl      = %d", request->passengerControl );
    if ( request->isLanguageSet                 )   CsDbg( dbglvl, "  language              = %d", request->language );
                                                    CsDbg( dbglvl, "  blockingReason        = %d", request->blockingReason );
    if ( request->isSerialNoSet                 )   CsDbg( dbglvl, "  serialNo              = %d", request->serialNo );
}

/*==========================================================================*
**
**  DebugShowLdtTPurseLoadRequest
**
**  Description     :
**      Shows the details of the TPurseLoad_t request structure.
**
**  Parameters      :
**      dbglvl              [I]     Debug level
**      request             [I]     TPurseLoad_t request
**
**  Returns         :
**      None
**
**  Notes           :
**
**==========================================================================*/

static  void    DebugShowLdtTPurseLoadRequest( int dbglvl, const TPurseLoad_t *request )
{
    CsDbg(     dbglvl, "Purse Load:" );
    CsDbg(     dbglvl, "  value                     = %d",      request->value );
    CsDbg(     dbglvl, "  serviceProviderId         = %d",      request->serviceProviderId );
    if ( request->isEntryPointIdSet != FALSE )
        CsDbg( dbglvl, "  entryPointId              = %d",      request->entryPointId );
    if ( request->isRouteIdSet != FALSE )
        CsDbg( dbglvl, "  routeId                   = %d",      request->routeId );
    if ( request->isStopIdSet != FALSE )
        CsDbg( dbglvl, "  stopId                    = %d",      request->stopId );
    if ( request->isActionSequenceNoSet != FALSE )
        CsDbg( dbglvl, "  actionSequenceNo          = %d",      request->actionSequenceNo );
}   /* DebugShowLdtTPurseLoadRequest( ) */

/*==========================================================================*
**
**  DebugShowLdtTPurseUpdateRequest
**
**  Description     :
**      Shows the details of the TPurseUpdate_t request structure.
**
**  Parameters      :
**      dbglvl              [I]     Debug level
**      request             [I]     TPurseUpdate_t request
**
**  Returns         :
**      None
**
**  Notes           :
**
**==========================================================================*/

static  void    DebugShowLdtTPurseUpdateRequest( int dbglvl, const TPurseUpdate_t *request )
{
    CsDbg(     dbglvl, "Purse Update:" );
    CsDbg(     dbglvl, "  serviceProviderId         = %d",      request->serviceProviderId );
    if ( request->isEntryPointIdSet != FALSE )
        CsDbg( dbglvl, "  entryPointId              = %d",      request->entryPointId );
    if ( request->isActionSequenceNoSet != FALSE )
        CsDbg( dbglvl, "  actionSequenceNo          = %d",      request->actionSequenceNo );
    if ( request->isAutoloadSet != FALSE )
        CsDbg( dbglvl, "  isAutoload                = %d",      request->isAutoload );
    if ( request->isAutoThresholdSet != FALSE )
        CsDbg( dbglvl, "  autoThreshold             = %d",      request->autoThreshold );
    if ( request->isAutoValueSet != FALSE )
        CsDbg( dbglvl, "  autoValue                 = %d",      request->autoValue );
    if ( request->isTransitOnlySet != FALSE )
        CsDbg( dbglvl, "  isTransitOnly             = %d",      request->isTransitOnly );
    CsDbg(     dbglvl, "  currentDeviceDate         = %d",      request->currentDeviceDate );
}   /* DebugShowLdtTPurseUpdateRequest( ) */


/*==========================================================================*
**
**  DebugShowLdtCappingUpdateRequest
**
**  Description     :
**      Shows the details of the CappingUpdate_t request structure.
**
**  Parameters      :
**      dbglvl              [I]     Debug level
**      request             [I]     CappingUpdate_t request
**
**  Returns         :
**      None
**
**  Notes           :
**
**==========================================================================*/

static  void    DebugShowLdtCappingUpdateRequest( int dbglvl, const CappingUpdate_t *request )
{
    CsDbg( dbglvl, "Capping Update:" );
                                            CsDbg( dbglvl, "  serviceProviderId = %d", request->serviceProviderId );
                                            CsDbg( dbglvl, "  endOfBusinessDay  = %d", (int)(request->endOfBusinessDay) );
    if ( request->isEntryPointIdSet     )   CsDbg( dbglvl, "  entryPointId      = %d", request->entryPointId );
    if ( request->isDailyExpirySet      )   CsDbg( dbglvl, "  dailyExpiry       = %d", request->dailyExpiry );
    if ( request->isDailyZoneLowSet     )   CsDbg( dbglvl, "  dailyZoneLow      = %d", request->dailyZoneLow );
    if ( request->isDailyZoneHighSet    )   CsDbg( dbglvl, "  dailyZoneHigh     = %d", request->dailyZoneHigh );
    if ( request->isDailyValueSet       )   CsDbg( dbglvl, "  dailyValue        = %d", request->dailyValue );
    if ( request->isWeeklyExpirySet     )   CsDbg( dbglvl, "  weeklyExpiry      = %d", request->weeklyExpiry );
    if ( request->isWeeklyZoneLowSet    )   CsDbg( dbglvl, "  weeklyZoneLow     = %d", request->weeklyZoneLow );
    if ( request->isWeeklyZoneHighSet   )   CsDbg( dbglvl, "  weeklyZoneHigh    = %d", request->weeklyZoneHigh );
    if ( request->isWeeklyValueSet      )   CsDbg( dbglvl, "  weeklyValue       = %d", request->weeklyValue );
}

/*==========================================================================*
**
**  DebugShowLdtAddUsageLog
**
**  Description     :
**      Shows the details of the TAppUsageLog_t request structure.
**
**  Parameters      :
**      dbglvl              [I]     Debug level
**      request             [I]     TAppUsageLog_t request
**
**  Returns         :
**      None
**
**  Notes           :
**
**==========================================================================*/

static  void    DebugShowLdtAddUsageLog( int dbglvl, const TAppUsageLog_t *request )
{
    CsDbg(     dbglvl, "Add Usage Log:" );
    CsDbg(     dbglvl, "  version                   = %d", request->version );
    CsDbg(     dbglvl, "  controlBitmap             = %02X",    request->controlBitmap );
    CsDbg(     dbglvl, "  transactionType           = %d", request->transactionType );
    CsDbg(     dbglvl, "  providerId                = %d", request->providerId );
    CsDbg(     dbglvl, "  transactionDateTime       = %d", request->transactionDateTime );
    CsDbg(     dbglvl, "  entryPointId              = %d", request->entryPointId );
    CsDbg(     dbglvl, "  routeId                   = %d", request->routeId );
    CsDbg(     dbglvl, "  stopId                    = %d", request->stopId );
    if ( request->isTransactionValueSet != FALSE )
        CsDbg( dbglvl, "  transactionValue          = %d", request->transactionValue );
    if ( request->isNewTPurseBalanceSet != FALSE )
        CsDbg( dbglvl, "  newTPurseBalance          = %d", request->newTPurseBalance );
    if ( request->isPaymentMethodSet != FALSE )
        CsDbg( dbglvl, "  paymentMethod             = %d", request->paymentMethod );
    if ( request->isZoneSet != FALSE )
        CsDbg( dbglvl, "  zone                      = %d", request->zone );
    if ( request->isProductValidationStatusSet != FALSE )
        CsDbg( dbglvl, "  productValidationStatus   = %d", request->productValidationStatus );
    if ( request->isProductIssuerIdSet != FALSE )
        CsDbg( dbglvl, "  productIssuerId           = %d", request->productIssuerId );
    if ( request->isProductIdSet != FALSE )
        CsDbg( dbglvl, "  productId                 = %d", request->productId );
    if ( request->isProductSerialNoSet != FALSE )
        CsDbg( dbglvl, "  productSerialNo           = %d", request->productSerialNo );
}   /* DebugShowLdtUsageLog( ) */

/*==========================================================================*
**
**  DebugShowLdtAddLoadLog
**
**  Description     :
**      Shows the details of the TAppLoadLog_t request structure.
**
**  Parameters      :
**      dbglvl              [I]     Debug level
**      request             [I]     TAppLoadLog_t request
**
**  Returns         :
**      None
**
**  Notes           :
**
**==========================================================================*/

static  void    DebugShowLdtAddLoadLog( int dbglvl, const TAppLoadLog_t *request )
{
    CsDbg(     dbglvl, "Add Load Log:" );
    CsDbg(     dbglvl, "  version                   = %d",      request->version );
    CsDbg(     dbglvl, "  controlBitmap             = %02X",    request->controlBitmap );
    CsDbg(     dbglvl, "  transactionType           = %d",      request->transactionType );
    CsDbg(     dbglvl, "  transactionSequenceNumber = %d",      request->transactionSequenceNumber );
    CsDbg(     dbglvl, "  providerId                = %d",      request->providerId );
    CsDbg(     dbglvl, "  transactionDateTime       = %d",      request->transactionDateTime );
    CsDbg(     dbglvl, "  entryPointId              = %d",      request->entryPointId );
    CsDbg(     dbglvl, "  routeId                   = %d",      request->routeId );
    CsDbg(     dbglvl, "  stopId                    = %d",      request->stopId );
    if ( request->isTransactionValueSet != FALSE )
        CsDbg( dbglvl, "  transactionValue          = %d",      request->transactionValue );
    if ( request->isNewTPurseBalanceSet != FALSE )
        CsDbg( dbglvl, "  newTPurseBalance          = %d",      request->newTPurseBalance );
    if ( request->isPaymentMethodSet != FALSE )
        CsDbg( dbglvl, "  paymentMethod             = %d",      request->paymentMethod );
    if ( request->isProductIssuerIdSet != FALSE )
        CsDbg( dbglvl, "  productIssuerId           = %d",      request->productIssuerId );
    if ( request->isProductSerialNoSet != FALSE )
        CsDbg( dbglvl, "  productSerialNo           = %d",      request->productSerialNo );
    if ( request->isProductIdSet != FALSE )
        CsDbg( dbglvl, "  productId                 = %d",      request->productId );
}   /* DebugShowLdtAddLoadLog( ) */

/*----------------------------------------------------------------------------
** FUNCTION           : DebugShowLdtTPurseUsageRequest
**
** DESCRIPTION        : Useful display of the request information passed to
**                      MYKI_LDT_TPurseUsageXxxxx functions
**
**
** INPUTS             : dbglvl              - debug Level
**                      request             - Request Structure
**
** RETURNS            :
**
----------------------------------------------------------------------------*/

static  void    DebugShowLdtTPurseUsageRequest( int dbglvl, const TPurseUsage_t *request )
{
    CsDbg( dbglvl, "TPurseUsage:" );
                                                    CsDbg( dbglvl, "  serviceProviderId     = %d", request->serviceProviderId );
                                                    CsDbg( dbglvl, "  value                 = %d", request->value );
    if ( request->isEntryPointIdSet             )   CsDbg( dbglvl, "  entryPointId          = %d", request->entryPointId );
    if ( request->isRouteIdSet                  )   CsDbg( dbglvl, "  routeId               = %d", request->routeId );
    if ( request->isStopIdSet                   )   CsDbg( dbglvl, "  stopId                = %d", request->stopId );
}

/*----------------------------------------------------------------------------
** FUNCTION           : DebugShowLdtAddShiftSummaryLog
**
** DESCRIPTION        : Useful display of the request information passed to
**                      MYKI_LDT_OAppShiftXxxxx functions
**
**
** INPUTS             : dbglvl              - debug Level
**                      request             - Request Structure
**
** RETURNS            :
**
----------------------------------------------------------------------------*/

static  void    DebugShowLdtAddShiftDataSummaryLog( int dbglvl, const OAppShiftSummaryLog_t *request )
{
    CsDbg( dbglvl, "ShiftSummaryLogData:" );
                                                    CsDbg( dbglvl, "  shiftId               = %d", request->shiftId );
                                                    CsDbg( dbglvl, "  shiftSequenceNumber   = %d", request->shiftSequenceNumber );

                                                    CsDbg( dbglvl, "  samId                 = %d", request->samId );
                                                    CsDbg( dbglvl, "  startTime             = %d", request->startTime );
                                                    CsDbg( dbglvl, "  closeTime             = %d", request->closeTime );
                                                    CsDbg( dbglvl, "  depositSequenceNumber = %d", request->depositSequenceNumber);
                                                    CsDbg( dbglvl, "  depositTotal          = %d", request->depositTotal );
                                                    CsDbg( dbglvl, "  paperTicketReturns    = %d", request->paperTicketReturns );
                                                    CsDbg( dbglvl, "  sundryItemReturns     = %d", request->sundryItemReturns );
                                                    CsDbg( dbglvl, "  cardReturns           = %d", request->cardReturns );
}
//=============================================================================
//
//  Sets the common elements of the request structure for a product update
//  from the global context data.
//
//=============================================================================

static  int     ProductUpdateInitialise( MYKI_BR_ContextData_t *pData, MYKI_Directory_t *pDirectory, ProductUpdate_t *pRequest )
{
    memset( pRequest, 0, sizeof( *pRequest ) );

    pRequest->serialNo              = pDirectory->SerialNo;
    pRequest->serviceProviderId     = pData->StaticData.serviceProviderId;
    pRequest->routeId               = pData->DynamicData.lineId;
    pRequest->stopId                = pData->DynamicData.stopId;
    pRequest->entryPointId          = pData->DynamicData.entryPointId;
    pRequest->isEntryPointIdSet     = TRUE;

    return 0;
}

//=============================================================================
//
//  Finalises a generic product update.
//
//=============================================================================

static  int     ProductUpdateFinalise( MYKI_BR_ContextData_t *pData, ProductUpdate_t *pRequest )
{
    DebugShowLdtProductUpdateRequest( BRLL_FIELD, pRequest );

#ifdef LDT_SUPPORTED

    if ( MYKI_LDT_ProductUpdate( pRequest, &pData->InternalData.TransactionData ) != LDT_SUCCESS )
    {
        CsErrx( "ProductUpdateFinalise() MYKI_LDT_ProductUpdate() failed" );
        return -1;
    }

#endif

    return 0;
}

//=============================================================================
//
//  Set the request elements by comparing the old and new product details.
//
//  Returns TRUE if anything has changed between old and new products
//
//  Note :
//  ------
//  Normally single-line constructions like this should not be used, but in this
//  case it shows the tabular nature of the algorithm (it would be nice if it could
//  be implemented via tables, but probably overkill), and makes editing and
//  visual checking easier.
//
//  In essence, each line is :
//      if ( product new value != product old value )
//      {
//          set the corresponding request field flag to TRUE
//          set the corresponding request field value to the product new value
//      }
//
//=============================================================================

static  int    ProductUpdateSetRequest( ProductUpdate_t *pRequest, MYKI_TAProduct_t *pProductOld, MYKI_TAProduct_t *pProductNew )
{
    int result = 0;

    if ( pProductNew->ActionSeqNo       != pProductOld->ActionSeqNo     )     {   pRequest->isActionSequenceNoSet = result = TRUE;     pRequest->actionSequenceNo = pProductNew->ActionSeqNo;    }
    if ( pProductNew->InstanceCount     != pProductOld->InstanceCount   )     {   pRequest->isInstanceCountSet    = result = TRUE;     pRequest->instanceCount    = pProductNew->InstanceCount;  }
    if ( pProductNew->PurchaseValue     != pProductOld->PurchaseValue   )     {   pRequest->isPurchaseValueSet    = result = TRUE;     pRequest->purchaseValue    = pProductNew->PurchaseValue;  }
    if ( pProductNew->ZoneLow           != pProductOld->ZoneLow         )     {   pRequest->isZoneLowSet          = result = TRUE;     pRequest->zoneLow          = pProductNew->ZoneLow;        }
    if ( pProductNew->ZoneHigh          != pProductOld->ZoneHigh        )     {   pRequest->isZoneHighSet         = result = TRUE;     pRequest->zoneHigh         = pProductNew->ZoneHigh;       }
    if ( pProductNew->StartDateTime     != pProductOld->StartDateTime   )     {   pRequest->isStartDateTimeSet    = result = TRUE;     pRequest->startDateTime    = pProductNew->StartDateTime;  }
    if ( pProductNew->EndDateTime       != pProductOld->EndDateTime     )     {   pRequest->isEndDateTimeSet      = result = TRUE;     pRequest->endDateTime      = pProductNew->EndDateTime;    }

    if ( ( pProductNew->ControlBitmap & TAPP_TPRODUCT_CONTROL_AUTOLOAD_BITMAP       ) != ( pProductOld->ControlBitmap & TAPP_TPRODUCT_CONTROL_AUTOLOAD_BITMAP       ) )     { pRequest->isAutoloadSet            = result = TRUE;     pRequest->isAutoload            = pProductNew->ControlBitmap & TAPP_TPRODUCT_CONTROL_AUTOLOAD_BITMAP       ; }
    if ( ( pProductNew->ControlBitmap & TAPP_TPRODUCT_CONTROL_PROVISIONAL_BITMAP    ) != ( pProductOld->ControlBitmap & TAPP_TPRODUCT_CONTROL_PROVISIONAL_BITMAP    ) )     { pRequest->isProvisionalSet         = result = TRUE;     pRequest->isProvisional         = pProductNew->ControlBitmap & TAPP_TPRODUCT_CONTROL_PROVISIONAL_BITMAP    ; }
    if ( ( pProductNew->ControlBitmap & TAPP_TPRODUCT_CONTROL_OFF_PEAK_BITMAP       ) != ( pProductOld->ControlBitmap & TAPP_TPRODUCT_CONTROL_OFF_PEAK_BITMAP       ) )     { pRequest->isOffpeakSet             = result = TRUE;     pRequest->isOffpeak             = pProductNew->ControlBitmap & TAPP_TPRODUCT_CONTROL_OFF_PEAK_BITMAP       ; }
    if ( ( pProductNew->ControlBitmap & TAPP_TPRODUCT_CONTROL_PREMIUM_BITMAP        ) != ( pProductOld->ControlBitmap & TAPP_TPRODUCT_CONTROL_PREMIUM_BITMAP        ) )     { pRequest->isPremiumSet             = result = TRUE;     pRequest->isPremium             = pProductNew->ControlBitmap & TAPP_TPRODUCT_CONTROL_PREMIUM_BITMAP        ; }
    if ( ( pProductNew->ControlBitmap & TAPP_TPRODUCT_CONTROL_TRIPDIR_STATUS_BITMAP ) != ( pProductOld->ControlBitmap & TAPP_TPRODUCT_CONTROL_TRIPDIR_STATUS_BITMAP ) )     { pRequest->isTripDirectionStatusSet = result = TRUE;     pRequest->isTripDirectionStatus = pProductNew->ControlBitmap & TAPP_TPRODUCT_CONTROL_TRIPDIR_STATUS_BITMAP ; }
    if ( ( pProductNew->ControlBitmap & TAPP_TPRODUCT_CONTROL_TRIPDIR_VALUE_BITMAP  ) != ( pProductOld->ControlBitmap & TAPP_TPRODUCT_CONTROL_TRIPDIR_VALUE_BITMAP  ) )     { pRequest->isTripDirectionValueSet  = result = TRUE;     pRequest->isTripDirectionValue  = pProductNew->ControlBitmap & TAPP_TPRODUCT_CONTROL_TRIPDIR_VALUE_BITMAP  ; }
    if ( ( pProductNew->ControlBitmap & TAPP_TPRODUCT_CONTROL_BORDER_STATUS_BITMAP  ) != ( pProductOld->ControlBitmap & TAPP_TPRODUCT_CONTROL_BORDER_STATUS_BITMAP  ) )     { pRequest->isBorderStatusSet        = result = TRUE;     pRequest->isBorderStatus        = pProductNew->ControlBitmap & TAPP_TPRODUCT_CONTROL_BORDER_STATUS_BITMAP  ; }
    if ( ( pProductNew->ControlBitmap & TAPP_TPRODUCT_CONTROL_BORDER_SIDE_BITMAP    ) != ( pProductOld->ControlBitmap & TAPP_TPRODUCT_CONTROL_BORDER_SIDE_BITMAP    ) )     { pRequest->isBorderSideSet          = result = TRUE;     pRequest->isBorderSide          = pProductNew->ControlBitmap & TAPP_TPRODUCT_CONTROL_BORDER_SIDE_BITMAP    ; }

    return result;
}

//=============================================================================
//
//  Perform a generic product update. Takes a pointer to an original copy
//  of the product and a pointer to an second copy with changes, and creates
//  an appropriate LDT Product Update request then performs the update.
//
//=============================================================================

int         myki_br_ldt_ProductUpdate( MYKI_BR_ContextData_t *pData, MYKI_Directory_t *pDirectory, MYKI_TAProduct_t *pProductOld, MYKI_TAProduct_t *pProductNew )
{
    ProductUpdate_t         request;

    ProductUpdateInitialise( pData, pDirectory, &request );

    ProductUpdateSetRequest( &request, pProductOld, pProductNew );

    return ProductUpdateFinalise( pData, &request );
}

//=============================================================================
//
//  Perform a generic product update if there are changes. Takes a pointer to an
//  original copy of the product and a pointer to an second copy with changes,
//  and creates an appropriate LDT Product Update request then performs the
//  update.
//
//=============================================================================

int         myki_br_ldt_ProductUpdate_IfChanged( MYKI_BR_ContextData_t *pData, MYKI_Directory_t *pDirectory, MYKI_TAProduct_t *pProductOld, MYKI_TAProduct_t *pProductNew )
{
    ProductUpdate_t         request;

    ProductUpdateInitialise( pData, pDirectory, &request );

    if ( ProductUpdateSetRequest( &request, pProductOld, pProductNew ) == TRUE )
    {
        return ProductUpdateFinalise( pData, &request );
    }
    return 0;
}

//=============================================================================
//
//  Extend the validity of a product.
//
//  This function should only be used when :
//  (a)     The product expiry date/time is being extended, and/or
//  (b)     The product zone range is being extended.
//
//=============================================================================

int         myki_br_ldt_ProductUpdate_Extend
(
    MYKI_BR_ContextData_t  *pData,
    MYKI_Directory_t       *pDirectory,
    MYKI_TAProduct_t       *pProductOld,
    MYKI_TAProduct_t       *pProductNew,
    int                     isClearBorderStatus,
    int                     showEntryPointId,
    int                     includeExtraFields      // Include three extra fields (Purchase Value, Zone Low and Zone High) that ACS sometimes includes and sometimes doesn't.
)
{
    ProductUpdate_t         request;

    ProductUpdateInitialise( pData, pDirectory, &request );

    ProductUpdateSetRequest( &request, pProductOld, pProductNew );

    //
    //  Equivalent to ProductUpdateFinalise() but calls MYKI_LDT_ProductUpdateExtend() instead of MYKI_LDT_ProductUpdate()
    //

    if ( includeExtraFields )
    {
        request.isPurchaseValueSet  = TRUE;
        request.purchaseValue       = pProductNew->PurchaseValue;

        request.isZoneLowSet        = TRUE;
        request.zoneLow             = pProductNew->ZoneLow;

        request.isZoneHighSet       = TRUE;
        request.zoneHigh            = pProductNew->ZoneHigh;
    }

    // If "Clear the BorderStatus bit of the product control bitmap" is called,
    // set the bitmap so that the control bitmap appears in the LDT even
    // though there may be no change required.
    if ( isClearBorderStatus )
    {
        request.isClearBorderStatusSet     = TRUE;
        request.clearBorderStatus          = TRUE;
    }

    // Hide the optional EntryPointId field from Product Update Extend.
    // It appears that LLSC_4_19 is calling Product Update Extend without this field set in LDT.
    if ( !showEntryPointId )
    {
        request.isEntryPointIdSet          = FALSE;
    }

    DebugShowLdtProductUpdateRequest( BRLL_FIELD, &request );

#ifdef LDT_SUPPORTED

    if ( MYKI_LDT_ProductUpdateExtend( &request, &pData->InternalData.TransactionData ) != LDT_SUCCESS )
    {
        CsErrx( "myki_br_ldt_ProductUpdate_Extend() MYKI_LDT_ProductUpdateExtend() failed" );
        return -1;
    }

#endif

    return 0;
}

//=============================================================================
//
//  Convenience function to clear the border status on a product.
//
//  Only use this function if clearing the border status is the only action
//  being applied. If more than one action is being applied, use the generic
//  myki_br_ldt_ProductUpdate() function.
//
//=============================================================================

int         myki_br_ldt_ProductUpdate_ClearBorderStatus( MYKI_BR_ContextData_t *pData, MYKI_Directory_t *pDirectory )
{
    ProductUpdate_t         request;

    ProductUpdateInitialise( pData, pDirectory, &request );

    request.isBorderStatusSet       = TRUE;
    request.isBorderStatus          = FALSE;

    return ProductUpdateFinalise( pData, &request );
}

//=============================================================================
//
//  Convenience function to clear the premium surcharge status on a product.
//
//  Only use this function if clearing the premium surcharge status is the only action
//  being applied. If more than one action is being applied, use the generic
//  myki_br_ldt_ProductUpdate() function.
//
//=============================================================================

int         myki_br_ldt_ProductUpdate_ClearPremiumSurcharge( MYKI_BR_ContextData_t *pData, MYKI_Directory_t *pDirectory )
{
    if ( pData->StaticData.AcsCompatibilityMode )
    {
        //  NOTE:   KAMCO reader does not perform ProductUpdate/ClearPremiumSurcharge action.

        CsDbg( BRLL_RULE, "myki_br_ldt_ProductUpdate_ClearPremiumSurcharge : not perform ProductUpdate/ClearPremiumSurcharge" );

        return 0;
    }
    else
    {
        ProductUpdate_t         request;

        ProductUpdateInitialise( pData, pDirectory, &request );

        request.isPremiumSet            = TRUE;
        request.isPremium               = FALSE;

        return ProductUpdateFinalise( pData, &request );
    }
}

//=============================================================================
//
//  Convenience function to clear the autoload status on a product.
//
//  Only use this function if clearing the autoload status is the only action
//  being applied. If more than one action is being applied, use the generic
//  myki_br_ldt_ProductUpdate() function.
//
//=============================================================================

int         myki_br_ldt_ProductUpdate_ClearAutoloadStatus( MYKI_BR_ContextData_t *pData, MYKI_Directory_t *pDirectory )
{
    ProductUpdate_t         request;

    ProductUpdateInitialise( pData, pDirectory, &request );

    request.isAutoloadSet           = TRUE;
    request.isAutoload              = FALSE;

    return ProductUpdateFinalise( pData, &request );
}

//=============================================================================
//
//  Convenience function to set the trip direction of a product.
//
//  Only use this function if setting the trip direction is the only action
//  being applied. If more than one action is being applied, use the generic
//  myki_br_ldt_ProductUpdate() function.
//
//=============================================================================

int         myki_br_ldt_ProductUpdate_SetTripDirection( MYKI_BR_ContextData_t *pData, MYKI_Directory_t *pDirectory, TripDirection_t direction )
{
    ProductUpdate_t         request;

    ProductUpdateInitialise( pData, pDirectory, &request );

    request.isTripDirectionStatusSet    = TRUE;
    request.isTripDirectionValueSet     = TRUE;

    switch ( direction )
    {
        case TRIP_DIRECTION_OUTBOUND :
            request.isTripDirectionStatus = TRUE;
            request.isTripDirectionValue  = TRUE;
            break;
        case TRIP_DIRECTION_INBOUND :
            request.isTripDirectionStatus = TRUE;
            request.isTripDirectionValue  = FALSE;
            break;
        case TRIP_DIRECTION_DISABLED :
            request.isTripDirectionStatus = FALSE;
            request.isTripDirectionValue  = TRUE;
            break;
        case TRIP_DIRECTION_UNKNOWN :
        default :
            request.isTripDirectionStatus = FALSE;
            request.isTripDirectionValue  = FALSE;
            break;
    }

    return ProductUpdateFinalise( pData, &request );
}

//=============================================================================
//
//  Convenience function to clear the border status on a product.
//
//  Only use this function if clearing the border status is the only action
//  being applied. If more than one action is being applied, use the generic
//  myki_br_ldt_ProductUpdate() function.
//
//=============================================================================

int         myki_br_ldt_ProductUpdate_SetOffPeak( MYKI_BR_ContextData_t *pData, MYKI_Directory_t *pDirectory )
{
    ProductUpdate_t         request;

    ProductUpdateInitialise( pData, pDirectory, &request );

    request.isOffpeakSet  = TRUE;
    request.isOffpeak     = TRUE;

    return ProductUpdateFinalise( pData, &request );
}

//=============================================================================
//
//  Convenience function to extend the expiry date on a product.
//
//  Only use this function if extending the date is the only action
//  being applied. If more than one action is being applied, use the generic
//  myki_br_ldt_ProductUpdateExtend() function.
//
//=============================================================================

int         myki_br_ldt_ProductUpdate_ExtendExpiryDateTime(
    MYKI_BR_ContextData_t  *pData,
    MYKI_Directory_t       *pDirectory,
    Time_t                  expiryDateTime,
    int                     showEntryPointId )
{
    ProductUpdate_t         request;

    ProductUpdateInitialise( pData, pDirectory, &request );

    request.isEndDateTimeSet        = TRUE;
    request.endDateTime             = expiryDateTime;

    //
    //  Equivalent to ProductUpdateFinalise() but calls MYKI_LDT_ProductUpdateExtend() instead of MYKI_LDT_ProductUpdate()
    //

    // A flag to hide the optional EntryPointId field from Product Update Extend.
    // BR_LLSC_6_15 is calling Product Update Extend without this field being set
    // in the LDT for certain steps.
    if ( !showEntryPointId )
    {
        request.isEntryPointIdSet          = FALSE;
    }

    DebugShowLdtProductUpdateRequest( BRLL_FIELD, &request );

#ifdef LDT_SUPPORTED

    if ( MYKI_LDT_ProductUpdateExtend( &request, &pData->InternalData.TransactionData ) != LDT_SUCCESS )
    {
        CsErrx( "myki_br_ldt_ProductUpdate_Extend() MYKI_LDT_ProductUpdateExtend() failed" );
        return -1;
    }

#endif

    return 0;
}

//=============================================================================
//
//  Perform the actionlist specified in the Actionlist union within the
//  BR Context data.
//
//=============================================================================

int                 myki_br_ldt_Actionlist( MYKI_BR_ContextData_t  *pData)
{
#ifdef LDT_SUPPORTED

	switch(pData->ActionList.type)
	{
	case ACTION_NULL:
		CsErrx( "myki_br_ldt_Actionlist ACTION_NULL type provided" );
		break;
    case ACTION_TAPP_UPDATE_BLOCK:
		DebugShowLdtAppUpdateRequest( BRLL_FIELD, &pData->ActionList.actionlist.tAppUpdate );
		if ( MYKI_LDT_TAppUpdateBlock( &pData->ActionList.actionlist.tAppUpdate, &pData->InternalData.TransactionData ) != LDT_SUCCESS )
		{
			CsErrx( "myki_br_ldt_Actionlist() MYKI_LDT_TAppUpdateBlock() failed" );
			return -1;
		}
		break;
    case ACTION_TAPP_UPDATE_UNBLOCK:
		DebugShowLdtAppUpdateRequest( BRLL_FIELD, &pData->ActionList.actionlist.tAppUpdate );
		if ( MYKI_LDT_TAppUpdateUnblock( &pData->ActionList.actionlist.tAppUpdate, &pData->InternalData.TransactionData ) != LDT_SUCCESS )
		{
			CsErrx( "myki_br_ldt_Actionlist() MYKI_LDT_TAppUpdateUnblock() failed" );
			return -1;
		}
		break;
    case ACTION_TAPP_UPDATE_ACTIVATE:
		DebugShowLdtAppUpdateRequest( BRLL_FIELD, &pData->ActionList.actionlist.tAppUpdate );
		if ( MYKI_LDT_TAppUpdateActivate( &pData->ActionList.actionlist.tAppUpdate, &pData->InternalData.TransactionData ) != LDT_SUCCESS )
		{
			CsErrx( "myki_br_ldt_Actionlist() MYKI_LDT_TAppUpdateActivate() failed" );
			return -1;
		}
		break;
    case ACTION_TAPP_UPDATE_NONE:
		DebugShowLdtAppUpdateRequest( BRLL_FIELD, &pData->ActionList.actionlist.tAppUpdate );
		if ( MYKI_LDT_TAppUpdate( &pData->ActionList.actionlist.tAppUpdate, &pData->InternalData.TransactionData ) != LDT_SUCCESS )
		{
			CsErrx( "myki_br_ldt_Actionlist() MYKI_LDT_TAppUpdate() failed" );
			return -1;
		}
		break;
// Missing no product Update Activate
//
// Missing no product Update Invalidate
//
// Missing no product Update Extend
//
    case ACTION_PRODUCT_UPDATE_BLOCK:
		DebugShowLdtProductUpdateRequest( BRLL_FIELD, &pData->ActionList.actionlist.productUpdate );
		if ( MYKI_LDT_ProductUpdateBlock( &pData->ActionList.actionlist.productUpdate, &pData->InternalData.TransactionData ) != LDT_SUCCESS )
		{
			CsErrx( "myki_br_ldt_Actionlist() MYKI_LDT_ProductUpdateBlock() failed" );
			return -1;
		}
		break;
    case ACTION_PRODUCT_UPDATE_UNBLOCK:
		DebugShowLdtProductUpdateRequest( BRLL_FIELD, &pData->ActionList.actionlist.productUpdate );
		if ( MYKI_LDT_ProductUpdateUnblock( &pData->ActionList.actionlist.productUpdate, &pData->InternalData.TransactionData ) != LDT_SUCCESS )
		{
			CsErrx( "myki_br_ldt_Actionlist() MYKI_LDT_ProductUpdateUnblock() failed" );
			return -1;
		}
		break;
    case ACTION_PRODUCT_UPDATE_NONE:
		DebugShowLdtProductUpdateRequest( BRLL_FIELD, &pData->ActionList.actionlist.productUpdate );
		if ( MYKI_LDT_ProductUpdate( &pData->ActionList.actionlist.productUpdate, &pData->InternalData.TransactionData ) != LDT_SUCCESS )
		{
			CsErrx( "myki_br_ldt_Actionlist() MYKI_LDT_ProductUpdateUnblock() failed" );
			return -1;
		}
		break;
    case ACTION_TPURSE_UPDATE_BLOCK:
		DebugShowLdtTPurseUpdateRequest( BRLL_FIELD, &pData->ActionList.actionlist.tPurseUpdate );
		if ( MYKI_LDT_TPurseUpdateBlock( &pData->ActionList.actionlist.tPurseUpdate, &pData->InternalData.TransactionData ) != LDT_SUCCESS )
		{
			CsErrx( "myki_br_ldt_Actionlist() MYKI_LDT_TPurseUpdateBlock() failed" );
			return -1;
		}
		break;
    case ACTION_TPURSE_UPDATE_UNBLOCK:
		DebugShowLdtTPurseUpdateRequest( BRLL_FIELD, &pData->ActionList.actionlist.tPurseUpdate );
		if ( MYKI_LDT_TPurseUpdateUnblock( &pData->ActionList.actionlist.tPurseUpdate, &pData->InternalData.TransactionData ) != LDT_SUCCESS )
		{
			CsErrx( "myki_br_ldt_Actionlist() MYKI_LDT_TPurseUpdateUnblock() failed" );
			return -1;
		}
		break;
    case ACTION_TPURSE_UPDATE_NONE:
		DebugShowLdtTPurseUpdateRequest( BRLL_FIELD, &pData->ActionList.actionlist.tPurseUpdate );
		if ( MYKI_LDT_TPurseUpdate( &pData->ActionList.actionlist.tPurseUpdate, &pData->InternalData.TransactionData ) != LDT_SUCCESS )
		{
			CsErrx( "myki_br_ldt_Actionlist() MYKI_LDT_TPurseUpdate() failed" );
			return -1;
		}
		break;
    case ACTION_TPURSE_LOAD_DEBIT: // Assumption MYKI_LDT_TPurseLoadReverse = ACTION_TPURSE_LOAD_DEBIT
		DebugShowLdtTPurseLoadRequest( BRLL_FIELD, &pData->ActionList.actionlist.tPurseLoad );
		if ( MYKI_LDT_TPurseLoadDebit( &pData->ActionList.actionlist.tPurseLoad, &pData->InternalData.TransactionData ) != LDT_SUCCESS )
		{
			CsErrx( "myki_br_ldt_Actionlist() MYKI_LDT_TPurseUpdate() failed" );
			return -1;
		}
		break;
    case ACTION_TPURSE_LOAD_NONE:
		DebugShowLdtTPurseLoadRequest( BRLL_FIELD, &pData->ActionList.actionlist.tPurseLoad );
		if ( MYKI_LDT_TPurseLoad( &pData->ActionList.actionlist.tPurseLoad, &pData->InternalData.TransactionData ) != LDT_SUCCESS )
		{
			CsErrx( "myki_br_ldt_Actionlist() MYKI_LDT_TPurseUpdate() failed" );
			return -1;
		}
		break;
// Missing Tpurse autoload
// Missing Tpurse load reverse
    case ACTION_PRODUCT_SALE_NONE:
		DebugShowLdtProductSaleRequest( BRLL_FIELD, &pData->ActionList.actionlist.productSale );
		if ( MYKI_LDT_ProductSale( &pData->ActionList.actionlist.productSale, &pData->InternalData.TransactionData ) != LDT_SUCCESS )
		{
			CsErrx( "myki_br_ldt_Actionlist() MYKI_LDT_TPurseUpdate() failed" );
			return -1;
		}

		break;
    case ACTION_OAPP_UPDATE_BLOCK:
		// TODO Add DebugShowLdtOAPPUpdateRequest
		if ( MYKI_LDT_OAppUpdateBlock( &pData->ActionList.actionlist.OAppUpdate, &pData->InternalData.TransactionData ) != LDT_SUCCESS )
		{
			CsErrx( "myki_br_ldt_Actionlist() MYKI_LDT_TPurseUpdate() failed" );
			return -1;
		}
		break;
    case ACTION_OAPP_UPDATE_UNBLOCK:
		// TODO Add DebugShowLdtOAPPUpdateRequest
		if ( MYKI_LDT_OAppUpdateUnblock( &pData->ActionList.actionlist.OAppUpdate, &pData->InternalData.TransactionData ) != LDT_SUCCESS )
		{
			CsErrx( "myki_br_ldt_Actionlist() MYKI_LDT_TPurseUpdate() failed" );
			return -1;
		}
		break;
	default:
		CsErrx( "myki_br_ldt_Actionlist Unknown type %d provided",  pData->ActionList.type);
		break;
	}


#else
	CsErrx( "myki_br_ldt_Actionlist LDT_SUPPORTED not set");
    // TODO review if LDT_SUPPORTED #define is still required.
#endif

    return 0;
}

//=============================================================================
//
//  Activate a product using the specified start and end date/time.
//
//=============================================================================

int         myki_br_ldt_ProductUpdate_ActivateEx(
    MYKI_BR_ContextData_t  *pData,
    MYKI_Directory_t       *pDirectory,
    Time_t                  startDateTime,
    Time_t                  endDateTime )
{
    ProductUpdate_t         request;

    ProductUpdateInitialise( pData, pDirectory, &request );

    request.isStartDateTimeSet  = TRUE;
    request.startDateTime       = startDateTime;

    request.isEndDateTimeSet    = TRUE;
    request.endDateTime         = endDateTime;
//    myki_br_RoundUpEndTime( pData, &request.endDateTime );                      // KWS: Should this be here, or in the calling code? It's a rule that may not apply to all products.

    DebugShowLdtProductUpdateRequest( BRLL_FIELD, &request );

#ifdef LDT_SUPPORTED

    if ( MYKI_LDT_ProductUpdateActivate( &request, &pData->InternalData.TransactionData ) != LDT_SUCCESS )
    {
        CsErrx( "myki_br_ldt_ProductUpdate_ActivateEx() MYKI_LDT_ProductUpdateActivate() failed" );
        return -1;
    }

#else

    pDirectory->Status  = TAPP_CONTROL_DIRECTORY_STATUS_ACTIVATED;

#endif

    return 0;
}

//=============================================================================
//
//  Activate a product using the specified start date/time and the default product
//  duration from CD to determine the end date/time.
//
//=============================================================================

int         myki_br_ldt_ProductUpdate_ActivateFrom( MYKI_BR_ContextData_t *pData, MYKI_Directory_t *pDirectory, Time_t startDateTime )
{
    ProductType_e           ProductType     = myki_br_cd_GetProductType( pDirectory->ProductId );
    Time_t                  endDateTime     = TIME_NOT_SET;

    switch ( ProductType )
    {
        case PRODUCT_TYPE_UNKNOWN :
        {
            CsErrx( "myki_br_ldt_ProductUpdate_Activate() Product ID %d - unable to determine product type", pDirectory->ProductId );
            return -1;
        }
        break;

        case PRODUCT_TYPE_EPASS :
        {
            MYKI_TAControl_t   *pMYKI_TAControl = NULL;
            MYKI_Directory_t   *pMYKI_Directory = NULL;
            MYKI_TAProduct_t   *pMYKI_TAProduct = NULL;
            Time_t              productDuration = 0;
            U8_t                index           = 0;

            if ( MYKI_CS_TAControlGet( &pMYKI_TAControl ) < 0 )
            {
                CsErrx( "myki_br_ldt_ProductUpdate_Activate() MYKI_CS_TAControlGet failed" );
                return -1;
            }

            index   = myki_br_GetDirWithSerial( pMYKI_TAControl, pDirectory->SerialNo );
            if ( index == 0 )
            {
                CsErrx( "myki_br_ldt_ProductUpdate_Activate() product SN=%d not found", pDirectory->SerialNo );
                return -1;
            }

            if ( myki_br_GetCardProduct( index, &pMYKI_Directory, &pMYKI_TAProduct ) < 0 )
            {
                CsErrx( "myki_br_ldt_ProductUpdate_Activate() myki_br_GetCardProduct(%d) failed", index );
                return -1;
            }

#ifdef OPT_MIMIC_KAMCO_READER_CHANGE_FIXED_EPASS_DATES
            if ( 0 )
#else
            if ( myki_br_cd_IsFixedePass( pMYKI_Directory->ProductId ) != FALSE )
#endif
            {
                /*  *** UNDOCUMENTED FEATURE!
                    DON'T change product StartDateTime/EndDateTime if fixed ePass */
                CsDbg( BRLL_RULE, "myki_br_ldt_ProductUpdate_Activate() TAppDirectory[%d].ProductId(%d) is a fixed ePass",
                        index, pMYKI_Directory->ProductId );

                startDateTime   = pMYKI_TAProduct->StartDateTime;
                endDateTime     = pMYKI_TAProduct->EndDateTime;
            }
            else
            {
                /*  *** UNDOCUMENTED FEATURE!
                    TAppProduct.InstanceCount contains the duration of ePass (in days) */
                if ( pMYKI_TAProduct->InstanceCount == 0 )
                {
                    CsErrx( "myki_br_ldt_ProductUpdate_Activate() TAProduct[%d].InstanceCount == 0", index );
                    return -1;
                }
                productDuration = (Time_t)DAYS_TO_SECONDS( pMYKI_TAProduct->InstanceCount );

                endDateTime     = myki_br_StartOfBusinessDateTime( pData, ( startDateTime + productDuration ) );
            }
        }
        break;

        case PRODUCT_TYPE_NHOUR :
        {
            if ( ( endDateTime = myki_br_NHourEndDateTime( pData ) ) == TIME_NOT_SET )
            {
                CsErrx( "myki_br_ldt_ProductUpdate_Activate() Unable to determine nHour End Date/Time" );
                return -1;
            }
        }
        break;

        case PRODUCT_TYPE_DAILY :
        {
        }
        break;

        case PRODUCT_TYPE_WEEKLY :
        {
        }
        break;

        default :
        {
            endDateTime     = startDateTime + myki_br_GetProductDuration( pData, ProductType );
        }
        break;
    }

    return myki_br_ldt_ProductUpdate_ActivateEx( pData, pDirectory, startDateTime, endDateTime );
}

//=============================================================================
//
//  Activate a product using the current date/time and the default product
//  duration from CD to determine the end date/time.
//
//=============================================================================

int         myki_br_ldt_ProductUpdate_Activate( MYKI_BR_ContextData_t *pData, MYKI_Directory_t *pDirectory )
{
    return myki_br_ldt_ProductUpdate_ActivateFrom( pData, pDirectory, pData->DynamicData.currentDateTime );
}

//=============================================================================
//
//  Invalidate a product.
//
//=============================================================================

int         myki_br_ldt_ProductUpdate_Invalidate( MYKI_BR_ContextData_t *pData, MYKI_Directory_t *pDirectory )
{
    ProductUpdate_t         request;

    ProductUpdateInitialise( pData, pDirectory, &request );

    DebugShowLdtProductUpdateRequest( BRLL_FIELD, &request );

#ifdef LDT_SUPPORTED

    if ( MYKI_LDT_ProductUpdateInvalidate( &request, &pData->InternalData.TransactionData ) != LDT_SUCCESS )
    {
        CsErrx( "myki_br_ldt_ProductUpdateInvalidate() MYKI_LDT_ProductUpdateInvalidate() failed" );
        return -1;
    }

#else

    pDirectory->Status  = TAPP_CONTROL_DIRECTORY_STATUS_UNUSED;

#endif

    return 0;
}

//=============================================================================
//
//  Note :  Same as all other myki_br_ldt_ProductUsage_*() except for
//          final LDT call.
//
//=============================================================================

int         myki_br_ldt_ProductUsage_ScanOff( MYKI_BR_ContextData_t *pData, MYKI_Directory_t *pDirectory )
{
    ProductUsage_t          request;

    memset( &request, 0, sizeof( request ) );

    request.serialNo                            = pDirectory->SerialNo;
    request.serviceProviderId                   = pData->StaticData.serviceProviderId;
    request.dateTime                            = pData->DynamicData.currentDateTime;
    request.zone                                = (U8_t)pData->DynamicData.currentZone;
    request.entryPointId                        = pData->DynamicData.entryPointId;
    request.routeId                             = pData->DynamicData.lineId;
    request.stopId                              = pData->DynamicData.stopId;

    //set originating information
    if ( pData->DynamicData.isOriginatingInformationSet != FALSE )
    {
        request.originatingServiceProviderId        = pData->DynamicData.originatingInformation.serviceProviderId;
        request.originatingEntryPointId             = pData->DynamicData.originatingInformation.location.EntryPointId;
        request.originatingRouteId                  = pData->DynamicData.originatingInformation.location.RouteId;
        request.originatingStopId                   = pData->DynamicData.originatingInformation.location.StopId;
        request.isOriginatingServiceProviderIdSet   = TRUE;
        request.isOriginatingEntryPointIdSet        = TRUE;
        request.isOriginatingRouteIdSet             = TRUE;
        if ( pData->DynamicData.originatingInformation.location.StopId != 0 )
            request.isOriginatingStopIdSet          = TRUE;
    }

    DebugShowLdtProductUsageRequest( BRLL_FIELD, &request );

    if ( pData->DynamicData.isForcedScanOff )
    {
        CsDbg( BRLL_CHOICE, "myki_br_ldt_ProductUsageScanOff() isForcedScanOff" );

#ifdef LDT_SUPPORTED

        if ( MYKI_LDT_ProductUsageForceScanOff(&request, &pData->InternalData.TransactionData ) != LDT_SUCCESS)
        {
            CsErrx("myki_br_ldt_ProductUsageScanOff() MYKI_LDT_ProductUsageForceScanOff() failed");
            return -1;
        }

#endif

    }
    else
    {
        CsDbg( BRLL_CHOICE, "myki_br_ldt_ProductUsageScanOff() NOT isForcedScanOff" );

#ifdef LDT_SUPPORTED

        if ( MYKI_LDT_ProductUsageScanOff(&request, &pData->InternalData.TransactionData ) != LDT_SUCCESS)
        {
            CsErrx("myki_br_ldt_ProductUsageScanOff() MYKI_LDT_ProductUsageScanOff() failed");
            return -1;
        }

#endif

    }

#ifndef LDT_SUPPORTED

    {
        MYKI_TAControl_t   *pMYKI_TAControl = NULL;
        if ( MYKI_CS_TAControlGet( &pMYKI_TAControl ) < 0 )
        {
            return -1;
        }
        pMYKI_TAControl->ProductInUse       = 0;
    }

#endif

    return 0;
}

//=============================================================================
//
//  Note :  Same as all other myki_br_ldt_ProductUsage_*() except for
//          final LDT call.
//
//
//
//=============================================================================

int         myki_br_ldt_ProductUsage_ScanOnEPass( MYKI_BR_ContextData_t *pData, MYKI_Directory_t *pDirectory )
{
    ProductUsage_t          request;

    memset( &request, 0, sizeof( request ) );

    request.serialNo                            = pDirectory->SerialNo;
    request.serviceProviderId                   = pData->StaticData.serviceProviderId;
    request.dateTime                            = pData->DynamicData.currentDateTime;
    request.zone                                = (U8_t)pData->DynamicData.currentZone;
    request.entryPointId                        = pData->DynamicData.entryPointId;
    request.routeId                             = pData->DynamicData.lineId;
    request.stopId                              = pData->DynamicData.stopId;

    //set originating information
    if ( pData->DynamicData.isOriginatingInformationSet != FALSE )
    {
        request.originatingServiceProviderId        = pData->DynamicData.originatingInformation.serviceProviderId;
        request.originatingEntryPointId             = pData->DynamicData.originatingInformation.location.EntryPointId;
        request.originatingRouteId                  = pData->DynamicData.originatingInformation.location.RouteId;
        request.originatingStopId                   = pData->DynamicData.originatingInformation.location.StopId;
        request.isOriginatingServiceProviderIdSet   = TRUE;
        request.isOriginatingEntryPointIdSet        = TRUE;
        request.isOriginatingRouteIdSet             = TRUE;
        if ( pData->DynamicData.originatingInformation.location.StopId != 0 )
            request.isOriginatingStopIdSet          = TRUE;
    }

    DebugShowLdtProductUsageRequest( BRLL_FIELD, &request );

#ifdef LDT_SUPPORTED

    if ( MYKI_LDT_ProductUsageScanOnEpass( &request, &pData->InternalData.TransactionData ) != LDT_SUCCESS )
    {
        CsErrx("myki_br_ldt_ProductUsage_ScanOnEPass() MYKI_LDT_ProductUsageScanOnEpass() failed");
        return -1;
    }

#else

    {
        MYKI_TAControl_t   *pMYKI_TAControl         = NULL;
        int                 i                       = 0;

        if ( MYKI_CS_TAControlGet( &pMYKI_TAControl ) < 0 || pMYKI_TAControl == NULL )
        {
            return -1;
        }   /* end-of-if */

        for ( i = 1; i < DIMOF( pMYKI_TAControl->Directory ); i++ )
        {
            if ( pMYKI_TAControl->Directory[ i ].SerialNo == request.serialNo )
            {
                pMYKI_TAControl->ProductInUse   = i;
                break;  /* out-of-for */
            }   /* end-of-if */
        }   /* end-of-for */
    }

#endif

    return 0;
}

//=============================================================================
//
//  Note :  Same as all other myki_br_ldt_ProductUsage_*() except for
//          final LDT call.
//
//=============================================================================

int                 myki_br_ldt_ProductUsage_ScanOnStoredValue( MYKI_BR_ContextData_t *pData, MYKI_Directory_t *pDirectory )
{
    ProductUsage_t          request;

    memset( &request, 0, sizeof( request ) );

    request.serialNo                            = pDirectory->SerialNo;
    request.serviceProviderId                   = pData->StaticData.serviceProviderId;
    request.dateTime                            = pData->DynamicData.currentDateTime;
    request.zone                                = (U8_t)pData->DynamicData.currentZone;
    request.entryPointId                        = pData->DynamicData.entryPointId;
    request.routeId                             = pData->DynamicData.lineId;
    request.stopId                              = pData->DynamicData.stopId;

    //set originating information
    if ( pData->DynamicData.isOriginatingInformationSet != FALSE )
    {
        request.originatingServiceProviderId        = pData->DynamicData.originatingInformation.serviceProviderId;
        request.originatingEntryPointId             = pData->DynamicData.originatingInformation.location.EntryPointId;
        request.originatingRouteId                  = pData->DynamicData.originatingInformation.location.RouteId;
        request.originatingStopId                   = pData->DynamicData.originatingInformation.location.StopId;
        request.isOriginatingServiceProviderIdSet   = TRUE;
        request.isOriginatingEntryPointIdSet        = TRUE;
        request.isOriginatingRouteIdSet             = TRUE;
        if ( pData->DynamicData.originatingInformation.location.StopId != 0 )
            request.isOriginatingStopIdSet          = TRUE;
    }

    DebugShowLdtProductUsageRequest( BRLL_FIELD, &request );

#ifdef LDT_SUPPORTED

    if ( MYKI_LDT_ProductUsageScanOnStoredValue( &request, &pData->InternalData.TransactionData ) != LDT_SUCCESS )
    {
        CsErrx("myki_br_ldt_ProductUsage_ScanOnStoredValue() MYKI_LDT_ProductUsageScanOnStoredValue() failed");
        return -1;
    }

#else

    {
        MYKI_TAControl_t   *pMYKI_TAControl         = NULL;
        int                 i                       = 0;

        if ( MYKI_CS_TAControlGet( &pMYKI_TAControl ) < 0 || pMYKI_TAControl == NULL )
        {
            return -1;
        }   /* end-of-if */

        for ( i = 1; i < DIMOF( pMYKI_TAControl->Directory ); i++ )
        {
            if ( pMYKI_TAControl->Directory[ i ].SerialNo == request.serialNo )
            {
                pMYKI_TAControl->ProductInUse   = i;
                break;  /* out-of-for */
            }   /* end-of-if */
        }   /* end-of-for */
    }

#endif

    return 0;
}

//=============================================================================
//
//  Note :  Same as all other myki_br_ldt_ProductUsage_*() except for
//          final LDT call.
//
//=============================================================================

int                 myki_br_ldt_ProductUsage_Reverse( MYKI_BR_ContextData_t *pData, MYKI_Directory_t *pDirectory )
{
#ifdef LDT_SUPPORTED

    ProductUsage_t          request;

    memset( &request, 0, sizeof( request ) );

    request.serialNo                            = pDirectory->SerialNo;
    request.serviceProviderId                   = pData->StaticData.serviceProviderId;
    request.dateTime                            = pData->DynamicData.currentDateTime;
    request.zone                                = (U8_t)pData->DynamicData.currentZone;
    request.entryPointId                        = pData->DynamicData.entryPointId;
    request.routeId                             = pData->DynamicData.lineId;
    request.stopId                              = pData->DynamicData.stopId;

    //set originating information
    if ( pData->DynamicData.isOriginatingInformationSet != FALSE )
    {
        request.originatingServiceProviderId        = pData->DynamicData.originatingInformation.serviceProviderId;
        request.originatingEntryPointId             = pData->DynamicData.originatingInformation.location.EntryPointId;
        request.originatingRouteId                  = pData->DynamicData.originatingInformation.location.RouteId;
        request.originatingStopId                   = pData->DynamicData.originatingInformation.location.StopId;
        request.isOriginatingServiceProviderIdSet   = TRUE;
        request.isOriginatingEntryPointIdSet        = TRUE;
        request.isOriginatingRouteIdSet             = TRUE;
        if ( pData->DynamicData.originatingInformation.location.StopId != 0 )
            request.isOriginatingStopIdSet          = TRUE;
    }

    DebugShowLdtProductUsageRequest( BRLL_FIELD, &request );

    if ( MYKI_LDT_ProductUsageReverse( &request, &pData->InternalData.TransactionData ) != LDT_SUCCESS )
    {
        CsErrx("myki_br_ldt_ProductUsage_Reverse() MYKI_LDT_ProductUsageReverse() failed");
        return -1;
    }

#endif

    return 0;
}

//=============================================================================
//
//
//
//=============================================================================

int                 myki_br_ldt_PurseUsage_Penalty( MYKI_BR_ContextData_t *pData, int amount )      // Positive amount to deduct
{
#ifdef LDT_SUPPORTED

    TPurseUsage_t           request;

    memset( &request, 0, sizeof( request ) );

    request.value                               = amount;
    request.serviceProviderId                   = pData->StaticData.serviceProviderId;
    request.isEntryPointIdSet                   = TRUE;
    request.entryPointId                        = pData->DynamicData.entryPointId;
    request.isRouteIdSet                        = TRUE;
    request.routeId                             = pData->DynamicData.lineId;
    request.isStopIdSet                         = TRUE;
    request.stopId                              = pData->DynamicData.stopId;

    DebugShowLdtTPurseUsageRequest( BRLL_FIELD, &request );

    if ( MYKI_LDT_TPurseUsagePenalty( &request, &pData->InternalData.TransactionData ) != LDT_SUCCESS )
    {
        CsErrx("myki_br_ldt_PurseUsage_Penalty() MYKI_LDT_TPurseUsagePenalty() failed");
        return -1;
    }

#endif

    return 0;
}

//=============================================================================
//
//
//
//=============================================================================

int                 myki_br_ldt_PurseUsage_Transit( MYKI_BR_ContextData_t *pData, int amount )      // Positive amount to deduct
{
    TPurseUsage_t           request;

    memset( &request, 0, sizeof( request ) );

    request.value                               = amount;
    request.serviceProviderId                   = pData->StaticData.serviceProviderId;
    request.isEntryPointIdSet                   = TRUE;
    request.entryPointId                        = pData->DynamicData.entryPointId;
    request.isRouteIdSet                        = TRUE;
    request.routeId                             = pData->DynamicData.lineId;
    request.isStopIdSet                         = TRUE;
    request.stopId                              = pData->DynamicData.stopId;

    DebugShowLdtTPurseUsageRequest( BRLL_FIELD, &request );

#ifdef LDT_SUPPORTED

    if ( MYKI_LDT_TPurseUsageTransit( &request, &pData->InternalData.TransactionData ) != LDT_SUCCESS )
    {
        CsErrx("myki_br_ldt_PurseUsage_Transit() MYKI_LDT_TPurseUsageTransit() failed");
        return -1;
    }

#endif

    return 0;
}


//=============================================================================
//
//
//
//=============================================================================

int                 myki_br_ldt_PurseUsage_Reverse( MYKI_BR_ContextData_t *pData, int amount )      // Positive amount to refund
{
#ifdef LDT_SUPPORTED

    TPurseUsage_t           request;

    memset( &request, 0, sizeof( request ) );

    request.value                               = amount;
    request.serviceProviderId                   = pData->StaticData.serviceProviderId;
    request.isEntryPointIdSet                   = TRUE;
    request.entryPointId                        = pData->DynamicData.entryPointId;
    request.isRouteIdSet                        = TRUE;
    request.routeId                             = pData->DynamicData.lineId;
    request.isStopIdSet                         = TRUE;
    request.stopId                              = pData->DynamicData.stopId;

    DebugShowLdtTPurseUsageRequest( BRLL_FIELD, &request );

    if ( MYKI_LDT_TPurseUsageReverse( &request, &pData->InternalData.TransactionData ) != LDT_SUCCESS )
    {
        CsErrx("myki_br_ldt_PurseUsage_Reverse() MYKI_LDT_TPurseUsageReverse() failed");
        return -1;
    }

#endif

    return 0;
}

//=============================================================================
//
//
//
//=============================================================================

int                 myki_br_ldt_PurseLoad_Autoload( MYKI_BR_ContextData_t *pData, int amount )
{
#ifdef LDT_SUPPORTED

    TPurseLoad_t           request;

    memset( &request, 0, sizeof( request ) );

    request.value                               = amount;
    request.serviceProviderId                   = pData->StaticData.serviceProviderId;
    request.isEntryPointIdSet                   = TRUE;
    request.entryPointId                        = pData->DynamicData.entryPointId;
    request.isRouteIdSet                        = TRUE;
    request.routeId                             = pData->DynamicData.lineId;
    request.isStopIdSet                         = TRUE;
    request.stopId                              = pData->DynamicData.stopId;


    if ( MYKI_LDT_TPurseLoadAutoload( &request,  &pData->InternalData.TransactionData ) != LDT_SUCCESS )
    {
        CsErrx("myki_br_ldt_PurseLoad_Autoload() MYKI_LDT_TPurseLoadAutoload() failed");
        return -1;
    }
#else

    // TODO Add this line for non-LDT builds.
    // pMYKI_TAPurseBalance->Balance += pMYKI_TAPurseControl->AutoValue;
#endif

    return 0;
}

int                 myki_br_ldt_PurseLoad_None( MYKI_BR_ContextData_t *pData, int amount )
{
#ifdef LDT_SUPPORTED

    TPurseLoad_t           request;

    memset( &request, 0, sizeof( request ) );

    request.value                               = amount;
    request.serviceProviderId                   = pData->StaticData.serviceProviderId;
    request.isEntryPointIdSet                   = TRUE;
    request.entryPointId                        = pData->DynamicData.entryPointId;
    request.isRouteIdSet                        = TRUE;
    request.routeId                             = pData->DynamicData.lineId;
    request.isStopIdSet                         = TRUE;
    request.stopId                              = pData->DynamicData.stopId;

    if ( MYKI_LDT_TPurseLoad( &request,  &pData->InternalData.TransactionData ) != LDT_SUCCESS )
    {
        CsErrx("MYKI_LDT_TPurseLoad() MYKI_LDT_TPurseLoad() failed");
        return -1;
    }
#else

    // TODO Add this line for non-LDT builds.
    // pMYKI_TAPurseBalance->Balance += pMYKI_TAPurseControl->AutoValue;
#endif

    return 0;
}

/*==========================================================================*
**
**  myki_br_ldt_PurseLoad_Reverse
**
**  Description     :
**      Performs TPurseLoad/Reverse.
**
**  Parameters      :
**      pData           [I/O]   BR context data
**      amount          [I]     amount to be reversed
**
**  Returns         :
**      0                       success
**      <0                      failed
**
**==========================================================================*/

int                 myki_br_ldt_PurseLoad_Reverse( MYKI_BR_ContextData_t *pData, int amount )
{
    #ifdef LDT_SUPPORTED
    TPurseLoad_t    request;
    int             result                      = 0;

    memset( &request, 0, sizeof( request ) );

    request.value                               = amount;
    request.serviceProviderId                   = pData->StaticData.serviceProviderId;
    request.isEntryPointIdSet                   = TRUE;
    request.entryPointId                        = pData->DynamicData.entryPointId;
    request.isRouteIdSet                        = TRUE;
    request.routeId                             = pData->DynamicData.lineId;
    request.isStopIdSet                         = TRUE;
    request.stopId                              = pData->DynamicData.stopId;

    DebugShowLdtTPurseLoadRequest( BRLL_FIELD, &request );

    if ( ( result = MYKI_LDT_TPurseLoadReverse( &request, &pData->InternalData.TransactionData ) ) != LDT_SUCCESS )
    {
        CsErrx( "myki_br_ldt_PurseLoad_Reverse : MYKI_LDT_TPurseLoadReverse() failed (%d)", result );
        return -1;
    }
    #endif

    return 0;
}   /*  myki_br_ldt_PurseLoad_Reverse( ) */

/*==========================================================================*
**
**  myki_br_ldt_PerformCappingReset
**
**  Description     :
**      .
**
**  Parameters      :
**      pData           [I/O]   BR context data
**      isDailyReset    [I]     TRUE if resetting daily capping
**      isWeeklyReset   [I]     TRUE if resetting weekly capping
**
**  Returns         :
**      0                       success
**      <0                      failed
**
**==========================================================================*/

int         myki_br_ldt_PerformCappingReset( MYKI_BR_ContextData_t *pData, int isDailyReset, int isWeeklyReset )
{
    CappingUpdate_t         request;

    CsDbg( BRLL_RULE, "myki_br_ldt_PerformCappingReset : isDailyReset = %d, isWeeklyReset = %d", isDailyReset, isWeeklyReset );

    memset( &request, 0, sizeof( request ) );

    request.serviceProviderId       = pData->StaticData.serviceProviderId;
    // Set isEntryPointIdSet to FALSE as ACS LDT does not set it.
    //request.isEntryPointIdSet       = TRUE;
    request.isEntryPointIdSet       = FALSE;
    request.entryPointId            = pData->DynamicData.entryPointId;
    request.endOfBusinessDay        = pData->Tariff.endOfBusinessDay;

    if ( isDailyReset == FALSE && isWeeklyReset == FALSE )
    {
        CsErrx( "myki_br_ldt_PerformCappingReset() no reset specified" );
        return -1;
    }

    if ( isDailyReset == TRUE )
    {
        request.isDailyExpirySet    = TRUE;
        request.isDailyZoneLowSet   = TRUE;
        request.isDailyZoneHighSet  = TRUE;
        request.isDailyValueSet     = TRUE;
        request.dailyExpiry         = pData->DynamicData.currentBusinessDate + 1;
        request.dailyZoneLow        = 0;
        request.dailyZoneHigh       = 0;
        request.dailyValue          = 0;
    }
    else
    {
        request.isDailyExpirySet    = FALSE;
        request.isDailyZoneLowSet   = FALSE;
        request.isDailyZoneHighSet  = FALSE;
        request.isDailyValueSet     = FALSE;
        request.dailyExpiry         = 0;
        request.dailyZoneLow        = 0;
        request.dailyZoneHigh       = 0;
        request.dailyValue          = 0;
    }

    if ( isWeeklyReset == TRUE )
    {
        request.isWeeklyExpirySet   = TRUE;
        request.isWeeklyZoneLowSet  = TRUE;
        request.isWeeklyZoneHighSet = TRUE;
        request.isWeeklyValueSet    = TRUE;
        request.weeklyExpiry        = myki_br_NextMonday( pData );
        request.weeklyZoneLow       = 0;
        request.weeklyZoneHigh      = 0;
        request.weeklyValue         = 0;
    }
    else
    {
        request.isWeeklyExpirySet   = FALSE;
        request.isWeeklyZoneLowSet  = FALSE;
        request.isWeeklyZoneHighSet = FALSE;
        request.isWeeklyValueSet    = FALSE;
        request.weeklyExpiry        = 0;
        request.weeklyZoneLow       = 0;
        request.weeklyZoneHigh      = 0;
        request.weeklyValue         = 0;
    }

    DebugShowLdtCappingUpdateRequest( BRLL_FIELD, &request );

#ifdef LDT_SUPPORTED

    if ( MYKI_LDT_CappingUpdate( &request, &pData->InternalData.TransactionData ) != LDT_SUCCESS )
    {
        CsErrx( "myki_br_ldt_PerformCappingReset() MYKI_LDT_CappingUpdate() failed" );
        return -1;
    }

#endif

    return 0;
}   /* myki_br_ldt_PerformCappingReset( ) */

/*==========================================================================*
**
**  myki_br_ldt_PerformCappingUpdate
**
**  Description     :
**      .
**
**  Parameters      :
**      pData           [I/O]   BR context data
**      pCappingData    [I]     new Capping data
**      isDailyReset    [I]     TRUE if resetting daily capping
**      isWeeklyReset   [I]     TRUE if resetting weekly capping
**
**  Returns         :
**      0                       success
**      <0                      failed
**
**==========================================================================*/

int         myki_br_ldt_PerformCappingUpdate( MYKI_BR_ContextData_t *pData, MYKI_TACapping_t *pCappingData, int isDailyUpdate, int isWeeklyUpdate )
{
    CappingUpdate_t         request;
    MYKI_TACapping_t       *pMYKI_TACapping     = NULL;

    CsDbg( BRLL_RULE, "myki_br_ldt_PerformCappingUpdate : isDailyUpdate = %d, isWeeklyUpdate = %d", isDailyUpdate, isWeeklyUpdate );

    memset( &request, 0, sizeof( request ) );

    request.serviceProviderId   = pData->StaticData.serviceProviderId;
    // Set isEntryPointIdSet to FALSE as ACS LDT does not set it.
    //request.isEntryPointIdSet       = TRUE;
    request.isEntryPointIdSet   = FALSE;
    request.entryPointId        = pData->DynamicData.entryPointId;
    request.endOfBusinessDay    = pData->Tariff.endOfBusinessDay;

    if(pCappingData==NULL)
    {
        CsErrx("myki_br_ldt_PerformCappingUpdate : pCappingData==NULL");
        return -1;
    }

    if ( MYKI_CS_TACappingGet( &pMYKI_TACapping ) < 0 )
    {
        CsErrx( "myki_br_ldt_PerformCappingUpdate : MYKI_CS_TACappingGet() failed" );
        return -1;
    }

    CsDbg( BRLL_RULE, "myki_br_ldt_PerformCappingUpdate : Daily Expiry (%d), Weekly Expiry (%d), Current (%d)",
        pMYKI_TACapping->Daily.Expiry, pMYKI_TACapping->Weekly.Expiry, pData->DynamicData.currentBusinessDate );

    if(isDailyUpdate==TRUE)
    {
        request.isDailyZoneLowSet   = TRUE;
        request.isDailyZoneHighSet  = TRUE;
        request.isDailyExpirySet    = TRUE;
        request.dailyExpiry         = pCappingData->Daily.Expiry;
        request.dailyZoneLow        = pCappingData->Daily.Zone.Low;
        request.dailyZoneHigh       = pCappingData->Daily.Zone.High;
        request.dailyValue          = pCappingData->Daily.Value;
        request.isDailyValueSet     = ( request.dailyValue != pMYKI_TACapping->Daily.Value ? TRUE : FALSE );
    }
    else
    {
        request.isDailyExpirySet    = FALSE;
        request.isDailyZoneLowSet   = FALSE;
        request.isDailyZoneHighSet  = FALSE;
        request.isDailyValueSet     = FALSE;
        request.dailyExpiry         = 0;
        request.dailyZoneLow        = 0;
        request.dailyZoneHigh       = 0;
        request.dailyValue          = 0;
    }

    if(isWeeklyUpdate==TRUE)
    {
        request.isWeeklyZoneLowSet  = TRUE;
        request.isWeeklyZoneHighSet = TRUE;
        request.isWeeklyExpirySet   = TRUE;
        request.weeklyExpiry        = pCappingData->Weekly.Expiry;
        request.weeklyZoneLow       = pCappingData->Weekly.Zone[ 0 ].Low;
        request.weeklyZoneHigh      = pCappingData->Weekly.Zone[ 0 ].High;
        request.weeklyValue         = pCappingData->Weekly.Value;
        request.isWeeklyValueSet    = ( request.weeklyValue != pMYKI_TACapping->Weekly.Value ? TRUE : FALSE );
    }
    else
    {
        request.isWeeklyExpirySet   = FALSE;
        request.isWeeklyZoneLowSet  = FALSE;
        request.isWeeklyZoneHighSet = FALSE;
        request.isWeeklyValueSet    = FALSE;
        request.weeklyExpiry        = 0;
        request.weeklyZoneLow       = 0;
        request.weeklyZoneHigh      = 0;
        request.weeklyValue         = 0;
    }

    DebugShowLdtCappingUpdateRequest( BRLL_FIELD, &request );

#ifdef LDT_SUPPORTED

    if ( MYKI_LDT_CappingUpdate( &request, &pData->InternalData.TransactionData ) != LDT_SUCCESS )
    {
        CsErrx("myki_br_ldt_PerformCappingUpdate() MYKI_LDT_CappingUpdate() failed");
        return -1;
    }

#endif

    return 0;
}

/*==========================================================================*
**
**  myki_br_ldt_PerformCappingUpdateDailyZones
**
**  Description     :
**      Set the daily capping low zone and daily capping high zone to a
**      single specified value
**
**  Parameters      :
**      pData           [I/O]   BR context data
**                              zoneValue
**
**  Returns         :
**      0                       success
**      <0                      failed
**
**==========================================================================*/

int         myki_br_ldt_PerformCappingUpdateDailyZones( MYKI_BR_ContextData_t *pData, U8_t zoneValue)
{
    CappingUpdate_t         request;

    CsDbg( BRLL_RULE, "myki_br_ldt_PerformCappingUpdateDailyZones : zoneValue = %d", zoneValue );

    memset( &request, 0, sizeof( request ) );

    request.serviceProviderId   = pData->StaticData.serviceProviderId;
    // Set isEntryPointIdSet to FALSE as ACS LDT does not set it.
    //request.isEntryPointIdSet       = TRUE;
    request.isEntryPointIdSet       = FALSE;
    request.entryPointId        = pData->DynamicData.entryPointId;
    request.endOfBusinessDay    = pData->Tariff.endOfBusinessDay;

    request.isDailyZoneLowSet   = TRUE;
    request.dailyZoneLow        = zoneValue;

    request.isDailyZoneHighSet  = TRUE;
    request.dailyZoneHigh       = zoneValue;

    DebugShowLdtCappingUpdateRequest( BRLL_FIELD, &request );

#ifdef LDT_SUPPORTED

    if ( MYKI_LDT_CappingUpdate( &request, &pData->InternalData.TransactionData ) != LDT_SUCCESS )
    {
        CsErrx("myki_br_ldt_PerformCappingUpdateDailyZones() MYKI_LDT_CappingUpdate() failed");
        return -1;
    }

#endif

    return 0;
}

/*==========================================================================*
**
**  myki_br_ldt_AppUpdate_Block
**
**  Description     :
**      Performs TAppUpdate/Block.
**
**  Parameters      :
**      pData           [I/O]   BR context data
**      blockingReason  [I]     blocking reason
**
**  Returns         :
**      0                       success
**      <0                      failed
**
**==========================================================================*/

int         myki_br_ldt_AppUpdate_Block( MYKI_BR_ContextData_t *pData, U8_t blockingReason )
{
    TAppUpdate_t            request;

    memset( &request, 0, sizeof( request ) );

    request.serviceProviderId           = pData->StaticData.serviceProviderId;
    request.isEntryPointIdSet           = TRUE;
    request.entryPointId                = pData->DynamicData.entryPointId;
    request.blockingReason              = blockingReason;

    DebugShowLdtAppUpdateRequest( BRLL_FIELD, &request );

    #ifdef  LDT_SUPPORTED
    if ( MYKI_LDT_TAppUpdateBlock( &request, &pData->InternalData.TransactionData ) != LDT_SUCCESS )
    {
        CsErrx( "myki_br_ldt_AppUpdate_Block: MYKI_LDT_TAppUpdateBlock() failed" );
        return  -1;
    }
    #else
    {
        MYKI_TAControl_t   *pMYKI_TAControl     = NULL;

        if ( MYKI_CS_TAControlGet( &pMYKI_TAControl ) < 0 )
        {
            return  -1;
        }
        pMYKI_TAControl->BlockingReason = request.blockingReason;
    }
    #endif  /*  LDT_SUPPORTED */

    return  0;
}   /*  myki_br_ldt_AppUpdate_Block( ) */

//=============================================================================
//
//
//
//=============================================================================

int         myki_br_ldt_AppUpdate_SetProductInUse( MYKI_BR_ContextData_t *pData, MYKI_Directory_t *pDirectory )
{
    TAppUpdate_t            request;

    memset( &request, 0, sizeof( request ) );

    request.serviceProviderId           = pData->StaticData.serviceProviderId;
    request.isEntryPointIdSet           = TRUE;
    request.entryPointId                = pData->DynamicData.entryPointId;
    request.isActionSequenceNoSet       = FALSE;
    request.actionSequenceNo            = 0;
    request.isExpiryDateSet             = FALSE;
    request.expiryDate                  = 0;
    request.isPassengerCodeSet          = FALSE;
    request.passengerCode               = 0;
    request.isPassengerCodeExpirySet    = FALSE;
    request.passengerCodeExpiry         = 0;
    request.isPassengerControlSet       = FALSE;
    request.passengerControl            = 0;
    request.isLanguageSet               = FALSE;
    request.language                    = 0;
    request.blockingReason              = 0;

    if ( pDirectory )
    {
        CsDbg( BRLL_RULE, "myki_br_ldt_AppUpdate_SetProductInUse : Serial Number = %d", pDirectory->SerialNo );

        if ( pDirectory->SerialNo != 0 )
        {
            request.serialNo        = pDirectory->SerialNo;
            request.isSerialNoSet   = TRUE;
        }
    }
    else
    {
        CsDbg( BRLL_RULE, "myki_br_ldt_AppUpdate_SetProductInUse : No product in use" );
        request.isSerialNoSet   = FALSE;
    }

    DebugShowLdtAppUpdateRequest( BRLL_FIELD, &request );

#ifdef LDT_SUPPORTED

    if ( MYKI_LDT_TAppUpdateSetProductInUse( &request, &pData->InternalData.TransactionData ) != LDT_SUCCESS )
    {
        CsErrx("myki_br_ldt_AppUpdate_SetProductInUse() MYKI_LDT_TAppUpdateSetProductInUse() failed");
        return -1;
    }

#else

    {
        MYKI_TAControl_t           *pMYKI_TAControl     = NULL;
        int                         i                   = 0;

        if ( MYKI_CS_TAControlGet( &pMYKI_TAControl ) < 0 )
        {
            return -1;
        }   /* end-of-if */

        if ( pDirectory == NULL )
        {
            pMYKI_TAControl->ProductInUse   = 0;
        }
        else
        {
            for ( i = 1; i < DIMOF( pMYKI_TAControl->Directory ); i++ )
            {
                if ( &pMYKI_TAControl->Directory[ i ] == pDirectory )
                {
                    pMYKI_TAControl->ProductInUse   = i;
                    break;  /* out-of-for */
                }   /* end-of-if */
            }   /* end-of-for */
        }   /* end-of-if */
    }

#endif

    return 0;
}




//=============================================================================
//
//
//
//=============================================================================

int         myki_br_ldt_OAppUpdate_Block( MYKI_BR_ContextData_t *pData )
{
	// Not supported
    return -1;
}

int
myki_br_ldt_OAppUpdate_AOCardProcessing( MYKI_BR_ContextData_t *pData )
{
    OAppInspectorDataUpdate_t request;

    memset(&request, 0, sizeof(request));


    request.boardingZone = pData->DynamicData.currentZone;
    request.isBoardingZoneSet = TRUE;
    request.boardingEntryPointId = pData->DynamicData.entryPointId;
    request.isBoardingEntryPointIdSet = TRUE;
    request.boardingStopId = pData->DynamicData.stopId;
    request.isBoardingStopIdSet = TRUE;
    request.boardingRouteId = pData->DynamicData.lineId;
    request.isBoardingRouteIdSet = TRUE;
    request.boardingTime = pData->DynamicData.currentDateTime;
    request.isBoardingTimeSet = TRUE;
    request.tripDirection = pData->DynamicData.currentTripDirection;
    request.isTripDirectionSet = TRUE; 
    request.samId = pData->StaticData.deviceSamId;
    request.isSAMIdSet = TRUE;
    request.providerId = pData->StaticData.serviceProviderId;
    request.isProviderIdSet = TRUE;

  /* TODO the folling fields are optional and should be populated in the future.
       Currently they are not stored. 
    request.driverId
    request.shiftId
    request.shiftSequenceNo
    request.paperTicketFirst
    request.paperTicketCancelled
    request.cardCount
  */
   
#ifdef LDT_SUPPORTED

    if (MYKI_LDT_OAppInspectorDataUpdate(&request, &pData->InternalData.TransactionData) != LDT_SUCCESS)
    {
         CsErrx( "myki_br_ldt_OAppUpdate_AOCardProcessing() MYKI_LDT_OAppInspectorDataUpdate() failed" );
         return -1;
    }

#else

    {
        MYKI_OAInspectorData_t *pMYKI_OAInspectorData   = NULL;

        if ( MYKI_CS_OAInspectorDataGet( &pMYKI_OAInspectorData ) != MYKI_CS_OK )
        {
            return  -1;
        }
/*      pMYKI_OAInspectorData->DriverId
        pMYKI_OAInspectorData->ShiftId
        pMYKI_OAInspectorData->ShiftSeqNo  */
        pMYKI_OAInspectorData->SamId                            = request.samId;
        pMYKI_OAInspectorData->ProviderId                       = request.providerId;
        pMYKI_OAInspectorData->BoardingLocation.EntryPointId    = request.boardingEntryPointId;
        pMYKI_OAInspectorData->BoardingLocation.RouteId         = request.boardingRouteId;
        pMYKI_OAInspectorData->BoardingLocation.StopId          = request.boardingStopId;
        pMYKI_OAInspectorData->BoardingZone                     = request.boardingZone;
/*      pMYKI_OAInspectorData->TripSeqNo */
        pMYKI_OAInspectorData->TripDirection                    = request.tripDirection;
        pMYKI_OAInspectorData->BoardingTime                     = request.boardingTime;
/*      pMYKI_OAInspectorData->CardCount
        pMYKI_OAInspectorData->PTFirst
        pMYKI_OAInspectorData->PTCount
        pMYKI_OAInspectorData->PTCancelled */
    }

#endif

    return 0;
}



//=============================================================================
//
//
//
//=============================================================================

int         myki_br_ldt_OAppUpdate_Unblock( MYKI_BR_ContextData_t *pData )
{
    return -1;
}

/*==========================================================================*
**
**  myki_br_NHourEndDateTime
**
**  Description     :
**      Determines N-HOUR's EndDateTime.
**
**  Parameters      :
**      pData               [I/O]   BR context data
**      startDateTime       [I]     n-Hour start date/time
**      duration            [I]     n-Hour duration
**      extension           [I]     n-Hour period extension (if any)
**
**  Returns         :
**      xxx                         N-HOUR's EndDateTime
**      TIME_NOT_SET                failed
**
**  Notes           :
**      Algorithm to derive N-HOUR's EndDateTime,
**
**      <Response sender="Ashish.Rane@nttdata.com" date="03/02/2014">
**      Case 1: Scan On Time is less than n-hour Evening Cutoff.
**      The expiry is: <Current Time> + <nHour Period> + <min, secs> to
**      the next full hour.
**      For example: Scan-On time : 10:04 AM, then the expiry should be :
**      10:04 AM + 2 + 56 = 2:00 PM.
**
**      Case 2: Scan On Time is greater than n-hour Evening Cutoff.
**      The expiry is: End of Transport Day.
**      For Example: Scan-On time: 19:05 PM, the expiry should be: next
**      Calendar day 3:00 AM.
**
**      Case 3: Scan On Time is [before] After n-hour evening cutoff and
**      before EndOfTransport but such that Scan-On Time + nhour Period +
**      next full hour is greater than EndOf Transport.
**      In this case the formula is same as case 1 above.
**      For Example: Scan On Time: 1:30 AM, then expiry is: 1:25 + 2 + 35 =
**      4:00 AM.
**      </Response>
**
**      <Response sender="Ashish.Rane@nttdata.com" date="11 June 2014 13:26"> 
**      The Trip Extension is always added after calculating the normal
**      product expiry with the configured rounding. 
**
**      So, what ACS ETH does is correct. 
**      Product End Date Time = Current Time /Start Date Time +
**                              N-hour Period plus +
**                              Configured Rounding +
**                              Trip Extension (if applicable) 
**      </Response> 
**
**==========================================================================*/

Time_t  myki_br_NHourEndDateTimeEx(
    MYKI_BR_ContextData_t  *pData,
    Time_t                  startDateTime,
    int                     duration,
    int                     periodExtension )
{
    Time_t                  EndDateTime             = TIME_NOT_SET;
    Time_t                  StartOfNextBusinessDay  = myki_br_EndOfBusinessDateTime( pData, startDateTime, FALSE );
    U16_t                   minsSinceMidnight       = 0;
    U16_t                   nHourEveningCutOff      = ( 18 /*06:00 PM*/ * 60 ); // Default value if tariff data is not available
    U16_t                   HHMM                    = 1800;
    struct tm               ctm;

    EndDateTime             = startDateTime + duration;
    myki_br_RoundUpEndTime( pData, &EndDateTime );
    if ( periodExtension > 0 )
    {
        EndDateTime        += periodExtension;
    }   /* end-of-if */

    if ( localtime_r( (time_t*)&startDateTime, &ctm ) == NULL )
    {
        CsErrx( "myki_br_NHourEndDateTime() localtime(%d) failed", startDateTime );
        return TIME_NOT_SET;
    }   /* end-of-if */
    minsSinceMidnight       = (U16_t)( ( ctm.tm_hour * 60 ) + ctm.tm_min );

    if ( MYKI_CD_getnHourEveningCutoff( &HHMM ) != FALSE )
    {
        /* Converts HHMM to minutes */
        nHourEveningCutOff  = ( ( HHMM / 100 ) * 60 ) + ( HHMM % 60 );      // Overrides default value with tariff value
    }   /* end-of-if */

    CsDbg( BRLL_RULE, "myki_br_NHourEndDateTime() StartDateTime             = %d", startDateTime );
    CsDbg( BRLL_RULE, "myki_br_NHourEndDateTime() Duration                  = %d", duration );
    CsDbg( BRLL_RULE, "myki_br_NHourEndDateTime() Extension                 = %d", periodExtension );
    CsDbg( BRLL_RULE, "myki_br_NHourEndDateTime() StartOfNextBusinessDay    = %d", StartOfNextBusinessDay );
    CsDbg( BRLL_RULE, "myki_br_NHourEndDateTime() EndDateTime               = %d", EndDateTime );
    CsDbg( BRLL_RULE, "myki_br_NHourEndDateTime() MinsSinceMidnight         = %d", minsSinceMidnight );
    CsDbg( BRLL_RULE, "myki_br_NHourEndDateTime() nHourEveningCutOff        = %d", nHourEveningCutOff );
    CsDbg( BRLL_RULE, "myki_br_NHourEndDateTime() EndOfBusinessDay          = %d", pData->Tariff.endOfBusinessDay );

    /*
     *  NOTE:   If start time is within the XXXXXXX period, the EndDateTime
     *          is set to start of next business day (if EndDateTime < StartOfNextBusinessDay)
     *
     *          +-------------------------------------- 03:00 Start of current business day
     *          |                   +------------------ 18:00 nHourEveningCutOff
     *          |                   |       +---------- 00:00
     *          |                   |       |       +-- 03:00 Start of next business day
     *          V                   V       V       V
     *          +-------------------+-------+-------+
     *          |                   |XXXXXXX|XXXXXXX|
     *          +-------------------+-------+-------+
     */
    if
    (
        (
            minsSinceMidnight >= nHourEveningCutOff ||
            minsSinceMidnight <  pData->Tariff.endOfBusinessDay
        ) &&
        EndDateTime < StartOfNextBusinessDay
    )
    {
        EndDateTime         = StartOfNextBusinessDay;
    }   /* end-of-if */

    CsDbg( BRLL_RULE, "myki_br_NHourEndDateTime() Resulted EndDateTime      = %d", EndDateTime );

    return EndDateTime;
}   /* myki_br_NHourEndDateTime( ) */

static Time_t myki_br_NHourEndDateTime( MYKI_BR_ContextData_t  *pData )
{
    return myki_br_NHourEndDateTimeEx( pData, pData->DynamicData.currentDateTime, myki_br_GetProductDuration( pData, PRODUCT_TYPE_NHOUR ), 0 );
}

/*==========================================================================*
**
**  myki_br_ldt_ProductSaleEx
**
**  Description     :
**      Performs ProductSale/None given the LDT product sale request.
**
**  Parameters      :
**      pData               [I/O]   BR context data
**      pRequest            [I]     LDT ProductSale request
**
**  Returns         :
**      0                           success
**      <0                          failed
**
**  Notes           :
**
**==========================================================================*/

int         myki_br_ldt_ProductSaleEx(
    MYKI_BR_ContextData_t  *pData,
    ProductSale_t          *pRequest )
{
    MYKI_TAControl_t       *pMYKI_TAControl     = NULL;
    MYKI_CD_Product_t       MYKI_CD_Product;
    int                     i                   = 0;

    if ( pData == NULL || pRequest == NULL )
    {
        return -1;
    }   /* end-of-if */


    if ( pRequest->issuerId == 0 /*NOT SET*/ )
    {
        if ( MYKI_CD_getProductStructure( pRequest->productId, &MYKI_CD_Product ) < 0 )
        {
            CsErrx( "myki_br_ldt_ProductSaleEx() MYKI_CD_getProductStructure(%d) failed",
                pRequest->productId );
            return -1;
        }   /* end-of-if */
        pRequest->issuerId              = MYKI_CD_Product.issuer_id;
    }   /* end-of-if */
    if ( pRequest->isStartDateTimeSet == FALSE )
    {
        pRequest->isStartDateTimeSet    = TRUE;
        pRequest->startDateTime         = pData->DynamicData.currentDateTime;
    }   /* end-of-if */
    if ( pRequest->isEndDateTimeSet == FALSE )
    {
        ProductType_e       ProductType = myki_br_cd_GetProductType( pRequest->productId );

        switch ( ProductType )
        {
        case PRODUCT_TYPE_UNKNOWN:
            CsErrx( "myki_br_ldt_ProductSaleEx() unknown product(%d) type", pRequest->productId );
            return -1;

        case PRODUCT_TYPE_NHOUR:
            pRequest->endDateTime       = myki_br_NHourEndDateTime( pData );
            if ( pRequest->endDateTime == TIME_NOT_SET )
            {
                return -1;
            }   /* end-of-if */
            break;

        default:
            pRequest->endDateTime       = pData->DynamicData.currentDateTime + myki_br_GetProductDuration( pData, ProductType );
            myki_br_RoundUpEndTime( pData, &pRequest->endDateTime );    // KWS: Should this be here, or in the calling code? It's a rule that may not apply to all products.
            break;
        }   /* end-of-switch */
        pRequest->isEndDateTimeSet      = TRUE;
    }   /* end-of-if */

    if ( pRequest->serviceProviderId == 0 /*NOT SET*/ )
    {
        pRequest->serviceProviderId     = pData->StaticData.serviceProviderId;
    }   /* end-of-if */

    pRequest->isEntryPointIdSet         = TRUE;
    pRequest->entryPointId              = pData->DynamicData.entryPointId;
    pRequest->isRouteIdSet              = TRUE;
    pRequest->routeId                   = pData->DynamicData.lineId;
    pRequest->isStopIdSet               = TRUE;
    pRequest->stopId                    = pData->DynamicData.stopId;
    pRequest->isOffpeakSet              = FALSE;
    pRequest->isOffpeak                 = FALSE;
    pRequest->serialNo                  = 0;        // Filled in on return from MYKI_LDT_ProductSale()

    CsDbg( BRLL_RULE,  "myki_br_ldt_ProductSaleEx() instanceCount = %d", pRequest->instanceCount);

    DebugShowLdtProductSaleRequest( BRLL_FIELD, pRequest );

#ifdef LDT_SUPPORTED

    if ( MYKI_LDT_ProductSale( pRequest, &pData->InternalData.TransactionData ) != LDT_SUCCESS )
    {
        CsErrx( "myki_br_ldt_ProductSaleEx() MYKI_LDT_ProductSale() failed" );
        return -1;
    }

#else

    if ( MYKI_CS_TAControlGet( &pMYKI_TAControl ) < 0 )
    {
        CsErrx( "myki_br_ldt_ProductSaleEx() MYKI_CS_TAControlGet() failed" );
        return -1;
    }

    i   = 1;
    while ( i < DIMOF( pMYKI_TAControl->Directory ) )
    {
        if ( pMYKI_TAControl->Directory[ i ].Status == TAPP_CONTROL_DIRECTORY_STATUS_UNUSED )
        {
            pMYKI_TAControl->Directory[ i ].Status      = TAPP_CONTROL_DIRECTORY_STATUS_ISSUED;
            pRequest->serialNo                          = \
            pMYKI_TAControl->Directory[ i ].SerialNo    = pMYKI_TAControl->NextProductSerialNo++;
            pMYKI_TAControl->Directory[ i ].ProductId   = pRequest->productId;
            pMYKI_TAControl->Directory[ i ].IssuerId    = pRequest->issuerId;
            break;  /* out-of-for */
        }   /* end-of-if */
        i++;
    }   /* end-of-for */
    if ( i >= DIMOF( pMYKI_TAControl->Directory ) )
    {
        CsErrx( "myki_br_ldt_ProductSaleEx() no unused product" );
        return -1;
    }   /* end-of-if */

#endif

    if (pRequest->serialNo == 0 )
        {
        CsErrx( "myki_br_ldt_ProductSaleEx() MYKI_LDT_ProductSale did not return serialNo" );
        return -1;
        }

    if ( MYKI_CS_TAControlGet( &pMYKI_TAControl ) < 0 )
    {
        CsErrx( "myki_br_ldt_ProductSaleEx() MYKI_CS_TAControlGet() failed" );
        return -1;
    }

    for ( i = 0 ; i < DIMOF( pMYKI_TAControl->Directory ); i++ )
    {
        CsDbg( BRLL_FIELD, "Dir entry %d serial number = %d\n", i, pMYKI_TAControl->Directory[ i ].SerialNo );

        if ( pMYKI_TAControl->Directory[ i ].SerialNo == pRequest->serialNo )
        {
            CsDbg( BRLL_FIELD, "New product serial number %d was added at index %d", pRequest->serialNo, i );
    return i;
}
    }

    CsErrx( "myki_br_ldt_ProductSaleEx() Failed to locate newly added product with serial number %d", pRequest->serialNo );
    return -1;
}   /* myki_br_ldt_ProductSaleEx( ) */

//=============================================================================
//
//
//
//=============================================================================

int         myki_br_ldt_ProductSale(
    MYKI_BR_ContextData_t   *pData,
    int                     productId,
    int                     zoneLow,
    int                     zoneHigh,
    int                     purchaseValue,
    int                     isProvisional )
{
    ProductSale_t           request;

    memset( &request, 0, sizeof( request ) );

    request.productId               = productId;
    request.purchaseValue           = purchaseValue;
    request.zoneLow                 = zoneLow;
    request.zoneHigh                = zoneHigh;
    request.isActionSequenceNoSet   = FALSE;
    request.actionSequenceNo        = 0;
    request.isProvisionalSet        = isProvisional;    // We want to change provisional bit
    request.isProvisional           = isProvisional;    // We want to set it (i.e. not clear it)
    request.isAutoloadSet           = FALSE;
    request.isAutoload              = FALSE;
    request.isInstanceCountSet      = FALSE;
    request.instanceCount           = 0;

    return myki_br_ldt_ProductSaleEx( pData, &request );
}

/*----------------------------------------------------------------------------
** FUNCTION           : myki_br_ldt_ProductSale_Upgrade
**
** DESCRIPTION        : Perform a ProductSale/Update
**
**
** INPUTS             : pData           - Contect Data
**                      pDirectory      - Directory entry data
**                      productId       - New product ID
**                      zoneLow         - New low Zone
**                      zoneHigh        - New high zone
**                      purchaseValue   - New Puchase Value
**                      expiryDateTime  - New expiry date time
**                                        Do not roundup.
**                                        Used to expire some products.
**
** RETURNS            : -1      - Error
**                      0       - No Error
**
----------------------------------------------------------------------------*/

int         myki_br_ldt_ProductSale_UpgradeEx
(
    MYKI_BR_ContextData_t  *pData,
    MYKI_Directory_t       *pDirectory,
    int                     productId,
    int                     zoneLow,
    int                     zoneHigh,
    int                     purchaseValue,
    Time_t                  expiryDateTime,
    int                     clearProvisional
)
{
    ProductSale_t           request;

    if ( pData == NULL || pDirectory == NULL )
    {
        return -1;
    }

    memset( &request, 0, sizeof( request ) );

    request.productId               = productId;
    request.issuerId                = 1; // TODO: Look up issuer id from product configuration
    request.purchaseValue           = purchaseValue;
    request.zoneLow                 = zoneLow;
    request.zoneHigh                = zoneHigh;
    request.isStartDateTimeSet      = FALSE;
    request.startDateTime           = 0;
    request.isEndDateTimeSet        = TRUE;
    request.endDateTime             = expiryDateTime;
    request.serviceProviderId       = pData->StaticData.serviceProviderId;
    request.isActionSequenceNoSet   = FALSE;
    request.actionSequenceNo        = 0;
    request.isEntryPointIdSet       = TRUE;
    request.entryPointId            = pData->DynamicData.entryPointId;
    request.isRouteIdSet            = TRUE;
    request.routeId                 = pData->DynamicData.lineId;
    request.isStopIdSet             = TRUE;
    request.stopId                  = pData->DynamicData.stopId;
    request.isAutoloadSet           = FALSE;
    request.isAutoload              = FALSE;
    request.isOffpeakSet            = FALSE;
    request.isOffpeak               = FALSE;
    request.isProvisionalSet        = clearProvisional;
    request.isProvisional           = FALSE;
    request.isInstanceCountSet      = FALSE;
    request.instanceCount           = 0;
    request.serialNo                = pDirectory->SerialNo;

    DebugShowLdtProductSaleRequest( BRLL_FIELD, &request );

#ifdef LDT_SUPPORTED

    if ( MYKI_LDT_ProductSaleUpgrade( &request, &pData->InternalData.TransactionData ) != LDT_SUCCESS )
    {
        CsErrx( "myki_br_ldt_ProductSale_Upgrade() MYKI_LDT_ProductSaleUpgrade() failed" );
        return -1;
    }

#endif

    return 0;
}   /* myki_br_ldt_ProductSale_UpgradeEx( ) */

int         myki_br_ldt_ProductSale_Upgrade
(
    MYKI_BR_ContextData_t  *pData,
    MYKI_Directory_t       *pDirectory,
    int                     productId,
    int                     zoneLow,
    int                     zoneHigh,
    int                     purchaseValue,
    Time_t                  expiryDateTime
)
{
    return  myki_br_ldt_ProductSale_UpgradeEx(
                pData, pDirectory, productId, zoneLow, zoneHigh, purchaseValue, expiryDateTime, TRUE );
}

//=============================================================================
//
//
//
//=============================================================================

int         myki_br_ldt_ProductSale_Upgrade_SameProduct
(
    MYKI_BR_ContextData_t  *pData,
    MYKI_Directory_t       *pDirectory,
    int                     zoneLow,
    int                     zoneHigh,
    int                     purchaseValue,
    Time_t                  expiryDateTime
)
{
    if ( pData == NULL || pDirectory == NULL )
    {
        return -1;
    }

    return myki_br_ldt_ProductSale_Upgrade( pData, pDirectory, pDirectory->ProductId, zoneLow, zoneHigh, purchaseValue, expiryDateTime );
}

//=============================================================================
//
//
//
//=============================================================================

int         myki_br_ldt_ProductSale_Reverse( MYKI_BR_ContextData_t *pData, MYKI_Directory_t *pDirectory )
{
#ifdef LDT_SUPPORTED

    ProductSale_t           request;

    memset( &request, 0, sizeof( request ) );

    request.productId               = pDirectory->ProductId;
    request.issuerId                = 1; // TODO: Look up issuer id from product configuration
    request.purchaseValue           = 0;
    request.zoneLow                 = 0;
    request.zoneHigh                = 0;
    request.isStartDateTimeSet      = TRUE;
    request.startDateTime           = pData->DynamicData.currentDateTime;
    request.isEndDateTimeSet        = TRUE;
    request.endDateTime             = pData->DynamicData.currentDateTime + myki_br_GetProductDuration( pData, myki_br_cd_GetProductType( pDirectory->ProductId ) );
    myki_br_RoundUpEndTime( pData, &request.endDateTime );                      // KWS: Should this be here, or in the calling code? It's a rule that may not apply to all products.
    request.serviceProviderId       = pData->StaticData.serviceProviderId;
    request.isActionSequenceNoSet   = FALSE;
    request.actionSequenceNo        = 0;
    request.isEntryPointIdSet       = TRUE;
    request.entryPointId            = pData->DynamicData.entryPointId;
    request.isRouteIdSet            = TRUE;
    request.routeId                 = pData->DynamicData.lineId;
    request.isStopIdSet             = TRUE;
    request.stopId                  = pData->DynamicData.stopId;
    request.isAutoloadSet           = FALSE;
    request.isAutoload              = FALSE;
    request.isOffpeakSet            = FALSE;
    request.isOffpeak               = FALSE;
    request.isProvisionalSet        = FALSE;
    request.isProvisional           = FALSE;
    request.isInstanceCountSet      = FALSE;
    request.instanceCount           = 0;
    request.serialNo                = pDirectory->SerialNo;

    if ( MYKI_LDT_ProductSaleReverse( &request, &pData->InternalData.TransactionData ) != LDT_SUCCESS )
    {
        CsErrx( "myki_br_ldt_ProductSale() MYKI_LDT_ProductSale() failed" );
        return -1;
    }

#endif

    return 0;
}

//=============================================================================
//
//  Create a usage log
//
//=============================================================================

int         myki_br_ldt_AddUsageLogEntry( MYKI_BR_ContextData_t *pData )
{
    DebugShowLdtAddUsageLog( BRLL_FIELD, &pData->InternalData.UsageLogData );

#ifdef LDT_SUPPORTED

    if ( MYKI_LDT_TAppUsageLog( &pData->InternalData.UsageLogData ) != LDT_SUCCESS )
    {
        CsErrx( "myki_br_ldt_AddUsageLogEntry() MYKI_LDT_TAppUsageLog() failed" );
        return -1;
    }

#endif

    return 0;
}

/*==========================================================================*
**
**  myki_br_ldt_AddLoadLogEntry
**
**  Description     :
**      Creates TAppLoadLog record.
**
**  Parameters      :
**      pData               [I]     BR context data
**
**  Returns         :
**      0                           success
**      <0                          failed
**
**  Notes           :
**
**==========================================================================*/

int         myki_br_ldt_AddLoadLogEntry( MYKI_BR_ContextData_t *pData )
{
    DebugShowLdtAddLoadLog( BRLL_FIELD, &pData->InternalData.LoadLogData );

    #ifdef LDT_SUPPORTED
    if ( MYKI_LDT_TAppLoadLog( &pData->InternalData.LoadLogData ) != LDT_SUCCESS )
    {
        CsErrx( "myki_br_ldt_AddLoadLogEntry() MYKI_LDT_TAppLoadLog() failed" );
        return -1;
    }
    #endif

    return 0;
}   /* myki_br_ldt_AddLoadLogEntry( ) */

//=============================================================================
//
//  Determine a provisional fare according to NTS0124 section 3.8.
//
//  Arguments :
//      pFare           Pointer to Currency_t value to hold the determined fare
//      passengerCode   Passenger type
//      zoneLow         Low Zone
//      zoneHigh        High Zone
//      ePurse          TRUE to return ePurse fare, FALSE to return Stored Value fare
//
//  Returns :
//      0       All ok
//      < 0     Error (*pFare is set to 0)
//
//  Notes :
//  -   NTS0124 states that the system must support up to 20 provisional fare types,
//      with the fare type determined by passenger type and/or day type. Apparently
//      this is no longer true - there are only 2 provisional fare types, full and
//      concession, with the fare type determined purely by the passenger type. The
//      fare type is determined as full if the passenger discount is zero (0), or
//      concession otherwise.
//
//  -   Because of this, rather than specifying the fare type when extracting the
//      data from CD, the fare for both fare types is returned by each call.
//
//  -   This also means that the current date/time is not required by this function.
//
//  -   NTS0124 also states that each fare type has two configured fares - one
//      for use when the card contains an ePass product valid for the scan-on
//      zone, and another for all other cases.
//
//  -   Because of this, both fares are returned by each CD call.
//
//  -   Therefore, only the low and high zone are passed to the CD call, which
//      returns 4 values :
//      -   sv_full         Stored Value Full fare          (no valid ePass and passenger discount = 0)
//      -   sv_conc         Stored Value Concession fare    (no valid ePass and passenger discount > 0)
//      -   epass_full      ePass Full fare                 (valid ePass and passenger discount = 0)
//      -   epass_conc      ePass Concession fare           (valid ePass and passenger discount > 0)
//
//  -   These are equivalent to calling (if these functions existed) :
//      -   GetProvisionalFare( zoneLow, zoneHigh, STORED_VALUE, FARETYPE_FULL )
//      -   GetProvisionalFare( zoneLow, zoneHigh, STORED_VALUE, FARETYPE_CONCESSION )
//      -   GetProvisionalFare( zoneLow, zoneHigh, EPASS       , FARETYPE_FULL )
//      -   GetProvisionalFare( zoneLow, zoneHigh, EPASS       , FARETYPE_CONCESSION )
//
//  -   NTS0124 states that the zone range is determined by the transport mode and
//      current route/stop or station/platform, but this is determined elsewhere
//      and the zone range is passed to this function, therefore the current
//      transport mode and route/stop or station/platform details do not need to be
//      passed to this function.
//
//=============================================================================

int     myki_br_GetProvisionalFare( Currency_t *pFare, int passengerCode, int zoneLow, int zoneHigh, int ePurse )
{
    MYKI_CD_PassengerType_t     passengerType;
    MYKI_CD_FaresProvisional_t  faresProvisional;
    Currency_t                  fare = 0;           // Just in case it's not set anywhere below (shouldn't happen)

    *pFare = 0;     // Just in case the caller doesn't check the return code on error, set it to a known value.

    if ( zoneLow < 0 || zoneLow > 255 || zoneHigh < 0 || zoneHigh > 255 || passengerCode < 0 || passengerCode > 255 )
    {
        CsErrx( "myki_br_GetProvisionalFare : Invalid low zone (%d), high zone (%d), or passenger code (%d), must all be between 0 and 255", zoneLow, zoneHigh, passengerCode );
        return -1;
    }

    if ( ! MYKI_CD_getPassengerTypeStructure( (U8_t)passengerCode, &passengerType ) )
    {
        CsErrx( "myki_br_GetProvisionalFare : MYKI_CD_getPassengerTypeStructure() failed" );
        return -1;
    }

    if ( passengerType.percent > 100 )      // No need to check for < 0 as it's an unsigned value. If it ever becomes signed, add a check for < 0.
    {
        CsErrx( "myki_br_GetProvisionalFare : Invalid passenger type percent (%d), must be between 0 and 100", passengerType.percent );
        return -1;
    }

    if ( ! MYKI_CD_getFaresProvisionalStructure( (U8_t)zoneLow, (U8_t)zoneHigh, &faresProvisional ) )
    {
        CsErrx( "myki_br_GetProvisionalFare : MYKI_CD_getFaresProvisionalStructure() failed" );
        return -1;
    }

    if ( ePurse )
    {
        if ( passengerType.percent == 0 )                           // Passenger type percent determines the fare type (full or concession)
            fare = faresProvisional.sv_full;
        else
            fare = faresProvisional.sv_conc;
    }
    else
    {
        if ( passengerType.percent == 0 )                           // Passenger type percent determines the fare type (full or concession)
            fare = faresProvisional.epass_full;
        else
            fare = faresProvisional.epass_conc;
    }

    *pFare = fare;
    return 0;
}

//=============================================================================
//
//  Determine an ePass fare according to NTS0124 section 3.14.
//
//  Arguments :
//      pFare           Pointer to Currency_t value to hold the determined fare
//      duration        Number of days validity for this ePass
//      zoneLow         Low Zone
//      zoneHigh        High Zone
//
//  Returns :
//      0       All ok
//      < 0     Error (*pFare is set to 0)
//
//  Notes :
//
//=============================================================================

int     myki_br_GetEpassFare( Currency_t *pFare, int duration, int zoneLow, int zoneHigh, int ePurse )
{
    MYKI_CD_FaresEpass_t        faresEpass;
    Currency_t                  fare = 0;           // Just in case it's not set anywhere below (shouldn't happen)

    *pFare = 0;     // Just in case the caller doesn't check the return code on error, set it to a known value.

    if ( zoneLow < 0 || zoneLow > 255 || zoneHigh < 0 || zoneHigh > 255 || duration < 0 || duration > 65535 )
    {
        CsErrx( "myki_br_GetEpassFare : Invalid low zone (%d), high zone (%d), or duration (%d), must all be between 0 and 255 or 65535", zoneLow, zoneHigh, duration );
        return -1;
    }

    if ( ! MYKI_CD_getFaresEpassStructure( (U8_t)zoneLow, (U8_t)zoneHigh, (U16_t)duration, &faresEpass ) )
    {
        CsErrx( "myki_br_GetEpassFare : MYKI_CD_getFaresProvisionalStructure() failed" );
        return -1;
    }

    fare = faresEpass.value;

    *pFare = fare;
    return 0;
}

/*----------------------------------------------------------------------------
** FUNCTION           : myki_br_buildDifferencePriceRequest
**
** DESCRIPTION        : Fill in a MYKI_CD_DifferentialPricingRequest_t
**                      structure from provided data. This function will zero
**                      the data structure before it fills it.
**
** INPUTS             : pData               [I]     BR context data
**                      MYKI_CD_DifferentialPricingRequest  - Structure to fill
**                      nZoneLow
**                      nZoneHigh
**                      nPassengerCode
**                      nTripDirection
**                      nDateTime
**
** RETURNS            :     -1      Error
**                          0       No Error
**
----------------------------------------------------------------------------*/

int myki_br_buildDifferencePriceRequest
(
    MYKI_BR_ContextData_t                *pData,
    MYKI_CD_DifferentialPricingRequest_t *MYKI_CD_DifferentialPricingRequest,
    int                                   nZoneLow,
    int                                   nZoneHigh,
    int                                   nPassengerCode,
    TripDirection_t                       nTripDirection,
    Time_t                                nScanOnDateTime,
    Time_t                                nScanOffDateTime
)
{
    #define MYKI_CD_DayOfWeek_Sunday        7
    struct tm*                              pTm = NULL;
    struct tm                               ScanOnTm;
    struct tm                               ScanOffTm;

    /*
    ** Init target data structure before we fill it in
    ** Assume that the user has not seeded any values into the structure
    */
    memset( MYKI_CD_DifferentialPricingRequest, 0, sizeof( *MYKI_CD_DifferentialPricingRequest ) );

    if ( ( pTm = localtime( (time_t*)&nScanOnDateTime ) ) == NULL )
    {
        CsErrx( "myki_br_buildDifferencePriceRequest: Invalid Scan On DateTime" );
        return -1;
    }
    ScanOnTm = *pTm;

    MYKI_CD_DifferentialPricingRequest->scanon_minutes_since_midnight    = ( ScanOnTm.tm_hour * 60 ) + ScanOnTm.tm_min;

    //  Between midnight and the "end of business day" (eg 3am), we need to consider this to be the previous "day".
    if ( MYKI_CD_DifferentialPricingRequest->scanon_minutes_since_midnight < 180 )
    {
        nScanOnDateTime -= HOURS_TO_SECONDS( 24 );
        if ( ( pTm = localtime( (time_t*)&nScanOnDateTime ) ) == NULL )
        {
            CsErrx( "myki_br_buildDifferencePriceRequest: Invalid Scan On DateTime" );
            return -1;
        }
        ScanOnTm = *pTm;
    }

    if ( ( pTm = localtime( (time_t*)&nScanOffDateTime ) ) == NULL )
    {
        CsErrx( "myki_br_buildDifferencePriceRequest: Invalid Scan Off DateTime" );
        return -1;
    }
    ScanOffTm = *pTm;

    MYKI_CD_DifferentialPricingRequest->scanoff_minutes_since_midnight   = ( ScanOffTm.tm_hour * 60 ) + ScanOffTm.tm_min;

    MYKI_CD_DifferentialPricingRequest->day_of_week                      = (U8_t)( ScanOnTm.tm_wday == DayOfWeek_Sunday ? MYKI_CD_DayOfWeek_Sunday : ScanOnTm.tm_wday );
    MYKI_CD_DifferentialPricingRequest->day_of_month                     = ScanOnTm.tm_mday;
    MYKI_CD_DifferentialPricingRequest->month                            = ScanOnTm.tm_mon + 1;

    MYKI_CD_DifferentialPricingRequest->rule_type                        = MYKI_CD_DIFFERENTIAL_PRICING_RULE_TYPE_ANY;
    MYKI_CD_DifferentialPricingRequest->passenger_type_null              = FALSE;
    MYKI_CD_DifferentialPricingRequest->passenger_type                   = (U8_t)nPassengerCode;
    MYKI_CD_DifferentialPricingRequest->zone_low                         = (U8_t)nZoneLow;
    MYKI_CD_DifferentialPricingRequest->zone_high                        = (U8_t)nZoneHigh;

    switch ( nTripDirection )
    {
    case TRIP_DIRECTION_INBOUND:
        MYKI_CD_DifferentialPricingRequest->trip_direction               = MYKI_CD_TRIP_DIRECTION_INBOUND;
        break;

    case TRIP_DIRECTION_OUTBOUND:
        MYKI_CD_DifferentialPricingRequest->trip_direction               = MYKI_CD_TRIP_DIRECTION_OUTBOUND;
        break;

    default:
        MYKI_CD_DifferentialPricingRequest->trip_direction               = MYKI_CD_TRIP_DIRECTION_UNKNOWN;
        break;
    }

    //
    //  Add in current route and service provider
    //
    pData->InternalData.route.route_id = pData->DynamicData.lineId;
    pData->InternalData.route.service_provider_id = pData->StaticData.serviceProviderId;

    MYKI_CD_DifferentialPricingRequest->route_count = 1;
    MYKI_CD_DifferentialPricingRequest->routes = &pData->InternalData.route;

    CsDbg( BRLL_RULE, "rule_type                      = %d", MYKI_CD_DifferentialPricingRequest->rule_type                      );
    CsDbg( BRLL_RULE, "passenger_type_null            = %d", MYKI_CD_DifferentialPricingRequest->passenger_type_null            );
    CsDbg( BRLL_RULE, "passenger_type                 = %d", MYKI_CD_DifferentialPricingRequest->passenger_type                 );
    CsDbg( BRLL_RULE, "scanon_minutes_since_midnight  = %d", MYKI_CD_DifferentialPricingRequest->scanon_minutes_since_midnight  );
    CsDbg( BRLL_RULE, "scanoff_minutes_since_midnight = %d", MYKI_CD_DifferentialPricingRequest->scanoff_minutes_since_midnight );
    CsDbg( BRLL_RULE, "day_of_week                    = %d", MYKI_CD_DifferentialPricingRequest->day_of_week                    );
    CsDbg( BRLL_RULE, "day_of_month                   = %d", MYKI_CD_DifferentialPricingRequest->day_of_month                   );
    CsDbg( BRLL_RULE, "month                          = %d", MYKI_CD_DifferentialPricingRequest->month                          );
    CsDbg( BRLL_RULE, "zone_low                       = %d", MYKI_CD_DifferentialPricingRequest->zone_low                       );
    CsDbg( BRLL_RULE, "zone_high                      = %d", MYKI_CD_DifferentialPricingRequest->zone_high                      );
    CsDbg( BRLL_RULE, "trip_direction                 = %d", MYKI_CD_DifferentialPricingRequest->trip_direction                 );
    CsDbg( BRLL_RULE, "route_count                    = %d", MYKI_CD_DifferentialPricingRequest->route_count                    );
    CsDbg( BRLL_RULE, "routes                         = %d", MYKI_CD_DifferentialPricingRequest->routes[ 0 ]                    );

    return 0;
}

/*==========================================================================*
**
**  myki_br_getFareStoredValue
**
**  Description     :
**      Determines SingleTrip/nHour/Daily/Weekly/Weekend/Entitlement
**      Stored Value fares.
**      If successful, the following data shall be updated,
**
**      Tariff.DailyCapValue
**      Tariff.WeeklyCapValue
**      Tariff.WeekendCapValue
**      Tariff.EntitlementZoneLow
**      Tariff.EntitlementZoneHigh
**      Tariff.EntitlementProduct
**      Tariff.EntitlementValue
**
**  Parameters      :
**      pApplicableFare     [O]     Returned applicable fare
**      pData               [I]     BR context data
**      nZoneLow            [I]     Zone low
**      nZoneHigh           [I]     Zone high
**      nPassengerCode      [I]     Passenger type code
**      nTripDirection      [I]     Trip direction
**      bFareRouteIdIsValid [I]     Not used
**      nFareRouteId        [I]     Not used
**      nDateTime           [I]     UTC
**
**  Returns         :
**      0                           successful
**      -1                          failed
**
**  Notes           :
**
**      Refer to KA0002 v7.2
**      Sect 2.2.4  Fares (Stored Value)
**      Sect 2.2.10 Differential Pricing (Stored Value)
**
**==========================================================================*/

int myki_br_getFareStoredValueEx
(
    Currency_t*                     pApplicableFare,
    MYKI_BR_ContextData_t*          pData,
    int                             nZoneLow,
    int                             nZoneHigh,
    int                             nPassengerCode,
    TripDirection_t                 nTripDirection,
    int                             bFareRouteIdIsValid,
    int                             nFareRouteId,
    Time_t                          nScanOnDateTime,
    Time_t                          nScanOffDateTime
)
{
    MYKI_CD_FaresStoredValue_t              MYKI_CD_FaresStoredValue;
    MYKI_CD_DifferentialPricingRequest_t    MYKI_CD_DifferentialPricingRequest;
    MYKI_CD_DifferentialPricing_t           MYKI_CD_DifferentialPricing;
    MYKI_CD_PassengerType_t                 passengerType;
    Currency_t                              nApplicableFare;

    /* Init data statructures */
    memset( &MYKI_CD_DifferentialPricing, 0, sizeof( MYKI_CD_DifferentialPricing  ) );
    memset( &MYKI_CD_FaresStoredValue   , 0, sizeof( MYKI_CD_FaresStoredValue     ) );

    //
    //  Get the standard Stored-Value fares from CD (single trip, nhour, daily, weekly, weekend)
    //

    if ( MYKI_CD_getFaresStoredValueStructure( (U8_t)nZoneLow, (U8_t)nZoneHigh, &MYKI_CD_FaresStoredValue ) == FALSE )
    {
        CsErrx( "myki_br_getFareStoredValueEx : MYKI_CD_getFaresStoredValueStructure() failed" );
        return -1;
    }

    CsDbg( BRLL_RULE, "myki_br_getFareStoredValueEx : SV Fares returned from CD :" );
    CsDbg( BRLL_RULE, "myki_br_getFareStoredValueEx :   SingleTrip : %d", MYKI_CD_FaresStoredValue.single_trip );
    CsDbg( BRLL_RULE, "myki_br_getFareStoredValueEx :   nHour      : %d", MYKI_CD_FaresStoredValue.nhour       );
    CsDbg( BRLL_RULE, "myki_br_getFareStoredValueEx :   Daily      : %d", MYKI_CD_FaresStoredValue.daily       );
    CsDbg( BRLL_RULE, "myki_br_getFareStoredValueEx :   Weekly     : %d", MYKI_CD_FaresStoredValue.weekly      );
    CsDbg( BRLL_RULE, "myki_br_getFareStoredValueEx :   Weekend    : %d", MYKI_CD_FaresStoredValue.weekend     );

    if ( ( MYKI_CD_FaresStoredValue.daily > 0x7FFFFFFF ) || ( MYKI_CD_FaresStoredValue.weekly > 0x7FFFFFFF ) || ( MYKI_CD_FaresStoredValue.weekend > 0x7FFFFFFF ) )
    {
        CsErrx( "myki_br_getFareStoredValueEx : Stored Value out of range" );
        return -1;
    }

    //
    //  Discount by passenger type and set the returned Tariff values (daily, weekly, weekend)
    //

    if ( ! MYKI_CD_getPassengerTypeStructure( (U8_t)nPassengerCode, &passengerType ) )
    {
        CsErrx( "myki_br_getFareStoredValueEx : MYKI_CD_getPassengerTypeStructure() failed" );
        return -1;
    }

    if ( passengerType.percent > 100 )      // No need to check for < 0 as it's an unsigned value. If it ever becomes signed, add a check for < 0.
    {
        CsErrx( "myki_br_getFareStoredValueEx : Invalid passenger type percent (%d), must be between 0 and 100", passengerType.percent );
        return -1;
    }

    CsDbg( BRLL_RULE, "myki_br_getFareStoredValueEx : Passenger Type = %d, Percent Discount = %d", nPassengerCode, passengerType.percent );

    pData->Tariff.singleTripValue       = CalculateDiscountedFare( MYKI_CD_FaresStoredValue.single_trip, passengerType.percent );
    pData->Tariff.nHourValue            = CalculateDiscountedFare( MYKI_CD_FaresStoredValue.nhour,       passengerType.percent );
    pData->Tariff.dailyCapValue         = CalculateDiscountedFare( MYKI_CD_FaresStoredValue.daily,       passengerType.percent );
    pData->Tariff.weeklyCapValue        = CalculateDiscountedFare( MYKI_CD_FaresStoredValue.weekly,      passengerType.percent );
    pData->Tariff.weekendCapValue       = CalculateDiscountedFare( MYKI_CD_FaresStoredValue.weekend,     passengerType.percent );
    pData->Tariff.entitlementProduct    = 0;
    pData->Tariff.entitlementValue      = 0;
    pData->Tariff.entitlementZoneLow    = 0;
    pData->Tariff.entitlementZoneHigh   = 0;
    nApplicableFare                     = pData->Tariff.singleTripValue;

    CsDbg( BRLL_RULE, "myki_br_getFareStoredValueEx : Tariff values after passenger discount :" );
    CsDbg( BRLL_RULE, "myki_br_getFareStoredValueEx :   SingleTrip : %d", pData->Tariff.singleTripValue );
    CsDbg( BRLL_RULE, "myki_br_getFareStoredValueEx :   nHour      : %d", pData->Tariff.nHourValue      );
    CsDbg( BRLL_RULE, "myki_br_getFareStoredValueEx :   Daily      : %d", pData->Tariff.dailyCapValue   );
    CsDbg( BRLL_RULE, "myki_br_getFareStoredValueEx :   Weekly     : %d", pData->Tariff.weeklyCapValue  );
    CsDbg( BRLL_RULE, "myki_br_getFareStoredValueEx :   Weekend    : %d", pData->Tariff.weekendCapValue );
    CsDbg( BRLL_RULE, "myki_br_getFareStoredValueEx : Applicable fare after passenger discount : %d", nApplicableFare );

    //
    //  Process any differential (ie exception) rules
    //

    /*
    ** Create the basic request, then add in fields specific to this operation
    */
    if (myki_br_buildDifferencePriceRequest(
                    pData,
                    &MYKI_CD_DifferentialPricingRequest,
                    nZoneLow,
                    nZoneHigh,
                    nPassengerCode,
                    nTripDirection,
                    nScanOnDateTime,
                    nScanOffDateTime) < 0)
    {
        /* Error. Details have been reproted */
        return -1;
    }

    MYKI_CD_DifferentialPricingRequest.rule_type = MYKI_CD_DIFFERENTIAL_PRICING_RULE_TYPE_STANDARD;

    /* Retrieves Differential Pricing (Stored Value) record */
    if ( MYKI_CD_getDifferentialPriceStructure( &MYKI_CD_DifferentialPricingRequest, &MYKI_CD_DifferentialPricing ) )
    {
        CsDbg
        (
            BRLL_RULE,
            "myki_br_getFareStoredValueEx : Returned rule id = %d (%s), type = %d, base = %d, applied = %d",
            MYKI_CD_DifferentialPricing.id,
            MYKI_CD_DifferentialPricing.short_desc,
            MYKI_CD_DifferentialPricing.discount_type,
            MYKI_CD_DifferentialPricing.base_discount,
            MYKI_CD_DifferentialPricing.applied_discount
        );

        /* And calculates fares */
        switch ( MYKI_CD_DifferentialPricing.discount_type )
        {
        case MYKI_CD_DISCOUNT_TYPE_STATIC:
            CsDbg( BRLL_RULE, "myki_br_getFareStoredValueEx : Static discount" );

            if ( ( pData->Tariff.entitlementProduct = MYKI_CD_DifferentialPricing.entitlement_product ) > 0 )
            {
                pData->Tariff.entitlementValue      = MYKI_CD_DifferentialPricing.applied_discount;
                pData->Tariff.entitlementZoneLow    = MYKI_CD_DifferentialPricing.entitlement_zone_low;
                pData->Tariff.entitlementZoneHigh   = MYKI_CD_DifferentialPricing.entitlement_zone_high;
            }
            else
            {
				/*  Diffenrential pricing only affects daily capping value */
                pData->Tariff.dailyCapValue   = MYKI_CD_DifferentialPricing.base_discount;
            }
            break;

        case MYKI_CD_DISCOUNT_TYPE_PERCENT:
            CsDbg( BRLL_RULE, "myki_br_getFareStoredValueEx : Percent discount" );

            if ( ( pData->Tariff.entitlementProduct = MYKI_CD_DifferentialPricing.entitlement_product ) > 0 )
            {
				/*  This should not happen !!! Entitlement products with Percent discount don't exist in Tariff. */
                CsErrx( "myki_br_getFareStoredValueEx : Unexpected entitlement product (%d) with percentage discount",
                        MYKI_CD_DifferentialPricing.entitlement_product );
                return -1;
            }
            else
            {
                nApplicableFare                     = CalculateDiscountedFare( nApplicableFare,               MYKI_CD_DifferentialPricing.applied_discount );

				/*  Diffenrential pricing only affects daily capping value */
                pData->Tariff.dailyCapValue         = CalculateDiscountedFare( pData->Tariff.dailyCapValue,   MYKI_CD_DifferentialPricing.base_discount );
            }
            break;

        default:
            CsErrx( "myki_br_getFareStoredValueEx : Unsupported discount type (%d)", MYKI_CD_DifferentialPricing.discount_type );
            return -1;
        }
    }

    CsDbg( BRLL_RULE, "myki_br_getFareStoredValueEx : dailyCapValue       = %d", pData->Tariff.dailyCapValue       );
    CsDbg( BRLL_RULE, "myki_br_getFareStoredValueEx : weeklyCapValue      = %d", pData->Tariff.weeklyCapValue      );
    CsDbg( BRLL_RULE, "myki_br_getFareStoredValueEx : weekendCapValue     = %d", pData->Tariff.weekendCapValue     );
    CsDbg( BRLL_RULE, "myki_br_getFareStoredValueEx : entitlementValue    = %d", pData->Tariff.entitlementValue    );
    CsDbg( BRLL_RULE, "myki_br_getFareStoredValueEx : entitlementZoneLow  = %d", pData->Tariff.entitlementZoneLow  );
    CsDbg( BRLL_RULE, "myki_br_getFareStoredValueEx : entitlementZoneHigh = %d", pData->Tariff.entitlementZoneHigh );
    CsDbg( BRLL_RULE, "myki_br_getFareStoredValueEx : applicableFare      = %d", nApplicableFare                   );

    if ( pApplicableFare != NULL )
    {
        *pApplicableFare    = nApplicableFare;
    }

    return 0;
}


//=============================================================================
//
//  Determine if the current location is the same as the scan-on location,
//  taking into account the differing definition of "same location" for rail
//  vs non-rail devices.
//
//  Returns TRUE if the locations are the same, FALSE if they're not.
//
//  This common function implements the following steps from multiple
//  rules in KA0004 (sample taken from BR_LLSC_4_3) :
//
//  4.  If the Mode of transport by the current device is Rail
//  a.  The scan-on location3 of the product in use1 is equal to the current location5.
//  5.  Else the scan-on location3 of the product in use1 is equal to the current location5 and the scan on Route/Stop is equal to the current route/stop of the device.

//  Notes :
//  -   If the modes of transport for the two locations are different, then
//      the above comparisons are all guaranteed to fail, since each of the
//      three elements are guaranteed to be different for differen modes -
//      see myki_br_context_data.h.
//  -   Therefore, the first check is that the modes are the same. If not,
//      the location is not the same. This is implied, but not stated, by
//      KA0004.
//
//=============================================================================

int     myki_br_CurrentLocationIsScanOnLocationOrRoute(
    MYKI_BR_ContextData_t  *pData,
    MYKI_TAProduct_t       *pMYKI_TAProduct,
    int                     bCheckSameStop )
{
    int     OrigMode;       // Transport mode of last usage location
    int     CurrMode;       // Transport mode of current location

    //  Get the origin and current location modes of transport

    OrigMode = myki_br_cd_GetTransportModeForProvider( pMYKI_TAProduct->LastUsage.ProviderId );
    CurrMode = pData->InternalData.TransportMode;

    //  If they're different, then it can't be the same location (for now ignores the possibility of a bus stop and train station at the same location)

    if ( OrigMode != CurrMode )
    {
        CsDbg( BRLL_RULE, "Scan-on mode (%d) != Current mode (%d)", OrigMode, CurrMode );
        return FALSE;
    }

    //  Modes are both the same, so from now on just check one of them (arbitrarily picked origin)

    if ( OrigMode == TRANSPORT_MODE_RAIL )
    {
        //  Rail - check the station ID (ie Entry Point ID) only

        if ( pMYKI_TAProduct->LastUsage.Location.EntryPointId != pData->DynamicData.entryPointId )
        {
            CsDbg( BRLL_RULE, "Rail and Scan-on station (%d) != Current station (%d)", pMYKI_TAProduct->LastUsage.Location.EntryPointId, pData->DynamicData.entryPointId );
            return FALSE;
        }
    }
    else
    {
        //  Bus, Tram, etc - check the vehicle ID (ie Entry Point ID) as well as the route and stop

        if ( pMYKI_TAProduct->LastUsage.Location.EntryPointId != pData->DynamicData.entryPointId )
        {
            CsDbg( BRLL_RULE, "Not rail and Scan-on vehicle (%d) != Current vehicle (%d)", pMYKI_TAProduct->LastUsage.Location.EntryPointId, pData->DynamicData.entryPointId );
            return FALSE;
        }

        if ( pMYKI_TAProduct->LastUsage.Location.RouteId != pData->DynamicData.lineId )
        {
            CsDbg( BRLL_RULE, "Not rail and Scan-on route (%d) != Current route (%d)", pMYKI_TAProduct->LastUsage.Location.RouteId, pData->DynamicData.lineId );
            return FALSE;
        }

        if ( bCheckSameStop != FALSE &&
             pMYKI_TAProduct->LastUsage.Location.StopId != pData->DynamicData.stopId )
        {
            CsDbg( BRLL_RULE, "Not rail and Scan-on stop (%d) != Current stop (%d)", pMYKI_TAProduct->LastUsage.Location.StopId, pData->DynamicData.stopId );
            return FALSE;
        }
    }

    //  All tests for differences passed, so it's the same location

    return TRUE;
}

int     myki_br_CurrentLocationIsScanOnLocation(
    MYKI_BR_ContextData_t  *pData,
    MYKI_TAProduct_t       *pMYKI_TAProduct )
{
    return  myki_br_CurrentLocationIsScanOnLocationOrRoute( pData, pMYKI_TAProduct, TRUE );
}   /*  myki_br_CurrentLocationIsScanOnLocation( ) */

/*==========================================================================*
**
**  myki_br_EndOfBusinessDateTime
**
**  Description     :
**      Determines the UTC End of Business Date/Time given UTC.
**
**  Parameters      :
**      pData               [I]     BR context data
**      dateTime            [I]     UTC
**      endOfWeek           [I]     TRUE if returning end business week
**                                  FALSE if returning end business day
**
**  Returns         :
**      xxx                         UTC end of business date/time
**
**  Notes           :
**      Tariff.EndOfBusinessDay is number of minutes since mid-night.
**      Business week starts/ends at Tariff.EndOfBusinessDay on next Monday.
**
**==========================================================================*/

Time_t myki_br_EndOfBusinessDateTime(
    MYKI_BR_ContextData_t*  pData,
    Time_t                  dateTime,
    int                     endOfWeek )
{
    struct tm              *pTm             = NULL;
    Time_t                  time            = TIME_NOT_SET;
    DateC19_t               date            = 0;
    DateC19_t               nDaysToAdd      = 0;

    if ( ( pTm = localtime( (time_t*)&dateTime ) ) != NULL )
    {
        time            = HOURS_TO_SECONDS(   pTm->tm_hour ) +
                          MINUTES_TO_SECONDS( pTm->tm_min  ) +
                                              pTm->tm_sec;
        pTm->tm_year   += ( 1900 - DATEC19_EPOCH );
        date            = mkdate( pTm );    /* Days since 31/12/1799 */
        if ( date == DATEC19_INVALID )
        {
            time    = TIME_NOT_SET;
        }
        else
        {
            if ( endOfWeek != FALSE )
            {
                /* Business week ends next Monday */
                if ( pTm->tm_wday == DayOfWeek_Monday )
                {
                    if ( time >= MINUTES_TO_SECONDS( pData->Tariff.endOfBusinessDay ) )
                    {
                        nDaysToAdd  = DayOfWeek_DaysOfWeek;
                    }   /* end-of-if */
                }
                else
                {
                    nDaysToAdd  = ( ( DayOfWeek_Monday + DayOfWeek_DaysOfWeek ) - pTm->tm_wday ) % DayOfWeek_DaysOfWeek;
                }   /* end-of-if */
            }
            else
            if ( time >= MINUTES_TO_SECONDS( pData->Tariff.endOfBusinessDay ) )
            {
                /* Business day ends tomorrow */
                nDaysToAdd  = 1;
            }   /* end-of-if */
            date           += nDaysToAdd;
            /* And converts back to UTC */
            pTm             = localdate( &date );
            pTm->tm_year   -= ( 1900 - DATEC19_EPOCH );
            pTm->tm_hour    = pData->Tariff.endOfBusinessDay / 60;
            pTm->tm_min     = pData->Tariff.endOfBusinessDay % 60;
            pTm->tm_sec     = 0;
            pTm->tm_isdst   = -1;
            time            = (Time_t)mktime( pTm );
        }   /* end-of-if */
    }   /* end-of-if */
    return time;
}   /* myki_br_EndOfBusinessDateTime( ) */

/*==========================================================================*
**
**  myki_br_StartOfBusinessDateTime
**
**  Description     :
**      Determines the UTC Start of Business Date/Time given UTC.
**
**  Parameters      :
**      pData               [I]     BR context data
**      dateTime            [I]     UTC
**
**  Returns         :
**      xxx                         UTC start of business date/time
**
**  Notes           :
**      Tariff.EndOfBusinessDay is number of minutes since mid-night.
**
**==========================================================================*/

Time_t myki_br_StartOfBusinessDateTime(
    MYKI_BR_ContextData_t*  pData,
    Time_t                  dateTime )
{
    struct tm              *pTm                     = NULL;
    Time_t                  time                    = TIME_NOT_SET;
    Time_t                  secondsSinceMidnight    = 0;

    /*
    **  Get the user provided data-time and break it apart
    */
    if ( ( pTm = localtime( (time_t*)&dateTime ) ) != NULL )
    {
        /*
        **  Add in the hours after midnight at which the business day starts
        **  Given that Tariff.EndOfBusinessDay is number of hours since mid-night
        **  to the end of the Business Day, then the start must be 24 hours
        **  before that.
        **
        **  If the current time is before the Tariff.endOfBusinessDay
        **  then the business day started yesterday.
        */
        secondsSinceMidnight    = HOURS_TO_SECONDS(   (Time_t)pTm->tm_hour ) +
                                  MINUTES_TO_SECONDS( (Time_t)pTm->tm_min  ) +
                                                      (Time_t)pTm->tm_sec;
        pTm->tm_hour        = (int)( pData->Tariff.endOfBusinessDay / 60 );
        if ( secondsSinceMidnight < MINUTES_TO_SECONDS( (Time_t)pData->Tariff.endOfBusinessDay ) )
        {
            /*  Business day is the day before specified date. */
            pTm->tm_hour   -= ( 1 /*DAY*/ * 24 );
        }
        pTm->tm_min         = (int)( pData->Tariff.endOfBusinessDay % 60 );
        pTm->tm_sec         = 0;
        pTm->tm_isdst       = -1;

        /*
        **  Process is back into a Time_t
        */
        time            = (Time_t)mktime( pTm );
    }
    return time;
}


/*==========================================================================*
**
**  myki_br_ToMykiDate
**
**  Description     :
**      Converts UTC to MYKI date, ie. days since reference date.
**
**  Parameters      :
**      pData               [I]     BR context data
**      dateTime            [I]     UTC
**
**  Returns         :
**      xxx                         converted MYKI date
**
**  Notes           :
**      DynamicData.currentBusinessDate is of type DateC19_t, ie.
**      days since Tuesday 31/12/1799.
**
**==========================================================================*/

DateC19_t myki_br_ToMykiDate( MYKI_BR_ContextData_t* pData, Time_t dateTime )
{
    DateC19_t output = 0;
    time_t input = dateTime;
    return time2date_r( &input, &output );
}   /* myki_br_ToMykiDate( ) */

/*==========================================================================*
**
**  myki_br_NextMonday
**
**  Description     :
**      Returns the absolute date of the next Monday.
**
**  Parameters      :
**      pData               [I]     BR context data
**
**  Returns         :
**      xxx                         business date of next Monday
**
**  Notes           :
**      DynamicData.currentBusinessDate is of type DateC19_t, ie.
**      days since Tuesday 31/12/1799.
**
**      Should be using currentDateTime instead of currentBusinessDate
**      i.e If current date is a Monday at 2.30am but Business Date is still
**      Sunday, extend the product until the following Monday 7 days later.
**
**==========================================================================*/

static DateC19_t myki_br_NextMonday( MYKI_BR_ContextData_t *pData )
{
    #define DayOfWeek_Reference     DayOfWeek_Tuesday

    DateC19_t   currentDate   = myki_br_ToMykiDate( pData, pData->DynamicData.currentDateTime );

    DateC19_t   TodayDayOfWeek      = ( currentDate + DayOfWeek_Reference ) % DayOfWeek_DaysOfWeek;

    DateC19_t   DaysToNextMonday    = \
                    ( TodayDayOfWeek == DayOfWeek_Monday ?
                            DayOfWeek_DaysOfWeek :
                            ( ( DayOfWeek_Monday + DayOfWeek_DaysOfWeek ) - TodayDayOfWeek ) % DayOfWeek_DaysOfWeek );

    return currentDate + DaysToNextMonday;
}   /* myki_br_NextMonday( ) */

//=============================================================================
//
//  Return the discounted fare given a full fare and a percentage discount.
//
//  Implements the rule fragment of the form:
//      Set the xxxx  to the fare  multiplied by 1 less the discount rate
//
//  Clarification:
//      The assumption of the rule fragment is that the discount rate is a
//      floating point number in the range 0..1. This is NOT the case.
//      The discount rate is actually a percentage.
//
//      Thus the calculation becomes:
//          Set the discounted fare to the base fare multiplied by ( 100 minus the discount rate ) / 100
//          ie: The discount rate is the amount by which the fare is reduced
//
//=============================================================================

Currency_t  CalculateDiscountedFare( Currency_t fullFare, int percentDiscount )
{
    //  No discount poosible on fares less than zero, or the special value 0x7FFFFFFF, just return the unmodified fare
    if ( ( fullFare < 0 ) || ( fullFare == 0x7FFFFFFF ) )
        return fullFare;

    //  Otherwise return the discounted fare
    return fullFare * ( 100 - percentDiscount ) / 100;
}

//=============================================================================
//
//
//
//=============================================================================

int     myki_br_ZonesTravelled( int zoneLow, int zoneHigh )
{
    U8_t    i;
    int     zoneCount = 0;

    for ( i = zoneLow; i <= zoneHigh; i++ )
    {
        if ( ! MYKI_CD_isCitySaverZone( i ) )
            zoneCount++;
    }

    return zoneCount;
}

//=============================================================================
//
//
//
//=============================================================================

int     myki_br_GetAdditionalMinutes( MYKI_BR_ContextData_t *pData, int zoneLow, int zoneHigh )
{
    int     zonesTravelled              = myki_br_ZonesTravelled( zoneLow, zoneHigh );
    int     extensions                  = 0;
    int     additionalMinutes           = 0;
    int     nHourMaximumDurationMinutes = (int)pData->Tariff.nHourMaximumDuration * 60;

    CsDbg( BRLL_RULE, "myki_br_GetAdditionalMinutes: Tariff.nHourExtendThreshold     : %d", pData->Tariff.nHourExtendThreshold );
    CsDbg( BRLL_RULE, "myki_br_GetAdditionalMinutes: Tariff.nHourExtendPeriodMinutes : %d", pData->Tariff.nHourExtendPeriodMinutes );
    CsDbg( BRLL_RULE, "myki_br_GetAdditionalMinutes: Tariff.nHourMaximumDuration     : %d", pData->Tariff.nHourMaximumDuration );

    CsDbg( BRLL_RULE, "myki_br_GetAdditionalMinutes: zones                           : %d - %d", zoneLow, zoneHigh );
    CsDbg( BRLL_RULE, "myki_br_GetAdditionalMinutes: zonesTravelled                  : %d", zonesTravelled );

    if ( pData->Tariff.nHourExtendThreshold != 0 )
    {
        extensions = zonesTravelled / pData->Tariff.nHourExtendThreshold;

        additionalMinutes = extensions * (int)pData->Tariff.nHourExtendPeriodMinutes;

        if ( ( pData->Tariff.nHourPeriodMinutes + additionalMinutes ) > nHourMaximumDurationMinutes )
        {
            additionalMinutes = nHourMaximumDurationMinutes - pData->Tariff.nHourPeriodMinutes;
        }
    }

    CsDbg( BRLL_RULE, "myki_br_GetAdditionalMinutes: extensions                      : %d", extensions );
    CsDbg( BRLL_RULE, "myki_br_GetAdditionalMinutes: additionalMinutes               : %d", additionalMinutes );

    return additionalMinutes;
}

//=============================================================================
//
//      ALWAYS round up the time to the next Tariff.nHourRoundingPortion
//      regardless of whether the specified time is a multiple of
//      Tariff.nHourRoundingPortion.
//
//=============================================================================

void        myki_br_RoundUpEndTime( MYKI_BR_ContextData_t *pData, Time_t *pTime )
{
    Time_t  nHourRoundingPortion    = MINUTES_TO_SECONDS( pData->Tariff.nHourRoundingPortion );

    if ( nHourRoundingPortion > 0 )
    {
        *pTime = *pTime + ( nHourRoundingPortion - ( *pTime % nHourRoundingPortion ) );
    }
}

//=============================================================================
//
//
//
//=============================================================================

int         myki_br_cd_GetZoneByLocation( int locationId )
{
    MYKI_CD_Locations_t         cdLocation;

    if ( locationId < 0 || locationId > 0xFFFF )
    {
        CsErrx( "getZoneByLocation() Invalid location ID %d", locationId );
        return -1;
    }

    if ( ! MYKI_CD_getLocationsStructure( (U16_t)locationId, &cdLocation ) )
    {
        CsErrx( "getZoneByLocation() Failed to get location data from CD for location ID %d", locationId );
        return -1;
    }

    return cdLocation.zone;
}

//=============================================================================
//
//  Used for efficiency when we already have the location details from CD.
//
//=============================================================================

int     myki_br_AddLocationToTransportLocations( MYKI_BR_ContextData_t *pData, MYKI_CD_Locations_t *pLocation )
{
    if ( pData->DynamicData.transportLocationsCount >= DIMOF( pData->DynamicData.transportLocations ) )
    {
        CsErrx( "myki_br_AddLocationToTransportLocations: Transport location list full (%d entries)", DIMOF( pData->DynamicData.transportLocations ) );
        return -1;
    }

    CsDbg( BRLL_FIELD, "myki_br_AddLocationToTransportLocations: %d:%d:%d",  pLocation->inner_zone,pLocation->zone, pLocation->outer_zone);
    pData->DynamicData.transportLocations[ pData->DynamicData.transportLocationsCount++ ] = *pLocation;
    return 0;
}

//=============================================================================
//
//  Used when we have a station ID but haven't had to already retrieve the
//  station and location details from CD.
//
//=============================================================================

int     myki_br_AddStationToTransportLocations( MYKI_BR_ContextData_t *pData, int stationId )
{
    MYKI_CD_Stations_t      station;
    MYKI_CD_Locations_t     location;

    if ( ! MYKI_CD_getStationsStructure( (U16_t)stationId, &station ) )
    {
        CsErrx( "myki_br_AddStationToTransportLocations : Failed to get station data from CD for station ID %d", stationId );
        return -1;
    }

    if ( ! MYKI_CD_getLocationsStructure( station.location, &location ) )
    {
        CsErrx( "myki_br_AddStationToTransportLocations : Failed to get location data from CD for location ID %d", station.location );
        return -1;
    }

    return myki_br_AddLocationToTransportLocations( pData, &location );
}

/*==========================================================================*
**
**  myki_br_AddStopToTransportLocations
**
**  Description     :
**      Adds stop to Transport Locations.
**
**  Parameters      :
**      pData               [I]     BR context data
**      routeId             [I]     route ID
**      stopId              [I]     stop ID
**      count               [I]     number of times to add the stop
**
**  Returns         :
**      0                           success
**      -1                          failed
**
**  Notes           :
**      
**
**==========================================================================*/

int     myki_br_AddStopToTransportLocationsEx
(
    MYKI_BR_ContextData_t  *pData,
    int                     routeId,
    int                     stopId,
    int                     count
)
{
    MYKI_CD_RouteStop_t*    pRouteStops     = NULL;
    int                     routeStopsCount = 0;
    MYKI_CD_Locations_t     location;
    int                     i;

    CsDbg( BRLL_RULE, "myki_br_AddStopToTransportLocationsEx : Route(%d) Stop(%d) Count(%d)", routeId, stopId, count );

    routeStopsCount = MYKI_CD_getRouteStopsStructure( routeId, stopId, stopId, &pRouteStops );
    if ( routeStopsCount <= 0 )
    {
        CsErrx( "myki_br_AddStopToTransportLocationsEx : MYKI_CD_getRouteStopsStructure(%d,%d,%d) failed", routeId, stopId, stopId );
        return -1;
    }

    // Use the first location, there should be only one and if more than one
    // the first will do
    if ( MYKI_CD_getLocationsStructure( pRouteStops[0].locationId, &location ) == FALSE )
    {
        CsErrx( "myki_br_AddStopToTransportLocationsEx : MYKI_CD_getLocationsStructure(%d) failed", pRouteStops[0].locationId );
        return -1;
    }

    for ( i = 0; i < count; ++i )
    {
        if ( myki_br_AddLocationToTransportLocations( pData, &location ) != 0 )
            return -1;
    }
    return 0;
}   /*  myki_br_AddStopToTransportLocationsEx( ) */

int     myki_br_AddStopToTransportLocations
(
    MYKI_BR_ContextData_t  *pData,
    int                     routeId,
    int                     stopId
)
{
	return myki_br_AddStopToTransportLocationsEx( pData, routeId, stopId, 1 );
}   /*  myki_br_AddStopToTransportLocations( ) */

/*==========================================================================*
**
**  myki_br_AddStopRangeToTransportLocations
**
**  Description     :
**      Adds stop range to Transport Locations.
**
**  Parameters      :
**      pData               [I]     BR context data
**      routeId             [I]     route ID
**      fromStopId          [I]     from stop ID, 0=First Stop
**      toStopId            [I]     to stop ID, 0=Last Stop
**
**  Returns         :
**      0                           success
**      -1                          failed
**
**  Notes           :
**      
**
**==========================================================================*/

int     myki_br_AddStopRangeToTransportLocations
(
    MYKI_BR_ContextData_t  *pData,
    int                     routeId,
    int                     fromStopId,
    int                     toStopId
)
{
    int                     numberOfRouteStops  = 0;
    int                     i                   = 0;
    MYKI_CD_Locations_t     location;
    MYKI_CD_RouteStop_t    *pRouteStops         = NULL;

    CsDbg( BRLL_RULE, "myki_br_AddStopRangeToTransportLocations : Route(%d) Stops(%d-%d)", routeId, fromStopId, toStopId );

    numberOfRouteStops  = MYKI_CD_getRouteStopsStructure( routeId, fromStopId, toStopId, &pRouteStops );
    if ( numberOfRouteStops <= 0 )
    {
        CsErrx( "myki_br_AddStopRangeToTransportLocations : MYKI_CD_getRouteStopsStructure(%d,%d,%d) failed(%d)", routeId, fromStopId, toStopId, numberOfRouteStops );
        return -1;
    }

    while ( i < numberOfRouteStops )
    {
        if ( MYKI_CD_getLocationsStructure( pRouteStops[ i ].locationId, &location ) == FALSE )
        {
            CsErrx( "myki_br_AddStopRangeToTransportLocations : MYKI_CD_getLocationsStructure(%d) failed", pRouteStops[ i ].locationId );
            return -1;
        }
        if ( myki_br_AddLocationToTransportLocations( pData, &location ) < 0 )
        {
            return -1;
        }
        i++;
    }

    return 0;
}   /*  myki_br_AddStopRangeToTransportLocations( ) */

//=============================================================================
//
//
//
//=============================================================================

int     myki_br_cd_GetStationDetails( int stationId, MYKI_CD_Stations_t *pStation, MYKI_CD_Locations_t *pLocation )
{
    if ( ! MYKI_CD_getStationsStructure( (U16_t)stationId, pStation ) )
    {
        CsErrx( "myki_br_GetStationDetails : Failed to get station data from CD for station ID %d", stationId );
        return -1;
    }

    if ( ! MYKI_CD_getLocationsStructure( pStation->location, pLocation ) )
    {
        CsErrx( "myki_br_GetStationDetails : Failed to get location data from CD for location ID %d", pStation->location );
        return -1;
    }

    return 0;
}

//=============================================================================
//
//  Sets the current trip direction and low/high zone based on the supplied
//  origin and destination zone.
//
//  Sets :
//  -   Low zone to the lower of the two.
//  -   High zone to the higher of the two.
//  -   Direction based on which of the two is higher.
//
//  Assumes :
//  -   Inbound is towards the CBD and Outbound is away from the CBD.
//  -   Lower numbered logical zones are closer to the CBD than higher numbered ones.
//
//  Arguments :
//      pData               Pointer to current BR context
//      originZone          The logical zone ID of the origin
//      destinationZone     The logical zone ID of the destination
//
//  Returns :
//      Always 0 (no failure possible).
//
//=============================================================================

int     myki_br_SetTripDirectionFromZones( MYKI_BR_ContextData_t *pData, int originZone, int destinationZone )
{
    if ( originZone > destinationZone)
    {
        pData->DynamicData.currentTripDirection = TRIP_DIRECTION_INBOUND;
//        pData->DynamicData.currentTripZoneLow   = destinationZone;
//        pData->DynamicData.currentTripZoneHigh  = originZone;
    }
    else if ( originZone < destinationZone)
    {
        pData->DynamicData.currentTripDirection = TRIP_DIRECTION_OUTBOUND;
//        pData->DynamicData.currentTripZoneLow   = originZone;
//        pData->DynamicData.currentTripZoneHigh  = destinationZone;
    }
    else
    {
        pData->DynamicData.currentTripDirection = TRIP_DIRECTION_UNKNOWN;
//        pData->DynamicData.currentTripZoneLow   = originZone;
//        pData->DynamicData.currentTripZoneHigh  = destinationZone;
    }

    return 0;
}

//=============================================================================
//
//  Returns :
//      > 0     one or more lines common to both stations
//      == 0    no lines common to both stations
//      < 0     error
//
//=============================================================================

int     myki_br_CommonLines( MYKI_BR_ContextData_t *pData, int stationId1, int stationId2 )
{
    int                         i;
    int                         j;
    MYKI_CD_U16Array_t          LineList1;
    MYKI_CD_U16Array_t          LineList2;
    MYKI_CD_LinesByStation_t    linesByStation;

    if ( ! MYKI_CD_getLinesByStationStructure( (U16_t)stationId1, &linesByStation, &LineList1 ) )
    {
        CsErrx( "myki_br_CommonLines : MYKI_CD_getLinesByStationStructure( %d ) failed", stationId1 );
        return -1;
    }

    if ( ! MYKI_CD_getLinesByStationStructure( (U16_t)stationId2, &linesByStation, &LineList2 ) )
    {
        CsErrx( "myki_br_CommonLines : MYKI_CD_getLinesByStationStructure( %d ) failed", stationId2 );
        return -1;
    }

    for ( i = 0; i < LineList2.arraySize; i++ )
    {
        for ( j = 0; j < LineList1.arraySize; j++ )
        {
            if ( LineList2.arrayOfU16[ i ] == LineList1.arrayOfU16[ j ] )
            {
                return 1;
            }
        }
    }

    return 0;
}

/*==========================================================================*
**
**  myki_br_IsDDA
**
**  Description     :
**      Determines if specified passenger type is DDA.
**
**  Parameters      :
**      PassengerCode       [I]     passenger type code
**
**  Returns         :
**      <0                          failed retrieving Passenger Type record
**      0                           passenger type is not DDA
**      >0                          passenger type is DDA
**
**  Notes           :
**
**==========================================================================*/

int     myki_br_IsDDA( U8_t PassengerCode )
{
    MYKI_CD_PassengerType_t MYKI_CD_PassengerType;
    char                    isdda;

    memset( &MYKI_CD_PassengerType, 0, sizeof( MYKI_CD_PassengerType ) );
    if ( MYKI_CD_getPassengerTypeStructure( PassengerCode, &MYKI_CD_PassengerType ) < 0 )
    {
        return -1;
    }   /* end-of-if */

    isdda   = MYKI_CD_PassengerType.isdda[ 0 ];     // TODO review myki_cd structure
    if ( isdda == 't' || isdda == 'T' ||            /* ie. "true" / "TRUE" */
         isdda == 'y' || isdda == 'Y' )             /* ie. "yes"  / "YES"  */
    {
        return 1;
    }   /* end-of-if */

    return 0;
}   /* myki_br_IsDDA( ) */

/*----------------------------------------------------------------------------
** FUNCTION           : myki_br_InitProductIterator
**
** DESCRIPTION        : Assist in iteration over all products
**                      Init the ProductIterator structure
**
** INPUTS             : pMYKI_TAControl         - Ref to MYKI_TAControl_t
**                      pType                   - Required Product Type
**                                                PRODUCT_TYPE_UNKNOWN - will get all products
**                                                for user to filter
**
** RETURNS            : Nothing
**
----------------------------------------------------------------------------*/

void myki_br_InitProductIterator(MYKI_TAControl_t *pMYKI_TAControl, ProductIterator *pIter, int pType)
{
    pIter->pMYKI_TAControl = pMYKI_TAControl;
    pIter->index = 0;               // First call will skip the purse as index 0
    pIter->pType = pType;
    pIter->error = 0;

    pIter->currentProduct = PRODUCT_TYPE_UNKNOWN;
    pIter->pProduct = NULL;
    pIter->pDirectory = NULL;
}
/*----------------------------------------------------------------------------
** FUNCTION           : myki_br_ProductIterate
**
** DESCRIPTION        : Assist in iteration over all products
**                      Returns the next product of matching type
**                      See myki_br_InitProductIterator
**                      Mode: Select specified product type
**                      Mode: Select all products
**                          Will ignore PRODUCT_TYPE_UNKNOWN
**                          Will only process USED directory entries
**
**                      If an error is detected an error flag will be
**                      set in the ProductIterator structure. Future calls
**                      will return with an error indication
**
** INPUTS             : pIter   - Ref to ProductIterator control data
**
** RETURNS            : >0      - Next product returned in pIter, return value is product index (1-5)
**                       0      - End of Products
**                      -1      - Error detected and reported via CsErrx
**
----------------------------------------------------------------------------*/

int myki_br_ProductIterate(ProductIterator *pIter)
{
    // Once an error, always an error
    if (pIter->error)
    {
        return -1;
    }

    //  Continue processing from the current point

    while ( ++(pIter->index) < DIMOF(pIter->pMYKI_TAControl->Directory))
    {
        if (pIter->pMYKI_TAControl->Directory[pIter->index].Status != TAPP_CONTROL_DIRECTORY_STATUS_UNUSED )
        {
            if (myki_br_GetCardProduct(pIter->index, &pIter->pDirectory, NULL) < 0)
            {
                CsErrx("myki_br_ProductIterate : myki_br_GetCardProduct( %d ) failed", pIter->index);
                pIter->error = 1;
                return -1;
            }

            // Get the type of this product. Ignoring unknown product types
            if ((pIter->currentProduct = myki_br_cd_GetProductType(pIter->pDirectory->ProductId)) != PRODUCT_TYPE_UNKNOWN)
            {
                // If the required type then get the body of the product
                if (PRODUCT_TYPE_UNKNOWN == pIter->pType || pIter->currentProduct == pIter->pType)
                {
                    if (myki_br_GetCardProduct(pIter->index, &pIter->pDirectory, &pIter->pProduct) < 0)
                    {
                        CsErrx("myki_br_ProductIterate : myki_br_GetCardProduct( %d ) failed", pIter->index);
                        pIter->error = 1;
                        return -1;
                    }
                    return pIter->index;
                }
            }
        }
    }

    // No more products to scan
    // Shouldn't need to set these values as the user should be using them once
    // the function has signaled that all has been done.

    pIter->pProduct = NULL;
    pIter->pDirectory = NULL;
    pIter->currentProduct = PRODUCT_TYPE_UNKNOWN;

    return 0;
}

/*----------------------------------------------------------------------------
** FUNCTION           : myki_br_getCommonDate
**
** DESCRIPTION        : Calculates the 'commonDate' as used by several rules
**                      Bad terminology. Should really be called applicableDate
**
**
** INPUTS             : pData               - Conext Data
**
** RETURNS            : Calculated commonDate
**
----------------------------------------------------------------------------*/

Time_t myki_br_getCommonDate(MYKI_BR_ContextData_t *pData)
{
    //  Text from several rules:
    //  If this is a forced scan off sequence (ie IsForcedScanOff is true)
    //  then for this business rule the date for calculations to be used is
    //  the Forced Scan Off Date else use the  common date if set else the
    //  current date time this will be referred to as the common date

    if ( pData->DynamicData.isForcedScanOff != FALSE )
    {
        CsDbg( BRLL_FIELD, "myki_br_getCommonDate: ForcedScanOff: %d",  pData->DynamicData.forcedScanOffDateTime);
        return pData->DynamicData.forcedScanOffDateTime;
    }

    if (pData->DynamicData.commonDateTime != TIME_NOT_SET)
    {
        CsDbg( BRLL_FIELD, "myki_br_getCommonDate: CommonDate: %d, Current: %d",  pData->DynamicData.commonDateTime, pData->DynamicData.currentDateTime);
        return pData->DynamicData.commonDateTime;
    }

    CsDbg( BRLL_FIELD, "myki_br_getCommonDate: CurrentDate: %d",  pData->DynamicData.currentDateTime);
    return pData->DynamicData.currentDateTime;
}

/*----------------------------------------------------------------------------
** FUNCTION           : myki_br_isPhysicalZoneOneOrZero
**
** DESCRIPTION        : Determines if a specified (logical zone) is either
**                      physical zone zero or physical zone one.
**
**                      Zones within the BR are logical, but a few rules
**                      specifically check for physical zone zero or physical zone one
**
**
** INPUTS             : lzone                   - Logical Zone to Test
**
** RETURNS            : 1   (True)
**                      0   (False)
**
----------------------------------------------------------------------------*/

int myki_br_isPhysicalZoneOneOrZero(U8_t lzone)
{
    //========================================================================
    //  Coding assumption
    //      Physical Zone == Logical Zone - 1
    //  This is based on observation of the existing CD
    //========================================================================

    if (lzone == 2 || lzone == 1)
    {
        return TRUE;
    }

    return FALSE;
}

//=============================================================================
//
//
//
//=============================================================================

int     myki_br_CalculateEPassCappingContribution( MYKI_BR_ContextData_t *pData, int passengerCode )
{
    int         i;
    Currency_t  fare;

    pData->DynamicData.cappingEpassZoneLow  = ZONE_MAP_MAX + 1;
    pData->DynamicData.cappingEpassZoneHigh = ZONE_MAP_MIN - 1;

    for ( i = 0; i < pData->InternalData.ZoneMap.productCount; i++ )
    {
        if
        (
            ( pData->InternalData.ZoneMap.productList[ i ].productType == PRODUCT_TYPE_EPASS ) &&
            myki_br_ZonesOverlapOrAdjacent
            (
                pData->InternalData.ZoneMap.productList[ i ].zoneLow,
                pData->InternalData.ZoneMap.productList[ i ].zoneHigh,
                pData->InternalData.ProposedMerge.ZoneLow,
                pData->InternalData.ProposedMerge.ZoneHigh
            )
        )
        {
            //  Fare is the nHour fare for this zone range for the current passenger type and date/time.
            //  Asssuming nHour fare is the same as Single Peak fare.
            if
            (
                myki_br_getFareStoredValueEx
                (
                    &fare,
					pData,
                    pData->InternalData.ZoneMap.productList[ i ].zoneLow,
                    pData->InternalData.ZoneMap.productList[ i ].zoneHigh,
                    passengerCode,
                    pData->DynamicData.currentTripDirection,
                    pData->DynamicData.fareRouteIdIsValid,
                    pData->DynamicData.fareRouteId,
					pData->DynamicData.currentDateTime,
					pData->DynamicData.currentDateTime
                ) < 0
            )
            {
                CsErrx( "myki_br_CalculateEPassCappingContribution : myki_br_getFareStoredValueEx() failed" );
                return -1;
            }

            pData->DynamicData.cappingContribution += fare;

            if ( pData->InternalData.ZoneMap.productList[ i ].zoneLow < pData->DynamicData.cappingEpassZoneLow )
                pData->DynamicData.cappingEpassZoneLow = pData->InternalData.ZoneMap.productList[ i ].zoneLow;

            if ( pData->InternalData.ZoneMap.productList[ i ].zoneHigh > pData->DynamicData.cappingEpassZoneHigh )
                pData->DynamicData.cappingEpassZoneHigh = pData->InternalData.ZoneMap.productList[ i ].zoneHigh;
        }
    }

    return 0;
}

/*==========================================================================*
**
**  myki_br_ldt_FailureResponse
**
**  Description     :
**      Generates LDT FailureResponse.
**
**  Parameters      :
**      pData               [I]     BR context data
**      eType               [I]     transaction type
**      eSubType            [I]     transaction sub-type
**      eFailureReason      [I]     failure reason
**
**  Returns         :
**      0                           success
**      <0                          failed
**
**  Notes           :
**
**==========================================================================*/

int myki_br_ldt_FailureResponse(
    MYKI_BR_ContextData_t      *pData,
    MYKI_UD_Type_t              eType,
    int                         eSubType,
    MYKI_UD_FailureReason_t     eFailureReason )
{
    LDTFailureResponse_t        LDTFailureResponse;

    LDTFailureResponse.type             = (U8_t)eType;
    LDTFailureResponse.subtype          = (U8_t)eSubType;
    LDTFailureResponse.failureReason    = (U8_t)eFailureReason;

    if ( MYKI_LDT_FailureResponse( &LDTFailureResponse, &pData->InternalData.TransactionData ) != LDT_SUCCESS )
    {
        CsErrx( "myki_br_ldt_FailureResponse : MYKI_LDT_FailureResponse() failed" );
        return -1;
    }   /* end-of-if */
    return 0;
}   /* myki_br_ldt_FailureResponse( ) */

/*==========================================================================*
**
** myki_br_ldt_GetShiftRecordTypeIndex
**
** Description     :    Helper function to retrieve the index of the Record List that contains the Record type 'recordType'
**
**
**  Parameters      :
**      recordType                      [I]     Record Type to look for in the record list
**      pMYKI_OAShiftDataRecordList     [I]     The record list to query
**      recordListSize                  [I]
**
**  Returns         :
**      0-14                        index of the record type stored in the list
**      <0                          failed or no record stored in the list.
**
**  Notes           :
**
**==========================================================================*/
U8_t myki_br_ldt_GetShiftRecordTypeIndex( U8_t recordType, const MYKI_OAShiftDataRecordList_t *pMYKI_OAShiftDataRecordList , U8_t recordListSize )
{
    U8_t recordIndex;

    if( pMYKI_OAShiftDataRecordList == NULL )
    {
        CsErrx( "myki_br_ldt_GetShiftRecordTypeIndex : pMYKI_OAShiftDataRecordList is NULL" );
        return -1;
    }

    for( recordIndex = 0; recordIndex < recordListSize; recordIndex++ )
    {
        if( pMYKI_OAShiftDataRecordList->MYKI_ShiftDataRecord[recordIndex].RecordType == recordType )
        {
            return recordIndex;
        }
    }

    return -1;
}

/*==========================================================================*
**
** myki_br_ldt_OAppShiftUpdate
**
** Description     :    Used by BR_VIX_OSC_3_3 to perform OAppShiftDataUpdate
**                      OAppShiftDataControl.ActiveRecordCount - (4)
**                      OAppShiftDataRecord[n]                 - (9)
**                      DynamicData.ShiftData                  - (10)

**                      1a. If the shift data record (9) for the payment (record) type exists on the card and, if one or more data fields of the shift data record in buffer (10) and from the card (9) is/are different,
**                          Perform OAppShiftDataUpdate/None with the index of the existing shift data record (9) on the card to update the shift data record.
**
**                      1b. If the shift data record (9) for the payment (record) type does not exist on the card,
**                          Perform OAppShiftDataUpdate/None with the index of active record count (4) to add shift data record to card, and
**                          Increment the active record count (4) by one.
**
**  Parameters      :
**      pData               [I]     BR context data
**      pActiveRecordCount  [I/O]   ShiftData active record count / Increment by 1 if a new record has been added
**
**  Returns         :
**      0                           success
**      <0                          failed
**
**  Notes           :
**
**==========================================================================*/
int
myki_br_ldt_OAppShiftUpdate
( 
    MYKI_BR_ContextData_t           *pData, 
    U8_t                            recordIndex,
    MYKI_OAShiftDataControl_t       *pMYKI_OAShiftDataControl
)
{
    OAppShiftDataUpdate_t           request;
    MYKI_BR_ShiftData_t             *pShiftData = pData->DynamicData.pShiftData;
    MYKI_OAShiftDataRecordList_t    *pMYKI_OAShiftDataRecordList = NULL; 
    U8_t                            recordIndexInList = 0;
    U8_t                            nShiftDataRecords = 0;

    const U8_t                      RECORD_NOT_FOUND  = -1;

    if( pData == NULL )
    {
        CsErrx ( "myki_br_ldt_OAppShiftUpdate : pData is NULL" );
        return -1;
    }
    if( pMYKI_OAShiftDataControl == NULL )
    {
        CsErrx( "myki_br_ldt_OAppShiftUpdate : pMYKI_OAShiftDataControl is NULL" );
        return -1;
    }
    /* Get shift record data on the card*/
    nShiftDataRecords = pMYKI_OAShiftDataControl->ActiveRecordCount;
    pMYKI_OAShiftDataControl->ActiveRecordCount = MYKI_MAX_SHIFTDATA_RECORDS;

    if ( ( MYKI_CS_OAShiftDataRecordsGet( pMYKI_OAShiftDataControl->ActiveRecordCount, &pMYKI_OAShiftDataRecordList ) ) != MYKI_CS_OK )
    { 
        pMYKI_OAShiftDataControl->ActiveRecordCount = nShiftDataRecords;
        CsErrx( "myki_br_ldt_OAppShiftUpdate : MYKI_CS_OAShiftDataRecordsGet() failed" );
        return -1;
    }

    pMYKI_OAShiftDataControl->ActiveRecordCount = nShiftDataRecords;
    /* Example shift Records on the card
     * -------------------
     * | Tpurse Record   |       |
     * -------------------       |
     * | Cash Record     |       |  RECORD TYPES / PAYMENT METHODS
     * -------------------       | 
     * | Voucher Record  |       |
     * -------------------       |
     * | UNDEFINED RECORD|       v
     * -------------------
     */
    /* if the payment is defined */
    if ( pShiftData->shiftTotals[recordIndex].RecordType != PAYMENT_METHOD_UNDEFINED )
    {
        memset( &request, 0, sizeof(request) );      
        recordIndexInList = myki_br_ldt_GetShiftRecordTypeIndex ( recordIndex, pMYKI_OAShiftDataRecordList, nShiftDataRecords );
        /* If the shift data record (9) for the payment (record) type exists on the card */            
        if( recordIndexInList != RECORD_NOT_FOUND )
        {
            MYKI_OAShiftDataRecord_t *pMYKI_OAShiftDataRecord = &(pMYKI_OAShiftDataRecordList->MYKI_ShiftDataRecord[recordIndexInList]); 

            /* And, if one or more data fields of the shift data record in buffer (10) and from the card (9) is/are different.
               Perform OAppShiftDataUpdate/None with the index of the existing shift data record (9) on the card to update the shift data record. */
            //request.isRecordTypeSet = TRUE;
            //request.RecordType = pShiftData->shiftTotals[recordIndex].RecordType;
            
            request.index = recordIndexInList;
            if( pMYKI_OAShiftDataRecord->SalesCount != pShiftData->shiftTotals[recordIndex].SalesCount )
            {
                request.isSalesCountSet = TRUE;
                request.salesCount      = pShiftData->shiftTotals[recordIndex].SalesCount;
            }
            if( pMYKI_OAShiftDataRecord->SalesValue != pShiftData->shiftTotals[recordIndex].SalesValue)
            {
                request.isSalesValueSet = TRUE;
                request.salesValue      = pShiftData->shiftTotals[recordIndex].SalesValue;
            }
            if( pMYKI_OAShiftDataRecord->ReversalsCount != pShiftData->shiftTotals[recordIndex].ReversalsCount )
            {
                request.isReversalsCountSet = TRUE;
                request.reversalsCount      = pShiftData->shiftTotals[recordIndex].ReversalsCount;
            }
            if( pMYKI_OAShiftDataRecord->ReversalsValue != pShiftData->shiftTotals[recordIndex].ReversalsValue )
            {
                request.isReversalsValueSet = TRUE;
                request.reversalsValue      = pShiftData->shiftTotals[recordIndex].ReversalsValue;
            }                
        }
        /* If the shift data record (9) for the payment (record) type does not exist on the card 
           Perform OAppShiftDataUpdate/None with the index of active record count (4) to add shift data record to card */
        else
        {
            request.index               = nShiftDataRecords;
            request.isRecordTypeSet     = TRUE;
            request.recordType          = pShiftData->shiftTotals[recordIndex].RecordType;
            request.isSalesCountSet     = TRUE;
            request.salesCount          = pShiftData->shiftTotals[recordIndex].SalesCount;
            request.isSalesValueSet     = TRUE;
            request.salesValue          = pShiftData->shiftTotals[recordIndex].SalesValue;
            request.isReversalsCountSet = TRUE;
            request.reversalsCount      = pShiftData->shiftTotals[recordIndex].ReversalsCount;
            request.isReversalsValueSet = TRUE;
            request.reversalsValue      = pShiftData->shiftTotals[recordIndex].ReversalsValue;          
            (pMYKI_OAShiftDataControl->ActiveRecordCount)++; /* Increment the active record count (4) by one. */
        }
#ifdef LDT_SUPPORTED    
        /* Perform OAppShiftDataUpdate/None with the index of the existing shift data record (9) on the card to update/Add to the shift data record. */
        if (MYKI_LDT_OAppShiftDataUpdate(&request, &pData->InternalData.TransactionData) != LDT_SUCCESS)
        {
            CsErrx( "myki_br_ldt_OAppShiftUpdate() MYKI_LDT_OAppShiftDataUpdate() failed" );
            return -1;
        }
#endif
         
    }
    return 0;
}

/*==========================================================================*
**
** myki_br_ldt_OAppShiftControlUpdate
**
** Description     :    Used by BR_VIX_OSC_3_3 to perform OAppShiftDataUpdate
**                      OAppRoles.Role[n].Type (1)
**                      OAppRoles.Role[n].Profile (2)
**                      OAppShiftDataControl.Status (3)
**                      OAppShiftDataControl.ActiveRecordCount (4)
**                      OAppShiftDataControl.ShiftId (5)
**                      OAppShiftDataControl.ShiftSeqNo (6)
**                      OAppShiftDataControl.StartTime (7)
**                      OAppShiftDataControl.CloseTime (8)
**                      OAppShiftDataRecord[n] (9)
**                      DynamicData.ShiftData (10)
**                      InternalData.ShiftLog (11)
**
**                      Perform OAppShiftDataControlUpdate/None,
**                      a.  Shift data status (3) set to "closed",
**                      b.  Active shift data record count (4) set to number of shift data records updated,
**                      c.  Shift identification (4) set to value from shift data buffer (10),
**                      d.  Shift sequence number (5) set to value from shift data buffer (10), and
**                      e.  Shift start time (7) and close time (8) set to values from shift data buffer (10).
**
**  Parameters      :
**      pData               [I]     BR context data
**      activeRecordCount   [I]     ShiftData active record count
**
**  Returns         :
**      0                           success
**      <0                          failed
**
**  Notes           :
**            
**==========================================================================*/
int
myki_br_ldt_OAppShiftControlUpdate( MYKI_BR_ContextData_t *pData, U8_t activeRecordCount )
{
    OAppShiftDataControlUpdate_t     request;
    MYKI_BR_ShiftData_t              *pShiftData = pData->DynamicData.pShiftData;

    memset( &request, 0, sizeof(request) );
    request.isStatusSet = TRUE;
    request.status = pShiftData->shiftStatus;
    request.isActiveRecordCountSet = TRUE;
    request.activeRecordCount = activeRecordCount;
    request.isShiftIdSet = TRUE;
    request.shiftId = ( pShiftData->shiftId & 0xffff );
    request.isShiftSequenceNumberSet = TRUE;
    request.shiftSequenceNumber = ( ( pShiftData->shiftId >> 16 ) & 0xffff );
    request.isStartTimeSet = TRUE;
    request.startTime = pShiftData->shiftStartTime;
    request.isCloseTimeSet = TRUE;
    request.closeTime = pShiftData->shiftEndTime;
     
#ifdef LDT_SUPPORTED       
    /* Write back to the card */
    if ( MYKI_LDT_OAppShiftDataControlUpdate( &request, &pData->InternalData.TransactionData ) != LDT_SUCCESS)
    {
        CsErrx( "myki_br_ldt_OAppShiftUpdate() MYKI_LDT_OAppShiftDataUpdate() failed" );
        return -1;
    }
#endif
    return 0;
    
}

/*==========================================================================*
**
** myki_br_ldt_AddShiftUsageLogEntry
**
** Description     :    Used by BR_VIX_OSC_3_3 to Modify shift log
**                      DynamicData.ShiftData - (10)
**                      InternalData.ShiftLog - (11)
**
**                      Modify shift log (11) - done in MYKI_BR_InitialiseLdtContext
**                      a.  Shift data status (3) set to "closed",
**                      b.  Active shift data record count (4) set to number of shift data records updated,
**                      c.  Shift identification (4) set to value from shift data buffer (10),
**                      d.  Shift sequence number (5) set to value from shift data buffer (10), and
**                      e.  Shift start time (7) and close time (8) set to values from shift data buffer (10).
**
**  Parameters      :
**      pData               [I]     BR context data
**
**  Returns         :
**      0                           success
**      <0                          failed
**
**  Notes           :
**
**==========================================================================*/
int
myki_br_ldt_AddShiftDataLogEntry( MYKI_BR_ContextData_t *pData )
{
    DebugShowLdtAddShiftDataSummaryLog( BRLL_FIELD, &pData->InternalData.ShiftDataSummaryLog );

#ifdef LDT_SUPPORTED      
    if ( MYKI_LDT_OAppShiftSummaryLog( &pData->InternalData.ShiftDataSummaryLog ) != LDT_SUCCESS )
    {
        CsErrx( "myki_br_ldt_AddShiftDataLogEntry() MYKI_LDT_OAppShiftSummaryLog() failed" );
        return -1;
    }

#endif
    return 0;
    
}
/*----------------------------------------------------------------------------
** FUNCTION           : myki_br_CheckProvisionProductZoneCoverage
**
** DESCRIPTION        : Used by BR_LLSC_7_4 and BR_LLSC_7_5 to examive zone
**                      coverage of products on the card
**
**                      For the provisional product zone range (zone low to zone high)
**                      verify that one or more zones does not have a
**                      product coverage as follows:
**                      a.  An activated product (that is not the provisional product)
**                          covers each zone
**                      b.  There is currently no product of type epass active;
**                          and a product of type epass that is inactive epass exists that:
**                          i.  covers one or more of the zones that in the provisional zone range
**                          ii. where start date time(14) is before than the
**                              scan on date(14) of the product in use(1)
**
**                      Clarification from Brendon.
**                      There can only every be ONE active ePass at any one time
**                      There can be multiple inactive ePasses
**                      If we need to active one of multiple inactive ePasses,
**                      then the rules are unclear. Activate whatever is easiest.
**
**                      This partially explains 'b'. We cannot active a ePass
**                      if there is already an actived ePass.
**
**
** INPUTS             : pData           - Br context Data
**                      pMYKI_TAControl - Ref to MYKI_TAControl_t
**                      pProductInUse   - Ref to Product currently in use
**                                        There will be a Product in Use
**                      pPass           - Address of data item to store
**                                        first inactive epass considered
**
** RETURNS            : Result Code of type myki_br_CheckProvisionProductZoneCoverage_t
**                              CHECK_PROVISION_PRODUCTZONE_COVERAGE_ERROR
**                              CHECK_PROVISION_PRODUCTZONE_COVERAGE_PARTIAL
**                              CHECK_PROVISION_PRODUCTZONE_COVERAGE_FULL
**                              CHECK_PROVISION_PRODUCTZONE_COVERAGE_FULL_WITH_INACTIVE_EPASS
**                      pPass argument provides pPass dir entry index
**
----------------------------------------------------------------------------*/

myki_br_CheckProvisionProductZoneCoverage_t myki_br_CheckProvisionProductZoneCoverage( MYKI_BR_ContextData_t *pData, MYKI_TAControl_t *pMYKI_TAControl, MYKI_TAProduct_t *pProductInUse, int *pPass)
{

    // Local typedef for use within the following function
    typedef enum
    {
        ZONE_COVERAGE_NOT_COVERED       = 0x00,
        ZONE_COVERAGE_ACTIVE_PRODUCT    = 0x01,
        ZONE_COVERAGE_INACTIVE_EPASS    = 0x02,
        ZONE_COVERAGE_BY_SOMETHING      = 0x10,
    } ZoneCoverage_t;

    int                 zone;
    int                 firstDir;
    int                 haveActiveEpass = 0;
    ZoneCoverage_t      zoneCoverage;
    ProductIterator     iProduct;

    //
    //  Clear the zone map
    //      Reusing existing data structure - as it is large
    //      Will use
    //          'priority'  - as a ZoneCoverage_t indication
    //          'dirIndex'  - mark ePass Uasge
    //

    for ( zone = 0; zone < DIMOF( pData->InternalData.ZoneMap.zoneList ); zone++ )
    {
        pData->InternalData.ZoneMap.zoneList[ zone ].priority = ZONE_COVERAGE_NOT_COVERED;
        pData->InternalData.ZoneMap.zoneList[ zone ].dirIndex = 0;      // ePass Used

    }

    pData->InternalData.ZoneMap.zoneLow  = pProductInUse->ZoneLow;
    pData->InternalData.ZoneMap.zoneHigh = pProductInUse->ZoneHigh;

    if ( pPass )
    {
        *pPass = 0;
    }

    //
    //  For each product
    //      Mark zones covered by an active non-provisional product
    //      Detect active epass product - and terminate scan
    //      Mark zones covered by inactive epass products where the start date
    //      time is before scan on date of the product in use
    //
    myki_br_InitProductIterator(pMYKI_TAControl, &iProduct, PRODUCT_TYPE_UNKNOWN);
    while (myki_br_ProductIterate(&iProduct) > 0)
    {
        ZoneCoverage_t       want_map = ZONE_COVERAGE_NOT_COVERED;

        if ( iProduct.currentProduct == PRODUCT_TYPE_EPASS )
        {
            if ( iProduct.pDirectory->Status == TAPP_CONTROL_DIRECTORY_STATUS_ACTIVATED )
            {
                haveActiveEpass = 1;
                want_map = ZONE_COVERAGE_ACTIVE_PRODUCT | ZONE_COVERAGE_BY_SOMETHING;
            }
            else
            {
                //  InActive ePass
                //  Ensure the start date time is before scan on date of
                //  the product in use
                if ( iProduct.pProduct->StartDateTime < pProductInUse->StartDateTime)
                {
                    want_map = ZONE_COVERAGE_INACTIVE_EPASS | ZONE_COVERAGE_BY_SOMETHING;
                }
            }
        }

        //
        // Test for activated, but not the provisional product
        //
        else if (    (iProduct.pDirectory->Status == TAPP_CONTROL_DIRECTORY_STATUS_ACTIVATED)
            && !(iProduct.pProduct->ControlBitmap & TAPP_TPRODUCT_CONTROL_PROVISIONAL_BITMAP) )
        {
            want_map = ZONE_COVERAGE_ACTIVE_PRODUCT | ZONE_COVERAGE_BY_SOMETHING;
        }

        //
        //  If we have decided that we need to include the zone map of the
        //  product ...
        //
        if ( want_map != ZONE_COVERAGE_NOT_COVERED )
        {
            for ( zone = iProduct.pProduct->ZoneLow;  zone <= iProduct.pProduct->ZoneHigh; zone++ )
            {
                pData->InternalData.ZoneMap.zoneList[ zone ].priority |= want_map;

                if ( want_map & ZONE_COVERAGE_INACTIVE_EPASS )
                {
                    pData->InternalData.ZoneMap.zoneList[ zone ].dirIndex = iProduct.index;
                }
            }
        }
    }
    if (iProduct.error)
    {
        CsErrx( "myki_br_CheckProvisionProductZoneCoverage : Iteration Failure" );
        return CHECK_PROVISION_PRODUCTZONE_COVERAGE_ERROR;
    }


    //
    //  Detect coverage of the provisional zone range
    //  Calculate return code
    //  pPass argument provides pPass dir entry index
    //
    zoneCoverage = ZONE_COVERAGE_ACTIVE_PRODUCT | ZONE_COVERAGE_BY_SOMETHING;
    firstDir = 0;
    for ( zone = pProductInUse->ZoneLow;  zone <= pProductInUse->ZoneHigh; zone++ )
    {
        zoneCoverage &= pData->InternalData.ZoneMap.zoneList[ zone ].priority;

        if ( (pData->InternalData.ZoneMap.zoneList[ zone ].priority & ZONE_COVERAGE_INACTIVE_EPASS) && (firstDir == 0) )
        {
            firstDir = pData->InternalData.ZoneMap.zoneList[ zone ].dirIndex;
        }
    }

    //
    //  zoneCoverage is an 'and' over all the zone bits
    //
    //  If 'ZONE_COVERAGE_ACTIVE_PRODUCT' is still set, then we have full coverage by active products
    //  If 'ZONE_COVERAGE_BY_SOMETHING' is set, but not 'ZONE_COVERAGE_ACTIVE_PRODUCT', then we have full coverage considering
    //  inactive ePasses
    //
    if ( zoneCoverage & ZONE_COVERAGE_ACTIVE_PRODUCT )
    {
        //  Full coverage
        return CHECK_PROVISION_PRODUCTZONE_COVERAGE_FULL;
    }

    if ( zoneCoverage == ZONE_COVERAGE_BY_SOMETHING )
    {
        //  Full coverage if we consider inActive ePass(s)
        if ( haveActiveEpass )
        {
            //  Have an active ePass so we can't activate another
            //  Indicate that we have partial coverage
            return CHECK_PROVISION_PRODUCTZONE_COVERAGE_PARTIAL;
        }
        else
        {
            //  Can activate the ePass
            //  Return the index to the first inactive ePass we found
            if ( pPass )
            {
                *pPass = firstDir;
            }
            return CHECK_PROVISION_PRODUCTZONE_COVERAGE_FULL_WITH_INACTIVE_EPASS;
        }
    }

    //  Do not have full coverage, not matter how hard we try
    return CHECK_PROVISION_PRODUCTZONE_COVERAGE_PARTIAL;
}

//=============================================================================
//
//  Return TRUE if the card has an active ePass product. Usually needed before
//  looking for products we can use, as we can only use (by first activating)
//  an inactive ePass if we don't have an existing active ePass on the card.
//
//=============================================================================

int     myki_br_HasActiveEpass( MYKI_BR_ContextData_t *pData, MYKI_TAControl_t *pMYKI_TAControl )
{
    ProductIterator             iProduct;

    CsDbg( BRLL_RULE, "myki_br_HasActiveEpass : Searching for an active ePass" );

    myki_br_InitProductIterator( pMYKI_TAControl, &iProduct, PRODUCT_TYPE_UNKNOWN );
    while ( myki_br_ProductIterate( &iProduct ) > 0 )
    {
        if ( iProduct.currentProduct == PRODUCT_TYPE_EPASS )
        {
            if ( iProduct.pDirectory->Status == TAPP_CONTROL_DIRECTORY_STATUS_ACTIVATED )
            {
                CsDbg( BRLL_RULE, "myki_br_HasActiveEpass : Card has an active ePass" );
                return TRUE;
            }
        }
    }

    if ( iProduct.error )
    {
        CsErrx( "myki_br_HasActiveEpass : Iteration Failure, returning FALSE" );
        return FALSE;
    }

    CsDbg( BRLL_RULE, "myki_br_HasActiveEpass : Card does not have an active ePass" );
    return FALSE;
}

/*==========================================================================*
**
**  myki_br_GetScanOnLocation
**
**  Description     :
**      Retrieves scan-on location.
**
**  Parameters      :
**      pLocation           [O]     returned location
**      pProductInUse       [I]     product in use
**
**  Returns         :
**      0                           success
**      <0                          failed
**
**  Notes           :
**
**==========================================================================*/

int     myki_br_GetScanOnLocation(
    MYKI_CD_Locations_t        *pLocation,
    const MYKI_TAProduct_t     *pProductInUse )
{
    TransportMode_e             TransportMode   = TRANSPORT_MODE_UNKNOWN;
    MYKI_CD_Stations_t          Station;
    MYKI_CD_RouteStop_t        *pRouteStops     = NULL;

    if ( pLocation == NULL || pProductInUse == NULL )
    {
        CsErrx( "myki_br_GetScanOnLocation : invalid parameter(s)" );
        return -1;
    }   /*  end-of-if */

    switch ( TransportMode = myki_br_cd_GetTransportModeForProvider( pProductInUse->LastUsage.ProviderId ) )
    {
    case    TRANSPORT_MODE_RAIL:
        /*  Determines location based on station/entry point id */
        if ( myki_br_cd_GetStationDetails( pProductInUse->LastUsage.Location.EntryPointId, &Station, pLocation ) < 0 )
        {
            CsErrx( "myki_br_GetScanOnLocation : myki_br_cd_GetStationDetails(%d) failed",
                    pProductInUse->LastUsage.Location.EntryPointId );
            return -1;
        }   /*  end-of-if */
        break;

    case    TRANSPORT_MODE_BUS:
    case    TRANSPORT_MODE_TRAM:
        /*  Determines location based on route and stop ids */
        if ( MYKI_CD_getRouteStopsStructure( 
                    pProductInUse->LastUsage.Location.RouteId,
                    pProductInUse->LastUsage.Location.StopId,
                    pProductInUse->LastUsage.Location.StopId,
                    &pRouteStops ) == 0 )
        {
            CsErrx( "myki_br_GetScanOnLocation : MYKI_CD_getRouteStopsStructure(%d,%d) failed",
                    pProductInUse->LastUsage.Location.RouteId,
                    pProductInUse->LastUsage.Location.StopId );
            return -1;
        }   /*  end-of-if */

        if ( MYKI_CD_getLocationsStructure( pRouteStops[ 0 ].locationId, pLocation ) == FALSE )
        {
            CsErrx( "myki_br_GetScanOnLocation : MYKI_CD_getLocationsStructure(%d) failed",
                    pRouteStops->locationId );
            return -1;
        }   /*  end-of-if */
        break;

    default:
        CsErrx( "myki_br_GetScanOnLocation : myki_br_cd_GetTransportModeForProvider(%d) invalid transport mode (%d)",
                pProductInUse->LastUsage.ProviderId, TransportMode );
        return -1;
    }   /*  end-of-switch */

    return 0;
}   /*  myki_br_GetScanOnLocation( ) */

/*==========================================================================*
**
**  myki_br_CreateCardSnapshot
**
**  Description     :
**      Creates card snapshot.
**
**  Parameters      :
**      pData               [I/O]   BR context data
**
**  Returns         :
**      0                           success
**      <0                          failed
**
**  Notes           :
**
**==========================================================================*/

int     myki_br_CreateCardSnapshot( MYKI_BR_ContextData_t *pData )
{
    int                     nResult                 = -1;

    if ( pData != NULL )
    {
        if ( pData->InternalData.pCardSnapshotPathFormat != NULL )
        {
            FILE           *fout                    = NULL;
            char            cardSnapshotPath[ 256 ];

            memset( cardSnapshotPath, 0, sizeof( cardSnapshotPath ) );
            CsSnprintf( cardSnapshotPath, sizeof( cardSnapshotPath ) - 1, pData->InternalData.pCardSnapshotPathFormat,
                    ( pData->InternalData.cardSnapshotSequenceNumber + 1 ) );
            CsDbg( 1, "myki_br_CreateCardSnapshot : create '%s'", cardSnapshotPath );
            fout    = fopen( cardSnapshotPath, "wt" );
            if ( fout != NULL )
            {
                /*  Generates card image snapshot */
                nResult = MYKI_FS_SerialiseToFile( fout );
                fclose( fout );
                pData->InternalData.cardSnapshotSequenceNumber++;

                if ( nResult < 0 )
                {
                    CsDbg( BRLL_RULE, "myki_br_CreateCardSnapshot : MYKI_FS_SerialiseToFile() failed (%d)", nResult );
                }
            }
        }
        else
        {
            pData->InternalData.cardSnapshotSequenceNumber++;
            CsDbg( BRLL_RULE, "myki_br_CreateCardSnapshot : card snapshot disabled (%d)",
                    pData->InternalData.cardSnapshotSequenceNumber );
            nResult = 0;
        }
    }
    return  nResult >= 0 ? 0 : -1;
}   /*  myki_br_CreateCardSnapshot( ) */

/*==========================================================================*
**
**  myki_br_isAO
**
**  Description     :
**      Checks to see if a Authorised officer card is presented
**
**  Parameters      :
**      type                [I]        card type
**	    profile             [I]        card profile
**  Returns         :
**      TRUE                           is an AO card
**      FALSE                          is not an AO card
**
**  Notes           :
**      Roles are defined from a json file loaded in readerapp and populates StaticData in the BR Context
**      The following AO's will be defined as an OperatorType_AUTHORISED_OFFICER
**	    AO Bus  (Type 13, Profile 1) - active AO types
**	    AO Tram (Type 14, Profile 1)
**	    AO Rail (Type 15, Profile 1) 
**==========================================================================*/
int     myki_br_isAO(MYKI_BR_ContextData_t *pData, int type, int profile)
{
    // Ignore if type and profile not set
    if ( (type == 0) && (profile == 0) )
    {
        return FALSE;
    }

    unsigned int i;
    for( i=0; i < DIMOF(pData->StaticData.staffRoles) ; i++ )
    { 
        if( pData->StaticData.staffRoles[i].operatorType == OperatorType_AUTHORISED_OFFICER )
        {
            if( (pData->StaticData.staffRoles[i].profile == profile) && (pData->StaticData.staffRoles[i].type == type) )
            {
                return TRUE;
            }
        }
        CsDbg( BRLL_RULE, "myki_br_isAO : Found an OPERATOR = '%d'", pData->StaticData.staffRoles[i].operatorType );

    }            
    CsDbg( BRLL_RULE, "myki_br_isAO : Could not find AO of type = (%d), profile = (%d)", type, profile );
    return FALSE;
} 

/*==========================================================================*
**
**  myki_br_isOfRole
**
**  Description     :
**      Determines if presented operator card is of specified role.
**
**  Parameters      :
**      pData           [I]     business rule context data.
**      eRole           [I]     operator role to check for.
**
**  Returns         :
**      >0                      if operator card is of specified role
**      0                       if operator card is not of specified role
**      <0                      if failed reading operator card
**
**==========================================================================*/

int     myki_br_isOfRole( MYKI_BR_ContextData_t *pData, OperatorType_e eRole )
{
    MYKI_OAControl_t   *pMYKI_OAControl = NULL;
    MYKI_OARoles_t     *pMYKI_OARoles   = NULL;
    int                 nResult         = 0;
    int                 nMaxRoles       = MYKI_MAX_ROLES;
    int                 nIndex          = 0;

    if ( ( nResult = MYKI_CS_OAControlGet( &pMYKI_OAControl ) ) != MYKI_CS_OK )
    {
        CsErrx( "myki_br_isOfRole : MYKI_CS_OAControlGet() failed (%d)", nResult );
        return  -1;
    }
    if ( pMYKI_OAControl->RolesCount <= MYKI_MAX_ROLES )
    {
        nMaxRoles   = pMYKI_OAControl->RolesCount;
    }
    else
    {
        CsWarnx( "myki_br_isOfRole : OAppControl.RolesCount(%d) > Maximum(%d)",
                pMYKI_OAControl->RolesCount, MYKI_MAX_ROLES );
    }

    if ( ( nResult = MYKI_CS_OARolesGet( &pMYKI_OARoles ) ) != MYKI_CS_OK )
    {
        CsErrx( "myki_br_isOfRole : MYKI_CS_OARolesGet failed (%d)", nResult );
        return  -1;
    }

    nResult = 0 /* NOT_FOUND */;
    while ( nResult == 0 && nIndex < nMaxRoles )
    {
        int nType       = (int)pMYKI_OARoles->Role[ nIndex ].Type;
        int nProfile    = (int)pMYKI_OARoles->Role[ nIndex ].Profile;
        int i           = 0;

        if ( nType != 0 || nProfile != 0 )
        {
            while ( nResult == 0 && i < DIMOF( pData->StaticData.staffRoles ) )
            {
                if
                (
                    pData->StaticData.staffRoles[ i ].operatorType == eRole     &&
                    pData->StaticData.staffRoles[ i ].type         == nType     &&
                    pData->StaticData.staffRoles[ i ].profile      == nProfile
                )
                {
                    nResult = 1 /* FOUND */;
                }
                i++;
            }   /*  end-of-while */
        }   /*  end-of-if */
        nIndex++;
    }   /*  end-of-while */

    return  nResult;
}   /*  myki_br_isOfRole( ) */

/*==========================================================================*
**
**  myki_br_Bcd16ToInt
**
**  Description     :
**      Converts BCD16 to integer.
**
**  Parameters      :
**      value           [I]     BCD16 value
**
**  Returns         :
**      xxx                     converted integer value
**
**==========================================================================*/

int     myki_br_Bcd16ToInt( int value )
{
    int ival    = 0;
    int i       = 4;
    while ( i > 0 )
    {
        int nibble  = value;
        switch ( i )
        {
        case    4:  nibble    >>= 4;
        case    3:  nibble    >>= 4;
        case    2:  nibble    >>= 4;
        case    1:  nibble     &= 0xf;
        }
        if ( nibble >= 0xa )
        {
            CsErrx( "myki_br_Bcd16ToInt : Invalid BCD16 value ($%04X)", value );
            nibble  = 0;
        }
        ival    = ( ival * 10 ) + nibble;
        i--;
    }
    return  ival;
}   /*  myki_br_Bcd16ToInt( ) */

/*==========================================================================*
**
**  myki_br_IntToBcd16
**
**  Description     :
**      Converts integer to BCD16.
**
**  Parameters      :
**      value           [I]     integer value
**
**  Returns         :
**      xxx                     converted BDC16 value
**
**==========================================================================*/

int     myki_br_IntToBcd16( int value )
{
    int bcdval  = 0x0000;
    int i       = 4;

    if ( value > 9999 )
    {
        CsErrx( "myki_br_IntToBcd16 : Invalid integer value (%d)", value );
        return  0x0000;
    }

    while ( i > 0 )
    {
        int nibble  = value;
        switch ( i )
        {
        case    4:  nibble  = ( nibble / 1000 ) % 10;   break;
        case    3:  nibble  = ( nibble /  100 ) % 10;   break;
        case    2:  nibble  = ( nibble /   10 ) % 10;   break;
        case    1:  nibble  =   nibble          % 10;   break;
        }
        bcdval  = ( bcdval << 4 ) | nibble;
        i--;
    }
    return  bcdval;
}   /*  myki_br_IntToBcd16( ) */
