#include <core/memory.h>

void M_ArenaInit(M_Arena *arena, const M_ArenaCreateInfo *info)
{
    arena->memAvail = info->memSize;

    if (info->external) {
        arena->buffer = info->buffer;
    } else {
        arena->heap = TRUE;
        arena->buffer = M_Alloc(BYTE_SIZE, info->bufferSize);
    }
}

void *M_ArenaAlloc(M_Arena *arena, usz size, usz count, M_ArenaStatusCode)
{
    void *buff = arena->buff + arena->used;
    arena->used += size * count;

    if (arena->used > arena->bufferSize)
        return NULL;

    return buff;
}

void M_ArenaDestroy(M_Arena *arena)
{
    if (arena->heap)
        M_Free(arena->buffer);
}

void M_BufferInit(M_Buffer *buff, const M_BufferInfo *info)
{
    buff->base = info->buffer;
    buff->size = info->size;
}

M_BufferStatus M_BufferRead(M_Buffer *buffer, void *src, usz dstCap, usz ammount)
{
    if (buffer->cursor + ammount > buffer->size)
        return M_BUFFER_OUT_OF_MEMORY;

    M_Copy(src, buffer->base, ammount);
    return M_BUFFER_SUCCESS;
}

M_BufferStatus M_BufferWrite(M_Buffer *buffer, void *dst, usz dstCap, usz ammount)
{
    if (buffer->cursor + ammount > buffer->size)
        return M_BUFFER_OUT_OF_MEMORY;

    M_Copy(buffer->base, src, ammount);
    return M_BUFFER_SUCCESS;
}

void InitPersistentArena(void)
{

}
