#include <stdio.h>
#include <fcntl.h>

int main(void)
{
    FILE *fp;
    int fd = open("test.txt", O_RDWR | O_CREAT| O_TRUNC);
    if(fd == -1)
    {
        printf("Error in opening file\n");
        return 1;
    }

    printf("first file descriptor: %d\n", fd);
    fp = fdopen(fd, "w+");
    fputs("Hello World", fp);
    printf("second file descirptor%d\n", fileno(fp));
    fclose(fp);
    return 0;

}