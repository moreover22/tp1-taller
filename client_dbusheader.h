#ifndef __CLIENT_DBUSHEADER_H
#define __CLIENT_DBUSHEADER_H
#include "client_dbus.h"
#include "client_dbusbuffer.h"

typedef struct dbusheader {
    dbus_t *dbus;
    dbusbuffer_t buffer;
} dbusheader_t;

void dbusheader_create(dbusheader_t *self, dbus_t *dbus);
void dbusheader_prepare(dbusheader_t *self);
void dbusheader_fill(dbusheader_t *self);
void dbusheader_destroy(dbusheader_t *self);

#endif
