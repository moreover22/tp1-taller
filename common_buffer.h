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

/**
 * @brief Constructor
 */
int buffer_create(buffer_t *self);

/**
 * @brief Dimensiona el buffer con el tamaño indicado.
 * @return 0 en caso de éxito.
 */
int buffer_set_size(buffer_t *self, size_t size);

/**
 * @brief Redimensiona el buffer con un new_size, sin perder los datos.
 * @return 0 en caso de éxito.
 */
int buffer_redimension(buffer_t *self, size_t new_size);

/**
 * @brief Mueve el cursor del buffer n posiciones.
 */
int buffer_move(buffer_t *self, size_t n);

/**
 * @brief Escribe en el buffer.
 * @param src: buffer origen.
 * @param src_i: cursor del buffer origen.
 * @param size: cantidad de bytes a escribir.
 * @return 0 en caso de éxito.
 */
int buffer_write(buffer_t *self, const char *src, size_t src_i, size_t size);

/**
 * @brief Escribe el size bytes desde el comienzo de src.
 * @return 0 en caso de éxito.
 */
int buffer_write_all(buffer_t *self, const char *src, size_t size);

/**
 * @brief Aumenta el cursor del buffer en uno.
 */
int buffer_next(buffer_t *self);

/**
 * @brief Al buffer de datos, se le aplica la función callback con su respectiva
 * longitud.
 */
int buffer_act(buffer_t *self, buffer_callback_t callback, void *ctx);

/**
 * @brief Destructor.
 */
int buffer_destroy(buffer_t *self);
#endif
