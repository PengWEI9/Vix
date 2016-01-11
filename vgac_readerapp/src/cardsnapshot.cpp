/* -*- mode: c; tabs: 4; -*- */
/*=============================================================================
**
**    Vix Technology                   Licensed software
**    (C) 2014                         All rights reserved
**
**=============================================================================
**
**  Project/Product : NGBU
**  Filename        : cardsnapshot.cpp
**  Author(s)       : An Tran
**
**  Description     :
*/
/**     @brief      Implements functions to create card snapshots.
**
*/
/*  Member(s)       :
**      initCardSnapShot        [public]    initialise card snapshot processing
**      CommitCardSnapshots     [public]    commit card snapshots
**      SetOneShotCardSnapshot  [public]    enable/disable one-shot card snapshot
**      StartCardSnapshot       [public]    start card snapshot processing
**      EndCardSnapshot         [public]    end card snapshot processing
**
**  Information     :
**   Compiler(s)    : C++
**   Target(s)      : Independent
**
**
**  Subversion      :
**      $Id: cardsnapshot.cpp 88451 2016-01-07 00:54:31Z atran $
**      $HeadURL: https://auperasvn01.aupera.erggroup.com/svn/DPG_SWBase/vgac_readerapp/trunk/src/cardsnapshot.cpp $
**
**  History         :
**   Vers.  Date        Aut.  Type     Description
**   -----  ----------  ----  -------  ----------------------------------------
**    1.00  13.11.15    ANT   Create
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

#include <json/json.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <cs.h>
#include <csf.h>
#include <myki_fs_serialise.h>
#include <myki_br.h>

#include "cardsnapshot.h"
#include "card_processing_thread.h"         /*  SetOneshotCardsnapshot */

/*
 *      Local Constants and Macros
 *      --------------------------
 */

#define DEFAULT_CardSnapshotPending         "/afc/data/images/pending/"             /**< Default pending/working card image snapshot folder */
#define DEFAULT_CardSnapshotConfirmed       "/afc/data/images/confirmed/"           /**< Default confirmed card image snapshot folder */
#define MAX_DUMP_FILENAME                   255

/*
 *      Local Variables
 *      ---------------
 */

static int          g_iniCardSnapshotTxnOption                          = 0;        /**< Pre/Post transaction card image snapshot option */
static int          g_iniCardSnapshotBREnabled                          = 0;        /**< BR_LLSC_0_1 card image snapshot enable/disable */
static char         g_iniCardSnapshotPending[   CSFINI_MAXVALUE + 1 ]   = { 0 };    /**< Pending/Working card image snapshot folder */
static char         g_iniCardSnapshotConfirmed[ CSFINI_MAXVALUE + 1 ]   = { 0 };    /**< Confirmed card image snapshot folder */
static int          g_oneShotCardSnapshot                               = 0;        /**< CLI one-shot card image snapshot */

static  char       *g_pCardSnapshotPathFormat                           = NULL;
static  char        g_cardSnapshotPathFormat[ MAX_DUMP_FILENAME ]       = { 0 };
static  char        g_cardSnapshotPath[ MAX_DUMP_FILENAME ]             = { 0 };

/*
 *      Local Prototypes
 *      ----------------
 */

static  int         TakeCardSnapshot( const char *pFilename );

    /**
     *  @brief  Initialises generating card snapshot.
     *  @return 0 if successful; otherwise failed.
     */
