#ifndef __CLIENT_DBUSBODY_H
#define __CLIENT_DBUSBODY_H
#include "client_dbus.h"
#include "client_dbusbuffer.h"

typedef struct dbusbody {
    dbus_t *dbus;
    dbusbuffer_t buffer;
} dbusbody_t;

/**
 * @brief Constructor.
 */
void dbusbody_create(dbusbody_t *self, dbus_t *dbus);

/**
 * @brief Calcula el espacio que se necesitará para manejar los datos.
 */
void dbusbody_prepare(dbusbody_t *self);

/**
 * @brief Codifica desde el 'comando' asociado al dbus, y completa el buffer.
 * @pre Previamente se debe llamar a dbusbody_prepare.
 */
void dbusbody_fill(dbusbody_t *self);

/**
 * @brief Destructor.
 */
void dbusbody_destroy(dbusbody_t *self);
#endif
