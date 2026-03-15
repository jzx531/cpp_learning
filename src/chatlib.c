#include "chatlib.h"

/*=======定义底层网络功能=========*/
int socketSetNonBlockNoDelay(int fd)
{
    int flags,yes = 1;
    //获取当前套接字的标志
    if((flags = fcntl(fd, F_GETFL))== -1) return -1;
    //设置非阻塞
    if(fcntl(fd, F_SETFL, flags | O_NONBLOCK) == -1)return -1;

    //设置tcp无延迟
    setsockopt(fd, IPPROTO_TCP, TCP_NODELAY, &yes, sizeof(yes));
    return 0;
}

/* 创建一个监听指定端口的TCP服务器套接字，准备接受连接。
 * 参数：port - 要监听的端口号
 * 返回值：成功返回套接字描述符，失败返回-1 */
int createTCPServer(int port) {
    int s, yes = 1;
    struct sockaddr_in sa;

    /* 创建TCP套接字 */
    if ((s = socket(AF_INET, SOCK_STREAM, 0)) == -1) return -1;
    /* 设置地址重用选项，最佳努力 */
    setsockopt(s, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes));

    /* 初始化服务器地址结构 */
    memset(&sa,0,sizeof(sa));
    sa.sin_family = AF_INET;
    sa.sin_port = htons(port);  /* 转换端口到网络字节序 */
    // sa.sin_addr.s_addr = htonl(INADDR_ANY);  /* 监听所有接口 */
    //使用本地换回地址
    sa.sin_addr.s_addr = htonl(INADDR_LOOPBACK);  /* 监听本地回环地址 */
    /* 绑定套接字到地址和端口 */
    if (bind(s,(struct sockaddr*)&sa,sizeof(sa)) == -1 ||
        listen(s, 511) == -1)  /* 开始监听，最大等待队列511 */
    {
        close(s);  /* 失败时关闭套接字 */
        return -1;
    }
    //打印server地址和端口
    printf("Server listening on %s:%d\n", inet_ntoa(sa.sin_addr), ntohs(sa.sin_port));
    return s;  /* 返回监听套接字 */
}

/* 创建一个TCP套接字并连接到指定的地址。
 * 成功时返回套接字描述符，否则返回-1。
 *
 * 如果'nonblock'不为零，套接字将被设置为非阻塞状态，
 * connect()尝试不会阻塞，但套接字可能不会立即准备好写入。
 * 参数：addr - 目标地址字符串，port - 端口号，nonblock - 是否非阻塞
 * 返回值：成功返回套接字描述符，失败返回-1 */
int TCPConnect(char *addr,int port,int nonblock){
    int s,retval = -1;
    struct addrinfo hints,*servinfo,*p;

    char portstr[6];/* 端口字符串，最大16位数字长度 */
    snprintf(portstr,6,"%d",port);//将端口号转换为字符串
    memset(&hints,0,sizeof(hints));
    hints.ai_family = AF_UNSPEC; /* 允许IPv4或IPv6 */
    hints.ai_socktype = SOCK_STREAM; /* 连接的是TCP流 */

    /*
    int getaddrinfo(const char *node,     // 主机名 (e.g., "www.google.com", "127.0.0.1") 或 NULL
                const char *service,  // 服务名/端口 (e.g., "80", "http") 或 NULL
                const struct addrinfo *hints, // 过滤器/偏好设置
                struct addrinfo **res);       // [输出] 结果链表头指针*/
    /* 获取地址信息 */
    if(getaddrinfo(addr,portstr,&hints,&servinfo)!=0) return -1;
    /*遍历地址列表尝试连接*/
    for (p = servinfo; p != NULL; p = p->ai_next) {
        /* 创建套接字 */
        if ((s = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1)
            continue;/* 失败则尝试下一个*/
        /* 设置非阻塞模式 */
        if(nonblock)
        {
            if(socketSetNonBlockNoDelay(s)!=0)
            {
                close(s);
                continue;
            }
        }
        /* 尝试连接 */
        if (connect(s,p->ai_addr,p->ai_addrlen)==-1)
        {
            if(errno == EINPROGRESS && nonblock) return s;
            /* 其他错误*/
            close(s);
            break;
        }
        /* 连接成功 */
        retval = s;
        break;
    }

    /* 释放地址信息 */
    freeaddrinfo(servinfo);
    return retval;
}

/* 如果监听套接字表示有新连接准备接受，我们使用accept()接受它，
 * 出错时返回-1，成功时返回新的客户端套接字。
 * 参数：server_socket - 服务器监听套接字
 * 返回值：成功返回客户端套接字描述符，失败返回-1 */
int acceptClient(int server_socket) {
    int s;

    while(1){
        /*循环直到成功或错误*/
        struct sockaddr_in sa;
        socklen_t slen = sizeof(sa);
        s = accept(server_socket, (struct sockaddr*)&sa, &slen);
        if(s == -1){
            if(errno == EINTR) /* 被信号中断*/
                continue;
            else /* 其他错误 */
                return -1;
        }
        break; /*成功接收连接*/
    }
    return s;
}

/* 定义一个分配器，在内存不足时总是崩溃：在大多数设计为长时间运行的程序中，
 * 这些不是库，尝试从内存不足中恢复往往是徒劳的，
 * 同时使整个程序变得糟糕。
 * 参数：size - 要分配的字节数
 * 返回值：分配的内存指针，失败时程序退出 */
void* chatMalloc(size_t size){
    void* p = malloc(size);
    if(p == NULL){
        perror("Out of memory");
        exit(1);
    }
    return p;
}

/* 同样对realloc()进行中止处理。
 * 参数：ptr - 原内存指针，size - 新大小
 * 返回值：重新分配的内存指针，失败时程序退出 */
void *chatRealloc(void *ptr, size_t size) {
    ptr = realloc(ptr,size);  /* 重新分配内存 */
    if (ptr == NULL) {
        perror("Out of memory");  /* 输出错误信息 */
        exit(1);  /* 程序退出 */
    }
    return ptr;  /* 返回指针 */
}












