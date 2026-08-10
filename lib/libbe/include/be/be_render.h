#ifndef __BE_RENDER_H__
#define __BE_RENDER_H__

#include <core/types.h>
#include <core/memory_macros.h>
#include <core/vulkan.h>

typedef struct {
    VkPhysicalDevice handle;
} BeVulkanPhysicalDevice;

typedef struct {
    MM_ARRAY_MEMBERS(BeVulkanPhysicalDevice);
} BeVulkanPhysicalDevices;

#define EACH_BE_VULKAN_PHYSICAL_DEVICES(device, devices)  BeVulkanPhysicalDevice *EACH_MM_ARRAY(device, devices)

typedef struct {
    VkSurfaceKHR handle;
} BeVulkanSurface;

typedef struct {
    VkDevice handle;
} BeVulkanDevice;

typedef struct {
    u32 index;
    VkQueue queue;
} BeVulkanQueue;

typedef struct {
    MM_ARRAY_MEMBERS(VkQueueFamilyProperties);
} BeVulkanQueueFamilyProperties;

typedef struct {
    MM_ARRAY_MEMBERS(VkImage);
} BeVulkanImages;

typedef struct {
    VkSwapchainKHR handle;
    BeVulkanImages swapchainImages;
} BeVulkanSwapchain;

typedef struct {
    MM_ARRAY_MEMBERS(char *);
} BeVulkanExtensions;

typedef struct {
    VkInstance instance;
    BeVulkanDevice logical_device;
    BeVulkaPhysicalDevice physical_device;
    BeVulkanSurface surface;

    BeVulkanQueue graphics_queue;
    BeVulkanQueue transfer_queue;

} BeRenderContext;

#define BE_RENDER_LAYER_SPEC BE_LAYER_SPEC(be_render)

BeRenderContext *be_render_on_attach(BeEngine *be);
void be_render_on_update(BeEngine *be, BeRenderContext *render);
void be_render_on_suspend(BeEngine *be, BeRenderContext *render);
void be_render_on_activate(BeEngine *be, BeRenderContext *render);
void be_render_on_event(BeEngine *be, BeRenderContext *render);
void be_render_on_detach(BeEngine *be, BeRenderContext *render);

#endif /* __BE_RENDER_H__ */
