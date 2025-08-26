#ifndef __SU_UTILS_H__
#define __SU_UTILS_H__

#include <core/types.h>
#include <core/utils.h>
#include <core/error_report.h>

typedef struct {
    u32 addr;
    u16 port;
} NetworkAddress;

enum socketStatusCodes {
    SU_STATUS_SUCCESS = 0,
    SU_STATUS_HOST_OUT_OF_FDS,
    SU_STATUS_PROCESS_OUT_OF_FDS,
    SU_STATUS_PROCESS_OUT_OF_SOCKETS,
    SU_STATUS_HOST_OUT_OF_SOCKETS,
    SU_STATUS_HOST_OUT_OF_MEMORY,
    SU_STATUS_PROTECTED_ADDRESS,
    SU_STATUS_ADDRESS_AND_PORT_IN_USE,
    SU_STATUS_INVALID_SOCKET,
    SU_STATUS_SOCKET_ALREADY_BOUND,
    SU_STATUS_EMPTY_CONNECTION_QUEUE,
    SU_STATUS_CONNECTION_WAS_ABORTED,
    SU_STATUS_CONNECTION_WAS_INTERRUPTED,
    SU_STATUS_SOCKET_IS_NOT_LISTENING,
    SU_STATUS_BLOCKED_BY_FIREWALL,
    SU_STATUS_PREVIOUS_CONNECT_INCOMPLETE,
    SU_STATUS_SERVICE_NOT_AVAILABLE,
    SU_STATUS_SOCKET_IS_NOT_CONNECTED,
    SU_STATUS_SOCKET_IS_ALREADY_CONNECTED,
    SU_STATUS_CONNECTION_TIMEOUT,
    SU_STATUS_RECEIVE_TIMEOUT,
    SU_STATUS_CLOSE_GOT_INTERRUPTED_BY_OS,
    SU_STATUS_INPUT_OUTPUT_ERROR,
    SU_STATUS_PEER_RESET_CONNECTION,
    SU_STATUS_SEND_GOT_INTERRUPTED_BY_OS,
    SU_STATUS_NO_TARGET_SPECIFIED,
    SU_STATUS_PEER_DOWN,
    SU_STATUS_RECV_GOT_INTERRUPTED_BY_OS,

    SU_STATUS_UNKNOWN,
    SU_STATUS_COUNT,
};

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

#define SU_FAILURE(st) (st != SU_STATUS_SUCCESS)

/* wrappers around setsockopt/getsockopt */
usz SU_EnableSockOpt(sz rawSocket, sz sockOpt);
usz SU_DisableSockOpt(sz rawSocket, sz sockOpt);

usz SU_GetSockOpt(sz rawSocket, sz sockOpt, sz *value);
usz SU_SetSockObj(sz rawSocket, sz sockOpt, void *sockObj, usz size);
usz SU_GetSockObj(sz rawSocket, sz sockOpt, void *sockObj, usz size);

usz SU_CreateTCPSocket(sz *rawSocket);
usz SU_BindAddr(sz rawSocket, NetworkAddress na);
usz SU_SetListenQueue(sz rawSocket, usz queueLength);
usz SU_Accept(sz *clientSocket, sz serverSocket, NetworkAddress *na);
usz SU_Connect(sz *serverSocket, sz clientSocket, NetworkAddress na);
usz SU_Close(sz *rawSocket);
usz SU_Shutdown(sz rawSocket);

usz SU_ReceiveData(sz rawSocket, char *buffer, usz size, usz flags);
usz SU_SendData(sz rawSocket, char *buffer, usz size, usz flags);

/* convert the atrocious Errno values to usable error codes */
usz SU_ErrnoCodeToStatus(sz statusCode, usz socketFunction, usz errnoVal);
char *SU_StringStatus(usz code);

#endif /* __SU_UTILS_H__ */
