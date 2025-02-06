#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#define BUF_SIZ 30
void ErrorHandling(char *message){
    fputs(message,stderr);
    fputc('\n',stderr);
    exit(1);
}

int main(int argc, char *argv[])
{
    int sock;
    struct sockaddr_in recv_adr;
    if(argc!= 3){
        printf("Usage : %s <IP> <Port>\n",argv[0]);
        exit(1);
    }
    sock = socket(PF_INET, SOCK_STREAM, 0);
    memset(&recv_adr, 0, sizeof(recv_adr));
    recv_adr.sin_family = AF_INET;
    recv_adr.sin_addr.s_addr = inet_addr(argv[1]);
    recv_adr.sin_port = htons(atoi(argv[2]));
    if(connect(sock, (struct sockaddr*)&recv_adr, sizeof(recv_adr)) == -1){
        ErrorHandling("connect() error");
    }
    char send_data[BUF_SIZ];
    write(sock,"123",strlen("123"));
    send(sock, "4", strlen("4"), MSG_OOB);
    write(sock,"567",strlen("567"));
    printf("urgent message to send : ");
    
    send(sock, "8", strlen("8"), MSG_OOB);
    
    close(sock);
    return 0;

}
