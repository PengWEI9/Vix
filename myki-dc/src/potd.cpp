/* -*- mode: c; tabs: 4; -*- */
/*=============================================================================
**
**    Vix Technology                   Licensed software
**    (C) 2014                         All rights reserved
**
**=============================================================================
**
**  Project/Product : MVU
**  Filename        : potd.cpp
**  Author(s)       : An Tran
**
**  Description     :
**      potd test utility
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

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <time.h>
#include <cs.h>

char* GeneratePasswordOfTheDay(char *pPassword, const char *pStaffId, int serviceProviderId, int staffRole);


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

int main( int argc, char *argv[ ] )
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
        fprintf( stderr, "Error: Failed generating password-of-the-day\n" );
        return  2;
    }

    fprintf( stdout, "%s\n", passwdOfTheDay ); 

    return  0;
}
