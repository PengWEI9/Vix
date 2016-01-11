/* Copyright (c) 2011. All rights reserved. ERG Transit Systems (HK) Ltd.
**
** CommonTimer.cpp - time class
**
** Version Date    Name       Reason
** 1.0     12/12/11  Gerry       Created
**
*/
#include "CommonTimer.h"

CsThread_t CommonTimer::CommonTimerThread[2];

CsMutex_t CommonTimer::CommonTimerMutex;
CsCond_t CommonTimer::CommonTimerThreadConditionSignal;
CsMutex_t CommonTimer::CommonTimerCompleteMutex;
CsCond_t CommonTimer::CommonTimerCompleteThreadConditionSignal;

CsMutex_t CommonTimer::MessageTimerMutex;
CsCond_t CommonTimer::MessageTimerThreadConditionSignal;
CsMutex_t CommonTimer::MessageTimerCompleteMutex;
CsCond_t CommonTimer::MessageTimerCompleteThreadConditionSignal;
int CommonTimer::CommonInterval;
int CommonTimer::CommonTimerStatus;
int CommonTimer::MessageInterval;
int CommonTimer::MessageTimerStatus;

CommonTimer::CommonTimer()
{
}

CommonTimer::~CommonTimer()
{
}

extern "C"
{
    // this C function will be used to receive the thread and pass it back to the Thread instance
	void* CommonTimerThread_Catch(void* arg)
		{
		    CommonTimer ct;

		    while(1)
		    {
		        CsCondWait(&ct.CommonTimerThreadConditionSignal,&ct.CommonTimerMutex);
                for(int i = 0; i < ct.CommonInterval && ct.getCommonTimerStatus() != 0; i++)
                {
                    if(ct.getCommonTimerStatus() == 1)
                    usleep(1000);
                }

                if(ct.getCommonTimerStatus() == 1)
                {
                    CsDebug(1,(1,"Message Common Timeout"));
                    ct.setCommonTimerStatus(2);
                }
                CsCondSignal(&ct.CommonTimerCompleteThreadConditionSignal);
            }
            /*  NEVER GET HERE! */
            return  NULL;
		}

		void* MessageTimerThread_Catch(void* arg)
		{
            CommonTimer ct;

		    while(1)
		    {
		        CsCondWait(&ct.MessageTimerThreadConditionSignal,&ct.MessageTimerMutex);
		        CsDebug(1,(1,"Message Timer Status - %d",ct.getMessageTimerStatus()));
                for(int i = 0; i < ct.MessageInterval&& ct.getMessageTimerStatus() != 0; i++)
                {
                    if(ct.getMessageTimerStatus() == 1)
                        usleep(1000);
                }

                if(ct.getMessageTimerStatus() == 1)
                {
                    CsDebug(1,(1,"Message Timer Timeout"));
                    ct.setMessageTimerStatus(2);
                }
                CsCondSignal(&ct.MessageTimerCompleteThreadConditionSignal);
		    }
            /*  NEVER GET HERE! */
            return  NULL;
		}
}
int CommonTimer::CreateTimerThread()
{
    CsThrCreate( NULL, 0, CommonTimerThread_Catch, (void *)NULL, CSTHR_BOUND, &CommonTimerThread[0]);
    CsThrCreate( NULL, 0, MessageTimerThread_Catch, (void *)NULL, CSTHR_BOUND, &CommonTimerThread[1]);
    return 0;
}
// method which starts the new thread
void CommonTimer::StartCommonTimer(int MsInterval)
{
    setCommonTimerStatus(1);
	CommonInterval = MsInterval;
	CsCondSignal(&CommonTimerThreadConditionSignal);
}

void CommonTimer::StartMessageTimer(int MsInterval)
{
    setMessageTimerStatus(1);
    MessageInterval = MsInterval;
    CsCondSignal(&MessageTimerThreadConditionSignal);
}

int CommonTimer::getCommonTimerStatus()
{
    return CommonTimerStatus;
}
int CommonTimer::setCommonTimerStatus(int CurrentStatus)
{
    CommonTimerStatus = CurrentStatus;
    return 0;
}

int CommonTimer::getMessageTimerStatus()
{
    return MessageTimerStatus;
}
int CommonTimer::setMessageTimerStatus(int CurrentStatus)
{
    MessageTimerStatus = CurrentStatus;
    return 0;
}

