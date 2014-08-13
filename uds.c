
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
    GMainLoop *timeLoop = g_main_loop_new(NULL,FALSE);
    g_timeout_add(5,timeout_cb,NULL);
    g_main_loop_run(timeLoop);
    g_main_loop_quit(timeLoop);
    return NULL;
}


gboolean timeout_cb(gpointer data)
{
    if (leela_globalmq_pop() != NULL)
    {
        g_debug("global queue not empty");
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
        return FALSE;
    }
    g_debug("cb_1 called");
    return TRUE;
}

gboolean worker_cb_2(gpointer data)
{
    struct lmsg_queue *mq = data;
    if (leela_mq_length(mq) > 10)
    {
        leela_globalmq_push(mq);
        return FALSE;
    }
    g_debug("cb_2 called");
    return TRUE;
}

static gpointer
_worker(gpointer param) {
    GMainLoop *timeLoop = g_main_loop_new(NULL,FALSE);
    struct lmsg_queue *mq = leela_mq_create(1);
    int t1,t2;
    t1 = g_random_int() % 100;
    t2 = g_random_int() % 100;
    g_timeout_add(t1,worker_cb_1,mq);
    g_timeout_add(t2,worker_cb_2,mq);
    g_main_loop_run(timeLoop);
    g_main_loop_quit(timeLoop);
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
