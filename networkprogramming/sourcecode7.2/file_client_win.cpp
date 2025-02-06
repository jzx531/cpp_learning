#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>

#define BUF_SIZE 30
void ErrorHandling(char *message){
    fputs(message,stderr);
    fputc('\n',stderr);
    exit(1);
}

int main(int argc, char* argv[]){
    WSADATA wsaData;
    SOCKET hSocket;
    FILE *fp;
    char szBuf[BUF_SIZE];
    int readCnt;
    SOCKADDR_IN serverAddr;
    if(argc!= 3){
        printf("Usage : %s <server IP> <file name>\n",argv[0]);
        exit(1);
    }
    if(WSAStartup(MAKEWORD(2,2),&wsaData)!= 0){
        ErrorHandling("WSAStartup() error");
    }
    fp = fopen("client.txt","wb");
    hSocket = socket(AF_INET,SOCK_STREAM,0);
    if(hSocket == INVALID_SOCKET){
        ErrorHandling("socket() error");
    }
    memset(&serverAddr,0,sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = inet_addr(argv[1]);
    serverAddr.sin_port = htons(atoi(argv[2]));
    connect(hSocket,(SOCKADDR*)&serverAddr,sizeof(serverAddr));
    while((readCnt = recv(hSocket,szBuf,BUF_SIZE,0))!= 0)
    {
        fwrite(szBuf,1,readCnt,fp);
    }
    puts("File received successfully");
    send(hSocket,"Thank you for connecting",23,0);
    fclose(fp);
    closesocket(hSocket);
    WSACleanup();
    return 0;
}