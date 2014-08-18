
#include "msg_queue.h"

struct leela_msg_queue {
	guint32 handle;
	GMutex mtx;
	GQueue *queue;
    struct leela_msg_queue *next;
};

struct leela_global_queue {
    // queue of lmsg_queue
    GQueue *qqueue;
    GMutex mtx;
};

static struct leela_global_queue *GQ = NULL;

void leela_mq_init()
{
    struct leela_global_queue *mq = g_malloc0(sizeof(*mq));
    g_mutex_init(&mq->mtx);
    mq->qqueue = g_queue_new();
    GQ = mq;
}

void
leela_globalmq_push(struct leela_msg_queue *mq)
{
    struct leela_global_queue *q = GQ;

    g_mutex_lock(&q->mtx);
    g_queue_push_tail(q->qqueue,mq);
    g_mutex_unlock(&q->mtx);

}

struct leela_msg_queue *
leela_globalmq_pop()
{
    struct leela_global_queue *q = GQ;
    struct leela_msg_queue *ret = NULL;

    g_mutex_lock(&q->mtx);
    ret = g_queue_pop_head(q->qqueue);
    g_mutex_unlock(&q->mtx);

    return ret;
}

struct leela_msg_queue *
leela_mq_create(guint32 handle)
{
    struct leela_msg_queue *mq = g_malloc0(sizeof(*mq));
	mq->handle = handle;
	mq->queue = g_queue_new();
	mq->next = NULL;
	g_mutex_init(&mq->mtx);
	return mq;
}

void leela_msg_free(gpointer data)
{
    struct leela_msg *msg = (struct leela_msg *)data;
    g_free(msg ? msg->data : NULL);
    g_free(msg);
}

void leela_mq_release(struct leela_msg_queue *mq,GDestroyNotify shit)
{
    if (mq != NULL)
    {
        g_mutex_clear(&mq->mtx);
        if (shit == NULL)
        {
            g_queue_free_full(mq->queue,leela_msg_free);
        }
        else
        {
            g_queue_free_full(mq->queue,shit);
        }
        g_free(mq);
    }
}

guint32
leela_mq_handle(struct leela_msg_queue *mq)
{
    return mq->handle;
}

guint
leela_mq_length(struct leela_msg_queue *mq)
{
    g_mutex_lock(&mq->mtx);
    guint len =  g_queue_get_length(mq->queue);
    g_mutex_unlock(&mq->mtx);
    return len;
}

gint
leela_mq_pop(struct leela_msg_queue *mq,struct leela_msg *msg)
{
    gint ret = 1;
    g_mutex_lock(&mq->mtx);

    int length = g_queue_get_length(mq->queue);
    if (length)
    {
        msg = g_queue_pop_head(mq->queue);
        ret = 0;
    }

    g_mutex_unlock(&mq->mtx);
    return ret;
}

gint
leela_mq_push(struct leela_msg_queue *mq,struct leela_msg *msg)
{
    g_assert(mq);
    g_mutex_lock(&mq->mtx);
    g_queue_push_tail(mq->queue,msg);
    if (!msg->in_global)
    {
        leela_globalmq_push(mq);
    }
    g_mutex_unlock(&mq->mtx);
    return 0;
}
