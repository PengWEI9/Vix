/* -*- mode: c; tabs: 4; -*- */
/*=============================================================================
 **
 **    Vix Technology                   Licensed software
 **    (C) 2014                         All rights reserved
 **
 **=============================================================================
 **
 **  Project/Product : MBU
 **  Filename        : message_reader.h
 **  Author(s)       : ?
 **
 **  Description     :
 **
 **      This header file contains:
 **
 **      Task IDs
 **      Message types
 **      generic messages - IPC_NoPayload_t is just a message header
 **
 **      Note that not all tasks expect to receive message, for example
 **      the ECU proxy's receive thread sends a message or rx char.
 **
 **  Member(s)       :
 **
 **  Information     :
 **   Compiler(s)    : C/C++
 **   Target(s)      : Independent
 **
 **  Subversion      :
 **      $Id: $
 **      $HeadURL: $
 **
 **  History         :
 **   Vers.  Date        Aut.  Type     Description
 **   -----  ----------  ----  -------  ----------------------------------------
 **    1.00  dd.mm.yy    ?     Create
 **    1.01  18.02.15    ANT   Add      MTU-4: Added IPC messages for FPDm
 **    1.02  23.03.15    ANT   Add      MTU-161: Added IPC messages for
 **                                     shift processing
 **
 **===========================================================================*/

#ifndef __MESSAGE_DC_H_INCLUDED
#define __MESSAGE_DC_H_INCLUDED

/*
 *      Includes
 *      --------
 /enum
 */

#include <message_assert.h>         /* static_assert() */
#include <message_base.h>
#include <csf.h>

/**
 *      Task identifications
 */
enum
{
    _not_used_TaskID,

    APP_TASK,						// readerapp
    APP_CMD_TASK,
    ECU_TASK,
    ECU_CMD_TASK,
    ECU_RECEIVE_TASK,               // private to the ECU_TASK
    UI_TASK,
    AVM_TASK,
    AVM_RECEIVE_TASK,
    DC_TASK,
    DC_CMD_TASK,
    WS_TASK,
    AS_TASK,
    MYKI_AVL_TaskId,
    DC_PRINTER_TASK,
    DC_PRINTER_RECEIVE_TASK,

    _last_reader_TaskID_plus1,
    //  NUMBER_OF_READER_TASKS = (_last_reader_TaskID_plus1 - 1)
};

/**
 *      Message types
 */
enum
{
    //
    // see message_ecu.h for ECU (proxy) -> APP, and APP -> ECU message payloads 
    //
    IPC_to_ecuproxy_first = (100),

    IPC_CSC_PRESENT = IPC_to_ecuproxy_first,
    /*
       Payload: bool transit_card

       When a transit card is presented the gate will respond with either:
       IPC_CSC_ALLOW_VALIDATION, IPC_CSC_STOP_VALIDATION, or
       in the exceptional circumstance of an unexpected card IPC_CSC_DISABLE
       */

    IPC_CSC_VALIDATION_OK,                  // IPC_ValidationSuccess_t
    IPC_CSC_VALIDATION_FAIL,                // IPC_ValidationFail_t

    /* For reporting the application state */
    IPC_APPLICATION_STATE,                  // IPC_ApplicationState_t

    IPC_ECU_SERIAL_RX_CHARACTER_AVAILABLE,  // IPC_Generic_t

    IPC_ECU_SERIAL_RX_FRAME_AVAILABLE,

    // message payload defined message_alert.h which uses udmf.h (from 'udmf.x')
    IPC_ECU_ALERT_MESSAGE,		            // reader alerts to send to EMM proxy (which knows how to forward)

    IPC_to_ecuproxy_last_plus1,
    //#define NUMBER_OF_IPCS_FOR_ECUPROXY     (IPC_to_ecuproxy_last_plus1 - IPC_to_ecuproxy_first)

    //
    IPC_to_app_first = (120),

    IPC_CSC_DISABLE = IPC_to_app_first,
    /*
       The gate cannot accept IPC_CSC_PRESENT messages. At startup the reader should assume a disable message has been received.

       Notes:

       1. if a card is presented at the ECU at the other end of the lane, the gate will disable the other reader
       2. if a card is presented at both readers, some logic will ensure that one ECU take control and the other relinquishes control.
       3. it will be possible for both readers to send card present messages at almost the same time, and, as a result the loosing end
       may be sending a disable message as the card message arrives. The reader should handle the disable message as a valid
       response to an outstanding card message.
       */

