#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

int main()
{
    char * addr = "192.168.1.1";
    struct in_addr in_addr_struct;
    if (inet_aton(addr, &in_addr_struct) == 0) {
        printf("Invalid IP address\n");
        return 1;
    }
    printf("IP address: %s\n", inet_ntoa(in_addr_struct));
    struct sockaddr_in sock_addr;
    inet_aton(addr,&sock_addr.sin_addr);
    printf("IP address: %s\n", inet_ntoa(sock_addr.sin_addr));
    char * addr2 = "192.168.1.2";
    sock_addr.sin_addr.s_addr = inet_addr(addr2);
    printf("IP address: %s\n", inet_ntoa(sock_addr.sin_addr));
    return 0;
}