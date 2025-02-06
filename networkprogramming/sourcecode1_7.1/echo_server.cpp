#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#define BUF_SIZE 1024
void ErrorHandling(char *message)
{
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(1);
}

int main(int argc, char *argv[])
{
    int serv_sock,clnt_sock;
    char message[BUF_SIZE];
    int str_len,i;
    struct sockaddr_in serv_addr,clnt_addr;
    socklen_t clnt_addr_size;
    if(argc!= 2)
    {
        printf("Usage : %s <port>\n", argv[0]);
        exit(1);
    }
    serv_sock = socket(PF_INET, SOCK_STREAM, 0);
    if(serv_sock == -1)
        ErrorHandling("socket() error");
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(atoi(argv[1]));
    if(bind(serv_sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) == -1)
        ErrorHandling("bind() error");
    if(listen(serv_sock, 5) == -1)
        ErrorHandling("listen() error");
    clnt_addr_size = sizeof(clnt_addr);
    for(i=0; i<5; i++){
        clnt_sock=accept(serv_sock, (struct sockaddr*)&clnt_addr, &clnt_addr_size);
        if(clnt_sock == -1)
            ErrorHandling("accept() error");
        else{
            printf("Connected client IP : %s, Port : %d\n", inet_ntoa(clnt_addr.sin_addr), ntohs(clnt_addr.sin_port));
        }
        while(str_len= read(clnt_sock, message, BUF_SIZE)!= 0)
        {
            str_len = strlen(message);
            write(clnt_sock,message,str_len);
            printf("string length : %d\n", str_len);
            printf("Message from client : %s\n", message);
        }
        close(clnt_sock);
    }
    close(serv_sock);
    return 0;
}