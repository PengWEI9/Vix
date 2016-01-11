/* Copyright (c) 2011. All rights reserved. ERG Transit Systems (HK) Ltd.
**
** CommonTimer.h - time class
**
** Version Date    Name       Reason
** 1.0     12/12/11  Gerry       Created
**
*/
#ifndef COMMONTIMER_H_INCLUDED
#define COMMONTIMER_H_INCLUDED

#include <stdio.h>
#include <unistd.h>
#include <iostream>

//Core Services
#include <cs.h>
#include <csthread.h>

class CommonTimer
{
private:
    static int CommonTimerStatus;
    static int MessageTimerStatus;
public:
	CommonTimer(void);
	~CommonTimer(void);

    void StartCommonTimer(int MsInterval);
    void StartMessageTimer(int MsInterval);

    int getCommonTimerStatus();
    int setCommonTimerStatus(int Status);

    int getMessageTimerStatus();
    int setMessageTimerStatus(int Status);

    int CreateTimerThread();

    static int CommonInterval;
	static int MessageInterval;

	static CsMutex_t CommonTimerMutex;
	static CsCond_t CommonTimerThreadConditionSignal;
	static CsMutex_t CommonTimerCompleteMutex;
	static CsCond_t CommonTimerCompleteThreadConditionSignal;

	static CsMutex_t MessageTimerMutex;
	static CsCond_t MessageTimerThreadConditionSignal;
	static CsMutex_t MessageTimerCompleteMutex;
	static CsCond_t MessageTimerCompleteThreadConditionSignal;

	static CsThread_t CommonTimerThread[2];
};

#endif // COMMONTIMER_H_INCLUDED
