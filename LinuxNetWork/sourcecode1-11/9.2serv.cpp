#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <strings.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
// #include <linux/in.h>
#include <signal.h>
#include <sys/uio.h>
#include <arpa/inet.h>

static struct iovec*vs=NULL,*vc=NULL;

void sig_proccess(int signo)
{
    printf("Process terminated\n");
    free(vs);
    free(vc);
    exit(0);
}
void sig_pipe(int signo)
{
    printf("SIGPIPE received\n");
    free(vs);
    free(vc);
    exit(0);
}

#define PORT 8080
#define BACKLOG 2

void process_conn_server(int s)
{
    char buffer[30];
    ssize_t size =0;
        struct iovec *v=(struct iovec *)malloc(3*sizeof(struct iovec));
    if(v == NULL){
        printf("Memory allocation failed\n");
        exit(1);
    }
    vs = v;
    v[0].iov_base = buffer;
    v[1].iov_base = buffer+10;
    v[2].iov_base = buffer+20;
    v[0].iov_len = 10;
    v[1].iov_len = 10;
    v[2].iov_len = 10;

    for(;;){
        size = readv(s,v,3);
        if(size ==0){
            return ;
        }
        snprintf((char *)v[0].iov_base, 10, "%d", (int)size);
        snprintf((char *)v[1].iov_base, 10, "Hello");
        snprintf((char *)v[2].iov_base, 10, "World\n");

        v[0].iov_len = strlen((const char*)v[0].iov_base);
        v[1].iov_len = strlen((const char*)v[1].iov_base);
        v[2].iov_len = strlen((const char*)v[2].iov_base);

        writev(s,v,3);
    }

}

void process_conn_client_msg(int s)
{
    char buffer[30];
    ssize_t size =0;
    struct iovec *v=(struct iovec *)malloc(3*sizeof(struct iovec));
    if(v == NULL){
        printf("Memory allocation failed\n");
        exit(1);
    }
    vs = v;
    v[0].iov_base = buffer;
    v[1].iov_base = buffer+10;
    v[2].iov_base = buffer+20;
    v[0].iov_len =v[1].iov_len =v[2].iov_len = 10;

    struct msghdr msg;
    msg.msg_name = NULL;
    msg.msg_namelen = 0;
    msg.msg_control =NULL;
    msg.msg_controllen = 0;
    msg.msg_iov = v;
    msg.msg_iovlen = 30;
    msg.msg_flags = 0;

    for(;;){
        size = recvmsg(s,&msg,0);
        if(size ==0){
            return ;
        }
        sprintf((char *)v[0].iov_base, "%d", (int)size);
        sprintf((char *)v[1].iov_base, "Hello");
        sprintf((char *)v[2].iov_base, "World\n");

        v[0].iov_len = strlen((const char*)v[0].iov_base);
        v[1].iov_len = strlen((const char*)v[1].iov_base);
        v[2].iov_len = strlen((const char*)v[2].iov_base);

        sendmsg(s,&msg,0);
    }
}

int main()
{
    int ss,sc;
    struct sockaddr_in server_addr,client_addr;
    int err;
    pid_t pid;
    signal(SIGPIPE,sig_pipe);
    signal(SIGINT,sig_proccess);
    ss = socket(AF_INET,SOCK_STREAM,0);
    if(ss == -1){
        printf("Socket creation failed\n");
        exit(1);
    }
    bzero(&server_addr,sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(PORT);
    err = bind(ss,(struct sockaddr*)&server_addr,sizeof(server_addr));
    if(err == -1){
        printf("Socket binding failed\n");
        exit(1);
    }
    err = listen(ss,BACKLOG);
    if(err == -1){
        printf("Listen failed\n");
        exit(1);
    }
    for(;;){
        socklen_t len = sizeof(client_addr);
        sc = accept(ss,(struct sockaddr*)&client_addr,&len);
        if(sc <0){
            continue;
        }
        pid = fork();
        if(pid == 0){
            close(ss);
            process_conn_server(sc);
        }
        else{
            close(sc);
        }
    }
    return 0;
}