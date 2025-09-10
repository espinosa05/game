#include <core/os.h>
#include <core/os_streams.h>
#include <core/os_file.h>
#include <core/os_socket.h>

#include <core/utils.h>
#include <core/strings.h>
#include <core/types.h>
#include <core/memory.h>
#include <core/cstd.h>
#include <core/linux.h>
#include <core/vulkan.h>
#include <core/os_vulkan.h>



#define OS_LINUX_SYSCALL_SUCCESS(rc) (rc >= 0)

/* static function declaration start */
static char *GetErrnoStr(int errnoVal);
static bool SupportedWindowResolution(OS_WindowCreateInfo *info);
static OS_StreamStatus OpenFileStream(OS_Stream *stream, const OS_FileCreateInfo *info);
static OS_StreamStatus OpenTCPSocketStream(OS_Stream *stream, const OS_SocketTCPCreateInfo *info);
static OS_StreamStatus OpenIPCSocketStream(OS_Stream *stream, const OS_SocketIPCCreateInfo *info);
static OS_SocketStatus OS_SocketErrnoCodeToStatus(sz statusCode, usz socketFunction, usz errnoVal);
static void OS_FileInitProcIOHandles(void) __FUNC_ATTR_CONSTRUCTOR__;
/* static function declaration end */

/* global data start */
OS_File *OS_STDIN;
OS_File *OS_STDOUT;
OS_File *OS_STDERR;

static OS_File __OS_StdIn;
static OS_File __OS_StdOut;
static OS_File __OS_StdErr;
/* global data end */

OS_WeStatus OS_WeInit(OS_WindowEnvironment *we)
{
    xcb_setup_t           *xcbSetup;
    xcb_screen_iterator_t xcbScreens;

    we->xcbConnection   = xcb_connect(NULL, NULL);
    xcbSetup            = (xcb_setup_t *)xcb_get_setup(we->xcbConnection);
    xcbScreens          = xcb_setup_roots_iterator(xcbSetup);
    we->xcbScreen       = xcbScreens.data;

    return OS_WE_STATUS_SUCCESS;
}

OS_WeStatus OS_WeShutdown(OS_WindowEnvironment *we)
{
    xcb_disconnect(we->xcbConnection);

    return OS_WE_STATUS_SUCCESS;
}

#define DEFAULT_BORDER_WIDTH        0
#define DEFAULT_BORDER_COLOR        0
#define DEFAULT_BACKGROUND_COLOR    0

OS_WeStatus OS_WeWindowCreate(OS_WindowEnvironment *we, OS_Window *win, OS_WindowCreateInfo *info)
{
    if (!SupportedWindowResolution(info)) {
        return OS_WE_STATUS_WINDOW_RESOLUTION_NOT_SUPPORTED;
    }

    win->xcbWindow = xcb_generate_id(we->xcbConnection);
    xcb_create_window(we->xcbConnection, XCB_COPY_FROM_PARENT,
                      win->xcbWindow,
                      we->xcbScreen->root,
                      info->xPos,
                      info->yPos,
                      info->width,
                      info->height,
                      DEFAULT_BORDER_WIDTH,
                      XCB_WINDOW_CLASS_INPUT_OUTPUT,
                      we->xcbScreen->root_visual,
                      0,
                      NULL);

    OS_WeWindowChangeTitle(we, win, info->initialTitle);

    return OS_WE_STATUS_SUCCESS;
}

void OS_WeWindowShow(OS_WindowEnvironment *we, OS_Window *win)
{
    xcb_map_window(we->xcbConnection, win->xcbWindow);
    xcb_flush(we->xcbConnection);
}


void OS_WeWindowHide(OS_WindowEnvironment *we, OS_Window *win)
{
    xcb_unmap_window(we->xcbConnection, win->xcbWindow);
    xcb_flush(we->xcbConnection);
}

#define UTF8_WIDTH 8

