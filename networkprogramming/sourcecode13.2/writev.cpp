#include <stdio.h>
#include <sys/uio.h>
#include <string.h>

int main(int argc, char *argv[])
{
    struct iovec vec[2];
    char buf1[] = "Hello ";
    char buf2[]="1234567";
    int str_len;
    vec[0].iov_base = buf1;
    vec[0].iov_len = strlen(buf1);
    vec[1].iov_base = buf2;
    vec[1].iov_len = strlen(buf2);

    str_len = writev(2, vec, 2);
    puts("");
    printf("Wrote %d bytes to stdout\n", str_len);
    return 0;
}