#ifndef __CORE_LINUX_UTILS_H__
#define __CORE_LINUX_UTILS_H__

#include <unistd.h>
#include <stddef.h>
#include <errno.h>
#include <string.h>

#define UNLIKELY(c)     __builtin_expect(!!(c), true)
#define LIKELY(c)       __builtin_expect(!!(c), false)

#define INLINE          __attribute__((always_inline)) inline
//#define PRINTF(f, as)   __attribute__((printf(f, as)))

#define getErrnoStr(v) strerror(v)
#define BUILTIN_UNREACHABLE() __builtin_unreachable()


#endif /* __CORE_LINUX_UTILS_H__ */
