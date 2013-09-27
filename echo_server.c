
#include "myc.h"
#include <glib.h>

int main(int argc,char* argv[])
{
    int listen_fd;
    if ((listen_fd = socket(AF_INET,SOCK_STREAM,0)) < 0)
        err_sys("create socket");

    struct sockaddr_in servaddr;
    memset(&servaddr,0,sizeof(servaddr));
    servaddr.sin_family=AF_INET;
    servaddr.sin_port=htons(3789);
    if( inet_pton(AF_INET,"127.0.0.1",&servaddr.sin_addr) <= 0)
        err_quit ("inet_pton");

    //bind & listen
    if (bind(listen_fd,(struct sockaddr *)&servaddr,sizeof(servaddr)) < 0)
        err_sys("bind");
    if (listen(listen_fd,LISTEN_QUEUE) < 0)
        err_sys("listen");
    setfdnonblock(listen_fd);

    return 0;
}
