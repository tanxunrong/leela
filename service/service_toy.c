
#include <mruby.h>
#include <mruby/array.h>
#include <mruby/class.h>
#include <mruby/compile.h>
#include <mruby/data.h>
#include <mruby/dump.h>
#include <mruby/error.h>
#include <mruby/gc.h>
#include <mruby/hash.h>
#include <mruby/irep.h>
#include <mruby/debug.h>
#include <mruby/numeric.h>
#include <mruby/object.h>
#include <mruby/opcode.h>
#include <mruby/proc.h>
#include <mruby/range.h>
#include <mruby/re.h>
#include <mruby/string.h>
#include <mruby/value.h>
#include <mruby/variable.h>

#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "../leela.h"
#include "../context.h"

struct toy {
    mrb_state *mrb;
    struct leela_context * ctx;
};


static int
_init(struct toy *toy, struct leela_context *ctx, const char *msg, size_t sz) {
    mrb_state *M = toy->mrb;
    toy->ctx = ctx;

    mrb_value snctx = mrb_cptr_value(M,toy->ctx);
    struct RClass *skynetClass = mrb_class_get(M,"Leela");
    g_assert(skynetClass != NULL);
    mrb_define_const(M,skynetClass,"@@CTX",snctx);

    mrb_load_file(M,'./toy/loader.rb');

    return 0;
}

static int
_launch(struct leela_context * context, void *ud, int type, int session, guint source , const gpointer msg, gsize sz) {
    g_assert(type == 0 && session == 0);
    struct toy *toy = ud;
    leela_context_callback(context, NULL, NULL);

    int err = _init(toy, context, msg, sz);
    if (err) {
        leela_command(context, "EXIT", NULL);
    }

    return 0;
}


int
toy_init(struct toy *toy, struct leela_context *ctx, const char * args) {
    int sz = strlen(args);
    gchar *tmp = g_strdup(args);
    leela_context_callback(ctx,toy,_launch);
    const gchar *self = leela_command(ctx,"REG",NULL);
    guint handle_id = strtoul(self+1,NULL,16);
    leela_send(ctx,0,handle_id,PTYPE_TAG_DONTCOPY,0,tmp,sz);
    return 0;
}

struct toy *
toy_create(void) {
    struct toy *toy = g_malloc0(sizeof(*toy));
    memset(toy,0,sizeof(*toy));
    toy->mrb = mrb_open();
    g_assert(toy->mrb != NULL);
    return toy;
}

void
toy_release(struct toy *toy) {
    mrb_close(toy->mrb);
    g_free(toy);
}


