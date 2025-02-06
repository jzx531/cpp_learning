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
#define MCAST_DATA "BROADCAST MESSAGE FROM MULTICAST"
#define MCAST_INTERVAL 5

int main(int argc,char *argv[])
{
    int s;
    struct sockaddr_in mcast_addr;
    s = socket(AF_INET, SOCK_DGRAM, 0);
    if(s == -1){
        perror("socket");
        exit(1);
    }
    memset(&mcast_addr,0,sizeof(mcast_addr));
    mcast_addr.sin_family = AF_INET;
    mcast_addr.sin_port = htons(MCAST_PORT);
    mcast_addr.sin_addr.s_addr = inet_addr(MCAST_ADDR);
    while(1){
        int  n = sendto(s, MCAST_DATA, sizeof(MCAST_DATA), 0, (struct sockaddr*)&mcast_addr, sizeof(mcast_addr));
        if(n<0){
            perror("sendto");
            exit(2);
        }
    }
}