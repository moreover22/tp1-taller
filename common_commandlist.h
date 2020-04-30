#ifndef __COMMON_COMMANDLIST_H
#define __COMMON_COMMANDLIST_H
#include <stdio.h>


typedef enum commandlist_state {SEARCHING = 0, IN_COMMAND = 1, FINISH_COMMAND, FINISH_FILE} cl_state_t;

typedef struct commandlist {
    FILE *stream;
    cl_state_t state;
    size_t start;
    size_t end;
} commandlist_t;

int commandlist_create(commandlist_t *self, FILE *stream);
int commandlist_next(commandlist_t *self, char **buffer);
int commandlist_destroy(commandlist_t *self);

#endif