    IPC_CSC_ENABLE,                 // The ECU is ready to accept card messages.

    /* expected replies to IPC_CSC_PRESENT, IPC_NoPayload_t */
    IPC_CSC_ALLOW_VALIDATION,       // EMM has successfully seized control of the gate
    IPC_CSC_STOP_VALIDATION,        // EMM could not sieze control of the gate

    IPC_CSC_DETECTION_ENABLE,       // IPC_NoPayload_t
    IPC_CSC_DETECTION_DISABLE,      // IPC_NoPayload_t

    IPC_ECU_COMMS_DOWN,             // IPC_NoPayload_t

    IPC_SET_LOCATION,               /**< IPC_IntPayload_t   - Sets current location */
    IPC_SHIFT,                      /**< IPC_IntPayload_t   - Current shift information */

    IPC_to_app_last_plus1,
    //#define NUMBER_OF_IPCS_FROM_ECU (IPC_to_app_last_plus1 - IPC_to_app_first)

    // IPC for UI process
    IPC_to_ui_first = (150),

    IPC_UI_CHANGE_SCREEN,           // use IPC_UI_ChangeScreen_t from message_ui.h to construct messages

    IPC_to_ui_last_plus1,

    // IPC for AVM process
    IPC_to_AVM_first = (160),

    IPC_AVM_SERIAL_RX_CHARACTER_AVAILABLE,	// receive thread woke from select()
    IPC_AVM_ALERT,							// the magic value representing device problems
    IPC_AVM_RUN_ID_REPORT,					// what the driver selected

    IPC_to_AVM_last_plus1,

    IPC_from_AVM_first = (170),

    IPC_AVM_COMMS_DOWN,
    IPC_AVM_COMMS_UP,
    IPC_AVM_NEW_POINT,		// AVM 'signpost code'
    IPC_AVM_TIME,			// as AVM broadcast

    IPC_from_AVM_last_plus1,

    IPC_from_3G_first = (180),
    IPC_3G_STATE,
    IPC_from_3G_last_plus1,    

    IPC_to_printer_first = (190),
    IPC_PRINTER_SERIAL_RX_CHARACTER_AVAILABLE,
    IPC_PRINTER_STATE,
    IPC_PRINTER_TPURSELOAD,
    IPC_PRINTER_SURCHARGE,
    IPC_PRINTER_PAPER,
    IPC_PRINTER_TAX_INVOICE,
    IPC_PRINTER_TPURSELOAD_REV,
    IPC_PRINTER_DRIVER_SHIFT_REP,
    IPC_PRINTER_JSON,
    IPC_to_printer_last_plus1, 
    // This set of IPC that can be redefined on a per-process basis
    // *** NEVER USE for messaging between processes
    IPC_private_first = (210),

    ITC_INTER_THREAD_PRIVATE_00 = IPC_private_first,
    ITC_INTER_THREAD_PRIVATE_01,
    ITC_INTER_THREAD_PRIVATE_02,
    ITC_INTER_THREAD_PRIVATE_03,
    ITC_INTER_THREAD_PRIVATE_04,
    ITC_INTER_THREAD_PRIVATE_05,
    ITC_INTER_THREAD_PRIVATE_06,
    ITC_INTER_THREAD_PRIVATE_07,
    ITC_INTER_THREAD_PRIVATE_08,
    ITC_INTER_THREAD_PRIVATE_09,

    IPC_private_last_plus1
};

