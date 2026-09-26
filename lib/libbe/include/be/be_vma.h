#ifndef __BE_BE_VULKAN_MEMORY_ALLOCATION_H__
#define __BE_BE_VULKAN_MEMORY_ALLOCATION_H__
#include <be/be_arena.h>

#include <core/types.h>
#include <core/vulkan.h>

typedef struct BeVmaAllocatorCreateInfo {
    VkPhysicalDevice physicalDevice;
    VkDevice device;
    BeArena *pArena;
} BeVmaAllocatorCreateInfo;
VK_DEFINE_HANDLE(BeVmaAllocator);

typedef struct BeVmaAllocationCreateInfo {
} BeVmaAllocationCreateInfo;

VkResult beVmaCreateAllocator(BeVmaAllocatorCreateInfo *pCreateInfo, BeVmaAllocator *pAllocator);
VkResult beVmaCreateBuffer(BeVmaAllocator allocator, VkBufferCreateInfo *pBufferCreateInfo, VkBuffer *pBuffer);
VkResult beVmaCreateImage(BeVmaAllocator allocator, VkImageCreateInfo *pImageCreateInfo, VkImage *pImage);

#endif /* __BE_BE_VULKAN_MEMORY_ALLOCATION_H__ */
