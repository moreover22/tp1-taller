#include <stddef.h>
#include <stdbool.h>
#include <arpa/inet.h>
#include "client_dbus.h"
#include "client_dbusbuffer.h"
#include "client_dbusbody.h"
#include "common_dbusutils.h"

/**
 * @brief Dado un caracter, indica si corresponde a la finalización de un 
 * parámetro.
 * @param c: Caracter a analizar.
 * @return true si corresponde a un delimitador.
 */
static bool _delimiter_body(char c);

/**
 * @brief Calcula el tamaño que ocupa, una vez codificado, un parámetro en un 
 * buffer.
 * @param len_parameter: longitud del parámetro a codificar.
 */
static size_t _sizeof_parameter(size_t len_parameter);

/**
 * @brief Dentro del comando, busca el índice donde inician los parametros.
 */
static int _find_parameter_start(const char *command, size_t command_len);

/**
 * @brief Escribe dentro del buffer de body, la cantidad indicada, desde index.
 */
static void _write_parameter_body_buffer(dbus_t *self, size_t len_parametro,
                                                                size_t index);

/**
 * @brief Wrapper de _sizeof_parameter.
 */
static size_t _size_of_body_parameter(dbus_t *self, size_t len);

/**
 * @brief Calcula el tamaño total del buffer para body, se guarde dentro del
 * dbus.
 * @param size: tamaño acumulativo de los parametros leidos.
 */
static void _sizer_body_buffer(dbus_t *self, size_t size);

/**
 * @brief Cuenta la cantidad de parametros que tiene el comando de dbus y lo 
 * guarda en el dbus.
 */
static void _dbusbody_contar_parametros(dbusbody_t *self);

/**
 * @brief Completa la configuración de body para dbusbuffer. 
 */
static void fill_body_buffer_conf(dbusbufferconf_t *body_buffer_conf, 
                                                                dbus_t *dbus);

static bool _delimiter_body(char c) {
    return c == PARAM_DELIMITER || c == END_PARAM;
}
static bool _break_condition_body(char c) {
    return false;
}
static size_t _sizeof_parameter(size_t len_parameter) {
    if (len_parameter <= 0) return 0;
    return len_parameter + SIZE_UINT32 + SIZE_NULL;
}
static int _find_parameter_start(const char *command, size_t command_len) {
    int i;
    for (i = 0; i < command_len && command[i] != START_PARAM; i++) {}
    return i + 1;
}
static void _write_parameter_body_buffer(dbus_t *self, size_t len_parametro, 
                                                                size_t index) {
    if (self->body_size <= 0) return;
    uint32_t length_le = uint32_to_le(len_parametro);
    buffer_write_all(&self->body_buffer, (char *) &length_le, SIZE_UINT32);
    size_t new_index = index - len_parametro;
    buffer_write(&self->body_buffer, self->command, new_index, len_parametro);
    buffer_next(&self->body_buffer);
}

static size_t _size_of_body_parameter(dbus_t *self, size_t len) {
    return _sizeof_parameter(len);
}

static void _sizer_body_buffer(dbus_t *self, size_t size) {
    if (self->cant_parametros == 0) 
        self->body_size = 0;
    else 
        self->body_size = size;
    
    buffer_set_size(&self->body_buffer, size);
}

static void fill_body_buffer_conf(dbusbufferconf_t *body_buffer_conf, 
                                                                dbus_t *dbus) {
    body_buffer_conf->s_point = _find_parameter_start(dbus->command, 
                                                            dbus->command_len);
    body_buffer_conf->delimiter_cond = _delimiter_body;
    body_buffer_conf->size_of = _size_of_body_parameter;
    body_buffer_conf->buffer_writer = _write_parameter_body_buffer;
    body_buffer_conf->break_cond = _break_condition_body;
    body_buffer_conf->buffer_sizer = _sizer_body_buffer;
}

static void _dbusbody_contar_parametros(dbusbody_t *self) {
    int s = _find_parameter_start(self->dbus->command, self->dbus->command_len);
    self->dbus->cant_parametros = 0;
    if (self->dbus->command[s] == END_PARAM)
        return;
    for (int i = s; i < self->dbus->command_len; i++) {
        if (_delimiter_body(self->dbus->command[i]))
            self->dbus->cant_parametros++;
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
