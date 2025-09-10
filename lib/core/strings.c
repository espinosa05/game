#include <core/strings.h>
#include <core/memory.h>

char *CStr_FormatVariadic(char *buffer, usz size, const char *fmt, VA_Args args, usz *sizeOut)
{
    usz bytesToBeWritten = vsnprintf(buffer, size, fmt, args);
    if (sizeOut)
        *sizeOut = bytesToBeWritten;

    return buffer;
}

void CStr_FormatAllocVariadic(char **buffer, const char *fmt, VA_Args args, usz *length)
{
    usz allocSize = 0;
    CStr_FormatVariadic(*buffer, allocSize, fmt, args, &allocSize);
    *buffer = M_Alloc(BYTE_SIZE, allocSize);
    CStr_FormatVariadic(*buffer, allocSize, fmt, args, NULL);

    if (length)
        *length = allocSize;
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

    CStr_FormatVariadic(buffer, size, fmt, args, NULL);

    return buffer;
}



