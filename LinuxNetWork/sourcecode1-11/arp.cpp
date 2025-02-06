#include <sys/socket.h>
#include <sys/ioctl.h>   /* ioctl 命令 */
#include <linux/if_ether.h> /* ether 样式 */
#include <net/if.h>     /* ifreq 结构 */
#include <netinet/in.h> /* in addr 结构 */
#include <arpa/inet.h>  /* inet_addr 函数 */
#include <linux/ip.h>   /* iphdr 结构 */
#include <linux/udp.h>  /* udphdr 结构 */
#include <linux/tcp.h>  /* tcphdr 结构 */
#include <cstring>
#include <unistd.h>     /* sleep 函数 */
#include <stdlib.h>
#include <stdio.h>
#include <netpacket/packet.h>

struct arppacket {
    unsigned short ar_hrd;  /* 硬件类型 */
    unsigned short ar_pro;  /* 协议类型 */
    unsigned char  ar_hln;  /* 硬件地址长度 */
    unsigned char  ar_pln;  /* 协议地址长度 */
    unsigned short ar_op;           /* ARP 操作码 */
    unsigned char  ar_sha[ETH_ALEN]; /* 发送方 MAC 地址 */
    unsigned char  ar_sip[4];       /* 发送方 IP 地址 */
    unsigned char  ar_tha[ETH_ALEN]; /* 目的 MAC 地址 */
    unsigned char  ar_tip[4];       /* 目的 IP 地址 */
};

int main(int argc, char *argv[]) {
    char ef[1500];         /* 以太帧缓冲区 */
    struct ethhdr *p_ethhdr;        /* 以太网头部指针 */
    unsigned char eth_dest[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF}; /* 目的以太网地址 */
    unsigned char eth_source[] = {0x00, 0x0C, 0x29, 0x73, 0x9D, 0x15}; /* 源以太网地址 */
    int n;
    int fd;

    fd = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL)); /* 使用 AF_PACKET 和 SOCK_RAW */
    // fd=socket(AF_INET, SOCK_PACKET, htons(0x0003));

    if (fd < 0) {
        perror("socket");
        return 1;
    }

    struct ifreq if_idx;
    memset(&if_idx, 0, sizeof(struct ifreq));
    strncpy(if_idx.ifr_name, "eth0", IFNAMSIZ-1); // 设置你的网络接口名
    if (ioctl(fd, SIOCGIFINDEX, &if_idx) < 0) {
        perror("SIOCGIFINDEX");
        return 1;
    }

    struct sockaddr_ll sll;
    memset(&sll, 0, sizeof(sll));
    sll.sll_family = AF_PACKET; // 设置地址族为 AF_PACKET
    sll.sll_ifindex = if_idx.ifr_ifindex; // 接口索引
    sll.sll_protocol = htons(ETH_P_ARP); // 协议类型
    sll.sll_hatype = 1; // 硬件地址类型，通常设置为1
    sll.sll_pkttype = PACKET_OUTGOING; // 数据包类型
    sll.sll_halen = ETH_ALEN; // 硬件地址长度
    memcpy(sll.sll_addr, eth_source, ETH_ALEN); // 源MAC地址

    if (bind(fd, (struct sockaddr*)&sll, sizeof(sll)) < 0) {
        perror("bind");
        close(fd);
        return 1;
    }

    /* 获取指向以太网头的指针 */
    p_ethhdr = (struct ethhdr *) ef;  
    /* 复制目的以太网地址 */
    memcpy(p_ethhdr->h_dest, eth_dest, ETH_ALEN);  
    memcpy(p_ethhdr->h_source, eth_source, ETH_ALEN); 
    p_ethhdr->h_proto = htons(ETH_P_ARP); /* 协议类型 */

    struct arppacket *p_arp;           /* 定位 ARP 包地址 */
    p_arp = (struct arppacket *) (ef + ETH_HLEN);             /* ARP 结构体 */
    p_arp->ar_hrd = htons(0x1);        /* arp 设备类型 */
    p_arp->ar_pro = htons(0x0800);     /* 协议类型 */
    p_arp->ar_hln = 6;                  /* 硬件地址长度 */
    p_arp->ar_pln = 4;                  /* IP 地址长度 */
    // p_arp->ar_op = htons(ARPOP_REQUEST); /* ARP 请求 */
    p_arp->ar_op = 1;
    /* 复制源以太网地址 */
    memcpy(p_arp->ar_sha, eth_source, ETH_ALEN); 
    /* 填写 IP 地址 */
    in_addr_t sip = inet_addr("172.18.156.93");
    memcpy(p_arp->ar_sip, &sip, 4);
    /* 复制目的以太网地址 */
    memcpy(p_arp->ar_tha, eth_dest, ETH_ALEN); 
    /* 目的 IP 地址 */
    in_addr_t tip = inet_addr("172.18.156.93");
    memcpy(p_arp->ar_tip, &tip, 4);

    /* 发送 ARP 请求 8 次，间隔 1s */
    int i;
    for(i = 0; i < 8; i++) {
        // n = write(fd, ef, sizeof(struct ethhdr) + sizeof(struct arppacket)); /* 发送 */
        n = write(fd, ef, ETH_FRAME_LEN); /* 发送 */
        if (n < 0) {
            perror("write");
            break;
        }
        sleep(1);                          /* 等待 1s */
    }

    close(fd);
    return 0;
}