
#include "myc.h"
#include "msg_queue.h"
#include <glib.h>
gboolean timeout_cb(gpointer data);
struct lmsg *
dummy_msg()
{
    struct lmsg *ret = g_malloc0(sizeof(*ret));
//    GTimeZone *zone = g_time_zone_new_local();
    GDateTime *now = g_date_time_new_now_local();
    ret->data = g_date_time_format(now,"%F %H:%M:%S");
    ret->sz = strlen(ret->data) + 1;

    return ret;
}

static gpointer
_timer(gpointer param) {
    GMainContext *timerCtx = g_main_context_new();
    GMainLoop *timeLoop = g_main_loop_new(timerCtx,FALSE);

    g_print("ctx %p loop %p\n",timerCtx,timeLoop);

    GSource *source = g_timeout_source_new(100);
    g_source_set_callback(source,timeout_cb,NULL,NULL);
    g_source_attach(source,timerCtx);

    g_main_loop_run(timeLoop);
    g_main_context_unref(timerCtx);
    return NULL;
}


gboolean timeout_cb(gpointer data)
{
    if (leela_globalmq_pop() != NULL)
    {
g_error("global queue not empty");
        return FALSE;
    }

    return TRUE;
}

gboolean worker_cb_1(gpointer data)
{
    struct lmsg_queue *mq = data;
    struct lmsg *dummy = dummy_msg();
    if (leela_mq_push(mq,dummy) != 0)
    {
//        leela_mq_release();
        return FALSE;
    }

    g_debug("cb_1 called");
    return TRUE;
}

gboolean worker_cb_2(gpointer data)
{
    struct lmsg_queue *mq = data;
    if (leela_mq_length(mq) > 100)
    {
        leela_globalmq_push(mq);
        return FALSE;
    }

    g_debug("cb_2 called");
    return TRUE;
}

static gpointer
_worker(gpointer param) {
    GMainContext *workerCtx = g_main_context_new();

    GMainLoop *timeLoop = g_main_loop_new(workerCtx,FALSE);
    g_print("worker %p loop %p\n",workerCtx,timeLoop);
    struct lmsg_queue *mq = leela_mq_create(1);
    int t1,t2;
    t1 = g_random_int() % 100;
    t2 = g_random_int() % 100;

    GSource *source1 = g_timeout_source_new(t1);
    g_source_set_callback(source1,worker_cb_1,mq,NULL);
    g_source_attach(source1,workerCtx);

    GSource *source2 = g_timeout_source_new(t2);
    g_source_set_callback(source2,worker_cb_2,mq,NULL);
    g_source_attach(source2,workerCtx);

    g_main_loop_run(timeLoop);
    g_main_context_unref(workerCtx);
    return NULL;
}


int main(int argc,char *argv[])
{
    leela_mq_init();
    GThread *timerThread = g_thread_new("timer",_timer,NULL);
    GThread *workerThread = g_thread_new("worker",_worker,NULL);
    g_thread_join(timerThread);
    g_thread_join(workerThread);
    return 0;
}
