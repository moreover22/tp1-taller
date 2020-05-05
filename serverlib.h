#ifndef __SERVERLIB_H
#define __SERVERLIB_H

#define SERVER_ERROR 1
#define SERVER_SUCCESS 0

#include <stddef.h>
#include "common_socket.h"

typedef struct server {
    socket_t socket;
} server_t;

/**
 * @brief Constructor.
 * @param service nombre de servicio o puerto.
 * @return 0 en caso de éxito.
 */
int server_create(server_t *self, char *service);

/**
 * @brief Bucle principal del server.
 * @return 0 en caso de éxito.
 */
int server_run(server_t *self);

/**
 * @brief Destructor.
 */
int server_destroy(server_t *self);
#endif
