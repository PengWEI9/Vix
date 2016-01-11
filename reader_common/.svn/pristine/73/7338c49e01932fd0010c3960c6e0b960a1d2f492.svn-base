/* comm.h
**
** originally sanfran windows code.
**
*/

#ifndef _COMM_H_
#define _COMM_H_

int CommOpen(const char *device_name, 
             unsigned long speed,    /* bps, eg 9600 */
             unsigned data_bits,     /* typically 8 */
             char parity,            /* 'n', 'o', 'e', 'm', 's' */
             unsigned stop_bits      /* 1 or 2 */);
int CommClose(int handle);
int CommGetDescriptor(int handle);	/* use for select, read done outside this class */
int CommSetSpeed(int handle, unsigned long speed);
int CommSend(int handle, const void *data, unsigned length);
int CommReceive(int handle, void *data, unsigned buf_size);
int CommReceiveTm(int handle, void *data, unsigned buf_size, int timeout, int readavaildata);
int CommClearReceiveBuffer(int handle);
int CommSetSendWait(int handle, int SendWait);

#endif /* _COMM_H_ */
