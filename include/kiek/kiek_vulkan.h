#ifndef __KIEK_VULKAN_H__
#define __KIEK_VULKAN_H__

#include <core/vulkan.h>
#include <core/memory.h>

typedef struct {
    VkDevice    handle;
} Kiek_VulkanDevice;

typedef struct {
    VkInstance          instance;
    VkSurfaceKHR        surface;
    Kiek_VulkanDevice   devices;
    M_Arena             *arena;
} Kiek_VulkanContext;

typedef struct {
    const char  *appName;
    M_Arena     *vulkanArena;
} Kiek_VulkanContextCreateInfo;

void Kiek_VulkanStartup(Kiek_VulkanContext *kvk, const Kiek_VulkanContextCreateInfo *kvkInfo);
void Kiek_VulkanShutdown(Kiek_VulkanContext *kvk);

#endif /* __KIEK_VULKAN_H__ */
