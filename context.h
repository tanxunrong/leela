
#ifndef LEELA_CONTEXT_H
#define LEELA_CONTEXT_H

#include "msg_queue.h"
#include "module.h"
#include <glib.h>


struct leela_context {
//    void * instance;
//    struct skynet_module * mod;
    guint ref;
    guint32 handle;
    gint session_id;
    struct leela_msg_queue *queue;
};

struct leela_context *leela_context_new(const gchar *name,const gchar *param);
gint leela_context_release(struct leela_context *ctx);

#endif
