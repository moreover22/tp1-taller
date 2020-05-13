#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>

#include "clientlib.h"
#include "client_dbus.h"
#define ERROR 1
#define SUCCESS 0


int send_message_callback(char *buff, size_t len, void *ctx) {
    dbus_t *dbus = (dbus_t *) ctx;
    if (client_send(&dbus->client, buff, len) <= 0) {
        return ERROR;
    }
    return SUCCESS;
}

int main(int argc, char const *argv[]) {
    FILE *input;
    if (argc == 4) 
        input = fopen(argv[3], "r");
    else if (argc == 3)
        input = stdin;
    else 
        return ERROR;
    if (!input)
        return ERROR;
    dbus_t dbus;
    dbus_create(&dbus, send_message_callback);
    client_t client;
    if (client_create(&client, argv[1], argv[2]) != CLIENT_SUCCESS) {
        fprintf(stderr, "Error al crear el cliente\n");
        fclose(input);
        return ERROR;
    }
    if (dbus_process_file_and_process(&dbus, input, &client) != 0) {
        fprintf(stderr, "No se puedo codificar");
        if (input != stdin) fclose(input);
        return ERROR;
    }
    client_destroy(&client);
    dbus_destroy(&dbus);
    if (input != stdin) 
        fclose(input);
    return SUCCESS;
}
