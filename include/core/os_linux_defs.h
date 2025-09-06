#ifndef __CORE_OS_LINUX_DEFS_H__
#define __CORE_OS_LINUX_DEFS_H__

#include <core/cstd.h>
#include <xcb/xcb.h>

typedef struct {
    xcb_connection_t    *xcbConnection;
    xcb_screen_t        *xcbScreen;
} OS_WindowManager;

typedef struct {
    xcb_window_t    xcbWindow;
    xcb_gcontext_t  xcbGraphicsContext;
} OS_Window;

typedef struct {
    pid_t pid;
} OS_Thread;

typedef struct {
    struct timespec start;
    struct timespec end;
} OS_Time;

#endif /* __CORE_OS_LINUX_DEFS_H__ */
