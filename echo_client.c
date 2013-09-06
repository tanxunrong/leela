

#include "myc.h"

int main(int argc,char* argv[])
{
    int sockfd,n;
    char recvline[MAXLINE+1];
    struct sockaddr_in servaddr;
    char buf[MAXLINE];

    if (argc != 2)
    {
        err_quit("Usage: daytime_client <IPAdress> <message>");
    }

    if ((sockfd = socket(AF_INET,SOCK_STREAM | SOCK_CLOEXEC,0)) < 0)
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

    fcntl(sockfd,F_SETFL,fcntl(sockfd,F_GETFL) | O_NONBLOCK);
    while (1)
    {
        fputs(">>",stdout);
        int readn=readline(stdin,buf,MAXLINE);
        writeAgain:
        if ( (n = write(sockfd,buf,(size_t)readn)) < 0 && errno != EAGAIN)
        {
            err_sys("send msg fail");
        }
        else if (n < 0 && errno == EAGAIN)
        {
            printf("wait second \n");
            sleep(1);
            goto writeAgain;
        }

        memset(buf,0,MAXLINE);
        readAgain:
        if( (n = read(sockfd,recvline,MAXLINE)) > 0)
        {
            recvline[n]='\0';
            if (fputs(recvline,stdout) == EOF)
            {
                err_sys("fputs error");
            }
        }

        if (n < 0 && errno == EAGAIN)
        {
            sleep(1);
            goto readAgain;
        }
        else if (n<0 && errno != EAGAIN)
        {
            err_sys("read error");
        }
        memset(recvline,0,MAXLINE+1);
    }

    close(sockfd);
    exit(0);
}
