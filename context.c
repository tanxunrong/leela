
#include "context.h"

struct leela_node {
    int count;
    int init;
};

/**
 * @brief G_NODE global node of context
 */
static struct leela_node G_NODE;

void context_num_add()
{
    g_atomic_int_inc(&G_NODE.count);
}

void context_num_del()
{
    g_atomic_int_add(&G_NODE.count,-1);
}

gint context_num_get()
{
    return G_NODE.count;
}

/**
 * @brief leela_context_callback
 * @param ctx
 * @param cb
 * @param userdata
 */
static void leela_context_callback(struct leela_context *ctx,leela_callback cb,gpointer *userdata)
{
    ctx->callback = cb;
    ctx->userdata = userdata;
}

/**
 * @brief leela_del_context
 * @param ctx
 */
static void
leela_del_context(struct leela_context *ctx)
{
    leela_mq_release(ctx->queue,g_free);
    leela_module_instance_release(ctx->module,ctx->init);
    g_free(ctx);
    context_num_del();
}

struct leela_context *leela_context_release(struct leela_context *ctx)
{
    if (g_atomic_int_dec_and_test(&ctx->ref))
    {
        leela_del_context(ctx);
        return NULL;
    }
    return ctx;
}

/**
 * @brief leela_context_new
 * @param name
 * @param param
 * @return
 */
struct leela_context * leela_context_new(const char * name, const char * param)
{
    struct leela_module *module = leela_module_query(name);
    if (module == NULL)
    {
        return NULL;
    }

    gpointer inst = leela_module_instance_create(module);
    if (inst == NULL)
    {
        return NULL;
    }

    struct leela_context *ctx = g_malloc0(sizeof(*ctx));
    ctx->module = module;
    ctx->instance = inst;
    ctx->ref = 2;
    ctx->callback = NULL;
    ctx->userdata = NULL;
    ctx->session_id = 0;
    ctx->init = FALSE;
    ctx->handle = leela_handle_register(ctx);

    struct leela_msg_queue *mq = ctx->queue = leela_mq_create(ctx->handle);

    ///@todo add global ctx num

    gint init_ok = leela_module_instance_init(module,inst,ctx,param);
    if (init_ok == 0)
    {
        struct leela_context *ret = leela_context_release(ctx);
        if (ret)
        {
            ctx->init = TRUE;
        }
        leela_globalmq_push(mq);
        if (ret)
        {
            g_print("launch %s %s",name,param);
        }
        return ret;
    }
    else
    {
        g_error("failed launch %s",name);
        guint32 handle = ctx->handle;
        leela_handle_retire(handle);
        leela_context_release(ctx);
        leela_mq_release(mq,g_free);
        return NULL;
    }
}

gint leela_context_newsession(struct leela_context * ctx)
{
    gint session = ++ctx->session_id & 0x7FFFFFFF;
    return session;
}

/**
 * @brief leela_context_grab,ref++
 * @param ctx
 */
void leela_context_grab(struct leela_context * ctx)
{
    g_atomic_int_inc(&ctx->ref);
}

/**
 * @brief leela_context_handle
 * @param ctx
 * @return
 */
guint32 leela_context_handle(struct leela_context *ctx)
{
    return ctx->handle;
}

/**
 * @brief leela_context_init
 * @param ctx
 * @param handle
 */
void leela_context_init(struct leela_context *ctx, guint32 handle)
{
    ctx->handle = handle;
}

/**
 * @brief leela_context_push
 * @param handle
 * @param message
 * @return
 */
gint leela_context_push(guint32 handle, struct leela_msg *message)
{
    struct leela_context *ctx = leela_handle_grab(handle);
    if (ctx == NULL)
    {
        return -1;
    }
    leela_mq_push(ctx->queue,message);
    leela_context_release(ctx);
    return 0;
}

/**
 * @brief leela_context_send
 * @param context
 * @param msg
 * @param sz
 * @param source
 * @param type
 * @param session
 */
void leela_context_send
(struct leela_context * ctx, gpointer userdata, gsize sz, guint32 source, gint type, gint session)
{
    struct leela_msg msg;
    msg.data = userdata;
    msg.sz = sz;
    msg.session = session;
    msg.source = source;
    msg.type = type;

    leela_mq_push(ctx->queue,&msg);
}


void leela_globalinit()
{
    G_NODE.count = 0;
    G_NODE.init = 1;
}
