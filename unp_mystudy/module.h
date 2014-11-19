
#ifndef LEELA_MODULE_H
#define LEELA_MODULE_H

#include <string.h>
#include <glib.h>
#include <gmodule.h>

struct leela_context;

typedef void * (*leela_dl_create)(void);
typedef int (*leela_dl_init)(void * inst, struct leela_context * ctx, const char * param);
typedef void (*leela_dl_release)(void * inst);

struct leela_module {
    gchar * name;
    GModule * module;
    leela_dl_create create;
    leela_dl_init init;
    leela_dl_release release;
};

void
leela_module_insert(struct leela_module *mod);

struct leela_module *
leela_module_query(const char * name);

gpointer
leela_module_instance_create(struct leela_module *);

int
leela_module_instance_init(struct leela_module *module, gpointer inst, struct leela_context *ctx, const gchar * parm);

void
leela_module_instance_release(struct leela_module *, void *inst);

void
leela_module_init(const char *path);

#endif
