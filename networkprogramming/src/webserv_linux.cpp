#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <errno.h>

#define BUF_SIZE 2048
#define BUF_SMALL 100

void* RequestHandler(void* arg);
char* ContentType(char* file);
void SendData(int sock, char* ct, char* fileName);
void SendErrorMSG(int sock);
void ErrorHandling(char* message);

int main(int argc, char* argv[]) {
    int servSock, clntSock;
    struct sockaddr_in servAddr, clntAddr;
    socklen_t clntAddrSz;

    pthread_t t_id;

    if (argc != 2) {
        printf("Usage : %s <port>\n", argv[0]);
        exit(1);
    }

    servSock = socket(PF_INET, SOCK_STREAM, 0);
    if (servSock == -1) {
        ErrorHandling("socket() error");
    }

    memset(&servAddr, 0, sizeof(servAddr));
    servAddr.sin_family = AF_INET;
    servAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servAddr.sin_port = htons(atoi(argv[1]));

    if (bind(servSock, (struct sockaddr*)&servAddr, sizeof(servAddr)) == -1) {
        ErrorHandling("bind() error");
    }

    if (listen(servSock, 5) == -1) {
        ErrorHandling("listen() error");
    }

    while (1) {
        clntAddrSz = sizeof(clntAddr);
        clntSock = accept(servSock, (struct sockaddr*)&clntAddr, &clntAddrSz);
        if (clntSock == -1) {
            ErrorHandling("accept() error");
        }
        printf("Connected client IP: %s, Port: %d\n", inet_ntoa(clntAddr.sin_addr), ntohs(clntAddr.sin_port));

        // 创建线程处理客户端请求
        if (pthread_create(&t_id, NULL, RequestHandler, (void*)&clntSock) != 0) {
            ErrorHandling("pthread_create() error");
        }
        pthread_detach(t_id); // 分离线程，线程结束后自动释放资源
    }
    close(servSock);
    return 0;
}

void* RequestHandler(void* arg) {
    int clntSock = *(int*)arg;
    char buf[BUF_SIZE];
    char method[BUF_SMALL];
    char ct[BUF_SMALL];
    char fileName[BUF_SMALL];

    int recvLen = recv(clntSock, buf, BUF_SIZE, 0);
    if (recvLen == -1 || recvLen == 0) {
        close(clntSock);
        return NULL;
    }

    if (strstr(buf, "HTTP/") == NULL) {
        SendErrorMSG(clntSock);
        close(clntSock);
        return NULL;
    }

    char* token = strtok(buf, " ");
    if (token == NULL) {
        SendErrorMSG(clntSock);
        close(clntSock);
        return NULL;
    }
    strcpy(method, token);

    if (strcmp(method, "GET") != 0) {
        SendErrorMSG(clntSock);
        close(clntSock);
        return NULL;
    }

    token = strtok(NULL, " ");
    if (token == NULL) {
        SendErrorMSG(clntSock);
        close(clntSock);
        return NULL;
    }
    strcpy(fileName, token + 1); // 跳过 '/'

    strcpy(ct, ContentType(fileName));
    SendData(clntSock, ct, fileName);
    close(clntSock);
    return NULL;
}

void SendData(int sock, char* ct, char* fileName) {
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

void SendErrorMSG(int sock) {
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
    perror(message);
    exit(1);
}