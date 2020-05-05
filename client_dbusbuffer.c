#include "client_dbusbuffer.h"

#define SUCCESS 0

int dbusbuffer_create(dbusbuffer_t *self, dbus_t *dbus, dbusbufferconf_t conf) {
    self->dbus = dbus;
    self->conf = conf;
    return 0;
}

int dbusbuffer_fill(dbusbuffer_t *self) {
    size_t length = 0;
    char c;
    for (int i = self->conf.s_point; i < self->dbus->command_len; i++) {
        c = self->dbus->command[i];
        if (self->conf.delimiter_cond(c)) {
            self->conf.buffer_writer(self->dbus, length, i);
            length = 0;
        } else {
            length++;
        }
        if (self->conf.break_cond(c)) break;
    }
    return SUCCESS;
}

int dbusbuffer_prepare(dbusbuffer_t *self) {
    size_t total_size = 0;
    size_t length = 0;
    char c;
    for (int i = self->conf.s_point; i < self->dbus->command_len; i++) {
        c = self->dbus->command[i];
        if (self->conf.delimiter_cond(c)) {
            total_size += self->conf.size_of(self->dbus, length); 
            length = 0;
        } else {
            length++;
        }   
        if (self->conf.break_cond(c)) break;
    }
    self->conf.buffer_sizer(self->dbus, total_size);
    return SUCCESS;
}

int dbusbuffer_destroy(dbusbuffer_t *self) {
    return 0;
}
