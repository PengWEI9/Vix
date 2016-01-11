#ifndef _SIGNALS_H_
#define _SIGNALS_H_

#include <stdlib.h> /* for NULL */

typedef void (*SignalHandler_f) (int sig);

#ifdef __cplusplus
int SigInit(SignalHandler_f handler = NULL);
#else
int SigInit(SignalHandler_f handler);
#endif

#endif /* _SIGNALS_H_ */
