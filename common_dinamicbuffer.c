#include "common_dinamicbuffer.h"

#define TAM_INICIAL 100 
#define REDIM_PROP 1.5

/**
 * @brief En caso de que no entren 'len' bytes en el buffer, se redimensiona sin
 * perder información
 * @return 0 en caso de éxito.
 */
static int _dinamicbuffer_redimension(dinamicbuffer_t *self, size_t len);

int dinamicbuffer_create(dinamicbuffer_t *self) {
    buffer_create(&self->buffer);
    if (buffer_set_size(&self->buffer, TAM_INICIAL) != 0)
        return 1;
    
    self->len = 0;
    self->tam_actual = TAM_INICIAL;
    return 0;
}

static int _dinamicbuffer_redimension(dinamicbuffer_t *self, size_t len) {
    if (self->len + len < self->tam_actual){
        return 0;
    }
    if (buffer_redimension(&self->buffer, self->tam_actual * REDIM_PROP) != 0) {
        return 1;
    }
    self->tam_actual *= REDIM_PROP;
    return 0;
}

int dinamicbuffer_append(dinamicbuffer_t *self, const char *data, 
                                                                size_t len) {
    if (_dinamicbuffer_redimension(self, len) != 0) {
        return 1;
    }
    if (buffer_write_all(&self->buffer, data, len) != 0) {
        return 1;
    }
    self->len += len;
    return 0;
}

int dinamicbuffer_destroy(dinamicbuffer_t *self) {
    buffer_destroy(&self->buffer);
    return 0;
}
int dinamicbuffer_act(dinamicbuffer_t *self, buffer_callback_t callback, 
                                                                void *ctx) {
    buffer_act(&self->buffer, callback, ctx);
    return 0;
}


