#ifndef __SERVER_H_
#define __SERVER_H__

#include "types.h"
#include "network_connection.h"

typedef struct {
    NetworkConnection connection;
    NetworkAddress clientAddr;
    NetworkAddress serverAddr;
} ServerContext;

enum addrTypes {
    ADDR_TYPE_IPV4,
    ADDR_TYPE_IPV6,
};

typedef struct {
    usz addrType;
    char *addr;
    char *port;
} ServerContextInitData;

void SRV_ContextInit(ServerContext *server, const ServerContextInitData initData);
void SRV_Start(ServerContext *server);
void SRV_WaitForClient(ServerContext *server);

void SRV_ReceiveSimpleFileTimeout(ServerContext *server, char **fileNameBuff, char **fileContentBuff, usz *fileContentLengthBuff, usz ms);
void SRV_ReceiveSimpleFile(ServerContext *server);

void SRV_Shutdown(ServerContext *server);

#endif /* __SERVER_H__ */
