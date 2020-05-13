#include "clientlib.h"
#include <stdlib.h>
int client_create(client_t *self, const char *hostname, const char *service) {
    if (socket_create(&self->socket, CLIENT) != 0) 
        return CLIENT_ERROR;
    if (socket_connect(&self->socket, hostname, service) != 0)
        return CLIENT_ERROR;
    return CLIENT_SUCCESS;
}

int client_send(client_t *self, char *buffer, size_t longitud) {
    return socket_send(&self->socket, buffer, longitud);
}
int client_receive(client_t *self, char *buffer, size_t longitud) {
    return socket_receive(&self->socket, buffer, longitud);
}

int client_destroy(client_t *self) {
    return socket_destroy(&self->socket);
}
