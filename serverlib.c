#include "serverlib.h"
#include <stdio.h>
#include <stdlib.h>
#include <byteswap.h>
#include <arpa/inet.h>
#include <string.h>
#include "common_dbusutils.h"
#include "server_dbusinterpreter.h"

#define SUCCESS 0
/**
 * @brief Intercambia mensajes con el client.
 * @return 0 en caso de éxito.
 * @pre el cliente ya se encuentra conectado.
 */
static int server_comunicate(server_t *self, socket_t *client);

int server_create(server_t *self, char *service) {
    if (socket_create(&self->socket, SERVER) != SUCCESS) {
        return SERVER_ERROR;
    }
    return socket_bind_and_listen(&self->socket, service);
}

int server_accept(server_t *self, socket_t *client) {
    if (socket_accept(&self->socket, client) != SUCCESS)
        return SERVER_ERROR;
    return SERVER_SUCCESS;
}


static int server_comunicate(server_t *self, socket_t *client) {
    char header_start[HEADER_START];

    if (socket_receive(client, (char *) &header_start, HEADER_START) <= 0) 
        return SERVER_ERROR;
    dbusinterpreter_t db_interp;
    dbusinterpreter_create(&db_interp);
    dbusinterpreter_header_start(&db_interp, header_start);
    dbusinterpreter_show_id(&db_interp);
    dbusinterpreter_get_and_show_header(&db_interp, client);
    dbusinterpreter_get_and_show_body(&db_interp, client);
    dbusinterpreter_destroy(&db_interp);
    if (socket_send(client, OK_MSG, LEN_OK_MSG) <= 0)
        return SERVER_ERROR;
    return SERVER_SUCCESS;
}

int server_run(server_t *self) {
    socket_t client;
    server_accept(self, &client);
    for (;;) {
        if (server_comunicate(self, &client) != SERVER_SUCCESS)
            break;
    }
    return SERVER_SUCCESS;
}

int server_destroy(server_t *self) {
    return socket_destroy(&self->socket);
}
