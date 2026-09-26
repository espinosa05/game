#ifndef __BE_RENDER_H__
#define __BE_RENDER_H__

#include <be/be_engine.h>
#include <be/be_vma.h>
#include <core/types.h>
#include <core/memory_macros.h>

#include <core/vulkan.h>

typedef struct {
    VkInstance handle;
} BeVulkanInstance;

typedef struct {
    VkPhysicalDevice handle;
    VkFormat swapchain_format;
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
    MM_ARRAY_MEMBERS(VkSurfaceFormatKHR);
} BeVulkanSurfaceFormats;

typedef struct {
    MM_ARRAY_MEMBERS(VkImage);
} BeVulkanImages;

typedef struct {
    MM_ARRAY_MEMBERS(VkImageView);
} BeVulkanImageViews;

typedef struct {
    MM_ARRAY_MEMBERS(VkSemaphore);
} BeVulkanSemaphores;

typedef struct {
    u32 swapchain_width;
    u32 swapchain_height;
    b32 recreate;
    BeVulkanImages images;
    BeVulkanImageViews image_views;
    BeVulkanSemaphores render_complete_semaphores;
    VkSwapchainKHR handle;
} BeVulkanSwapchain;

typedef struct {
    MM_ARRAY_MEMBERS(const char *);
} BeVulkanExtensions;

typedef struct {
    MM_ARRAY_MEMBERS(VkExtensionProperties);
} BeVulkanExtensionProperties;

typedef struct {
    VkImage handle;
    VkImageView view;
} BeVulkanImage;

typedef struct {
    BeVulkanInstance instance;
    BeVulkanSurface surface;

    BeVulkanPhysicalDevice physical_device;
    BeVulkanDevice logical_device;

    BeVulkanQueue graphics_queue;
    BeVulkanSwapchain swapchain;

    BeVulkanImage depth_image;
    BeVmaAllocator device_allocator;
} BeVkRenderContext;

#define BE_VK_RENDER_LAYER_SPEC BE_LAYER_SPEC(be_vk_render)

BeVkRenderContext *be_vk_render_on_attach(BeEngine *be);
void be_vk_render_on_update(BeEngine *be, BeVkRenderContext *rc);
void be_vk_render_on_suspend(BeEngine *be, BeVkRenderContext *rc);
void be_vk_render_on_activate(BeEngine *be, BeVkRenderContext *rc);
void be_vk_render_on_event(BeEngine *be, BeVkRenderContext *rc);
void be_vk_render_on_detach(BeEngine *be, BeVkRenderContext *rc);

#endif /* __BE_RENDER_H__ */
