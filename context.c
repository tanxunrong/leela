
#include "context.h"

struct leela_node {
    int count;
    int init;
    guint monitor_exit;
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

static void
handle_exit(struct leela_context * context, guint handle) {
    if (handle == 0) {
        handle = context->handle;
        g_error("kill self");
    } else {
        g_printerr("kill %x",handle);
    }

    if (G_NODE.monitor_exit) {
        leela_send(context,  handle, G_NODE.monitor_exit, PTYPE_CLIENT, 0, NULL, 0);
    }

    leela_handle_retire(handle);
}

struct command_func {
    const char *name;
    const char * (*func)(struct leela_context * context, const char * param);
};

//static const char *
//cmd_timeout(struct leela_context * context, const char * param) {
//    char * session_ptr = NULL;
//    int ti = strtol(param, &session_ptr, 10);
//    int session = leela_context_newsession(context);
//    leela_timeout(context->handle, ti, session);
//    sprintf(context->result, "%d", session);
//    return context->result;
//}

static const char *
cmd_reg(struct leela_context * context, const char * param) {
    if (param == NULL || param[0] == '\0') {
        sprintf(context->result, ":%x", context->handle);
        return context->result;
    } else if (param[0] == '.') {
        return leela_name_handle(context->handle, param + 1);
    } else {
        g_error("Can't register global name %s in C", param);
        return NULL;
    }
}

//static const char *
//cmd_query(struct leela_context * context, const char * param) {
//    if (param[0] == '.') {
//        uint32_t handle = leela_handle_findname(param+1);
//        if (handle) {
//            sprintf(context->result, ":%x", handle);
//            return context->result;
//        }
//    }
//    return NULL;
//}

//static const char *
//cmd_name(struct leela_context * context, const char * param) {
//    int size = strlen(param);
//    char name[size+1];
//    char handle[size+1];
//    sscanf(param,"%s %s",name,handle);
//    if (handle[0] != ':') {
//        return NULL;
//    }
//    uint32_t handle_id = strtoul(handle+1, NULL, 16);
//    if (handle_id == 0) {
//        return NULL;
//    }
//    if (name[0] == '.') {
//        return leela_handle_namehandle(handle_id, name + 1);
//    } else {
//        leela_error(context, "Can't set global name %s in C", name);
//    }
//    return NULL;
//}

//static const char *
//cmd_now(struct leela_context * context, const char * param) {
//    uint32_t ti = leela_gettime();
//    sprintf(context->result,"%u",ti);
//    return context->result;
//}

static const char *
cmd_exit(struct leela_context * context, const char * param) {
    handle_exit(context, 0);
    return NULL;
}

//static const char *
//cmd_kill(struct leela_context * context, const char * param) {
//    uint32_t handle = 0;
//    if (param[0] == ':') {
//        handle = strtoul(param+1, NULL, 16);
//    } else if (param[0] == '.') {
//        handle = leela_handle_findname(param+1);
//    } else {
//        leela_error(context, "Can't kill %s",param);
//        // todo : kill global service
//    }
//    if (handle) {
//        handle_exit(context, handle);
//    }
//    return NULL;
//}

//static const char *
//cmd_launch(struct leela_context * context, const char * param) {
//    size_t sz = strlen(param);
//    char tmp[sz+1];
//    strcpy(tmp,param);
//    char * args = tmp;
//    char * mod = strsep(&args, " \t\r\n");
//    args = strsep(&args, "\r\n");
//    struct leela_context * inst = leela_context_new(mod,args);
//    if (inst == NULL) {
//        return NULL;
//    } else {
//        id_to_hex(context->result, inst->handle);
//        return context->result;
//    }
//}

//static const char *
//cmd_getenv(struct leela_context * context, const char * param) {
//    return leela_getenv(param);
//}

//static const char *
//cmd_setenv(struct leela_context * context, const char * param) {
//    size_t sz = strlen(param);
//    char key[sz+1];
//    int i;
//    for (i=0;param[i] != ' ' && param[i];i++) {
//        key[i] = param[i];
//    }
//    if (param[i] == '\0')
//        return NULL;

//    key[i] = '\0';
//    param += i+1;

//    leela_setenv(key,param);
//    return NULL;
//}

//static const char *
//cmd_starttime(struct leela_context * context, const char * param) {
//    uint32_t sec = leela_gettime_fixsec();
//    sprintf(context->result,"%u",sec);
//    return context->result;
//}

//static const char *
//cmd_endless(struct leela_context * context, const char * param) {
//    if (context->endless) {
//        strcpy(context->result, "1");
//        context->endless = false;
//        return context->result;
//    }
//    return NULL;
//}

//static const char *
//cmd_abort(struct leela_context * context, const char * param) {
//    leela_handle_retireall();
//    return NULL;
//}

//static const char *
//cmd_monitor(struct leela_context * context, const char * param) {
//    uint32_t handle=0;
//    if (param == NULL || param[0] == '\0') {
//        if (G_NODE.monitor_exit) {
//            // return current monitor serivce
//            sprintf(context->result, ":%x", G_NODE.monitor_exit);
//            return context->result;
//        }
//        return NULL;
//    } else {
//        if (param[0] == ':') {
//            handle = strtoul(param+1, NULL, 16);
//        } else if (param[0] == '.') {
//            handle = leela_handle_findname(param+1);
//        } else {
//            leela_error(context, "Can't monitor %s",param);
//            // todo : monitor global service
//        }
//    }
//    G_NODE.monitor_exit = handle;
//    return NULL;
//}

//static const char *
//cmd_mqlen(struct leela_context * context, const char * param) {
//    int len = leela_mq_length(context->queue);
//    sprintf(context->result, "%d", len);
//    return context->result;
//}

static struct command_func cmd_funcs[] = {
//    { "TIMEOUT", cmd_timeout },
    { "REG", cmd_reg },
//    { "QUERY", cmd_query },
//    { "NAME", cmd_name },
//    { "NOW", cmd_now },
    { "EXIT", cmd_exit },
//    { "KILL", cmd_kill },
//    { "LAUNCH", cmd_launch },
//    { "GETENV", cmd_getenv },
//    { "SETENV", cmd_setenv },
//    { "STARTTIME", cmd_starttime },
//    { "ENDLESS", cmd_endless },
//    { "ABORT", cmd_abort },
//    { "MONITOR", cmd_monitor },
//    { "MQLEN", cmd_mqlen },
    { NULL, NULL },
};

const char *
leela_command(struct leela_context * context, const char * cmd , const char * param) {
    struct command_func * method = &cmd_funcs[0];
    while(method->name) {
        if (strcmp(cmd, method->name) == 0) {
            return method->func(context, param);
        }
        ++method;
    }

    return NULL;
}


/**
 * @brief leela_context_callback
 * @param ctx
 * @param cb
 * @param userdata
 */
void leela_context_callback(struct leela_context *ctx,gpointer userdata,leela_callback cb)
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
    leela_module_instance_release(ctx->module,ctx->instance);
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
    ctx->endless = FALSE;
    ctx->handle = leela_handle_register(ctx);

