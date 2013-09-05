#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <time.h>
#include <signal.h>

#define MAXLINE 1000
#define LISTEN_QUEUE 5
#define SA "const sockaddr"

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

int readline(FILE *file,char* buf,size_t len)
{
    char tmp[len+1];
    memset(tmp,0,len+1);
    int i = 0;
    while ( (tmp[i++] = fgetc(file)) != '\n' )
    {
        if (i >= len)
            break;
    }
    tmp[len]='\0';
    memcpy(buf,tmp,i);
    return i;
}

void timeout(int signum)
{
    while(signum-- > 0)
    {
        fputs("pool your shit\n",stdout);
    }
}

