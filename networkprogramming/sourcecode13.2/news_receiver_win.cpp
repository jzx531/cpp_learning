#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>
#include <ws2tcpip.h>

#define BUF_SIZ 30
void ErrorHandling(char *message)
{
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(1);
}

int main(int argc, char *argv[])
{
    WSADATA wsaData;
    SOCKET hRecvSock;
    SOCKADDR_IN adr;
    struct ip_mreq join_Adr;
    char buf[BUF_SIZ];
    int strlen;

    if(argc!=3){
        printf("USAGE: %s <GROUP IP> <port>\n", argv[0]);
        exit(1);
    }

    if(WSAStartup(MAKEWORD(2,2), &wsaData)!=0){
        ErrorHandling("WSAStartup() error");
    }

    hRecvSock = socket(PF_INET, SOCK_DGRAM, 0);
    memset(&adr, 0, sizeof(adr));
    adr.sin_family = AF_INET;
    adr.sin_addr.s_addr = htonl(INADDR_ANY);
    adr.sin_port = htons(atoi(argv[2]));
    if(bind(hRecvSock, (SOCKADDR*)&adr, sizeof(adr)) == SOCKET_ERROR){
        ErrorHandling("bind() error");
    }
    join_Adr.imr_multiaddr.s_addr = inet_addr(argv[1]);
    join_Adr.imr_interface.s_addr = htonl(INADDR_ANY);
    if(setsockopt(hRecvSock, IPPROTO_IP, IP_ADD_MEMBERSHIP, (char*)&join_Adr, sizeof(join_Adr)) == SOCKET_ERROR){
        ErrorHandling("setsockopt() error");
    }

    while(1){
        memset(buf, 0, BUF_SIZ);
        strlen = recvfrom(hRecvSock, buf, BUF_SIZ, 0,NULL, 0);
        if(strlen <0){
            break;
        }
        printf("Received message: %s\n", buf);
    }

    closesocket(hRecvSock);
    WSACleanup();
    return 0;
}