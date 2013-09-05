
#include "myc.h"

int main(int argc,char* argv[])
{
    int listen_fd,conn_fd;
    int n;
    struct sockaddr_in servaddr,cliaddr;
    char buf[MAXLINE];

    time_t ticks;

    if ((listen_fd = socket(AF_INET,SOCK_STREAM,0)) < 0)
        err_sys("create socket");
    memset(buf,0,MAXLINE);
    servaddr.sin_family=AF_INET;
    servaddr.sin_port=htons(3789);
    inet_pton(AF_INET,"127.0.0.1",&servaddr.sin_addr);

    if (bind(listen_fd,(struct sockaddr *)&servaddr,sizeof(servaddr)) < 0)
        err_sys("bind");

    if (listen(listen_fd,LISTEN_QUEUE) < 0)
        err_sys("listen");
    while (1)
    {
        if ((conn_fd = accept(listen_fd,(struct sockaddr *)NULL,0)) < 0)
        {
            err_sys("accept");
        }
        pid_t pid= fork();
        if (pid == 0)
        {
            close(listen_fd);
            while(1)
            {
                if ((n=read(conn_fd,buf,MAXLINE)) < 0)
                    err_sys("read");
                printf("pid : %d ppid : %d \n",getpid(),getppid());
                fputs(buf,stdout);

                ticks=time(NULL);
                memset(buf,0,MAXLINE);
                snprintf(buf,sizeof(buf),"%.24s\r\n",ctime(&ticks));
                write(conn_fd,buf,sizeof(buf));
                memset(buf,0,MAXLINE);
            }
            close(conn_fd);
        }
        else
        {
            printf("fork pid %d",pid);
            close(conn_fd);
        }
    }
}
