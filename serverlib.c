#include "serverlib.h"
#include <stdio.h>
#include <stdlib.h>
#include <byteswap.h>
#include <arpa/inet.h>

#define OK_MSG "OK\n"
#define LEN_OK_MSG 3
#define SUCCESS 0

int server_create(server_t *self, char *service) {
    if (socket_create(&self->socket, SERVER) != SUCCESS) {
        return SERVER_ERROR;
    }
    return socket_bind_and_listen(&self->socket, service);
}

int server_accept(server_t *self, socket_t *client) {
    if (socket_accept(&self->socket, client) != SUCCESS)
        return SERVER_ERROR;
    return SERVER_SUCCESS;
}

int server_comunicate(server_t *self, socket_t *client) {
    unsigned short len = 0;
    if (socket_receive(client, (char *) &len, 2) != SUCCESS) 
        return SERVER_ERROR;
    len = bswap_16(len);
    len = ntohs(len);
    if (len <= 0)
        return SERVER_ERROR;
    printf("[DEBUG] Server receive: %d\n", len);
    char *resp = malloc(len + 1);
    if (resp == NULL) {
        fprintf(stderr, "No se puedo allocar memoria.\n");
        return SERVER_ERROR;
    }
    if (socket_receive(client, resp, len) != SUCCESS) {
        free(resp);
        return SERVER_ERROR;
    }
    resp[len] = 0;  
    printf("[DEBUG] |%s| --- \n", resp);
    free(resp);
    if (socket_send(client, OK_MSG, LEN_OK_MSG) != SUCCESS)
        return SERVER_ERROR;

    return SERVER_SUCCESS;
}

int server_run(server_t *self, char *buffer, size_t longitud) {
    socket_t client;
    server_accept(self, &client);
    for(;;) {
        if (server_comunicate(self, &client) != SERVER_SUCCESS)
            break;
    }
    return SERVER_SUCCESS;
}

int server_destroy(server_t *self) {
    return socket_destroy(&self->socket);
}