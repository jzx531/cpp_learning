#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>
#include <time.h>

#include "util.hpp"

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <filename>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    const char *filepath = argv[1];
    struct stat sb;

    if (stat(filepath, &sb) == -1) {
        perror("stat");
        exit(EXIT_FAILURE);
    }

    // 打印所有 struct stat 成员（按常见字段）
    printf("=== struct stat members ===\n");
    printf("st_dev     : %ld (device ID containing file)\n", (long)sb.st_dev);
    printf("st_ino     : %lu (inode number)\n", (unsigned long)sb.st_ino);
    printf("st_mode    : %lo (file type and permissions, octal)\n", (unsigned long)sb.st_mode);
    printf("st_nlink   : %ld (number of hard links)\n", (long)sb.st_nlink);
    printf("st_uid     : %d (user ID of owner)\n", (int)sb.st_uid);
    printf("st_gid     : %d (group ID of owner)\n", (int)sb.st_gid);
    printf("st_rdev    : %ld (device ID (if special file))\n", (long)sb.st_rdev);
    printf("st_size    : %lld (total size in bytes)\n", (long long)sb.st_size);
    printf("st_blksize : %ld (block size for filesystem I/O)\n", (long)sb.st_blksize);
    printf("st_blocks  : %lld (number of 512B blocks allocated)\n", (long long)sb.st_blocks);

    // 时间戳（以秒为单位的 time_t）
    printf("st_atime   : %ld (last access time)\n", (long)sb.st_atime);
    printf("st_mtime   : %ld (last modification time)\n", (long)sb.st_mtime);
    printf("st_ctime   : %ld (last status change time)\n", (long)sb.st_ctime);

    // 可选：打印可读时间格式
    printf("\n--- Human-readable timestamps ---\n");
    printf("Access : %s", ctime(&sb.st_atime));
    printf("Modify : %s", ctime(&sb.st_mtime));
    printf("Change : %s", ctime(&sb.st_ctime));

    return 0;
}