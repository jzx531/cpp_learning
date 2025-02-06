#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/epoll.h>
#include <errno.h>
#include <fcntl.h>
#define MAX_CLNT 256
#define BUF_SIZE 100
int clnt_cnt = 0;
int clnt_socks[MAX_CLNT];
pthread_mutex_t mutex;

void error_handling(const char *message)
{
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(1);
}


void send_msg(char *msg, int len)
{
    int i;
    pthread_mutex_lock(&mutex);
    for(i=0; i<clnt_cnt; i++)
    {
        write(clnt_socks[i], msg, len);
    }
    pthread_mutex_unlock(&mutex);
}

void * handle_clnt(void *arg)
{
    int clnt_sock = *(int*)arg;
    int str_len=0,i;
    char msg[BUF_SIZE];
    while ((str_len=read(clnt_sock, msg, sizeof(msg)))!=0)
    {
        send_msg(msg, str_len);
    }
    pthread_mutex_lock(&mutex);
    for(i=0; i<clnt_cnt; i++)//处理断开连接的client
    {
        if(clnt_socks[i]==clnt_sock)
        {
            while(i++<clnt_cnt-1)
            {
                clnt_socks[i]=clnt_socks[i+1];
            }
            break;
        }
    }
    clnt_cnt--;
    pthread_mutex_unlock(&mutex);
    close(clnt_sock);
    pthread_exit(NULL);
}


int main(int argc, char *argv[])
{
    int serv_sock,clnt_sock;
    struct sockaddr_in serv_addr, clnt_addr;
    socklen_t clnt_addr_size;
    pthread_t tid;
    if(argc!=2){
        printf("Usage : %s <port>\n", argv[0]);
        exit(1);
    
    }
    pthread_mutex_init(&mutex, NULL);
    serv_sock=socket(PF_INET, SOCK_STREAM, 0);
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family=AF_INET;
    serv_addr.sin_addr.s_addr=htonl(INADDR_ANY);
    serv_addr.sin_port=htons(atoi(argv[1]));
    if(bind(serv_sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr))==-1){
        error_handling("bind() error");
    }
    if(listen(serv_sock, 5)==-1){
        error_handling("listen() error");
    }
    while(1){
        clnt_addr_size=sizeof(clnt_addr);
        clnt_sock=accept(serv_sock, (struct sockaddr*)&clnt_addr, &clnt_addr_size);
        if(clnt_sock==-1){
            error_handling("accept() error");
        }
        pthread_mutex_lock(&mutex);
        if(clnt_cnt>=MAX_CLNT){
            printf("Too many clients\n");
            close(clnt_sock);
        }
        else{
            clnt_socks[clnt_cnt]=clnt_sock;
            clnt_cnt++;
            pthread_create(&tid, NULL, handle_clnt, &clnt_sock);
        }
        pthread_mutex_unlock(&mutex);
        pthread_detach(tid);
        printf("Connected client IP:%s\n", inet_ntoa(clnt_addr.sin_addr));
    }
}     
