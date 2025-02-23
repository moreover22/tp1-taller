#define _POSIX_C_SOURCE 200809L
#include "common_socket.h"

#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netdb.h>


#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>


#define SKT_ERROR -1
#define ERROR -1
#define SUCCESS 0
#define LISTEN_BACKLOG 10

#define INVALID_FD -1
typedef struct addrinfo addrinfo_t;


/**
 * @brief Dado un host y un servicio, se resuelve la información de direcciones
 * IPv 4 asociada a ese host. 
 * @param host nombre de dominio o ip en forma de cadena.
 * @param service nombre de servicio o puerto.
 * @param result la información sera entrega mediante una lista enlazada
 *              alojada en esta variable.
 * @return 0 en caso de éxito, o el código de error en caso de que falle.
 */
static int _socket_resolve_addrinfo(socket_t *self, const char *host, 
                                    const char *service, addrinfo_t **result);
/**
 * @brief Dada información de un host particular se intentará establecer una conexión.
 * @param host_info información del host al que se quiere conectar.
 * @return 0 en caso de éxito, -1 en caso de error.
 * @post En caso de éxito se asocia el file_descriptor del socket.
 */
static int _socket_engage_host(socket_t *self, addrinfo_t *host_info);

/**
 * @brief Dada una lista de hosts se intentará acoplar secuencialmente a cada uno,
 * hasta poder conectarse a alguno.
 * @param hosts lista de hosts posibles a conectarse.
 * @return devuelve true en caso de éxito, false en caso contrario.
 */
static bool _socket_multiple_engage(socket_t *self, addrinfo_t *hosts);


/**
 * @brief Se intentará asociar el servicio y host con el socket.
 * @param host nombre de dominio o ip en forma de cadena.
 * @param service nombre de servicio o puerto.
 * @return devuelve 0 en caso de éxito.
 */
static int _socket_engage_service(socket_t *self, const char *host, 
                                                        const char *service);

/**
 * @brief Se intercambiarán datos. La función de intercambio se configura antes
 * de llamar a la función.
 * @param buf buffer a emplear.
 * @param longitud tamaño del buffer.
 * @return Cantidad de bytes intercambiados.
 */
static int _socket_interchange_data(socket_t *self, char *buf, size_t length);


/**
 * @brief Función nula.
 */
static int _engage_null_options(int fd);

/**
 * @brief Se configura el socketopt para el bind.
 * @param fd file descritor asociado al socket.
 * @return 0 en caso de éxito.
 */
static int _engage_bind_options(int fd);

static ssize_t _interchanger_send(int sockfd, char *buf, size_t len);
static ssize_t _interchanger_receive(int sockfd, char *buf, size_t len);


int socket_create(socket_t *self, int how) {
    self->file_descriptor = INVALID_FD;
    switch (how) {
        case SERVER:
            self->conf.ai_flags = AI_PASSIVE;
            self->conf.engager = bind;
            self->conf.engage_opt = _engage_bind_options;
            // self->conf.engage_opt = _engage_null_options;

            break;
        case CLIENT:
            self->conf.ai_flags = AF_UNSPEC;
            self->conf.engager = connect;
            self->conf.engage_opt = _engage_null_options;
            break;
    }
    return SUCCESS;
}

int socket_destroy(socket_t *self) {
    if (self->file_descriptor != INVALID_FD) {    
        socket_shutdown(self, SHUT_RDWR);
        close(self->file_descriptor);
    }
    return SUCCESS;
}

int socket_shutdown(socket_t *self, int how) {
    return shutdown(self->file_descriptor, how);
}

int socket_send(socket_t *self, char *mensaje, size_t length) {
    self->conf.interchanger = _interchanger_send;
    return _socket_interchange_data(self, mensaje, length);
}

int socket_receive(socket_t *self, char *buf, size_t length) {
    self->conf.interchanger = _interchanger_receive;
    return _socket_interchange_data(self, buf, length);
}

int socket_connect(socket_t *self, const char *host, const char *service) {
    return _socket_engage_service(self, host, service);
}

