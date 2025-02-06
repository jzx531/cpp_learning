#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/epoll.h>
#include <errno.h>
#include <fcntl.h>

#define BUF_SIZ 4
#define EPOLL_SIZE 50
void error_handling(const char *message)
{
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(1);
}

void setnonblockingmode(int fd)
{
    int flag =fcntl(fd, F_GETFL, 0);
    fcntl(fd,F_SETFL, flag | O_NONBLOCK);
}

int main(int argc, char *argv[])
{
    int serv_sock,clnt_sock;
    struct sockaddr_in serv_addr,clnt_addr;
    socklen_t adr_size;
    int str_len,i;
    char buf[BUF_SIZ];

    struct epoll_event *ep_events;
    struct epoll_event event;
    int epfd,event_cnt;
    if(argc!= 2) {
        printf("Usage : %s <port>\n", argv[0]);
        exit(1);
    }
    serv_sock = socket(PF_INET, SOCK_STREAM, 0);
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(atoi(argv[1]));
    if(bind(serv_sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) == -1) {
        error_handling("bind() error");
    }
    if(listen(serv_sock, 5) == -1) {
        error_handling("listen() error");
    }
    epfd = epoll_create(EPOLL_SIZE);
    ep_events = (struct epoll_event*)malloc(EPOLL_SIZE * sizeof(struct epoll_event));
    event.data.fd = serv_sock;
    event.events = EPOLLIN;
    epoll_ctl(epfd, EPOLL_CTL_ADD, serv_sock, &event);
    while(1) {
        event_cnt = epoll_wait(epfd, ep_events, EPOLL_SIZE, -1);
        if(event_cnt == -1)
        {
            puts("epoll_wait error");
            break;
        }
        puts("return epoll_wait");
        for(i =0;i<event_cnt;i++)
        {
            if(ep_events[i].data.fd==serv_sock)
            {
                adr_size=sizeof(clnt_addr);
                clnt_sock = accept(serv_sock, (struct sockaddr*)&clnt_addr, &adr_size);
                setnonblockingmode(clnt_sock);
                event.data.fd = clnt_sock;
                event.events = EPOLLIN | EPOLLET;
                epoll_ctl(epfd, EPOLL_CTL_ADD, clnt_sock, &event);
                printf("Connected client : %d\n", clnt_sock);
            }
            else 
            {
                while(1)
                {
                    str_len = read(ep_events[i].data.fd,buf,BUF_SIZ);
                    if(str_len == 0)
                    {
                        printf("Client %d disconnected\n",ep_events[i].data.fd);
                        epoll_ctl(epfd,EPOLL_CTL_DEL,ep_events[i].data.fd,NULL);
                        close(ep_events[i].data.fd);
                        break;
                    }
                    else if(str_len <0)
                    {
                        if(errno == EAGAIN)
                        {
                            break;
                        }
                    }
                    else{
                        printf("Received message from client %d : %s\n",ep_events[i].data.fd,buf);
                        write(ep_events[i].data.fd,buf,str_len);
                    }
                }
            }
        }
    }

}