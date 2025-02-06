#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netdb.h>

int error_handling(char *message)
{
    fputs(message, stderr);
    fputc('\n', stderr);
    return -1;
}

int main(int argc,char *argv[])
{
    int sock;
    int snd_buf,rcv_buf,state;
    socklen_t len;

    sock = socket(PF_INET,SOCK_STREAM,0);
    len =sizeof(snd_buf);
    state = getsockopt(sock,SOL_SOCKET,SO_SNDBUF,(void*)&snd_buf,&len);

    if(state) error_handling("getsockopt() error");
    len = sizeof(rcv_buf);
    state = getsockopt(sock,SOL_SOCKET,SO_RCVBUF,(void*)&rcv_buf,&len);
    if(state) error_handling("getsockopt error");
    printf("Input buffer size: %d\n",rcv_buf);
    printf("output buf size:%d\n",snd_buf);
    return 0;
}