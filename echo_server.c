
#include "myc.h"
#include <event2/event.h>
#include <glib.h>

void communicate_client(evutil_socket_t fd,short what,void *extra)
{
    const char *data = extra;
    printf("get an event on socket %d : %s %s %s %s \n extra data %s\n",
           (int)fd,
           (what & EV_TIMEOUT) ? "timeout" : " ",
           (what & EV_READ) ? "read" : " ",
           (what & EV_WRITE) ? "write" : " ",
           (what & EV_SIGNAL) ? "signal" : " ",
           data);
    if (what & EV_READ)
    {
        char buf[MAXLINE];
        memset(buf,0,MAXLINE);
        int n = read(fd,buf,MAXLINE);
        buf[n]='\0';
        fputs(buf,stdout);
    }
    if (what & EV_WRITE)
        write(fd,"got you",strlen("got you")+1);

}

int main(int argc,char* argv[])
{
    int listen_fd;
    struct sockaddr_in servaddr,cliaddr;
    char buf[MAXLINE];
    char addr[MAXLINE];

    struct event_config *base_config = event_config_new();
    event_config_avoid_method(base_config,"select");
//    struct timeval msec_100 = {0,100*1000};
    struct event_base * main_base = event_base_new_with_config(base_config);
    if (main_base == NULL)
        err_sys("create event base");
    else
    {
        printf("event base backend use %s \n",event_base_get_method(main_base));
    }
    event_config_free(base_config);

    memset(&cliaddr,0,sizeof(cliaddr));
    memset(buf,0,MAXLINE);
    memset(addr,0,MAXLINE);

    servaddr.sin_family=AF_INET;
    servaddr.sin_port=htons(3789);
    if( inet_pton(AF_INET,"127.0.0.1",&servaddr.sin_addr) <= 0)
        err_quit ("inet_pton");

    if ((listen_fd = socket(AF_INET,SOCK_STREAM,0)) < 0)
        err_sys("create socket");

    if (bind(listen_fd,(struct sockaddr *)&servaddr,sizeof(servaddr)) < 0)
        err_sys("bind");

    if (listen(listen_fd,LISTEN_QUEUE) < 0)
        err_sys("listen");

    setfdnonblock(listen_fd);

    socklen_t cliaddr_len = 0;
    FILE *logfile = fopen("/tmp/event.test.log","w+");
    if(logfile == NULL) err_sys("fopen");

    struct timeval ten_second = {10,0};
        int conn_fd;
        GOTO_ACCEPT:
        if ((conn_fd = accept(listen_fd,(struct sockaddr *)&cliaddr,&cliaddr_len)) < 0)
        {
            if (errno == EAGAIN || errno == EWOULDBLOCK)
            {
                sleep(1);
                goto GOTO_ACCEPT;
            }
        }
        printf("peer addr port %d \n",ntohl(cliaddr.sin_port));
        inet_ntop(cliaddr.sin_family,(void *)&cliaddr.sin_addr,addr,MAXLINE);
        printf("peer addr host %s \n",addr);
        struct event * sock_event;
        sock_event= event_new(main_base,conn_fd,EV_TIMEOUT | EV_READ | EV_WRITE ,
                              (void*)communicate_client,"conn");
        if (sock_event == NULL)
            err_sys("event_new");
        event_add(sock_event,&ten_second);

        event_base_dump_events(main_base,logfile);
    event_base_dispatch(main_base);
    event_base_free(main_base);
    return 0;
}
