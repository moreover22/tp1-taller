#ifndef __COMMON_BUFFERDINAMICO_H
#define __COMMON_BUFFERDINAMICO_H
#include <stddef.h>
#include "common_buffer.h"
typedef struct bufferdinamico {
    buffer_t buffer;
    size_t len;
    size_t tam_actual;
} bufferdinamico_t;

int bufferdinamico_create(bufferdinamico_t *self);
int bufferdinamico_append(bufferdinamico_t *self, const char *data, size_t len);
int bufferdinamico_destroy(bufferdinamico_t *self);
int bufferdinamico_act(bufferdinamico_t *self, buffer_callback_t callback, 
                                                                void *ctx);
#endif
