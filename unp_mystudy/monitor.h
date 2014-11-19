
#ifndef LEELA_MONITOR_H
#define LEELA_MONITOR_H

#include <glib.h>

struct leela_monitor {
    gint version;
    gint check_version;
    guint src;
    guint dest;
};

struct leela_monitor * leela_monitor_new();
void leela_monitor_delete(struct leela_monitor *);
void leela_monitor_trigger(struct leela_monitor *, guint source, guint destination);
void leela_monitor_check(struct leela_monitor *);

#endif
