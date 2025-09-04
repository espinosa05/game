#include <core/vulkan.h>
#include <core/utils.h>

typedef struct {
    VkInstance  *i;
} CR_VulkanContext;

typedef struct {
    /* optional argument! */
    const M_Arena *arena;
    const M_Arena *arena;
} CR_VulkanContextCreateInfo;

void CR_VulkanInit(CR_VulkanContext *vkContext, CR_VulkanContextCreateInfo *vkContextInfo);
void CR_VulkanShutdown(CR_VulkanContext *vkContext);
