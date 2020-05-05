#ifndef __COMMON_BUFFERDINAMICO_H
#define __COMMON_BUFFERDINAMICO_H
#include <stddef.h>
#include "common_buffer.h"
typedef struct bufferdinamico {
    buffer_t buffer;
    size_t len;
    size_t tam_actual;
} bufferdinamico_t;

/**
 * @brief Constructor.
 */
int bufferdinamico_create(bufferdinamico_t *self);

/**
 * @brief Escribe al final del buffer.
 * @param data: datos a escribir.
 * @param len: bytes a escribir.
 * @return 0 en caso de éxito
 */
int bufferdinamico_append(bufferdinamico_t *self, const char *data, size_t len);

/**
 * @brief Se aplica sobre el buffer de datos la función callback, con su 
 * longitud.
 * @return 0 en caso de éxito
 */
int bufferdinamico_act(bufferdinamico_t *self, buffer_callback_t callback, 
                                                                void *ctx);

/**
 * @brief Destructor.
 */
int bufferdinamico_destroy(bufferdinamico_t *self);
#endif
