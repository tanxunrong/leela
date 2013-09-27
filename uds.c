
#include "myc.h"
#include <glib.h>

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
    GMainLoop *loop;
    loop=g_main_loop_new(NULL,FALSE);
    g_timeout_add(100,timeout_cb,loop);
    g_main_loop_run(loop);
    g_main_loop_unref(loop);
    return 0;
}
