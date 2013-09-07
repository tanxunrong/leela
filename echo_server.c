
#include "myc.h"

int main(int argc,char* argv[])
{
    int listen_fd;
    struct sockaddr_in servaddr,cliaddr;
    char buf[MAXLINE];
    char addr[MAXLINE];
    char rbuf[MAXLINE];
    char wbuf[MAXLINE];

    socklen_t cliaddr_len;
    time_t ticks;
    fd_set rfds,wfds,allfds;
    FD_ZERO(&rfds);
    FD_ZERO(&wfds);
    FD_ZERO(&allfds);

    struct timeval timeout;
    int maxfd=0;

    timeout.tv_sec = 0;
    timeout.tv_usec = 300;
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
        int realMaxFd=0;
        for(int j=0;j<maxfd;j++)
        {
            if(FD_ISSET(j,&allfds) > 0)
            {
//                printf("conn %d still exists \n",j);
                FD_SET(j,&rfds);FD_SET(j,&wfds);
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
                    read(i,rbuf,MAXLINE);
                }
                if (FD_ISSET(i,&wfds) > 0)
                {
//                    printf("fd %d ready to write \n",i);
                    if (write(i,wbuf,MAXLINE) <= 0 )
                        perror("write");
                }
            }

        }
        timeout.tv_sec = 0;
        timeout.tv_usec = 100;
    }
    return 0;

}
