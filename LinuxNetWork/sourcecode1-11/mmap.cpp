#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h> // mmap
#include <string.h>   // memset, memcpy
#include <stdio.h>
#include <unistd.h>   // lseek, write, close

#define FILELENGTH 80

int main(void) {
    int fd = -1;
    char buf[] = "quick brown fox jumps over the lazy dog";
    char *ptr = NULL;

    // 打开文件 mmap.txt，如果文件不存在则创建
    fd = open("mmap.txt", O_RDWR | O_CREAT, S_IRWXU);
    if (fd == -1) {
        perror("Failed to open file");
        return -1;
    }

    // 将文件大小设置为 80 字节
    lseek(fd, FILELENGTH - 1, SEEK_SET);
    write(fd, "a", 1);

    // 将文件映射到内存
    ptr = (char *)mmap(NULL, FILELENGTH, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (ptr == MAP_FAILED) {
        perror("mmap failure");
        close(fd);
        return -1;
    }

    // 将 buf 复制到内存映射区域（从偏移 16 开始）
    memcpy(ptr + 16, buf, strlen(buf) + 1);

    // 解除内存映射
    munmap(ptr, FILELENGTH);

    // 关闭文件描述符
    close(fd);

    return 0;
}