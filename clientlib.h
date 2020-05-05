#ifndef __CLIENTLIB_H
#define __CLIENTLIB_H
#include <stddef.h>
#include "common_socket.h"

#define CLIENT_ERROR 1
#define CLIENT_SUCCESS 0

typedef struct client {
    socket_t socket;
} client_t;

/**
 * @brief Constructor.
 * @param host: ip o hostname.
 * @param service: numero de puerto o nombre de servicio.
 * @return CLIENT_SUCCESS en caso de exito.
 */
int client_create(client_t *self, const char *hostname, const char *service);

/**
 * @brief Envia un mensaje.
 * @param buffer: mensaje.
 * @param longitud: longitud del mensaje.
 * @return Cantidad de bytes que logró enviar.
 */
int client_send(client_t *self, char *buffer, size_t longitud);

/**
 * @brief Recibe un mensaje.
 * @param buffer: buffer destino del mensaje.
 * @param longitud: longitud del mensaje.
 * @return Cantidad de bytes que logró recibir.
 */
int client_receive(client_t *self, char *buffer, size_t longitud);

/**
 * @brief Destructor.
 */
int client_destroy(client_t *self);
#endif
