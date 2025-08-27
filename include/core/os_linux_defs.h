#ifndef __CORE_OS_LINUX_DEFS_H__
#define __CORE_OS_LINUX_DEFS_H__

#include <core/cstd.h>

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xos.h>
#include <X11/Xatom.h>

typedef struct {
    Display *xDisplay;
    int     xScreen;
} OS_WindowManager;

typedef struct {
    Window  xWindow;
    GC      *xGc;
} OS_Window;

typedef struct {
    pid_t pid;
} OS_Thread;


/* we just define the native handle for a socket. No additional API*/
typedef int OS_Socket;


typedef struct {
    struct timespec start;
    struct timespec end;
} OS_Time;

#endif /* __CORE_OS_LINUX_DEFS_H__ */
