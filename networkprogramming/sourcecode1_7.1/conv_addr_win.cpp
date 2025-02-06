#undef UNICODE
#undef _UNICODE
#include <winsock2.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char* argv[])
{
    char * strAddr = "192.168.1.1";
    char strAddrBuf[56];
    SOCKADDR_IN servaddr;
    int sizeserv;
    DWORD size;

    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);
    sizeserv = sizeof(servaddr);
    WSAStringToAddress(strAddr, AF_INET, NULL, (SOCKADDR*)&servaddr, &sizeserv);
    size = sizeof(strAddrBuf);
    WSAAddressToString((SOCKADDR*)&servaddr, sizeof(servaddr), NULL, strAddrBuf, &size);
    printf("IP address: %s\n", strAddrBuf);
    WSACleanup();
    return 0;
}