#include <stdio.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <string.h>
#include <process.h>

#define BUF_SIZE 100
#define NAME_CLNT 20

void ErrorHandling(char *msg)
{
    fputs(msg, stderr);
    fputs("\n", stderr);
    exit(1);
}

unsigned int WINAPI SendMsg(void *arg);
unsigned int WINAPI RecvMsg(void *arg);

char name[NAME_CLNT];
char msg[BUF_SIZE];

int main(int argc, char *argv[])
{
    WSADATA wsaData;
    SOCKET sock;
    SOCKADDR_IN servAdr;
    HANDLE hSndThread, hRcvThread;

    if (argc != 4)
    {
        printf("Usage : %s <server IP> <server port> <client name>\n", argv[0]);
        exit(1);
    }

    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
    {
        ErrorHandling("WSAStartup() error");
    }

    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == INVALID_SOCKET)
    {
        ErrorHandling("socket() error");
    }

    memset(&servAdr, 0, sizeof(servAdr));
    servAdr.sin_family = AF_INET;
    servAdr.sin_addr.s_addr = inet_addr(argv[1]);
    servAdr.sin_port = htons(atoi(argv[2]));

    if (connect(sock, (SOCKADDR *)&servAdr, sizeof(servAdr)) == SOCKET_ERROR)
    {
        ErrorHandling("connect() error");
    }

    // 初始化客户端名称
    strncpy(name, argv[3], NAME_CLNT - 1);
    name[NAME_CLNT - 1] = '\0';

    hSndThread = (HANDLE)_beginthreadex(NULL, 0, SendMsg, (void *)sock, 0, NULL);
    hRcvThread = (HANDLE)_beginthreadex(NULL, 0, RecvMsg, (void *)sock, 0, NULL);

    WaitForSingleObject(hSndThread, INFINITE);
    WaitForSingleObject(hRcvThread, INFINITE);

    CloseHandle(hSndThread);
    CloseHandle(hRcvThread);
    closesocket(sock);
    WSACleanup();

    return 0;
}

unsigned int WINAPI SendMsg(void *arg)
{
    SOCKET sock = (SOCKET)arg;
    char sendbuf[BUF_SIZE + NAME_CLNT];
    while (1)
    {
        fgets(msg, BUF_SIZE, stdin);
        if (!strcmp(msg, "q\n") || !strcmp(msg, "Q\n"))
        {
            closesocket(sock);
            return 1; // 退出线程
        }
        sprintf(sendbuf, "%s:%s", name, msg);
        send(sock, sendbuf, strlen(sendbuf), 0);
    }
    return 0;
}

unsigned int WINAPI RecvMsg(void *arg)
{
    SOCKET sock = (SOCKET)arg;
    char recvbuf[BUF_SIZE + NAME_CLNT];
    while (1)
    {
        int len = recv(sock, recvbuf, BUF_SIZE + NAME_CLNT, 0);
        if (len == SOCKET_ERROR || len == 0)
        {
            printf("Server closed the connection or error occurred.\n");
            closesocket(sock);
            return 1; // 退出线程
        }
        recvbuf[len] = '\0';
        printf("%s", recvbuf);
    }
    return 0;
}