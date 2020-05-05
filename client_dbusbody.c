#include <stddef.h>
#include <stdbool.h>
#include <arpa/inet.h>
#include <stdio.h>
#include "client_dbus.h"
#include "client_dbusbuffer.h"
#include "client_dbusbody.h"
#include "common_dbusutils.h"

static bool _delimiter_body(char c) {
    return c == ARG_DELIMITER || c == END_ARG;
}
static bool _break_condition_body(char c) {
    return false;
}
static size_t _sizeof_argument(size_t len_argument) {
    if (len_argument <= 0) return 0;
    return len_argument + SIZE_UINT32 + SIZE_NULL;
}
static int _find_argument_start(const char *command, size_t command_len) {
    int i;
    for (i = 0; i < command_len && command[i] != START_ARG; i++) {}
    return i + 1;
}
static void _write_argument_body_buffer(dbus_t *self, size_t len_argumento, 
                                                                size_t index) {
    if (self->body_size <= 0) return;
    uint32_t length_le = uint32_to_le(len_argumento);
    buffer_write_all(&self->body_buffer, (char *) &length_le, SIZE_UINT32);
    size_t new_index = index - len_argumento;
    buffer_write(&self->body_buffer, self->command, new_index, len_argumento);
    buffer_next(&self->body_buffer);
}

static size_t _size_of_body_argument(dbus_t *self, size_t len) {
    return _sizeof_argument(len);
}

static void _sizer_body_buffer(dbus_t *self, size_t size) {
    if (self->cant_argumentos == 0) 
        self->body_size = 0;
    else 
        self->body_size = size;
    
    buffer_set_size(&self->body_buffer, size);
}

void fill_body_buffer_conf(dbusbufferconf_t *body_buffer_conf, dbus_t *dbus) {
    body_buffer_conf->s_point = _find_argument_start(dbus->command, 
                                                            dbus->command_len);
    body_buffer_conf->delimiter_cond = _delimiter_body;
    body_buffer_conf->size_of = _size_of_body_argument;
    body_buffer_conf->buffer_writer = _write_argument_body_buffer;
    body_buffer_conf->break_cond = _break_condition_body;
    body_buffer_conf->buffer_sizer = _sizer_body_buffer;
}


static void _dbusbody_contar_parametros(dbusbody_t *self) {
    int s = _find_argument_start(self->dbus->command, self->dbus->command_len);
    self->dbus->cant_argumentos = 0;
    if (self->dbus->command[s] == END_ARG)
        return;
    for (int i = s; i < self->dbus->command_len; i++) {
        if (_delimiter_body(self->dbus->command[i]))
            self->dbus->cant_argumentos++;
    }
}

void dbusbody_create(dbusbody_t *self, dbus_t *dbus) {
    dbusbufferconf_t buffer_conf;
    self->dbus = dbus;
    _dbusbody_contar_parametros(self);  
    buffer_create(&self->dbus->body_buffer);
    fill_body_buffer_conf(&buffer_conf, self->dbus);
    dbusbuffer_create(&self->buffer, self->dbus, buffer_conf);
}

void dbusbody_prepare(dbusbody_t *self) {
    dbusbuffer_prepare(&self->buffer);
}

void dbusbody_fill(dbusbody_t *self) {
    dbusbuffer_fill(&self->buffer);
}

void dbusbody_destroy(dbusbody_t *self) {
    dbusbuffer_destroy(&self->buffer);
    buffer_destroy(&self->dbus->body_buffer);
}
