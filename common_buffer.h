#ifndef __COMMON_BUFFER_H
#define __COMMON_BUFFER_H
#include <stddef.h>
#define BUFFER_SUCCESS 0
#define BUFFER_ERROR 1

typedef struct buffer {
    char *buffer;
    size_t size;
    size_t i;
} buffer_t;

typedef int (*buffer_callback_t) (char *buff, size_t len, void *ctx);

int buffer_create(buffer_t *self);
int buffer_set_size(buffer_t *self, size_t size);
int buffer_redimension(buffer_t *self, size_t new_size);
int buffer_move(buffer_t *self, size_t n);
int buffer_write(buffer_t *self, const char *src, size_t src_i, size_t size);
int buffer_write_all(buffer_t *self, const char *src, size_t size);
int buffer_next(buffer_t *self);
int buffer_act(buffer_t *self, buffer_callback_t callback, void *ctx);
int buffer_destroy(buffer_t *self);
#endif
