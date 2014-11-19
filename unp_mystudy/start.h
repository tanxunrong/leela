#ifndef LEELA_START_H
#define LEELA_START_H

#include "leela.h"
#include "module.h"
#include "context.h"
#include "monitor.h"
#include <glib.h>
#include <unistd.h>

struct monitor {
    int count;
    struct leela_monitor ** m;
    GCond cond;
    GMutex mutex;
    int sleep;
};

struct worker_parm {
    struct monitor *m;
    int id;
    int weight;
};

void
_start_worker(guint worker_num);

#endif
