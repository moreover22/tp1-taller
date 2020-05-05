#include <stdio.h>
#include <string.h>

#include "clientlib.h"
#include "client_dbus.h"
#include "common_buffer.h"
#include "client_dbusbuffer.h"
#include "client_dbusheader.h"
#include "client_dbusbody.h"
#include "common_dbusutils.h"

#define SUCCESS 0
#define ERROR 0

static void _init_arg_types(dbus_t *self) {
    const char *DESTINO_ID_T = (char []) DESTINO_ID;
    const char *PATH_ID_T = (char []) PATH_ID;
    const char *INTERFAZ_ID_T = (char []) INTERFAZ_ID;
    const char *FUNCION_ID_T = (char []) FUNCION_ID;
    const char *FIRMA_ID_T = (char []) FUNCION_ID;

    memcpy(&self->arg_types[0], DESTINO_ID_T, INFORMATION_PARAM);
    memcpy(&self->arg_types[1], PATH_ID_T, INFORMATION_PARAM);
    memcpy(&self->arg_types[2], INTERFAZ_ID_T, INFORMATION_PARAM);
    memcpy(&self->arg_types[3], FUNCION_ID_T, INFORMATION_PARAM);
    memcpy(&self->arg_types[4], FIRMA_ID_T, INFORMATION_PARAM);
}

int dbus_create(dbus_t *self, dbus_callback_t callback) {
    _init_arg_types(self);
    self->id = 1;
    self->callback = callback;
    return SUCCESS;
}

int dbus_destroy(dbus_t *self) {
    return SUCCESS;
}

int dbus_create_client(dbus_t *self, const char *host, const char *service) {
    return client_create(&self->client, host, service);
}

int dbus_destroy_client(dbus_t *self) {
    return client_destroy(&self->client);
}

static int _dbusbuffers_create_and_fill(dbus_t *self, dbusheader_t *header, 
                                                            dbusbody_t *body) {
    dbusheader_create(header, self);
    dbusbody_create(body, self);
    dbusbody_prepare(body);
    dbusheader_prepare(header);
    dbusbody_fill(body);
    dbusheader_fill(header);
    return SUCCESS;
}
static int _dbusbuffers_destroy(dbus_t *self, dbusheader_t *header, 
                                                            dbusbody_t *body) {
    dbusbody_destroy(body);
    dbusheader_destroy(header);
    return SUCCESS;
}

static int _dbusbuffers_act(dbus_t *self) {
    buffer_act(&self->header_buffer, self->callback, self);
    buffer_act(&self->body_buffer, self->callback, self);
    return SUCCESS;
}

int dbus_process(dbus_t *self, const char *command) {
    self->cant_argumentos = 0;
    self->last_argument_id = 0;
    self->command = command;
    self->command_len = strlen(command);
    dbusheader_t header;
    dbusbody_t body;
    _dbusbuffers_create_and_fill(self, &header, &body);
    _dbusbuffers_act(self);
    _dbusbuffers_destroy(self, &header, &body);

    char response[LEN_OK_MSG + 1];
    if (client_receive(&self->client, response, LEN_OK_MSG) <= 0) {
        return ERROR;
    }
    response[LEN_OK_MSG] = 0;
    
    printf(RESPONSE_CLIENT_FORMAT, self->id, response);
    
    self->id++;
    return SUCCESS;
}
