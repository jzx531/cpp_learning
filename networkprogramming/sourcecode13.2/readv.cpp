#include <sys/uio.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define BUF_SIZE 100

int main(int argc, char *argv[])
{
    struct iovec iov[2];
    char buf1[BUF_SIZE];
    char buf2[BUF_SIZE];
    int str_len;
    iov[0].iov_base = buf1;
    iov[0].iov_len = 5;
    iov[1].iov_base = buf2;
    iov[1].iov_len = BUF_SIZE;

    str_len = readv(0, iov, 2);
    printf("read %d bytes from stdin\n", str_len);
    printf("buf1: %s\n", buf1);
    printf("buf2: %s\n", buf2);
    return 0;
}
