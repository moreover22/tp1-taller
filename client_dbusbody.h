#ifndef __CLIENT_DBUSBODY_H
#define __CLIENT_DBUSBODY_H
#include "client_dbus.h"
#include "client_dbusbuffer.h"

typedef struct dbusbody {
    dbus_t *dbus;
    dbusbuffer_t buffer;
} dbusbody_t;

void dbusbody_create(dbusbody_t *self, dbus_t *dbus);
void dbusbody_prepare(dbusbody_t *self);
void dbusbody_fill(dbusbody_t *self);
void dbusbody_destroy(dbusbody_t *self);
#endif
