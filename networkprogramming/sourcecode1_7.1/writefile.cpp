#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
void error_handling(char *message)
{
    fputs(message,stderr);
    fputc('\n',stderr);
    _exit(1);
}

int main(int argc, char *argv[])
{
    int fd;
    char buf[]="Hello, world!\n";
    fd = open("test.txt",O_WRONLY|O_CREAT|O_TRUNC);
    if(fd == -1)
        error_handling("open() error");
    printf("file descriptor: %d\n",fd);
    if(write(fd,buf,sizeof(buf))==-1)
        error_handling("write() error");
    close(fd);
    return 0;
}


