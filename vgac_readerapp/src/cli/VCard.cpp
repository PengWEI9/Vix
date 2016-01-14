/* -*- mode: c; tabs: 4; -*- */
/*=============================================================================
**
**    Vix Technology                   Licensed software
**    (C) 2014                         All rights reserved
**
**=============================================================================
**
**  Project/Product : MBU
**  Filename        : VCard.cpp
**  Author(s)       : ?
**
**  Description     :
**      Implements card's reader commands.
**
**  Function(s)     :
**      loadCard            [private]   load card image
**
**  Information     :
**   Compiler(s)    : C++
**   Target(s)      : Independent
**
**  Subversion      :
**      $Id: VCard.cpp 88945 2016-01-12 03:56:05Z atran $
**      $HeadURL: https://auperasvn01.aupera.erggroup.com/svn/DPG_SWBase/vgac_readerapp/trunk/src/cli/VCard.cpp $
**
**  History         :
**   Vers.  Date        Aut.  Type     Description
**   -----  ----------  ----  -------  ----------------------------------------
**    1.00  dd.mm.yy    ???   Create
**    1.01  22.12.14    ANT   Add      Added setting virtual SAM ID
**
**===========================================================================*/

#include <json/json.h>
#include "corebasetypes.h"
#include "cs.h"
#include "VCard.h"
#include "envcmd.h"
#include "cli.h"
#include "ipc.h"
#include "ct_desfire_virtual.h"
#include "myki_fs_serialise.h"
#include "myki_br.h"
#include "datec19.h"
#include "card_processing_thread.h"
#include "app_debug_levels.h"
#include "dataxfer.h"
#include "cardsnapshot.h"

#include <math.h>
#include <unistd.h>

#define TRANSIT_CARD "tsc"
#define OPERATOR_CARD "osc"
#define AUTOLOAD_ACTIVATED "Activated"
#define AUTOLOAD_INACTIVATED "Inactivated"

    /** Scans card image file for hexadecimal field */
