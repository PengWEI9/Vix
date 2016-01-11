/***************************************************************************
 * Copyright (c) 2012 Vix Technology. All rights reserved
 *
 * Module name   : SerialClass.cpp
 * Module type   : Source
 * Compiler(s)   : C++
 * Environment(s): LINUX
 *
 * Description:
 *
     Serial class to register as a transmitter for the serial port driver

 * Version   Who      Date       Description
   1.0       EAH      14/08/2013 Created
 *
 * $Id:  $
 * $HeadURL: $
 *.........................................................................*/
 
#include "csf.h"
#include "csthread.h"
#include "comm.h"
#include <errno.h>

#include <serpent/kdrv_serial.h>
#include <sys/ioctl.h>

#include <ipc.h>
#include <message_base.h>
#include "Serial.h"
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#define COMMSERIAL_SUCCESS 0
#define COMMSERIAL_FAILURE -1

// Constructor
SerialClass::SerialClass(/* TaskID_t */ int reportRecieveCharacter_SourceQueueId, /* IPC_t */ int reportRecieveCharacter_MessageId) :
        m_reportRecieveCharacter_SourceQueueId(reportRecieveCharacter_SourceQueueId),
        m_reportRecieveCharacter_MessageId(reportRecieveCharacter_MessageId)
{
    m_commHandle = INVALID_COMMS_HANDLE;
    m_queueId = 0;
    
    m_timeoutCallback = NULL;
    m_noCommsTimer = 0;
    
    m_port[0]   = 0;
    m_speed     = 0;
    m_data_bits = 0;
    m_parity    = 0;
    m_stop_bits = 0;

    m_timoutPeriod = 0;    
    
}

// Destructor
SerialClass::~SerialClass()
{
    ClosePort();
}


int  SerialClass::getSerialTimeout() // TODO Unused api, pick better names.
{
    return m_timoutPeriod;
}

void SerialClass::setSerialTimeout(int timeout)
{
    m_timoutPeriod = timeout;
}


// port      - serial device name
// speed     - eg 9600 
// data_bits - typically 8 
// parity    - 'n', 'o', 'e', 'm', 's' 
// stop_bits -  1 or 2);
int SerialClass::OpenPort(const char* port, unsigned long speed, unsigned int data_bits, char parity, unsigned int stop_bits)
{
    strncpy(m_port, port, MAX_PORTNAME_LENGTH);
    m_speed     = speed;
    m_data_bits = data_bits;
    m_parity    = parity;
    m_stop_bits = stop_bits;  
    
	// open the port
    return OpenPort();
}

int SerialClass::OpenPort()
{
    CsDebug(3, (3, "SerialClass::OpenPort: open port %s %d %d %c %d, handle=%d", m_port, m_speed, m_data_bits, m_parity, m_stop_bits));
    m_commHandle = CommOpen(m_port, m_speed, m_data_bits, m_parity, m_stop_bits);
    
    CsDebug(4, (4, "SerialClass::OpenPort: handle=%d",  m_commHandle));
    if(m_commHandle!=INVALID_COMMS_HANDLE) // NB Zero is a valid handle id
        return COMMSERIAL_SUCCESS;
    else
        return COMMSERIAL_FAILURE;
}

int SerialClass::ClosePort()
{
    if(m_commHandle!=INVALID_COMMS_HANDLE)
    {
        CsDebug(3, (3, "Closing com port %d", m_commHandle));
        int ret = CommClose(m_commHandle);
        m_commHandle = INVALID_COMMS_HANDLE;
        return ret;
    }
    
    return COMMSERIAL_SUCCESS;
}

void SerialClass::start()
{
	//InstallSiu();
    CsThrCreate(NULL, 0, &readThread, (void *)this, CSTHR_BOUND, NULL);
}

int SerialClass::Write(const void *WriteBuffer, unsigned short Length)
{
	if (m_commHandle == INVALID_COMMS_HANDLE)
		OpenPort();
    int ret = CommSend(m_commHandle, WriteBuffer, Length);
    CsDebug(5, (5, "SerialClass::Write(h=%d): len=%d, ret=%d", m_commHandle, Length, ret));
	return COMMSERIAL_SUCCESS;
}

