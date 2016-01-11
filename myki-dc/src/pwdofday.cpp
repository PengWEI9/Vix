/* -*- mode: c; tabs: 4; -*- */
/*=============================================================================
**
**    Vix Technology                   Licensed software
**    (C) 2014                         All rights reserved
**
**=============================================================================
**
**  Project/Product : MVU
**  Filename        : pwdofday.c
**  Author(s)       : An Tran
**
**  Description     :
**      Implements the Password-Of-The-Day algorithm
**
**  Member(s)       :
**      GeneratePasswordOfTheDay
**                              [public]    generates password-of-the-day
**      HastToPasswordChar      [private]   converts hash bits to password char
**
**  Information     :
**   Compiler(s)    : C++
**   Target(s)      : Independent
**
**
**  Subversion      :
**      $Id: $
**      $HeadURL: $
**
**  History         :
**   Vers.  Date        Aut.  Type     Description
**   -----  ----------  ----  -------  ----------------------------------------
**    1.00  14.04.15    ANT   Create
**
**===========================================================================*/

/*
 *      Options
 *      -------
 */

    /*  Enable(!=0)/Disable(=0) unit-test code */
//#define UT_PASSWORD_OF_THE_DAY              1

/*
 *      Includes
 *      --------
 */

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <time.h>
#include <cs.h>

/*
 *      External References
 *      -------------------
 */

    /*  LIBWEBSOCKETS/MD5.H */
extern  "C" {void    md5( unsigned char *input, int len, unsigned char output[ 16 ] );}

/*
 *      Local Constants and Macros
 *      --------------------------
 */

/*
 *      Local Function Prototypes
 *      -------------------------
 */

static  char    HashToPasswordChar( const unsigned char *pHash, int frBit1, int toBit1, int frBit2, int toBit2 );

/*
 *      Local Variables
 *      ---------------
 */

/*==========================================================================*
**
**  GeneratePasswordOfTheDay
**
**  Description     :
**      Generates the Password-Of-The-Day.
**
**  Parameters      :
**      pPassword           [O]     returned password-of-the-day
**      pStaffId            [I]     staff id (printable ASCII)
**      serviceProviderId   [I]     service provider id (1-65535)
**      staffRole           [I]     staff role (1-?)
**
**  Returns         :
**      NULL                        failed
**      Else                        generated password-of-the-day
**
**  Notes           :
**
**
**==========================================================================*/

char       *GeneratePasswordOfTheDay(
    char               *pPassword,
    const char         *pStaffId,
    int                 serviceProviderId,
    int                 staffRole )
{
    #define STAFF_ID_LEN_MAX        8
    #define SERVICE_PROVIDER_ID_MAX 65535

    int                 staffIdLen  = ( pStaffId != NULL ? strlen( pStaffId ) : 0 );
    CsTime_t            nowCsTime   = { 0 };
    time_t              nowTime     = 0;
    struct tm           nowTm       = { 0 };
    char                buf[ 64 ];
    unsigned char       hash[ ( 128 /*BITS*/ / 8 ) ];

    if ( staffIdLen ==0 || staffIdLen > STAFF_ID_LEN_MAX )
    {
        CsErrx( "GeneratePasswordOfTheDay : invalid staff-id '%s'", pStaffId == NULL ? "(null)" : pStaffId );
        return  NULL;
    }

    if ( serviceProviderId <= 0 || serviceProviderId > SERVICE_PROVIDER_ID_MAX )
    {
        CsErrx( "GeneratePasswordOfTheDay : invalid service-provider-id (%d)", serviceProviderId );
        return  NULL;
    }

    CsTime( &nowCsTime );
    nowTime = (time_t)nowCsTime.tm_secs;
    localtime_r( &nowTime, &nowTm );

    /*
     *  Formats text string:
     *  +---+---+-----//----+---+---+-----//----+---+-----//----+---+---------------+---+
     *  | a | 7 |  StaffId  | ^ | [ |    SPID   | $ | StaffRole | # |  DD/MM/YYYY   | @ |
     *  +---+---+-----//----+---+---+-----//----+---+-----//----+---+---------------+---+
     */
    memset( buf, 0, sizeof( buf ) );
    CsSnprintf( buf, sizeof( buf ) - 1, "a7%s^[%d$%d#%02d/%02d/%04d@",
            pStaffId,
            serviceProviderId,
            staffRole,
            nowTm.tm_mday, ( nowTm.tm_mon + 1 ), ( nowTm.tm_year + 1900 ) );

    /*  Calculates MD5 hash code */
    md5( (unsigned char *)buf, strlen( buf ), hash );

    /*  And derives password-of-the-day characters from MD5 hash bit ranges.
     *
     *                                              FrBit1      ToBit1      FrBit2      ToBit2
     *                                              ================================================*/
    pPassword[ 0 ]  = HashToPasswordChar( hash,     0,          2,          92,         94          );
    pPassword[ 1 ]  = HashToPasswordChar( hash,     101,        102,        9,          12          );
    pPassword[ 2 ]  = HashToPasswordChar( hash,     34,         39,         0,          -1          );
    pPassword[ 3 ]  = HashToPasswordChar( hash,     127,        127,        16,         20          );
    pPassword[ 4 ]  = HashToPasswordChar( hash,     108,        110,        113,        115         );
    pPassword[ 5 ]  = HashToPasswordChar( hash,     86,         87,         26,         29          );
    pPassword[ 6 ]  = HashToPasswordChar( hash,     59,         61,         56,         58          );
    pPassword[ 7 ]  = HashToPasswordChar( hash,     106,        109,        105,        106         );
    pPassword[ 8 ]  = 0;

    return  pPassword;
}   /* GeneratePasswordOfTheDay( ) */

