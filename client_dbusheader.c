#include <stddef.h>
#include <stdbool.h>
#include <arpa/inet.h>

#include "client_dbus.h"
#include "client_dbusbuffer.h"
#include "client_dbusheader.h"
#include "common_dbusutils.h"
/**
 * @brief Calcula es espacio total que ocuparia un argumento de longitud 
 * len_argumento, una vez codificada en un buffer.
 */
static size_t _sizeof_argument(size_t len_argument);

/**
 * @brief Calcula la cantidad de espacio que ocupa la firma del header.
 */
static size_t _sizeof_signature(size_t cant_argumentos);

/**
 * @brief Escribe en el buffer de header un argumento de longitud len_argument
 * desde el index indicado.
 */
static void _write_argument_header_buffer(dbus_t *self, size_t len_argument,
                                                                size_t index);
/**
 * @brief Determina si 'c' está delimitando un argumento.
 */
static bool _delimiter_header(char c);

/**
 * @return Devuelve true una vez que 'c' indica el comienzo de parametros y la 
 * finalización de argumentos.
 */
static bool _break_condition_header(char c);

/**
 * @brief Calcula el tamaño del buffer para el header.
 */
static void _sizer_header_buffer(dbus_t *self, size_t size);

/**
 * @brief Wraper de _sizeof_argument.
 */
static size_t _size_of_header_argument(dbus_t *self, size_t len);

static size_t _sizeof_argument(size_t len_argument) {
    return INFORMATION_ARG + SIZE_UINT32 + len_argument;
}

static size_t _sizeof_signature(size_t cant_argumentos) {
    if (cant_argumentos == 0) 
        return 0;
    return INFORMATION_ARG + SIZE_BYTE + cant_argumentos + SIZE_NULL;
}

static void _write_argument_header_buffer(dbus_t *self, size_t len_argument, 
                                                                size_t index) {
    char * arg_type = (char *) &self->arg_types[self->last_argument_id];
    buffer_write_all(&self->header_buffer, arg_type, FOUR_BYTES);
    self->last_argument_id++;
    uint32_t length_le = uint32_to_le(len_argument);
    buffer_write_all(&self->header_buffer, (char *) &length_le, SIZE_UINT32);
    size_t new_index = index - len_argument;
    buffer_write(&self->header_buffer, self->command, new_index, len_argument);
    buffer_next(&self->header_buffer);
    len_argument++;
    buffer_move(&self->header_buffer, padding(len_argument, ALIGN));
}

static bool _delimiter_header(char c) {
    return c == ARG_DELIMITER || c == START_PARAM;
}

static bool _break_condition_header(char c) {
    return c == START_PARAM;
}

static void _sizer_header_buffer(dbus_t *self, size_t size) {
    size_t size_firma = _sizeof_signature(self->cant_parametros);
    size_t size_buffer = size + size_withpadding(size_firma, ALIGN);
    size_buffer += HEADER_START;
    self->header_size = size + size_firma;
    buffer_set_size(&self->header_buffer, size_buffer);  
}

static size_t _size_of_header_argument(dbus_t *self, size_t len) {
    size_t len_argument = len + SIZE_NULL;
    len_argument = size_withpadding(len_argument, ALIGN);
    return _sizeof_argument(len_argument);
}

void fill_header_buffer_conf(dbusbufferconf_t *header_buffer_conf) {
    header_buffer_conf->s_point = 0;
    header_buffer_conf->delimiter_cond = _delimiter_header;
    header_buffer_conf->size_of = _size_of_header_argument;
    header_buffer_conf->buffer_writer = _write_argument_header_buffer;
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
    if (self->cant_parametros == 0)
        return;
    char * arg_type = (char []) FIRMA_ID;
    buffer_write_all(&self->header_buffer, arg_type, FOUR_BYTES);

    char le_cant_arguments = (char) self->cant_parametros;
    buffer_write_all(&self->header_buffer, &le_cant_arguments, SIZE_BYTE);

    uint16_to_le(50);
    char type_string = STRING_TYPE;
    for (int i = 0; i < self->cant_parametros; i++) 
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
