

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

    struct sigaction alarm_act,ignore_act;
    alarm_act.sa_flags = SA_RESTART;
    alarm_act.sa_handler = timeout;
    sigemptyset(&alarm_act.sa_mask);
    sigaddset(&alarm_act.sa_mask,SIGQUIT);

    ignore_act.sa_flags = SA_RESTART;
    ignore_act.sa_handler = SIG_IGN;
    sigemptyset(&ignore_act.sa_mask);
    //sigaddset(&ignore_act.sa_mask,SIGQUIT);

    int readn;
    while (1)
    {
        fputs(">>",stdout);
        sigaction(SIGALRM,&alarm_act,NULL);
        alarm(10);
        readn=readline(stdin,buf,MAXLINE);
        sigaction(SIGALRM,&ignore_act,NULL);

        if ( (n = write(sockfd,buf,(size_t)readn)) < 0 )
        {
            err_sys("send msg fail");
        }

        memset(buf,0,MAXLINE);
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
        memset(recvline,0,MAXLINE+1);
    }

    return 0;
}
