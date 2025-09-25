#include <core/strings.h>
#include <core/memory.h>

void Str_BuilderInit(Str_Builder *sb, usz initCap)
{
    sb->count       = 0;
    sb->cap         = initCap;
    sb->elements    = M_Alloc(sizeof(*sb->elements), sb->cap);
}

void Str_BuilderAppend(Str_Builder *sb, char *str)
{
    ASSERT(sb->count + 1 <= sb->cap, "string builder corrupted!");

    if (sb->count + 1 == sb->cap) {
        sb->cap++;
        sb->elements = M_Realloc(sb->elements, sizeof(*sb->elements), sb->cap);
    }
    sb->elements[sb->count] = str;
    sb->count++;
}

void Str_BuilderToCStrAlloc(Str_Builder *sb, char **dst)
{
    char *base; /* stores the base address of the buffer */
    char *cursor; /* keeps track of the elements */
    usz elementLength;
    usz buffLength;

    /* create first element */
    elementLength = CStr_Length(sb->elements[0]);
    base = M_Alloc(sizeof(*base), elementLength + NULL_TERM_SIZE);
    cursor = base;
    buffLength = elementLength;
    CStr_Copy(cursor, sb->elements[0], elementLength);
    NULL_TERM_BUFF(cursor, elementLength);

    for (usz i = 1; i < sb->count; ++i) {
        /* reallocate the buffer */
        base = M_Realloc(base, sizeof(*sb->elements), buffLength);
        CStr_Copy(cursor, sb->elements[i], elementLength);
        NULL_TERM_BUFF(cursor, elementLength);
        cursor += elementLength;
        buffLength += elementLength;
    }

    *dst = base;
}

void Str_BuilderDelete(const Str_Builder sb)
{
    M_Free(sb.elements);
}

char *CStr_FormatVariadic(char *buffer, usz size, const char *fmt, VA_Args args, usz *sizeOut)
{
    fprintf(stderr, "calling vsnprintf with buffer=%p, size=%lu, fmt=%p, args=%p\n", buffer, size, fmt, (void *)args);

    usz bytesToBeWritten = vsnprintf(buffer, 0, fmt, args);
    if (sizeOut)
        *sizeOut = bytesToBeWritten;

    return buffer;
}

void CStr_FormatAllocVariadic(char **buffer, const char *fmt, VA_Args args, usz *length)
{
    usz allocSize = 0;
    allocSize = vasprintf(buffer, fmt, args);
    if (length)
        *length = allocSize;
}

void CStr_FormatAlloc(char **buffer, const char *fmt, ...)
{
    VA_Args args;
    VA_Start(args, fmt);

    CStr_FormatAllocVariadic(buffer, fmt, args, NULL);
}

char *CStr_Format(char *buffer, usz size, const char *fmt, ...)
{
    VA_Args args;
    VA_Start(args, fmt);

    CStr_FormatVariadic(buffer, size, fmt, args, NULL);

    return buffer;
}



