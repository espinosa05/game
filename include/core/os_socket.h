#ifndef __CORE_OS_SOCKET_H__
#define __CORE_OS_SOCKET_H__

#include <core/platform.h>
#include <core/cstd.h>

typedef struct {
    u32 ipv4;
    u16 port;
} OS_NetworkAddress;

#if defined(PLATFORM_LINUX)
typedef int OS_Socket;
#elif defined(PLATFORM_WINDOWS)
typedef SOCKET OS_Socket;
#else
# error "platform not supported yet"
#endif /* PLATFORM_LINUX */

enum socketStatusCodes {
    OS_SOCKET_STATUS_SUCCESS = 0,
#if defined(PLATFORM_LINUX)
    OS_SOCKET_STATUS_HOST_OUT_OF_FDS,
    OS_SOCKET_STATUS_PROCESS_OUT_OF_FDS,
#elif defined(PLATFORM_WINDOWS)
/* nothing */
#endif
    OS_SOCKET_STATUS_PROCESS_OUT_OF_SOCKETS,
    OS_SOCKET_STATUS_HOST_OUT_OF_SOCKETS,
    OS_SOCKET_STATUS_HOST_OUT_OF_MEMORY,
    OS_SOCKET_STATUS_PROTECTED_ADDRESS,
    OS_SOCKET_STATUS_ADDRESS_AND_PORT_IN_USE,
    OS_SOCKET_STATUS_INVALID_SOCKET,
    OS_SOCKET_STATUS_SOCKET_ALREADY_BOUND,
    OS_SOCKET_STATUS_EMPTY_CONNECTION_QUEUE,
    OS_SOCKET_STATUS_CONNECTION_WAS_ABORTED,
    OS_SOCKET_STATUS_CONNECTION_WAS_INTERRUPTED,
    OS_SOCKET_STATUS_SOCKET_IS_NOT_LISTENING,
    OS_SOCKET_STATUS_BLOCKED_BY_FIREWALL,
    OS_SOCKET_STATUS_PREVIOUS_CONNECT_INCOMPLETE,
    OS_SOCKET_STATUS_SERVICE_NOT_AVAILABLE,
    OS_SOCKET_STATUS_SOCKET_IS_NOT_CONNECTED,
    OS_SOCKET_STATUS_SOCKET_IS_ALREADY_CONNECTED,
    OS_SOCKET_STATUS_CONNECTION_TIMEOUT,
    OS_SOCKET_STATUS_RECEIVE_TIMEOUT,
    OS_SOCKET_STATUS_CLOSE_GOT_INTERRUPTED_BY_OS,
    OS_SOCKET_STATUS_INPUT_OUTPUT_ERROR,
    OS_SOCKET_STATUS_PEER_RESET_CONNECTION,
    OS_SOCKET_STATUS_SEND_GOT_INTERRUPTED_BY_OS,
    OS_SOCKET_STATUS_NO_TARGET_SPECIFIED,
    OS_SOCKET_STATUS_PEER_DOWN,
    OS_SOCKET_STATUS_RECV_GOT_INTERRUPTED_BY_OS,

    OS_SOCKET_STATUS_UNKNOWN,
    OS_SOCKET_STATUS_COUNT,
};
typedef usz OS_SocketStatus;

#define OS_SOCKET_FAILURE(st) !OS_SOCKET_SUCCESS(st)
#define OS_SOCKET_SUCCESS(st) (st == SU_STATUS_SUCCESS)

typedef struct {
    OS_NetworkAddress *netAddr;
    usz queueLength;
} OS_SocketTCPCreateInfo;

typedef struct {
    const char *path;
} OS_SocketIPCCreateInfo;

/* wrappers around setsockopt/getsockopt */
OS_SocketStatus OS_SocketEnableSockOpt(const OS_Socket *sock, sz sockOpt);
OS_SocketStatus OS_SocketDisableSockOpt(const OS_Socket *sock, sz sockOpt);

OS_SocketStatus OS_SocketGetSockOpt(const OS_Socket *sock, sz sockOpt, sz *value);
OS_SocketStatus OS_SocketSetSockObj(const OS_Socket *sock, sz sockOpt, const void *sockObj, usz size);
OS_SocketStatus OS_SocketGetSockObj(const OS_Socket *sock, sz sockOpt, void *sockObj, usz size);

OS_SocketStatus OS_SocketCreateTCPSocket(OS_Socket *sock, const OS_SocketTCPCreateInfo *info);
OS_SocketStatus OS_SocketCreateIPCSocket(OS_Socket *sock, const OS_SocketIPCCreateInfo *info);

OS_SocketStatus OS_SocketAccept(const OS_Socket *server, OS_Socket *client, OS_NetworkAddress *address);
OS_SocketStatus OS_SocketConnect(const OS_Socket *client, OS_Socket *server, const OS_NetworkAddress *address);

OS_SocketStatus OS_SocketClose(const OS_Socket *sock);
OS_SocketStatus OS_SocketShutdown(const OS_Socket *sock);

OS_SocketStatus OS_SocketReceiveData(const OS_Socket *sock, char *buffer, usz size, usz flags);
OS_SocketStatus OS_SocketSendData(const OS_Socket *sock, char *buffer, usz size, usz flags);

/* convert the atrocious Errno values to usable error codes */
OS_SocketStatus OS_SocketErrnoCodeToStatus(sz statusCode, usz socketFunction, usz errnoVal);
char *OS_SocketStringStatus(usz code);

#endif /* __CORE_OS_SOCKET_H__ */