int
initCardSnapShot( void )
{
    char            iniBuf[ CSFINI_MAXVALUE + 1 ];

    #define SetNumericConfig(CFG, VALUE, DEFAULT) \
    do { \
        if ( CsfIniExpand(CFG, iniBuf, sizeof(iniBuf)) == 0 ) \
        { \
            VALUE = atoi(iniBuf); \
        } \
        else \
        { \
            VALUE = DEFAULT; \
            CsWarnx("Could not read %s, setting \"%s\" to the default value of %d", CFG, #VALUE, DEFAULT); \
        } \
    } while (0)
    SetNumericConfig( "MykiApp General:CardSnapshotTxnOption",  g_iniCardSnapshotTxnOption, 0   );
    #undef  SetNumericConfig

    strcpy( g_iniCardSnapshotPending, DEFAULT_CardSnapshotPending );
    if ( CsfIniExpand( "MykiApp General:CardSnapshotPending", g_iniCardSnapshotPending, CSFINI_MAXVALUE ) == 0 )
    {
    }
    if ( CsMkdir( g_iniCardSnapshotPending, 0777 ) < 0 )
    {
        CsErrx( "initCardSnapShot : CsMkdir('%s') failed", g_iniCardSnapshotPending );

        /*  Disables creating card image snapshots */
        g_iniCardSnapshotTxnOption  = 0;
        g_iniCardSnapshotBREnabled  = 0;
    }

    strcpy( g_iniCardSnapshotConfirmed, DEFAULT_CardSnapshotConfirmed );
    if ( CsfIniExpand( "MykiApp General:CardSnapshotConfirmed", g_iniCardSnapshotConfirmed, CSFINI_MAXVALUE ) == 0 )
    {
    }
    if ( CsMkdir( g_iniCardSnapshotConfirmed, 0777 ) < 0 )
    {
        CsErrx( "initCardSnapShot : CsMkdir('%s') failed", g_iniCardSnapshotConfirmed );

        /*  Disables creating card image snapshots */
        g_iniCardSnapshotTxnOption  = 0;
        g_iniCardSnapshotBREnabled  = 0;
    }

    return  0;
}

    /**
     *  @brief  Commits generated card images (if any).
     *  @return 0 if successful; otherwise failed.
     */
int
CommitCardSnapshots( int mode )
{
    CsGlob_t            csGlob;
    int                 i                   = 0;
    int                 nResult             = 0;
    char                suffix[ 16 ]        = { 0 };
    char                cardSnapshotPendingPath[ MAX_DUMP_FILENAME ];
    char                cardSnapshotPendingConfirmed[ MAX_DUMP_FILENAME ];

    if ( CsGlob( g_iniCardSnapshotPending, &csGlob, "*.*" ) == 0 )
    {
        switch ( mode )
        {
        case    CARD_SNAPSHOT_ERROR:
        case    CARD_SNAPSHOT_UNCONFIRMED:
            strcpy( suffix, mode == CARD_SNAPSHOT_UNCONFIRMED ? ".unconfirmed" : ".error" );
            /*  DROP THROUGH TO NEXT CASE! */

        case    CARD_SNAPSHOT_CONFIRMED:
            /*  Moves all pending/working card image snapshots to CardSnapshotConfirmed folder */
            for ( i = 0; i < csGlob.gl_argc; ++i )
            {
                /*  From path */
                memset( cardSnapshotPendingPath, 0, sizeof( cardSnapshotPendingPath ) );
                CsSnprintf( cardSnapshotPendingPath, sizeof( cardSnapshotPendingPath ) - 1, "%s%s",
                        g_iniCardSnapshotPending, csGlob.gl_argv[ i ] );

                /*  To path */
                memset( cardSnapshotPendingConfirmed, 0, sizeof( cardSnapshotPendingConfirmed ) );
                CsSnprintf( cardSnapshotPendingConfirmed, sizeof( cardSnapshotPendingConfirmed ) - 1, "%s%s%s",
                        g_iniCardSnapshotConfirmed, csGlob.gl_argv[ i ], suffix );

                if ( ( nResult = CsFileRename( cardSnapshotPendingPath, cardSnapshotPendingConfirmed ) ) < 0 )
                {
                    CsErrx( "CommitCardSnapshots : CsFileRename('%s','%s') failed(%d)",
                            cardSnapshotPendingPath, cardSnapshotPendingConfirmed, nResult );
                    (void)unlink( cardSnapshotPendingPath );
                }
            }
            break;

        case    CARD_SNAPSHOT_REMOVE:
            /*  Removes all pending/working card image snapshots */
            for ( i = 0; i < csGlob.gl_argc; ++i )
            {
                memset( cardSnapshotPendingPath, 0, sizeof( cardSnapshotPendingPath ) );
                CsSnprintf( cardSnapshotPendingPath, sizeof( cardSnapshotPendingPath ) - 1, "%s%s",
                        g_iniCardSnapshotPending, csGlob.gl_argv[ i ] );
                (void)unlink( cardSnapshotPendingPath );
            }
            break;

        default:
            break;
        }

        CsGlobFree( &csGlob );
    }

    return  0;
}

    /**
     *  @brief  Enables/Disables one-shot card snapshot.
     *          This function enables generating card snapshots pre/post business
     *          rule execution.
     *  @param  bEnable 0=disabled; else number of consecutive card snapshot sets.
     */
void
SetOneShotCardSnapshot( int bEnable )
{
    g_oneShotCardSnapshot   = bEnable;
}

    /**
     *  @brief  Pre business rule card snapshot processing.
     *  @param  pData business rule context data.
     *  @return 0 if successful; otherwise failed.
     */
int
StartCardSnapshot( MYKI_BR_ContextData_t *pData, const char *pCsn )
{
    int                 i               = 0;
    CT_CardInfo_t      *pCT_CardInfo    = NULL;
    char                csn[ ( CT_CARDINFO_SERIAL_LENGTH * 2 ) + 1 ];

    if ( pCsn == NULL )
    {
        memset( csn, 0, sizeof( csn ) );
        if ( MYKI_CS_GetCardInfo( &pCT_CardInfo ) == MYKI_CS_OK )
        {
            for ( i = 0; i < pCT_CardInfo->SerialLength; i++ )
            {
                CsSnprintf( &csn[ i * 2 ], ( 2 + 1 ), "%02X", pCT_CardInfo->SerialNumber[ i ] );
            }
            pCsn    = csn;
        }
    }

    g_pCardSnapshotPathFormat   = NULL;
    memset( g_cardSnapshotPathFormat, 0, sizeof( g_cardSnapshotPathFormat ) );

    if
    (
        pCsn != NULL &&
        (
            g_iniCardSnapshotTxnOption != 0 ||
            g_iniCardSnapshotBREnabled != 0 ||
            g_oneShotCardSnapshot      != 0
        )
    )
    {
        time_t          nowTimeT    = (time_t)pData->DynamicData.currentDateTime;
        struct tm       ctm;
        struct tm      *pTm         = localtime_r( &nowTimeT, &ctm );

        /*  Generates card image snapshot path name format, ie. $(CardSnapshotPending)/NNNNNNNN_YYYYMMDD_HHMMSS_NN%02d.ini */
        if ( pTm != NULL )
        {
            CsSnprintf( g_cardSnapshotPathFormat, sizeof( g_cardSnapshotPathFormat ) - 1, "%s%s_%04d%02d%02d_%02d%02d%02d_%02d%%02d.ini",
                    g_iniCardSnapshotPending,
                    pCsn,
                    ( pTm->tm_year + 1900 ), ( pTm->tm_mon + 1 ), pTm->tm_mday,
                    pTm->tm_hour, pTm->tm_min, pTm->tm_sec,
                    g_oneShotCardSnapshot );
            g_pCardSnapshotPathFormat   = g_cardSnapshotPathFormat;
        }
    }
    pData->InternalData.pCardSnapshotPathFormat =
            g_iniCardSnapshotBREnabled != 0 || g_oneShotCardSnapshot != 0 ?
                g_pCardSnapshotPathFormat :
                NULL;

    if
    (
        g_pCardSnapshotPathFormat != NULL &&
        ( ( g_iniCardSnapshotTxnOption & CARD_SNAPSHOT_PRE_TRANSACTION ) != 0 || g_oneShotCardSnapshot != 0 )
    )
    {
        /*  Saves "input" card image */
        memset( g_cardSnapshotPath, 0, sizeof( g_cardSnapshotPath ) );
        CsSnprintf( g_cardSnapshotPath, sizeof( g_cardSnapshotPath ) - 1, g_pCardSnapshotPathFormat, 0 /* PRE-TRANSACTION */ );
        TakeCardSnapshot( g_cardSnapshotPath );
    }

    return  0;
}

    /**
     *  @brief  Post business rule card snapshot processing.
     *  @param  pData business rule context data.
     *  @return 0 if successful; otherwise failed.
     */
int 
EndCardSnapshot( MYKI_BR_ContextData_t *pData )
{
    if
    (
        g_pCardSnapshotPathFormat != NULL &&
        ( ( g_iniCardSnapshotTxnOption & CARD_SNAPSHOT_POST_TRANSACTION ) != 0 || g_oneShotCardSnapshot != 0 )
    )
    {
        /*  Saves "ouput" card image */
        memset( g_cardSnapshotPath, 0, sizeof( g_cardSnapshotPath ) );
        CsSnprintf( g_cardSnapshotPath, sizeof( g_cardSnapshotPath ) - 1, g_pCardSnapshotPathFormat, 99 /* POST-TRANSACTION */ );
        TakeCardSnapshot( g_cardSnapshotPath );

    }
    g_oneShotCardSnapshot   = g_oneShotCardSnapshot > 0 ? ( g_oneShotCardSnapshot - 1 ) : 0;

    return  0;
}

    /**
     *  @brief  Generates card image.
     *  @param  pPath card image file path.
     */
static
int
TakeCardSnapshot( const char *pPath )
{
    FILE       *fout    = fopen( pPath, "wt" );

    if ( fout != NULL )
    {
        MYKI_FS_SerialiseToFile( fout );

        fclose( fout );
    }

    return  0;
}
