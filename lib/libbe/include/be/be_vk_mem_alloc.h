#ifndef __BE_BE_VULKAN_MEMORY_ALLOCATOR_H__
#define __BE_BE_VULKAN_MEMORY_ALLOCATOR_H__

#include <core/types.h>
#include <core/vulkan.h>

typedef struct {
} BeVmaMemoryArenaCreateInfo;

typedef struct {

} BeVmaMemoryArenaChunk;

typedef struct {
    BeVmaMemoryArenaChunk *fist;
    BeVmaMemoryArenaChunk *last;
} BeVmaMemoryArena;

VkResult beVmaCreateMemoryArena(BeVkMemoryArenaCreateInfo *info, BeVkMemoryArena *arena);
VkResult beVmaDestroyMemoryArena(const BeVkMemoryArena arena);


#endif /* __BE_BE_VULKAN_MEMORY_ALLOCATOR_H__ */
