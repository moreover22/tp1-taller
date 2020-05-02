#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <byteswap.h>
#include <stdbool.h>

#include "clientlib.h"
#include "common_socket.h"
#include "common_commandlist.h"
#include "common_buffer.h"

#define ERROR 1
#define SUCCESS 0

void print_by_bytes(void *p, size_t len) {
    size_t k;
    printf("(");
    for (k = 0; k < len; ++k)
        printf("%02X ", ((unsigned char *)p)[k]);
    printf(")\n");
}

#define SO_UINT 2

#define SIZE_UINT16 4
#define SIZE_NULL 1
#define SIZE_BYTE 1
#define ALIGN 8
#define INFORMATION_PARAM 4
#define PARAM_DELIMITER ' '

#define START_ARG '(' 
#define ARG_DELIMITER ','
#define END_ARG ')'

typedef struct dbus {
    // callback
    const char *command;
    size_t command_len;
    size_t cant_argumentos;
} dbus_t;

int dbus_create(dbus_t *self) {
    return SUCCESS;
}
int dbus_destroy(dbus_t *self) {
    return SUCCESS;
}

size_t padding(size_t size, int modulo) {
    if (size % modulo == 0) return 0;
    return modulo - size % modulo;
}
size_t size_with_padding(size_t size, int modulo) {
    return size + padding(size, modulo);
}

uint16_t uint16_to_le(uint16_t numero) {
    uint16_t numero_le = htons(numero);
    return bswap_16(numero_le);
}

static size_t _sizeof_parameter(size_t len_parameter) {
    return INFORMATION_PARAM + SIZE_UINT16 + len_parameter;
}

static size_t _sizeof_firma(size_t cant_argumentos) {
    return INFORMATION_PARAM + SIZE_BYTE + cant_argumentos + SIZE_NULL;
}

static int _prepare_header_buffer(dbus_t *self, buffer_t *buffer) {
    size_t header_size = 0;
    size_t len_parameter = 0;
    char c;
    for (int i = 0; i < self->command_len; i++) {
        c = self->command[i];
        if (c == PARAM_DELIMITER || c == START_ARG) {
            len_parameter += SIZE_NULL;
            len_parameter = size_with_padding(len_parameter, ALIGN);
            header_size += _sizeof_parameter(len_parameter); 
            len_parameter = 0;
        } else {
            len_parameter++;
        }
        if (c == START_ARG) break;
    }
    size_t size_firma = _sizeof_firma(self->cant_argumentos);
    size_t size_buffer = header_size + size_with_padding(size_firma, ALIGN) + 16;
    header_size += size_firma;
    buffer_set_size(buffer, size_buffer);
    return SUCCESS;
}
static size_t _sizeof_argument(size_t len_argument) {
    return len_argument + SIZE_UINT16 + SIZE_NULL;
}
static int _find_argument_start(const char *command, size_t command_len) {
    int i;
    for (i = 0; i < command_len; i++)
        if (command[i] == START_ARG)
            break;
    return i;
}
static int _prepare_body_buffer(dbus_t *self, buffer_t *buffer) {
    int i = _find_argument_start(self->command, self->command_len);
    size_t size_body = 0;
    size_t len_argument = 0;
    char c;
    self->cant_argumentos = 0;
    for (int j = i; j < self->command_len; j++) {
        c = self->command[j];
        if (c == ARG_DELIMITER || c == END_ARG) {
            size_body += _sizeof_argument(len_argument);
            len_argument = 0;
            self->cant_argumentos++;
        } else {
            len_argument++;
        }
    }
    buffer_set_size(buffer, size_body);
    return SUCCESS;
}
static int _fill_body_buffer(dbus_t *self, buffer_t *buffer) {
    int i = _find_argument_start(self->command, self->command_len);
    size_t len_argumento = 0;
    char c;
    for (int j = i; j < self->command_len; j++) {
        c = self->command[j];
        if (c == ARG_DELIMITER || c == END_ARG) {
            uint16_t len_little_endian = uint16_to_le(len_argumento);
            buffer_write_all(buffer, (char *) &len_little_endian, SIZE_UINT16);
            buffer_write(buffer, self->command, j - len_argumento, len_argumento);
            len_argumento = 0;
        } else {
            len_argumento++;
        }
    }
    return SUCCESS;
}
static int _fill_header_buffer(dbus_t *self, buffer_t *buffer) {
    size_t len_parameter = 0;
    char c;
    for (int i = 0; i < self->command_len; i++) {
        c = self->command[i];
        if (c == PARAM_DELIMITER || c == START_ARG) {
            uint16_t len_little_endian = uint16_to_le(len_parameter);
            buffer_write_all(buffer, (char *) &len_little_endian, SIZE_UINT16);
            buffer_write(buffer, self->command, i - len_parameter, len_parameter);
            buffer_next(buffer);
            buffer_move(buffer, padding(len_parameter, ALIGN));
            len_parameter = 0;
        } else {
            len_parameter++;
        }
        if (c == START_ARG) break;
    }
    return SUCCESS;
}

int dbus_procces(dbus_t *self, const char * command) {
    self->command_len = strlen(command);
    self->command = command;
    buffer_t body_buffer;
    buffer_t header_buffer;
    buffer_create(&body_buffer);
    buffer_create(&header_buffer);
    _prepare_body_buffer(self, &body_buffer);
    _prepare_header_buffer(self, &header_buffer);
    _fill_body_buffer(self, &body_buffer);
    _fill_header_buffer(self, &header_buffer);
    print_by_bytes(header_buffer.buffer, header_buffer.i); // debug
    print_by_bytes(body_buffer.buffer, 19); // debug
    buffer_destroy(&body_buffer);
    buffer_destroy(&header_buffer);
    return SUCCESS;
}



int main(int argc, char const *argv[]) {
    char *buff = "destPrueba /taller/pruebatp taller.tp1.test llamada(foo,taller)";
    dbus_t dbus;
    dbus_create(&dbus);
    dbus_procces(&dbus, buff);
    dbus_destroy(&dbus);

    /*
    FILE *input;
    if (argc >= 4) 
        input = fopen(argv[3], "r");
    else
        input = stdin;
    if (!input)
        return ERROR;

    client_t client;
    if (client_create(&client, argv[1], argv[2]) != CLIENT_SUCCESS) {
        fprintf(stderr, "Error al crear el cliente\n");
        fclose(input);
        return ERROR;
    }

    char *buff;

    commandlist_t cl;
    commandlist_create(&cl, input);
    int i = 0; // debug 
    while (commandlist_next(&cl, &buff) == 0) {
        uint16_t len = strlen(buff);
        len = htons(len);
        len = bswap_16(len);
        client_send(&client, (char *) &len, SO_UINT);
        if (client_send(&client, buff, len) != CLIENT_SUCCESS) {
            free(buff);
            fprintf(stderr, "Error al enviar mensaje\n");
            return ERROR;
        }
        free(buff);
        char response[4];
        client_receive(&client, response, 3);
        response[3] = 0;
        printf("[DEBUG] Serv-response %s\n", response);
        i++; // debug
        if (i >= 400) break; //debug
    }
    commandlist_destroy(&cl);
    client_destroy(&client);
    if (input != stdin) 
        fclose(input);
    printf("SUCCESS\n");
    return SUCCESS;
    */
}
