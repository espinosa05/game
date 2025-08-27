#include <core/strings.h>
#include <core/memory.h>

char *CStr_FormatVariadic(char *buffer, usz size, char *fmt, VA_Args args)
{
    vsnprintf(buffer, size, fmt, args);

    return buffer;
}

void CStr_FormatAllocVariadic(char **buffer, char *fmt, VA_Args args)
{
    vasprintf(buffer, fmt, args);
}

void CStr_FormatAlloc(char **buffer, char *fmt, ...)
{
    VA_Args args;
    VA_Start(fmt, args);

    CStr_FormatAllocVariadic(buffer, size, fmt ,args);
}

char *CStr_Format(char *buffer, usz size, char *fmt, ...)
{
    VA_Args args;
    VA_Start(fmt, args);

    CStr_FormatVariadic(buffer, size, fmt, args);

    return buffer;
}

