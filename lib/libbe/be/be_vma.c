#include <be/be_vma.h>
#include <core/memory.h>
#include <core/utils.h>

/* static function declaration start */
/* static function declaration end */

struct BeVmaAllocator_T {
    u32 static_heap_index;
    u32 transient_heap_index;

    usz static_heap_allocated;
    usz transient_heap_allocated;

    usz static_heap_used;
    usz transient_heap_used;

    VkDeviceMemory static_memory;
    VkDeviceMemory transient_memory;
};


VkResult beVmaCreateAllocator(BeVmaAllocatorCreateInfo *pCreateInfo, BeVmaAllocator *pAllocator)
{
    BeVmaAllocator allocator = be_arena_alloc(pCreateInfo->pArena, sizeof(*allocator), 1);

    VkPhysicalDeviceMemoryBudgetPropertiesEXT memory_budget_properties = {0};
    memory_budget_properties.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MEMORY_BUDGET_PROPERTIES_EXT;
    memory_budget_properties.pNext = NULL;

    VkPhysicalDeviceMemoryProperties2 memory_properties2 = {0};
    memory_properties2.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MEMORY_PROPERTIES_2;
    memory_properties2.pNext = &memory_budget_properties;
    vkGetPhysicalDeviceMemoryProperties2(pCreateInfo->physicalDevice, &memory_properties2);

    VkPhysicalDeviceMemoryProperties memory_properties = memory_properties2.memoryProperties;

    const VkMemoryPropertyFlags required_static_flags = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;

    b32 static_found = FALSE;

    for (u32 i = 0; i < memory_properties.memoryTypeCount; ++i) {
        VkMemoryType type = memory_properties.memoryTypes[i];
        usz heap_size = memory_budget_properties.heapUsage[type.heapIndex];
        if ((type.propertyFlags & required_static_flags) == required_static_flags) {
            static_found = TRUE;
            allocator->static_heap_index = type.heapIndex;
            allocator->static_heap_used = 0;
            allocator->static_heap_allocated = CORE_MIN(MB(1500), heap_size);
            allocator->static_heap_allocated /= 3;
            break;
        }
    }

    if (!static_found) {
        return VK_ERROR_OUT_OF_DEVICE_MEMORY;
    }

    *pAllocator = allocator;

    return VK_SUCCESS;
}


VkResult beVmaCreateBuffer(BeVmaAllocator allocator, VkBufferCreateInfo *pBufferCreateInfo, VkBuffer *pBuffer)
{
    return VK_SUCCESS;
}

VkResult beVmaCreateImage(BeVmaAllocator allocator, VkImageCreateInfo *pImageCreateInfo, VkImage *pImage)
{
    return VK_SUCCESS;
}

