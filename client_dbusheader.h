#ifndef __CLIENT_DBUSHEADER_H
#define __CLIENT_DBUSHEADER_H
#include "client_dbus.h"
#include "client_dbusbuffer.h"

typedef struct dbusheader {
    dbus_t *dbus;
    dbusbuffer_t buffer;
} dbusheader_t;

/**
 * @brief Constructor.
 */
void dbusheader_create(dbusheader_t *self, dbus_t *dbus);

/**
 * @brief Calcula el espacio que se necesitará para manejar los datos.
 */
void dbusheader_prepare(dbusheader_t *self);

/**
 * @brief Codifica desde el 'comando' asociado al dbus, y completa el buffer.
 * @pre Previamente se debe llamar a dbusheader_prepare.
 */
void dbusheader_fill(dbusheader_t *self);

/**
 * @brief Destructor.
 */
void dbusheader_destroy(dbusheader_t *self);

#endif
