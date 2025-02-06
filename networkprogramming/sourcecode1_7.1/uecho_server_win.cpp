#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <winsock2.h>

void ErrorHandling(char *message)
{
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(1);
}
#define BUF_SIZE 30

int main(int argc, char* argv[])
{
    WSADATA wsaData;
    SOCKET servSock;
    char message[BUF_SIZE];
    int strlen;
    int clntAdrSZ;

    SOCKADDR_IN servAdr,clntAdr;
    if(argc!= 2)
    {
        printf("Usage : %s <port>\n", argv[0]);
        exit(1);
    }

    if(WSAStartup(MAKEWORD(2,2), &wsaData)!= 0)
    {
        ErrorHandling("WSAStartup() error");
    }
    servSock = socket(PF_INET, SOCK_DGRAM, 0);
    if( servSock== INVALID_SOCKET)
    {
        ErrorHandling("socket() error");
    }

    memset(&servAdr,0,sizeof(servAdr));
    servAdr.sin_family = AF_INET;
    servAdr.sin_addr.s_addr = htonl(INADDR_ANY);
    servAdr.sin_port = htons(atoi(argv[1]));

    if(bind(servSock, (SOCKADDR*)&servAdr, sizeof(servAdr)) == SOCKET_ERROR)
    {
        ErrorHandling("bind() error");
    }
    while(1)
    {
        clntAdrSZ = sizeof(clntAdr);
        strlen = recvfrom(servSock, message, BUF_SIZE, 0, (SOCKADDR*)&clntAdr, &clntAdrSZ);
        printf("Message : %s\n",message);
        sendto(servSock, message, strlen, 0, (SOCKADDR*)&clntAdr, clntAdrSZ);
    }
    closesocket(servSock);
    WSACleanup();
    return 0;
}
