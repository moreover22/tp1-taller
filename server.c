#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <arpa/inet.h>
#include <byteswap.h>
#include "common_socket.h"
#include "serverlib.h"

#define ERROR 1
#define SUCCESS 0

int main(int argc, char *argv[]) {
    server_t server;
    if (server_create(&server, argv[1]) != SERVER_SUCCESS) {
        fprintf(stderr, "Error al crear el server. %s\n", strerror(errno));
        return ERROR;
    }
    if (server_run(&server) != SERVER_SUCCESS) {
        server_destroy(&server);
        fprintf(stderr, "Error al corriendo el server. %s \n", strerror(errno));
        return ERROR;
    }
    if (server_destroy(&server) != SERVER_SUCCESS) {
        fprintf(stderr, "Error al destruir el server\n"); 
        return ERROR;
    }
    return SUCCESS;
}
