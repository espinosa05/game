#ifndef __CORE_OS_H__
#define __CORE_OS_H__

#include <core/types.h>
#include <core/utils.h>
#include <core/os_streams.h>

#define X_POS_CENTERED (sz)(-1)
#define Y_POS_CENTERED (sz)(-1)


typedef u32 OS_WeStatus;
enum osWeStatusCodes {
    OS_WE_STATUS_SUCCESS = 0,
    OS_WE_STATUS_COULDNT_START,
    OS_WE_STATUS_WINDOW_RESOLUTION_NOT_SUPPORTED,

    OS_WE_STATUS_UNKNOWN,
    OS_WE_STATUS_COUNT,
};

typedef struct {
    usz         width;
    usz         height;
    usz         xPos;
    usz         yPos;

    const char  *initialTitle;
} OS_WindowCreateInfo;


typedef usz OS_ThreadStatus;
enum osThreadStatusCodes {
    OS_THREAD_STATUS_SUCCESS = 0,
    OS_THREAD_STATUS_FAILED_TO_CREATE,

    OS_THREAD_STATUS_FAILURE, /* general failure */
    OS_THREAD_STATUS_UNKNOWN,
    OS_THREAD_STATUS_COUNT,
};

typedef sz OS_Tid;
typedef void (*OS_ThreadFunction) (void *);

enum osExitCodes {
    OS_EXIT_SUCCESS = 0,
    OS_EXIT_FAILURE = 1,
};

#if defined (CORE_PLATFORM_LINUX)
#   include "os_linux_defs.h"
#elif defined (CORE_PLATFORM_WINDOWS)
#   include "os_windows_defs.h"
#elif defined (CORE_PLATFORM_MACOS)
#   include "os_macos_defs.h"
#endif /* CORE_PLATFORM_LINUX */

OS_WeStatus OS_WeInit(OS_WindowEnvironment *we);
OS_WeStatus OS_WeShutdown(OS_WindowEnvironment *we);

OS_WeStatus OS_WeWindowCreate(OS_WindowEnvironment *we, OS_Window *win, OS_WindowCreateInfo *info);
void OS_WeWindowShow(OS_WindowEnvironment *we, OS_Window *win);
void OS_WeWindowHide(OS_WindowEnvironment *we, OS_Window *win);
void OS_WeWindowChangeTitle(OS_WindowEnvironment *we, OS_Window *win, const char *title);
OS_WeStatus OS_WeWindowClose(OS_WindowEnvironment *we, OS_Window *win);

OS_ThreadStatus OS_ThreadSpawn(OS_Thread *thr, OS_ThreadFunction func, void *arg);
OS_ThreadStatus OS_ThreadJoin(OS_Thread *thr, void *ret);
//OS_ThreadStatus OS_ThreadGetID(OS_Thread *thr, OS_Tid *tid);
//OS_ThreadStatus OS_ThreadDetach(OS_Thread *thr);

void OS_TimeGetCurrent(OS_Time *time);
void OS_TimeStart(OS_Time *time);
void OS_TimeEnd(OS_Time *time);

u64 OS_TimeGetUsec(OS_Time *time);
f64 OS_TimeGetMsec(OS_Time *time);
usz OS_TimeGetSec(OS_Time *time);

void OS_SleepUsec(u32 usec);
void OS_SleepMsec(f32 msec);
void OS_SleepSec(u32 sec);

#define OS_Exit(code) exit(code)

#endif /* __V2_CORE_OS_H__ */
