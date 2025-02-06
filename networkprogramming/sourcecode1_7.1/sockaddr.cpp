#define _CRT_SECURE_NO_WARNINGS
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>
#include <cstring>
#include <winsock2.h> 
// #pragma comment (lib, "ws2_32.lib") //加载 ws2_32.dll
int main()
{
    sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(80);
    addr.sin_addr.S_un.S_addr = inet_addr("192.168.1.100");

    // 使用 inet_ntoa
    char* address = inet_ntoa(addr.sin_addr);
    printf("Address: %s\n", address);

    // 或者使用 inet_ntop，这是一个更安全的选择
    // char addres[INET_ADDRSTRLEN];
    // inet_ntop(AF_INET, &(addr.sin_addr), addres, INET_ADDRSTRLEN);
    // printf("Address: %s\n", addres);

    return 0;
}
