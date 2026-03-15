#include "smallchatClient.h"


void disableRawModeAtExit(void);

/**
 * setRawMode - 设置或取消终端原始模式
 * @fd: 文件描述符（通常是标准输入）
 * @enable: 1 表示启用原始模式，0 表示禁用
 *
 * 原始模式说明：
 *   终端默认是规范模式（canonical mode），在这种模式下：
 *   - 输入会经过处理（如退格键删除、回车转换等）
 *   - 程序只能按行读取输入（需要按回车）
 *
 *   原始模式（raw mode）下：
 *   - 每个按键立即发送给程序，无需等待回车
 *   - 不对输入进行任何处理
 *   - 适合交互式应用程序如聊天客户端
 *
 * 返回值：成功返回 0，失败返回 -1
 */
int setRawMode(int fd, int enable) {
    static struct termios orig_termios;//保存原始终端状态
    static int atexit_registered = 0;//避免多次注册atexit函数
    static int rawmode_is_set = 0;//标记当前是否处于原始模式
    struct termios raw;

    /* 如果 enable 为 0，只需在当前已设置原始模式时禁用它 */
    if(enable == 0){
        /* 此时不检查返回值，因为可能已经太晚了 */
        if (rawmode_is_set && tcsetattr(fd,TCSAFLUSH,&orig_termios) != -1)
            rawmode_is_set = 0;
        return 0;
    }

    /*启用原始模式*/
    if (!isatty(fd)) goto fatal;  /* 检查 fd 是否是终端 */
    if (!atexit_registered) {
        atexit(disableRawModeAtExit);  /* 注册退出处理函数 */
        atexit_registered = 1;
    }
    if (tcgetattr(fd,&orig_termios) == -1) goto fatal;  /* 获取当前终端设置 */

    raw = orig_termios;  /* 复制原始模式设置，然后修改 */
    /* 输入模式：禁用中断、禁用 CR 到 NL 的转换、禁用奇偶校验、
     * 禁用字符剥离、禁用输出流控 */
    raw.c_iflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON);
    /* 输出模式 - 不做修改。我们希望保留后处理，
     * 这样 \n 会自动转换为 \r\n */
    // raw.c_oflag &= ...
    /* 控制模式 - 设置为 8 位字符 */
    raw.c_cflag |= (CS8);
    /* 本地模式：禁用回显、禁用规范模式、禁用扩展功能，
     * 但保留信号字符（^Z、^C）的处理 */
    raw.c_lflag &= ~(ECHO | ICANON | IEXTEN);
    /* 控制字符 - 设置返回条件：最小字节数和超时时间
     * 我们希望每次读取返回一个字节，不超时 */
    raw.c_cc[VMIN] = 1; raw.c_cc[VTIME] = 0; /* 1 字节，无超时 */

    /* 刷新缓冲区后将终端设置为原始模式 */
    if (tcsetattr(fd,TCSAFLUSH,&raw) < 0) goto fatal;
    rawmode_is_set = 1;
    return 0;

fatal:
    errno = ENOTTY;  /* 不是终端 */
    return -1;
}


/* 程序退出时尝试将终端恢复到初始状态 */
void disableRawModeAtExit(void) {
    setRawMode(STDIN_FILENO,0);
}

/**
 * terminalCleanCurrentLine - 清除终端当前行
 * 使用 ANSI 转义序列 \e[2K 清除整行内容
 */
void terminalCleanCurrentLine(void) {
    write(fileno(stdout),"\e[2K",4);
}

/**
 * terminalCursorAtLineStart - 将光标移动到行首
 * 使用回车符 \r 实现
 */
void terminalCursorAtLineStart(void) {
    write(fileno(stdout),"\r",1);
}

int inputBufferAppend(struct InputBuffer *ib, int c) {
    if (ib->len >= IB_MAX) return IB_ERR; /* 空间不足 */

    ib->buf[ib->len] = c;
    ib->len++;
    return IB_OK;
}

void inputBufferHide(struct InputBuffer *ib);
void inputBufferShow(struct InputBuffer *ib);

/* 功能：处理用户的键盘输入，更新输入缓冲区状态
 * 支持的特殊字符：
 *   - 回车 (\r): 表示一行输入完成
 *   - 换行 (\n): 忽略，只处理回车
 *   - 退格 (127): 删除最后一个字符
 *
 * 返回值：IB_GOTLINE 表示完成一行，IB_OK 表示正常处理，IB_ERR 表示错误
 */
int inputBufferFeedChar(struct InputBuffer *ib, int c)
{
    switch(c){
        case '\n':break;
        case '\r':return IB_GOTLINE;
        case 127://退格键backspace
            if (ib->len > 0) {
                ib->len--;
            inputBufferHide(ib);  /* 隐藏当前行 */
            inputBufferShow(ib);  /* 重新显示更新后的行 */
        }
        break;
    default:
        /*普通字符,添加到缓冲区并显示*/
        if (inputBufferAppend(ib,c) == IB_OK)
            write(fileno(stdout),ib->buf+ib->len-1,1);
        break;
    }
    return IB_OK;
}

