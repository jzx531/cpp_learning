#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

void ErrorHandling(char *message)
{
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(1);
}

int main(int argc,char *argv[])
{
    int sock;
    struct sockaddr_in serv_addr;
    char message[30];
    int read_len, idx=0;
    int str_len=0;
    if(argc!= 3)
    {
        printf("Usage : %s <IP> <Port>\n", argv[0]);
        return 1;
    }
    sock = socket(PF_INET, SOCK_STREAM, 0);
    if(sock ==-1){
        ErrorHandling("socket() error");
    }
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr(argv[1]);
    serv_addr.sin_port = htons(atoi(argv[2]));
    if(connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) == -1){
        ErrorHandling("connect() error");
    }
    while(read_len=read(sock, &message[idx++],1))
    {
        if(read_len==-1){
            ErrorHandling("read() error");
        }
        str_len+=read_len;
    }
    printf("Received message from server : %s\n", message);
    printf("Message length : %d\n", str_len);
    close(sock);
    return 0;
}