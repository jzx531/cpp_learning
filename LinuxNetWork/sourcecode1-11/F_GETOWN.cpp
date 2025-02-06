#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
int main(void)
{
    int uid;
    int fd = open("test.txt",O_RDWR);
    uid = fcntl(fd, F_GETOWN, 0);
    printf("The owner of the file is %d\n",uid);
    close(fd);
    return 0;
}