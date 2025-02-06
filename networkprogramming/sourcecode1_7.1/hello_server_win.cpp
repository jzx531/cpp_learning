#include <winsock2.h>
#include <stdlib.h>
#include <stdio.h>
void ErrorHandling(char *message)
{
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(1);
}

int main(int argc, char* argv[])
{
    WSADATA wsaData;
    SOCKET hServSock, hClntSock;
    SOCKADDR_IN servAddr, clntAddr;
    int szClntAddr;
    char message[] = "Hello Client";
    if(argc!=2)
    {
        printf("Usage : %s <port>\n", argv[0]);
        exit (1);
    }
    if(WSAStartup(MAKEWORD(2,2), &wsaData)!=0)
    {
        ErrorHandling("WSAStartup() error");
    }
    hServSock = socket(AF_INET, SOCK_STREAM, 0);
    if(hServSock == INVALID_SOCKET)
    {
        ErrorHandling("socket() error");
    }
    memset(&servAddr, 0, sizeof(servAddr));
    servAddr.sin_family = AF_INET;
    servAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servAddr.sin_port = htons(atoi(argv[1]));
    if(bind(hServSock, (SOCKADDR*)&servAddr, sizeof(servAddr)) == SOCKET_ERROR)
    {
        ErrorHandling("bind() error");
    }
    if(listen(hServSock, 5) == SOCKET_ERROR)
    {
        ErrorHandling("listen() error");
    }
    szClntAddr = sizeof(clntAddr);
    hClntSock = accept(hServSock, (SOCKADDR*)&clntAddr, &szClntAddr);
    if(hClntSock == INVALID_SOCKET)
    {
        ErrorHandling("accept() error");
    }
    send(hClntSock, message, strlen(message), 0);
    closesocket(hClntSock);
    closesocket(hServSock);
    WSACleanup();
    return 0;
}