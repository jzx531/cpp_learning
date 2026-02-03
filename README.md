# LoggerSystem

[TOC]

## 日志消息等级
* OFF:     关闭
* DEBUG:   调试,调试时的关键信息输出
* INFO:    提示，普通的提示性日志信息
* WARN:    警告，不影响运行但需要注意的日志
* ERROR:   错误，程序运行时出现错误
* FATAL:   致命，代码无法正常运行的日志


## 日志消息设置

```
[%d{%H:%M:%S}][%t][%c][%f:%l][%p][%m][%n]；
```

* 时间: 描述本条日志输出的时间
* 线程ID: 描述日志归属于的线程号
* 日志等级：日志等级
* 日志文件名：描述本条日志在哪一个源码文件中输出
* 日志行号：描述本条日志在源码文件中的行号
* 日志数据：本条日志的有效数据载荷

%d 表示日期，包含子格式 {%H:%M:%S}；
%T 表示缩进；
%t 表示线程ID；
%c 表示日志器名称；
%f 表示源码文件名；
%l 表示源码行号；
%p 表示日志级别；
%m 表示主体消息；
%n 表示换行；

## 日志输出
* 标准输出：表示将日志信息进行标准输出打印
* 日志文件输出：表示将日志写入指定文件末尾
* 滚动文件输出：当前以文件大小进行控制，当一个文件大小达到指定大小，则切换下一个文件进行输出

struct stat:

```c
struct stat {
    dev_t     st_dev;         // 文件所在设备的ID
    ino_t     st_ino;         // 文件的inode号
    mode_t    st_mode;        // 文件的访问权限和类型
    nlink_t   st_nlink;       // 文件的硬链接数量
    uid_t     st_uid;         // 文件的所有者的用户ID
    gid_t     st_gid;         // 文件的所有者的组ID
    off_t     st_size;        // 文件的大小（以字节为单位）
    time_t    st_atime;       // 文件的最后访问时间
    time_t    st_mtime;       // 文件的最后修改时间
    time_t    st_ctime;       // 文件的最后状态改变时间
    blksize_t st_blksize;     // 文件系统I/O操作的最佳块大小
    blkcnt_t  st_blocks;      // 文件占用的块数
};

```
测试上述结构：

```c
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>
#include <time.h>

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
```
输出结果为
```
=== struct stat members ===
st_dev     : 69 (device ID containing file)
st_ino     : 1970324837120372 (inode number)
st_mode    : 100777 (file type and permissions, octal)
st_nlink   : 1 (number of hard links)
st_uid     : 1000 (user ID of owner)
st_gid     : 1000 (group ID of owner)
st_rdev    : 0 (device ID (if special file))
st_size    : 1525 (total size in bytes)
st_blksize : 4096 (block size for filesystem I/O)
st_blocks  : 8 (number of 512B blocks allocated)
st_atime   : 1770101362 (last access time)
st_mtime   : 1770101333 (last modification time)
st_ctime   : 1770101333 (last status change time)

--- Human-readable timestamps ---
Access : Tue Feb  3 14:49:22 2026
Modify : Tue Feb  3 14:48:53 2026
Change : Tue Feb  3 14:48:53 2026
```


```cpp
string substr (size_t pos = 0, size_t len = npos) const;
```
* pos - 指示要复制的第一个字符的位置。
* len - 指示子字符串中包含的字符数。


