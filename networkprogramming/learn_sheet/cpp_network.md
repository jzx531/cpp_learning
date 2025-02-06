# Visual c++ network programming
[TOC]

## Socket 套接字编程

Winsock中 套接字地址结构定义如下
```c++
struct sockaddr_in{
    short sin_family;
    unsigned short sin_port;
    struct in_addr sin_addr;
    char sin_zero[8];
};
```

sin_family 使用AF_INET 表示程序所使用的家庭地址是TCP/IP

```c++
struct in_addr{
    union{
        struct{
            unsigned char s_b1,s_b2,s_b3,s_b4;
        }S_un_b;
        struct{
            unsigned short s_w1,s_w2;
        }s_un_w;
        unsigned long S_addr;
    }S_un;
};

sockaddr_in addr;
addr.sin_addr.S_un.S_addr = inet_addr("192.168.1.100");
```

在Socket套接字编程中,传输数据的排列顺序以网络字节顺序和主机字节顺序为主

在Winsock中提供了几个关于网络字节顺序与主机字节顺序之间的转换函数,函数定义如下
```c++
u_short htons(u_short hostshort);
u_short ntohs(u_short netshort);
u_long htonl(u_long hostlong);
u_long ntohl(u_long netlong);
unsigned long inet_addr(const char FAR *cp);
char FAR *inet_ntoa(struct in_addr in);
```

实例程序
```c++
sockaddr_in addr;
in_addr in_add;
addr.sin_family = AF_INET;
addr.sin_port = htons(80);
in_add.in_addr.S_un.S_addr = inet_addr("192.168.1.100");
char addr[]=inet_ntoa(addr.in_addr.S_un.S_addr);
```

Socket相关函数