int socket_bind_and_listen(socket_t *self, const char *service) {
    if (_socket_engage_service(self, INADDR_ANY, service) != SUCCESS) {
        return ERROR;
    }
    if (listen(self->file_descriptor, LISTEN_BACKLOG) != SUCCESS) {
        return ERROR;
    }
    return SUCCESS;
}

int socket_accept(socket_t *self, socket_t *client) {
    int peerskt = accept(self->file_descriptor, NULL, NULL);
    if (peerskt == -1)
        return ERROR;
    client->file_descriptor = peerskt;
    return SUCCESS;
}

static int _socket_resolve_addrinfo(socket_t *self, const char *host, 
                                    const char *service, addrinfo_t **result) {
    addrinfo_t hints;
    memset(&hints, 0, sizeof(addrinfo_t)); 
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = self->conf.ai_flags;
    int status = getaddrinfo(host, service, &hints, result);
    if (status == -1) {
        fprintf(stderr, "Error in getaddrinfo: %s\n", gai_strerror(status));
        return ERROR;
    }
    return SUCCESS;
}


static int _socket_engage_host(socket_t *self, addrinfo_t *host_info) {
    addrinfo_t *hi = host_info;
    int fd = socket(hi->ai_family, hi->ai_socktype, hi->ai_protocol);
    if (fd == SKT_ERROR) {
        fprintf(stderr, "Error socket: %s\n", strerror(errno));
        return ERROR;
    }
    if (self->conf.engage_opt(fd) != SUCCESS) {
        fprintf(stderr, "Error engage options: %s\n", strerror(errno));
        close(fd);
        return ERROR;
    }
    if (self->conf.engager(fd, hi->ai_addr, hi->ai_addrlen) == SKT_ERROR) {
        fprintf(stderr, "Error engage: %s\n", strerror(errno));
        close(fd);
        return ERROR;
    }
    self->file_descriptor = fd;
    return SUCCESS;
}

static bool _socket_multiple_engage(socket_t *self, addrinfo_t *hosts) {
    addrinfo_t *ptr = hosts;
    bool is_engaged = false;
    while (ptr != NULL && !is_engaged) {
        is_engaged = (_socket_engage_host(self, ptr) == SUCCESS);
        ptr = ptr->ai_next;
    }
    return is_engaged;
}

static int _socket_engage_service(socket_t *self, const char *host, 
            const char *service) {
    addrinfo_t *result;
    
    int status = _socket_resolve_addrinfo(self, host, service, &result);
    if (status == ERROR) return ERROR;
    bool is_engaged = _socket_multiple_engage(self, result);
    freeaddrinfo(result);
    if (!is_engaged) {
        fprintf(stderr, "Error. No se pudo conectar.\n");
        return ERROR;
    }
    return SUCCESS;
}

static int _socket_interchange_data(socket_t *self, char *buf, size_t length) {
    ssize_t bytes_interchanged = 0;
    ssize_t total = 0;
    ssize_t restantes = length;
    int fd = self->file_descriptor;
    interchanger_t interchanger = self->conf.interchanger;
    while (restantes > 0) {
        bytes_interchanged = interchanger(fd, &buf[total], restantes);
        if (bytes_interchanged ==  0) {
            break;
        } else if (bytes_interchanged < 0) {
            fprintf(stderr, "Error sending data: %s\n", strerror(errno));
            return ERROR;
        }
        total += bytes_interchanged;
        restantes = length - bytes_interchanged;
    }
    return total;
}

static int _engage_null_options(int fd) {
    return SUCCESS;
}

static int _engage_bind_options(int fd) {
    int val = 1;
    return setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &val, sizeof(val));
}

static ssize_t _interchanger_send(int sockfd, char *buf, size_t len) {
    return send(sockfd, buf, len, MSG_NOSIGNAL);
}

static ssize_t _interchanger_receive(int sockfd, char *buf, size_t len) {
    return recv(sockfd, buf, len, 0);
}
