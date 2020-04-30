#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include "common_socket.h"

#define ERROR 1
#define SUCCESS 0

typedef struct server {
    socket_t socket;
} server_t;

int server_create(server_t *self, char *service) {
    if (socket_create(&self->socket, SERVER) != SUCCESS) {
        return ERROR;
    }
    return socket_bind_and_listen(&self->socket, service);
}

int server_run(server_t *self, char *buffer, size_t longitud) {
    socket_t client;
    printf(".");

    if (socket_accept(&self->socket, &client) != SUCCESS) {
        return ERROR;
    }
    char buf[5];
    memset(buf, 0, 5);

    printf("%d", __LINE__);
    socket_receive(&client, buf, 5);
    printf("%s", buf);
    // socket_send(&client, "OK\n", 4);
    return SUCCESS;
}

int server_destroy(server_t *self) {
    return socket_destroy(&self->socket);
}

int main(int argc, char *argv[]) {
    socket_t socket;
    if (socket_create(&socket, SERVER) != SUCCESS) {
        return ERROR;
    }
    printf("..%d\n", __LINE__);
    socket_bind_and_listen(&socket, argv[1]);
    printf("..%d\n", __LINE__);
    socket_t client;
    printf("%d", __LINE__);

    if (socket_accept(&socket, &client) != SUCCESS) {
        return ERROR;
    }
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
    char resp[3];
    socket_receive(&client, resp, 2);
    int len = atoi(resp);
    printf("len %d\n", len);
    printf("..%d\n", __LINE__);
    resp[2] = 0;
    printf("|%s| --- \n", resp);
    char *data = malloc(len);
    socket_receive(&client, data, len);
    socket_send(&client, "OK...\n", strlen("OK...\n"));
    // printf("%s", buf);
    socket_destroy(&socket);

    /*
    size_t len = 300;
    char buf[300];

    memset(buf, 0, len);
    server_t server;

    if (server_create(&server, argv[1]) != SUCCESS) {
        fprintf(stderr, "Error al crear el server. %s\n", strerror(errno));
        return ERROR;
    }

    if (server_run(&server, buf, len) != SUCCESS) {
        server_destroy(&server);
        fprintf(stderr, "Error al recibir mensaje %s \n", strerror(errno));
        return ERROR;
    }
    
    printf("OK.\n");
    // printf("%s\n", buf);

    if (server_destroy(&server) != SUCCESS) {
        fprintf(stderr, "Error al destruir el server\n"); 
        return ERROR;
    }
    */
    return SUCCESS;
}