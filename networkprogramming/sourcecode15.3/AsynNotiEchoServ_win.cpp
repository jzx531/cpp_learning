#include <stdio.h>
#include <string.h>
#include <winsock2.h>
#include <ws2tcpip.h>

#define BUF_SIZE 100

void CompressSockets(SOCKET hSockArr[], int idx,int total);
void CompressEvents(WSAEVENT hEventArr[], int idx, int total);
void ErrorHandling(char * msg);

int main(int argc, char* argv[])
{
    WSADATA wsaData;
    SOCKET hServSock,hClntSock;
    SOCKADDR_IN servAddr, clntAddr;
    
    SOCKET hSockArr[WSA_MAXIMUM_WAIT_EVENTS];
    WSAEVENT hEventArr[WSA_MAXIMUM_WAIT_EVENTS];
    WSAEVENT newEvent;
    WSANETWORKEVENTS networkEvents;
    int numOfClntSock = 0;
    int strLen,i;
    int postInfo,startIdx;
    int clntAdrLen;
    char msg[BUF_SIZE];

    if(argc!=2){
        printf("Usage : %s <port>\n",argv[0]);
        exit(1);
    }
    if(WSAStartup(MAKEWORD(2,2),&wsaData)!=0){
        ErrorHandling("WSAStartup() error!");
    }
    hServSock=socket(PF_INET,SOCK_STREAM,0);
    memset(&servAddr,0,sizeof(servAddr));
    servAddr.sin_family=AF_INET;
    servAddr.sin_addr.s_addr=htonl(INADDR_ANY);
    servAddr.sin_port=htons(atoi(argv[1]));
    if(bind(hServSock,(SOCKADDR*)&servAddr,sizeof(servAddr))==SOCKET_ERROR){
        ErrorHandling("bind() error!");
    }
    if(listen(hServSock,5)==SOCKET_ERROR){
        ErrorHandling("listen() error!");
    }
    newEvent = WSACreateEvent();
    if(WSAEventSelect(hServSock,newEvent,FD_ACCEPT)==SOCKET_ERROR){
        ErrorHandling("WSAEventSelect() error!");
    }
    hEventArr[numOfClntSock]=newEvent;
    hSockArr[numOfClntSock]=hServSock;
    numOfClntSock++;

    while(1){
        postInfo = WSAWaitForMultipleEvents(numOfClntSock,hEventArr,FALSE,WSA_INFINITE,FALSE);
        startIdx = postInfo - WSA_WAIT_EVENT_0;
        for(i=startIdx;i<numOfClntSock;i++)
        {
            int sigEventIdx = WSAWaitForMultipleEvents(1,&hEventArr[i],TRUE,0,FALSE);
            if(sigEventIdx==WSA_WAIT_TIMEOUT||sigEventIdx==WSA_WAIT_FAILED){
                continue;
            }
            else{
                sigEventIdx = i;
                WSAEnumNetworkEvents(hSockArr[sigEventIdx],hEventArr[sigEventIdx],&networkEvents);
                if(networkEvents.lNetworkEvents&FD_ACCEPT)//请求连接时
                {
                    if(networkEvents.iErrorCode[FD_ACCEPT_BIT]!=0){
                        printf("FD_ACCEPT error!\n");
                        break;
                    }
                    clntAdrLen = sizeof(clntAddr);
                    hClntSock = accept(hSockArr[sigEventIdx],(SOCKADDR*)&clntAddr,&clntAdrLen);
                    newEvent = WSACreateEvent();
                    WSAEventSelect(hClntSock,newEvent,FD_READ|FD_CLOSE);

                    hEventArr[numOfClntSock]=newEvent;
                    hSockArr[numOfClntSock]=hClntSock;
                    numOfClntSock++;
                    printf("New client connected from %s:%d\n",inet_ntoa(clntAddr.sin_addr),ntohs(clntAddr.sin_port));
                }
                if(networkEvents.lNetworkEvents&FD_READ)//数据到达时
                {
                    if(networkEvents.iErrorCode[FD_READ_BIT]!=0)
                    {
                        puts("FD_READ error!");
                        break;
                    }
                    strLen=recv(hSockArr[sigEventIdx],msg,BUF_SIZE,0);
                    send(hSockArr[sigEventIdx],msg,strLen,0);
                    
                }
                if(networkEvents.lNetworkEvents&FD_CLOSE)//连接断开时
                {
                    if(networkEvents.iErrorCode[FD_CLOSE_BIT]!=0)
                    {
                        puts("FD_CLOSE error!");
                        break;
                    }
                    printf("Client from %s:%d disconnected\n",inet_ntoa(clntAddr.sin_addr),ntohs(clntAddr.sin_port));
                    closesocket(hSockArr[sigEventIdx]);
                    WSACloseEvent(hEventArr[sigEventIdx]);
                    numOfClntSock--;
                    CompressSockets(hSockArr,sigEventIdx,numOfClntSock);
                    CompressEvents(hEventArr,sigEventIdx,numOfClntSock);
                }
            }
        }

    }
    WSACleanup();
    return 0;
}

void CompressSockets(SOCKET hSockArr[], int idx,int total)
{
    int i;
    for(i=idx;i<total;i++){
        hSockArr[i]=hSockArr[i+1];
    }
}

void CompressEvents(WSAEVENT hEventArr[], int idx, int total)
{
    int i;
    for(i=idx;i<total;i++){
        hEventArr[i]=hEventArr[i+1];
    }
}

void ErrorHandling(char * msg)
{
    fputs(msg,stderr);
    fputc('\n',stderr);
    exit(1);
}