    struct leela_msg_queue *mq = ctx->queue = leela_mq_create(ctx->handle);

    context_num_add();

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



/**
 * @brief leela_context_newsession
 * @param ctx
 * @return
 */
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

static void
_dispatch_msg(struct leela_context *ctx,struct leela_msg *msg)
{
    g_assert(ctx->init);

    if (!ctx->callback(ctx,ctx->userdata,msg->type,msg->session,msg->source,msg,msg->sz))
    {
        g_free(msg->data);
        g_free(msg);
    }
}


/**
 * @brief leela_context_msg_dispatch
 * @param moniter
 * @param mq
 * @param weight
 * @return
 */
struct leela_msg_queue *
leela_context_msg_dispatch(struct leela_monitor *moniter, struct leela_msg_queue *mq, gint weight)
{
    if (mq == NULL)
    {
        mq = leela_globalmq_pop();
        if (mq == NULL)
        {
            return NULL;
        }
    }

    guint handle = leela_mq_handle(mq);
    struct leela_context *ctx = leela_handle_grab(handle);
    if (ctx == NULL)
    {
        leela_mq_release(mq,NULL);
        return leela_globalmq_pop();
    }

    gint i,n = 1;
    struct leela_msg *msg = NULL;
    struct leela_msg **mstaddr = &msg;
    g_assert(mstaddr);
    for(i=0;i<n;i++)
    {
        if (leela_mq_pop(mq,msg))
        {
            leela_context_release(ctx);
            return leela_globalmq_pop();
        }
        else if(i==0 && weight >=0)
        {
            n = leela_mq_length(mq);
            n >>= weight;
        }

        leela_monitor_trigger(moniter,(msg ? msg->source : 0),handle);
        if (msg)
        {
            if (ctx->callback == NULL)
            {

                g_free(msg->data);
                g_free(msg);

            }
            else
            {
                _dispatch_msg(ctx,msg);
            }
        }
        leela_monitor_trigger(moniter,0,0);
    }

    struct leela_msg_queue *nq = leela_globalmq_pop();
    if (nq)
    {
        leela_globalmq_push(mq);
        mq = nq;
    }

    leela_context_release(ctx);

    return mq;
}

/**
 * @brief leela_context_dispatch_all
 * @param ctx
 */
void leela_context_dispatch_all(struct leela_context * ctx)
{
    struct leela_msg *msg = NULL;
    struct leela_msg_queue *mq = ctx->queue;
    if (!leela_mq_pop(mq,msg))
    {
        _dispatch_msg(ctx,msg);
    }
}

gint
leela_send(struct leela_context * ctx, guint source, guint destination , gint type, gint session, gpointer data, gsize sz)
{
    if (type & PTYPE_TAG_ALLOCSESSION)
    {
        g_assert(ctx->session_id == 0);
        session = leela_context_newsession(ctx);
    }

    if (!(type & PTYPE_TAG_DONTCOPY))
    {
        gchar *copy = g_malloc0(sz+1);
        memcpy(copy,data,sz);
        memset(copy+sz,'\0',1);
        data = copy;
    }

    if (source == 0)
    {
        source = ctx->handle;
    }

    if (destination == 0)
    {
        return session;
    }
    struct leela_msg *msg = g_malloc0(sizeof(*msg));
    msg->data = data;
    msg->source = source;
    msg->session = session;
    msg->sz = sz;
    if (leela_context_push(destination,msg))
    {
        g_free(data);
        g_free(msg);
        return -1;
    }
    return session;
}

gint
leela_sendname(struct leela_context * ctx, const char * addr , int type, int session, gpointer msg, gsize sz)
{
    guint source = ctx->handle;
    guint des = 0;

    if (addr[0] == ':')
    {
        des = strtoul(addr+1,NULL,16);
    }
    else if (addr[0] == '.')
    {
        des = leela_handle_findname(addr+1);
        if (des == 0)
        {
            if (type & PTYPE_TAG_DONTCOPY)
            {
                g_free(msg);
            }
            return session;
        }
    }

    return leela_send(ctx,source,des,type,session,msg,sz);
}

void
leela_context_endless(guint32 handle)
{
    struct leela_context *ctx = leela_handle_grab(handle);
    if (ctx == NULL)
    {
        return;
    }
    ctx->endless = TRUE;
    leela_context_release(ctx);
}

/**
 * @brief leela_globalinit
 */
void leela_globalinit()
{
    G_NODE.count = 0;
    G_NODE.init = 1;
}