static_assert((IPC_t)IPC_management_last_plus1  <= (IPC_t)IPC_to_ecuproxy_first,    "Number range clash: 'management' IPCs overran the 'to ecuproxy' range");
static_assert((IPC_t)IPC_to_ecuproxy_last_plus1 <= (IPC_t)IPC_to_app_first,         "Number range clash: 'to ecuproxy' IPCs overrun the 'to app' range");
static_assert((IPC_t)IPC_to_app_last_plus1      <= (IPC_t)IPC_to_ui_first,          "Number range clash: 'to app' IPCs overrun the 'to ui' range");
static_assert((IPC_t)IPC_to_ui_last_plus1       <= (IPC_t)IPC_to_AVM_first,         "Number range clash: 'to ui' IPCs overrun the 'to AVM' range");
static_assert((IPC_t)IPC_to_AVM_last_plus1      <= (IPC_t)IPC_from_AVM_first,       "Number range clash: 'to AVM' IPCs overrun the 'from AVM' range");
static_assert((IPC_t)IPC_from_AVM_last_plus1    <= (IPC_t)IPC_from_3G_first,        "Number range clash: 'from AVM' IPCs overrun the 'private' range");
static_assert((IPC_t)IPC_from_3G_last_plus1     <= (IPC_t)IPC_to_printer_first,     "Number range clash: 'from 3G' IPCs overrun the 'private' range");
static_assert((IPC_t)IPC_to_printer_last_plus1  <= (IPC_t)IPC_private_first,        "Number range clash: 'to printer' IPCs overrun the 'private' range");

/**
 *      Application states.
 */
typedef enum
{
    IPC_APPLICATION_STATE_UNKNOWN = 1,
    IPC_APPLICATION_STATE_INITIALISING,
    IPC_APPLICATION_STATE_DISABLED,
    IPC_APPLICATION_STATE_ENABLED,
    IPC_APPLICATION_STATE_VALIDATING,
    IPC_APPLICATION_STATE_OUT_OF_SERVICE,
    IPC_APPLICATION_STATE_OUT_OF_ORDER
} IPC_ApplicationState_e;

/**
 *      3G Connection states.
 */
typedef enum
{
    IPC_3G_UNINITIALISED = 0,
    IPC_3G_DISCONNECTED,
    IPC_3G_CONNECTING,
    IPC_3G_CONNECTED,
    IPC_3G_ERROR
} IPC_3GState_e;

/**
 *      Signal Quality states.
 */
typedef enum
{
    IPC_3G_SIGNAL_QUALITY_UNKNOWN = 0,
    IPC_3G_SIGNAL_QUALITY_MARGINAL,
    IPC_3G_SIGNAL_QUALITY_OK,
    IPC_3G_SIGNAL_QUALITY_GOOD,
    IPC_3G_SIGNAL_QUALITY_EXCELLENT
} IPC_3GSignalQuality_e;

/*
 * Messages shall be defined as follows:
 */

/**
 *      IPC_APPLICATION_STATE - Application state notification.
 */
typedef struct
{
    IPC_header_t            hdr;
    IPC_ApplicationState_e  state;
} IPC_ApplicationState_t;

/**
 *      IPC_CSC_PRESENT - Card presented notification.
 */
typedef struct
{
    IPC_header_t    hdr;
    int             cardAppTransit;
    int             passengerCode;
} IPC_CardPresent_t;

// AVM from app
//
/*
 *      IPC_AVM_RUN_ID_REPORT - run/table/driver
 */
typedef struct
{
    IPC_header_t	hdr;
    unsigned			depot;	// 1..26 representing depot charaters 'A' - 'Z'
#define TRAM_AVM_RUN_NUM_MAX			(511)
#define TRAM_AVM_RUN_NUM_NOT_SET		(0)
#define TRAM_AVM_RUN_NUM_UNSCHEDULED	(505)
    unsigned			runNumber;
#define TRAM_AVM_TABLE_NUM_MAX	(9999)
    unsigned			tableNumber;
#define TRAM_AVM_DRIVER_ID_MAX	(999999)
    unsigned			driverID;
} IPC_AVM_RunID_Report_t;

// AVM to app
//
typedef struct 
{
    struct SignPostCode_t {
        unsigned char	east;
        unsigned char	north;
    }				code;
    unsigned char	level;
    unsigned int	odo;	/* metres */
} MsgSignPost_t;

typedef struct
{
    IPC_header_t    hdr;
    MsgSignPost_t	sp;
} IPC_AVM_Signpost_t;

typedef struct 
{
    time_t	broadcast;	/* the AVM time */
    time_t	OS_Time;	/* local time() when the broadcast was recieved */
} MsgAVM_Time_t;

typedef struct 
{
    IPC_header_t    hdr;
    MsgAVM_Time_t	t;
} IPC_AVM_Time_t;

typedef struct
{
    IPC_header_t            hdr;
    IPC_3GState_e           state;
    IPC_3GSignalQuality_e   signalQuality;  
    int                     signalQualityValue;  // Raw value
} IPC_3G_State_t;

//NGBU-488 define more as needed for topup receipt