int SerialClass::SetPortOptions(RS_protocol_e p, bool terminate)
{
	if (m_commHandle == INVALID_COMMS_HANDLE)
		return COMMSERIAL_FAILURE;

	int fd = CommGetDescriptor(m_commHandle);

	int mode;

	int result = ioctl(fd, KDRV_SERIAL_MODE_GET, &mode);
	if (result < 0)
	{
		CsErrx("SerialClass::SetPortOptions Get mode failed");
		return COMMSERIAL_FAILURE;
	}

	if (KDRV_SERIAL_MODE_RS485 & mode)
	{
		CsDebug(5, (5, "SerialClass::SetPortOptions (get) RS-485"));
	}
	else if(KDRV_SERIAL_MODE_RS422 & mode)
	{
		CsDebug(5, (5, "SerialClass::GetPortOptions (get) RS-422 Mode"));
	}
	else
	{
		CsDebug(5, (5, "SerialClass::GetPortOptions (get) RS-232 Mode"));
	}
	switch (p) {
	case SerialClass::RS232:
		CsDebug(3, (3, "SerialClass::SetPortOptions RS-232 Mode"));
		mode = KDRV_SERIAL_MODE_RS232;
		break;
	case SerialClass::RS422:
		CsDebug(3, (3, "SerialClass::SetPortOptions RS-422 Mode"));
		mode = KDRV_SERIAL_MODE_RS422;
		break;
	case SerialClass::RS485:
		CsDebug(3, (3, "SerialClass::SetPortOptions RS-485 Mode"));
		mode = KDRV_SERIAL_MODE_RS485;
		break;
	default:
		CsErrx("SerialClass::SetPortOptions invalid mode %d", p);
		return COMMSERIAL_FAILURE;
	}
	if (terminate)
	{
		CsDebug(3, (3, "SerialClass::SetPortOptions termination present"));
		mode |= KDRV_SERIAL_MODE_TERMINATION;
	}

	result = ioctl(fd, KDRV_SERIAL_MODE_SET, &mode);
	if (result < 0)
	{
		CsErrx("SerialClass::SetPortOptions Set mode failed");
		return COMMSERIAL_FAILURE;
	}

	return COMMSERIAL_SUCCESS;
}

int SerialClass::SetOnePointFiveStopBits(void)
{
	int fd = CommGetDescriptor(m_commHandle);

#define MPC52xx_PSC_MODE_ONE_STOP       0x07
#define MPC52xx_PSC_MODE_1P5_STOP       0x08
#define MPC52xx_PSC_MODE_TWO_STOP       0x0f

#define MPC5121_UART_CUSTOM_STOP_BITS   _IOW('E', 3, int)

    int custom_stop_bits = MPC52xx_PSC_MODE_1P5_STOP;

	int result = ioctl(fd, MPC5121_UART_CUSTOM_STOP_BITS, &custom_stop_bits);
	if (result < 0)
	{
		CsErrx("SerialClass::SetOnePointFiveStopBits failed");
		return COMMSERIAL_FAILURE;
	}
	CsDebug(3, (3, "SerialClass::SetOnePointFiveStopBits success"));
	return COMMSERIAL_SUCCESS;
}

void* SerialClass::readThread(void* this_pointer)
{
    static_cast<SerialClass*>(this_pointer)->Read();
    
    return NULL;
}

