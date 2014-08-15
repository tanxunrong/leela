
#include "context.h"

static void
leela_del_context(struct leela_context *ctx)
{
    leela_mq_release(ctx->queue,g_free);
    g_free(ctx);
    ///@todo other stuff
    ///     mq_mark_release
    ///     context_dec
    ///     module_instance_release
}

gint leela_context_release(struct leela_context *ctx)
{
    if (g_atomic_int_dec_and_test(&ctx->ref) == FALSE)
    {
        leela_del_context(ctx);
        return 0;
    }
    return -1;
}
