#ifndef __COMMON_DINAMICBUFFER_H
#define __COMMON_DINAMICBUFFER_H
#include <stddef.h>
#include "common_buffer.h"

typedef struct dinamicbuffer {
    buffer_t buffer;
    size_t len;
    size_t tam_actual;
} dinamicbuffer_t;

/**
 * @brief Constructor.
 */
int dinamicbuffer_create(dinamicbuffer_t *self);

/**
 * @brief Escribe al final del buffer.
 * @param data: datos a escribir.
 * @param len: bytes a escribir.
 * @return 0 en caso de éxito
 */
int dinamicbuffer_append(dinamicbuffer_t *self, const char *data, size_t len);

/**
 * @brief Se aplica sobre el buffer de datos la función callback, con su 
 * longitud.
 * @return 0 en caso de éxito
 */
int dinamicbuffer_act(dinamicbuffer_t *self, buffer_callback_t callback, 
                                                                void *ctx);

/**
 * @brief Destructor.
 */
int dinamicbuffer_destroy(dinamicbuffer_t *self);
#endif
