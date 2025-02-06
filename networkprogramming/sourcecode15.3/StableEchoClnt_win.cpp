#include <stdio.h>
#include <stdlib.h>
#include <winsock2.h>

#define BUF_SIZE 1024

void ErrorHandler(char* msg)
{
    fputs(msg,stderr);
    fputs("\n",stderr);
    exit(1);
}

int main(int argc, char* argv[])
{
    WSADATA wsaData;
    SOCKET hSocket;
    SOCKADDR_IN addrSrv;
    char message[BUF_SIZE];
    int strLen,readLen;

    if(argc!=3){
        printf("Usage: %s <server IP> <server port>\n", argv[0]);
        return 1;
    }
    if(WSAStartup(MAKEWORD(2,2),&wsaData)!=0){
        ErrorHandler("WSAStartup failed");
    }
    hSocket= socket(PF_INET,SOCK_STREAM,0);
    if(hSocket==INVALID_SOCKET){
        ErrorHandler("socket failed");
    }
    memset(&addrSrv,0,sizeof(addrSrv));
    addrSrv.sin_family=AF_INET;
    addrSrv.sin_port=htons(atoi(argv[2]));
    addrSrv.sin_addr.s_addr=inet_addr(argv[1]);
    if(connect(hSocket,(SOCKADDR*)&addrSrv,sizeof(addrSrv))==SOCKET_ERROR){
        ErrorHandler("connect failed");
    }
    else{
        puts("Connected to server");
    }
    while(1){
        fputs("Input message:",stdout);
        fgets(message,BUF_SIZE,stdin);
        if(!strcmp(message,"q\n")||!strcmp(message,"Q\n"))
            break;
        strLen = strlen(message);
        send(hSocket,message,strLen,0);
        readLen =0;
        while(1)
        {
            readLen+=recv(hSocket,&message[readLen],BUF_SIZE-1,0);
            printf("readLen=%d\n",readLen);
            if(readLen>=strLen) break;
        }
        message[readLen]='\0';
        printf("Received message: %s\n",message);
    }
    closesocket(hSocket);
    WSACleanup();
    return 0;

}