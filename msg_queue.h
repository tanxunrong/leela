#ifndef LEELA_MSG_QUEUE_H
#define LEELA_MSG_QUEUE_H

#include <glib.h>

struct lmsg{
	guint32 source;
	gint session;
	gpointer data;
	gsize sz;
};
struct lmsg_queue;

void
leela_globalmq_push(struct lmsg_queue *mq) ;

struct lmsg_queue *
leela_globalmq_pop() ;
struct lmsg_queue *
leela_mq_create(guint32 handle) ;

void leela_mq_release(struct lmsg_queue *mq,GDestroyNotify shit) ;
guint32
leela_mq_handle(struct lmsg_queue *mq) ;

guint
leela_mq_length(struct lmsg_queue *mq) ;

gint
leela_mq_pop(struct lmsg_queue *mq,struct lmsg *msg);

gint
leela_mq_push(struct lmsg_queue *mq,struct lmsg *msg);

void leela_mq_init();
#endif
