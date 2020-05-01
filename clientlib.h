#ifndef __CLIENTLIB_H
#define __CLIENTIB_H
#include <stddef.h>
#include "common_socket.h"

#define CLIENT_ERROR 1
#define CLIENT_SUCCESS 0

typedef struct client {
    socket_t socket;
} client_t;

int client_create(client_t *self, const char *hostname, const char *service);
int client_send(client_t *self, char *buffer, size_t longitud);
int client_receive(client_t *self, char *buffer, size_t longitud);
int client_destroy(client_t *self);

#endif