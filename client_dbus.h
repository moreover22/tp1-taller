#ifndef __CLIENT_DBUS_H
#define __CLIENT_DBUS_H
#include <stddef.h>
#include <arpa/inet.h>
#include "clientlib.h"
#include "common_buffer.h"
#include "clientlib.h"

typedef int (*dbus_callback_t) (char *buff, size_t len, void *ctx);

typedef struct dbus {
    dbus_callback_t callback;
    const char *command;
    size_t command_len;
    client_t client;
    size_t cant_argumentos;
    size_t header_size;
    size_t body_size;
    buffer_t header_buffer;
    buffer_t body_buffer;
    uint32_t id;
    char **arg_types[5];
    size_t last_argument_id;
} dbus_t;

int dbus_create(dbus_t *self, dbus_callback_t callback);
int dbus_destroy(dbus_t *self);
int dbus_process(dbus_t *self, const char * command);

int dbus_create_client(dbus_t *self, const char *host, const char *service);
int dbus_destroy_client(dbus_t *self);

#endif
