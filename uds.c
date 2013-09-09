
#include "myc.h"

int main(int argc,char*argv[])
{
    int sockfd;
    struct sockaddr_un addr;
    char buf[MAXLINE];
    int n;
    memset(buf,0,MAXLINE);
    memset(&addr,0,sizeof(addr));
    addr.sun_family = AF_UNIX;
    char *sockname = "/tmp/testsock";
    strncpy(addr.sun_path,sockname,sizeof(addr.sun_path)-1);
    if((sockfd = socket(AF_UNIX,SOCK_STREAM,0)) < 0)
        err_sys("create sock");
    if(bind(sockfd,(struct sockaddr *)&addr,sizeof(addr)) < 0)
        err_sys("bind");
    if(listen(sockfd,LISTEN_QUEUE) < 0)
        err_sys("listen");

    int connfd;
    if ((connfd = accept(sockfd,NULL,0)) < 0 )
        err_sys("accept");
    int count = 0;
    while(1)
        {
        if ((n=read(connfd,buf,MAXLINE)) < 0)
        {
            perror("read");
            break;
        }
        else if(n == 0)
            sleep(1);
        else
        {
            count++;
//            fputs(buf,stdout);
            memset(buf,0,MAXLINE);
        }
    }
    printf("count %d \n",count);
    close(sockfd);
    unlink(sockname);
    return 0;
}
