#ifndef __COMMON_SOCKET_H
#define __COMMON_SOCKET_H

#include <stddef.h>
#include "common_socket_conf.h"

typedef struct socket {
    int file_descriptor;
    configuration_t conf;
} socket_t;

#define SERVER 0
#define CLIENT 1
/**
 * Constructor socket
 */
int socket_create(socket_t *self, int how);

/**
 * Destructor socket
 * @return -1 en caso de error, 0 en caso de éxito.
 */
int socket_destroy(socket_t *self);

/**
 * Se establece una conexión con un host mediandte un servicio.
 * @param host nombre de dominio o ip en forma de cadena.
 * @param service nombre de servicio o puerto.
 * @return -1 en caso de error, 0 en caso de éxito.
 * Lado del cliente
 */
int socket_connect(socket_t *self, const char *host, const char *service);

/**
 * Se envia un mensaje a host previamente conectado.
 * @param mensaje mensaje a ser enviado.
 * @param longitud longitud del mensaje.
 * @return -1 en caso de error, 0 en caso de éxito.
 * Lado del cliente
 */
int socket_send(socket_t *self, char *mensaje, size_t longitud);

int socket_bind_and_listen(socket_t *self, const char *service);

int socket_accept(socket_t *self, socket_t *client);

int socket_receive(socket_t *self, char *buffer, size_t longitud);

int socket_shutdown(socket_t *self, int how);
#endif
