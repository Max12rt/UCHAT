#include "../../inc/main.h"

int connect_to_server(void) {
    struct sockaddr_in server;
    sock = socket(AF_INET , SOCK_STREAM , 0);

    server.sin_family = AF_INET;
    server.sin_addr.s_addr = inet_addr(SERVER_ADDR);
    server.sin_port = htons(SERVER_PORT);
    inet_pton(AF_INET, SERVER_ADDR, &server.sin_addr);
    connect(sock, (struct sockaddr *)&server, sizeof(server));

    return sock;
}
