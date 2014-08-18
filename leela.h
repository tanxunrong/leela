
#ifndef LEELA_H
#define LEELA_H

#include <glib.h>

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

struct leela_context;

typedef int (*leela_callback)
(struct leela_context *ctx,gpointer userdata,gint type,gint session,guint32 source,const gpointer msg,size_t size);

const char *
leela_command(struct leela_context * context, const char * cmd , const char * param);

gint
leela_send(struct leela_context * context, guint source, guint destination , gint type, gint session, gpointer data, gsize sz);

void
leela_context_callback(struct leela_context *ctx,gpointer userdata,leela_callback cb);

gint
leela_sendname(struct leela_context * context, const char * destination , int type, int session, gpointer msg, gsize sz);

#endif
