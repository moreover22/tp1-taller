#ifndef __COMMON_COMMANDLIST_H
#define __COMMON_COMMANDLIST_H
#include <stdio.h>
#define WORD_SIZE 32

typedef struct commandlist {
    FILE *stream;
    char buff_estatico[WORD_SIZE];
    size_t buff_estatico_len;
} commandlist_t;
/**
 * @brief Constructor.
 * @param stream: puntero a archivo ya abierto.
 * @return 0 en caso de éxito.
 */
int commandlist_create(commandlist_t *self, FILE *stream);

/**
 * @brief Lee el próximo comando del archivo.
 * @return 0 en caso de que haya leído un comando.
 */
int commandlist_next(commandlist_t *self, char **buffer);

/**
 * @brief Destructor.
 */
int commandlist_destroy(commandlist_t *self);
#endif
