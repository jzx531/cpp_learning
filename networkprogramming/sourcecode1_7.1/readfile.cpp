#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#define BUFSIZ 1024
void error_handling(char *message)
{
    fputs(message,stderr);
    fputc('\n',stderr);
    _exit(1);
}

int main(int argc, char *argv[])
{
    int fd;
    char buf[BUFSIZ];
    fd = open("test.txt", O_RDONLY);
    if(fd == -1)
        error_handling("open() error");
    if(read(fd, buf, BUFSIZ) == -1)
        error_handling("read() error");
    printf("read data: %s\n", buf);
    close(fd);
    return 0;
}