#ifndef __CORE_CSTD_H__
#define __CORE_CSTD_H__

#include <assert.h>
#include <ctype.h>
#include <errno.h>
#include <float.h>
#include <inttypes.h>
#include <limits.h>
#include <locale.h>
#include <math.h>
#include <setjmp.h>
#include <signal.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <tgmath.h>
#include <time.h>
#include <wchar.h>
#include <wctype.h>
#include <complex.h>

#include <core/platform.h>

#ifdef CORE_PLATFORM_LINUX

#define _GNU_SOURCE
#include <alloca.h>
#include <dirent.h>
#include <dlfcn.h>
#include <execinfo.h>
#include <fcntl.h>
#include <fnmatch.h>
#include <getopt.h>
#include <glob.h>
#include <iconv.h>
#include <langinfo.h>
#include <libgen.h>
#include <malloc.h>
#include <mntent.h>
#include <nl_types.h>
#include <paths.h>
#include <pthread.h>
#include <regex.h>
#include <sched.h>
#include <search.h>
#include <semaphore.h>
#include <spawn.h>
#include <strings.h>
#include <sys/epoll.h>
#include <sys/file.h>
#include <sys/ioctl.h>
#include <sys/ipc.h>
#include <sys/mman.h>
#include <sys/msg.h>
#include <sys/resource.h>
#include <sys/select.h>
#include <sys/shm.h>
#include <sys/socket.h>
#include <sys/syscall.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/times.h>
#include <sys/types.h>
#include <sys/un.h>
#include <sys/utsname.h>
#include <sys/wait.h>
#include <termios.h>
#include <unistd.h>
#include <netinet/in.h>
#endif /* PLATFORM_LINUX */

#ifdef CORE_PLATFORM_WINDOWS
#include <conio.h>
#include <direct.h>
#include <io.h>
#include <process.h>
#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <sys/locking.h>
#include <sys/stat.h>
#include <sys/timeb.h>
#include <sys/types.h>
#include <tchar.h>
#endif /* PLATFORM_WINDOWS */

#endif /* __CORE_CSTD_H__ */
