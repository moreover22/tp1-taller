#include <stdio.h>
#include <stddef.h>
#include <stdbool.h>
#include <arpa/inet.h>

#include "client_dbus.h"
#include "client_dbusbuffer.h"
#include "client_dbusheader.h"
#include "common_dbusutils.h"


static size_t _sizeof_parameter(size_t len_parameter) {
    return INFORMATION_PARAM + SIZE_UINT32 + len_parameter;
}

static size_t _sizeof_firma(size_t cant_argumentos) {
    if (cant_argumentos == 0) 
        return 0;
    return INFORMATION_PARAM + SIZE_BYTE + cant_argumentos + SIZE_NULL;
}

static void _write_parameter_header_buffer(dbus_t *self, size_t len_parameter, 
                                                                size_t index) {
    char * arg_type = (char *) &self->arg_types[self->last_argument_id];
    buffer_write_all(&self->header_buffer, arg_type, FOUR_BYTES);
    self->last_argument_id++;
    uint32_t length_le = uint32_to_le(len_parameter);
    buffer_write_all(&self->header_buffer, (char *) &length_le, SIZE_UINT32);
    size_t new_index = index - len_parameter;
    buffer_write(&self->header_buffer, self->command, new_index, len_parameter);
    buffer_next(&self->header_buffer);
    len_parameter++;
    buffer_move(&self->header_buffer, padding(len_parameter, ALIGN));
}

static bool _delimiter_header(char c) {
    return c == PARAM_DELIMITER || c == START_ARG;
}

static bool _break_condition_header(char c) {
    return c == START_ARG;
}

static void _sizer_header_buffer(dbus_t *self, size_t size) {
    size_t size_firma = _sizeof_firma(self->cant_argumentos);
    size_t size_buffer = size + size_withpadding(size_firma, ALIGN);
    size_buffer += HEADER_START;
    self->header_size = size + size_firma;
    buffer_set_size(&self->header_buffer, size_buffer);  
}

static size_t _size_of_header_parameter(dbus_t *self, size_t len) {
    size_t len_parameter = len + SIZE_NULL;
    len_parameter = size_withpadding(len_parameter, ALIGN);
    return _sizeof_parameter(len_parameter);
}

void fill_header_buffer_conf(dbusbufferconf_t *header_buffer_conf) {
    header_buffer_conf->s_point = 0;
    header_buffer_conf->delimiter_cond = _delimiter_header;
    header_buffer_conf->size_of = _size_of_header_parameter;
    header_buffer_conf->buffer_writer = _write_parameter_header_buffer;
    header_buffer_conf->break_cond = _break_condition_header;
    header_buffer_conf->buffer_sizer = _sizer_header_buffer;
}

int _fill_header_start(dbus_t *self) {
    buffer_write_all(&self->header_buffer, (char []) START_HEADER, FOUR_BYTES);
    uint32_t le_body_size = uint32_to_le(self->body_size);
    buffer_write_all(&self->header_buffer, (char *) &le_body_size, SIZE_UINT32);
    uint32_t le_id = uint32_to_le(self->id);
    buffer_write_all(&self->header_buffer, (char *) &le_id, SIZE_UINT32);
    uint32_t le_h_size = uint32_to_le(self->header_size);
    buffer_write_all(&self->header_buffer, (char *) &le_h_size, SIZE_UINT32);
    return 0;
}

void _fill_header_signature(dbus_t *self) {
    if (self->cant_argumentos == 0)
        return;
    char * arg_type = (char []) FIRMA_ID;
    buffer_write_all(&self->header_buffer, arg_type, FOUR_BYTES);

    char le_cant_arguments = (char) self->cant_argumentos;
    buffer_write_all(&self->header_buffer, &le_cant_arguments, SIZE_BYTE);

    uint16_to_le(50);
    char type_string = STRING_TYPE;
    for (int i = 0; i < self->cant_argumentos; i++) 
        buffer_write_all(&self->header_buffer, &type_string, SIZE_BYTE);
}

void dbusheader_create(dbusheader_t *self, dbus_t *dbus) {
    dbusbufferconf_t buffer_conf;
    self->dbus = dbus;
    buffer_create(&self->dbus->header_buffer);
    fill_header_buffer_conf(&buffer_conf);
    dbusbuffer_create(&self->buffer, self->dbus, buffer_conf);
}

void dbusheader_prepare(dbusheader_t *self) {
    dbusbuffer_prepare(&self->buffer);
}

void dbusheader_fill(dbusheader_t *self) {
    _fill_header_start(self->dbus);
    dbusbuffer_fill(&self->buffer);
    _fill_header_signature(self->dbus);
}

void dbusheader_destroy(dbusheader_t *self) {
    dbusbuffer_destroy(&self->buffer);
    buffer_destroy(&self->dbus->header_buffer);
}
