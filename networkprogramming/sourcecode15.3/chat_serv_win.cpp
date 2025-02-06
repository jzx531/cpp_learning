#include <stdio.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <string.h>
#include <process.h>

#define BUF_SIZE 100
#define MAX_CLNT 256

unsigned WINAPI HandleClnt(void* arg);
void SendMsg(char *msg, int len);
void ErrorHandling(char *msg)
{
    fputs(msg, stderr);
    fputs("\n", stderr);
    exit(1);
}

int clntCnt = 0;
SOCKET clntSocks[MAX_CLNT];
HANDLE hMutex;

int main(int argc, char* argv[])
{
    WSADATA wsaData;
    SOCKET servSock, clntSock;
    struct sockaddr_in servAddr, clntAddr;
    int clntAddrSz;
    HANDLE hThread;
    if(argc!= 2)
    {
        printf("Usage : %s <port>\n", argv[0]);
       exit(1);
    }

    if(WSAStartup(MAKEWORD(2,2), &wsaData)!= 0)
    {
        ErrorHandling("WSAStartup() error");
    }

    hMutex = CreateMutex(NULL, FALSE, NULL);
    servSock = socket(PF_INET, SOCK_STREAM, 0);
    memset(&servAddr, 0, sizeof(servAddr));
    servAddr.sin_family = AF_INET;
    servAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servAddr.sin_port = htons(atoi(argv[1]));

    if(bind(servSock, (SOCKADDR*)&servAddr, sizeof(servAddr)) == SOCKET_ERROR)
    {
        ErrorHandling("bind() error");
    }

    if(listen(servSock, 5) == SOCKET_ERROR)
    {
        ErrorHandling("listen() error");
    }

    while(1)
    {
        clntAddrSz = sizeof(clntAddr);
        clntSock = accept(servSock, (SOCKADDR*)&clntAddr, &clntAddrSz);
        if(clntSock == INVALID_SOCKET)
        {
            ErrorHandling("accept() error");
        }
        WaitForSingleObject(hMutex, INFINITE);
        clntSocks[clntCnt] = clntSock;
        clntCnt++;
        ReleaseMutex(hMutex);
        hThread = (HANDLE)_beginthreadex(NULL, 0, HandleClnt, (void*)&clntSock, 0, NULL);
        printf("Client %s connected\n", inet_ntoa(clntAddr.sin_addr));
    }
    closesocket(servSock);
    WSACleanup();
    return 0;
}

unsigned WINAPI HandleClnt(void* arg)
{
    SOCKET clntSock = *(SOCKET*)arg;
    char msg[BUF_SIZE];
    int strLen=0,i;
    while((strLen=recv(clntSock, msg, BUF_SIZE, 0)) > 0)
    {
        SendMsg(msg, strLen);
    }
    WaitForSingleObject(hMutex, INFINITE);
    for(i=0; i<clntCnt; i++)
    {
        if(clntSocks[i] == clntSock)
        {
            while(i++<clntCnt-1)
            {
                clntSocks[i] = clntSocks[i+1];
            }
            break;
        }
    }
    clntCnt--;
    ReleaseMutex(hMutex);
    closesocket(clntSock);
    return 0;
}

void SendMsg(char *msg, int len)
{
    WaitForSingleObject(hMutex, INFINITE);
    for(int i=0; i<clntCnt; i++)
    {
        send(clntSocks[i], msg, len, 0);
    }
    ReleaseMutex(hMutex);
}
