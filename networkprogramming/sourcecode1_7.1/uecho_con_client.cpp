#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

void ErrorHandling(char *message)
{
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(1);
}

#define BUF_SIZE 30

int main(int argc,char *argv[])
{
    int sock;
    char message[BUF_SIZE];
    int str_len;
    socklen_t adr_sz;

    struct sockaddr_in serv_adr;
    if(argc!= 3)
    {
        printf("Usage : %s <IP> <Port>\n", argv[0]);
        exit(1);
    }

    sock = socket(PF_INET,SOCK_DGRAM,0);
    if(sock == -1)
    {
        ErrorHandling("socket() error");
    }
    memset(&serv_adr,0,sizeof(serv_adr));
    serv_adr.sin_family = AF_INET;
    serv_adr.sin_addr.s_addr = inet_addr(argv[1]);
    serv_adr.sin_port = htons(atoi(argv[2]));

    connect(sock,(struct sockaddr*)&serv_adr,sizeof(serv_adr));
    while(1){
        fputs("Insert message to send : ",stdout);
        fgets(message,BUF_SIZE,stdin);
        if(!strcmp(message,"q\n")||!strcmp(message,"Q\n"))break;
        write(sock,message,strlen(message));
        str_len = read(sock,message,BUF_SIZE);
        message[str_len] = 0;
        printf("Received message : %s\n",message);
    }
    close(sock);
    return 0;
}