#ifndef __COMMON_DBUSUTILS_H
#define __COMMON_DBUSUTILS_H

#include <arpa/inet.h>
#include <stddef.h>

#define SIZE_UINT16 2
#define SIZE_UINT32 4
#define SIZE_NULL 1
#define SIZE_BYTE 1
#define ALIGN 8
#define HEADER_START (4 * SIZE_UINT32)
#define INFORMATION_PARAM 4
#define NULL_CHARACTER 0
#define FOUR_BYTES 4

#define PARAM_DELIMITER ' '
#define START_ARG '(' 
#define ARG_DELIMITER ','
#define END_ARG ')'

#define STRING_TYPE 's'

#define LE 'l'
#define METHOD_CALL 1
#define PROTOCOL_VERSION 1

#define ID_DESTINO 6
#define MSG_DESTINO "Destino"
#define ID_RUTA 1
#define MSG_RUTA "Ruta"
#define ID_INTERFAZ 2
#define MSG_INTERFAZ "Interfaz"
#define ID_METODO 3
#define MSG_METODO "Metodo"
#define ID_PARAMETROS 8
#define MSG_PARAMETROS "Parametros"

#define START_HEADER {LE, METHOD_CALL, NULL_CHARACTER, PROTOCOL_VERSION}
#define PATH_ID {ID_RUTA, 1, 'o', NULL_CHARACTER}
#define INTERFAZ_ID {ID_INTERFAZ, 1, STRING_TYPE, NULL_CHARACTER}
#define FUNCION_ID {ID_METODO, 1, STRING_TYPE, NULL_CHARACTER}
#define DESTINO_ID {ID_DESTINO, 1, STRING_TYPE, NULL_CHARACTER}
#define FIRMA_ID {ID_PARAMETROS, 1, 'g', NULL_CHARACTER}


#define OK_MSG "OK\n"
#define LEN_OK_MSG 3

#define MSG_ID_FORMAT "* Id: 0x%08x\n"
#define ITEM_BULLET "* "
#define ITEM_END ":"
#define ITEM_FORMAT " %s\n"
#define PARAM_FORMAT "%s\n"
#define PARAM_BULLET "    * "
#define RESPONSE_CLIENT_FORMAT "0x%08x: %s"

uint32_t uint32_to_le(uint32_t numero);
uint16_t uint16_to_le(uint16_t numero);
uint32_t le_to_uint32(uint32_t numero);

size_t padding(size_t size, int modulo);
size_t size_withpadding(size_t size, int modulo);
void print_by_bytes(void *p, size_t len);
#endif
