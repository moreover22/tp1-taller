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

int dbusinterpreter_read(dbusinterpreter_t *self, const char *src, char *dest, 
                                                                size_t len);
int dbusinterpreter_create(dbusinterpreter_t *self);

int dbusinterpreter_destroy(dbusinterpreter_t *self);

int dbusinterpreter_header_start(dbusinterpreter_t *self, char *buff);

void dbusinterpreter_show_id(dbusinterpreter_t *self);
void dbusinterpreter_show_arguments(dbusinterpreter_t *self, char *buff);
void dbusinterpreter_show_parameters(dbusinterpreter_t *self, char *buff);

int dbusinterpreter_get_header(dbusinterpreter_t *self, char **buff, 
                                                            socket_t *client);
int dbusinterpreter_get_body(dbusinterpreter_t *self, char **buff, 
                                                            socket_t *client);

int dbusinterpreter_get_and_show_body(dbusinterpreter_t *self, socket_t *skt);

int dbusinterpreter_get_and_show_header(dbusinterpreter_t *self, 
                                                                socket_t *skt);

#endif
