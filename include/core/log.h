#ifndef __CORE_LOG_H__
#define __CORE_LOG_H__

#include <core/cstd.h>
#include <core/os_streams.h>
#include <core/macros.h>
#include <core/strings.h>

#define ANSI_COLOR_BLACK        "\033[30m"
#define ANSI_COLOR_RED          "\033[31m"
#define ANSI_COLOR_GREEN        "\033[32m"
#define ANSI_COLOR_YELLOW       "\033[33m"
#define ANSI_COLOR_BLUE         "\033[34m"
#define ANSI_COLOR_MAGENTA      "\033[35m"
#define ANSI_COLOR_CYAN         "\033[36m"
#define ANSI_COLOR_WHITE        "\033[37m"
#define ANSI_COLOR_DEFAULT      "\033[39m"
#define ANSI_COLOR_BG_BLACK     "\033[40m"
#define ANSI_COLOR_BG_RED       "\033[41m"
#define ANSI_COLOR_BG_GREEN     "\033[42m"
#define ANSI_COLOR_BG_YELLOW    "\033[43m"
#define ANSI_COLOR_BG_BLUE      "\033[44m"
#define ANSI_COLOR_BG_MAGENTA   "\033[45m"
#define ANSI_COLOR_BG_CYAN      "\033[46m"
#define ANSI_COLOR_BG_WHITE     "\033[47m"
#define ANSI_COLOR_BG_DEFAULT   "\033[48m"

#define MAX_LOG_LINE_LENGTH 256 + NULL_TERM_SIZE

#define LOC_FMT "%s:%d:%s"
#define LOC_ARG __FILE__, __LINE__, __func__

#define F_LOG(file, ...) OS_StreamPrintf(file, __VA_ARGS__)
#define LOG(...) F_LOG(STDERR_STREAM, __VA_ARGS__)

#define F_LOG_T(file, color, tag, ...)                                                \
    MACRO_START                                                                       \
        F_LOG(file, "[" color tag ANSI_COLOR_DEFAULT "] [" LOC_FMT "] : ", LOC_ARG);  \
        F_LOG(file, __VA_ARGS__);                                                     \
        F_LOG(file, "\n");                                                            \
    MACRO_END

#define LOG_T(tag, color, ...) F_LOG_T(OS_STDERR, tag, color, __VA_ARGS__)

/* not a huge fan of all the compound buffers convoluting the stack */
#define INFO_LOG_STR(...)  CStr_Format((char[MAX_LOG_LINE_LENGTH]) {0}, MAX_LOG_LINE_LENGTH, __VA_ARGS__)
#define WARN_LOG_STR(...)  CStr_Format((char[MAX_LOG_LINE_LENGTH]) {0}, MAX_LOG_LINE_LENGTH, __VA_ARGS__)
#define ERROR_LOG_STR(...) CStr_Format((char[MAX_LOG_LINE_LENGTH]) {0}, MAX_LOG_LINE_LENGTH, __VA_ARGS__)

/* not sure if scoping really helps here */
#define INFO_LOG(...)   { F_LOG(INFO_LOG_STR(__VA_ARGS__)) }
#define WARN_LOG(...)   { F_LOG(WARN_LOG_STR(__VA_ARGS__)) }
#define ERROR_LOG(...)  { F_LOG(ERROR_LOG_STR(__VA_ARGS__)) }


#endif /* __CORE_LOG_H__ */
