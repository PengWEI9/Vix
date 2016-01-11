#include <cs.h>
#include <csf.h>
#include <cstdlib>
#include <cstring>
#include <ipc.h>
#include <message_base.h>
#include <message_dc.h>
#include <string>
#include <fstream>
//#include <jsonParser.h>

#include <iostream>
using namespace std;
typedef enum
{	
    IPC_TOPUP = 1,
    IPC_TOPUP_INV_THRESHOLD_WARNING = 10,              // Total above Invoice threshold
    IPC_TOPUP_REV = 2,
    IPC_DRIVER_REPORT = 3,	// First / Single portion
    IPC_DRIVER_PORTION_REPORT = 4, 	// First / Single portion
    IPC_DRIVER_MANUAL_REPORT = 5,
    IPC_NONTRANSIT=6,
    IPC_NONTRANSIT_SURCHARGE=7,
    IPC_NONTRANSIT_REVERSAL=8,
    IPC_DRIVER_REPORT_BROUGHT_FORWARD = 30,
    IPC_DRIVER_REPORT_OUT_OF_BALANCE = 31,
    IPC_DRIVER_REPORT_BROUGHT_FORWARD_0 = 32,
    IPC_DRIVER_PORTION_REPORT_BROUGHT_FORWARD = 40,
    IPC_DRIVER_PORTION_REPORT_OUT_OF_BALANCE = 41,
    IPC_DRIVER_PORTION_REPORT_BROUGHT_FORWARD_0 = 42,
    IPC_DRIVER_MANUAL_REPORT_BROUGHT_FORWARD = 50,
    IPC_DRIVER_MANUAL_REPORT_OUT_OF_BALANCE = 51,
    IPC_DRIVER_MANUAL_REPORT_FIRST_PORTION = 52

} IPC_MESSAGE_TYPE_E;

int printerQueueId = -1;                                              // Incoming queue; also posted to by our serial receive thread

void testNonTransitWithSurcharge(int queueId)
{
    ifstream stream("test_assets/nontransit_surcharge_02.json");
    string buf((istreambuf_iterator<char>(stream)), istreambuf_iterator<char>());

    cout << buf << endl;

    IPC_header_t hdr = { IPC_PRINTER_JSON, DC_PRINTER_TASK };
    std::cout<<"~~~~~~~~~~~~~~~~~~~~~~~~~~~MSG type when sending message::"<<hdr.type<< std::endl;
    IpcSendMulti(queueId, &hdr, buf.c_str(), buf.size());
}
void testNonTransitReversal(int queueId)
{
    ifstream stream("test_assets/nontransit_surcharge_03.json");
    string buf((istreambuf_iterator<char>(stream)), istreambuf_iterator<char>());

    cout << buf << endl;

    IPC_header_t hdr = {199, DC_PRINTER_TASK };
    std::cout<<"~~~~~~~~~~~~~~~~~~~~~~~~~~~~MSG type when sending message::"<<hdr.type<< std::endl;
    IpcSendMulti(queueId, &hdr, buf.c_str(), buf.size());
}

void fillCommonDataStruct(IPC_Common_Details_t *pCommonData)
{
    pCommonData->terminalID        = 128934;	
    strncpy(pCommonData->busID, "123456", IPC_STRING_MAX_LENGTH);
    pCommonData->shiftNumber       = 123;
    pCommonData->transactionNumber = 56; 
    // pCommonData->transactionDate = time(0); 

    //	strcpy(pCommonData->location,"Geelong Rail");	
    strcpy(pCommonData->location,"Mathew Flindes Girls Secondary College and even significantly longer location string");	
    strcpy(pCommonData->mykiReference,"308425123455115");
    strcpy(pCommonData->salesMessage,"Sales message sample");

}

