#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <sys/un.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <time.h>
#include <signal.h>
#include <fcntl.h>
#include <sys/select.h>
#include <pthread.h>

#define MAXLINE 1000
#define LISTEN_QUEUE 5

void err_quit(char* str)
{
    fprintf(stderr,"%s",str);
    exit(EXIT_FAILURE);
}

void err_sys(char *str)
{
    perror(str);
    exit(errno);
}

void setfdnonblock(int fd)
{
    int fctl = fcntl(fd,F_GETFL);
    fctl |= O_NONBLOCK;
    fcntl(fd,F_SETFL,fctl);
}

void set_keepalive(int sockfd,int alive,int idle,int intval,int count)
{
    if (alive <= 0 || idle <= 0 || count <= 0 || intval <= 0)
        err_quit("set keepalive,negative param");
    if (setsockopt(sockfd,IPPROTO_TCP,SO_KEEPALIVE,(void *)&alive,sizeof(alive)) < 0)
        err_sys("keep alive");
    /*if (setsockopt(sockfd,IPPROTO_TCP,TCP_KEEPIDLE,(void *)&idle,sizeof(idle)) < 0)
        err_sys("keep idle");
    if (setsockopt(sockfd,IPPROTO_TCP,TCP_KEEPINTVL,(void *)&intval,sizeof(intval)) < 0)
        err_sys("keep intval");
    if (setsockopt(sockfd,IPPROTO_TCP,TCP_KEEPCNT,(void *)&count,sizeof(count)) < 0)
        err_sys("keep cnt");*/
}
