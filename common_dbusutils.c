
#include "common_dbusutils.h"
#include <byteswap.h>
#include <stdio.h>
uint32_t uint32_to_le(uint32_t numero) {
    uint32_t numero_le = htonl(numero);
    return bswap_32(numero_le);
}

uint32_t le_to_uint32(uint32_t numero) {
    uint32_t numero_h = bswap_32(numero);
    return ntohl(numero_h);
}

uint16_t uint16_to_le(uint16_t numero) {
    uint16_t numero_le = htons(numero);
    return bswap_16(numero_le);
}

void print_by_bytes(void *p, size_t len) {
    size_t k;
    for (k = 0; k < len; ++k) {
        if (k % 20 == 0) printf("\n");
        if (k % 4 == 0) printf("| ");
        printf("%02X ", ((unsigned char *)p)[k]);
    }
    printf("\n");
}

size_t padding(size_t size, int modulo) {
    if (size % modulo == 0) return 0;
    return modulo - size % modulo;
}

size_t size_withpadding(size_t size, int modulo) {
    return size + padding(size, modulo);
}
