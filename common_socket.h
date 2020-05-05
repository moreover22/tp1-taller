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
 * @brief Constructor socket
 * @param how: modo de apertura del socket, puede ser SERVER o CLIENT.
 */
int socket_create(socket_t *self, int how);

/**
 * @brief Destructor socket
 * @return -1 en caso de error, 0 en caso de éxito.
 */
int socket_destroy(socket_t *self);

/**
 * @brief Se establece una conexión con un host mediandte un servicio.
 * @param host nombre de dominio o ip en forma de cadena.
 * @param service nombre de servicio o puerto.
 * @return -1 en caso de error, 0 en caso de éxito.
 * Lado del cliente
 */
int socket_connect(socket_t *self, const char *host, const char *service);

/**
 * @brief Se envia un mensaje a host previamente conectado.
 * @param mensaje mensaje a ser enviado.
 * @param longitud longitud del mensaje.
 * @return Cantidad de bytes enviados.
 */
int socket_send(socket_t *self, char *mensaje, size_t longitud);

/**
 * @brief Se enlaza el servicio 'service' y se activa la cola de clientes.
 * @param longitud longitud del mensaje.
 * @param service nombre de servicio o puerto.
 * @return -1 en caso de error, 0 en caso de éxito.
 * Lado del server
 */
int socket_bind_and_listen(socket_t *self, const char *service);

/**
 * @brief Se queda a la espera de la conexión del cliente.
 * @param client cliente que será aceptado.
 * @return -1 en caso de error, 0 en caso de éxito.
 * Lado del server
 */
int socket_accept(socket_t *self, socket_t *client);


/**
 * @brief Se recibe un mensaje del peer previamente conectado.
 * @param buffer buffer donde se guardará el mensaje.
 * @param longitud tamaño del buffer.
 * @return Cantidad de bytes recibidos.
 */
int socket_receive(socket_t *self, char *buffer, size_t longitud);

/**
 * @brief Permite cerrar los canales del socket.
 * @param how: canal a cerrar (SHUT_RD, SHUT_WR, SHUT_RDWR)
 */
int socket_shutdown(socket_t *self, int how);
#endif
