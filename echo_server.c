
#include "myc.h"

int main(int argc,char* argv[])
{
    int listen_fd,conn_fd;
    int n;
    struct sockaddr_in servaddr,cliaddr;
    char buf[MAXLINE];
    char addr[MAXLINE];
    socklen_t cliaddr_len;

    time_t ticks;

    if ((listen_fd = socket(AF_INET,SOCK_STREAM,0)) < 0)
        err_sys("create socket");
    memset(&cliaddr,0,sizeof(cliaddr));
    memset(buf,0,MAXLINE);
    memset(addr,0,MAXLINE);
    servaddr.sin_family=AF_INET;
    servaddr.sin_port=htons(3789);
    inet_pton(AF_INET,"127.0.0.1",&servaddr.sin_addr);

    if (bind(listen_fd,(struct sockaddr *)&servaddr,sizeof(servaddr)) < 0)
        err_sys("bind");

    if (listen(listen_fd,LISTEN_QUEUE) < 0)
        err_sys("listen");
    while (1)
    {
        if ((conn_fd = accept(listen_fd,(struct sockaddr *)&cliaddr,&cliaddr_len)) < 0)
            err_sys("accept");
        printf("peer addr port %u \n",ntohl(cliaddr.sin_port));
        inet_ntop(cliaddr.sin_family,(void *)&cliaddr.sin_addr,addr,MAXLINE);
        printf("peer addr host %s \n",addr);
        memset(addr,0,MAXLINE);
        int fp = fcntl(conn_fd,F_GETFL);
        fp |= FD_CLOEXEC;
        fcntl(conn_fd,F_SETFL,fp);

        pid_t pid;
        if ( (pid = fork()) == 0)
        {
            printf("pid : %d ppid : %d \n",getpid(),getppid());
            fcntl(conn_fd,F_SETFL,fcntl(conn_fd,F_GETFL) | O_NONBLOCK);
            //set_keepalive(conn_fd,5,2,1,3);
            time_t alive_time=time(NULL);
            while(1){
                if ((n=read(conn_fd,buf,MAXLINE)) < 0)
                {
                    if(errno != EAGAIN)
                        err_sys("read");
                    else
                    {
                        int cha;
                        if( (cha = (time(NULL) - alive_time)) > 6)
                        {
                            printf("lost contact for %d second\n",cha);
                            goto closeProcess;
                        }
                        sleep(1);
                        continue;
                    }
                }
                alive_time = time(NULL);
                fputs(buf,stdout);

                ticks=time(NULL);
                memset(buf,0,MAXLINE);
                snprintf(buf,sizeof(buf),"%.24s\r\n",ctime(&ticks));
                write(conn_fd,buf,sizeof(buf));
                memset(buf,0,MAXLINE);
            }
            closeProcess:
            close(conn_fd);
			printf("child %d exit\n",getpid());
			exit(0);
        }
        else
        {
            printf("fork pid %d \n",pid);
            close(conn_fd);
        }
    }
}