/**
 * inputBufferHide - 隐藏用户当前输入的行
 * @ib: 输入缓冲区指针（未使用，仅为 API 一致性保留）
 *
 * 功能：清除当前行并将光标移到行首，用于重新显示更新后的内容
 */
void inputBufferHide(struct InputBuffer *ib) {
    (void)ib; /* 未使用的变量，但 API 概念上需要 */
    terminalCleanCurrentLine();    /* 清除当前行 */
    terminalCursorAtLineStart();   /* 光标移到行首 */
}

/**
 * inputBufferShow - 重新显示当前输入行
 * @ib: 输入缓冲区指针
 *
 * 功能：在清除行后重新显示缓冲区内容，用于退格删除后刷新显示
 */
void inputBufferShow(struct InputBuffer *ib) {
    write(fileno(stdout),ib->buf,ib->len);
}

/**
 * inputBufferClear - 重置缓冲区为空
 * @ib: 输入缓冲区指针
 *
 * 功能：清空缓冲区内容，通常在用户发送消息后调用
 */
void inputBufferClear(struct InputBuffer *ib) {
    ib->len = 0;
    inputBufferHide(ib);  /* 隐藏空行 */
}

/* =============================================================================
 * 主程序逻辑
 * 功能：创建 TCP 连接，使用 select() 多路复用处理用户输入和服务器消息
 * ========================================================================== */

/**
 * main - 程序入口函数
 * @argc: 命令行参数个数
 * @argv: 命令行参数数组
 *
 * 用法：smallchat-client <服务器地址> <端口号>
 *
 * 工作流程：
 *   1. 解析命令行参数，获取服务器地址和端口
 *   2. 创建 TCP 连接连接到服务器
 *   3. 设置终端为原始模式
 *   4. 进入主循环，使用 select() 监听两个文件描述符：
 *      - 标准输入：用户键盘输入
 *      - 服务器 socket：服务器发来的消息
 *   5. 连接断开时退出程序
 *
 * 返回值：0 表示正常退出
 */
int smallchatClient(int argc, char **argv)
{
    /* 检查命令行参数*/
    if (argc != 4) {
        fprintf(stderr,"Usage: %s <服务器地址> <端口号>\n",argv[0]);
        return 1;
    }
    /* 创建 TCP 连接 */
    int s = TCPConnect(argv[2],atoi(argv[3]),0);
    if (s == -1) {
        fprintf(stderr, "连接服务器失败 (地址：%s, 端口：%s)\n", argv[2], argv[3]);
        perror("连接服务器失败");
        exit(1);
    }

    /* 将终端设置为原始模式：这样用户每按一个键就能立即接收到，
     * 无需等待回车，也没有任何转义序列转换 */
    setRawMode(fileno(stdin),1);
    /* 等待标准输入或服务器 socket 有数据 */
    fd_set readfds;           /* 文件描述符集合，用于 select() */
    int stdin_fd = fileno(stdin);  /* 标准输入的文件描述符 */

    struct InputBuffer ib;    /* 输入缓冲区，存储用户正在输入的行 */
    inputBufferClear(&ib);    /* 初始化清空缓冲区 */

    while (1) {
        FD_ZERO(&readfds);
        FD_SET(stdin_fd,&readfds);
        FD_SET(s,&readfds);
        int maxfd = (s > stdin_fd)? s : stdin_fd;
        int n = select(maxfd+1,&readfds,NULL, NULL, NULL);
        /* select() 阻塞等待，直到有文件描述符可读 */
        int num_events = select(maxfd+1, &readfds, NULL, NULL, NULL);
        if (num_events == -1) {
            perror("select() 错误");
            exit(1);
        } else if (num_events) {
            /* 处理文件描述符 */
            char buf[1024];
            if(FD_ISSET(s,&readfds)){
                /* 服务器 socket 可读，接收消息 */
                ssize_t n = recv(s,buf,sizeof(buf),0);
                if (n == -1) {
                    perror("接收消息失败");
                    exit(1);
                }
                /* 隐藏当前输入行，显示服务器消息，再重新显示输入行 */
                inputBufferHide(&ib);
                write(fileno(stdout),buf,n);
                inputBufferShow(&ib);
            }else if(FD_ISSET(stdin_fd,&readfds)){
                /* 用户从终端输入数据 */
                ssize_t count = read(stdin_fd,buf,sizeof(buf));
                for (int j = 0; j < count; j++) {
                    int res = inputBufferFeedChar(&ib,buf[j]);
                    switch(res){
                        case IB_GOTLINE:
                            /* 一行输入完成，发送消息 */
                            inputBufferAppend(&ib,'\n');  /* 添加换行符 */
                            inputBufferHide(&ib);         /* 隐藏当前行 */
                            write(fileno(stdout),"you> ", 5);  /* 显示提示 */
                            write(fileno(stdout),ib.buf,ib.len);  /* 显示发送内容 */
                            write(s,ib.buf,ib.len);       /* 发送到服务器 */
                            inputBufferClear(&ib); 
                            break;       /* 清空缓冲区 */
                        case IB_OK:
                            break;       /* 正常处理 */
                    }
                }
            }
        }
    }
    close(s);
    return 0;
}
