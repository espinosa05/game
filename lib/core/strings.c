#include <core/strings.h>
#include <core/memory.h>

char *CStr_FormatVariadic(char *buffer, usz size, const char *fmt, VA_Args args)
{
    vsnprintf(buffer, size, fmt, args);

    return buffer;
}

void CStr_FormatAllocVariadic(char **buffer, const char *fmt, VA_Args args, usz *length)
{
    usz lengthVal = vasprintf(buffer, fmt, args);

    if (length)
        *length = lengthVal;
}

void CStr_FormatAlloc(char **buffer, const char *fmt, ...)
{
    VA_Args args;
    VA_Start(args, fmt);

    CStr_FormatAllocVariadic(buffer, fmt , args, NULL);
}

char *CStr_Format(char *buffer, usz size, const char *fmt, ...)
{
    VA_Args args;
    VA_Start(args, fmt);

    CStr_FormatVariadic(buffer, size, fmt, args);

    return buffer;
}



