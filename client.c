#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>

#include "clientlib.h"
#include "client_dbus.h"
#define ERROR 1
#define SUCCESS 0


int enviar_callback(char *buff, size_t len, void *ctx) {
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
    dbus_create(&dbus, enviar_callback);

    if (dbus_process_file_and_send(&dbus, input, argv[1], argv[2]) != 0) {
        fprintf(stderr, "No se puedo codificar");
        if (input != stdin) fclose(input);
        return ERROR;
    }
    dbus_destroy(&dbus);    
    if (input != stdin) 
        fclose(input);
    return SUCCESS;
}
