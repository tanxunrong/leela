
#ifndef LEELA_CONTEXT_H
#define LEELA_CONTEXT_H

#include "msg_queue.h"
#include <glib.h>

struct leela_context {
//    void * instance;
//    struct skynet_module * mod;
    gint ref;
    guint32 handle;
    gint session_id;
    struct leela_msg_queue *queue;
};

gint leela_context_release(struct leela_context *ctx);

#endif
