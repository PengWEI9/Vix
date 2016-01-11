/*
** Copyright (c) 2011 ERG Transit Systems (HK) Ltd.
**
**
*/

#include "signals.h"    /* ensure header is self contained */

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#if defined(linux)
#include <unistd.h>
#include <sys/mman.h>
#include <signal.h>
#include <signal.h>
#include <execinfo.h>
#endif

// need to have gdb debug library on platform for this to work
#define HAVE_BACKTRACE  1
#define crash_log       stderr

#ifdef COBRA
#ifndef arm
	#define arm
#endif
#endif

#if defined(arm)

#include <asm/sigcontext.h>

extern void * __libc_stack_end;

struct backtrace_frame_t
{
    void * fp;
    void * lr;
};

int armBacktrace(void** array, int size)
{
    void* top_frame_p;
    void* current_frame_p;
    struct backtrace_frame_t* frame_p;
    int frame_count;

    top_frame_p = __builtin_frame_address(0);
    current_frame_p = top_frame_p;
    frame_p = (struct backtrace_frame_t*)((void**)(current_frame_p)-1);
    frame_count = 0;

    if ( __builtin_return_address(0) != frame_p->lr )
    {
        fprintf(crash_log, "backtrace error: __builtin_return_address(0) %d != frame_p->lr %d\n", int(__builtin_return_address(0)), int(frame_p->lr));
        return frame_count;
    }

    if (
        (current_frame_p != NULL) &&
        (current_frame_p > (void*)&frame_count) &&
        (current_frame_p < __libc_stack_end)
       )
    {
        while (frame_count < size
               && current_frame_p != NULL
               && current_frame_p > (void*)&frame_count
               && current_frame_p < __libc_stack_end)
        {
            frame_p = (struct backtrace_frame_t*)((void**)(current_frame_p)-1);
            array[frame_count] = frame_p->lr;
            frame_count++;
            current_frame_p = frame_p->fp;
        }
    }

    return frame_count;
}

/* the parameters are specific to the target platform */
static void segv_handler (int sig, int r1, int r2, int r3, struct sigcontext sc)
{
#if HAVE_BACKTRACE
    int i;
    void *bt[128] = {0};
    int bt_size;
#endif

//    if (crash_log != 0)
    {
        fprintf(crash_log, "Segmentation fault caught!\n");
        fprintf(crash_log, "pc = 0x%lX\n", sc.arm_pc);
        fprintf(crash_log, "r0 = 0x%lX r1 = 0x%lX r2 = 0x%lX\n", sc.arm_r0, sc.arm_r1, sc.arm_r2);
        fprintf(crash_log, "r3 = 0x%lX r4 = 0x%lX r5 = 0x%lX\n", sc.arm_r3, sc.arm_r4, sc.arm_r5);
        fprintf(crash_log, "sp = 0x%lX ip = 0x%lX fault\n", sc.arm_sp, sc.arm_ip);
        fprintf(crash_log, "address    = 0x%lX\n", sc.fault_address);
        fprintf(crash_log, "error code = 0x%lX\n", sc.error_code);
    
#if HAVE_BACKTRACE
        fprintf(crash_log, "########## Backtrace ##########\n");
        fprintf(crash_log, "## note: binutil's addr2line can be used to translate address to source line\n");
        bt_size = armBacktrace(bt, sizeof(bt) / sizeof(void *));
        fprintf(crash_log, "Number of elements in backtrace: %d\n", bt_size);
		char** symbols = backtrace_symbols(bt, bt_size);
        for (i = 0; i < bt_size; i++)
        {
            fprintf(crash_log, "%d: %p %s\n", i, bt[i], (symbols[i] == 0 ? "" : symbols[i]));
        }
#endif
    }
    exit(1);
}

#endif

static SignalHandler_f UserHandler = NULL;

/* basic handler */
static void handler (int sig)
{
    fprintf(stderr, "\nsignal %d caught, quitting!\n", sig);

    if (UserHandler)
        UserHandler(sig);

    /* standard exit handlers */
    exit(1);
}

/* initialise signal handlers */

int SigInit(SignalHandler_f _handler /* = NULL */)
{
    struct sigaction segv_act;

    UserHandler = _handler;

// not using log yet
// crash_log = fopen("crashlog.txt", "a+");
    
    segv_act.sa_handler = (void (*)(int)) handler;
    sigemptyset(&segv_act.sa_mask);
    segv_act.sa_flags = 0;

#if defined(arm)
    /* 
    ** we only want these on the target platform:
    ** we want normal coredumps on a PC platform.
    */
    /* bus fault */
    if (sigaction(SIGBUS, &segv_act, NULL) < 0)
    {
        printf("error setting handler");
    }
    /* floating point exception */
    if (sigaction(SIGFPE, &segv_act, NULL) < 0)
    {
        printf("error setting handler");
    }
#endif

    /* basic ctrl-c stuff */
    if (sigaction(SIGTERM, &segv_act, NULL) < 0)
    {
        printf("error setting handler");
    }
    
    if (sigaction(SIGINT, &segv_act, NULL) < 0)
    {
        printf("error setting handler");
    }

#if defined(arm)
    /* segfault: override the default handler here */
    segv_act.sa_handler = (void (*)(int)) segv_handler;
    if (sigaction(SIGSEGV, &segv_act, NULL) < 0)
    {
        printf("error setting handler");
    }
#endif
	signal(SIGPIPE, SIG_IGN);
    return 0;
}

#if defined(TEST)
extern "C" int failfunc(int depth)
{
    char *p = (char*)1;
    int i = 0;

    if ( depth < 6 )
    {
        failfunc(depth+1);
    }
    else
    {
		//raise(SIGSEGV);
        while (1)
        {
            if (i++ == 10)
                *p = 2;

            sleep(1);
        }    
    }
}

int main()
{
    SigInit();
    failfunc(0);
}
#endif

/* end of file */