void OS_WeWindowChangeTitle(OS_WindowEnvironment *we, OS_Window *win, const char *title)
{
    xcb_intern_atom_cookie_t xcbCookie;
    xcb_intern_atom_reply_t *xcbReply;
    xcb_atom_t windowName;
    xcb_atom_t utf8String;
    usz titleLength;

    {
        xcbCookie   = xcb_intern_atom(we->xcbConnection, 0, CONST_STRLEN("_NET_WM_NAME"), "_NET_WM_NAME");
        xcbReply    = xcb_intern_atom_reply(we->xcbConnection, xcbCookie, NULL);
        windowName  = xcbReply->atom;
        M_Free(xcbReply);
    }

    {
        xcbCookie   = xcb_intern_atom(we->xcbConnection, 0, CONST_STRLEN("UTF8_STRING"), "UTF8_STRING");
        xcbReply    = xcb_intern_atom_reply(we->xcbConnection, xcbCookie, NULL);
        utf8String  = xcbReply->atom;
        M_Free(xcbReply);
    }

    titleLength = CStr_Length(title);
    xcb_change_property(we->xcbConnection,
                        XCB_PROP_MODE_REPLACE,
                        win->xcbWindow,
                        windowName,
                        utf8String,
                        UTF8_WIDTH,
                        titleLength,
                        title);

    xcb_change_property(we->xcbConnection,
                        XCB_PROP_MODE_REPLACE,
                        win->xcbWindow,
                        XCB_ATOM_WM_NAME,
                        XCB_ATOM_STRING,
                        UTF8_WIDTH,
                        titleLength,
                        title);

    xcb_flush(we->xcbConnection);
}

OS_WeStatus OS_WeWindowClose(OS_WindowEnvironment *we, OS_Window *win)
{
    xcb_destroy_window(we->xcbConnection, win->xcbWindow);
    xcb_flush(we->xcbConnection);

    return OS_WE_STATUS_SUCCESS;
}

void OS_WeGetRequiredExtensions(OS_WindowEnvironmentExtensions *weExtensions)
{
    static const char *requiredXcbExtensions[] = {
        VK_KHR_SURFACE_EXTENSION_NAME,
        VK_KHR_XCB_SURFACE_EXTENSION_NAME,
    };

    weExtensions->count = ARRAY_SIZE(requiredXcbExtensions);
    weExtensions->names = requiredXcbExtensions;
}

OS_SurfaceStatus OS_SurfaceCreate(OS_Surface *surface, const OS_SurfaceCreateInfo *info)
{
    VkResult status = -1;
    VkXcbSurfaceCreateInfoKHR createInfo = {
        .sType      = VK_STRUCTURE_TYPE_XCB_SURFACE_CREATE_INFO_KHR,
        .connection = info->we->xcbConnection,
        .window     = info->win->xcbWindow,
    };
    VkInstance instance = *info->instance;
    status = vkCreateXcbSurfaceKHR(instance, &createInfo, NULL, &surface->handle);
    if (status != VK_SUCCESS) {
        return OS_SURFACE_STATUS_FAILED_TO_CREATE_SURFACE;
    }

    return OS_SURFACE_STATUS_SUCCESS;
}

enum threadStatus { THREAD_FAILED = -1, THREAD_CREATED = 0, };
OS_ThreadStatus OS_ThreadSpawn(OS_Thread *thr, OS_ThreadFunction func, void *arg)
{
    struct clone_args cloneArgs = {0};
    struct rlimit stackSize = {0};
    void *stackBuffer = NULL;
    OS_ThreadStatus ret = OS_THREAD_STATUS_FAILURE;

    /* getrlimit does not return useful errors */
    getrlimit(RLIMIT_STACK, &stackSize);
    stackBuffer = mmap(NULL, stackSize.rlim_cur, PROT_READ | PROT_WRITE,
                       MAP_GROWSDOWN | MAP_PRIVATE | MAP_ANONYMOUS | MAP_STACK, -1, 0);

    ASSERT_RT(stackBuffer != MAP_FAILED, "failed to allocate stack for new process");

    cloneArgs.flags = CLONE_VM | CLONE_FS | CLONE_FILES | CLONE_SIGHAND;
    cloneArgs.exit_signal = SIGCHLD;
    cloneArgs.stack = (u64)stackBuffer;
    cloneArgs.stack_size = stackSize.rlim_cur;

    thr->pid = syscall(SYS_clone3, &cloneArgs, sizeof(cloneArgs));

    switch (thr->pid) {
    case THREAD_FAILED:
        ret = OS_THREAD_STATUS_FAILED_TO_CREATE;
        break;
    case THREAD_CREATED:
        INFO_LOG("thread started with PID: %d", thr->pid);
        func(arg);
        ret = OS_THREAD_STATUS_SUCCESS;
        break;
    }
    return ret;
}

OS_ThreadStatus OS_ThreadJoin(OS_Thread *thr, void *ret)
{
    TODO("implement OS_ThreadJoin");
    UNUSED(ret);

    int status = 0;
    waitpid(thr->pid, &status, 0);

    return OS_THREAD_STATUS_SUCCESS;;
}

void OS_TimeGetCurrent(OS_Time *time)
{
    ASSERT_RT(0 == clock_gettime(CLOCK_REALTIME, &time->end),
              "couldn't get system time: "STR_FMT, GetErrnoStr(errno));
}

