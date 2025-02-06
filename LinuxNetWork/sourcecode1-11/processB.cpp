#include <sys/types.h>
#include <sys/socket.h>
#include <linux/un.h>
#include <string.h>
#include <signal.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/uio.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <pthread.h>

ssize_t recv_fd(int fd,void *data ,size_t bytes,int *(recvfd))
{
    struct msghdr msghdr_recv; 
    struct iovec iov[1];
    size_t n;
    int newfd;
    union{
        struct cmsghdr cm;
        char control[CMSG_SPACE(sizeof(int))];
    }control_un;

    struct cmsghdr *pcmsghdr;
    msghdr_recv.msg_control = control_un.control;
    msghdr_recv.msg_controllen = sizeof(control_un.control);
    
    msghdr_recv.msg_name = NULL;
    msghdr_recv.msg_namelen = 0;

    iov[0].iov_base = data;
    iov[0].iov_len = bytes;
    msghdr_recv.msg_iov = iov;
    msghdr_recv.msg_iovlen = 1;

    if((n=recvmsg(fd,&msghdr_recv,0))<=0)
        return n;
    if((pcmsghdr=CMSG_FIRSTHDR(&msghdr_recv))!=NULL && pcmsghdr->cmsg_len == CMSG_LEN(sizeof(int)))
    {
        if(pcmsghdr->cmsg_level != SOL_SOCKET)
            printf("Error: level is not SOL_SOCKET\n");
        if(pcmsghdr->cmsg_type != SCM_RIGHTS)
            printf("Error: type is not SCM_RIGHTS\n");
        *recvfd =*((int*)CMSG_DATA(pcmsghdr));
    }
    else{
        *recvfd = -1;
    }
    return n;
}

int my_open(const char *pathname, int mode) {
    int fd, sockfd[2], status;
    pid_t childpid;
    char c, argsockfd[10], argmode[10];

    socketpair(AF_LOCAL, SOCK_STREAM, 0, sockfd); /* 创建socket */
    if ((childpid = fork()) == 0) { /* 子进程 */
        close(sockfd[0]); /* 关闭sockfd[0] */
        snprintf(argsockfd, sizeof(argsockfd), "%d", sockfd[1]); /* socket描述符 */
        snprintf(argmode, sizeof(argmode), "%d", mode); /* 打开文件的方式 */
        
        execl("./openfile", (char *)"openfile", argsockfd, pathname, argmode, (char *)NULL); /* 执行进程A */
        printf("execv error\n");
    }

    /* 父进程 */
    close(sockfd[1]); /* 关闭sockfd[1] */
    /* 等待进程结束 */
    waitpid(childpid, &status, 0);

    if (WIFEXITED(status) == 0) { /* 判断子进程是否结束 */
        printf("child did not terminate\n");
    }
    if ((status = WEXITSTATUS(status)) == 0) { /* 子进程结束 */
        recv_fd(sockfd[0], &c, 1, &fd) ;

    } else {
        errno = status;
        fd = -1;
    }
    

    close(sockfd[0]); /* 关闭sockfd[0] */
    return fd; /* 返回进程A打开文件的描述符 */
}

#define BUFFSIZE 256
int main(int argc ,char *argv[])
{
    int fd,n;
    char buff[BUFFSIZE];
    if(argc!=2) printf("Usage: %s filename\n", argv[0]);
    if((fd=my_open(argv[1], O_RDONLY))<0)printf("Error: %s\n", strerror(errno));
    while((n=read(fd,buff,BUFFSIZE))>0)
        write(1,buff,n);
    close(fd);
    return 0;
}