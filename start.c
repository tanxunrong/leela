#include "start.h"

static void
free_monitor(struct monitor *m)
{
    for(int i =0;i<m->count;i++)
    {
        leela_monitor_delete(m->m[i]);
    }
    g_free(m->m);
    g_free(m);
}

static void *
_timer(void *p)
{
    struct monitor *m = p;
    while(1)
    {
        if (context_num_get() < 0)
        {
            break;
        }

        if (m->sleep >= 1)
        {
            g_cond_signal(&m->cond);
        }

        usleep(50000);
    }

    g_cond_broadcast(&m->cond);
    return NULL;
}

static void *
_monitor(void *p) {
    struct monitor * monitor = p;
    int i;
    int n = monitor->count;
    for (;;) {
        if (context_num_get() == 0)
        {
            break;
        }

        for (i=0;i<n;i++) {
            leela_monitor_check(monitor->m[i]);
        }
        for (i=0;i<5;i++){
            sleep(1);
        }
    }

    return NULL;
}

static void *
_worker(void *param)
{
    struct worker_parm *wp = param;
    int id = wp->id;
    int weight = wp->weight;
    struct monitor *m = wp->m;
    struct leela_monitor *monitor = m->m[id];
    struct leela_msg_queue *mq = NULL;
    while(1)
    {
        mq = leela_context_msg_dispatch(monitor,mq,weight);
        if (mq == NULL)
        {
            g_mutex_lock(&m->mutex);
            m->sleep ++;
            g_cond_wait(&m->cond,&m->mutex);
            --m->sleep;
            g_mutex_unlock(&m->mutex);
        }
    }
    return NULL;
}

void
_start_worker(guint thread_num)
{
#define LEELA_EXTRA_THREADNUM 2
    GThread *all_threads[thread_num+LEELA_EXTRA_THREADNUM];

    struct monitor *m = g_malloc0(sizeof(*m));
    g_mutex_init(&m->mutex);
    g_cond_init(&m->cond);
    m->count = thread_num;
    m->sleep = 0;

    m->m = g_malloc0(thread_num * sizeof(struct leela_monitor *));
    int i = 0;
    for(i=0;i<thread_num;i++)
    {
        m->m[i] = leela_monitor_new();
    }

    all_threads[0] = g_thread_new("monitor",_monitor,m);
    all_threads[1] = g_thread_new("timer",_timer,m);

    static int weight[] = {
        -1, -1, -1, -1, 0, 0, 0, 0,
        1, 1, 1, 1, 1, 1, 1, 1,
        2, 2, 2, 2, 2, 2, 2, 2,
        3, 3, 3, 3, 3, 3, 3, 3, };
    struct worker_parm wp[thread_num];

    for(i=0;i<thread_num;i++)
    {
        wp[i].id = i;
        wp[i].m = m;
        if (i < sizeof(weight)/sizeof(weight[0])) {
            wp[i].weight= weight[i];
        } else {
            wp[i].weight = 0;
        }

        all_threads[i+LEELA_EXTRA_THREADNUM] = g_thread_new("worker",_worker,&wp[i]);
    }

    for(i=0;i<thread_num+LEELA_EXTRA_THREADNUM;i++)
    {
        g_thread_join(all_threads[i]);
    }

    free_monitor(m);
}
