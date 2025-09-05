#include <core/utils.h>
#include <core/os_vulkan.h>
#include <core/memory.h>
#include <core/vulkan.h>

void OS_WmGetRequiredExtensions(OS_WindowManagerExtensions *wmExtensions)
{
    const char *requiredXlibExtensions[] = {
        VK_KHR_SURFACE_EXTENSION_NAME,
        VK_KHR_XCB_SURFACE_EXTENSION_NAME,
    };

    wmExtensions->count = ARRAY_SIZE(requiredXlibExtensions);
    wmExtensions->names = M_Alloc(sizeof(*wmExtensions->names), wmExtensions->count);
    M_Copy(wmExtensions->names, requiredXlibExtensions, sizeof(char *)*wmExtensions->count);
}

void OS_WmCleanupRequiredExtensions(const OS_WindowManagerExtensions wmExtensions)
{
    M_Free(wmExtensions.names);
}

OS_SurfaceStatus OS_SurfaceCreate(OS_Surface *surface, const OS_SurfaceCreateInfo *info)
{
    VkResult status = -1;
    VkXlibSurfaceCreateInfoKHR createInfo = {
        .sType  = VK_STRUCTURE_TYPE_XLIB_SURFACE_CREATE_INFO_KHR,
        .dpy    = info->wm->xDisplay,
        .window = info->win->xWindow,
    };
    VkInstance instance = *info->instance;
    status = vkCreateXlibSurfaceKHR(instance, &createInfo, NULL, &surface->handle);
    if (status != VK_SUCCESS) {
        return OS_SURFACE_STATUS_FAILED_TO_CREATE_SURFACE;
    }

    return OS_SURFACE_STATUS_SUCCESS;
}
