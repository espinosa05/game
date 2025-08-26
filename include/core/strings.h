#ifndef __CORE_STRINGS_H__
#define __CORE_STRINGS_H__

#include <core/types.h>
#include <core/utils.h>
#include <core/cstd.h>
#include <core/variadic.h>

#define NULL_TERM_SIZE 1
#define NULL_TERM_BUFF(s, n) (s)[n] = '\0'

#define CStr_Length(s) strlen(s)
#define CStr_Compare(s1, s2, n) (0 == strncmp(s1, s2, n))

char *CStr_Format(char *buffer, usz size, char *fmt, ...);
void CStr_FormatAlloc(char **buffer, char *fmt, ...);

char *CStr_FormatVariadic(char *buffer, usz size, char *fmt, VA_Args args);
char *CStr_FormatAllocVariadic(char **buffer, char *fmt, VA_Args args);

#endif /* __CORE_STRINGS_H__ */