void OS_TimeStart(OS_Time *time)
{
    ASSERT_RT(0 == clock_gettime(CLOCK_MONOTONIC, &time->start),
              "couldn't measure time start: "STR_FMT, GetErrnoStr(errno));
}

void OS_TimeEnd(OS_Time *time)
{
    ASSERT_RT(0 == clock_gettime(CLOCK_MONOTONIC, &time->end),
              "couldn't measure time end: "STR_FMT, GetErrnoStr(errno));
}

usz OS_TimeGetNsec(OS_Time *time)
{
    return (time->end.tv_sec - time->start.tv_sec) * 1000000000 + (time->end.tv_nsec - time->start.tv_nsec);
}

u64 OS_TimeGetUsec(OS_Time *time)
{
    return OS_TimeGetNsec(time)/1000;
}

f64 OS_TimeGetMsec(OS_Time *time)
{
    return ((f64)OS_TimeGetUsec(time))/1000;
}

usz OS_TimeGetSec(OS_Time *time)
{
    return ((usz)OS_TimeGetMsec(time))/1000;
}

void OS_SleepUsec(u32 usec)
{
    usleep(usec);
}

void OS_SleepMsec(f32 msec)
{
    OS_SleepUsec((u32)(msec * 1000));
}

void OS_SleepSec(u32 sec)
{
    /* we don't call 'os_sleep_msec',
     * as 1000000 usec appears to trigger EINVAL on some systems
     * */
    sleep((unsigned int)sec);
}

usz OS_GetMaxPathLength(char *dir)
{
    return pathconf(dir, _PC_PATH_MAX);
}

OS_StreamStatus OS_StreamOpen(OS_Stream *stream, const OS_StreamCreateInfo *info)
{
    OS_StreamStatus st = OS_STREAM_STATUS_UNKNOWN;
    switch (info->type) {
    case OS_STREAM_TYPE_IPC:
        st = OpenIPCSocketStream(stream, info->raw.ipcInfo);
        break;
    case OS_STREAM_TYPE_NETWORK:
        st = OpenTCPSocketStream(stream, info->raw.tcpInfo);
        break;
    case OS_STREAM_TYPE_FILE:
        st = OpenFileStream(stream, info->raw.fileInfo);
        break;
    }

    return st;
}

OS_StreamStatus OS_StreamClose(OS_Stream *stream)
{
    switch (stream->type) {
    case OS_STREAM_TYPE_IPC:
    case OS_STREAM_TYPE_NETWORK:
        OS_SocketClose(&stream->raw.socket);
        break;
    case OS_STREAM_TYPE_FILE:
        OS_FileClose(&stream->raw.file);
        break;
    }

    return OS_STREAM_STATUS_SUCCESS;
}

#define COPY_CHUNK_SIZE 64
OS_StreamStatus OS_StreamCopy(OS_Stream *dst, OS_Stream *src, usz ammount)
{
    for (usz i = 0; i < ammount/COPY_CHUNK_SIZE; ++i) {
        u8 chunk[COPY_CHUNK_SIZE];
        OS_StreamRead(src, chunk, sizeof(*chunk), COPY_CHUNK_SIZE);
        OS_StreamWrite(dst, chunk, sizeof(*chunk), COPY_CHUNK_SIZE);
    }

    for (usz i = 0; i < ammount%COPY_CHUNK_SIZE; ++i) {
        u8 bytes;
        OS_StreamRead(src, &bytes, sizeof(bytes), BYTE_SIZE);
        OS_StreamWrite(dst, &bytes, sizeof(bytes), BYTE_SIZE);
    }

    return OS_STREAM_STATUS_SUCCESS;
}

OS_StreamStatus OS_StreamRead(OS_Stream *stream, void *buffer, const usz bufferSize, const usz size)
{
    switch (stream->type) {
    case OS_STREAM_TYPE_IPC:
    case OS_STREAM_TYPE_NETWORK:
        OS_SocketReceiveData(&stream->raw.socket, buffer, bufferSize, size, 0);
        break;
    case OS_STREAM_TYPE_FILE:
        OS_FileRead(&stream->raw.file, buffer, bufferSize, size);
        break;
    case OS_STREAM_TYPE_BYTE:
        M_BufferRead(&stream->raw.byteBuffer, buffer, bufferSize, size);
        break;
    }

    return OS_STREAM_STATUS_SUCCESS;
}

