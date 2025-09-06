#ifndef __KIEK_VULKAN_H__
#define __KIEK_VULKAN_H__

#include <core/vulkan.h>
#include <core/utils.h>
#include <core/log.h>

typedef struct {
    VkDevice                            handle;
    VkPhysicalDeviceProperties          properties;
    VkPhysicalDeviceFeatures            features;
    VkPhysicalDeviceMemoryProperties    memoryProperties;
} Kiek_VulkanDeviceContext;

typedef struct {
    /* Vulkan API instance */
    VkInstance                  instance;
    /* Vulkan Device information */
    Kiek_VulkanDeviceContext    *device;
} Kiek_VulkanContext;

typedef struct {
    u32 major;
    u32 minor;
    u32 patch;
} Kiek_ApplicationVersionHeader;

typedef struct {
    Kiek_ApplicationVersionHeader   *appVersionHeader;
    /* optional argument! */
    const char                      *appName;
} Kiek_VulkanContextCreateInfo;

#ifndef KIEK_ENGINE_VERSION_MAJOR
#   define KIEK_ENGINE_VERSION_MAJOR 0
#endif /* KIEK_ENGINE_VERSION_MAJOR */
#ifndef KIEK_ENGINE_VERSION_MINOR
#   define KIEK_ENGINE_VERSION_MINOR 0
#endif /* KIEK_ENGINE_VERSION_MINOR */
#ifndef KIEK_ENGINE_VERSION_PATCH
#   define KIEK_ENGINE_VERSION_PATCH 0
#ifndef KIEK_ENGINE_VERSION_PATCH

void Kiek_VulkanStartup(Kiek_VulkanContext *kvk, const Kiek_VulkanContextCreateInfo kvkInfo);
void Kiek_VulkanShutdown(Kiek_VulkanContext *kvk);

#endif __KIEK_VULKAN_H__
