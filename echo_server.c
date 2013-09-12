
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
/*
    signal(SIGPIPE,SIG_IGN);

    while(1)
    {
        int conn_fd=0;
        if ((conn_fd = accept(listen_fd,(struct sockaddr *)&cliaddr,&cliaddr_len)) < 0)
        {
            if (errno == EAGAIN || errno == EWOULDBLOCK)
                sleep(1);
        }
        else
        {
            printf("peer addr port %u \n",ntohl(cliaddr.sin_port));
            inet_ntop(cliaddr.sin_family,(void *)&cliaddr.sin_addr,addr,MAXLINE);
            printf("peer addr host %s \n",addr);
            maxfd = (maxfd >= conn_fd+1 ) ? maxfd : conn_fd+1;
            FD_SET(conn_fd,&allfds);
        }
        FD_ZERO(&rfds);
        FD_ZERO(&wfds);
//        FD_ZERO(&expfds);
        int realMaxFd=0;
        for(int j=0;j<maxfd;j++)
        {
            if(FD_ISSET(j,&allfds) > 0)
            {
//                printf("conn %d still exists \n",j);
                FD_SET(j,&rfds);FD_SET(j,&wfds);//FD_SET(j,&expfds);
                realMaxFd = (realMaxFd < j) ? j : realMaxFd;
            }
        }
        maxfd = realMaxFd+1;

        int ready;
        if( (ready = select(maxfd,&rfds,&wfds,NULL,&timeout)) < 0)
            err_sys ("select");
        else if (ready  == 0)
            continue;
        else
        {

            ticks=time(NULL);
            memset(wbuf,0,MAXLINE);
            snprintf(wbuf,sizeof(buf),"%.24s\r\n",ctime(&ticks));
            for(int i=0;i<maxfd;i++)
            {
                if(FD_ISSET(i,&rfds) > 0)
                {
//                    printf("fd %d ready to read \n",i);
                    memset(rbuf,0,MAXLINE);
                    if((rn =recv(i,rbuf,MAXLINE,0)) < 0 )
                        perror("recv");
                    else if (rn == 0)
                    {
                        printf("sock fd %d shut down orderly \n",i);
                        FD_CLR(i,&allfds);
                        close(i);
                        break;
                    }
                }
                if (FD_ISSET(i,&wfds) > 0)
                {
//                    printf("fd %d ready to write \n",i);
                    if (write(i,wbuf,MAXLINE) <= 0 )
                    {
                        perror("write");
                        FD_CLR(i,&allfds);
                        close(i);
                    }
                }
            }

        }
        timeout.tv_sec = 0;
        timeout.tv_usec = 100;
    }*/
}