OS_StreamStatus OS_StreamWrite(OS_Stream *stream, void *buffer, const usz bufferSize, const usz size)
{
    switch (stream->type) {
    case OS_STREAM_TYPE_IPC:
    case OS_STREAM_TYPE_NETWORK:
        OS_SocketSendData(&stream->raw.socket, buffer, bufferSize, size, 0);
        break;
    case OS_STREAM_TYPE_FILE:
        OS_FileWrite(&stream->raw.file, buffer, bufferSize, size);
        break;
    case OS_STREAM_TYPE_BYTE:
        M_BufferWrite(&stream->raw.byteBuffer, buffer, bufferSize, size);
        break;
    }

    return OS_STREAM_STATUS_SUCCESS;
}

OS_StreamStatus OS_StreamPrintf(OS_Stream *stream, const char *fmt, ...)
{
    TODO("implement Variadic I/O functions");
    char *buffer = NULL;
    VA_Args args;
    usz messageLength;

    VA_Start(args, fmt);
    CStr_FormatAllocVariadic(&buffer, fmt, args, &messageLength);
    OS_StreamWrite(stream, buffer, messageLength, messageLength);

    M_Free(buffer);

    return OS_STREAM_STATUS_SUCCESS;
}

enum { OS_LINUX_STDIN = STDIN_FILENO, OS_LINUX_STDOUT = STDOUT_FILENO, OS_LINUX_STDERR = STDERR_FILENO, };
static void OS_FileInitProcIOHandles(void)
{
    __OS_StdIn  = OS_LINUX_STDIN;
    __OS_StdOut = OS_LINUX_STDOUT;
    __OS_StdErr = OS_LINUX_STDERR;

    OS_STDIN    = &__OS_StdIn;
    OS_STDOUT   = &__OS_StdOut;
    OS_STDERR   = &__OS_StdErr;
}

void OS_FileOpen(OS_File *f, const OS_FileCreateInfo *info)
{
    TODO("error handling");
    *f = open(info->path, info->perm);
    ASSERT_RT(OS_LINUX_SYSCALL_SUCCESS(*f), "syscall open failed!");
}

void OS_FileCreate(OS_File *f, const OS_FileCreateInfo *info)
{
    TODO("error handling");
    u32 fdPerm = info->perm | O_CREAT; /* the user does not need to know this happens */
    *f = open(info->path, fdPerm, S_IWUSR);
    ASSERT_RT(OS_LINUX_SYSCALL_SUCCESS(*f), "open syscall failed!");
}

void OS_FileRead(const OS_File *f, void *buffer, const usz bufferSize, const usz bytes)
{
    TODO("error handling");
    ASSERT(bytes <= bufferSize, "out of bounds read!");
    sz st = read(*f, buffer, bytes);
    ASSERT(OS_LINUX_SYSCALL_SUCCESS(st), "read syscall failed!");
}

void OS_FileWrite(const OS_File *f, const void *buffer, const usz bufferSize, const usz bytes)
{
    ASSERT(bytes <= bufferSize, "out of bounds write!");
    sz st = write(*f, buffer, bytes);
    ASSERT(OS_LINUX_SYSCALL_SUCCESS(st), "syscall write failed!");
}

void OS_FilePrintf(const OS_File *f, const char *fmt, ...)
{
    char *msg;
    VA_Args args;
    usz msgLength = 0;

    VA_Start(args, fmt);
    CStr_FormatAllocVariadic(&msg, fmt, args, NULL);
    OS_FileWrite(f, msg, msgLength, msgLength);
    M_Free(msg);
}

void OS_FileClose(OS_File *f)
{
    TODO("error handling");
    sz st = close(*f);
    ASSERT(OS_LINUX_SYSCALL_SUCCESS(st), "syscall close failed!");
    *f = OS_FILE_INVALID;
}

void OS_FileFlush(const OS_File *f)
{
    IMPL();
    UNUSED(f);
}

enum socketFunctions {
    SOCK_FN_SOCKET,
    SOCK_FN_BIND,
    SOCK_FN_LISTEN,
    SOCK_FN_ACCEPT,
    SOCK_FN_CONNECT,
    SOCK_FN_SEND,
    SOCK_FN_RECV,
    SOCK_FN_SENDTO,
    SOCK_FN_RECVFROM,
    SOCK_FN_SENDMSG,
    SOCK_FN_RECVMSG,
    SOCK_FN_SHUTDOWN,
    SOCK_FN_CLOSE,
    SOCK_FN_GETSOCKOPT,
    SOCK_FN_SETSOCKOPT,
    SOCK_FN_GETSOCKNAME,
    SOCK_FN_GETPEERNAME,
    SOCK_FN_SELECT,
    SOCK_FN_POLL,
    SOCK_FN_EPOLL_CREATE,
    SOCK_FN_EPOLL_CTL,
    SOCK_FN_EPOLL_WAIT,
    SOCK_FN_ACCEPT4,
};

