#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include "common_socket.h"
#include "common_commandlist.h"

#define ERROR 1
#define SUCCESS 0

typedef struct client {
    socket_t socket;
} client_t;

int client_create(client_t *self, const char *hostname, const char *service) {
    if (socket_create(&self->socket, CLIENT) != 0) 
        return ERROR;

    if (socket_connect(&self->socket, hostname, service) != 0)
        return ERROR;
    return SUCCESS;
}

int client_send(client_t *self, char *buffer, size_t longitud) {
    return socket_send(&self->socket, buffer, longitud);
}

int client_destroy(client_t *self) {
    return socket_destroy(&self->socket);
}


int main(int argc, char const *argv[]) {
    FILE *input;
    if (argc >= 4) 
        input = fopen(argv[3], "r");
    else
        input = stdin;
    if (!input)
        return ERROR;

    client_t client;
    if (client_create(&client, argv[1], argv[2]) != SUCCESS) {
        fprintf(stderr, "Error al crear el cliente\n");
        fclose(input);
        return ERROR;
    }

    char *buff;

    commandlist_t cl;
    commandlist_create(&cl, input);
    int i = 0; // debug
    while(commandlist_next(&cl, &buff) == 0) {
        // if (client_send(&client, buff, strlen(buff)) != SUCCESS) {
        //     free(buff);
        //     fprintf(stderr, "Error al enviar mensaje\n");
        //     return ERROR;
        // }
        free(buff);

        // printf("SUCCESS\n");
        char buff = 4;
        socket_send(&(client.socket), &buff, 2);

        socket_send(&(client.socket), "Hola", strlen("Hola"));
        
        char resp[30];
        
        socket_receive(&(client.socket), resp, 30);
        printf("|%s| --- \n", resp);
        // client_receive(&client, resp, );
        i++; // debug
        if (i >= 1) break; //debug
    }

    commandlist_destroy(&cl);
    client_destroy(&client);
    if (input != stdin) 
        fclose(input);
    printf("SUCCESS\n");
    return SUCCESS;
}