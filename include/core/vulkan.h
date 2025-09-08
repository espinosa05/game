#ifndef __CORE_VULKAN_H__
#define __CORE_VULKAN_H__

#include <vulkan/vulkan.h>
#include <vulkan/vk_enum_string_helper.h>

#include <core/platform.h>
#if defined(CORE_PLATFORM_LINUX)
#   include <xcb/xcb.h>
#   include <xcb/xproto.h>
#   include <vulkan/vulkan_xcb.h>
#elif defined(CORE_PLATFORM_WINDOWS)
#   error "windows not supported right now!"
#   include <vulkan/vulkan_win32.h>
#endif /* CORE_PLATFORM_LINUX */

#endif /* __CORE_VULKAN_H__ */
