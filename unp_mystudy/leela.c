
#include "leela.h"
#include "start.h"

#include <mruby.h>
#include <glib.h>


int main(int argc,char *argv[])
{
    mrb_close(mrb_open());

    leela_mq_init();
    leela_handle_init();
    leela_module_init("/home/tanxr/workspace/unp_mystudy-build/lib?.so");

    struct leela_context *logger = leela_context_new("logger","/tmp/leela.log");
    g_assert(logger);

    struct leela_context *ctx = leela_context_new("toy","load");
    g_assert(ctx);

    _start_worker(1);

    return 0;
}