#define SOCKOPT_ENABLE &(int) {1}
#define SOCKOPT_DISABLE &(int) {0}
OS_SocketStatus OS_SocketEnableSockOpt(const OS_Socket *sock, sz sockOpt)
{
    sz st = setsockopt(*sock, SOL_SOCKET, sockOpt, SOCKOPT_ENABLE, sizeof(int));
    return OS_SocketErrnoCodeToStatus(st, SOCK_FN_SETSOCKOPT, errno);
}

OS_SocketStatus OS_SocketDisableSockOpt(const OS_Socket *sock, sz sockOpt)
{
    sz st = setsockopt(*sock, SOL_SOCKET, sockOpt, SOCKOPT_DISABLE, (socklen_t) {sizeof(int)});
    return OS_SocketErrnoCodeToStatus(st, SOCK_FN_SETSOCKOPT, errno);
}

OS_SocketStatus OS_SocketGetSockOpt(const OS_Socket *sock, sz sockOpt, sz *value)
{
    sz st = getsockopt(*sock, SOL_SOCKET, sockOpt, value, &(socklen_t) {sizeof(value)});
    return OS_SocketErrnoCodeToStatus(st, SOCK_FN_GETSOCKOPT, errno);
}

OS_SocketStatus OS_SocketSetSockObj(const OS_Socket *sock, sz sockOpt, const void *sockObj, usz size)
{
    sz st = setsockopt(*sock, SOL_SOCKET, sockOpt, sockObj, size);
    return OS_SocketErrnoCodeToStatus(st, SOCK_FN_SETSOCKOPT, errno);
}

OS_SocketStatus OS_SocketGetSockObj(const OS_Socket *sock, sz sockOpt, void *sockObj, usz size)
{
    sz st = getsockopt(*sock, SOL_SOCKET, sockOpt, sockObj, &(socklen_t) {size});
    return OS_SocketErrnoCodeToStatus(st, SOCK_FN_GETSOCKOPT, errno);
}

OS_SocketStatus OS_SocketCreateTCPSocket(OS_Socket *sock, const OS_SocketTCPCreateInfo *info)
{
    struct sockaddr_in socketAddress = {0};
    sz st = 0;

    st = socket(AF_INET, SOCK_STREAM, 0);
    if (!OS_LINUX_SYSCALL_SUCCESS(st))
        return OS_SocketErrnoCodeToStatus(st, SOCK_FN_SOCKET, errno);

    *sock = st;
    socketAddress.sin_family        = AF_INET;
    socketAddress.sin_addr.s_addr   = info->netAddr->ipv4;
    socketAddress.sin_port          = info->netAddr->port;

    st = bind(*sock, (struct sockaddr *)&socketAddress, sizeof(socketAddress));
    if (!OS_LINUX_SYSCALL_SUCCESS(st))
        return OS_SocketErrnoCodeToStatus(st, SOCK_FN_BIND, errno);

    st = listen(*sock, info->queueLength);

    return OS_SocketErrnoCodeToStatus(st, SOCK_FN_LISTEN, errno);
}

OS_SocketStatus OS_SocketCreateIPCSocket(OS_Socket *sock, const OS_SocketIPCCreateInfo *info)
{
    UNUSED(sock);
    UNUSED(info);
    IMPL();
    return OS_SOCKET_STATUS_SUCCESS;
}

OS_SocketStatus OS_SocketBindPath(const OS_Socket *sock)
{
    UNUSED(sock);
    return OS_SOCKET_STATUS_SUCCESS;
}

OS_SocketStatus OS_SocketAccept(const OS_Socket *server, OS_Socket *client, OS_NetworkAddress *address)
{
    sz st = 0;
    struct sockaddr_in socketAddress = {0};
    st = accept(*server, (struct sockaddr *)&socketAddress, &(socklen_t) {sizeof(socketAddress)});
    if (!OS_LINUX_SYSCALL_SUCCESS(st))
        return OS_SocketErrnoCodeToStatus(st, SOCK_FN_ACCEPT, errno);

    *client = st;
    address->ipv4 = socketAddress.sin_addr.s_addr;
    address->port = socketAddress.sin_port;

    return OS_SOCKET_STATUS_SUCCESS;
}

