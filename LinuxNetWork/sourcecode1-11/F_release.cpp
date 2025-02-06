#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
 
int main() {
    int fd;
    struct flock fl;
 
    // 打开文件
    fd = open("example.txt", O_RDWR|O_CREAT|O_EXCL);
    if (fd == -1) {
        perror("open");
        exit(1);
    }
 
    // 设置锁结构体
    fl.l_type = F_WRLCK;   // 写锁
    fl.l_start = 0;        // 锁的起始位置
    fl.l_whence = SEEK_SET; // 相对于文件开头
    fl.l_len = 0;          // 锁的长度，0表示到EOF
 
    // 尝试加锁
    if (fcntl(fd, F_SETLK, &fl) == -1) {
        perror("fcntl");
        close(fd);
        exit(1);
    } else {
        printf("Lock acquired.\n");
    }
 
    // 模拟一些操作...
    sleep(10); // 假设我们在处理文件时需要一段时间
 
    // 释放锁
    fl.l_type = F_UNLCK; // 解锁
    if (fcntl(fd, F_SETLK, &fl) == -1) {
        perror("fcntl");
        close(fd);
        exit(1);
    } else {
        printf("Lock released.\n");
    }
 
    close(fd);
    return 0;
}