#include <winsock2.h>
#include <stdlib.h>
#include <stdio.h>
void ErrorHandling(char *message)
{
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(1);
}

int main(int argc, char *argv[])
{
    WSADATA wsaData;
    SOCKET hSocket;
    SOCKADDR_IN servAddr;
    char message[1024];
    int strlen;
    if(argc!= 3)
    {
        printf("Usage : %s <server IP> <port>\n", argv[0]);
        return 1;
    }
    if(WSAStartup(MAKEWORD(2, 2), &wsaData)!= 0)
    {
        ErrorHandling("WSAStartup() error");
    }
    hSocket = socket(AF_INET, SOCK_STREAM, 0);
    if(hSocket == INVALID_SOCKET)
    {
        ErrorHandling("socket() error");
    }
    memset(&servAddr, 0, sizeof(servAddr));
    servAddr.sin_family = AF_INET;
    servAddr.sin_addr.s_addr = inet_addr(argv[1]);
    servAddr.sin_port = htons(atoi(argv[2]));
    if(connect(hSocket, (SOCKADDR*)&servAddr, sizeof(servAddr)) == SOCKET_ERROR)
    {
        ErrorHandling("connect() error");
    }
    strlen = recv(hSocket, message, sizeof(message)-1, 0);
    if(strlen ==-1) ErrorHandling("recv() error");
    printf("Received message : %s\n", message);
    closesocket(hSocket);
    WSACleanup();
    return 0;
}