#include <net/net.h>
#include <net/net_con.h>
#include <net/socket_utils.h>
#include <net/berkeley_sockets.h>
#include <core/cstd.h>
#include <core/utils.h>

#define SOCKOPT_ENABLE &(int) {1}
#define SOCKOPT_DISABLE &(int) {0}

usz SockUtil_EnableSockOpt(sz rawSocket, sz sockOpt)
{
    sz scRc = setsockopt(rawSocket, SOL_SOCKET, sockOpt, SOCKOPT_ENABLE, sizeof(int));

    return SockUtil_ErrnoCodeToStatus(scRc, SOCK_FN_SETSOCKOPT, errno);
}

usz SockUtil_DisableSockOpt(sz rawSocket, sz sockOpt)
{
    sz scRc = setsockopt(rawSocket, SOL_SOCKET, sockOpt, SOCKOPT_DISABLE, sizeof(int));

    return SockUtil_ErrnoCodeToStatus(scRc, SOCK_FN_SETSOCKOPT, errno);
}

usz SockUtil_GetSockOpt(sz rawSocket, sz sockOpt, sz *value)
{
    int soValue = 0;
    socklen_t sLen = sizeof(soValue);
    sz scRc = getsockopt(rawSocket, SOL_SOCKET, sockOpt, &soValue, &sLen);
    *value = (sz)soValue;

    return SockUtil_ErrnoCodeToStatus(scRc, SOCK_FN_GETSOCKOPT, errno);
}

usz SockUtil_SetSockObj(sz rawSocket, sz sockOpt, void *sockObj, usz size)
{
    sz scRc = setsockopt(rawSocket, SOL_SOCKET, sockOpt, sockObj, size);

    return SockUtil_ErrnoCodeToStatus(scRc, SOCK_FN_SETSOCKOPT, errno);
}

usz SockUtil_GetSockObj(sz rawSocket, sz sockOpt, void *sockObj, usz size)
{
    socklen_t sLen = size;
    sz scRc = getsockopt(rawSocket, SOL_SOCKET, sockOpt, sockObj, &sLen);

    return SockUtil_ErrnoCodeToStatus(scRc, SOCK_FN_GETSOCKOPT, errno);
}

usz SockUtil_CreateTCPSocket(sz *rawSocket)
{
    *rawSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    return SockUtil_ErrnoCodeToStatus(*rawSocket, SOCK_FN_SOCKET, errno);
}

usz SockUtil_BindAddr(sz rawSocket, NetworkAddress na)
{
    struct sockaddr_in inAddr = {
        .sin_family = AF_INET,
        .sin_addr.s_addr = na.addr,
        .sin_port = na.port,
    };

    sz scRc = bind(rawSocket, (struct sockaddr *)&inAddr, sizeof(inAddr));

    return SockUtil_ErrnoCodeToStatus(scRc, SOCK_FN_BIND, errno);
}

usz SockUtil_SetListenQueue(sz rawSocket, usz queueLength)
{
    sz scRc = listen(rawSocket, queueLength);
    return SockUtil_ErrnoCodeToStatus(scRc, SOCK_FN_LISTEN, errno);
}

usz SockUtil_Accept(sz *clientSocket, sz serverSocket, NetworkAddress *na)
{
    struct sockaddr_in clientAddr = {0};
    socklen_t clientLen = sizeof(clientAddr);

    *clientSocket = accept(serverSocket, (struct sockaddr *)&clientAddr, &clientLen);
    na->addr = clientAddr.sin_addr.s_addr;
    na->port = clientAddr.sin_port;

    return SockUtil_ErrnoCodeToStatus(*clientSocket, SOCK_FN_ACCEPT, errno);
}

usz SockUtil_Connect(sz *serverSocket, sz clientSocket, NetworkAddress na)
{
    struct sockaddr_in serverAddr = {
        .sin_family = AF_INET,
        .sin_addr.s_addr = na.addr,
        .sin_port = na.port,
    };

    sz scRc = connect(clientSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr));
    *serverSocket = clientSocket;

    return SockUtil_ErrnoCodeToStatus(scRc, SOCK_FN_CONNECT, errno);
}

usz SockUtil_Close(sz *sock)
{
    sz scRc = close(*sock);
    *sock = 0;
    return SockUtil_ErrnoCodeToStatus(scRc, SOCK_FN_CLOSE, errno);
}

