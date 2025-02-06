#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>

void error_handling(char *message)
{
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(1);
}
#define BUF_SIZE 30

int main(int argc, char *argv[])
{
    int acpt_sock, recv_sock;
    struct sockaddr_in acpt_addr, recv_addr;
    int str_len,state;
    socklen_t recv_addr_len;
    char buf[BUF_SIZE];
    if(argc!= 2){
        printf("Usage : %s <port>\n", argv[0]);
        exit(1);
    }
    acpt_sock = socket(PF_INET, SOCK_STREAM, 0);
    memset(&acpt_addr,0,sizeof(acpt_addr));
    acpt_addr.sin_family = AF_INET;
    acpt_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    acpt_addr.sin_port = htons(atoi(argv[1]));

    if(bind(acpt_sock, (struct sockaddr*)&acpt_addr, sizeof(acpt_addr)) == -1){
        error_handling("bind() error");
    }
    if(listen(acpt_sock, 5) == -1){
        error_handling("listen() error");
    }
    recv_addr_len = sizeof(recv_addr);
    recv_sock = accept(acpt_sock, (struct sockaddr*)&recv_addr, &recv_addr_len);
    if(recv_sock == -1){
        error_handling("accept() error");
    }
    while(1){
        str_len = recv(recv_sock, buf, sizeof(buf)-1, MSG_PEEK|MSG_DONTWAIT);
        if(str_len>0)
            break;
    }
    buf[str_len] = 0;
    printf("Received message : %s\n", buf);
    
    str_len=recv(recv_sock, buf, sizeof(buf)-1, 0);
    buf[str_len] = 0;
    printf("Received message : %s\n", buf);
    close(recv_sock);
    close(acpt_sock);
    return 0;
}