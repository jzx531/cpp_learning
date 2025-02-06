#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>

int main(void)
{
    int flags = -1;
    char buf[]="FCNTL";
    int fd = open("F_SETFL.txt", O_RDWR|O_CREAT);
    flags=fcntl(fd, F_GETFL, 0);
    flags |= O_APPEND;
    flags = fcntl(fd, F_SETFL, &flags);
    if (flags < 0) {
        perror("fcntl");
        return 1;
    }
    write(fd, buf, strlen(buf));
    close(fd);
    return 0;
}