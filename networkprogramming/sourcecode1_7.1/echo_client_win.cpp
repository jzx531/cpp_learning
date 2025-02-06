#include <stdio.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <string.h>
#include <cstring>
#define BUF_SIZE 1024
void ErrorHandling(char *message)
{
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(1);
}

int main(int argc, char* argv[])
{
    WSADATA wsaData;
    SOCKET hsock;
    char message[BUF_SIZE];
    int strlen;
    SOCKADDR_IN servAddr;
    
    if(argc!= 3)
    {
        printf("Usage : %s <server IP> <port>\n", argv[0]);
        exit(1);
    }

    if(WSAStartup(MAKEWORD(2,2), &wsaData)!= 0)
    {
        ErrorHandling("WSAStartup() error");
    }
    hsock = socket(AF_INET, SOCK_STREAM, 0);
    if(hsock == INVALID_SOCKET)
    {
        ErrorHandling("socket() error");
    }

    memset(&servAddr, 0, sizeof(servAddr));
    servAddr.sin_family = AF_INET;
    servAddr.sin_addr.s_addr = inet_addr(argv[1]);
    servAddr.sin_port = htons(atoi(argv[2]));

    if(connect(hsock, (SOCKADDR*)&servAddr, sizeof(servAddr)) == SOCKET_ERROR)
    {
        ErrorHandling("connect() error");
    }
    else {
        printf("Connected to server\n");
    }
    while(1)
    {
        fputs("Enter message to send : ", stdout);
        fgets(message, BUF_SIZE, stdin);
        if(!strcmp(message,"q\n")||!strcmp(message,"Q\n"))
        {break;}
        send(hsock, message, std::strlen(message), 0);
        strlen = recv(hsock, message, BUF_SIZE-1, 0);
        message[strlen] = '\0';
        printf("Received message : %s\n", message);
    }
    closesocket(hsock);
    WSACleanup();
    return 0;
}


