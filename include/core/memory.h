#ifndef __CORE_MEMORY_H__
#define __CORE_MEMORY_H__

#include <core/utils.h>
#include <core/error_report.h>
#include <core/cstd.h> /* memset, malloc, realloc, free */

enum mem_sizes {
    BYTE_SIZE   = 1,
    KB_SIZE     = 1 << 10,
    MB_SIZE     = 1 << 20,
    GB_SIZE     = 1 << 30,
};

#ifdef GNUC
#define MEMORY_BARRIER() asm volatile ("" ::: "memory")
#else
#define MEMORY_BARRIER()
#endif

#define PAGE_SIZE (KB_SIZE*4)
#define PAGES(c) (PAGE_SIZE*c)

#define M_Alloc(s, c)       malloc(s*c)
#define M_Free(p)           free(p)
#define M_Realloc(p, s, c)  realloc(p, s*c)
#define M_Set(p, b, c)      memset(p, b, c)
#define M_Copy              memcpy

#define M_Zero(p, s)        \
    MACRO_START             \
        MEMORY_BARRIER();   \
        M_Set(p, 0, s);     \
        MEMORY_BARRIER();   \
    MACRO_END

typedef struct {
    b32 heap;
    void *buffer;
    usz memAvail;
    usz memUsed;
} M_Arena;

typedef struct {
    usz memSize;
    b32 external;
    void *buffer;
} M_ArenaCreateInfo;

void M_ArenaInit(M_Arena *arena, const M_ArenaCreateInfo *info);
void *M_ArenaAlloc(M_Arena *arena, usz size, usz count);
void M_ArenaDestroy(M_Arena *arena);

typedef struct {
    usz size;
    usz cursor;
    void *base;
} M_Buffer;

typedef struct {
    void *buffer;
    usz size;
} M_BufferCreateInfo;

enum M_BufferStatusCodes {
    M_BUFFER_STATUS_SUCCESS = 0,
    M_BUFFER_STATUS_OUT_OF_MEMORY,
};
typedef u32 M_BufferStatus;

void M_BufferInit(M_Buffer *buffer, const M_BufferCreateInfo *info);
M_BufferStatus M_BufferRead(M_Buffer *buffer, void *dst, usz dstCap, usz ammount);
M_BufferStatus M_BufferWrite(M_Buffer *buffer, void *dst, usz dstCap, usz ammount);

void InitPersistentArena(void);

#endif /* __CORE_MEMORY_H__ */