OS_SocketStatus OS_SocketConnect(const OS_Socket *client, OS_Socket *server, const OS_NetworkAddress *address)
{
    usz st = 0;
    struct sockaddr_in socketAddress = {
        .sin_addr.s_addr = address->ipv4,
        .sin_port = address->port,
    };

    st = connect(*client, (struct sockaddr *)&socketAddress, sizeof(socketAddress));
    *server = st;

    return OS_SocketErrnoCodeToStatus(st, SOCK_FN_CONNECT, errno);
}

OS_SocketStatus OS_SocketClose(const OS_Socket *sock)
{
    sz st = 0;

    st = close(*sock);

    return OS_SocketErrnoCodeToStatus(st, SOCK_FN_CLOSE, errno);
}

OS_SocketStatus OS_SocketShutdown(const OS_Socket *sock, usz how)
{
    sz st = 0;
    st = shutdown(*sock, how);

    return OS_SocketErrnoCodeToStatus(st, SOCK_FN_SHUTDOWN, errno);
}

OS_SocketStatus OS_SocketReceiveData(const OS_Socket *sock, char *buffer, const usz bufferSize, usz size, usz flags)
{
    sz st = 0;

    if (size > bufferSize) {
        return OS_SOCKET_STATUS_OUT_OF_BOUNDS_READ;
    }

    st = recv(*sock, buffer, size, flags);

    return OS_SocketErrnoCodeToStatus(st, SOCK_FN_RECV, errno);
}

OS_SocketStatus OS_SocketSendData(const OS_Socket *sock, char *buffer, const usz bufferSize, usz size, usz flags)
{
    sz st = 0;

    if (size > bufferSize) {
        return OS_SOCKET_STATUS_OUT_OF_BOUNDS_WRITE;
    }
    st = send(*sock, buffer, size, flags);

    return OS_SocketErrnoCodeToStatus(st, SOCK_FN_SEND, errno);
}

/* big ugly function... don't blame me. Blame horrily outdated POSIX errors */

