#ifndef SMALLCHATSERVER_H
#define SMALLCHATSERVER_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <sys/select.h>
#include <unistd.h>

#include "chatlib.h"

#define MAX_CLIENTS 1000 // 这是最高的客户端文件描述符编号。
#define SERVER_PORT 7711  // 服务器监听端口

/* 这个结构体表示一个连接的客户端。信息很少：
 * 套接字描述符和昵称（如果设置了），否则昵称的第一个字节设为0。
 * 客户端可以使用/nick <nickname>命令设置昵称。 */
struct client {
    int fd;     // 客户端套接字描述符
    char *nick; // 客户端昵称
};

/* 这个全局结构体封装了聊天的全局状态。 */
struct chatState {
    int serversock;     // 监听服务器套接字
    int numclients;     // 当前连接的客户端数量
    int maxclient;      // 最大的客户端文件描述符编号
    struct client *clients[MAX_CLIENTS]; // 客户端存储在对应套接字描述符的槽位中
};

int smallchatServer(void);

#endif