#define SCAN_H32(pfile, field) \
    do { \
        char sInputLine[512];\
        int tmpInt;\
        /*read line*/\
        int Result = fscanf(pfile, " %511[^\r\n]", sInputLine);\
        if (Result<1) break;\
        /*skip full line comment*/\
        if (strchr("#", *sInputLine)) continue;\
        /*read field*/\
        Result = sscanf(sInputLine, #field " =0x%08X\n", &tmpInt);\
        field = tmpInt;\
        if (Result == 0) continue;\
        break;\
    } while(1);

bool isAutoloadEnabled(MYKI_Card_t *pCard);
void getProducts(MYKI_Card_t *pCard, char *cardInfo);
void getUsageLog(MYKI_Card_t *pCard, char *cardInfo);
void getLoadLog(MYKI_Card_t *pCard, char *cardInfo);

bool readCard(void /*MYKI_Card_t*/ *args)
{
    MYKI_Card_t* pCard = reinterpret_cast<MYKI_Card_t*>(args);
    bool ret = true;
    U8_t appList;

    //CsErrx("iscardpreset returned %s\n",MYKI_CS_IsCardPresent());
    CsDebug(APP_DEBUG_FLOW, (APP_DEBUG_FLOW, "readCard: About to read card"));

    if ( (ret = MYKI_CS_GetCardInfo(&(pCard->pCT_CardInfo))) < 0 )
    {
        CsErrx("readCard: MYKI_CS_GetCardInfo failed with return value %d", ret); 
        return false;
    }

    if ( (ret = MYKI_CS_OpenCard(MYKI_CS_OPEN_LIST_APPS, &appList)) < 0 )
    {
        CsErrx("readCard: MYKI_CS_OpenCard failed with return value %d", ret); 
        return false;
    }
        
    if ( (ret = MYKI_CS_CAIssuerGet(&(pCard->pCAIssuer))) != 0)
    {
        CsErrx("readCard: MYKI_CS_CAIssuerGet failed with return value %d", ret); 
        return false;
    }

    if ( (ret = MYKI_CS_CAControlGet(&(pCard->pCAControl))) != 0)
    {
        CsErrx("readCard: MYKI_CS_CAControlGet failed with return value %d", ret);
        return false;
    }

    if ( (ret = MYKI_CS_TAIssuerGet(&(pCard->pTAIssuer))) != 0)
    {
        CsErrx("readCard: MYKI_CS_TAIssuerGet failed with return value %d", ret);
        return false;
    }

    if ( (ret = MYKI_CS_TAControlGet(&(pCard->pTAControl))) != 0)
    {
        CsErrx("readCard: MYKI_CS_TAControlGet failed with return value %d", ret);
        return false;
    }

    if ( (ret = MYKI_CS_TACappingGet(&(pCard->pTACapping))) != 0)
    {
        CsErrx("readCard: MYKI_CS_TACappingGet failed with return value %d", ret);
        return false;
    }

    if (  (ret = MYKI_CS_TAUsageLogRecordsGet(pCard->pTAIssuer->UsageLogCount,&pCard->pTAUsageLogRecordList)) < 0)
    {
        CsErrx("readCard: MYKI_CS_TAUsageLogRecordsGet failed with return value %d", ret);
        return false;
    }else{
        CsWarnx("readCard: MYKI_CS_TAUsageLogRecordsGet returned value %d", ret);
    }

    if ( (ret = MYKI_CS_TALoadLogRecordsGet(pCard->pTAIssuer->LoadLogCount, &pCard->pTALoadLogRecordList)) < 0)
    {
        CsErrx("readCard: MYKI_CS_TALoadLogRecordsGet failed with return value %d", ret);
        return false;
    }else{
        CsWarnx("readCard: MYKI_CS_TALoadLogRecordsGet returned value %d", ret);
    }

    for (int i = 0; i < MYKI_MAX_CONTRACTS; i++)
    if ( (ret = MYKI_CS_TAProductGet(i, &pCard->pTAProduct[i])) != 0)
    {
        CsErrx("readCard: MYKI_CS_TAProductGet failed with return value %d", ret);
        return false;
    }
 
    if ( (ret = MYKI_CS_TAPurseControlGet(&pCard->pTAPurseControl)) < 0)
    {
        CsErrx("readCard: MYKI_CS_TAPurseControlGet failed with return value %d", ret);
        return false;
    }

    if ( (ret = MYKI_CS_TAPurseBalanceGet(&pCard->pTAPurseBalance)) < 0)
    {
        CsErrx("readCard: MYKI_CS_TAPurseBalanceGet failed with return value %d", ret);
        return false;
    }

    CsDebug(APP_DEBUG_FLOW, (APP_DEBUG_FLOW, "readCard: Finished reading card"));

    return true;
/*
   pCard->MYKI_TAUsageLogRecord;
   pCard->MYKI_TALoadLogRecord;

   pCard->MYKI_OAIssuer;
   pCard->MYKI_OAControl;
   pCard->MYKI_OAPIN;
   pCard->MYKI_OARoles;
   card->MYKI_OAShiftData;
   card->MYKI_OAShiftDataControl;
   card->MYKI_OAInspectorData;
   card->MYKI_OAShiftLog;
*/
}

typedef struct
{
    void* pFunctionArgs;
    bool (*pFunction)(void*);
    bool complete;
    bool functionResult;
    CsMutex_t lock;
    CsCond_t condition;
} CardCallback_t;

void CardCallback(void* args)
{
    CardCallback_t* pArgs = reinterpret_cast<CardCallback_t*>(args);
    bool ret = pArgs->pFunction(pArgs->pFunctionArgs);
    CsMutexLock(&pArgs->lock);
    pArgs->functionResult = ret;
    pArgs->complete = true;
    CsCondBcast(&pArgs->condition);
    CsMutexUnlock(&pArgs->lock);
}

void DoCardCallback(CardCallback_t& cardCallback, bool (*pFunction)(void*), void* pFunctionArgs, bool skipCardDetect)
{
    CsDebug(APP_DEBUG_DETAIL, (APP_DEBUG_DETAIL, "CLI waiting for callback at 0x%x . skipdetect=%s", int(pFunction), (skipCardDetect==true?"true":"false") ));
    CsMutexInit(&cardCallback.lock, CSSYNC_THREAD);
    CsCondInit(&cardCallback.condition, CSSYNC_THREAD);
    CsMutexLock(&cardCallback.lock);
    cardCallback.pFunction = pFunction;
    cardCallback.pFunctionArgs = pFunctionArgs;
    cardCallback.functionResult = false;
    cardCallback.complete = false;

    IPC_CardFunction_t message;
    message.hdr.type = IPC_CARD_FUNCTION;
    message.hdr.source = APP_TASK;
    message.fromProcessId = getpid();
    message.function = CardCallback;
    message.functionArgument = &cardCallback;
    message.skipCardDetect = (skipCardDetect ? 1 : 0);
    IpcSend(IpcGetID(APP_TASK), &message, sizeof(message));

    CsMutexUnlock(&cardCallback.lock);
}

void WaitForCallbackCompletion(CardCallback_t& cardCallback)
{
    CsMutexLock(&cardCallback.lock);
    while ( !cardCallback.complete )
        CsCondWait(&cardCallback.condition, &cardCallback.lock);
    CsMutexUnlock(&cardCallback.lock);
    CsCondDestroy(&cardCallback.condition);
    CsMutexDestroy(&cardCallback.lock);
    CsDebug(APP_DEBUG_DETAIL, (APP_DEBUG_DETAIL, "CLI callback complete 0x%x", int(cardCallback.pFunction)));
}


bool detectCard(void /*char* */ *args)
{
    U8_t appList;
    int cnt_cards;
    MYKI_BR_ContextData_t *pContextData = GetCardProcessingThreadContextData();
    char * cardType = reinterpret_cast<char*>(args);

    if (strcmp(cardType, TRANSIT_CARD) == 0)
    {
         pContextData->DynamicData.isTransit = true; 
    }
    else if (strcmp(cardType, OPERATOR_CARD) == 0)
    {
         pContextData->DynamicData.isOperator = true; 
    }
    else
    {
        CsErrx("detectCard: Wrong card type %s", cardType);
        return false;
    }

    cnt_cards = MYKI_CS_DetectCard(false) ;
    if (cnt_cards == 0 )
    {
        CsWarnx("detectCard: Card not present");
        return true; // Not a failure when cards are removed.

    }else if (cnt_cards <0)
    {
        CsErr("detectCard: CommsError.");
        return false;
    }

    if (MYKI_CS_OpenCard(MYKI_CS_OPEN_DEFAULT, &appList) < 0)
    {
        CsErrx("detectCard: Card can not be open");
        return false;
    }

    return true;
}

void sendCardNotifyMsg()
{
    /*
    CsDebug(APP_DEBUG_DETAIL, (APP_DEBUG_DETAIL, "About to notify main app of card"));
    IPC_Generic_t message;
    message.hdr.type = IPC_CARD_NOTIFY;
    message.hdr.source = APP_TASK;
    IpcSend(IpcGetID(APP_TASK), &message, sizeof(message));
    */
}

bool card_detect(int argc, const char *argv[], void *data )
//const char *cardType)
{
    CardCallback_t detectCallback;
    //DoCardCallback(detectCallback, detectCard, const_cast<char*>(cardType), true);
    DoCardCallback(detectCallback, detectCard, const_cast<char*>(argv[0]), true);
    sendCardNotifyMsg();
    WaitForCallbackCompletion(detectCallback);
        return detectCallback.functionResult;
}

bool closeCard(void /* NULL */ *args)
{
    return (MYKI_CS_CloseCard() == 0 ? true : false);
}

bool card_close()
{
    CardCallback_t closeCallback;
    DoCardCallback(closeCallback, closeCard, NULL, true);
    sendCardNotifyMsg();
    WaitForCallbackCompletion(closeCallback);
        return closeCallback.functionResult;
}

bool card_notify()
{
    polling_enable();
    /*  NOTE:   The sendCardNotifyMsg() function causes (VGAC) card
    **          processing thread to close card session prematurely.
    **          Hence, it fails processing card "instruction". This
    **          does not seem to affect the current flow and hence,
    **          is removed.
    sendCardNotifyMsg();
    */
    return true;
}

/*==========================================================================*
**
**  loadCard
**
**  Description     :
**      Loads card image.
**
**  Parameters      :
**      args                [I]     card image path
**
**  Returns         :
**      true                        success
**      false                       otherwise
**
**==========================================================================*/

bool loadCard( void /* char* */ *args )
{
    FILE                   *pFile           = NULL;
    bool                    ret             = false;
    char                   *fName           = reinterpret_cast<char*>( args );
    MYKI_BR_ContextData_t  *pData           = GetCardProcessingThreadContextData();
    U32_t                   LastChangeSamId = 0;
    U32_t                   deviceSamId     = ( pData != NULL ? pData->StaticData.deviceSamId : 0 );

    CsDebug( APP_DEBUG_FLOW, ( APP_DEBUG_FLOW, "About to load the data to card from file %s", fName ) );

    MYKI_CS_DiagTraceControl( MYKI_CS_DIAG_TRACE_ALL );

    pFile   = fopen( fName, "r" );
    if ( pFile == NULL )
    {
        CsErr( "loadCard: Failed to open file %s", fName );
        return ret;
    }

    /*  Sets loading card image TPurseControl.LastChangeSamId */
    SCAN_H32( pFile, LastChangeSamId );
    if ( fseek( pFile, 0, SEEK_SET ) != 0 )
    {
        CsErrx( "loadCard: Failed resetting input card image file" );
        fclose( pFile );
        return ret;
    }
    if ( LastChangeSamId != deviceSamId )
    {
        SetKSamId( LastChangeSamId );
    }

    ret = ( MYKI_FS_DeSerialiseFromFile( pFile ) == 0 ? true : false );

    /*  And restores SAMID */
    SetKSamId( deviceSamId );

    if ( !ret )
    {
        CsErrx( "loadCard: Failed to load Card from file %s", fName );
    }

    if ( fclose( pFile ) )
    {
        CsErr( "loadCard: Error closing file pointer to %s", fName );
    }

    return ret;
}   /*  loadCard( ) */

bool card_load(int argc, const char *argv[], void *data)
//const char *fName)
{
    const char* fName = argv[0];
    CardCallback_t loadCallback;
    DoCardCallback(loadCallback, loadCard, const_cast<char*>(fName), true);
    sendCardNotifyMsg();
    WaitForCallbackCompletion(loadCallback);
        return loadCallback.functionResult;
}

bool imageCard(void /* char* */ *args)
{
    bool ret = false;
    CT_VirtualDeviceParams_t VirtualDeviceParams;
    const char** argv= reinterpret_cast<const char**>(args);
    const char * dName = argv[0];
    const char * dUID = argv[1];
    int scan_cnt;

    MYKI_CS_DiagTraceControl(MYKI_CS_DIAG_TRACE_ALL);
    CsDebug(APP_DEBUG_DETAIL, (APP_DEBUG_DETAIL, "Setting Card image directory to %s", dName));
    /* Set virtual DESFire CT DetectCard response parameters... */
    VirtualDeviceParams.DetectCardResult = 1; /*1=card present, 0=timeout*/

    /* Set desfire virtual card image directory... */
    strcpy(VirtualDeviceParams.ImageFileDirectory, dName);
    CsDebug(APP_DEBUG_DETAIL, (APP_DEBUG_DETAIL, "Setting Card image UID to %s", dUID));

    unsigned int serialNumber[7] = {0};
    scan_cnt = sscanf(dUID, "%02X%02X%02X%02X%02X%02X%02X",
       &serialNumber[0] ,
       &serialNumber[1] ,
       &serialNumber[2] ,
       &serialNumber[3] ,
       &serialNumber[4] ,
       &serialNumber[5] ,
       &serialNumber[6] 
    );
    for (int i = 0; i < scan_cnt; ++i)
    {
        VirtualDeviceParams.CardInfo.SerialNumber[i] = serialNumber[i];
    }
    CsDebug(APP_DEBUG_DETAIL, (APP_DEBUG_DETAIL, "Card UID length %d", scan_cnt));
        /* Set the virtual desfire card 7 byte UID/SerialNumber */
        //VirtualDeviceParams.CardInfo.SerialNumber[0] = 0x01;
        //VirtualDeviceParams.CardInfo.SerialNumber[1] = 0x02;
        //VirtualDeviceParams.CardInfo.SerialNumber[2] = 0x03;
        //VirtualDeviceParams.CardInfo.SerialNumber[3] = 0x04;
        //VirtualDeviceParams.CardInfo.SerialNumber[4] = 0x05;
        //VirtualDeviceParams.CardInfo.SerialNumber[5] = 0x06;
        //VirtualDeviceParams.CardInfo.SerialNumber[6] = 0x07;
        VirtualDeviceParams.CardInfo.SerialLength = scan_cnt;

    /* Set virtual DESFire CT OpenCard response parameters... */
    VirtualDeviceParams.OpenCardResult = CT_OK;

    /* Set virtual DESFire 'device' parameters */
    VirtualDeviceParams.DetectCardPresentDelayMs    = 10;
    VirtualDeviceParams.OtherCommandResponseDelayMs = 10;
    VirtualDeviceParams.CheckCardPresentCountLimit  = 10;


    ret = CT_Ioctl( 0x500, CT_IOCTL_SET_VIRTUAL_DEVICE_PARAMS, &VirtualDeviceParams, NULL);

    if (ret != CT_OK)
    {
        CsErrx("imageCard: Failed to set virtual card image directory to %s: returned %d", dName, ret);
        return false;
    }

    return true;
}

    /**
     *  Processes 'card image' CLI command.
     *  @param  argc number of arguments.
     *  @param  argv argument values.
     *  @param  data not used.
     *  @return true if successful; false otherwise.
     */
bool
card_image( int argc, const char *argv[], void *data )
{
    #define card_image_IMAGE_PATH           0
    #define card_image_CARD_SERIAL_NUMBER   1

    CardCallback_t      imageCallback;

    if ( argc                                  != ( card_image_CARD_SERIAL_NUMBER + 1 ) ||
         argv[ card_image_IMAGE_PATH         ] == NULL ||
         argv[ card_image_CARD_SERIAL_NUMBER ] == NULL )
    {
        CsErrx( "card_image : invalid parameter(s)" );
        return  false;
    }

    /*  Disables card polling */
    polling_disable();

    CsDbg( APP_DEBUG_DETAIL, "card_image : About to set image directory to %s", argv[ card_image_IMAGE_PATH ] );
    CsDbg( APP_DEBUG_DETAIL, "card_image : About to set card serial to %s", argv[ card_image_CARD_SERIAL_NUMBER ] );

    DoCardCallback( imageCallback, imageCard, argv, true /* SKIP_CARD_DETECT */ );
    WaitForCallbackCompletion( imageCallback );

    return  imageCallback.functionResult;
}

bool writeCard(void /* char* */ *args)
{
    int ret = 0;
    FILE *pFile;
    char * fName = reinterpret_cast<char*>(args);

    CsDebug(APP_DEBUG_DETAIL, (APP_DEBUG_DETAIL, "About to Write the card to file %s", fName));

    pFile = fopen(fName, "w");

    if (pFile == NULL)
    {
        CsErr("writeCard: Failed to open file %s", fName);
        return ret;
    }

    ret = MYKI_FS_SerialiseToFile(pFile);

    CsDebug(APP_DEBUG_DETAIL, (APP_DEBUG_DETAIL, "MYKI_FS_SerialiseToFile returns result %d", ret));

    if (ret<0) // As per return spec for  MYKI_FS_SerialiseToFile
        CsErrx("writeCard: Failed to write Card to file error %d, File: %s", ret, fName);

    if(fclose(pFile))
    {
        CsErr("writeCard: Error closing file pointer to %s", fName);
    }

    return ret>=0;
}

bool card_write(int argc, const char *argv[], void *data)
//const char *fName)
{
    const char *fName = argv[0];
    CardCallback_t writeCallback;
    DoCardCallback(writeCallback, writeCard, const_cast<char*>(fName), true);
    sendCardNotifyMsg();
    WaitForCallbackCompletion(writeCallback);
        return writeCallback.functionResult;
}

char* card_enquiry()
{
    MYKI_Card_t card;
    MYKI_Card_t *pCard = &card;
    char * pCardInfo = (char*)CsMalloc(2000);

    char buf[1000];
    DateC19_t expiryDate;

    memset(&card, 0, sizeof(card));
    memset(pCardInfo, 0, sizeof(pCardInfo));
    CsDebug(APP_DEBUG_DETAIL, (APP_DEBUG_DETAIL, "About to read card"));


    CardCallback_t enquiryCallback;
    sendCardNotifyMsg();
    DoCardCallback(enquiryCallback, readCard, pCard, false);
    WaitForCallbackCompletion(enquiryCallback);

    if (!enquiryCallback.functionResult)
    {
        CsErrx("card_enquiry: readCard failed");
        sprintf(pCardInfo, "Unable to read Card\n");

        return pCardInfo;
    }

    CsDebug(APP_DEBUG_DETAIL, (APP_DEBUG_DETAIL, "Card is read."));

    char cardUID[32];
    if(pCard && pCard->pCT_CardInfo)
    {
        char* t = cardUID;
        for(int i=0; i < pCard->pCT_CardInfo->SerialLength ;i++)
        {
            t += sprintf(t,"%02X", pCard->pCT_CardInfo->SerialNumber[i] );
        }
        /*sprintf(cardUID,"%02X%02X%02X%02X%02X%02X%02X",
                pCard->pCT_CardInfo->SerialNumber[0],
                pCard->pCT_CardInfo->SerialNumber[1],
                pCard->pCT_CardInfo->SerialNumber[2],
                pCard->pCT_CardInfo->SerialNumber[3],
                pCard->pCT_CardInfo->SerialNumber[4],
                pCard->pCT_CardInfo->SerialNumber[5],
                pCard->pCT_CardInfo->SerialNumber[6]);
*/
    }

    sprintf(buf, "** CARD ENQUIRY (%s) ***********************************\n",cardUID);
    strcat(pCardInfo, buf);

    strcat(pCardInfo, "Card application\n");
    strcat(pCardInfo, "----------------\n");

    if( pCard && pCard->pCAIssuer )
    {
        sprintf(buf, " ISSUER: %-7d    CSN: %d\n", pCard->pCAIssuer->IssuerId, pCard->pCAIssuer->CSN);
        strcat(pCardInfo, buf);
    }else{
        sprintf(buf, " ISSUER: ?        CSN: ?\n" );
        strcat(pCardInfo, buf);
    }

    if( pCard && pCard->pCAControl )
    {
        sprintf(buf, " STATUS: %-10d       Expiry: %.24s\n", pCard->pCAControl->Status, cdate(&(pCard->pCAControl->ExpiryDate)));
        strcat(pCardInfo, buf);
    }else{
        sprintf(buf, " STATUS: ?        Expiry: ?\n");
        strcat(pCardInfo, buf);
    }

    strcat(pCardInfo, "\n------------------------------------------------------------------------------\n"); 
    strcat(pCardInfo, "\nTransit Application\n");
    strcat(pCardInfo, "-------------------\n");

    if( pCard && pCard->pTAControl )
    {
        sprintf(buf, " STATUS: %-10d    Expiry: %.24s      ProvisionalFare: %d\n", pCard->pTAControl->Status, cdate(&(pCard->pTAControl->ExpiryDate)), pCard->pTAControl->ProvisionalFare);
        strcat(pCardInfo, buf);
    }else{
        sprintf(buf, " STATUS: ?        Expiry: ?       ProvisionalFare: ?\n");
        strcat(pCardInfo, buf);
    }

    strcat(pCardInfo, "\n TPURSE:\n");

    if( pCard && pCard->pTAPurseBalance )
    {
        sprintf(buf, " BALANCE: $%5.02f  --", (pCard->pTAPurseBalance->Balance/100.0) );
        strcat(pCardInfo, buf);
    }else{
        sprintf(buf, " BALANCE: $%5s  --", "?");
        strcat(pCardInfo, buf);
    }
    if( pCard && pCard->pTAPurseControl )
    {
        sprintf(buf, "   STATUS: %s    ASN: %d\n", isAutoloadEnabled(pCard)==true?AUTOLOAD_ACTIVATED:AUTOLOAD_INACTIVATED, pCard->pTAPurseControl->ActionSeqNo);
        strcat(pCardInfo, buf);
    }else{
        sprintf(buf, "   STATUS: ?     ASN: ?\n");
        strcat(pCardInfo, buf);
    }

    if( pCard && pCard->pTAControl )
    {
        sprintf(buf, "\n PRODUCTS: (ProductInUse=%d)\n", pCard->pTAControl->ProductInUse);
        strcat(pCardInfo, buf);
    }else{
        sprintf(buf, "\n PRODUCTS: (ProductInUse=?)\n");
        strcat(pCardInfo, buf);
    }
    getProducts(pCard, pCardInfo);


    strcat(pCardInfo, "\n CAPPING:\n");
    if( pCard && pCard->pTACapping )
    {
        expiryDate = pCard->pTACapping->Daily.Expiry;
        sprintf(buf, " DAILY  EXP=%.24s  ZL=%d ZH=%d VAL=$%.02f \n", cdate(&expiryDate), pCard->pTACapping->Daily.Zone.Low, pCard->pTACapping->Daily.Zone.High, (pCard->pTACapping->Daily.Value/100.0));
        strcat(pCardInfo, buf);
        expiryDate = pCard->pTACapping->Weekly.Expiry;
        sprintf(buf, " WEEKLY EXP=%.24s  ZL=%d ZH=%d VAL=$%.02f \n", cdate(&expiryDate), pCard->pTACapping->Weekly.Zone[0].Low, pCard->pTACapping->Weekly.Zone[0].High, (pCard->pTACapping->Weekly.Value/100.0));
        strcat(pCardInfo, buf);
    }else{
        sprintf(buf, " DAILY  EXP=%24s  ZL=  ZH=  VAL=$ \n", "");
        strcat(pCardInfo, buf);
        sprintf(buf, " WEEKLY EXP=%24s  ZL=  ZH=  VAL=$ \n", "");
        strcat(pCardInfo, buf);
    }


    strcat(pCardInfo, "\n USAGE LOGS:\n");
    getUsageLog(pCard, pCardInfo);

    strcat(pCardInfo, "\n LOAD LOGS:\n");
    getLoadLog(pCard, pCardInfo);

    strcat(pCardInfo, "\n");
    return pCardInfo;
}

/*==========================================================================*
**
**  card_unconfirmed
**
**  Description     :
**      Processes "card unconfirmed" CLI command.
**
**  Parameters      :
**      argc                [I]     number of arguments
**      argv                [I]     argument values
**      data                [I]     not used
**
**  Returns         :
**      true                success
**      false               otherwise
**
**  Notes           :
**
**
**==========================================================================*/

bool card_unconfirmed( int argc, const char *argv[], void *data )
{
    unsigned char   WriteFlag   = ( unsigned char )( strcmp( argv[ 0 ], "true" ) == 0 ? TRUE : FALSE );

    MYKI_CS_TestHookSet( EDF_TESTHOOK_AMBIGUOUS_COMMIT, EDF_TESTHOOK_ACTION_ENABLE_ONCE, &WriteFlag, sizeof( WriteFlag ) );
    return true;
}   /*  card_unconfirmed( ) */

/*==========================================================================*
**
**  card_snapshot
**
**  Description     :
**      Processes "card snapshot" CLI command.
**
**  Parameters      :
**      argc                [I]     number of arguments
**      argv                [I]     argument values
**      data                [I]     not used
**
**  Returns         :
**      true                success
**      false               otherwise
**
**  Notes           :
**
**
**==========================================================================*/

bool card_snapshot( int argc, const char *argv[], void *data )
{
    int             bEnable     = FALSE;

    if ( argc == 0 )
    {
        bEnable = 1;
    }
    else
    {
        switch ( argv[ 0 ][ 0 ] )
        {
        case    'e':    case    'E':    /*  'Enable'    */
        case    't':    case    'T':    /*  'True'      */
        case    'y':    case    'Y':    /*  'Yes'       */
            bEnable = 1;
            break;

        default:
            bEnable = atoi( argv[ 0 ] );
            break;
        }
    }
    SetOneShotCardSnapshot( bEnable );

    return  true;
}   /*  card_snapshot( ) */

    /**
     *  @brief  Callback function to process 'card validatetransitcard'
     *          command.
     *  @param  pArgs not used.
     *  @return true if successful; false otherwise.
     */
static
bool
CbValidateTransitCard( void *pArgs )
{
    Json::Value             request;
    Json::FastWriter        fw;
    std::string             requestJson;
    char                    TerminalId[ 32 ]    = { 0 };
    MYKI_BR_ContextData_t  *pData               = GetCardProcessingThreadContextData( );

    CsSnprintf( TerminalId, sizeof( TerminalId ), "%d", pData->StaticData.deviceId );
    request[ "terminalid" ]                     = TerminalId;
    request[ "name"       ]                     = "validatetransitcard";
    requestJson                                 = fw.write( request );
    ungetMessage( requestJson );

    return  true;
}

    /**
     *  @brief  Processes 'card validatetransitcard' CLI command.
     *  This function formats and "sends" a JSON 'validatetransitcard' request.
     *  @return true if success; false otherwise.
     */
bool
card_validatetransitcard( )
{
    CardCallback_t  cb;

    /*  Registers call-back function for validating transit card */
    DoCardCallback( cb, CbValidateTransitCard, NULL, true /*SKIP_CARD_POLLING*/ );

    /*  Waits for call-back function to complete */
    WaitForCallbackCompletion( cb );

    return  cb.functionResult;
}

bool isAutoloadEnabled(MYKI_Card_t *pCard)
{
    return (pCard->pTAPurseControl->ControlBitmap & 64) == 0 ? false : true;
}

void getProducts(MYKI_Card_t *pCard, char *cardInfo)
{
    char buf[1000];
    char bufdate[30];
    char bufdate2[30];
    DateC19_t c19time;
    int i;

    if(!pCard || !cardInfo) return;
    if(!pCard->pTAControl ) return;
    
// nHour (PID=4) EX=16-09-2013 12:00 Z=1/4 SQ=3 SN=2 VAL=$8.54 COUNT=0 ---V- ASN=0 USG=16/09/2013 09:00:03 STATUS=Activated TripDir=Unknown

    for (i = 1; i < MYKI_DIRECTORY_MAX_LENGTH; i++)
    {
        if (pCard->pTAControl->Directory[i].IssuerId > 0)
        {
            //sprintf(buf, " %d:ISSUER: %d  PRODUCT: %d\n", i, pCard->pTAControl->Directory[i].IssuerId, pCard->pTAControl->Directory[i].ProductId) ;
            //strcat(cardInfo, buf);
            c19time = time2date((time_t*)&pCard->pTAProduct[i]->EndDateTime);  
            cdate_r(&c19time,bufdate) ;
            c19time = time2date((time_t*)&pCard->pTAProduct[i]->LastUsage.DateTime);  
            cdate_r(&c19time,bufdate2) ;
            sprintf(buf, " %d:%s (PID=%d) EX=%.24s Z=%d/%d SQ=%d SN=%d VAL=$%.02f COUNT=%d ---V- ASN=%d USG=%.24s STATUS=%d TripDir=%d\n"
                        , i
                        , "?????"
                        , pCard->pTAControl->Directory[i].ProductId
                        , bufdate
                        , pCard->pTAProduct[i]->ZoneLow
                        , pCard->pTAProduct[i]->ZoneHigh
                        , pCard->pTAProduct[i]->NextTxSeqNo
                        , pCard->pTAControl->Directory[i].SerialNo 
                        , (pCard->pTAProduct[i]->PurchaseValue /100.0) 
                        , pCard->pTAProduct[i]->InstanceCount
                        , pCard->pTAProduct[i]->ActionSeqNo
                        , bufdate2
                        , pCard->pTAControl->Directory[i].Status
                        , i
                    );
            strcat(cardInfo, buf);

        }else{
            sprintf(buf, " %d:not issued\n", i);
            strcat(cardInfo, buf);
        }
    }
}

void getUsageLog(MYKI_Card_t *pCard, char *cardInfo)
{
    char buf[255];
    char bufdate[30];
    DateC19_t c19time;

    if(!pCard || !cardInfo) return;
    if(!pCard->pTAIssuer ) return;
    if(!pCard->pTAUsageLogRecordList ) return;
    
 //   if (pCard->pTAIssuer->UsageLogCount == pCard->pTAUsageLogRecordList->NumberOfRecords )
  //  {
        //sprintf(buf, "UsageLogCount: %d Number of records:%d\n", pCard->pTAIssuer->UsageLogCount , pCard->pTAUsageLogRecordList->NumberOfRecords);
        //strcat(cardInfo, buf);
        for (int i = 0; i < pCard->pTAUsageLogRecordList->NumberOfRecords; i++)
        {
            c19time = time2date((time_t*)&pCard->pTAUsageLogRecordList->MYKI_TAUsageLogRecord[i].TxDateTime);  
            cdate_r(&c19time,bufdate) ;

            sprintf(buf, " [%d]  TxV=$%.02f BAL=$%.02f / Z=%d STATUS=%d / PID=%d SN=%d / L= S= E=%d SP=%d TxT=%d TxDT=%.24s\n"
                        , i
                        , (pCard->pTAUsageLogRecordList->MYKI_TAUsageLogRecord[i].TxValue /100.0)
                        , (pCard->pTAUsageLogRecordList->MYKI_TAUsageLogRecord[i].NewTPurseBalance /100.0)
                        , pCard->pTAUsageLogRecordList->MYKI_TAUsageLogRecord[i].Zone
                        , pCard->pTAUsageLogRecordList->MYKI_TAUsageLogRecord[i].ProductValidationStatus
                        , pCard->pTAUsageLogRecordList->MYKI_TAUsageLogRecord[i].ProductId
                        , pCard->pTAUsageLogRecordList->MYKI_TAUsageLogRecord[i].ProductSerialNo
                        , pCard->pTAUsageLogRecordList->MYKI_TAUsageLogRecord[i].Location.EntryPointId
                        , pCard->pTAUsageLogRecordList->MYKI_TAUsageLogRecord[i].ProviderId 
                        , pCard->pTAUsageLogRecordList->MYKI_TAUsageLogRecord[i].TxType
                        , bufdate) ;                                           
            strcat(cardInfo, buf);          

        }
/*
    }
    else
    {
        sprintf(buf, "UsageLogCount: %d  is not equal to TAUsageLogRecordList NumberOfRecords %d\n", pCard->pTAIssuer->UsageLogCount, pCard->pTAUsageLogRecordList->NumberOfRecords);
        strcat(cardInfo, buf);
    }
*/
}

void getLoadLog(MYKI_Card_t *pCard, char *cardInfo)
{
    char buf[255]; //maximum line length
    char bufdate[30];
    DateC19_t c19time;
    
    if(!pCard || !cardInfo) return;
    if(!pCard->pTAIssuer ) return;
    if(!pCard->pTALoadLogRecordList ) return;

    //if (pCard->pTAIssuer->LoadLogCount == pCard->pTALoadLogRecordList->NumberOfRecords )
    //{
        sprintf(buf, "LoadLogCount: %d \n", pCard->pTAIssuer->LoadLogCount);
        for (int i = 0; i < pCard->pTALoadLogRecordList->NumberOfRecords; i++)
        {
            c19time = time2date((time_t*)&pCard->pTALoadLogRecordList->MYKI_TALoadLogRecord[i].TxDateTime);  
            cdate_r(&c19time,bufdate) ;
            sprintf(buf, " [%d]  TxV=$%d BAL=$%d - / L= S= E=%d SP=%d TxT=%d TxDT=%.24s\n"
                        , i
                        , pCard->pTALoadLogRecordList->MYKI_TALoadLogRecord[i].TxValue 
                        , pCard->pTALoadLogRecordList->MYKI_TALoadLogRecord[i].NewTPurseBalance 
                        , pCard->pTALoadLogRecordList->MYKI_TALoadLogRecord[i].Location.EntryPointId 
                        , pCard->pTALoadLogRecordList->MYKI_TALoadLogRecord[i].ProviderId 
                        , pCard->pTALoadLogRecordList->MYKI_TALoadLogRecord[i].TxType
                        , bufdate) ;
            strcat(cardInfo, buf);
        }
    //}
    //else
    //{
        //sprintf(buf, "LoadLogCount: %d  is not equal to TALoadLogRecordList NumberOfRecords %d\n", pCard->pTAIssuer->LoadLogCount, pCard->pTALoadLogRecordList->NumberOfRecords);
        //strcat(cardInfo, buf);
    //}
}
