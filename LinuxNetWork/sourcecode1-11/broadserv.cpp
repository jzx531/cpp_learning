#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <signal.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/uio.h>
#include <fcntl.h>

#define IP_FOUND "IP_FOUND"           /* IP发现命令 */
#define IP_FOUND_ACK "IP_FOUND_ACK"   /* IP发现应答命令 */
#define MCAST_PORT 12345              /* 请根据需要定义合适的端口 */
#define BUFFER_LEN 32

void HandleIPFound(void* arg)
{
    int ret = -1;
    int sock = -1;                      // 使用int代替SOCKET
    struct sockaddr_in local_addr;      /* 本地地址 */
    struct sockaddr_in from_addr;       /* 客户端地址 */
    int from_len = sizeof(from_addr);   // 初始化from_len
    int count = -1;
    fd_set readfd;
    char buff[BUFFER_LEN];
    struct timeval timeout;
    timeout.tv_sec = 2;                 /* 超时时间2s */
    timeout.tv_usec = 0;

    printf("==>HandleIPFound\n"); // 使用printf代替DBGPRINT

    sock = socket(AF_INET, SOCK_DGRAM, 0); /* 创建数据报套接字 */
    if (sock < 0) {
        printf("HandleIPFound: socket init error\n");
        return;
    }

    /* 数据清零 */
    memset((void*)&local_addr, 0, sizeof(struct sockaddr_in));

    local_addr.sin_family = AF_INET;             /* 协议内容 */
    local_addr.sin_addr.s_addr = htonl(INADDR_ANY); /* 本地地址 */
    local_addr.sin_port = htons(MCAST_PORT);     /* 端口 */

    /* 绑定 */
    ret = bind(sock, (struct sockaddr*)&local_addr, sizeof(local_addr));
    if (ret != 0) {
        printf("HandleIPFound: bind error\n");
        close(sock); // 确保在错误时关闭套接字
        return;
    }

    /* 主处理过程 */
    while (1)
    {
        /* 文件描述符集合清零 */
        FD_ZERO(&readfd);
        /* 将套接字文件描述符加入读集合 */
        FD_SET(sock, &readfd);
        ret = select(sock + 1, &readfd, NULL, NULL, &timeout);
        
        switch (ret)
        {
            case -1: /* 发生错误 */
                perror("select error"); // 打印错误信息
                break;
            case 0:  /* 超时 */
                // 超时所要执行的代码
                break;
            default: /* 有数据到来 */
                if (FD_ISSET(sock, &readfd))
                {
                    /* 接收数据 */
                    count = recvfrom(sock, buff, BUFFER_LEN, 0,
                                     (struct sockaddr *)&from_addr, (socklen_t*)&from_len);
                    printf("Recv msg is %s\n", buff); // 使用printf代替DBGPRINT
                    if (strstr(buff, IP_FOUND))
                    {
                        /* 判断是否匹配 */
                        /* 将应答数据复制出去 */
                        memcpy(buff, IP_FOUND_ACK, strlen(IP_FOUND_ACK) + 1);
                        /* 发送应答端 */
                        count = sendto(sock, buff, strlen(buff), 0,
                                       (struct sockaddr *)&from_addr, from_len);
                    }
                }
        }
    }

    printf("<=HandleIPFound\n"); // 使用printf代替PRINT
    close(sock); // 关闭套接字
    return;
}

int main(int argc, char* argv[])
{
    HandleIPFound(NULL);
    return 0;
}