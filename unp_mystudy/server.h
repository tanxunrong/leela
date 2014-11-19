
#ifndef LEELA_SERVER_H
#define LEELA_SERVER_H

#include <glib.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>

struct leela_conn {
    guint32 conn_id;
    int fd;
};


#endif
