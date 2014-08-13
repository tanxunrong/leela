
#include "myc.h"
#include <glib.h>
gboolean timeout_cb(gpointer data);

static gpointer
_timer(gpointer *param) {
    GMainLoop *timeLoop = g_main_loop_new(NULL,false);
    g_timeout_add(5,timeout_cb,NULL);
    g_main_loop_run(timeLoop);
    g_main_loop_unref(timeLoop);
    return NULL;
}


gboolean timeout_cb(gpointer data)
{
    static int i=0;
    i++;
    g_print("timeout callback %d times called",i);
    if(10==i)
    {
        g_main_loop_quit((GMainLoop*)data);
        return FALSE;
    }
    return TRUE;
}

int main(int argc,char *argv[])
{
    GThread *timerThread = g_thread_new("timer",_timer,NULL);
    g_thread_join(timerThread);
    return 0;
}
