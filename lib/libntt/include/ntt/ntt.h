#ifndef __NTT_NTT_H__
#define __NTT_NTT_H__

#include <core/types.h>
#include <core/memory_macros.h>

typedef s64 NttID;

typedef struct {
    MM_ARRAY_MEMBERS(NttID);
} NttIDs;

typedef void (*NttSystem) (void *);

typedef struct {
    MM_SPARSE_SET_MEMBERS(NttSystem);
} NttSystems;

#endif /* __NTT_NTT_H__ */
