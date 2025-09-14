#ifndef __OS_DYNAMIC_LIBRARY_H__
#define __OS_DYNAMIC_LIBRARY_H__

#include <core/platform.h>

#if defined(PLATFORM_LINUX)
typedef void *OS_Library;

#elif defined(PLATFORM_WINDOWS)
#include <libloaderapi.h>
typedef HMODULE OS_Library;
#else
#   error "not supported"
#endif /**/

typedef struct {
    const char *path;
    const char *name;
} OS_LibraryCreateInfo;

void OS_OpenLibrary(OS_Library *lib, const OS_LibraryCreateInfo *info);
void OS_LoadLibrarySymbol(const OS_Library *lib, void **dst, const char *symbol);
void OS_CloseLibrary(const OS_Library *lib);

#endif /* __OS_DYNAMIC_LIBRARY_H__ */
