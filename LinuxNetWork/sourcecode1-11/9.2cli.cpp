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

// extern void sig_proccess(int signo);
// extern void sig_pipe(int signo);
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


void process_conn_client(int s)
{
    char buffer[30];
    ssize_t size =0;
    struct iovec *v=(struct iovec *)malloc(3*sizeof(struct iovec));
    if(v == NULL){
        printf("Memory allocation failed\n");
        exit(1);
    }
    vc = v;
    v[0].iov_base = buffer;
    v[1].iov_base = buffer+10;
    v[2].iov_base = buffer+20;
    v[0].iov_len = 10;
    v[1].iov_len = 10;
    v[2].iov_len = 10;
 
    int i=0;
    for(;;)
    {
        size = read(0,v[0].iov_base,10);
        if(size >0){
            v[0].iov_len = size;
            writev(s,v,1);
            v[0].iov_len=v[1].iov_len=v[2].iov_len=10;
            size=read(s,v,3);
            for(i=0;i<3;i++)
            {
                if(v[i].iov_len > 0){
                    write(1,v[i].iov_base,v[i].iov_len);
                }
            }
        }
    }
}

void process_conn_client(int s)
{
    char buffer[30];
    ssize_t size =0;
    struct msghdr msg;
    struct iovec *v=(struct iovec *)malloc(3*sizeof(struct iovec));
    if(v == NULL){
        printf("Memory allocation failed\n");
        exit(1);
    }
    vc = v;
    v[0].iov_base = buffer;
    v[1].iov_base = buffer+10;
    v[2].iov_base = buffer+20;
    v[0].iov_len = v[1].iov_len = v[2].iov_len = 10;
        
    msg.msg_name = NULL;
    msg.msg_namelen = 0;
    msg.msg_control =NULL;
    msg.msg_controllen = 0;
    msg.msg_iov = v;
    msg.msg_iovlen = 30;
    msg.msg_flags = 0;
    int i=0;
    for(;;)
    {
        size = read(0,v[0].iov_base,10);
        if(size >0){
            v[0].iov_len = size;
            sendmsg(s,&msg,0);
            v[0].iov_len=v[1].iov_len=v[2].iov_len=10;
            for(i=0;i<3;i++)
            {
                if(v[i].iov_len > 0){
                    write(1,v[i].iov_base,v[i].iov_len);
                }
            }
        }
    }
}

static int s;

void sig_proccess_client(int signo)
{
    printf("Client process terminated\n");
    close(s);
    exit(0);
}

#define PORT 8080

int main(int argc, char *argv[])
{
    struct sockaddr_in server_addr;
    int err;
    if(argc!= 2)
    {
        printf("Usage: %s <server ip address>\n", argv[0]);
        exit(1);
    }
    signal(SIGINT,sig_proccess);
    signal(SIGPIPE,sig_pipe);

    s = socket(AF_INET, SOCK_STREAM, 0);
    if(s <0){
        printf("Socket creation failed\n");
        exit(1);
    
    }
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    err = inet_pton(AF_INET, argv[1], &server_addr.sin_addr);
    connect(s, (struct sockaddr *)&server_addr, sizeof(server_addr));
    process_conn_client(s);
    close(s);
    return 0;
}
