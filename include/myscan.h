#ifndef MYSCAN_H
#define MYSCAN_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <winsock2.h>
#include <pthread.h>
#include <Windows.h>
#include <unistd.h>


#define DEFAULT_THREADS 10              //默认线程数

// 分配给线程的数据结构，每个线程负责一个IP和端口的扫描
struct ScanData {
    unsigned int ip; // IP地址（无符号整型，点分十进制转为整数）
    int port;        // 端口号（1~65535）
};

void usage();                   // 打印用法说明
void Init();                    // 初始化网络和线程属性
unsigned int getip(char *);     // 字符串IP转整数
char *ipback(unsigned int);     // 整数IP转字符串
void scan(unsigned int, unsigned int, unsigned int); // 端口扫描主控
void *threadscan(void *);       // 线程扫描函数

int myscan(int argc, char *argv[]);

#endif // MYSCAN_H

