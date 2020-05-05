#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "server_dbusinterpreter.h"
#include "common_dbusutils.h"
#include "serverlib.h"

static void _mostrar_parametro(size_t id_parametro) {
    switch (id_parametro) {
        case ID_DESTINO:
            printf("%s", MSG_DESTINO);
            break;
        case ID_RUTA:
            printf("%s", MSG_RUTA);
            break;
        case ID_INTERFAZ:
            printf("%s", MSG_INTERFAZ);
            break;
        case ID_METODO:
            printf("%s", MSG_METODO);
            break;
        case ID_PARAMETROS:
            printf("%s", MSG_PARAMETROS);
            break;
        default:
            break;
    }
}
int dbusinterpreter_read(dbusinterpreter_t *self, const char *src, char *dest, 
                                                                size_t len) {
    memcpy(dest, &src[self->i_read], len);
    self->i_read += len;
    return 0;
}

int dbusinterpreter_create(dbusinterpreter_t *self) {
    return 0;
}

int dbusinterpreter_destroy(dbusinterpreter_t *self) {
    return 0;
}

int dbusinterpreter_header_start(dbusinterpreter_t *self, char *buff) {
    self->i_read = 0;
    uint32_t header_info;
    dbusinterpreter_read(self, buff, (char *) &header_info, SIZE_UINT32);
    dbusinterpreter_read(self, buff, (char *) &self->len_body, SIZE_UINT32);
    self->len_body = le_to_uint32(self->len_body);
    dbusinterpreter_read(self, buff, (char *)  &self->id, SIZE_UINT32);
    self->id = le_to_uint32(self->id);
    dbusinterpreter_read(self, buff, (char *) &self->len_header, SIZE_UINT32);
    self->len_header = le_to_uint32(self->len_header);
    self->len_header = size_withpadding(self->len_header, ALIGN);
    return 0;
}

void dbusinterpreter_show_id(dbusinterpreter_t *self) {
    printf(MSG_ID_FORMAT, self->id);
}

void dbusinterpreter_show_arguments(dbusinterpreter_t *self, char *buff) {
    self->i_read = 0;
    for (int i = 0; i < self->len_header; i++) {
        char param_id = buff[self->i_read];
        printf("%s", ITEM_BULLET);
        _mostrar_parametro(param_id);
        self->i_read += FOUR_BYTES;
        printf("%s", ITEM_END);
        if (param_id == ID_PARAMETROS) 
            break;
        uint32_t len_argumento;
        dbusinterpreter_read(self, buff, (char *) &len_argumento, SIZE_UINT32);
        len_argumento = le_to_uint32(len_argumento);
        printf(ITEM_FORMAT, &buff[self->i_read]);
        self->i_read += size_withpadding(len_argumento + SIZE_NULL, ALIGN);
        if (self->i_read >= self->len_header) 
            break;
    }
}
void dbusinterpreter_show_parameters(dbusinterpreter_t *self, char *buff) {
    self->i_read = 0;
    for (int i = 0; i < self->len_body; i++) {
        printf("%s", PARAM_BULLET);
        uint32_t len_parametro;
        dbusinterpreter_read(self, buff, (char *) &len_parametro, SIZE_UINT32);
        len_parametro = le_to_uint32(len_parametro);
        printf(PARAM_FORMAT, &buff[self->i_read]);
        self->i_read += len_parametro + SIZE_NULL;
        if (self->i_read >= self->len_body) 
            break;
    }
}

int dbusinterpreter_get_header(dbusinterpreter_t *self, char **buff, 
                                                            socket_t *client) {
    *buff = malloc(self->len_header * sizeof(char));
    if (*buff == NULL) {
        fprintf(stderr, "No se puedo allocar memoria.\n");
        return SERVER_ERROR;
    }
    if (socket_receive(client, *buff, self->len_header) <= 0) 
        return SERVER_ERROR;
    return 0;
}

int dbusinterpreter_get_body(dbusinterpreter_t *self, char **buff, 
                                                            socket_t *client) {
    *buff = malloc(self->len_body * sizeof(char));
    if (*buff == NULL) {
        fprintf(stderr, "No se puedo allocar memoria.\n");
        return SERVER_ERROR;
    }
    if (socket_receive(client, *buff, self->len_body) <= 0) 
        return SERVER_ERROR;
    return 0;
}

int dbusinterpreter_get_and_show_body(dbusinterpreter_t *self, socket_t *skt) {
    if (self->len_body <= 0) 
        return 1;
    char *body;
    dbusinterpreter_get_body(self, &body, skt);
    dbusinterpreter_show_parameters(self, body);
    free(body);
    printf("\n");
    return 0;
}

int dbusinterpreter_get_and_show_header(dbusinterpreter_t *self, 
                                                                socket_t *skt) {
    char *header;
    dbusinterpreter_get_header(self, &header, skt);    
    dbusinterpreter_show_arguments(self, header);
    free(header);
    printf("\n");
    return 0;
}

