#include <stdio.h>
#include <winsock2.h>
#include <string.h>
#include <stdlib.h>

#define BUF_SIZE 1024
#define RLT_SIZE 4
#define OPSZ 4

void ErrorHandling(char *message)
{
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(1);
}

int calculate(int opnum,int opnds[],char op)
{
    int result = opnds[0],i;
    switch(op)
    {
        case '+':
            for(i=1;i<opnum;i++)
                result += opnds[i];
            break;
        case '-':
            for(i=1;i<opnum;i++)
                result -= opnds[i];
            break;
        case '*':
            for(i=1;i<opnum;i++)
                result *= opnds[i];
            break;
    }
    return result;
}

int main(int argc, char* argv[])
{
    WSADATA wsaData;
    SOCKET hServSock, hClntSock;
    char opinfo[BUF_SIZE];
    for(int i=0;i<BUF_SIZE;i++)
        opinfo[i] = 0;
    int result,opndCnt,i;
    int recvCnt,recvLen;
    SOCKADDR_IN servAddr, clntAddr;
    int clntAdrSize;
    if(argc!= 2)
    {
        printf("Usage : %s <port>\n", argv[0]);
        return 1;
    }
    if(WSAStartup(MAKEWORD(2,2), &wsaData)!= 0)
    {
        ErrorHandling("WSAStartup() error");
    }
    hServSock = socket(AF_INET, SOCK_STREAM, 0);
    if(hServSock == INVALID_SOCKET)
    {
        ErrorHandling("socket() error");
    }
    memset(&servAddr, 0, sizeof(servAddr));
    servAddr.sin_family = AF_INET;
    servAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servAddr.sin_port = htons(atoi(argv[1]));
    if(bind(hServSock, (SOCKADDR*)&servAddr, sizeof(servAddr)) == SOCKET_ERROR)
    {
        ErrorHandling("bind() error");
    }
    if(listen(hServSock, 5) == SOCKET_ERROR)
    {
        ErrorHandling("listen() error");
    }
    clntAdrSize = sizeof(clntAddr);
    for(i=0;i<5;i++)
    {
        opndCnt = 0;
        hClntSock = accept(hServSock, (SOCKADDR*)&clntAddr, &clntAdrSize);
        if(hClntSock == INVALID_SOCKET)
        {
            ErrorHandling("accept() error");
        }
        recv(hClntSock,(char*)&opndCnt,1,0);
        recvLen=0;
        // opndCnt = ntohl(opndCnt);
        printf("opndCnt : %d\n",opndCnt);
        int idx = 0;
        while((opndCnt*OPSZ+1)>recvLen)
        {
            recvCnt = recv(hClntSock,&opinfo[recvLen],OPSZ,0);
            recvLen += recvCnt;
            printf("recvCnt : %d\n",recvCnt);
            printf("opinfo : %d\n",*(int*)&opinfo[(idx++)*OPSZ]);
        }
        for (i = 0; i < opndCnt; i++)
        {
            printf("opnd : %d\n",(*(int*)&opinfo[i * OPSZ]));
        }
        printf("recvLen : %d\n",recvLen);
        result = calculate(opndCnt,(int*)opinfo,opinfo[recvLen-1]);
        printf("op : %c\n",opinfo[recvLen-1]);
        printf("Result : %d\n",result);
        send(hClntSock,(char*)&result,RLT_SIZE,0);
        closesocket(hClntSock);
    }
    closesocket(hServSock);
    WSACleanup();
    return 0;
}