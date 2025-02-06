#include <stdio.h>
#include <stdlib.h>
#include <winsock2.h>

#define BUF_SIZE 1024

void CALLBACK ReadCompRoutine(DWORD,DWORD,LPWSAOVERLAPPED,DWORD);
void CALLBACK WriteCompRoutine(DWORD,DWORD,LPWSAOVERLAPPED,DWORD);
void ErrorHandler(char* msg)
{
    fputs(msg,stderr);
    fputs("\n",stderr);
    exit(1);
}

typedef struct {
    SOCKET hClntSock;
    char buf[BUF_SIZE];
    WSABUF wsaBuf;
}PER_IO_DATA,*LPPER_IO_DATA;

int main(int argc, char* argv[])
{
    WSADATA wsaData;
    SOCKET hLisnSock,hRecvSock;
    SOCKADDR_IN lisnAdr,recvAdr;
    LPWSAOVERLAPPED lpOvLp;
    DWORD recvBytes;
    LPPER_IO_DATA hbInfo;
    DWORD mode = 1,flagInfo=0;
    int recvAdrsz;
    if(argc!=2){
        printf("Usage: %s <port>\n",argv[0]);
        exit(1);
    }

    if(WSAStartup(MAKEWORD(2,2),&wsaData)!=0){
        ErrorHandler("WSAStartup() error");
    }

    hLisnSock = WSASocket(PF_INET,SOCK_STREAM,0,NULL,0,WSA_FLAG_OVERLAPPED);
    ioctlsocket(hLisnSock,FIONBIO,&mode);

    memset(&lisnAdr,0,sizeof(lisnAdr));
    lisnAdr.sin_family = AF_INET;
    lisnAdr.sin_addr.s_addr = htonl(INADDR_ANY);
    lisnAdr.sin_port = htons(atoi(argv[1]));

    if(bind(hLisnSock,(SOCKADDR*)&lisnAdr,sizeof(lisnAdr))!=0){
        ErrorHandler("bind() error");
    }

    if(listen(hLisnSock,5)!=0){
        ErrorHandler("listen() error");
    }
    recvAdrsz = sizeof(recvAdr);
    while(1){
        SleepEx(100,TRUE);
        hRecvSock = accept(hLisnSock,(SOCKADDR*)&recvAdr,&recvAdrsz);
        if(hRecvSock==INVALID_SOCKET){
            if(WSAGetLastError()==WSAEWOULDBLOCK)
            {
                continue;
            }
            else{
                ErrorHandler("accept() error");
            }
        }
        puts("New connection accepted");
        lpOvLp = (LPWSAOVERLAPPED)malloc(sizeof(WSAOVERLAPPED));
        memset(lpOvLp,0,sizeof(WSAOVERLAPPED));

        hbInfo = (LPPER_IO_DATA)malloc(sizeof(PER_IO_DATA));
        hbInfo->hClntSock = (DWORD)hRecvSock;
        hbInfo->wsaBuf.buf = hbInfo->buf;
        hbInfo->wsaBuf.len = BUF_SIZE;

        lpOvLp->hEvent = (HANDLE)hbInfo;
        printf("hRecvSock = %d\n",hRecvSock);
        // 设置接收套接字为非阻塞模式
        WSARecv(hRecvSock, &(hbInfo->wsaBuf), 1, &recvBytes, &flagInfo, lpOvLp, ReadCompRoutine);

    }
    closesocket(hLisnSock);
    closesocket(hRecvSock);
    WSACleanup();
    return 0;
}

void CALLBACK ReadCompRoutine(DWORD dwErr,DWORD szRecvBytes,LPWSAOVERLAPPED lpOverlpapped,DWORD dwFlags)
{
    LPPER_IO_DATA hbInfo = (LPPER_IO_DATA)(lpOverlpapped->hEvent);
    SOCKET hRecvSock = (SOCKET)(hbInfo->hClntSock);
    LPWSABUF bufInfo = &(hbInfo->wsaBuf);
    DWORD sentBytes;
    printf("recvBytes = %d\n",szRecvBytes);

    if(szRecvBytes == 0){
        closesocket(hRecvSock);
        free(hbInfo);
        free(lpOverlpapped);
        puts("client disconnected...");
    }
    else{
        bufInfo->len = szRecvBytes;
        printf("recvBytes = %d\n",szRecvBytes);
        WSASend(hRecvSock,bufInfo,1,&sentBytes,0,lpOverlpapped,WriteCompRoutine);
    }
}

void CALLBACK WriteCompRoutine(DWORD dwErr,DWORD szSentBytes,LPWSAOVERLAPPED lpOverlapped,DWORD dwFlags)
{
    LPPER_IO_DATA hbInfo = (LPPER_IO_DATA)(lpOverlapped->hEvent);
    SOCKET hRecvSock = (SOCKET)hbInfo->hClntSock;
    LPWSABUF bufInfo = &hbInfo->wsaBuf;
    DWORD recvBytes;
    DWORD flagInfo =0;
    printf("recvBytes = %d\n",szSentBytes);
    WSARecv(hRecvSock,bufInfo,1,&recvBytes,&flagInfo,lpOverlapped,ReadCompRoutine);
}

