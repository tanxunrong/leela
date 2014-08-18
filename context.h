
#ifndef LEELA_CONTEXT_H
#define LEELA_CONTEXT_H

#include "msg_queue.h"
#include "module.h"
#include "handle.h"
#include "monitor.h"
#include "leela.h"

#include <glib.h>
#include <stdio.h>
#include <stdlib.h>

struct leela_context {
    void * instance;
    struct leela_module * module;
    struct leela_msg_queue *queue;
    leela_callback callback;
    gchar result[32];
    gpointer userdata;
    gint ref;
    guint32 handle;
    gint session_id;
    gboolean init;
    gboolean endless;
};

struct leela_context * leela_context_new(const char * name, const char * param);

gint leela_context_newsession(struct leela_context *);

void leela_context_grab(struct leela_context *);

struct leela_context * leela_context_release(struct leela_context *);

guint32 leela_context_handle(struct leela_context *ctx);

void leela_context_init(struct leela_context *ctx, guint32 handle);

gint leela_context_push(guint32 handle, struct leela_msg *msg);

void leela_context_send
(struct leela_context * context, gpointer userdata, gsize sz, guint32 source, gint type, gint session);

struct leela_msg_queue *
leela_context_msg_dispatch(struct leela_monitor *, struct leela_msg_queue *, gint weight);

gint context_num_get();



void
leela_context_dispatch_all(struct leela_context * context);

void
leela_context_endless(guint32 handle);

void
leela_globalinit();

void
leela_globalexit(void);

void
leela_initthread(int m);

#endif
