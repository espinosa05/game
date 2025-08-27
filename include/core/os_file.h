#ifndef __CORE_OS_FILE_H__
#define __CORE_OS_FILE_H__

#include <core/platform.h>
#include <core/cstd.h>
#include <core/types.h>

#ifdef PLATFORM_LINUX
typedef int OS_File;
typedef int OS_FilePerm;
enum filePerm {
    FPERM_READ = O_RDONLY,
    FPERM_WRITE = O_WRONLY,
    FPERM_READ_AND_WRITE = O_RDWR,
};

#elif defined(PLATFORM_WINDOWS)

typedef HANDLE OS_File;
typedef DWORD OS_FilePerm;
enum filePerm {
    FPERM_READ = GENERIC_READ,
    FPERM_WRITE = GENERIC_WRITE,
    FPERM_READ_AND_WRITE = GENERIC_READ | GENERIC_WRITE,
};

#endif

typedef struct {
    const char *path;
    OS_FilePerm perm;
} OS_FileCreateInfo;

extern OS_File *OS_STDIN;
extern OS_File *OS_STDOUT;
extern OS_File *OS_STDERR;

void OS_FileOpen(OS_File *f, const OS_FileCreateInfo *info);
void OS_FileCreate(const OS_File *f, const OS_FileCreateInfo *info);
void OS_FileRead(const OS_File *f, void *buffer, const usz bufferSize, const usz bytes);
void OS_FileWrite(const OS_File *f, const void *buffer, const usz bufferSize, const usz bytes);
void OS_FilePrintf(const OS_File *f, const char *fmt, ...);
void OS_FileClose(OS_File *f);
void OS_FileFlush(const OS_File *f);

#endif /* __CORE_OS_FILE_H__ */
