#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

#define BUF_SIZE 30
void ErrorHandling(char *message)
{
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(1);
}

int main(int argc, char *argv[])
{
    int sock;
    char msg1[]="Hello, client";
    char msg2[]="I am UDP host2";
    char msg3[]="Goodbye, client";

    struct sockaddr_in your_addr;
    if(argc!=3){
        printf("Usage : %s <IP> <port>\n", argv[0]);
        exit(1);
    }
    
    sock=socket(PF_INET, SOCK_DGRAM, 0);
    if(sock==-1){
        ErrorHandling("socket() error");
    }
    memset(&your_addr, 0, sizeof(your_addr));
    your_addr.sin_family=AF_INET;   
    your_addr.sin_addr.s_addr=inet_addr(argv[1]);
    your_addr.sin_port=htons(atoi(argv[2]));

    sendto(sock, msg1, strlen(msg1), 0, (struct sockaddr*)&your_addr, sizeof(your_addr));
    sendto(sock, msg2, strlen(msg2), 0, (struct sockaddr*)&your_addr, sizeof(your_addr));
    sendto(sock, msg3, strlen(msg3), 0, (struct sockaddr*)&your_addr, sizeof(your_addr));
    close(sock);
    return 0;
}

