#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

#define BUF_SIZE 30

void error_handling(char *message)
{
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(1);
}

int main(int argc, char *argv[])
{
    int recv_sock;
    struct sockaddr_in adr;
    int str_len;
    char buf[BUF_SIZE];
    if(argc!=2)
    {
        printf("Usage : %s <port>\n", argv[0]);
        exit(1);
    }

    recv_sock = socket(AF_INET, SOCK_DGRAM, 0);
    memset(&adr,0,sizeof(adr));
    adr.sin_family = AF_INET;
    adr.sin_addr.s_addr = htonl(INADDR_ANY);
    adr.sin_port = htons(atoi(argv[1]));

    if(bind(recv_sock, (struct sockaddr*)&adr, sizeof(adr)) == -1)
    {
        error_handling("bind() error");
    }

    while(1)
    {
        str_len = recvfrom(recv_sock, buf, BUF_SIZE, 0, NULL,0);
        if(str_len == -1)
        {
            break;
        }
        buf[str_len] = 0;
        printf("Received Data : %s\n", buf);
    }

    close(recv_sock);
    return 0;
}