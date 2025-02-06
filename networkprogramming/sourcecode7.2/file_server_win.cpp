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

int main(int argc,char *argv[])
{
    WSADATA wsaData;
    SOCKET hServSock,hClnSock;
    FILE *fp;
    char buf[BUF_SIZE];
    int readCnt;
    SOCKADDR_IN serAdr,clntAdr;
    int clntAdrSz;

    if(argc!=2){
        printf("Usage : %s <port>\n",argv[0]);
        exit(1);
    }

    if(WSAStartup(MAKEWORD(2,2),&wsaData)!=0){
        ErrorHandling("WSAStartup() error");
    }
    fp =fopen("test.txt","rb");
    hServSock=socket(AF_INET,SOCK_STREAM,0);
    if(hServSock==INVALID_SOCKET){
        ErrorHandling("socket() error");
    }
    memset(&serAdr,0,sizeof(serAdr));
    serAdr.sin_family=AF_INET;
    serAdr.sin_addr.s_addr=htonl(INADDR_ANY);
    serAdr.sin_port=htons(atoi(argv[1]));
    if(bind(hServSock,(SOCKADDR*)&serAdr,sizeof(serAdr))==SOCKET_ERROR){
        ErrorHandling("bind() error");
    }
    if(listen(hServSock,5)==SOCKET_ERROR){
        ErrorHandling("listen() error");
    }
    clntAdrSz=sizeof(clntAdr);
    hClnSock=accept(hServSock,(SOCKADDR*)&clntAdr,&clntAdrSz);
    if(hClnSock==INVALID_SOCKET){
        ErrorHandling("accept() error");
    }
    while(1){
        readCnt=recv(hClnSock,buf,BUF_SIZE,0);
        if(readCnt<BUF_SIZE){
            send(hClnSock,buf,readCnt,0);
            break;
        
        }
        send(hClnSock,(char*)&buf,BUF_SIZE,0);
    }
    shutdown(hClnSock,SD_SEND);
    recv(hClnSock,buf,BUF_SIZE,0);
    printf("Received message from client : %s\n",buf);
    fclose(fp);
    closesocket(hClnSock);
    closesocket(hServSock);
    WSACleanup();
    return 0;
}
