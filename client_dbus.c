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
static int _dbusbuffers_act(dbus_t *self);

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
static int dbus_process(dbus_t *self, const char * command);

/**
 * @brief Crea el cliente con la información de los parametros. 
 * @param host: ip o hostname.
 * @param service: numero de puerto o nombre de servicio.
 * @return 0 en caso de éxito.
 */
static int dbus_create_client(dbus_t *self, const char *host, 
                                                        const char *service);

/**
 * @brief Libera recursos de conexión. 
 * @return 0 en caso de éxito.
 */
static int dbus_destroy_client(dbus_t *self);

int dbus_create(dbus_t *self, dbus_callback_t callback) {
    _init_arg_types(self);
    self->id = 1;
    self->callback = callback;
    return SUCCESS;
}


int dbus_process_file_and_send(dbus_t *self, FILE *input, const char *host, 
                                                            const char *port) {
    if (dbus_create_client(self, host, port) != CLIENT_SUCCESS) {
        fprintf(stderr, "Error al crear el cliente\n");
        fclose(input);
        return ERROR;
    }
    char *buff;
    commandlist_t cl;
    commandlist_create(&cl, input);
    while (commandlist_next(&cl, &buff) == 0) {
        if (dbus_process(self, buff) != 0) {
            free(buff);
            fprintf(stderr, "Error al enviar mensaje\n");
            return ERROR;
        }
        free(buff);
    }
    commandlist_destroy(&cl);
    dbus_destroy_client(self);
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

int dbus_create_client(dbus_t *self, const char *host, const char *service) {
    return client_create(&self->client, host, service);
}

static int dbus_process(dbus_t *self, const char *command) {
    self->cant_parametros = 0;
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

int dbus_destroy_client(dbus_t *self) {
    return client_destroy(&self->client);
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

static int _dbusbuffers_act(dbus_t *self) {
    buffer_act(&self->header_buffer, self->callback, self);
    buffer_act(&self->body_buffer, self->callback, self);
    return SUCCESS;
}

static int _dbusbuffers_destroy(dbus_t *self, dbusheader_t *header, 
                                                            dbusbody_t *body) {
    dbusbody_destroy(body);
    dbusheader_destroy(header);
    return SUCCESS;
}

