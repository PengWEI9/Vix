#ifndef _OLS_PRIV_H
#define _OLS_PRIV_H

#include "olsmsg.h"

// unprotected send and receive

extern int32_t
_OlsMsgSnd(
    OLSMSGHD *hMsg,
    int32_t msqid, void *msgp, size_t msgsz,
    const CsTime_t *pAbstime);

extern int32_t
_OlsMsgRcv(
    OLSMSGHD *hMsg, int32_t msqid,
    void *msgp, size_t msgsz, int32_t msgtyp, int32_t msgflg, size_t *pSize,
    const CsTime_t *pAbstime);

#endif // _OLS_PRIV_H
