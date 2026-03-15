#ifndef SMALLCHATCLIENT_H
#define SMALLCHATCLIENT_H



#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <sys/select.h>
#include <unistd.h>
#include <termios.h>    /* 终端控制函数，用于设置原始模式 */
#include <errno.h>

#include "chatlib.h"

#define IB_MAX 128  /* 输入缓冲区最大长度（字节） */

/**
 * struct InputBuffer - 输入缓冲区结构体
 * 用于存储用户正在输入的行，支持简单的编辑功能
 *
 * @buf: 存储输入数据的缓冲区
 * @len: 当前缓冲区中的数据长度
 */

 /* inputBuffer*() 函数返回值定义： */
#define IB_ERR 0        /* 错误：无法完成操作 */
#define IB_OK 1         /* 成功：已接收新字符或完成操作 */
#define IB_GOTLINE 2    /* 用户已完成一行输入（按了回车） */


struct InputBuffer {
    char buf[IB_MAX];       /* 存储数据的缓冲区 */
    int len;                /* 当前长度 */
};


int smallchatClient(int argc, char **argv);


#endif // SMALLCHATCLIENT_H

