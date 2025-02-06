#include <stdio.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>

#define BUF_SIZE 1024
void ErrorHandling(char *message)
{
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(1);
}

int main(int argc,char* argv[])
{
    WSADATA wsaData;
    SOCKET hServSock, hClntSock;
    char message[BUF_SIZE];
    int strlen,i;
    SOCKADDR_IN servAddr, clntAddr;
    int clntAddrLen;
    if(argc!= 2)
    {
        printf("Usage : %s <port>\n", argv[0]);
        exit(1);
    }
    if(WSAStartup(MAKEWORD(2,2), &wsaData)!= 0)
    {
        ErrorHandling("WSAStartup() error!");
    }
    hServSock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if(hServSock == INVALID_SOCKET)
    {
        ErrorHandling("socket() error!");
    }
    memset(&servAddr, 0, sizeof(servAddr));
    servAddr.sin_family = AF_INET;
    servAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servAddr.sin_port = htons(atoi(argv[1]));
    if(bind(hServSock, (SOCKADDR*)&servAddr, sizeof(servAddr)) == SOCKET_ERROR)
    {
        ErrorHandling("bind() error!");
    }
    if(listen(hServSock, 5) == SOCKET_ERROR)
    {
        ErrorHandling("listen() error!");
    }
    clntAddrLen = sizeof(clntAddr);
    for (i = 0; i < 5; i++)
    {
        hClntSock = accept(hServSock, (SOCKADDR*)&clntAddr, &clntAddrLen);
        if(hClntSock == INVALID_SOCKET)
        {
            ErrorHandling("accept() error!");
        }
        else
        {
            printf("Connected client IP : %s\n", inet_ntoa(clntAddr.sin_addr));
        }
        while(strlen=recv(hClntSock, message, BUF_SIZE, 0)!= 0)
        {
            send(hClntSock, message, strlen, 0);
        }
        closesocket(hClntSock);
    }
    closesocket(hServSock);
    WSACleanup();
    return 0;
}