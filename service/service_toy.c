
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

#include "../context.h"

struct toy {
    mrb_state *mrb;
    struct leela_context * ctx;
};


//static int
//_init(struct toy *mrb, struct skynet_context *ctx, const char * args, size_t sz) {
//    mrb_state *M = mrb->mrb;
//    mrb->ctx = ctx;
//    return 0;
//}

//static int
//_launch(struct skynet_context * context, void *ud, int type, int session, uint32_t source , const void * msg, size_t sz) {
//    assert(type == 0 && session == 0);
//    struct toy *mrb = ud;
//    skynet_callback(context, NULL, NULL);
//    int err = _init(mrb, context, msg, sz);
//    if (err) {
//        skynet_command(context, "EXIT", NULL);
//    }

//    return 0;
//}


int
toy_init(struct toy *toy, struct leela_context *ctx, const char * args) {
    int sz = strlen(args);
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


