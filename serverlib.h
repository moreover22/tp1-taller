#ifndef __SERVERLIB_H
#define __SERVERLIB_H

#define SERVER_ERROR 1
#define SERVER_SUCCESS 0

#include <stddef.h>
#include "common_socket.h"

typedef struct server {
    socket_t socket;
} server_t;

int server_create(server_t *self, char *service);
int server_run(server_t *self, char *buffer, size_t longitud);
int server_destroy(server_t *self);
#endif
