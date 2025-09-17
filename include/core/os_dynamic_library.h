#ifndef __OS_DYNAMIC_LIBRARY_H__
#define __OS_DYNAMIC_LIBRARY_H__

#include <core/platform.h>

#if defined(CORE_PLATFORM_LINUX)
typedef void *OS_Library;

#elif defined(CORE_PLATFORM_WINDOWS)
#include <libloaderapi.h>
typedef HMODULE OS_Library;
#else
#   error "not supported"
#endif /**/

typedef struct {
    char *path;
    char *name;
} OS_LibraryCreateInfo;

void OS_OpenLibrary(OS_Library *lib, const OS_LibraryCreateInfo *info);
void OS_LoadLibrarySymbol(OS_Library *lib, void **dst, const char *symbol);
void OS_CloseLibrary(OS_Library lib);


#endif /* __OS_DYNAMIC_LIBRARY_H__ */
