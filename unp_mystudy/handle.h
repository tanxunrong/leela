
#ifndef LEELA_HANDLE_H
#define LEELA_HANDLE_H

#include "msg_queue.h"
#include "context.h"
#include <glib.h>

struct leela_handle_name {
    guint handle; /** unique in global handle_store*/
    gchar *name;
};

struct leela_handle_store {
    guint handleIdx;
    GList *ctxList;
    GMutex mtx;
    GArray *handleAry;
};

guint leela_handle_findname(const char *name);
const char *leela_name_handle(guint handle,const char *name);
void leela_handle_init();

guint
leela_handle_register(struct leela_context *ctx);

struct leela_context *
leela_handle_grab(guint handle);

void leela_handle_retire(guint handle);
void leela_handle_retire_all();

#endif
