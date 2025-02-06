#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BUF_SIZ 1024
void ErrorHandling(char *message){
    fputs(message,stderr);
    fputc('\n',stderr);
    exit(1);
}

int main(int argc, char* argv[]){
    WSADATA wsaData;
    SOCKET serv_sock, clnt_sock;
    SOCKADDR_IN serv_addr, clnt_addr;
    TIMEVAL timeout;
    fd_set reads,cpyReads;

    int adrSz;
    int strlen,fdnum,i;
    char buf[BUF_SIZ];

    if(argc!= 2){
        printf("Usage : %s <port>\n",argv[0]);
        exit(1);
    }
    //Initialise Winsock
    if(WSAStartup(MAKEWORD(2,2),&wsaData)!= 0){
        ErrorHandling("WSAStartup() error");
    }

    //Create socket
    serv_sock = socket(PF_INET,SOCK_STREAM,0);
    if(serv_sock == INVALID_SOCKET){
        ErrorHandling("socket() error");
    }

    //Prepare server address
    memset(&serv_addr,0,sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(atoi(argv[1]));

    //Bind socket
    if(bind(serv_sock,(SOCKADDR*)&serv_addr,sizeof(serv_addr)) == SOCKET_ERROR){
        ErrorHandling("bind() error");
    }

    //Listen socket
    if(listen(serv_sock,5) == SOCKET_ERROR){
        ErrorHandling("listen() error");
    }

    FD_ZERO(&reads);
    FD_SET(serv_sock,&reads);

    while(1){
        cpyReads = reads;
        timeout.tv_sec = 5;
        timeout.tv_usec = 0;
        if((fdnum = select(0,&cpyReads,NULL,NULL,&timeout)) == SOCKET_ERROR){
            break;
        }
        if(fdnum == 0){
            printf("timeout\n");
            continue;
        }

        for(i=0;i<reads.fd_count;i++)
        {
            if(FD_ISSET(reads.fd_array[i],&cpyReads))
            {
                if(reads.fd_array[i] == serv_sock)
                {
                    adrSz = sizeof(clnt_addr);
                    clnt_sock = accept(serv_sock,(SOCKADDR*)&clnt_addr,&adrSz);
                    if(clnt_sock == INVALID_SOCKET){
                        ErrorHandling("accept() error");
                    }
                    FD_SET(clnt_sock,&reads);
                    printf("Connected client from %s:%d\n",inet_ntoa(clnt_addr.sin_addr),ntohs(clnt_addr.sin_port));
                }
                else
                {
                    memset(buf,0,BUF_SIZ);
                    if((strlen = recv(reads.fd_array[i],buf,BUF_SIZ,0)) == SOCKET_ERROR){
                        ErrorHandling("recv() error");
                    }
                    if(strlen == 0){
                        printf("Client disconnected\n");
                        closesocket(reads.fd_array[i]);
                        FD_CLR(reads.fd_array[i],&reads);
                    }
                    else{
                        send(reads.fd_array[i],buf,strlen,0);
                    }
                }
            }
        }
    }
        closesocket(serv_sock);
        WSACleanup();
        return 0;
}