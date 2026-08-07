#ifndef __NTT_H__
#define __NTT_H__

#include <core/memory_macros.h>

typedef u64 NttID;
typedef struct {
    MM_ARRAY_MEMBERS(NttID);
} NttIDs;

#endif /* __NTT_H__ */
