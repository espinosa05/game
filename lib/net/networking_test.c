#include <core/c_std_library.h>
#include <core/error_report.h>
#include <core/utils.h>
#include <net/net.h>
#include <net/net_con.h>


void printErrorReportAndDie(ErrorReport er)
{
    fprintf(stderr, "=== the program has encountered a fatal exception! ===\n"
                    "=== dumping information below! ===");
    fprintf(stderr, "\n%s", er.rString);
    ABORT();
}

#define REQUEST_HEADER "GET_FILE00000008test.txt"
int main(int argc, char **argv)
{
    NetworkConnection client = {0};
    ErrorReport er = {0};
    usz st = NC_STATUS_SUCCESS;
    st = NC_InitClient(&client, &er);
    if (NC_STATUS_SUCCESS != st){
        printErrorReportAndDie(er);
    }

    NetworkAddress serverAddress = {
        .addr = NET_StrToInetAddr(argc > 1 ? argv[1] : "127.0.0.1"),
        .port = NET_StrToInetPort(argc > 2 ? argv[2] : "34939"),
    };

    fprintf(stderr, "attempting connection to -> %s:%s\n",
                    NET_InetAddrToStr(serverAddress.addr, (char[INET_ADDR_STRLEN]){0}),
                    NET_InetPortToStr(serverAddress.port, (char[INET_PORT_STRLEN]){0}));

    st = NC_RequestConnection(&client, serverAddress, &er);
    fprintf(stderr, "NCRC -> %d\n", st);
    if (st != NC_STATUS_SUCCESS) {
        if (st == NC_STATUS_TIMED_OUT) {
            printf("Connection timed out");
        }
        NC_CloseConnection(&client);
        printErrorReportAndDie(er);
    }

    char request[] = {REQUEST_HEADER};
    NC_SendToServer(&client, request, sizeof(request), &er);
    if (st != NC_STATUS_SUCCESS) {
        printErrorReportAndDie(er);
    }

    NC_CloseConnection(&client);

    return EXIT_SUCCESS;
}
