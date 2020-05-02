#ifndef __SOCKET_CONF_H
#define __SOCKET_CONF_H

#include <sys/types.h>
#include <sys/socket.h>

#include "common_socket.h"

typedef int (*engager_t)(int sockfd, const struct sockaddr *addr, 
                                                        socklen_t addrlen);
typedef ssize_t (*interchanger_t)(int sockfd, char *buf, size_t len);

typedef int (*engage_options_t)(int fd);

typedef struct configuration {
    engager_t engager;
    interchanger_t interchanger;
    engage_options_t engage_opt;
    int ai_flags;
} configuration_t; 
#endif
