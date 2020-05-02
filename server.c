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

void print_by_bytes(void *p, size_t len) {
    size_t k;
    printf("(");
    for (k = 0; k < len; ++k)
        printf("%02X", ((unsigned char *)p)[k]);
    printf(")\n");
}

int main(int argc, char *argv[]) {
    size_t len = 300;
    char buf[300];
    memset(buf, 0, len);
    server_t server;
    if (server_create(&server, argv[1]) != SERVER_SUCCESS) {
        fprintf(stderr, "Error al crear el server. %s\n", strerror(errno));
        return ERROR;
    }
    if (server_run(&server, buf, len) != SERVER_SUCCESS) {
        server_destroy(&server);
        fprintf(stderr, "Error al corriendo el server. %s \n", strerror(errno));
        return ERROR;
    }
    if (server_destroy(&server) != SERVER_SUCCESS) {
        fprintf(stderr, "Error al destruir el server\n"); 
        return ERROR;
    }
    printf("OK.\n");
    return SUCCESS;
}


    // socket_t socket;
    // if (socket_create(&socket, SERVER) != SUCCESS) {
    //     return ERROR;
    // }
    // socket_bind_and_listen(&socket, argv[1]);
    // socket_t client;
    // if (socket_accept(&socket, &client) != SUCCESS) {
    //     return ERROR;
    // }
    // char buf[300];
    // memset(buf, 0, 300);
    // socket_receive(&client, buf, 299);
    // printf("%s", buf);

    // char buf[300];
    // memset(buf, 0, 300);
    // socket_send(&client, "Hola\n", strlen("Hola\n"));
    // char buf[300];
    // memset(buf, 0, 300);
    // printf("..%d\n", __LINE__);
    // char resp[2];

    // unsigned short len;
    // socket_receive(&client, (char *) &len, 2);
    // print_by_bytes((void *) &len, 2);
    // printf("server receive: %d\n", len);
    // len = bswap_16(len);
    // printf("server receive: %d\n", len);
    // len = ntohs(len);
    // printf("server receive: %d\n", len);

    // char *resp = malloc(len + 1);
    // socket_receive(&client, resp, len);
    // resp[len] = 0;
    // printf("|%s| --- \n", resp);
    // free(resp);

    // socket_send(&client, "OK\n", 3);
    
    // /*
    // len = swap_16(len);
    // len = ntohl(len);
    // printf("len %d\n", len);
    // printf("..%d\n", __LINE__);
    // resp[2] = 0;
    // printf("|%s| --- \n", resp);
    // char *data = malloc(len);
    // socket_receive(&client, data, len);
    // // printf("%s", buf);
    // */
    // socket_destroy(&socket);
