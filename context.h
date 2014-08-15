
#ifndef LEELA_CONTEXT_H
#define LEELA_CONTEXT_H

#include <glib.h>

struct leela_context {
//    void * instance;
//    struct skynet_module * mod;
    guint32 handle;
    gint session_id;
    struct lmsg_queue *queue;
};


#endif
