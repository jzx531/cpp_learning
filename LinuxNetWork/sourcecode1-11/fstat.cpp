#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>
#include <time.h>

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Usage: %s <file_path>\n", argv[0]);
        return -1;
    }

    const char *file_path = argv[1];
    struct stat st;

    // 获取文件状态
    if (-1 == stat(file_path, &st)) {
        perror("Failed to get file status");
        return -1;
    }

    // 打印文件元数据
    printf("File ID (st_dev): %d\n", st.st_dev);          // 文件所在设备的 ID
    printf("Inode number (st_ino): %ld\n", st.st_ino);    // 文件的 inode 编号
    printf("File mode (st_mode): %d\n", st.st_mode);      // 文件类型和权限
    printf("Hard links (st_nlink): %d\n", st.st_nlink);   // 硬链接数量
    printf("Owner UID (st_uid): %d\n", st.st_uid);        // 文件所有者的用户 ID
    printf("Group GID (st_gid): %d\n", st.st_gid);        // 文件所属组的组 ID
    printf("Device ID (st_rdev): %d\n", st.st_rdev);      // 设备 ID（如果是设备文件）
    printf("File size (st_size): %ld\n", st.st_size);     // 文件大小（字节）
    printf("Block size (st_blksize): %ld\n", st.st_blksize); // 文件系统的 I/O 块大小
    printf("Number of blocks (st_blocks): %ld\n", st.st_blocks); // 文件占用的块数

    // 打印时间戳（转换为可读格式）
    printf("Last access time: %s", ctime(&st.st_atime));
    printf("Last modification time: %s", ctime(&st.st_mtime));
    printf("Last status change time: %s", ctime(&st.st_ctime));

    return 0;
}