void fillCompanyDataStruct(IPC_Company_Details_t *pCompanyData)
{
    strcpy(pCompanyData->companyPhone,"+61312345678");
    strcpy(pCompanyData->companyName,"Test Company");
    strcpy(pCompanyData->companyABN,"NN NNN NNN NNN");
    strcpy(pCompanyData->companyWebSite,"https://translate.google.com.au");

}

void fillTopUpTestStruct( IPC_Printer_TpurseLoad_t * pBuf, bool invThresholdWarn )
{
    memset( pBuf, 0,  sizeof(IPC_Printer_TpurseLoad_t) );
    int addAmount = invThresholdWarn ? 100000 : 0;

    pBuf->hdr.type                     = IPC_PRINTER_TPURSELOAD;
    pBuf->hdr.source                   = DC_PRINTER_TASK;

    fillCommonDataStruct( &(pBuf->commonData) );

    pBuf->receiptInvoiceThreshold      = 100000;
    pBuf->amountNewBalance             = 2005 + addAmount;
    pBuf->amountTopUp                  = 200 + addAmount;
    pBuf->amountTotal                  = 200 + addAmount;

    fillCompanyDataStruct( &(pBuf->companyData) );	

}

void fillNonTransitBuff_1( IPC_PayloadMulti_t * pBuf)
{
    memset( pBuf, 0,  sizeof(IPC_PayloadMulti_t) );

    pBuf->hdr.type                     = IPC_PRINTER_JSON;
    pBuf->numberOfBlocks               = 3;
    pBuf->blockSequenceNumber          = 1;
    pBuf->blockSize                    = 256;
    std::string data = 
        "{"  
        "\"data\":{"
        "\"BusId\":\"VIXBUS-3\","
        "\"CompanyABN\":\"37 509 050 593\","
        "\"CompanyName\":\"Public Transport Victoria\","
        "\"CompanyPhone\":\"1800 800 007\","
        "\"CompanyWebSite\":\"ptv.vic.gov.au\","
        "\"Location\":\"Parkdale Secondary College\","
        "\"PrintTaxInvoice\":true,"
        "\"SalesMessage\":\"\","
        "\"ShiftNumber\":\"68\","
        "\"TerminalId\":\"90400642\","
        "\"amountgst\":55,"
        "\"amountpaid\":2700,"
        "\"amountrounding\":0,"
        "\"amounttotal\":2700,"
        "\"cardnumber\":\"308425966515888\","
        "\"cartitems\":["
        "{"
        "\"GSTApplicable\":true,"
        "\"PLU\":null,";
       
        
        
        
        
        

        pBuf->blockSize = data.size();
        strncpy(pBuf->data, data.c_str(), data.size());
        cout<<"LOADING:\n"<<pBuf->data<<endl;
        }
