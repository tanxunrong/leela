

#include "myc.h"

int main(int argc,char* argv[])
{
    int sockfd,n;
    char recvline[MAXLINE+1];
    struct sockaddr_in servaddr;
    char buf[MAXLINE];
    memset(recvline,0,sizeof(recvline));
    servaddr.sin_family=AF_INET;
    servaddr.sin_port=htons(3789);
    if (inet_pton(AF_INET,"127.0.0.1",&servaddr.sin_addr) <= 0)
        err_sys("IPv4 host addr false");

    if ((sockfd = socket(AF_INET,SOCK_STREAM,0)) < 0)
        err_quit("create socket err");
    if ( connect(sockfd,(struct sockaddr *)&servaddr,sizeof(struct sockaddr_in)) < 0)
        err_sys("connect error");
    setfdnonblock(sockfd);
    char *msg="client msg";
    fd_set rfds,wfds;
    FD_ZERO(&rfds);FD_ZERO(&wfds);
    FD_SET(sockfd,&rfds);FD_SET(sockfd,&wfds);
    struct timeval timeout;
    timeout.tv_sec = 0;timeout.tv_usec = 200;
    while (1)
    {
        FD_ZERO(&rfds);FD_ZERO(&wfds);
        FD_SET(sockfd,&rfds);FD_SET(sockfd,&wfds);
        timeout.tv_sec = 0;timeout.tv_usec = 200;

        int ready=0;
        if ((ready = select(sockfd+1,&rfds,&wfds,NULL,&timeout)) < 0)
            err_sys("select");
        else if(ready == 0)
             continue;
        else
        {
            if(FD_ISSET(sockfd,&wfds) > 0)
            {
                write(sockfd,msg,strlen(msg));
            }
            if(FD_ISSET(sockfd,&rfds) > 0)
            {
                memset(recvline,0,MAXLINE+1);
                read(sockfd,recvline,MAXLINE);
                if (fputs(recvline,stdout) == EOF)
                {
                    err_sys("fputs error");
                }
            }
        }

    }

    return 0;
}
