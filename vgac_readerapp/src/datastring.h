/* -*- mode: c; tabs: 4; -*- */
/*=============================================================================
**
**    Vix Technology                   Licensed software
**    (C) 2015                         All rights reserved
**
**=============================================================================
**
**  Project/Product : MVU
**  Filename        : datastring.h
**  Author(s)       : An Tran
**
**  Description     :
*/
/**     @file
**      @brief  Declares function prototypes to convert card data to
**              printable text.
*/
/*  Member(s)       :
**
**
**  Information     :
**   Compiler(s)    : C++
**   Target(s)      : Independent
**
**  Subversion      :
**      $Id: datastring.h 87801 2015-12-30 03:21:31Z atran $
**      $HeadURL: https://auperasvn01.aupera.erggroup.com/svn/DPG_SWBase/vgac_readerapp/trunk/src/datastring.h $
**
**  History         :
**   Vers.  Date        Aut.  Type     Description
**   -----  ----------  ----  -------  ----------------------------------------
**    1.00  31.08.15    ANT   Create
**
**===========================================================================*/

#if     !defined( __DATASTRING_H_INCLUDED )
#define __DATASTRING_H_INCLUDED         1

/*
 *      Includes
 *      --------
 */

#include <corebasetypes.h>
#include <cs.h>
#include <datec19.h>
#include <myki_cd.h>

/*
 *      Prototypes
 *      ----------
 */

std::string U8ToString( U8_t value );
std::string U16ToString( U16_t value );
std::string MoneyS32ToString( S32_t value );
std::string MoneyU32ToString( U32_t value );
std::string DateC19ToString( DateC19_t date );
std::string DateC19ToStringWithEndOfTransport( DateC19_t date );
std::string getMykiCardNumber( );
std::string getMykiPAN( );
std::string getMykiMoneyBalance( );
std::string getAutoloadThreshold( );
std::string getAutoloadEnabled( );
std::string getAutoloadAmount( );
std::string getTransitProvisionalFare( );
std::string getCardExpiryDate( );
std::string getTransitExpiryDate( );
std::string getTransitPassengerCodeExpiryDate( );
std::string getIssuerId( );
std::string getCardApplicationStatus( );
std::string getBlockingReason( U8_t blockingReason );
std::string getBlockingReasonCode( U8_t blockingReason );
std::string getCardApplicationBlockingReason( );
std::string getTransitApplicationStatus( );
std::string getTransitApplicationRegistrationType( );
std::string getTransitApplicationBlockingReason( );
std::string getSerialNumber( );
std::string getSurcharge( );
std::string getTouchedOn( );
std::string getOutstandingDefaultFare( );
std::string getTransitPassengerCode( );
std::string getTransitVersion( );
std::string getDailyCappingExpiry( );
std::string getDailyCappingZones( );
std::string getDailyCappingValue( );
std::string getWeeklyCappingExpiry( );
std::string getWeeklyCappingZones( int index = 0 );
std::string getWeeklyCappingValue( );
std::string TimeToString( Time_t time );
std::string getLocation( MYKI_Location_t &Location );
std::string getProductType( U16_t productId, U16_t instanceCount );
std::string getProviderMode( U16_t providerId );
std::string getProviderName( U16_t providerId );
std::string getProductStatus( U8_t status );
std::string getPaymentMethod( U8_t PaymentMethod );
std::string getTxTypeSigning( U8_t txType );

int         myki_gac_GetCardProduct( unsigned int dirIndex, MYKI_Directory_t **pDirectory, MYKI_TAProduct_t **pProduct );
U8_t        LogicalZoneToPhysicalZone( U8_t value );
void        DateC19ToDateYMDHHMM( DateC19_t dateC19, int *pYear, int *pMonth, int *pMday, int *pHour, int *pMinute );
int32_t     CsStrTimeISO( const CsTime_t *pTm, int32_t len, char *pBuffer );

#endif  /*  !__DATASTRING_H_INCLUDED */
