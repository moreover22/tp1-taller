#ifndef __CLIENT_DBUS_H
#define __CLIENT_DBUS_H
#include <stddef.h>
#include <arpa/inet.h>
#include <stdio.h>

#include "clientlib.h"
#include "common_buffer.h"
#include "clientlib.h"

typedef int (*dbus_callback_t) (char *buff, size_t len, void *ctx);

typedef struct dbus {
    dbus_callback_t callback;
    const char *command;
    size_t command_len;
    size_t cant_parametros;
    size_t header_size;
    size_t body_size;
    buffer_t header_buffer;
    buffer_t body_buffer;
    uint32_t id;
    char **arg_types[5];
    size_t last_argument_id;
} dbus_t;

/**
 * @brief Constructor. 
 * @param callback: Función que será llamada una vez tenido el buffer con el 
 * commando ya codificado.
 * @return 0 en caso de éxito.
 */
int dbus_create(dbus_t *self, dbus_callback_t callback);

/**
 * @brief Lee los comandos del archivo input y los manda al host remoto.
 * @return 0 en caso de éxito.
 */
int dbus_process_file_and_process(dbus_t *self, FILE *input, client_t *client);

/**
 * @brief Destructor. 
 */
int dbus_destroy(dbus_t *self);
#endif
