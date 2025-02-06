#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <net/if_arp.h>
#include <string.h>
#include <linux/sockios.h>
#include <linux/if.h>
#include <stdlib.h>
#include <sys/ioctl.h>

int main(int argc, char *argv[])
{
    int s;          /* 套接字描述符 */
    int err = -1;  /* 错误值 */

    /* 建立一个数据报套接字 */
    s = socket(AF_INET, SOCK_DGRAM, 0);
    if (s < 0) {
        printf("socket() 出错\n");
        return -1;
    }
    /* 获取网络接口的名称 */
    
    {
    struct ifreq ifr;
    ifr.ifr_ifindex = 2;  /* 获取第 2 个网络接口的名称 */
    err = ioctl(s, SIOCGIFNAME, &ifr);
    
    if (err) {
        printf("SIOCGIFNAME Error\n");
    } else {
        printf("the %dst interface is: %s\n", ifr.ifr_ifindex, ifr.ifr_name);
    }
    }

    /* 获取网络接口配置信息 */
    {
        struct ifreq ifr;
        /* 查询网卡 "eth0" 的状态 */
        memcpy(ifr.ifr_name, "eth0", 5); 
        /* 获取标志 */
        err = ioctl(s, SIOCGIFFLAGS, &ifr);
        
        if (!err) {
            printf("SIOCGIFFLAGS: %d\n", ifr.ifr_flags);
        }
        /* 获取 METRIC */
        err = ioctl(s, SIOCGIFMETRIC, &ifr);
        if (!err) {
            printf("SIOCGIFMETRIC: %d\n", ifr.ifr_metric);
        }

        /* 获取 MTU */
        err = ioctl(s, SIOCGIFMTU, &ifr);
        if (!err) {
            printf("SIOCGIFMTU: %d\n", ifr.ifr_mtu);
        }
    
    /* 获取 MAC 地址 */
    err = ioctl(s, SIOCGIFHWADDR, &ifr);
    if (!err) {
    unsigned char *hw = (unsigned char *)ifr.ifr_hwaddr.sa_data;
    printf("SIOCGIFHWADDR: %02x:%02x:%02x:%02x:%02x:%02x\n", hw[0], hw[1], hw[2], hw[3], hw[4], hw[5]);
    }
    
    /* 获取网卡映射参数 */
    err = ioctl(s, SIOCGIFMAP, &ifr);
    if (!err) {
        printf("SIOCGIFMAP, mem_start: %d, mem_end: %d, base_addr: %d, dma: %d, port: %d\n",
            ifr.ifr_map.mem_start,  /* 开始地址 */
            ifr.ifr_map.mem_end,    /* 结束地址 */
            ifr.ifr_map.base_addr,   /* 基地址 */
            ifr.ifr_map.irq,        /* 中断 */
            ifr.ifr_map.dma,        /* DMA */
            ifr.ifr_map.port);      /* 端口 */
    }
    /* 获取网卡序列号 */
    err = ioctl(s, SIOCGIFINDEX, &ifr);
    if (!err) {
        printf("SIOCGIFINDEX: %d\n", ifr.ifr_ifindex);
    }
    /* 获取发送队列长度 */
    err = ioctl(s, SIOCGIFTXQLEN, &ifr);
    if (!err) {
        printf("SIOCGIFTXQLEN: %d\n", ifr.ifr_qlen);
    }
    }
    /* 获取网络接口 IP 地址 */
    {
    struct ifreq ifr;

    /*方便操作设置指向 sockaddr_in 的指针*/
    struct sockaddr_in *sin = (struct sockaddr_in *)&ifr.ifr_addr;
    char ip[16];  /* 保存 IP 地址字符串 */
    memset(ip, 0, 16);
    memcpy(ifr.ifr_name, "eth0", 5);  /* 查询 eth0 */

    /* 查询本地 IP 地址 */
    err = ioctl(s, SIOCGIFADDR, &ifr);
    if (!err) {
        inet_ntop(AF_INET, &sin->sin_addr.s_addr, ip, 16);
        printf("SIOCGIFADDR: %s\n", ip);
    }

    /* 查询广播 IP 地址 */
    err = ioctl(s, SIOCGIFBRDADDR, &ifr);
    if (!err) {
        inet_ntop(AF_INET, &sin->sin_addr.s_addr, ip, 16);
        printf("SIOCGIFBRDADDR: %s\n", ip);
    }

    /* 查询远端 IP 地址 */
        err = ioctl(s, SIOCGIFDSTADDR, &ifr);
        if (!err) {
            inet_ntop(AF_INET, &sin->sin_addr.s_addr, ip, 16);
            printf("SIOCGIFDSTADDR: %s\n", ip);
        }
    
    /* 查询网络掩码 */
    err = ioctl(s, SIOCGIFNETMASK, &ifr);
    if (!err) {
        /* 将网络掩码转换为点分十进制的字符串 */
        inet_ntop(AF_INET, &sin->sin_addr.s_addr, ip, 16);
        printf("SIOCGIFNETMASK: %s\n", ip);
    }
    }

    /* 测试设置 IP 地址 */
    {
        struct ifreq ifr;
        struct sockaddr_in *sin = (struct sockaddr_in *)&ifr.ifr_addr;
        char ip[16];  /* 保存 IP 地址字符串 */
        int err = -1;

        /* 将本机 IP 地址设置为 192.168.1.175 */
        printf("Set IP to 192.168.1.175\n");
        memset(&ifr, 0, sizeof(ifr));
        memcpy(ifr.ifr_name, "eth0", 5);  /* eth0 网卡设置 IP 地址 */
        inet_pton(AF_INET, "192.168.1.175", &sin->sin_addr);  /* 将字符串转换为网络字节序的形式 */

        sin->sin_family = AF_INET;  /* 设置地址族 */
        err = ioctl(s, SIOCSIFADDR, &ifr);  /* 设置 IP 地址 */
        if (err) {
            printf("SIOCSIFADDR error\n");
        } else {
            printf("check IP -- \n");
            memset(&ifr, 0, sizeof(ifr));
            memcpy(ifr.ifr_name, "eth0", 5);  /* 再次查询 IP 地址 */
            ioctl(s, SIOCGIFADDR, &ifr);
            inet_ntop(AF_INET, &sin->sin_addr.s_addr, ip, 16);  /* 将 IP 地址转换为字符串 */
            printf("%s\n", ip);  /* 打印 IP 地址 */
        }
    }

}


