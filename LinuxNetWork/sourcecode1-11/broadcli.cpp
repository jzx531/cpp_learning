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
#include <sys/ioctl.h>
#include <net/if.h>


#define IP_FOUND "IP_FOUND"           /* IP发现命令 */
#define IP_FOUND_ACK "IP_FOUND_ACK"   /* IP应答命令 */
#define IFNAME "eth0"
#define MCAST_PORT 12345              /* 请根据需要定义合适的端口 */

void IPFound(void* arg) {
    #define BUFFER_LEN 32
    int ret = -1;
    int sock = -1;
    int so_broadcast = 1;
    struct ifreq ifr;
    struct sockaddr_in broadcast_addr; /* 本地地址 */
    struct sockaddr_in from_addr;       /* 源地址 */
    int from_len = sizeof(from_addr);   // 初始化from_len
    int count = -1;
    fd_set readfd;
    char buff[BUFFER_LEN];
    struct timeval timeout;
    timeout.tv_sec = 2;   /* 超时时间2s */
    timeout.tv_usec = 0;

    sock = socket(AF_INET, SOCK_DGRAM, 0); /* 创建数据报套接字 */
    if (sock < 0) {
        printf("HandleIPFound: socket init error\n");
        return;
    }

    /* 将需要使用的网络接口字符串名称复制到结构体中 */
    strcpy(ifr.ifr_name, IFNAME);
    /* 请求命令，获取网络接口的广播地址 */
    if (ioctl(sock, SIOCGIFBRDADDR, &ifr) == -1) {
        perror("ioctl error");
        close(sock); // 关闭套接字
        exit(1);
    }

    /* 将获取的广播地址复制到broadcast_addr */
    memcpy(&broadcast_addr, &ifr.ifr_broadaddr, sizeof(struct sockaddr_in));
    broadcast_addr.sin_port = htons(MCAST_PORT); /* 设置端口号 */

    /* 设置套接字选项，使sock可以进行广播操作 */
    ret = setsockopt(sock, SOL_SOCKET, SO_BROADCAST, &so_broadcast, sizeof(so_broadcast));
    if (ret < 0) {
        perror("setsockopt error");
        close(sock); // 关闭套接字
        return;
    }

    /* 主处理过程 */
    int times = 10;
    for (int i = 0; i < times; i++) {
        /* 广播发送服务器地址请求 */
        ret = sendto(sock, IP_FOUND, strlen(IP_FOUND), 0,
                     (struct sockaddr*)&broadcast_addr, sizeof(broadcast_addr));
        if (ret == -1) {
            continue;
        }

        /* 文件描述符集合清零 */
        FD_ZERO(&readfd);
        /* 将套接字文件描述符加入读集合 */
        FD_SET(sock, &readfd);

        /* select 监听是否有数据到来 */
        ret = select(sock + 1, &readfd, NULL, NULL, &timeout);
        switch (ret) {
            case -1: /* 发生错误 */
                perror("select error");
                break;
            case 0:  /* 超时 */
                // 超时所要执行的代码
                break;
            default: /* 有数据到来 */
                if (FD_ISSET(sock, &readfd)) {
                    /* 接收数据 */
                    count = recvfrom(sock, buff, BUFFER_LEN, 0,
                                     (struct sockaddr*)&from_addr, (socklen_t*)&from_len);
                    printf("Recv msg is %s\n", buff);
                    if (strstr(buff, IP_FOUND_ACK)) {
                        printf("found server, IP is %s\n", inet_ntoa(from_addr.sin_addr));
                    }
                    break; /* 成功接收到服务端地址，退出 */
                }
        }
    }
    
    close(sock); // 确保在结束时关闭套接字
}

int main() {
    IPFound(NULL); // 传递 NULL，因为当前实现没有使用 arg
    return 0;
}
