#ifndef __CLIENT_DBUSBUFFER_H
#define __CLIENT_DBUSBUFFER_H
#include <stdbool.h>
#include <stddef.h>
#include "client_dbus.h"

typedef bool (*delimiter_condition_t) (char c);
typedef bool (*break_condition_t) (char c);
typedef void (*write_to_buffer_t) (dbus_t *self, size_t len, size_t i);
typedef size_t (*size_of_t) (dbus_t *self, size_t len);
typedef void (*buffer_sizer_t) (dbus_t *self, size_t size);

typedef struct dbusbufferconf {
    int s_point;
    delimiter_condition_t delimiter_cond;
    break_condition_t break_cond;
    write_to_buffer_t buffer_writer;
    size_of_t size_of;
    buffer_sizer_t buffer_sizer;
} dbusbufferconf_t;

typedef struct dbusbuffer {
    dbus_t *dbus;
    dbusbufferconf_t conf;
} dbusbuffer_t;

int dbusbuffer_create(dbusbuffer_t *self, dbus_t *dbus, dbusbufferconf_t conf);
int dbusbuffer_prepare(dbusbuffer_t *self);
int dbusbuffer_fill(dbusbuffer_t *self);
int dbusbuffer_destroy(dbusbuffer_t *self);
#endif
