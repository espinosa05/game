#ifndef __NET_CON_H__
#define __NET_CON_H__

#include <core/types.h>
#include <net/socket_utils.h>

typedef struct {
    sz clientSocket;
    sz serverSocket;
} NetworkConnection;

enum ncStatusCodes {
    NC_STATUS_SUCCESS = 0,
    NC_STATUS_TIMED_OUT,

    NC_STATUS_FAILURE,
};


b32 NC_FatalError(usz status);

usz NC_InitServer(NetworkConnection *con, NetworkAddress na, ErrorReport *er);
usz NC_InitClient(NetworkConnection *con, ErrorReport *er);

usz NC_RequestConnection(NetworkConnection *con, NetworkAddress na, ErrorReport *er);
usz NC_AcceptRequest(NetworkConnection *con, NetworkAddress *na, ErrorReport *er);
usz NC_CloseConnection(NetworkConnection *con);

usz NC_ReadClientData(NetworkConnection *con, char *buffer, usz size, ErrorReport *er);
usz NC_ReadServerData(NetworkConnection *con, char *buffer, usz size, ErrorReport *er);

usz NC_SendToServer(NetworkConnection *con, char *buffer, usz size, ErrorReport *er);
usz NC_SendToClient(NetworkConnection *con, char *buffer, usz size, ErrorReport *er);

usz NC_GetServerReceiveTimeoutValue(NetworkConnection *con, usz *ms, ErrorReport *er);
usz NC_SetServerReceiveTimeoutValue(NetworkConnection *con, usz ms, ErrorReport *er);

#endif /* __NET_CON_H__ */