void fillNonTransitBuff_2 ( IPC_PayloadMulti_t * pBuf)
{
    memset( pBuf, 0,  sizeof(IPC_PayloadMulti_t) );

    pBuf->hdr.type                     = IPC_PRINTER_JSON;
    pBuf->numberOfBlocks               = 3;
    pBuf->blockSequenceNumber          = 2;
    pBuf->blockSize                    = 256;
    //strcpy(pBuf->data,                       
    //
    string data = 
        "\"Price\":600,"
        "\"id\":114,"
        "\"issuer_id\":1,"
        "\"itemnumber\":1,"
        "\"long_desc\":\"1. myki Full fare\","
        "\"short_desc\":\"1. myki Full fare\","
        "\"subtype\":\"None\","
        "\"type\":\"ThirdParty\""
        "},"
        "{"
        "\"GSTApplicable\":false,"
        "\"PLU\":null,"
        "\"Price\":100,"
        "\"id\":118,"
        "\"issuer_id\":1,"
        "\"itemnumber\":2,"
        "\"long_desc\":\"Telebus Full fare\","
        "\"short_desc\":\"Telebus Full fare\","
        "\"subtype\":\"None\","
        "\"type\":\"PremiumSurcharge\""
        "},"
        "{"  
        "\"GSTApplicable\":false,"
        "\"PLU\":null,"
        "\"Price\":2000,"
        "\"id\":300,"
        "\"issuer_id\":1,"
        "\"itemnumber\":3,"
        "\"long_desc\":\"\","
        "\"short_desc\":\"Add Value\","
        "\"subtype\":\"\","
        
        "\"type\":\"LLSC\""
        "}"
        "],"
        "\"paymenttype\":\"cash\","
        "\"printerMsgInvoiceThreshold\":100000,"
        "\"remvalue\":33815,"
        "\"saleseqno\":345,"
        "\"string_reason\":\"TPURSE_LOADED\","
        "\"surcharge\":{"
        "\"amountgst\":0,"
        "\"amountpaid\":100,"
        "\"amountrounding\":0,"
        "\"amounttotal\":100,"
        "\"cartitems\":["
        "{"  
        "\"GSTApplicable\":false,"
        "\"Price\":100,"
        "\"id\":118,"
        ;
        pBuf->blockSize = data.size();
    strncpy(pBuf->data, data.c_str(), data.size());

    cout<<"LOADING:\n"<<pBuf->data<<endl;
}
void fillNonTransitBuff_3( IPC_PayloadMulti_t * pBuf)
{
    memset( pBuf, 0,  sizeof(IPC_PayloadMulti_t) );

    pBuf->hdr.type                     = IPC_PRINTER_JSON;
    pBuf->numberOfBlocks               = 3;
    pBuf->blockSequenceNumber          = 3;
    pBuf->blockSize                    = 256;
    // strcpy(pBuf->data,  
    std::string data = 

        "\"issuer_id\":1,"
        "\"itemnumber\":2,"
        "\"long_desc\":\"Telebus Full fare\","
        "\"short_desc\":\"Telebus Full fare\","
        "\"subtype\":\"None\","
        "\"type\":\"PremiumSurcharge\""
        "}"
        "]"
        "},"
        "\"txnamount\":2000,"
        "\"txnseqno\":378,"
        "\"vat_rate\":10"
        "},"
        "\"name\":\"productsales\","
        "\"string_error\":\"WS_SUCCESS\","
        "\"terminalid\":\"90400642\","
        "\"timestamp\":\"2015-12-18T16:46:10+10:00\","
        "\"type\":\"set\","
        "\"userid\":\"\""
        "}"
        ;
        pBuf->blockSize = data.size();
    strncpy(pBuf->data, data.c_str(), data.size());
    cout<<"LOADING:\n"<<pBuf->data<<endl;
}
void fillTopUpRevTestStruct(IPC_Reversal_TpurseLoad_t * pBuf)
{
    memset(pBuf, 0,  sizeof(IPC_Reversal_TpurseLoad_t));

    pBuf->hdr.type                     = IPC_PRINTER_TPURSELOAD_REV;
    pBuf->hdr.source                   = DC_PRINTER_TASK;

    fillCommonDataStruct( &(pBuf->commonData) );

    pBuf->amountNewBalance             = 1050;
    pBuf->amountTopUp                  = 2008;
    pBuf->amountTotal                  = 10070;

    pBuf->origTransactionNumber        = 5678;

    pBuf->origDate = time(0); 

    fillCompanyDataStruct( &(pBuf->companyData) );

}


