#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

#define BUF_SIZE 1024
#define RLT_SIZE 4
#define OPSZ 4

void ErrorHandling(char *message)
{
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(1);
}

int main(int argc, char *argv[])
{
    int sock;
    char opmsg[BUF_SIZE];
    int result,opnd_cut,i;
    struct sockaddr_in serv_addr;
    if(argc!= 3)
    {
        printf("Usage : %s <IP> <port>\n", argv[0]);
        exit(1);
    }

    sock = socket(AF_INET, SOCK_STREAM, 0);
    if(sock == -1)
        ErrorHandling("socket() error");
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr(argv[1]);
    serv_addr.sin_port = htons(atoi(argv[2]));
    if(connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) == -1)
        ErrorHandling("connect() error");
    else
        printf("Connected\n");
    fputs("OPERAND COUNT",stdout);
    scanf("%d",&opnd_cut);
    opmsg[0] = (char)opnd_cut;
    for(i=0;i<opnd_cut;i++){
        printf("OPERAND %d : ",i+1);
        scanf("%d",(int*)&opmsg[i*OPSZ+1]);
    }
    fgetc(stdin);
    fputs("SENDING OPERANDS...",stdout);
    scanf("%c",opmsg+opnd_cut*OPSZ+1);
    write(sock, opmsg, opnd_cut*OPSZ+2);
    read(sock, &result, RLT_SIZE);
    printf("RESULT : %d\n",result);
    close(sock);
    return 0;
}