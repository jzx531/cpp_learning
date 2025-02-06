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
    SOCKET sock;
    WSADATA wsaData;
    SOCKADDR_IN servaddr;
    char message[BUF_SIZE];
    char recv_buf[BUF_SIZE];
    int str_len;

    if (argc!= 3) {
        printf("Usage : %s <server IP> <port>\n", argv[0]);
        return 1;
    }
    if(WSAStartup(MAKEWORD(2, 2), &wsaData)!= 0) {
        ErrorHandling("WSAStartup() error");
    }

    sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock == INVALID_SOCKET) {
        ErrorHandling("socket() error");
    }

    memset(&servaddr,0,sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = inet_addr(argv[1]);
    servaddr.sin_port = htons(atoi(argv[2]));
    connect(sock, (SOCKADDR*)&servaddr, sizeof(servaddr));
    while(1){
        fputs("iNSERT MESSAGE : ", stdout);
        fgets(message, sizeof(message), stdin);
        if(!strcmp(message, "q\n")||!strcmp(message, "Q\n")) break;
        send(sock, message, strlen(message), 0);
        str_len = recv(sock, recv_buf, sizeof(recv_buf)-1, 0);
        recv_buf[str_len] = '\0';
        printf("MESSAGE FROM SERVER : %s\n", recv_buf);
    }
    closesocket(sock);
    WSACleanup();
    return 0;
}



