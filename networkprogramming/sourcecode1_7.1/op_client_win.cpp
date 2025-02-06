#include <stdio.h>
#include <winsock2.h>
#include <string.h>
#include <stdlib.h>

#define BUF_SIZE 1024
#define RLT_SIZE 4
#define OPSZ 4

void ErrorHandling(char *message)
{
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(1);
}

int main(int argc, char* argv[])
{
    WSADATA wsaData;
    SOCKET hSock;
    SOCKADDR_IN servAddr;

    char opmsg[BUF_SIZE];
    int result,opndCnt,i;

    if(argc!= 3)
    {
        printf("Usage : %s <server IP> <port>\n", argv[0]);
        exit(1);
    }
    if(WSAStartup(MAKEWORD(2,2),&wsaData)!= 0)
    {
        ErrorHandling("WSAStartup() error");
    }
    hSock = socket(AF_INET, SOCK_STREAM, 0);
    if(hSock == INVALID_SOCKET)
    {
        ErrorHandling("socket() error");
    }
    memset(&servAddr, 0, sizeof(servAddr));
    servAddr.sin_family = AF_INET;
    servAddr.sin_addr.s_addr = inet_addr(argv[1]);
    servAddr.sin_port = htons(atoi(argv[2]));
    if(connect(hSock, (SOCKADDR*)&servAddr, sizeof(servAddr)) == SOCKET_ERROR)
    {
        ErrorHandling("connect() error");
    }
    else {
        printf("Connected to server.\n");
    }
    fputs("operand count: ", stdout);
    scanf("%d", &opndCnt);
    opmsg[0] = opndCnt;
    for(i=0; i<opndCnt; i++)
    {
        printf("operand %d: ", i);
        scanf("%d", (int*)&opmsg[i*OPSZ+1]);
        printf("%d ", *(int*)&opmsg[i*OPSZ+1]);
    }
    fgetc(stdin);
    fputs("operator:", stdout);
    scanf("%c", &opmsg[opndCnt*OPSZ+1]);
    send(hSock, opmsg, opndCnt*OPSZ+2, 0);
    recv(hSock, (char*)&result, RLT_SIZE, 0);

    printf("Result: %d\n", result);
    closesocket(hSock);
    WSACleanup();
    return 0;
}