static OS_SocketStatus OS_SocketErrnoCodeToStatus(sz statusCode, usz socketFunction, usz errnoVal)
{
    if (statusCode == 0)
        return OS_SOCKET_STATUS_SUCCESS;

    switch (socketFunction) {
    case SOCK_FN_SOCKET:
        switch (errnoVal) {
        case EMFILE: return OS_SOCKET_STATUS_HOST_OUT_OF_SOCKETS;
        case ENOBUFS: return OS_SOCKET_STATUS_HOST_OUT_OF_MEMORY;
        case ENOMEM: return OS_SOCKET_STATUS_HOST_OUT_OF_MEMORY;
        }
        UNREACHABLE();
    case SOCK_FN_BIND:
        switch (errnoVal) {
        case EACCES: return OS_SOCKET_STATUS_PROTECTED_ADDRESS;
        case EADDRINUSE: return OS_SOCKET_STATUS_ADDRESS_AND_PORT_IN_USE;
        case EBADF: return OS_SOCKET_STATUS_INVALID_SOCKET;
        case EINVAL: return OS_SOCKET_STATUS_SOCKET_ALREADY_BOUND;
        }
        UNREACHABLE();
    case SOCK_FN_LISTEN:
        switch (errnoVal) {
        case EADDRINUSE: return OS_SOCKET_STATUS_ADDRESS_AND_PORT_IN_USE;
        case EBADF: return OS_SOCKET_STATUS_INVALID_SOCKET;
        case ENOTSOCK: return OS_SOCKET_STATUS_INVALID_SOCKET;
        }
        UNREACHABLE();
    case SOCK_FN_ACCEPT:
        switch (errnoVal) {
        case EWOULDBLOCK: return OS_SOCKET_STATUS_EMPTY_CONNECTION_QUEUE;
#if !defined(CORE_PLATFORM_LINUX)
        case EAGAIN: return OS_SOCKET_STATUS_EMPTY_CONNECTION_QUEUE;
#endif /* CORE_PLATFORM_LINUX */
        case EBADF: return OS_SOCKET_STATUS_INVALID_SOCKET;
        case ECONNABORTED: return OS_SOCKET_STATUS_CONNECTION_WAS_ABORTED;
        case EINTR: return OS_SOCKET_STATUS_CONNECTION_WAS_INTERRUPTED;
        case EINVAL: return OS_SOCKET_STATUS_SOCKET_IS_NOT_LISTENING;
        case EMFILE: return OS_SOCKET_STATUS_PROCESS_OUT_OF_FDS;
        case ENFILE: return OS_SOCKET_STATUS_HOST_OUT_OF_FDS;
        case ENOBUFS: return OS_SOCKET_STATUS_HOST_OUT_OF_SOCKETS;
        case ENOMEM: return OS_SOCKET_STATUS_HOST_OUT_OF_SOCKETS;
        case ENOTSOCK: return OS_SOCKET_STATUS_INVALID_SOCKET;
        }
        UNREACHABLE();
    case SOCK_FN_CONNECT:
        switch (errnoVal) {
        case EPERM: return OS_SOCKET_STATUS_BLOCKED_BY_FIREWALL;
        case EADDRINUSE: return OS_SOCKET_STATUS_ADDRESS_AND_PORT_IN_USE;
        case EALREADY: return OS_SOCKET_STATUS_PREVIOUS_CONNECT_INCOMPLETE;
        case EBADF: return OS_SOCKET_STATUS_INVALID_SOCKET;
        case ECONNREFUSED: return OS_SOCKET_STATUS_SERVICE_NOT_AVAILABLE;
        case EINPROGRESS: TODO("implement EINPROGRESS for connect()"); ABORT();
        case EINTR: return OS_SOCKET_STATUS_CONNECTION_WAS_INTERRUPTED;
        case EISCONN: return OS_SOCKET_STATUS_SOCKET_IS_ALREADY_CONNECTED;
        case ENOTSOCK: return OS_SOCKET_STATUS_INVALID_SOCKET;
        case ETIMEDOUT: return OS_SOCKET_STATUS_CONNECTION_TIMEOUT;
        }
        UNREACHABLE();
    case SOCK_FN_SEND:
        switch (errnoVal) {
        case EAGAIN: TODO("implement EAGAIN for send()"); ABORT();
        case EALREADY: TODO("implement EALREADY for send()"); ABORT();
        case EBADF: return OS_SOCKET_STATUS_INVALID_SOCKET;
        case ECONNRESET: return OS_SOCKET_STATUS_PEER_RESET_CONNECTION;
        case EDESTADDRREQ: return OS_SOCKET_STATUS_NO_TARGET_SPECIFIED;
        case EINTR: return OS_SOCKET_STATUS_SEND_GOT_INTERRUPTED_BY_OS;
        case ENOMEM: return OS_SOCKET_STATUS_HOST_OUT_OF_MEMORY;
        case ENOTCONN: return OS_SOCKET_STATUS_SOCKET_IS_NOT_CONNECTED;
        case ENOTSOCK: return OS_SOCKET_STATUS_INVALID_SOCKET;
        }
        UNREACHABLE();
    case SOCK_FN_RECV:
        switch (errnoVal) {
        case EBADF: return OS_SOCKET_STATUS_INVALID_SOCKET;
        case ENOTSOCK: return OS_SOCKET_STATUS_INVALID_SOCKET;
        case ECONNREFUSED: return OS_SOCKET_STATUS_PEER_DOWN;
        case EINTR: return OS_SOCKET_STATUS_RECV_GOT_INTERRUPTED_BY_OS;
        case ENOTCONN: return OS_SOCKET_STATUS_SOCKET_IS_NOT_CONNECTED;
        }
        UNREACHABLE();
    case SOCK_FN_SENDTO:
        UNREACHABLE();
    case SOCK_FN_RECVFROM:
        UNREACHABLE();
    case SOCK_FN_SENDMSG:
        UNREACHABLE();
    case SOCK_FN_RECVMSG:
        UNREACHABLE();
    case SOCK_FN_SHUTDOWN:
        UNREACHABLE();
    case SOCK_FN_CLOSE:
        switch (errnoVal) {
        case EBADF: return OS_SOCKET_STATUS_INVALID_SOCKET;
        case EINTR: return OS_SOCKET_STATUS_CLOSE_GOT_INTERRUPTED_BY_OS;
        case EIO: TODO("check if EIO makes sense upon closing socket!");
                  return OS_SOCKET_STATUS_INPUT_OUTPUT_ERROR;
        }
        UNREACHABLE();
    case SOCK_FN_GETSOCKOPT:
        UNREACHABLE();
    case SOCK_FN_SETSOCKOPT:
        UNREACHABLE();
    case SOCK_FN_GETSOCKNAME:
        UNREACHABLE();
    case SOCK_FN_GETPEERNAME:
        UNREACHABLE();
    case SOCK_FN_SELECT:
        UNREACHABLE();
    case SOCK_FN_POLL:
        UNREACHABLE();
    case SOCK_FN_EPOLL_CREATE:
        UNREACHABLE();
    case SOCK_FN_EPOLL_CTL:
        UNREACHABLE();
    case SOCK_FN_EPOLL_WAIT:
        UNREACHABLE();
    case SOCK_FN_ACCEPT4:
        UNREACHABLE();
    }

    UNREACHABLE();
}

