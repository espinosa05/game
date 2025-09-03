#ifndef __CORE_OS_VULKAN_H__
#define __CORE_OS_VULKAN_H__

#include "os.h"
#include <core/vulkan.h>

typedef struct {
    u32         count;
    const char  **names;
} OS_WindowManagerExtensions;

typedef struct {
    VkSurfaceKHR handle;
} OS_Surface;

typedef struct {
    VkInstance          *instance;
    OS_WindowManager    *wm;
    OS_Window           *win;
} OS_SurfaceCreateInfo;

typedef u32 OS_SurfaceStatus;
enum osSurfaceStatus {
    OS_SURFACE_STATUS_SUCCESS = 0,
    OS_SURFACE_STATUS_FAILED_TO_CREATE_SURFACE,

    OS_SURFACE_STATUS_COUNT,
#define OS_SURFACE_STATUS_UNKNOWN OS_SURFACE_STATUS_COUNT
};

void OS_WM_GetRequiredExtensions(OS_WindowManagerExtensions *wmExtensions);
void OS_WM_CleanupRequiredExtensions(OS_WindowManagerExtensions *wmExtensions);
OS_SurfaceStatus os_SurfaceCreate(OS_Surface *surface, OS_SurfaceCreateInfo *info);

#endif /* __V2_CORE_OS_VULKAN_H__ */