#define IPC_STRING_MAX_LENGTH 100

typedef struct
{
    char                  companyPhone[IPC_STRING_MAX_LENGTH];
    char                  companyName[IPC_STRING_MAX_LENGTH];
    char                  companyABN[IPC_STRING_MAX_LENGTH];
    char                  companyWebSite[IPC_STRING_MAX_LENGTH];	
} IPC_Company_Details_t;


typedef struct
{
    char                  busID[IPC_STRING_MAX_LENGTH];
    int32_t               terminalID;	
    int32_t               shiftNumber;
    int32_t               transactionNumber;	

    char                  mykiReference[IPC_STRING_MAX_LENGTH];
    char                  location[IPC_STRING_MAX_LENGTH];
    char                  salesMessage[IPC_STRING_MAX_LENGTH]; 
    time_t                transactionDate;
} IPC_Common_Details_t;


typedef struct 
{
    IPC_header_t          hdr;	

    IPC_Company_Details_t companyData;	

    IPC_Common_Details_t  commonData;

    int32_t               receiptInvoiceThreshold;

    unsigned int          amountTopUp;	 
    int                   amountNewBalance;
    int                   amountTotal;	

} IPC_Printer_TpurseLoad_t;

typedef struct 
{
    IPC_header_t          hdr;	

    IPC_Company_Details_t companyData;	

    IPC_Common_Details_t  commonData;	

    unsigned int          amountTopUp;	 
    int                   amountNewBalance;
    int                   amountTotal;	

    int32_t               origTransactionNumber;

    time_t                origDate;

} IPC_Reversal_TpurseLoad_t;

/** Shift report types */
typedef enum
{
    REPORT_END_OF_SHIFT,                /**< End of shift report */
    REPORT_SHIFT_PORTION,               /**< End of shift portion report */
    REPORT_MANUAL                       /**< End of shift report without Driver card update */

} IPC_ReportType_e;

/** Start shift cash status */
typedef enum
{
    CASH_NONE,                          /**< No cash brought forward */
    CASH_BROUGHT_FORWARD,               /**< Cash brought forward */
    CASH_OUT_OF_BALANCE                 /**< Cash out-of-balance (Driver card not updated on last shift close) */

} IPC_CashStatus_e;

/** Shift totals report masking levels */
typedef enum
{
    MASK_ALL = 0,                       /**< Masked out all shift total amounts and counters */
    MASK_TOTALS =1,                     /**< Masked out shift total amounts (only) */
    MASK_NONE = 2	                    /**< Show all shift total amounts and counters */

} IPC_ShiftTotalsMaskLevel_e;

/** Start shift cash details */
typedef struct
{
    IPC_CashStatus_e cashStatus;
    int              cashAmount;        /**< Cash amount brought forward */

} IPC_ShiftCashStatus_t;

typedef struct
{
    int         grossAmount;
    int         annulledAmount;
    int         netAmount;

} IPC_ShiftTotals_t;

typedef struct
{
    int         grossTickets;
    int         annulledTickets;
    int         netTickets;

} IPC_ShiftTickets_t;

typedef struct
{
    char                  operatorID[IPC_STRING_MAX_LENGTH];
    int                   shiftNumber;
    char                  busID[IPC_STRING_MAX_LENGTH];
    time_t                shiftStartDate;
    time_t                lastPortionStartDate;
    time_t                shiftEndDate;
    IPC_ShiftCashStatus_t shiftCash;

} IPC_DriverShiftDetails_t;

/** IPC_PRINTER_DRIVER_SHIFT_REP - Print Driver Shift Report */
typedef struct 
{
    IPC_header_t              hdr;
    IPC_ReportType_e          reportType;
    IPC_ShiftTotalsMaskLevel_e totalsMaskLevel;
    IPC_DriverShiftDetails_t  shiftDetails;

    IPC_ShiftTotals_t         cashTotals;
    IPC_ShiftTotals_t         otherTotals;

    IPC_ShiftTickets_t        paperTickets;
    IPC_ShiftTickets_t        sundryTickets;

    int                       fullPassCount;
    int                       concessionPassCount;

    int                       annulledTickets;
    int                       netCash;

    char                      footer[IPC_STRING_MAX_LENGTH];

} IPC_DriverShiftReport_t;

#endif //__MESSAGE_DC_H_INCLUDED
