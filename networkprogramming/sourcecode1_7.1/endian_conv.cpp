#include <stdio.h>
#include <arpa/inet.h>

int main(){
    unsigned short hsot_port=0x1234;
    unsigned short net_port;
    unsigned long host_addr=0x12345678;
    unsigned long net_addr;

    net_port=htons(hsot_port);
    net_addr=htonl(host_addr);
    printf("Host port: %#x\n",hsot_port);
    printf("Network port: %#x\n",net_port);
    printf("Host address: %#lx\n",host_addr);
    printf("Network address: %#lx\n",net_addr);

    return 0;
}