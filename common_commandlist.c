#include "common_commandlist.h"
#include "common_dbusutils.h"
#include "common_buffer.h"
#include "common_bufferdinamico.h"
#include <stdlib.h>
#include <string.h>

#define EOL '\n'
/**
 * @brief Lee un comando desde el archivo del commandlist, y lo guarda en 
 * buffer bd. Un comando se delimita por EOL.
 * @return 0 en caso de éxito.
 */
static int _read_command(commandlist_t *self, bufferdinamico_t * bd);

int commandlist_create(commandlist_t *self, FILE *stream) {
    self->stream = stream;
    self->buff_estatico_len = 0;
    memset(&self->buff_estatico, 0, WORD_SIZE);
    return 0;
}

int callback_copy_to_buff(char *buff, size_t len, void *ctx) {
    char **buffer_dest = ctx;
    int actual_len = strlen(buff);
    *buffer_dest = malloc(sizeof(char) * (actual_len + 1));
        if (*buffer_dest == NULL) {
        fprintf(stderr, "No se puedo allocar memoria para comando\n");
        return 1;
    }
    memset(*buffer_dest, 0, actual_len + 1);
    memcpy(*buffer_dest, buff, actual_len);
    return 0;
}

static int _read_command(commandlist_t *self, bufferdinamico_t * bd) {
    char buffer_estatico[WORD_SIZE + 1];
    while (!feof(self->stream)) {
        memset(buffer_estatico, 0, WORD_SIZE + 1);
        int bytes_read = fread(buffer_estatico, sizeof(char), WORD_SIZE, 
                                                                self->stream);
        buffer_estatico[WORD_SIZE] = 0;
        char *end_of_line = strchr(buffer_estatico, EOL);
        if (end_of_line != NULL) {
            size_t i_eol = end_of_line - buffer_estatico + 1;
            size_t new_len = bytes_read - i_eol;
            bufferdinamico_append(bd, buffer_estatico, i_eol);
            memcpy(&self->buff_estatico, &buffer_estatico[i_eol], new_len);
            self->buff_estatico_len = new_len;
            break;
        }
        bufferdinamico_append(bd, buffer_estatico, bytes_read);
    }
    return 0;
}

int commandlist_next(commandlist_t *self, char **buffer) {
    if (feof(self->stream) && self->buff_estatico_len <= 0){
        return 1;
    }
    bufferdinamico_t bd;
    bufferdinamico_create(&bd);
    if (self->buff_estatico_len > 0) {
        bufferdinamico_append(&bd, self->buff_estatico, 
                                                    self->buff_estatico_len);
        self->buff_estatico_len = 0;
    }
    _read_command(self, &bd);
    bufferdinamico_act(&bd, callback_copy_to_buff, buffer);
    bufferdinamico_destroy(&bd);
    return 0;
}

int commandlist_destroy(commandlist_t *self) {
    return 0;
}
