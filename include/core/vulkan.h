#ifndef __CORE_VULKAN_H__
#define __CORE_VULKAN_H__

#include <core/platform.h>
#include <vulkan/vulkan.h>

#if defined(CORE_PLATFORM_LINUX)
#   include <vulkan/vulkan_xlib.h>
#   include <vulkan/vulkan_wayland.h>
#elif defined(CORE_PLATFORM_WINDOWS)
#   error "windows not supported right now!"
#   include <vulkan/vulkan_win32.h>
#endif /* CORE_PLATFORM_LINUX */

#endif /* __CORE_VULKAN_H__ */
