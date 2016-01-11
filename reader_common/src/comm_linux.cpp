/* Copyright (c) 2009 ERG Transit Systems (HK) Ltd.
*
* Linux serial comms library: originally taken from core services.
*
*
*/

#include <termios.h>
#include <fcntl.h>
#include <stdio.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <ctype.h>
#include <cs.h>
#include "comm.h"

/*
 * Constants
 */
#define TIMEOUT     80
#define BAUDRATE    B19200
#define COMMPORT    0
#define WORDLEN     CS8 
#define BUFSIZE     1024
#define MAX_PORTS   30

/*
 * Macros
 */
/* due to limitation, only support 9600, 19200, 38400, 57600 and 115200 */
#define CheckBaudRate(b)    ((((b) == 9600) || ((b) == 19200) || ((b) == 38400) || ((b) == 57600) || ((b) == 115200)) ? 1 : 0)
#define CheckParity(c)      ((((c) == 0) || ((c) == 'o') || ((c) == 'n') || ((c) == 'e')) ? 1 : 0)
#define GetBaudRate(b)      (((b)  == 9600) ? B9600 : \
              ((b) == 19200) ? B19200 : \
              ((b) == 38400) ? B38400 : \
              ((b) == 57600) ? B57600 : B115200)
#define GetParity(c)        (((c) == 'o') ? (PARENB|PARODD) : ((c) == 'e') ? PARENB : 0)
#define CheckStopBits(s)    ((((s) >= 0) && ((s) <= 2)) ? 1 : 0)
#define GetStopBits(s)      (((s) == 2) ? CSTOPB : 0) 
#define GetWordLen(l)       (((l) == 5) ? CS5 : \
              ((l) == 6) ? CS6 : \
              ((l) == 7) ? CS7 : CS8)

/* for testing */
//#define _DEBUG_SERIAL_ 1

typedef struct
{
    int fd;             /* os handle */
    int baud;
    int parity;
    int stop_bits;
    int data_bits;
    int auto_drain;     /* drain tx buffer on each write (blocking) */
} CommPort_t;

static CommPort_t ports[MAX_PORTS];

#define HANDLE(handle) ports[handle]

void Trapdoor()
{
    static int init;
    int i;

    if (!init)
    {
        for (i = 0; i < MAX_PORTS; i++)
        {
            memset(&ports[i], 0, sizeof(CommPort_t));
            ports[i].fd = -1;
        }
        init = 1;
    }
}

static ulong CommGetMsTime(void)
{
    struct timeval t;
    struct timezone tz;

    gettimeofday(&t, &tz);
    return ((ulong) (t.tv_sec * 1000 + (t.tv_usec / 1000))); /* bug fix */
}

unsigned long TimerGetMilliseconds(void)
{
    return CommGetMsTime();
}


void TimerDelay(unsigned long Delay)
{
    usleep(Delay * 1000);
}


