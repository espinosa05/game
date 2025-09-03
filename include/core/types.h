#ifndef __TYPES_H__
#define __TYPES_H__

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <inttypes.h>
#include <stdarg.h>

typedef ptrdiff_t   sz;
typedef size_t      usz;

typedef uint8_t     u8;
typedef uint16_t    u16;
typedef uint32_t    u32;
typedef uint64_t    u64;

typedef int8_t      s8;
typedef int16_t     s16;
typedef int32_t     s32;
typedef int64_t     s64;

typedef float       f32;
typedef double      f64;

enum                b32Values { FALSE = 0, TRUE = 1 };
typedef uint32_t    b32; /* boolean type */

typedef const char  *cstr_t;    /* looks pretty while casting */

#define U8_MAX      ((1<<8)-1)
#define U16_MAX     ((1<<16)-1)
#define U32_MAX     ((1<<32)-1)
#define U64_MAX     ((1<<64)-1)

#define S8_MAX      (U8_MAX/2)
#define S16_MAX     (U16_MAX/2)
#define S32_MAX     (U32_MAX/2)
#define S64_MAX     (U32_MAX/2)

#define SZ_FMT      "%"PRIi64   /* weird and hardcoded */
#define USZ_FMT     "%"PRIu64   /* applies to this aswell */

#define PTR_FMT     "%"PRIuPTR

#define U8_FMT      "%"PRIu8
#define U16_FMT     "%"PRIu16
#define U32_FMT     "%"PRIu32
#define U64_FMT     "%"PRIu64

#define S8_FMT      "%"PRIi8
#define S16_FMT     "%"PRIi16
#define S32_FMT     "%"PRIi32
#define S64_FMT     "%"PRIi64

#define U8_X_FMT    "%02"PRIx8
#define U16_X_FMT   "%04"PRIx16
#define U32_X_FMT   "%08"PRIx32
#define U64_X_FMT   "%016"PRIx64

#define STR_FMT     "%s"

#endif /* __TYPES_H__ */
