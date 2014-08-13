
#include "msg_queue.h"

struct lmsg_queue {
	guint32 handle;
	GMutex mtx;
	GQueue *queue;
	struct lmsg_queue *next;
};

struct lglobal_queue {
    // queue of lmsg_queue
    GQueue *qqueue;
    GMutex mtx;
};

static struct lglobal_queue *GQ = NULL;

void leela_mq_init()
{
    struct lglobal_queue *mq = g_malloc0(sizeof(*mq));
    g_mutex_init(&mq->mtx);
    mq->qqueue = g_queue_new();
    GQ = mq;
}

void
leela_globalmq_push(struct lmsg_queue *mq)
{
    struct lglobal_queue *q = GQ;
    g_mutex_lock(&q->mtx);
    g_queue_push_tail(q->qqueue,mq);
    g_mutex_unlock(&q->mtx);

}

struct lmsg_queue *
leela_globalmq_pop()
{
    struct lglobal_queue *q = GQ;
    struct lmsg_queue *ret = NULL;
    g_mutex_lock(&q->mtx);

    ret = g_queue_pop_head(q->qqueue);
    g_mutex_unlock(&q->mtx);

    return ret;
}

	struct lmsg_queue *
leela_mq_create(guint32 handle)
{
	struct lmsg_queue *mq = g_malloc0(sizeof(*mq));
	mq->handle = handle;
	mq->queue = g_queue_new();
	mq->next = NULL;
	g_mutex_init(&mq->mtx);
	return mq;
}

void leela_mq_release(struct lmsg_queue *mq,GDestroyNotify shit)
{
	g_mutex_clear(&mq->mtx);
    g_queue_free_full(mq->queue,shit);
	g_free(mq);
}

	guint32
leela_mq_handle(struct lmsg_queue *mq)
{
	return mq->handle;
}

	guint
leela_mq_length(struct lmsg_queue *mq)
{
	g_mutex_lock(&mq->mtx);
	guint len =  g_queue_get_length(mq->queue);
	g_mutex_unlock(&mq->mtx);
	return len;
}

	gint
leela_mq_pop(struct lmsg_queue *mq,struct lmsg *msg)
{
	gint ret = 1;
	g_mutex_lock(&mq->mtx);

	msg = (struct lmsg *)g_queue_pop_head(mq->queue);
	if (msg == NULL)
	{
		ret = 0;
	}
	g_mutex_unlock(&mq->mtx);
	return ret;
}

	gint
leela_mq_push(struct lmsg_queue *mq,struct lmsg *msg)
{
	if (msg == NULL)
	{
        g_debug("msg nil");
	}

    gint ret = 0;
	g_mutex_lock(&mq->mtx);
    g_queue_push_tail(mq->queue,msg);
	g_mutex_unlock(&mq->mtx);
	return ret;
}