usz SockUtil_ErrnoCodeToStatus(sz scRc, usz socketFunction, usz errnoVal)
{
    if (scRc < 0) {
        switch (socketFunction) {
        case SOCK_FN_SOCKET:
            switch (errnoVal) {
            case EMFILE: return SOCK_UTIL_STATUS_HOST_OUT_OF_SOCKETS;
            case ENOBUFS: return SOCK_UTIL_STATUS_HOST_OUT_OF_MEMORY;
            case ENOMEM: return SOCK_UTIL_STATUS_HOST_OUT_OF_MEMORY;
            }
            UNREACHABLE();
        case SOCK_FN_BIND:
            switch (errnoVal) {
            case EACCES: return SOCK_UTIL_STATUS_PROTECTED_ADDRESS;
            case EADDRINUSE: return SOCK_UTIL_STATUS_ADDRESS_AND_PORT_IN_USE;
            case EBADF: return SOCK_UTIL_STATUS_INVALID_SOCKET;
            case EINVAL: return SOCK_UTIL_STATUS_SOCKET_ALREADY_BOUND;
            }
            UNREACHABLE();
        case SOCK_FN_LISTEN:
            switch (errnoVal) {
            case EADDRINUSE: return SOCK_UTIL_STATUS_ADDRESS_AND_PORT_IN_USE;
            case EBADF: return SOCK_UTIL_STATUS_INVALID_SOCKET;
            case ENOTSOCK: return SOCK_UTIL_STATUS_INVALID_SOCKET;
            }
            UNREACHABLE();
        case SOCK_FN_ACCEPT:
            switch (errnoVal) {
            case EWOULDBLOCK: return SOCK_UTIL_STATUS_EMPTY_CONNECTION_QUEUE;
#if !defined(__linux__)
            case EAGAIN: return SOCK_UTIL_STATUS_EMPTY_CONNECTION_QUEUE;
#endif /* __linux__ */
            case EBADF: return SOCK_UTIL_STATUS_INVALID_SOCKET;
            case ECONNABORTED: return SOCK_UTIL_STATUS_CONNECTION_WAS_ABORTED;
            case EINTR: return SOCK_UTIL_STATUS_CONNECTION_WAS_INTERRUPTED;
            case EINVAL: return SOCK_UTIL_STATUS_SOCKET_IS_NOT_LISTENING;
            case EMFILE: return SOCK_UTIL_STATUS_PROCESS_OUT_OF_FDS;
            case ENFILE: return SOCK_UTIL_STATUS_HOST_OUT_OF_FDS;
            case ENOBUFS: return SOCK_UTIL_STATUS_HOST_OUT_OF_SOCKETS;
            case ENOMEM: return SOCK_UTIL_STATUS_HOST_OUT_OF_SOCKETS;
            case ENOTSOCK: return SOCK_UTIL_STATUS_INVALID_SOCKET;
            }
            UNREACHABLE();
        case SOCK_FN_CONNECT:
            switch (errnoVal) {
            case EPERM: return SOCK_UTIL_STATUS_BLOCKED_BY_FIREWALL;
            case EADDRINUSE: return SOCK_UTIL_STATUS_ADDRESS_AND_PORT_IN_USE;
            case EALREADY: return SOCK_UTIL_STATUS_PREVIOUS_CONNECT_INCOMPLETE;
            case EBADF: return SOCK_UTIL_STATUS_INVALID_SOCKET;
            case ECONNREFUSED: return SOCK_UTIL_STATUS_SERVICE_NOT_AVAILABLE;
            case EINPROGRESS: TODO("implement EINPROGRESS for connect()"); ABORT();
            case EINTR: return SOCK_UTIL_STATUS_CONNECTION_WAS_INTERRUPTED;
            case EISCONN: return SOCK_UTIL_STATUS_SOCKET_IS_ALREADY_CONNECTED;
            case ENOTSOCK: return SOCK_UTIL_STATUS_INVALID_SOCKET;
            case ETIMEDOUT: return SOCK_UTIL_STATUS_CONNECTION_TIMEOUT;
            }
            UNREACHABLE();
        case SOCK_FN_SEND:
            switch (errnoVal) {
            case EAGAIN: TODO("implement EAGAIN for send()"); ABORT();
            case EALREADY: TODO("implement EALREADY for send()"); ABORT();
            case EBADF: return SOCK_UTIL_STATUS_INVALID_SOCKET;
            case ECONNRESET: return SOCK_UTIL_STATUS_PEER_RESET_CONNECTION;
            case EDESTADDRREQ: return SOCK_UTIL_STATUS_NO_TARGET_SPECIFIED;
            case EINTR: return SOCK_UTIL_STATUS_SEND_GOT_INTERRUPTED_BY_OS;
            case ENOMEM: return SOCK_UTIL_STATUS_HOST_OUT_OF_MEMORY;
            case ENOTCONN: return SOCK_UTIL_STATUS_SOCKET_IS_NOT_CONNECTED;
            case ENOTSOCK: return SOCK_UTIL_STATUS_INVALID_SOCKET;
            }
            UNREACHABLE();
        case SOCK_FN_RECV:
            switch (errnoVal) {
            case EBADF: return SOCK_UTIL_STATUS_INVALID_SOCKET;
            case ENOTSOCK: return SOCK_UTIL_STATUS_INVALID_SOCKET;
            case ECONNREFUSED: return SOCK_UTIL_STATUS_PEER_DOWN;
            case EINTR: return SOCK_UTIL_STATUS_RECV_GOT_INTERRUPTED_BY_OS;
            case ENOTCONN: return SOCK_UTIL_STATUS_SOCKET_IS_NOT_CONNECTED;
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
            case EBADF: return SOCK_UTIL_STATUS_INVALID_SOCKET;
            case EINTR: return SOCK_UTIL_STATUS_CLOSE_GOT_INTERRUPTED_BY_OS;
            case EIO: TODO("check if EIO makes sense upon closing socket!");
                      return SOCK_UTIL_STATUS_INPUT_OUTPUT_ERROR;
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

    return SOCK_UTIL_STATUS_SUCCESS;
}

char *g_SockUtil_StatusCodeStrings[] = {
    ENUM_STR_ENTRY(SOCK_UTIL_STATUS_SUCCESS),
    ENUM_STR_ENTRY(SOCK_UTIL_STATUS_HOST_OUT_OF_FDS),
    ENUM_STR_ENTRY(SOCK_UTIL_STATUS_PROCESS_OUT_OF_FDS),
    ENUM_STR_ENTRY(SOCK_UTIL_STATUS_PROCESS_OUT_OF_SOCKETS),
    ENUM_STR_ENTRY(SOCK_UTIL_STATUS_HOST_OUT_OF_SOCKETS),
    ENUM_STR_ENTRY(SOCK_UTIL_STATUS_HOST_OUT_OF_MEMORY),
    ENUM_STR_ENTRY(SOCK_UTIL_STATUS_PROTECTED_ADDRESS),
    ENUM_STR_ENTRY(SOCK_UTIL_STATUS_ADDRESS_AND_PORT_IN_USE),
    ENUM_STR_ENTRY(SOCK_UTIL_STATUS_INVALID_SOCKET),
    ENUM_STR_ENTRY(SOCK_UTIL_STATUS_SOCKET_ALREADY_BOUND),
    ENUM_STR_ENTRY(SOCK_UTIL_STATUS_EMPTY_CONNECTION_QUEUE),
    ENUM_STR_ENTRY(SOCK_UTIL_STATUS_CONNECTION_WAS_ABORTED),
    ENUM_STR_ENTRY(SOCK_UTIL_STATUS_CONNECTION_WAS_INTERRUPTED),
    ENUM_STR_ENTRY(SOCK_UTIL_STATUS_SOCKET_IS_NOT_LISTENING),
    ENUM_STR_ENTRY(SOCK_UTIL_STATUS_SOCKET_IS_NOT_CONNECTED),
    ENUM_STR_ENTRY(SOCK_UTIL_STATUS_BLOCKED_BY_FIREWALL),
    ENUM_STR_ENTRY(SOCK_UTIL_STATUS_PREVIOUS_CONNECT_INCOMPLETE),
    ENUM_STR_ENTRY(SOCK_UTIL_STATUS_SERVICE_NOT_AVAILABLE),
    ENUM_STR_ENTRY(SOCK_UTIL_STATUS_CONNECTION_TIMEOUT),
    ENUM_STR_ENTRY(SOCK_UTIL_STATUS_RECEIVE_TIMEOUT),
    ENUM_STR_ENTRY(SOCK_UTIL_STATUS_CLOSE_GOT_INTERRUPTED_BY_OS),
    ENUM_STR_ENTRY(SOCK_UTIL_STATUS_INPUT_OUTPUT_ERROR),
    ENUM_STR_ENTRY(SOCK_UTIL_STATUS_PEER_RESET_CONNECTION),
    ENUM_STR_ENTRY(SOCK_UTIL_STATUS_NO_TARGET_SPECIFIED),
    ENUM_STR_ENTRY(SOCK_UTIL_STATUS_PEER_DOWN),
    ENUM_STR_ENTRY(SOCK_UTIL_STATUS_RECV_GOT_INTERRUPTED_BY_OS),
    ENUM_STR_ENTRY(SOCK_UTIL_STATUS_UNKNOWN),
};

char *SockUtil_StringStatus(usz code)
{
    if (code > SOCK_UTIL_STATUS_UNKNOWN)
        code = SOCK_UTIL_STATUS_UNKNOWN;

    return g_SockUtil_StatusCodeStrings[code];
}

usz SockUtil_ReceiveData(sz rawSocket, char *buffer, usz size, usz flags)
{
    sz scRc = recv(rawSocket, buffer, size, (int)flags);
    return SockUtil_ErrnoCodeToStatus(scRc, SOCK_FN_RECV, errno);
}

usz SockUtil_SendData(sz rawSocket, char *buffer, usz size, usz flags)
{
    sz scRc = send(rawSocket, buffer, size, (int)flags);
    return SockUtil_ErrnoCodeToStatus(scRc, SOCK_FN_SEND, errno);
}

#define SOCK_UTIL_ERR_FMT(suCall) "[SOCK_UTIL] %s -> %s\n", #suCall
#define SOCK_UTIL_ERR_FMT_ARG(errMsg) errMsg
#define MAX_CONNECTION_QUEUE 3

b32 NetCon_FatalError(usz status)
{
    switch (status) {
    case NET_CON_STATUS_SUCCESS:
    case NET_CON_STATUS_TIMED_OUT:
        return FALSE;
    case NET_CON_STATUS_FAILURE:
        return TRUE;
    }
    UNREACHABLE();
}

ER_AppendReport()
{
    LL_Append(&er->list, report);
}

usz NetCon_InitServerER(NetworkConnection *con, NetworkAddress na, ErrorReport *er)
{
    usz st = SOCK_UTIL_STATUS_SUCCESS;

    st = SockUtil_CreateTCPSocket(&con->serverSocket);
    if (SOCK_UTIL_FAILURE(st)) {
        ER_AppendReport(er, SOCK_UTIL_ERR_FMT(SockUtil_CreateTCPSocket),
                            SOCK_UTIL_ERR_FMT_ARG(SockUtil_StringStatus(st)));
        return NET_CON_STATUS_FAILURE;
    }

    st = SockUtil_BindAddr(con->serverSocket, na);
    if (SOCK_UTIL_FAILURE(st)) {
        ER_AppendReport(er, SOCK_UTIL_ERR_FMT(SockUtil_BindAddr),
                            SOCK_UTIL_ERR_FMT_ARG(SockUtil_StringStatus(st)));
        return NET_CON_STATUS_FAILURE;
    }

    st = SockUtil_EnableSockOpt(con->serverSocket, SO_REUSEADDR);
    if (SockUtil_FAILURE(st)) {
        ER_AppendReport(er, SOCK_UTIL_ERR_FMT(SockUtil_EnableSockopt),
                            SOCK_UTIL_ERR_FMT_ARG(SockUtil_StringStatus(st)));
        ER_AppendReport(er, "while setting SO_REUSEADDR\n");
        return NET_CON_STATUS_FAILURE;
    }

    st = SockUtil_EnableSockOpt(con->serverSocket, SO_REUSEPORT);
    if (SOCK_UTIL_FAILURE(st)) {
        ER_AppendReport(er, SOCK_UTIL_ERR_FMT(SockUtil_EnableSockopt),
                            SOCK_UTIL_ERR_FMT_ARG(SockUtil_StringStatus(st)));
        ER_AppendReport(er, "while setting SO_REUSEPORT\n");
        return NET_CON_STATUS_FAILURE;
    }

    st = SockUtil_SetListenQueue(con->serverSocket, MAX_CONNECTION_QUEUE);
    if (SOCK_UTIL_FAILURE(st)) {
        ER_AppendReport(er, SOCK_UTIL_ERR_FMT(SockUtil_SetListenQueue),
                            SOCK_UTIL_ERR_FMT_ARG(SockUtil_StringStatus(st)));
        return NET_CON_STATUS_FAILURE;
    }

    return NC_STATUS_SUCCESS;
}

usz NetCon_InitClient(NetworkConnection *con, ErrorReport *er)
{
    usz st = SOCK_UTIL_STATUS_SUCCESS;

    st = SockUtil_CreateTCPSocket(&con->clientSocket);
    if (SOCK_UTIL_FAILURE(st)) {
        ER_AppendReport(er, SOCK_UTIL_ERR_FMT(SockUtil_CreateTCPSocket),
                            SOCK_UTIL_ERR_FMT_ARG(SockUtil_StringStatus(st)));
        return NET_CON_STATUS_FAILURE;
    }

    return NET_CON_STATUS_SUCCESS;
}

usz NetCon_RequestConnection(NetworkConnection *con, NetworkAddress na, ErrorReport *er)
{
    usz ret = NET_CON_STATUS_SUCCESS;
    sz st = SOCK_UTIL_STATUS_SUCCESS;

    st = SockUtil_Connect(&con->serverSocket, con->clientSocket, na);
    if (SOCK_UTIL_FAILURE(st)) {
        ret = NET_CON_STATUS_FAILURE;
        ER_AppendReport(er, SOCK_UTIL_ERR_FMT(SockUtil_Connect),
                            SOCK_UTIL_ERR_FMT_ARG(SockUtil_StringStatus(st)));

        if (st == SOCK_UTIL_STATUS_CONNECTION_TIMEOUT)
            ret = NET_CON_STATUS_TIMED_OUT;
    }
    return ret;
}

usz NetCon_AcceptRequest(NetworkConnection *con, NetworkAddress *na, ErrorReport *er)
{
    sz st = SOCK_UTIL_STATUS_SUCCESS;

    st = SockUtil_Accept(&con->clientSocket, con->serverSocket, na);
    if (SockUtil_FAILURE(st)) {
        ER_AppendReport(er, SOCK_UTIL_ERR_FMT(SockUtil_Accept),
                            SOCK_UTIL_ERR_FMT_ARG(SockUtil_StringStatus(st)));
        return NET_CON_STATUS_FAILURE;
    }

    return NET_CON_STATUS_SUCCESS;
}

usz NetCon_CloseConnection(NetworkConnection *con)
{
    SockUtil_Close(&con->serverSocket);
    SockUtil_Close(&con->serverSocket);

    return NET_CON_STATUS_SUCCESS;
}

usz NetCon_ReadServerData(NetworkConnection *con, char *buffer, usz size, ErrorReport *er)
{
    usz ret = NET_CON_STATUS_SUCCESS;
    sz st = SockUtil_ReceiveData(con->serverSocket, buffer, size, 0);

    if (SockUtil_FAILURE(st)) {
        ret = NET_CON_STATUS_FAILURE;
        ER_AppendReport(er, SOCK_UTIL_ERR_FMT(SockUtil_ReceiveData),
                            SOCK_UTIL_ERR_FMT_ARG(SockUtil_StringStatus(st)));
        if (st == SOCK_UTIL_STATUS_CONNECTION_TIMEOUT)
            ret = NET_CON_STATUS_TIMED_OUT;
    }

    return ret;
}

usz NetCon_ReadClientData(NetworkConnection *con, char *buffer, usz size, ErrorReport *er)
{
    usz ret = NET_CON_STATUS_SUCCESS;
    sz st = SockUtil_ReceiveData(con->clientSocket, buffer, size, 0);

    if (SockUtil_FAILURE(st)) {
        ret = NC_STATUS_FAILURE;
        ER_AppendReport(er, SOCK_UTIL_ERR_FMT(SockUtil_ReceiveData),
                            SOCK_UTIL_ERR_FMT_ARG(SockUtil_StringStatus(st)));
        if (st == SOCK_UTIL_STATUS_CONNECTION_TIMEOUT)
            ret = NET_CON_STATUS_TIMED_OUT;
    }

    return ret;
}

usz NetCon_SendToServer(NetworkConnection *con, char *buffer, usz size, ErrorReport *er)
{
    sz st = SOCK_UTIL_STATUS_SUCCESS;

    st = SockUtil_SendData(con->serverSocket, buffer, size, 0);
    if (SockUtil_FAILURE(st)) {
        ER_AppendReport(er, SOCK_UTIL_ERR_FMT(SockUtil_SendData),
                            SOCK_UTIL_ERR_FMT_ARG(SockUtil_StringStatus(st)));
        return NC_STATUS_FAILURE;
    }
    return NET_CON_STATUS_SUCCESS;
}

usz NetCon_SendToClient(NetworkConnection *con, char *buffer, usz size, ErrorReport *er)
{
    usz ret = NET_CON_STATUS_SUCCESS;
    sz st = SOCK_UTIL_STATUS_SUCCESS;

    st = SockUtil_SendData(con->clientSocket, buffer, size, 0);
    if (SockUtil_FAILURE(st)) {
        ER_AppendReport(er, SOCK_UTIL_ERR_FMT(SockUtil_SendData),
                            SOCK_UTIL_ERR_FMT_ARG(SockUtil_StringStatus(st)));
        return NET_CON_STATUS_FAILURE;
    }

    return ret;
}

usz NetCon_GetServerReceiveTimeoutValue(NetworkConnection *con, usz *ms, ErrorReport *er)
{
    usz st = SOCK_UTIL_STATUS_SUCCESS;
    struct timeval timeout = {0};

    st = SockUtil_GetSockObj(con->serverSocket, SO_RCVTIMEO, &timeout, sizeof(timeout));
    if (SockUtil_FAILURE(st)) {
        ER_AppendReport(er, SOCK_UTIL_ERR_FMT(SockUtil_GetSockObj),
                            SOCK_UTIL_ERR_FMT_ARG(SockUtil_StringStatus(st)));
        return NC_STATUS_FAILURE;
    }

    *ms = (timeout.tv_sec * 1000) + (usz)(timeout.tv_usec/1000);
    return NET_CON_STATUS_TIMED_OUT;
}

usz NC_SetServerReceiveTimeoutValue(NetworkConnection *con, usz ms, ErrorReport *er)
{
    usz st = SOCK_UTIL_STATUS_SUCCESS;
    struct timeval timeout = {
        .tv_sec = ms/1000,
        .tv_usec = (ms%1000)*1000,
    };

    st = SockUtil_SetSockObj(con->serverSocket, SO_RCVTIMEO, &timeout, sizeof(timeout));
    if (SockUtil_FAILURE(st)) {
        ER_AppendReport(er, SOCK_UTIL_ERR_FMT(SockUtil_SetSockObj),
                            SOCK_UTIL_ERR_FMT_ARG(SockUtil_StringStatus(st)));
        return NET_CON_STATUS_FAILURE;
    }

    return NET_CON_STATUS_SUCCESS;
}

u32 NetUtils_StrToInetAddr(char *s)
{
    return inet_addr(s);
}

u16 NetUtils_StrToInetPort(char *s)
{
    return htons((u16)strtoul(s, NULL, 10));
}

#define IPV4_OCTET_COUNT 4

u32 NetUtils_StrToInetAddrER(char *s, ErrorReport *er)
{
    u32 hostOrderAddr = 0;
    char *savePtr = NULL;
    char *strAddr = strdup(s);
    char *p = strAddr;

    savePtr = strAddr;

    for (usz i = 0; i < IPV4_OCTET_COUNT; ++i) {
        char *octet = strtok_r(p, ".", &savePtr);
        if (!octet) {
            ER_AppendReport(er, ERROR_LOG_STR("invalid IP format!\n"));
            return 0xFFFFFFFF;
        }
        p = NULL;
        hostOrderAddr |= (u32)atoi(octet) << (8*(3-i));
    }
    free(strAddr);

    return (u32)htonl(hostOrderAddr);
}

u16 NetUtils_StrToInetPortER(char *s, ErrorReport *er)
{
    UNUSED(er);
    return NetUtils_StrToInetPort(s);
}

char *NetUtils_InetAddrToStr(u32 addr, char buff[INET_ADDR_STRLEN])
{
    struct in_addr addrStruct = {
        .s_addr = addr,
    };
    return (char *)inet_ntop(AF_INET, &addrStruct, buff, INET_ADDR_STRLEN);
}

char *NetUtils_InetPortToStr(u16 port, char buff[INET_PORT_STRLEN])
{
    snprintf(buff, INET_PORT_STRLEN, "%u", ntohs(port));
    return  buff;
}

