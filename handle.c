
#include "handle.h"

static struct lhandle_store *GH = NULL;
void leela_handle_init()
{
    g_assert(GH == NULL);
    struct lhandle_store *h = g_malloc0(sizeof(*h));
    g_mutex_init(&h->mtx);
    h->handleAry = g_array_new(FALSE,TRUE,sizeof(struct lhandle_name));
    h->ctxList = g_list_alloc();
    GH = h;
}

guint32 leela_handle_findname(const char *name)
{
    g_assert(name != NULL);
    struct lhandle_store *h = GH;
    guint32 ret = 0;

    g_mutex_lock(&h->mtx);

    for(int i=0;i< h->handleAry->len;i++)
    {
        struct lhandle_name *ival = &g_array_index(h->handleAry,struct lhandle_name,i);
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

const char *leela_name_handle(guint32 handle,const char *name)
{
    g_assert(name != NULL);
    struct lhandle_store *h = GH;

    g_mutex_lock(&h->mtx);

    for(int i=0;i< h->handleAry->len;i++)
    {
        struct lhandle_name *ival = &g_array_index(h->handleAry,struct lhandle_name,i);
        if (g_str_equal(&ival->name,name))
        {
            g_mutex_unlock(&h->mtx);
            return NULL;
        }
    }

    struct lhandle_name *newHandle = g_malloc0(sizeof(*newHandle));
    newHandle->handle = handle;
    newHandle->name = g_strdup(name);
    g_array_append_val(h->handleAry,*newHandle);

    g_mutex_unlock(&h->mtx);

    return newHandle->name;
}

guint
leela_handle_register(struct leela_context *ctx)
{
    g_assert(ctx);
    struct lhandle_store *h = GH;
    g_mutex_lock(&h->mtx);
    h->ctxList = g_list_append(h->ctxList,ctx);
    guint handle = ++h->handleIdx;
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

struct leela_context *
leela_handle_grab(guint handle)
{
    struct lhandle_store *h = GH;
    struct leela_context *ret = NULL;
    g_mutex_lock(&h->mtx);
    GList *ll = h->ctxList;
    while(ll != NULL)
    {
        struct leela_context *ctx = g_list_nth(ll,0);
        if (ctx->handle == handle)
        {
            ret = ll->data;
            break;
        }
        ll = g_list_next(h->ctxList);
    }
    g_mutex_unlock(&h->mtx);
    return ret;
}