int CommOpen(const char *device_name, 
                         unsigned long speed,
                         unsigned data_bits,
                         char parity,
                         unsigned stop_bits)
{
    struct termios tio;
    int i = -1, fd = -1;

    Trapdoor();

    if (!CheckParity(parity) || !CheckBaudRate(speed) || !CheckStopBits(stop_bits))
        return -1;

    /* open port */
    fd = open(device_name, O_RDWR|O_NOCTTY|O_NONBLOCK);
    if (fd < 0) 
    {
#if defined(_DEBUG_SERIAL_)
        CsDebug(1, (1, "CommOpen: open failed"));
#endif
        return -1;
    }

#if defined(_DEBUG_SERIAL_)
    CsDebug(1, (1, "CommOpen: opened %s (fd:%d)", device_name, fd));
#endif

    /* find a handle */
    for (i = 0; i < MAX_PORTS; i++)
    {
        if (HANDLE(i).fd == -1)
            break;
    }

    if (i == MAX_PORTS)
        return -1;

    HANDLE(i).fd = fd;
    
    /* don't wory about these for now */
    //HANDLE(i).baud       = GetBaudRate(speed);
    //HANDLE(i).parity     = GetParity(parity);
    //HANDLE(i).stop_bits  = GetStopBits(stop_bits);
    //HANDLE(i).data_bits  = GetWordLen(data_bits);

    /* setup baudrate and other communication port configuration */
    //newtio.c_cflag = HANDLE(i).baud       |
    //                 HANDLE(i).parity     |
    //                 HANDLE(i).stop_bits  |
    //                 HANDLE(i).data_bits  |
    //                 CLOCAL|CREAD|PARENB;
    /* setup baudrate and other communication port configuration */

    /* get current attributes */
    tcgetattr(fd, &tio);

    /* set input/ouput baud rate */
    cfsetispeed(&tio, GetBaudRate(speed));
    cfsetospeed(&tio, GetBaudRate(speed));

    /* reset current databits, parity and stopbits settings */
    tio.c_cflag &= ~(CSIZE | PARENB | PARODD | CSTOPB);
    /* set new settings */
    tio.c_cflag |= (GetWordLen(data_bits) | GetParity(parity) | GetStopBits(stop_bits) | CREAD | CLOCAL);

    tio.c_iflag     = 0;
    tio.c_oflag     = 0;
    tio.c_lflag     = 0;
    tio.c_cc[VMIN]  = 1;
    tio.c_cc[VTIME] = 0;

    /* write out the attributes */
    tcsetattr(fd, TCSAFLUSH, &tio);

    CsDebug(1, (1, "CommOpen: opened handle %d(%d)", i, fd));

    return i;
}


int  CommClose(int handle)
{
    if (handle < 0 || handle >= MAX_PORTS)
        return -1;

    if (close(HANDLE(handle).fd) != 0)
        return -1;

    HANDLE(handle).fd = -1;

    return 0;
}

int CommSetSpeed(int handle, unsigned long speed)
{
    struct termios tio;

    if (handle < 0 || handle >= MAX_PORTS)
        return -1;

    tcgetattr(HANDLE(handle).fd, &tio);
    cfsetospeed(&tio, GetBaudRate(speed));
    tcsetattr(HANDLE(handle).fd, TCSANOW, &tio);

    return 0;
}

int CommGetDescriptor(int handle)
{
    if (handle < 0 || handle >= MAX_PORTS)
        return -1;

    return HANDLE(handle).fd;
}


int CommSetSendWait(int handle, int SendWait)
{
    if (handle < 0 || handle >= MAX_PORTS)
        return -1;

   HANDLE(handle).auto_drain = SendWait;

   return 0; 
}

int CommReceiveTm(int handle, void *data, unsigned buf_size, int timeout, int readavaildata)
{
#if defined(_DEBUG_SERIAL_)    
    ulong timestart, timestop;
#endif
    fd_set fdset;
    ulong now, end, remaining;
    int nToRead = buf_size; /* number of bytes left to read */
    unsigned int nRead = 0;          /* number of bytes read so far */
    unsigned char *ptr = (unsigned char *) data;
    int ret;
    struct timeval tv;

    if (handle < 0 || handle >= MAX_PORTS)
        return -1;

    if (data == NULL)
        return -1;

    now = CommGetMsTime();

#if defined(_DEBUG_SERIAL_)    
    CsDebug(2, (2, "CommReceive: h=(%d[%d]) len=%d, timeout=%d", handle, HANDLE(handle).fd, buf_size, timeout));
    timestart = now;
#endif

    end        = now + (ulong) timeout;
    tv.tv_sec  = 0;
    tv.tv_usec = timeout * 1000;

    do 
    {
        FD_ZERO(&fdset);
        FD_SET(HANDLE(handle).fd, &fdset);

#if defined(_DEBUG_SERIAL_)    
        CsDebug(5, (5, "CommReceive: begin select"));
#endif

        ret = select(HANDLE(handle).fd+1, &fdset, 0, 0, &tv);

#if defined(_DEBUG_SERIAL_)    
        CsDebug(5, (5, "CommReceive: select returned %d", ret));
#endif

        switch (ret)
        {
            case -1:
                /* error */
                if (errno != EAGAIN && errno != EINTR)
                {
#if defined(_DEBUG_SERIAL_)
                    CsDebug(1, (1, "CommReceive: error %d on select", errno));
#endif
                    return -1;
                }
                break;

            case 0:
                /* no data, timed out */
#if defined(_DEBUG_SERIAL_)
                CsDebug(1, (1, "CommReceive: timeout on select"));
#endif
                return nRead;

            default:
                /* got data, read it */
                ret = read(HANDLE(handle).fd, ptr, nToRead);
                if (ret > 0)
                {
#if defined(_DEBUG_SERIAL_)    
                    CsDebug(5, (5, "CommReceive: read %d byte(s)", ret));
#endif
                    nRead += ret;
                    nToRead -= ret;
                    ptr += ret;
                }
#if defined(_DEBUG_SERIAL_)
                else if (ret == 0)
                {
                    CsDebug(1, (1, "CommReceive: no data to read!"));
                    return -1;
                }
#endif
                break;
        }

        /* re-calculate the timeout */
        now = CommGetMsTime();
        if (end > now)
            remaining  = end - now;
        else    
            break;  /* exit immediately */
        
        if (readavaildata == 1 && nRead > 0)
        {
            // if we have data, we can return immediately if param says yes
            CsDebug(1, (1, "CommReceive: ra=yes"));
            break;
        }

        tv.tv_sec  = 0;
        tv.tv_usec = remaining * 1000;
    } while (nToRead > 0 && nRead < buf_size);

#if defined(_DEBUG_SERIAL_)    
    timestop        = CommGetMsTime();
    CsDebug(1, (1, "CommReceive: read %d byte(s) taken %dms", nRead, (timestop - timestart)));
#endif

    return nRead;
}

