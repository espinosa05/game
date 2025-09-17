#ifndef __CORE_STRINGS_H__
#define __CORE_STRINGS_H__

#include <core/types.h>
#include <core/utils.h>
#include <core/cstd.h>
#include <core/variadic.h>

typedef struct {
    char *start;
    char *end;
} Str_View;

typedef struct {
    char    **elements;
    usz     count;
    usz     cap;
} Str_Builder;

void Str_BuilderInit(Str_Builder *sb, usz initCap);
void Str_BuilderAppend(Str_Builder *sb, char *str);
void Str_BuilderToCStrAlloc(Str_Builder *sb, char **dst);
void Str_BuilderDelete(const Str_Builder sb);

#define NULL_TERM_SIZE 1
#define NULL_TERM_BUFF(s, n) (s)[n] = '\0'
#define CONST_STRLEN(str)   (sizeof(str)/sizeof(*str)-NULL_TERM_SIZE)

#define CStr_Length strlen
#define CStr_Compare(s1, s2) (0 == strcmp(s1, s2))
#define CStr_Concat strncat
#define CStr_Copy strncpy

char *CStr_Format(char *buffer, usz size, const char *fmt, ...);
void CStr_FormatAlloc(char **buffer, const char *fmt, ...);
char *CStr_FormatVariadic(char *buffer, usz size, const char *fmt, VA_Args args, usz *sizeOut);
void CStr_FormatAllocVariadic(char **buffer, const char *fmt, VA_Args args, usz *length);

#endif /* __CORE_STRINGS_H__ */
