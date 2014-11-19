#ifndef LEELA_MSG_QUEUE_H
#define LEELA_MSG_QUEUE_H

#include <glib.h>
#include <string.h>

struct leela_msg{
	guint32 source;
	gint session;
	gpointer data;
	gsize sz;
};

struct leela_msg_queue {
    gboolean in_global;
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

void
leela_globalmq_push(struct leela_msg_queue *mq) ;

struct leela_msg_queue *
leela_globalmq_pop() ;

struct leela_msg_queue *
leela_mq_create(guint32 handle) ;

void
leela_mq_release(struct leela_msg_queue *mq,GDestroyNotify shit) ;

guint32
leela_mq_handle(struct leela_msg_queue *mq) ;

guint
leela_mq_length(struct leela_msg_queue *mq) ;

gint
leela_mq_pop(struct leela_msg_queue *mq,struct leela_msg **msg);

gint
leela_mq_push(struct leela_msg_queue *mq,struct leela_msg *msg);

void
leela_mq_init();

#endif
