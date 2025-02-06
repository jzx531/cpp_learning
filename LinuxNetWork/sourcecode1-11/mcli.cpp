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


#define MCAST_PORT 8888
#define MCAST_ADDR "224.0.0.88"
#define MCAST_INTERVAL 5
#define BUFF_SIZE 256

int main(int argc,char *argv[])
{
    int s;
    struct sockaddr_in local_addr;
    int err =-1;
    s = socket(AF_INET,SOCK_DGRAM,0);
    if(s == -1){
        perror("socket()");
        return -1;
    }

    memset(&local_addr,0,sizeof(local_addr));
    local_addr.sin_family = AF_INET;
    local_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    local_addr.sin_port=htons(MCAST_PORT);

    err = bind(s,(struct sockaddr*)&local_addr,sizeof(local_addr));

    if(err == -1){  
        perror("bind");
        exit(2);
    }

    int loop =1;
    err = setsockopt(s,IPPROTO_IP,IP_MULTICAST_LOOP, &loop,sizeof(loop));
    if(err == -1){
        perror("socketopt error");
        exit(3);
    }

    struct ip_mreq mreq;                /* 加入组播 */
    mreq.imr_multiaddr.s_addr = inet_addr(MCAST_ADDR); /* 多播地址 */
    mreq.imr_interface.s_addr = htonl(INADDR_ANY);   /* 网络接口为任意 */

    err = setsockopt(s, IPPROTO_IP, IP_ADD_MEMBERSHIP, &mreq, sizeof(mreq));
    if (err < 0) {
        perror("setsockopt(): IP_ADD_MEMBERSHIP");
        return -4;
    }

    int times = 0;
    int addr_len = 0;
    char buff[BUFF_SIZE];
    int n = 0;

    for (times = 0; times < 5; times++) { /* 循环接收广播的信息，5 次后退出 */
        addr_len = sizeof(local_addr);    /* 清空接收缓冲区 */
        memset(buff, 0, BUFF_SIZE);       /* 清空接收缓冲区 */

        n = recvfrom(s, buff, BUFF_SIZE, 0, (struct sockaddr*)&local_addr, (socklen_t*)&addr_len);
        if (n == -1) {
            perror("recvfrom()");
        }
        
        printf("Recv %dst message from server: %s\n", times, buff);
        sleep(MCAST_INTERVAL);
    }

    err = setsockopt(s, IPPROTO_IP, IP_DROP_MEMBERSHIP, &mreq, sizeof(mreq)); /* 退出组播 */

    close(s);
    return 0;


}