static char *g_OS_SocketStatusCodeStrings[] = {
    ENUM_STR_ENTRY(OS_SOCKET_STATUS_SUCCESS),
    ENUM_STR_ENTRY(OS_SOCKET_STATUS_HOST_OUT_OF_FDS),
    ENUM_STR_ENTRY(OS_SOCKET_STATUS_PROCESS_OUT_OF_FDS),
    ENUM_STR_ENTRY(OS_SOCKET_STATUS_PROCESS_OUT_OF_SOCKETS),
    ENUM_STR_ENTRY(OS_SOCKET_STATUS_HOST_OUT_OF_SOCKETS),
    ENUM_STR_ENTRY(OS_SOCKET_STATUS_HOST_OUT_OF_MEMORY),
    ENUM_STR_ENTRY(OS_SOCKET_STATUS_PROTECTED_ADDRESS),
    ENUM_STR_ENTRY(OS_SOCKET_STATUS_ADDRESS_AND_PORT_IN_USE),
    ENUM_STR_ENTRY(OS_SOCKET_STATUS_INVALID_SOCKET),
    ENUM_STR_ENTRY(OS_SOCKET_STATUS_SOCKET_ALREADY_BOUND),
    ENUM_STR_ENTRY(OS_SOCKET_STATUS_EMPTY_CONNECTION_QUEUE),
    ENUM_STR_ENTRY(OS_SOCKET_STATUS_CONNECTION_WAS_ABORTED),
    ENUM_STR_ENTRY(OS_SOCKET_STATUS_CONNECTION_WAS_INTERRUPTED),
    ENUM_STR_ENTRY(OS_SOCKET_STATUS_SOCKET_IS_NOT_LISTENING),
    ENUM_STR_ENTRY(OS_SOCKET_STATUS_SOCKET_IS_NOT_CONNECTED),
    ENUM_STR_ENTRY(OS_SOCKET_STATUS_BLOCKED_BY_FIREWALL),
    ENUM_STR_ENTRY(OS_SOCKET_STATUS_PREVIOUS_CONNECT_INCOMPLETE),
    ENUM_STR_ENTRY(OS_SOCKET_STATUS_SERVICE_NOT_AVAILABLE),
    ENUM_STR_ENTRY(OS_SOCKET_STATUS_CONNECTION_TIMEOUT),
    ENUM_STR_ENTRY(OS_SOCKET_STATUS_RECEIVE_TIMEOUT),
    ENUM_STR_ENTRY(OS_SOCKET_STATUS_CLOSE_GOT_INTERRUPTED_BY_OS),
    ENUM_STR_ENTRY(OS_SOCKET_STATUS_INPUT_OUTPUT_ERROR),
    ENUM_STR_ENTRY(OS_SOCKET_STATUS_PEER_RESET_CONNECTION),
    ENUM_STR_ENTRY(OS_SOCKET_STATUS_NO_TARGET_SPECIFIED),
    ENUM_STR_ENTRY(OS_SOCKET_STATUS_PEER_DOWN),
    ENUM_STR_ENTRY(OS_SOCKET_STATUS_RECV_GOT_INTERRUPTED_BY_OS),
    ENUM_STR_ENTRY(OS_SOCKET_STATUS_UNKNOWN),
};

char *OS_SocketStringStatus(usz code)
{
    if (code > OS_SOCKET_STATUS_UNKNOWN)
        code = OS_SOCKET_STATUS_UNKNOWN;

    return g_OS_SocketStatusCodeStrings[code];
}

static char *GetErrnoStr(int errnoVal)
{
    return strerror(errnoVal);
}

static bool SupportedWindowResolution(OS_WindowCreateInfo *info)
{
    IMPL();
    UNUSED(info);
    return true;
}

static OS_StreamStatus OpenFileStream(OS_Stream *stream, const OS_FileCreateInfo *info)
{
    OS_FileOpen(&stream->raw.file, info);
    return OS_STREAM_STATUS_SUCCESS;
}

static OS_StreamStatus OpenTCPSocketStream(OS_Stream *stream, const OS_SocketTCPCreateInfo *info)
{
    OS_SocketCreateTCPSocket(&stream->raw.socket, info);
    return OS_STREAM_STATUS_SUCCESS;
}

static OS_StreamStatus OpenIPCSocketStream(OS_Stream *stream, const OS_SocketIPCCreateInfo *info)
{
    OS_SocketCreateIPCSocket(&stream->raw.socket, info);
    return OS_STREAM_STATUS_SUCCESS;
}


