#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <stdbool.h>

#include "clientlib.h"
#include "common_socket.h"
#include "common_commandlist.h"
#include "client_dbus.h"
#include "common_dbusutils.h"
#define ERROR 1
#define SUCCESS 0
#define SO_UINT 2

int enviar_callback(char *buff, size_t len, void *ctx) {
    dbus_t *dbus = (dbus_t *) ctx;
    if (client_send(&dbus->client, buff, len) <= 0) {
        return ERROR;
    }
    return SUCCESS;
}

int main(int argc, char const *argv[]) {
    FILE *input;
    if (argc >= 4) 
        input = fopen(argv[3], "r");
    else
        input = stdin;
    if (!input)
        return ERROR;
    dbus_t dbus;
    dbus_create(&dbus, enviar_callback);
    if (dbus_create_client(&dbus, argv[1], argv[2]) != CLIENT_SUCCESS) {
        fprintf(stderr, "Error al crear el cliente\n");
        fclose(input);
        return ERROR;
    }

    char *buff;
    commandlist_t cl;
    commandlist_create(&cl, input);
    while (commandlist_next(&cl, &buff) == 0) {
        if (dbus_process(&dbus, buff) != 0) {
            free(buff);
            fprintf(stderr, "Error al enviar mensaje\n");
            return ERROR;
        }
        free(buff);
    }

    commandlist_destroy(&cl);
    
    dbus_destroy_client(&dbus);
    dbus_destroy(&dbus);
    
    if (input != stdin) 
        fclose(input);
    
    return SUCCESS;
}
