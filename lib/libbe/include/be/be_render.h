#ifndef __BE_RENDER_H__
#define __BE_RENDER_H__

#include <be/be_engine.h>
#include <core/types.h>
#include <core/memory_macros.h>
#include <core/vulkan.h>

typedef struct {
    VkInstance handle;
} BeVulkanInstance;

typedef struct {
    VkPhysicalDevice handle;
} BeVulkanPhysicalDevice;

typedef struct {
    MM_ARRAY_MEMBERS(VkPhysicalDevice);
} BeVulkanPhysicalDevices;

#define EACH_BE_VULKAN_PHYSICAL_DEVICES(device, devices) VkPhysicalDevice *EACH_MM_ARRAY(device, devices)

typedef struct {
    VkSurfaceKHR handle;
} BeVulkanSurface;

typedef struct {
    VkDevice handle;
} BeVulkanDevice;

typedef struct {
    u32 index;
    VkQueue handle;
} BeVulkanQueue;

typedef struct {
    MM_ARRAY_MEMBERS(VkQueueFamilyProperties2);
} BeVulkanQueueFamilyProperties;

typedef struct {
    MM_ARRAY_MEMBERS(VkImage);
} BeVulkanImages;

typedef struct {
    VkSurfaceFormatKHR handle;
} BeVulkanSurfaceFormat;

typedef struct {
    MM_ARRAY_MEMBERS(VkSurfaceFormatKHR);
} BeVulkanSurfaceFormats;

typedef struct {
    VkSwapchainKHR handle;
    BeVulkanImages swapchainImages;
} BeVulkanSwapchain;

typedef struct {
    MM_ARRAY_MEMBERS(const char *const);
} BeVulkanExtensions;

typedef struct {
    BeVulkanInstance instance;
    BeVulkanSurface surface;

    BeVulkanPhysicalDevice physical_device;
    BeVulkanDevice logical_device;

    BeVulkanQueue graphics_queue;
} BeRenderContext;

#define BE_RENDER_LAYER_SPEC BE_LAYER_SPEC(be_render)

BeRenderContext *be_render_on_attach(BeEngine *be);
void be_render_on_update(BeEngine *be, BeRenderContext *render);
void be_render_on_suspend(BeEngine *be, BeRenderContext *render);
void be_render_on_activate(BeEngine *be, BeRenderContext *render);
void be_render_on_event(BeEngine *be, BeRenderContext *render);
void be_render_on_detach(BeEngine *be, BeRenderContext *render);

#endif /* __BE_RENDER_H__ */
