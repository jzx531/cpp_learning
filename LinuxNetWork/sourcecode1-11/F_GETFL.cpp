#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>

int main(void) {
    int flags = -1;
    int accmode = -1;

    // 获取标准输入（STDIN）的文件状态标志
    flags = fcntl(0, F_GETFL, 0);
    if (flags < 0) {
        perror("Failed to use fcntl");
        return -1;
    }

    // 提取访问模式
    accmode = flags & O_ACCMODE;
    if (accmode == O_RDONLY) {
        printf("STDIN READ ONLY\n");
    } else if (accmode == O_WRONLY) {
        printf("STDIN WRITE ONLY\n");
    } else if (accmode == O_RDWR) {
        printf("STDIN READ WRITE\n");
    } else {
        printf("STDIN UNKNOWN MODE\n");
    }

    // 检查其他标志
    if (flags & O_APPEND) {
        printf("STDIN APPEND\n");
    }
    if (flags & O_NONBLOCK) {
        printf("STDIN NONBLOCK\n");
    }

    return 0;
}