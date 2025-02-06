#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>
#include <process.h>

#define BUF_SIZE 2048
#define BUF_SMALL 100

unsigned WINAPI RequestHandler(void* arg);
char* ContentType(char* file);
void SendData(SOCKET sock, char* ct, char* fileName);
void SendErrorMSG(SOCKET sock);
void ErrorHandling(char* message);

int main(int argc, char* argv[]) {
    WSADATA wsaData;
    SOCKET hServSock, hClntSock;
    SOCKADDR_IN servAddr, clntAddr;
    int clntAddrSz;

    HANDLE hThread;
    DWORD dwThreadID;

    if (argc != 2) {
        printf("Usage : %s <port>\n", argv[0]);
        exit(1);
    }

    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
        ErrorHandling("WSAStartup() error!");

    hServSock = socket(PF_INET, SOCK_STREAM, 0);
    memset(&servAddr, 0, sizeof(servAddr));
    servAddr.sin_family = AF_INET;
    servAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servAddr.sin_port = htons(atoi(argv[1]));
    if (bind(hServSock, (SOCKADDR*)&servAddr, sizeof(servAddr)) != 0)
        ErrorHandling("bind() error!");
    if (listen(hServSock, 5) != 0)
        ErrorHandling("listen() error!");

    while (1) {
        clntAddrSz = sizeof(clntAddr);
        hClntSock = accept(hServSock, (SOCKADDR*)&clntAddr, &clntAddrSz);
        printf("Connected client IP:%s, Port:%d\n", inet_ntoa(clntAddr.sin_addr), ntohs(clntAddr.sin_port));
        hThread = (HANDLE)_beginthreadex(NULL, 0, RequestHandler, (void*)&hClntSock, 0, (unsigned*)&dwThreadID);
        CloseHandle(hThread); // 关闭线程句柄
    }
    closesocket(hServSock);
    WSACleanup();
    return 0;
}

unsigned WINAPI RequestHandler(void* arg) {
    SOCKET hClntSock = *(SOCKET*)arg;
    char buf[BUF_SIZE];
    char method[BUF_SMALL];
    char ct[BUF_SMALL];
    char fileName[BUF_SMALL];

    int recvLen = recv(hClntSock, buf, BUF_SIZE, 0);
    if (recvLen == SOCKET_ERROR || recvLen == 0) {
        closesocket(hClntSock);
        return 1;
    }

    if (strstr(buf, "HTTP/") == NULL) {
        SendErrorMSG(hClntSock);
        closesocket(hClntSock);
        return 1;
    }

    char* token = strtok(buf, " ");
    if (token == NULL) {
        SendErrorMSG(hClntSock);
        closesocket(hClntSock);
        return 1;
    }
    strcpy(method, token);

    if (strcmp(method, "GET") != 0) {
        SendErrorMSG(hClntSock);
        closesocket(hClntSock);
        return 1;
    }

    token = strtok(NULL, " ");
    if (token == NULL) {
        SendErrorMSG(hClntSock);
        closesocket(hClntSock);
        return 1;
    }
    strcpy(fileName, token + 1); // 跳过 '/'

    strcpy(ct, ContentType(fileName));
    SendData(hClntSock, ct, fileName);
    closesocket(hClntSock);
    return 0;
}

void SendData(SOCKET sock, char* ct, char* fileName) {
    char protocol[BUF_SMALL] = "HTTP/1.0 200 OK\r\n";
    char servName[BUF_SMALL] = "Server: MyServer/1.0\r\n";
    char cntLen[BUF_SMALL];
    char cntType[BUF_SMALL];
    char buf[BUF_SIZE];
    FILE* sendFile;

    sprintf(cntType, "Content-Type: %s\r\n", ct);
    if ((sendFile = fopen(fileName, "rb")) == NULL) {
        SendErrorMSG(sock);
        return;
    }

    fseek(sendFile, 0, SEEK_END);
    long fileSize = ftell(sendFile);
    fseek(sendFile, 0, SEEK_SET);
    sprintf(cntLen, "Content-Length: %ld\r\n", fileSize);

    send(sock, protocol, strlen(protocol), 0);
    send(sock, servName, strlen(servName), 0);
    send(sock, cntLen, strlen(cntLen), 0);
    send(sock, cntType, strlen(cntType), 0);
    send(sock, "\r\n", 2, 0); // 空行分隔头部和内容

    size_t bytesRead;
    while ((bytesRead = fread(buf, 1, BUF_SIZE, sendFile)) > 0) {
        send(sock, buf, bytesRead, 0);
    }

    fclose(sendFile);
}

void SendErrorMSG(SOCKET sock) {
    char protocol[] = "HTTP/1.0 404 Not Found\r\n";
    char servName[] = "Server: MyServer/1.0\r\n";
    char cntLen[BUF_SMALL];
    char cntType[] = "Content-Type: text/html\r\n";
    char content[] = "<html><head><title>404 Not Found</title></head><body><h1>404 Not Found</h1></body></html>";

    sprintf(cntLen, "Content-Length: %zu\r\n", strlen(content));

    send(sock, protocol, strlen(protocol), 0);
    send(sock, servName, strlen(servName), 0);
    send(sock, cntLen, strlen(cntLen), 0);
    send(sock, cntType, strlen(cntType), 0);
    send(sock, "\r\n", 2, 0); // 空行分隔头部和内容
    send(sock, content, strlen(content), 0);
}

char* ContentType(char* file) {
    char* extension = strrchr(file, '.');
    if (extension == NULL) {
        return "text/plain"; // 默认类型
    }
    extension++; // 跳过 '.'
    if (strcmp(extension, "html") == 0) return "text/html";
    else return "text/plain";
}

void ErrorHandling(char* message) {
    printf("%s\n", message);
}