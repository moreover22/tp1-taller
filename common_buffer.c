#include <stdlib.h>
#include <string.h>
#include "common_buffer.h"

int buffer_create(buffer_t *self) {    
    self->i = 0;
    return BUFFER_SUCCESS;
}

int buffer_set_size(buffer_t *self, size_t size) {
    self->buffer = malloc(size * sizeof(char));
    if (self->buffer == NULL) 
        return BUFFER_ERROR;
    memset(self->buffer, 0, size);
    return BUFFER_SUCCESS;
}

int buffer_move(buffer_t *self, size_t n) {
    self->i += n;
    return BUFFER_SUCCESS;
}

int buffer_write(buffer_t *self, const char *src, size_t src_i, size_t size) {
    strncpy(&self->buffer[self->i], &src[src_i], size);
    buffer_move(self, size);
    return BUFFER_SUCCESS;
}

int buffer_write_all(buffer_t *self, const char *src, size_t size) {
    buffer_write(self, src, 0, size);
    return BUFFER_SUCCESS;
}
int buffer_next(buffer_t *self) {
    self->i++;
    return BUFFER_SUCCESS;
}

int buffer_destroy(buffer_t *self) {
    free(self->buffer);
    return BUFFER_SUCCESS;
}
