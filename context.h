
#ifndef LEELA_CONTEXT_H
#define LEELA_CONTEXT_H

#include "msg_queue.h"
#include "module.h"
#include "handle.h"
#include <glib.h>
#include <stdio.h>

#define PTYPE_TEXT 0
#define PTYPE_RESPONSE 1
#define PTYPE_MULTICAST 2
#define PTYPE_CLIENT 3
#define PTYPE_SYSTEM 4
#define PTYPE_HARBOR 5
#define PTYPE_SOCKET 6
// read lualib/skynet.lua examples/simplemonitor.lua
#define PTYPE_ERROR 7
// read lualib/skynet.lua lualib/mqueue.lua lualib/snax.lua
#define PTYPE_RESERVED_QUEUE 8
#define PTYPE_RESERVED_DEBUG 9
#define PTYPE_RESERVED_LUA 10
#define PTYPE_RESERVED_SNAX 11

#define PTYPE_TAG_DONTCOPY 0x10000
#define PTYPE_TAG_ALLOCSESSION 0x20000


typedef int (*leela_callback)
(struct leela_context *ctx,gpointer userdata,gint type,gint session,guint32 source,const gpointer msg,size_t size);

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
};

struct leela_monitor {
    gint version;
    gint check_version;
    guint src;
    guint dest;
};

gint
leela_send(struct leela_context * context, guint source, guint destination , gint type, gint session, gpointer data, gsize sz);

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

const char *
leela_command(struct leela_context * context, const char * cmd , const char * param);

void
leela_context_callback(struct leela_context *ctx,gpointer userdata,leela_callback cb);

void
leela_context_dispatch_all(struct leela_context * context);

void
leela_context_endless(guint32 handle);   // for monitor

void
leela_globalinit();

void
leela_globalexit(void);

void
leela_initthread(int m);

#endif