void fillDriverShiftTotalsTestStruct( IPC_DriverShiftReport_t * ipcRequest, int reportType, int cashBroughtForward, IPC_ShiftTotalsMaskLevel_e maskLevel )
{
    IPC_DriverShiftDetails_t * driverShiftDetails = &(ipcRequest->shiftDetails);

    memset( ipcRequest, 0,  sizeof(IPC_DriverShiftReport_t));

    ipcRequest->hdr.type		= IPC_PRINTER_DRIVER_SHIFT_REP;
    ipcRequest->hdr.source  	= DC_PRINTER_TASK;

    // Shift Report Header fields
    strncpy( driverShiftDetails->operatorID, "DC12345", IPC_STRING_MAX_LENGTH );  
    driverShiftDetails->shiftNumber = 6678;  
    strncpy( driverShiftDetails->busID, "1101AO", IPC_STRING_MAX_LENGTH );  

    driverShiftDetails->shiftEndDate = time(NULL);  
    driverShiftDetails->shiftStartDate = driverShiftDetails->shiftEndDate  - 7200;  
    driverShiftDetails->lastPortionStartDate = driverShiftDetails->shiftStartDate + 3600;  

    char footerMsg[] = "*This includes the total brought forward from previous shifts written to the staff access card";
    strncpy(ipcRequest->footer, footerMsg, IPC_STRING_MAX_LENGTH);


    ipcRequest->totalsMaskLevel = maskLevel;  
    driverShiftDetails->shiftCash.cashAmount = cashBroughtForward;  
    if ( cashBroughtForward == 0 ) {
        driverShiftDetails->shiftCash.cashStatus = CASH_NONE;  
    }

    switch ( reportType ) {
    case IPC_DRIVER_REPORT_BROUGHT_FORWARD:
    case IPC_DRIVER_REPORT_BROUGHT_FORWARD_0:
        driverShiftDetails->shiftCash.cashStatus = CASH_BROUGHT_FORWARD;  
        ipcRequest->reportType	= REPORT_END_OF_SHIFT;
        break;
    case IPC_DRIVER_REPORT_OUT_OF_BALANCE:
        driverShiftDetails->shiftCash.cashStatus = CASH_OUT_OF_BALANCE;  
    case IPC_DRIVER_REPORT:
        ipcRequest->reportType	= REPORT_END_OF_SHIFT;
        driverShiftDetails->lastPortionStartDate = driverShiftDetails->shiftStartDate;  
        break;
    case IPC_DRIVER_PORTION_REPORT_BROUGHT_FORWARD:
    case IPC_DRIVER_PORTION_REPORT_BROUGHT_FORWARD_0:
        driverShiftDetails->shiftCash.cashStatus = CASH_BROUGHT_FORWARD;  
        ipcRequest->reportType	= REPORT_SHIFT_PORTION;
        break;
    case IPC_DRIVER_PORTION_REPORT_OUT_OF_BALANCE:
        driverShiftDetails->shiftCash.cashStatus = CASH_OUT_OF_BALANCE;  
    case IPC_DRIVER_PORTION_REPORT:
        ipcRequest->reportType	= REPORT_SHIFT_PORTION;
        driverShiftDetails->lastPortionStartDate = driverShiftDetails->shiftStartDate;  
        break;
    case IPC_DRIVER_MANUAL_REPORT_BROUGHT_FORWARD:
        driverShiftDetails->shiftCash.cashStatus = CASH_BROUGHT_FORWARD;  
        ipcRequest->reportType	= REPORT_MANUAL;
        break;
    case IPC_DRIVER_MANUAL_REPORT_OUT_OF_BALANCE:
        driverShiftDetails->shiftCash.cashStatus = CASH_OUT_OF_BALANCE;  
    case IPC_DRIVER_MANUAL_REPORT:
        ipcRequest->reportType	= REPORT_MANUAL;
        break;
    case IPC_DRIVER_MANUAL_REPORT_FIRST_PORTION:
        driverShiftDetails->shiftCash.cashStatus = CASH_BROUGHT_FORWARD;  
        driverShiftDetails->lastPortionStartDate = driverShiftDetails->shiftStartDate;
        break;
    default: 
        CsErrx("fillDriverShiftTotalsTestStruct:: Invalid report type %d", reportType);
        break;
    }

    // Shift Report body fields
    // Cash 
    ipcRequest->cashTotals.grossAmount = 6300 + cashBroughtForward;
    ipcRequest->cashTotals.annulledAmount = 300;
    ipcRequest->cashTotals.netAmount = 6000 + cashBroughtForward;

    // Other t-purse payments
    ipcRequest->otherTotals.grossAmount = 900;
    ipcRequest->otherTotals.annulledAmount = 0;
    ipcRequest->otherTotals.netAmount = 900;

    // PaperTickets
    ipcRequest->paperTickets.grossTickets = 0;
    ipcRequest->paperTickets.annulledTickets = 0;
    ipcRequest->paperTickets.netTickets = 0;

    // Sundry Tickets
    ipcRequest->sundryTickets.grossTickets = 11;
    ipcRequest->sundryTickets.annulledTickets = 1;
    ipcRequest->sundryTickets.netTickets = 10;

    // Passenger count
    ipcRequest->fullPassCount = 12;
    ipcRequest->concessionPassCount = 22;

    ipcRequest->annulledTickets = 1;
    ipcRequest->netCash = 6000 + cashBroughtForward;
}