//#define ____DEBUG_SERIAL_
//#define _DEBUG_SERIAL_
int SerialClass::Read(void)
{
	int	fd;

	if (m_commHandle == INVALID_COMMS_HANDLE)
		OpenPort();

	fd = CommGetDescriptor(m_commHandle);
	if (fd == -1)
	{
        CsErrx("SerialClass::Read: file handle %d is not valid, cannot select/read", m_commHandle);
        return  COMMSERIAL_FAILURE;
	}
    
    // Call timeout
    if(m_timeoutCallback)
        m_timeoutCallback(m_noCommsTimer);

	while (1)
	{
		struct timeval tv;
		fd_set fdset;
    	int ret;

        FD_ZERO(&fdset);
        FD_SET(fd, &fdset);

		tv.tv_sec  = 0;
		tv.tv_usec = 5* 1000 * 1000; // MAGIC 5 seconds

#if defined(____DEBUG_SERIAL_)    
        CsDebug(5, (5, "SerialClass::Read: begin select on h:(%d[%d])", m_commHandle, fd));
#endif
        ret = select(fd + 1, &fdset, NULL, NULL, &tv);

        switch (ret) {
		case -1:
			/* error */
			if (errno != EAGAIN && errno != EINTR)
			{
				CsDebug(1, (1, "SerialClass::Read: error %d on select", errno));
                
                
                CsDebug(1, (1, "SerialClass::Read: closing port and reopen after wait"));
                ClosePort();
				
				usleep(1000000); // Reopen in 1 sec, TODO set in #define 
				
				// re-open it
				CsDebug(1, (1, "SerialClass::Read: re-opening port"));
				OpenPort();
			}
			else
			{
#if defined(_DEBUG_SERIAL_)
				CsDebug(5, (5, "SerialClass::Read: an expected early return %d on select", errno));
#endif
			}
			continue;

		case 0:
			/* no data, timed out */
#if defined(____DEBUG_SERIAL_)
			CsDebug(1, (9, "SerialClass::Read: timeout on select"));
#endif
			continue;

		default:
			/* got data, read it */
            
			break;
        }

		if (FD_ISSET(fd, &fdset))
		{
			int BytesRead;
			#define READBUFFERSIZE	64
			char	buf[ IPC_GENERIC_SIZE(READBUFFERSIZE) ];
			IPC_Generic_t *pMsg = (IPC_Generic_t *)buf;

			BytesRead = read(fd, pMsg->data, READBUFFERSIZE);
			CsDebug(8, (8, "SerialClass::Read(%d):%d", fd, BytesRead));

			if (BytesRead <=0)
			{
                if(BytesRead ==0)
                    CsDebug(7, (7, "SerialClass::Read: EOF returned, closing port;"));
                else
                    CsDebug(7, (7, "SerialClass::Read: fatal error returned, closing port;"));
				ClosePort();
				
                // Report timeout to running application
                if(m_timeoutCallback)
                    m_timeoutCallback(m_noCommsTimer);
                
				usleep(100*1000);
				
				// re-open it
				CsDebug(7, (7, "SerialClass::Read: re-opening port"));
				OpenPort();
				continue;
			}
            else
            {   
                pMsg->hdr.type    = m_reportRecieveCharacter_MessageId;
                pMsg->hdr.source  = m_reportRecieveCharacter_SourceQueueId;
                pMsg->len = BytesRead;

                CsDebug(5, (5, "SerialClass::Read: got %u characters", BytesRead));
                if ((ret = IpcSend(m_queueId, pMsg, IPC_GENERIC_SIZE(BytesRead))) != 0)
                    CsErrx("SerialClass::Read: failed to report rx-data-available to main thread");                 
            }
            
		}
		else // if (FD_ISSET(pipefd, &fdset))
		{
			CsErrx("SerialClass::Read: unexpected unblock of select()");
		}
	}
	/*NOTREACHED*/

	return 0;
}

void SerialClass::setTimeoutCallback(TimeoutCallback_t callback)
{
    m_timeoutCallback = callback;
    if (CsfTimerCreate2(m_timeoutCallback, "comms timeout", &m_noCommsTimer) != 0)
    {
        CsErrx("SerialClass: unable to create timeout timer");
    }
}

void SerialClass::ResetTimeout(void)
{
    // By default the timeout is not used.
    
    TimerSpec_t v = { {0}, {m_timoutPeriod, 0} };   // seconds
	CsfTimerSet(m_noCommsTimer, TMR_RELTIME, &v);
}

void SerialClass::setQueueId(int IPCQueueId)
{
    m_queueId = IPCQueueId;
}


