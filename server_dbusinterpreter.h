#ifndef __SERVER_DBUSINTERPRETER_H
#define __SERVER_DBUSINTERPRETER_H
#include <stddef.h>
#include <arpa/inet.h>
#include "common_socket.h"

typedef struct dbusinterpreter {
    size_t i_read;
    uint32_t len_header;
    uint32_t len_body;
    uint32_t id;
} dbusinterpreter_t;

/**
 * @brief Constructor
 */
int dbusinterpreter_create(dbusinterpreter_t *self);

/**
 * @brief Se leen los primeros bytes del mensaje, que contiene información
 * del largo del resto del header y el body.
 */
int dbusinterpreter_header_start(dbusinterpreter_t *self, char *buff);

/**
 * @brief Imprime por stdout el id del último mensaje recibido.
 */
void dbusinterpreter_show_id(dbusinterpreter_t *self);

/**
 * @brief Se recibe desde el host remoto el body del mensaje y se imprime en el
 * stdout.
 * @param skt: socket asociado al host remoto.
 * @return 0 en caso de éxito. 
 * @pre se debe llamar a dbusinterpreter_header_start antes.
 */
int dbusinterpreter_get_and_show_body(dbusinterpreter_t *self, socket_t *skt);

/**
 * @brief Se recibe desde el host remoto el header del mensaje y se imprime en
 * el stdout.
 * @param skt: socket asociado al host remoto.
 * @return 0 en caso de éxito. 
 * @pre se debe llamar a dbusinterpreter_header_start antes.
 */
int dbusinterpreter_get_and_show_header(dbusinterpreter_t *self, 
                                                                socket_t *skt);

/**
 * @brief Destructor.
 */
int dbusinterpreter_destroy(dbusinterpreter_t *self);

#endif
