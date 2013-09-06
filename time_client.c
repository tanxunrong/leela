
#include "myc.h"

int main(int argc,char* argv[])
{
    int sockfd,n;
    char recvline[MAXLINE + 1];
    struct sockaddr_in servaddr;

    if (argc != 3)
    {
        err_quit("Usage: daytime_client <IPAdress> <message>");
    }

    if ((sockfd = socket(AF_INET,SOCK_STREAM,0)) < 0)
    {
        err_quit("create socket err");
    }
    memset(recvline,0,sizeof(recvline));
    servaddr.sin_family=AF_INET;
    servaddr.sin_port=htons(3789);

    if (inet_pton(AF_INET,argv[1],&servaddr.sin_addr) <= 0)
    {
        err_sys("IPv4 host addr false");
    }

    if ( connect(sockfd,(struct sockaddr *)&servaddr,sizeof(struct sockaddr_in)) < 0)
    {
        err_sys("connect error");
    }

    if ( (n = write(sockfd,argv[2],strlen(argv[2])+1)) < 0 )
    {
        err_sys("send msg fail");
    }

    while ( (n = read(sockfd,recvline,MAXLINE)) > 0)
    {
        recvline[n]='\0';
        if (fputs(recvline,stdout) == EOF)
        {
            err_sys("fputs error");
        }
    }

    if (n < 0)
    {
        err_sys("read error");
    }

    return 0;
}

