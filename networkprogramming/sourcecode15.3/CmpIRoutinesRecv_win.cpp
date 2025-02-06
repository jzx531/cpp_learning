#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>

#define BUF_SIZE 1024
void CALLBACK CompRoutine(DWORD,DWORD,LPWSAOVERLAPPED,DWORD);
void ErrorHandling(char *message);

WSABUF dataBuf;
char buf[BUF_SIZE];
DWORD recvBytes=0;

int main(int argc,char* argv[])
{
    WSADATA wsaData;
    SOCKET hLisnSock,hRecvSock;
    SOCKADDR_IN lisnAdr,recvAdr;

    WSAOVERLAPPED overlapped;
    WSAEVENT evObj;

    int idx;
    int recvAdrSz;
    DWORD flags=0;
    if(argc!= 2){
        printf("Usage: %s <port>\n",argv[0]);
        return 1;
    }
    if(WSAStartup(MAKEWORD(2,2),&wsaData)!= 0){
        ErrorHandling("WSAStartup() error");
    }
    hLisnSock=WSASocket(PF_INET,SOCK_STREAM,0,NULL,0,WSA_FLAG_OVERLAPPED);
    memset(&lisnAdr,0,sizeof(lisnAdr));
    lisnAdr.sin_family=AF_INET;
    lisnAdr.sin_addr.s_addr=htonl(INADDR_ANY);
    lisnAdr.sin_port=htons(atoi(argv[1]));

    if(bind(hLisnSock,(SOCKADDR*)&lisnAdr,sizeof(lisnAdr))!= 0){
        ErrorHandling("bind() error");
    }
    if(listen(hLisnSock,5)!= 0){
        ErrorHandling("listen() error");
    }
    
    recvAdrSz=sizeof(recvAdr);
    hRecvSock=accept(hLisnSock,(SOCKADDR*)&recvAdr,&recvAdrSz);
    if(hRecvSock == INVALID_SOCKET){
        ErrorHandling("accept() error");
    }

    memset(&overlapped,0,sizeof(overlapped));
    dataBuf.len=BUF_SIZE;
    dataBuf.buf = buf;
    evObj=WSACreateEvent();
    if(WSARecv(hRecvSock,&dataBuf,1,&recvBytes,&flags,&overlapped,CompRoutine)==SOCKET_ERROR)
    {
        if(WSAGetLastError()!=WSA_IO_PENDING)
        {
            puts("background data receive");
        }
    }
    idx = WSAWaitForMultipleEvents(1,&evObj,FALSE,INFINITE,TRUE);
    if(idx==WAIT_IO_COMPLETION){
        puts("background data receive completed");
    }
    else{
        ErrorHandling("WSAWaitForMultipleEvents() error");
    }
    WSACloseEvent(evObj);
    closesocket(hRecvSock);
    closesocket(hLisnSock);
    WSACleanup();
    return 0;
}

void CALLBACK CompRoutine(DWORD dwErr,DWORD cbTransferred,LPWSAOVERLAPPED lpOverlapped,DWORD dwFlags)
{
    if(dwErr!=0)
    {
        ErrorHandling("background data receive");
    }
    else{
        recvBytes=cbTransferred;
        printf("Received data: %s\n",buf);
    }
}

void ErrorHandling(char *message)
{
    fputs(message,stderr);
    fputc('\n',stderr);
    exit(1);
}