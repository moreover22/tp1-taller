#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "clientlib.h"
#include "client_dbus.h"
#include "common_buffer.h"
#include "client_dbusbuffer.h"
#include "client_dbusheader.h"
#include "client_dbusbody.h"
#include "common_dbusutils.h"
#include "common_commandlist.h"

#define SUCCESS 0
#define ERROR 0

/**
 * @brief Le aplica la funcón de callback a los buffers de header y de body
 * del dbus.
 */
static int _dbusbuffers_act(dbus_t *self, client_t *client);

/**
 * @brief Inicializa la información de los argumentos que manejará el protocolo
 * dbus.
 */
static void _init_arg_types(dbus_t *self);

/**
 * @brief Prepara los parámetros de header y de body, reserva memoria y los 
 * completa con la información del comando de dbus.
 */
static int _dbusbuffers_create_and_fill(dbus_t *self, dbusheader_t *header, 
                                                            dbusbody_t *body);
/**
 * @brief Libera recursos de buffers.
 */
static int _dbusbuffers_destroy(dbus_t *self, dbusheader_t *header, 
                                                            dbusbody_t *body);
/**
 * @brief Codifica el comando con el protocolo dbus y llama a la función de 
 * callback asignada previamente. 
 * @param command: Comando de dbus.
 * @return 0 en caso de éxito.
 */
static int _dbus_process(dbus_t *self, const char * command, client_t *client);


int dbus_create(dbus_t *self, dbus_callback_t callback) {
    _init_arg_types(self);
    self->id = 1;
    self->callback = callback;
    return SUCCESS;
}

int dbus_process_file_and_process(dbus_t *self, FILE *input, client_t *client) {
    char *buff;
    commandlist_t cl;
    commandlist_create(&cl, input);

    while (commandlist_next(&cl, &buff) == 0) {
        if (_dbus_process(self, buff, client) != 0) {
            free(buff);
            fprintf(stderr, "Error al enviar mensaje\n");
            return ERROR;
        }
        free(buff);
    }
    commandlist_destroy(&cl);
    return 0;
}

static void _init_arg_types(dbus_t *self) {
    const char *DESTINO_ID_T = (char []) DESTINO_ID;
    const char *PATH_ID_T = (char []) PATH_ID;
    const char *INTERFAZ_ID_T = (char []) INTERFAZ_ID;
    const char *FUNCION_ID_T = (char []) FUNCION_ID;
    const char *FIRMA_ID_T = (char []) FUNCION_ID;
    memcpy(&self->arg_types[0], DESTINO_ID_T, INFORMATION_ARG);
    memcpy(&self->arg_types[1], PATH_ID_T, INFORMATION_ARG);
    memcpy(&self->arg_types[2], INTERFAZ_ID_T, INFORMATION_ARG);
    memcpy(&self->arg_types[3], FUNCION_ID_T, INFORMATION_ARG);
    memcpy(&self->arg_types[4], FIRMA_ID_T, INFORMATION_ARG);
}

static int _dbus_process(dbus_t *self, const char *command, client_t *client) {
    self->cant_parametros = 0;
    self->last_argument_id = 0;
    self->command = command;
    self->command_len = strlen(command);
    dbusheader_t header;
    dbusbody_t body;

    _dbusbuffers_create_and_fill(self, &header, &body);
    _dbusbuffers_act(self, client);
    _dbusbuffers_destroy(self, &header, &body);
    char response[LEN_OK_MSG + 1];
    if (client_receive(client, response, LEN_OK_MSG) <= 0) {
        return ERROR;
    }
    response[LEN_OK_MSG] = 0;
    printf(RESPONSE_CLIENT_FORMAT, self->id, response);
    self->id++;
    return SUCCESS;
}


int dbus_destroy(dbus_t *self) {
    return SUCCESS;
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

static int _dbusbuffers_act(dbus_t *self, client_t *client) {
    buffer_act(&self->header_buffer, self->callback, client);
    buffer_act(&self->body_buffer, self->callback, client);
    return SUCCESS;
}

static int _dbusbuffers_destroy(dbus_t *self, dbusheader_t *header, 
                                                            dbusbody_t *body) {
    dbusbody_destroy(body);
    dbusheader_destroy(header);
    return SUCCESS;
}