int main(int argc, char *argv[])
{
    int sendResult = 0;
    int initResult = 0;	
    int iChoice    = 0;
    int maskLevel  = 2;
    string str_converted;

    atexit((void (*)())IpcClose);

    const size_t nonTransitBufSize = sizeof(IPC_PayloadMulti_t) + 800;
    IPC_PayloadMulti_t *nonTransitBuf_1 = reinterpret_cast<IPC_PayloadMulti_t*>(calloc(1, nonTransitBufSize));
    IPC_PayloadMulti_t *nonTransitBuf_2 = reinterpret_cast<IPC_PayloadMulti_t*>(calloc(1, nonTransitBufSize));
    IPC_PayloadMulti_t *nonTransitBuf_3 = reinterpret_cast<IPC_PayloadMulti_t*>(calloc(1, nonTransitBufSize));


    if( argc < 2 )
        iChoice = IPC_TOPUP;
    else { 
        iChoice = atoi(argv[1]);
        if( argc > 2 )
            maskLevel = atoi(argv[2]);
    }

    if ( ( initResult = IpcInit() ) != 0 )                           // Initialize IPC communication
    {
        CsErrx("main:: IpcInit FAILED result %d", initResult);
        return -1;
    }

    printerQueueId   = IpcGetID(DC_PRINTER_TASK); 

    switch(iChoice)
    {
    case IPC_TOPUP:
    case IPC_TOPUP_INV_THRESHOLD_WARNING:   
        cout<<"TOP UP"<<endl;
        IPC_Printer_TpurseLoad_t  topUpBuf;
        fillTopUpTestStruct(&topUpBuf, iChoice == IPC_TOPUP_INV_THRESHOLD_WARNING);

        if( ( sendResult = IpcSend(printerQueueId, &topUpBuf, sizeof topUpBuf) ) != 0 )  
            CsErrx("main:: FAILED TO SEND IPC_TOPUP MESSAGE  %d", sendResult);			
        else
            CsDebug(3, (3, "main:: SENDING IPC_TOPUP MESSAGE")); 
        break;

    case IPC_TOPUP_REV:
        cout<<"TOP UP REVERSE"<<endl;
        IPC_Reversal_TpurseLoad_t topUpRevBuf;
        fillTopUpRevTestStruct(&topUpRevBuf);

        if( ( sendResult = IpcSend(printerQueueId, &topUpRevBuf, sizeof topUpRevBuf) ) != 0 )  
            CsErrx("main:: FAILED TO SEND IPC_TOPUP_REV MESSAGE  %d", sendResult);
        else
            CsDebug(3, (3, "main:: SENDING IPC_TOPUP_REV MESSAGE")); 
        break;

        //Non Transit
    case IPC_NONTRANSIT:
        cout<<"NON TRANSIT"<<endl;

        fillNonTransitBuff_1(nonTransitBuf_1);
        fillNonTransitBuff_2(nonTransitBuf_2);
        fillNonTransitBuff_3(nonTransitBuf_3);

        cout<<nonTransitBuf_1->data;
        cout<<nonTransitBuf_2->data;
        cout<<nonTransitBuf_3->data<<endl;

        if( ( sendResult = IpcSend(printerQueueId, nonTransitBuf_1, nonTransitBufSize) ) != 0 ||
                ( sendResult = IpcSend(printerQueueId, nonTransitBuf_2, nonTransitBufSize) ) != 0 ||
                ( sendResult = IpcSend(printerQueueId, nonTransitBuf_3, nonTransitBufSize) ) != 0
          )  
        {
            CsErrx("main:: FAILED TO SEND IPC_NONTRANSIT MESSAGE  %d", sendResult);
            cout<<"IPC SENDING MESSAGE --------  FAILED"<<endl;
        }
        else
        {
            CsDebug(3, (3, "main:: SENDING IPC_NONTRANSIT MESSAGE")); 
            cout<<"IPC SENDING MESSAGE --------  OK"<<endl;
        }
        break;
    case IPC_NONTRANSIT_SURCHARGE:
        cout << "NON TRANSIT SURCHARGE" << endl;
        testNonTransitWithSurcharge(printerQueueId);
        break;
     // number 8 for argument 1 when run the test 
    case IPC_NONTRANSIT_REVERSAL:
        cout << "NON TRANSIT REVERSAL" << endl;
        testNonTransitReversal(printerQueueId);
        break;

    case IPC_DRIVER_REPORT:
    case IPC_DRIVER_REPORT_OUT_OF_BALANCE:
    case IPC_DRIVER_PORTION_REPORT:
    case IPC_DRIVER_PORTION_REPORT_OUT_OF_BALANCE:
    case IPC_DRIVER_MANUAL_REPORT:
    case IPC_DRIVER_MANUAL_REPORT_OUT_OF_BALANCE:
    case IPC_DRIVER_REPORT_BROUGHT_FORWARD_0:
    case IPC_DRIVER_PORTION_REPORT_BROUGHT_FORWARD_0:
        IPC_DriverShiftReport_t driverShiftReportBuf;
        fillDriverShiftTotalsTestStruct(&driverShiftReportBuf, iChoice, 0, (IPC_ShiftTotalsMaskLevel_e)maskLevel);

        if( ( sendResult = IpcSend(printerQueueId, &driverShiftReportBuf, sizeof ( driverShiftReportBuf) ) ) != 0 ) {  
            CsErrx("main:: FAILED TO SEND IPC_DriverShiftReport MESSAGE  %d", sendResult);
        }		
        else
            CsDebug(3, (3, "main:: SENDING IPC_DriverShiftReport MESSAGE")); 
        break;

    case IPC_DRIVER_REPORT_BROUGHT_FORWARD:
    case IPC_DRIVER_PORTION_REPORT_BROUGHT_FORWARD:
    case IPC_DRIVER_MANUAL_REPORT_BROUGHT_FORWARD:
    case IPC_DRIVER_MANUAL_REPORT_FIRST_PORTION:
        IPC_DriverShiftReport_t driverShiftReportBufBf;
        fillDriverShiftTotalsTestStruct(&driverShiftReportBufBf, iChoice, 2000, (IPC_ShiftTotalsMaskLevel_e)maskLevel);

        if( ( sendResult = IpcSend(printerQueueId, &driverShiftReportBufBf, sizeof driverShiftReportBufBf) ) != 0 )  
            CsErrx("main:: FAILED TO SEND IPC_DriverShiftReport MESSAGE  %d", sendResult);
        else
            CsDebug(3, (3, "main:: SENDING IPC_DriverShiftReport MESSAGE")); 
        break;

    default:
        CsWarnx("Unknown message type");
        break;
    }	

    return 0;
}


