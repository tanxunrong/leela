
#include "myc.h"
#include <glib.h>

typedef struct sock_conn {
    int sockfd;
    int epIdx;
    int status;
    char wbuf[MAXLINE];
    char rbuf[MAXLINE];
//    struct sockaddr_in cliaddr;
}*sock_conn_t;

static pthread_t listenTh;
static int listen_fd;
static sig_atomic_t shut_down=0;
#define CONN_MAXFD 65536
static sock_conn_t conn_table[CONN_MAXFD] = {0};

#define EPOLL_NUM 6
#define THREAD_PER_EPOLL_FD 8
#define NUM_TOTAL_WORKERS (EPOLL_NUM * THREAD_PER_EPOLL_FD)
static int epFd[EPOLL_NUM];
static pthread_t worker[NUM_TOTAL_WORKERS];

void shut_down_server(void *arg)
{
    shut_down=1;
}

void *listenWorker(void *arg)
{
    int lisEpfd=epoll_create(10);

    struct epoll_event evReg;
    evReg.events = EPOLLIN;
    evReg.data.fd = lisEpfd;

    epoll_ctl(lisEpfd,EPOLL_CTL_ADD,listen_fd,&evReg);

    struct epoll_event evHappen;

    int rrindex=0;
    while (shut_down == 0)
    {
        int numEvent=epoll_wait(lisEpfd,&evHappen,1,1000);
        if(numEvent > 0)
        {
            int sockfd = accept(listen_fd,NULL,NULL);
            setfdnonblock(sockfd);

            evReg.data.fd = sockfd;
            evReg.events = EPOLLIN | EPOLLOUT | EPOLLONESHOT;

            conn_table[sockfd].epIdx = rrindex;

            epoll_ctl(epFd[rrindex],EPOLL_CTL_ADD,sockfd,&evReg);
            rrindex = (rrindex+1) % EPOLL_NUM;
        }
    }
}

int handleWriteEvent(sock_conn_t conn)
{

}

int handleReadEvent(sock_conn_t conn)
{

}

void closeConnection(sock_conn_t conn)
{

}

void *workerThread(void *arg)
{
    int epfd = *(int*)arg;
    struct epoll_event evHappen;
    struct epoll_event evReg;

    while(shut_down == 0)
    {
        int numEvents = epoll_wait(epfd,&evHappen,1,1000);
        if (numEvents > 0)
        {
            int sock = evHappen.data.fd;
            sock_conn_t conn = &conn_table[sock];
            if (evHappen.events & EPOLLOUT)
            {
                if (handleWriteEvent(conn) == -1)
                {
                    closeConnection(conn);
                    continue;
                }

            }
        }
    }
}

int main(int argc,char* argv[])
{
    struct sigaction sigact;
    memset(&sigact,0,sizeof(sigact));

    sigact.sa_handler=shut_down_server;
    sigaction(SIGINT,&sigact,NULL);
    sigaction(SIGTERM,&sigact,NULL);

    for(int i=0;i<EPOLL_NUM;i++)
    {
        epFd[i]=epoll_create(20);
    }

    if ((listen_fd = socket(AF_INET,SOCK_STREAM,0)) < 0)
        err_sys("create socket");

    struct sockaddr_in servaddr;
    memset(&servaddr,0,sizeof(servaddr));
    servaddr.sin_family=AF_INET;
    servaddr.sin_port=htons(3789);
    if( inet_pton(AF_INET,"127.0.0.1",&servaddr.sin_addr) <= 0)
        err_quit ("inet_pton");

    //bind & listen
    if (bind(listen_fd,(struct sockaddr *)&servaddr,sizeof(servaddr)) < 0)
        err_sys("bind");
    if (listen(listen_fd,LISTEN_QUEUE) < 0)
        err_sys("listen");
    setfdnonblock(listen_fd);

    pthread_create(&listenTh,NULL,(void*)listenWorker,NULL);
    for(int i=0;i<EPOLL_NUM;i++)
    {
        for(int j=0;j<THREAD_PER_EPOLL_FD;j++)
        {
            pthread_create(worker+i*EPOLL_NUM+j,NULL,(void*)workerThread,epFd+i);
        }
    }

    for(int i=0;i<NUM_TOTAL_WORKERS;i++)
    {
        pthread_join(worker[i],NULL);
    }
    pthread_join(listen_fd,NULL);

    struct epoll_event evReg;
    for(int j=0;j<CONN_MAXFD;j++)
    {
        sock_conn_t conn = conn_table+i;
        if(conn->status)
        {
            epoll_ctl(conn->epIdx,EPOLL_CTL_DEL,conn->sockfd,&evReg);
            close(conn->sockfd);
        }
    }
    for(int i=0;i<EPOLL_NUM;i++)
    {
        close(epFd[i]);
    }
    close(listen_fd);

    return 0;
}
