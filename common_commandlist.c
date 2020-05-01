#include "common_commandlist.h"
#include <stdlib.h>
#include <string.h>

#define EOL '\n'

int commandlist_create(commandlist_t *self, FILE *stream) {
    self->stream = stream;
    self->state = SEARCHING;
    self->start = 0;
    self->end = 0;
    return 0;
}

typedef cl_state_t (*next_state_t) (commandlist_t *self, char c);
static cl_state_t _next_state_searching(commandlist_t *self, char c) {
    cl_state_t new_state;
    if (c == EOL) {
        new_state = SEARCHING;
    } else if (c == EOF) {
        new_state = FINISH_FILE;
    } else {
        self->start = ftell(self->stream);
        self->end = self->start;
        new_state = IN_COMMAND;
    }
    return new_state;
}

static cl_state_t _next_state_in_command(commandlist_t *self, char c) {
    cl_state_t new_state;
    if (c == EOL || c == EOF) {
        new_state = FINISH_COMMAND;
    } else {
        self->end++;
        new_state = IN_COMMAND;
    }
    return new_state;
}

static void _commandlist_next_state(commandlist_t *self, char c) {
    cl_state_t state = self->state;
    next_state_t proximo_estado[] = {
        _next_state_searching,
        _next_state_in_command
    };
    if (state == SEARCHING || state == IN_COMMAND) {
        self->state = proximo_estado[state](self, c);
    }
}

static void commandlist_next_line(commandlist_t *self) {
    do {
        // TODO change to single byte reading to 32 bytes reading.
        char c = fgetc(self->stream);
        _commandlist_next_state(self, c);
    } while (self->state != FINISH_COMMAND 
                    && self->state != FINISH_FILE);
}

static int _commandlist_buffer_command(commandlist_t *self, char **buffer) {
    int longitud = self->end - self->start + 1;
    *buffer = malloc(sizeof(char) * (longitud + 1));
    if (buffer == NULL) 
        return 1;
    if (fseek(self->stream, self->start - 1, SEEK_SET) < 0) {
        free(buffer);
        return 1;
    }
    int bytes_read = fread(*buffer, sizeof(char), longitud, self->stream);
    if (bytes_read < longitud) {
        free(buffer);
        return 1;
    }
    (*buffer)[longitud] = '\0'; 
    return 0;
}

int commandlist_next(commandlist_t *self, char **buffer) {
    while (self->state != FINISH_COMMAND) {
        commandlist_next_line(self);
        if (self->state == FINISH_FILE)
            return 1;
    }

    if (_commandlist_buffer_command(self, buffer) != 0) 
        return 1;

    if (self->state != FINISH_FILE)
        self->state = SEARCHING;
    
    return 0;
}

int commandlist_destroy(commandlist_t *self) {
    // Destructor
    return 0;
}