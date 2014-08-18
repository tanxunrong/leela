
#include "handle.h"

static struct leela_handle_store *GH = NULL;

/**
 * @brief leela_handle_init,init the global handle_store
 */
void leela_handle_init()
{
    g_assert(GH == NULL);
    struct leela_handle_store *h = g_malloc0(sizeof(*h));
    g_mutex_init(&h->mtx);
    h->handleAry = g_array_new(FALSE,TRUE,sizeof(struct leela_handle_name));

    /**
     * must be null
     */
    h->ctxList = NULL;
    h->handleIdx = 0;

    GH = h;
}

/**
 * @brief leela_handle_findname
 * @param name
 * @return handle_id
 */
guint leela_handle_findname(const char *name)
{
    g_assert(name != NULL);
    struct leela_handle_store *h = GH;
    guint ret = 0;

    g_mutex_lock(&h->mtx);

    for(int i=0;i< h->handleAry->len;i++)
    {
        struct leela_handle_name *ival = &g_array_index(h->handleAry,struct leela_handle_name,i);
        if (g_str_equal(ival->name,name) == TRUE)
        {
            ret = ival->handle;
            g_debug("name %s handle %d",name,ret);
            break;
        }
    }

    g_mutex_unlock(&h->mtx);
    return ret;
}

/**
 * @brief leela_name_handle
 * @param handle
 * @param name
 * @return name
 */
const char *leela_name_handle(guint handle,const char *name)
{
    g_assert(name != NULL);
    struct leela_handle_store *h = GH;

    g_mutex_lock(&h->mtx);

    for(int i=0;i< h->handleAry->len;i++)
    {
        struct leela_handle_name *ival = &g_array_index(h->handleAry,struct leela_handle_name,i);
        if (g_str_equal(&ival->name,name))
        {
            g_mutex_unlock(&h->mtx);
            return NULL;
        }
    }

    struct leela_handle_name *newHandle = g_malloc0(sizeof(*newHandle));
    newHandle->handle = handle;
    newHandle->name = g_strdup(name);
    g_array_append_val(h->handleAry,*newHandle);

    /**
     * do not free the pointer name
     */
    g_free(newHandle);

    g_mutex_unlock(&h->mtx);

    return newHandle->name;
}

/**
 * @brief leela_handle_register
 * @param ctx
 * @return
 */
guint leela_handle_register(struct leela_context *ctx)
{
    g_assert(ctx);
    struct leela_handle_store *h = GH;
    g_mutex_lock(&h->mtx);
    h->ctxList = g_list_append(h->ctxList,ctx);
    guint handle = ++h->handleIdx;

    /**
     * @todo replace with context_init func later
     */
    ctx->handle = handle;
    g_mutex_unlock(&h->mtx);
    return handle;
}

//static gint
//leela_ctx_cmp(gconstpointer element,gconstpointer data)
//{
//    const GList *ele = element;
//    guint handle = *(const guint*)data;
//    struct leela_context *ctx = g_list_nth_data(ele,0);
//    if (ctx->handle == handle)
//    {
//        return 0;
//    }
//    return 1;
//}

/**
 * @brief leela_handle_grab
 * @param handle
 * @return
 */
struct leela_context *
leela_handle_grab(guint handle)
{
    struct leela_handle_store *h = GH;
    struct leela_context *ret = NULL;

    g_mutex_lock(&h->mtx);
    GList *ll = h->ctxList;
    while(ll)
    {
        struct leela_context *ctx = (struct leela_context *)g_list_nth_data(ll,0);
        if (ctx->handle == handle)
        {
            ret = ctx;
            leela_context_grab(ret);
            break;
        }
        ll = g_list_next(h->ctxList);
    }
    g_mutex_unlock(&h->mtx);

    return ret;
}

/**
 * @brief leela_handle_retire
 * @param handle
 */
void leela_handle_retire(guint handle)
{
    struct leela_context *ctx = leela_handle_grab(handle);
    struct leela_handle_store *h=GH;
    if (ctx)
    {
        g_mutex_lock(&GH->mtx);
        GH->ctxList = g_list_remove(h->ctxList,ctx);

        for(int i=0;i< h->handleAry->len;i++)
        {
            struct leela_handle_name *ival = &g_array_index(h->handleAry,struct leela_handle_name,i);
            if (ival->handle == handle);
            {
                g_array_remove_index(h->handleAry,i);
                g_free(ival->name);
                g_free(ival);
                break;
            }
        }

        leela_context_release(ctx);
        g_mutex_unlock(&h->mtx);
    }
    else
    {
        g_error("handle %u empty",handle);
    }
}

/**
 * @brief leela_handle_retire_all
 */
void leela_handle_retire_all()
{
    struct leela_handle_store *h=GH;
    if (h->ctxList != NULL)
    {
        g_mutex_lock(&h->mtx);
//        struct leela_context *ctx = glis
        /// @brief free context
        GList *ll = h->ctxList;
        while(ll)
        {
            struct leela_context *ctx = (struct leela_context *)ll->data;
            leela_context_release(ctx);
            ll->data = NULL;
            ll = ll->next;
        }
        g_list_free(h->ctxList);

        /// @brief free handle_name
        for(int i=0;i< h->handleAry->len;i++)
        {
            struct leela_handle_name *ival = &g_array_index(h->handleAry,struct leela_handle_name,i);
            g_free(ival->name);
            ival->name = NULL;
        }
        g_assert(g_array_free(h->handleAry,TRUE) == NULL);
        g_mutex_unlock(&h->mtx);
    }
}
