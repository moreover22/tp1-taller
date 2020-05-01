#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <byteswap.h>

#include "clientlib.h"
#include "common_socket.h"
#include "common_commandlist.h"

#define ERROR 1
#define SUCCESS 0

void print_by_bytes(void *p, size_t len) {
    size_t k;
    printf("(");
    for (k = 0; k < len; ++k)
        printf("%02X", ((unsigned char *)p)[k]);
    printf(")\n");
}
#define SO_UINT 2

int main(int argc, char const *argv[]) {
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
    while(commandlist_next(&cl, &buff) == 0) {
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
        if (i >= 4) break; //debug
    }
    commandlist_destroy(&cl);
    client_destroy(&client);
    if (input != stdin) 
        fclose(input);
    printf("SUCCESS\n");
    return SUCCESS;
}