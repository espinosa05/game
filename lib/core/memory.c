#include <core/memory.h>

void M_ArenaInit(M_Arena *arena, const M_ArenaCreateInfo *info)
{
    arena->memAvail = info->memSize;

    if (info->external) {
        arena->buffer = info->buffer;
    } else {
        arena->heap = TRUE;
        arena->buffer = M_Alloc(BYTE_SIZE, info->memSize);
    }
}

void *M_ArenaAlloc(M_Arena *arena, usz size, usz count)
{
    void *buff = (u8 *)arena->buffer + arena->memUsed;
    arena->memUsed += size * count;

    if (arena->memUsed > arena->memAvail)
        return NULL;

    return buff;
}

void M_ArenaDestroy(M_Arena *arena)
{
    if (arena->heap)
        M_Free(arena->buffer);
}

void M_BufferInit(M_Buffer *buff, const M_BufferCreateInfo *info)
{
    buff->base = info->buffer;
    buff->size = info->size;
}

M_BufferStatus M_BufferRead(M_Buffer *buffer, void *src, usz dstCap, usz ammount)
{
    if (buffer->cursor + ammount > buffer->size)
        return M_BUFFER_STATUS_OUT_OF_MEMORY;

    if (dstCap < ammount)
        return M_BUFFER_STATUS_OUT_OF_BOUNDS_READ;

    M_Copy(src, buffer->base, ammount);
    return M_BUFFER_STATUS_SUCCESS;
}

M_BufferStatus M_BufferWrite(M_Buffer *buffer, void *dst, usz dstCap, usz ammount)
{
    if (buffer->cursor + ammount > buffer->size)
        return M_BUFFER_STATUS_OUT_OF_MEMORY;

    if (dstCap < ammount)
        return M_BUFFER_STATUS_OUT_OF_BOUNDS_WRITE;

    M_Copy(buffer->base, dst, ammount);
    return M_BUFFER_STATUS_SUCCESS;
}

void M_ArrayInit(M_Array *array, usz width, usz initSize)
{
    array->data     = M_Alloc(width, InitSize);
    array->width    = width;
    array->count    = 0;
    array->cap      = initSize;
}

void M_ArrayInsert(const M_Array *array, usz index, void *element)
{
    if (index > array->cap)
        array->data = M_Realloc(aray->data, index + 1);

    if (index > array->count)
        array->count = index;

    M_Copy(&array->data[index], element, array->width);
}

void M_ArrayGet(const M_Array *arry, usz index, void *element)
{
    M_Copy(element, &array->data[index], array->width);
}

void M_ArrayAppend(const M_Array *array, void *element)
{
    M_ArrayInsert(array, array->count, element);
}

void M_ArrayCopy(M_Array *src, M_Array *dst)
{
    *dst = *src;
    dst->data = M_Alloc(dst->width, dst->count);
    M_Copy(dst->data, src->data, src->width*src->count);
}

void M_ArrayDelete(M_Array *array)
{
    M_Free(array->data);
}