int CommReceive(int handle, void *data, unsigned buf_size)
{
    return CommReceiveTm(handle, data, buf_size, 80, 0);
}

int CommSend(int handle, const void *data, unsigned length)
{
#if defined(_DEBUG_SERIAL_)    
    ulong timestart, timestop;
#endif
    unsigned int nBytesWritten = 0;
    int bytes;
    unsigned char *ptr = (unsigned char *) data;

    if (handle < 0 || handle >= MAX_PORTS)
        return -1;

    if (data == NULL || length <= 0)
        return -1;

#if defined(_DEBUG_SERIAL_)
    timestart = CommGetMsTime();
    CsDebug(4, (4, "CommDrvWrite: h=(%d[%d]) write %d byte(s) from %p", handle, HANDLE(handle).fd, length, data));
#endif

    while (nBytesWritten <  length)
    {
        bytes = write(HANDLE(handle).fd, ptr, length - nBytesWritten);
#if defined(_DEBUG_SERIAL_)
        CsDebug(5, (5, "CommDrvWrite: write returned %d", bytes));
#endif
        if (bytes < 0)
        {
            if (errno != EAGAIN && errno != EINTR && errno != EAGAIN)
            {
                /* some write error */
#if defined(_DEBUG_SERIAL_)
                CsDebug(1, (1, "CommDrvWrite: h(%d[%d]) write error (%d)", handle, HANDLE(handle).fd, errno));
#endif
                return -1;
            }
            /* sleep one ms (one timeslice, generally > 1ms) */
            usleep(1*1000);
        }
        else
        {
            nBytesWritten += bytes;
            ptr += bytes;
        }
    }

    if (HANDLE(handle).auto_drain)
    {
#if defined(_DEBUG_SERIAL_)    
        CsDebug(5, (5, "CommDrvWrite: flushing"));
#endif
        tcdrain(HANDLE(handle).fd);
    }

#if defined(_DEBUG_SERIAL_)    
    timestop = CommGetMsTime();
    CsDebug(3, (3, "CommDrvWrite: wrote %d byte(s) taken %dms", length, (timestop - timestart) ));
#endif

    return nBytesWritten;
}


/*
*  Flush/discard data in receiving queue of serial device.
*/
int CommClearReceiveBuffer(int handle)
{
    if (handle < 0 || handle >= MAX_PORTS)
        return -1;

    if (tcflush(HANDLE(handle).fd, TCIFLUSH) == -1)
        return -1;

    return 0;
}

/* end of file */