/*==========================================================================*
**
**  HashToPasswordChar
**
**  Description     :
**      Converts hash bit ranges to password character.
**
**  Parameters      :
**      frBit1          [I]     bit range 1 - from bit
**      toBit1          [I]     bit range 1 - to bit (inclusive)
**      frBit2          [I]     bit range 2 - from bit
**      toBit2          [I]     bit range 2 - to bit (inclusive)
**
**  Returns         :
**      xxx                     password character
**
**  Notes           :
**      Bit 0 refers to the left-most bit of the hash and,
**      Bit 127 refers to the right-most bit of the hash.
**
**==========================================================================*/

static  char    HashToPasswordChar(
    const unsigned char    *pHash,
    int                     frBit1,
    int                     toBit1,
    int                     frBit2,
    int                     toBit2 )
{
    #define IS_SET( p, b )      ( (p)[ (b) / 8 ] & BITS[ (b) % 8 ] )
    static const unsigned char  BITS[ ] =
    {
        0x80,   0x40,   0x20,   0x10,
        0x08,   0x04,   0x02,   0x01
    };  /*  BITS[ ] */
    static const char          *pPasswordChars  = "DX0PZA7CFQ54GEVM2NKW9JB6RSQ1UL3HY8TX";   /*  MAGIC! */
    int                         nPasswordChars  = strlen( pPasswordChars );
    unsigned char               n               = 0;
    int                         bno             = 0;

    bno = frBit1;
    while ( bno <= toBit1 )
    {
        n     <<= 1;
        if ( IS_SET( pHash, bno ) )
        {
            n++;
        }
        bno++;
    }
    bno = frBit2;
    while ( bno <= toBit2 )
    {
        n     <<= 1;
        if ( IS_SET( pHash, bno ) )
        {
            n++;
        }
        bno++;
    }
    return  pPasswordChars[ n % nPasswordChars ];
}   /*  HashToPasswordChar( ) */

#if     defined( UT_PASSWORD_OF_THE_DAY )
/*==========================================================================*
**
**  main
**
**  Description     :
**      Password-of-the-day unit-test "C" main entry .
**
**  Parameters      :
**      argc            [I]     number of arguments
**      argv            [I]     argument values
**                              0:  application path
**                              1:  staff id
**                              2:  service provider id
**                              3:  staff role
**
**  Returns         :
**      0                       success
**      Else                    failed
**
**  Notes           :
**
**==========================================================================*/

int main( int argc, const char *argv[ ] )
{
    #define ARG_APPLICATION_NAME        0
    #define ARG_STAFF_ID                1
    #define ARG_SERVICE_PROVIDER_ID     2
    #define ARG_STAFF_ROLE              3
    #define NARGS                       4

    const char     *pStaffId            = NULL;
    int             serviceProviderId   = 0;
    int             staffRole           = 0;
    char            passwdOfTheDay[ 16 ];

    if ( argc != NARGS )
    {
        fprintf( stderr, "Invalid argument(s)\n" );
        fprintf( stderr, "usage:  %s staff-id service-provider-id staff-role\n\n", CsBasename( argv[ ARG_APPLICATION_NAME ] ) );
        fprintf( stderr, "        staff-id            alpha-numeric staff id\n" );
        fprintf( stderr, "        service-provider-id numeric service provider id\n" );
        fprintf( stderr, "        staff-role          numeric staff role\n\n" );
        return  1;
    }

    pStaffId            =       argv[ ARG_STAFF_ID              ];
    serviceProviderId   = atoi( argv[ ARG_SERVICE_PROVIDER_ID   ] );
    staffRole           = atoi( argv[ ARG_STAFF_ROLE            ] );

    if ( GeneratePasswordOfTheDay( passwdOfTheDay, pStaffId, serviceProviderId, staffRole ) == NULL )
    {
        fprintf( stderr, "Failed generating password-of-the-day\n" );
        return  2;
    }

    fprintf( stdout, "password-of-the-day is '%s'\n", passwdOfTheDay );

    return  0;
}
#endif  /*  UT_PASSWORD_OF_THE_DAY */
