#ifndef __CORE_OS_STREAMS_H__
#define __CORE_OS_STREAMS_H__

#include <core/os_file.h>
#include <core/os_socket.h>
#include <core/memory.h>
#include <core/types.h>
#include <core/utils.h>

/* not sure how much of a performance penalty tagged unions have.... but(t) fuck it! */
enum OS_StreamTypes {
    OS_STREAM_TYPE_IPC,
    OS_STREAM_TYPE_NETWORK,
    OS_STREAM_TYPE_FILE,
    OS_STREAM_TYPE_BYTE,
};

typedef struct {
    usz type;
    union {
        OS_SocketIPCCreateInfo  *ipcInfo;
        OS_SocketTCPCreateInfo  *tcpInfo;
        OS_FileCreateInfo       *fileInfo;
        M_BufferCreateInfo      *byteBufferInfo;
    } raw;
} OS_StreamCreateInfo;

typedef struct {
    usz type;
    union {
        OS_File         file;
        OS_Socket       socket;
        M_Buffer        byteBuffer;
    } raw;
} OS_Stream;

enum OS_StreamStatusCodes {
    OS_STREAM_STATUS_SUCCESS = 0,

    OS_STREAM_STATUS_COUNT,
#define OS_STREAM_STATUS_UNKNOWN OS_STREAM_STATUS_COUNT
};
typedef usz OS_StreamStatus;

OS_StreamStatus OS_StreamOpen(OS_Stream *stream, const OS_StreamCreateInfo *info);
OS_StreamStatus OS_StreamClose(OS_Stream *stream);
OS_StreamStatus OS_StreamCopy(OS_Stream *dst, OS_Stream *src, usz ammount);
OS_StreamStatus OS_StreamRead(OS_Stream *stream, void *buffer, const usz bufferSize, const usz size);
OS_StreamStatus OS_StreamWrite(OS_Stream *stream, void *buffer, const usz bufferSize, const usz size);
OS_StreamStatus OS_StreamPrintf(OS_Stream *stream, const char *fmt, ...);

#if 0
OS_StreamStatus OS_StreamOpenAsync(OS_Stream *stream, const OS_StreamCreateInfo *info);
OS_StreamStatus OS_StreamCloseAsync(OS_Stream *stream);
OS_StreamStatus OS_StreamCopyAsync(OS_Stream *dst, OS_Stream *src, usz ammount);
OS_StreamStatus OS_StreamReadAsync(OS_Stream *stream, void *buffer, const usz size);
OS_StreamStatus OS_StreamWriteAsync(OS_Stream *stream, const void *buffer, const usz size);
OS_StreamStatus OS_StreamPrintfAsync(OS_Stream *stream, const char *fmt, ...);
#endif
#endif /* __CORE_OS_STREAMS_H__ */
