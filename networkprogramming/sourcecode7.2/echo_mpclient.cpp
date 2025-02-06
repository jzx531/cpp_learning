#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define BUF_SIZE 30
int error_handling(char *message)
{
    fputs(message, stderr);
    fputc('\n', stderr);
    return -1;
}

void read_routine(int sock,char* buf)
{
    while(1)
    {
        int str_len = recv(sock,buf,BUF_SIZE,0);
        if(str_len == 0) return;
        // str_len = strlen(buf);
        buf[str_len] = '\0';
        printf("Received message: %s\n",buf);
    }
}

void write_routine(int sock,char* buf)
{
    while(1)
    {
        fgets(buf,BUF_SIZE,stdin);
        if(!strcmp(buf,"q\n")||!strcmp(buf,"Q\n"))
        {
            shutdown(sock,SHUT_RDWR);
            return;
        }
        write(sock,buf,strlen(buf));
    }
}

int main(int argc, char *argv[])
{
    int sock;
    pid_t pid;
    char buf[BUF_SIZE];
    struct sockaddr_in serv_adr;
    if(argc != 3)
    {
        printf("Usage : %s <IP> <port>\n",argv[0]);
        exit(1);
    }

    sock = socket(PF_INET,SOCK_STREAM,0);
    memset(&serv_adr,0,sizeof(serv_adr));
    serv_adr.sin_family = AF_INET;
    serv_adr.sin_addr.s_addr = inet_addr(argv[1]);
    serv_adr.sin_port = htons(atoi(argv[2]));

    if(connect(sock,(struct sockaddr*)&serv_adr,sizeof(serv_adr)) == -1)
    {
        error_handling("connect() error");
    }

    pid = fork();
    if(pid == -1)
    {
        error_handling("fork() error");
    }
    else if(pid == 0)
    {
        // child process
        write_routine(sock,buf);
    }
    else
    {
        // parent process
        read_routine(sock,buf);
    }

    close(sock);
    return 0;
}