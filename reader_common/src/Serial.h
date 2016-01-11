/* -*- mode: c tabs: 4 -*- *************************************************
 * Copyright (c) 2012 Vix Technology. All rights reserved
 *
 * Module name   : CommSerial.h
 * Module type   : Header
 * Compiler(s)   : C++
 * Environment(s): LINUX
 *
 * Description:
 *
     Serial class to register as a transmitter for the serial port driver
 *
 * Version   Who      Date       Description
   1.0       EAH      14/08/2013 Created
 *
 * $Id:  $
 * $HeadURL: $
 *.........................................................................*/

#ifndef SERIALCLASS_H
#define SERIALCLASS_H
 
#define PARITY_NONE  'n'
#define PARITY_EVEN  'e'
#define PARITY_ODD   'o'
#define PARITY_MARK  'm'
#define PARITY_SPACE 's'

#define INVALID_COMMS_HANDLE -1

#define MAX_PORTNAME_LENGTH 256

// to make use of timers, the user must have called CsfTimerInit

typedef void (*TimeoutCallback_t)(uint32_t);
 
class SerialClass {
    public:
        SerialClass(/* TaskID_t */ int reportRecieveCharacter_SourceQueueId, /* IPC_t */ int reportRecieveCharacter_MessageId);
        ~SerialClass();
    protected:
        int OpenPort(void);
    public:
        int OpenPort(const char* port, unsigned long speed, unsigned int data_bits, char parity, unsigned int stop_bits);
        typedef enum { RS232, RS422, RS485 } RS_protocol_e;
        int SetPortOptions(RS_protocol_e p, bool terminate = false);
        int SetOnePointFiveStopBits(void);
        
        int ClosePort(void);
        int Write(const void *WriteBuffer, unsigned short Length);
        int Read(void);
    
        void ResetTimeout(void);
        
        void setQueueId(int IPCQueueId);
        
        void setTimeoutCallback(TimeoutCallback_t);
        void setWriteCallback(TimeoutCallback_t);
        
        void start();
        
        int  getSerialTimeout();
        void setSerialTimeout(int timeout);

    private:
        SerialClass();
        static void* readThread(void*);
    public:
        uint32_t m_noCommsTimer;
    private:
        int m_commHandle;

        int m_queueId;
        int m_timoutPeriod; // Number of seconds before reporting link down
        
        // reporting parameters
        int m_reportRecieveCharacter_SourceQueueId;
        int m_reportRecieveCharacter_MessageId;


        // COM port settings
        char            m_port[MAX_PORTNAME_LENGTH+1];
        unsigned long   m_speed;
        unsigned int    m_data_bits;
        char            m_parity;
        unsigned int    m_stop_bits;
        
        TimeoutCallback_t m_timeoutCallback;
};

#endif // SERIALCLASS_H
